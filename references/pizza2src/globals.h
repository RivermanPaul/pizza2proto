//
//  globals.h
//  NewRMIPhone
//
//  Created by Paul Stevens on 12/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include "rmglobals.h"
#include "rmstdinput.h"
#include "rmgui.h"
#include <map>

struct PizzaGOStd;
struct PointerDataStd;
struct PhysicsLevelStd;
struct PizzaController;
struct PlayerDataMap;
struct IAPGUI;
struct TempGameplayData;

extern PizzaController* controller;
extern PlayerDataMap* defaultPlayer;
extern PlayerDataMap* activePlayer;
extern TempGameplayData* activeGameplayData;

enum PizzaPhase
{
  PHASE_BG_TERRAIN,
  PHASE_SHADOWS,
  PHASE_PLAYER,
  PHASE_PUPPY_CAGES,  // no objects are here, this is a special phase
  PHASE_PUPPY,  // make them behind the cage enemy but in front of player
  PHASE_ENEMY_STD,
  PHASE_ALLY_STD,
  PHASE_FG_WATER,
  PHASE_FG_TERRAIN,
  PHASE_PIRATE_GEMS,  // not drawn in the regular set
  NUM_PHASES
};

enum PGOType
{
  TYPE_UNSET = -1,
  
  TYPE_PLAYER = 1,
  TYPE_PLAYER_LIGHTSABER,
  TYPE_PICKUP,
  TYPE_ENEMY,
  TYPE_TERRAIN,
  
  TYPE_DEBRIS,
  TYPE_PUFFBALL,
  TYPE_PUPPY,
  TYPE_BALLOON,
  TYPE_SUMO_BOSS,
  TYPE_BOSS_LIGHTSABER,

  TYPE_BOSS_ATTACHMENT,  // used for shrooms and lightbulbs
  
  TYPE_PACHINKO_BALL,
  TYPE_PACHINKO_HOLE,
  TYPE_PROJECTILE,  // dragon fireball uses this
  
  TYPE_PASSIVE
};

enum EnemyType
{
  ENEMY_NONE = -1,  // this is used to indicate there is no enemy
  
  // graveyard
  GL_ENEMY_SPEARMAN_UNARMED,
  GL_ENEMY_SPEARMAN_SIDE,
  GL_ENEMY_SPEARMAN_VERT,
  GL_ENEMY_BOMBBAT,

  // jurassic park
  GL_ENEMY_JUMPING_SPIDER,
  GL_ENEMY_CAVEMAN,
  GL_ENEMY_ENT,

  // bakery
  GL_ENEMY_DONUT,
  GL_ENEMY_UTENSIL,

  // dump
  GL_ENEMY_GIANT,
  GL_ENEMY_DOZER,

  // body
  GL_ENEMY_SCIENTIST,
  GL_ENEMY_ALIEN,
  
  // silicon valley
  GL_ENEMY_WISP,
  GL_ENEMY_CHIP,

  // catacombs
  GL_ENEMY_CUPID,
  GL_ENEMY_SPINY,
  GL_ENEMY_REAPER,
  
  // maybe only its own mode
  GL_ENEMY_HAMMER_BROS,  // only enable this one regardless of which is used
  GL_ENEMY_TRIANGLE_BROS,
  GL_ENEMY_DYNAMITE_BROS,

  PU_ENEMY_CAGE,
  PU_ENEMY_FLAMESKULL,
  PU_ENEMY_CHICKEN,
  PU_ENEMY_MOHAWK,
  PU_ENEMY_BOMB,
  AN_ENEMY_SKULL,
  FI_BALLOON_BASKET,
  
  NUM_ENEMY_TYPES
};


enum TerrainValue
{
  VALUE_UNSET = -1,
  VALUE_BUBBLE = 1,
  VALUE_BURNING_BLOCK
};

enum CoinValue
{
  NO_COIN = -3,  // this is for enemies that don't drop something
  RANDOM_COIN = -2,
  RANDOM_GEM = -1,
  
  FIRST_COIN_TYPE,
  COIN_PENNY = FIRST_COIN_TYPE,
  COIN_NICKEL,
  COIN_DIME,
  COIN_QUARTER,
  LAST_COIN_TYPE = COIN_QUARTER,
  
  FIRST_GEM_TYPE,
  GEM_BLUE = FIRST_GEM_TYPE,
  GEM_GREEN,
  GEM_RED,
  GEM_YELLOW,
  LAST_GEM_TYPE = GEM_YELLOW,
  
  GOURDO_ITEM
};

