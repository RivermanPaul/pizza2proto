/*
 *  graphicsman.cpp
 *  RMProject
 *
 *  Created by Paul Stevens on 9/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "resourceman.h"
#include "rmglobals.h"
#include "mainapp.h"
#include "rmfont.h"
#include "rmspine.h"

// ========================= ResourceCacheItem =========================== //

ResourceCacheItem::ResourceCacheItem() :
  resType(-1),
  resIndex(-1),

  filename(),

  isLoaded(false),
  willDelete(true),
  assetCats(RMSystem->usingAssetCats),
  onDemand(false)
{

}

// ========================= ImageCacheItem =========================== //

ImageCacheItem::ImageCacheItem() :
  ResourceCacheItem(),

  img(NULL),

  loadHandle(0.0, 0.0),
  loadOverexpose(false)
{

}

void ImageCacheItem::loadResource()
{
  RMSystem->usingAssetCats = assetCats;

  Logical handleIsNorm = std::abs(loadHandle.x) <= 1.01 && std::abs(loadHandle.y) <= 1.01;
  
  if (handleIsNorm == true)
  {
    img = Image::create_handled_norm(filename, loadHandle, loadOverexpose);
  }
  else
  {
    img = Image::create_handled_abs(filename, loadHandle, loadOverexpose);
  }
}

// ========================= ImageListCacheItem =========================== //

ImageTilesCacheItem::ImageTilesCacheItem() :
  ResourceCacheItem(),

  set(),

  rows(0),
  cols(0),

  loadHandle(0.0, 0.0),
  loadOverexpose(false)
{

}

void ImageTilesCacheItem::loadResource()
{
  RMSystem->usingAssetCats = assetCats;

  Logical handleIsNorm = std::abs(loadHandle.x) <= 1.01 && std::abs(loadHandle.y) <= 1.01;

  if (handleIsNorm == true)
  {
    Image::create_tiles_norm(filename, set, cols, rows, loadHandle, loadOverexpose);
  }
  else
  {
    Image::create_tiles_abs(filename, set, cols, rows, loadHandle, loadOverexpose);
  }
}

void ImageTilesCacheItem::unloadResource()
{
  free_and_null(set);
}

// ========================= ImageCollectionCacheItem =========================== //

ImageCollectionCacheItem::ImageCollectionCacheItem() :
  ResourceCacheItem(),

  set(),

  ownsImages(false)
{

}

void ImageCollectionCacheItem::unloadResource()
{
  if (ownsImages == true)
  {
    free_and_null(set);
  }
}

// ========================= DWarpCacheItem =========================== //


DWarpCacheItem::DWarpCacheItem() :
ResourceCacheItem(),

warp(NULL),
imgIndex(-1)
{

}

void DWarpCacheItem::loadResource()
{
  warp = new DWarpAnimation(filename.as_cstr(), getImg(imgIndex));
}

// ========================= SoundCacheItem =========================== //

SoundCacheItem::SoundCacheItem() :
ResourceCacheItem(),

sound(NULL)
{

}

void SoundCacheItem::loadResource()
{
  sound = Sound::create(filename);
}


// ========================= FontCacheItem =========================== //

FontCacheItem::FontCacheItem() :
  ResourceCacheItem(),

  font(NULL),
  charSet(),
  charSizeAdjust(0.0, 0.0)
{

}

void FontCacheItem::loadResource()
{
  RMSystem->usingAssetCats = assetCats;

  font = new RMFont(filename, charSet);
  font->widthAdjust = charSizeAdjust.x;
  font->lineHeight = font->char_height() - charSizeAdjust.y;
}

void* FontCacheItem::getResource()
{
  return font;
}

void FontCacheItem::unloadResource()
{
  delete font;
}

// ========================= FontCacheItem =========================== //

SpineAnimCacheItem::SpineAnimCacheItem() :
  ResourceCacheItem(),

  atlasData(NULL),
  skeletonData(NULL),
  animStateData(NULL),

  fullImgStr(),

  animScale(1.0)
{

}

void SpineAnimCacheItem::loadResource()
{
  String1 fullName = fullImgStr;
  fullName.as_substring(fullImgStr, 0, fullImgStr.index_of(".") - 1);
  fullName += ".atlas";
  
  RMSystem->usingAssetCats = assetCats;
  atlasData = spAtlas_createFromFile(fullName.as_cstr(), NULL);
  
  if (atlasData == NULL)
  {
    cout << "SpineAnimCacheItem::loadResource(): Failed to load atlas " << fullName << endl;
    rmassert(false);
  }
  else
  {
     Coord1 numPages = 0;
     spAtlasPage* currPage = atlasData->pages;

     while (currPage != NULL)
     {
       numPages++;
       currPage = currPage->next;
     }

     Coord1 numRegions = 0;
     spAtlasRegion* currRegion = atlasData->regions;

     while (currRegion != NULL)
     {
       numRegions++;
       currRegion = currRegion->next;
     }

     // cout << "loaded atlas with " << numPages << " numRregions " << numRegions << endl;
  }

  /*
  spSkeletonJson* binary = spSkeletonJson_create(atlas);
  binary->scale = animScale;
  
  if (binary == NULL)
  {
    printf("SpineAnimCacheItem::loadResource(): spSkeletonBinary NULL, error: %s\n", binary->error);
  }
  
  fullName = filename;
  fullName += ".json";
  // cout << "reading " << fullName << endl;
  skeletonData = spSkeletonJson_readSkeletonDataFile(binary, fullName.as_cstr());
  
  if (skeletonData == NULL)
  {
    printf("Failed to load skeleton data %s", fullName.as_cstr());
    spAtlas_dispose(atlas);
    rmassert(false);
  }
  else
  {
    // success, do nothing
  }
  
  spSkeletonJson_dispose(binary);
  
  animStateData = spAnimationStateData_create(skeletonData);
  
  if (animStateData == NULL)
  {
    rmassert(false);
  }
   */
  

  spSkeletonBinary* binary = spSkeletonBinary_create(atlasData);
  binary->scale = animScale;
  
  if (binary == NULL)
  {
    printf("SpineAnimCacheItem::loadResource(): spSkeletonBinary NULL, error: %s\n", binary->error);
  }
  
  fullName = filename;
  fullName += ".skel";
  skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, fullName.as_cstr());
  
  if (skeletonData == NULL)
  {
    printf("Failed to load skeleton data %s\n", fullName.as_cstr());
    spAtlas_dispose(atlasData);
    rmassert(false);
  }
  else
  {
    // success, do nothing
    // cout << "loaded Spine " << fullName << endl;
  }
  
  spSkeletonBinary_dispose(binary);
  
  animStateData = spAnimationStateData_create(skeletonData);
  
  if (animStateData == NULL)
  {
    rmassert(false);
  }
}

