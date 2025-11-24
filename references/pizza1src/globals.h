//
//  globals.h
//  PizzaVsSkeletons
//
//  Created by Paul Stevens on 6/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include "rmglobals.h"
#include "rmstdinput.h"
#include "rmgui.h"

struct GamesInterstitialGUIStd;

extern Point1 PLAY_X;
extern Point1 PLAY_Y;
extern Point1 PLAY_W;
extern Point1 PLAY_H;
extern Point1 PLAY_CX;
extern Point1 PLAY_CY;
extern Point2 PLAY_XY;
extern Point2 PLAY_WH;
extern Point2 PLAY_CENTER;
extern Box PLAY_BOX;

static const Point2 RM_WH(568.0, 384.0);
static const Box RM_BOX(Point2(0.0, 0.0), RM_WH);

extern String1 imgSuffix;
extern String1 langSuffix;

enum WorldType
{
  WORLD_GRAVEYARD,
  WORLD_BADLANDS,
  WORLD_QUARRY,
  WORLD_PIRATE,
  WORLD_STADIUM,
  
  WORLD_NUCLEAR,
  WORLD_COMPUTER,
  WORLD_CARNIVAL,
  WORLD_MOON,
  WORLD_MEAT,
  
  WORLD_CLOUD,
  WORLD_MARKET,
  
  NUM_WORLD_TYPES
};

// LevelType matches level icon indices
enum LevelType
{
  LEVEL_GLADIATOR,
  LEVEL_KARATE,
  LEVEL_LAKE,
  LEVEL_SLOPES,
  LEVEL_WRECKING,
  
  LEVEL_BALANCE,
  LEVEL_BOUNCE,
  LEVEL_PUPPY,
  LEVEL_ANGRY,
  LEVEL_SUMO,
  
  LEVEL_PLANE,
  LEVEL_TIMING,
  
  NUM_LEVEL_TYPES
};

enum GladiatorLevelType
{
  // 0
  GLADIATOR_TUTORIAL,
  
  // 1
  GLADIATOR_SPEARMEN,
  GLADIATOR_SPINY,
  GLADIATOR_GARGOYLE,
  GLADIATOR_STICKY,  // sticky x2
  GLADIATOR_OSTRICH_V,
  
  // 6
  GLADIATOR_GIANT,  // sticky x1
  GLADIATOR_FLAMESKULL,  // flames
  GLADIATOR_OSTRICH_H,
  GLADIATOR_CUPID,  // flames
  GLADIATOR_ROBOT,
  
  // 11
  GLADIATOR_WISP,
  GLADIATOR_GRAVITY,  // flames
  GLADIATOR_NAGA,  // 
  GLADIATOR_STICKY_MIX,  // some flames
  GLADIATOR_ENDING,
  
  // 16
  GLADIATOR_LITE_ENDING,
  GLADIATOR_RABBIT,
  GLADIATOR_TAIL,
  GLADIATOR_DISK
};

enum KarateLevelType
{
  KARATE_STILL,
  KARATE_BOUNCE,
  KARATE_WALK,
  KARATE_FLY,
  KARATE_HOP,
  
  KARATE_SPEAR,
  KARATE_ILLUSION,
  KARATE_CIRCLE,
  KARATE_GRAVITY,
  KARATE_MIX,
  
  KARATE_MUMMY
};

enum SlopeLevelType
{
  SLOPES_A,
  SLOPES_B,
  SLOPES_C,
  SLOPES_D,
  SLOPES_E,
  
  SLOPES_F,
  SLOPES_G,
  SLOPES_H,
  SLOPES_I,
  SLOPES_J  
};

enum BalanceLevelType
{
  BALANCE_A,
  BALANCE_B,
  BALANCE_C,
  BALANCE_D,
  BALANCE_E,
  
  BALANCE_F,
  BALANCE_G,
  BALANCE_H,
  BALANCE_I
};

enum BounceLevelType
{
  BOUNCE_EASY,
  BOUNCE_ARROW,
  BOUNCE_HEAVY,
  BOUNCE_SQUISH,
  BOUNCE_BIG,
  
