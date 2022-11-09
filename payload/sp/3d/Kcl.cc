#include "Kcl.hh"

extern "C" {
#include <revolution.h>
#include <revolution/gx.h>
}

#include <algorithm>
#include <egg/math/eggMath.hh>
namespace SP {

// Sizes, in bytes
struct SectionSizes {
    const u32 pos_data_size = 0;
    const u32 nrm_data_size = 0;
    const u32 prism_data_size = 0;
    const u32 block_data_size = 0;
};

static SectionSizes GetSectionSizes(const KCollisionV1Header &header, u32 file_size) {
    /*
     0 pos_data_offset
     1 nrm_data_offset
     2 prism_data_offset
     3 block_data_offset
     4 <end_of_file>
    */
    struct SectionEntry {
        u32 offset = 0;
        size_t index = 0;
        u32 size = 0;
    };

    // prism_data_offset is 1-indexed, so the offset is pulled back
    std::array<SectionEntry, 5> sections{SectionEntry{.offset = header.pos_data_offset, .index = 0},
            SectionEntry{.offset = header.nrm_data_offset, .index = 1},
            SectionEntry{.offset = static_cast<u32>(
                                 header.prism_data_offset + sizeof(KCollisionPrismData)),
                    .index = 2},
            SectionEntry{.offset = header.block_data_offset, .index = 3},
            SectionEntry{.offset = file_size, .index = 4}};

    std::sort(sections.begin(), sections.end(),
            [](auto &lhs, auto &rhs) { return lhs.offset < rhs.offset; });

    for (size_t i = 0; i < 4; ++i) {
        sections[i].size = sections[i + 1].offset - sections[i].offset;
    }

    std::sort(sections.begin(), sections.end(),
            [](auto &lhs, auto &rhs) { return lhs.index < rhs.index; });

    const u32 pos_data_size = sections[0].size;
    const u32 nrm_data_size = sections[1].size;
    const u32 prism_data_size = sections[2].size;
    const u32 block_data_size = sections[3].size;

    return {pos_data_size, nrm_data_size, prism_data_size, block_data_size};
}

template <typename T, typename byte_view_t>
static inline T *reinterpret_buffer(byte_view_t data, unsigned offset = 0) {
    if (offset + sizeof(T) > data.size_bytes())
        return nullptr;

    return reinterpret_cast<T *>(data.data() + offset);
}

template <typename T, typename byte_view_t>
static inline std::span<T> span_cast(byte_view_t data, unsigned offset = 0) {
    if (offset + sizeof(T) > data.size_bytes())
        return {};

    const size_t buffer_len = ROUND_DOWN(data.size_bytes(), sizeof(T));
    return {reinterpret_cast<T *>(data.data() + offset),
            reinterpret_cast<T *>(data.data() + buffer_len)};
}

KclFile::KclFile(std::span<const u8> bytes) {
    m_header = reinterpret_buffer<const KCollisionV1Header>(bytes);
    assert(m_header != nullptr);
    if (m_header == nullptr) {
        return;
    }
    const auto sizes = GetSectionSizes(*m_header, bytes.size());
    SP_LOG("SIZES: position %u bytes; normal %u bytes; prism %u bytes; block %u bytes",
            sizes.pos_data_size, sizes.nrm_data_size, sizes.prism_data_size, sizes.block_data_size);
    if (m_header->pos_data_offset + sizes.pos_data_size < bytes.size()) {
        pos = span_cast<const Vec3>(bytes.subspan(m_header->pos_data_offset, sizes.pos_data_size));
    }
    if (m_header->nrm_data_offset + sizes.nrm_data_size < bytes.size()) {
        nrm = span_cast<const Vec3>(bytes.subspan(m_header->nrm_data_offset, sizes.nrm_data_size));
    }
    if (m_header->prism_data_offset + sizes.prism_data_size < bytes.size()) {
        prism = span_cast<const KCollisionPrismData>(
                bytes.subspan(m_header->prism_data_offset, sizes.prism_data_size));
    }
    if (m_header->block_data_offset + sizes.block_data_size < bytes.size()) {
        block = bytes.subspan(m_header->block_data_offset, sizes.block_data_size);
    }
}

inline std::array<Vec3, 3> FromPrism(float height, const Vec3 &pos, const Vec3 &fnrm,
        const Vec3 &enrm1, const Vec3 &enrm2, const Vec3 &enrm3) {
    auto CrossA = cross(enrm1, fnrm);
    auto CrossB = cross(enrm2, fnrm);
    auto Vertex1 = pos;
    auto Vertex2 = pos + CrossB * (height / dot(CrossB, enrm3));
    auto Vertex3 = pos + CrossA * (height / dot(CrossA, enrm3));

    return {Vertex1, Vertex2, Vertex3};
}

KclVis::KclVis(std::span<const u8> file) : m_file(file) {
    prepare();
}
static GXColor TransformHRadians(GXColor in, // color to transform
        float H) {
    float U = EGG::Math<float>::cos(H);
    float W = EGG::Math<float>::sin(H);

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
inline GXColor GetKCLColor(u32 attr, GXColor base_color) {
    const auto attr_fraction = static_cast<float>(attr & 31) / 31.0f;

    return TransformHRadians(base_color, attr_fraction * (3.1415f * 2.0f) + 60.0f);
}
void KclVis::prepare() {
    const size_t count = m_file.prism.size();
    m_DLSize = count * 3 * sizeof(DirectVertex);
    SP_LOG("Prism count: %u; Buffer size: %u\n", (u32)count, m_DLSize);
    m_DL = std::make_unique<u8[]>(m_DLSize);
    assert(m_DL);

    DirectVertex *vertices = reinterpret_cast<DirectVertex *>(m_DL.get());

    for (size_t i = 0; i < count; ++i) {
        const KCollisionPrismData &prism = m_file.prism[i];
        const auto &fnrm = m_file.nrm[prism.fnrm_i];
        const auto verts = FromPrism(prism.height, m_file.pos[prism.pos_i], fnrm,
                m_file.nrm[prism.enrm1_i], m_file.nrm[prism.enrm2_i], m_file.nrm[prism.enrm3_i]);
        auto gx_clr = GetKCLColor(prism.attribute, GXColor{0xf4, 0xa2, 0x61, 0x88});
        u32 clr = (u32&)gx_clr;
        vertices[i * 3] = {verts[0], clr};
        vertices[i * 3 + 1] = {verts[1], clr};
        vertices[i * 3 + 2] = {verts[2], clr};
    }
}

namespace lgx {
void bypassSend(u32 regval) {
    GXColor1x8(0x61);
    GXColor1u32(regval);
}
} // namespace lgx
struct TranslucentVertexColors {
    TranslucentVertexColors() {
        createDisplayListCached();
    }

    void createDisplayListCached() {
        alignas(32) static const u8 CACHED_DL[] = {0x61, 0x28, 0x04, 0x90, 0x00, 0x61, 0xC0, 0x08,
                0xA8, 0x9F, 0x61, 0xC1, 0x08, 0xFF, 0xD0, 0x61, 0xC0, 0x08, 0xAF, 0xFF, 0x61, 0xC1,
                0x08, 0xBF, 0xF0, 0x61, 0x22, 0x00, 0x06, 0x0C, 0x61, 0x40, 0x00, 0x00, 0x17, 0x61,
                0x41, 0x00, 0x04, 0xBD, 0x61, 0xF3, 0x3F, 0x00, 0x00, 0x61, 0x40, 0x00, 0x00, 0x07,
                0x61, 0x41, 0x00, 0x04, 0xBD, 0x61, 0x41, 0x00, 0x04, 0xAD, 0x61, 0x00, 0x00, 0x00,
                0x10, 0x08, 0x50, 0x00, 0x00, 0x22, 0x00, 0x08, 0x60, 0x00, 0x00, 0x00, 0x00, 0x10,
                0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x01, 0x08, 0x70, 0x40, 0x77, 0x70, 0x09,
                0x08, 0x80, 0xC8, 0x24, 0x12, 0x09, 0x08, 0x90, 0x04, 0x82, 0x41, 0x20, 0x00, 0x10,
                0x00, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x10, 0x0E, 0x00,
                0x00, 0x05, 0x01, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x05, 0x01, 0x10, 0x00,
                0x00, 0x10, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        static_assert((std::size(CACHED_DL) % 32) == 0);
        mDl = {CACHED_DL, std::size(CACHED_DL)};
    }

    void use() {
        GXCallDisplayList(mDl.data(), mDl.size());
        lgx::bypassSend(0x43000041);
    }
    void unuse() {
        lgx::bypassSend(0x43000041);
    }

private:
    std::span<const u8> mDl;
};

void KclVis::render(const float mtx[3][4]) {
    GXLoadPosMtxImm(mtx, 0);
    TranslucentVertexColors mMaterial;
    mMaterial.use();

    const size_t count = m_file.prism.size();
    assert(m_DL != nullptr);
    DirectVertex *vertices = reinterpret_cast<DirectVertex *>(m_DL.get());
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, count * 3);
    for (size_t i = 0; i < count * 3; ++i) {
        auto &v = vertices[i];
        GXPosition3f32(v.pos.x, v.pos.y, v.pos.z);
        GXColor1u32(v.clr);
    }
    GXEnd();
    mMaterial.unuse();
}

} // namespace SP