void* SpineAnimCacheItem::getResource()
{
  return animStateData;
}

void SpineAnimCacheItem::unloadResource()
{
  spAtlas_dispose(atlasData);
  spSkeletonData_dispose(skeletonData);
  spAnimationStateData_dispose(animStateData);
}

// ========================= ResourceCache =========================== //

ResourceCache::ResourceCache(Coord1 numResourceIDs) :
  itemList(numResourceIDs, true),
  loadQueue(),

  newItemsWillDelete(true),

  currLoadIndex(0),
  numLoaded(0),
  numToLoad(0)
{
  itemList.set_all(NULL);
}

void ResourceCache::setItemDef(Coord1 itemIndex, Coord1 itemType, const String1& filename)
{
  itemList[itemIndex]->resIndex = itemIndex;
  itemList[itemIndex]->resType = itemType;
  itemList[itemIndex]->filename = filename;
  itemList[itemIndex]->willDelete = newItemsWillDelete;
}

//void ResourceCache::prepLoad()
//{
//  currLoadIndex = 0;
//  numLoaded = 0;
//
//  for (Coord1 i = 0; i < itemList.count; ++i)
//  {
//    ResourceCacheItem* item = itemList[i];
//
//    if (item == NULL || item->isLoaded == true)
//    {
//      continue;
//    }
//
//    numToLoad++;
//  }
//}
//
//void ResourceCache::loadStride(Coord1 numberOfLoads)
//{
//  Coord1 loadsLeft = numberOfLoads;
//
//  for ( /*intentionally empty*/ ; currLoadIndex < itemList.count; ++currLoadIndex)
//  {
//    ResourceCacheItem* item = itemList[currLoadIndex];
//
//    if (item == NULL || item->isLoaded == true)
//    {
//      continue;
//    }
//
//    item->loadResource();
//    item->isLoaded = true;
//
//    loadsLeft--;
//    numLoaded++;
//
//    if (loadsLeft <= 0 || numLoaded >= numToLoad)
//    {
//      break;
//    }
//  }
//}

