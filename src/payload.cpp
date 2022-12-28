#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <map>
#include <string>

#include "atlas_types.h"
//#include "config.h"
#include "protocol.h"
#include "protocol_structures.h"

typedef enum
{
    CFG_ROLE_LISTENER = 0,
    CFG_ROLE_TAG,
    CFG_ROLE_ANCHOR,
    CFG_ROLE_TAG_TDOA,
    CFG_ROLE_NUM_MODES,
    CFG_ROLE_SYNC_ANCHOR
} cfgRole_t;