  BOUNCE_LIGHTNING,
  BOUNCE_EXPLODE,
  BOUNCE_SUN,
  BOUNCE_STREAKER,
  BOUNCE_ENERGY
};

enum PuppyLevelType
{
  PUPPY_EASY,
  PUPPY_SPIKEPIT,
  PUPPY_MOLE,
  PUPPY_SPIKEBALL,
  PUPPY_FALLPIT,
  
  PUPPY_FIREBALL,
  PUPPY_BOMB,
  PUPPY_MIMIC,
  PUPPY_ARROW,
  PUPPY_MIX,
  
  PUPPY_SUN
};

enum AngryLevelType
{
  ANGRY_STONEHENGE,
  ANGRY_CASTLE,
  ANGRY_PYRAMID,
  ANGRY_CABIN,
  ANGRY_FORT,
  
  ANGRY_IGLOO,
  ANGRY_PAGODA,
  ANGRY_GLASS,
  ANGRY_CAVE,
  ANGRY_CLOUDCITY,
  
  ANGRY_MARKET
};

enum LakeLevelType
{
  LAKE_EASY,
  LAKE_PEARL,
  LAKE_STARFISH,
  LAKE_PUFFER,
  LAKE_TIDE,
  
  LAKE_SWORDFISH,
  LAKE_POISON,
  LAKE_HARPOON,
  LAKE_BARREL,
  LAKE_EEL
};

enum WreckLevelType
{
  WRECK_LINES,
  WRECK_CIVILIAN,
  WRECK_BASEBALL,
  WRECK_CLIMBER,
  WRECK_JUNK,
  
  WRECK_FLIER,
  WRECK_HARDWALL,
  WRECK_UFO,
  WRECK_CUTTER,
  WRECK_MIX,
  
  WRECK_BATTER
};

enum SumoLevelType
{
  SUMO_GRAVEYARD,
  SUMO_BADLANDS,
  SUMO_QUARRY,
  SUMO_PIRATE,
  SUMO_STADIUM,
  
  SUMO_NUCLEAR,
  SUMO_COMPUTER,
  SUMO_CARNIVAL,
  SUMO_MOON,
  SUMO_MEAT,
  
  SUMO_CLOUD,
  SUMO_MARKET
};

enum PlaneLevelType
{
  PLANE_CUPID,
  PLANE_WISP,
  PLANE_NAGA,
  PLANE_NODE
};

enum TimingLevelType
{
  TIMING_CIRCLES,
  TIMING_SCALERS,
  TIMING_SLIDERS
};

enum GameVarType
{
  VAR_DAMAGE,
  VAR_DEFEATED,
  VAR_NUM_COINS,
  VAR_CASH,
  VAR_SCORE,
  VAR_SECONDS,
  VAR_WHEEL_EARNED,
  
  NUM_VAR_TYPES
};

struct LevelData
{
  // GENERATED DATA
  Coord2 typeData;  // [LEVEL_ANGRY, ANGRY_STONEHENGE]
  Coord2 goalTypes;  // StarGoalType
  Coord2 goalValues;  // could be time, boards, etc, indices correspond 
  Logical lockBroken;  // if stars = 0, lockBroken is false until player touches
  String1 instructionsBase;
  
  // SAVED DATA
  Coord1 starsEarned;  // 0 stars is unlocked but not yet beaten
  Coord1 bestScore;
  
  enum StarGoalType
  {
    GOAL_TIME,
    GOAL_UNHARMED,
    GOAL_BOARDS,
    GOAL_COINS,
    GOAL_OBJECTS,
    GOAL_ENEMIES,
    GOAL_METERS,  // uses VAR_DEFEATED
    GOAL_WHEEL,
    
    NUM_GOAL_TYPES
  };
  
  LevelData();
  LevelData(Coord1 modeID, Coord1 levelID, Coord1 setGoalValA = 0, Coord1 setGoalValB = 0);
  void setModeDefaults();  // sets goals and instructions based on level type
  void setStarsEarned(Coord1 stars);
};

