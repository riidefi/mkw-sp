#pragma once

#include <Common.hh>

namespace Registry {

enum class Character {
    Mario = 0x00,
    BabyPeach = 0x01,
    Waluigi = 0x02,
    Bowser = 0x03,
    BabyDaisy = 0x04,
    DryBones = 0x05,
    BabyMario = 0x06,
    Luigi = 0x07,
    Toad = 0x08,
    DonkeyKong = 0x09,
    Yoshi = 0x0A,
    Wario = 0x0B,
    BabyLuigi = 0x0C,
    Toadette = 0x0D,
    KoopaTroopa = 0x0E,
    Daisy = 0x0F,
    Peach = 0x10,
    Birdo = 0x11,
    DiddyKong = 0x12,
    KingBoo = 0x13,
    BowserJr = 0x14,
    DryBowser = 0x15,
    FunkyKong = 0x16,
    Rosalina = 0x17,
    SmallMiiOutfitAMale = 0x18,
    SmallMiiOutfitAFemale = 0x19,
    SmallMiiOutfitBMale = 0x1A,
    SmallMiiOutfitBFemale = 0x1B,
    SmallMiiOutfitCMale = 0x1C,
    SmallMiiOutfitCFemale = 0x1D,
    MediumMiiOutfitAMale = 0x1E,
    MediumMiiOutfitAFemale = 0x1F,
    MediumMiiOutfitBMale = 0x20,
    MediumMiiOutfitBFemale = 0x21,
    MediumMiiOutfitCMale = 0x22,
    MediumMiiOutfitCFemale = 0x23,
    LargeMiiOutfitAMale = 0x24,
    LargeMiiOutfitAFemale = 0x25,
    LargeMiiOutfitBMale = 0x26,
    LargeMiiOutfitBFemale = 0x27,
    LargeMiiOutfitCMale = 0x28,
    LargeMiiOutfitCFemale = 0x29,
    MediumMii = 0x2A,
    SmallMii = 0x2B,
    LargeMii = 0x2C,
};

enum class Vehicle {
    StandardKartSmall = 0x00,
    StandardKartMedium = 0x01,
    StandardKartLarge = 0x02,
    BoosterSeat = 0x03,
    ClassicDragster = 0x04,
    Offroader = 0x05,
    MiniBeast = 0x06,
    WildWing = 0x07,
    FlameFlyer = 0x08,
    CheepCharger = 0x09,
    SuperBlooper = 0x0A,
    PiranhaProwler = 0x0B,
    TinyTitan = 0x0C,
    Daytripper = 0x0D,
    Jetsetter = 0x0E,
    BlueFalcon = 0x0F,
    Sprinter = 0x10,
    Honeycoupe = 0x11,
    StandardBikeSmall = 0x12,
    StandardBikeMedium = 0x13,
    StandardBikeLarge = 0x14,
    BulletBike = 0x15,
    MachBike = 0x16,
    FlameRunner = 0x17,
    BitBike = 0x18,
    Sugarscoot = 0x19,
    WarioBike = 0x1A,
    Quacker = 0x1B,
    ZipZip = 0x1C,
    ShootingStar = 0x1D,
    Magikruiser = 0x1E,
    Sneakster = 0x1F,
    Spear = 0x20,
    JetBubble = 0x21,
    DolphinDasher = 0x22,
    Phantom = 0x23,
};

enum class Controller {
    WiiWheel = 0x0,
    WiiRemoteAndNunchuck = 0x1,
    Classic = 0x2,
    GameCube = 0x3,
};

enum class Course {
    MarioCircuit = 0x0,
    MooMooMeadows = 0x1,
    MushroomGorge = 0x2,
    GrumbleVolcano = 0x3,
    ToadsFactory = 0x4,
    CoconutMall = 0x5,
    DKSummit = 0x6,
    WarioGoldMine = 0x7,
    LuigiCircuit = 0x8,
    DaisyCircuit = 0x9,
    MoonviewHighway = 0xA,
    MapleTreeway = 0xB,
    BowsersCastle = 0xC,
    RainbowRoad = 0xD,
    DryDryRuins = 0xE,
    KoopaCape = 0xF,
    GCNPeachBeach = 0x10,
    GCNMarioCircuit = 0x11,
    GCNWaluigiStadium = 0x12,
    GCNDKMountain = 0x13,
    DSYoshiFalls = 0x14,
    DSDesertHills = 0x15,
    DSPeachGardens = 0x16,
    DSDelfinoSquare = 0x17,
    SNESMarioCircuit3 = 0x18,
    SNESGhostValley2 = 0x19,
    N64MarioRaceway = 0x1A,
    N64SherbetLand = 0x1B,
    N64BowsersCastle = 0x1C,
    N64DKsJungleParkway = 0x1D,
    GBABowserCastle3 = 0x1E,
    GBAShyGuyBeach = 0x1F,
    DelfinoPier = 0x20,
    BlockPlaza = 0x21,
    ChainChompRoulette = 0x22,
    FunkyStadium = 0x23,
    ThwompDesert = 0x24,
    GCNCookieLand = 0x25,
    DSTwilightHouse = 0x26,
    SNESBattleCourse4 = 0x27,
    GBABattleCourse3 = 0x28,
    N64Skyscraper = 0x29,
    GalaxyColosseum = 0x36,
    WinDemo = 0x37,
    LoseDemo = 0x38,
    DrawDemo = 0x39,
    EndingDemo = 0x3A,
};

// Excerpt from Sound::SoundId
//
// The rationale is that magic_enum is limited to 300 enum values, which the SoundID far exceeds.
enum class CourseBgm {
    STRM_N_CIRCUIT1_N = 117,
    STRM_N_CIRCUIT1_F = 118,
    STRM_N_FARM_N = 119,
    STRM_N_FARM_F = 120,
    STRM_N_KINOKO_N = 121,
    STRM_N_KINOKO_F = 122,
    STRM_N_FACTORY_N = 123,
    STRM_N_FACTORY_F = 124,
    STRM_N_CASTLE_N = 125,
    STRM_N_CASTLE_F = 126,
    STRM_N_SHOPPING_N = 127,
    STRM_N_SHOPPING_F = 128,
    STRM_N_SNOWBOARD_N = 129,
    STRM_N_SNOWBOARD_F = 130,
    STRM_N_TRUCK_N = 131,
    STRM_N_TRUCK_F = 132,
    STRM_N_WATER_N = 133,
    STRM_N_WATER_F = 134,
    STRM_N_CIRCUIT2_N = 135,
    STRM_N_CIRCUIT2_F = 136,
    STRM_N_DESERT_N = 137,
    STRM_N_DESERT_F = 138,
    STRM_N_VOLCANO_N = 139,
    STRM_N_VOLCANO_F = 140,
    STRM_N_RIDGEHIGHWAY_N = 141,
    STRM_N_RIDGEHIGHWAY_F = 142,
    STRM_N_TREEHOUSE_N = 143,
    STRM_N_TREEHOUSE_F = 144,
    STRM_N_KOOPA_N = 145,
    STRM_N_KOOPA_F = 146,
    STRM_N_RAINBOW_N = 147,
    STRM_N_RAINBOW_F = 148,
    STRM_R_AGB_BEACH_N = 149,
    STRM_R_AGB_BEACH_F = 150,
    STRM_R_SFC_OBAKE_N = 151,
    STRM_R_SFC_OBAKE_F = 152,
    STRM_R_SFC_CIRCUIT_N = 153,
    STRM_R_SFC_CIRCUIT_F = 154,
    STRM_R_AGB_KUPPA_N = 155,
    STRM_R_AGB_KUPPA_F = 156,
    STRM_R_64_SHERBET_N = 157,
    STRM_R_64_SHERBET_F = 158,
    STRM_R_64_CIRCUIT_N = 159,
    STRM_R_64_CIRCUIT_F = 160,
    STRM_R_64_JUNGLE_N = 161,
    STRM_R_64_JUNGLE_F = 162,
    STRM_R_64_KUPPA_N = 163,
    STRM_R_64_KUPPA_F = 164,
    STRM_R_GC_BEACH_N = 165,
    STRM_R_GC_BEACH_F = 166,
    STRM_R_GC_CIRCUIT_N = 167,
    STRM_R_GC_CIRCUIT_F = 168,
    STRM_R_GC_STADIUM_N = 169,
    STRM_R_GC_STADIUM_F = 170,
    STRM_R_GC_MOUNTAIN_N = 171,
    STRM_R_GC_MOUNTAIN_F = 172,
    STRM_R_DS_JUNGLE_N = 173,
    STRM_R_DS_JUNGLE_F = 174,
    STRM_R_DS_TOWN_N = 175,
    STRM_R_DS_TOWN_F = 176,
    STRM_R_DS_DESERT_N = 177,
    STRM_R_DS_DESERT_F = 178,
    STRM_R_DS_GARDEN_N = 179,
    STRM_R_DS_GARDEN_F = 180,
    STRM_N_BTL_VENICE_N = 181,
    STRM_N_BTL_VENICE_F = 182,
    STRM_N_BTL_BLOCK_N = 183,
    STRM_N_BTL_BLOCK_F = 184,
    STRM_N_BTL_SKATE_N = 185,
    STRM_N_BTL_SKATE_F = 186,
    STRM_N_BTL_CASINO_N = 187,
    STRM_N_BTL_CASINO_F = 188,
    STRM_N_BTL_SANDSTONE_N = 189,
    STRM_N_BTL_SANDSTONE_F = 190,
    STRM_R_BTL_GC_COOKIE_N = 191,
    STRM_R_BTL_GC_COOKIE_F = 192,
    STRM_R_BTL_DS_HOUSE_N = 193,
    STRM_R_BTL_DS_HOUSE_F = 194,
    STRM_R_BTL_SFC_BTL4_N = 195,
    STRM_R_BTL_SFC_BTL4_F = 196,
    STRM_R_BTL_AGB_BTL3_N = 197,
    STRM_R_BTL_AGB_BTL3_F = 198,
    STRM_R_BTL_64_MATENRO_N = 199,
    STRM_R_BTL_64_MATENRO_F = 200,
    STRM_M_BOSS1 = 201,
    STRM_O_GP_AWARD = 202,
    STRM_O_VS_AWARD = 203,
    STRM_O_AWARD_CUT = 204,
    STRM_O_AWARD_LOSE = 205,
    STRM_O_ENDING_PART_A = 206,
    STRM_O_GALLERY = 207,
};

extern const Course OrderedCourses[0x20];

bool IsCombinationValid(Character character, Vehicle vehicle);

u32 GetCharacterMessageId(u32 characterId, bool resolveMiiNames);
REPLACE bool UseBattleRenders();

s32 GetButtonIndexFromCourse(Course course);
const char *GetCharacterPane(u32 characterId);
const char *GetCupIconName(u32 cupId);
u32 GetCupMessageId(u32 cupId);
u32 GetCourseCupId(Course courseId);
u32 GetCourseName(Course courseId);

const char *REPLACED(GetItemPane)(u32 itemId, u32 count);
REPLACE const char *GetItemPane(u32 itemId, u32 count);

// Returns the message ID for the given error code.
int REPLACED(WifiErrorExplain)(u32 error_code);
REPLACE u32 WifiErrorExplain(u32 error_code);

} // namespace Registry
