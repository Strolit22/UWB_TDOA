#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <DW1000.h>
#include "stimer.h"
HardwareSerial Serial1(PA10, PA9);
// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0; // irq pin
const uint8_t PIN_SS = PA4; // spi select pin

// DEBUG packet sent status and count
boolean received = false;
boolean error = false;
int16_t numReceived = 0; // todo check int type
String message;

extern void app_protocol_start(void);
void handleReceived() ;
void handleError();
void receiver();
void print(void *param)
{
  Serial1.println("Ahihi");
}
void setup() {
  // DEBUG monitoring
  Serial1.begin(115200);
  Serial1.println(F("### DW1000-arduino-receiver-test ###"));
  app_protocol_start();
  
  // initialize the driver
  // DW1000.begin(PIN_IRQ, PIN_RST);
  // DW1000.select(PIN_SS);
  // Serial1.println(F("DW1000 initialized ..."));
  // // general configuration
  // DW1000.newConfiguration();
  // DW1000.setDefaults();
  // DW1000.setDeviceAddress(6);
  // DW1000.enableDebounceClock();
  // DW1000.enableLedBlinking();
  // DW1000.setGPIOMode(MSGP0, LED_MODE);
  // DW1000.setGPIOMode(MSGP1, LED_MODE);
  // // DW1000.setGPIOMode(MSGP2, LED_MODE);
  // // DW1000.setGPIOMode(MSGP3, LED_MODE);
  // DW1000.setNetworkId(10);
  // DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
  // DW1000.commitConfiguration();
  // Serial1.println(F("Committed configuration ..."));
  // // DEBUG chip info and registers pretty printed
  // char msg[128];
  // DW1000.getPrintableDeviceIdentifier(msg);
  // Serial1.print("Device ID: "); Serial1.println(msg);
  // DW1000.getPrintableExtendedUniqueIdentifier(msg);
  // Serial1.print("Unique ID: "); Serial1.println(msg);
  // DW1000.getPrintableNetworkIdAndShortAddress(msg);
  // Serial1.print("Network ID & Device Address: "); Serial1.println(msg);
  // DW1000.getPrintableDeviceMode(msg);
  // Serial1.print("Device mode: "); Serial1.println(msg);
  // // attach callback for (successfully) received messages
  // DW1000.attachReceivedHandler(handleReceived);
  // DW1000.attachReceiveFailedHandler(handleError);
  // DW1000.attachErrorHandler(handleError);

  // // start reception
  // receiver();
}

void handleReceived() {
  // status change on reception success
  received = true;
}

void handleError() {
  error = true;
}

void receiver() {
  DW1000.newReceive();
  DW1000.setDefaults();
  // so we don't need to restart the receiver manually
  DW1000.receivePermanently(true);
  DW1000.startReceive();
}

void loop() {
  // enter on confirmation of ISR status change (successfully received)
  processTimerEvents();
  if (received) {
    numReceived++;
    // get data as string
    byte data[8] = {0};
    //String msg;
    DW1000.getData(data, 8);
    byte id[8] = {0};
    DW1000.getbit();
  }
  //   DW1000Time ReceiveTime;
  //   DW1000.getReceiveTimestamp(ReceiveTime);
  //   Serial1.print("Received message ... #"); 
  //   Serial1.println(numReceived);
  //   Serial1.print("Rx Timestamp: ");
  //   Serial1.println(ReceiveTime.getAsMicroSeconds());
  //   Serial1.print("Data is ..."); 
  //   for (int i=0; i<8;i++) Serial1.printf("%.2x ",data[i]);
  //   Serial1.println();
  //   Serial1.print("FP power is [dBm] ... "); Serial1.println(DW1000.getFirstPathPower());
  //   Serial1.print("RX power is [dBm] ... "); Serial1.println(DW1000.getReceivePower());
  //   Serial1.print("Signal quality is ... "); Serial1.println(DW1000.getReceiveQuality());
  //   received = false;
  // }
  // if (error) {
  //   Serial1.println("Error receiving a message");
  //   error = false;
  //   DW1000.getData(message);
  //   Serial1.print("Error data is ... "); Serial1.println(message);
  // }
}
