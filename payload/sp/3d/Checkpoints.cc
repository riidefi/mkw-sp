#include "Checkpoints.hh"
#include <Common.hh>
#include <algorithm>
#include <cstring>
#include <game/kart/KartObjectProxy.hh>
#include <game/system/CourseMap.hh>
#include <game/system/RaceManager.hh>
#include <iterator>
extern "C" {
#include <revolution.h>
#include <revolution/gx.h>
}

namespace EGG {
template <typename T>
struct Math {
    static T sin(T);
    static T cos(T);
};
} // namespace EGG

GXColor TransformH(GXColor in, // color to transform
        float H) {
    float U = EGG::Math<float>::cos(H * 3.141592f / 180.0f);
    float W = EGG::Math<float>::sin(H * 3.141592f / 180.0f);

    GXColor ret;
    ret.r = (.299 + .701 * U + .168 * W) * in.r + (.587 - .587 * U + .330 * W) * in.g +
            (.114 - .114 * U - .497 * W) * in.b;
    ret.g = (.299 - .299 * U - .328 * W) * in.r + (.587 + .413 * U + .035 * W) * in.g +
            (.114 - .114 * U + .292 * W) * in.b;
    ret.b = (.299 - .3 * U + 1.25 * W) * in.r + (.587 - .588 * U - 1.05 * W) * in.g +
            (.114 + .886 * U - .203 * W) * in.b;
    ret.a = in.a;
    return ret;
}

namespace lgx {
void bypassSend(u32 regval) {
    GXColor1x8(0x61);
    GXColor1u32(regval);
}
} // namespace lgx

namespace SP {

System::MapdataCheckPathSectionAccessor *CheckPaths() {
    return System::CourseMap::spInstance->mpCheckPath;
}

struct TranslucentVertexColors {
    TranslucentVertexColors() {
        createDisplayListCached();
    }