void ResourceCache::prepLoad()
{
  loadQueue.clear();
  currLoadIndex = 0;
  numLoaded = 0;

  for (Coord1 i = 0; i < itemList.count; ++i)
  {
    ResourceCacheItem* item = itemList[i];

    if (item == NULL || item->isLoaded == true || item->onDemand == true)
    {
      continue;
    }

    loadQueue.add(item);

    numToLoad++;
  }
}

void ResourceCache::remotesReady()
{
  for (Coord1 i = 0; i < itemList.count; ++i)
  {
    ResourceCacheItem* item = itemList[i];

    if (item == NULL || item->isLoaded == true || item->onDemand == false)
    {
      continue;
    }

    loadQueue.add(item);
  }
}

void ResourceCache::loadStride(Coord1 numberOfLoads)
{
  Coord1 loadsLeft = numberOfLoads;

  for ( /*intentionally empty*/ ; currLoadIndex < loadQueue.count; ++currLoadIndex)
  {
    ResourceCacheItem* item = loadQueue[currLoadIndex];

    if (item == NULL || item->isLoaded == true)
    {
      continue;
    }

    item->loadResource();
    item->isLoaded = true;

    loadsLeft--;
    numLoaded++;

    if (loadsLeft <= 0 || numLoaded >= numToLoad)
    {
      break;
    }
  }
}

/*
 Logical ResourceCache::isDoneLoading()
 {
 return numLoaded >= numToLoad || currLoadIndex >= itemList.count;
 }
 */
Logical ResourceCache::isDoneLoading()
{
  return numLoaded >= numToLoad || currLoadIndex >= loadQueue.count;
}

void ResourceCache::loadAllNow()
{
  prepLoad();
  loadStride(10000);
}

void ResourceCache::unload()
{
  for (Coord1 i = 0; i < itemList.count; ++i)
  {
    ResourceCacheItem* item = itemList[i];

    if (item == NULL || item->willDelete == false)
    {
      continue;
    }

    unloadItem(i);
  }
}

void ResourceCache::unloadItem(Coord1 index)
{
  ResourceCacheItem* item = itemList[index];
  
  if (item == NULL)
  {
    return;
  }
  
  item->unloadResource();
  delete item;
  itemList[index] = NULL;
}

Point1 ResourceCache::percentLoaded()
{
  if (numToLoad >= 1)
  {
    return RM::clamp(static_cast<Point1>(numLoaded) / static_cast<Point1>(numToLoad),
                     0.0, 1.0);
  }

  return 1.0;
}


// ========================= ResourceManager =========================== //

ResourceCache resCache(NUM_TOTAL_RES);

spSkeleton* skeleton = NULL;
spAnimationState* animationState = NULL;

namespace ResourceManager
{
  String1
  workingStr(32),
  imgExtSuffix(".png"),
  imgSizeSuffix("");

  Point1 spineAnimScale = 0.25;

  Bitmap* hobbitBG = NULL;

  void loadCommon()
  {
    imgExtSuffix = ".png";
    resCache.newItemsWillDelete = false;
    // RMSystem->usingAssetCats = RMSystem->get_OS_family() == IOS_FAMILY;
    RMSystem->usingAssetCats = false;
    RMSystem->fallbackIsEmptyStyle = false;

    loadFont(futura130Font, "futura_130_efigs", EFIGS_CHARS, -7.0);

    loadTiles(selector3x3Normal, "selector_normal", 3, 3, HANDLE_TL, true);
    loadTiles(selector3x3Add, "selector_add", 3, 3, HANDLE_TL, true);
    loadTiles(selector3x3Fill, "selector_fill", 3, 3, HANDLE_TL, true);

    resCache.loadAllNow();
    resCache.newItemsWillDelete = true;
  }

