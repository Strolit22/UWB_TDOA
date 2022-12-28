#include <Arduino.h>
#include <HardwareSerial.h>
#include "stimer.h"
#define MAX_RX_BUFFER 128
#define PREAMBLE    0x62b5
uint8_t RxBuffer[MAX_RX_BUFFER] = {0};
int size = 0;
const uint16_t payloadSize = 0x5FF;

#pragma pack(1)
typedef struct
{
    uint16_t preamble;
    uint16_t messageId;
    uint16_t payloadLength;
    uint8_t payload[payloadSize];
    uint16_t checksum;
} packet_t;



packet_t packet = {0};
uint16_t CalculatedChecksum(packet_t *packet)
{

}
void ProcessPacket(uint8_t *buffer, int size, packet_t *packet)
{
    Serial1.printf("Receive %d bytes \r\n", size);
    char *err = NULL;
    int success = -1;
    packet->preamble = buffer[0] >> 8 | buffer[1];
    packet->messageId = buffer[2] | buffer[3];
    do {
        
        if (packet->preamble != PREAMBLE) 
        {
            err = "Wrong Preamble Code";
            break;
        }
        
    } while (0);
    
    
    // for (int i = 0; i < packet->size; i++)
    //     Serial1.printf("%c", packet->data[i]);
    // Serial1.println();
    // packet->size = 0;
    // memset(packet->data, 0, MAX_RX_BUFFER);
}
void SerialPoll(void *param)
{
    if (Serial1.available())
    {
        int c = 0;
        int index = 0;
        uint32_t tick = millis();
        while (true)
        {
            c = Serial1.read();
            if (c >= 0)
            {
                RxBuffer[index++] = (uint8_t)c;
                tick = millis();
            }
            if (millis() - tick >= 10)
            {
                size = index;
                ProcessPacket(RxBuffer, size, &packet);
                break;
            }
        }
    }
    startTimer(10, SerialPoll, NULL);
}

void app_protocol_start(void)
{
    startTimer(10, SerialPoll, NULL);
}
