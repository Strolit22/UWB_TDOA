#include <Arduino.h>
/*
 * SPI1
 * CS 20 PA4
 * CLK 21 PA5
 * MISO 22 PA6
 * MOSI 23 PA7
 * IRQ 26 PB0
 * RST 33 PB12
 */
#include <SPI.h>
#include <DW1000.h>
#include <HardwareSerial.h>
HardwareSerial Serial1(PA10, PA9);

// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0;  // irq pin
const uint8_t PIN_SS = PA4;   // spi select pin

// DEBUG packet sent status and count
volatile boolean received = false;
volatile boolean error = false;
volatile unsigned long delaySent = 0;
int16_t sentNum = 0;              // todo check int type
volatile int16_t numReceived = 0; // todo check int type
String message;
char msg[128];
DW1000Time sentTime;

void receiver();
void handleReceived();
void handleError();
void transmitter();
void receive();

struct time
{
  DW1000Time Trp;
  DW1000Time Tsr;
  DW1000Time Trf;
} time;

String id_anchor = "01";

enum
{
  Pol,
  Res,
  Final,
  Rep
};
volatile int check = Pol;

void setup()
{
  // DEBUG monitoring
  Serial1.begin(115200);
  Serial1.println(F("### DW1000-arduino-receiver-test ###"));
  // initialize the driver
  DW1000.begin(PIN_IRQ, PIN_RST);
  DW1000.select(PIN_SS);
  Serial1.println(F("DW1000 initialized ..."));
  // general configuration
  DW1000.newConfiguration();
  DW1000.setDefaults();
  DW1000.setDeviceAddress(1);
  DW1000.setNetworkId(10);
  DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
  DW1000.commitConfiguration();
  Serial1.println(F("Committed configuration ..."));
  DW1000.attachReceivedHandler(handleReceived);
  DW1000.attachReceiveFailedHandler(handleError);
  DW1000.attachErrorHandler(handleError);
  // start reception
  receiver();
}

void handleReceived()
{
  // status change on reception success
  received = true;
}

void handleError()
{
  error = true;
}

void receiver()
{
  DW1000.newReceive();
  DW1000.setDefaults();
  // so we don't need to restart the receiver manually
  DW1000.receivePermanently(true);
  DW1000.startReceive();
}

void transmitter()
{
  // transmit some data
  sentNum++;
  Serial1.print("Transmitting to Tag... #");
  Serial1.println(sentNum);
  DW1000.newTransmit();
  DW1000.setDefaults();
  String msg = id_anchor;
  DW1000.setData(msg);
  // delay sending the message for the given amount
  DW1000Time deltaTime = DW1000Time(10, DW1000Time::MILLISECONDS);
  DW1000.setDelay(deltaTime);
  DW1000.startTransmit();
  //delaySent = millis();
  if (check == Res)
  {
    check = Final;
  }
  if (check == Rep)
  {
    check = Pol;
    sentNum = 0;
  }
}
void receive()
{
  if (received)
  {
    numReceived++;
    // get data as string
    DW1000.getData(message);
    // Serial1.print("Received message ... #"); Serial1.println(numReceived);
    Serial1.print("Data is ... ");
    Serial1.println(message);
    if (message == "01")
    {
      DW1000Time newSentTime;
      DW1000.getReceiveTimestamp(newSentTime);
      Serial1.print("Trp = ");
      Serial1.println(newSentTime.getAsMicroSeconds());
      if (check == Pol)
      {
        check = Res;
      }
      if (check == Final)
      {
        check = Rep;
      }
      received = false;
    }
  }
}
void loop()
{
  switch (check)
  {
  case Pol:
    receive();
    break;
  case Res:
    delay(500);
    transmitter();
    break;
  case Final:
    receive();
    break;
  case Rep:
    delay(500);
    transmitter();
    break;
  }
  // if (check == Pol)
  // {
  //   delay(1000);
  // }
}
// "IDanchor" "timestamp"