#include <Arduino.h>
#include <HardwareSerial.h>
#include "stimer.h"
#include <iostream>
#include <map>
#define MAX_RX_BUFFER 128
#define PREAMBLE    0x62b5
#define TXID_ANCHOR 0xdeca030000000001
uint8_t RxBuffer[MAX_RX_BUFFER] = {0};
int size = 0;
const uint16_t payloadSize = 0x5FF;

// class Parser
// {
// private:
//     const uint64_t ticksPerRevolution = 0x10000000000;
//     std::map<uint64_t, protocol::Protocol *> m_decoders;
//     // std::map<uint64_t, std::map<uint64_t, std::map<uint64_t, measurement_t>>> m_measurements;
//     // std::map<uint64_t, uint64_t> m_lastSequence;
//     // std::map<uint64_t, uint64_t> m_lastTimestamp;
//     // std::map<uint64_t, uint64_t> m_tagWhitelist;

// public:
//     Parser();
//     ~Parser();

//     //void initialize(const configParser_t &parser, const configAnchors_t &anchors, const configSync_t &sync);
//     void poll();
//     //void extractSamples(std::vector<sample_t> *s);
// };

#pragma pack(1)
typedef struct
{
    uint16_t preamble;
    uint16_t messageId;
    uint16_t payloadLength;
    uint8_t payload[payloadSize];
    uint16_t checksum;
} packet_t;

typedef struct
{
    uint8_t role;
    uint64_t eui;
    uint8_t channel;
} msgConfig_t;

typedef struct
{
    uint32_t syncPeriod;
    uint32_t tagPeriod;
} msgPeriod_t;

typedef struct
{
    uint64_t txId;
    uint64_t rxId;
    uint64_t rxTs;
    uint8_t seqNr;
} msgToa_t;

packet_t packet = {0};

uint16_t CalculatedChecksum(packet_t *packet)
{
    uint8_t a = 0;
    uint8_t b = 0;

    a = a + (packet->messageId & 0xFF);
    b = b + a;

    a = a + (packet->messageId >> 8);
    b = b + a;

    a = a + (packet->payloadLength & 0xFF);
    b = b + a;

    a = a + (packet->payloadLength >> 8);
    b = b + a;

    for (uint16_t i = 0; i < packet->payloadLength; ++i)
    {
        a = a + packet->payload[i];
        b = b + a;
    }

    return (b << 8) + a;
}

void ProcessPacket(uint8_t *buffer, int size, packet_t *packet)
{
    Serial1.printf("Receive %d bytes \r\n", size);
    char *err = NULL;
    int success = -1;
    packet->preamble = buffer[0] >> 8 | buffer[1];
    packet->messageId = buffer[2] | buffer[3];
    packet->payloadLength = buffer[4] | buffer[5];
    packet->payload[size] = buffer[6];
    packet->checksum = buffer[7] | buffer[8];
    do {
        
        if (packet->preamble != PREAMBLE) 
        {
            err = "Wrong Preamble Code";
            break;
        }

        if (packet->messageId != msg_id_config && packet->messageId != msg_id_period && packet->messageId != msg_id_toa && packet->messageId != msg_id_toad)
        {
            err = "Wrong Message ID";
            break;
        }

        if (CalculatedChecksum(packet) != packet->checksum)
        {
            err = "CHECKSUM ERROR!!!!";
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

// void Parser::poll()
// {
//     for (auto it = m_decoders.begin(); it != m_decoders.end(); ++it)
//     {
//         protocol::packet_t packet;
//         if (it->second->poll(&packet))
//         {
//             if (it->second->calculateCheckSum(&packet) != packet.checksum)
//             {
//                 std::cout << it->first << " CHECKSUM ERROR!!! ";
//                 std::cout << std::hex << "x" << packet.messageId << ", x" << packet.payloadLength << ", x" << packet.checksum << ", x" << it->second->calculateCheckSum(&packet);
//                 std::cout << " payload:";
//                 for (uint16_t i = 0; i < packet.payloadLength; ++i)
//                 {
//                     std::cout << std::hex << " x" << (int)packet.payload[i];
//                 }
//                 std::cout << std::endl;
//             }
//             else
//             {
//                 protocol::msgToa_t msg;
//                 memcpy(&msg, packet.payload, sizeof(protocol::msgToa_t));

//                 //std::cout << it->first << " Received packet ";
//                 //std::cout << std::hex << msg.txId << ", " << msg.rxId << ", x" << msg.rxTs << ", x" << (int)msg.seqNr << std::endl;

//                 // Linearize receive Timestamps
//                 uint64_t ts = msg.rxTs;
//                 uint64_t rxeui = msg.rxId;

//                 if (m_lastTimestamp.find(rxeui) == m_lastTimestamp.end())
//                 {
//                     m_lastTimestamp.insert(std::make_pair(rxeui, ts));
//                 }
//                 else
//                 {
//                     int64_t lastts = m_lastTimestamp[rxeui];
//                     int64_t res = lastts % ticksPerRevolution;

//                     if ((int64_t)ts - (int64_t)res > 0)
//                     {
//                         m_lastTimestamp[rxeui] = lastts + (ts - res);
//                         //std::cout << "+ ts:" << ts << " lastts:" << lastts << " res:" << res << std::endl;
//                     }
//                     if ((int64_t)ts - (int64_t)res < 0)
//                     {
//                         m_lastTimestamp[rxeui] = lastts + (ts - res) + ticksPerRevolution;
//                         //std::cout << "- ts:" << ts << " lastts:" << lastts << " res:" << res << std::endl;
//                     }
//                 }

//                 if (m_tagWhitelist.find(msg.txId) != m_tagWhitelist.end())
//                 {
//                     measurement_t meas;
//                     meas.hts = std::chrono::system_clock::now();
//                     meas.ts = m_lastTimestamp[rxeui];
//                     meas.txeui = msg.txId;
//                     meas.rxeui = msg.rxId;
//                     meas.seq = msg.seqNr;
//                     meas.fpPower = 0;
//                     meas.rxPower = 0;
//                     meas.fpRatio = 0;

//                     m_measurements[meas.txeui][meas.seq].insert(std::make_pair(meas.rxeui, meas));
//                 }
//                 else
//                 {
//                     std::cout << "Warning: " << msg.txId << " not in Whitelist " << std::endl;
//                 }
//             }
//         }
//     }

//     usleep(100);
// }