extern ArrayList<String2> 
goalEarnedStrs,
goalRequiredStrs;

extern ArrayList<String1> modeStrs;
extern ArrayList<String1> worldStrs;

extern ArrayList<ArrayList<LevelData> > WORLD_LEVELS;
extern ArrayList<ArrayList<Coord1> > STARS_TO_UNLOCK;

extern String2 MASTER_LSTRING;  // this is for debug
extern ArrayList<String2> LSTRINGS;
extern String1 LANGUAGE_CODE;
extern LanguageID LANGUAGE_ID;

// chunk types
enum ChunkType
{
  CHUNK_SKI_START,
  CHUNK_SKI_4_1,  
  CHUNK_SKI_3_1,
  CHUNK_SKI_2_1,
  CHUNK_SKI_JUMP,
  CHUNK_SKI_DROP,
  CHUNK_SKI_GAP,
  CHUNK_SKI_BOUNCEGAP,
  CHUNK_SKI_BOUNCEJUMP,
  CHUNK_SKI_DUALCOIN,
  CHUNK_SKI_SPIKEPLATFORMS,
  CHUNK_SKI_FLAMESKULL,
  CHUNK_SKI_HILLSKULL,
  CHUNK_SKI_JAGSKULL,
  CHUNK_SKI_LONGFALL,
  CHUNK_SKI_SPIKEFALL,
  CHUNK_SKI_STRAIGHTBREAK,
  CHUNK_SKI_SWITCHSLOPE,
  CHUNK_SKI_SWITCHGAP,
  CHUNK_SKI_SPIKEPATHS,
  CHUNK_SKI_FALLBREAK,
  CHUNK_SKI_END,
  
  CHUNK_SPIKE_START,
  CHUNK_SPIKE_FLAT,
  CHUNK_SPIKE_BUMP,
  CHUNK_SPIKE_DOWNHILL,
  CHUNK_SPIKE_DROP,
  CHUNK_SPIKE_UPHILL,
  CHUNK_SPIKE_BOUNCECIRCLES,
  CHUNK_SPIKE_BOUNCEDROPS,
  CHUNK_SPIKE_LONGFALL,
  CHUNK_SPIKE_NOSPIKE,
  CHUNK_SPIKE_RISER,
  CHUNK_SPIKE_PILLAR,
  CHUNK_SPIKE_SPIKEBALL,
  CHUNK_SPIKE_SPRING,
  CHUNK_SPIKE_STRAIGHTBOUNCE,
  CHUNK_SPIKE_TRANSFERBLOCK,
  CHUNK_SPIKE_TRANSFERDROP,
  CHUNK_SPIKE_TRANSFERSPIKE,
  CHUNK_SPIKE_BOUNCESTEPS,
  CHUNK_SPIKE_DOWNHILLBUMP,
  CHUNK_SPIKE_DUALRISE,
  CHUNK_SPIKE_RISEBALLS,
  CHUNK_SPIKE_RISESTEPS,
  CHUNK_SPIKE_BALLDOWNHILL,
  CHUNK_SPIKE_BALLJUMP,
  CHUNK_SPIKE_ZIGZAG,
  CHUNK_SPIKE_END,
  
  CHUNK_PUPPY_DOGHOUSE,
  CHUNK_PUPPY_HILL,
  CHUNK_PUPPY_CRATE,
  CHUNK_PUPPY_DIP,
  CHUNK_PUPPY_SPIKEPITHILL,
  CHUNK_PUPPY_SPIKEJUMP,
  CHUNK_PUPPY_TRICKYJUMP,
  CHUNK_PUPPY_LAVAHILL,
  CHUNK_PUPPY_BIGOBJHILL,
  CHUNK_PUPPY_BOUNCEPIT,
  CHUNK_PUPPY_CRATEPIT,
  CHUNK_PUPPY_DUALFIREBALL,
  CHUNK_PUPPY_GROUNDTRIANGLE,
  CHUNK_PUPPY_LAVAPOOL,
  CHUNK_PUPPY_OBJSTEPS,
  CHUNK_PUPPY_PILLARS,
  CHUNK_PUPPY_QUADSPIKE,
  CHUNK_PUPPY_SINGLETRI,
  CHUNK_PUPPY_SPIKEBALLBUMPS,
  CHUNK_PUPPY_SPIKEBALLHILL,
  CHUNK_PUPPY_SPIKEBALLPILLARS,
  CHUNK_PUPPY_SPIKEBALLPIT,
  CHUNK_PUPPY_SWINGSQUARE,
  CHUNK_PUPPY_TRIANGLEPIT,
  CHUNK_PUPPY_WALLJUMP,
  