  void loadTitle()
  {
    
  }

  void initTitle()
  {

  }

  void unloadTitle()
  {
    unloadCaches();
  }
  
  void loadTestCinema()
  {
    loadSpine(introPalaceMedSpine, "intro_palace_med");

    loadFont(shackletonFont, "shackleton", EFIGS_CHARS, -7.0);

    loadSound(boxThumpSFX, "box_thump.wav");
    loadSound(carAwaySFX, "car_away_squeal01.wav");
    loadSound(carDecelSFX, "car_decelerate_01.wav");
    loadSound(tireSkidSFX, "tire_skid_squeal03.wav");
  }
  
  void initTestCinema()
  {
    
  }
  
  void unloadTestCinema()
  {
    unloadCaches();
  }

  void loadPlay()
  {
    loadImage(bgImg, "bg_0");

    loadPizzaPlayer();
    // loadImage(skullTestImg, "angry_castle_block_big_window", HANDLE_TL);
    
    //==== JACRUM algorithmic block ====//
    loadImage(brickCenterImg, "brick_center");
    loadImage(brick9TopImg, "brick_9top");
    loadImage(breakableA9TopImg, "breakable_a_9top");
    loadImage(breakableACenterImg, "breakable_a_center");
    loadImage(breakableB9TopImg, "breakable_b_9top");
    loadImage(breakableBCenterImg, "breakable_b_center");
    loadImage(steelCenterImg, "steel_center");
    loadImage(steel9TopImg, "steel_9top");
    loadImage(mover9TopImg, "mover_9top");
    loadImage(moverCenterImg, "mover_center");
    loadImage(spikebarImg, "spikebar");
    
    loadImage(blockWoodFillImg, "block_wood_fill");
    loadImage(blockWoodBorderImg, "block_wood_border");
    
    loadImage(blockSandstoneFillImg, "block_sandstone_fill");
    loadImage(blockSandstoneBorderImg, "block_sandstone_border");
    loadImage(blockSandstoneTrixImg, "block_sandstone_trix");
    
    loadImage(blockSandstoneCrackedFillImg, "block_sandstone_cracked_fill");
    loadImage(blockSandstoneCrackedBorderImg, "block_sandstone_cracked_border");
    loadImage(blockSandstoneCrackedTrixImg, "block_sandstone_cracked_trix");
    
    loadImage(blockSandstoneSmallBorderImg, "block_sandstone_small_border");
    loadImage(blockSandstoneSmallTrixImg, "block_sandstone_small_trix");
    
    loadImage(blockSandstoneSmallCrackedBorderImg, "block_sandstone_small_cracked_border");
    loadImage(blockSandstoneSmallCrackedTrixImg, "block_sandstone_small_cracked_trix");
    
    loadImage(blockSteelFillImg, "block_steel_fill");
    loadImage(blockSteelBorderImg, "block_steel_border");
    loadImage(blockSteelTrixImg, "block_steel_trix");
    
    loadImage(blockSpikesBorderImg, "block_spikes_border");
    loadImage(algoSpikesImg, "terrain_spikes");

    //==== END JACRUM ====//

    loadTiles(coinPickupSet, "coins", 16, 4, HANDLE_C);

    loadImage(crackImg, "cracks");
    loadTiles(dmgMultiTexSet, "breakchunks_sheet", 2, 2);
    
    // loadImage(halfSpikerImg, "spike_breaker", Point2(16.0, 16.0));
    loadImage(halfSpikeRight, "spikebreaker_right", Point2(64.0, 64.0));
    loadImage(halfSpikeDown, "spikebreaker_down", Point2(64.0, 64.0));
    loadImage(halfSpikeLeft, "spikebreaker_left", Point2(96.0, 64.0));
    loadImage(halfSpikeUp, "spikebreaker_up", Point2(64.0, 102.0));
    loadImage(spikeball64Img, "spikeball_small", HANDLE_C);
    loadImage(spikeball128Img, "spikeball_big", HANDLE_C);
    loadImage(bouncerImg, "bouncer_small", HANDLE_C);

    loadImage(batExplosionImg, "explosion", HANDLE_C);

    loadImage(genericShadowImg, "shadow", HANDLE_C);
    
    //=== particle effects
    loadImage(hitBaseGlowImg, "hit_baseglow", Point2(32.0, 32.0));
    loadImage(hitSmokeImg, "hit_smoke", Point2(16.0, 16.0) * 0.5);
    loadImage(hitSparkImg, "hit_spark", Point2(24.0, 16.0) * 0.5);
    loadTiles(hitSplashSet, "hit_splash", 1, 4, Point2(8.0, 8.0));
    //=== end particle effects
    
    loadImage(tempLightningImg, "lightning_front", HANDLE_LC, true);

    loadSound(cowbellSnd, "cowbell.wav");
    // loadSound(musicSnd, "story.ima4");

  }

