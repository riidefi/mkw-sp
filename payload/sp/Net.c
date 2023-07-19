#include "Net.h"

#include <revolution.h>
#include <string.h>

#if 0
#include <egg/core/eggHeap.h>
#endif

#define LOADING_SECTION(...)

static bool sNetIsInit = false;
#if 0
EGG_Heap *spSocketHeap = NULL;
#endif
static OSMutex sSocketMutex;

static void *so_alloc(u32 id, s32 size) {
#if 0
    assert(spSocketHeap);
#endif

    OSLockMutex(&sSocketMutex);

    // Prefer the slab allocator
    {
        void *slab_res = TryAllocFromSlabs(size);
        if (slab_res != NULL) {
            OSUnlockMutex(&sSocketMutex);
            return slab_res;
        }
    }

#if 0
    // Fall back to ExpHeap
    SP_LOG("[SO] Alloc ID(%i) %i bytes\n", id, size);
    void *res = EGG_Heap_alloc(size, 32, spSocketHeap);
    SP_LOG("[SO] Alloc @ %p\n", res);


    OSUnlockMutex(&sSocketMutex);
    return res;
#endif

    OSUnlockMutex(&sSocketMutex);

    SP_LOG("ALLOC ID %i, ALLOC SIZE %i", id, size);

    assert(!"Bad alloc");
    return NULL;
}
static void so_free(u32 UNUSED(id), void *ptr, s32 size) {
#if 0
    assert(spSocketHeap);
#endif

    OSLockMutex(&sSocketMutex);

    // TODO: In theory, we do not need to lock a mutex to free from the slab allocator (?)
    if (TryFreeFromSlabs(ptr, size)) {
        OSUnlockMutex(&sSocketMutex);
        return;
    }

    assert(!"Bad alloc");
#if 0
    OSReport("[SO] Free %i bytes\n", size);
    EGG_Heap_free(ptr, spSocketHeap);
#endif
    OSUnlockMutex(&sSocketMutex);
}

static bool Net_initLow(NetSlabs *netSlabs) {
    if (sNetIsInit)
        return true;

    LOADING_SECTION("Initializing network");

    OSInitMutex(&sSocketMutex);

#if 0
    {
        LOADING_SECTION("Creating linked-list heap");

        spSocketHeap = EGG_ExpHeap_create(
                NET_HEAP_SIZE, scn->heapCollection.heaps[1], /* attr= */ 0);
        assert(spSocketHeap && "Failed to allocate socket heap");
    }
#endif

    {
        LOADING_SECTION("-> Creating slab allocator");

#if 0
        sSlabs = EGG_Heap_alloc(sizeof(NetSlabs), 4, spSocketHeap);
#endif
        sSlabs = netSlabs;
        assert(sSlabs && "Failed to create slab allocator");
        memset(sSlabs, 0, sizeof(*sSlabs));
    }

    {
        LOADING_SECTION("Initializing Socket library");

        SOLibraryConfig cfg;
        cfg.alloc = so_alloc;
        cfg.free = so_free;
        int res = SOInit(&cfg);
        SP_LOG("SOInit returned %i", res);
        assert(res == 0 && "SOInit failed");
    }

    sNetIsInit = true;
    return sNetIsInit;
}

bool Net_init(void) {
    NetSlabs *slabs = OSAllocFromMEM2ArenaLo(sizeof(NetSlabs), 4);

    return Net_initLow(slabs);
}

typedef struct {
    char _[0x28];
    char accountData[0x40];
    // ...
} Holder_UserData;

static Holder_UserData **UserData = (Holder_UserData **)0x809C2110;

extern bool DWC_CheckUserData(void *);
extern bool DWCi_Acc_IsAuthentic(void *);

u8 sAccData[] = {
    //
    0x00, 0x00, 0x00, 0x40,                                                  // size
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x1B, 0xE8, 0xDB,  // psuedo
    0x00, 0x00, 0x09, 0x1C, 0x28, 0xC5, 0xC6, 0x65, 0x47, 0x1B, 0xE8, 0xDB,  // authentic
    0x23, 0xDC, 0x5B, 0x5F,  // gs_profile_Id
    0x00, 0x00, 0x00, 0x00,  // flag
    'R', 'M', 'C', 'J',      // gamecode
    0x00, 0x00, 0x00, 0x00,  // reserved[0]
    0x00, 0x00, 0x00, 0x00,  // reserved[1]
    0x00, 0x00, 0x00, 0x00,  // reserved[2]
    0x00, 0x00, 0x00, 0x00,  // reserved[3]
    0x00, 0x00, 0x00, 0x00,  // reserved[4]
    0xF7, 0xD5, 0xE2, 0x1F,  // crc32
};

extern int doCheckuserData;

bool MyCheckUserData(void* p) {
    memcpy(p, sAccData, sizeof(sAccData));
    return DWC_CheckUserData(p);
}


PATCH_BL(doCheckuserData, MyCheckUserData);

int NetManager_connect;
void NetManager_myConnect() {
    Holder_UserData *pUserData = *UserData;

    memcpy(&pUserData->accountData, sAccData, sizeof(pUserData->accountData));

    SP_LOG("@@@@@@@@@@@@@@@@");

    SP_LOG("DWCi_Acc_IsAuthentic");
    assert(DWCi_Acc_IsAuthentic(pUserData->accountData));

    SP_LOG("DWC_CheckUserData");
    assert(DWC_CheckUserData(sAccData));

    SP_LOG("Done4");
}
PATCH_BL(NetManager_connect + 0x38, NetManager_myConnect);

