#include "DrawDebug.hh"
#include "Checkpoints.hh"
#include <game/gfx/DrawList.hh>

extern "C" {
#include <revolution/gx.h>
#include <revolution.h>
}

namespace SP {

// TODO: This is in ObjManager
REPLACE void DrawDebug() {
    GXLoadPosMtxImm((const float(*)[4])Gfx::DrawList::spInstance->getViewMatrix().data(), 0);
    DrawCheckpoints();
}

} // namespace SP