enum CurrencyType
{
  CURRENCY_GOLD,
  CURRENCY_KEYS
};

enum PhysIDType
{
  PHYS_PGO,
  PHYS_PARTICLE
};

enum LevelType
{
  LEVEL_TYPE_GATE, // this is not a playable type
  LEVEL_TYPE_PPARLOR,  // this is not a regular level
  LEVEL_TYPE_GLADIATOR,
  LEVEL_TYPE_ANGRY,
  LEVEL_TYPE_SPRINT,
  LEVEL_TYPE_PIRATE,
  LEVEL_TYPE_PUPPY,
  LEVEL_TYPE_FIREMAN,
  LEVEL_TYPE_SUMO,
  LEVEL_TYPE_LAKE,
  LEVEL_TYPE_PLANE,
  
  NUM_LEVEL_TYPES
};

enum GladiatorLevelType
{
  GL_LEVEL_UNARMED,
  GL_LEVEL_SPEARMEN_SIDE,
  GL_LEVEL_SPEARMEN_VERT,
  GL_LEVEL_BOMBBAT_1,
  GL_LEVEL_BOMBBAT_2,

  GL_LEVEL_SPIDER_1,
  GL_LEVEL_SPIDER_2,
  GL_LEVEL_CAVEMAN_1,
  GL_LEVEL_CAVEMAN_2,
  GL_LEVEL_ENT_1,

  GL_LEVEL_ENT_2,
  GL_LEVEL_DONUT_1,
  GL_LEVEL_DONUT_2,
  GL_LEVEL_UTENSIL_1,
  GL_LEVEL_UTENSIL_2,

  GL_LEVEL_GIANT_1,
  GL_LEVEL_GIANT_2,
  GL_LEVEL_DOZER_1,
  GL_LEVEL_DOZER_2,
  GL_LEVEL_SCIENTIST_1,

  GL_LEVEL_SCIENTIST_2,
  GL_LEVEL_ALIEN_1,
  GL_LEVEL_ALIEN_2,
  GL_LEVEL_WISP_1,
  GL_LEVEL_WISP_2,

  GL_LEVEL_CHIP_1,
  GL_LEVEL_CHIP_2,
  GL_LEVEL_CUPID_1,
  GL_LEVEL_CUPID_2,
  GL_LEVEL_SPINY_1,

  GL_LEVEL_SPINY_2,
  GL_LEVEL_REAPER_1,
  GL_LEVEL_REAPER_2,
  
};

enum PlaneLevelType
{
  PL_LEVEL_MOHAWK,
  PL_LEVEL_CHICKEN,
  PL_LEVEL_BOMBBAT,
  PL_LEVEL_SPIDER,
  PL_LEVEL_DONUT,

  PL_LEVEL_FLAMESKULL,
  PL_LEVEL_SPIDER_2,
  PL_LEVEL_FLAMESKULL_2,
  PL_LEVEL_CHIP,
  PL_LEVEL_DONUT_2,

  PL_LEVEL_WISP,
  PL_LEVEL_CHIP_2,
  PL_LEVEL_CUPID,
  PL_LEVEL_WISP_2,
  PL_LEVEL_CUPID_2
};

enum AngryLevelType
{
  AN_LEVEL_0,
  AN_LEVEL_1,
  AN_LEVEL_2,
  AN_LEVEL_3,
  AN_LEVEL_4,
  AN_LEVEL_5,
  AN_LEVEL_6,
  AN_LEVEL_7,
  AN_LEVEL_8,
  AN_LEVEL_9,
  AN_LEVEL_TEST
};

enum PuppyLevelType
{
  PU_LEVEL_0,
  PU_LEVEL_1,
  PU_LEVEL_2,
  PU_LEVEL_3,
  PU_LEVEL_4,
  PU_LEVEL_5,
  PU_LEVEL_6,
  PU_LEVEL_7,
  PU_LEVEL_8,
  PU_LEVEL_9
};

enum PirateLevelType
{
  PI_LEVEL_0,
  PI_LEVEL_1,
  PI_LEVEL_2,
  PI_LEVEL_3,
  PI_LEVEL_4,
  PI_LEVEL_5,
  PI_LEVEL_6,
  PI_LEVEL_7,
  PI_LEVEL_8,
  PI_LEVEL_9,
  PI_LEVEL_TEST
};