    void createDisplayListCached() {
        alignas(32) static u8 CACHED_DL[] = {0x61, 0x28, 0x04, 0x90, 0x00, 0x61, 0xC0, 0x08, 0xA8,
                0x9F, 0x61, 0xC1, 0x08, 0xFF, 0xD0, 0x61, 0xC0, 0x08, 0xAF, 0xFF, 0x61, 0xC1, 0x08,
                0xBF, 0xF0, 0x61, 0x22, 0x00, 0x06, 0x0C, 0x61, 0x40, 0x00, 0x00, 0x17, 0x61, 0x41,
                0x00, 0x04, 0xBD, 0x61, 0xF3, 0x3F, 0x00, 0x00, 0x61, 0x40, 0x00, 0x00, 0x07, 0x61,
                0x41, 0x00, 0x04, 0xBD, 0x61, 0x41, 0x00, 0x04, 0xAD, 0x61, 0x00, 0x00, 0x00, 0x10,
                0x08, 0x50, 0x00, 0x00, 0x22, 0x00, 0x08, 0x60, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x01, 0x08, 0x70, 0x40, 0x77, 0x70, 0x09, 0x08,
                0x80, 0xC8, 0x24, 0x12, 0x09, 0x08, 0x90, 0x04, 0x82, 0x41, 0x20, 0x00, 0x10, 0x00,
                0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00,
                0x05, 0x01, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x05, 0x01, 0x10, 0x00, 0x00,
                0x10, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        mDl = &CACHED_DL;
        mDlSize = 0xA0;
    }

    void use() {
        GXCallDisplayList(mDl, mDlSize);
        lgx::bypassSend(0x43000041);
    }
    void unuse() {
        lgx::bypassSend(0x43000041);
    }

private:
    void *mDl;
    u32 mDlSize;
};

class Checkpoints {
public:
    static void onDraw() {
        TranslucentVertexColors mMaterial;
        mMaterial.use();

        for (int i = 0; i < CheckPaths()->mNumEntries; ++i) {
            System::MapdataCheckPath *ckph = CheckPaths()->mEntryAccessors[i]->mpData;

            drawCheckPath(ckph);
        }
        mMaterial.unuse();
    }
    static void drawCheckPath(System::MapdataCheckPath *ckph) {
        int range_start = ckph->start;
        int range_end = ckph->start + ckph->size;

        for (int j = range_start; j < range_end; ++j) {
            System::MapdataCheckPoint *ckpt =
                    System::CourseMap::spInstance->mpCheckPoint->mEntryAccessors[j]->mpData;

            // Skip non-key checkpoints
            if (ckpt->lapCheck == 0xFF && ckpt->nextPt != 0 &&
                    !(ckpt->nextPt == 0xFF && ckph->next[0] == 0))
                continue;

            u8 next_id = ckpt->nextPt;

            // Sequence edge
            if (next_id == 0xFF) {
                if (ckph->next[1] != 0xFF)
                    SP_LOG("!!!\n!!! Odd edge case where a key checkpoint is a "
                           "checkpath edge\n!!!\n");
                // FIXME
                // Are there multiple quads in this case?
                // Right now we just take the first
                // FIXME: We assume the first is the only one!
                u8 next_ckph = ckph->next[0];

                if (next_ckph == 0xFF) {
                    SP_LOG("!!!\n!!! What custom track is this?!\n!!!\n");
                    continue; // Just give up at this point
                }
                next_id = CheckPaths()->mEntryAccessors[next_ckph]->mpData->start;
            }

            {
                System::MapdataCheckPoint *next =
                        System::CourseMap::spInstance->mpCheckPoint->mEntryAccessors[next_id]
                                ->mpData;

                // TODO: Maybe make color match depth
                u32 clr = 0xAAAAAA33;
                if (ckpt->lapCheck != 0xFF)
                    clr = calcColor(j, System::CourseMap::spInstance->mpCheckPoint->mNumEntries);
                drawCheckPoint(ckpt, next, clr);
            }
        }
    }
    static u32 calcColor(u32 i, u32 num) {
        GXColor base;
        (u32 &)base = 0xf4a26199; // 88

        GXColor color = TransformH(base, 360.f * static_cast<float>(i) / static_cast<float>(num));

        return (u32 &)color;
    }
    static void drawCheckPoint(System::MapdataCheckPoint *ckpt, System::MapdataCheckPoint *next,
            u32 color) {
        float top = 5000.0f;
        float bottom = -5000.0f;

        float next_top = 5000.0f;
        float next_bottom = -5000.0f;

        System::MapdataJugemPoint *pt =
                System::CourseMap::spInstance->mpJugemPoint->mEntryAccessors[ckpt->jugemIndex]
                        ->mpData;
        System::MapdataJugemPoint *npt =
                System::CourseMap::spInstance->mpJugemPoint->mEntryAccessors[next->jugemIndex]
                        ->mpData;

        top += pt->position.y;
        bottom += pt->position.y;

        next_top += npt->position.y;
        next_bottom += npt->position.y;

        struct Vertex {
            Vec3 pos;
            u32 flags;
        };

        enum Flags { VTX_DEFAULT = 0, VTX_TRANS = 1 };

        u32 color_trans = (color & 0xFFFFFF00) | 0x40;

#define NUM_QUADS 4

        Vertex faces[NUM_QUADS][4] = {// Front face
                {{{ckpt->left.x, top, ckpt->left.y}, VTX_DEFAULT},
                        {{ckpt->left.x, bottom, ckpt->left.y}, VTX_DEFAULT},
                        {{ckpt->right.x, bottom, ckpt->right.y}, VTX_DEFAULT},
                        {{ckpt->right.x, top, ckpt->right.y}, VTX_DEFAULT}},
                // Left face
                {{{ckpt->left.x, top, ckpt->left.y}, VTX_DEFAULT},
                        {{ckpt->left.x, bottom, ckpt->left.y}, VTX_DEFAULT},
                        {{next->left.x, next_bottom, next->left.y}, VTX_TRANS},
                        {{next->left.x, next_top, next->left.y}, VTX_TRANS}},
                // Right face
                {{{ckpt->right.x, top, ckpt->right.y}, VTX_DEFAULT},
                        {{ckpt->right.x, bottom, ckpt->right.y}, VTX_DEFAULT},
                        {{next->right.x, next_bottom, next->right.y}, VTX_TRANS},
                        {{next->right.x, next_top, next->right.y}, VTX_TRANS}},
                // Back face
                {{{next->left.x, next_top, next->left.y}, VTX_TRANS},
                        {{next->left.x, next_bottom, next->left.y}, VTX_TRANS},
                        {{next->right.x, next_bottom, next->right.y}, VTX_TRANS},
                        {{next->right.x, next_top, next->right.y}, VTX_TRANS}}};

        int to_draw_quads = NUM_QUADS;

        if (ckpt->lapCheck == 0xFF)
            to_draw_quads = 3;

        GXBegin(GX_QUADS, GX_VTXFMT0, to_draw_quads * 4);

        for (int quad = 0; quad < to_draw_quads; ++quad) {
            for (int vert = 0; vert < 4; ++vert) {
                const Vertex &v = faces[quad][vert];

                u32 _c = v.flags & VTX_TRANS ? color_trans : color;
                GXPosition3f32(v.pos.x, v.pos.y, v.pos.z);
                GXColor1u32(_c);
            }
        }
        // GXEnd

        for (int quad = 0; quad < NUM_QUADS; ++quad) {
            GXBegin(GX_LINESTRIP, GX_VTXFMT0, 5);
            for (int vert = 0; vert < 4 + 1; ++vert) {
                const Vertex &v = faces[quad][vert % 4];

                u32 _c = color | 0xFF;
                //	if ((v.flags & VTX_TRANS) == 0)
                //		_c &= 0xFF;
                GXPosition3f32(v.pos.x, v.pos.y, v.pos.z);
                GXColor1u32(_c);
            }
            // GXEnd
        }
    }
};

struct Tracers {
#pragma pack(push)
#pragma pack(1)
    struct Vert {
        Vec3 pos;
        u32 clr;
    };
    struct DL {
        u8 pad0 = 0;
        u8 cmd = (u8)GX_LINES | (u8)GX_VTXFMT0;
        u16 count = 0;
        Vert lines[2 * 11];
        u8 pad1[32] = {};
    };
    alignas(32) DL m_DL;
#pragma pack(pop)