  NUM_CHUNK_TYPES
};

enum PuppyVariation
{
  FAVOR_NOTHING,
  FAVOR_CAGE,
  FAVOR_CRATE,
  FAVOR_MOLE,
  FAVOR_MIMIC,
  FAVOR_CAGE_SPECIAL
};

// DO NOT CHANGE ORDER, these mirror the tab indices and the item set indices
enum PizzaLayer
{
  CHEESE_LAYER,
  TOPPINGS_LAYER,
  EYES_LAYER,
  MOUTH_LAYER,
  CLOTHES_LAYER,
  
  NUM_PIZZA_LAYERS
};

struct PizzaItem
{
  String2 name;
  Coord1 price;
  
  PizzaItem() {} // this is just for the compiler to init the array list
  PizzaItem(const String2& setName, Coord1 setPrice) : name(setName), price(setPrice) {}
};

extern ArrayList<ArrayList<PizzaItem> > TOPPING_DEFS;

const Point2 EYE_HANDLES[] = 
{
  Point2(0.0, 0.0),
  Point2(80.0, 48.0),
  Point2(92,56),
  Point2(176,16),
  Point2(76,56),
  Point2(96,40),
  Point2(80,48),
  Point2(96,64),
  Point2(84,80),
  Point2(120,32),
  Point2(72,80),
  Point2(92,48),
  Point2(108,72)
};

const Point2 MOUTH_HANDLES[] = 
{
  Point2(0.0, 0.0),
  Point2(80.0, -16.0),
  Point2(52.0, -24.0),
  Point2(72,-32),
  Point2(80,0),
  Point2(80,-16),
  Point2(56,-8),
  Point2(88,-32),
  Point2(88,-32),
  Point2(96,16),
  Point2(64,-32),
  Point2(80,-32),
  Point2(104,-40),
};

const Point2 CLOTHES_HANDLES[] = 
{
  Point2(0.0, 0.0),
  Point2(168.0, 208.0),
  Point2(152,88),
  Point2(240,184),
  Point2(168,256),
  Point2(200,80),
  Point2(256,32),
  Point2(96,120),
  Point2(208,256),
  Point2(208,176),
  Point2(160,184),
  Point2(160,208),
  Point2(200, 220)
};