enum FiremanLevelType
{
  FI_LEVEL_0,
  FI_LEVEL_1,
  FI_LEVEL_2,
  FI_LEVEL_3,
  FI_LEVEL_4,
  FI_LEVEL_5,
  FI_LEVEL_6,
  FI_LEVEL_7,
  FI_LEVEL_8,
  FI_LEVEL_9,
  FI_LEVEL_TEST
};

enum SprintLevelType
{
  SP_LEVEL_0,
  SP_LEVEL_1,
  SP_LEVEL_2,
  SP_LEVEL_3,
  SP_LEVEL_4,
  SP_LEVEL_5,
  SP_LEVEL_6,
  SP_LEVEL_7,
  SP_LEVEL_8,
  SP_LEVEL_9
};

enum SumoLevelType
{
  SUMO_LEVEL_GRAVEYARD,
  SUMO_LEVEL_JURASSIC,
  SUMO_LEVEL_CUPCAKE,
  SUMO_LEVEL_DUMP,
  SUMO_LEVEL_EDISON,
  
  SUMO_LEVEL_FUNGUS,
  SUMO_LEVEL_EMPEROR,
  SUMO_LEVEL_SNOWBALL
};

enum LakeLevelType
{
  LAKE_LEVEL_EASY,
  LAKE_LEVEL_LOBSTER,
  LAKE_LEVEL_LOBSTER_2,
  LAKE_LEVEL_PEARL,
  LAKE_LEVEL_JELLYFISH,
  
  LAKE_LEVEL_JELLYFISH_2,
  LAKE_LEVEL_BARREL,
  LAKE_LEVEL_BARREL_2,
  LAKE_LEVEL_URCHIN,
  LAKE_LEVEL_SPINECLAM,
  
  LAKE_LEVEL_SPINECLAM_2,
  LAKE_LEVEL_SWORDFISH,
  LAKE_LEVEL_STINGRAY,
  LAKE_LEVEL_SHARK,
  LAKE_LEVEL_SHARK_2,
  
  LAKE_LEVEL_TEST
};

enum ToadhouseLevelType
{
  PARLOR_LEVEL_GRAVEYARD_1,
  PARLOR_LEVEL_GRAVEYARD_2,
  PARLOR_LEVEL_GRAVEYARD_3,
  PARLOR_LEVEL_GRAVEYARD_4,

  PARLOR_LEVEL_JURASSIC_1,
  PARLOR_LEVEL_JURASSIC_2,
  PARLOR_LEVEL_JURASSIC_3,
  PARLOR_LEVEL_JURASSIC_4,

  PARLOR_LEVEL_BAKERY_1,
  PARLOR_LEVEL_BAKERY_2,
  PARLOR_LEVEL_BAKERY_3,
  PARLOR_LEVEL_BAKERY_4,

  PARLOR_LEVEL_DUMP_1,
  PARLOR_LEVEL_DUMP_2,
  PARLOR_LEVEL_DUMP_3,
  PARLOR_LEVEL_DUMP_4,

  PARLOR_LEVEL_EDISON_1,
  PARLOR_LEVEL_EDISON_2,
  PARLOR_LEVEL_EDISON_3,
  PARLOR_LEVEL_EDISON_4,

  PARLOR_LEVEL_BODY_1,
  PARLOR_LEVEL_BODY_2,
  PARLOR_LEVEL_BODY_3,
  PARLOR_LEVEL_BODY_4,

  PARLOR_LEVEL_SKULL_1,
  PARLOR_LEVEL_SKULL_2,
  PARLOR_LEVEL_SKULL_3,
  PARLOR_LEVEL_SKULL_4
};

enum GateLevelType
{
  GATE_LEVEL_GRAVEYARD_1,
  GATE_LEVEL_GRAVEYARD_2,
  GATE_LEVEL_GRAVEYARD_3,
  GATE_LEVEL_GRAVEYARD_4,
  GATE_LEVEL_GRAVEYARD_5,

  GATE_LEVEL_JURASSIC_1,
  GATE_LEVEL_JURASSIC_2,
  GATE_LEVEL_JURASSIC_3,
  GATE_LEVEL_JURASSIC_4,
  GATE_LEVEL_JURASSIC_5,

  GATE_LEVEL_BAKERY_1,
  GATE_LEVEL_BAKERY_2,
  GATE_LEVEL_BAKERY_3,
  GATE_LEVEL_BAKERY_4,
  GATE_LEVEL_BAKERY_5,

  GATE_LEVEL_DUMP_1,
  GATE_LEVEL_DUMP_2,
  GATE_LEVEL_DUMP_3,
  GATE_LEVEL_DUMP_4,
  GATE_LEVEL_DUMP_5,
  GATE_LEVEL_DUMP_6,

