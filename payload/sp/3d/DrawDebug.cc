#include "DrawDebug.hh"
#include "Checkpoints.hh"
#include <game/render/DrawList.hh>
#include <game/system/SaveManager.hh>

extern "C" {
#include <revolution.h>
#include <revolution/gx.h>
}

#ifdef DRAW_PERF
extern "C" {
#include <sp/FlameGraph.h>
}
#endif // DRAW_PERF

#include <sp/settings/ClientSettings.hh>

#include "Kcl.hh"
#include <game/system/ResourceManager.hh>

namespace SP {

// TODO: This code being in the SP folder doesn't make too much sense I think.

KclVis *kcl;

void DrawDebug() {
#ifdef DRAW_PERF
    SIMPLE_PERF;
#endif // DRAW_PERF
    const std::array<float, 12> mtx = Render::DrawList::spInstance->getViewMatrix();
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugCheckpoints>();
    if (setting == SP::ClientSettings::DebugCheckpoints::Enable) {
#ifdef DRAW_PERF
        SIMPLE_PERF_NAMED("Checkpoints");
#endif // DRAW_PERF
       // DrawCheckpoints(Decay(mtx));
        if (kcl) {
            kcl->render(Decay(mtx));
        }
    }
}

void REPLACED(KCLManager_fromFile)(void *mgr, void *file);
REPLACE void KCLManager_fromFile(void *mgr, void *file) {
    REPLACED(KCLManager_fromFile)(mgr, file);
    std::span<const u8> bytes{(u8 *)file, (u8 *)file + 10000000 /* act of faith */};
    kcl = new KclVis(bytes);
}

void REPLACED(KCLManager_dsi)(void* mgr);
REPLACE void KCLManager_dsi(void* mgr) {
    REPLACED(KCLManager_dsi)(mgr);
    delete kcl;
    kcl = nullptr;
}

} // namespace SP
