/*
 *  graphicsman.h
 *  RMProject
 *
 *  Created by Paul Stevens on 9/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RESOURCEMAN_H
#define RESOURCEMAN_H

#include "rmarraylist.h"
#include "rmimage.h"
#include "rmsound.h"
#include "rmdmorph.h"

struct RMFont;
struct FlipbookAnimation;
struct spSkeleton;
struct spAnimationStateData;
struct spAnimationState;
struct spSkeletonData;
struct spAtlas;

enum
{
  RES_IMAGE,
  RES_IMAGE_SET,
  RES_DWARP,
  RES_SOUND,
  RES_FONT,
  RES_SPINE,

  NUM_RES_TYPES
};

enum ResourceID
{
  // GUI common
  selector3x3Normal,
  selector3x3Fill,
  selector3x3Add,
  
  // later we can add the functionality for 1 set to represent multiple loads
  bgLayer0,
  bgLayer1,
  bgLayer2,
  bgLayer3,
  bgLayer4,
  bgLayer5,
  bgLayer6,
  bgLayer7,
  bgLayer8,
  bgLayer9,
  bgLayer10,
  bgObject0,
  bgObject1,
  bgObject2,
  bgObject3,
  bgObject4,
  bgObject5,
  bgObject6,
  bgObject7,
  bgObject8,
  bgObject9,
  bgObject10,
  bgObject11,
  bgObject12,
  bgObject13,
  bgObject14,
  bgObject15,
  terrainBordersImg,
  terrainFillImg,

  futura130Font,
  bgImg,
  paddleImg,
  arrowImg,
  
  // cinema
  introPalaceMedSpine,
  boxThumpSFX,
  carAwaySFX,
  carDecelSFX,
  tireSkidSFX,

  // map
  levelNodesSet,
  mapPizzaImg,
  mapBackgroundSet,
  mapObjectSet,
  mapTreeSpine,
  mapFlowersSpine,
  mapDeadTreeSpine,
  mapPalmTreeSpine,
  mapFernsSpine,
  mapVolcanoSpine,
  mapCandleSpine,
  mapBananaSpine,
  mapLEDSpine,
  mapServerSpine,
  mapPolypsSpine,
  mapPalaceSpine,
  mapBridgeHSpine,
  mapBridgeVSpine,
  mapWave1Spine,
  mapWave2Spine,
  mapShopSpine,
  mapHammerBrosSpine,
  
  dmgImgTri,
  dmgImgQuad,
  
  genericShadowImg,
  
  // these are general use images in sumo mode
  sumoBossImg,  // this is probably just temp, usually a spine
  sumoBoss2Img,
  sumoBoss3Img,
  sumoBoss4Img,
  sumoBoss5Img,
  sumoBoss6Img,
  sumoBoss7Img,
  sumoBoss8Img,
  breakableObjA,
  breakableObjB,
  breakableObjC,
  breakableObjD,
  breakableObjE,
  breakableObjF,

  /*JACRUM algorithmic block*/
  yaryortImg,
  
  redrockCenterImg,
  redrockBorderImg,
  redrockTerrainBordersImg,
  spikebarImg,
  brickCenterImg,
  brick9TopImg,
  glassCenterImg,
  glass9TopImg,
  glassTriXImg,
  stoneCenterImg,
  stone9TopImg,
  woodCenterImg,
  wood9TopImg,
  steelCenterImg,
  steel9TopImg,
  
  breakableA9TopImg,
  breakableACenterImg,
  breakableB9TopImg,
  breakableBCenterImg,
  mover9TopImg,
  moverCenterImg,
  
  blockWoodFillImg,
  blockWoodBorderImg,
  blockSandstoneFillImg,
  blockSandstoneBorderImg,
  blockSandstoneTrixImg,
  blockSandstoneCrackedFillImg,
  blockSandstoneCrackedBorderImg,
  blockSandstoneCrackedTrixImg,
  blockSandstoneSmallBorderImg,
  blockSandstoneSmallTrixImg,
  blockSandstoneSmallCrackedBorderImg,
  blockSandstoneSmallCrackedTrixImg,
  blockSteelFillImg,
  blockSteelBorderImg,
  blockSteelTrixImg,
  blockWorldFillImg,
  blockWorldBorderImg,
  blockWorldTrixImg,
  blockSpikesBorderImg,
  algoSpikesImg,
  /*END JACRUM*/
  
  coinPickupSet,
  crackImg,
 
  dmgMultiTexSet,
  
  // halfSpikerImg,
  halfSpikeRight,    // the RDLU order here is on purpose
  halfSpikeDown,
  halfSpikeLeft,
  halfSpikeUp,
  
  spikeball64Img,
  spikeball128Img,
  bouncerImg,
  
  waterSimBodyImg,
  waterSimHighlightImg,

  // == particles
  hitBaseGlowImg,
  hitSmokeImg,
  hitSparkImg,
  hitSplashSet,
  // == end particles

  tempLightningImg,  // for emperor seasoning

  moregamesAnimSet,
  moregamesButtonSet,

  pizzaBaseImg,
  pizzaToppings0Img,
  pizzaToppings1Img,
  pizzaToppings2Img,
  pizzaEyesSpine,
  pizzaMouthSpine,
  pizzaNoseSpine,
  pizzaAccSpine,
  
  // gladiator enemies
  skullTestImg,
  gilaSpine,
  unarmedSkeletonSpine,
  horiSpearmanSpine,
  vertSpearmanSpine,
  spineySpine,
  donutSpine,
  donutHoleImg,
  entBodySpine,
  entRootSpine,
  reaperSpine,
  cupidSpine,
  cupidArrowImg,
  giantSpine,
  giantHeadImg,
  wispSpine,
  wispFireSpine,
  utensilSpine,
  chipSpine,
  jumpingSpiderSpine,
  scientistSpine,
  scientistVialImg,
  scientistAcidSpine,
  dozerSpine,
  cavemanSpine,
  alienSpine,
  alienLaserSet,
  batSpine,
  batExplosionImg,
  hammerBrosSpine,
  hammerImg,
  dynamiteImg,
  speedSquareImg,
  
  // misc chunk images
  doghouseSet,
  fblockSpine,
  // gourdoLanternImg,
  gourdoLanternSpine,
  spikeballImg,
  spikeballBigImg,
  spinnerImg,
  spinnerBoltImg,
  moverPlatform192Img,
  moverPlatform384Img,
  disintigrateBlockSpine,
  
  // puppy images
  chickenSpine,
  flameskullSpine,
  mohawkSpine,
  bombSpine,
  puppiesSpine,
  pupCageSet,
  
  // angry images
  angrySkullA,
  angrySkullB,
  angrySkullC,
  angrySkullFunA,
  angrySkullFunB,
  angrySkullFunC,

  // fireman images
  balloonBasketSpine,
  balloonSpine,
  splashImg,
  firemanFlameSet,
  
  // this is assumed to be the first burning block
  firemanTeddySet,

  // this is assumed to be the second burning block and each size repeates once
  firemanPortaSet,
  firemanDavidSet,
  
  firemanBooksSet,
  firemanCrownSet,
  
  firemanHornSet,
  firemanNuclearSet,

  firemanVanSet,
  firemanWaffleSet,
  
  firemanMonaLisaSet,
  firemanComputerSet,
  
  firemanStonehengeSet,
  firemanMotorcycleSet,
  
  firemanBurgerSet,
  firemanSynthSet,
  
  // this is assumed to be the second last block size
  firemanCartSet,
  firemanBarrowSet,
  
  // pirate images
  // pirateShipImg,
  pirateShipSpine,
  // pirateGemSet,
  pirateChestSpine,
  gemBluePickupSet,
  gemGreenPickupSet,
  gemRedPickupSet,
  gemYellowPickupSet,
  
  // sprint images
  sprintFlagSpine,
  
  // plane images
  planePlayerImg,
  planePlayerSet,
  planePlayerSpine,
  planeMohawkImg,
  planeBulletImg,
  
  // lake images
  fish16Spine,
  fish32Spine,
  fish64Spine,
  fish128Spine,
  fish256Spine,
  fish512Spine,
  planktonSpine,
  seahorseSpine,
  shark128Spine,
  shark256Spine,
  clamSpine,
  barrelSpine,
  jellyLargeSpine,
  jellyMedSpine,
  jellySmallSpine,
  lobsterSpine,
  stingraySpine,
  urchinSpine,
  
  // shop images
  museoFont,
  shackletonFont,  // also used in cinema
  shopPizzaIconImg,
  shopPricePaperSet,
  shopButtonSpine,
  shopButtonImgs,
  shopCoinImg,
  shopGemImg,
  shopFloorImg,
  
  // equip images
  equipPeelImg,
  equipMenuBaseImg,
  equipMenuTabsSet,
  equipItemImg,
  equipMoreArrowImg,
  equipSelectSquaresSet,
  equipSelectNumbersSet,
  equipDoneCheckImg,

  musicSnd,
  cowbellSnd,

  NUM_TOTAL_RES
};