  GATE_LEVEL_EDISON_1,
  GATE_LEVEL_EDISON_2,
  GATE_LEVEL_EDISON_3,
  GATE_LEVEL_EDISON_4,
  GATE_LEVEL_EDISON_5,
  GATE_LEVEL_EDISON_6,
  GATE_LEVEL_EDISON_7,
  GATE_LEVEL_EDISON_8,

  GATE_LEVEL_BODY_1,
  GATE_LEVEL_BODY_2,
  GATE_LEVEL_BODY_3,
  GATE_LEVEL_BODY_4,
  GATE_LEVEL_BODY_5,
  GATE_LEVEL_BODY_6,
  GATE_LEVEL_BODY_7,

  GATE_LEVEL_SKULL_1,
  GATE_LEVEL_SKULL_2,
  GATE_LEVEL_SKULL_3,
  GATE_LEVEL_SKULL_4,
  GATE_LEVEL_SKULL_5,
  GATE_LEVEL_SKULL_6,
  GATE_LEVEL_SKULL_7,
  GATE_LEVEL_SKULL_8
};

enum ChunkBuildType
{
  CHUNK_VERSION_GENERIC,
  CHUNK_VERSION_ANGRY,
  CHUNK_VERSION_PIRATE,
  CHUNK_VERSION_CAGED_PUPPY,
  CHUNK_VERSION_BALLOON_BASKET,
  CHUNK_VERSION_BLOCK,
  CHUNK_VERSION_COIN,
  CHUNK_VERSION_ENEMY,
  
  CHUNK_VERSION_BURNING_BLOCKS
};

enum IngredientLocation
{
  LOCATION_BASE,
  LOCATION_EYES,
  LOCATION_NOSE,
  LOCATION_MOUTH,
  LOCATION_ACCESSORY,
  LOCATION_TOPPING,
  LOCATION_SEASONING,
  
  NUM_INGREDIENT_LOCATIONS
};

// do not change the order after version 1.0, just add to the end
enum IngredientType
{
  ING_NONE,
  
  ING_BASE_CHEESE,
  ING_BASE_PESTO,
  ING_BASE_WOOD,
  ING_BASE_MEATBALL,
  ING_BASE_UFO,
  ING_BASE_ZEBRA,
  ING_BASE_SNOWCONE,
  ING_BASE_SOCCER,
  
  ING_TOPPING_PEPPERONI,
  ING_TOPPING_MUSHROOM,
  ING_TOPPING_PEPPERS,
  ING_TOPPING_OLIVES,
  ING_TOPPING_PINEAPPLE,
  ING_TOPPING_SAUSAGE,
  ING_TOPPING_BACON,
  ING_TOPPING_ANCHOVIES,
  ING_TOPPING_BUGS,
  ING_TOPPING_GUMMY_WORMS,
  ING_TOPPING_PAPER_CLIPS,
  ING_TOPPING_XMAS_LIGHTS,
  
  ING_EYES_REGULAR,
  ING_EYES_GOOGLEY,
  ING_EYES_COAL,
  ING_EYES_HUMAN,
  ING_EYES_DEVIL,
  ING_EYES_DOG,
  ING_EYES_CAT,
  ING_EYES_CYCLOPS,
  
  ING_MOUTH_REGULAR,
  ING_MOUTH_VAMPIRE,
  ING_MOUTH_HUMAN,
  ING_MOUTH_DOG,
  ING_MOUTH_CAT,
  ING_MOUTH_DEVIL,
  
  ING_NOSE_DOG,
  ING_NOSE_CAT,
  ING_NOSE_HUMAN,
  ING_NOSE_RHINO,
  ING_NOSE_CARROT,
  ING_NOSE_ELEPHANT,
  
  ING_ACC_BEANIE,
  ING_ACC_PROPELLOR,
  ING_ACC_WOMENS_MARCH,
  ING_ACC_DOG_EARS,
  ING_ACC_CAT_EARS,
  ING_ACC_TOUPEE,
  ING_ACC_WEDDING_DRESS,
  ING_ACC_DREADLOCKS,
  ING_ACC_TRIHAWK,

