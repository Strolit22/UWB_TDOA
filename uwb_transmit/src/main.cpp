#include <Arduino.h>
/*/
 *   * SPI1
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
// HardwareTimer timer(TIM1);
// HardwareTimer timer(TIM2);
// HardwareTimer timer1(TIM1);
// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0;  // irq pin
const uint8_t PIN_SS = SS;    // spi select pin

// DEBUG packet sent status and count
boolean sent = false;
volatile boolean sentAck = false;
volatile unsigned long delaySent = 0;
volatile boolean received = false;
volatile int16_t sentNum = 0; // todo check int type
// int16_t sentNum = 0; // todo check int type
volatile int16_t numReceived = 0; // todo check int type
String message;
DW1000Time sentTime;

struct id_anchor
{
  String anchor1 = "01";
  String anchor2 = "02";
  String anchor3 = "03";
  String anchor4 = "04";
} id_anchor;
String check_anchor = id_anchor.anchor1;
char msg[120];

enum
{
  Pol,
  Res,
  Final,
  Rep
};
volatile int check = Pol;

struct time
{
  DW1000Time Tsp;
  DW1000Time Trr;
  DW1000Time Tsf;
} time;

void handleSent();
void transmitter();
void handleReceived();
void receiver();
void receive();
void handleReceived()
{
  // status change on reception success
  received = true;
}

void handleSent()
{
  // status change on sent success
  sentAck = true;
}

void receiver()
{
  DW1000.newReceive();
  DW1000.setDefaults();
  // so we don't need to restart the receiver manually
  DW1000.receivePermanently(true);
  DW1000.startReceive();
}
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
  DW1000.setDeviceAddress(0);
  DW1000.setNetworkId(10);
  DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
  DW1000.commitConfiguration();
  Serial1.println(F("Committed configuration ..."));
  DW1000.attachSentHandler(handleSent);
  DW1000.attachReceivedHandler(handleReceived);
  // transmitter();
  receiver();
}

void transmitter()
{
  // transmit some data
  sentNum++;
  Serial1.println("Transmitting to anchor #" + check_anchor);
  Serial1.print("Number");
  Serial1.println(sentNum);
  DW1000.newTransmit();
  DW1000.setDefaults();
  String msg = check_anchor;
  // DW1000.getPrintableNetworkIdAndShortAddress(msg);
  DW1000.setData(msg);
  // delay sending the message for the given amount
  DW1000Time deltaTime = DW1000Time(10, DW1000Time::MILLISECONDS);
  DW1000.setDelay(deltaTime);
  DW1000.startTransmit();
  //delaySent = millis();
  if (check == Pol)
  {
    check = Res;
  }
  if (check == Final)
  {
    check = Rep;
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
    if (message == "01")
    {
      Serial1.print("Received message ... #");
      Serial1.println(numReceived);
      Serial1.print("Data is ... ");
      Serial1.println(message);
      if (check == Res)
      {
        check = Final;
      }
      if (check == Rep)
      {
        check = Pol;
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
    delay(500);
    transmitter();
    break;
  case Res:
    receive();
    break;
  case Final:
    delay(500);
    transmitter();
    break;
  case Rep:
    receive();
    break;
  }
  // if (check == Pol)
  // {
  //   delay(2000);
  // }
}