const Coord1 TOPPINGS_PER_PIZZA = 5;
const Point2 TOPPING_HANDLES[][TOPPINGS_PER_PIZZA] =
{
  {
    Point2(0.0, 0.0),
    Point2(0.0, 0.0),
    Point2(0.0, 0.0),
    Point2(0.0, 0.0),
    Point2(0.0, 0.0)
  },

  {
    Point2(116.0, 28.0),
    Point2(36.0, 56.0),
    Point2(-20.0, 100.0),
    Point2(-72.0, 52.0),
    Point2(-28.0, -52.0)
  },
  
  {
    Point2(116.0, 84.0),
    Point2(-4.0, 108.0),
    Point2(-32.0, -40.0),
    Point2(36.0, -32.0),
    Point2(100.0, -8.0)
  },
  
  {
    Point2(100,84),
    Point2(-12,108),
    Point2(-52,-16),
    Point2(52,-68),
    Point2(100,32)
  },

  {
    Point2(80,68),
    Point2(-16,96),
    Point2(-44,-20),
    Point2(16,-48),
    Point2(104,-12)
  },

  {
    Point2(84,72),
    Point2(-4,88),
    Point2(-64,48),
    Point2(-4,-32),
    Point2(100,20)
  },
  
  // 4
  {
    Point2(116,84),
    Point2(-4,108),
    Point2(-32,-20),
    Point2(36,-32),
    Point2(100,8)
  },

  // 5
  {
    Point2(60,112),
    Point2(-16,100),
    Point2(-40,8),
    Point2(44,-16),
    Point2(112,-8)
  },

  // 8
  {
    Point2(96,84),
    Point2(20,144),
    Point2(-56,56),
    Point2(24,-12),
    Point2(120,8)
  },

  // 9
  {
    Point2(28,116),
    Point2(-40,88),
    Point2(-64,-20),
    Point2(56,-36),
    Point2(112,36)
  },

  // 10
  {
    Point2(48.0, 96.0),
    Point2(-24.0, 72.0),
    Point2(-32.0, -8.0),
    Point2(56.0, -24.0),
    Point2(96.0, 32.0)
  },

  // 11
  {
    Point2(100.0, 84.0),
    Point2(-20.0, 84.0),
    Point2(-8.0, 12.0),
    Point2(52.0, -56.0),
    Point2(124.0, 12.0)
  },

  // 12
  {
    Point2(160.0, 104.0),
    Point2(56.0, 88.0),
    Point2(-48.0, 128.0),
    Point2(108.0, -8.0),
    Point2(-56.0, -28.0)
  },
};

extern ArrayList<ColorP4> MAP_SKY_COLORS;
extern Point2 CHUNK_ENDS[NUM_CHUNK_TYPES];
extern ArrayList<Point1> CUPID_FIRE_TIMES;

// constants
const Point1 
PLAYER_ROLL_FORCE = 7500.0,
PLAYER_ROLL_TORQUE = 330.0,
PLAYER_JUMP_VEL = -400.0;

const Point1 KARATE_MIDDLE_WIDTH = 480.0;

const Point2 LAKE_SIZE(3072.0, 1536.0);
const Coord2 LAKE_SCREENS(6, 4);  // these are ipad sized
const Point2 LAKE_SCREEN_SIZE(512.0, 384.0);
const Point2 LAKE_WORLD_SIZE(LAKE_SCREEN_SIZE.x * LAKE_SCREENS.x, 
                             LAKE_SCREEN_SIZE.y * LAKE_SCREENS.y);
const Point2 LAKE_PLAYER_START(LAKE_SCREEN_SIZE * Point2(2.0, 4.0) - Point2(240.0, 160.0));
const Box LAKE_WORLD_BOX(Point2(0.0, 0.0), LAKE_WORLD_SIZE);

const ColorP4 BLINK_STAR_COLORS[] =
{
  RM::color255(255,255,255),
  RM::color255(0,0,0),
  RM::color255(92,207,0),
  RM::color255(38,102,0),
  RM::color255(176,256,128),
  RM::color255(247,69,0),
  RM::color255(117,46,0),
  RM::color255(266,181,140),
  RM::color255(0,153,255),
  RM::color255(0,61,99),
  RM::color255(148,209,255),
  RM::color255(255,219,0),
  RM::color255(255,130,0),
  RM::color255(255,135,173),
  RM::color255(255,69,173),
  RM::color255(138,31,255)
};

const ColorP4 SPIN_DISK_COLORS[] =
{
  RM::color255(236,134,104),
  RM::color255(255,209,104),
  RM::color255(153,209,107),
  RM::color255(61,186,224),
  RM::color255(153,138,204)
};

const ColorP4 
HEALTH_COLOR_START = RM::color255(236, 100, 68),
HEALTH_COLOR_PULSE = RM::color255(255, 233, 109);

Char const * const playerFilename = "player.sav";
Char const * const levelFilename = "levels.sav";

Char const * const PLAYER_PASTEBOARD_LITE = "com.paulstevens.pizzavsskeletonsfree.player";
Char const * const LEVEL_PASTEBOARD_LITE = "com.paulstevens.pizzavsskeletonsfree.levels";