  void initPlay()
  {
    initPizzaPlayer();
    
    getImg(tempLightningImg)->repeats();
  }

  void unloadPlay()
  {
    unloadCaches();
  }
  
  void loadPizzaPlayer()
  {
    /*
    loadImage(pizzaBaseImg, "tempcrust", HANDLE_C);
    loadImage(pizzaToppings0Img, "temp_pepperoni", HANDLE_C);
    loadImage(pizzaToppings1Img, "temp_olive", HANDLE_C);
    loadImage(pizzaToppings2Img, "temp_mushroom", HANDLE_C);
    loadSpine(pizzaEyesSpine, "tempeyes_ping");
     */
    loadImage(pizzaBaseImg, "crust_cheese", HANDLE_C);
    loadImage(pizzaToppings0Img, "temp_pepperoni", HANDLE_C);
    loadImage(pizzaToppings1Img, "temp_olive", HANDLE_C);
    loadImage(pizzaToppings2Img, "temp_mushroom", HANDLE_C);
    loadSpine(pizzaEyesSpine, "eyes_normal");
    loadSpine(pizzaMouthSpine, "mouth_normal");
    loadSpine(pizzaNoseSpine, "nose_carrot");
    loadSpine(pizzaAccSpine, "acc_beanie");
  }
  
  void initPizzaPlayer()
  {
    
  }

  void unloadPizzaPlayer()
  {
    resCache.unloadItem(pizzaBaseImg);
    resCache.unloadItem(pizzaToppings0Img);
    resCache.unloadItem(pizzaToppings1Img);
    resCache.unloadItem(pizzaToppings2Img);
    resCache.unloadItem(pizzaEyesSpine);
    resCache.unloadItem(pizzaMouthSpine);
    resCache.unloadItem(pizzaNoseSpine);
    resCache.unloadItem(pizzaAccSpine);
  }

  void loadMap()
  {
    RMSystem->usingAssetCats = false;
    RMSystem->fallbackIsEmptyStyle = false;

    loadPizzaPlayer();
    
    loadTiles(levelNodesSet, "level_nodes", 5, 1, HANDLE_C);
    loadImage(mapPizzaImg, "mappizza", HANDLE_C);
    loadSpine(mapTreeSpine, "map_tree");
    loadSpine(mapFlowersSpine, "map_flowers");
    loadSpine(mapDeadTreeSpine, "map_deadtree");
    loadSpine(mapPalmTreeSpine, "map_palm");
    loadSpine(mapFernsSpine, "map_ferns");
    loadSpine(mapVolcanoSpine, "map_volcano");
    loadSpine(mapCandleSpine, "map_candle");
    loadSpine(mapBananaSpine, "map_banana");
    loadSpine(mapLEDSpine, "map_led");
    loadSpine(mapServerSpine, "map_server");
    loadSpine(mapPolypsSpine, "map_polyps");
    loadSpine(mapPalaceSpine, "map_palace");
    loadSpine(mapBridgeHSpine, "map_bridgeh");
    loadSpine(mapBridgeVSpine, "map_bridgev");
    loadSpine(mapWave1Spine, "map_wave1");
    loadSpine(mapWave2Spine, "map_wave2");
    loadSpine(mapShopSpine, "map_shop");
    loadSpine(mapHammerBrosSpine, "map_hammerbro");

    loadTiles(mapBackgroundSet, "map_terrain_padded", 32, 32);
    loadTiles(mapObjectSet, "map_objects_padded", 16, 16);

    /*JACRUM algorithmic block*/
    loadImage(yaryortImg, "yaryort");
    //loadImage(redrockBottomImg, "redrock_bottom");
    //loadImage(redrockTopImg, "redrock_top");
    //loadImage(redrockLeftImg, "redrock_left");
    //loadImage(redrockRightImg, "redrock_right");
    loadImage(redrockCenterImg, "redrock_center");
    loadImage(redrockBorderImg, "redrock_border");
    loadImage(redrockTerrainBordersImg, "redrock_terrain_borders");
    loadImage(spikebarImg, "spikebar");
    loadImage(brickCenterImg, "brick_center");
    loadImage(brick9TopImg, "brick_9top");
    loadImage(glassCenterImg, "glass_center");
    loadImage(glass9TopImg, "glass_9top");
    loadImage(glassTriXImg, "glass_trix");
    loadImage(stoneCenterImg, "stone_center");
    loadImage(stone9TopImg, "stone_9top");
    loadImage(woodCenterImg, "wood_center");
    loadImage(wood9TopImg, "wood_9top");
    loadImage(steelCenterImg, "steel_center");
    loadImage(steel9TopImg, "steel_9top");
    /*END JACRUM*/
  }
  