  SEAS_HP_0,
  SEAS_HP_1,
  SEAS_HP_2,
  SEAS_FLOATY_0,
  SEAS_FLOATY_1,
  SEAS_FLOATY_2,
  SEAS_MOVE_SPEED_0,
  SEAS_MOVE_SPEED_1,
  SEAS_MOVE_SPEED_2,
  SEAS_ACCEL_0,
  SEAS_ACCEL_1,
  SEAS_ACCEL_2,
  SEAS_INVINCIBILITY_0,
  SEAS_INVINCIBILITY_1,
  SEAS_INVINCIBILITY_2,
  SEAS_DOUBLE_JUMP_0,
  SEAS_DOUBLE_JUMP_1,
  SEAS_DOUBLE_JUMP_2,
  SEAS_JUMP_HEIGHT_0,
  SEAS_JUMP_HEIGHT_1,
  SEAS_JUMP_HEIGHT_2,
  SEAS_STOMPER_0,
  SEAS_STOMPER_1,
  SEAS_STOMPER_2,
  SEAS_HEAL_0,
  SEAS_HEAL_1,
  SEAS_HEAL_2,
  SEAS_SHIELD_0,
  SEAS_SHIELD_1,
  SEAS_SHIELD_2,
  SEAS_BANKER_0,
  SEAS_BANKER_1,
  SEAS_BANKER_2,
  SEAS_DRAGON_0,
  SEAS_DRAGON_1,
  SEAS_DRAGON_2,
  SEAS_EMPEROR_0,
  SEAS_EMPEROR_1,
  SEAS_EMPEROR_2,

  NUM_INGREDIENTS
};

enum LevelState
{
  LEVEL_PLAY,
  LEVEL_PASSIVE,
  LEVEL_LOSE,
  LEVEL_WIN
};

enum ChunkType
{
  CHUNK_SKI_START,
  CHUNK_SKI_4_1,
  CHUNK_SKI_3_1,
  
  CHUNK_PUPPY_DOGHOUSE,
  CHUNK_PUPPY_BIGHILL,
  CHUNK_PUPPY_BIGHILL_DOWN,
  CHUNK_BOUNCER_A,

  NUM_CHUNK_TYPES
};

enum BackgroundType
{
  // world backgrounds
  GRAVEYARD_BG,
  JURASSIC_BG,
  CAKE_BG,
  DUMP_BG,
  SILICON_BG,
  BODY_BG,
  CATACOMBS_BG,
  
  // special mode backgrounds
  PUPPY_BG,
  FIREMAN_BG,
  LAKE_BG,
  REDROCK_BG,
  PIZZERIA_BG,
};

enum MapObjectLayer
{
  MAP_PATHING_LAYER,
  MAP_OBJECT_LAYER,
  MAP_DYNAMIC_LEVELS_LAYER,
  MAP_LEVELNUMS_LAYER,
  MAP_UNLOCKS_LAYER,
  MAP_REGION_LAYER
};

enum VisitSquareType
{
  VISITABLE_NULL = -1,
  VISITABLE_LEVEL,
  VISITABLE_PARLOR,
  VISITABLE_GATE,
  VISITABLE_SKELE
};

enum SaberTouchType
{
  SABER_TOUCHING_NOTHING,
  SABER_TOUCHING_ME,
  SABER_TOUCHING_MY_SABER
};

enum BGLayerSuggestion
{
  BG_LAYER_FAR,
  BG_LAYER_CLOSE,
  BG_LAYER_CLOSEST,
  BG_LAYER_PF,
  BG_LAYER_FG
};

enum HashDataTypes
{
  BOOL_TYPE,
  INT_TYPE,
  FLOAT_TYPE,
  STRING1_TYPE,
  STRING2_TYPE
};

// Why these should all be primitive types (int, string, array):
//
// The whole point is that there is only 1 parser, but that 1 parser has the flexibility to read
// every type of data, even as individual items are added or removed.

// Arrays could work when they are a single data type, like an array of points, as long as
// each element has the same meaning. Individual meaning = individual key.
struct RMGenericItem
{
  String1 myKey;
  Coord1 type;
  
  RMGenericItem(Coord1 setType) : myKey(), type(setType) {}
  virtual ~RMGenericItem() {}
  virtual void read(const String1& utf8Str) = 0;
  virtual void write(String1& output) = 0;
  virtual void* getData() {return NULL;}
  virtual RMGenericItem* createNew() = 0;
};

struct RMBoolItem : RMGenericItem
{
  Logical data;
  
  RMBoolItem(Logical setData) : RMGenericItem(BOOL_TYPE), data(setData) {}
  virtual RMGenericItem* createNew() {return new RMBoolItem(false);}
  virtual void read(const String1& utf8Str);
  virtual void write(String1& output);
  virtual void* getData() {return &data;}
};

struct RMIntItem : RMGenericItem
{
  Coord1 data;
  