Char const * const FULL_VERSION_URL = "http://bit.ly/pizzavssk";

// this is BEFORE the game's achievement offset
const Coord1 
ACH_10TH_LEVEL = 0,
ACH_50TH_LEVEL = 1,
ACH_100TH_LEVEL = 2,
ACH_BOUGHT_ING = 3,
ACH_SECRET_ING = 4,
ACH_ALL_RETRO = 5,
ACH_50_STARS = 6,
ACH_300_STARS = 7,
ACH_WHEEL_DOLLAR = 8,
ACH_3_PEARLS = 9,
ACH_PHOTO = 10,
ACH_WINDY = 11,
ACH_SHAVE = 12,
ACH_CARPET = 13,
ACH_DEBUGGER = 14;

struct PizzaController : PointerDataStd, AccelDataStd
{
  GamePadDataStd gamepad;
  KeyboardDataStd keyboard;
  
  Point1 calib;
  Point1 calibVert;
  
  Logical pressTaken;
  Logical usedDirsLast;  // if you last clicked the screen or the mouse, this is false
  
  PizzaController();
  void update();

  Point2 getXY();
  Point2 cursorToScreen(Point2 cursorXY);

  Logical isOutsidePause();
  Logical clickedOutsidePause();
  
  Logical getAnyConfirmStatus(Coord1 status);

  Logical getConfirmButtonStatus(Coord1 status);
  Logical getCancelButtonStatus(Coord1 status);
  Logical getDirectionStatus(Coord1 dir8, Coord1 status, Coord1 controllerID);  // includes sticks
  
  Logical getDirectionalButtonStatus(Coord1 dir8, Coord1 status);  // returns dir8 or ORIGIN
  Coord1 getDirection(Coord1 status);  // returns dir8 or ORIGIN
  void updateTypeUsedLast();
};

struct ScoreManager;
struct PizzaExtender;

// =============================== Pizza ============================ //

namespace Pizza 
{
  extern Logical iPad;
  extern Logical lite;
  extern Logical widescreen;
  
  extern PizzaController* controller;
  extern GamesInterstitialManagerStd* moreGamesMan;
  extern PizzaExtender* extender;

  const Point1 SCENE_WIPE_DUR = 0.35;
  const Point1 GUI_TRANS_DUR = 0.25;
  
  extern ScoreManager* scoreMan;
  extern Coord1 LEADERBOARD_ID;
  extern Coord1 ACH_OFFSET;
  extern Logical gcLogonShown;
  extern Logical useLoadImg;
  
  extern Point1 worldMapAlpha;
  extern Logical liteBtnDown; // this is a hack to disable the full version button when true
  extern Point2 platformTL; // (0.0, 0.0) on iPhone, (-32.0, -64.0) on iPad

  extern CameraStd
  platformCam,
  adjustCam;  // this is to make pizza's coordinate system work (0,0) with a (480x320) res

  extern Box platScreenBox; // includes ipad offset, not scale
  extern Coord1 shareBGID;  // save the last one in memory
  extern Logical showShopTut;
  
  // DEBUG constants
  extern Logical 
  DEBUG_COLLISION,
  DEBUG_PHYSICS,
  DEBUG_INVINCIBLE,
  DEBUG_NO_INTERFACE;

  extern const Char* DEBUG_LANGUAGE;  // NULL means off
  
  // gameplay vars
  extern Coord2 currWorldPair;  // ex. [world4, level8]
  extern Coord2 currLevelDef;   // ex. [level sumo, sumo moon]
  
  // ======== saved in player.sav
  extern Coord1 playerCash; // player's stable cash, in cents
  extern ArrayList<Coord1> playerOutfit;
  extern ArrayList<Coord1> playerUnlocks;
  // ========
  
  extern Coord1 playerScore;  // player's stable score, does NOT included current level, NOT STORED
  extern Coord1 playerStars; // this is calculated during "updatePlayerStars" NOT STORED
  
