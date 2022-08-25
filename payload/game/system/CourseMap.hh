#pragma once

#include <Common.hh>

namespace System {

// TODO: Better naming convention

struct MapdataCheckPath {
  u8 start; //!< [+0x00]
  u8 size;  //!< [+0x01]

  u8 last[6]; //!< [+0x02]
  u8 next[6]; //!< [+0x08]
};
struct MapdataCheckPoint {
  Vec2 left;  //!< [+0x00] First point of the checkpoint.
  Vec2 right; //!< [+0x08] Second point of the checkpoint.
  u8 jugemIndex;       //!< [+0x10] Respawn point.
  u8 lapCheck; //!< [+0x11] 0 - start line, 1-254 - key checkpoints, 255 -
               //!< normal
  u8 prevPt;   //!< [+0x12] Last checkpoint. 0xFF -> sequence edge
  u8 nextPt;   //!< [+0x13] Next checkpoint. 0xFF -> sequence edge
};
struct MapdataJugemPoint {
  Vec3 position, rotation;
  u16 id;
  s16 range;
};



//! @brief Section Header that starts all .kmp file sections
struct ResSectionHeader {
  u32 magic;  //!< [+0x00] Four characters
  u16 nEntry; //!< [+0x04] Number of entries in the section
  union {
    u16 userData; //!< [+0x06] The POTI section stores the total number of
                  //!< points of all routes. The CAME section store different
                  //!< values.
    struct {
      s8 openingPanIdx; //!< [+0x06] CAME opening pan idx.
      s8 videoPanIdx;   //!< [+0x07] CAME video pan idx.
    } cameraData;
  };
};

//!
template <class res> class MapdataAccessorBase {
public:
  res* mpData; //!< [+0x00] Pointer to the mapping data structure.

  explicit inline MapdataAccessorBase(void* p) : mpData((res*)p) {}
  explicit inline MapdataAccessorBase(const void* p) : mpData((res*)p) {}

  inline res* ptr() { return mpData; }
  inline const res* ptr() const { return mpData; }

  inline res& ref() { return *mpData; }
  inline const res& ref() const { return *mpData; }
};


// hacky res definition
template <typename T> struct SectionAccessor {
  T** mEntryAccessors; //!< [+0x00]
  u16 mNumEntries;             //!< [+0x04]
  ResSectionHeader* mpSection; //!< [+0x08]

  inline T* tryGet(u32 index) {
    return index < mNumEntries ? mEntryAccessors[index] : NULL;
  }

  inline const T* tryGet(u32 index) const {
    return index < mNumEntries ? mEntryAccessors[index] : NULL;
  }
};

struct MapdataCheckPointAccessor : public MapdataAccessorBase<MapdataCheckPoint> {
    // ...
};
struct MapdataCheckPathAccessor : public MapdataAccessorBase<MapdataCheckPath> {
    // ...
};
struct MapdataJugemPointAccessor : public MapdataAccessorBase<MapdataJugemPoint> {
    // ...
};


struct MapdataCheckPointSectionAccessor : public SectionAccessor<MapdataCheckPointAccessor> {
    // ...
};
struct MapdataCheckPathSectionAccessor : public SectionAccessor<MapdataCheckPathAccessor> {
    // ...
};
struct MapdataJugemPointSectionAccessor : public SectionAccessor<MapdataJugemPointAccessor> {
    // ...
};


class CourseMap {
public:
  virtual void vf00() = 0;
  virtual void vf04() = 0;
  virtual void dt() = 0;

  static CourseMap* spInstance;

  void* mpCourse;

  void* mpKartPoint;
  void* mpEnemyPath;
  void* mpEnemyPoint;
  void* mpItemPath;
  void* mpItemPoint;
  MapdataCheckPathSectionAccessor* mpCheckPath;
  MapdataCheckPointSectionAccessor* mpCheckPoint;
  void* mpPointInfo;
  void* mpGeoObj;
  void* mpArea;
  void* mpCamera;
  MapdataJugemPointSectionAccessor* mpJugemPoint;
  void* mpCannonPoint;
  void* mpStageInfo;
  void* mpMissionPoint;

  void* mGoalCamera;
  void* mType9Camera;
  void* mOpeningPanCamera;
  u32 _50; // UNK
};

} // namespace System