  RMIntItem(Coord1 setData) : RMGenericItem(INT_TYPE), data(setData) {}
  virtual RMGenericItem* createNew() {return new RMIntItem(0);}
  virtual void read(const String1& utf8Str);
  virtual void write(String1& output);
  virtual void* getData() {return &data;}
};

struct RMFloatItem : RMGenericItem
{
  Point1 data;
  
  RMFloatItem(Point1 setData) : RMGenericItem(FLOAT_TYPE), data(setData) {}
  virtual RMGenericItem* createNew() {return new RMFloatItem(0.0);}
  virtual void read(const String1& utf8Str);
  virtual void write(String1& output);
  virtual void* getData() {return &data;}
};

struct RMString2Item : RMGenericItem
{
  String2 data;
  
  RMString2Item(const String2& setData) : RMGenericItem(STRING2_TYPE), data(setData) {}
  virtual RMGenericItem* createNew() {return new RMString2Item(String2());}
  virtual void read(const String1& utf8Str);
  virtual void write(String1& output);
  virtual void* getData() {return &data;}
};

struct MapComparator
{
  Logical operator()(String1* left, String1* right) const;
};

typedef std::map<String1*, RMGenericItem*, MapComparator> PizzaCppMap;

// Different map instances:
// 1. defaultPlayer. This is created on init, ALWAYS exists, has every key, and the values never change.
// 2. currPlayer. This is the current stable player. Has every key.
// 3. tempPlayer. Used to load icloud data. only has keys that are CHANGED
// 4. savePlayer. 
struct PlayerDataMap
{
  PizzaCppMap cppMap;
  String1 strHelper;  // helps for building keys
  
  PlayerDataMap();
  virtual ~PlayerDataMap();
  virtual void initDefaultPlayer();
  // void deep_copy_from(PlayerDataMap& source);  // got an error trying to make it a const reference
  
  void addBoolItem(const String1& key, Logical defaultData);
  void addIntItem(const String1& key, Coord1 defaultData);
  void addFloatItem(const String1& key, Point1 defaultData);
  void addStr2Item(const String1& key, const Char* defaultData);
  
  RMGenericItem* getItem(const Char* key);
  Logical& getBoolItem(const Char* key);
  Coord1& getIntItem(const Char* key);
  Point1& getFloatItem(const Char* key);
  String2& getString2Item(const Char* key);
  
  String1& levelStr(Coord1 levelIndex, const String1& dataKey);
  String1& ingredientStr(Coord1 ingIndex, const String1& dataKey);
  
  // convenience calls
  Point1& getLevelFloat(Coord1 levelIndex, const String1& keyPrefix);
  Logical& getLevelBool(Coord1 levelIndex, const String1& keyPrefix);
  Coord1& getLevelInt(Coord1 levelIndex, const String1& keyPrefix);
  Logical& getIngredientBool(Coord1 ingIndex, const String1& keyPrefix);

  Coord1 countTotalStars();
  Coord1& getCurrency(Coord1 type);
  
  // sub id is if you want to specify something like topping 2
  Coord1& getIngLocation(Coord1 locationID, Coord1 subID);
  
  // onlyChanged = true makes it so that no values matching the defaults get written
  void write(String1& output, Logical onlyChanged = true);
  void read(const String1& input);
  
  virtual void freeAll();
};

struct LevelData
{
  Coord1 index;
  String2 localizedName;
  
  Coord2 modeData;  // [LEVEL_TYPE, LEVEL_NUM]
  Coord1 region;
  
  Coord1 unlockRequirement;  // what this actually is depends on modeData.x
  
  // pizza parlors use this, it's the index of the ingredient defition in ingredientList,
  //  NOT the ingredient type
  DataList<Coord1> ingredientDefIndices;
  
  LevelData();
  
  Logical isLocked();  // reads from active player
  void unlockForActivePlayer();  // does NOT save
};

// this resets when you start a level
struct TempGameplayData
{
  Coord1 currLevelIndex;
  
  Point1
  currTime,
  maxPlayerHP,  // required for a max of healing
  currPlayerHP;
  
  DataList<Coord1> collectedCoins;  // includes gems
  
  // for fireman
  Coord1 objectsIgnited;
  Coord1 objectsExtinguished;
  
  TempGameplayData();
  void addHP(Point1 value);
};

struct IngredientData
{
  Coord1
  index,
  baseType,  // this is the "0-version" of the seasoning
  location,
  upgradeLevel;
  
  Coord2 price;  // [type, val]
  