  extern Point2 cameraPosition;
  extern Point2 cameraMoved;
  
  extern ArrayList<Coord1> currGameVars;
  extern Point1 currFloatTime;
  extern Coord2 currIngredientFound;  // usually (-1, -1)
  extern Coord2 currIngredientUnlocked; // usually (-1, -1)
  
  // this should be called after global constants are initialized and version is set up
  Logical isFreeVersion();
  void initGameConstants();
  void initMoreGamesMan();
  void initWorldConstants();
  void initToppingConstants();
  void setLevels();
  void processStrings();
  
  Logical isWorldUnlocked(Coord1 worldID);
  Logical isLevelUnlocked(Coord2 levelID);
  Coord1 topWorldUnlocked();
  
  Logical toppingExists(Coord2 toppingID);
  Logical isToppingUnlocked(Coord2 toppingID);
  void unlockTopping(Coord2 toppingID);  // DOES NOT SAVE in case called 2+ times in a row
  
  Byte4 starBitsForWonLevel();  // BITMAP of stars, always returns 1+ because it assumes the level is won
  void runGameTime(Point1 time);  // adds to current time
  Coord1 goalTypeToVarIndex(Coord1 goalType);
  
  LevelData& currLevelData();  // uses currWorldPair
  LevelData& getLevelData(Coord2 worldPair);
  Coord2 lookupLevelPos(Coord2 levelDef);  // [GLADIATOR, SPEARMEN] -> [WORLD_GRAVEYARD, 1] or [-1, -1] error
  Coord1 calcStarBonus(Coord1 worldID, Coord1 numStars);
  Coord1 calcTimeBonus(const LevelData& level, Coord1 seconds);
  Logical alreadyBeatenGame();
  
  void getInstructionsStr(const LevelData& level, String1& output);
  
  void unlockLevelsDebug();  
  void unlockOutfitsDebug();
  
  void resetGame();
  
  // call after initilizing levels then updatePlayerStats()
  void loadPlayer();  
  void loadLevels();
  
  void readPlayer(const String1& fullPlayerStr);
  void savePlayer();  // saves cash, curr outfit, unlocked toppings
  void writePlayer(String1& destination);
  
  void readLevels(const String1& fullLevelStr);
  void saveLevels();  // saves all level scores and star coints
  void writeLevels(String1& destination);

  void updatePlayerStats();
  Coord1 starsInFirst100();  // counts stars in first 100 levels
  
  Box deviceScreenBox();
  void updateScreenCam();
  
  void letterboxContent(Point2 maxContent);
  Box leftLetterbox(Point2 maxContent);
  Box rightLetterbox(Point2 maxContent);
  Box contentBox(Point2 maxContent);
}

// =============================== ScoreManager ============================ //

struct ScoreManager : SystemGameCenterListener
{  
  // settings
  String1 saveFilename;
  Point1 repostSeconds;
  
  ArrayList<Coord1> topScores;
  Coord1 scoresPosted;
  
  ArrayList<Point1> achievements;
  Coord1 achPosted;
  
  Logical needsSave;  // indicates that a score or achievement has been earned since the last save
  Point1 currTime;
  
  Logical recentLoginFailed;
  
  static Logical DEBUG_REPORT;
  static const Coord1 MAX_SCORES = 4;
  static const Coord1 MAX_ACHIEVEMENTS = 30;
  
  ScoreManager();
  
  void load();
  void save();
  
  void update();
  
  void checkAndRepost();
  Coord1 topScore();
  
  // this is boardID, not the localized version
  // returns true if it's the new high score.
  Logical earnedScore(Coord1 localID, Coord1 score);
  
  // returns true if the achievement was just finished for the first time,
  //   according to local cached data
  Logical earnedAchievement(Coord1 localID, Point1 percent = 100.0);
  
  // ========== from SystemGameCenterListener
  void score_posted(Coord1 boardID, Coord1 score);
  void achievement_posted(Coord1 achID, Point1 percent);
  void gc_tried_logon(Logical success);
};

#endif
