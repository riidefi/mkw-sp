#pragma once

#include "KartState.h"

#include "../race/Driver.h"

#include "../sound/KartSound.h"

typedef struct {
    u8 _00[0x04 - 0x00];
    KartState *state;
    u8 _08[0x14 - 0x08];
    Driver *driver;
    u8 _18[0x1c - 0x18];
    KartSound *sound;
    u8 _20[0x64 - 0x20];
} KartAccessor;
static_assert(sizeof(KartAccessor) == 0x64);

typedef struct {
    KartAccessor *accessor;
    u8 _0[0xc - 0x4];
} KartObjectProxy;
static_assert(sizeof(KartObjectProxy) == 0xc);