struct ResourceCacheItem
{
  Coord1
  resType,   // ResourceType
  resIndex;

  String1 filename;

  Logical
  isLoaded,  // true will skip it over for loadResource()
  willDelete,  // false will skip it over for unloadResource()
  assetCats,
  onDemand;

  ResourceCacheItem();
  virtual ~ResourceCacheItem() {}

  virtual void loadResource() = 0;
  virtual void unloadResource() = 0;
  virtual void* getResource() = 0;
};

struct ImageCacheItem : ResourceCacheItem
{
  Image* img;

  Point2 loadHandle;
  Logical loadOverexpose;

  ImageCacheItem();

  void loadResource();
  void* getResource() {return img;}
  void unloadResource() {delete img;}
};

/*
struct TileLoadDef
{
  String1 filename;
  Point2 loadHandle;
  Logical overexpose;
  
  TileLoadDef();
};
 */

struct ImageTilesCacheItem : ResourceCacheItem
{
  DataList<Image*> set;

  Coord1
  rows,
  cols;

  Point2 loadHandle;
  Logical loadOverexpose;

  ImageTilesCacheItem();
  void loadResource();
  void unloadResource();
  void* getResource() {return &set;}
};

struct ImageCollectionCacheItem : ResourceCacheItem
{
  DataList<Image*> set;