  String2 localizedName;

  Point4 dataVals;
  
  IngredientData();
  
  Logical isIngredient() {return location != LOCATION_SEASONING;}
  Logical isSeasoning() {return location == LOCATION_SEASONING;}
  Logical getBaseSeasoning() {return baseType;}
  
  Logical playerUnlocked();
  Logical playerBought();
  Logical playerCanAfford();
};

struct PizzaController : ButtonPresser
{
  PointerDataStd pointer;
  AccelDataStd accelerometer;
  GamePadDataStd gamepad;
  KeyboardDataStd keyboard;

  Point1 calib;
  Point1 calibVert;
  
  Logical pressTaken;
  Logical usedDirsLast;  // if you last clicked the screen or the mouse, this is false

  PizzaController();
  void update();
  
  Point2 getXY();
  Point2 getOldXY();
  Point2 cursorToScreen(Point2 cursorXY);
  
  Logical getAnyConfirmStatus(Coord1 status);
  
  Logical getConfirmButtonStatus(Coord1 status);
  Logical getCancelButtonStatus(Coord1 status);
  Logical getDirectionStatus(Coord1 dir8, Coord1 status, Coord1 controllerID);  // includes sticks
  
  Logical getDirectionalButtonStatus(Coord1 dir8, Coord1 status);  // returns dir8 or ORIGIN
  Coord1 getDirection(Coord1 status);  // returns dir8 or ORIGIN
  void updateTypeUsedLast();

  // from ButtonPresser
  virtual Logical clickStatus(Coord1 type);
  virtual void blockClicker();
};

struct iCloudListener : SystemICloudListener
{
  iCloudListener();

  void read_icloud_finished(Logical success);  // sets readCache on success first
  void write_icloud_finished(Logical success);
};

struct IAPManager : SystemIAPListener
{
  IAPGUI* gui;
  
  IAPManager();
  
  void iap_purchased();
  void iap_failed();
};

const Point2
RM_WH(960.0, 540.0),
SAFE_WH(720.0, 443.0);

const Box
RM_BOX(Point2(0.0, 0.0), RM_WH),
SAFE_BOX(Box::from_center(RM_BOX.center(), SAFE_WH));

const Point1
SCENE_TRANS_DUR = 0.5,
GUI_TRANS_DUR = 0.25,

PLAYER_ROLL_FORCE = 7500.0,
PLAYER_ROLL_TORQUE = 330.0,
PLAYER_JUMP_VEL = -700.0,

PLAYER_CAM_TOP_PAD_STD = 128.0,

PIZZA_INNER_RADIUS = 200.0,  // this does not include the squishy crust
DEFAULT_SPOKE_LENGTH = 224.0,

ANGRY_SKULL_RAD = 36.0;

const ColorP4 GUI_HIGHLIGHT_COLOR(1.0, 1.0, 0.0, 1.0);

//const Point2 LAKE_SIZE(3072.0, 1536.0);
const Coord2 LAKE_SCREENS(6, 4);  // these are ipad sized
const Point2 LAKE_SCREEN_SIZE(960.0, 540.0);
const Point2 LAKE_WORLD_SIZE(LAKE_SCREEN_SIZE.x * LAKE_SCREENS.x,
                             LAKE_SCREEN_SIZE.y * LAKE_SCREENS.y);
const Point2 LAKE_PLAYER_START(LAKE_SCREEN_SIZE * Point2(2.0, 4.0) - RM_WH * 0.5);
const Box LAKE_WORLD_BOX(Point2(0.0, 0.0), LAKE_WORLD_SIZE);

const Coord1
NUM_PIZZA_SPOKES = 24,
TOPPINGS_PER_SELECTION = 5,
NUM_SELECTED_TOPPINGS = 3,
ITEMS_PER_SHOP = 3;

typedef Char const * const ConstCStr;

ConstCStr
PLAYER_FILENAME = "Asa24.sav";

ConstCStr
FILE_TYPE_INT_KEY = "FILE_TYPE_INT_KEY",
MUSIC_BOOL_KEY = "MUSIC_BOOL_KEY",
SFX_BOOL_KEY = "SFX_BOOL_KEY",
CURRENT_VERSION_STR2_KEY = "CURRENT_VERSION_STR2_KEY",
TRIED_RATE_VERSION_STR2_KEY = "TRIED_RATE_VERSION_STR2_KEY",
TOTAL_TIME_FLOAT_KEY = "TOTAL_TIME_FLOAT_KEY",
COINS_KEY = "COINS_KEY",
GEMS_KEY = "GEMS_KEY",

