#include <Common.hh>
#include <stdalign.h>
#include <revolution/gx.h>
#include "sp/CircularBuffer.hh"

class Material {
public:
    void use() const {
        GXCallDisplayList(CACHED_DL, sizeof(CACHED_DL));
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
    }
    void end() const {
        // The DL disables alpha update, so reset that
        bypassSetAlphaMask(true);
    }

	void bypassSetAlphaMask(bool enabled) const {
		GXColor1x8(0x61);
		GXColor1u32(0x42000041 | (enabled ? 0x1000000 : 0));
	}
private:
		alignas(32) static const u8 CACHED_DL[] = {
0x61, 0x28, 0x00, 0x00, 0x00,
0x61, 0xC0, 0x08, 0xAF, 0xFF,
0x61, 0xC1, 0x08, 0xBF, 0xF0,
0x61, 0x22, 0x00, 0x06, 0x0C,
0x61, 0x40, 0x00, 0x00, 0x17,
0x61, 0x41, 0x00, 0x04, 0x3D,
0x61, 0xF3, 0x3F, 0x00, 0x00,
0x61, 0x42, 0x00, 0x00, 0x41,
0x61, 0x00, 0x00, 0x00, 0x10,
0x08, 0x50, 0x00, 0x00, 0x22, 0x00,
0x08, 0x60, 0x00, 0x00, 0x00, 0x00,
0x10, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x01,
0x08, 0x70, 0x41, 0x37, 0x70, 0x09,
0x08, 0x80, 0xB9, 0xDC, 0xEE, 0x77,
0x08, 0x90, 0x3B, 0x9D, 0xCE, 0xE7,
0x00,
0x10, 0x00, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x01,
0x10, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00, 0x05, 0x01,
0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x05, 0x01,
0x10, 0x00, 0x00, 0x10, 0x3F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00
		};

};

static Material sMaterial;

static void BeginLines(u32 count) {
    GXBegin(GX_LINES, GX_VTXFMT0, count * 2);
}

static void DrawLine(Vec3 fromPos, u32 fromClr, Vec3 toPos, u32 toClr) {
    GXPosition3f32(fromPos.x, fromPos.y, fromPos.z);
    GXColor1u32(fromClr);
    GXPosition3f32(toPos.x, toPos.y, toPos.z);
    GXColor1u32(toClr);
}

static void EndLines() {
    // GXEnd(): Empty on release
}

struct Tracers {
    u32 mMaxRecordedPlayer = 0;
    SP::CircularBuffer<Vec3, 12> mPositions;
    SP::CircularBuffer<u32, 12> mColors;

    void onPositionUpdate(const Vec3& pos, u32 id, u32 lap) {
        ++mMaxRecordedPlayer;
        static const u32 colors[2] = {
            0xFFEE0077, // You
            0xCCCC0077  // Ghost
        };
        Vec3 _pos = pos;
        u32 _clr = colors[id != 0];
        mPositions.push(_pos);
        mColors.push(_clr);
    }
    void onDraw() {
        if (mMaxRecordedPlayer == 0) return;
        sMaterial.use();
        BeginLines(mMaxRecordedPlayer - 1);
        for (u32 i = 1; i < mMaxRecordedPlayer; ++i) {
            DrawLine(*mPositions.front(), *mColors.front(), *mPositions.back(), *mColors.back());
            mPositions.pop();
            mColors.pop();
        }
        EndLines();
        mMaxRecordedPlayer = 0;
        mPositions.reset();
        mColors.reset();
        sMaterial.end();
    }
};

}