#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"
// #define SPI_SCK 18
// #define SPI_MISO 19
// #define SPI_MOSI 23
// #define DW_CS 4
 
// // connection pins
// const uint8_t PIN_RST = 27; // reset pin
// const uint8_t PIN_IRQ = 34; // irq pin
// const uint8_t PIN_SS = 4;   // spi select pin
HardwareSerial Serial1(PA10, PA9);
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0;  // irq pin
const uint8_t PIN_SS = SS;    // spi select pin
void newRange();
void newDevice(DW1000Device *device);
void inactiveDevice(DW1000Device *device);
// TAG antenna delay defaults to 16384
// leftmost two bytes below will become the "short address"
char tag_addr[] = "7D:00:22:EA:82:60:3B:9C";
 
void setup()
{
  Serial1.begin(115200);
  delay(1000);
 
  //init the configuration
  //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
 
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
 
// start as tag, do not assign random short address
 
  DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}
 
void loop()
{
  DW1000Ranging.loop();
}
 
void newRange()
{
  Serial1.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial1.print(",");
  Serial1.println(DW1000Ranging.getDistantDevice()->getRange());
}
 
void newDevice(DW1000Device *device)
{
  Serial1.print("Device added: ");
  Serial1.println(device->getShortAddress(), HEX);
}
 
void inactiveDevice(DW1000Device *device)
{
  Serial1.print("delete inactive device: ");
  Serial1.println(device->getShortAddress(), HEX);
}


// char this_anchor_addr[] = "84:00:22:EA:82:60:3B:9C";
// float this_anchor_target_distance = 0.5; // measured distance to anchor in m

// uint16_t this_anchor_Adelay = 16600; //starting value
// uint16_t Adelay_delta = 100; //initial binary search step size
 
 
// void setup()
// {
//   Serial1.begin(115200);
//   while (!Serial1);
//   //init the configuration
//   //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
//   DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
 
 
//   Serial1.print("Starting Adelay "); Serial1.println(this_anchor_Adelay);
//   Serial1.print("Measured distance "); Serial1.println(this_anchor_target_distance);
  
//   DW1000.setAntennaDelay(this_anchor_Adelay);
 
//   DW1000Ranging.attachNewRange(newRange);
//   DW1000Ranging.attachNewDevice(newDevice);
//   DW1000Ranging.attachInactiveDevice(inactiveDevice);
//   //Enable the filter to smooth the distance
//   //DW1000Ranging.useRangeFilter(true);
 
//   //start the module as anchor, don't assign random short address
//   DW1000Ranging.startAsAnchor(this_anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
 
// }
 
// void loop()
// {
//   DW1000Ranging.loop();
// }
 
// void newRange()
// {
//   static float last_delta = 0.0;
//   Serial1.print(DW1000Ranging.getDistantDevice()->getShortAddress(), DEC);
 
//   float dist = 0;
//   for (int i = 0; i < 100; i++) {
//     // get and average 100 measurements
//     dist += DW1000Ranging.getDistantDevice()->getRange();
//   }
//   dist /= 100.0;
//   Serial1.print(",");
//   Serial1.print(dist); 
//   if (Adelay_delta < 3) {
//     Serial1.print(", final Adelay ");
//     Serial1.println(this_anchor_Adelay);
// //    Serial1.print("Check: stored Adelay = ");
// //    Serial1.println(DW1000.getAntennaDelay());
//     while(1);  //done calibrating
//   }
 
//   float this_delta = dist - this_anchor_target_distance;  //error in measured distance
 
//   if ( this_delta * last_delta < 0.0) Adelay_delta = Adelay_delta / 2; //sign changed, reduce step size
//     last_delta = this_delta;
  
//   if (this_delta > 0.0 ) this_anchor_Adelay += Adelay_delta; //new trial Adelay
//   else this_anchor_Adelay -= Adelay_delta;
  
//   Serial1.print(", Adelay = ");
//   Serial1.println (this_anchor_Adelay);
// //  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
//   DW1000.setAntennaDelay(this_anchor_Adelay);
// }
 
// void newDevice(DW1000Device *device)
// {
//   Serial1.print("Device added: ");
//   Serial1.println(device->getShortAddress(), HEX);
// }
 
// void inactiveDevice(DW1000Device *device)
// {
//   Serial1.print("delete inactive device: ");
//   Serial1.println(device->getShortAddress(), HEX);
// }