BASE_INGREDIENT_KEY = "BASE_INGREDIENT_KEY",
EYES_INGREDIENT_KEY = "EYES_INGREDIENT_KEY",
NOSE_INGREDIENT_KEY = "NOSE_INGREDIENT_KEY",
MOUTH_INGREDIENT_KEY = "MOUTH_INGREDIENT_KEY",
ACCESSORY_INGREDIENT_KEY = "ACCESSORY_INGREDIENT_KEY",
TOPPING_0_INGREDIENT_KEY = "TOPPING_0_INGREDIENT_KEY",
TOPPING_1_INGREDIENT_KEY = "TOPPING_1_INGREDIENT_KEY",
TOPPING_2_INGREDIENT_KEY = "TOPPING_2_INGREDIENT_KEY",
SEASONING_0_KEY = "SEASONING_0_KEY",
SEASONING_1_KEY = "SEASONING_1_KEY",

// hammer bros positions, if negative then they are offscreen
// these should get saved any time the hammer bros move
BROS_1X_KEY = "BROS_1X_KEY",
BROS_1Y_KEY = "BROS_1Y_KEY",
BROS_2X_KEY = "BROS_2X_KEY",
BROS_2Y_KEY = "BROS_2Y_KEY",

LAST_PLAYED_LEVEL_KEY = "LAST_PLAYED_LEVEL_KEY",

// related to levels
LEVEL_PREFIX_KEY = "L_",
L_TIME_KEY = "TIME_KEY",
L_FIRST_STAR_KEY = "FIRST_STAR_KEY",
L_SECOND_STAR_KEY = "SECOND_STAR_KEY",
L_THIRD_STAR_KEY = "THIRD_STAR_KEY",
L_TOTAL_SCORE_KEY = "TOTAL_SCORE_KEY",
L_LOCKED_KEY = "LOCKED_KEY",

// related to toppings
INGREDIENT_PREFIX_KEY = "I_",
I_UNLOCKED_KEY = "UNLOCKED_KEY",
I_OWNED_KEY = "OWNED_KEY";

extern ArrayList<LevelData> levelList;
extern ArrayList<IngredientData> ingredientList;
extern DataList<Coord1> numSelectableIngs;

extern RenderTarget* offscreenBuffer;
extern Image* skullImg;   /////////////////////

extern String2 MASTER_LSTRING;  // this is for debug, it's the entire string file
extern ArrayList<String2> LSTRINGS;  // the processed lines from the string file
extern String1 LANGUAGE_CODE;  // the string representation of the chosen language
extern LanguageID LANGUAGE_ID;  // index (enumeration) of chosen language
extern String1 langSuffix;
extern String2 EFIGS_CHARS;
extern String2 ASCII_CHARS;

namespace Pizza
{
  extern iCloudListener cloudResponder;
  extern GamesInterstitialManagerStd* moreGamesMan;
  extern IAPManager* iapMan;

  extern CameraStd platformCam;

  extern Logical
  gcLogonShown,
  DRAW_DEBUG,
  DRAW_DEBUG_PHYS,
  DEBUG_INVINCIBLE,
  DEBUG_KEYSTROKES,
  DEBUG_MAP_PATHING,
  DEBUG_IGNORE_SAVING,
  DEBUG_IGNORE_LOADING;
  
  extern Coord1 DEBUG_LEVEL;  // if -1 this is ignored
  
  void initGameConstants();  // call after global constants are initialized and version is set up
  void resetGame();
  void resetLevel();

  void loadPlayerData();
  void savePlayerData();

  void resetAndSavePlayer();
  
  void gotAchievement(Coord1 val);

  void processStrings();
  
  void setLevelData();
  void addLevelData(Coord1 modeType, Coord1 modeLevel);
  void addParlorData(Coord1 parlorID, Coord1 ing1, Coord1 ing2, Coord1 ing3);

  void setIngredientData();
  void addIngredientData(Coord1 setType, Coord1 setLocation, Coord1 strIndex);
  void addSeasoningData(Coord1 setType, Coord1 setBaseType, Coord1 setUpgradeLevel, Coord1 strIndex);
  
  // pass in the BASE ingredient (at level 0)
  // note: 0 means nothing equipped, otherwise it will be 1-3
  // if the same ingredient type is equipped twice, it will use the higher magnitude
  Coord1 getEquippedSeasoningLevel(Coord1 baseIngredientID);
}

Box deviceScreenBox();
Box deviceSafeBox();

#endif
