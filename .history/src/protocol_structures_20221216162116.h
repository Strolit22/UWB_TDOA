//
//  protocol_structures.h
//  atlas
//
//  Created by Janis on 26.05.15.
//  Copyright (c) 2015 Janis. All rights reserved.
//

#ifndef __atlas__protocol_structures__
#define __atlas__protocol_structures__

namespace protocol
{

#pragma pack(1)
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

}

#endif /* defined(__atlas__protocol_structures__) */