  void initMap()
  {
    initPizzaPlayer();
    
    for (Coord1 i = 0; i < getImgSet(mapBackgroundSet).count; ++i)
    {
      getImgSet(mapBackgroundSet)[i]->nudge_uv_box(1.0);
    }
    
    for (Coord1 i = 0; i < getImgSet(mapObjectSet).count; ++i)
    {
      getImgSet(mapObjectSet)[i]->nudge_uv_box(1.0);
    }
  }
  
  void unloadMap()
  {
    unloadCaches();
  }

  void loadImage(Coord1 imgIndex, const Char* imgBase, Point2 smartHandle, Logical overexpose)
  {
    ImageCacheItem* loader = new ImageCacheItem();
    loader->loadHandle = smartHandle;
    loader->loadOverexpose = overexpose;

    resCache.itemList[imgIndex] = loader;
    resCache.setItemDef(imgIndex, RES_IMAGE, imgStr(imgBase));
  }

  void loadTiles(Coord1 setIndex, const Char* imgBase,
                 Coord1 cols, Coord1 rows, Point2 smartHandle, Logical overexpose)
  {
    ImageTilesCacheItem* loader = new ImageTilesCacheItem();
    loader->loadHandle = smartHandle;
    loader->loadOverexpose = overexpose;
    loader->cols = cols;
    loader->rows = rows;

    resCache.itemList[setIndex] = loader;
    resCache.setItemDef(setIndex, RES_IMAGE_SET, imgStr(imgBase));
  }
  
  /*
  void loadSingleTile(Coord1 setIndex, const Char* imgBase,
                      Point2 smartHandle, Logical overexpose)
  {
    ImageTilesCacheItem* loader = new ImageTilesCacheItem();
    loader->loadHandle = smartHandle;
    loader->loadOverexpose = overexpose;
    
    resCache.itemList[setIndex] = loader;
    resCache.setItemDef(setIndex, RES_IMAGE_SET, imgStr(imgBase));
  }

  ImageCollectionCacheItem* createDerivImgSet(Coord1 setIndex, Logical ownsImages)
  {
    ImageCollectionCacheItem* setCache = new ImageCollectionCacheItem();
    setCache->ownsImages = ownsImages;

    resCache.itemList[setIndex] = setCache;
    resCache.setItemDef(setIndex, RES_IMAGE_SET, "");

    return setCache;
  }
   */

  void loadTilesMorph(Coord1 setIndex, const Char* imgBase, Coord1 cols, Coord1 rows,
                      Coord1 morphIndex)
  {
    // loadTiles(setIndex, imgBase, cols, rows);
    // dmorphCache[morphIndex]->addLayer(imgsetCache[setIndex]);
  }

  void loadWarp(Coord1 warpIndex, const Char* warpFilename, Coord1 imgIndex,
                Logical loops)
  {
    DWarpCacheItem* loader = new DWarpCacheItem();
    loader->imgIndex = imgIndex;

    resCache.itemList[warpIndex] = loader;
    resCache.setItemDef(warpIndex, RES_DWARP, warpFilename);
  }

