#include <Common.S>

// We already initialize in RootScene
//
// NOTE: We already allocated a networking heap in RootScene
// This means NetScene gets 650kb for just DWC, since it doesn't handle SO anymore.
// Since most of that usage is for DWC itself, this is quite small.
//

PATCH_REPLACE_START(NetManager_connect, 0x3c)
    li r3, 0 // +4 [bl SOStartup]
PATCH_REPLACE_END(NetManager_connect, 0x3c)

PATCH_BL_START(GetGsbrcd, 0)
    mflr r12
    bl strEnd
        .string "RMCJ13hnq6r"
        .align 4
    strEnd:
        mflr r25
    mtlr r12
    blr
PATCH_BL_END(GetGsbrcd, 0)