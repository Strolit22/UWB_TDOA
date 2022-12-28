#include <Arduino.h>

/*
 * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 * Decawave DW1000 library for arduino.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file BasicReceiver.ino
 * Use this to test simple sender/receiver functionality with two
 * DW1000. Complements the "BasicSender" example sketch.
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  
 */

#include <SPI.h>
#include <DW1000.h>
#include <HardwareSerial.h>
#include <protocol.cpp>

HardwareSerial Serial1(PA10, PA9);

// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

// DEBUG packet sent status and count
boolean sent = false;
volatile boolean sentAck = false;
volatile unsigned long delaySent = 0;
int16_t sentNum = 0; // todo check int type
DW1000Time sentTime;
String idFrom;



void handleSent();
void transmitter();
void loop();

void setup()
{
  // DEBUG monitoring
  Serial1.begin(115200);
  Serial1.println(F("### DW1000-arduino-sender-test ###"));
  // initialize the driver
  DW1000.begin(PIN_IRQ, PIN_RST);
  DW1000.select(PIN_SS);
  Serial1.println(F("DW1000 initialized ..."));
  // general configuration
  DW1000.newConfiguration();
  DW1000.setDefaults();
  DW1000.setDeviceAddress(5);
  DW1000.setNetworkId(10);
  DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
  DW1000.commitConfiguration();
  Serial1.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000.getPrintableDeviceIdentifier(msg);
  Serial1.print("Device ID: ");
  Serial1.println(msg);
  DW1000.getPrintableExtendedUniqueIdentifier(msg);
  Serial1.print("Unique ID: ");
  Serial1.println(msg);
  DW1000.getPrintableNetworkIdAndShortAddress(msg);
  Serial1.print("Network ID & Device Address: ");
  Serial1.println(msg);
  DW1000.getPrintableDeviceMode(msg);
  Serial1.print("Device mode: ");
  Serial1.println(msg);
  // attach callback for (successfully) sent messages
  DW1000.attachSentHandler(handleSent);
  // start a transmission
  transmitter();
}

void handleSent()
{
  // status change on sent success
  sentAck = true;
}

void transmitter()
{
  // transmit some data
  Serial1.print("Transmitting packet ... #");
  Serial1.println(sentNum);
  DW1000.newTransmit();
  DW1000.setDefaults();
  // packet_t Packet = {0};
  // Packet.preamble = PREAMBLE;
  // Packet.messageId = 11;
  // Packet.
  msgToa_t report = {0};
  report.txId = idFrom;
  report.rxId = TXID_ANCHOR;
  report.rxTs = ;

  String msg = "Hello DW1000, it's #";
  msg += sentNum;
  DW1000.setData(msg);
  // delay sending the message for the given amount
  DW1000Time deltaTime = DW1000Time(10, DW1000Time::MILLISECONDS);
  DW1000.setDelay(deltaTime);
  DW1000.startTransmit();
  delaySent = millis();
}

void loop()
{
  if (!sentAck)
  {
    return;
  }
  // continue on success confirmation
  // (we are here after the given amount of send delay time has passed)
  sentAck = false;
  // update and print some information about the sent message
  Serial1.print("ARDUINO delay sent [ms] ... ");
  Serial1.println(millis() - delaySent);
  DW1000Time newSentTime;
  DW1000.getTransmitTimestamp(newSentTime);
  Serial1.print("Processed packet ... #");
  Serial1.println(sentNum);
  Serial1.print("Sent timestamp ... ");
  Serial1.println(newSentTime.getAsMicroSeconds());
  // note: delta is just for simple demo as not correct on system time counter wrap-around
  Serial1.print("DW1000 delta send time [ms] ... ");
  Serial1.println((newSentTime.getAsMicroSeconds() - sentTime.getAsMicroSeconds()) * 1.0e-3);
  sentTime = newSentTime;
  sentNum++;
  // again, transmit some data
  transmitter();
}