  void loadSound(Coord1 soundIndex, const Char* filename)
  {
    SoundCacheItem* loader = new SoundCacheItem();

    resCache.itemList[soundIndex] = loader;
    resCache.setItemDef(soundIndex, RES_SOUND, filename);
  }

  void loadFont(Coord1 fontIndex, const Char* filename, const String2& fontChars,
                Point1 widthAdjust, Point1 heightAdjust)
  {
    FontCacheItem* loader = new FontCacheItem();
    loader->charSet = fontChars;
    loader->charSizeAdjust.set(widthAdjust, heightAdjust);

    resCache.itemList[fontIndex] = loader;
    resCache.setItemDef(fontIndex, RES_FONT, imgStr(filename));
  }

  void loadSpine(Coord1 spineIndex, const Char* baseFilename)
  {
    SpineAnimCacheItem* loader = new SpineAnimCacheItem();
    loader->animScale = spineAnimScale;
    loader->fullImgStr = imgStr(baseFilename);
    
    resCache.itemList[spineIndex] = loader;
    resCache.setItemDef(spineIndex, RES_SPINE, baseFilename);
  }

  String1& imgStr(const Char* imgBase)
  {
    workingStr.clear();
    workingStr += imgBase;

    if (workingStr.last() == '&')
    {
      workingStr.remove_last();
      workingStr += langSuffix;
    }

    if (RMSystem->usingAssetCats == false)
    {
      workingStr += imgSizeSuffix;
      workingStr += imgExtSuffix;
    }

    return workingStr;
  }

  String1& pngStr(const Char* imgBase)
  {
    workingStr.clear();
    workingStr += imgBase;

    if (workingStr.last() == '&')
    {
      workingStr.remove_last();
      workingStr += langSuffix;
    }

    if (RMSystem->usingAssetCats == false)
    {
      workingStr += imgSizeSuffix;
      workingStr += PNG_EXT;
    }

    return workingStr;
  }

  String1& warpStr(const Char* warpBase)
  {
    workingStr.clear();
    workingStr += warpBase;
    workingStr += ".dwarp4";
    return workingStr;
  }

  Sound* getSoundByFilename(const String1& fullFilename)
  {
    for (Coord1 i = 0; i < NUM_TOTAL_RES; ++i)
    {
      ResourceCacheItem* item = resCache.itemList[i];
      
      if (item != NULL &&
          item->resType == RES_SOUND)
      {
        if (fullFilename == item->filename)
        {
          return reinterpret_cast<Sound*>(item->getResource());
        }
      }
    }
    
    return NULL;
  }

  void unloadCaches()
  {
    resCache.unload();
  }
}

Image* getImg(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_IMAGE);

  return reinterpret_cast<Image*>(item->getResource());
}

DataList<Image*>& getImgSet(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    rmassert(false);  // if the item doesn't exist at all, I can't reutrn its img set
  }
  
  rmassert(item->resType == RES_IMAGE_SET);

  return *(reinterpret_cast<DataList<Image*>* >(item->getResource()));
}

DWarpAnimation* getWarp(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_DWARP);

  return reinterpret_cast<DWarpAnimation*>(item->getResource());
}

Sound* getSound(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_SOUND);

  return reinterpret_cast<Sound*>(item->getResource());
}

RMFont* getFont(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_FONT);

  return reinterpret_cast<RMFont*>(item->getResource());
}

spAtlas* getSpineAtlas(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_SPINE);

  return static_cast<SpineAnimCacheItem*>(item)->atlasData;
}

spSkeletonData* getSpineSkeleton(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];

  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_SPINE);

  return static_cast<SpineAnimCacheItem*>(item)->skeletonData;
}

spAnimationStateData* getSpineAnim(Coord1 resID)
{
  ResourceCacheItem* item = resCache.itemList[resID];
  
  if (item == NULL)
  {
    return NULL;
  }
  
  rmassert(item->resType == RES_SPINE);
  
  return static_cast<SpineAnimCacheItem*>(item)->animStateData;
}

const String2& getStr(Coord1 strID)
{
  return LSTRINGS[strID];
}