  Logical ownsImages;

  ImageCollectionCacheItem();
  void loadResource() {}
  void unloadResource();
  void* getResource() {return &set;}
};

struct DWarpCacheItem : ResourceCacheItem
{
  DWarpAnimation* warp;
  Coord1 imgIndex;

  DWarpCacheItem();

  void loadResource();
  void* getResource() {return warp;}
  void unloadResource() {delete warp;}
};

struct SoundCacheItem : ResourceCacheItem
{
  Sound* sound;

  SoundCacheItem();

  void loadResource();
  void* getResource() {return sound;}
  void unloadResource() {delete sound;}
};

struct FontCacheItem : ResourceCacheItem
{
  RMFont* font;
  String2 charSet;
  Point2 charSizeAdjust;

  FontCacheItem();

  void loadResource();
  void* getResource();
  void unloadResource();
};

struct SpineAnimCacheItem : ResourceCacheItem
{
  spAtlas* atlasData;
  spSkeletonData* skeletonData;
  spAnimationStateData* animStateData;

  // this is to store the language and size extension that the .skel file doesn't need
  String1 fullImgStr;
  
  Point1 animScale;

  // the "filename" just holds the root, all extensions are added
  SpineAnimCacheItem();

  // overrides
  void loadResource();
  void* getResource();
  void unloadResource();
};


struct ResourceCache
{
  DataList<ResourceCacheItem*> itemList;
  DataList<ResourceCacheItem*> loadQueue;

  Logical
  newItemsWillDelete;  // sets willDelete for new ResourceCacheItem

  Coord1
  currLoadIndex,
  numLoaded,
  numToLoad;

  explicit ResourceCache(Coord1 numResourceIDs);
  void setItemDef(Coord1 itemIndex, Coord1 itemType, const String1& filename);

  void prepLoad();
  void remotesReady();
  void loadStride(Coord1 numberOfLoads);
  void loadAllNow();

  void unload();
  void unloadItem(Coord1 index);
  Point1 percentLoaded();
  Logical isDoneLoading();
};

extern ResourceCache resCache;

namespace ResourceManager
{
  extern String1
  workingStr,
  imgExtSuffix,
  imgSizeSuffix;

  extern Point1 spineAnimScale;

  extern Bitmap* hobbitBG;

  Char const * const PNG_EXT = ".png";
  Char const * const CI4_EXT = ".ci4";

  void loadCommon();

  void loadTitle();
  void initTitle();
  void unloadTitle();

  void loadTestCinema();
  void initTestCinema();
  void unloadTestCinema();

  void loadPlay();
  void initPlay();
  void unloadPlay();

  void loadMap();
  void initMap();
  void unloadMap();
  
  void loadPizzaPlayer();
  void initPizzaPlayer();
  void unloadPizzaPlayer();

  String1& imgStr(const Char* imgBase);  // uses default image extension (.ci4)
  String1& pngStr(const Char* imgBase);  // uses .png image extension
  String1& warpStr(const Char* warpBase);

  void loadImage(Coord1 imgIndex, const Char* imgBase,
                 Point2 smartHandle = Point2(0.0, 0.0), Logical overexpose = false);

  void loadTiles(Coord1 setIndex, const Char* imgBase,
                 Coord1 cols, Coord1 rows, Point2 smartHandle = Point2(0.0, 0.0),
                 Logical overexpose = false);
  
  /*
  void loadSingleTile(Coord1 setIndex, const Char* imgBase,
                      Point2 smartHandle = Point2(0.0, 0.0),
                      Logical overexpose = false);

  ImageCollectionCacheItem* createDerivImgSet(Coord1 setIndex, Logical ownsImages);
   */

  void loadTilesMorph(Coord1 setIndex, const Char* imgBase, Coord1 cols, Coord1 rows,
                      Coord1 morphIndex);

  void loadWarp(Coord1 warpIndex, const Char* warpFilename, Coord1 tilesIndex,
                Logical loops = false);

  void loadSound(Coord1 soundIndex, const Char* filename);

  void loadFont(Coord1 fontIndex, const Char* filename, const String2& fontChars,
                Point1 widthAdjust = 0.0, Point1 heightAdjust = 0.0);

  void loadSpine(Coord1 spineIndex, const Char* baseFilename);

  // this just loops through the list to find it
  Sound* getSoundByFilename(const String1& fullFilename);
  
  void unloadCaches();
}

Image* getImg(Coord1 resID);
DataList<Image*>& getImgSet(Coord1 resID);
DWarpAnimation* getWarp(Coord1 resID);
Sound* getSound(Coord1 resID);
RMFont* getFont(Coord1 resID);
spAtlas* getSpineAtlas(Coord1 resID);
spSkeletonData* getSpineSkeleton(Coord1 resID);
spAnimationStateData* getSpineAnim(Coord1 resID);
const String2& getStr(Coord1 strID);

#endif