    void reset() {
        m_DL.count = 0;
    }
    bool tryAddLine(Vert vert0, Vert vert1) {
        if (static_cast<std::size_t>(m_DL.count) + 2 > std::size(m_DL.lines)) {
            assert(!"Not enough room");
            return false;
        }
        m_DL.lines[m_DL.count++] = vert0;
        m_DL.lines[m_DL.count++] = vert1;
        return true;
    }
    void draw() {
        const u32 bufLen = 4 + m_DL.count * sizeof(Vert);
        const u32 bufLenRounded = ROUND_UP(bufLen, 32);
        assert(bufLenRounded <= sizeof(m_DL));
        memset(reinterpret_cast<u8 *>(&m_DL) + bufLen, 0, bufLenRounded - bufLen);
        assert(((u32)&m_DL) % 32 == 0);
        assert(bufLenRounded % 32 == 0);
        // DCFlushRange(&m_DL, bufLenRounded);
        // GXCallDisplayList(&m_DL, bufLen);
        for (f64* it = (f64*)&m_DL; it != (f64*)((u8*)(&m_DL) + bufLenRounded); ++it) {
            WGPIPE._f64 = *it;
        }
    }
};

static Tracers sTracers;
Vec3 sCurrent;

void onPositionUpdate(Vec3 pos, u32 id) {
    if (id == 0) {
        sTracers.reset();
        sCurrent = pos;
        return;
    }
    static const u32 colors[2] = {
            0x00FF3C77, // You
            0xCCCC0077 // Ghost
    };
    sTracers.tryAddLine(Tracers::Vert{.pos = sCurrent, .clr = colors[0]},
            Tracers::Vert{.pos = pos, .clr = colors[1]});
}
const Vec3 *PhysicsHook(Kart::KartObjectProxy *proxy) {
    const Vec3 *position = proxy->getPos();
    System::RaceManager *raceManager = System::RaceManager::Instance();
    if (raceManager == nullptr)
        return position;
    const u32 id = proxy->getPlayerId();
    // const u32 lap = raceManager->player(id)->lap();
    onPositionUpdate(*position, id);
    return position;
}
extern "C" int PhysicsHookAddr;
PATCH_BL(PhysicsHookAddr, PhysicsHook);

void DrawCheckpoints() {
    Checkpoints::onDraw();
    sTracers.draw();
}

} // namespace SP