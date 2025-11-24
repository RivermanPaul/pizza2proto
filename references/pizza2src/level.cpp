/*
 *  level.cpp
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 11/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "level.h"
#include "mainapp.h"
#include "rmstdinput.h"
#include "resourceman.h"
#include "rmeffect.h"

using std::cout;
using std::endl;
using namespace ResourceManager;

// ========================= PizzaLevelInterface =========================== //

PizzaLevelInterface::PizzaLevelInterface() :
  ActionListener(),
  SpineEventListener(),

  levelID(),
  levelRegion(GRAVEYARD_BG),

  levelState(LEVEL_PASSIVE),

  randGen(4385378),

  tutorial(NULL),
  tutShown(false),

  rmboxCamera(),
  shaker(0.3),

  worldTimeMult(1.0),

  musicFilename(16),

  currPointList(64),
  endpoint(),

  enemiesEnabled(8),

  backgroundType(GRAVEYARD_BG),

  healTimer(5.0),  // this is set during update

  shieldDownTimer(10.0),
  shieldActive(false),

  updateGOBs(true),
  drawGOBs(true)
{
  rmboxCamera.size = RM_WH;
  
  shieldDownTimer.reset(12.0 - 2.0 * Pizza::getEquippedSeasoningLevel(SEAS_SHIELD_0));
}

PizzaLevelInterface::~PizzaLevelInterface()
{
  if (RMPhysics != NULL)
  {
    delete RMPhysics;
    RMPhysics = NULL;
  }
}

Logical PizzaLevelInterface::lostToDMGStd()
{
  return activeGameplayData->currPlayerHP <= 0.0;
}

Point2 PizzaLevelInterface::rmcamboxToScreen(Point2 rmcamboxPt)
{
  Box originRMCamBox(0.0, 0.0, rmboxCamera.size.x, rmboxCamera.size.y);
  
  Point2 result = (rmcamboxPt / originRMCamBox.wh - 0.5) * 2.0;
  result *= deviceScreenBox().xy / rmboxCamera.zoom;
  return result;
}

void PizzaLevelInterface::addLevelPt(Point1 xVal, Point1 yVal)
{
  currPointList.add(Point2(xVal, yVal) + endpoint);
}

void PizzaLevelInterface::clearPtList()
{
  currPointList.clear();
}

void PizzaLevelInterface::winLevel()
{
  if (levelState != LEVEL_PLAY) return;
  
  cout << "WIN" << endl;
  levelState = LEVEL_WIN;

  Point1 oldTime = activePlayer->getLevelFloat(activeGameplayData->currLevelIndex, L_TIME_KEY);
  
  if (activeGameplayData->currTime < oldTime)
  {
    activePlayer->getLevelFloat(activeGameplayData->currLevelIndex, L_TIME_KEY) = activeGameplayData->currTime;
  }
  
  activePlayer->getLevelBool(activeGameplayData->currLevelIndex, L_FIRST_STAR_KEY) = true;
  Pizza::savePlayerData();

  MapScene* map = new MapScene();
  map->anyLevelJustBeaten = true;
  RMSceneMan->sceneTransition(map);
}

void PizzaLevelInterface::loseLevel()
{
  if (levelState != LEVEL_PLAY) return;

  cout << "LOSE" << endl;
  levelState = LEVEL_LOSE;

  RMSceneMan->sceneTransition(new MapScene());
}

void PizzaLevelInterface::createSparksEffect(Point2 xy, Point1 scale, Point1 angle, ActionList* list)
{
  AdvancedBurstGenDef def(xy, angle, scale);
  
  // ============= smoke
  AdvancedBurstEffect* currEffect = new AdvancedBurstEffect(getImg(hitSmokeImg), 10,
                                                            def, RenderOptions::BLEND_NORMAL);
  AdvancedBurst& smoke = currEffect->burst;
  
  smoke.minLifespan = 1.0;
  smoke.maxLifespan = 2.0;
  
  smoke.minScale = Point2(0.1, 0.1);
  smoke.maxScale = Point2(1.0, 1.0);
  smoke.lockScale = true;
  
  smoke.minMvtAngle = 0.0; //(radians)
  smoke.maxMvtAngle = TWO_PI;
  
  smoke.colorChange = true;
  //smoke.colorChangeStartColor = ARGBColor.WHITE;
  smoke.colorChangeEndColor = ColorP4(1.0, 1.0, 1.0, 0.0);
  smoke.colorChangeFunction = &ParticleFn::linear;
  
  smoke.scaleChange = true;
  smoke.scaleChangeFunction = &ParticleFn::quinticEaseOut;
  smoke.scaleChangeFactor = Point2(2.0, 1.5);
  
  smoke.radiate = true;
  smoke.radiateMinDist = 25.0 * 0.5;
  smoke.radiateMaxDist = 50.0 * 0.5;
  smoke.radiateAngleAppliesToImgAngle = true;
  smoke.radiateFunction = &ParticleFn::linear;
  
  smoke.minRadius = 0.0;
  smoke.maxRadius = 32.0 * 0.5;
  
  list->addX(currEffect);
  
  // ============= base glow
  currEffect = new AdvancedBurstEffect(getImg(hitBaseGlowImg), 1,
                                       def,
                                       RenderOptions::BLEND_OVEREXPOSE);
  AdvancedBurst& baseglow = currEffect->burst;
  
  baseglow.minLifespan = 0.375;
  baseglow.maxLifespan = 0.375;
  
  baseglow.minScale = Point2(1.5, 1.5); //scale
  baseglow.maxScale = Point2(1.5, 1.5);
  
  //boolean scaleChange;
  //public Point2 endScale = Point2(2.0, 2.0);
  //public Function scaleChangeFunction = &ParticleFn::linear;
  baseglow.scaleChange = true;
  baseglow.scaleChangeFunction = &ParticleFn::quinticEaseOut;
  baseglow.scaleChangeFactor = Point2(1.5, 1.25);
  
  baseglow.colorChange = true;
  //baseglow.colorChangeStartColor = ARGBColor.WHITE;
  baseglow.colorChangeEndColor = ColorP4(1.0, 1.0, 1.0, 0.0);
  baseglow.colorChangeFunction = &ParticleFn::quadEaseIn;
  
  list->addX(currEffect);

  // ============= splash
  currEffect = new AdvancedBurstEffect(getImgSet(hitSplashSet), 20, def,
                                       RenderOptions::BLEND_OVEREXPOSE);
  AdvancedBurst& splash = currEffect->burst;
  
  splash.minLifespan = 0.225;
  splash.maxLifespan = 0.375;
  
  splash.minMvtAngle = -1.92; //(radians)
  splash.maxMvtAngle = 1.91;
  
  splash.radiate = true;
  splash.radiateMinDist = 75.0 * 0.5;
  splash.radiateMaxDist = 100.0 * 0.5;
  splash.radiateAngleAppliesToImgAngle = true;
  splash.radiateFunction = &ParticleFn::linear;
  
  splash.minScale = Point2(0.5, 0.25);
  splash.maxScale = Point2(2.0, 1.0);
  
  splash.scaleChange = true;
  splash.scaleChangeFunction = &ParticleFn::linear;
  splash.scaleChangeFactor = Point2(0.5, 1.25);
  
  splash.frameBlendSequence = true;
  splash.frameBlendSequenceStartFrames.add(0);
  splash.frameBlendSequenceNumFrames = 4;
  splash.frameBlendSequenceFunction = &ParticleFn::linear;
  
  splash.colorChange = true;
  //splash.colorChangeStartColor = ARGBColor.WHITE;
  splash.colorChangeEndColor = ColorP4(1.0, 1.0, 1.0, 0.0);
  splash.colorChangeFunction = &ParticleFn::cubicEaseIn;
  
  list->addX(currEffect);

  // ============= sparks
  currEffect = new AdvancedBurstEffect(getImg(hitSparkImg), 20, def,
                                       RenderOptions::BLEND_OVEREXPOSE);
  AdvancedBurst& sparks = currEffect->burst;
  
  sparks.minLifespan = 0.3;
  sparks.maxLifespan = 0.45;
  
  sparks.minRadius = 16 * 0.5;
  sparks.maxRadius = 32 * 0.5;
  
  sparks.minMvtAngle = -1.3; //(radians)
  sparks.maxMvtAngle = 1.3;
  
  sparks.minScale = Point2(2.0, 0.25);
  sparks.maxScale = Point2(4.0, 1.0);
  
  sparks.scaleChange = true;
  sparks.scaleChangeFunction = &ParticleFn::linear;
  sparks.scaleChangeFactor = Point2(0.25, 1.5);
  
  sparks.colorChange = true;
  //sparks.colorChangeStartColor = ARGBColor.WHITE;
  sparks.colorChangeEndColor = ColorP4(1.0, 1.0, 1.0, 0.0);
  sparks.colorChangeFunction = &ParticleFn::cubicEaseIn;
  
  sparks.radiate = true;
  sparks.radiateMinDist = 50.0 * 0.5;
  sparks.radiateMaxDist = 250.0 * 0.5;
  sparks.radiateAngleAppliesToImgAngle = true;
  sparks.radiateFunction = &ParticleFn::cubicEaseOut;
  
  list->addX(currEffect);
}

void PizzaLevelInterface::createTriangleShatter(VisRectangular* object, Coord2 dimensions, Point2 durationRange,
                                                ActionList* list)
{
  ParticleEffect* effect = new ParticleEffect();
  
  // fast version
  ProceduralBurst* burst = new ProceduralBurst(*object, dimensions.y, dimensions.x, durationRange);
  
  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(0.0, 150.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-TWO_PI, TWO_PI);
  
  burst->gravity.set(0.0, 40.0);
  burst->scaleBounds.set(0.75, 2.0);
  
  effect->emitters.add(burst);
  list->addX(effect);
}

PizzaLevelInterface* PizzaLevelInterface::createLevel(Coord2 levelType)
{
  PizzaLevelInterface* result = NULL;
  
  switch (levelType.x)
  {
    default:
    case LEVEL_TYPE_GLADIATOR:
    {
      switch (levelType.y)
      {
        default: cout << "Gladiator level " << levelType.y << " not found" << endl;
        case GL_LEVEL_UNARMED: result = new GLLevelUnarmed(); break;
        case GL_LEVEL_SPEARMEN_SIDE: result = new GLLevelSpearmenSide(); break;
        case GL_LEVEL_SPEARMEN_VERT: result = new GLLevelSpearmenVert(); break;
        case GL_LEVEL_BOMBBAT_1: result = new GLLevelBombbat(); break;
        case GL_LEVEL_BOMBBAT_2: result = new GLLevelBombbat2(); break;
          
        case GL_LEVEL_SPIDER_1: result = new GLLevelSpider(); break;
        case GL_LEVEL_SPIDER_2: result = new GLLevelSpider2(); break;
        case GL_LEVEL_CAVEMAN_1: result = new GLLevelCaveman(); break;
        case GL_LEVEL_CAVEMAN_2: result = new GLLevelCaveman2(); break;
        case GL_LEVEL_ENT_1: result = new GLLevelEnt(); break;
          
        case GL_LEVEL_ENT_2: result = new GLLevelEnt2(); break;
        case GL_LEVEL_DONUT_1: result = new GLLevelDonut(); break;
        case GL_LEVEL_DONUT_2: result = new GLLevelDonut2(); break;
        case GL_LEVEL_UTENSIL_1: result = new GLLevelUtensil(); break;
        case GL_LEVEL_UTENSIL_2: result = new GLLevelUtensil2(); break;
          
        case GL_LEVEL_GIANT_1: result = new GLLevelGiant(); break;
        case GL_LEVEL_GIANT_2: result = new GLLevelGiant2(); break;
        case GL_LEVEL_DOZER_1: result = new GLLevelDozer(); break;
        case GL_LEVEL_DOZER_2: result = new GLLevelDozer2(); break;
        case GL_LEVEL_SCIENTIST_1: result = new GLLevelScientist(); break;
          
        case GL_LEVEL_SCIENTIST_2: result = new GLLevelScientist2(); break;
        case GL_LEVEL_ALIEN_1: result = new GLLevelAlien(); break;
        case GL_LEVEL_ALIEN_2: result = new GLLevelAlien2(); break;
        case GL_LEVEL_WISP_1: result = new GLLevelWisp(); break;
        case GL_LEVEL_WISP_2: result = new GLLevelWisp2(); break;
          
        case GL_LEVEL_CHIP_1: result = new GLLevelChip(); break;
        case GL_LEVEL_CHIP_2: result = new GLLevelChip2(); break;
        case GL_LEVEL_CUPID_1: result = new GLLevelCupid(); break;
        case GL_LEVEL_CUPID_2: result = new GLLevelCupid2(); break;
        case GL_LEVEL_SPINY_1: result = new GLLevelSpiny(); break;
          
        case GL_LEVEL_SPINY_2: result = new GLLevelSpiny2(); break;
        case GL_LEVEL_REAPER_1: result = new GLLevelReaper(); break;
        case GL_LEVEL_REAPER_2: result = new GLLevelReaper2(); break;
      }
      break;
    }
      
    case LEVEL_TYPE_PLANE:
    {
      switch (levelType.y)
      {
        default: cout << "Plane level " << levelType.y << " not found" << endl;
        case PL_LEVEL_MOHAWK: result = new PLLevelMohawk(); break;
        case PL_LEVEL_CHICKEN: result = new PLLevelChicken(); break;
        case PL_LEVEL_BOMBBAT: result = new PLLevelBombbat(); break;
        case PL_LEVEL_SPIDER: result = new PLLevelSpider(); break;
        case PL_LEVEL_DONUT: result = new PLLevelDonut(); break;
          
        case PL_LEVEL_FLAMESKULL: result = new PLLevelFlameskull(); break;
        case PL_LEVEL_SPIDER_2: result = new PLLevelSpider2(); break;
        case PL_LEVEL_FLAMESKULL_2: result = new PLLevelFlameskull2(); break;
        case PL_LEVEL_CHIP: result = new PLLevelChip(); break;
        case PL_LEVEL_DONUT_2: result = new PLLevelDonut2(); break;
          
        case PL_LEVEL_WISP: result = new PLLevelWisp(); break;
        case PL_LEVEL_CHIP_2: result = new PLLevelChip2(); break;
        case PL_LEVEL_CUPID: result = new PLLevelCupid(); break;
        case PL_LEVEL_WISP_2: result = new PLLevelWisp2(); break;
        case PL_LEVEL_CUPID_2: result = new PLLevelCupid2(); break;
      }
      break;
    }
      
    case LEVEL_TYPE_PIRATE:
    {
      switch (levelType.y)
      {
        default: cout << "pirate level " << levelType.y << " not recognized!" << endl;
        case PI_LEVEL_0: result = new PILevel0(); break;
        case PI_LEVEL_1: result = new PILevel1(); break;
        case PI_LEVEL_2: result = new PILevel2(); break;
        case PI_LEVEL_3: result = new PILevel3(); break;
        case PI_LEVEL_4: result = new PILevel4(); break;
        case PI_LEVEL_5: result = new PILevel5(); break;
        case PI_LEVEL_6: result = new PILevel6(); break;
        case PI_LEVEL_7: result = new PILevel7(); break;
        case PI_LEVEL_8: result = new PILevel8(); break;
        case PI_LEVEL_9: result = new PILevel9(); break;
        case PI_LEVEL_TEST: result = new PILevelTest(); break;
      }
      break;
    }

    case LEVEL_TYPE_FIREMAN:
    {
      switch (levelType.y)
      {
        default: cout << "fireman level " << levelType.y << " not recognized!" << endl;
        case FI_LEVEL_0: result = new FILevel0(); break;
        case FI_LEVEL_1: result = new FILevel1(); break;
        case FI_LEVEL_2: result = new FILevel2(); break;
        case FI_LEVEL_3: result = new FILevel3(); break;
        case FI_LEVEL_4: result = new FILevel4(); break;
        case FI_LEVEL_5: result = new FILevel5(); break;
        case FI_LEVEL_6: result = new FILevel6(); break;
        case FI_LEVEL_7: result = new FILevel7(); break;
        case FI_LEVEL_8: result = new FILevel8(); break;
        case FI_LEVEL_9: result = new FILevel9(); break;
        case FI_LEVEL_TEST: result = new FILevelTest(); break;
      }
      break;
    }
      
    case LEVEL_TYPE_ANGRY:
    {
      switch (levelType.y)
      {
        default: cout << "angry level " << levelType.y << " not recognized!" << endl;
        case AN_LEVEL_0: result = new ANLevel0(); break;
        case AN_LEVEL_1: result = new ANLevel1(); break;
        case AN_LEVEL_2: result = new ANLevel2(); break;
        case AN_LEVEL_3: result = new ANLevel3(); break;
        case AN_LEVEL_4: result = new ANLevel4(); break;
        case AN_LEVEL_5: result = new ANLevel5(); break;
        case AN_LEVEL_6: result = new ANLevel6(); break;
        case AN_LEVEL_7: result = new ANLevel7(); break;
        case AN_LEVEL_8: result = new ANLevel8(); break;
        case AN_LEVEL_9: result = new ANLevel9(); break;

        case AN_LEVEL_TEST: result = new ANLevelTest(); break;
      }
      
      break;
    }
      
    case LEVEL_TYPE_SPRINT:
    {
      switch (levelType.y)
      {
        default: cout << "sprint level " << levelType.y << " not recognized!" << endl;
        case SP_LEVEL_0: result = new SPLevel0(); break;
        case SP_LEVEL_1: result = new SPLevel1(); break;
        case SP_LEVEL_2: result = new SPLevel2(); break;
        case SP_LEVEL_3: result = new SPLevel3(); break;
        case SP_LEVEL_4: result = new SPLevel4(); break;
        case SP_LEVEL_5: result = new SPLevel5(); break;
        case SP_LEVEL_6: result = new SPLevel6(); break;
        case SP_LEVEL_7: result = new SPLevel7(); break;
        case SP_LEVEL_8: result = new SPLevel8(); break;
        case SP_LEVEL_9: result = new SPLevel9(); break;
      }
      
      break;
    }
      
    case LEVEL_TYPE_PUPPY:
    {
      switch (levelType.y)
      {
        default: cout << "puppy level " << levelType.y << " not recognized!" << endl;
        case PU_LEVEL_0: result = new PULevel0(); break;
        case PU_LEVEL_1: result = new PULevel1(); break;
        case PU_LEVEL_2: result = new PULevel2(); break;
        case PU_LEVEL_3: result = new PULevel3(); break;
        case PU_LEVEL_4: result = new PULevel4(); break;
        case PU_LEVEL_5: result = new PULevel5(); break;
        case PU_LEVEL_6: result = new PULevel6(); break;
        case PU_LEVEL_7: result = new PULevel7(); break;
        case PU_LEVEL_8: result = new PULevel8(); break;
        case PU_LEVEL_9: result = new PULevel9(); break;
      }
      
      break;
    }
      
    case LEVEL_TYPE_SUMO:
    {
      switch (levelType.y)
      {
        default:
        case SUMO_LEVEL_GRAVEYARD: result = new SumoLevelGraveyard(); break;
        case SUMO_LEVEL_JURASSIC: result = new SumoLevelTrike(); break;
        case SUMO_LEVEL_CUPCAKE: result = new SumoLevelCupcake(); break;
        case SUMO_LEVEL_DUMP: result = new SumoLevelJunkboys(); break;
        case SUMO_LEVEL_EDISON: result = new SumoLevelEdison(); break;
        case SUMO_LEVEL_FUNGUS: result = new SumoLevelFungus(); break;
        case SUMO_LEVEL_EMPEROR: result = new SumoLevelEmperor(); break;

        // unused so far
        case SUMO_LEVEL_SNOWBALL: result = new SumoLevelSnowball(); break;
      }
      break;
    }
      
    case LEVEL_TYPE_PPARLOR:
    {
      result = new PizzeriaLevel(); break;
    }
      
    case LEVEL_TYPE_LAKE:
    {
      switch (levelType.y)
      {
        default:
        case LAKE_LEVEL_EASY: result = new LALevelEasy(); break;
        case LAKE_LEVEL_LOBSTER: result = new LALevelLobster(); break;
        case LAKE_LEVEL_LOBSTER_2: result = new LALevelLobster2(); break;
        case LAKE_LEVEL_PEARL: result = new LALevelPearl(); break;
        case LAKE_LEVEL_JELLYFISH: result = new LALevelJellyfish(); break;
          
        case LAKE_LEVEL_JELLYFISH_2: result = new LALevelJellyfish2(); break;
        case LAKE_LEVEL_BARREL: result = new LALevelBarrel(); break;
        case LAKE_LEVEL_BARREL_2: result = new LALevelBarrel2(); break;
        case LAKE_LEVEL_URCHIN: result = new LALevelUrchin(); break;
        case LAKE_LEVEL_SPINECLAM: result = new LALevelSpineclam(); break;
          
        case LAKE_LEVEL_SPINECLAM_2: result = new LALevelSpineclam2(); break;
        case LAKE_LEVEL_SWORDFISH: result = new LALevelSwordfish(); break;
        case LAKE_LEVEL_STINGRAY: result = new LALevelStingray(); break;
        case LAKE_LEVEL_SHARK: result = new LALevelShark(); break;
        case LAKE_LEVEL_SHARK_2: result = new LALevelShark2(); break;

        case LAKE_LEVEL_TEST: result = new LALevelTest(); break;
      }
      break;
    }
  }
  
  return result;
}

PizzaBackground* PizzaLevelInterface::createRegionalBackground(Coord1 region,
                                                               PizzaLevelInterface* level)
{
  PizzaBackground* result = NULL;
  
  switch (region)
  {
    default:
    case GRAVEYARD_BG: result = new GraveyardBackground(); break;
    case JURASSIC_BG: result = new JurassicBackground(); break;
    case CAKE_BG: result = new CakeBackground(); break;
    case DUMP_BG: result = new DumpBackground(); break;
    case SILICON_BG: result = new SiliconBackground(); break;
    case BODY_BG: result = new BodyBackground(); break;
    case CATACOMBS_BG: result = new CatacombsBackground(); break;
  }

  result->level = level;
  
  return result;
}

void PizzaLevelInterface::loadEnabledEnemies()
{
  for (Coord1 i = 0; i < enemiesEnabled.count; ++i)
  {
    loadEnemy(enemiesEnabled[i]);
  }
}

void PizzaLevelInterface::loadGenericChunkObjects()
{
  loadSpine(fblockSpine, "fblock");
  // loadImage(gourdoLanternImg, "gourdolantern", HANDLE_C);
  loadSpine(gourdoLanternSpine, "gourdo");
  loadImage(spikeballImg, "spikeball", HANDLE_C);
  loadImage(spinnerImg, "spinner", HANDLE_C);
  loadImage(spinnerBoltImg, "spinner_bolt", HANDLE_C);
  loadImage(moverPlatform192Img, "mover_short", Point2(16.0, 16.0));
  loadImage(moverPlatform384Img, "mover_long", Point2(16.0, 16.0));
  loadImage(waterSimBodyImg, "water_sim");
  loadImage(waterSimHighlightImg, "water_sim_highlight");
  loadSpine(disintigrateBlockSpine, "breakable");
  
  // handle roughly  collision TL
  loadTiles(shopButtonImgs, "big_red_button", 3, 1, Point2(44.0, 10.0));
}

void PizzaLevelInterface::loadEnemy(Coord1 enemyType)
{
  switch (enemyType)
  {
    default:
    case GL_ENEMY_SPEARMAN_UNARMED:
      loadSpine(unarmedSkeletonSpine, "unarmed");
      break;
    case GL_ENEMY_SPEARMAN_SIDE:
      loadSpine(horiSpearmanSpine, "spearman_forward");
      break;
    case GL_ENEMY_SPEARMAN_VERT:
      loadSpine(vertSpearmanSpine, "spearman_up");
      break;
    case GL_ENEMY_SPINY:
      loadSpine(spineySpine, "spikey");
      break;
    case GL_ENEMY_DONUT:
      loadSpine(donutSpine, "donut");
      loadImage(donutHoleImg, "donut_hole", HANDLE_C);
      break;
    case GL_ENEMY_ENT:
      loadSpine(entBodySpine, "ent");
      loadSpine(entRootSpine, "ent_root");
      break;
    case GL_ENEMY_REAPER:
      loadSpine(reaperSpine, "reaper");
      break;
    case GL_ENEMY_CUPID:
      loadSpine(cupidSpine, "cupid");
      loadImage(cupidArrowImg, "cupid_arrow", Point2(122.0, 8.0));
      break;
    case GL_ENEMY_GIANT:
      loadSpine(giantSpine, "giant");
      loadImage(giantHeadImg, "giant_head", HANDLE_C);
      break;
    case GL_ENEMY_WISP:
      loadSpine(wispSpine, "wisp");
      loadSpine(wispFireSpine, "wisp_attack");
      break;
    case GL_ENEMY_UTENSIL:
      loadSpine(utensilSpine, "utensil");
      break;
    case GL_ENEMY_CHIP:
      loadSpine(chipSpine, "chip");
      break;
    case GL_ENEMY_JUMPING_SPIDER:
      loadSpine(jumpingSpiderSpine, "spider");
      break;
    case GL_ENEMY_SCIENTIST:
      loadSpine(scientistSpine, "scientist");
      loadSpine(scientistAcidSpine, "acid_pool");
      loadImage(scientistVialImg, "test_tube", HANDLE_C);
      break;
    case GL_ENEMY_DOZER:
      loadSpine(dozerSpine, "dozer");
      break;
    case GL_ENEMY_CAVEMAN:
      loadSpine(cavemanSpine, "wheelman");
      break;
    case GL_ENEMY_ALIEN:
      loadSpine(alienSpine, "alien");
      loadTiles(alienLaserSet, "alien_bullet", 2, 1, HANDLE_C);
      break;
    case GL_ENEMY_BOMBBAT:
      loadSpine(batSpine, "bombbat");
      break;
    case GL_ENEMY_HAMMER_BROS:
      loadSpine(hammerBrosSpine, "bros");
      loadImage(hammerImg, "hammer", HANDLE_C);
      loadImage(dynamiteImg, "dynamite", Point2(16.0, 56.0));
      loadImage(speedSquareImg, "triangle", HANDLE_C);
      loadImage(scientistVialImg, "test_tube", HANDLE_C);  //////////
      break;
      
    case GL_ENEMY_DYNAMITE_BROS:
    case GL_ENEMY_TRIANGLE_BROS:
      rmassert(false);  // only enable GL_ENEMY_HAMMER_BROS since they share resources
      break;
      
    case PU_ENEMY_CHICKEN:
      loadSpine(chickenSpine, "rooster");
      break;
    case PU_ENEMY_FLAMESKULL:
      loadSpine(flameskullSpine, "flameskull");
      break;
    case PU_ENEMY_MOHAWK:
      loadSpine(mohawkSpine, "mohawk");
      break;
    case PU_ENEMY_BOMB:
      loadSpine(bombSpine, "bomb");
      break;
  }
}

void PizzaLevelInterface::updateSeasonings()
{
  updateHealing();
  updateShielding();
  updateBanking();
}

void PizzaLevelInterface::updateHealing()
{
  Coord1 healingLevel = Pizza::getEquippedSeasoningLevel(SEAS_HEAL_0);
  
  if (healingLevel <= 0)
  {
    return;
  }
  
  Point1 healInterval = 20.0 - 3.0 * healingLevel;
  
  if (activeGameplayData->currPlayerHP < activeGameplayData->maxPlayerHP - 0.1)
  {
    healTimer.update();
  }
  else
  {
    healTimer.reset(healInterval);
  }
  
  if (healTimer.getActive() == false)
  {
    activeGameplayData->addHP(1.0);
    healTimer.reset(healInterval);
  }
}

void PizzaLevelInterface::updateShielding()
{
  Coord1 shieldLevel = Pizza::getEquippedSeasoningLevel(SEAS_SHIELD_0);

  if (shieldLevel <= 0)
  {
    return;
  }
  
  if (shieldActive == false)
  {
    shieldDownTimer.update();
  }
  
  if (shieldDownTimer.getActive() == false)
  {
    shieldActive = true;
  }
}

Logical PizzaLevelInterface::tryHitShield()
{
  if (shieldActive == false)
  {
    return false;
  }
  
  shieldActive = false;
  shieldDownTimer.reset();
  return true;
}


// ========================= PizzaObjman =========================== //

PizzaObjman::PizzaObjman() :
  ObjmanStd<PizzaGOStd>(),

  bgTerrainList(),
  sortedDrawList(16),

  debrisList(),
  frontDebrisList()
{
  
}

PizzaObjman::~PizzaObjman()
{
  free_clear(bgTerrainList);
}

void PizzaObjman::update()
{
  updateObjects();
  garbageCollector();
  debrisList.update();
  frontDebrisList.update();
  actions.update();
  
  Point1 time = RM::pop_time_mult();
  frontActions.update();
  RM::push_time_mult(time);
}

void PizzaObjman::updateObjects()
{
  for (Coord1 i = 0; i < bgTerrainList.count; ++i)
  {
    bgTerrainList[i]->update();
  }
  
  ObjmanStd<PizzaGOStd>::updateObjects();
}

void PizzaObjman::drawObjects()
{
  sortedDrawList.clear();
  
  ObjmanStd<PizzaGOStd>::drawObjects();
  
  insertion_sort_ptrs(sortedDrawList);
  
  for (Coord1 i = 0; i < sortedDrawList.count; ++i)
  {
    sortedDrawList[i]->drawSorted();
  }
}

void PizzaObjman::drawBGTerrain()
{
  for (Coord1 i = 0; i < bgTerrainList.count; ++i) bgTerrainList[i]->redraw();
}

void PizzaObjman::addBGTerrain(PizzaGOStd* terrain)
{
  bgTerrainList.add(terrain);
}


// ========================= PhysicsLevelStd =========================== //

PhysicsLevelStd::PhysicsLevelStd() :
  PizzaLevelInterface(),
  b2ContactListener(),

  objman(),
  player(NULL),
  background(NULL),

  worldBox(RM_BOX),
  spikedTerrain(false),
  bounceTerrain(false),
  useOctBall(false),

  totalDestructibles(0),

  chunkWeights(NUM_CHUNK_TYPES, true),
  groundCameraPts(128),
  ceilingCameraPts(32),

  tiltPercent(0.0),
  cameraTowardGroundMult(0.85),

  skullsCreated(0),
  skullsDestroyed(0),

  puppyList(10),
  currBreed(randGen.randi(0, 2)),
  puppiesToFind(0),
  puppiesToReturn(0),
  puppiesTotal(0),
  puppyStartPt(0.0, 0.0),
  puppyHousePt(0.0, 0.0),
  objectiveDir(1),

  flagStartPt(0.0, 0.0),

  boss(NULL),

  pirateWinBox(-10000.0, -10000.0, 1.0, 1.0),
  pirateChest(NULL),
  pirateShip(NULL),
  shipWater(NULL),

  lastRaycastFixture(NULL),
  lastRaycastPt(0.0, 0.0),
  lastRaycastNormal(0.0, 0.0),
  lastRaycastFraction(0.0)

{
  objman.init(NUM_PHASES);
  
  ceilingCameraPts.add(Point2(-99999.0, 99999.0));
}

void PhysicsLevelStd::loadRes()
{

}

void PhysicsLevelStd::unloadRes()
{
  delete player;
  delete background;
}

void PhysicsLevelStd::initRes()
{
  RMPhysics = new RivermanPhysics(Point2(0.0, 800.0));
  RMPhysics->SetContactListener(this);
  
  if (Pizza::DRAW_DEBUG_PHYS == true)
  {
    RMPhysics->enable_debug_draw();
  }
  
  if (background != NULL)
  {
    background->init();
  }

  player = new PizzaPlayer();
  player->level = this;
  player->player = player;
  // getSound(musicSnd)->play();

  initSublevel();

  // Called AFTER initSublevel so that levels can change pizza's params before load
  // The check is so that other levels can call load first if they want, during their initSublevel sequence
  if (player->fixture == NULL)
  {
    player->load();
  }
}

void PhysicsLevelStd::update()
{
  updateGame();
}

void PhysicsLevelStd::updateGame()
{
  RM::push_time_mult(worldTimeMult);
  
  if (updateGOBs == true)
  {
    RMPhysics->update();
    updateSeasonings();
    player->update();  // this should happen before the controls update to update standing
    
    objman.updateObjects();
    objman.actions.update();
    
    updateControls();
  }
  
  updateLevelAI();
  updateCamera(false);

  if (background != NULL) background->update();

  if (levelState == LEVEL_PLAY)
  {
    activeGameplayData->currTime += RM::timePassed();
  }
  
  RM::pop_time_mult();
  
  objman.frontActions.update();
}

void PhysicsLevelStd::redraw()
{
  // == camera on
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);

  // draw background
  if (background != NULL)
  {
    background->drawBackground();
  }
  
  rmboxCamera.apply();

  if (drawGOBs == true)
  {
    objman.drawPhase(PHASE_BG_TERRAIN);
    colorizeAfterTerrain();
    
    drawShadows();
    
    player->redraw();
    objman.debrisList.redraw();
    drawPuppyCageBacks();
    objman.drawObjects();
    drawPirateShipFront();
    objman.frontDebrisList.redraw();
    objman.drawPhase(PHASE_PIRATE_GEMS);
  }
  
  drawAfterObjects();  // does nothing by default
  
  if (background != NULL) background->drawForeground();

  objman.actions.redraw();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    drawCameraTop();
    drawCameraGround();
  }
  
  RMPhysics->draw_debug();
  RMGraphics->pop_camera();
  // == camera off

  objman.frontActions.redraw();
  drawHUD();
}

void PhysicsLevelStd::drawPuppyCageBacks()
{
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->getActive() == false ||
        enemy->type != TYPE_ENEMY ||
        enemy->value != PU_ENEMY_CAGE)
    {
      continue;
    }
    
    PuppyCage* cage = static_cast<PuppyCage*>(enemy);
    cage->drawBack();
  }
}

void PhysicsLevelStd::updateCamera(Logical snapIntoPlace)
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = closestEnemyX();
  
  updateZoomStd(PLAYER_CAM_TOP_PAD_STD);
  updateFacingStd(enemyAnywhere, closestX);
  enemyCameraXStd(enemyAnywhere, closestX, snapIntoPlace);
  
  rmboxCamera.xy.y = 0.0;
}

void PhysicsLevelStd::updateCameraPuppy(Logical snapIntoPlace)
{
  Point1 camTargetX = player->getX();
  
  if (objectiveDir == 1)
  {
    camTargetX = player->getX() + rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
  }
  else
  {
    camTargetX = player->getX() - RM_WH.x - rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
  }
  
  Point3 objectsOfInterestData = evalObjectsOfInterestPuppy();
  
  // only do something different with the camera if there was at least 1 object of interest
  if (objectsOfInterestData.x >= 0.99)
  {
    Point1 averageInterestPoint = objectsOfInterestData.y / objectsOfInterestData.x;
    camTargetX = RM::lerp(player->getX(), averageInterestPoint, 0.5) - RM_WH.x * 0.5;
  }
  
  rmboxCamera.xy.x = RM::flatten(rmboxCamera.xy.x, camTargetX, RM::timePassed() * playerTXVel() * 1.2);
  
  // move the camera's y valuue 50% closer to its target every second
  updateCamBottomToGround(snapIntoPlace);
  
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
  
  player->setFacing(objectiveDir > 0.0 ? 1 : -1);
  
  updateZoomPuppy(128.0);
  
  // if (sun != NULL) sun->updatePostCamera();
}

void PhysicsLevelStd::updateCameraFree(Logical snapIntoPlace)
{
  Point1 camTargetX = player->getX();
  
  camTargetX = RM::lerp(player->getX() - RM_WH.x - rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x,
                        player->getX() + rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x,
                        playerXVelNorm() * 0.5 + 0.5);

  Point3 objectsOfInterestData = evalObjectsOfInterestFree();
  
  // only do something different with the camera if there was at least 1 object of interest
  if (objectsOfInterestData.x >= 0.99)
  {
    Point1 averageInterestPoint = objectsOfInterestData.y / objectsOfInterestData.x;
    camTargetX = RM::lerp(player->getX(), averageInterestPoint, 0.5) - RM_WH.x * 0.5;
  }
  
  rmboxCamera.xy.x = RM::flatten(rmboxCamera.xy.x, camTargetX, RM::timePassed() * playerTXVel() * 1.2);
  
  // move the camera's y valuue 50% closer to its target every second
  updateCamBottomToGround(snapIntoPlace);
  
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
  
  if (std::abs(playerXVelNorm()) > 0.2)
  {
    player->setFacing(playerXVelNorm() < 0.0 ? -1 : 1);
  }
  
  updateZoomFree(128.0);
  
  // if (sun != NULL) sun->updatePostCamera();
}

void PhysicsLevelStd::updateCameraSumo(Logical snapIntoPlace)
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = closestEnemyX();
  
  updateFacingStd(enemyAnywhere, closestX);
  updateCamBottomToGround(snapIntoPlace);

  updateZoomSumo(64.0);
  
  Point1 interestingEnemyX = closestX;
  
  // boss has strictly higher camera priority than enemies
  if (boss != NULL)
  {
    interestingEnemyX = boss->getX();
  }
  
  enemyCameraXStd(enemyAnywhere, interestingEnemyX, snapIntoPlace);
  
  clampRMBoxCamScreenXToWorld();
}

void PhysicsLevelStd::drawShadows()
{
  drawShadowsChunkMode();
}

void PhysicsLevelStd::drawShadowsChunkMode()
{
  
}

void PhysicsLevelStd::drawHUD()
{
  if (boss != NULL)
  {
    String2 bossDmgStr;
    bossDmgStr.double_string(boss->currHP, 4, 1);
    getFont(futura130Font)->draw(bossDmgStr, deviceSafeBox().norm_pos(HANDLE_TR), Point2(0.6, 0.6),
                                 0.0, HANDLE_TR, RED_SOLID);
  }
  
  if (player != NULL)
  {
    String2 playerDmgStr;
    playerDmgStr.double_string(activeGameplayData->currPlayerHP, 4, 1);
    getFont(futura130Font)->draw(playerDmgStr, deviceSafeBox().norm_pos(HANDLE_TL), Point2(0.6, 0.6),
                                 0.0, HANDLE_TL, BLUE_SOLID);
  }
}

void PhysicsLevelStd::updateCamBottomToGround(Logical snapIntoPlace)
{
  Point1 lerpWeight = snapIntoPlace == true ? 1.0 : cameraTowardGroundMult * RM::timePassed();
  rmboxCamera.xy.y = RM::lerp(rmboxCamera.xy.y, getCameraBottomY() - RM_WH.y, lerpWeight);
}

void PhysicsLevelStd::updateControls()
{
  if (levelState != LEVEL_PLAY) return;

  if (controller->getAnyConfirmStatus(JUST_PRESSED) == true ||
      controller->keyboard.get_status(JUST_PRESSED, kVK_Space) == true ||
      controller->keyboard.get_status(JUST_PRESSED, kVK_UpArrow) == true ||
      controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_W) == true)
  {
    // player asks level for jump mult here, level takes sticky into account
    player->tryJump();
    
    // ======================
    /*
    static Logical done = false;
    
    if (done == false)
    {
      done = true;
      Coord2 breakGrid(3, 3);
      VisRectangular testVR;
      testVR.setImage(getImg(pizzaBaseImg));
      testVR.setXY(400.0, 100.0);
      testVR.setScale(0.25, 0.25);
      
      BoneBreakEffect* breaker = new BoneBreakEffect(testVR, breakGrid.x, breakGrid.y);
      breaker->setVelFromPtPos(testVR.xy, 0.0);
      addDebris(breaker);
    }
     */
    /*
    if (objman.phaseList[PHASE_ENEMY_STD].count >= 1)
    {
      PizzaGO* firstEnemy = objman.phaseList[PHASE_ENEMY_STD].first->data;
      // cout << "first enemy " << firstEnemy << " pizza " << player << endl;
      // cout << "first enemy script " << &firstEnemy->script << endl;
      
      if (firstEnemy->getActive() == true)
      {
        // cout << "call break" << endl;
        firstEnemy->destroyed();
      }
    }
     */
    // ======================
  }

  tiltPercent = tiltMagnitude();

  player->tryMove(tiltPercent);

  // stickies slow the player here
  /*
  if ((playerMotionFlags & PizzaGO::SLOW_BIT) != 0)
  {
    Point2 vel = player->get_velocity();
    player->set_velocity(Point2(RM::flatten(vel.x, 0.0, 800.0 * RM::timePassed()), vel.y));
  }
   */

  // this is for slopes to even out bumpy areas
  if (playerGroundStick() == true &&
      player->standingTimer.getActive() == true &&
      player->nojumpTimer.getActive() == false)
  {
    player->apply_force(Point2(0.0, 1000.0), player->getXY());
  }
}

void PhysicsLevelStd::addDebris(ActionEvent* debrisEffect)
{
  objman.debrisList.addX(debrisEffect);
}

void PhysicsLevelStd::addFrontDebris(ActionEvent* debrisEffect)
{
  objman.frontDebrisList.addX(debrisEffect);
}

Point1 PhysicsLevelStd::closestEnemyX()
{
  /*
  Point1 invalidVal = 100000.0;
  Point1 closestValue = -invalidVal;
  Point1 closestLeft = -invalidVal;
  Point1 closestRight = invalidVal;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->type == TYPE_TERRAIN) continue;
    
    Point1 enemyX = enemyNode->data->getX();
    
    if (enemyX < player->getX() &&
        std::abs(player->getX() - enemyX) > closestLeft)
    {
      closestLeft = enemyX - player->getX();
    }
    else if (enemyX > player->getX() &&
             std::abs(player->getX() - enemyX) < closestRight)
    {
      closestRight = enemyX - player->getX();
    }
  }
  
  Logical enemyToLeft = closestLeft > -invalidVal;
  Logical enemyToRight = closestRight < invalidVal;
  
  Logical enemyAnywhere = enemyToLeft || enemyToRight;
  if (enemyAnywhere == false) return player->getX();
  
  closestValue = closestLeft;
  if (closestRight < std::abs(closestLeft)) closestValue = closestRight;
  
  return closestValue;
   */
  Point1 invalidVal = 100000.0;
  Point1 closestLeft = -invalidVal;
  Point1 closestRight = invalidVal;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->type == TYPE_TERRAIN) continue;
    
    Point1 enemyX = enemyNode->data->getX();
    
    if (enemyX < player->getX())
    {
      closestLeft = std::max(closestLeft, enemyX);
    }
    else if (enemyX > player->getX())
    {
      closestRight = std::min(closestRight, enemyX);
    }
  }
  
  // cout << "closest left " << closestLeft << " closest right " << closestRight << endl;
  
  Logical enemyToLeft = closestLeft > -invalidVal + 1.0;
  Logical enemyToRight = closestRight < invalidVal - 1.0;

  // cout << "to left " << enemyToLeft << " to right " << enemyToRight << endl;

  Logical enemyAnywhere = enemyToLeft || enemyToRight;
  
  if (enemyAnywhere == false)
  {
    return player->getX();
  }
  
  Point1 closestValue = closestLeft;
  
  if (std::abs(player->getX() - closestRight) < std::abs(player->getX() - closestLeft))
  {
    closestValue = closestRight;
    // cout << "closestValue is right " << closestValue << endl;
  }
  
  return closestValue;
}

Point1 PhysicsLevelStd::playerTXVel()
{
  return 600.0 + 100.0 * Pizza::getEquippedSeasoningLevel(SEAS_MOVE_SPEED_0);
}

Point1 PhysicsLevelStd::closestEnemyX(Point1 dir)
{
  Point1 closestValue = 100000.0 * dir;
  Logical enemyAnywhere = false;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    Point1 enemyX = enemyNode->data->getX();
    
    if (dir > 0.0 && enemyX < closestValue) closestValue = enemyX;
    else if (dir < 0.0 && enemyX > closestValue) closestValue = enemyX;
    
    enemyAnywhere = true;
  }
  
  if (enemyAnywhere == false) return player->getX();
  
  return closestValue;
}

void PhysicsLevelStd::createTopography(const Point2* topPoints, Coord1 numPoints, Image* firstImg)
{
  for (Coord1 i = 0; i < numPoints - 1; ++i)
  {
    Image* img = i == 0 ? firstImg : NULL;
    createSlopeQuad(topPoints[i], Point2(topPoints[i].x, topPoints[i].y + 64.0),
                    Point2(topPoints[i+1].x, topPoints[i+1].y + 64.0), topPoints[i+1],
                    img);
  }
}

void PhysicsLevelStd::invisibleWallsStd(const Box& wallBox, Point1 wallThickness)
{
  // left wall
  addPGO(TerrainQuad::create_ground_TL(Point2(wallBox.left() - wallThickness, wallBox.top()),
                                       Point2(wallThickness, wallBox.height()),
                                       NULL));
  
  // right wall
  addPGO(TerrainQuad::create_ground_TL(Point2(wallBox.right(), wallBox.top()),
                                       Point2(wallThickness, wallBox.height()),
                                       NULL));
  
  // top wall
  addPGO(TerrainQuad::create_ground_TL(Point2(wallBox.left(), wallBox.top() - wallThickness),
                                       Point2(wallBox.width(), wallThickness),
                                       NULL));
}

void PhysicsLevelStd::invisibleWallsLR(Point2 leftGroundPt, Point2 rightGroundPt)
{
  Point1 wallWidth = 128.0;
  Point1 wallHeight = 1024.0;
  
  // left wall
  TerrainQuad* leftWall = TerrainQuad::create_ground_TL(
                                                        Point2(leftGroundPt.x - wallWidth, leftGroundPt.y - wallHeight),
                                                        Point2(wallWidth, wallHeight),
                                                        NULL);
  addPGO(leftWall);
  leftWall->flags = 0x0;  // for spiked
  
  // right wall
  TerrainQuad* rightWall = TerrainQuad::create_ground_TL(
                                                         Point2(rightGroundPt.x, rightGroundPt.y - wallHeight),
                                                         Point2(wallWidth, wallHeight),
                                                         NULL);
  addPGO(rightWall);
  rightWall->flags = 0x0;  // for spiked
  
}

TerrainQuad* PhysicsLevelStd::createSlopeQuad(const Point2& pt0, const Point2& pt1,
                                         const Point2& pt2, const Point2& pt3,
                                         Image* img)
{
  TerrainQuad* quad = TerrainQuad::create_static_quad(pt0 + endpoint, pt1 + endpoint,
                                                      pt2 + endpoint, pt3 + endpoint);
  quad->setImage(img);
  addPGO(quad);
  return quad;
}

void PhysicsLevelStd::startStanding(Point2 groundPt)
{
  player->setXY(groundPt.x, groundPt.y - player->getHeight());
  player->isStanding = true;
  player->updateFromWorld();
}

void PhysicsLevelStd::updateZoomStd(Point1 yPad)
{
  Point1 playerTop = player->collisionBox().top() + yPad;
  Point1 camCeiling = deviceScreenBox().top();
  Point1 diff = playerTop - camCeiling;
  
  if (diff < 0.0)
  {
    Point1 camZoom = diff / (deviceScreenBox().height() * 1.0);
    camZoom = 1.0 / (1.0 + -camZoom);
    rmboxCamera.zoom.set(camZoom, camZoom);
  }
  else
  {
    rmboxCamera.zoom.set(1.0, 1.0);
  }
  
  // cout << "estinng zoom to " << rmboxCamera.zoom << endl;
}

void PhysicsLevelStd::updateZoomPuppy(Point1 playerYPad)
{
  Point1 yBottom = rmboxCamera.untranslatedBox().bottom();
  
  Point1 playerTop = player->collisionBox().top() + playerYPad;
  Point1 enemyTop = playerTop;
  Point1 camCeil = getCameraTopY();
  
  Point3 objectsOfInterestData = evalObjectsOfInterestPuppy();
  
  if (objectsOfInterestData.x >= 0.99)
  {
    enemyTop = objectsOfInterestData.z;
  }
  
  Point1 targetTop = std::min(playerTop, enemyTop);
  targetTop = std::min(targetTop, camCeil);
  // cout << "cam ceil " << camCeil << endl;

  Point1 camCeiling = yBottom - (RM_WH.y - deviceScreenBox().top());
  Point1 diff = targetTop - camCeiling;

  Point1 targetZoom = 1.0;
  // cout << "y bottom " << yBottom << " cam ceiling " << camCeiling << " diff " << diff << endl;

  if (diff < 0.0)
  {
    targetZoom = diff / (deviceScreenBox().height() * 1.0);
    targetZoom = 1.0 / (1.0 + -targetZoom);
    // rmboxCamera.zoom.set(camZoom, camZoom);
  }
  
  RM::flatten_me(rmboxCamera.zoom.x, targetZoom, 1.0 * RM::timePassed());
  RM::flatten_me(rmboxCamera.zoom.y, targetZoom, 1.0 * RM::timePassed());
}

void PhysicsLevelStd::updateZoomFree(Point1 playerYPad)
{
  Point1 yBottom = rmboxCamera.untranslatedBox().bottom();
  
  Point1 playerTop = player->collisionBox().top() + playerYPad;
  Point1 enemyTop = playerTop;
  Point1 camCeil = getCameraTopY();
  
  Point3 objectsOfInterestData = evalObjectsOfInterestFree();
  
  if (objectsOfInterestData.x >= 0.99)
  {
    enemyTop = objectsOfInterestData.z;
  }
  
  Point1 targetTop = std::min(playerTop, enemyTop);
  targetTop = std::min(targetTop, camCeil);
  // cout << "cam ceil " << camCeil << endl;
  
  Point1 camCeiling = yBottom - (RM_WH.y - deviceScreenBox().top());
  Point1 diff = targetTop - camCeiling;
  
  Point1 targetZoom = 1.0;
  // cout << "y bottom " << yBottom << " cam ceiling " << camCeiling << " diff " << diff << endl;
  
  if (diff < 0.0)
  {
    targetZoom = diff / (deviceScreenBox().height() * 1.0);
    targetZoom = 1.0 / (1.0 + -targetZoom);
    // rmboxCamera.zoom.set(camZoom, camZoom);
  }
  
  RM::flatten_me(rmboxCamera.zoom.x, targetZoom, 1.0 * RM::timePassed());
  RM::flatten_me(rmboxCamera.zoom.y, targetZoom, 1.0 * RM::timePassed());
}

void PhysicsLevelStd::updateZoomSumo(Point1 yPad)
{
  Point1 yBottom = rmboxCamera.untranslatedBox().bottom();
  
  Point1 playerTop = player->collisionBox().top() + yPad;
  Point1 bossTop = playerTop + yPad;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->type == TYPE_SUMO_BOSS)
    {
      bossTop = std::min(bossTop, boss->collisionBox().top()  + yPad);
    }
  }
  
  Point1 targetTop = std::min(playerTop, bossTop);
  
  // only go at most a screen above the player so the camera doesn't zoom WAY out
  targetTop = std::max(targetTop, playerTop - RM_WH.y);
  
  Point1 camCeiling = yBottom - (RM_WH.y - deviceScreenBox().top());
  Point1 diff = targetTop - camCeiling;
  
  // cout << "y bottom " << yBottom << " cam ceiling " << camCeiling << " diff " << diff << endl;
  
  if (diff < 0.0)
  {
    Point1 camZoom = diff / (deviceScreenBox().height() * 1.0);
    camZoom = 1.0 / (1.0 + -camZoom);
    rmboxCamera.zoom.set(camZoom, camZoom);
  }
  else
  {
    rmboxCamera.zoom.set(1.0, 1.0);
  }
}

void PhysicsLevelStd::clampRMBoxCamScreenXToWorld()
{
  clampRMBoxCamScreenXToRange(worldBox.left(), worldBox.right());
}

void PhysicsLevelStd::clampRMBoxCamScreenXToRange(Point1 leftX, Point1 rightX)
{
  // not clamping the left side of rmboxCam--clamping the left side of the SCREEN
  Point1 screenLeftOffset = rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
  Point1 screenRightOffset = rmcamboxToScreen(Point2(RM_BOX.right(), 0.0)).x;
  rmboxCamera.clampHorizontal(Point2(leftX + screenLeftOffset,
                                     rightX + screenRightOffset));
}

void PhysicsLevelStd::updateFacingStd(Logical hasEnemies, Point1 closestX)
{
  // update facing
  Coord1 facing = 1;
  
  if (hasEnemies == true)
  {
    facing = closestX < player->getX() ? -1 : 1;
  }
  else
  {
    facing = tiltPercent < 0.0 ? -1 : 1;
  }
  
  player->setFacing(facing);
}

void PhysicsLevelStd::enemyCameraXStd(Logical hasEnemies, Point1 closestX, Logical snapIntoPlace)
{
  /*
  Point1 cameraTarget = rmboxCamera.xy.x;
  
  // enemy onscreen
  if (hasEnemies == true && std::abs(closestX) < deviceScreenBox().width())
  {
    Point1 between = RM::lerp(player->getX(), player->getX() + closestX, 0.5);
    cameraTarget = between - RM_WH.x * 0.5;
  }
  // enemy offscreen
  else if (hasEnemies == true)
  {
    cameraTarget = (player->facing == 1 ? player->getX() + rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x :
                    player->getX() - RM_WH.x - rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x);
  }
  // no enemies
  else
  {
    Point1 percentTarget = (tiltPercent + 1.0) * 0.5;  // 0.0-1.0
    cameraTarget = RM::lerp(player->getX() - deviceScreenBox().width(), player->getX(), percentTarget);
  }
  
  rmboxCamera.xy.x = RM::flatten(rmboxCamera.xy.x, cameraTarget, RM::timePassed() * playerTXVel() * 1.2);
  
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
   */
  
  Point1 cameraTarget = rmboxCamera.xy.x;

//  cout << "closest enemy "
//      << closestX << " player->getX " << player->getX() << " std::abs(player->getX() - closestX) "
//  << std::abs(player->getX() - closestX)
//    << endl;

  // enemy onscreen
  if (hasEnemies == true && std::abs(player->getX() - closestX) < deviceScreenBox().width())
  {
    Point1 between = RM::lerp(player->getX(), closestX, 0.5);
    cameraTarget = between - RM_WH.x * 0.5;
//    cout << "onscreen, player X " << player->getX() << " closest enemy "
//        << closestX << " betweem " << between << " target " << cameraTarget << endl;
  }
  // enemy offscreen
  else if (hasEnemies == true)
  {
    cameraTarget = (player->facing == 1 ? player->getX() + rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x :
                    player->getX() - RM_WH.x - rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x);
//    cout << "offscreen" << endl;
  }
  // no enemies
  else
  {
    Point1 percentTarget = (tiltPercent + 1.0) * 0.5;  // 0.0-1.0
    cameraTarget = RM::lerp(player->getX() - deviceScreenBox().width(), player->getX(), percentTarget);
//    cout << "none" << endl;
  }
  
  Point1 speed = snapIntoPlace ? 1000000.0 : RM::timePassed() * playerTXVel() * 1.2;
  rmboxCamera.xy.x = RM::flatten(rmboxCamera.xy.x, cameraTarget, speed);
  
  rmboxCamera.handle = RM_BOX.norm_pos(HANDLE_BC);
}

void PhysicsLevelStd::zoomToMinY(Point1 yTarget)
{
  rmboxCamera.zoom = minYZoom(yTarget);
}

Point2 PhysicsLevelStd::minYZoom(Point1 yTarget)
{
  Point1 highestTop = yTarget;
  Point1 camCeiling = rmboxCamera.getY();
  Point1 diff = highestTop - camCeiling;
  
  if (diff < 0.0)
  {
    Point1 camZoom = diff / rmboxCamera.handle.y;
    camZoom = 1.0 / (1.0 + -camZoom);
    return Point2(camZoom, camZoom);
  }
  else
  {
    return Point2(1.0, 1.0);
  }
}

Logical PhysicsLevelStd::lostToFallingStd()
{
  return player->collisionCircle().top() > getCameraBottomY();
}


Logical PhysicsLevelStd::enemyActive()
{
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    if (enemyNode->data->getActive() == true) return true;
  }
  
  return false;
}

Point1 PhysicsLevelStd::tiltMagnitude()
{
  Point1 result = 0.0;
  
  // cout << "tilt % " << Pizza::controller->get_accel().x << endl;
  Point1 topTolerance = 0.25;
  
  if (RMSystem->get_OS_family() == APPLETV_FAMILY)
  {
    topTolerance = 0.5;
  }
  
  Point1 accelXClamp = RM::clamp(controller->accelerometer.get_accel().x - controller->calib,
                                 -topTolerance, topTolerance);
  
  // controls for simulator
  if (RMSystem->on_iphone_simulator() == true)
  {
    if (controller->clickStatus(PRESSED) == true)
    {
      result = (controller->getXY().x - deviceScreenBox().center().x) / deviceScreenBox().center().x;
    }
    else
    {
      result = 0.0;
    }
  }
  // handle joysticks, they take precedence because they are
  else if (RM::approxEq(controller->gamepad.get_joystick(kGP_Joystick_L, 0).x, 0.0) == false)
  {
    result = RM::clamp(controller->gamepad.get_joystick(kGP_Joystick_L, 0).x, -1.0, 1.0);
  }
  else if (RM::approxEq(controller->gamepad.get_joystick(kGP_Joystick_L, 1).x, 0.0) == false)
  {
    result = RM::clamp(controller->gamepad.get_joystick(kGP_Joystick_L, 1).x, -1.0, 1.0);
  }
  // the accelerometers comes before dpad input because you sort of use the dpad to jump on apple tv
  else if (controller->getDirectionalButtonStatus(WEST, PRESSED) == true)
  {
    result = -1.0;
  }
  else if (controller->getDirectionalButtonStatus(EAST, PRESSED) == true)
  {
    result = 1.0;
  }
  else if (std::abs(accelXClamp) >= 0.01)
  {
    result = accelXClamp / topTolerance;
  }
  
  return result;
}

void PhysicsLevelStd::reportPlayerLanded(Logical slam)
{
  if (slam == true)
  {
    reportSlam();
  }
}

void PhysicsLevelStd::reportPlayerBump(Point1 impulseVal)
{
  // cout << "imp " << impulseVal << endl;
  
  if (impulseVal > 1000.0)
  {
    Point1 shakeMag = std::min(impulseVal / 1000.0, 6.0);
    Point1 shakeTime = shakeMag / 10.0;
    
    shaker.setMagnitude(shakeMag);
    shaker.timedShake(shakeTime);
  }
}

void PhysicsLevelStd::reportSlam()
{
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->lifeState == RM::ACTIVE)
    {
      enemy->pizzaSlammed();
    }
  }
  
  // this is for the buttons in the shop
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_BG_TERRAIN].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->lifeState == RM::ACTIVE)
    {
      enemy->pizzaSlammed();
    }
  }
}

Point1 PhysicsLevelStd::tiltMagnitudeVert()
{
  Point1 result = 0.0;
  
  // calculate accelerometer values
  // cout << "tilt % " << Pizza::controller->get_accel() << endl;
  Point1 maxTolerance = 0.25;
  Coord1 axisID = 2;
  
  if (RMSystem->get_OS_family() == APPLETV_FAMILY)
  {
    maxTolerance = 0.75;
    axisID = 1;
  }
  
  Point1 accelZClamp = RM::clamp(controller->accelerometer.get_accel()[axisID] - controller->calib,
                                 -maxTolerance, maxTolerance);
  
  // controls for simulator
  if (RMSystem->on_iphone_simulator() == true)
  {
    if (controller->clickStatus(PRESSED) == true)
    {
      result = (controller->getXY().y - SCREEN_CY) / SCREEN_CY;
    }
    else
    {
      result = 0.0;
    }
  }
  else if (RM::approxEq(controller->gamepad.get_joystick(kGP_Joystick_L, 0).y, 0.0) == false)
  {
    result = RM::clamp(controller->gamepad.get_joystick(kGP_Joystick_L, 0).y, -1.0, 1.0);
  }
  else if (RM::approxEq(controller->gamepad.get_joystick(kGP_Joystick_L, 1).y, 0.0) == false)
  {
    result = RM::clamp(controller->gamepad.get_joystick(kGP_Joystick_L, 1).y, -1.0, 1.0);
  }
  else if (controller->getDirectionalButtonStatus(SOUTH, PRESSED) == true)
  {
    result = 1.0;
  }
  else if (controller->getDirectionalButtonStatus(NORTH, PRESSED) == true)
  {
    result = -1.0;
  }
  else if (std::abs(accelZClamp) >= 0.03 && RMSystem->get_OS_family() != IOS_FAMILY)
  {
    result = accelZClamp / maxTolerance;
  }
  
  return result;
}

void PhysicsLevelStd::addPGO(PizzaGOStd* pgo)
{
  pgo->level = this;
  pgo->player = player;
  objman.addPGO(pgo, pgo->phase);
  pgo->load();
}

PizzaGOStd* PhysicsLevelStd::createEnemy(Coord1 enemyType)
{
  PizzaGOStd* skele = NULL;

  switch (enemyType)
  {
    default:
    case GL_ENEMY_SPEARMAN_UNARMED:
      skele = new UnarmedSkeleton();
      break;
    case GL_ENEMY_SPEARMAN_SIDE:
      skele = new SkeletonSpearman(SkeletonSpearman::VAL_SIDEWAYS);
      break;
    case GL_ENEMY_SPEARMAN_VERT:
      skele = new SkeletonSpearman(SkeletonSpearman::VAL_VERTICAL);
      break;
    case GL_ENEMY_SPINY:
      skele = new Spiny();
      break;
    case GL_ENEMY_DONUT:
      skele = new DonutCannon();
      break;
    case GL_ENEMY_ENT:
      skele = new Ent();
      break;
    case GL_ENEMY_REAPER:
      skele = new GrimReaper();
      break;
    case GL_ENEMY_CUPID:
      skele = new SkeleCupid();
      break;
    case GL_ENEMY_GIANT:
      skele = new Giant();
      break;
    case GL_ENEMY_WISP:
      skele = new Wisp();
      break;
    case GL_ENEMY_UTENSIL:
      skele = new Utensil();
      break;
    case GL_ENEMY_CHIP:
      skele = new Chip();
      break;
    case GL_ENEMY_JUMPING_SPIDER:
      skele = new JumpingSpider();
      break;
    case GL_ENEMY_SCIENTIST:
      skele = new Scientist();
      break;
    case GL_ENEMY_DOZER:
      skele = new Dozer();
      break;
    case GL_ENEMY_CAVEMAN:
      skele = new Caveman();
      break;
    case GL_ENEMY_ALIEN:
      skele = new Alien();
      break;
    case GL_ENEMY_BOMBBAT:
      skele = new BombBat();
      break;
    case GL_ENEMY_HAMMER_BROS:
    case GL_ENEMY_TRIANGLE_BROS:
    case GL_ENEMY_DYNAMITE_BROS:
      skele = new HammerBro(enemyType);
      break;
      
    case PU_ENEMY_CHICKEN:
      skele = new Chicken();
      break;
    case PU_ENEMY_FLAMESKULL:
      skele = new Flameskull();
      break;
    case PU_ENEMY_MOHAWK:
      skele = new Mohawk();
      break;

    case ENEMY_NONE:
      skele = NULL;
      break;
  }
  
  return skele;
}

void PhysicsLevelStd::createBlocksFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    TerrainQuad* dstructBlock = TerrainQuad::create_block_TL(currPointList[i], currPointList[i + 1] - currPointList[i],
                                                              NULL);
    dstructBlock->setGenericBlockTL(currPointList[i], currPointList[i + 1] - currPointList[i]);
    dstructBlock->setXY(currPointList[i]);
    addPGO(dstructBlock);

    // level has to be set before calling this
    dstructBlock->setDrawerRandomizedBreakable();
  }
}

void PhysicsLevelStd::createBlocksFromTLandWH()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    Point2 size = currPointList[i + 1] - endpoint;
    
    TerrainQuad* dstructBlock = TerrainQuad::create_block_TL(currPointList[i], size,
                                                             NULL);
    dstructBlock->setGenericBlockTL(currPointList[i], size);
    dstructBlock->setXY(currPointList[i]);
    addPGO(dstructBlock);
    
    // level has to be set before calling this
    dstructBlock->setDrawerRandomizedBreakable();
  }
}

void PhysicsLevelStd::createSteelFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    TerrainQuad* dstructBlock = TerrainQuad::create_block_TL(currPointList[i], currPointList[i + 1] - currPointList[i],
                                                             NULL);
    dstructBlock->setGenericBlockTL(currPointList[i], currPointList[i + 1] - currPointList[i]);
    dstructBlock->setXY(currPointList[i]);

    dstructBlock->crushesPhysical = false;
    dstructBlock->set_restitution(0.0);
    dstructBlock->change_density(10.0);
    
    addPGO(dstructBlock);
    
    // level has to be set before calling this
    dstructBlock->setDrawerSteel();
  }
}

void PhysicsLevelStd::createSteelFromTLandWH()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    Point2 size = currPointList[i + 1] - endpoint;

    TerrainQuad* dstructBlock = TerrainQuad::create_block_TL(currPointList[i], size,
                                                             NULL);
    dstructBlock->setGenericBlockTL(currPointList[i], size);
    dstructBlock->setXY(currPointList[i]);
    dstructBlock->crushesPhysical = false;
    dstructBlock->set_restitution(0.0);
    dstructBlock->change_density(10.0);
    addPGO(dstructBlock);
    
    // level has to be set before calling this
    dstructBlock->setDrawerSteel();
  }
}

void PhysicsLevelStd::createStaticBreakableBlocksFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    TerrainQuad* dstructBlock = TerrainQuad::create_static_TL(currPointList[i], currPointList[i + 1] - currPointList[i],
                                                             NULL);
    dstructBlock->setXY(currPointList[i] + (currPointList[i + 1] - currPointList[i]) * 0.5);
    dstructBlock->setGenericBlockTL(currPointList[i], currPointList[i + 1] - currPointList[i]);
    addPGO(dstructBlock);

    // level has to be set before calling this
    dstructBlock->setDrawerRandomizedBreakable();
  }
}

void PhysicsLevelStd::createSpikeBlocksFromQuads()
{
  for (Coord1 i = 0; i < currPointList.count; i += 4)
  {
    TerrainQuad* spikeQuad = TerrainQuad::create_static_quad(currPointList[i], currPointList[i + 1],
                                                             currPointList[i + 2], currPointList[i + 3]);
    spikeQuad->setSpikey();
    addPGO(spikeQuad);
    
    // level has to be set before calling this
    spikeQuad->setDrawerSpikes();
  }
}


void PhysicsLevelStd::createHiddenSkullsInLastBlockFromC(Logical useSkulls)
{
  if (useSkulls == false)
  {
    return;
  }
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    AngrySkull* skull = new AngrySkull();
    skull->setXY(currPointList[i]);
    addPGO(skull);
    skull->hideInBreakable(objman.phaseList[PHASE_BG_TERRAIN].getLast(), 0x0);
  }
}

void PhysicsLevelStd::createHiddenPhysCoinsInLastBlockFromC()
{
  objman.phaseList[PHASE_BG_TERRAIN].getLast()->coinToDropType = NO_COIN;

  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaCoinStd* coin = new PizzaCoinStd();
    coin->usePhysics = true;
    coin->setXY(currPointList[i]);
    addPGO(coin);
    coin->hideInBreakable(objman.phaseList[PHASE_BG_TERRAIN].getLast(), 0x0);
  }
}


void PhysicsLevelStd::createBurningBlocksFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    addPGO(TerrainQuad::createFiremanBlock(currPointList[i], currPointList[i+1]));
  }
}

void PhysicsLevelStd::createBurningBlocksFromBCandWH()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    Point2 size = currPointList[i+1] - endpoint;
    Point2 topLeft = currPointList[i] - Point2(size.x * 0.5, size.y);
    
    addPGO(TerrainQuad::createFiremanBlock(topLeft, topLeft + size));
  }
}

void PhysicsLevelStd::createBurningBlocksFromBC(Point1 squareSize)
{
  for (Coord1 i = 0; i < currPointList.count; i++)
  {
    Point2 size(squareSize, squareSize);
    Point2 topLeft = currPointList[i] - Point2(size.x * 0.5, size.y);
    
    addPGO(TerrainQuad::createFiremanBlock(topLeft, topLeft + size));
  }
}

void PhysicsLevelStd::createSpinnersFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    addPGO(TerrainQuad::createSpinner(currPointList[i], currPointList[i+1]));
  }
}

void PhysicsLevelStd::createCoinsFromCenter(Point2 offset)
{
  DataList<Point1> coinRandomizer;
  coinRandomizer.add(10.0);  // penny
  coinRandomizer.add(3.5);  // nickel
  coinRandomizer.add(2.0);  // dime
  coinRandomizer.add(1.0);  // quarter
  // average: 4.5 cents per coin

  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaCoinStd* coin = new PizzaCoinStd();
    coin->setXY(currPointList[i] + offset);
    coin->value = COIN_PENNY + RM::random_index_weighted(coinRandomizer);
    addPGO(coin);
  }
}

void PhysicsLevelStd::createCoinsFromGroundPts()
{
  createCoinsFromCenter(Point2(0.0, -PIZZA_INNER_RADIUS));
}

void PhysicsLevelStd::createSkullsCentered(Point2 offset)
{
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    AngrySkull* skull = new AngrySkull();
    skull->setXY(currPointList[i] + offset);
    addPGO(skull);
  }
}

void PhysicsLevelStd::createSkullsGroundPts()
{
  createSkullsCentered(Point2(0.0, -ANGRY_SKULL_RAD));
}

void PhysicsLevelStd::createPuppiesGroundPts()
{
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    Billy* newPuppy = new Billy();
    newPuppy->setXY(currPointList[i] + Point2(0.0, -24.0));
    addPGO(newPuppy);
    
    PuppyCage* cage = new PuppyCage();
    cage->puppy = newPuppy;
    cage->setXY(currPointList[i]);
    addPGO(cage);
  }
}

void PhysicsLevelStd::createGemsFromCenter(Point2 offset)
{
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaCoinStd* gem = new PizzaCoinStd(currPointList[i] + offset,
                                         RM::randi(FIRST_GEM_TYPE, LAST_GEM_TYPE));
    addPGO(gem);
  }
}

void PhysicsLevelStd::createGemsFromGroundPts()
{
  createGemsFromCenter(Point2(0.0, -PIZZA_INNER_RADIUS));
}

void PhysicsLevelStd::createEnemiesFromBCLine(Coord1 subType)
{
  if (subType == ENEMY_NONE)
  {
    return;
  }
  
  for (Coord1 i = 0; i < currPointList.count; i += 3)
  {
    PizzaGOStd* enemy = createEnemy(subType);
    enemy->setXY(currPointList[i]);
    enemy->myPlatformLine.set(currPointList[i + 1], currPointList[i + 2]);
    addPGO(enemy);
  }
}

void PhysicsLevelStd::createThemedGroundFromCWPts()
{
  TerrainGround* ground = TerrainGround::createThemedGround(currPointList, backgroundType, true);
  ground->phase = PHASE_FG_TERRAIN;   ///////// REMOVE?
  
  addPGO(ground);
}

void PhysicsLevelStd::createChickensFromLines()
{
  DataList<Point2> lineList(2);
  
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    lineList.clear();
    lineList.add(currPointList[i]);
    lineList.add(currPointList[i + 1]);
    
    Chicken* chicken = new Chicken();
    chicken->setXY(lineList.first());
    chicken->pathMover.init(lineList, 1);
    chicken->pathMover.speed = 128.0;
    addPGO(chicken);
  }
}

void PhysicsLevelStd::createItemBlocksFromTL(Coord1 itemType)
{
  for (Coord1 i = 0; i < currPointList.count; i++)
  {
    TerrainQuad* itemBlock = TerrainQuad::createSurpriseBlock(currPointList[i], itemType);
    addPGO(itemBlock);
  }
}

void PhysicsLevelStd::createGourdoBlocksFromTL()
{
  createItemBlocksFromTL(GOURDO_ITEM);
}

void PhysicsLevelStd::createFadeBlocksFromTLandBR()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    TerrainQuad* landPiece = TerrainQuad::create_fading_redrock(
        currPointList[i], currPointList[i + 1] - currPointList[i]);
    addPGO(landPiece);
  }
}

void PhysicsLevelStd::createHalfSpikeBlocksFromTL_BR_side()
{
  for (Coord1 i = 0; i < currPointList.count; i += 3)
  {
    Box blockBox = Box::from_corners(currPointList[i], currPointList[i+1]);
    Point2 directionPt = currPointList[i+2];
    Coord1 spikeDir = 0;
    
    if (directionPt.y > blockBox.bottom())
    {
      spikeDir = 1;
    }
    else if (directionPt.x < blockBox.left())
    {
      spikeDir = 2;
    }
    else if (directionPt.y < blockBox.top())
    {
      spikeDir = 3;
    }
    
    TerrainQuad* landPiece = TerrainQuad::create_halfspike_TL(
         blockBox.xy, blockBox.wh, spikeDir);
    addPGO(landPiece);
  }

}

void PhysicsLevelStd::createBouncersFromCandRC()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    Point1 radius = currPointList[i + 1].x - currPointList[i].x;
    
    TerrainQuad* bouncer = TerrainQuad::create_static_circle(currPointList[i], radius, NULL);
    bouncer->setBouncy(0.75);
    bouncer->setImage(getImg(bouncerImg));
    bouncer->autoScale();
    addPGO(bouncer);
  }
}

void PhysicsLevelStd::createFlyerFromSingleLine(Coord1 flyerType)
{
  if (flyerType == ENEMY_NONE)
  {
    return;
  }
  
  PizzaGOStd* pgo = createEnemy(flyerType);
  pgo->setXY(currPointList.first());
  pgo->pathMover.init(currPointList, 1);
  pgo->pathMover.speed = 128.0;
  addPGO(pgo);
}

void PhysicsLevelStd::createFlyersFromLines(Coord1 flyerType)
{
  if (flyerType == ENEMY_NONE)
  {
    return;
  }
  
  DataList<Point2> currLine;
  
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    currLine.clear();
    currLine.add(currPointList[i]);
    currLine.add(currPointList[i + 1]);
    
    PizzaGOStd* pgo = createEnemy(flyerType);
    pgo->setXY(currLine.first());
    pgo->pathMover.init(currLine, 1);
    pgo->pathMover.speed = 128.0;

    addPGO(pgo);
  }
}

void PhysicsLevelStd::createSpikeCirclesFromCandRC()
{
  for (Coord1 i = 0; i < currPointList.count; i += 2)
  {
    Point1 radius = currPointList[i + 1].x - currPointList[i].x;
    addPGO(TerrainGround::createSpikeyCircle(Circle(currPointList[i], radius)));
  }
}

void PhysicsLevelStd::createSpikeGroundCW()
{
  addPGO(TerrainGround::createSpikeyGround(currPointList));
}

WaterArea* PhysicsLevelStd::createWaterFromCWQuads()
{
  DataList<Point2> currQuad;
  WaterArea* water = NULL;

  for (Coord1 i = 0; i < currPointList.count; i += 4)
  {
    currQuad.clear();
    currQuad.add(currPointList[i]);
    currQuad.add(currPointList[i+1]);
    currQuad.add(currPointList[i+2]);
    currQuad.add(currPointList[i+3]);

    water = new WaterArea(currQuad);
    addPGO(water);
  }
  
  return water;
}

void PhysicsLevelStd::createObjectiveFromGroundPt(Coord1 levelType)
{
  switch (levelType)
  {
    case LEVEL_TYPE_ANGRY:
      createSkullsGroundPts();
      break;
    case LEVEL_TYPE_FIREMAN:
      createBurningBlocksFromBC(112.0);
      break;
    case LEVEL_TYPE_PIRATE:
      createGemsFromGroundPts();
      break;
    case LEVEL_TYPE_PUPPY:
      createPuppiesGroundPts();
      break;
      
    default:
      createCoinsFromGroundPts();
      break;
  }
}

void PhysicsLevelStd::dipBallChunk(Logical useCage, Logical useBlocks, Logical useBurningBlocks,
                                   Logical useSkulls, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // left ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(200.0, -32.0);
  addLevelPt(368.0, 24.0);
  addLevelPt(368.0, 184.0);
  addLevelPt(0.0, 184.0);
  createThemedGroundFromCWPts();
  
  // middle ground
  clearPtList();
  addLevelPt(1000.0, 344.0);
  addLevelPt(1288.0, 248.0);
  addLevelPt(1736.0, 248.0);
  addLevelPt(2024.0, 344.0);
  addLevelPt(2024.0, 456.0);
  addLevelPt(1000.0, 456.0);
  createThemedGroundFromCWPts();

  // end ground
  clearPtList();
  addLevelPt(2656.0, 24.0);
  addLevelPt(2824.0, -24.0);
  addLevelPt(3024.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3024.0, 184.0);
  addLevelPt(2656.0, 184.0);
  createThemedGroundFromCWPts();

  // spike ball
  clearPtList();
  addLevelPt(680.0, 104.0);
  addLevelPt(768.0, 104.0);
  addLevelPt(2352.0, 104.0);
  addLevelPt(2440.0, 104.0);
  createSpikeCirclesFromCandRC();
  
  // coins
  clearPtList();
  addLevelPt(384.0, -192.0);
  addLevelPt(536.0, -280.0);
  addLevelPt(704.0, -328.0);
  addLevelPt(864.0, -288.0);
  addLevelPt(1016.0, -160.0);
  addLevelPt(1168.0, 48.0);
  addLevelPt(1856.0, 48.0);
  addLevelPt(2008.0, -160.0);
  addLevelPt(2160.0, -288.0);
  addLevelPt(2320.0, -328.0);
  addLevelPt(2488.0, -280.0);
  addLevelPt(2640.0, -192.0);
  createCoinsFromCenter();
  
  // camera ground points
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(200.0, -24.0);
  addLevelPt(368.0, 24.0);
  addLevelPt(680.0, 16.0);
  addLevelPt(1288.0, 248.0);
  addLevelPt(1736.0, 248.0);
  addLevelPt(2352.0, 16.0);
  addLevelPt(2824.0, -24.0);
  addLevelPt(3024.0, 0.0);
  addCameraGroundPts(currPointList);

  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1472.0, 216.0);
    addLevelPt(1552.0, 216.0);
    createSkullsCentered();
  }
  
  if (useBlocks == true)
  {
    clearPtList();
    addLevelPt(1320.0, 144.0);
    addLevelPt(104.0, 104.0);
    addLevelPt(1600.0, 144.0);
    addLevelPt(104.0, 104.0);
    createBlocksFromTLandWH();
  }
  
  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(1320.0, 144.0);
    addLevelPt(1424.0, 248.0);
    addLevelPt(1600.0, 144.0);
    addLevelPt(1704.0, 248.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1512.0, 248.0);
    createPuppiesGroundPts();
  }
  
  clearPtList();
  addLevelPt(1512.0, 248.0);
  addLevelPt(1288.0, 248.0);
  addLevelPt(1736.0, 248.0);
  createEnemiesFromBCLine(enemyType);
  
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::cutleryChunk(Coord1 levelType,
                                   Coord1 valleyEnemyType, Coord1 hillEnemyType)
{
  Point2 nextEndpoint;
  
  // ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(840.0, -10.0);
  addLevelPt(856.0, -210.0);
  addLevelPt(1168.0, -242.0);
  addLevelPt(1352.0, -58.0);
  addLevelPt(2056.0, -10.0);
  addLevelPt(2064.0, -186.0);
  addLevelPt(2328.0, -178.0);
  addLevelPt(2496.0, -10.0);
  addLevelPt(3024.0, -26.0);
  addLevelPt(3120.0, -242.0);
  addLevelPt(3640.0, -218.0);
  addLevelPt(3768.0, -18.0);
  addLevelPt(4400.0, -42.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(4403.0, 257.0);
  addLevelPt(0.0, 259.0);
  createThemedGroundFromCWPts();
  
  // coins
  clearPtList();
  addLevelPt(736.0, -262.0);
  addLevelPt(784.0, -320.0);
  addLevelPt(842.0, -360.0);
  addLevelPt(1192.0, -376.0);
  addLevelPt(1258.0, -336.0);
  addLevelPt(1324.0, -284.0);
  addLevelPt(1924.0, -270.0);
  addLevelPt(1970.0, -322.0);
  addLevelPt(2022.0, -370.0);
  addLevelPt(2184.0, -384.0);
  addLevelPt(2366.0, -340.0);
  addLevelPt(2434.0, -300.0);
  addLevelPt(2492.0, -250.0);
  addLevelPt(2990.0, -224.0);
  addLevelPt(3032.0, -290.0);
  addLevelPt(3082.0, -358.0);
  addLevelPt(3622.0, -364.0);
  addLevelPt(3684.0, -326.0);
  addLevelPt(3748.0, -270.0);
  createCoinsFromCenter();
  
  // create utensils
  clearPtList();
  addLevelPt(650.0, -10.0);  // enemy
  addLevelPt(228.0, -4.0);  // surface start
  addLevelPt(836.0, -14.0);  // surface end
  addLevelPt(1778.0, -28.0);  // enemy
  addLevelPt(1350.0, -56.0);  // surface start
  addLevelPt(2054.0, -12.0);  // surface end
  addLevelPt(2754.0, -20.0);  // enemy
  addLevelPt(2498.0, -12.0);  // surface start
  addLevelPt(3018.0, -28.0);  // surface end
  addLevelPt(4028.0, -28.0);  // enemy
  addLevelPt(3766.0, -18.0);  // surface start
  addLevelPt(4400.0, -38.0);  // surface end
  createEnemiesFromBCLine(hillEnemyType);
  
  // create donuts
  clearPtList();
  addLevelPt(1010.0, -228.0);  // enemy
  addLevelPt(858.0, -214.0);  // surface start
  addLevelPt(1170.0, -244.0);  // surface end
  addLevelPt(3426.0, -230.0);  // enemy
  addLevelPt(3118.0, -244.0);  // surface start
  addLevelPt(3642.0, -222.0);  // surface end
  createEnemiesFromBCLine(valleyEnemyType);
  
  clearPtList();
  addLevelPt(2192.0, -178.0);
  createObjectiveFromGroundPt(levelType);

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pitpitChunk(Logical useCage, Logical useSkull,
                                  Logical useBBlock, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 8.0);
  addLevelPt(424.0, 296.0);
  addLevelPt(1312.0, 392.0);
  addLevelPt(1856.0, 760.0);
  addLevelPt(3352.0, 952.0);
  addLevelPt(3504.0, 496.0);
  addLevelPt(3616.0, 392.0);
  addLevelPt(3976.0, 392.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3976.0, 1096.0);
  addLevelPt(0.0, 1096.0);
  createThemedGroundFromCWPts();

  // lava
  clearPtList();
  addLevelPt(408.0, 160.0);
  addLevelPt(1336.0, 272.0);
  addLevelPt(1896.0, 584.0);
  addLevelPt(3416.0, 784.0);
  addLevelPt(3368.0, 944.0);
  addLevelPt(1856.0, 760.0);
  addLevelPt(1312.0, 392.0);
  addLevelPt(416.0, 296.0);
  TerrainGround* lavaGround = TerrainGround::createThemedGround(currPointList, backgroundType, true);
  lavaGround->setLava();
  addPGO(lavaGround);

  // platform chunk 1
  clearPtList();
  addLevelPt(948.0, -58.0);
  addLevelPt(1172.0, -84.0);
  addLevelPt(1172.0, 56.0);
  addLevelPt(1022.0, 94.0);
  createThemedGroundFromCWPts();

  // platform chunk 2
  clearPtList();
  addLevelPt(1764.0, 158.0);
  addLevelPt(2018.0, 132.0);
  addLevelPt(2004.0, 282.0);
  addLevelPt(1848.0, 336.0);
  createThemedGroundFromCWPts();

  // platform chunk 3
  clearPtList();
  addLevelPt(2546.0, 412.0);
  addLevelPt(2902.0, 534.0);
  addLevelPt(2846.0, 664.0);
  addLevelPt(2626.0, 614.0);
  createThemedGroundFromCWPts();

  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 8.0);
  addLevelPt(408.0, 152.0);
  addLevelPt(1328.0, 272.0);
  addLevelPt(1888.0, 584.0);
  addLevelPt(3416.0, 776.0);
  addLevelPt(3504.0, 496.0);
  addLevelPt(3608.0, 392.0);
  addLevelPt(3976.0, 392.0);
  addCameraGroundPts(currPointList);

  // coins
  clearPtList();
  addLevelPt(534.0, -208.0);
  addLevelPt(602.0, -308.0);
  addLevelPt(740.0, -346.0);
  addLevelPt(856.0, -290.0);
  addLevelPt(916.0, -184.0);
  addLevelPt(1160.0, -190.0);
  addLevelPt(1230.0, -236.0);
  addLevelPt(1332.0, -260.0);
  addLevelPt(1438.0, -270.0);
  addLevelPt(1560.0, -258.0);
  addLevelPt(1638.0, -216.0);
  addLevelPt(1712.0, -158.0);
  addLevelPt(1772.0, -84.0);
  addLevelPt(1824.0, 22.0);
  addLevelPt(2002.0, 48.0);
  addLevelPt(2100.0, 6.0);
  addLevelPt(2218.0, -2.0);
  addLevelPt(2312.0, 0.0);
  addLevelPt(2416.0, 28.0);
  addLevelPt(2516.0, 72.0);
  addLevelPt(2592.0, 122.0);
  addLevelPt(2646.0, 210.0);
  addLevelPt(2682.0, 304.0);
  addLevelPt(2906.0, 450.0);
  addLevelPt(3042.0, 376.0);
  addLevelPt(3174.0, 346.0);
  addLevelPt(3306.0, 330.0);
  addLevelPt(3436.0, 330.0);
  addLevelPt(3572.0, 360.0);
  createCoinsFromCenter();
  
  // create cupids
  clearPtList();
  addLevelPt(1066.0, -72.0);  // enemy
  addLevelPt(950.0, -56.0);
  addLevelPt(1172.0, -84.0);
  addLevelPt(1896.0, 142.0);  // enemy
  addLevelPt(1764.0, 158.0);
  addLevelPt(2018.0, 128.0);
  addLevelPt(2728.0, 474.0);  // enemy
  addLevelPt(2546.0, 414.0);
  addLevelPt(2902.0, 534.0);
  addLevelPt(3790.0, 344.0);  // enemy
  addLevelPt(3612.0, 396.0);
  addLevelPt(3980.0, 396.0);
  createEnemiesFromBCLine(enemyType);
  
  // create chunk variations
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(3790.0, 396.0);
    createPuppiesGroundPts();
  }
  
  if (useSkull == true)
  {
    clearPtList();
    addLevelPt(3790.0, 396.0);
    createSkullsGroundPts();
  }
  
  if (useBBlock == true)
  {
    clearPtList();
    addLevelPt(3790.0, 396.0);
    addLevelPt(112.0, 112.0);
    createBurningBlocksFromBCandWH();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::maplebarsChunk(Coord1 levelType, Coord1 enemyType)
{
  Point2 nextEndpoint;

  // platform chunks
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(440.0, 48.0);
  addLevelPt(400.0, 824.0);
  addLevelPt(0.0, 824.0);

  addLevelPt(944.0, 312.0);
  addLevelPt(1288.0, 256.0);
  addLevelPt(1344.0, 1120.0);
  addLevelPt(976.0, 1160.0);

  addLevelPt(1721.0, -24.0);
  addLevelPt(2029.0, -40.0);
  addLevelPt(2117.0, 741.0);
  addLevelPt(1788.0, 744.0);

  addLevelPt(2755.0, 310.0);
  addLevelPt(3031.0, 259.0);
  addLevelPt(3054.0, 1241.0);
  addLevelPt(2704.0, 1176.0);

  addLevelPt(3751.0, 348.0);
  addLevelPt(3988.0, 309.0);
  addLevelPt(4055.0, 1088.0);
  addLevelPt(3795.0, 1173.0);

  addLevelPt(4618.0, 237.0);
  addLevelPt(5288.0, 232.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(5288.0, 1008.0);
  addLevelPt(4688.0, 1037.0);

  for (Coord1 i = 0; i < currPointList.count; i += 4)
  {
    TerrainGround* landPiece = TerrainGround::createThemedGround(currPointList,
        Coord2(i, i + 3), backgroundType);
    landPiece->setXY(currPointList[i]);
    addPGO(landPiece);
  }

  // blocks that break after a few seconds
  clearPtList();
  addLevelPt(480.0, 320.0);
  addLevelPt(608.0, 320.0);
  addLevelPt(736.0, 320.0);
  addLevelPt(1344.0, 96.0);
  addLevelPt(1472.0, -32.0);
  addLevelPt(2080.0, -64.0);
  addLevelPt(2208.0, 64.0);
  addLevelPt(2336.0, 64.0);
  addLevelPt(3168.0, 320.0);
  addLevelPt(3296.0, 320.0);
  addLevelPt(3424.0, 288.0);
  addLevelPt(4224.0, 192.0);
  addLevelPt(4224.0, 320.0);
  addLevelPt(4352.0, 256.0);
  
  for (Coord1 i = 0; i < currPointList.count; i++)
  {
    TerrainQuad* landPiece = TerrainQuad::create_fading_redrock(currPointList[i], Point2(128.0, 128.0));
    addPGO(landPiece);
  }
  
  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(480.0, 320.0);
  addLevelPt(864.0, 320.0);
  addLevelPt(1288.0, 256.0);
  addLevelPt(1720.0, -24.0);
  addLevelPt(2024.0, -40.0);
  addLevelPt(2752.0, 304.0);
  addLevelPt(3752.0, 344.0);
  addLevelPt(4616.0, 232.0);
  addLevelPt(5288.0, 232.0);
  addCameraGroundPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(951.0, 209.0);
  addLevelPt(1047.0, 193.0);
  addLevelPt(1145.0, 176.0);
  addLevelPt(1253.0, 158.0);
  addLevelPt(1741.0, -119.0);
  addLevelPt(1824.0, -126.0);
  addLevelPt(1915.0, -131.0);
  addLevelPt(2007.0, -132.0);
  addLevelPt(2759.0, 230.0);
  addLevelPt(2863.0, 204.0);
  addLevelPt(2985.0, 173.0);
  addLevelPt(3765.0, 259.0);
  addLevelPt(3854.0, 243.0);
  addLevelPt(3959.0, 224.0);
  addLevelPt(4679.0, -107.0);
  addLevelPt(4797.0, -77.0);
  addLevelPt(4898.0, -35.0);
  addLevelPt(4996.0, 7.0);
  addLevelPt(5091.0, 52.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaCoinStd* coin = new PizzaCoinStd();
    coin->setXY(currPointList[i]);
    addPGO(coin);
  }
  
  // create chips
  clearPtList();
  addLevelPt(1203.0, 268.0);  // enemy
  addLevelPt(939.0, 307.0);
  addLevelPt(1290.0, 256.0);
  addLevelPt(1992.0, -38.0);  // enemy
  addLevelPt(1719.0, -27.0);
  addLevelPt(2030.0, -41.0);
  addLevelPt(1879.0, -146.0);  // enemy
  addLevelPt(1718.0, -27.0);
  addLevelPt(2030.0, -40.0);
  addLevelPt(2878.0, 285.0);  // enemy
  addLevelPt(2753.0, 308.0);
  addLevelPt(3032.0, 259.0);
  addLevelPt(3936.0, 316.0);  // enemy
  addLevelPt(3751.0, 345.0);
  addLevelPt(3988.0, 307.0);
  addLevelPt(3832.0, 188.0);  // enemy
  addLevelPt(3749.0, 346.0);
  addLevelPt(3987.0, 309.0);
  addLevelPt(4940.0, 232.0);  // enemy
  addLevelPt(4616.0, 234.0);
  addLevelPt(5287.0, 235.0);
  addLevelPt(4743.0, 133.0);  // enemy
  addLevelPt(4616.0, 234.0);
  addLevelPt(5290.0, 236.0);
  addLevelPt(5044.0, 116.0);  // enemy
  addLevelPt(4615.0, 236.0);
  addLevelPt(5288.0, 237.0);
  createEnemiesFromBCLine(enemyType);
  
  clearPtList();
  addLevelPt(5099.0, 235.0);
  createObjectiveFromGroundPt(levelType);

  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::spearpitChunk(Logical useCage, Logical useSkulls,
                                    Logical useBurners, Coord1 topEnemy)
{
  Point2 nextEndpoint;
  
  // platform chunks
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(544.0, 0.0);
  addLevelPt(544.0, 416.0);
  addLevelPt(2752.0, 416.0);
  addLevelPt(2752.0, 0.0);
  addLevelPt(3200.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(3200.0, 528.0);
  addLevelPt(0.0, 528.0);
  createThemedGroundFromCWPts();
  
  // moving platform
  clearPtList();
  addLevelPt(660.0, -8.0);
  addLevelPt(852.0, -8.0);
  addLevelPt(852.0, 56.0);
  addLevelPt(662.0, 56.0);
  TerrainQuad* mover = TerrainQuad::create_static_quad(currPointList[0], currPointList[1],
                                                       currPointList[2], currPointList[3]);
  mover->setImage(getImg(moverPlatform192Img));
  addPGO(mover);
  
  clearPtList();
  addLevelPt(664.0, 0.0);  // start
  addLevelPt(2480.0, 0.0);  // end

  mover->pathPoints = currPointList;
  mover->enablePathMove();
  
  // bomb bat
  clearPtList();
  addLevelPt(1592.0, -8.0);
  addLevelPt(1304.0, -8.0);
  addLevelPt(1832.0, -8.0);
  createEnemiesFromBCLine(topEnemy);

  // coins
  clearPtList();
  addLevelPt(768.0, -224.0);
  addLevelPt(896.0, -224.0);
  addLevelPt(1024.0, -224.0);
  addLevelPt(1152.0, -224.0);
  addLevelPt(1280.0, -224.0);
  addLevelPt(1408.0, -224.0);
  addLevelPt(1536.0, -224.0);
  addLevelPt(1664.0, -224.0);
  addLevelPt(1792.0, -224.0);
  addLevelPt(1920.0, -224.0);
  addLevelPt(2048.0, -224.0);
  addLevelPt(2176.0, -224.0);
  addLevelPt(2304.0, -224.0);
  addLevelPt(2432.0, -224.0);
  addLevelPt(2560.0, -224.0);
  addLevelPt(2688.0, -224.0);
  createCoinsFromCenter();
  
  // create spearmen
  clearPtList();
  addLevelPt(544.0, 416.0);  // ground start
  addLevelPt(2752.0, 416.0);  // ground end
  addLevelPt(648.0, 416.0);  // start enemies
  addLevelPt(816.0, 416.0);
  addLevelPt(976.0, 416.0);
  addLevelPt(1160.0, 416.0);
  addLevelPt(1328.0, 416.0);
  addLevelPt(1512.0, 416.0);
  addLevelPt(1688.0, 416.0);
  addLevelPt(1864.0, 416.0);
  addLevelPt(2048.0, 416.0);
  addLevelPt(2232.0, 416.0);
  addLevelPt(2408.0, 416.0);
  addLevelPt(2592.0, 416.0);
  
  for (Coord1 i = 2; i < currPointList.count; i++)
  {
    SkeletonSpearman* enemy = new SkeletonSpearman(SkeletonSpearman::VAL_VERTICAL);
    enemy->setXY(currPointList[i]);
    enemy->myPlatformLine.set(currPointList[0], currPointList[1]);
    
    if (i % 2 == 0)
    {
      enemy->walkSpeed = 0.0;
    }
    
    addPGO(enemy);
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(2992.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useBurners == true)
  {
    clearPtList();
    addLevelPt(2992.0, 0.0);
    addLevelPt(112.0, 112.0);
    createBurningBlocksFromBCandWH();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(230.0, -31.0);
    addLevelPt(311.0, -31.0);
    createSkullsCentered();
    
    clearPtList();
    addLevelPt(78.0, -104.0);
    addLevelPt(358.0, 104.0);
    createBlocksFromTLandWH();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::spikeyMountainChunk(Coord1 levelType, Coord1 groundEnemyType, Coord1 flyerType)
{
  Point2 nextEndpoint;
  
  // terrain ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(544.0, 0.0);
  addLevelPt(586.0, -195.0);
  addLevelPt(1037.0, -217.0);
  addLevelPt(1091.0, -406.0);
  addLevelPt(1647.0, -368.0);
  addLevelPt(1601.0, -562.0);
  addLevelPt(1972.0, -609.0);
  addLevelPt(2072.0, -823.0);
  addLevelPt(2574.0, -798.0);
  addLevelPt(3200.0, -798.0);
//  addLevelPt(3200.0, -655.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3200.0, -526.0);
  addLevelPt(2570.0, -665.0);
  addLevelPt(2174.0, -549.0);
  addLevelPt(2064.0, -405.0);
  addLevelPt(1890.0, -411.0);
  addLevelPt(1815.0, -203.0);
  addLevelPt(1295.0, -236.0);
  addLevelPt(1150.0, -57.0);
  addLevelPt(802.0, -11.0);
  addLevelPt(792.0, 120.0);
  addLevelPt(0.0, 120.0);
  createThemedGroundFromCWPts();
  
  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(328.0, 0.0);
  addLevelPt(584.0, -192.0);
  addLevelPt(1096.0, -408.0);
  addLevelPt(1600.0, -560.0);
  addLevelPt(2072.0, -824.0);
  addLevelPt(2568.0, -800.0);
  addLevelPt(3200.0, -656.0);
  addCameraGroundPts(currPointList);
  
  // bomb bat
  clearPtList();
  addLevelPt(2488.0, -920.0);
  addLevelPt(2080.0, -824.0);
  addLevelPt(2568.0, -800.0);
  createEnemiesFromBCLine(flyerType);
  
  // coins
  clearPtList();
  addLevelPt(240.0, -272.0);
  addLevelPt(336.0, -367.0);
  addLevelPt(451.0, -422.0);
  addLevelPt(578.0, -455.0);
  addLevelPt(966.0, -529.0);
  addLevelPt(1062.0, -625.0);
  addLevelPt(1177.0, -681.0);
  addLevelPt(1306.0, -713.0);
  addLevelPt(1490.0, -711.0);
  addLevelPt(1583.0, -807.0);
  addLevelPt(1701.0, -861.0);
  addLevelPt(1827.0, -895.0);
  addLevelPt(2338.0, -1022.0);
  addLevelPt(2336.0, -1104.0);
  addLevelPt(2336.0, -1183.0);
  addLevelPt(2646.0, -1010.0);
  addLevelPt(2776.0, -1004.0);
  addLevelPt(2900.0, -973.0);
  addLevelPt(3013.0, -897.0);
  createCoinsFromCenter();
  
  // create spiny
  clearPtList();
  addLevelPt(779.0, -205.0);
  addLevelPt(586.0, -195.0);
  addLevelPt(1036.0, -217.0);
  addLevelPt(1370.0, -386.0);
  addLevelPt(1091.0, -406.0);
  addLevelPt(1588.0, -371.0);
  addLevelPt(1796.0, -588.0);
  addLevelPt(1601.0, -561.0);
  addLevelPt(1973.0, -609.0);
  addLevelPt(2171.0, -819.0);
  addLevelPt(2073.0, -824.0);
  addLevelPt(2295.0, -813.0);
  createEnemiesFromBCLine(groundEnemyType);

  // objective
  clearPtList();
  addLevelPt(2349.0, -805.0);
  createObjectiveFromGroundPt(levelType);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::puppyMoat(Logical useCage, Logical useGems,
                                Coord1 waterEnemy, Logical useBBlocks,
                                Coord1 standingEnemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(640.0, 384.0);
  addLevelPt(1024.0, 384.0);
  addLevelPt(1280.0, 0.0);
  addLevelPt(1600.0, 0.0);
  addLevelPt(1856.0, 384.0);
  addLevelPt(2240.0, 384.0);
  addLevelPt(2496.0, 0.0);
  addLevelPt(2880.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(2880.0, 96.0);
  addLevelPt(2504.0, 96.0);
  addLevelPt(2240.0, 480.0);
  addLevelPt(1856.0, 480.0);
  addLevelPt(1576.0, 88.0);
  addLevelPt(1288.0, 88.0);
  addLevelPt(1024.0, 464.0);
  addLevelPt(640.0, 464.0);
  addLevelPt(384.0, 96.0);
  addLevelPt(0.0, 96.0);
  createThemedGroundFromCWPts();
  
  // coins
  clearPtList();
  addLevelPt(640.0, 128.0);
  addLevelPt(776.0, 8.0);
  addLevelPt(896.0, 8.0);
  addLevelPt(1024.0, 128.0);
  addLevelPt(1856.0, 128.0);
  addLevelPt(1984.0, 8.0);
  addLevelPt(2112.0, 8.0);
  addLevelPt(2240.0, 128.0);
  createCoinsFromCenter();
  
  // waters
  clearPtList();
  addLevelPt(424.0, 64.0);
  addLevelPt(1240.0, 64.0);
  addLevelPt(1024.0, 384.0);
  addLevelPt(640.0, 384.0);
  addLevelPt(1640.0, 64.0);
  addLevelPt(2456.0, 64.0);
  addLevelPt(2240.0, 384.0);
  addLevelPt(1856.0, 384.0);
  createWaterFromCWQuads();

  // water spines 1
  clearPtList();
  addLevelPt(640.0, 256.0);
  addLevelPt(1024.0, 256.0);
  addLevelPt(1024.0, 384.0);
  addLevelPt(640.0, 384.0);
  createSpikeBlocksFromQuads();
  
  // water spines 2
  clearPtList();
  addLevelPt(1856.0, 256.0);
  addLevelPt(2240.0, 256.0);
  addLevelPt(2240.0, 384.0);
  addLevelPt(1856.0, 384.0);
  createSpikeBlocksFromQuads();

  if (useGems == true)
  {
    clearPtList();
    addLevelPt(832.0, 128.0);
    addLevelPt(2048.0, 128.0);
    createGemsFromCenter();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1440.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useBBlocks == true)
  {
    clearPtList();
    addLevelPt(1440.0, 0.0);
    addLevelPt(112.0, 112.0);
    createBurningBlocksFromBCandWH();
  }
  
  // enemy over water
  clearPtList();
  addLevelPt(656.0, 64.0);
  addLevelPt(424.0, 64.0);
  addLevelPt(848.0, 64.0);
  addLevelPt(1008.0, 64.0);
  addLevelPt(848.0, 64.0);
  addLevelPt(1232.0, 64.0);
  addLevelPt(1808.0, 64.0);
  addLevelPt(1640.0, 64.0);
  addLevelPt(2008.0, 64.0);
  addLevelPt(2192.0, 64.0);
  addLevelPt(2008.0, 64.0);
  addLevelPt(2456.0, 64.0);
  createEnemiesFromBCLine(waterEnemy);

  // standing enemy
  clearPtList();
  addLevelPt(2688.0, 0.0);
  addLevelPt(2496.0, 0.0);
  addLevelPt(2880.0, 0.0);
  createEnemiesFromBCLine(standingEnemyType);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::puppySpikeFlight(Coord1 levelType, Logical useRightItem, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(768.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1728.0, -896.0);
  addLevelPt(1920.0, -896.0);
  addLevelPt(1920.0, -704.0);
  addLevelPt(1728.0, -704.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 3
  clearPtList();
  addLevelPt(2816.0, 0.0);
  addLevelPt(4032.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(4032.0, 192.0);
  addLevelPt(2816.0, 192.0);
  createThemedGroundFromCWPts();
  
  // ceiling
  clearPtList();
  addLevelPt(384.0, -1632.0);
  addLevelPt(3584.0, -1632.0);
  addLevelPt(3584.0, -1472.0);
  addLevelPt(384.0, -1472.0);
  createThemedGroundFromCWPts();
  
  // camera bot
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(1264.0, -144.0);
  addLevelPt(1656.0, -744.0);
  addLevelPt(2016.0, -736.0);
  addLevelPt(2432.0, -328.0);
  addLevelPt(2816.0, 0.0);
  addLevelPt(4032.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(80.0, 0.0);
  addLevelPt(384.0, -280.0);
  addLevelPt(648.0, -1056.0);
  addLevelPt(1096.0, -1344.0);
  addLevelPt(1400.0, -1472.0);
  addLevelPt(2432.0, -1472.0);
  addLevelPt(3144.0, -1056.0);
  addLevelPt(3480.0, -392.0);
  addCameraCeilingPts(currPointList);

  // coins
  clearPtList();
  addLevelPt(800.0, -448.0);
  addLevelPt(1008.0, -432.0);
  addLevelPt(816.0, -592.0);
  addLevelPt(720.0, -768.0);
  addLevelPt(848.0, -800.0);
  addLevelPt(936.0, -656.0);
  addLevelPt(1072.0, -736.0);
  addLevelPt(1200.0, -656.0);
  addLevelPt(1168.0, -848.0);
  addLevelPt(1072.0, -984.0);
  addLevelPt(1264.0, -968.0);
  addLevelPt(1384.0, -864.0);
  addLevelPt(1384.0, -1064.0);
  addLevelPt(1536.0, -1032.0);
  addLevelPt(1416.0, -1184.0);
  addLevelPt(1600.0, -1200.0);
  addLevelPt(1680.0, -1272.0);
  addLevelPt(1840.0, -1144.0);
  addLevelPt(2000.0, -1216.0);
  addLevelPt(2064.0, -984.0);
  addLevelPt(2216.0, -1064.0);
  addLevelPt(2272.0, -1240.0);
  addLevelPt(2352.0, -864.0);
  addLevelPt(2448.0, -1064.0);
  addLevelPt(2632.0, -1048.0);
  addLevelPt(2672.0, -1152.0);
  addLevelPt(2272.0, -664.0);
  addLevelPt(2512.0, -720.0);
  addLevelPt(2608.0, -672.0);
  addLevelPt(2736.0, -816.0);
  addLevelPt(2904.0, -832.0);
  addLevelPt(2704.0, -624.0);
  addLevelPt(2768.0, -512.0);
  addLevelPt(2936.0, -632.0);
  addLevelPt(3040.0, -656.0);
  addLevelPt(2968.0, -448.0);
  createCoinsFromCenter();

  // gourdos
  clearPtList();
  addLevelPt(384.0, -288.0);
  
  if (useRightItem == true)
  {
    addLevelPt(3360.0, -392.0);
  }
  
  createGourdoBlocksFromTL();

  // spikes
  clearPtList();
  addLevelPt(1217.0, -195.0);
  addLevelPt(1279.0, -198.0);
  addLevelPt(1663.0, -797.0);
  addLevelPt(1729.0, -797.0);
  addLevelPt(703.0, -1000.0);
  addLevelPt(766.0, -996.0);
  addLevelPt(1154.0, -1280.0);
  addLevelPt(1216.0, -1279.0);
  addLevelPt(2018.0, -798.0);
  addLevelPt(2081.0, -799.0);
  addLevelPt(2438.0, -381.0);
  addLevelPt(2494.0, -380.0);
  addLevelPt(3090.0, -998.0);
  addLevelPt(3153.0, -998.0);
  createSpikeCirclesFromCandRC();
  
  // objective
  clearPtList();
  addLevelPt(1824.0, -896.0);
  createObjectiveFromGroundPt(levelType);

  // enemy
  clearPtList();
  addLevelPt(3032.0, 0.0);
  addLevelPt(2808.0, 0.0);
  addLevelPt(3224.0, 0.0);
  addLevelPt(3464.0, 0.0);
  addLevelPt(3224.0, 0.0);
  addLevelPt(3648.0, 0.0);
  addLevelPt(3856.0, 0.0);
  addLevelPt(3648.0, 0.0);
  addLevelPt(4016.0, 0.0);
  createEnemiesFromBCLine(enemyType);

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::puppySpinnerSpan(Logical useCage, Logical useGems,
                                       Logical useSkull, Logical useBBlocks, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(768.0, 256.0);
  addLevelPt(0.0, 256.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(2432.0, 0.0);
  addLevelPt(2816.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2816.0, 256.0);
  addLevelPt(2176.0, 256.0);
  createThemedGroundFromCWPts();
  
  // camera bot
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(832.0, 320.0);
  addLevelPt(2112.0, 320.0);
  addLevelPt(2432.0, 0.0);
  addLevelPt(2816.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(552.0, -456.0);
  addLevelPt(968.0, -536.0);
  addLevelPt(2112.0, -536.0);
  addLevelPt(2288.0, -464.0);
  addCameraCeilingPts(currPointList);

  // spinners
  clearPtList();
  addLevelPt(832.0, 256.0);
  addLevelPt(1152.0, 320.0);
  addLevelPt(1152.0, 256.0);
  addLevelPt(1488.0, 320.0);
  addLevelPt(1488.0, 256.0);
  addLevelPt(1824.0, 320.0);
  addLevelPt(1824.0, 256.0);
  addLevelPt(2112.0, 320.0);
  createSpinnersFromTLandBR();
  
  // coins
  clearPtList();
  addLevelPt(1024.0, -72.0);
  addLevelPt(1280.0, -72.0);
  addLevelPt(1536.0, -72.0);
  addLevelPt(1792.0, -72.0);
  addLevelPt(2048.0, -72.0);
  createCoinsFromCenter();
  
  // coin blocks
  clearPtList();
  addLevelPt(1472.0, -384.0);
  addLevelPt(1216.0, -384.0);
  createItemBlocksFromTL(RANDOM_COIN);

  // conditional item blocks
  clearPtList();
  addLevelPt(960.0, -384.0);
  addLevelPt(1728.0, -384.0);
  addLevelPt(1984.0, -384.0);

  if (useGems == true)
  {
    createItemBlocksFromTL(RANDOM_GEM);
  }
  else
  {
    createItemBlocksFromTL(RANDOM_COIN);
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(2608.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useSkull == true)
  {
    clearPtList();
    addLevelPt(2608.0, 0.0);
    createSkullsGroundPts();
  }
  
  if (useBBlocks == true)
  {
    clearPtList();
    addLevelPt(2608.0, 0.0);
    addLevelPt(64.0, 64.0);
    createBurningBlocksFromBCandWH();
  }
  
  if (enemyType != ENEMY_NONE)
  {
    clearPtList();
    addLevelPt(264.0, 0.0);
    addLevelPt(88.0, 0.0);
    addLevelPt(456.0, 0.0);
    addLevelPt(2616.0, 0.0);
    addLevelPt(2432.0, 0.0);
    addLevelPt(2776.0, 0.0);
    createEnemiesFromBCLine(enemyType);
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angryBouncePyramid(Logical useSkulls, Coord1 flyerType)
{
  Point2 nextEndpoint;
  
  cout << "bounce pyramid" << endl;
  
  // platform chunks
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(536.0, 0.0);
  addLevelPt(536.0, 528.0);
  addLevelPt(0.0, 528.0);
  createThemedGroundFromCWPts();

  // chunk 2
  clearPtList();
  addLevelPt(1840.0, -600.0);
  addLevelPt(2440.0, -600.0);
  addLevelPt(2440.0, -392.0);
  addLevelPt(1840.0, -392.0);
  createThemedGroundFromCWPts();

  // chunk 3
  clearPtList();
  addLevelPt(4416.0, -1264.0);
  addLevelPt(5024.0, -1264.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(5024.0, -712.0);
  addLevelPt(4416.0, -712.0);
  createThemedGroundFromCWPts();
  
  // bouncers
  clearPtList();
  addLevelPt(1240.0, -211.0);
  addLevelPt(3232.0, -738.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    TerrainQuad* bouncer = TerrainQuad::create_static_circle(currPointList[i], 112.0, NULL);
    bouncer->setBouncy(0.75);
    bouncer->setImage(getImg(bouncerImg));
    bouncer->autoScale();
    addPGO(bouncer);
  }
  
  // bottom camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(536.0, -4.0);
  addLevelPt(1236.0, -304.0);
  addLevelPt(1836.0, -604.0);
  addLevelPt(2444.0, -604.0);
  addLevelPt(3228.0, -828.0);
  addLevelPt(4412.0, -1264.0);
  addLevelPt(5024.0, -1264.0);
  addCameraGroundPts(currPointList);
  
  // top camera
  clearPtList();
  addLevelPt(244.0, 4.0);
  addLevelPt(656.0, -616.0);
  addLevelPt(1060.0, -676.0);
  addLevelPt(1440.0, -936.0);
  addLevelPt(1572.0, -992.0);
  addLevelPt(1968.0, -700.0);
  addLevelPt(2308.0, -704.0);
  addLevelPt(2636.0, -1100.0);
  addLevelPt(2940.0, -1228.0);
  addLevelPt(3952.0, -1648.0);
  addCameraCeilingPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(509.0, -245.0);
  addLevelPt(540.0, -378.0);
  addLevelPt(589.0, -503.0);
  addLevelPt(659.0, -602.0);
  addLevelPt(781.0, -557.0);
  addLevelPt(828.0, -439.0);
  addLevelPt(861.0, -535.0);
  addLevelPt(940.0, -647.0);
  addLevelPt(1066.0, -663.0);
  addLevelPt(1173.0, -585.0);
  addLevelPt(1231.0, -455.0);
  addLevelPt(1270.0, -578.0);
  addLevelPt(1302.0, -708.0);
  addLevelPt(1364.0, -826.0);
  addLevelPt(1448.0, -921.0);
  addLevelPt(1573.0, -978.0);
  addLevelPt(1706.0, -946.0);
  addLevelPt(1799.0, -857.0);
  addLevelPt(1883.0, -753.0);
  addLevelPt(2431.0, -770.0);
  addLevelPt(2468.0, -906.0);
  addLevelPt(2527.0, -1021.0);
  addLevelPt(2641.0, -1089.0);
  addLevelPt(2757.0, -1004.0);
  addLevelPt(2837.0, -1139.0);
  addLevelPt(2946.0, -1216.0);
  addLevelPt(3072.0, -1185.0);
  addLevelPt(3161.0, -1085.0);
  addLevelPt(3240.0, -945.0);
  addLevelPt(3284.0, -1072.0);
  addLevelPt(3328.0, -1200.0);
  addLevelPt(3385.0, -1319.0);
  addLevelPt(3468.0, -1423.0);
  addLevelPt(3574.0, -1505.0);
  addLevelPt(3693.0, -1574.0);
  addLevelPt(3821.0, -1614.0);
  addLevelPt(3954.0, -1636.0);
  addLevelPt(4089.0, -1631.0);
  addLevelPt(4218.0, -1603.0);
  addLevelPt(4334.0, -1538.0);
  addLevelPt(4440.0, -1446.0);
  createCoinsFromCenter();
  
  // create bats
  clearPtList();
  addLevelPt(840.0, -176.0);
  addLevelPt(672.0, -120.0);
  addLevelPt(1032.0, -240.0);
  addLevelPt(2768.0, -728.0);
  addLevelPt(2616.0, -688.0);
  addLevelPt(2904.0, -768.0);
  createEnemiesFromBCLine(flyerType);
  
  // create destructible blocks
  clearPtList();
  addLevelPt(1968.0, -704.0);
  addLevelPt(104.0, 104.0);
  addLevelPt(2208.0, -704.0);
  addLevelPt(104.0, 104.0);
  addLevelPt(4556.0, -1368.0);
  addLevelPt(104.0, 104.0);
  addLevelPt(4796.0, -1368.0);
  addLevelPt(104.0, 104.0);
  addLevelPt(4520.0, -1472.0);
  addLevelPt(414.0, 104.0);
  createBlocksFromTLandWH();
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(2023.0, -738.0);
    addLevelPt(2105.0, -634.0);
    addLevelPt(4522.0, -1297.0);
    addLevelPt(4760.0, -1297.0);
    addLevelPt(4765.0, -1505.0);
    addLevelPt(4847.0, -1505.0);
    createSkullsCentered();
  }

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angryMagicPyramid(Logical useSkulls, Coord1 lineFlyerType, Coord1 groundType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1024.0, 0.0);
  addLevelPt(1024.0, 512.0);
  addLevelPt(0.0, 512.0);
  createThemedGroundFromCWPts();

  // terrain ground 2
  clearPtList();
  addLevelPt(2816.0, -992.0);
  addLevelPt(3368.0, -992.0);
  addLevelPt(3576.0, -1148.0);
  addLevelPt(3928.0, -1148.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3928.0, -348.0);
  addLevelPt(2816.0, -348.0);
  createThemedGroundFromCWPts();
  
  // magic block top
  clearPtList();
  addLevelPt(732.0, -68.0);
  addLevelPt(804.0, -68.0);
  addLevelPt(824.0, 0.0);
  addLevelPt(712.0, 0.0);
  TerrainQuad* magicTrigger = TerrainQuad::create_static_quad(currPointList[0], currPointList[1],
                                                              currPointList[2], currPointList[3]);
  magicTrigger->enableMagicTrigger(10.0);
  addPGO(magicTrigger);

  // magic platform chunks
  clearPtList();
  addLevelPt(1088.0, -288.0);
  addLevelPt(1216.0, -288.0);
  addLevelPt(1216.0, -160.0);
  addLevelPt(1088.0, -160.0);

  addLevelPt(1664.0, -544.0);
  addLevelPt(1792.0, -544.0);
  addLevelPt(1792.0, -416.0);
  addLevelPt(1664.0, -416.0);

  addLevelPt(2240.0, -768.0);
  addLevelPt(2368.0, -768.0);
  addLevelPt(2368.0, -640.0);
  addLevelPt(2240.0, -644.0);

  for (Coord1 i = 0; i < currPointList.count; i += 4)
  {
    TerrainGround* landPiece = TerrainGround::createThemedGround(currPointList,
        Coord2(i, i + 3), backgroundType);
    landPiece->setXY(currPointList[i]);
    landPiece->magicTriggerOff();
    magicTrigger->magicObjectList.add(landPiece);
    addPGO(landPiece);
  }
  
  // breakable blocks
  clearPtList();
  addLevelPt(2888.0, -1440.0);
  addLevelPt(3008.0, -1376.0);
  addLevelPt(2848.0, -1376.0);
  addLevelPt(3200.0, -1248.0);
  addLevelPt(2880.0, -1248.0);
  addLevelPt(3008.0, -992.0);
  addLevelPt(3136.0, -1248.0);
  addLevelPt(3264.0, -1120.0);
  addLevelPt(3168.0, -1120.0);
  addLevelPt(3296.0, -992.0);
  createBlocksFromTLandBR();

  // bot camera
  clearPtList();
  addLevelPt(704.0, -48.0);
  addLevelPt(1088.0, -288.0);
  addLevelPt(1216.0, -288.0);
  addLevelPt(1664.0, -544.0);
  addLevelPt(1792.0, -544.0);
  addLevelPt(2240.0, -768.0);
  addLevelPt(2368.0, -768.0);
  addLevelPt(2816.0, -992.0);
  addLevelPt(3368.0, -992.0);
  addLevelPt(3576.0, -1148.0);
  addLevelPt(3928.0, -1148.0);
  addCameraGroundPts(currPointList);
  
  // top camera
  clearPtList();
  addLevelPt(848.0, -496.0);
  addLevelPt(1352.0, -776.0);
  addLevelPt(1464.0, -816.0);
  addLevelPt(1904.0, -1056.0);
  addLevelPt(2008.0, -1096.0);
  addLevelPt(2480.0, -1272.0);
  addLevelPt(2592.0, -1312.0);
  addLevelPt(2952.0, -1504.0);
  addCameraCeilingPts(currPointList);

  // coins
  clearPtList();
  addLevelPt(774.0, -217.0);
  addLevelPt(800.0, -358.0);
  addLevelPt(848.0, -482.0);
  addLevelPt(956.0, -522.0);
  addLevelPt(1043.0, -423.0);
  addLevelPt(1106.0, -331.0);
  addLevelPt(1152.0, -330.0);
  addLevelPt(1197.0, -331.0);
  addLevelPt(1282.0, -493.0);
  addLevelPt(1307.0, -633.0);
  addLevelPt(1355.0, -757.0);
  addLevelPt(1463.0, -798.0);
  addLevelPt(1551.0, -699.0);
  addLevelPt(1681.0, -575.0);
  addLevelPt(1727.0, -575.0);
  addLevelPt(1772.0, -576.0);
  addLevelPt(1831.0, -772.0);
  addLevelPt(1855.0, -912.0);
  addLevelPt(1904.0, -1037.0);
  addLevelPt(2012.0, -1077.0);
  addLevelPt(2100.0, -978.0);
  addLevelPt(2257.0, -798.0);
  addLevelPt(2304.0, -798.0);
  addLevelPt(2349.0, -798.0);
  addLevelPt(2410.0, -989.0);
  addLevelPt(2435.0, -1129.0);
  addLevelPt(2483.0, -1254.0);
  addLevelPt(2591.0, -1294.0);
  addLevelPt(2680.0, -1196.0);
  createCoinsFromCenter();
  
  // physical coins
  /*
  clearPtList();
  addLevelPt(1120.0, -1551.0);
  addLevelPt(1184.0, -1551.0);
  addLevelPt(1312.0, -1551.0);
  addLevelPt(1376.0, -1551.0);
  addLevelPt(1504.0, -1552.0);
  addLevelPt(1569.0, -1551.0);
  addLevelPt(1696.0, -1551.0);
  addLevelPt(1792.0, -1551.0);
  addLevelPt(1888.0, -1551.0);
  addLevelPt(2016.0, -1551.0);
  addLevelPt(2080.0, -1551.0);
  addLevelPt(2208.0, -1551.0);
  addLevelPt(2272.0, -1551.0);
  addLevelPt(2400.0, -1551.0);
  addLevelPt(2464.0, -1551.0);
  addLevelPt(1792.0, -911.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaCoinStd* coin = new PizzaCoinStd();
    coin->usePhysics = true;
    coin->setXY(currPointList[i]);
    addPGO(coin);
  }
   */
  
  // create spikey
  clearPtList();
  addLevelPt(3716.0, -1148.0);
  addLevelPt(3576.0, -1148.0);
  addLevelPt(3928.0, -1148.0);
  createEnemiesFromBCLine(groundType);
  
  // flameskull 1
  clearPtList();
  addLevelPt(1448.0, -560.0);
  addLevelPt(1392.0, -408.0);
  addLevelPt(1528.0, -480.0);
  createFlyerFromSingleLine(lineFlyerType);
  
  // flameskull 2
  clearPtList();
  addLevelPt(2008.0, -832.0);
  addLevelPt(1968.0, -624.0);
  addLevelPt(2072.0, -672.0);
  createFlyerFromSingleLine(lineFlyerType);

  // flameskull 3
  clearPtList();
  addLevelPt(2576.0, -1104.0);
  addLevelPt(2528.0, -888.0);
  addLevelPt(2664.0, -952.0);
  createFlyerFromSingleLine(lineFlyerType);

  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(3072.0, -1024.0);
    addLevelPt(3136.0, -1024.0);
    addLevelPt(3232.0, -1280.0);
    addLevelPt(3072.0, -1408.0);
    createSkullsCentered();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angrySkyhenge(Logical useSkulls, Logical useCages)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(512.0, 256.0);
  addLevelPt(512.0, 640.0);
  addLevelPt(0.0, 640.0);
  createThemedGroundFromCWPts();
  
  // skyhenge base
  clearPtList();
  addLevelPt(2176.0, -224.0);
  addLevelPt(3520.0, -224.0);
  addLevelPt(3520.0, -64.0);
  addLevelPt(2176.0, -64.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(5504.0, 0.0);
  addLevelPt(5756.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(5756.0, 640.0);
  addLevelPt(5246.0, 640.0);
  addLevelPt(5246.0, 254.0);
  createThemedGroundFromCWPts();
  
  // moving platform
  clearPtList();
  addLevelPt(508.0, 64.0);
  addLevelPt(508.0, 128.0);
  addLevelPt(892.0, 128.0);
  addLevelPt(892.0, 64.0);
  TerrainQuad* mover = TerrainQuad::create_static_quad(currPointList[0], currPointList[1],
                                                       currPointList[2], currPointList[3]);
  mover->setImage(getImg(moverPlatform384Img));
  addPGO(mover);
  
  clearPtList();
  addLevelPt(508.0, 64.0);  // start
  addLevelPt(4900.0, 0.0);  // end
  
  mover->pathPoints = currPointList;
  mover->enablePathMove();
  
  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(508.0, 64.0);
  addLevelPt(5436.0, 64.0);
  addLevelPt(5500.0, 0.0);
  addLevelPt(5756.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // breakable blocks
  clearPtList();
  addLevelPt(2176.0, -352.0);
  addLevelPt(2304.0, -224.0);
  addLevelPt(2400.0, -576.0);
  addLevelPt(2528.0, -224.0);
  addLevelPt(2656.0, -576.0);
  addLevelPt(2784.0, -224.0);
  addLevelPt(2880.0, -800.0);
  addLevelPt(3040.0, -224.0);
  addLevelPt(3328.0, -800.0);
  addLevelPt(3488.0, -224.0);
  addLevelPt(2336.0, -736.0);
  addLevelPt(2816.0, -576.0);
  addLevelPt(2816.0, -992.0);
  addLevelPt(3552.0, -800.0);
  createBlocksFromTLandBR();
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(2368.0, -256.0);
    addLevelPt(2272.0, -384.0);
    addLevelPt(2560.0, -256.0);
    addLevelPt(2624.0, -256.0);
    addLevelPt(2592.0, -312.0);
    addLevelPt(2496.0, -768.0);
    addLevelPt(2656.0, -768.0);
    addLevelPt(2752.0, -768.0);
    addLevelPt(3168.0, -1024.0);
    addLevelPt(3264.0, -1024.0);
    addLevelPt(3072.0, -256.0);
    addLevelPt(3136.0, -256.0);
    addLevelPt(3200.0, -256.0);
    addLevelPt(3264.0, -256.0);
    addLevelPt(3104.0, -311.0);
    addLevelPt(3168.0, -312.0);
    addLevelPt(3232.0, -315.0);
    createSkullsCentered();
  }
  
  if (useCages == true)
  {
    clearPtList();
    addLevelPt(2592.0, -224.0);
    addLevelPt(3120.0, -224.0);
    addLevelPt(3248.0, -224.0);
    createPuppiesGroundPts();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angrySpikeblock(Logical useSkulls)
{
  Point2 nextEndpoint;
  
  // terrain ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(672.0, -64.0);
  addLevelPt(816.0, -96.0);
  addLevelPt(1280.0, -96.0);
  addLevelPt(1376.0, -64.0);
  addLevelPt(1536.0, -96.0);
  addLevelPt(1856.0, -96.0);
  addLevelPt(2048.0, 0.0);
  addLevelPt(2208.0, -32.0);
  addLevelPt(2296.0, -96.0);
  addLevelPt(2904.0, -96.0);
  addLevelPt(3008.0, -192.0);
  addLevelPt(3552.0, -192.0);
  addLevelPt(3680.0, -160.0);
  addLevelPt(3928.0, -160.0);
  addCameraGroundPts(currPointList);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3928.0, 416.0);
  addLevelPt(0.0, 416.0);
  createThemedGroundFromCWPts();
  
  // camera top
  clearPtList();
  addLevelPt(888.0, -456.0);
  addLevelPt(1568.0, -480.0);
  addLevelPt(2368.0, -480.0);
  addLevelPt(3048.0, -1056.0);
  addLevelPt(3168.0, -1056.0);
  addLevelPt(3296.0, -984.0);
  addLevelPt(3424.0, -864.0);
  addCameraCeilingPts(currPointList);
  
  // halfspike breakable blocks
  clearPtList();
  addLevelPt(832.0, -384.0);
  addLevelPt(960.0, -384.0);
  addLevelPt(1088.0, -384.0);
  addLevelPt(1568.0, -224.0);
  addLevelPt(1568.0, -352.0);
  addLevelPt(2496.0, -352.0);
  addLevelPt(2496.0, -224.0);
  addLevelPt(3040.0, -1056.0);
  addLevelPt(3040.0, -928.0);
  addLevelPt(3168.0, -864.0);
  addLevelPt(3296.0, -864.0);
  
  DataList<Coord1> spikeSides(currPointList.count);
  spikeSides.add(1);
  spikeSides.add(1);
  spikeSides.add(1);
  spikeSides.add(0);
  spikeSides.add(0);
  spikeSides.add(2);
  spikeSides.add(2);
  spikeSides.add(2);
  spikeSides.add(2);
  spikeSides.add(3);
  spikeSides.add(3);

  for (Coord1 i = 0; i < currPointList.count; i++)
  {
    TerrainQuad* halfspike = TerrainQuad::create_halfspike_TL(currPointList[i], Point2(128.0, 128.0),
                                                              spikeSides[i]);
    addPGO(halfspike);
  }
  
  // coins
  clearPtList();
  addLevelPt(888.0, -440.0);
  addLevelPt(1024.0, -440.0);
  addLevelPt(1152.0, -440.0);
  addLevelPt(1500.0, -172.0);
  addLevelPt(1500.0, -304.0);
  addLevelPt(1500.0, -420.0);
  addLevelPt(2560.0, -416.0);
  addLevelPt(2692.0, -288.0);
  addLevelPt(2692.0, -156.0);
  addLevelPt(3232.0, -676.0);
  addLevelPt(3372.0, -676.0);
  createCoinsFromCenter();

  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1024.0, -128.0);
    addLevelPt(1088.0, -128.0);
    addLevelPt(1760.0, -128.0);
    addLevelPt(2368.0, -128.0);
    addLevelPt(2432.0, -128.0);
    addLevelPt(2400.0, -185.0);
    addLevelPt(3264.0, -896.0);
    addLevelPt(3328.0, -895.0);
    addLevelPt(3296.0, -952.0);
    createSkullsCentered();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angryToppleTower(Logical useSkulls, Logical useGems)
{
  Point2 nextEndpoint;
  
  // terrain ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(512.0, 256.0);
  addLevelPt(3072.0, 256.0);
  addLevelPt(3328.0, 0.0);
  addLevelPt(3584.0, 0.0);
  addCameraGroundPts(currPointList);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3584.0, 640.0);
  addLevelPt(0.0, 640.0);
  createThemedGroundFromCWPts();
  
  // breakable blocks
  clearPtList();
  addLevelPt(1728.0, -256.0);
  addLevelPt(1856.0, 256.0);
  addLevelPt(1728.0, -768.0);
  addLevelPt(1856.0, -256.0);
  addLevelPt(1536.0, -896.0);
  addLevelPt(2048.0, -768.0);
  addLevelPt(1536.0, -1408.0);
  addLevelPt(1664.0, -896.0);
  addLevelPt(1920.0, -1408.0);
  addLevelPt(2048.0, -896.0);
  addLevelPt(1024.0, -1536.0);
  addLevelPt(2560.0, -1408.0);
  createBlocksFromTLandBR();
  
  // coins
  clearPtList();
  addLevelPt(319.0, -182.0);
  addLevelPt(417.0, -87.0);
  addLevelPt(515.0, 8.0);
  addLevelPt(627.0, 84.0);
  addLevelPt(766.0, 92.0);
  addLevelPt(901.0, 93.0);
  addLevelPt(1037.0, 98.0);
  addLevelPt(1174.0, 99.0);
  addLevelPt(1309.0, 100.0);
  addLevelPt(1444.0, 100.0);
  addLevelPt(1579.0, 96.0);
  addLevelPt(1986.0, 98.0);
  addLevelPt(2121.0, 101.0);
  addLevelPt(2258.0, 100.0);
  addLevelPt(2393.0, 101.0);
  addLevelPt(2528.0, 98.0);
  addLevelPt(2665.0, 95.0);
  addLevelPt(2800.0, 92.0);
  addLevelPt(2939.0, 84.0);
  addLevelPt(3051.0, 8.0);
  addLevelPt(3149.0, -87.0);
  addLevelPt(3246.0, -181.0);
  createCoinsFromCenter();

  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1696.0, -928.0);
    addLevelPt(1888.0, -928.0);
    addLevelPt(1056.0, -1568.0);
    addLevelPt(1248.0, -1568.0);
    addLevelPt(1440.0, -1568.0);
    addLevelPt(1632.0, -1568.0);
    addLevelPt(1952.0, -1568.0);
    addLevelPt(2144.0, -1568.0);
    addLevelPt(2336.0, -1568.0);
    addLevelPt(2528.0, -1568.0);
    createSkullsCentered();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1584.0, -32.0);
    addLevelPt(1984.0, -32.0);
    createGemsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angryCrossing(Logical useSkulls, Logical useGem,
                                    Logical useBBlocks, Logical useCage)
{
  Point2 nextEndpoint;
  
  // top left ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(640.0, 64.0);
  addLevelPt(640.0, 384.0);
  addLevelPt(448.0, 576.0);
  addLevelPt(0.0, 576.0);
  createThemedGroundFromCWPts();

  // middle ground
  clearPtList();
  addLevelPt(2304.0, 704.0);
  addLevelPt(2624.0, 704.0);
  addLevelPt(2688.0, 1344.0);
  addLevelPt(2496.0, 1512.0);
  addLevelPt(2176.0, 1344.0);
  createThemedGroundFromCWPts();

  // right ground
  clearPtList();
  addLevelPt(4728.0, 584.0);
  addLevelPt(4992.0, 584.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(4992.0, 1920.0);
  addLevelPt(4184.0, 1920.0);
  addLevelPt(4288.0, 1432.0);
  addLevelPt(4336.0, 888.0);
  addLevelPt(4520.0, 648.0);
  createThemedGroundFromCWPts();

  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(640.0, 64.0);
  addLevelPt(2304.0, 704.0);
  addLevelPt(2624.0, 704.0);
  addLevelPt(4448.0, 728.0);
  addLevelPt(4520.0, 640.0);
  addLevelPt(4728.0, 584.0);
  addLevelPt(4992.0, 584.0);
  addCameraGroundPts(currPointList);
  
  // moving platform
  clearPtList();
  addLevelPt(640.0, 64.0);
  addLevelPt(832.0, 64.0);
  addLevelPt(832.0, 128.0);
  addLevelPt(640.0, 128.0);
  TerrainQuad* mover = TerrainQuad::create_static_quad(currPointList[0], currPointList[1],
                                                       currPointList[2], currPointList[3]);
  mover->setImage(getImg(moverPlatform192Img));
  addPGO(mover);
  
  clearPtList();
  addLevelPt(640.0, 64.0);  // start
  addLevelPt(4216.0, 640.0);  // end
  
  mover->pathPoints = currPointList;
  mover->pathMoveSpeed = 300.0;
  mover->enablePathMove();
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(2384.0, 704.0);
    addLevelPt(2464.0, 704.0);
    createSkullsGroundPts();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(2552.0, 704.0);
    createPuppiesGroundPts();
  }
  
  if (useGem == true)
  {
    clearPtList();
    addLevelPt(2464.0, 704.0);
    createGemsFromGroundPts();
  }
  
  if (useBBlocks == true)
  {
    clearPtList();
    addLevelPt(2368.0, 512.0);
    addLevelPt(2560.0, 704.0);
    createBurningBlocksFromTLandBR();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angrySpinners(Logical useSkulls, Logical useCage, Logical useGem,
                                    Logical useBBlocks, Coord1 flyerType)
{
  Point2 nextEndpoint;
  
  // bottom left ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(384.0, 576.0);
  addLevelPt(0.0, 576.0);
  createThemedGroundFromCWPts();
  
  // middle ground
  clearPtList();
  addLevelPt(1280.0, -576.0);
  addLevelPt(1664.0, -576.0);
  addLevelPt(1664.0, 256.0);
  addLevelPt(1280.0, 256.0);
  createThemedGroundFromCWPts();

  // right ground
  clearPtList();
  addLevelPt(2688.0, -1152.0);
  addLevelPt(3328.0, -1152.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3328.0, -320.0);
  addLevelPt(2688.0, -320.0);
  createThemedGroundFromCWPts();

  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(704.0, -256.0);
  addLevelPt(1024.0, -256.0);
  addLevelPt(1280.0, -576.0);
  addLevelPt(1664.0, -576.0);
  addLevelPt(1984.0, -768.0);
  addLevelPt(2304.0, -768.0);
  addLevelPt(2688.0, -1152.0);
  addLevelPt(3328.0, -1152.0);
  addCameraGroundPts(currPointList);
  
  // spinners, [TL, BR]
  clearPtList();
  addLevelPt(704.0, -320.0);
  addLevelPt(1024.0, -256.0);
  addLevelPt(1984.0, -832.0);
  addLevelPt(2304.0, -768.0);
  createSpinnersFromTLandBR();
  
  // coins
  clearPtList();
  addLevelPt(802.0, -560.0);
  addLevelPt(863.0, -560.0);
  addLevelPt(925.0, -560.0);
  addLevelPt(2080.0, -1024.0);
  addLevelPt(2144.0, -1024.0);
  addLevelPt(2208.0, -1024.0);
  addLevelPt(2716.0, -1280.0);
  addLevelPt(2780.0, -1344.0);
  addLevelPt(2844.0, -1280.0);
  addLevelPt(2908.0, -1344.0);
  addLevelPt(2972.0, -1280.0);
  addLevelPt(3036.0, -1344.0);
  addLevelPt(3100.0, -1280.0);
  createCoinsFromCenter();
  
  // fliers
  clearPtList();
  addLevelPt(1152.0, -576.0);
  addLevelPt(1152.0, -360.0);
  addLevelPt(1800.0, -760.0);
  addLevelPt(1800.0, -528.0);
  addLevelPt(2504.0, -1152.0);
  addLevelPt(2504.0, -992.0);
  createFlyersFromLines(flyerType);
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1344.0, -576.0);
    addLevelPt(1412.0, -576.0);
    addLevelPt(1600.0, -576.0);
    createSkullsGroundPts();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1548.0, -576.0);
    createPuppiesGroundPts();
  }

  if (useGem == true)
  {
    clearPtList();
    addLevelPt(1412.0, -576.0);
    createGemsFromGroundPts();
  }
  
  if (useBBlocks == true)
  {
    clearPtList();
    addLevelPt(1308.0, -740.0);
    addLevelPt(1472.0, -576.0);
    createBurningBlocksFromTLandBR();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::angryPit(Logical useSkulls, Logical useCage,
                               Logical useGem, Coord1 enemyType)
{
  Point2 nextEndpoint;

  // left part of stairs
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(576.0, 0.0);
  addLevelPt(576.0, 256.0);
  addLevelPt(768.0, 256.0);
  addLevelPt(768.0, 512.0);
  addLevelPt(960.0, 512.0);
  addLevelPt(960.0, 840.0);
  addLevelPt(888.0, 840.0);
  addLevelPt(888.0, 584.0);
  addLevelPt(704.0, 584.0);
  addLevelPt(704.0, 312.0);
  addLevelPt(496.0, 312.0);
  addLevelPt(496.0, 72.0);
  addLevelPt(0.0, 72.0);
  createThemedGroundFromCWPts();

  // center part of stairs
  clearPtList();
  addLevelPt(960.0, 736.0);
  addLevelPt(992.0, 768.0);
  addLevelPt(1632.0, 768.0);
  addLevelPt(1664.0, 736.0);
  addLevelPt(1664.0, 840.0);
  addLevelPt(960.0, 840.0);
  createThemedGroundFromCWPts();
  
  // right part of stairs
  clearPtList();
  addLevelPt(1664.0, 768.0);
  addLevelPt(1664.0, 512.0);
  addLevelPt(1856.0, 512.0);
  addLevelPt(1856.0, 256.0);
  addLevelPt(2048.0, 256.0);
  addLevelPt(2048.0, 0.0);
  addLevelPt(2176.0, 0.0);
  addLevelPt(2496.0, -128.0);
  addLevelPt(2688.0, -128.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2688.0, -48.0);
  addLevelPt(2184.0, 80.0);
  addLevelPt(2184.0, 376.0);
  addLevelPt(1968.0, 376.0);
  addLevelPt(1968.0, 592.0);
  addLevelPt(1760.0, 592.0);
  addLevelPt(1760.0, 832.0);
  addLevelPt(1664.0, 832.0);
  createThemedGroundFromCWPts();

  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(376.0, 0.0);
  addLevelPt(960.0, 768.0);
  addLevelPt(1664.0, 768.0);
  addLevelPt(2176.0, 0.0);
  addLevelPt(2496.0, -128.0);
  addLevelPt(2688.0, -128.0);
  addCameraGroundPts(currPointList);

  // camera ceil
  clearPtList();
  addLevelPt(616.0, -456.0);
  addLevelPt(1056.0, -496.0);
  addLevelPt(1976.0, -472.0);
  addLevelPt(2384.0, -448.0);
  addCameraCeilingPts(currPointList);

  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(832.0, -64.0);
    addLevelPt(1408.0, -64.0);
    addLevelPt(1696.0, -128.0);
    addLevelPt(1376.0, 192.0);
    addLevelPt(1024.0, 576.0);
    addLevelPt(1280.0, 448.0);
    addLevelPt(1376.0, 768.0);
    addLevelPt(1440.0, 768.0);
    createSkullsGroundPts();
  }
  
  // enemies
  clearPtList();
  addLevelPt(752.0, -64.0);
  addLevelPt(576.0, -64.0);
  addLevelPt(1024.0, -64.0);
  addLevelPt(1128.0, -64.0);
  addLevelPt(968.0, -96.0);
  addLevelPt(1264.0, -88.0);
  addLevelPt(1496.0, -64.0);
  addLevelPt(1360.0, -64.0);
  addLevelPt(1656.0, -64.0);
  addLevelPt(1920.0, -64.0);
  addLevelPt(1752.0, -128.0);
  addLevelPt(2048.0, -64.0);
  addLevelPt(2320.0, -56.0);
  addLevelPt(2120.0, -8.0);
  addLevelPt(2488.0, -128.0);

  // don't create any enemies if it's set to none
  if (enemyType == ENEMY_NONE)
  {
    clearPtList();
  }
  
  for (Coord1 i = 0; i < currPointList.count; i += 3)
  {
    PizzaGOStd* wisp = createEnemy(enemyType);
    wisp->setXY(currPointList[i]);
    wisp->myPlatformLine.set(currPointList[i+1], currPointList[i+2]);
    addPGO(wisp);
  }
  
  if (useGem == true)
  {
    clearPtList();
    addLevelPt(752.0, -64.0);
    createGemsFromGroundPts();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1576.0, 768.0);
    createPuppiesGroundPts();
  }
  
  // block 1
  clearPtList();
  addLevelPt(576.0, -64.0);
  addLevelPt(1024.0, 128.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(672.0, 32.0);
  addLevelPt(944.0, 72.0);
  createHiddenSkullsInLastBlockFromC(useSkulls);
  
  clearPtList();
  addLevelPt(785.0, 34.0);
  addLevelPt(865.0, 0.0);
  createHiddenPhysCoinsInLastBlockFromC();
  
  // block 2
  clearPtList();
  addLevelPt(1088.0, -64.0);
  addLevelPt(1664.0, 128.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1216.0, 64.0);
  createHiddenSkullsInLastBlockFromC(useSkulls);

  clearPtList();
  addLevelPt(1312.0, 0.0);
  addLevelPt(1456.0, 64.0);
  addLevelPt(1600.0, 16.0);
  createHiddenPhysCoinsInLastBlockFromC();

  // block 3
  clearPtList();
  addLevelPt(1664.0, -128.0);
  addLevelPt(1856.0, 128.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1728.0, -48.0);
  addLevelPt(1792.0, 16.0);
  addLevelPt(1728.0, 64.0);
  createHiddenPhysCoinsInLastBlockFromC();

  // block 4
  clearPtList();
  addLevelPt(1856.0, -64.0);
  addLevelPt(2048.0, 256.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1936.0, 16.0);
  addLevelPt(2000.0, 128.0);
  addLevelPt(1952.0, 192.0);
  createHiddenPhysCoinsInLastBlockFromC();
  
  // block 5 second row
  clearPtList();
  addLevelPt(576.0, 128.0);
  addLevelPt(768.0, 256.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(640.0, 176.0);
  createHiddenPhysCoinsInLastBlockFromC();
  
  // block 6 second row
  clearPtList();
  addLevelPt(768.0, 128.0);
  addLevelPt(1216.0, 448.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(896.0, 320.0);
  createHiddenSkullsInLastBlockFromC(useSkulls);

  clearPtList();
  addLevelPt(944.0, 208.0);
  addLevelPt(1112.0, 240.0);
  addLevelPt(1088.0, 336.0);
  createHiddenPhysCoinsInLastBlockFromC();

  // block 7 second row
  clearPtList();
  addLevelPt(1344.0, 192.0);
  addLevelPt(1856.0, 512.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1504.0, 288.0);
  addLevelPt(1760.0, 352.0);
  createHiddenSkullsInLastBlockFromC(useSkulls);

  clearPtList();
  addLevelPt(1424.0, 416.0);
  addLevelPt(1552.0, 384.0);
  addLevelPt(1616.0, 432.0);
  addLevelPt(1664.0, 320.0);
  createHiddenPhysCoinsInLastBlockFromC();

  // block 8 third row
  clearPtList();
  addLevelPt(960.0, 576.0);
  addLevelPt(1152.0, 768.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1024.0, 640.0);
  addLevelPt(1088.0, 688.0);
  createHiddenPhysCoinsInLastBlockFromC();

  // block 9 third row
  clearPtList();
  addLevelPt(1152.0, 448.0);
  addLevelPt(1344.0, 768.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  addLevelPt(1232.0, 560.0);
  addLevelPt(1264.0, 672.0);
  createHiddenPhysCoinsInLastBlockFromC();
  
  // remaining blocks
  clearPtList();
  addLevelPt(1408.0, 128.0);
  addLevelPt(1728.0, 192.0);
  addLevelPt(1472.0, 576.0);
  addLevelPt(1664.0, 768.0);
  createStaticBreakableBlocksFromTLandBR();

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flyerChunk(Coord1 flyer1Type, Coord1 flyer2Type, Coord1 flyer3Type)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(96.0, 0.0);
  addLevelPt(192.0, 32.0);
  addLevelPt(480.0, 32.0);
  addLevelPt(560.0, 288.0);
  addLevelPt(512.0, 576.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1152.0, -128.0);
  addLevelPt(1600.0, -128.0);
  addLevelPt(1552.0, 264.0);
  addLevelPt(1568.0, 576.0);
  addLevelPt(1152.0, 576.0);
  addLevelPt(1088.0, 160.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 3
  clearPtList();
  addLevelPt(2336.0, 64.0);
  addLevelPt(2720.0, 64.0);
  addLevelPt(2728.0, 424.0);
  addLevelPt(2680.0, 576.0);
  addLevelPt(2264.0, 576.0);
  addLevelPt(2264.0, 320.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 4
  clearPtList();
  addLevelPt(3264.0, -160.0);
  addLevelPt(3584.0, -160.0);
  addLevelPt(3648.0, -32.0);
  addLevelPt(3584.0, 64.0);
  addLevelPt(3264.0, 32.0);
  addLevelPt(3200.0, -64.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 5
  clearPtList();
  addLevelPt(3968.0, 576.0);
  addLevelPt(3968.0, 376.0);
  addLevelPt(4024.0, 32.0);
  addLevelPt(4512.0, -64.0);
  addLevelPt(5024.0, -64.0);
  addCameraGroundPts(currPointList);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(5024.0, 576.0);
  createThemedGroundFromCWPts();

  // camera ceil
  clearPtList();
  addLevelPt(688.0, -408.0);
  addLevelPt(992.0, -448.0);
  addLevelPt(2024.0, -480.0);
  addLevelPt(2560.0, -272.0);
  addLevelPt(3416.0, -576.0);
  addLevelPt(3552.0, -568.0);
  addLevelPt(4224.0, -208.0);
  addCameraCeilingPts(currPointList);
  
  // unbreakable blocks
  clearPtList();
  addLevelPt(1248.0, -224.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(1440.0, -224.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(1312.0, -320.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(1408.0, -320.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(2496.0, -32.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(2496.0, -128.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(2496.0, -224.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3344.0, -256.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3456.0, -256.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3344.0, -352.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3456.0, -352.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3376.0, -448.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3488.0, -448.0);
  addLevelPt(96.0, 96.0);
  addLevelPt(3424.0, -544.0);
  addLevelPt(96.0, 96.0);
  createSteelFromTLandWH();
  
  // chicken path
  clearPtList();
  addLevelPt(840.0, 0.0);
  addLevelPt(840.0, -672.0);
  createFlyersFromLines(flyer1Type);
  
  // mohawk path
  clearPtList();
  addLevelPt(2184.0, -32.0);
  addLevelPt(2904.0, -32.0);
  createFlyersFromLines(flyer2Type);

  // flameskull path
  clearPtList();
  addLevelPt(3240.0, -272.0);
  addLevelPt(3648.0, -264.0);
  addLevelPt(3824.0, 0.0);
  addLevelPt(3600.0, 184.0);
  addLevelPt(3216.0, 144.0);
  addLevelPt(3056.0, -80.0);
  createFlyerFromSingleLine(flyer3Type);

  // coins
  clearPtList();
  addLevelPt(526.0, -160.0);
  addLevelPt(591.0, -281.0);
  addLevelPt(699.0, -363.0);
  addLevelPt(825.0, -401.0);
  addLevelPt(958.0, -394.0);
  addLevelPt(1089.0, -347.0);
  addLevelPt(1368.0, -200.0);
  addLevelPt(1416.0, -200.0);
  addLevelPt(1416.0, -144.0);
  addLevelPt(1368.0, -144.0);
  addLevelPt(1621.0, -278.0);
  addLevelPt(1704.0, -381.0);
  addLevelPt(1821.0, -431.0);
  addLevelPt(1957.0, -444.0);
  addLevelPt(2092.0, -431.0);
  addLevelPt(2214.0, -370.0);
  addLevelPt(2305.0, -278.0);
  addLevelPt(2368.0, -161.0);
  addLevelPt(2675.0, -112.0);
  addLevelPt(2750.0, -219.0);
  addLevelPt(2873.0, -281.0);
  addLevelPt(3003.0, -318.0);
  addLevelPt(3135.0, -336.0);
  addLevelPt(3270.0, -346.0);
  addLevelPt(3643.0, -341.0);
  addLevelPt(3773.0, -308.0);
  addLevelPt(3898.0, -252.0);
  addLevelPt(4020.0, -196.0);
  addLevelPt(4133.0, -112.0);
  addLevelPt(4243.0, -95.0);
  addLevelPt(4374.0, -148.0);
  addLevelPt(4526.0, -172.0);
  addLevelPt(4678.0, -172.0);
  addLevelPt(4829.0, -172.0);
  createCoinsFromCenter();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::waterChunk()
{
  Point2 nextEndpoint;
  
  // main bottom ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(768.0, 1312.0);
  addLevelPt(3072.0, 1312.0);
  addLevelPt(3456.0, 0.0);
  addLevelPt(3928.0, 0.0);
  addCameraGroundPts(currPointList);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3928.0, 1696.0);
  addLevelPt(0.0, 1696.0);
  createThemedGroundFromCWPts();
  
  clearPtList();
  addLevelPt(1632.0, 192.0);
  addLevelPt(2304.0, 192.0);
  addCameraCeilingPts(currPointList);
  
  // terrain ground top
  clearPtList();
  addLevelPt(1312.0, -736.0);
  addLevelPt(2560.0, -736.0);
  addLevelPt(2304.0, 192.0);
  addLevelPt(1632.0, 192.0);
  createThemedGroundFromCWPts();
  
  // water region
  clearPtList();
  addLevelPt(528.0, 96.0);
  addLevelPt(3432.0, 96.0);
  addLevelPt(3072.0, 1312.0);
  addLevelPt(768.0, 1312.0);
  
  WaterArea* water = new WaterArea(currPointList);
  addPGO(water);
  
  // lava chunk
  clearPtList();
  addLevelPt(1632.0, 192.0);  // top chunk
  addLevelPt(2304.0, 192.0);
  addLevelPt(2304.0, 320.0);
  addLevelPt(1632.0, 320.0);
  createSpikeBlocksFromQuads();
  
  clearPtList();
  addLevelPt(1632.0, 1184.0);  // bottom
  addLevelPt(2304.0, 1184.0);
  addLevelPt(2304.0, 1312.0);
  addLevelPt(1632.0, 1312.0);
  createSpikeBlocksFromQuads();

  // coins
  clearPtList();
  addLevelPt(353.0, -285.0);
  addLevelPt(480.0, -233.0);
  addLevelPt(590.0, -151.0);
  addLevelPt(689.0, -59.0);
  addLevelPt(788.0, 36.0);
  addLevelPt(880.0, 133.0);
  addLevelPt(975.0, 234.0);
  addLevelPt(1462.0, 705.0);
  addLevelPt(1718.0, 784.0);
  addLevelPt(1985.0, 814.0);
  addLevelPt(2255.0, 801.0);
  addLevelPt(2524.0, 721.0);
  addLevelPt(2855.0, 281.0);
  addLevelPt(2921.0, 167.0);
  addLevelPt(3000.0, 59.0);
  addLevelPt(3100.0, -38.0);
  addLevelPt(3215.0, -107.0);
  addLevelPt(3339.0, -155.0);
  addLevelPt(3472.0, -186.0);
  addLevelPt(3603.0, -199.0);
  createCoinsFromCenter();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pizzeriaChunk()
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(768.0, -64.0);
  addLevelPt(960.0, -64.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(960.0, 160.0);
  addLevelPt(0.0, 160.0);

  TerrainGround* ground = TerrainGround::createNoThemeGround(currPointList, backgroundType, true);
  ground->setImage(getImg(shopFloorImg));
  addPGO(ground);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintBombtower(Logical useSkulls, Logical useBombs)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1792.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(1792.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  // camera
  clearPtList();
  addLevelPt(1792.0, 0.0);
  addCameraGroundPts(currPointList);

  // metal blocks
  clearPtList();
  addLevelPt(768.0, -128.0);
  addLevelPt(832.0, 0.0);
  addLevelPt(928.0, -128.0);
  addLevelPt(992.0, 0.0);
  addLevelPt(1088.0, -128.0);
  addLevelPt(1152.0, 0.0);
  addLevelPt(768.0, -192.0);
  addLevelPt(960.0, -128.0);
  addLevelPt(960.0, -192.0);
  addLevelPt(1152.0, -128.0);
  addLevelPt(768.0, -320.0);
  addLevelPt(832.0, -192.0);
  addLevelPt(928.0, -320.0);
  addLevelPt(992.0, -192.0);
  addLevelPt(1088.0, -320.0);
  addLevelPt(1152.0, -192.0);
  addLevelPt(768.0, -448.0);
  addLevelPt(832.0, -320.0);
  addLevelPt(832.0, -384.0);
  addLevelPt(1088.0, -320.0);
  addLevelPt(1088.0, -448.0);
  addLevelPt(1152.0, -320.0);
  addLevelPt(928.0, -512.0);
  addLevelPt(992.0, -384.0);
  createSteelFromTLandBR();
  
  // bombs or skulls
  clearPtList();
  addLevelPt(885.0, -227.0);
  addLevelPt(887.0, -428.0);
  addLevelPt(1046.0, -420.0);
  addLevelPt(1046.0, -225.0);

  
  if (useBombs == true)
  {
    addLevelPt(1043.0, -34.0);
    addLevelPt(879.0, -37.0);
    
    for (Coord1 i = 0; i < currPointList.count; ++i)
    {
      MotionBomb* bomb = new MotionBomb();
      bomb->setXY(currPointList[i]);
      addPGO(bomb);
    }
  }
  
  if (useSkulls == true)
  {
    createSkullsCentered();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintBounceChasm(Logical useGems)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(376.0, 0.0);
  addLevelPt(568.0, 64.0);
  addLevelPt(528.0, 208.0);
  addLevelPt(368.0, 304.0);
  addLevelPt(0.0, 328.0);
  createThemedGroundFromCWPts();

  // terrain ground 2
  clearPtList();
  addLevelPt(7152.0, 0.0);
  addLevelPt(7528.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(7528.0, 328.0);
  addLevelPt(7160.0, 304.0);
  addLevelPt(7000.0, 208.0);
  addLevelPt(6960.0, 64.0);
  createThemedGroundFromCWPts();

  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(376.0, 0.0);
  addLevelPt(568.0, 64.0);
  addLevelPt(1264.0, -40.0);
  addLevelPt(2264.0, -200.0);
  addLevelPt(3104.0, -16.0);
  addLevelPt(4048.0, -224.0);
  addLevelPt(5072.0, -288.0);
  addLevelPt(6224.0, -40.0);
  addLevelPt(6960.0, 64.0);
  addLevelPt(7152.0, 0.0);
  addLevelPt(7528.0, 0.0);
  addCameraGroundPts(currPointList);

  // bouncers
  clearPtList();
  addLevelPt(1261.0, -36.0);
  addLevelPt(2270.0, -201.0);
  addLevelPt(3108.0, -21.0);
  addLevelPt(4043.0, -223.0);
  addLevelPt(5066.0, -288.0);
  addLevelPt(6232.0, -40.0);
  
  for (Coord1 i = 0; i < currPointList.count; i += 1)
  {
    addPGO(TerrainQuad::create_bouncer(currPointList[i], 100.0));
  }
  
  // coins
  clearPtList();
  addLevelPt(549.0, -209.0);
  addLevelPt(653.0, -291.0);
  addLevelPt(779.0, -337.0);
  addLevelPt(913.0, -351.0);
  addLevelPt(1045.0, -331.0);
  addLevelPt(1480.0, -357.0);
  addLevelPt(1609.0, -404.0);
  addLevelPt(1739.0, -440.0);
  addLevelPt(1873.0, -457.0);
  addLevelPt(2010.0, -458.0);
  addLevelPt(2498.0, -464.0);
  addLevelPt(2634.0, -469.0);
  addLevelPt(2764.0, -441.0);
  addLevelPt(2885.0, -389.0);
  addLevelPt(2994.0, -309.0);
  addLevelPt(3263.0, -387.0);
  addLevelPt(3375.0, -457.0);
  addLevelPt(3506.0, -506.0);
  addLevelPt(3638.0, -528.0);
  addLevelPt(3772.0, -527.0);
  addLevelPt(4248.0, -563.0);
  addLevelPt(4379.0, -595.0);
  addLevelPt(4513.0, -613.0);
  addLevelPt(4649.0, -618.0);
  addLevelPt(4784.0, -607.0);
  addLevelPt(5416.0, -614.0);
  addLevelPt(5553.0, -608.0);
  addLevelPt(5687.0, -586.0);
  addLevelPt(5819.0, -551.0);
  addLevelPt(5943.0, -501.0);
  addLevelPt(6357.0, -293.0);
  addLevelPt(6492.0, -318.0);
  addLevelPt(6627.0, -309.0);
  addLevelPt(6754.0, -275.0);
  addLevelPt(6873.0, -207.0);
  createCoinsFromCenter();
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1256.0, -208.0);
    addLevelPt(2264.0, -376.0);
    addLevelPt(3120.0, -184.0);
    addLevelPt(4032.0, -400.0);
    addLevelPt(5064.0, -456.0);
    addLevelPt(6224.0, -216.0);
    createGemsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintSpearmanPitA(Logical useGems, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(384.0, 128.0);
  addLevelPt(320.0, 256.0);
  addLevelPt(288.0, 384.0);
  addLevelPt(0.0, 384.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1408.0, 0.0);
  addLevelPt(1792.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(1792.0, 384.0);
  addLevelPt(1504.0, 384.0);
  addLevelPt(1472.0, 256.0);
  addLevelPt(1408.0, 128.0);
  createThemedGroundFromCWPts();

  // camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1792.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // blocks that break after a few seconds
  clearPtList();
  addLevelPt(384.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(640.0, 0.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(896.0, 0.0);
  addLevelPt(1024.0, 0.0);
  addLevelPt(1152.0, 0.0);
  addLevelPt(1280.0, 0.0);
  
  for (Coord1 i = 0; i < currPointList.count; i++)
  {
    TerrainQuad* landPiece = TerrainQuad::create_fading_redrock(currPointList[i], Point2(128.0, 128.0));
    addPGO(landPiece);
  }
  
  // coins
  clearPtList();
  addLevelPt(400.0, -80.0);
  addLevelPt(528.0, -80.0);
  addLevelPt(656.0, -80.0);
  addLevelPt(784.0, -80.0);
  addLevelPt(912.0, -80.0);
  createCoinsFromCenter();
  
  // spearman
  clearPtList();
  addLevelPt(1672.0, 0.0);
  addLevelPt(1408.0, 0.0);
  addLevelPt(1792.0, 0.0);
  
  if (enemyType != ENEMY_NONE)
  {
    PizzaGOStd* enemy = createEnemy(enemyType);
    enemy->setXY(currPointList[0]);
    enemy->myPlatformLine.set(currPointList[1], currPointList[2]);
    
    if (enemyType == GL_ENEMY_SPEARMAN_VERT ||
        enemyType == GL_ENEMY_SPEARMAN_SIDE)
    {
      static_cast<SkeletonSpearman*>(enemy)->walkSpeed = 0.0;
      enemy->faceDir(-1);
    }
    
    addPGO(enemy);
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1040.0, -80.0);
    createGemsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintSpikejump(Logical useGems)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(856.0, 0.0);
  addLevelPt(1280.0, -64.0);
  addLevelPt(1952.0, -64.0);
  addLevelPt(2296.0, 0.0);
  addLevelPt(3328.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(3328.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  // upper camera segment 1
  clearPtList();
  addLevelPt(264.0, -456.0);
  addLevelPt(576.0, -608.0);
  addLevelPt(2752.0, -608.0);
  addLevelPt(2976.0, -400.0);
  addCameraCeilingPts(currPointList);

  // top piece 1
  clearPtList();
  addLevelPt(448.0, -832.0);
  addLevelPt(896.0, -832.0);
  addLevelPt(896.0, -736.0);
  addLevelPt(768.0, -608.0);
  addLevelPt(576.0, -608.0);
  addLevelPt(448.0, -736.0);
  createThemedGroundFromCWPts();
  
  // top piece 2
  clearPtList();
  addLevelPt(2432.0, -832.0);
  addLevelPt(2880.0, -832.0);
  addLevelPt(2880.0, -736.0);
  addLevelPt(2752.0, -608.0);
  addLevelPt(2560.0, -608.0);
  addLevelPt(2432.0, -736.0);
  createThemedGroundFromCWPts();
  
  // spikey quads
  clearPtList();
  addLevelPt(576.0, -608.0);
  addLevelPt(768.0, -608.0);
  addLevelPt(768.0, -544.0);
  addLevelPt(576.0, -544.0);
  addLevelPt(1408.0, -128.0);
  addLevelPt(1824.0, -128.0);
  addLevelPt(1824.0, -64.0);
  addLevelPt(1408.0, -64.0);
  addLevelPt(2560.0, -608.0);
  addLevelPt(2752.0, -608.0);
  addLevelPt(2752.0, -544.0);
  addLevelPt(2560.0, -544.0);
  createSpikeBlocksFromQuads();
  
  // coins
  clearPtList();
  addLevelPt(773.0, -190.0);
  addLevelPt(901.0, -225.0);
  addLevelPt(1034.0, -258.0);
  addLevelPt(1170.0, -280.0);
  addLevelPt(1305.0, -299.0);
  addLevelPt(1440.0, -310.0);
  addLevelPt(1573.0, -317.0);
  addLevelPt(1710.0, -316.0);
  addLevelPt(1846.0, -312.0);
  addLevelPt(1983.0, -303.0);
  addLevelPt(2118.0, -290.0);
  addLevelPt(2251.0, -268.0);
  addLevelPt(2382.0, -230.0);
  addLevelPt(2503.0, -171.0);
  createCoinsFromCenter();

  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1000.0, 816.0);
    addLevelPt(1656.0, 792.0);
    createGemsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintSpikeys(Logical useGems, Coord1 groundType, Coord1 lineFlyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(864.0, 0.0);
  addLevelPt(864.0, -224.0);
  addLevelPt(1408.0, -224.0);
  addLevelPt(1408.0, 0.0);
  addLevelPt(1792.0, 0.0);
  addLevelPt(1792.0, -224.0);
  addLevelPt(2336.0, -224.0);
  addLevelPt(2336.0, 0.0);
  addLevelPt(2816.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(2816.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  // coins
  clearPtList();
  addLevelPt(1319.0, -359.0);
  addLevelPt(1420.0, -442.0);
  addLevelPt(1550.0, -472.0);
  addLevelPt(1686.0, -471.0);
  addLevelPt(1817.0, -445.0);
  addLevelPt(1917.0, -371.0);
  createCoinsFromCenter();
  
  // spinies
  clearPtList();
  addLevelPt(1008.0, -224.0);
  addLevelPt(864.0, -224.0);
  addLevelPt(1408.0, -224.0);
  addLevelPt(1256.0, -224.0);
  addLevelPt(864.0, -224.0);
  addLevelPt(1408.0, -224.0);
  addLevelPt(1936.0, -224.0);
  addLevelPt(1792.0, -224.0);
  addLevelPt(2336.0, -224.0);
  addLevelPt(2216.0, -224.0);
  addLevelPt(1792.0, -224.0);
  addLevelPt(2336.0, -224.0);
  
  for (Coord1 i = 0; i < currPointList.count; i += 3)
  {
    PizzaGOStd* enemy = createEnemy(groundType);
    enemy->setXY(currPointList[i]);
    enemy->myPlatformLine.set(currPointList[i + 1], currPointList[i + 2]);
    
    if (groundType == GL_ENEMY_SPINY)
    {
      static_cast<Spiny*>(enemy)->shouldFollowPlayer = false;
    }
    
    addPGO(enemy);
  }
  
  // mohawk path
  clearPtList();
  addLevelPt(1609.0, -529.0);
  addLevelPt(1605.0, -262.0);
  createFlyersFromLines(lineFlyType);
  
  // create chunk variations
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1600.0, -112.0);
    createGemsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintCavemen(Logical useGems, Logical useCages,
                                    Logical useSkulls, Logical useBBlocks, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(3968.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(3968.0, 144.0);
  addLevelPt(0.0, 144.0);
  createThemedGroundFromCWPts();
  
  // coins
  clearPtList();
  addLevelPt(520.0, -192.0);
  addLevelPt(624.0, -272.0);
  addLevelPt(872.0, -208.0);
  addLevelPt(976.0, -128.0);
  addLevelPt(1112.0, -96.0);
  addLevelPt(1232.0, -152.0);
  addLevelPt(1336.0, -240.0);
  addLevelPt(1592.0, -248.0);
  addLevelPt(1696.0, -160.0);
  addLevelPt(1808.0, -96.0);
  addLevelPt(1944.0, -112.0);
  addLevelPt(2056.0, -200.0);
  addLevelPt(2168.0, -272.0);
  addLevelPt(2296.0, -280.0);
  addLevelPt(2408.0, -208.0);
  addLevelPt(2520.0, -120.0);
  addLevelPt(2656.0, -96.0);
  addLevelPt(2776.0, -152.0);
  addLevelPt(2880.0, -240.0);
  addLevelPt(3128.0, -248.0);
  addLevelPt(3232.0, -160.0);
  addLevelPt(3352.0, -104.0);
  addLevelPt(3488.0, -128.0);
  addLevelPt(3600.0, -208.0);
  createCoinsFromCenter();
  
  if (useCages == true)
  {
    clearPtList();
    addLevelPt(1536.0, 0.0);
    addLevelPt(2808.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1536.0, 0.0);
    addLevelPt(2808.0, 0.0);
    createSkullsGroundPts();
  }
  
  if (useBBlocks == true)
  {
    clearPtList();
    addLevelPt(1536.0, 0.0);
    addLevelPt(112.0, 112.0);
    addLevelPt(2808.0, 0.0);
    addLevelPt(112.0, 112.0);
    createBurningBlocksFromBCandWH();
  }
  
  clearPtList();
  addLevelPt(760.0, -280.0);
  addLevelPt(1464.0, -288.0);
  addLevelPt(2296.0, -272.0);
  addLevelPt(3000.0, -288.0);
  
  if (useGems == true)
  {
    createGemsFromCenter();
  }
  else
  {
    createCoinsFromCenter();
  }
  
  if (enemyType != ENEMY_NONE)
  {
    clearPtList();
    addLevelPt(1104.0, 0.0);
    addLevelPt(760.0, 0.0);
    addLevelPt(1480.0, 0.0);
    addLevelPt(1944.0, 0.0);
    addLevelPt(1592.0, 0.0);
    addLevelPt(2296.0, 0.0);
    addLevelPt(3264.0, 0.0);
    addLevelPt(3000.0, 0.0);
    addLevelPt(3616.0, 0.0);
    createEnemiesFromBCLine(enemyType);
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintDozerDig(Logical useGems, Logical useCages,
                                     Logical useBurningBlocks, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1 (bottom left)
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(640.0, 128.0);
  addLevelPt(640.0, 832.0);
  addLevelPt(704.0, 1136.0);
  addLevelPt(1024.0, 1216.0);
  addLevelPt(1024.0, 1368.0);
  addLevelPt(472.0, 1160.0);
  addLevelPt(472.0, 176.0);
  addLevelPt(0.0, 176.0);
  createThemedGroundFromCWPts();

  // lower right terrain
  clearPtList();
  addLevelPt(1032.0, 1368.0);
  addLevelPt(1032.0, 1216.0);
  addLevelPt(2560.0, 1216.0);
  addLevelPt(2816.0, 1152.0);
  addLevelPt(3200.0, 1152.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3200.0, 1368.0);
  createThemedGroundFromCWPts();
  
  // upper right terrain
  clearPtList();
  addLevelPt(1600.0, 128.0);
  addLevelPt(1736.0, 128.0);
  addLevelPt(1856.0, 0.0);
  addLevelPt(1856.0, -1088.0);
  addLevelPt(2096.0, -1088.0);
  addLevelPt(2096.0, 576.0);
  addLevelPt(1736.0, 576.0);
  addLevelPt(1600.0, 448.0);
  createThemedGroundFromCWPts();

  // fading blocks
  clearPtList();
  addLevelPt(640.0, 128.0);
  addLevelPt(832.0, 320.0);
  addLevelPt(832.0, 128.0);
  addLevelPt(1024.0, 320.0);
  addLevelPt(1024.0, 128.0);
  addLevelPt(1216.0, 320.0);
  addLevelPt(1216.0, 128.0);
  addLevelPt(1408.0, 320.0);
  addLevelPt(1408.0, 128.0);
  addLevelPt(1600.0, 320.0);
  createFadeBlocksFromTLandBR();
  
  // camera bot
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(272.0, 0.0);
  addLevelPt(704.0, 1136.0);
  addLevelPt(1024.0, 1216.0);
  addLevelPt(2560.0, 1216.0);
  addLevelPt(2816.0, 1152.0);
  addLevelPt(3200.0, 1152.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(1600.0, 448.0);
  addLevelPt(1736.0, 576.0);
  addLevelPt(2240.0, 576.0);
  addLevelPt(2368.0, 448.0);
  addCameraGroundPts(currPointList);

  // coins
  clearPtList();
  addLevelPt(640.0, 64.0);
  addLevelPt(784.0, 64.0);
  addLevelPt(920.0, 64.0);
  addLevelPt(1056.0, 64.0);
  addLevelPt(1192.0, 64.0);
  addLevelPt(1328.0, 64.0);
  addLevelPt(1464.0, 64.0);
  addLevelPt(1600.0, 64.0);
  addLevelPt(1152.0, 520.0);
  addLevelPt(1168.0, 656.0);
  addLevelPt(1208.0, 784.0);
  addLevelPt(1272.0, 912.0);
  addLevelPt(1384.0, 992.0);
  addLevelPt(1512.0, 1032.0);
  addLevelPt(1656.0, 1032.0);
  createCoinsFromCenter();
  
  if (useCages == true)
  {
    clearPtList();
    addLevelPt(2488.0, 1216.0);
    createPuppiesGroundPts();
  }
  
  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(1600.0, 0.0);
    addLevelPt(1736.0, 128.0);
    addLevelPt(1024.0, 1088.0);
    addLevelPt(1152.0, 1216.0);
    addLevelPt(1536.0, 1088.0);
    addLevelPt(1664.0, 1216.0);
    addLevelPt(2304.0, 1088.0);
    addLevelPt(2432.0, 1216.0);
    createBurningBlocksFromTLandBR();
  }
  
  clearPtList();
  addLevelPt(1712.0, 48.0);
  addLevelPt(1152.0, 392.0);
  
  if (useGems == true)
  {
    createGemsFromCenter();
  }
  else
  {
    createCoinsFromCenter();
  }
  
  if (enemyType != ENEMY_NONE)
  {
    clearPtList();
    addLevelPt(1448.0, 128.0);
    addLevelPt(640.0, 128.0);
    addLevelPt(1728.0, 128.0);
    addLevelPt(2080.0, 1216.0);
    addLevelPt(1032.0, 1216.0);
    addLevelPt(2560.0, 1216.0);
    createEnemiesFromBCLine(enemyType);
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintJumpChoice(Logical useGems)
{
  Point2 nextEndpoint;
  
  // terrain ground 1 (bottom left)
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(640.0, 0.0);
  addLevelPt(1176.0, 472.0);
  addLevelPt(1520.0, 472.0);
  addLevelPt(2048.0, 0.0);
  addLevelPt(2688.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(2688.0, 144.0);
  addLevelPt(1528.0, 656.0);
  addLevelPt(1176.0, 656.0);
  addLevelPt(592.0, 160.0);
  addLevelPt(0.0, 160.0);
  createThemedGroundFromCWPts();
  
  // camera top
  clearPtList();
  addLevelPt(640.0, -384.0);
  addLevelPt(1064.0, -552.0);
  addLevelPt(1624.0, -536.0);
  addLevelPt(2040.0, -368.0);
  addCameraCeilingPts(currPointList);
  
  // spikes
  clearPtList();
  addLevelPt(1280.0, -64.0);
  addLevelPt(1408.0, -64.0);
  addLevelPt(1408.0, 64.0);
  addLevelPt(1280.0, 64.0);
  createSpikeBlocksFromQuads();
  
  // water
  clearPtList();
  addLevelPt(712.0, 64.0);
  addLevelPt(1976.0, 64.0);
  addLevelPt(1520.0, 472.0);
  addLevelPt(1168.0, 472.0);
  createWaterFromCWQuads();
  
  // coins
  clearPtList();
  addLevelPt(712.0, -272.0);
  addLevelPt(816.0, -360.0);
  addLevelPt(936.0, -416.0);
  addLevelPt(1064.0, -448.0);
  addLevelPt(1200.0, -464.0);
  addLevelPt(1480.0, -456.0);
  addLevelPt(1608.0, -440.0);
  addLevelPt(1744.0, -408.0);
  addLevelPt(1864.0, -352.0);
  addLevelPt(1976.0, -280.0);
  createCoinsFromCenter();
  
  clearPtList();
  addLevelPt(1336.0, -456.0);
  
  if (useGems == true)
  {
    createGemsFromCenter();
  }
  else
  {
    createCoinsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::sprintEndingRight()
{
  Point2 nextEndpoint;
  
  // terrain ground 1 (bottom left)
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(3416.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(3416.0, 136.0);
  addLevelPt(0.0, 136.0);
  createThemedGroundFromCWPts();

  // coins
  clearPtList();
  addLevelPt(624.0, -272.0);
  addLevelPt(760.0, -280.0);
  addLevelPt(872.0, -208.0);
  addLevelPt(976.0, -128.0);
  addLevelPt(1112.0, -96.0);
  addLevelPt(1232.0, -152.0);
  addLevelPt(1344.0, -240.0);
  addLevelPt(1464.0, -288.0);
  addLevelPt(1592.0, -248.0);
  addLevelPt(1688.0, -160.0);
  addLevelPt(1808.0, -96.0);
  addLevelPt(1952.0, -112.0);
  addLevelPt(2056.0, -200.0);
  addLevelPt(2168.0, -272.0);
  addLevelPt(2296.0, -272.0);
  addLevelPt(2408.0, -208.0);
  addLevelPt(2520.0, -120.0);
  addLevelPt(2656.0, -96.0);
  createCoinsFromCenter();
  
  // unarmed skeles
  clearPtList();
  addLevelPt(744.0, 0.0);
  addLevelPt(528.0, 0.0);
  addLevelPt(936.0, 0.0);
  addLevelPt(1200.0, 0.0);
  addLevelPt(1016.0, 0.0);
  addLevelPt(1312.0, 0.0);
  addLevelPt(1568.0, 0.0);
  addLevelPt(1392.0, 0.0);
  addLevelPt(1672.0, 0.0);
  addLevelPt(1952.0, 0.0);
  addLevelPt(1760.0, 0.0);
  addLevelPt(2080.0, 0.0);
  addLevelPt(2288.0, 0.0);
  addLevelPt(2152.0, 0.0);
  addLevelPt(2432.0, 0.0);
  addLevelPt(2728.0, 0.0);
  addLevelPt(2528.0, 0.0);
  addLevelPt(2888.0, 0.0);
  createEnemiesFromBCLine(GL_ENEMY_SPEARMAN_UNARMED);
  
  // flag
  clearPtList();
  addLevelPt(3000.0, 0.0);
  addLevelPt(3000.0, -248.0);
  addLevelPt(3016.0, -248.0);
  addLevelPt(3016.0, 0.0);
  
  /*
  TerrainGround* flag = TerrainGround::createNoThemeGround(currPointList, -1, true);
  flag->setImage(getImg(sprintFlagImg));
  addPGO(flag);
  flag->set_collisions(0x1 << TYPE_TERRAIN, 0x0);
   */
  SprintFlag* flag = new SprintFlag();
  flag->setXY(currPointList.last());
  addPGO(flag);

  flagStartPt = Point2(3000.0, 0.0) + endpoint;
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pirateShipChunk()
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(224.0, 0.0);
  addLevelPt(288.0, 256.0);
  addLevelPt(0.0, 256.0);
  createThemedGroundFromCWPts();

  // terrain ground 2
  clearPtList();
  addLevelPt(1600.0, -96.0);
  addLevelPt(1888.0, -128.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(1888.0, 256.0);
  addLevelPt(1536.0, 256.0);
  createThemedGroundFromCWPts();

  // ship image top left
  clearPtList();
  addLevelPt(800.0, -18.0);
  
  pirateShip = new PirateShip();
  pirateShip->setXY(currPointList.first());
  addPGO(pirateShip);

  // chest top left
  clearPtList();
  addLevelPt(736.0, -146.0);
  
  pirateChest = new PirateChest();
  pirateChest->ship = pirateShip;
  pirateChest->setXY(currPointList.first() + pirateChest->getSize() * 0.5);
  addPGO(pirateChest);

  // water
  clearPtList();
  addLevelPt(240.0, 64.0);
  addLevelPt(1568.0, 64.0);
  addLevelPt(1536.0, 256.0);
  addLevelPt(288.0, 256.0);
  shipWater = createWaterFromCWQuads();

  // corners of area you can stand to trigger win
  clearPtList();
  addLevelPt(640.0, -160.0);
  addLevelPt(1008.0, 32.0);
  pirateWinBox = Box::from_corners(currPointList[0], currPointList[1]);

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pirateFBlocks(Coord1 powerupType, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(888.0, 0.0);
  addLevelPt(1032.0, 64.0);
  addLevelPt(1408.0, -64.0);
  addLevelPt(1664.0, 64.0);
  addLevelPt(2048.0, 0.0);
  addLevelPt(2368.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(2368.0, 208.0);
  addLevelPt(0.0, 208.0);
  createThemedGroundFromCWPts();
  
  // camera top
  clearPtList();
  addLevelPt(336.0, -944.0);
  addLevelPt(496.0, -1096.0);
  addLevelPt(2112.0, -1104.0);
  addLevelPt(2312.0, -944.0);
  addCameraCeilingPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(912.0, -848.0);
  addLevelPt(1024.0, -928.0);
  addLevelPt(1136.0, -880.0);
  addLevelPt(1208.0, -760.0);
  addLevelPt(1248.0, -632.0);
  addLevelPt(1304.0, -504.0);
  addLevelPt(1336.0, -632.0);
  addLevelPt(1384.0, -760.0);
  addLevelPt(1448.0, -872.0);
  addLevelPt(1576.0, -928.0);
  addLevelPt(1688.0, -848.0);
  createCoinsFromCenter();

  // bouncer
  clearPtList();
  addLevelPt(1312.0, -288.0);
  addLevelPt(1376.0, -288.0);
  createBouncersFromCandRC();
  
  // gourdo block
  clearPtList();
  addLevelPt(832.0, -768.0);
  addLevelPt(1664.0, -768.0);
  createItemBlocksFromTL(powerupType);
  
  // enemies
  clearPtList();
  addLevelPt(672.0, 0.0);
  addLevelPt(520.0, 0.0);
  addLevelPt(888.0, 0.0);
  addLevelPt(2208.0, 0.0);
  addLevelPt(2048.0, 0.0);
  addLevelPt(2368.0, 0.0);
  createEnemiesFromBCLine(enemyType);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pirateGourdoLantern(Logical useGems, Logical useRightGourdo, Coord1 lineFlyerType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1024.0, 0.0);
  addLevelPt(1024.0, 312.0);
  addLevelPt(0.0, 312.0);
  createThemedGroundFromCWPts();

  // terrain ground 2
  clearPtList();
  addLevelPt(3560.0, -1848.0);
  addLevelPt(3800.0, -1920.0);
  addLevelPt(4224.0, -1920.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(4224.0, -1672.0);
  addLevelPt(3504.0, -1672.0);
  createThemedGroundFromCWPts();

  // camera bottom
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1280.0, 128.0);
  addLevelPt(1600.0, -936.0);
  addLevelPt(2416.0, -1120.0);
  addLevelPt(3560.0, -1856.0);
  addLevelPt(3800.0, -1920.0);
  addLevelPt(4224.0, -1920.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(496.0, -536.0);
  addLevelPt(1472.0, -1520.0);
  addLevelPt(2200.0, -1800.0);
  addLevelPt(3840.0, -2664.0);
  addLevelPt(4024.0, -2592.0);
  addCameraCeilingPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(1040.0, -496.0);
  addLevelPt(1056.0, -624.0);
  addLevelPt(1088.0, -760.0);
  addLevelPt(1136.0, -888.0);
  addLevelPt(1208.0, -1008.0);
  addLevelPt(1288.0, -1112.0);
  addLevelPt(1384.0, -1208.0);
  addLevelPt(1632.0, -1320.0);
  addLevelPt(1760.0, -1344.0);
  addLevelPt(1896.0, -1344.0);
  addLevelPt(2032.0, -1320.0);
  addLevelPt(2160.0, -1280.0);
  addLevelPt(2424.0, -1296.0);
  addLevelPt(2520.0, -1400.0);
  addLevelPt(2584.0, -1512.0);
  addLevelPt(2648.0, -1640.0);
  addLevelPt(2712.0, -1752.0);
  addLevelPt(2880.0, -1968.0);
  addLevelPt(2992.0, -2040.0);
  addLevelPt(3112.0, -2096.0);
  addLevelPt(3248.0, -2120.0);
  createCoinsFromCenter();
  
  // extra coins or gems
  clearPtList();
  addLevelPt(1032.0, -360.0);
  addLevelPt(1520.0, -1280.0);
  addLevelPt(2328.0, -1240.0);
  addLevelPt(2792.0, -1872.0);
  addLevelPt(3408.0, -2088.0);

  if (useGems == true)
  {
    createGemsFromCenter();
  }
  else
  {
    createCoinsFromCenter();
  }
  
  // gourdo blocks
  clearPtList();
  addLevelPt(736.0, -408.0);
  
  if (useRightGourdo == true)
  {
    addLevelPt(3760.0, -2184.0);
  }
  
  createItemBlocksFromTL(GOURDO_ITEM);
  
  // mohawks
  clearPtList();
  addLevelPt(1576.0, -1136.0);
  addLevelPt(1688.0, -904.0);
  addLevelPt(2160.0, -1512.0);
  addLevelPt(2408.0, -1552.0);
  addLevelPt(3120.0, -1904.0);
  addLevelPt(3184.0, -1728.0);
  createFlyersFromLines(lineFlyerType);
  
  // spike ball
  clearPtList();
  addLevelPt(1152.0, 136.0);
  addLevelPt(1272.0, 136.0);
  createSpikeCirclesFromCandRC();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::pirateWater(Logical useGems, Coord1 lineFlyerType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(648.0, 392.0);
  addLevelPt(1096.0, 448.0);
  addLevelPt(1544.0, 576.0);
  addLevelPt(1688.0, 576.0);
  addLevelPt(1688.0, 784.0);
  addLevelPt(336.0, 552.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1688.0, 576.0);
  addLevelPt(2056.0, 384.0);
  addLevelPt(2304.0, 0.0);
  addLevelPt(2560.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2560.0, 208.0);
  addLevelPt(1688.0, 776.0);
  createThemedGroundFromCWPts();
  
  // camera bottom
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(640.0, 384.0);
  addLevelPt(1096.0, 448.0);
  addLevelPt(1536.0, 576.0);
  addLevelPt(1688.0, 576.0);
  addLevelPt(2056.0, 384.0);
  addLevelPt(2304.0, 0.0);
  addLevelPt(2560.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(584.0, -408.0);
  addLevelPt(2328.0, -424.0);
  addCameraCeilingPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(576.0, -144.0);
  addLevelPt(696.0, -72.0);
  addLevelPt(968.0, -48.0);
  addLevelPt(1104.0, -72.0);
  addLevelPt(1232.0, -104.0);
  addLevelPt(1368.0, -128.0);
  addLevelPt(1496.0, -160.0);
  addLevelPt(1624.0, -184.0);
  addLevelPt(1896.0, -192.0);
  addLevelPt(2032.0, -168.0);
  addLevelPt(2160.0, -112.0);
  addLevelPt(744.0, 176.0);
  addLevelPt(904.0, 272.0);
  addLevelPt(1064.0, 168.0);
  addLevelPt(1072.0, 336.0);
  addLevelPt(1216.0, 312.0);
  addLevelPt(1240.0, 192.0);
  addLevelPt(1520.0, 208.0);
  addLevelPt(1776.0, 136.0);
  addLevelPt(1848.0, 352.0);
  addLevelPt(2048.0, 184.0);
  createCoinsFromCenter();
  
  // extra coins or gems
  clearPtList();
  addLevelPt(832.0, -48.0);
  addLevelPt(1768.0, -192.0);
  addLevelPt(624.0, 144.0);
  addLevelPt(1504.0, 320.0);
  addLevelPt(2032.0, 288.0);
  
  if (useGems == true)
  {
    createGemsFromCenter();
  }
  else
  {
    createCoinsFromCenter();
  }
  
  // flameskulls
  clearPtList();
  addLevelPt(800.0, -192.0);
  addLevelPt(1008.0, -200.0);
  addLevelPt(1560.0, -192.0);
  addLevelPt(1400.0, -296.0);
  addLevelPt(2112.0, -224.0);
  addLevelPt(1864.0, -304.0);
  createFlyersFromLines(lineFlyerType);
  
  // water
  clearPtList();
  addLevelPt(424.0, 64.0);
  addLevelPt(2264.0, 64.0);
  addLevelPt(2264.0, 624.0);
  addLevelPt(464.0, 624.0);
  createWaterFromCWQuads();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameBalloonBucketChunk(Logical useBucket, Logical useCoins, Logical useGems,
                                              Logical useSkulls, Logical useBlocks, Logical useBurningBlocks,
                                              Logical useCage, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(640.0, -96.0);
  addLevelPt(1152.0, -96.0);
  addLevelPt(1400.0, 0.0);
  addLevelPt(1792.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(1792.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();
  
  if (useBucket == true)
  {
    clearPtList();
    addLevelPt(820.0, -238.0);
    
    BalloonBasket* basket = new BalloonBasket();
    basket->setXY(currPointList.first());
    addPGO(basket);
  }
  
  if (useCoins == true)
  {
    // coins
    clearPtList();
    addLevelPt(633.0, -243.0);
    addLevelPt(765.0, -334.0);
    addLevelPt(901.0, -400.0);
    addLevelPt(1026.0, -336.0);
    addLevelPt(1161.0, -233.0);
    createCoinsFromCenter();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(896.0, -168.0);
    addLevelPt(896.0, -488.0);
    createGemsFromCenter();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(792.0, -304.0);
    addLevelPt(896.0, -96.0);
    addLevelPt(1008.0, -304.0);
    createSkullsGroundPts();
  }
  
  if (useBlocks == true)
  {
    // blocks
    clearPtList();
    addLevelPt(640.0, -240.0);
    addLevelPt(736.0, -96.0);
    addLevelPt(1056.0, -240.0);
    addLevelPt(1152.0, -96.0);
    addLevelPt(640.0, -304.0);
    addLevelPt(1152.0, -240.0);
    createBlocksFromTLandBR();
  }
  
  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(720.0, -168.0);
    addLevelPt(824.0, -96.0);
    addLevelPt(968.0, -168.0);
    addLevelPt(1072.0, -96.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(904.0, -96.0);
    createPuppiesGroundPts();
  }
  
  // enemy
  clearPtList();
  addLevelPt(904.0, -96.0);
  addLevelPt(640.0, -96.0);
  addLevelPt(1152.0, -96.0);
  createEnemiesFromBCLine(enemyType);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameBounceChunk(Logical useBurningBlocks, Logical useSkulls,
                                       Logical useBlocks, Logical useGems, Logical useCage)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(520.0, -64.0);
  addLevelPt(544.0, 384.0);
  addLevelPt(0.0, 384.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(992.0, -320.0);
  addLevelPt(1248.0, -320.0);
  addLevelPt(1216.0, -96.0);
  addLevelPt(1248.0, 96.0);
  addLevelPt(1184.0, 384.0);
  addLevelPt(992.0, 384.0);
  addLevelPt(1024.0, 192.0);
  addLevelPt(992.0, -96.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 3
  clearPtList();
  addLevelPt(1664.0, 64.0);
  addLevelPt(1920.0, 0.0);
  addLevelPt(2176.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2176.0, 384.0);
  addLevelPt(1728.0, 384.0);
  createThemedGroundFromCWPts();
  
  // bottom camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(520.0, -64.0);
  addLevelPt(792.0, -224.0);
  addLevelPt(992.0, -320.0);
  addLevelPt(1248.0, -320.0);
  addLevelPt(1480.0, -224.0);
  addLevelPt(1664.0, 64.0);
  addLevelPt(1920.0, 0.0);
  addLevelPt(2176.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // top camera
  clearPtList();
  addLevelPt(789.0, -316.0);
  addLevelPt(838.0, -450.0);
  addLevelPt(923.0, -563.0);
  addLevelPt(1045.0, -642.0);
  addLevelPt(1188.0, -647.0);
  addLevelPt(1306.0, -589.0);
  addLevelPt(1425.0, -480.0);
  addLevelPt(1487.0, -361.0);
  addCameraCeilingPts(currPointList);
  
  // bouncers
  clearPtList();
  addLevelPt(800.0, -160.0);
  addLevelPt(1480.0, -160.0);
  
  for (Coord1 i = 0; i < currPointList.count; i += 1)
  {
    addPGO(TerrainQuad::create_bouncer(currPointList[i], 100.0, 0.75));
  }
  
  // coins
  clearPtList();
  addLevelPt(809.0, -311.0);
  addLevelPt(859.0, -438.0);
  addLevelPt(939.0, -544.0);
  addLevelPt(1301.0, -565.0);
  addLevelPt(1395.0, -469.0);
  addLevelPt(1468.0, -353.0);
  createCoinsFromCenter();

  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(992.0, -416.0);
    addLevelPt(1084.0, -320.0);
    addLevelPt(1084.0, -480.0);
    addLevelPt(1180.0, -320.0);
    addLevelPt(1180.0, -384.0);
    addLevelPt(1248.0, -320.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(992.0, -592.0);
    addLevelPt(1080.0, -320.0);
    addLevelPt(1160.0, -592.0);
    addLevelPt(1248.0, -320.0);
    createSkullsGroundPts();
  }
  
  if (useBlocks == true)
  {
    clearPtList();
    addLevelPt(992.0, -492.0);
    addLevelPt(1080.0, -320.0);
    addLevelPt(1160.0, -492.0);
    addLevelPt(1248.0, -320.0);
    createBlocksFromTLandBR();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1052.0, -617.0);
    addLevelPt(1183.0, -627.0);
    createGemsFromCenter();
  }
  else
  {
    clearPtList();
    addLevelPt(1052.0, -617.0);
    addLevelPt(1183.0, -627.0);
    createCoinsFromCenter();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1120.0, -320.0);
    createPuppiesGroundPts();
  }

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flamePitsChunk(Logical useBurningBlocks, Logical useGems, Logical useCage,
                                     Logical useSkulls, Coord1 lineFlyerType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(512.0, 64.0);
  addLevelPt(448.0, 384.0);
  addLevelPt(0.0, 384.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1008.0, 32.0);
  addLevelPt(1200.0, 32.0);
  addLevelPt(1168.0, 384.0);
  addLevelPt(976.0, 384.0);
  addLevelPt(976.0, 192.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 3
  clearPtList();
  addLevelPt(1664.0, 64.0);
  addLevelPt(1920.0, 0.0);
  addLevelPt(2176.0, 0.0);
  addLevelPt(2176.0, 384.0);
  addLevelPt(1728.0, 384.0);
  createThemedGroundFromCWPts();

  // terrain ground 4
  clearPtList();
  addLevelPt(2720.0, 32.0);
  addLevelPt(2912.0, 32.0);
  addLevelPt(2880.0, 384.0);
  addLevelPt(2688.0, 384.0);
  addLevelPt(2688.0, 192.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 5
  clearPtList();
  addLevelPt(3392.0, 64.0);
  addLevelPt(3648.0, 0.0);
  addLevelPt(3904.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3904.0, 384.0);
  addLevelPt(3456.0, 384.0);
  createThemedGroundFromCWPts();

  // bottom camera
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(256.0, 0.0);
  addLevelPt(512.0, 64.0);
  addLevelPt(1008.0, 32.0);
  addLevelPt(1200.0, 32.0);
  addLevelPt(1664.0, 64.0);
  addLevelPt(1920.0, 0.0);
  addLevelPt(2176.0, 0.0);
  addLevelPt(2720.0, 32.0);
  addLevelPt(2912.0, 32.0);
  addLevelPt(3392.0, 64.0);
  addLevelPt(3648.0, 0.0);
  addLevelPt(3904.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // coins
  clearPtList();
  addLevelPt(456.0, -148.0);
  addLevelPt(560.0, -221.0);
  addLevelPt(826.0, -295.0);
  addLevelPt(957.0, -309.0);
  addLevelPt(1229.0, -304.0);
  addLevelPt(1362.0, -285.0);
  addLevelPt(1610.0, -181.0);
  addLevelPt(1704.0, -94.0);
  addLevelPt(2106.0, -146.0);
  addLevelPt(2217.0, -213.0);
  addLevelPt(2473.0, -298.0);
  addLevelPt(2608.0, -313.0);
  addLevelPt(2878.0, -334.0);
  addLevelPt(3015.0, -328.0);
  addLevelPt(3413.0, -238.0);
  addLevelPt(3527.0, -171.0);
  createCoinsFromCenter();
  
  // chickens
  clearPtList();
  addLevelPt(768.0, -232.0);
  addLevelPt(768.0, 128.0);
  createFlyersFromLines(lineFlyerType);
  
  clearPtList();
  addLevelPt(1464.0, -232.0);
  addLevelPt(1464.0, 128.0);
  createFlyersFromLines(lineFlyerType);

  clearPtList();
  addLevelPt(2408.0, -232.0);
  addLevelPt(2408.0, 128.0);
  createFlyersFromLines(lineFlyerType);

  clearPtList();
  addLevelPt(3128.0, -232.0);
  addLevelPt(3128.0, 128.0);
  createFlyersFromLines(lineFlyerType);

  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(1036.0, -128.0);
    addLevelPt(1132.0, 32.0);
    addLevelPt(2760.0, -48.0);
    addLevelPt(2880.0, 32.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(696.0, -272.0);
    addLevelPt(1112.0, -312.0);
    addLevelPt(1504.0, -232.0);
    addLevelPt(2352.0, -272.0);
    addLevelPt(2760.0, -336.0);
    addLevelPt(3248.0, -280.0);
    createGemsFromCenter();
  }
  else
  {
    clearPtList();
    addLevelPt(696.0, -272.0);
    addLevelPt(1112.0, -312.0);
    addLevelPt(1504.0, -232.0);
    addLevelPt(2352.0, -272.0);
    addLevelPt(2760.0, -336.0);
    addLevelPt(3248.0, -280.0);
    createCoinsFromCenter();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(2064.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1096.0, 32.0);
    addLevelPt(2064.0, 0.0);
    addLevelPt(2816.0, 32.0);
    createSkullsGroundPts();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameSpearGuardsChunk(Logical useBurningBlocks, Logical useGems, Logical useCage,
                                            Logical useSkulls, Logical useBlocks, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(512.0, 0.0);
  addLevelPt(1280.0, -64.0);
  addLevelPt(1800.0, -64.0);
  addLevelPt(2816.0, 64.0);
  addLevelPt(3328.0, 64.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(3328.0, 224.0);
  addLevelPt(0.0, 224.0);
  createThemedGroundFromCWPts();
  
  // coins
  clearPtList();
  addLevelPt(1455.0, -345.0);
  addLevelPt(1519.0, -408.0);
  addLevelPt(1455.0, -473.0);
  addLevelPt(1584.0, -474.0);
  addLevelPt(1584.0, -345.0);
  createCoinsFromCenter();
  
  // enemies
  clearPtList();
  addLevelPt(1000.0, -40.0);
  addLevelPt(520.0, 0.0);
  addLevelPt(1272.0, -64.0);
  addLevelPt(2288.0, 0.0);
  addLevelPt(1800.0, -64.0);
  addLevelPt(2832.0, 64.0);
  createEnemiesFromBCLine(enemyType);
  
  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(1440.0, -224.0);
    addLevelPt(1600.0, -64.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1522.0, -554.0);
    addLevelPt(1520.0, -272.0);
    createGemsFromCenter();
  }
  
  if (useCage == true)
  {
    clearPtList();
    addLevelPt(1520.0, -64.0);
    createPuppiesGroundPts();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1520.0, -224.0);
    createSkullsGroundPts();
  }
  
  if (useBlocks == true)
  {
    clearPtList();
    addLevelPt(1440.0, -224.0);
    addLevelPt(1600.0, -64.0);
    createBlocksFromTLandBR();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameSpikeyStepsChunk(Logical useBurningBlocks, Logical useSkulls,
                                            Logical useBlocks, Logical useGems, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(800.0, 0.0);
  addLevelPt(800.0, -224.0);
  addLevelPt(1536.0, -224.0);
  addLevelPt(1536.0, -448.0);
  addLevelPt(2272.0, -448.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(2272.0, -184.0);
  addLevelPt(368.0, 384.0);
  addLevelPt(0.0, 384.0);
  createThemedGroundFromCWPts();
  
  // enemies
  clearPtList();
  addLevelPt(1096.0, -224.0);
  addLevelPt(936.0, -224.0);
  addLevelPt(1272.0, -224.0);
  addLevelPt(1768.0, -448.0);
  addLevelPt(1536.0, -448.0);
  addLevelPt(1984.0, -448.0);
  createEnemiesFromBCLine(enemyType);
  
  if (useBurningBlocks == true)
  {
    clearPtList();
    addLevelPt(672.0, -128.0);
    addLevelPt(800.0, 0.0);
    addLevelPt(1408.0, -342.0);
    addLevelPt(1536.0, -224.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(736.0, -224.0);
    addLevelPt(1472.0, -448.0);
    createSkullsGroundPts();
  }
  
  if (useBlocks == true)
  {
    clearPtList();
    addLevelPt(544.0, -128.0);
    addLevelPt(672.0, 0.0);
    addLevelPt(672.0, -224.0);
    addLevelPt(800.0, 0.0);
    addLevelPt(1408.0, -448.0);
    addLevelPt(1536.0, -224.0);
    createBlocksFromTLandBR();
    
    if (useGems == true)
    {
      objman.phaseList[PHASE_BG_TERRAIN].getLast()->coinToDropType = RANDOM_GEM;
    }

    // also make some coins
    clearPtList();
    addLevelPt(608.0, -192.0);
    addLevelPt(736.0, -336.0);
    addLevelPt(1472.0, -560.0);
    createCoinsFromCenter();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameFlyer(Logical useBurners, Logical useCages,
                                 Logical useGems, Logical useSkulls,
                                 Coord1 lineFlyerType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(576.0, 0.0);
  addLevelPt(552.0, 192.0);
  addLevelPt(0.0, 192.0);
  createThemedGroundFromCWPts();

  // terrain ground 2
  clearPtList();
  addLevelPt(2080.0, 0.0);
  addLevelPt(2272.0, 0.0);
  addLevelPt(2304.0, 216.0);
  addLevelPt(2072.0, 216.0);
  createThemedGroundFromCWPts();

  // terrain ground 3
  clearPtList();
  addLevelPt(3584.0, 64.0);
  addLevelPt(3776.0, 64.0);
  addLevelPt(3808.0, 232.0);
  addLevelPt(3552.0, 232.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 4
  clearPtList();
  addLevelPt(5440.0, 0.0);
  addLevelPt(6016.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(6016.0, 160.0);
  addLevelPt(5416.0, 160.0);
  createThemedGroundFromCWPts();

  // camera ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(576.0, 0.0);
  addLevelPt(2080.0, 0.0);
  addLevelPt(2272.0, 0.0);
  addLevelPt(3584.0, 64.0);
  addLevelPt(3776.0, 64.0);
  addLevelPt(5440.0, 0.0);
  addLevelPt(6016.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(1120.0, -992.0);
  addLevelPt(1664.0, -1000.0);
  addLevelPt(2784.0, -696.0);
  addLevelPt(3344.0, -632.0);
  addLevelPt(4640.0, -840.0);
  addLevelPt(5168.0, -808.0);
  addCameraCeilingPts(currPointList);

  // coins
  clearPtList();
  addLevelPt(1123.0, -886.0);
  addLevelPt(1256.0, -916.0);
  addLevelPt(1392.0, -925.0);
  addLevelPt(1520.0, -927.0);
  addLevelPt(1661.0, -912.0);
  addLevelPt(2783.0, -460.0);
  addLevelPt(2914.0, -432.0);
  addLevelPt(3053.0, -430.0);
  addLevelPt(3185.0, -431.0);
  addLevelPt(3326.0, -438.0);
  addLevelPt(4653.0, -679.0);
  addLevelPt(4780.0, -726.0);
  addLevelPt(4904.0, -745.0);
  addLevelPt(5042.0, -725.0);
  addLevelPt(5157.0, -660.0);
  createCoinsFromCenter();
  
  if (useCages == true)
  {
    clearPtList();
    addLevelPt(2176.0, 0.0);
    addLevelPt(3688.0, 64.0);
    createPuppiesGroundPts();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(2176.0, -40.0);
    addLevelPt(3688.0, 24.0);
    createGemsFromCenter();
  }
  
  if (useBurners == true)
  {
    clearPtList();
    addLevelPt(2048.0, -256.0);
    addLevelPt(2304.0, 0.0);
    addLevelPt(3552.0, -192.0);
    addLevelPt(3808.0, 64.0);
    createBurningBlocksFromTLandBR();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(2176.0, 0.0);
    addLevelPt(3688.0, 64.0);
    createSkullsGroundPts();
  }
  
  // flyers
  clearPtList();
  addLevelPt(1400.0, -688.0);
  addLevelPt(1408.0, -232.0);
  addLevelPt(3040.0, -560.0);
  addLevelPt(3040.0, -232.0);
  addLevelPt(4568.0, -672.0);
  addLevelPt(4568.0, -272.0);
  createFlyersFromLines(lineFlyerType);
  
  // gourdo block
  clearPtList();
  addLevelPt(640.0, -320.0);
  addLevelPt(5312.0, -320.0);
  createGourdoBlocksFromTL();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameSpikeblocks(Logical useBurners, Logical useCages,
                                       Logical useGems, Logical useSkulls, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(768.0, 88.0);
  addLevelPt(0.0, 88.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1536.0, 0.0);
  addLevelPt(3200.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3200.0, 88.0);
  addLevelPt(1536.0, 88.0);
  createThemedGroundFromCWPts();
  
  // camera ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(3200.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1064.0, -1128.0);
  addLevelPt(2296.0, -1128.0);
  addLevelPt(2752.0, -168.0);
  addCameraCeilingPts(currPointList);

  // fader blocks
  clearPtList();
  addLevelPt(640.0, -256.0);
  addLevelPt(768.0, -128.0);
  addLevelPt(896.0, -576.0);
  addLevelPt(1024.0, -448.0);
  createFadeBlocksFromTLandBR();
  
  // half spikers
  clearPtList();
  addLevelPt(640.0, -128.0);
  addLevelPt(768.0, 0.0);
  addLevelPt(592.0, -48.0);
  addLevelPt(768.0, -256.0);
  addLevelPt(896.0, -128.0);
  addLevelPt(840.0, -328.0);
  addLevelPt(896.0, -384.0);
  addLevelPt(1024.0, -256.0);
  addLevelPt(968.0, -424.0);
  addLevelPt(1088.0, -512.0);
  addLevelPt(1216.0, -384.0);
  addLevelPt(1032.0, -456.0);
  addLevelPt(1216.0, -640.0);
  addLevelPt(1344.0, -512.0);
  addLevelPt(1136.0, -584.0);
  addLevelPt(1344.0, -640.0);
  addLevelPt(1472.0, -512.0);
  addLevelPt(1424.0, -696.0);
  addLevelPt(1920.0, -256.0);
  addLevelPt(2048.0, -128.0);
  addLevelPt(1984.0, -336.0);
  addLevelPt(2048.0, -256.0);
  addLevelPt(2176.0, -128.0);
  addLevelPt(2112.0, -304.0);
  addLevelPt(2048.0, -1000.0);
  addLevelPt(2176.0, -960.0);
  addLevelPt(2112.0, -912.0);
  addLevelPt(2176.0, -896.0);
  addLevelPt(2304.0, -768.0);
  addLevelPt(2376.0, -832.0);
  addLevelPt(2176.0, -768.0);
  addLevelPt(2304.0, -640.0);
  addLevelPt(2368.0, -704.0);
  addLevelPt(2176.0, -640.0);
  addLevelPt(2304.0, -512.0);
  addLevelPt(2360.0, -576.0);
  addLevelPt(2176.0, -512.0);
  addLevelPt(2304.0, -384.0);
  addLevelPt(2376.0, -448.0);
  addLevelPt(2176.0, -384.0);
  addLevelPt(2304.0, -256.0);
  addLevelPt(2352.0, -320.0);
  addLevelPt(2176.0, -256.0);
  addLevelPt(2304.0, -128.0);
  addLevelPt(2368.0, -184.0);
  addLevelPt(2176.0, -128.0);
  addLevelPt(2304.0, 0.0);
  addLevelPt(2352.0, -56.0);
  createHalfSpikeBlocksFromTL_BR_side();
  
  // coins
  clearPtList();
  addLevelPt(720.0, -456.0);
  addLevelPt(728.0, -592.0);
  addLevelPt(768.0, -712.0);
  addLevelPt(856.0, -808.0);
  addLevelPt(944.0, -728.0);
  addLevelPt(1000.0, -832.0);
  addLevelPt(1032.0, -968.0);
  addLevelPt(1152.0, -1032.0);
  addLevelPt(1280.0, -1056.0);
  addLevelPt(1416.0, -1040.0);
  addLevelPt(1544.0, -1008.0);
  addLevelPt(1672.0, -952.0);
  addLevelPt(2480.0, -600.0);
  addLevelPt(2432.0, -552.0);
  addLevelPt(2432.0, -472.0);
  addLevelPt(2496.0, -424.0);
  addLevelPt(2496.0, -352.0);
  addLevelPt(2448.0, -304.0);
  createCoinsFromCenter();
  
  if (useCages == true)
  {
    clearPtList();
    addLevelPt(2816.0, 0.0);
    createPuppiesGroundPts();
  }
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1760.0, -88.0);
    addLevelPt(2816.0, -56.0);
    createGemsFromCenter();
  }
  
  if (useSkulls == true)
  {
    clearPtList();
    addLevelPt(1760.0, -88.0);
    addLevelPt(2816.0, -56.0);
    createSkullsGroundPts();
  }
  
  if (useBurners == true)
  {
    clearPtList();
    addLevelPt(1664.0, -192.0);
    addLevelPt(1856.0, 0.0);
    createBurningBlocksFromTLandBR();
  }

  // enemy
  clearPtList();
  addLevelPt(2816.0, 0.0);
  addLevelPt(2560.0, 0.0);
  addLevelPt(3120.0, 0.0);
  createEnemiesFromBCLine(enemyType);
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::flameSpinners(Logical useBurners, Logical useGems, Coord1 enemyType)
{
  Point2 nextEndpoint;
  
  // terrain ground 1
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(448.0, 216.0);
  addLevelPt(0.0, 216.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 2
  clearPtList();
  addLevelPt(1344.0, -192.0);
  addLevelPt(1856.0, -192.0);
  addLevelPt(1928.0, 168.0);
  addLevelPt(1296.0, 64.0);
  createThemedGroundFromCWPts();
  
  // terrain ground 3
  clearPtList();
  addLevelPt(2928.0, -176.0);
  addLevelPt(3088.0, -256.0);
  addLevelPt(3392.0, -256.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(3392.0, -24.0);
  addLevelPt(2904.0, -32.0);
  createThemedGroundFromCWPts();
  
  // camera ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(384.0, 0.0);
  addLevelPt(1344.0, -192.0);
  addLevelPt(1856.0, -192.0);
  addLevelPt(2928.0, -184.0);
  addLevelPt(3088.0, -256.0);
  addLevelPt(3392.0, -256.0);
  addCameraGroundPts(currPointList);
  
  // camera top
  clearPtList();
  addLevelPt(408.0, -416.0);
  addLevelPt(1336.0, -1032.0);
  addLevelPt(2072.0, -1040.0);
  addLevelPt(2936.0, -624.0);
  addCameraCeilingPts(currPointList);

  // spinners
  clearPtList();
  addLevelPt(768.0, -256.0);
  addLevelPt(1088.0, -192.0);
  addLevelPt(1408.0, -704.0);
  addLevelPt(1728.0, -640.0);
  addLevelPt(2304.0, -320.0);
  addLevelPt(2624.0, -256.0);
  createSpinnersFromTLandBR();
  
  // coins
  clearPtList();
  addLevelPt(966.0, -451.0);
  addLevelPt(986.0, -583.0);
  addLevelPt(1045.0, -709.0);
  addLevelPt(1131.0, -810.0);
  addLevelPt(1242.0, -885.0);
  addLevelPt(1368.0, -936.0);
  addLevelPt(1496.0, -965.0);
  addLevelPt(1634.0, -974.0);
  addLevelPt(1770.0, -968.0);
  addLevelPt(1901.0, -942.0);
  addLevelPt(2029.0, -905.0);
  addLevelPt(2153.0, -846.0);
  addLevelPt(2264.0, -769.0);
  addLevelPt(2355.0, -670.0);
  addLevelPt(2417.0, -550.0);
  createCoinsFromCenter();

  clearPtList();
  addLevelPt(3240.0, -256.0);
  addLevelPt(3088.0, -256.0);
  addLevelPt(3384.0, -256.0);
  createEnemiesFromBCLine(enemyType);
  
  if (useGems == true)
  {
    clearPtList();
    addLevelPt(1632.0, -336.0);
    addLevelPt(1624.0, -976.0);
    createGemsFromCenter();
  }
  
  if (useBurners == true)
  {
    clearPtList();
    addLevelPt(1408.0, -320.0);
    addLevelPt(1536.0, -192.0);
    addLevelPt(1472.0, -448.0);
    addLevelPt(1600.0, -320.0);
    addLevelPt(1536.0, -320.0);
    addLevelPt(1728.0, -192.0);
    createBurningBlocksFromTLandBR();
  }
  
  clearPtList();
  endpoint += nextEndpoint;
}

PizzaGOStd* PhysicsLevelStd::createSpecialObject(Coord2 objectType)
{
  PizzaGOStd* pgo = NULL;
  
  switch (objectType.x)
  {
    case TYPE_PUPPY:
      break;
    case TYPE_PICKUP:
      break;
  }
  
  return pgo;
}

Point3 PhysicsLevelStd::evalObjectsOfInterestPuppy()
{
  // look for every object of interest and move the camera accordingly
  Point1 numObjectsOfInterest = 0.0;
  Point1 runningInterestX = 0.0;
  Point1 highestY = 1000000.0;
  
  for (Coord1 i = 0; i < puppyList.count; ++i)
  {
    if (puppyList[i]->aiState < Billy::ON_PLAYER &&
        objectCloseToPlayerPuppy(puppyList[i]) == true)
    {
      // camTargetX = RM::lerp(player->getX(), puppyList[i]->getX(), 0.5) - RM_WH.x * 0.5;
      numObjectsOfInterest += 1.0;
      runningInterestX += puppyList[i]->getX();
      highestY = std::min(highestY, puppyList[i]->collisionBox().top());
    }
  }
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (objectCloseToPlayerPuppy(enemy) == true)
    {
      numObjectsOfInterest += 1.0;
      runningInterestX += enemy->getX();
      highestY = std::min(highestY, enemy->visibleAABB().top());
    }
  }
  
  return Point3(numObjectsOfInterest, runningInterestX, highestY);
}

Point3 PhysicsLevelStd::evalObjectsOfInterestFree()
{
  // look for every object of interest and move the camera accordingly
  Point1 numObjectsOfInterest = 0.0;
  Point1 runningInterestX = 0.0;
  Point1 highestY = 1000000.0;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (objectCloseToPlayerFree(enemy) == true)
    {
      numObjectsOfInterest += 1.0;
      runningInterestX += enemy->getX();
      highestY = std::min(highestY, enemy->visibleAABB().top());
    }
  }
  
  return Point3(numObjectsOfInterest, runningInterestX, highestY);
}

Logical PhysicsLevelStd::objectCloseToPlayerPuppy(PizzaGOStd* obj)
{
  Point1 partialScreenAhead = player->getX() + RM_WH.x * objectiveDir * 0.5;
  
  if (obj != NULL &&
      obj->getActive() == true &&
      // obj->onRMScreen() == true  &&
      RM::distance_to(obj->getXY(), rmboxCamera.myBox().center()) <= RM_WH.x * 1.1 &&
      std::abs(partialScreenAhead - obj->getX()) < RM_WH.x * 0.55)
  {
    return true;
  }
  
  return false;
}

Logical PhysicsLevelStd::objectCloseToPlayerFree(PizzaGOStd* obj)
{
  Point1 partialScreenAhead = player->getX() + RM_WH.x * playerXVelNorm() * 0.5;
  
  if (obj != NULL &&
      obj->getActive() == true &&
      // obj->onRMScreen() == true  &&
      RM::distance_to(obj->getXY(), rmboxCamera.myBox().center()) <= RM_WH.x * 1.1 &&
      std::abs(partialScreenAhead - obj->getX()) < RM_WH.x * 0.55)
  {
    return true;
  }
  
  return false;
}

Point1 PhysicsLevelStd::getCameraTopY()
{
  Point1 result = getCameraCeilY(rmboxCamera.untranslatedBox().xRange());
  return result;
}

Point1 PhysicsLevelStd::getCameraCeilY(Point2 xRange)
{
  Point1 currHigh = 100000.0;
  // cout << "checking range " << xRange << endl;
  
  // test the leftmost point if necessary
  if (xRange.x <= ceilingCameraPts.first().x ||
      xRange.y <= ceilingCameraPts.first().x)
  {
    currHigh = std::min(currHigh, ceilingCameraPts.first().y);
    // cout << "testing leftmost" << endl;
  }
  
  // test the rightmost point if necessary
  if (xRange.x >= ceilingCameraPts.last().x ||
      xRange.y >= ceilingCameraPts.last().x)
  {
    currHigh = std::min(currHigh, ceilingCameraPts.last().y);
    // cout << "testing rightmost" << endl;
  }
  
  Logical passedLeft = false;
  
  for (Coord1 i = 0; i < ceilingCameraPts.count - 1; ++i)
  {
    Line currLine = Line(ceilingCameraPts[i], ceilingCameraPts[i + 1]);
    
    if (xRange.x >= ceilingCameraPts[i].x &&
        xRange.x <= ceilingCameraPts[i + 1].x)
    {
      // found the leftmost point somewhere in this line, test it
      currHigh = std::min(currHigh, currLine.get_y_at_x(xRange.x));
      passedLeft = true;
    }
    // testing the left edge of a segment that's in camera view
    else if (passedLeft == true)
    {
      currHigh = std::min(currHigh, ceilingCameraPts[i].y);
    }
    
    if (xRange.y >= ceilingCameraPts[i].x &&
        xRange.y <= ceilingCameraPts[i + 1].x)
    {
      // found the rightmost point somewhere in this line, test it and we're done
      currHigh = std::min(currHigh, currLine.get_y_at_x(xRange.y));
      // cout << "return regular right " << currHigh << endl;
      return currHigh;
    }
    // testing the right edge of a segment that's in camera view
    else if (passedLeft == true)
    {
      currHigh = std::min(currHigh, ceilingCameraPts[i + 1].y);
    }
  }
  
  return currHigh;
}

void PhysicsLevelStd::drawCameraTop()
{
  for (Coord1 i = 0; i < ceilingCameraPts.count - 1; ++i)
  {
    Line(ceilingCameraPts[i], ceilingCameraPts[i + 1]).draw(CYAN_SOLID);
  }
  
  rmboxCamera.untranslatedBox().draw_outline(WHITE_SOLID);
  
  Point1 untranslatedCamLeft = rmboxCamera.untranslatedBox().left();
  Point1 untranslatedCamRight = rmboxCamera.untranslatedBox().right();
  // Point1 camGroundY = getCameraBottomY();
  // Line(Point2(untranslatedCamLeft, camGroundY), Point2(untranslatedCamRight, camGroundY)).draw(YELLOW_SOLID);
  // cout << "camGroundY " << camGroundY << endl;
}

void PhysicsLevelStd::starterChunk()
{
  Point2 nextEndpoint;
  
  // terrain ground 1 (bottom left)
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(1080.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addCameraGroundPts(currPointList);
  addLevelPt(1080, 136.0);
  addLevelPt(0.0, 136.0);
  createThemedGroundFromCWPts();
  
  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::bouncerAChunk(Logical useCage, Logical useGem,
                                    Logical useBBlock, Logical useSkull)
{
  Point2 nextEndpoint;
  
  // left ground
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(352.0, 0.0);
  addLevelPt(352.0, 128.0);
  addLevelPt(0.0, 128.0);
  createThemedGroundFromCWPts();
  
  // middle ground sticking up
  clearPtList();
  addLevelPt(1088.0, -384.0);
  addLevelPt(1264.0, -384.0);
  addLevelPt(1264.0, 128.0);
  addLevelPt(1088.0, 128.0);
  createThemedGroundFromCWPts();

  // right ground
  clearPtList();
  addLevelPt(2000.0, 0.0);
  addLevelPt(2352.0, 0.0);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2352.0, 128.0);
  addLevelPt(2000.0, 128.0);
  createThemedGroundFromCWPts();
  
  // cam bot
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(352.0, 0.0);
  addLevelPt(624.0, 128.0);
  addLevelPt(1728.0, 128.0);
  addLevelPt(2000.0, 0.0);
  addLevelPt(2352.0, 0.0);
  addCameraGroundPts(currPointList);
  
  // cam top
  clearPtList();
  addLevelPt(552.0, -416.0);
  addLevelPt(976.0, -592.0);
  addLevelPt(1368.0, -584.0);
  addLevelPt(1808.0, -336.0);
  addCameraCeilingPts(currPointList);
  
  // bouncers
  clearPtList();
  addLevelPt(736.0, 128.0);
  addLevelPt(848.0, 128.0);
  addLevelPt(1616.0, 128.0);
  addLevelPt(1728.0, 128.0);
  createBouncersFromCandRC();
  
  // coins
  clearPtList();
  addLevelPt(336.0, -128.0);
  addLevelPt(432.0, -192.0);
  addLevelPt(544.0, -272.0);
  addLevelPt(648.0, -192.0);
  addLevelPt(744.0, -128.0);
  addLevelPt(808.0, -256.0);
  addLevelPt(872.0, -384.0);
  addLevelPt(976.0, -496.0);
  addLevelPt(1376.0, -496.0);
  addLevelPt(1480.0, -384.0);
  addLevelPt(1544.0, -256.0);
  addLevelPt(1608.0, -128.0);
  addLevelPt(1704.0, -192.0);
  addLevelPt(1808.0, -272.0);
  addLevelPt(1920.0, -192.0);
  addLevelPt(2008.0, -128.0);
  createCoinsFromCenter();
  
  clearPtList();
  addLevelPt(1176.0, -384.0);  // ground point
  
  if (useGem == true)
  {
    createGemsFromGroundPts();
  }
  else
  {
    createCoinsFromGroundPts();
  }
  
  if (useSkull == true)
  {
    createSkullsGroundPts();
  }
  else if (useBBlock == true)
  {
    addLevelPt(176.0, 48.0);
    createBurningBlocksFromBCandWH();
  }
  else if (useCage == true)
  {
    createPuppiesGroundPts();
  }
  
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::puppyDoghouseChunk()
{
  Point2 nextEndpoint;

  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(2048.0, 0.0);
  addCameraGroundPts(currPointList);
  nextEndpoint += currPointList.last() - endpoint;
  addLevelPt(2048.0, 184.0);
  addLevelPt(0.0, 184.0);
  createThemedGroundFromCWPts();
  
  clearPtList();
  addLevelPt(832.0, 0.0);
  addLevelPt(832.0, -136.0);
  addLevelPt(904.0, -192.0);
  addLevelPt(1048.0, -192.0);
  addLevelPt(1088.0, -128.0);
  addLevelPt(1088.0, -0.0);
  
  TerrainGround* house = TerrainGround::createNoThemeGround(currPointList, -1, true);
  house->setImage(getImgSet(doghouseSet)[0]);
  addPGO(house);
  
  Decoration* houseFront = new Decoration(endpoint + Point2(832.0, 0.0),
                                          getImgSet(doghouseSet)[1]);
  houseFront->phase = PHASE_FG_TERRAIN;
  houseFront->type = TYPE_TERRAIN;
  addPGO(houseFront);
  
  clearPtList();
  
  puppyHousePt = Point2(900.0, -8.0) + endpoint;
  puppyStartPt = Point2(1408.0, -8.0) + endpoint;

  clearPtList();
  endpoint += nextEndpoint;
}

void PhysicsLevelStd::addCameraGroundPts(DataList<Point2>& groundPts)
{
  /*
  // the bottom of the riverman camera is this far below the ground
  Point1 camGroundPad = 124.0;
  Line currLowLine(groundPts.first() + camGroundPad, groundPts.last() + camGroundPad);
  
  // the points of interest are the two endpoints,
  // and any point that falls below the line connecting them
  for (Coord1 i = 0; i < groundPts.count; ++i)
  {
    Point2 currPt = groundPts[i] + Point2(0.0, camGroundPad);
    
    // always add the endpoints
    if (i == 0 || i == groundPts.last_i())
    {
      groundCameraPts.add(currPt);
      continue;
    }
    
    // see if this point is below the current low line
    if (currLowLine.is_point_below_line(currPt) == true)
    {
      groundCameraPts.add(currPt);
    }
  }
   */
  
  // the bottom of the riverman camera is this far below the ground
  Point1 camGroundPad = 124.0;

  for (Coord1 i = 0; i < groundPts.count; ++i)
  {
    Point2 camPt = groundPts[i] + Point2(0.0, camGroundPad);
    groundCameraPts.add(camPt);
  }
}

void PhysicsLevelStd::addCameraCeilingPts(DataList<Point2>& ceilPts)
{
  Point1 camCeilPad = -16.0;
  
  // the first two points of the ceiling cam should just get us to the first
  // segment
  if (ceilingCameraPts.count == 1)
  {
    ceilingCameraPts.add(Point2(ceilPts[0].x - 1.0, 99999.0));
  }
  
  for (Coord1 i = 0; i < ceilPts.count; ++i)
  {
    ceilingCameraPts.add(ceilPts[i] + Point2(0.0, camCeilPad));
  }
  
  ceilingCameraPts.add(Point2(ceilPts.last().x + 1.0, 99999.0));
}

void PhysicsLevelStd::drawCameraGround()
{
  for (Coord1 i = 0; i < groundCameraPts.count - 1; ++i)
  {
    Line(groundCameraPts[i], groundCameraPts[i + 1]).draw(BLUE_SOLID);
  }
  
  rmboxCamera.untranslatedBox().draw_outline(WHITE_SOLID);
  
  Point1 untranslatedCamLeft = rmboxCamera.untranslatedBox().left();
  Point1 untranslatedCamRight = rmboxCamera.untranslatedBox().right();
  // Point1 camGroundY = getCameraBottomY();
  // Line(Point2(untranslatedCamLeft, camGroundY), Point2(untranslatedCamRight, camGroundY)).draw(YELLOW_SOLID);
  // cout << "camGroundY " << camGroundY << endl;
}

void PhysicsLevelStd::puppyFound(Billy* puppy)
{
  puppiesToFind--;
  // gotScore(350);
  
  if (puppiesToFind <= 0)
  {
    objectiveDir *= -1.0;

    directionChanged();
    /*
    addTextEffectStd(LSTRINGS[102], PLAY_CENTER);
    
    for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
         enemyNode != NULL;
         enemyNode = enemyNode->next)
    {
      PizzaGO* enemy = enemyNode->data;
      enemy->puppyTurn(objectiveDir);
    }
    
    for (LinkedListNode<PizzaGO*>* allyNode = objman.allyList.first;
         allyNode != NULL;
         allyNode = allyNode->next)
    {
      PizzaGO* ally = allyNode->data;
      ally->puppyTurn(objectiveDir);
    }
     */
  }
  
  puppyWasFound();
  
}

void PhysicsLevelStd::puppyReturned(Billy* puppy)
{
  puppiesToReturn--;
  
  if (puppiesToReturn <= 0)
  {
    levelState = LEVEL_PLAY;  // this hack forces the win to trigger
    winLevel();
  }
}

/*
Point1 PhysicsLevelStd::getCameraGroundY(Point1 xVal)
{
  if (xVal <= groundCameraPts.first().x)
  {
    return groundCameraPts.first().y;
  }
  else if (xVal >= groundCameraPts.last().x)
  {
    return groundCameraPts.last().y;
  }
  
  for (Coord1 i = 0; i < groundCameraPts.count - 1; ++i)
  {
    if (xVal >= groundCameraPts[i].x &&
        xVal <= groundCameraPts[i + 1].x)
    {
      return Line(groundCameraPts[i], groundCameraPts[i + 1]).get_y_at_x(xVal);
    }
  }
  
  return 0.0;
}
 */

Point1 PhysicsLevelStd::getCameraGroundY(Point2 xRange)
{
  Point1 currLow = -100000.0;
  
  // test the leftmost point if necessary
  if (xRange.x <= groundCameraPts.first().x ||
      xRange.y <= groundCameraPts.first().x)
  {
    currLow = std::max(currLow, groundCameraPts.first().y);
    // cout << "testing leftmost" << endl;
  }
  
  // test the rightmost point if necessary
  if (xRange.x >= groundCameraPts.last().x ||
      xRange.y >= groundCameraPts.last().x)
  {
    currLow = std::max(currLow, groundCameraPts.last().y);
    // cout << "testing rightmost" << endl;
  }
  
  Logical passedLeft = false;
  
  for (Coord1 i = 0; i < groundCameraPts.count - 1; ++i)
  {
    Line currLine = Line(groundCameraPts[i], groundCameraPts[i + 1]);
    
    if (xRange.x >= groundCameraPts[i].x &&
        xRange.x <= groundCameraPts[i + 1].x)
    {
      // found the leftmost point somewhere in this line, test it
      currLow = std::max(currLow, currLine.get_y_at_x(xRange.x));
      passedLeft = true;
    }
    // testing the left edge of a segment that's in camera view
    else if (passedLeft == true)
    {
      currLow = std::max(currLow, groundCameraPts[i].y);
    }
    
    if (xRange.y >= groundCameraPts[i].x &&
        xRange.y <= groundCameraPts[i + 1].x)
    {
      // found the rightmost point somewhere in this line, test it and we're done
      currLow = std::max(currLow, currLine.get_y_at_x(xRange.y));
      // cout << "return regular right " << currLow << endl;
      return currLow;
    }
    // testing the right edge of a segment that's in camera view
    else if (passedLeft == true)
    {
      currLow = std::max(currLow, groundCameraPts[i + 1].y);
    }
  }
  
  return currLow;
}

Point1 PhysicsLevelStd::getCameraBottomY()
{
  Point1 result = getCameraGroundY(Point2(rmboxCamera.untranslatedBox().left(),
                                          rmboxCamera.untranslatedBox().right()));
  return result;
}

void PhysicsLevelStd::addAction(ActionEvent* ae)
{
  objman.addAction(ae);
}

void PhysicsLevelStd::addFrontAction(ActionEvent* ae)
{
  objman.addFrontAction(ae);
}

float32 PhysicsLevelStd::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                                       const b2Vec2& normal, float32 fraction)
{
  if (fixture == player->fixture)
  {
    return -1;
  }
  
  lastRaycastFixture = fixture;
  lastRaycastPt = b2dpt_to_world(point);
  lastRaycastNormal = b2dpt_to_world(normal);
  lastRaycastFraction = fraction;
  
  return lastRaycastFraction;
}

void PhysicsLevelStd::BeginContact(b2Contact* contact)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();

  if (userData1 == NULL || userData2 == NULL)
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }

  PizzaGOStd* pgo1 = static_cast<PizzaGOStd*>(userData1);
  PizzaGOStd* pgo2 = static_cast<PizzaGOStd*>(userData2);
}

void PhysicsLevelStd::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}

void PhysicsLevelStd::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();

  if (userData1 == NULL || userData2 == NULL)
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }

  Physical* phys1 = static_cast<Physical*>(userData1);
  Physical* phys2 = static_cast<Physical*>(userData2);
  
  Point1 finalImpulse = b2dsca_to_world(std::abs(impulse->normalImpulses[0]));
  
  if (impulse->count == 2)
  {
    finalImpulse = std::max(finalImpulse, b2dsca_to_world(std::abs(impulse->normalImpulses[1])));
  }
  
  Coord1 phys1Category = contact->GetFixtureA()->GetFilterData().categoryBits;
  Coord1 phys2Category = contact->GetFixtureB()->GetFilterData().categoryBits;
  
  // both are PGOs, do a regular collision callback
  if ((phys1Category != (0x1 << TYPE_DEBRIS)) &&
      (phys2Category != (0x1 << TYPE_DEBRIS)))
  {
    PizzaGOStd* pgo1 = static_cast<PizzaGOStd*>(userData1);
    PizzaGOStd* pgo2 = static_cast<PizzaGOStd*>(userData2);

    Logical impactLargeEnough = finalImpulse > 100.0 ||
        contact->GetFixtureA()->IsSensor() == true ||
        contact->GetFixtureB()->IsSensor() == true;

    if (pgo1->lifeState != RM::REMOVE &&
        pgo2->lifeState != RM::REMOVE &&
        impactLargeEnough == true)
    {
      pgo1->collidedPhysical(contact, pgo2, finalImpulse);
      pgo2->collidedPhysical(contact, pgo1, finalImpulse);
    }
  }

  // possibly dstruct particle 1
  if (phys1Category == (0x1 << TYPE_DEBRIS))
  {
    particleCollision(static_cast<PhysPolyParticle*>(userData1), finalImpulse);
  }

  // possibly dstruct particle 2
  if (phys2Category == (0x1 << TYPE_DEBRIS))
  {
    particleCollision(static_cast<PhysPolyParticle*>(userData2), finalImpulse);
  }
}

void PhysicsLevelStd::EndContact(b2Contact* contact)
{

}

PizzaPlayer* PhysicsLevelStd::findPlayer(PizzaGOStd* pgo1, PizzaGOStd* pgo2)
{
  if (pgo1 == player) return static_cast<PizzaPlayer*>(pgo1);
  if (pgo2 == player) return static_cast<PizzaPlayer*>(pgo2);
  return NULL;
}

template <typename PGOType>
PGOType* PhysicsLevelStd::findObj(PizzaGOStd* obj1, PizzaGOStd* obj2, Coord1 type)
{
  if (obj1->type == type) return static_cast<PGOType*>(obj1);
  if (obj2->type == type) return static_cast<PGOType*>(obj2);
  return NULL;
}

void PhysicsLevelStd::particleCollision(PhysPolyParticle* particle, Point1 impulse)
{  
  if (impulse >= 75.0)
  {
    particle->wasDestroyed = true;
  }
}

void PhysicsLevelStd::spine_event_callback(spAnimationState* state, spEventType eventType,
                                      spTrackEntry* entry, spEvent* event)
{
  // cout << "spine callback with event type " << eventType << endl;
}

// ========================= PhysLevelChunkStd =========================== //

PhysLevelChunkStd::PhysLevelChunkStd() :
  level(NULL),

  startPt(),

  myEnemyList()
{
  
}

void PhysLevelChunkStd::createCoinsFromPts()
{
  
}

void PhysLevelChunkStd::addCameraGroundFromPts()
{
  
}

// ========================= TestLevel =========================== //

TestLevel::TestLevel() :
  PhysicsLevelStd()
{
  
}

void TestLevel::initRes()
{
  PhysicsLevelStd::initRes();
  
  createWorld(RM_BOX);
  
  player = new PizzaPlayer();
  player->level = this;
  player->load();
  
//  ParticleTestSkull* skull = new ParticleTestSkull();
//  skull->setXY(Point2(350.0, 160.0));
//  addPGO(skull);
  
  levelState = LEVEL_PLAY;
}

void TestLevel::createWorld(const Box& setWorldBox)
{
  TerrainQuad* ground = TerrainQuad::create_ground_TL(Point2(0, 290.0),
                                                      Point2(RM_WH.x * 3.0, 32.0),
                                                      NULL);
  addPGO(ground);
}

// ========================= GladiatorLevel =========================== //

GladiatorLevel::GladiatorLevel() :
  PhysicsLevelStd(),

  enemyWeights(NUM_ENEMY_TYPES, true),
  enemyCounts(NUM_ENEMY_TYPES, true),
  groundY(0.0),

  numWaves(16),
  currWave(0),
  currSide(1),
  currCombo(0),

  worldSpawnPad(128.0),
  camSpawnPad(128.0)
{
  // 0-out weights
  for (Coord1 i = 0; i < enemyWeights.count; ++i) enemyWeights[i] = 0.0;
  for (Coord1 i = 0; i < enemyCounts.count; ++i) enemyCounts[i].set(1, 1);
  
  // musicFilename = "gladiator$.ima4";
  // musicFilename.int_replace('$', (Pizza::currLevelDef.y % 3) + 1);
}

void GladiatorLevel::preload()
{
  background = createRegionalBackground(levelRegion, this);
}

void GladiatorLevel::loadRes()
{
  background->load();
  
  loadEnabledEnemies();
}

void GladiatorLevel::unloadRes()
{
  // if any of the enemies need special free instructions make sure to
  // do the same in PlaneLevel
}

void GladiatorLevel::initSublevel()
{
  createWorldStd();
  worldCreated();
  
  // this is to fix the camera in the beginning
  createWave();
  currWave++;
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void GladiatorLevel::createWorld(const Box& setWorldBox)
{
  worldBox = setWorldBox;
  
  groundY = 416.0;
  Point1 wallThickness = 128.0;
  
  /*
  TerrainQuad* ground = TerrainQuad::create_ground_TL(Point2(0.0, groundY),
                                                      Point2(worldBox.width(), 128.0),
                                                      NULL);
  addPGO(ground);
   */
  DataList<Point2> groundPts;
  groundPts.add(Point2(0.0, groundY));
  groundPts.add(Point2(worldBox.width(), groundY));
  groundPts.add(Point2(worldBox.width(), groundY + wallThickness));
  groundPts.add(Point2(0.0, groundY + wallThickness));
  
  TerrainGround* ground = TerrainGround::createThemedGround(groundPts, backgroundType, true);
  ground->phase = PHASE_FG_TERRAIN;
  addPGO(ground);

  invisibleWallsStd(worldBox);
}

void GladiatorLevel::createWorldStd()
{
  createWorld(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 3.0, RM_WH.y * 3.0));
  
  // ground->set_friction(1.0);
  player->load();
  startStanding(Point2(192.0, getGroundY()));
  
  updateCamera(true);
}

void GladiatorLevel::enemyDefeated(PizzaGOStd* enemy)
{
  /*
  currCombo++;
  
  Coord1 baseScore = enemy->basePoints;
  Coord1 bonusScore = (Point1) baseScore * ((Point1) (currCombo - 1) / 8.0);
  Coord1 totalScore = baseScore + bonusScore;
  gotScore(totalScore);
  
  if (currCombo >= 3)
  {
    BonusText* comboNum = new BonusText(String2("x$", '$', currCombo),
                                        ResourceManager::font, enemy->collisionCircle().xy);
    comboNum->scale.set(0.5, 0.5);
    comboNum->init();
    addAction(comboNum);
  }
   */
}

void GladiatorLevel::pizzaDamaged()
{
  currCombo = 0;
}

void GladiatorLevel::drawBGTerrain()
{
  // background->drawLandscapeTilesStd(imgsetCache[bgLayerSet].last(), 1.0);
}

void GladiatorLevel::updateCamera(Logical snapIntoPlace)
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = closestEnemyX();
  
  updateZoomStd(PLAYER_CAM_TOP_PAD_STD);

  updateFacingStd(enemyAnywhere, closestX);
  enemyCameraXStd(enemyAnywhere, closestX, snapIntoPlace);
  rmboxCamera.xy.y = 0.0;

  // not clamping the left side of rmboxCam--clamping the left side of the SCREEN
  Point1 screenLeftOffset = rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
  Point1 screenRightOffset = rmcamboxToScreen(Point2(RM_BOX.right(), 0.0)).x;
  rmboxCamera.clampHorizontal(Point2(worldBox.left() + screenLeftOffset,
                                     worldBox.right() + screenRightOffset));

}

void GladiatorLevel::updateLevelAI()
{
  if (levelState == LEVEL_PLAY)
  {
    if (currWave >= numWaves && enemyActive() == false)
    {
      winLevel();
    }
    else if (lostToDMGStd() == true)
    {
      loseLevel();
    }
  }
  else
  {
    return;
  }
  
  if (enemyActive() == true)
  {
    return;
  }

  Box camBox = rmboxCamera.myBox();
  Box pizzaBox = player->collisionBox();
  
  Logical leftAvailable = camBox.left() - camSpawnPad > worldBox.left() + worldSpawnPad;
  Logical rightAvailable = camBox.right() + camSpawnPad < worldBox.right() - worldSpawnPad;
  
  if (leftAvailable == false && rightAvailable == false) return;
  
  Coord1 botRand = leftAvailable ? 0 : 1;
  Coord1 topRand = rightAvailable ? 1 : 0;
  Coord1 randSide = RM::randi(botRand, topRand);
  
  currSide = randSide;
  
  if (currWave < numWaves)
  {
    createWave();
    currWave++;
  }
}

Point1 GladiatorLevel::spawnXStd(Coord1 side, Point1 lerpMin)
{
  Point1 createPad = 128.0;
  Box camBox = rmboxCamera.myBox();
  Point2 xRange(0.0, 0.0);
  
  if (side == 0)
  {
    // create on left
    xRange = Point2(worldBox.left() + createPad, camBox.left() - createPad);
    xRange.y = RM::lerp(xRange, 1.0 - lerpMin);
  }
  else
  {
    // create on right
    xRange = Point2(camBox.right() + createPad, worldBox.right() - createPad);
    xRange.x = RM::lerp(xRange, lerpMin);
  }
  
  Point1 result = RM::randf(xRange);
  return result;
}

Point1 GladiatorLevel::spawnYStd(Coord1 index, Coord1 number)
{
  return spawnYStd(index, number, Point2(getGroundY() - 24.0, getGroundY() + 24.0));
}

Point1 GladiatorLevel::spawnYStd(Coord1 index, Coord1 number, Point2 bounds)
{
  return RM::lerp(bounds.x, bounds.y, (index + 1.0) / (number + 1.0));
}

void GladiatorLevel::randomWave()
{
  Coord1 enemyType = RM::random_index_weighted(enemyWeights, randGen);
  Coord1 numEnemies = randGen.randi(enemyCounts[enemyType].x, enemyCounts[enemyType].y);
  createGroup(enemyType, numEnemies);
}

void GladiatorLevel::createGroup(Coord1 enemyType, Coord1 count)
{
  for (Coord1 i = 0; i < count; ++i)
  {
    createSingle(enemyType, Coord2(i, count));
  }
}

PizzaGOStd* GladiatorLevel::createSingle(Coord1 enemyType, Coord2 countData)
{
  PizzaGOStd* skele = createEnemy(enemyType);
  
  Point2 flyBoundsStd(RM_BOX.top() + RM_WH.y * 0.0, RM_BOX.top() + RM_WH.y * 0.5);
  Point2 groundPosStd(spawnXStd(currSide), spawnYStd(countData.x, countData.y));
  Point2 groundPosFar(spawnXStd(currSide, 0.75), spawnYStd(countData.x, countData.y));
  Point2 flyPosStd(spawnXStd(currSide), spawnYStd(countData.x, countData.y, flyBoundsStd));
  Point2 position = groundPosStd;

  skele->setXY(position);
  skele->myPlatformLine.x = Point2(worldBox.left(), getGroundY(worldBox.left()));
  skele->myPlatformLine.y = Point2(worldBox.right(), getGroundY(worldBox.right()));
  addPGO(skele);
  
  return skele;
}

void GladiatorLevel::createWaveNewEnemyStd()
{
  switch (currWave)
  {
    case 0:
      createGroup(enemiesEnabled[0]);
      break;
    case 1:
      createGroup(enemiesEnabled[0]);
      break;
    case 2:
      createGroup(enemiesEnabled[2], 2);
      break;
    case 3:
      createGroup(enemiesEnabled[0]);
      break;
    case 4:
      setProb(enemiesEnabled[0], 2.0, 1, 1);
      setProb(enemiesEnabled[1], 1.0, 1, 1);
      setProb(enemiesEnabled[2], 1.5, 1, 1);
      randomWave();
      break;
    case 6:
      createGroup(enemiesEnabled[0]);
      createGroup(enemiesEnabled[1], 2);
      break;
    case 9:
      setProb(enemiesEnabled[0], 1.5, 1, 2);
      setProb(enemiesEnabled[1], 2.0, 1, 2);
      setProb(enemiesEnabled[2], 1.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(enemiesEnabled[0], 1, 3);
      setProb(enemiesEnabled[1], 1, 3);
      setProb(enemiesEnabled[2], 1, 3);
      randomWave();
      break;
    case 15:
      createGroup(enemiesEnabled[0], 1);
      createGroup(enemiesEnabled[2], 3);
      break;
      
    default:
      randomWave();
      break;
  }
}

void GladiatorLevel::createWaveNewHarderStd()
{
  switch (currWave)
  {
    case 0:
      createGroup(enemiesEnabled[0]);
      break;
    case 1:
      createGroup(enemiesEnabled[1]);
      break;
    case 2:
      createGroup(enemiesEnabled[2], 2);
      break;
    case 3:
      createGroup(enemiesEnabled[2]);
      break;
    case 4:
      setProb(enemiesEnabled[0], 2.0, 1, 1);
      setProb(enemiesEnabled[1], 1.0, 1, 1);
      setProb(enemiesEnabled[2], 1.5, 1, 1);
      randomWave();
      break;
    case 6:
      createGroup(enemiesEnabled[0], 1);
      createGroup(enemiesEnabled[1], 1);
      break;
    case 9:
      setProb(enemiesEnabled[0], 1.5, 1, 2);
      setProb(enemiesEnabled[1], 2.0, 1, 2);
      setProb(enemiesEnabled[2], 1.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(enemiesEnabled[0], 2, 3);
      setProb(enemiesEnabled[1], 2, 3);
      setProb(enemiesEnabled[2], 2, 3);
      randomWave();
      break;
    case 15:
      createGroup(enemiesEnabled[0], 2);
      createGroup(enemiesEnabled[1], 1);
      createGroup(enemiesEnabled[2], 1);
      break;
      
    default:
      randomWave();
      break;
  }
}

PizzaGOStd* GladiatorLevel::createSingle(Coord1 enemyType, Coord1 groupID)
{
  return createSingle(enemyType, Coord2(0, 1));
}

Point1 GladiatorLevel::getGroundY(Point1 xCoord)
{
  return groundY;
}

void GladiatorLevel::setProb(Coord1 enemyType, Point1 weight)
{
  enemyWeights[enemyType] = weight;
}

void GladiatorLevel::setProb(Coord1 enemyType, Coord1 lowCount, Coord1 highCount)
{
  enemyCounts[enemyType].set(lowCount, highCount);
}

void GladiatorLevel::setProb(Coord1 enemyType, Point1 weight, Coord1 lowCount, Coord1 highCount)
{
  enemyWeights[enemyType] = weight;
  enemyCounts[enemyType].set(lowCount, highCount);
}

void GladiatorLevel::placeSticky(Point1 courseXPercent)
{
  // createSticky(Point2(RM::lerp(worldBox.left(), worldBox.right(), courseXPercent), getGroundY() - 32.0));
}

// ========================= GLLevelUnarmed =========================== //

GLLevelUnarmed::GLLevelUnarmed() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  
  numWaves = 5;
}

void GLLevelUnarmed::preload()
{
  background = new IntroLevelBackground();
  background->level = this;
}

void GLLevelUnarmed::createWorldStd()
{
  createWorld(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 2.0, RM_WH.y * 3.0));
  
  // ground->set_friction(1.0);
  player->load();
  startStanding(Point2(192.0, getGroundY()));
  
  updateCamera(true);
}

void GLLevelUnarmed::worldCreated()
{
  // disable the normal ground from drawing, the ground is built in to the bg
  objman.phaseList[PHASE_FG_TERRAIN].getFirst()->setAlpha(0.0);
}

void GLLevelUnarmed::createWave()
{
  // spear side, spear vert
  switch (currWave)
  {
    case 0:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 1:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 2:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 3:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 4:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 5:
      createGroup(GL_ENEMY_SPEARMAN_SIDE);
      break;
    default:
      setProb(GL_ENEMY_SPEARMAN_UNARMED, 1.0, 1, 1);
      randomWave();
      break;
  }
}

// ========================= GLLevelSpearmen =========================== //

GLLevelSpearmenSide::GLLevelSpearmenSide() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

void GLLevelSpearmenSide::createWave()
{
  // spear side, spear vert
  switch (currWave)
  {
    case 0:
      createGroup(GL_ENEMY_SPEARMAN_SIDE);
      break;
    case 1:
      createGroup(GL_ENEMY_SPEARMAN_UNARMED);
      break;
    case 2:
      createGroup(GL_ENEMY_SPEARMAN_SIDE);
      break;
    case 3:
      setProb(GL_ENEMY_SPEARMAN_UNARMED, 2.0, 1, 1);
      setProb(GL_ENEMY_SPEARMAN_SIDE, 2.0, 1, 1);
      randomWave();
      break;
    case 7:
      setProb(GL_ENEMY_SPEARMAN_SIDE, 2.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(GL_ENEMY_SPEARMAN_UNARMED, 1, 2);
      setProb(GL_ENEMY_SPEARMAN_SIDE, 2, 2);
      randomWave();
      break;
    default:
      randomWave();
      break;
  }
}

// ========================= GLLevelSpearmenVert =========================== //

GLLevelSpearmenVert::GLLevelSpearmenVert() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

void GLLevelSpearmenVert::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(enemiesEnabled[0]);
      break;
    case 1:
      createGroup(enemiesEnabled[0]);
      break;
    case 2:
      createGroup(enemiesEnabled[2], 2);
      break;
    case 3:
      createGroup(enemiesEnabled[0]);
      break;
    case 4:
      setProb(enemiesEnabled[0], 2.0, 1, 1);
      setProb(enemiesEnabled[1], 1.0, 1, 1);
      setProb(enemiesEnabled[2], 1.5, 1, 1);
      randomWave();
      break;
    case 6:
      createGroup(enemiesEnabled[0]);
      createGroup(enemiesEnabled[1], 2);
      break;
    case 9:
      setProb(enemiesEnabled[0], 1.5, 1, 2);
      setProb(enemiesEnabled[1], 2.0, 1, 2);
      setProb(enemiesEnabled[2], 1.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(enemiesEnabled[0], 1, 3);
      setProb(enemiesEnabled[1], 1, 3);
      setProb(enemiesEnabled[2], 1, 3);
      randomWave();
      break;
    case 15:
      createGroup(enemiesEnabled[0], 1);
      createGroup(enemiesEnabled[2], 3);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ========================= GLLevelBombbat =========================== //

GLLevelBombbat::GLLevelBombbat() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

// ========================= GLLevelBombbat2 =========================== //

GLLevelBombbat2::GLLevelBombbat2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

// ========================= GLLevelSpider =========================== //

GLLevelSpider::GLLevelSpider() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}


// ========================= GLLevelSpider2 =========================== //

GLLevelSpider2::GLLevelSpider2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

// ========================= GLLevelCaveman =========================== //

GLLevelCaveman::GLLevelCaveman() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

// ========================= GLLevelCaveman2 =========================== //

GLLevelCaveman2::GLLevelCaveman2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
}

// ========================= GLLevelEnt =========================== //

GLLevelEnt::GLLevelEnt() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_ENT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
}


// ========================= GLLevelEnt2 =========================== //

GLLevelEnt2::GLLevelEnt2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_ENT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}


// ========================= GLLevelDonut =========================== //

GLLevelDonut::GLLevelDonut() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
}


// ========================= GLLevelDonut2 =========================== //

GLLevelDonut2::GLLevelDonut2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}


// ========================= GLLevelUtensil =========================== //

GLLevelUtensil::GLLevelUtensil() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
}


// ========================= GLLevelUtensil2 =========================== //

GLLevelUtensil2::GLLevelUtensil2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_ENT);
}


// ========================= GLLevelGiant =========================== //

GLLevelGiant::GLLevelGiant() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
}


// ========================= GLLevelGiant2 =========================== //

GLLevelGiant2::GLLevelGiant2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_ENT);
}


// ========================= GLLevelDozer =========================== //

GLLevelDozer::GLLevelDozer() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}


// ========================= GLLevelDozer2 =========================== //

GLLevelDozer2::GLLevelDozer2() :
  GladiatorLevel()
{
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_DONUT);
}

// ========================= GLLevelScientist =========================== //

GLLevelScientist::GLLevelScientist() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
}


// ========================= GLLevelScientist2 =========================== //

GLLevelScientist2::GLLevelScientist2() :
  GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}

// ========================= GLLevelAlien =========================== //

GLLevelAlien::GLLevelAlien() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_GIANT);
}


// ========================= GLLevelAlien2 =========================== //

GLLevelAlien2::GLLevelAlien2() :
  GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
  enemiesEnabled.add(GL_ENEMY_ENT);
}

// ========================= GLLevelWisp =========================== //

GLLevelWisp::GLLevelWisp() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
}


// ========================= GLLevelWisp2 =========================== //

GLLevelWisp2::GLLevelWisp2() :
  GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_DONUT);
}


// ========================= GLLevelChip =========================== //

GLLevelChip::GLLevelChip() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}


// ========================= GLLevelChip2 =========================== //

GLLevelChip2::GLLevelChip2() :
  GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_WISP);
}

// ========================= GLLevelCupid =========================== //

GLLevelCupid::GLLevelCupid() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}


// ========================= GLLevelCupid2 =========================== //

GLLevelCupid2::GLLevelCupid2() :
  GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_WISP);
}

// ========================= GLLevelSpiny =========================== //

GLLevelSpiny::GLLevelSpiny() :
GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
}


// ========================= GLLevelSpiny2 =========================== //

GLLevelSpiny2::GLLevelSpiny2() :
GladiatorLevel()
{

  
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_WISP);
}

// ========================= GLLevelReaper =========================== //

GLLevelReaper::GLLevelReaper() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
}


// ========================= GLLevelReaper2 =========================== //

GLLevelReaper2::GLLevelReaper2() :
  GladiatorLevel()
{
  
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_WISP);
}

// ========================= GLLevelTest =========================== //

GLLevelTest::GLLevelTest() :
  GladiatorLevel(),

  testEnemyType(GL_ENEMY_HAMMER_BROS)
{
  
  enemiesEnabled.add((EnemyType) testEnemyType);
}

void GLLevelTest::createWave()
{
  switch (currWave)
  {
    case 0:
      // setProb(testEnemyType, 2.0, 1, 1);
      setProb(GL_ENEMY_HAMMER_BROS, 1.0, 1, 1);
      setProb(GL_ENEMY_TRIANGLE_BROS, 1.0, 1, 1);
      setProb(GL_ENEMY_DYNAMITE_BROS, 1.0, 1, 1);
      randomWave();
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== JengaLevel ================================ //

JengaLevel::JengaLevel() :
  PhysicsLevelStd(),

  groundY(0.0)
{
  background = new UFOBackground();
  background->level = this;
}

void JengaLevel::preload()
{
  
}

void JengaLevel::loadRes()
{
  cout << "JengaLevel::loadRes()" << endl;
  
  /*JACRUM algorithmic block*/
  ResourceManager::loadImage(redrockCenterImg, "redrock_center");
  ResourceManager::loadImage(redrockTerrainBordersImg, "redrock_border");
  /*END JACRUM*/
  
  background->load();
}

void JengaLevel::unloadRes()
{
  
}

void JengaLevel::initSublevel()
{
  createWorld();
  worldCreated();
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void JengaLevel::createWorldStd(const Box& setWorldBox, Logical withGround)
{
  worldBox = setWorldBox;
  
  if (withGround == true)
  {
    groundY = 416.0;
    Point1 wallThickness = 128.0;
    
    TerrainQuad* ground = TerrainQuad::create_ground_TL(Point2(0.0, groundY),
                                                        Point2(worldBox.width(), 32.0),
                                                        NULL);
    addPGO(ground);
  }
  
  invisibleWallsStd(worldBox);

  // startStanding(Point2(192.0, getGroundY()));
  player->setXY(192.0, 0.0);
  updateCamera(true);
}

void JengaLevel::updateStatus()
{
  // winLevel() or loseLevel()
}

void JengaLevel::pizzaDamaged()
{
  
}

void JengaLevel::drawBGTerrain()
{
  
}

void JengaLevel::updateCamera(Logical snapIntoPlace)
{
  Logical puffballExists = objman.phaseList[PHASE_ALLY_STD].count != 0;
  
  if (puffballExists == true)
  {
    PizzaGOStd* puffball = objman.phaseList[PHASE_ALLY_STD].getFirst();

    // will zoom out so it can see the furthest of these points:
    // 1. top of pizza and puff
    // 2. left and right of pizza and puff
    Point1 top = std::min(player->collisionBox().top() + 64.0, puffball->collisionCircle().top() - 64.0);
    Point1 camCeiling = deviceScreenBox().top();
    Point1 diff = top - camCeiling;
    Point1 topZoom = 1.0;
    
    if (diff < 0.0)
    {
      topZoom = diff / (deviceScreenBox().height() * 1.0);
      topZoom = 1.0 / (1.0 + -topZoom);
    }
    
    Point1 leftEdge = std::min(puffball->getX(), player->getX()) - 64.0;
    Point1 rightEdge = std::max(puffball->getX(), player->getX()) + 64.0;
    
    Point1 finalLeft = leftEdge + rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
    Point1 finalRight = rightEdge + rmcamboxToScreen(Point2(RM_BOX.right(), 0.0)).x;
    
    Point1 zoom = deviceScreenBox().wh.x / (finalRight - finalLeft);
    zoom = std::min(zoom, 1.0);
    zoom = std::min(zoom, topZoom);
    rmboxCamera.setZoom(zoom);
    
    Point1 xTarget = RM::lerp(finalLeft, finalRight, 0.5) - RM_WH.x * 0.5;
    rmboxCamera.xy.x = RM::flatten(rmboxCamera.xy.x, xTarget, RM::timePassed() * playerTXVel() * 1.2);
    rmboxCamera.xy.y = 0.0;

    rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);

    player->setFacing((player->getX() <= puffball->getX()) ? 1 : -1);
  }
  // this is mainly for debug if there is no puffball yet
  else
  {
    Logical enemyAnywhere = enemyActive();
    Point1 closestX = closestEnemyX();
    
    updateZoomStd(PLAYER_CAM_TOP_PAD_STD);
    updateFacingStd(enemyAnywhere, closestX);
    enemyCameraXStd(enemyAnywhere, closestX, snapIntoPlace);
    rmboxCamera.xy.y = 0.0;

    // not clamping the left side of rmboxCam--clamping the left side of the SCREEN
    Point1 screenLeftOffset = rmcamboxToScreen(Point2(RM_BOX.left(), 0.0)).x;
    Point1 screenRightOffset = rmcamboxToScreen(Point2(RM_BOX.right(), 0.0)).x;
    rmboxCamera.clampHorizontal(Point2(worldBox.left() + screenLeftOffset,
                                       worldBox.right() + screenRightOffset));

  }
}

Point1 JengaLevel::getGroundY(Point1 xCoord)
{
  return groundY;
}

void JengaLevel::updateLevelAI()
{
  
}

// ==================================== JengaLevelPyramid ================================ //

void JELevelPyramid::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 3.0, RM_WH.x * 3.0, RM_WH.y * 4.0));

  clearPtList();
  addLevelPt(880.0, 272.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1232.0, 272.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1584.0, 272.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1936.0, 272.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(2288.0, 272.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1232.0, 128.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1584.0, 128.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1936.0, 128.0);
  addLevelPt(352.0, 144.0);
  addLevelPt(1584.0, -16.0);
  addLevelPt(352.0, 144.0);
  createBlocksFromTLandWH();
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1759.0, -56.0));
  addPGO(puff);
}

// ==================================== JELevelPusher ================================ //

void JELevelPusher::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 2.0, RM_WH.y * 3.0));
  
  Point2 blockPts[] =
  {
    Point2(1000.0, 192.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockPts[i],
                                                        Point2(102.0, 224.0), NULL);
    block->crushValue = 500.0;
    addPGO(block);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1144.0, 384.0));
  addPGO(puff);
}

// ==================================== JELevelClouds ================================ //

void JELevelClouds::createWorld()
{
  // doesn't use regular ground
  createWorldStd(Box(0.0, -RM_WH.y * 3.0, RM_WH.x * 4.5, RM_WH.y * 4.0), false);
  
  Point2 blockPts[] =
  {
    Point2(920.0, 280.0),
    Point2(1264.0, 144.0),
    Point2(1616.0, 8.0),
    Point2(2672.0, 112.0),
    Point2(3016.0, -24.0),
    Point2(3368.0, -160.0)
  };
  
  Point2 blockSizes[] =
  {
    Point2(72.0, 136.0),
    Point2(72.0, 272.0),
    Point2(72.0, 408.0),
    Point2(72.0, 136.0),
    Point2(72.0, 272.0),
    Point2(72.0, 408.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockPts[i], blockSizes[i], NULL);
    block->crushValue = 5000.0;
    addPGO(block);
  }
  
  Point2 staticBlockPts[] =
  {
    Point2(0.0, 416.0),
    Point2(920.0, 416.0),
    Point2(1264.0, 416.0),
    Point2(1616.0, 416.0),
    Point2(2672.0, 248.0),
    Point2(3016.0, 248.0),
    Point2(3368.0, 248.0)
  };
  
  Point2 staticBlockSizes[] =
  {
    Point2(568.0, 128.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0)
  };

  for (Coord1 i = 0; i < RM::count(staticBlockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_ground_TL(staticBlockPts[i], staticBlockSizes[i], NULL);
    addPGO(block);
  }

  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(3400.0, -192.0));
  addPGO(puff);
}

// ==================================== JELevelColumns ================================ //

void JELevelColumns::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 2.0, RM_WH.y * 3.0));
  
  Point2 blockPts[] =
  {
    Point2(920.0, 136.0),
    Point2(1264.0, -144.0),
    Point2(1616.0, -424.0)
  };
  
  Point2 blockSizes[] =
  {
    Point2(72.0, 280.0),
    Point2(72.0, 560.0),
    Point2(72.0, 840.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockPts[i], blockSizes[i], NULL);
    block->crushValue = 5000.0;
    addPGO(block);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1648.0, -456.0));
  addPGO(puff);
}

// ==================================== JELevelBombs ================================ //

void JELevelBombs::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 2.0, RM_WH.y * 3.0));
  
  Point2 bombPts[] =
  {
    Point2(856.0, 376.0),
    Point2(1336.0, 384.0),
  };
  
  for (Coord1 i = 0; i < RM::count(bombPts); ++i)
  {
    JengaBomb* bomb = new JengaBomb();
    bomb->setXY(bombPts[i]);
    addPGO(bomb);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1104.0, 376.0));
  addPGO(puff);
}

// ==================================== JELevelBombfield ================================ //

void JELevelBombfield::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 2.0, RM_WH.y * 3.0));
  
  Point2 blockPts[] =
  {
    Point2(1072.0, 288.0),
    Point2(1344.0, 144.0),
    Point2(1576.0, 288.0)
  };
  
  Point2 blockSizes[] =
  {
    Point2(72.0, 128.0),
    Point2(72.0, 272.0),
    Point2(72.0, 128.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockPts[i], blockSizes[i], NULL);
    block->crushValue = 5000.0;
    addPGO(block);
  }
  
  Point2 bombPts[] =
  {
    Point2(824.0, 380.0),
    Point2(1108.0, 256.0),
    Point2(1616.0, 260.0),
    Point2(1932.0, 388.0)
  };
  
  for (Coord1 i = 0; i < RM::count(bombPts); ++i)
  {
    JengaBomb* bomb = new JengaBomb();
    bomb->setXY(bombPts[i]);
    addPGO(bomb);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1376.0, 108.0));
  addPGO(puff);
}

// ==================================== JELevelBombfield ================================ //

void JELevelBombAsym::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 3.0, RM_WH.y * 3.0));
  
  Point2 blockPts[] =
  {
    Point2(888.0, 208.0)
  };
  
  Point2 blockSizes[] =
  {
    Point2(72.0, 208.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockPts[i], blockSizes[i], NULL);
    block->crushValue = 5000.0;
    addPGO(block);
  }
  
  Point2 bombPts[] =
  {
    Point2(924.0, 172.0),
    Point2(1356.0, 380.0)
  };
  
  for (Coord1 i = 0; i < RM::count(bombPts); ++i)
  {
    JengaBomb* bomb = new JengaBomb();
    bomb->setXY(bombPts[i]);
    addPGO(bomb);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(1204.0, 380.0));
  addPGO(puff);
}

// ==================================== JELevelBombfield ================================ //

void JELevelAerials::createWorld()
{
  createWorldStd(Box(0.0, -RM_WH.y * 2.0, RM_WH.x * 3.0, RM_WH.y * 3.0));
  
  Point2 blockPts[] =
  {
    Point2(752.0, 256.0),
    Point2(1248.0, 112.0),
    Point2(1392.0, 120.0),
    Point2(1840.0, -40.0),
    Point2(2000.0, 344.0),
    Point2(2200.0, -160.0),
    Point2(2544.0, -240.0)
  };
  
  Point2 blockSizes[] =
  {
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 72.0),
    Point2(72.0, 656.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_ground_TL(blockPts[i], blockSizes[i], NULL);
    
    if (i == 3 || i == 6)
    {
      block->setBreakaway();
    }
    
    addPGO(block);
  }
  
  Point2 bombPts[] =
  {
    Point2(1052.0, 380.0),
    Point2(1428.0, 84.0),
    Point2(2036.0, 312.0),
    Point2(2236.0, -192.0)
  };
  
  for (Coord1 i = 0; i < RM::count(bombPts); ++i)
  {
    JengaBomb* bomb = new JengaBomb();
    bomb->setXY(bombPts[i]);
    addPGO(bomb);
  }
  
  JengaPuffball* puff = new JengaPuffball();
  puff->setXY(Point2(2584.0, -276.0));
  addPGO(puff);
}

// ==================================== AngryLevel ================================ //

AngryLevel::AngryLevel() :
  PhysicsLevelStd(),

  timeAllowed(60.0)
{
  objman.bgTerrainList.ensure_capacity(256);
  
  musicFilename = "puppy.ima4";
}

void AngryLevel::preload()
{
  background = createRegionalBackground(levelRegion, this);
  background->foregroundDisabled = true;
}

// overrides
void AngryLevel::loadRes()
{
  loadEnabledEnemies();
  loadGenericChunkObjects();
  
  loadSpine(angrySkullA, "angry_skull_a");
  loadSpine(angrySkullB, "angry_skull_b");
  loadSpine(angrySkullC, "angry_skull_c");
  loadSpine(angrySkullFunA, "angry_skull_special_a");
  loadSpine(angrySkullFunB, "angry_skull_special_b");
  loadSpine(angrySkullFunC, "angry_skull_special_c");

  background->load();
}

void AngryLevel::unloadRes()
{
  
}

void AngryLevel::initSublevel()
{
  endpoint = Point2(0.0, 416.0);
  
  starterChunk();
  createLevel();
  // addTextEffectStd(LSTRINGS[101], Point2(PLAY_CX, PLAY_CY));
  
  // worldBox = Box(0.0, -RM_WH.y * 2.0, endpoint.x, RM_WH.y * 3.0);
  // invisibleWallsStd(worldBox);
  
  player->load();
  startStanding(Point2(656.0, 416.0));
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void AngryLevel::createLevel()
{
  // createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void AngryLevel::updateCamera(Logical snapIntoPlace)
{
  updateCameraFree(snapIntoPlace);
}

void AngryLevel::updateLevelAI()
{
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  if (skullsCreated >= 1 &&
      skullsDestroyed >= skullsCreated)
  {
    winLevel();
  }
  else if (lostToFallingStd() == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void AngryLevel::drawHUD()
{
  PhysicsLevelStd::drawHUD();
  
  // time
  Point1 displayTime = activeGameplayData->currTime;
  Point2 camTR = deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-8.0, 24.0);
  String2 timeStr("$");
  timeStr.time_string('$', displayTime, 1, false);
  getFont(futura130Font)->draw_in_box(timeStr, camTR, 0.6, 128.0, HANDLE_TR, WHITE_SOLID);
  
  // draw remaining skulls left
  String2 skullsLeftStr;
  skullsLeftStr.int_string(skullsCreated - skullsDestroyed);
  getFont(futura130Font)->draw(skullsLeftStr, deviceSafeBox().norm_pos(HANDLE_TR), Point2(0.6, 0.6),
                               0.0, HANDLE_TR, RED_SOLID);

  if (skullsCreated - skullsDestroyed > 0)
  {
    Coord1 skullDir = closestSkullDir();
    String2 skullDirStr = "<-- Closest skull";
    
    if (skullDir == 1)
    {
      skullDirStr = "Closest skull -->";
    }
    
    Point2 camBC = deviceSafeBox().norm_pos(HANDLE_BC);
    getFont(futura130Font)->draw_in_box(skullDirStr,
                                        camBC + Point2(0.0, -32.0), 0.75, 256.0, HANDLE_BC, CYAN_SOLID);
  }
}

Coord1 AngryLevel::closestSkullDir()
{
  Coord1 dir = 0;
  
  Point1 closestDistance = 100000.0;
  PizzaGOStd* closestSkull = NULL;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->getActive() == false ||
        enemy->value != AN_ENEMY_SKULL)
    {
      continue;
    }
    
    Point1 currDistance = std::abs(enemy->getX() - player->getX());
    
    if (currDistance < closestDistance)
    {
      closestDistance = currDistance;
      closestSkull = enemy;
    }
  }
  
  if (closestSkull != NULL)
  {
    dir = RM::sign(closestSkull->getX() - player->getX());
  }
  
  return dir;
}

Coord1 AngryLevel::calcWinBonus()
{
  return 250 * levelID;
}

void AngryLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== ANLevel0

ANLevel0::ANLevel0() :
  AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void ANLevel0::createLevel()
{
  flameBalloonBucketChunk(false, true, false, true, false, false, false, ENEMY_NONE);
  flameBounceChunk(false, true, false, false, false);
  flameSpearGuardsChunk(false, false, false, true, true, GL_ENEMY_SPEARMAN_VERT);
  angryToppleTower(true, false);
}

// ==================================== ANLevel1

ANLevel1::ANLevel1() :
AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void ANLevel1::createLevel()
{
  angrySkyhenge(true, false);
  pirateWater(false, PU_ENEMY_CHICKEN);
  puppySpikeFlight(LEVEL_TYPE_ANGRY, false, GL_ENEMY_SPEARMAN_SIDE);
}

// ==================================== ANLevel2

ANLevel2::ANLevel2() :
AngryLevel()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void ANLevel2::createLevel()
{
  flameFlyer(false, false, false, true, PU_ENEMY_FLAMESKULL);
  angryToppleTower(true, false);
  bouncerAChunk(false, false, false, true);
  angrySpinners(true, false, false, false, PU_ENEMY_CHICKEN);
}

// ==================================== ANLevel3

ANLevel3::ANLevel3() :
AngryLevel()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void ANLevel3::createLevel()
{
  angrySpikeblock(true);
  flameBalloonBucketChunk(false, false, false, true, true, false, false, ENEMY_NONE);
  sprintJumpChoice(false);
  flamePitsChunk(false, false, false, true, PU_ENEMY_MOHAWK);
}

// ==================================== ANLevel4

ANLevel4::ANLevel4() :
AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}

void ANLevel4::createLevel()
{
  angryPit(true, false, false, ENEMY_NONE);
  sprintCavemen(false, false, true, false, GL_ENEMY_CAVEMAN);
  sprintBombtower(true, false);
}

// ==================================== ANLevel5

ANLevel5::ANLevel5() :
AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_WISP);

}

void ANLevel5::createLevel()
{
  puppySpikeFlight(LEVEL_TYPE_ANGRY, false, GL_ENEMY_UTENSIL);
  spikeyMountainChunk(LEVEL_TYPE_ANGRY, GL_ENEMY_UTENSIL, GL_ENEMY_WISP);
  angrySkyhenge(true, false);
  flameBounceChunk(false, true, false, false, false);
}

// ==================================== ANLevel6

ANLevel6::ANLevel6() :
AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void ANLevel6::createLevel()
{
  flameSpikeyStepsChunk(false, true, false, false, GL_ENEMY_UTENSIL);
  sprintCavemen(false, false, true, false, GL_ENEMY_SPEARMAN_VERT);
  cutleryChunk(LEVEL_TYPE_ANGRY, GL_ENEMY_SPEARMAN_VERT, GL_ENEMY_SPEARMAN_VERT);
  angryBouncePyramid(true, PU_ENEMY_CHICKEN);
}

// ==================================== ANLevel7

ANLevel7::ANLevel7() :
AngryLevel()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
}

void ANLevel7::createLevel()
{
  angryMagicPyramid(true, PU_ENEMY_FLAMESKULL, GL_ENEMY_CAVEMAN);
  sprintSpearmanPitA(false, GL_ENEMY_SPEARMAN_UNARMED);
  sprintSpearmanPitA(false, GL_ENEMY_CAVEMAN);
  flameBalloonBucketChunk(false, false, false, true, true, false, false, GL_ENEMY_BOMBBAT);
  sprintSpearmanPitA(false, GL_ENEMY_ALIEN);
  angryCrossing(true, false, false, false);
}

// ==================================== ANLevel8

ANLevel8::ANLevel8() :
AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_REAPER);
}

void ANLevel8::createLevel()
{
  angrySpikeblock(true);
  angryPit(true, false, false, GL_ENEMY_WISP);
  angryMagicPyramid(true, PU_ENEMY_MOHAWK, GL_ENEMY_SPINY);
  angryMagicPyramid(true, PU_ENEMY_FLAMESKULL, GL_ENEMY_REAPER);
}

// ==================================== ANLevel9

ANLevel9::ANLevel9() :
AngryLevel()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_CHIP);

}

void ANLevel9::createLevel()
{
  angrySpinners(true, false, false, false, PU_ENEMY_FLAMESKULL);
  sprintBombtower(true, false);
  angrySkyhenge(true, false);
  angryToppleTower(true, false);
  angryBouncePyramid(true, GL_ENEMY_CHIP);
  angrySpinners(true, false, false, false, PU_ENEMY_FLAMESKULL);
}


// ==================================== ANLevelTest

ANLevelTest::ANLevelTest() :
  AngryLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void ANLevelTest::createLevel()
{
  angryPit(true, false, false, ENEMY_NONE);
//  sprintCavemen(TYPE_ENEMY, GL_ENEMY_CAVEMAN);
  // angryPit(true, true, false, false);
}


// ==================================== SumoLevel ================================ //

SumoLevel::SumoLevel() :
  PhysicsLevelStd(),

  groundY(0.0)
{
  
}

void SumoLevel::preload()
{
  background = createRegionalBackground(levelRegion, this);
}

void SumoLevel::loadRes()
{
  background->load();
}

void SumoLevel::unloadRes()
{
  
}

void SumoLevel::initSublevel()
{
  createWorld();
  worldCreated();
  
  player->createLightsaber();
  
//  rmboxCamera.xy.x = player->getX();
//  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
  updateCamera(true);
}

void SumoLevel::updateLevelAI()
{
  if (levelState != LEVEL_PLAY) return;
  
  Coord1 bossCount = 0;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->getActive() == true &&
        enemy->type == TYPE_SUMO_BOSS)
    {
      bossCount++;
      continue;
    }
  }
  
  if (bossCount == 0)
  {
    winLevel();
  }
  else if (lostToFallingStd() == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void SumoLevel::drawBGTerrain()
{
  worldBox.draw_solid(WHITE_SOLID);
}

void SumoLevel::drawAfterObjects()
{
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->getActive() == false)
    {
      continue;
    }
    
    enemy->drawLightsaber();
  }
  
  player->drawLightsaber();
}

void SumoLevel::createBossSabersFromCurrPoints(Point1 thickness)
{
  Coord1 sabersInSet = currPointList.count / 2;

  for (Coord1 i = 0; i < sabersInSet; ++i)
  {
    boss->lightsaberFixtures.add(boss->create_box_fixture_center(boss->body, currPointList[i],
        Point2(currPointList[i + sabersInSet].x, thickness),
        currPointList[i + sabersInSet].y));
    boss->lightsaberFixtures.last()->SetUserData(boss);
    boss->set_collisions(boss->lightsaberFixtures.last(),
                         1 << TYPE_BOSS_LIGHTSABER,
                         (0x1 << TYPE_PLAYER) |
                         (0x1 << TYPE_PLAYER_LIGHTSABER));
  }
}


void SumoLevel::updateCamera(Logical snapIntoPlace)
{
  updateCameraSumo(snapIntoPlace);
}

Point1 SumoLevel::getGroundY(Point1 xCoord)
{
  return groundY;
}

// ==================================== SumoLevelEasy ================================ //

void SumoLevelEasy::loadRes()
{
  SumoLevel::loadRes();
}

void SumoLevelEasy::createWorld()
{
  clearPtList();
  addLevelPt(0.0, 368.0);
  addLevelPt(80.0, 440.0);
  addLevelPt(880.0, 440.0);
  addLevelPt(928.0, 416.0);
  addLevelPt(976.0, 440.0);
  addLevelPt(1816.0, 440.0);
  addLevelPt(1896.0, 368.0);
  addLevelPt(1896.0, 536.0);
  addLevelPt(0.0, 536.0);
  createThemedGroundFromCWPts();

  player->load();
  startStanding(Point2(656.0, 416.0));
  
  /*
  clearPtList();
  addLevelPt(1736.0, 216.0);
  boss = new SumoBoss();
  boss->setXY(currPointList.last());
  addPGO(boss);
  
  // create boss lightsaber
  Point2 saberSize = Point2(256.0, 16.0);
  Point2 saberCenter = Point2(-boss->getWidth() - saberSize.x * 0.5, -saberSize.y * 0.5);
  boss->lightsaberFixture = boss->create_box_fixture_center(boss->body, saberCenter, saberSize, 0.0);
  boss->set_collisions(boss->lightsaberFixture, 1 << TYPE_BOSS_LIGHTSABER, 0xffff & (~(0x1 << TYPE_TERRAIN)));

  // lightsaber crossbar
  Point2 crossbarSize = saberSize * Point2(0.25, 1.0);
  b2Fixture* saberCross = boss->create_box_fixture_center(boss->body,
      saberCenter + Point2(saberSize.x * 0.25, 0.0), crossbarSize, HALF_PI);
  boss->set_collisions(saberCross, 1 << TYPE_BOSS_LIGHTSABER, 0xffff & (~(0x1 << TYPE_TERRAIN)));
   */
  
  for (Coord1 i = 0; i < 2; ++i)
  {
    clearPtList();
    addLevelPt(1736.0, 216.0);
    boss = new SumoBoss();
    boss->wh *= 0.5;
    boss->setXY(currPointList.last() + Point2(0.0, -boss->getWidth() * 1.25));
    addPGO(boss);
    
    // create boss lightsaber
    Point2 saberSize = Point2(128.0, 16.0);
    Point2 saberCenter = Point2(-boss->getWidth() - saberSize.x * 0.5, -saberSize.y * 0.5);
    boss->lightsaberFixtures.add(boss->create_box_fixture_center(boss->body, saberCenter, saberSize, 0.0));
    boss->lightsaberFixtures.first()->SetUserData(boss);
    boss->set_collisions(boss->lightsaberFixtures.last(),
                         1 << TYPE_BOSS_LIGHTSABER, 0xffff & (~(0x1 << TYPE_TERRAIN)));
  }

  clearPtList();
  addLevelPt(0.0, 440.0);
  addLevelPt(1896.0, 440.0);
  addCameraGroundPts(currPointList);
}

// ==================================== SumoLevelGraveyard ================================ //

void SumoLevelGraveyard::loadRes()
{
  SumoLevel::loadRes();

  // for sumo graves
  loadImage(breakableObjA, "grave_a", HANDLE_C);
  loadImage(breakableObjB, "grave_b", HANDLE_C);
  loadImage(breakableObjC, "grave_c", HANDLE_C);
}

void SumoLevelGraveyard::createWorld()
{
  // graves
  clearPtList();
  addLevelPt(104.0, 16.0);  // start grave a
  addLevelPt(1308.0, 4.0);
  addLevelPt(1680.0, 80.0);  // start grave b
  addLevelPt(2988.0, -88.0);
  addLevelPt(328.0, 40.0);  // start grave c
  addLevelPt(1876.0, 68.0);
  addLevelPt(2688.0, -4.0);
  
  // create graves, first so they draw behind the background
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    Coord1 graveIndex = 0;
    
    if (i >= 2)
    {
      graveIndex = 1;
    }
    
    if (i >= 4)
    {
      graveIndex = 2;
    }
    
    Image* graveImg = getImg(breakableObjA + graveIndex);
    TerrainQuad* grave = TerrainQuad::create_gravestone_BC(currPointList[i], graveImg);
    // grave->dmgOverlayImg = getImg(crackImg);
    
    if (i == 0 || i == 3)
    {
      grave->crushesPhysical = false;
    }
    
    addPGO(grave);
  }
  
  // create ground contour
  clearPtList();
  addLevelPt(0.0, 0.0);
  addLevelPt(180.0, 12.0);
  addLevelPt(420.0, 44.0);
  addLevelPt(616.0, 16.0);
  addLevelPt(848.0, -32.0);
  addLevelPt(1172.0, -32.0);
  addLevelPt(1440.0, 12.0);
  addLevelPt(1648.0, 76.0);
  addLevelPt(1876.0, 68.0);
  addLevelPt(2012.0, -4.0);
  addLevelPt(2204.0, -52.0);
  addLevelPt(2356.0, -52.0);
  addLevelPt(2592.0, 4.0);
  addLevelPt(2792.0, -40.0);
  addLevelPt(2932.0, -112.0);
  addLevelPt(3072.0, -132.0);
  addCameraGroundPts(currPointList);

  addLevelPt(3072.0, 264.0);
  addLevelPt(0.0, 264.0);
  createThemedGroundFromCWPts();

  // set player
  clearPtList();
  addLevelPt(960.0, -32.0);
  player->load();
  startStanding(currPointList.last());

  // create boss
  clearPtList();
  addLevelPt(2284.0, -252.0);
  boss = new SumoBoss();
  boss->setXY(currPointList.last());
  boss->setMaxHP(10.0);
  addPGO(boss);
   
  // lightsaber
  Point2 saberSize = Point2(256.0, 16.0);
  Point2 saberCenter = Point2(-boss->getWidth() - saberSize.x * 0.5, -saberSize.y * 0.5);
  boss->lightsaberFixtures.add(boss->create_box_fixture_center(boss->body, saberCenter, saberSize, 0.0));
  boss->lightsaberFixtures.first()->SetUserData(boss);
  boss->set_collisions(boss->lightsaberFixtures.first(), 1 << TYPE_BOSS_LIGHTSABER,
                       0xffff & (~(0x1 << TYPE_TERRAIN)));
  
  // invisible walls
  clearPtList();
  addLevelPt(192.0, 0.0);
  addLevelPt(2880.0, 264.0);
  worldBox = Box::from_corners(currPointList.first(), currPointList.last());
  worldBox.grow_up(4096.0);
  invisibleWallsStd(worldBox, 128.0);
  
  // background objects
  clearPtList();
  addLevelPt(336.0, 32.0);
  addLevelPt(776.0, -16.0);
  addLevelPt(1496.0, 32.0);
  addLevelPt(2040.0, -8.0);
  addLevelPt(2352.0, -48.0);
  addLevelPt(2552.0, 0.0);
  addLevelPt(528.0, 40.0);
  addLevelPt(1424.0, 40.0);
  addLevelPt(2184.0, -32.0);

  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    Coord1 layer = i >= 6 ? BG_LAYER_FG : BG_LAYER_PF;
    // Coord1 layer = BG_LAYER_FG;
    
    background->createSpineObj(currPointList[i], bgObject6 + i, "wind", layer, RM::randf(0.8, 1.2));
  }
}

// ==================================== SumoLevelTrike ================================ //

void SumoLevelTrike::loadRes()
{
  SumoLevel::loadRes();

  // for blocks
  loadImage(breakableObjA, "stone_block_128", HANDLE_C);
  
  loadImage(sumoBossImg, "temp_boss_trike", Point2(261.0, 227.0));
}

void SumoLevelTrike::createWorld()
{
  background->foregroundDisabled = true;

  // blocks
  clearPtList();
  addLevelPt(960.0, 768.0);
  addLevelPt(1088.0, 752.0);
  addLevelPt(1216.0, 792.0);
  addLevelPt(1344.0, 768.0);
  addLevelPt(1472.0, 776.0);
  addLevelPt(1600.0, 768.0);
  addLevelPt(1728.0, 816.0);
  addLevelPt(1856.0, 768.0);
  addLevelPt(1984.0, 776.0);
  
  // create blocks, first so they draw behind the background
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    TerrainQuad* block = TerrainQuad::create_trike_block_TL(currPointList[i], getImg(breakableObjA));
    addPGO(block);
  }
  
  // invisible wall box
  clearPtList();
  addLevelPt(128.0, -712.0);
  addLevelPt(2944.0, -712.0);
  addLevelPt(2944.0, 4096.0);
  addLevelPt(128.0, 4096.0);
  addPGO(TerrainGround::createNoThemeGround(currPointList, backgroundType, true));

  // left starting ground
  clearPtList();
  addLevelPt(128.0, 328.0);
  addLevelPt(192.0, 512.0);
  addLevelPt(256.0, 576.0);
  addLevelPt(384.0, 640.0);
  addLevelPt(768.0, 640.0);
  addLevelPt(960.0, 768.0);
  addLevelPt(960.0, 1024.0);
  addLevelPt(832.0, 1152.0);
  addLevelPt(512.0, 1280.0);
  addLevelPt(256.0, 1408.0);
  addLevelPt(128.0, 1728.0);
  createThemedGroundFromCWPts();

  // right starting ground
  clearPtList();
  addLevelPt(2944.0, 328.0);
  addLevelPt(2944.0, 1728.0);
  addLevelPt(2816.0, 1408.0);
  addLevelPt(2560.0, 1280.0);
  addLevelPt(2240.0, 1152.0);
  addLevelPt(2112.0, 1024.0);
  addLevelPt(2112.0, 768.0);
  addLevelPt(2304.0, 640.0);
  addLevelPt(2688.0, 640.0);
  addLevelPt(2816.0, 576.0);
  addLevelPt(2880.0, 512.0);
  createThemedGroundFromCWPts();
  
  // left block in lava
  clearPtList();
  addLevelPt(512.0, 4096.0);
  addLevelPt(640.0, 3520.0);
  addLevelPt(768.0, 3520.0);
  addLevelPt(896.0, 3648.0);
  addLevelPt(896.0, 4096.0);
  createThemedGroundFromCWPts();

  // center block in lava
  clearPtList();
  addLevelPt(1280.0, 4096.0);
  addLevelPt(1360.0, 3744.0);
  addLevelPt(1488.0, 3616.0);
  addLevelPt(1624.0, 3664.0);
  addLevelPt(1728.0, 4096.0);
  createThemedGroundFromCWPts();

  // right block in lava
  clearPtList();
  addLevelPt(2048.0, 4096.0);
  addLevelPt(2112.0, 3584.0);
  addLevelPt(2248.0, 3616.0);
  addLevelPt(2368.0, 3680.0);
  addLevelPt(2432.0, 4096.0);
  createThemedGroundFromCWPts();

  // lava area
  clearPtList();
  addLevelPt(0.0, 3776.0);
  addLevelPt(3064.0, 4088.0);
  addPGO(new LavaPatch(currPointList[0], currPointList[1] - currPointList[0]));
  
  // camera ground
  clearPtList();
  addLevelPt(-8.0, 640.0);
  addLevelPt(768.0, 640.0);
  addLevelPt(1032.0, 824.0);
  addLevelPt(2032.0, 832.0);
  addLevelPt(2304.0, 640.0);
  addLevelPt(3064.0, 640.0);
  addCameraGroundPts(currPointList);

  // set player
  clearPtList();
  addLevelPt(600.0, 640.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(2544.0, 424.0);
  boss = new SumoBoss();
  boss->setXY(currPointList.last());
  boss->setWH(216.0, 216.0);
  boss->hflip();
  boss->setImage(getImg(sumoBossImg));
  boss->setMaxHP(12.0);
  addPGO(boss);
  
  // lightsabers
  clearPtList();
  // centers
  addLevelPt(-219.0, 36.0);
  addLevelPt(-283.0, -107.0);
  addLevelPt(-133.0, -145.0);
  // length, thickness
  addLevelPt(77.0, 0.2267);
  addLevelPt(184.0, 0.4146);
  addLevelPt(178.0, 0.625);
  
  createBossSabersFromCurrPoints();

  // world box, for camera clamp
  clearPtList();
  addLevelPt(128.0, -736.0);
  addLevelPt(2944.0, 4096.0);
  worldBox = Box::from_corners(currPointList.first(), currPointList.last());
}

void SumoLevelTrike::updateLevelAI()
{
  SumoLevel::updateLevelAI();
  
  if (player->getY() >= 832.0 && groundLevelChanged == false)
  {
    groundLevelChanged = true;
    cameraTowardGroundMult = 2.0;
    
    // destroy all blocks so the boss will fall through soon
    for (LinkedListNode<PizzaGOStd*>* terrainNode = objman.phaseList[PHASE_BG_TERRAIN].first;
         terrainNode != NULL;
         terrainNode = terrainNode->next)
    {
      PizzaGOStd* pgo = terrainNode->data;

      if (pgo->getActive() == true && pgo->getImage() == getImg(breakableObjA))
      {
        // this will destroy it on the next update
        pgo->destroyedPhysical = true;
      }
    }
    
    groundCameraPts.clear();

    clearPtList();
    addLevelPt(0.0, 3776.0);
    addLevelPt(3072.0, 3776.0);
    addCameraGroundPts(currPointList);
  }
}

// ==================================== SumoLevelCupcake ================================ //

void SumoLevelCupcake::loadRes()
{
  SumoLevel::loadRes();

  // cupcake breakables
  loadImage(breakableObjA, "donutz", HANDLE_C);
  loadImage(breakableObjB, "loafz", HANDLE_C);
  loadImage(breakableObjC, "rollz", HANDLE_C);

  loadTiles(firemanFlameSet, "fireman_flame", 4, 4, Point2(84.0, 218.0), true);

  loadImage(sumoBossImg, "temp_cupcakez", Point2(152.0, 304.0));
}

void SumoLevelCupcake::createWorld()
{
  // baked goods
  clearPtList();
  addLevelPt(224.0, 572.0);  // donuts
  addLevelPt(1136.0, 673.0);
  addLevelPt(1413.0, 482.0);
  addLevelPt(337.0, 720.0);  // loafs
  addLevelPt(465.0, 624.0);
  addLevelPt(673.0, 721.0);
  addLevelPt(1409.0, 624.0);
  addLevelPt(1424.0, 720.0);
  addLevelPt(1649.0, 529.0);
  addLevelPt(1745.0, 625.0);
  addLevelPt(1762.0, 722.0);
  addLevelPt(681.0, 621.0);  // rolls
  addLevelPt(790.0, 625.0);
  addLevelPt(889.0, 721.0);
  addLevelPt(1234.0, 553.0);
  
  // create baked goods
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    TerrainQuad* breakable = NULL;

    // donut
    if (i <= 2)
    {
      breakable = TerrainQuad::create_donut_C(currPointList[i], getImg(breakableObjA));
    }
    // loaf
    else if (i <= 10)
    {
      breakable = TerrainQuad::create_loaf_C(currPointList[i], getImg(breakableObjB));
    }
    // roll
    else
    {
      breakable = TerrainQuad::create_roll_C(currPointList[i], getImg(breakableObjC));
    }
    
    breakable->respondsToCupcakeFlame = true;
    addPGO(breakable);
  }
  
  // create ground contour
  clearPtList();
  addLevelPt(0.0, 768.0);
  addLevelPt(2048.0, 768.0);
  addCameraGroundPts(currPointList);
  addLevelPt(2048.0, 896.0);
  addLevelPt(0.0, 896.0);
  createThemedGroundFromCWPts();

  // set player
  clearPtList();
  addLevelPt(471.0, 575.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(1656.0, 349.0);
  boss = new SumoBoss();
  boss->setWH(144.0, 144.0);  // radius
  boss->setXY(currPointList.last());
  boss->saberStrikePower = 0.0;
  boss->saberDOTPower = 0.5;
  boss->strengthMult = 0.25;
  boss->geometry = SumoBoss::HEXAGON;
  boss->setImage(getImg(sumoBossImg));
  boss->setMaxHP(12.0);
  addPGO(boss);
  
  // lightsaber (candle top)
  boss->lightsaberFixtures.add(boss->create_circle_fixture(boss->body, 32.0, Point2(0.0, -272.0)));
  boss->lightsaberFixtures.first()->SetSensor(true);
  boss->lightsaberFixtures.first()->SetUserData(boss);
  boss->set_collisions(boss->lightsaberFixtures.first(), 1 << TYPE_BOSS_LIGHTSABER,
                       0xffff & (~(0x1 << TYPE_PLAYER_LIGHTSABER)));
  
  // invisible walls
  clearPtList();
  addLevelPt(128.0, 0.0);
  addLevelPt(1920.0, 768.0);
  worldBox = Box::from_corners(currPointList.first(), currPointList.last());
  worldBox.grow_up(4096.0);
  invisibleWallsStd(worldBox, 128.0);
}

// ==================================== SumoLevelEmperor ================================ //

SumoLevelEmperor::SumoLevelEmperor() :
  SumoLevel(),

  bossThrowTimer(8.0),
  currBossSaber(0)
{
  
}

void SumoLevelEmperor::loadRes()
{
  SumoLevel::loadRes();

  // misc. items
  loadImage(breakableObjA, "forcebone", HANDLE_C);
  loadImage(breakableObjB, "rib", HANDLE_TL);
  loadImage(breakableObjC, "temp_batclops", HANDLE_C);
  
  // boss and lightsaber items
  loadImage(sumoBossImg, "temp_emperor_1", Point2(636.0, 666.0));
  loadImage(sumoBoss2Img, "temp_emperor_2", Point2(636.0, 666.0));
  loadImage(sumoBoss3Img, "temp_emperor_3", Point2(636.0, 666.0));
  loadImage(sumoBoss4Img, "temp_emperor_4", Point2(636.0, 666.0));
  loadImage(sumoBoss5Img, "temp_emperor_5", Point2(636.0, 666.0));
}

void SumoLevelEmperor::createWorld()
{
  // rib tips in 3's. Order: top right, bottom, top left
  /*
  clearPtList();
  addLevelPt(488.0, 536.0);
  addLevelPt(496.0, 620.0);
  addLevelPt(448.0, 548.0);
  addLevelPt(648.0, 636.0);
  addLevelPt(656.0, 716.0);
  addLevelPt(608.0, 648.0);
  addLevelPt(808.0, 572.0);
  addLevelPt(820.0, 652.0);
  addLevelPt(764.0, 580.0);
  addLevelPt(984.0, 476.0);
  addLevelPt(996.0, 556.0);
  addLevelPt(940.0, 488.0);
  addLevelPt(1740.0, 488.0);
  addLevelPt(1744.0, 572.0);
  addLevelPt(1696.0, 504.0);
  addLevelPt(1900.0, 600.0);
  addLevelPt(1904.0, 684.0);
  addLevelPt(1856.0, 616.0);
  addLevelPt(2072.0, 576.0);
  addLevelPt(2080.0, 656.0);
  addLevelPt(2032.0, 592.0);
  addLevelPt(2232.0, 488.0);
  addLevelPt(2240.0, 568.0);
  addLevelPt(2192.0, 500.0);
  addLevelPt(2376.0, 424.0);
  addLevelPt(2384.0, 504.0);
  addLevelPt(2336.0, 436.0);
  addLevelPt(3040.0, 428.0);
  addLevelPt(3048.0, 504.0);
  addLevelPt(3004.0, 440.0);
  addLevelPt(3176.0, 476.0);
  addLevelPt(3184.0, 556.0);
  addLevelPt(3136.0, 488.0);
  addLevelPt(3324.0, 508.0);
  addLevelPt(3336.0, 584.0);
  addLevelPt(3284.0, 516.0);
  addLevelPt(3484.0, 504.0);
  addLevelPt(3492.0, 584.0);
  addLevelPt(3448.0, 516.0);
  addLevelPt(3652.0, 460.0);
  addLevelPt(3660.0, 540.0);
  addLevelPt(3616.0, 468.0);

  DataList<Point2> helperList(4);
  
  for (Coord1 i = 0; i < currPointList.count; i += 3)
  {
    helperList.add(currPointList[i]);
    helperList.add(currPointList[i + 1]);
    helperList.add(currPointList[i + 2]);
    addPGO(new TerrainGround(currPointList));
  }

  // ribs
  clearPtList();
  addLevelPt(384.0, 112.0);
  addLevelPt(544.0, 208.0);
  addLevelPt(704.0, 144.0);
  addLevelPt(880.0, 48.0);
  addLevelPt(1632.0, 64.0);
  addLevelPt(1792.0, 176.0);
  addLevelPt(1968.0, 152.0);
  addLevelPt(2128.0, 64.0);
  addLevelPt(2272.0, 0.0);
  addLevelPt(2938.0, 0.0);
  addLevelPt(3072.0, 48.0);
  addLevelPt(3222.0, 80.0);
  addLevelPt(3380.0, 78.0);
  addLevelPt(3548.0, 32.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    addPGO(new Decoration(currPointList[i], getImg(breakableObjB)));
  }
   
   // ceiling contour
   clearPtList();
   addLevelPt(0.0, 0.0);
   addLevelPt(240.0, 0.0);
   addLevelPt(528.0, 224.0);
   addLevelPt(848.0, 224.0);
   addLevelPt(1088.0, 0.0);
   addLevelPt(1504.0, 0.0);
   addLevelPt(1792.0, 224.0);
   addLevelPt(2256.0, 224.0);
   addLevelPt(2496.0, 0.0);
   addLevelPt(2848.0, 0.0);
   addLevelPt(3072.0, 96.0);
   addLevelPt(3536.0, 128.0);
   addLevelPt(3840.0, 0.0);
   addLevelPt(4096.0, 0.0);
   addPGO(new TerrainGround(currPointList));
   */
  
  // player's platform
  clearPtList();
  addLevelPt(256.0, 2048.0);
  addLevelPt(256.0, 1648.0);
  addLevelPt(320.0, 1584.0);
  addLevelPt(704.0, 1584.0);
  addLevelPt(768.0, 1648.0);
  addLevelPt(768.0, 2048.0);
  createThemedGroundFromCWPts();
  
  // left skull platform
  clearPtList();
  addLevelPt(1024.0, 2048.0);
  addLevelPt(1024.0, 1664.0);
  addLevelPt(1104.0, 1584.0);
  addLevelPt(1360.0, 1672.0);
  addLevelPt(1408.0, 2048.0);
  TerrainGround* currGround = new TerrainGround(currPointList);
  JitterBehavior* groundMover = new JitterBehavior(currGround, Point2(16.0, 32.0), Point2(2.1, 4.30));
  currGround->actions.addX(groundMover);
  createThemedGroundFromCWPts();

  
  // middle skull platform
  clearPtList();
  addLevelPt(1840.0, 2048.0);
  addLevelPt(1824.0, 1704.0);
  addLevelPt(1936.0, 1552.0);
  addLevelPt(2152.0, 1520.0);
  addLevelPt(2208.0, 1648.0);
  addLevelPt(2280.0, 2048.0);
  currGround = new TerrainGround(currPointList);
  groundMover = new JitterBehavior(currGround, Point2(16.0, 32.0), Point2(1.4, 6.30));
  currGround->actions.addX(groundMover);
  createThemedGroundFromCWPts();

  // right skull platform
  clearPtList();
  addLevelPt(2648.0, 2048.0);
  addLevelPt(2648.0, 1664.0);
  addLevelPt(2864.0, 1512.0);
  addLevelPt(2976.0, 1592.0);
  addLevelPt(3032.0, 2048.0);
  currGround = new TerrainGround(currPointList);
  groundMover = new JitterBehavior(currGround, Point2(16.0, 32.0), Point2(2.9, 5.10));
  currGround->actions.addX(groundMover);
  createThemedGroundFromCWPts();

  // boss's platform
  clearPtList();
  addLevelPt(3328.0, 2048.0);
  addLevelPt(3328.0, 1648.0);
  addLevelPt(3392.0, 1584.0);
  addLevelPt(3776.0, 1584.0);
  addLevelPt(3840.0, 1648.0);
  addLevelPt(3840.0, 2048.0);
  createThemedGroundFromCWPts();

  // lava
  clearPtList();
  addLevelPt(0.0, 1792.0);
  addLevelPt(4096.0, 2048.0);
  Box lavaBox = Box::from_corners(currPointList[0], currPointList[1]);
  lavaBox.grow_left(4096.0);
  lavaBox.grow_right(4096.0);
  addPGO(new LavaPatch(lavaBox));
  
  // ground below lava
  clearPtList();
  addLevelPt(-10000.0, 2048.0);
  addLevelPt(10000.0, 2048.0);
  addPGO(new TerrainGround(currPointList));

  // camera ground
  clearPtList();
  addLevelPt(0.0, 1792.0);
  addLevelPt(4096.0, 1792.0);
  addCameraGroundPts(currPointList);

  // set player
  clearPtList();
  addLevelPt(512.0, 1584.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(3584.0, 1376.0);
  boss = new SumoBoss();
  boss->setWH(200.0, 200.0);  // radius
  boss->setXY(currPointList.last());
  boss->setImage(getImg(sumoBossImg));
  boss->bouncesOnLava = true;
  boss->setMaxHP(30.0);
  addPGO(boss);
  
  // create 3 starting bats
  clearPtList();
  addLevelPt(4236.0, 1373.0);
  addLevelPt(5410.0, 1104.0);
  addLevelPt(7489.0, 1426.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    Batclops* bat = new Batclops();
    bat->setXY(currPointList[i]);
    bat->activeXRange.set(0.0, 4096.0);
    addPGO(bat);
  }
  
  // starting lightsaber
  clearPtList();
  // centers
  addLevelPt(-257.0, -263.0);
  // length, angle
  addLevelPt(336.0, 0.7811);
  createBossSabersFromCurrPoints();
  
  // set the invisible walls to be extremely far away for the sake of camera clamping
  Point1 worldlimit = 10000.0;
  worldBox = Box::from_corners(Point2(-worldlimit, -worldlimit), Point2(worldlimit, worldlimit));
}

void SumoLevelEmperor::updateLevelAI()
{
  SumoLevel::updateLevelAI();
  
  bossThrowTimer.update();
  
  if (bossThrowTimer.getActive() == false)
  {
    ForceBone* bone = new ForceBone();
    bone->parent = boss;
    addPGO(bone);
    
    bossThrowTimer.reset();
  }
  
  Point1 dmgPerSaber = 5.0;
  
  if (currBossSaber == 0 &&
      boss->currHP <= boss->maxHP - dmgPerSaber * 1.0)
  {
    currBossSaber++;
    
    // saber set 2
    clearPtList();
    // centers
    addLevelPt(-273.0, -184.0);
    addLevelPt(-181.0, -284.0);
    // length, angles
    addLevelPt(174.0, 0);
    addLevelPt(169.0, 1.5707);
    createBossSabersFromCurrPoints();
  }
  else if (currBossSaber == 1 &&
           boss->currHP <= boss->maxHP - dmgPerSaber * 2.0)
  {
    currBossSaber++;
    
    // saber set 3
    clearPtList();
    // centers
    addLevelPt(-380.0, -310.0);
    addLevelPt(-306.0, -389.0);
    // length, angles
    addLevelPt(144.0, 0.0138);
    addLevelPt(142.0, 1.5707);
    createBossSabersFromCurrPoints();
  }
  else if (currBossSaber == 2 &&
           boss->currHP <= boss->maxHP - dmgPerSaber * 3.0)
  {
    currBossSaber++;
    
    clearPtList();
    // centers
    addLevelPt(-435.0, -256.0);
    addLevelPt(-442.0, -368.0);
    addLevelPt(-362.0, -452.0);
    addLevelPt(-245.0, -452.0);
    // length, angles
    addLevelPt(142.0, 5.4878);
    addLevelPt(141.0, 0.7853);
    addLevelPt(141.0, 0.7953);
    addLevelPt(144.0, 5.4977);
    createBossSabersFromCurrPoints();
  }
  else if (currBossSaber == 3 &&
           boss->currHP <= boss->maxHP - dmgPerSaber * 4.0)
  {
    currBossSaber++;
    
    // saber set 5
    clearPtList();
    // centers
    addLevelPt(-314.0, -62.0);
    addLevelPt(-522.0, -288.0);
    addLevelPt(-274.0, -532.0);
    addLevelPt(-45.0, -284.0);
    // length, angles
    addLevelPt(250.0, 4.7203);
    addLevelPt(253.0, 0.7826);
    addLevelPt(256.0, 0.7826);
    addLevelPt(254.0, 0.0039);
    createBossSabersFromCurrPoints();
  }
}

void SumoLevelEmperor::drawAfterObjects()
{
  SumoLevel::drawAfterObjects();
  
  // draw special emperor sabers
  for (Coord1 i = 1; i <= currBossSaber; ++i)
  {
    VisRectangular::drawImg(boss, getImg(sumoBossImg + i));
  }
}

// ==================================== SumoLevelEmperor ================================ //

SumoLevelRay::SumoLevelRay() :
  SumoLevel(),

  bubbleTimer(3.0, this),
  lastBubbleSide(-1)
{
  
}

void SumoLevelRay::loadRes()
{
  SumoLevel::loadRes();

  // misc. items
  loadImage(breakableObjA, "bubble256", HANDLE_C);
  
  // boss and lightsaber items
  loadImage(sumoBossImg, "temp_raybody", HANDLE_C);
  loadImage(sumoBoss2Img, "temp_raytail64", HANDLE_C);
  loadImage(sumoBoss3Img, "temp_raytail48", HANDLE_C);
  loadImage(sumoBoss4Img, "temp_raytail32", HANDLE_C);
  loadImage(sumoBoss5Img, "temp_raytailtip", Point2(50.0, 50.0));
}

void SumoLevelRay::createWorld()
{
  // coral contour plus bottom
  clearPtList();
  addLevelPt(376.0, 3072.0);
  addLevelPt(320.0, 2944.0);
  addLevelPt(128.0, 2688.0);
  addLevelPt(128.0, 2432.0);
  addLevelPt(256.0, 1848.0);
  addLevelPt(192.0, 1528.0);
  addLevelPt(264.0, 1208.0);
  addLevelPt(472.0, 1024.0);
  addLevelPt(664.0, 904.0);
  addLevelPt(536.0, 776.0);
  addLevelPt(320.0, 776.0);
  addLevelPt(128.0, 640.0);
  addLevelPt(128.0, 192.0);
  addLevelPt(320.0, 128.0);
  addLevelPt(512.0, 128.0);
  addLevelPt(640.0, 192.0);
  addLevelPt(760.0, 192.0);
  addLevelPt(952.0, 64.0);
  addLevelPt(1536.0, 64.0);
  addLevelPt(1728.0, 128.0);
  addLevelPt(1984.0, 128.0);
  addLevelPt(2296.0, 64.0);
  addLevelPt(2496.0, 64.0);
  addLevelPt(2752.0, 128.0);
  addLevelPt(2880.0, 256.0);
  addLevelPt(2944.0, 288.0);
  addLevelPt(2944.0, 640.0);
  addLevelPt(2800.0, 1208.0);
  addLevelPt(2880.0, 1536.0);
  addLevelPt(2816.0, 1856.0);
  addLevelPt(2944.0, 2432.0);
  addLevelPt(2944.0, 2688.0);
  addLevelPt(2752.0, 2944.0);
  addLevelPt(2696.0, 3072.0);
  addLevelPt(376.0, 3072.0);
  addPGO(new TerrainGround(currPointList));

  // spikey at bottom
  clearPtList();
  addLevelPt(448.0, 3072.0);
  addLevelPt(448.0, 2880.0);
  addLevelPt(576.0, 2752.0);
  addLevelPt(704.0, 2688.0);
  addLevelPt(2416.0, 2688.0);
  addLevelPt(2552.0, 2752.0);
  addLevelPt(2680.0, 2880.0);
  addLevelPt(2680.0, 3072.0);
  TerrainGround* spikeGround = new TerrainGround(currPointList);
  spikeGround->setSpikey();
  addPGO(spikeGround);

  // set player
  clearPtList();
  addLevelPt(432.0, 768.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  boss = new SumoRayBoss();

  // boss world center
  clearPtList();
  addLevelPt(696.0, 2192.0);
  boss->setWH(320.0, 320.0);  // radius
  boss->setXY(currPointList.first());
  boss->setImage(getImg(sumoBossImg));
  addPGO(boss);
  
  // tail shape centers
  clearPtList();
  addLevelPt(-382.0, 0.0);  // 64
  addLevelPt(-497.0, 0.0);  // 48
  addLevelPt(-576.0, 0.0);  // 32
  addLevelPt(-641.0, 0.0);  // 32
  addLevelPt(-705.0, 0.0);  // 32
  addLevelPt(-784.0, 0.0);  // tip
  addLevelPt(64.0, PI);
  addLevelPt(48.0, PI);
  addLevelPt(32.0, PI);
  addLevelPt(32.0, PI);
  addLevelPt(32.0, PI);
  addLevelPt(48.0, PI);

  Coord1 sabersInSet = currPointList.count / 2;

  for (Coord1 i = 0; i < sabersInSet; ++i)
  {
    boss->lightsaberFixtures.add(boss->create_circle_fixture(boss->body, currPointList[i + sabersInSet].x,
                                                          currPointList[i]));
    boss->lightsaberFixtures.first()->SetUserData(boss);
    boss->set_collisions(boss->lightsaberFixtures.first(),
                         1 << TYPE_BOSS_LIGHTSABER, 0xffff & (~(0x1 << TYPE_TERRAIN)));
  }
  
  // tail tip points
  clearPtList();
  addLevelPt(-735.0, 47.0);
  addLevelPt(-861.0, -1.0);
  addLevelPt(-736.0, -48.0);

  // bubble spawn range [lowX, highX]
  clearPtList();
  addLevelPt(2168.0, 1560.0);
  addLevelPt(1232.0, 2424.0);
  addLevelPt(2224.0, 3512.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    createBubble(currPointList[i], RM::randf(0.5, 1.0));
  }

  // set the invisible walls for the sake of camera clamping
  worldBox = Box::from_corners(Point2(0.0, 0.0), Point2(3072.0, 3072.0));
}

void SumoLevelRay::updateCamera(Logical snapIntoPlace)
{
  shaker.update();

  rmboxCamera.setXY(player->getXY() - RM_WH * 0.5);
  rmboxCamera.handle = RM_WH * 0.5;
  rmboxCamera.setZoom(0.2);
  
  Box clampBox = Box::from_corners(worldBox.norm_pos(HANDLE_TL) +
                                   rmcamboxToScreen(RM_BOX.norm_pos(HANDLE_TL)),
                                   worldBox.norm_pos(HANDLE_BR) +
                                   rmcamboxToScreen(RM_BOX.norm_pos(HANDLE_BR)));
  rmboxCamera.clampToArea(clampBox);
}

void SumoLevelRay::updateLevelAI()
{
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  bubbleTimer.update();
  
  // bubbles pop if touching pizza too high, to avoid collision issues
  for (LinkedListNode<PizzaGOStd*>* bubbleNode = objman.phaseList[PHASE_BG_TERRAIN].first;
       bubbleNode != NULL;
       bubbleNode = bubbleNode->next)
  {
    PizzaGOStd* bubble = bubbleNode->data;
    
    if (bubble->getActive() == false ||
        bubble->type != TYPE_TERRAIN ||
        bubble->value != VALUE_BUBBLE)
    {
      continue;
    }
    
    bubble->addY(-256.0 * RM::timePassed());
    
    if (bubble->getY() < 888.0)
    {
      // should play a little particle effect here
      bubble->set_collisions(0x1 << TYPE_TERRAIN, 0x0);
      bubble->setRemove();
    }
  }
}

void SumoLevelRay::createBubble(Point2 center, Point1 scale)
{
  Image* bubbleImg = getImg(breakableObjA);
  Point1 bubbleRadius = bubbleImg->natural_size().x * 0.5 * scale;
  
  Point1 range = worldBox.width() * 0.3;
  
  TerrainQuad* bubble = TerrainQuad::create_static_circle(center, bubbleRadius, bubbleImg);
  bubble->value = VALUE_BUBBLE;
  bubble->setScale(scale, scale);
  bubble->setBouncy(1.0);
  addPGO(bubble);
  
  // setting it to be kinematic and setting the velocity allows it to be more bouncy
  // because it's a moving body
  bubble->body->SetType(b2_kinematicBody);
}

void SumoLevelRay::callback(ActionEvent* caller)
{
  if (caller == &bubbleTimer)
  {
    Image* bubbleImg = getImg(breakableObjA);
    Point1 bubbleScale = RM::randf(0.5, 1.0);
    Point1 bubbleRadius = bubbleImg->natural_size().x * 0.5 * bubbleScale;
    
    Point2 bubblePos = worldBox.norm_pos(HANDLE_BC);
    bubblePos.y += bubbleRadius;
    bubblePos.x = lastBubbleSide == -1 ? RM::lerp(984.0, 1824.0, RM::randf()) :
        RM::lerp(1824.0, 2520.0, RM::randf());
    
    createBubble(bubblePos, bubbleScale);

    lastBubbleSide *= -1;
    
    bubbleTimer.reset();
  }
}

// ==================================== SumoLevelFungus ================================ //

void SumoLevelFungus::loadRes()
{
  SumoLevel::loadRes();

  // misc. items
  loadImage(breakableObjA, "shroom_0", HANDLE_TL);
  loadImage(breakableObjB, "shroom_1", HANDLE_TL);
  loadImage(breakableObjC, "shroom_2", HANDLE_TL);
  loadImage(breakableObjD, "spore", HANDLE_C);
  loadImage(breakableObjE, "sporesaber",  HANDLE_LC);

  // boss and lightsaber items
  loadImage(sumoBossImg, "sumo_fungus", Point2(408, 306.0));
  loadImage(sumoBoss2Img, "sumo_fungus_shroom0", HANDLE_C);
  loadImage(sumoBoss3Img, "sumo_fungus_shroom1", HANDLE_C);
  loadImage(sumoBoss4Img, "sumo_fungus_shroom2", HANDLE_C);
  loadImage(sumoBoss5Img, "sumo_fungus_shroom3", HANDLE_C);
  loadImage(sumoBoss6Img, "sumo_fungus_shroom4", HANDLE_C);
  loadImage(sumoBoss7Img, "sumo_fungus_shroom5", HANDLE_C);
  loadImage(sumoBoss8Img, "sumo_fungus_shroom6", HANDLE_C);
}

void SumoLevelFungus::createWorld()
{
  // mushrooms
  clearPtList();
  addLevelPt(256.0, 896.0);
  addLevelPt(640.0, 832.0);
  addLevelPt(1088.0, 896.0);
  addLevelPt(1280.0, 832.0);
  addLevelPt(1728.0, 896.0);
  addLevelPt(2112.0, 896.0);
  addLevelPt(2496.0, 896.0);
  addLevelPt(2688.0, 896.0);
  addLevelPt(2880.0, 832.0);
  addLevelPt(3328.0, 896.0);
  addLevelPt(3520.0, 896.0);
  
  DataList<Coord1> shroomIndices(currPointList.count);
  shroomIndices.add(0);
  shroomIndices.add(1);
  shroomIndices.add(2);
  shroomIndices.add(1);
  shroomIndices.add(0);
  shroomIndices.add(0);
  shroomIndices.add(2);
  shroomIndices.add(2);
  shroomIndices.add(1);
  shroomIndices.add(2);
  shroomIndices.add(0);

  Point1 waveTime = 0.0;

  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    TerrainQuad* mushroom = TerrainQuad::create_mushroom(currPointList[i], shroomIndices[i]);
    addPGO(mushroom);
    
    JitterBehavior* waver = new JitterBehavior(&mushroom->xy, Point2(0.0, 64.0), Point2(4.0, 4.0));
    waver->currTime = waveTime;
    waver->update();
    addAction(waver);
    
    waveTime += (1.0 / 60.0) * 15.0;
  }
  
  // camera ground
  clearPtList();
  addLevelPt(256.0, 960.0);
  addLevelPt(3904.0, 960.0);
  addCameraGroundPts(currPointList);
  
  // set player
  clearPtList();
  addLevelPt(864.0, 832.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(3096.0, 616.0);
  boss = new SumoFungusBoss();
  boss->setWH(200.0, 200.0);  // radius
  boss->setXY(currPointList.last());
  boss->setImage(getImg(sumoBossImg));
  boss->setMaxHP(20.0);
  addPGO(boss);

  // mushroom chunks
  clearPtList();
  addLevelPt(-108.0, -161.0);
  addLevelPt(-1.0, -211.0);
  addLevelPt(231.0, 3.0);
  addLevelPt(167.0, 151.0);
  addLevelPt(-2.0, 218.0);
  addLevelPt(-141.0, 169.0);
  addLevelPt(-204.0, 50.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    b2Fixture* currShroom = boss->create_circle_fixture(boss->body, 32.0, Point2(currPointList[i]));
    currShroom->SetUserData(boss);
    Physical::set_collisions(currShroom, 1 << TYPE_BOSS_ATTACHMENT,
                             0xffff & (~(0x1 << TYPE_PLAYER_LIGHTSABER)));
    
    boss->attachments.add(BossAttachment());
    boss->attachments.last().parent = boss;
    boss->attachments.last().fixture = currShroom;
    boss->attachments.last().img = getImg(sumoBoss2Img + i);
    boss->attachments.last().bossOffset = currPointList[i];
  }
  
  // starting lightsaber
  clearPtList();
  // centers
  addLevelPt(-252.0, -107.0);
  addLevelPt(203.0, -189.0);
  // length, angle
  addLevelPt(197.0, 0.3729);
  addLevelPt(204.0, 2.4668);
  createBossSabersFromCurrPoints();
  
  // set the invisible walls to be extremely far away for the sake of camera clamping
  Point1 worldlimit = 10000.0;
  worldBox = Box::from_corners(Point2(-worldlimit, -worldlimit), Point2(worldlimit, worldlimit));
}

void SumoLevelFungus::drawAfterObjects()
{
  player->drawLightsaber();
}

// ==================================== SumoLevelSnowball ================================ //

SumoLevelSnowball::SumoLevelSnowball() :
  SumoLevel()
{

}

void SumoLevelSnowball::loadRes()
{
  SumoLevel::loadRes();

  // boss and lightsaber items
  loadImage(sumoBossImg, "sumo_snowball", Point2(321, 261.0));
}

void SumoLevelSnowball::createWorld()
{
  // create ground contour
  clearPtList();
  addLevelPt(0.0, 1024.0);
  addLevelPt(640.0, 1024.0);
  addLevelPt(768.0, 1408.0);
  addLevelPt(1024.0, 1728.0);
  addLevelPt(1344.0, 1960.0);
  addLevelPt(1784.0, 2048.0);
  addLevelPt(2304.0, 2048.0);
  addLevelPt(2752.0, 1960.0);
  addLevelPt(3072.0, 1728.0);
  addLevelPt(3328.0, 1408.0);
  addLevelPt(3456.0, 1024.0);
  addCameraGroundPts(currPointList);

  addLevelPt(4096.0, 1024.0);
  addLevelPt(4096.0, 2560.0);
  addLevelPt(0.0, 2560.0);
  createThemedGroundFromCWPts();
  
  // set player
  clearPtList();
  addLevelPt(480.0, 1024.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(3552.0, 824.0);
  boss = new SumoBoss();
  boss->setWH(200.0, 200.0);  // radius
  boss->setXY(currPointList.last());
  boss->setImage(getImg(sumoBossImg));
  addPGO(boss);
  
  // starting lightsaber
  clearPtList();
  // centers
  addLevelPt(-273.0, 168.0);
  addLevelPt(0.0, 180.0);
  addLevelPt(286.0, 146.0);
  // length, angle
  addLevelPt(95.0, 0.529);
  addLevelPt(507.0, 6.2674);
  addLevelPt(95.0, 5.6084);
  createBossSabersFromCurrPoints();
  
  // set the invisible walls to be extremely far away for the sake of camera clamping
  clearPtList();
  addLevelPt(176.0, -1504.0);
  addLevelPt(3896.0, 2552.0);
  worldBox = Box::from_corners(currPointList[0], currPointList[1]);
  invisibleWallsStd(worldBox, 128.0);
}


// ==================================== SumoLevelJunkboys ================================ //

SumoLevelJunkboys::SumoLevelJunkboys() :
  SumoLevel()
{

}

void SumoLevelJunkboys::loadRes()
{
  SumoLevel::loadRes();

  // misc. objects
  loadImage(breakableObjA, "tire", HANDLE_C);
  
  // boss and lightsaber items
  loadImage(sumoBossImg, "temp_junkboy_a", Point2(162.0, 291.0));
  loadImage(sumoBoss2Img, "temp_junkboy_b", Point2(162.0, 291.0));
  loadImage(sumoBoss3Img, "temp_junkboy_c", Point2(158, 287.0));
}

void SumoLevelJunkboys::createWorld()
{
  // left terrain
  clearPtList();
  addLevelPt(192.0, -720.0);
  addLevelPt(192.0, 832.0);
  addLevelPt(320.0, 960.0);
  addLevelPt(640.0, 960.0);
  addLevelPt(768.0, 1088.0);
  addLevelPt(960.0, 1088.0);
  addLevelPt(960.0, 1280.0);
  addLevelPt(0.0, 1280.0);
  addLevelPt(0.0, -720.0);
  createThemedGroundFromCWPts();

  // middle terrain piece
  clearPtList();
  addLevelPt(1408.0, 1280.0);
  addLevelPt(1408.0, 1024.0);
  addLevelPt(1600.0, 960.0);
  addLevelPt(1664.0, 768.0);
  addLevelPt(1984.0, 896.0);
  addLevelPt(1984.0, 1088.0);
  addLevelPt(2048.0, 1152.0);
  addLevelPt(2048.0, 1280.0);
  createThemedGroundFromCWPts();

  // right terrain piece
  clearPtList();
  addLevelPt(2496.0, 1280.0);
  addLevelPt(2496.0, 1152.0);
  addLevelPt(2624.0, 1024.0);
  addLevelPt(3136.0, 1024.0);
  addLevelPt(3264.0, 832.0);
  addLevelPt(3520.0, 832.0);
  addLevelPt(3584.0, 640.0);
  addLevelPt(3840.0, 640.0);
  addLevelPt(3904.0, 512.0);
  addLevelPt(3904.0, -800.0);
  addLevelPt(4096.0, -800.0);
  addLevelPt(4096.0, 1280.0);
  createThemedGroundFromCWPts();
  
  // ground contour
  clearPtList();
  addLevelPt(320.0, 960.0);
  addLevelPt(760.0, 1088.0);
  addLevelPt(3136.0, 1024.0);
  addLevelPt(3840.0, 640.0);
  addCameraGroundPts(currPointList);

  // tires
  clearPtList();
  addLevelPt(1189.0, 1152.0);
  addLevelPt(2266.0, 1152.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    TerrainQuad* tire = TerrainQuad::create_static_circle(currPointList[i], 192.0, getImg(breakableObjA));
    tire->setBouncy(0.75);
    addPGO(tire);
  }
  
  // set player
  clearPtList();
  addLevelPt(512.0, 960.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(3009.0, 929.0);
  addLevelPt(3405.0, 735.0);
  addLevelPt(3713.0, 545.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    SumoJunkBoss* junkBoss = new SumoJunkBoss();
    boss = junkBoss;
    boss->setWH(96.0, 96.0);  // radius
    boss->setXY(currPointList[i]);
    boss->setImage(getImg(sumoBossImg + i));
    boss->setMaxHP(8.0);

    if (i == 0)
    {
      junkBoss->waitRange *= 0.5;
    }
    else if (i == 1)
    {
      junkBoss->jumpVel *= 1.5;
    }
    else if (i == 2)
    {
      junkBoss->spinVal = TWO_PI * 2.0;
    }
    
    addPGO(boss);
    
    boss->lightsaberFixtures.add(boss->create_box_fixture_center(boss->body, Point2(-49.0, -152.0),
                                                              Point2(179.0, 8.0), 1.2244));
    boss->lightsaberFixtures.first()->SetUserData(boss);
    boss->set_collisions(boss->lightsaberFixtures.first(), 1 << TYPE_BOSS_LIGHTSABER,
                         0xffff &
                         (~(0x1 << TYPE_TERRAIN)) &
                         (~(0x1 << TYPE_SUMO_BOSS)) &
                         (~(0x1 << TYPE_BOSS_LIGHTSABER)));
  }
  
  // invisible walls
  clearPtList();
  addLevelPt(192.0, -800.0);
  addLevelPt(3904.0, 1280.0);
  worldBox = Box::from_corners(currPointList[0], currPointList[1]);
  invisibleWallsStd(worldBox, 128.0);
}

// ==================================== SumoLevelJunkboys ================================ //

SumoLevelEdison::SumoLevelEdison() :
  SumoLevel()
{
  
}

void SumoLevelEdison::loadRes()
{
  SumoLevel::loadRes();

  // misc. objects
  loadImage(breakableObjA, "bulb_a", Point2(131.0, 129.0));
  loadImage(breakableObjB, "bulb_b", Point2(196.0, 196.0));
  loadImage(breakableObjC, "bulb_c", Point2(96.0, 282.0));
  loadImage(breakableObjD, "bulb_a_broken", Point2(131.0, 129.0));
  loadImage(breakableObjE, "bulb_b_broken", Point2(196.0, 196.0));
  loadImage(breakableObjF, "bulb_c_broken", Point2(96.0, 282.0));

  // boss and lightsaber items
  loadImage(sumoBossImg, "temp_sumo_edison", Point2(258.0, 320.0));
  loadImage(sumoBoss2Img, "edison_bulb", Point2(66.0, 66.0));
  loadImage(sumoBoss3Img, "edison_saber_left", Point2(320.0, 64.0));
  loadImage(sumoBoss4Img, "edison_saber_right", Point2(64.0, 64.0));
}

void SumoLevelEdison::createWorld()
{
  // bulbs
  clearPtList();
  addLevelPt(832.0, 1600.0);  // A
  addLevelPt(1536.0, 1536.0);
  addLevelPt(2560.0, 1536.0);
  addLevelPt(2752.0, 1344.0);
  addLevelPt(328.0, 1528.0);  // B
  addLevelPt(1160.0, 1656.0);
  addLevelPt(1864.0, 1656.0);
  addLevelPt(0.0, 776.0);  // C
  addLevelPt(512.0, 1480.0);
  addLevelPt(2048.0, 1536.0);
  addLevelPt(2240.0, 1472.0);
  addLevelPt(2880.0, 840.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    PizzaGOStd* bulb = NULL;
    
    // small circle bulb
    if (i <= 3)
    {
      bulb = TerrainQuad::create_static_circle(currPointList[i], 128.0, getImg(breakableObjA));

    }
    // large circle bulb
    else if (i <= 6)
    {
      bulb = TerrainQuad::create_static_circle(currPointList[i], 184.0, getImg(breakableObjB));
    }
    // large rectangular lightbulb
    else
    {
      bulb = TerrainQuad::create_ground_TL(currPointList[i], Point2(192.0, 568.0), getImg(breakableObjC));
    }

    bulb->crushesPhysical = true;
    bulb->crushValue = 5000.0;
    addPGO(bulb);
  }
  
  // camera ground contour
  clearPtList();
  addLevelPt(312.0, 1352.0);
  addLevelPt(608.0, 1480.0);
  addLevelPt(1848.0, 1472.0);
  addLevelPt(2144.0, 1536.0);
  addLevelPt(2544.0, 1408.0);
  addLevelPt(2760.0, 1216.0);
  addCameraGroundPts(currPointList);
  
  // set player
  clearPtList();
  addLevelPt(632.0, 1472.0);
  player->load();
  startStanding(currPointList.last());
  
  // create boss
  clearPtList();
  addLevelPt(2304.0, 1152.0);
  SumoEdisonBoss* edison = new SumoEdisonBoss();
  boss = edison;
  boss->geometry = SumoBoss::OCTAGON;
  boss->setWH(256.0, 256.0);  // radius
  boss->setXY(currPointList.last());
  boss->setImage(getImg(sumoBossImg));
  boss->setMaxHP(16.0);
  addPGO(boss);
  
  // lightbulbs chunks
  clearPtList();
  addLevelPt(181.0, 179.0);
  addLevelPt(-2.0, 254.0);
  addLevelPt(-183.0, 176.0);
  addLevelPt(-182.0, -182.0);
  addLevelPt(0.0, -257.0);
  addLevelPt(181.0, -182.0);
  
  for (Coord1 i = 0; i < currPointList.count; ++i)
  {
    b2Fixture* currBulb = boss->create_circle_fixture(boss->body, 32.0, Point2(currPointList[i]));
    currBulb->SetUserData(boss);
    Physical::set_collisions(currBulb, 1 << TYPE_BOSS_ATTACHMENT,
                             0xffff & (~(0x1 << TYPE_PLAYER_LIGHTSABER)));
    
    boss->attachments.add(BossAttachment());
    boss->attachments.last().parent = boss;
    boss->attachments.last().fixture = currBulb;
    boss->attachments.last().img = getImg(sumoBoss2Img);
    boss->attachments.last().bossOffset = currPointList[i];
  }
  
  // create lightsabers
  clearPtList();
  // centers
  addLevelPt(-403.0, 0.0);
  addLevelPt(403.0, 0.0);
  // polar
  addLevelPt(328.0, 0.0);
  addLevelPt(328.0, 0.0);
  createBossSabersFromCurrPoints(64.0);
  
  edison->turnOnSaber(0);  // this makes it not collide with player saber
  edison->turnOffSaber(1);
  
  // invisible walls
  clearPtList();
  addLevelPt(120.0, -800.0);
  addLevelPt(2936.0, 1920.0);
  worldBox = Box::from_corners(currPointList[0], currPointList[1]);
  invisibleWallsStd(worldBox, 128.0);
}

void SumoLevelEdison::objectCrushedStd(PizzaGOStd* pgo)
{
  if (pgo->type != TYPE_TERRAIN)
  {
    PhysicsLevelStd::objectCrushedStd(pgo);
    return;
  }
  
  TerrainQuad* bulb = static_cast<TerrainQuad*>(pgo);
  
  if ((bulb->flags & PizzaGOStd::SPIKED_BIT) != 0)
  {
    // already spiked, do nothing
  }
  else
  {
    createTriangleShatter(bulb, Coord2(10, 10), Point2(3.0, 4.0), &objman.actions);
    
    bulb->setSpikey();
    
    if (bulb->getImage() == getImg(breakableObjA))
    {
      bulb->setImage(getImg(breakableObjA + 3));
    }
    else if (bulb->getImage() == getImg(breakableObjA))
    {
      bulb->setImage(getImg(breakableObjB + 3));
    }
    else
    {
      bulb->setImage(getImg(breakableObjC + 3));
    }
  }
}

// ==================================== PachinkoLevel ================================ //

PachinkoLevel::PachinkoLevel() :
  PhysicsLevelStd(),

  startScript(this),
  ballDropScript(this)
{
  background = new UFOBackground();
  background->level = this;
}

void PachinkoLevel::preload()
{
  
}

void PachinkoLevel::loadRes()
{
  background->load();
}

void PachinkoLevel::unloadRes()
{
  
}

void PachinkoLevel::initSublevel()
{
  // done before the world is created so that subclasses know the correct size
  player->wh /= 4.0;
  player->scale /= 4.0;
  player->fullSpokeLength /= 4.0;
  player->rollForce /= 16.0;
  player->rollTorque /= 16.0;
  player->setXY(Point2(472.0, 112.0));
  player->load();
  player->face.eyesWarper.scaleRoot(0.25);
  
  createWorld();
  worldCreated();
  
  rmboxCamera.xy.x = 0.0;
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_C);

  // intro sequence where it scrolls up
  RMPhysics->set_gravity_rm(Point2(0.0, 0.0));
  rmboxCamera.addY(RM_WH.y * 3.0);
  levelState = LEVEL_PASSIVE;
}

void PachinkoLevel::levelStart()
{
  startScript.wait(2.0);
  startScript.enqueueX(new ArcsineFn(&rmboxCamera.xy.y, player->getY() - RM_WH.y * 0.25, 4.0));
}

void PachinkoLevel::updateStatus()
{
  // winLevel() or loseLevel()
}

void PachinkoLevel::pizzaDamaged()
{
  
}

void PachinkoLevel::drawBGTerrain()
{
  
}

void PachinkoLevel::updateCamera(Logical snapIntoPlace)
{
  if (levelState == LEVEL_PLAY)
  {
    rmboxCamera.setX(0.0);
    rmboxCamera.setY(player->getY() - RM_WH.y * 0.25);
  }
}

void PachinkoLevel::updateLevelAI()
{
  startScript.update();
  ballDropScript.update();
  
  if (levelState == LEVEL_PLAY &&
      player->collisionCircle().top() >= worldBox.bottom())
  {
    loseLevel();
  }
}

void PachinkoLevel::callback(ActionEvent* caller)
{
  if (levelState == LEVEL_PASSIVE &&
      caller == &startScript)
  {
    levelState = LEVEL_PLAY;
    RMPhysics->set_gravity_rm(Point2(0.0, 800.0));
    
    ballDropScript.wait(2.0);
  }
  
  if (levelState == LEVEL_PLAY &&
      caller == &ballDropScript)
  {
    PachinkoBall* newBall = new PachinkoBall();
    newBall->setXY(472.0, 112.0);
    addPGO(newBall);
    
    newBall->set_velocity(Point2(RM::randf(-64.0, 64.0), 0.0));

    ballDropScript.wait(0.3);
  }
}

// ==================================== PALevelEasy ================================ //

void PALevelEasy::createWorld()
{
  {
    DataList<Point2> groundPts(16);
    groundPts.add(Point2(400.0, 48.0));
    groundPts.add(Point2(96.0, 304.0));
    groundPts.add(Point2(8.0, 576.0));
    groundPts.add(Point2(24.0, 1944.0));
    groundPts.add(Point2(304.0, 2120.0));
    groundPts.add(Point2(320.0, 2152.0));
    addPGO(new TerrainGround(groundPts));
  }
  
  {
    DataList<Point2> groundPts(16);
    groundPts.add(Point2(544.0, 48.0));
    groundPts.add(Point2(848.0, 304.0));
    groundPts.add(Point2(936.0, 576.0));
    groundPts.add(Point2(920.0, 1944.0));
    groundPts.add(Point2(640.0, 2120.0));
    groundPts.add(Point2(624.0, 2152.0));
    addPGO(new TerrainGround(groundPts));
  }
  
  Point2 pegPts[] =
  {
    Point2(476.0, 264.0),
    Point2(196.0, 364.0),
    Point2(376.0, 368.0),
    Point2(580.0, 372.0),
    Point2(744.0, 372.0),
    Point2(284.0, 464.0),
    Point2(472.0, 464.0),
    Point2(664.0, 464.0),
    Point2(46.0, 914.0),
    Point2(79.0, 942.0),
    Point2(312.0, 851.0),
    Point2(298.0, 878.0),
    Point2(273.0, 907.0),
    Point2(250.0, 934.0),
    Point2(635.0, 852.0),
    Point2(639.0, 882.0),
    Point2(662.0, 914.0),
    Point2(693.0, 943.0),
    Point2(405.0, 972.0),
    Point2(428.0, 1001.0),
    Point2(460.0, 1027.0),
    Point2(495.0, 1051.0),
    Point2(672.0, 1081.0),
    Point2(873.0, 1056.0),
    Point2(903.0, 1016.0),
    Point2(178.0, 1087.0),
    Point2(31.0, 1195.0),
    Point2(55.0, 1217.0),
    Point2(278.0, 1251.0),
    Point2(325.0, 1235.0),
    Point2(368.0, 1200.0),
    Point2(390.0, 1167.0),
    Point2(555.0, 1220.0),
    Point2(788.0, 1230.0),
    Point2(38.0, 1390.0),
    Point2(71.0, 1421.0),
    Point2(118.0, 1422.0),
    Point2(153.0, 1390.0),
    Point2(452.0, 1331.0),
    Point2(615.0, 1376.0),
    Point2(647.0, 1406.0),
    Point2(694.0, 1409.0),
    Point2(730.0, 1376.0),
    Point2(893.0, 1424.0),
    Point2(911.0, 1402.0),
    Point2(111.0, 1590.0),
    Point2(138.0, 1562.0),
    Point2(168.0, 1526.0),
    Point2(324.0, 1492.0),
    Point2(357.0, 1525.0),
    Point2(403.0, 1525.0),
    Point2(440.0, 1492.0),
    Point2(568.0, 1522.0),
    Point2(597.0, 1552.0),
    Point2(756.0, 1578.0),
    Point2(192.0, 1693.0),
    Point2(333.0, 1717.0),
    Point2(372.0, 1670.0),
    Point2(582.0, 1707.0),
    Point2(606.0, 1672.0),
    Point2(630.0, 1707.0),
    Point2(811.0, 1721.0),
    Point2(782.0, 1770.0),
    Point2(754.0, 1813.0),
    Point2(743.0, 1875.0),
    Point2(707.0, 1906.0),
    Point2(660.0, 1906.0),
    Point2(627.0, 1875.0),
    Point2(483.0, 1835.0),
    Point2(448.0, 1789.0),
    Point2(416.0, 1835.0),
    Point2(254.0, 1865.0),
    Point2(218.0, 1898.0),
    Point2(173.0, 1897.0),
    Point2(139.0, 1866.0),
    Point2(412.0, 1976.0),
    Point2(445.0, 2007.0),
    Point2(491.0, 2008.0),
    Point2(529.0, 1976.0)
  };
  
  for (Coord1 i = 0; i < RM::count(pegPts); ++i)
  {
    TerrainQuad* peg = TerrainQuad::create_pachinko_peg(pegPts[i]);
    addPGO(peg);
  }
  
  Point2 goalPts[] =
  {
    // 1*
    Point2(98.0, 1393.0),
    Point2(384.0, 1496.0),
    Point2(674.0, 1379.0),
    
    // 2*
    Point2(200.0, 1869.0),
    Point2(689.0, 1877.0),
    
    // 3*
    Point2(472.0, 1979.0)
  };
  
  for (Coord1 i = 0; i < RM::count(goalPts); ++i)
  {
    PachinkoHole* hole = new PachinkoHole();
    hole->setXY(goalPts[i]);
    
    if (i <= 2)
    {
      hole->numStars = 1;
    }
    else if (i <= 4)
    {
      hole->numStars = 2;
    }
    else
    {
      hole->numStars = 3;
    }
    
    addPGO(hole);
  }
  
  Box crossBoxes[] =
  {
    Box(Point2(56.0, 680.0), Point2(200.0, 16.0)),
    Box(Point2(152.0, 592.0), Point2(16.0, 200.0)),
    
    Box(Point2(368.0, 680.0), Point2(200.0, 16.0)),
    Box(Point2(464.0, 592.0), Point2(16.0, 200.0)),
    
    Box(Point2(688.0, 680.0), Point2(200.0, 16.0)),
    Box(Point2(784.0, 592.0), Point2(16.0, 200.0))
  };
  
  Point2 crossCenters[] =
  {
    Point2(160.0, 688.0),
    Point2(472.0, 688.0),
    Point2(792.0, 688.0)
  };
  
  for (Coord1 i = 0; i < RM::count(crossCenters); ++i)
  {
    TerrainQuad* cross = TerrainQuad::create_pachinko_cross(crossCenters[i], crossBoxes[i * 2].wh);
    addPGO(cross);
  }

  // right now this is hard coded but could be read from the points
  worldBox = RM_BOX;
  worldBox.wh.y *= 4.0;
}


// ==================================== PuppyLevel ================================ //

PuppyLevel::PuppyLevel() :
  PhysicsLevelStd(),

  crateTimer(4.0, this),
  cratesMade(0),

  arrowTimer(4.0, true, this),
  bombTimer(4.0, true, this),

  billyState(WAIT_FOR_RETURN),
  billyJumpTimer(0.75, this),
  currJumpingBilly(0)

  // sun(NULL)
{
  objman.bgTerrainList.ensure_capacity(256);
  
  billyJumpTimer.repeats();
  
  crateTimer.setInactive();
  arrowTimer.setInactive();
  bombTimer.setInactive();
  
  musicFilename = "puppy.ima4";
}

void PuppyLevel::preload()
{
  background = new PuppyBackground();
  background->level = this;
  background->foregroundDisabled = true;
}

// overrides
void PuppyLevel::loadRes()
{
  loadEnabledEnemies();
  loadGenericChunkObjects();

  loadTiles(doghouseSet, "puphouse", 2, 1, Point2(24.0, 208.0));
  loadSpine(puppiesSpine, "puppies");
  loadTiles(pupCageSet, "pupcage", 2, 3, Point2(32.0, 24.0));
  
  background->load();
}

void PuppyLevel::unloadRes()
{
  
}

void PuppyLevel::initSublevel()
{
  endpoint = Point2(0.0, 416.0);
  
  puppyDoghouseChunk();
  
  createLevel();
  // addTextEffectStd(LSTRINGS[101], Point2(PLAY_CX, PLAY_CY));
  
  // worldBox = Box(0.0, -RM_WH.y * 2.0, endpoint.x, RM_WH.y * 3.0);
  // invisibleWallsStd(worldBox);
  
  player->load();
  startStanding(Point2(256.0, 416.0));

  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void PuppyLevel::createLevel()
{
  // createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void PuppyLevel::updateCamera(Logical snapIntoPlace)
{
  updateCameraPuppy(snapIntoPlace);
}

void PuppyLevel::updateLevelAI()
{
  if (levelState == LEVEL_PLAY)
  {
    if (lostToFallingStd() == true)
    {
      loseLevel();
    }
    else if (lostToDMGStd() == true)
    {
      loseLevel();
    }
  }
  
  if (billyState == WAIT_FOR_RETURN)
  {
    crateTimer.update();
    arrowTimer.update();
    bombTimer.update();
    
    if (puppiesToFind == 0 && player->getX() < puppyStartPt.x + 128.0)
    {
      // if (sun != NULL) sun->puppyDone(puppyHousePt + Point2(128.0, -128.0));
      
      levelState = LEVEL_WIN;
      
      billyState = BILLYS_RETURNING;
      callback(&billyJumpTimer);
    }
  }
  else if (billyState == BILLYS_RETURNING)
  {
    billyJumpTimer.update();
  }
  
  // stop player while they are winning
  if (levelState == LEVEL_WIN)
  {
    player->set_velocity(Point2(0.0, player->get_velocity().y));
    player->set_ang_vel(0.0);
  }
}

void PuppyLevel::drawHUD()
{
  PhysicsLevelStd::drawHUD();
  /*
  imgCache[skullBarFrameImg]->draw(Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y));
  
  String2 text(8);
  text = "$/%";
  text.int_replace('$', puppiesTotal - puppiesToFind);
  text.int_replace('%', puppiesTotal);
  ResourceManager::font->draw(text, Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y) +
                              Point2(63.0, -44.0), Point2(1.0, 1.0), COLOR_FULL);
   */
  Point1 displayTime = activeGameplayData->currTime;
  Point2 camTR = deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-8.0, 24.0);
  String2 timeStr("$");
  timeStr.time_string('$', displayTime, 1, false);
  getFont(futura130Font)->draw_in_box(timeStr, camTR, 0.6, 128.0, HANDLE_TR, WHITE_SOLID);
}

void PuppyLevel::colorizeAfterTerrain()
{
  RMGraphics->pop_camera();
  RMGraphics->colorize_grey_noise(Quad(RM_BOX), getImg(bgLayer8), 0.25);
  RMGraphics->push_camera(rmboxCamera);
}

Coord1 PuppyLevel::calcWinBonus()
{
  return 250 * levelID;
}

void PuppyLevel::callback(ActionEvent* caller)
{
  if (caller == &billyJumpTimer)
  {
    if (currJumpingBilly < puppyList.count)
    {
      puppyList[currJumpingBilly]->puppyJumpHome();
      currJumpingBilly++;
    }
  }
  
  /*
  if (caller == &arrowTimer)
  {
    Coord1 horiArrowDir = objectiveDir > 0.0 ? 1 : -1;
    Coord1 finalArrowDir = randGen.randl() ? 0 : horiArrowDir;
    arrowFromOffscreen(finalArrowDir, true);
  }
  else if (caller == &bombTimer)
  {
    createBomb(Point2(player->getX() + PLAY_W * 0.7 * objectiveDir, camera.myBox().top() - 64.0),
               Point2(0.0, 0.0));
  }
  else if (caller == &billyJumpTimer)
  {
    if (currJumpingBilly < puppyList.count)
    {
      puppyList[currJumpingBilly]->puppyJumpHome();
      currJumpingBilly++;
    }
  }
  else if (caller == &junkTimer)
  {
    Coord1 camCorner = objectiveDir > 0.0 ? 1 : 0;
    Point1 randX = RM::randf(player->collisionBox().corner(camCorner).x, camera.myBox().corner(camCorner).x);
    Point2 createLocation = Point2(randX, camera.myBox().top() - 64.0);
    createMeat(createLocation);
  }
  else if (caller == &crateTimer)
  {
    Coord1 numToMake = RM::randi(1, 3);
    
    for (Coord1 i = 0; i < numToMake; ++i)
    {
      Coord1 camCorner = objectiveDir > 0.0 ? 1 : 0;
      Point1 randX = RM::randf(camBox().corner(camCorner).x - 128.0,
                               player->collisionBox().corner(camCorner).x - 128.0);
      Point1 randY = camBox().top() - RM::randf(64.0, 96.0);
      createPuppyJunkCrate(Point2(randX, randY));
      cratesMade++;
    }
    
    if (cratesMade >= 20)
    {
      crateTimer.setInactive();
    }
    else
    {
      crateTimer.reset(3.5 + 1.5 * numToMake);
    }
  }
   */
}

// ==================================== PULevel0

PULevel0::PULevel0() :
  PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void PULevel0::createLevel()
{
  flameBalloonBucketChunk(false, true, false, false, false, false, true, ENEMY_NONE);
  flameSpearGuardsChunk(false, false, true, false, false, GL_ENEMY_DONUT);
  spikeyMountainChunk(LEVEL_TYPE_PUPPY, GL_ENEMY_SPEARMAN_VERT, GL_ENEMY_BOMBBAT);
  spearpitChunk(true, false, false, GL_ENEMY_BOMBBAT);
}

// ==================================== PULevel1

PULevel1::PULevel1() :
PuppyLevel()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void PULevel1::createLevel()
{
  dipBallChunk(true, false, false, false, ENEMY_NONE);
  flamePitsChunk(false, false, true, false, PU_ENEMY_CHICKEN);
  pirateWater(false, PU_ENEMY_MOHAWK);
  flameBounceChunk(false, false, false, false, true);
}



// ==================================== PULevel2

PULevel2::PULevel2() :
PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

void PULevel2::createLevel()
{
  pitpitChunk(true, false, false, GL_ENEMY_BOMBBAT);
  pirateWater(false, GL_ENEMY_SPEARMAN_SIDE);
  puppySpikeFlight(LEVEL_TYPE_PUPPY, true, GL_ENEMY_SPEARMAN_VERT);
  puppySpinnerSpan(true, false, false, false, GL_ENEMY_BOMBBAT);
}



// ==================================== PULevel3

PULevel3::PULevel3() :
PuppyLevel()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_GIANT);

}

void PULevel3::createLevel()
{
  angrySkyhenge(false, true);
  flameBalloonBucketChunk(false, true, false, false, false, false, true, ENEMY_NONE);
  flamePitsChunk(false, false, true, false, PU_ENEMY_MOHAWK);
  puppySpinnerSpan(true, false, false, false, GL_ENEMY_GIANT);
}



// ==================================== PULevel4

PULevel4::PULevel4() :
PuppyLevel()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void PULevel4::createLevel()
{
  waterChunk();
  angrySpinners(false, true, false, false, PU_ENEMY_FLAMESKULL);
  spearpitChunk(true, false, false, GL_ENEMY_WISP);
}



// ==================================== PULevel5

PULevel5::PULevel5() :
PuppyLevel()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_WISP);
}

void PULevel5::createLevel()
{
  flamePitsChunk(false, false, true, false, PU_ENEMY_MOHAWK);
  dipBallChunk(true, true, false, false, ENEMY_NONE);
  pitpitChunk(true, false, false, GL_ENEMY_WISP);
}



// ==================================== PULevel6

PULevel6::PULevel6() :
PuppyLevel()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);

}

void PULevel6::createLevel()
{
  angryCrossing(false, false, false, true);
  maplebarsChunk(LEVEL_TYPE_PUPPY, GL_ENEMY_CHIP);
  angrySpinners(false, true, false, false, PU_ENEMY_MOHAWK);
}



// ==================================== PULevel7

PULevel7::PULevel7() :
PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);

}

void PULevel7::createLevel()
{
  waterChunk();
  sprintJumpChoice(false);
  flameSpikeblocks(false, true, false, false, GL_ENEMY_ALIEN);
  flameFlyer(false, true, false, false, PU_ENEMY_MOHAWK);
}



// ==================================== PULevel8

PULevel8::PULevel8() :
PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_REAPER);

}

void PULevel8::createLevel()
{
  angryPit(false, true, false, GL_ENEMY_CHIP);
  angryBouncePyramid(false, PU_ENEMY_FLAMESKULL);
  dipBallChunk(true, false, false, false, GL_ENEMY_SPINY);
  flamePitsChunk(false, false, true, false, PU_ENEMY_FLAMESKULL);
}



// ==================================== PULevel9

PULevel9::PULevel9() :
PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void PULevel9::createLevel()
{
  puppyMoat(true, false, GL_ENEMY_WISP, false, GL_ENEMY_REAPER);
  spearpitChunk(true, false, false, GL_ENEMY_CHIP);
  puppySpikeFlight(LEVEL_TYPE_PUPPY, true, GL_ENEMY_ALIEN);
  puppySpikeFlight(LEVEL_TYPE_PUPPY, true, GL_ENEMY_REAPER);
  puppySpinnerSpan(true, false, false, false, GL_ENEMY_REAPER);
}

// ==================================== PULevelTest

PULevelTest::PULevelTest() :
  PuppyLevel()
{
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_SPINY);
}

void PULevelTest::createLevel()
{
  puppySpinnerSpan(true, false, false, false, GL_ENEMY_WISP);
  puppySpinnerSpan(true, false, false, false, GL_ENEMY_WISP);
}



// ==================================== SprintLevelStd ================================ //

SprintLevelStd::SprintLevelStd() :
  PhysicsLevelStd(),

  timeAllowed(60.0)
{
  objman.bgTerrainList.ensure_capacity(256);
  
  musicFilename = "puppy.ima4";
}

void SprintLevelStd::preload()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);

  background = createRegionalBackground(levelRegion, this);
  background->foregroundDisabled = true;
}

// overrides
void SprintLevelStd::loadRes()
{
  loadEnabledEnemies();
  loadGenericChunkObjects();
  // loadImage(sprintFlagImg, "sprint_flag", HANDLE_BL);
  loadSpine(sprintFlagSpine, "sprintflag");
  
  background->load();
}

void SprintLevelStd::unloadRes()
{
  
}

void SprintLevelStd::initSublevel()
{
  endpoint = Point2(0.0, 416.0);
  
  starterChunk();
  createLevel();
  sprintEndingRight();
  
  player->load();
  startStanding(Point2(656.0, 416.0));
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void SprintLevelStd::createLevel()
{
  // createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void SprintLevelStd::updateCamera(Logical snapIntoPlace)
{
  updateCameraPuppy(snapIntoPlace);
}

void SprintLevelStd::updateLevelAI()
{
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  if (player->getX() >= flagStartPt.x)
  {
    levelState = LEVEL_WIN;

    player->set_velocity(Point2(player->get_velocity().x * 0.5, player->get_velocity().y));
    player->set_ang_vel(player->get_ang_vel() * 0.5);

    ActionQueue* winScript = new ActionQueue();
    winScript->wait(2.0);
    winScript->enqueueX(new LevelOverCommand(this));
    addAction(winScript);
  }
  else if (activeGameplayData->currTime >= timeAllowed)
  {
    loseLevel();
  }
  else if (lostToFallingStd() == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void SprintLevelStd::drawHUD()
{
  PhysicsLevelStd::drawHUD();
  
  Point1 displayTime = std::max(timeAllowed - activeGameplayData->currTime, 0.0);
  Point2 camTR = deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-8.0, 8.0);
  String2 timeStr("$");
  timeStr.time_string('$', displayTime, 1, false);
  getFont(futura130Font)->draw_in_box(timeStr, camTR, 0.75, 128.0, HANDLE_TR, WHITE_SOLID);
}

Coord1 SprintLevelStd::calcWinBonus()
{
  return 250 * levelID;
}

void SprintLevelStd::callback(ActionEvent* caller)
{
  
}

// ==================================== SPLevel0

SPLevel0::SPLevel0() :
  SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

void SPLevel0::createLevel()
{
  flameBalloonBucketChunk(false, false, false, false, true, false, false, ENEMY_NONE);
  bouncerAChunk(false, false, false, false);
  flameBalloonBucketChunk(false, true, false, false, false, false, false, GL_ENEMY_SPEARMAN_SIDE);
  pirateWater(false, ENEMY_NONE);
  sprintSpikejump(false);
}

// ==================================== SPLevel1

SPLevel1::SPLevel1() :
SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
}

void SPLevel1::createLevel()
{
  pirateGourdoLantern(false, false, ENEMY_NONE);
  cutleryChunk(LEVEL_TYPE_SPRINT, ENEMY_NONE, GL_ENEMY_SPEARMAN_SIDE);
  flameBalloonBucketChunk(false, true, false, false, false, false, false, GL_ENEMY_SPEARMAN_VERT);
  flameSpikeyStepsChunk(false, false, true, false, GL_ENEMY_SPEARMAN_SIDE);
}

// ==================================== SPLevel2

SPLevel2::SPLevel2() :
  SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
}

void SPLevel2::createLevel()
{
  angryMagicPyramid(false, ENEMY_NONE, GL_ENEMY_BOMBBAT);
  waterChunk();
  sprintJumpChoice(false);
  spikeyMountainChunk(LEVEL_TYPE_SPRINT, GL_ENEMY_SPEARMAN_UNARMED, ENEMY_NONE);
}


// ==================================== SPLevel3

SPLevel3::SPLevel3() :
SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
}

void SPLevel3::createLevel()
{
  dipBallChunk(false, true, false, false, GL_ENEMY_BOMBBAT);
  angrySpikeblock(false);
  bouncerAChunk(false, false, false, false);
  sprintCavemen(false, false, false, false, GL_ENEMY_CAVEMAN);
}


// ==================================== SPLevel4

SPLevel4::SPLevel4() :
SprintLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(PU_ENEMY_BOMB);
}

void SPLevel4::createLevel()
{
  sprintBombtower(false, true);
  maplebarsChunk(false, GL_ENEMY_BOMBBAT);
  flyerChunk(PU_ENEMY_MOHAWK, PU_ENEMY_CHICKEN, PU_ENEMY_MOHAWK);
//  sprintBounceChasm(false);
}


// ==================================== SPLevel5

SPLevel5::SPLevel5() :
SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_DONUT);

}

void SPLevel5::createLevel()
{
  sprintSpikejump(false);
  sprintSpearmanPitA(false, GL_ENEMY_GIANT);
  sprintSpikejump(false);
  sprintSpikeys(false, GL_ENEMY_SPEARMAN_VERT, PU_ENEMY_MOHAWK);
  dipBallChunk(false, false, false, false, GL_ENEMY_DONUT);
}


// ==================================== SPLevel6

SPLevel6::SPLevel6() :
SprintLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
}

void SPLevel6::createLevel()
{
  waterChunk();
  pirateGourdoLantern(false, false, PU_ENEMY_FLAMESKULL);
  waterChunk();
  flameBalloonBucketChunk(false, false, false, false, true, false, false, ENEMY_NONE);
  angrySpikeblock(false);
}


// ==================================== SPLevel7

SPLevel7::SPLevel7() :
SprintLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void SPLevel7::createLevel()
{
  flameFlyer(false, false, false, false, PU_ENEMY_CHICKEN);
  angrySkyhenge(false, false);
  pirateWater(false, PU_ENEMY_MOHAWK);
}


// ==================================== SPLevel8

SPLevel8::SPLevel8() :
SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_CHIP);

}

void SPLevel8::createLevel()
{
  sprintBounceChasm(false);
  sprintDozerDig(false, false, false, GL_ENEMY_WISP);
  sprintSpikeys(false, GL_ENEMY_SCIENTIST, PU_ENEMY_FLAMESKULL);
  maplebarsChunk(LEVEL_TYPE_SPRINT, GL_ENEMY_CHIP);
}


// ==================================== SPLevel9

SPLevel9::SPLevel9() :
SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(PU_ENEMY_BOMB);
}

void SPLevel9::createLevel()
{
  sprintSpearmanPitA(false, GL_ENEMY_GIANT);
  sprintDozerDig(false, false, false, GL_ENEMY_ALIEN);
  sprintDozerDig(false, false, false, GL_ENEMY_REAPER);
  waterChunk();
  waterChunk();
  flyerChunk(PU_ENEMY_CHICKEN, PU_ENEMY_FLAMESKULL, PU_ENEMY_FLAMESKULL);
}


// ==================================== SPLevelTest

SPLevelTest::SPLevelTest() :
  SprintLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void SPLevelTest::createLevel()
{
  waterChunk();
  angrySpinners(true, false, false, false, PU_ENEMY_FLAMESKULL);
  dipBallChunk(false, true, false, false, GL_ENEMY_SPEARMAN_UNARMED);
  bouncerAChunk(false, false, false, false);
  sprintJumpChoice(false);
}

// ==================================== PirateLevel ================================ //

PirateLevel::PirateLevel() :
  PhysicsLevelStd(),

  startedWin(false),
  timeAllowed(300.0)
{
  objman.bgTerrainList.ensure_capacity(256);
  
  musicFilename = "puppy.ima4";
}

void PirateLevel::preload()
{
  background = createRegionalBackground(levelRegion, this);
  background->foregroundDisabled = true;
}

// overrides
void PirateLevel::loadRes()
{
  // loadImage(pirateShipImg, "pirateship");
  loadSpine(pirateShipSpine, "pirateship");
  // loadTiles(pirateChestSet, "treasure_chest", 2, 1, HANDLE_C);
  loadSpine(pirateChestSpine, "chest");
  // loadTiles(pirateGemSet, "gems", 4, 1, HANDLE_C);
  loadTiles(gemBluePickupSet, "gem_blue", 4, 4, HANDLE_C);
  loadTiles(gemGreenPickupSet, "gem_green", 4, 4, HANDLE_C);
  loadTiles(gemRedPickupSet, "gem_red", 4, 4, HANDLE_C);
  loadTiles(gemYellowPickupSet, "gem_yellow", 4, 4, HANDLE_C);

  loadEnabledEnemies();
  loadGenericChunkObjects();

  background->load();
}

void PirateLevel::unloadRes()
{
  
}

void PirateLevel::initSublevel()
{
  endpoint = Point2(0.0, 416.0);

  starterChunk();
  createLevel();
  pirateShipChunk();
  // addTextEffectStd(LSTRINGS[101], Point2(PLAY_CX, PLAY_CY));
  
  // worldBox = Box(0.0, -RM_WH.y * 2.0, endpoint.x, RM_WH.y * 3.0);
  // invisibleWallsStd(worldBox);
  
  player->load();
  startStanding(Point2(656.0, 416.0));
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void PirateLevel::createLevel()
{
  // createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void PirateLevel::updateCamera(Logical snapIntoPlace)
{
  updateCameraPuppy(snapIntoPlace);
}

void PirateLevel::updateLevelAI()
{
  // randomly disturb water under ship
  shipWater->simBox.disturb(shipWater->myAABB.random_pt().x - shipWater->myAABB.left(), 4.0,
                            6.0);
  
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  if (startedWin == false &&
      player->collisionBox().right() >= pirateWinBox.left())
  {
    // levelState = LEVEL_WIN;
    // player->set_velocity(Point2(0.0, player->get_velocity().y));
    
    startedWin = true;
    
    ActionQueue* winScript = new ActionQueue();
    
    winScript->wait(0.25);
    
    ActionQueue* openScript = new ActionQueue();
    openScript->enqueueX(new StartSpineCommand(&pirateChest->mySpineAnim, "open", 0, false));
    openScript->wait(1.5);  // this is the fly time of a gem
    openScript->enqueueX(new StartSpineCommand(&pirateChest->mySpineAnim, "open_gemget", 0, true));

    winScript->enqueueAddX(openScript, &objman.actions);

    for (Coord1 gemType = FIRST_GEM_TYPE; gemType <= LAST_GEM_TYPE; ++gemType)
    {
      for (Coord1 i = 0; i < activeGameplayData->collectedCoins[gemType]; ++i)
      {
        winScript->enqueueMidCallback(this, gemType);
        winScript->wait(0.25);
      }
    }
    
//    ActionQueue* closeScript = new ActionQueue();
    // closeScript->wait(0.5);
//    closeScript->enqueueX(new StartSpineCommand(&pirateChest->mySpineAnim, "close", 0));
//    winScript->enqueueAddX(closeScript, &objman.actions);

    winScript->wait(1.25);
    winScript->enqueueX(new StartSpineCommand(&pirateChest->mySpineAnim, "close", 0, false));
    winScript->wait(0.5);
    winScript->enqueueX(new LevelOverCommand(this));
    addAction(winScript);
  }
  else if (lostToFallingStd() == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void PirateLevel::drawPirateShipFront()
{
  if (pirateShip->onRMScreen() == true)
  {
    pirateShip->renderSpineAnim(pirateShip->frontAnim);
  }
}

void PirateLevel::drawHUD()
{
  /*
   imgCache[skullBarFrameImg]->draw(Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y));
   
   String2 text(8);
   text = "$/%";
   text.int_replace('$', puppiesTotal - puppiesToFind);
   text.int_replace('%', puppiesTotal);
   ResourceManager::font->draw(text, Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y) +
   Point2(63.0, -44.0), Point2(1.0, 1.0), COLOR_FULL);
   */
  PhysicsLevelStd::drawHUD();
  
  Point1 displayTime = std::max(timeAllowed - activeGameplayData->currTime, 0.0);
  Point2 camTR = deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-8.0, 8.0);
  String2 timeStr("$");
  timeStr.time_string('$', displayTime, 1, false);
  getFont(futura130Font)->draw_in_box(timeStr, camTR, 0.75, 128.0, HANDLE_TR, WHITE_SOLID);
}

Coord1 PirateLevel::calcWinBonus()
{
  return 250 * levelID;
}

void PirateLevel::midCallback(Coord1 value)
{
  if (value >= FIRST_GEM_TYPE && value <= LAST_GEM_TYPE)
  {
    PizzaCoinStd* gem = new PizzaCoinStd(player->getXY(), value);
    gem->phase = PHASE_PIRATE_GEMS;
    gem->lifeState = RM::PASSIVE;
    addPGO(gem);
    
    Point1 flyTime = 1.5;

    addAction(new GemSpiraler(gem, pirateChest));
  }
}

void PirateLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== PILevel0

GemSpiraler::GemSpiraler(PizzaCoinStd* setGem, PirateChest* setChest) :
  ActionEvent(),
  ActionListener(),

  gem(setGem),
  chest(setChest),

  currDistance(0.0),
  currAngle(0.0),

  distanceMover(&currDistance, 0.0, 1.5),
  angleMover(&currAngle, 1.0, 1.5)
{
  Point2 startPolar = RM::cart_to_pol(gem->getXY() - chest->getXY());
  
  currDistance = startPolar.x;
  currAngle = startPolar.y;
  
  distanceMover.setListener(this);
  angleMover.reset(currAngle + TWO_PI * 1.25);
}

void GemSpiraler::updateMe()
{
  distanceMover.update();
  angleMover.update();
  gem->setXY(RM::pol_to_cart(currDistance, currAngle) + chest->getXY());
}

void GemSpiraler::callback(ActionEvent* caller)
{
  gem->setRemove();
  done();
}

// ==================================== PILevel0

PILevel0::PILevel0() :
  PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void PILevel0::createLevel()
{
  puppyMoat(false, true, ENEMY_NONE, false, GL_ENEMY_SPEARMAN_SIDE);
  sprintJumpChoice(true);
  waterChunk();
  pirateWater(true, PU_ENEMY_MOHAWK);
}

// ==================================== PILevel1

PILevel1::PILevel1() :
  PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
}

void PILevel1::createLevel()
{
  puppySpinnerSpan(false, true, false, false, GL_ENEMY_CAVEMAN);
  angryToppleTower(false, true);
  sprintBounceChasm(true);
  sprintSpikeys(true, GL_ENEMY_SPEARMAN_VERT, PU_ENEMY_MOHAWK);
}

// ==================================== PILevel2

PILevel2::PILevel2() :
PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(PU_ENEMY_BOMB);

}

void PILevel2::createLevel()
{
  sprintBombtower(false, true);
  sprintBounceChasm(true);
  pitpitChunk(false, false, false, GL_ENEMY_BOMBBAT);
  flameSpinners(false, true, GL_ENEMY_BOMBBAT);
}

// ==================================== PILevel3

PILevel3::PILevel3() :
PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);

}

void PILevel3::createLevel()
{
  angryCrossing(false, true, false, false);
  pirateFBlocks(RANDOM_GEM, GL_ENEMY_GIANT);
  pirateWater(true, PU_ENEMY_CHICKEN);
}

// ==================================== PILevel4

PILevel4::PILevel4() :
  PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
}

void PILevel4::createLevel()
{
  sprintDozerDig(true, false, false, GL_ENEMY_DOZER);
  cutleryChunk(LEVEL_TYPE_PIRATE, GL_ENEMY_UTENSIL, GL_ENEMY_DONUT);
  puppyMoat(false, true, ENEMY_NONE, false, GL_ENEMY_SPEARMAN_UNARMED);
}

// ==================================== PILevel5

PILevel5::PILevel5() :
PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_CAVEMAN);
  enemiesEnabled.add(GL_ENEMY_CHIP);

}

void PILevel5::createLevel()
{
  flameSpearGuardsChunk(false, true, false, false, false, GL_ENEMY_CAVEMAN);
  angryToppleTower(false, true);
  angryCrossing(false, true, false, false);
  pitpitChunk(false, false, false, GL_ENEMY_CHIP);
}

// ==================================== PILevel6

PILevel6::PILevel6() :
PirateLevel()
{
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_GIANT);
}

void PILevel6::createLevel()
{
  angrySpinners(false, false, true, false, PU_ENEMY_FLAMESKULL);
  angryPit(false, false, true, GL_ENEMY_BOMBBAT);
  bouncerAChunk(false, true, false, false);
  pirateFBlocks(RANDOM_GEM, GL_ENEMY_GIANT);
}

// ==================================== PILevel7

PILevel7::PILevel7() :
PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_GIANT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
}

void PILevel7::createLevel()
{
  puppyMoat(false, true, GL_ENEMY_WISP, false, GL_ENEMY_GIANT);
  sprintJumpChoice(true);
  sprintJumpChoice(true);
  flameBounceChunk(false, false, false, true, false);
  angryMagicPyramid(false, PU_ENEMY_FLAMESKULL, GL_ENEMY_SPEARMAN_UNARMED);
}

// ==================================== PILevel8

PILevel8::PILevel8() :
PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_UTENSIL);
  enemiesEnabled.add(GL_ENEMY_CUPID);

}

void PILevel8::createLevel()
{
  angryToppleTower(false, true);
  pirateFBlocks(RANDOM_GEM, GL_ENEMY_REAPER);
  pirateGourdoLantern(true, false, PU_ENEMY_FLAMESKULL);
  angryToppleTower(false, true);
  cutleryChunk(LEVEL_TYPE_PIRATE, GL_ENEMY_UTENSIL, GL_ENEMY_CUPID);
}

// ==================================== PILevel9

PILevel9::PILevel9() :
  PirateLevel()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(GL_ENEMY_CHIP);
}

void PILevel9::createLevel()
{
  angryBouncePyramid(false, GL_ENEMY_CHIP);
  sprintSpikeys(true, GL_ENEMY_CHIP, PU_ENEMY_MOHAWK);
  puppyMoat(false, true, GL_ENEMY_CHIP, false, GL_ENEMY_CHIP);
  flameSpikeyStepsChunk(false, false, false, true, GL_ENEMY_CHIP);
  sprintBounceChasm(true);
  sprintBounceChasm(true);
}


// ==================================== PILevelTest

PILevelTest::PILevelTest() :
  PirateLevel()
{
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
}

void PILevelTest::createLevel()
{
  pirateWater(true, PU_ENEMY_FLAMESKULL);
  pirateWater(true, PU_ENEMY_FLAMESKULL);
}

// ==================================== FiremanLevelStd ================================ //

FiremanLevelStd::FiremanLevelStd() :
  PhysicsLevelStd(),

  timeAllowed(120.0)
{
  objman.bgTerrainList.ensure_capacity(256);
  
  musicFilename = "puppy.ima4";
}

void FiremanLevelStd::preload()
{
  background = new FiremanBackground();
  background->level = this;
  background->foregroundDisabled = true;
}

// overrides
void FiremanLevelStd::loadRes()
{
  loadSpine(balloonBasketSpine, "bucket");
  loadSpine(balloonSpine, "balloon");
  loadImage(splashImg, "splash", HANDLE_C);
  loadTiles(firemanFlameSet, "fireman_flame", 4, 4, Point2(84.0, 218.0), true);
  
  loadTiles(firemanTeddySet, "fireman_teddy", 2, 1, Point2(0.0, 32.0));
  loadTiles(firemanPortaSet, "fireman_portapotty", 2, 1, Point2(16, 16));
  loadTiles(firemanDavidSet, "fireman_david", 2, 1, Point2(16, 48));
  loadTiles(firemanBooksSet, "fireman_books", 2, 1, Point2(32, 16));
  loadTiles(firemanCrownSet, "fireman_crown", 2, 1, Point2(16, 32));
  loadTiles(firemanHornSet, "fireman_horn", 2, 1, Point2(16, 16));
  loadTiles(firemanNuclearSet, "fireman_nuclear", 2, 1, Point2(16, 16));
  loadTiles(firemanVanSet, "fireman_van", 2, 1, Point2(32, 16));
  loadTiles(firemanWaffleSet, "fireman_waffle", 2, 1, Point2(16, 16));
  loadTiles(firemanMonaLisaSet, "fireman_monalisa", 2, 1, Point2(16, 16));
  loadTiles(firemanComputerSet, "fireman_computer", 2, 1, Point2(16, 16));
  loadTiles(firemanStonehengeSet, "fireman_stonehenge", 2, 1, Point2(32, 16));
  loadTiles(firemanMotorcycleSet, "fireman_motorcycle", 2, 1, Point2(32, 16));
  loadTiles(firemanBurgerSet, "fireman_burger", 2, 1, Point2(16, 32));
  loadTiles(firemanSynthSet, "fireman_synth", 2, 1, Point2(16, 16));
  loadTiles(firemanCartSet, "fireman_cart", 2, 1, Point2(48, 48));
  loadTiles(firemanBarrowSet, "fireman_barrow", 2, 1, Point2(112, 48));
  
  loadEnabledEnemies();
  loadGenericChunkObjects();

  background->load();
}

void FiremanLevelStd::unloadRes()
{
  
}

void FiremanLevelStd::initSublevel()
{
  endpoint = Point2(0.0, 416.0);
  
  starterChunk();
  createLevel();
  // addTextEffectStd(LSTRINGS[101], Point2(PLAY_CX, PLAY_CY));
  
  // worldBox = Box(0.0, -RM_WH.y * 2.0, endpoint.x, RM_WH.y * 3.0);
  // invisibleWallsStd(worldBox);
  
  player->load();
  startStanding(Point2(656.0, 416.0));
  
  rmboxCamera.xy.x = player->getX();
  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
}

void FiremanLevelStd::createLevel()
{
  // createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void FiremanLevelStd::updateCamera(Logical snapIntoPlace)
{
  updateCameraFree(snapIntoPlace);
  // rmboxCamera.setZoom(0.1);
}

void FiremanLevelStd::updateLevelAI()
{
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  // this is technically for sprint
  if (activeGameplayData->objectsExtinguished >= activeGameplayData->objectsIgnited)
  {
    winLevel();
  }
  else if (activeGameplayData->currTime >= timeAllowed)
  {
    loseLevel();
  }
  else if (lostToFallingStd() == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void FiremanLevelStd::drawHUD()
{
  PhysicsLevelStd::drawHUD();

  Point1 displayTime = std::max(timeAllowed - activeGameplayData->currTime, 0.0);
  Point2 camTR = deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-8.0, 8.0);
  String2 timeStr("$");
  timeStr.time_string('$', displayTime, 1, false);
  getFont(futura130Font)->draw_in_box(timeStr, camTR, 0.75, 128.0, HANDLE_TR, WHITE_SOLID);
  
  Coord1 numBurningnLeft = activeGameplayData->objectsIgnited - activeGameplayData->objectsExtinguished;
  getFont(futura130Font)->draw_in_box(String2("$", '$', numBurningnLeft),
                                      camTR + Point2(0.0, 32.0), 0.75, 128.0, HANDLE_TR, RED_SOLID);

  if (player->balloonList.count == 0)
  {
    Coord1 basketDir = closestBalloonsDir();
    String2 basketDirStr = "<-- Closest balloons";
    
    if (basketDir == 1)
    {
      basketDirStr = "Closest balloons -->";
    }
    
    Point2 camBC = deviceSafeBox().norm_pos(HANDLE_BC);
    getFont(futura130Font)->draw_in_box(basketDirStr,
                                        camBC + Point2(0.0, -32.0), 0.75, 256.0, HANDLE_BC, CYAN_SOLID);
  }
}

Coord1 FiremanLevelStd::closestBalloonsDir()
{
  Coord1 dir = 0;
  
  Point1 closestDistance = 100000.0;
  PizzaGOStd* closestBasket = NULL;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->value == FI_BALLOON_BASKET)
    {
      Point1 currDistance = std::abs(enemy->getX() - player->getX());
      
      if (currDistance < closestDistance)
      {
        closestDistance = currDistance;
        closestBasket = enemy;
      }
    }
  }
  
  if (closestBasket != NULL)
  {
    dir = RM::sign(closestBasket->getX() - player->getX());
  }
  
  return dir;
}

Coord1 FiremanLevelStd::calcWinBonus()
{
  return 250 * levelID;
}

void FiremanLevelStd::callback(ActionEvent* caller)
{
  
}

// ==================================== FILevel0

FILevel0::FILevel0() :
  FiremanLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void FILevel0::createLevel()
{
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameSpearGuardsChunk(true, false, false, false, false, GL_ENEMY_SPEARMAN_UNARMED);
  flameSpikeyStepsChunk(true, false, false, false, GL_ENEMY_SPEARMAN_VERT);
  pirateWater(false, PU_ENEMY_CHICKEN);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameBounceChunk(true, false, false, false, false);
}

// ==================================== FILevel1

FILevel1::FILevel1() :
  FiremanLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void FILevel1::createLevel()
{
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  sprintSpearmanPitA(false, GL_ENEMY_SPEARMAN_SIDE);
  flameBalloonBucketChunk(false, false, false, false, false, true, false, ENEMY_NONE);
  pirateGourdoLantern(false, false, PU_ENEMY_CHICKEN);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  spearpitChunk(false, false, true, GL_ENEMY_BOMBBAT);
}

// ==================================== FILevel2

FILevel2::FILevel2() :
  FiremanLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);

}

void FILevel2::createLevel()
{
  flameBalloonBucketChunk(true, true, false, false, false, false, false, ENEMY_NONE);
  angrySpinners(false, false, false, true, PU_ENEMY_CHICKEN);
  angrySpinners(false, false, false, true, PU_ENEMY_CHICKEN);
  flameBounceChunk(true, false, false, false, false);
  flameBounceChunk(true, false, false, false, false);
  flameBalloonBucketChunk(true, true, false, false, false, false, false, ENEMY_NONE);
}

// ==================================== FILevel3

FILevel3::FILevel3() :
  FiremanLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_SCIENTIST);
  enemiesEnabled.add(GL_ENEMY_DONUT);
}

void FILevel3::createLevel()
{
  puppyMoat(false, false, GL_ENEMY_BOMBBAT, true, GL_ENEMY_SCIENTIST);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  puppySpinnerSpan(false, false, false, true, GL_ENEMY_DONUT);
  sprintCavemen(false, false, false, true, GL_ENEMY_SCIENTIST);
}

// ==================================== FILevel4

FILevel4::FILevel4() :
  FiremanLevelStd()
{
  
}

void FILevel4::createLevel()
{
  flameBounceChunk(true, false, false, false, false);
  flameBounceChunk(true, false, false, false, false);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  bouncerAChunk(false, false, true, false);
  bouncerAChunk(false, false, true, false);
  bouncerAChunk(false, false, true, false);
}

// ==================================== FILevel5

FILevel5::FILevel5() :
  FiremanLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_SIDE);

}

void FILevel5::createLevel()
{
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameSpearGuardsChunk(true, false, false, false, false, GL_ENEMY_SPEARMAN_SIDE);
  flameSpikeyStepsChunk(true, false, false, false, GL_ENEMY_CUPID);
  flyerChunk(PU_ENEMY_CHICKEN, PU_ENEMY_MOHAWK, PU_ENEMY_FLAMESKULL);
  flamePitsChunk(true, false, false, false, PU_ENEMY_MOHAWK);
}

// ==================================== FILevel6

FILevel6::FILevel6() :
  FiremanLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(GL_ENEMY_SPINY);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_CHIP);
}

void FILevel6::createLevel()
{
  flameSpearGuardsChunk(true, false, false, false, false, GL_ENEMY_CUPID);
  spikeyMountainChunk(LEVEL_TYPE_FIREMAN, GL_ENEMY_SPINY, GL_ENEMY_WISP);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameSpinners(true, false, GL_ENEMY_CHIP);
  flameSpikeblocks(true, false, false, false, GL_ENEMY_SPINY);
  flameSpikeyStepsChunk(true, false, false, false, GL_ENEMY_CHIP);
}

// ==================================== FILevel7

FILevel7::FILevel7() :
  FiremanLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_UNARMED);

}

void FILevel7::createLevel()
{
  flameSpikeblocks(true, false, false, false, GL_ENEMY_REAPER);
  flameSpikeyStepsChunk(true, false, true, false, GL_ENEMY_CHIP);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  sprintSpikejump(false);
  puppySpikeFlight(LEVEL_TYPE_FIREMAN, false, GL_ENEMY_SPEARMAN_UNARMED);
  flameBounceChunk(true, false, false, false, false);
}

// ==================================== FILevel8

FILevel8::FILevel8() :
  FiremanLevelStd()
{
  enemiesEnabled.add(GL_ENEMY_ALIEN);
  enemiesEnabled.add(GL_ENEMY_CUPID);
  enemiesEnabled.add(GL_ENEMY_SPEARMAN_VERT);
}

void FILevel8::createLevel()
{
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameSpearGuardsChunk(true, false, false, false, false, GL_ENEMY_ALIEN);
  dipBallChunk(false, false, true, false, GL_ENEMY_CUPID);
  flameBounceChunk(true, false, false, false, false);
  pitpitChunk(false, false, true, GL_ENEMY_SPEARMAN_VERT);
}

// ==================================== FILevel9

FILevel9::FILevel9() :
  FiremanLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_DOZER);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
}

void FILevel9::createLevel()
{
  flamePitsChunk(true, false, false, false, PU_ENEMY_MOHAWK);
  flameFlyer(true, false, false, false, PU_ENEMY_FLAMESKULL);
  flameBalloonBucketChunk(true, false, false, false, false, false, false, ENEMY_NONE);
  flameFlyer(true, false, false, false, PU_ENEMY_FLAMESKULL);
  sprintDozerDig(false, false, true, GL_ENEMY_DOZER);
  flamePitsChunk(true, false, false, false, PU_ENEMY_CHICKEN);
}

// ==================================== FILevelTest

FILevelTest::FILevelTest() :
  FiremanLevelStd()
{
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(GL_ENEMY_REAPER);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
}

void FILevelTest::createLevel()
{
  flameFlyer(true, false, false, false, PU_ENEMY_CHICKEN);
  flameSpinners(true, false, GL_ENEMY_BOMBBAT);
}
// ==================================== LakeLevel ================================ //

LakeLevel::LakeLevel() :
  PizzaLevelInterface(),

  objman(),
  player(NULL),
  bg(NULL),

  cameraStaller(0.5),

  bubbleTimer(1.35, true, this),
  harpoonTimer(1.0, this),
  tideMax(0.0),
  tideData(0.5, 1.0),

  maxHearts(3),
  pearlsEaten(0),

  runnerPercent(0.0),
  chasePercent(0.0),
  poisonPercent(0.0)
{
  cameraStaller.setInactive();
  harpoonTimer.setInactive();
  shaker.setDuration(0.3);
  musicFilename = "lake.ima4";

  objman.init(NUM_PHASES);
}

void LakeLevel::preload()
{
  bg = new LakeBackground();
  bg->level = this;
}

void LakeLevel::loadRes()
{
  loadSpine(fish16Spine, "fish0");
  loadSpine(fish32Spine, "fisha");
  loadSpine(fish64Spine, "fishb");
  loadSpine(fish128Spine, "fishc");
  loadSpine(fish256Spine, "fishd");
  loadSpine(fish512Spine, "fishe");
  loadSpine(planktonSpine, "plankton");
  loadSpine(seahorseSpine, "seahorse");
  loadSpine(shark128Spine, "bonefish_small");
  loadSpine(shark256Spine, "bonefish_large");
  loadSpine(clamSpine, "clam");
  loadSpine(barrelSpine, "barrel");
  loadSpine(jellyLargeSpine, "jelly_large");
  loadSpine(jellyMedSpine, "jelly_med");
  loadSpine(jellySmallSpine, "jelly_small");
  loadSpine(lobsterSpine, "lobster");
  loadSpine(stingraySpine, "stingray");
  loadSpine(urchinSpine, "urchin");

  bg->load();
}

LakeLevel::~LakeLevel()
{
  delete bg;
  delete player;
}

void LakeLevel::unloadRes()
{
  
}

void LakeLevel::initRes()
{
  // initScoreText();
  
  bg->init();
  
  player = new LakePlayer();
  player->level = this;
  player->setXY(LAKE_PLAYER_START);
  player->load();
  
  updateCamera(true);
  
  initSublevel();
}

void LakeLevel::initSublevel()
{
  populateWorld();
}

void LakeLevel::update()
{
  // ========== modified time
  RM::push_time_mult(worldTimeMult);
  
  bg->update();
  
  objman.updateObjects();
  objman.actions.update();
  objman.garbageCollector();
  
  player->addX(objman.tide * RM::timePassed());
  updateSeasonings();
  player->update();
  
  if (levelState == LEVEL_PLAY)
  {
    RM::bounce_arcsine(objman.tide, tideData, Point2(-tideMax, tideMax), 0.1 * RM::timePassed());
    // Pizza::runGameTime(RM::timePassed());
    harpoonTimer.update();
    
    if (lostToDMGStd() == true)
    {
      loseLevel();
    }
  }
  
  updateControls();
  bubbleTimer.update();
  updateCamera(false);
  
  RM::pop_time_mult();
  // ==========
  
  objman.frontActions.update();
}

void LakeLevel::redraw()
{
  // ========== camera on
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);

  bg->drawBackground();

  rmboxCamera.apply();

  objman.drawPhase(PHASE_ENEMY_STD);
  player->redraw();

  bg->drawForeground();

  objman.actions.redraw();

  RMGraphics->pop_camera();
  // ========== camera off
  
  drawHUD();

  LAKE_WORLD_BOX.draw_outline(RED_SOLID);
 
  objman.frontActions.redraw();
}

void LakeLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  if (controller->getAnyConfirmStatus(JUST_PRESSED) == true)
  {
    player->tryJump();
  }
  
  player->tryMove(PhysicsLevelStd::tiltMagnitude());
  player->setFacing(PhysicsLevelStd::tiltMagnitude() < 0.0 ? -1 : 1);
}

void LakeLevel::updateCamera(Logical snapIntoPlace)
{
  shaker.update();
  
  // this is to facilitate other things moving the player
  if (levelState == LEVEL_LOSE) return;
  
  rmboxCamera.setXY(player->getXY() - RM_WH * 0.5);
  rmboxCamera.handle = RM_WH * 0.5;
  
  cameraStaller.update();
  
  if (cameraStaller.getActive() == false)
  {
    rmboxCamera.zoom.x = RM::flatten(rmboxCamera.zoom.x, 1.0 - 0.166 * (player->sizeIndex - LakeGO::SIZE_32),
                                     0.166 * RM::timePassed());
  }
  
  rmboxCamera.zoom.y = rmboxCamera.zoom.x;
  
  Box clampBox = Box::from_corners(LAKE_WORLD_BOX.norm_pos(HANDLE_TL) +
                                   rmcamboxToScreen(RM_BOX.norm_pos(HANDLE_TL)),
                                   LAKE_WORLD_BOX.norm_pos(HANDLE_BR) +
                                   rmcamboxToScreen(RM_BOX.norm_pos(HANDLE_BR)));
  rmboxCamera.clampToArea(clampBox);
}

void LakeLevel::playerGrew(Coord1 newSize)
{
  cameraStaller.reset();
}

void LakeLevel::addAction(ActionEvent* ae)
{
  objman.addAction(ae);
}

void LakeLevel::addFrontAction(ActionEvent* ae)
{
  objman.addFrontAction(ae);
}

void LakeLevel::ateFish(LakeGO* fish)
{
  player->ateFish(fish);
  if (fish->type == LakeGO::LAKE_TYPE_PEARL) pearlsEaten++;
  
  shaker.setMagnitude(fish->sizeIndex + 2.0);
  shaker.timedShake(0.3 + fish->sizeIndex * 0.1);
}

void LakeLevel::playerDamaged(Point1 dmg)
{
  if (levelState != LEVEL_PLAY)
  {
    return;
  }
  
  activeGameplayData->addHP(-dmg);

  if (activeGameplayData->currPlayerHP <= 0.0)
  {
    loseLevel();
  }
}

void LakeLevel::drawHUD()
{
  String2 playerDmgStr;
  playerDmgStr.double_string(activeGameplayData->currPlayerHP, 4, 1);
  getFont(futura130Font)->draw(playerDmgStr, deviceSafeBox().norm_pos(HANDLE_TL), Point2(0.6, 0.6),
                               0.0, HANDLE_TL, BLUE_SOLID);
  /*
  Coord1 currHearts = std::max(maxHearts - Pizza::currGameVars[VAR_DAMAGE], 0);
  
  for (Coord1 i = 0; i < maxHearts; ++i)
  {
    Logical filled = (i + 1) <= currHearts;
    Image* heartImg = imgsetCache[lakeHeartSet][(filled ? 0 : 1)];
    heartImg->draw(heartPos(i));
  }
   */
}

Point2 LakeLevel::heartPos(Coord1 heartID)
{
  return Point2(0.0, 0.0);
  // return Point2(28.0 + 40.0 * heartID, 24.0) + Pizza::platformTL;
}

void LakeLevel::placeLakeGOStd(LakeGO* lgo)
{
  Box createBox = LAKE_WORLD_BOX;
  createBox.xy.y += 128.0;
  createBox.grow_down(-256.0);
  
  Point2 target(RM::randf(createBox.left(), createBox.right()),
                RM::randf(createBox.top(), createBox.bottom()));
  lgo->setXY(target);
  
  Box fishBox = lgo->collisionCircle().AABB();
  fishBox.xy -= 64.0;
  fishBox.grow_right(128.0);
  fishBox.grow_down(128.0);
  
  if (fishBox.collision(rmboxCamera.myBox()) == true)
  {
    // this pushes it a screen away left or right. if it goes out of the
    // world bounds it will just get removed and another will generate
    lgo->addX((RM_WH.x + 128.0) * (lgo->getX() > player->getX() ? 1.0 : -1.0));
  }
}

LakeFish* LakeLevel::createNormalFish(Coord1 size)
{
  LakeFish* fish = new LakeFish(size);
  if (randGen.randf(0.0, 1.0) < runnerPercent) fish->willRun = true;
  if (randGen.randf(0.0, 1.0) < chasePercent) fish->willChase = true;
  if (randGen.randf(0.0, 1.0) < poisonPercent && size != LakeGO::SIZE_16) fish->poison = true;
  
  placeLakeGOStd(fish);
  addPGO(fish);
  return fish;
}

void LakeLevel::placeAndAddLGO(LakeGO* lgo)
{
  placeLakeGOStd(lgo);
  addPGO(lgo);
}

LakeFish* LakeLevel::createNormalFish(const LakeFish& original)
{
  LakeFish* fish = NULL;
  
  if (original.isShark == false)
  {
    fish = new LakeFish(original.sizeIndex);
  }
  else
  {
    fish = new LakeShark(original.sizeIndex);
  }
  
  fish->poison = original.poison;
  fish->willRun = original.willRun;
  fish->willChase = original.willChase;
  
  placeLakeGOStd(fish);
  addPGO(fish);
  return fish;
}

void LakeLevel::addPGO(LakeGO* pgo)
{
  pgo->level = this;
  pgo->player = player;
  objman.addPGO(pgo, pgo->phase);
  pgo->load();
}

/*
LakeBubble* LakeLevel::createBubble()
{
  LakeBubble* bubble = new LakeBubble();
  Box camBox = camera.myBox();
  bubble->setX(RM::randf(camBox.left(), camBox.right()));
  bubble->setY(camBox.bottom() + 64.0);
  
  addAlly(bubble);
  return bubble;
}

LakeCoin* LakeLevel::createCoin()
{
  LakeCoin* coin = new LakeCoin();
  Box camBox = camera.myBox();
  coin->setX(RM::randf(camBox.left(), camBox.right()));
  coin->setY(camBox.bottom() + 64.0);
  coin->value = SlopeCoin::randValueAll();
  
  addAlly(coin);
  return coin;
}

Coord1 LakeLevel::calcWinBonus()
{
  return 1000 + levelID * 200;
}

LakeUnlockable* LakeLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID)
{
  if (canUnlockTopping(toppingID) == false) return NULL;
  
  LakeUnlockable* floater = new LakeUnlockable(toppingID);
  floater->setXY(center);
  addAlly(floater);
  
  return floater;
}
 */

void LakeLevel::callback(ActionEvent* caller)
{
  /*
  if (caller == &harpoonTimer)
  {
    addEnemy(new Harpoon(Point2(player->getXY())));
    harpoonTimer.reset(4.0);
  }
  else if (caller == &bubbleTimer)
  {
    createBubble();
    
    if (Pizza::currGameVars[VAR_NUM_COINS] < 60 &&
        RM::randl() == true)
    {
      createCoin();
    }
  }
   */
}

// ==================================== LALevelEasy

void LALevelEasy::populateWorld()
{
  Coord1 sizeNums[] = {25, 10, 8, 7, 5};
  runnerPercent = 0.5;

  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
}

// ==================================== LALevelLobster

void LALevelLobster::populateWorld()
{
  Coord1 sizeNums[] = {20, 10, 8, 7, 5};
  runnerPercent = 0.5;

  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 15; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelLobster2

void LALevelLobster2::populateWorld()
{
  Coord1 sizeNums[] = {20, 1, 5, 7, 5};
  runnerPercent = 0.65;
  chasePercent = 0.5;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 25; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelPearl

void LALevelPearl::populateWorld()
{
  Coord1 sizeNums[] = {20, 10, 8, 7, 5};
  runnerPercent = 0.65;
  chasePercent = 0.5;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeLobster());
  for (Coord1 i = 0; i < 3; ++i) placeAndAddLGO(new LakePearl());
}

// ==================================== LALevelJellyfish

void LALevelJellyfish::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 7, 5};
  runnerPercent = 0.75;
  chasePercent = 0.65;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 15; ++i) placeAndAddLGO(new LakeJellyfish());
  for (Coord1 i = 0; i < 3; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 3; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelJellyfish2

void LALevelJellyfish2::populateWorld()
{
  for (Coord1 i = 0; i < 50; ++i)
  {
    placeAndAddLGO(new LakeJellyfish());
  }
}

// ==================================== LALevelBarrel

void LALevelBarrel::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 7, 5};
  runnerPercent = 0.75;
  chasePercent = 0.65;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i)
    {
      createNormalFish(sizeType);
    }
  }
  
  for (Coord1 i = 0; i < 8; ++i) placeAndAddLGO(new LakeBarrel());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelBarrel2

void LALevelBarrel2::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 7, 5};
  runnerPercent = 0.8;
  chasePercent = 0.7;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i)
    {
      createNormalFish(sizeType);
    }
  }
  
  for (Coord1 i = 0; i < 16; ++i) placeAndAddLGO(new LakeBarrel());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeJellyfish());
}

// ==================================== LALevelUrchin

void LALevelUrchin::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 7, 5};
  runnerPercent = 0.85;
  chasePercent = 0.7;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i)
    {
      createNormalFish(sizeType);
    }
  }
  
  for (Coord1 i = 0; i < 8; ++i) placeAndAddLGO(new LakeUrchin());
  for (Coord1 i = 0; i < 1; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeJellyfish());
}

// ==================================== LALevelSpineclam

void LALevelSpineclam::populateWorld()
{
  Coord1 sizeNums[] = {13, 15, 10, 7, 5};
  runnerPercent = 0.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i)
    {
      createNormalFish(sizeType);
    }
  }
  
  for (Coord1 i = 0; i < 8; ++i) placeAndAddLGO(new LakeUrchin());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSpineclam());
}

// ==================================== LALevelSpineclam2

void LALevelSpineclam2::populateWorld()
{
  Coord1 sizeNums[] = {12, 3, 3, 3, 12};
  runnerPercent = 0.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 14; ++i) placeAndAddLGO(new LakeUrchin());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSpineclam());
}

// ==================================== LALevelSwordfish

void LALevelSwordfish::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 7, 5};
  runnerPercent = 1.0;
  chasePercent = 0.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 8; ++i) placeAndAddLGO(new LakeSwordfish());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakeBarrel());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelStingray

void LALevelStingray::populateWorld()
{
  Coord1 sizeNums[] = {12, 12, 12, 10, 8};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 10; ++i) placeAndAddLGO(new LakeStingray());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSwordfish());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeJellyfish());
}

// ==================================== LALevelShark

void LALevelShark::populateWorld()
{
  Coord1 sizeNums[] = {12, 12, 12, 12, 12};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeStingray());
  for (Coord1 i = 0; i < 1; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 10; ++i) placeAndAddLGO(
      new LakeShark(RM::randi(LakeGO::SIZE_128, LakeGO::SIZE_256)));
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeUrchin());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeJellyfish());
}

// ==================================== LALevelShark2

void LALevelShark2::populateWorld()
{
  Coord1 sizeNums[] = {10, 12, 14, 16, 18};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeStingray());
  for (Coord1 i = 0; i < 1; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 10; ++i) placeAndAddLGO(
      new LakeShark(RM::randi(LakeGO::SIZE_128, LakeGO::SIZE_256)));
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSpineclam());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeLobster());
}

// ==================================== LALevelTest

void LALevelTest::populateWorld()
{
  Coord1 sizeNums[] = {10, 12, 14, 16, 18};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeStingray());
  for (Coord1 i = 0; i < 2; ++i) placeAndAddLGO(new LakePearl());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(
                                                 new LakeShark(RM::randi(LakeGO::SIZE_128, LakeGO::SIZE_256)));
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSpineclam());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeLobster());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeJellyfish());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeUrchin());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeSwordfish());
  for (Coord1 i = 0; i < 5; ++i) placeAndAddLGO(new LakeBarrel());
}

// ==================================== PlaneLevel ================================ //

PlaneLevel::PlaneLevel() :
  PizzaLevelInterface(),
  b2ContactListener(),

  objman(),
  player(NULL),
  bg(NULL),

  debrisList(),
  spawnQueue(this),

  wavePercents(NUM_ENEMY_TYPES, true),
  enemiesPerGroup(NUM_ENEMY_TYPES, true),
  groupsPerWave(1, 1),

  lastWave(9),
  currWave(-1)
{
  randGen.reset(0x24ed517f);
  musicFilename = "skyfighter.ima4";
  
  wavePercents.set_all(0.0);
  enemiesPerGroup.set_all(Coord2(1, 1));
  
  enemiesEnabled.add(PU_ENEMY_MOHAWK);
  enemiesEnabled.add(PU_ENEMY_CHICKEN);
  enemiesEnabled.add(GL_ENEMY_BOMBBAT);
  enemiesEnabled.add(GL_ENEMY_JUMPING_SPIDER);
  enemiesEnabled.add(GL_ENEMY_DONUT);
  enemiesEnabled.add(PU_ENEMY_FLAMESKULL);
  enemiesEnabled.add(GL_ENEMY_CHIP);
  enemiesEnabled.add(GL_ENEMY_WISP);
  enemiesEnabled.add(GL_ENEMY_CUPID);
}

PlaneLevel::~PlaneLevel()
{
  delete bg;
  delete player;
}

void PlaneLevel::preload()
{
  bg = new PlaneBG();
  bg->level = this;
}

void PlaneLevel::loadRes()
{
  loadImage(planePlayerImg, "temp_plane", HANDLE_C);
  loadTiles(planePlayerSet, "pizza_plane_spritesheet", 4, 4, Point2(64.0, 28.0));
  loadSpine(planePlayerSpine, "pizza_plane");
  loadImage(planeBulletImg, "plane_bullet", Point2(64.0, 8.0));
  
  loadEnabledEnemies();

  bg->load();
}

void PlaneLevel::unloadRes()
{
  
}

void PlaneLevel::initRes()
{
  bg->init();
  
  objman.init(NUM_PHASES);
  
  RMPhysics = new RivermanPhysics(Point2(0.0, 800.0), true);
  RMPhysics->SetContactListener(this);
  
  //  initScoreText();
  //  initHealth();
  
  player = new PlanePlayer();
  player->level = this;
  player->setXY(deviceSafeBox().left() + 128.0, deviceSafeBox().center().y);
  player->load();
  
  spawnQueue.wait(1.0);
}


void PlaneLevel::update()
{
  /*
  if (tutorial->shouldUpdateGame() == true) updateGame();
  
  updateTutorials();
   */
  updateGame();
}

void PlaneLevel::redraw()
{
  bg->drawBackground();
  
  // ========== camera on
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  
  debrisList.redraw();
  objman.drawObjects();
  objman.actions.redraw();
  player->redraw();
  RMPhysics->draw_debug();
  
  RMGraphics->pop_camera();
  // ========== camera off
  
  bg->drawForeground();
  
  RMGraphics->pop_camera();
  
  drawHUD();
//  scoreText.drawText();
//  tutorial->redraw();
  objman.frontActions.redraw();
}

void PlaneLevel::updateGame()
{
  // ========== modified time
  RM::push_time_mult(worldTimeMult);
  
  RMPhysics->update();
  
  debrisList.update();
  objman.updateObjects();
  objman.actions.update();
  objman.garbageCollector();
  
  updateControls();
  updateSeasonings();
  player->update();
  
  waveGenerator();
  
  shaker.update();
  updateCamera();
  
  if (levelState == LEVEL_PLAY)
  {
    // Pizza::runGameTime(RM::timePassed());
    updateStatus();
  }
  
  // updateHealthBar();
  bg->update();
  
  RM::pop_time_mult();
  // ==========
  
  objman.frontActions.update();
}

void PlaneLevel::drawHUD()
{
  String2 playerDmgStr;
  playerDmgStr.double_string(activeGameplayData->currPlayerHP, 4, 1);
  getFont(futura130Font)->draw(playerDmgStr, deviceSafeBox().norm_pos(HANDLE_TL), Point2(0.6, 0.6),
                               0.0, HANDLE_TL, BLUE_SOLID);
}

void PlaneLevel::loadEnemy(Coord1 enemyType)
{
  switch (enemyType)
  {
    default:
      PizzaLevelInterface::loadEnemy(enemyType);
      break;
  }
}

void PlaneLevel::waveGenerator()
{
  if (levelState != LEVEL_PLAY) return;
  
  spawnQueue.update();
  
  if (objman.phaseList[PHASE_ENEMY_STD].count == 0 &&
      spawnQueue.length() == 0)
  {
    if (currWave < lastWave)
    {
      currWave++;
      createWave();
    }
    else
    {
      winLevel();
    }
  }
}

void PlaneLevel::createRandomWave()
{
  Coord1 numGroups = RM::randi(groupsPerWave);
  
  for (Coord1 i = 0; i < numGroups; ++i)
  {
    Coord1 type = RM::random_index_weighted(wavePercents, randGen);
    Coord1 groupSize = randGen.randi(enemiesPerGroup[type]);
    queueGroup(type, groupSize);
  }
}

void PlaneLevel::enemyDefeated(PlaneGO* enemy)
{
  /*
  if (enemy->basePoints == 0) return;
  
  Pizza::currGameVars[VAR_DEFEATED]++;
  
  gotScore(enemy->basePoints);
   */
}

void PlaneLevel::queueGroup(Coord1 type, Coord1 groupSize)
{
  spawnQueue.enqueueX(new CreatePLGroupCMD(this, type, groupSize));
}

void PlaneLevel::smallShake()
{
  shaker.setMagnitude(3.0);
  shaker.timedShake(0.3);
}

void PlaneLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  // ios
  if (RMSystem->get_OS_family() == IOS_FAMILY)
  {
    if (controller->clickStatus(PRESSED) == true)
    {
      player->tryFire();
      player->tryMoveToPoint(controller->getXY().y);
    }
    else
    {
      player->tryMoveDirection(PhysicsLevelStd::tiltMagnitudeVert());
      
      if (controller->getConfirmButtonStatus(PRESSED) == true)
      {
        player->tryFire();
      }
    }
  }
  // mac
  else if (RMSystem->get_OS_family() == MAC_FAMILY)
  {
    if (controller->clickStatus(PRESSED) == true)
    {
      player->tryFire();
      player->tryMoveToPoint(controller->getXY().y);
    }
    else
    {
      player->tryMoveDirection(PhysicsLevelStd::tiltMagnitudeVert());
      
      if (controller->getConfirmButtonStatus(PRESSED) == true)
      {
        player->tryFire();
      }
    }
  }
  // apple tv
  else
  {
    player->tryMoveDirection(PhysicsLevelStd::tiltMagnitudeVert());
    
    if (controller->getConfirmButtonStatus(PRESSED) == true)
    {
      player->tryFire();
    }
  }
}

void PlaneLevel::updateStatus()
{
  // winning is handled in wave generator
  if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void PlaneLevel::updateCamera()
{
  shaker.update();
}

Coord1 PlaneLevel::calcWinBonus()
{
  return 0;
  // return 1000 + 125 * levelID;
}

/*
PlaneUnlockable* PlaneLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet)
{
  return NULL;
}
 */
  
void PlaneLevel::addAction(ActionEvent* ae)
{
  objman.addAction(ae);
}

void PlaneLevel::addDebris(ActionEvent* ae)
{
  debrisList.addX(ae);
}

void PlaneLevel::addFrontAction(ActionEvent* ae)
{
  objman.addFrontAction(ae);
}

void PlaneLevel::addPGO(PlaneGO* pgo)
{
  pgo->level = this;
  pgo->player = player;
  objman.addPGO(pgo, (PizzaPhase) pgo->phase);
  pgo->load();
}

void PlaneLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== CreatePLGroupCMD

CreatePLGroupCMD::CreatePLGroupCMD(PlaneLevel* setLevel, Coord1 setType,
                                   Coord1 setGroupSize) :
  Command(),

  level(setLevel),
  type(setType),
  groupSize(setGroupSize)
{
  
}

void CreatePLGroupCMD::execute()
{
  Point1 yBuffer = 64.0;
  Point2 firstXY = Point2(deviceScreenBox().right() + 48.0,
                          RM::randf(deviceSafeBox().top() + yBuffer,
                                    deviceSafeBox().bottom() - yBuffer));
  
  if (type == PU_ENEMY_MOHAWK ||
      type == PU_ENEMY_FLAMESKULL)
  {
    for (Coord1 i = 0; i < groupSize; ++i)
    {
      level->spawnQueue.enqueueX(new CreatePLEnemyCMD(level, type, firstXY + Point2(48.0 * i, 0.0)));
    }
    
    level->spawnQueue.wait(0.75);
  }
  else
  {
    for (Coord1 i = 0; i < groupSize; ++i)
    {
      level->spawnQueue.enqueueX(new CreatePLEnemyCMD(level, type, firstXY));
      level->spawnQueue.wait(0.75);
    }
  }
}

// ==================================== CreatePLEnemyCMD

CreatePLEnemyCMD::CreatePLEnemyCMD(PlaneLevel* setLevel, Coord1 setType,
                                   Point2 setXY) :
  level(setLevel),
  type(setType),
  xy(setXY)
{
  
}

void CreatePLEnemyCMD::execute()
{
  PlaneGO* pgo = NULL;
  Box screenBounds = deviceScreenBox();
  
  switch (type)
  {
    default:
    case GL_ENEMY_BOMBBAT:
    {
      pgo = new PlaneBombbat();
      
      Point1 batX = RM::lerp(screenBounds.xRange(), RM::randf(0.6, 0.8));
      Point1 batTopY = screenBounds.top() - 64.0;
      Point1 batBotY = screenBounds.bottom() + 64.0;

      pgo->setX(batX);
      pgo->setY(RM::randl() ? batTopY : batBotY);
      
      break;
    }

    case GL_ENEMY_CHIP:
    {
      pgo = new PlaneChip();

      Point1 chipTopY = screenBounds.top() + 64.0;
      Point1 chipBotY = screenBounds.bottom() - 64.0;
      
      pgo->setX(xy.x);
      pgo->setY(RM::randl() ? chipTopY : chipBotY);
      break;
    }
    case GL_ENEMY_CUPID:
      pgo = new PlaneCupid();
      pgo->setXY(xy);
      break;

    case GL_ENEMY_DONUT:
    {
      pgo = new PlaneDonut();
      
      Point1 donutX = RM::lerp(screenBounds.xRange(), RM::randf(0.6, 0.8));
      pgo->setX(donutX);
      pgo->setY(deviceScreenBox().top() - 80.0);
      break;
    }
    
    case PU_ENEMY_MOHAWK:
      pgo = new PlaneMohawk();
      pgo->setXY(xy);
      break;

    case PU_ENEMY_CHICKEN:
    {
      pgo = new PlaneChicken();
      pgo->setXY(xy);
      pgo->addX(64.0);
      break;
    }

    case PU_ENEMY_FLAMESKULL:
      pgo = new PlaneFlameskull();
      pgo->setXY(xy);
      break;

    case GL_ENEMY_JUMPING_SPIDER:
      pgo = new PlaneSpider();
      pgo->setX(xy.x);
      pgo->setY(deviceSafeBox().bottom());
      
      if (RM::randl() == true)
      // if (xy.y <= deviceSafeBox().center().y)
      {
        pgo->setY(deviceSafeBox().top());
      }
      
      break;

    case GL_ENEMY_WISP:
      pgo = new PlaneWisp();
      pgo->setXY(xy);
      break;

  }
  
  level->addPGO(pgo);
}

// ==================================== PLLevelMohawk

void PLLevelMohawk::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 1:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 2:
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 3:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 4:
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      break;
    case 5:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 6:
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 7:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      break;
    case 8:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      break;
    case 9:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      break;
  }
}

// ==================================== PLLevelChicken

void PLLevelChicken::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 1:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 2:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 3:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 4:
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      break;
    case 5:
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 6:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 7:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 8:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 9:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
  }
}

// ==================================== PLLevelBombbat

void PLLevelBombbat::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 2:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 3:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 4:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 5:
      queueGroup(GL_ENEMY_BOMBBAT, 5);
      break;
    case 6:
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 7:
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 8:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 9:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_BOMBBAT, 3);
      break;
  }
}

// ==================================== PLLevelSpider

void PLLevelSpider::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      break;
    case 2:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 3:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 4:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 4);
      break;
    case 5:
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      break;
    case 6:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 7:
      queueGroup(PU_ENEMY_CHICKEN, 4);
      break;
    case 8:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 9:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      break;
  }
}

// ==================================== PLLevelDonut

void PLLevelDonut::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_DONUT, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_DONUT, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_BOMBBAT, 3);
      break;
    case 3:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 4:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 5:
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 6:
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_DONUT, 2);
      break;
    case 7:
      queueGroup(GL_ENEMY_DONUT, 3);
      break;
    case 8:
      queueGroup(PU_ENEMY_MOHAWK, 7);
      break;
    case 9:
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_DONUT, 2);
      break;
  }
}

// ==================================== PLLevelFlameskull

void PLLevelFlameskull::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 1:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      break;
    case 3:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 4:
      queueGroup(PU_ENEMY_CHICKEN, 4);
      break;
    case 5:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      break;
    case 6:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      break;
    case 7:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      break;
    case 8:
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
  }
}

// ==================================== PLLevelSpider2

void PLLevelSpider2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 5);
      break;
    case 1:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      break;
    case 3:
      queueGroup(GL_ENEMY_DONUT, 3);
      break;
    case 4:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 6);
      break;
    case 5:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      break;
    case 6:
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      break;
    case 7:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 8:
      queueGroup(GL_ENEMY_BOMBBAT, 3);
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      break;
    case 9:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 10);
      break;
  }
}

// ==================================== PLLevelFlameskull2

void PLLevelFlameskull2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      break;
    case 1:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 2:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 3:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 4:
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
    case 5:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 6:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_CHICKEN, 5);
      break;
    case 7:
      queueGroup(PU_ENEMY_FLAMESKULL, 8);
      queueGroup(GL_ENEMY_DONUT, 4);
      break;
    case 8:
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
  }
}

// ==================================== PLLevelChip

void PLLevelChip::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_CHIP, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_CHIP, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_CHIP, 2);
      break;
    case 3:
      queueGroup(GL_ENEMY_CHIP, 4);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      break;
    case 4:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 5:
      queueGroup(GL_ENEMY_CHIP, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(GL_ENEMY_CHIP, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(GL_ENEMY_CHIP, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 6:
      queueGroup(GL_ENEMY_CHIP, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 7:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_CHIP, 2);
      break;
    case 8:
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 9:
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
  }
}


// ==================================== PLLevelDonut2

void PLLevelDonut2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_DONUT, 5);
      break;
    case 1:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      break;
    case 2:
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      queueGroup(GL_ENEMY_DONUT, 2);
      queueGroup(PU_ENEMY_MOHAWK, 2);
      break;
    case 3:
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 4);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      break;
    case 4:
      queueGroup(GL_ENEMY_DONUT, 6);
      break;
    case 5:
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(GL_ENEMY_DONUT, 3);
      break;
    case 6:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_MOHAWK, 5);
      break;
    case 7:
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      queueGroup(GL_ENEMY_DONUT, 1);
      queueGroup(PU_ENEMY_MOHAWK, 1);
      break;
    case 8:
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      break;
    case 9:
      queueGroup(GL_ENEMY_DONUT, 8);
      break;
  }
}

// ==================================== PLLevelWisp

void PLLevelWisp::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_WISP, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_WISP, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_WISP, 1);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 3:
      queueGroup(GL_ENEMY_WISP, 3);
      break;
    case 4:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 5:
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 1);
      break;
    case 6:
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 7:
      queueGroup(GL_ENEMY_BOMBBAT, 4);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      break;
    case 8:
      queueGroup(GL_ENEMY_WISP, 4);
      break;
    case 9:
      queueGroup(GL_ENEMY_WISP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 3);
      break;
  }
}

// ==================================== PLLevelChip2

void PLLevelChip2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_CHIP, 4);
      break;
    case 1:
      queueGroup(GL_ENEMY_CHIP, 6);
      break;
    case 2:
      queueGroup(GL_ENEMY_WISP, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(GL_ENEMY_CHIP, 4);
      break;
    case 3:
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(GL_ENEMY_CHIP, 2);
      break;
    case 4:
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_WISP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 5:
      queueGroup(GL_ENEMY_CHIP, 8);
      break;
    case 6:
      queueGroup(GL_ENEMY_WISP, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 4);
      queueGroup(GL_ENEMY_WISP, 1);
      break;
    case 7:
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      break;
    case 8:
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_CHIP, 3);
      break;
  }
}

// ==================================== PLLevelCupid

void PLLevelCupid::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_CUPID, 1);
      break;
    case 1:
      queueGroup(GL_ENEMY_CUPID, 2);
      break;
    case 2:
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(GL_ENEMY_CUPID, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(PU_ENEMY_CHICKEN, 2);
      break;
    case 3:
      queueGroup(PU_ENEMY_CHICKEN, 2);
      queueGroup(GL_ENEMY_DONUT, 4);
      break;
    case 4:
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
    case 5:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_CUPID, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_CUPID, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 6:
      queueGroup(PU_ENEMY_CHICKEN, 1);
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(PU_ENEMY_CHICKEN, 1);
      break;
    case 7:
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_DONUT, 2);
      break;
    case 8:
      queueGroup(GL_ENEMY_CUPID, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(PU_ENEMY_CHICKEN, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_DONUT, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      break;
  }
}

// ==================================== PLLevelWisp2

void PLLevelWisp2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_WISP, 4);
      break;
    case 1:
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(PU_ENEMY_MOHAWK, 3);
      queueGroup(GL_ENEMY_CHIP, 3);
      break;
    case 2:
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(GL_ENEMY_WISP, 3);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
    case 3:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(GL_ENEMY_CHIP, 6);
      break;
    case 4:
      queueGroup(GL_ENEMY_WISP, 1);
      queueGroup(GL_ENEMY_CHIP, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      queueGroup(GL_ENEMY_WISP, 1);
      queueGroup(GL_ENEMY_CHIP, 1);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 1);
      break;
    case 5:
      queueGroup(GL_ENEMY_WISP, 6);
      break;
    case 6:
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CHIP, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      break;
    case 7:
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_MOHAWK, 4);
      break;
    case 8:
      queueGroup(GL_ENEMY_CHIP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_JUMPING_SPIDER, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CHIP, 2);
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      break;
  }
}

// ==================================== PLLevelCupid2

void PLLevelCupid2::createWave()
{
  switch (currWave)
  {
    default:
    case 0:
      queueGroup(GL_ENEMY_CUPID, 4);
      break;
    case 1:
      queueGroup(GL_ENEMY_WISP, 4);
      break;
    case 2:
      queueGroup(GL_ENEMY_BOMBBAT, 10);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(PU_ENEMY_FLAMESKULL, 1);
      queueGroup(GL_ENEMY_CUPID, 2);
      break;
    case 3:
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(GL_ENEMY_BOMBBAT, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      queueGroup(PU_ENEMY_FLAMESKULL, 3);
      break;
    case 4:
      break;
    case 5:
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(GL_ENEMY_WISP, 3);
      queueGroup(GL_ENEMY_CUPID, 3);
      break;
    case 6:
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 5);
      break;
    case 7:
      queueGroup(GL_ENEMY_CUPID, 3);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 20);
      break;
    case 8:
      queueGroup(GL_ENEMY_CUPID, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_WISP, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 5);
      queueGroup(PU_ENEMY_FLAMESKULL, 2);
      break;
    case 9:
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CUPID, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CUPID, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      queueGroup(PU_ENEMY_FLAMESKULL, 4);
      queueGroup(GL_ENEMY_CUPID, 2);
      queueGroup(GL_ENEMY_WISP, 2);
      queueGroup(GL_ENEMY_BOMBBAT, 2);
      break;
  }
}


// ======================= CreatePGOParticlesAction ========================= //

CreateRenderedParticlesAction::CreateRenderedParticlesAction(RenderableParticleBreakerInf* setPgo) :
  ActionEvent(),

  pgo(setPgo),
  img(NULL),

  captured(false),
  localAABB()
{
  
}

CreateRenderedParticlesAction::~CreateRenderedParticlesAction()
{
  // This will only delete the image if for some reason it was created
  // but not yet actually used.  The instant it's used, the particle effect takes ownership.
  delete img;
  img = NULL;
}

void CreateRenderedParticlesAction::updateMe()
{
  // cout << "update break" << endl;

  if (img != NULL && captured == true)
  {
    // static_cast<PlaneGO*>(pgo)->level->worldTimeMult = 0.01;
    // create particles
    // pre-rendered particles don't need the transform data of the parent VisRectangle
    
//    Bitmap* bmp = RMGraphics->render_tgt_to_bitmap(offscreenBuffer,
//                                                   Box(0.0, 0.0, localAABB.wh.x, localAABB.wh.y));
//
//    img = new Image(*bmp);
//    img->handle = -localAABB.xy;
//    delete bmp;

    // because all this data is baked in to the pre-rendered image
    VisRectangular breaker;
    breaker.setImage(img);
    breaker.setXY(pgo->get_visrect()->getXY());
    breaker.setRotation(pgo->get_visrect()->getRotation());

    BoneBreakEffect* breakEffect = new BoneBreakEffect(breaker, 3, 3);
    breakEffect->ownsImage = true;
    img = NULL;  // avoids deleting image, particle effect now owns image
    
    pgo->particles_ready(breakEffect);
    
    // pgo->level->worldTimeMult = 0.1;
    // cout << "created particles" << endl;

    done();
  }
}

void CreateRenderedParticlesAction::redrawMe()
{
  // cout << "draw break" << endl;
  static Coord1 i = 0;

//  if (i <= 1)
//  {
//    i++;
//    return;
//  }
  /*
  else if (i == 5)
  {
   
    Bitmap* bmp = RMGraphics->render_tgt_to_bitmap(offscreenBuffer,
                                                   Box(0.0, 0.0, localAABB.wh.x, localAABB.wh.y));
    
    img = new Image(*bmp);
    img->handle = -localAABB.xy;
    delete bmp;

    i++;
  }
  else
  {
    i++;
  }
   */

  if (img == NULL && captured == false)
  {
    ArrayList<CameraStd> currCameraStack = RMGraphics->cameraStack;  // copy entire camera stack
    RMGraphics->clear_cameras();
    RMGraphics->set_render_target(offscreenBuffer);
    RMGraphics->clear(WHITE_TRANSPARENT);
   
    Point2 realXY = pgo->get_visrect()->getXY();
    Point1 realRotation = pgo->get_visrect()->getRotation();
    Logical realHFlip = pgo->get_visrect()->getHFlip();
    Logical realVFlip = pgo->get_visrect()->getVFlip();

    pgo->get_visrect()->setRotation(0.0);

    pgo->predraw_call();
    
    localAABB = pgo->get_local_draw_AABB();   ////////////
    pgo->get_visrect()->setXY(-localAABB.xy);
//    cout << "localAABB " << localAABB << endl;
//    cout << "setting xy to " << pgo->get_visrect()->getXY() << endl;
    
    pgo->render_for_particles();
    captured = true;

    pgo->get_visrect()->setXY(realXY);
    pgo->get_visrect()->setRotation(realRotation);
   
    pgo->postdraw_call();

    /*
    Bitmap* bmp = RMGraphics->render_tgt_to_bitmap(offscreenBuffer,
        Box(0.0, 0.0, localAABB.wh.x, localAABB.wh.y));

    img = new Image(*bmp);
    img->handle = -localAABB.xy;
    delete bmp;
     */
    Texture* texture = RMGraphics->render_tgt_to_texture(offscreenBuffer,
        Box(0.0, 0.0, localAABB.wh.x, localAABB.wh.y));
    img = new Image(texture);
    img->handle = -localAABB.xy;

    RMGraphics->set_render_target(RMGraphics->screenTgt);
    RMGraphics->cameraStack = currCameraStack;  // reset entire camera stack to what it was
    RMGraphics->cameras_changed();
  }
}

// ==================================== PizzeriaLevel ================================ //

PizzeriaLevel::PizzeriaLevel() :
  PhysicsLevelStd(),

  parlorBG(NULL),
  chef(NULL),

  wrappedChefMsg(),
  chefMsgSize(Point2(512.0, 82.0)),
  chefMsgScale(0.864),

  buttons(ITEMS_PER_SHOP),

  ingredientPtrs(ITEMS_PER_SHOP)
{
  parlorBG = new PizzeriaBG();
  background = parlorBG;
  backgroundType = PIZZERIA_BG;
  //  background = new TestPanoBackground();
  background->level = this;
  
  objman.bgTerrainList.ensure_capacity(256);
  
  musicFilename = "puppy.ima4";
}

void PizzeriaLevel::preload()
{
  LevelData& levelDef = levelList[activeGameplayData->currLevelIndex];
  
  for (Coord1 i = 0; i < ITEMS_PER_SHOP; ++i)
  {
    ingredientPtrs.add(&ingredientList[levelDef.ingredientDefIndices[i]]);
  }
}

// overrides
void PizzeriaLevel::loadRes()
{
  loadFont(museoFont, "museo", EFIGS_CHARS, 0.0);
  loadFont(shackletonFont, "shackleton", EFIGS_CHARS, -7.0);

  loadImage(shopPizzaIconImg, "shop_pizzaicon");
  loadTiles(shopPricePaperSet, "shop_pricepapers", 1, 3, Point2(78.0, 39.0));
  loadSpine(shopButtonSpine, "shop_button");
  // loadTiles(shopButtonImgs, "big_red_button", 3, 1, Point2(80.0, 64.0));
  loadImage(shopCoinImg, "coin");
  loadImage(shopGemImg, "gem");
  loadImage(shopFloorImg, "shop_floor", Point2(0.0, 64.0));

  loadImage(equipPeelImg, "equip_peel");
  loadImage(equipMenuBaseImg, "equip_menubase");
  loadTiles(equipMenuTabsSet, "equip_menutabs", 7, 1);
  loadImage(equipItemImg, "equip_item");
  loadImage(equipMoreArrowImg, "equip_more_arrow", HANDLE_C);
  loadTiles(equipSelectSquaresSet, "equip_select_squares", 3, 1);
  loadTiles(equipSelectNumbersSet, "equip_select_numbers", 3, 1);
  loadImage(equipDoneCheckImg, "done_check");

  loadEnabledEnemies();
  
  background->load();
}

void PizzeriaLevel::unloadRes()
{
  
}

void PizzeriaLevel::initSublevel()
{
  endpoint = Point2(0.0, 384.0);
  
  // updateGOBs = false;
  // drawGOBs = false;
  
  pizzeriaChunk();
  
  // worldBox = Box(0.0, -RM_WH.y * 2.0, endpoint.x, RM_WH.y * 3.0);
  // invisibleWallsStd(worldBox);
  
  Point1 masterSizeMult = 0.33;
  player->wh *= masterSizeMult;
  player->scale *= masterSizeMult;
  player->fullSpokeLength *= masterSizeMult;
  player->rollForce *= masterSizeMult;
  player->rollTorque *= masterSizeMult;

  player->load();
  startStanding(Point2(144.0, 416.0));
  
  Box wallBox = Box::from_corners(Point2(0.0, 0.0), Point2(960.0, 540.0));
  // invisibleWallsStd(wallBox);
  
  chef = new ShopChef();
  chef->setXY(817.0, 321.0);
  addPGO(chef);
//  rmboxCamera.xy.x = player->getX();
//  rmboxCamera.handle = deviceScreenBox().norm_pos(HANDLE_BC);
  
  wrappedChefMsg = getStr(2);
  Str::reline_to_box(wrappedChefMsg, getFont(museoFont), chefMsgScale, chefMsgSize);
  
  Point1 buttonXs[] = {288.0, 482.0, 673.0};

  for (Coord1 i = 0; i < ITEMS_PER_SHOP; ++i)
  {
    PizzeriaBuyButton* newButton = new PizzeriaBuyButton();
    buttons.add(newButton);
    newButton->buttonIndex = i;
    newButton->setXY(buttonXs[i], 384.0);
    
    addPGO(newButton);
  }
  
  syncAllButtons();
  RMSceneMan->pushGUI(new ShopGUI(this, true));
}

void PizzeriaLevel::updateButtonState(Coord1 index)
{
  PizzeriaBuyButton* currButton = buttons[index];
  
  if (ingredientPtrs[index]->playerBought() == true)
  {
    currButton->setState(PizzeriaBuyButton::DOWN_STATE);
  }
  else if (ingredientPtrs[index]->playerUnlocked() == false ||
           ingredientPtrs[index]->playerCanAfford() == false)
  {
    currButton->setState(PizzeriaBuyButton::DISABLE_STATE);
  }
  else
  {
    currButton->setState(PizzeriaBuyButton::UP_STATE);
  }
}

void PizzeriaLevel::updateCamera(Logical snapIntoPlace)
{
  // updateCameraFree(snapIntoPlace);
}

void PizzeriaLevel::updateLevelAI()
{
  // check player leaving
  if (levelState == LEVEL_PLAY)
  {
    if (player->collisionBox().right() <= deviceScreenBox().left() ||
        player->collisionBox().left() >= deviceScreenBox().right())
    {
      RMSceneMan->sceneTransition(new MapScene());
      levelState = LEVEL_WIN;
    }
  }
  
}

void PizzeriaLevel::zoomInToEquip(Point1 transTime)
{
  updateGOBs = false;
  
  ActionQueue* transScript = new ActionQueue();
  transScript->enqueueX(new LinearFn(&rmboxCamera.xy.y, -RM_WH.y, transTime));
  transScript->enqueueX(new SetValueCommand<Logical>(&drawGOBs, false));
  
  addFrontAction(transScript);
  addFrontAction(new LinearFn(&parlorBG->fireZoom, 2.0, transTime * 2.0));
}

void PizzeriaLevel::zoomOutToBuy(Point1 transTime)
{
  ActionQueue* transScript = new ActionQueue();
  transScript->wait(transTime);
  transScript->enqueueX(new SetValueCommand<Logical>(&drawGOBs, true));
  transScript->enqueueX(new LinearFn(&rmboxCamera.xy.y, 0.0, transTime));
  transScript->enqueueX(new SetValueCommand<Logical>(&updateGOBs, true));
  
  addFrontAction(transScript);
  addFrontAction(new LinearFn(&parlorBG->fireZoom, 1.0, transTime * 2.0));
}


void PizzeriaLevel::drawHUD()
{
  // ================ push top hud cam
  CameraStd topHUDCam = rmboxCamera;
  topHUDCam.xy.y *= -1.0;
  
  Point2 safeTR = deviceSafeBox().norm_pos(HANDLE_TR);
  
  // player coins
  RMGraphics->push_camera(topHUDCam);
  getImg(shopCoinImg)->draw(deviceSafeBox().norm_pos(HANDLE_TR) + Point2(-302.0, 4.0));
  getFont(shackletonFont)->draw_in_box(String2("$", '$', activePlayer->getIntItem(COINS_KEY)),
      safeTR + Point2(-248.0, 29.0), 1.0, 127.0, 0.0, HANDLE_LC, WHITE_SOLID);

  // player gems
  //  getImg(shopGemImg)->draw(safeTR + Point2(-122.0, -2.0));
  //  getFont(shackletonFont)->draw_in_box(String2("$", '$', activePlayer->getIntItem(GEMS_KEY)),
  //      safeTR + Point2(-72.0, 29.0), 1.0, 127.0, 0.0, HANDLE_LC, WHITE_SOLID);
  
  RMGraphics->pop_camera();
  // ================ top hud cam off

  // ================ push gameplay cam
  RMGraphics->push_camera(rmboxCamera);
  
  // chef message background
  Box(223.0, 105.0, 512.0, 82.0).draw_solid(ColorP4(0.0, 0.0, 0.0, 0.65));
  
  getFont(museoFont)->draw_in_box(wrappedChefMsg, Point2(480.0, 146.0), chefMsgScale, chefMsgSize,
                                  HANDLE_C, WHITE_SOLID);
  
  Point1 paperXs[] = {288.0, 482.0, 673.0};
  
  for (Coord1 i = 0; i < ITEMS_PER_SHOP; ++i)
  {
    Box safeBox = deviceSafeBox();
    Point1 spacing = 192.0;
    Point2 paperXY = Point2(paperXs[i], 433.0);
    
    getImgSet(shopPricePaperSet)[i]->draw(paperXY);
    
    // topping name
    getFont(museoFont)->draw_in_box(ingredientPtrs[i]->localizedName, paperXY + Point2(0.0, -22.0),
                                    0.55, 137.0, HANDLE_C, RM::color255(0x146e05ff));

    // price
    String2 priceStr(8);
    priceStr = "^$";
    
    if (ingredientPtrs[i]->price.x == CURRENCY_KEYS)
    {
      priceStr.replace('^', '~');
    }
    
    priceStr.int_replace('$', ingredientPtrs[i]->price.y);
    
    getFont(museoFont)->draw_in_box(priceStr, paperXY + Point2(0.0, 10.0),
                                    1.0, 137.0, HANDLE_C, RM::color255(0x972207ff));

    
  }
  
  RMGraphics->pop_camera();
  // ================ pop gameplay cam
  
}

void PizzeriaLevel::shopButtonPressed(Coord1 buttonIndex)
{
  chef->startClap();
  
  activePlayer->getCurrency(ingredientPtrs[buttonIndex]->price.x) -= ingredientPtrs[buttonIndex]->price.y;
  activePlayer->getIngredientBool(ingredientPtrs[buttonIndex]->index, I_OWNED_KEY) = true;
  activePlayer->getIngLocation(ingredientPtrs[buttonIndex]->location, 0) = ingredientPtrs[buttonIndex]->index;
  Pizza::savePlayerData();
  
  syncAllButtons();
}

void PizzeriaLevel::syncAllButtons()
{
  for (Coord1 i = 0; i < buttons.count; ++i)
  {
    updateButtonState(i);
  }
}

void PizzeriaLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== PizzaTutorial ==================================== //

LevelOverCommand::LevelOverCommand(PizzaLevelInterface* setLevel) :
  Command(),

  level(setLevel)
{
  
}

void LevelOverCommand::execute()
{
  if (level->levelState == LEVEL_WIN)
  {
    level->levelState = LEVEL_PLAY;
    level->winLevel();
  }
  else
  {
    level->levelState = LEVEL_PLAY;
    level->loseLevel();
  }
}

// ==================================== PizzaTutorial ==================================== //

PizzaTutorial::PizzaTutorial(PizzaLevelInterface* setLevel) :
  RivermanGUI(),

  level(setLevel),
  nextTutorial(NULL),
  updateGame(false),

  boxSize(16.0, 16.0),
  boxColor(1.0, 1.0, 1.0, 1.0),
  transProgress(0.0)
{
  window.setXY(deviceScreenBox().center());
  window.setFont(getFont(futura130Font));
  window.setScale(0.7, 0.7);
  window.textHandle.set(0.5, 0.5);
}

PizzaTutorial::~PizzaTutorial()
{
  delete nextTutorial;
}

void PizzaTutorial::update()
{
  script.update();
  actions.update();
  updateWidgets();
  
  window.setAlpha(transProgress);
  
  if (state == RM::ACTIVE)
  {
    /*
    if (Pizza::controller->clickedOutsidePause() == true)
    {
      click(NULL);
    }
     */
  }
}

void PizzaTutorial::redraw()
{
  drawActions();
  
  if (transProgress > 0.01)
  {
    Point2 boxXY = Box::from_center(deviceScreenBox().center(), boxSize).xy;
    ColorP4 finalColor = boxColor;
    finalColor.w = transProgress;
    
    // imgCache[losePx]->draw(boxXY, boxSize, 0.0, finalColor);
  }
  
  window.drawText();
}

void PizzaTutorial::setWindowText(const String2& text)
{
  window.setText(text);
  window.scaleToTextSize(boxSize - 16.0);
}

void PizzaTutorial::transIn()
{
  state = RM::TRANS_IN;
  script.enqueueX(new LinearFn(&transProgress, 1.0, 0.20));
}

void PizzaTutorial::transOut()
{
  state = RM::TRANS_OUT;
  script.enqueueX(new LinearFn(&transProgress, 0.0, 0.20));
}

void PizzaTutorial::resolveGUI()
{
  state = RM::REMOVE;
  startResolve();
}

void PizzaTutorial::click(Widget* caller)
{
  transOut();
}

// ==================================== BGSpineObject ================================ //

BGSpineObject::BGSpineObject() :
  VisRectangular(),
  speedMult(1.0),

  layerSuggestion(BG_LAYER_FAR),

  mySpineAnim()
{
  
}

void BGSpineObject::initSpineAnim(Coord1 spineResIndex, const String1& startTrackName, Logical looping)
{
  initSpineAnim(mySpineAnim, spineResIndex, startTrackName, looping);
}

void BGSpineObject::initSpineAnim(SpineAnimator& anim, Coord1 spineResIndex,
                               const String1& startTrackName, Logical looping)
{
  anim.init(getSpineSkeleton(spineResIndex), getSpineAnim(spineResIndex));
  spSkeleton_setToSetupPose(anim.spineSkeleton);
  spSkeleton_updateWorldTransform(anim.spineSkeleton);
  
  if (startTrackName.length >= 1)
  {
    Coord1 track = 0;
    Point1 delay = 0.0;
    spAnimationState_addAnimationByName(anim.spineAnimState,
                                        track, startTrackName.as_cstr(), looping ? 1 : 0, delay);
  }
}

void BGSpineObject::updateSpineAnim(SpineAnimator& anim)
{
  anim.updateSpineSkeleton(speedMult * RM::timePassed());
}

void BGSpineObject::renderSpineAnim(SpineAnimator& anim)
{
  anim.syncFromVisRect(*this);
  anim.renderSpineSkeleton();
}

void BGSpineObject::updateSpineAnim()
{
  updateSpineAnim(mySpineAnim);
}

void BGSpineObject::renderSpineAnim()
{
  renderSpineAnim(mySpineAnim);
}

Logical BGSpineObject::usingSpineAnim()
{
  return mySpineAnim.spineAnimState != NULL;
}

Logical BGSpineObject::onRMScreen()
{
  // return level->rmboxCamera.myBox().collision(visibleAABB());
  return true;
}

// ==================================== PizzaBackground ================================ //

PizzaBackground::PizzaBackground() :
  StdBackground(),

  level(NULL),

  spineObjList(16),

  totalWidth(0.0),
  trackBounds(0.0, 0.0),
  foregroundDisabled(false),

  fgObjXVals(),
  fgObjImgIDs()
{
  
}

PizzaBackground::~PizzaBackground()
{
  free_clear(spineObjList);
}

CameraStd PizzaBackground::applyHParallaxCam(Point1 scaleFactor)
{
  CameraStd bgCam = level->rmboxCamera;
  bgCam.setHParallax(level->rmboxCamera, scaleFactor);
  bgCam.xy.y = 0.0;
  bgCam.apply();
  return bgCam;
}


void PizzaBackground::drawLandscapeTilesStd(Image* img, Point1 drawY, Point1 scaleFactor)
{
  drawLandscapeTilesAndSpineObjsStd(img, drawY, scaleFactor, -1);
}

void PizzaBackground::drawLandscapeTilesAndSpineObjsStd(Image* img, Point1 drawY,
                                                        Point1 scaleFactor, Coord1 objLayer,
                                                        Logical objsAreFront)
{
  CameraStd bgCam = applyHParallaxCam(scaleFactor);
  
  Point1 invScale = 1.0 / scaleFactor;
  Point1 regularSize = img->natural_size().x;
  
  Box layerBox = bgCam.myBox();
  // cout << "level left " << screenBox.left() << " bg left " << layerBox.left() << endl;
  
  // the -64 and +64 are for shake padding
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left() - 64.0, regularSize);
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right() + 64.0, regularSize);
  
  // cout << "start layerBox " << layerBox << endl;

  // draw spine objects now if they are in back
  if (objLayer != -1 &&
      objsAreFront == false)
  {
    for (Coord1 i = startInterval; i <= endInterval; ++i)
    {
      Point1 xOff = i * regularSize;
      drawSpineObjLayer(objLayer, xOff);
    }
  }
  
  // draw tiling bg 
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    Point1 xOff = i * regularSize;
    Point2 drawXY(xOff, drawY);

    img->draw(drawXY);
    // cout << "drawing layer size " << regularSize << " at xoff " << xOff << endl;
  }
  
  // draw spine objects now if they are in front
  if (objLayer != -1 &&
      objsAreFront == true)
  {
    for (Coord1 i = startInterval; i <= endInterval; ++i)
    {
      Point1 xOff = i * regularSize;
      drawSpineObjLayer(objLayer, xOff);
    }
  }
}

void PizzaBackground::drawLandscapeTilesStd(const ImageGrid& grid, Point1 drawY, Point1 scaleFactor)
{
  CameraStd bgCam = applyHParallaxCam(scaleFactor);
  
  Point1 invScale = 1.0 / scaleFactor;
  Point1 regularSize = grid.totalWidth();
  
  Box layerBox = bgCam.myBox();
  // cout << "level left " << screenBox.left() << " bg left " << layerBox.left() << endl;
  
  // the -64 and +64 are for shake padding
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left() - 64.0, regularSize);
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right() + 64.0, regularSize);
  
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    Point1 xOff = i * regularSize;
    Point2 drawXY(xOff, drawY);
    grid.draw(drawXY);
  }
}

void PizzaBackground::drawLandscapeTilesFar(Image* img, Point1 drawY, Point1 scrollFactor)
{
  CameraStd bgCam = level->rmboxCamera;
  
  Box screenBox = bgCam.myBox();
  Point1 invScale = 1.0 / scrollFactor;
  Point1 regularSize = img->natural_size().x;
  
  bgCam.setXY(level->rmboxCamera.getX() * scrollFactor, 0.0);
  bgCam.apply();
  Box layerBox = bgCam.myBox();
  // cout << "level left " << screenBox.left() << " bg left " << layerBox.left() << endl;
  
  // the -64 and +64 are for shake padding
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left() - 64.0, regularSize);
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right() + 64.0, regularSize);
  
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    Point1 xOff = i * regularSize;
    Point2 drawXY(xOff, drawY);
    img->draw(drawXY);
  }
}

// adds to the list automatically
BGSpineObject* PizzaBackground::createSpineObj(Point2 xy, Coord1 spineIndex, const Char* startAnim,
                                               Coord1 layerHint, Point1 speed)
{
  spineObjList.add(new BGSpineObject());
  spineObjList.last()->setXY(xy);
  spineObjList.last()->speedMult = speed;
  spineObjList.last()->layerSuggestion = layerHint;
  spineObjList.last()->initSpineAnim(spineIndex, startAnim);
  
  return spineObjList.last();
}

void PizzaBackground::updateSpineObjects()
{
  for (Coord1 i = 0; i < spineObjList.count; ++i)
  {
    spineObjList[i]->updateSpineAnim();
  }
}

void PizzaBackground::drawSpineObjLayer(Coord1 layer)
{
  drawSpineObjLayer(layer, 0.0);
}

void PizzaBackground::drawSpineObjLayer(Coord1 layer, Point1 xOffset)
{
  for (Coord1 i = 0; i < spineObjList.count; ++i)
  {
    if (spineObjList[i]->layerSuggestion == layer)
    {
      Point1 startX = spineObjList[i]->getX();
      
      spineObjList[i]->addX(xOffset);
      spineObjList[i]->renderSpineAnim();
      spineObjList[i]->setX(startX);
    }
  }
}

void PizzaBackground::drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY)
{
  drawLandscapeTilesSlopes(img, scaleFactor, drawY, level->rmboxCamera);
}

void PizzaBackground::drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY,
                                               const CameraStd& levelCam)
{
  CameraStd bgCam = levelCam;
  Point1 invScale = 1.0 / scaleFactor;
  Point1 regularSize = img->natural_size().x;
  
  bgCam.setXY(bgCam.getX() * scaleFactor, 0.0);
  bgCam.zoom.set(1.0, 1.0);
  bgCam.apply();
  
  Box layerBox = bgCam.myBox();
  layerBox.wh = SCREEN_WH;
  
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left(), regularSize);
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right(), regularSize);
  
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    Point1 xOff = i * regularSize;
    Point2 drawXY(xOff, drawY);
    img->draw(drawXY);
  }
}

void PizzaBackground::drawTilingXYBG(Coord1 imgIndex, Point1 zoomRatio)
{
  Image* img = getImg(imgIndex);
  Point2 regularSize = img->natural_size();
  
  CameraStd parallaxCam = level->rmboxCamera;
  parallaxCam.setXYParallax(level->rmboxCamera, zoomRatio);
  Box cameraBox = parallaxCam.myBox();
  
  // the +/- 4 is for shaking
  Coord2 startIntervals = RM::segment_distances(Point2(0.0, 0.0),
                                                cameraBox.xy - 4.0, regularSize) - 1;
  Coord2 endIntervals = RM::segment_distances(Point2(0.0, 0.0),
                                              cameraBox.norm_pos(HANDLE_BR) + 4.0,
                                              regularSize) + 1;
  
  parallaxCam.apply();
  
  for (Coord1 row = startIntervals.y; row <= endIntervals.y; ++row)
  {
    for (Coord1 col = startIntervals.x; col <= endIntervals.x; ++col)
    {
      Point2 offset = regularSize * Point2(col, row);
      img->draw(offset);
    }
  }
  
  // cout << "drawing from " << startIntervals << " to " << endIntervals << endl;
}


void PizzaBackground::fgObjCreator(Point2 xRange, Coord2 imgIndexRange)
{
  Point1 nextX = xRange.x;
  
  while (nextX <= xRange.y)
  {
    fgObjXVals.add(nextX);
    fgObjImgIDs.add(RM::randi(imgIndexRange));
    
    nextX += RM_WH.x * RM::randf(1.0, 2.0);
  }
}

void PizzaBackground::drawFGObjects(Point1 zoomFactor)
{
  if (foregroundDisabled == true)
  {
    return;
  }
  
  CameraStd bgCam = level->rmboxCamera;
  
  bgCam.applyHParallax(level->rmboxCamera, zoomFactor);
  
  for (Coord1 i = 0; i < fgObjXVals.count; ++i)
  {
    getImg(fgObjImgIDs[i])->draw(fgObjXVals[i], deviceScreenBox().bottom());
    // cout << "drawing " << objectImgIDs[i] << " at " << objectXVals[i] << endl;
  }
  
  level->rmboxCamera.apply();
}

// ==================================== UFOBackground ================================ //

UFOBackground::UFOBackground() :
  PizzaBackground()
{
  
}

void UFOBackground::load()
{
  ResourceManager::loadImage(bgLayer0, "ufo_bg_0", HANDLE_BL);
  ResourceManager::loadImage(bgLayer1, "ufo_bg_1", HANDLE_BL);
  ResourceManager::loadImage(bgLayer2, "ufo_bg_2", HANDLE_BL);
  ResourceManager::loadImage(bgLayer3, "ufo_bg_3", HANDLE_BL);
  
  // terrain
  loadImage(redrockCenterImg, "redrock_center");
  loadImage(redrockBorderImg, "redrock_border");
  loadImage(redrockTerrainBordersImg, "redrock_terrain_borders");
}

void UFOBackground::init()
{
  level->backgroundType = REDROCK_BG;
}

void UFOBackground::update()
{
  
}

void UFOBackground::drawBackground()
{
  drawLandscapeTilesStd(getImg(bgLayer0), 336.0, 0.0001);
  drawLandscapeTilesStd(getImg(bgLayer1), 376.0, 0.25);
  drawLandscapeTilesStd(getImg(bgLayer2), 448.0, 0.5);
  drawLandscapeTilesStd(getImg(bgLayer3), 540.0, 1.0);
}

void UFOBackground::drawForeground()
{
  
}

// ==================================== TestPanoBackground ================================ //

TestPanoBackground::TestPanoBackground() :
  PizzaBackground(),

  mainLayerGrid(5, 1),
  objectLayerGrid(5, 1)
{
  
}

void TestPanoBackground::load()
{
  ResourceManager::loadImage(bgLayer0, "graveyard_sky");
  
  ResourceManager::loadImage(bgLayer1, "graveyard_main_a");
  ResourceManager::loadImage(bgLayer2, "graveyard_main_b");
  ResourceManager::loadImage(bgLayer3, "graveyard_main_c");
  ResourceManager::loadImage(bgLayer4, "graveyard_main_d");
  ResourceManager::loadImage(bgLayer5, "graveyard_main_e");

  ResourceManager::loadImage(bgObject0, "graveyard_mbg_a");
  ResourceManager::loadImage(bgObject1, "graveyard_mbg_b");
  ResourceManager::loadImage(bgObject2, "graveyard_mbg_c");
  ResourceManager::loadImage(bgObject3, "graveyard_mbg_d");
  ResourceManager::loadImage(bgObject4, "graveyard_mbg_e");
  
  // ground terrain
  loadImage(terrainBordersImg, "graveyard_terrain_borders");
  loadImage(terrainFillImg, "graveyard_terrain_fill");
}

void TestPanoBackground::init()
{
  getImg(bgLayer1)->handle_norm(HANDLE_BL);
  
  mainLayerGrid.set(getImg(bgLayer1), 0, 0);
  mainLayerGrid.set(getImg(bgLayer2), 1, 0);
  mainLayerGrid.set(getImg(bgLayer3), 2, 0);
  mainLayerGrid.set(getImg(bgLayer4), 3, 0);
  mainLayerGrid.set(getImg(bgLayer5), 4, 0);

  mainLayerGrid.setTileSize(getImg(bgLayer1));
  mainLayerGrid.setHandleSmart(HANDLE_BL);

  getImg(bgObject0)->handle = Point2(0.0, 1024.0 - getImg(bgObject0)->natural_size().y);
  getImg(bgObject1)->handle = Point2(0.0, 1024.0 - getImg(bgObject1)->natural_size().y);
  getImg(bgObject2)->handle = Point2(0.0, 1024.0 - getImg(bgObject2)->natural_size().y);
  getImg(bgObject3)->handle = Point2(0.0, 1024.0 - getImg(bgObject3)->natural_size().y);
  getImg(bgObject4)->handle = Point2(0.0, 1024.0 - getImg(bgObject4)->natural_size().y);
  objectLayerGrid.set(getImg(bgObject0), 0, 0);
  objectLayerGrid.set(getImg(bgObject1), 1, 0);
  objectLayerGrid.set(getImg(bgObject2), 2, 0);
  objectLayerGrid.set(getImg(bgObject3), 3, 0);
  objectLayerGrid.set(getImg(bgObject4), 4, 0);
  
  objectLayerGrid.setTileSize(Point2(512.0, getImg(bgLayer1)->natural_size().y));
  objectLayerGrid.setHandleSmart(HANDLE_BL);

  level->backgroundType = GRAVEYARD_BG;
}

void TestPanoBackground::update()
{
  objectCreator();
}

void TestPanoBackground::drawBackground()
{
  Point1 bottomY = 540.0;
  
  // back color
  RMGraphics->clear(RM::color255(0x6B737EFF));

  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), bottomY, 0.25);
  
  // main background layer
  drawLandscapeTilesStd(mainLayerGrid, bottomY, 0.5);
  
  // objects
  drawLandscapeTilesStd(objectLayerGrid, bottomY, 0.75);
  /*
  applyHParallaxCam(0.75);
  
  for (Coord1 i = 0; i < backgroundObjs.count; ++i)
  {
    Coord1 imgIndex = bgObject0 + RM::clamp(RM::float_to_int(backgroundObjs[i].y), 0, 4);
    Point2 position = Point2(backgroundObjs[i].x, bottomY);
    
    getImg(imgIndex)->draw(position);
  }
   */
}

void TestPanoBackground::objectCreator()
{
  // Line camXRange = level->rmboxCamera
}

void TestPanoBackground::drawForeground()
{
  
}

// =============================== IntroLevelBackground ============================ //

IntroLevelBackground::IntroLevelBackground() :
  PizzaBackground()
{
  
}

void IntroLevelBackground::load()
{
  // layers
  loadImage(bgLayer0, "bg_palace_0", HANDLE_BL);
  loadImage(bgLayer1, "bg_palace_1", HANDLE_BL);
  loadImage(bgLayer2, "bg_palace_2", HANDLE_BL);
  
  loadSpine(bgObject0, "president_sleeping");
}

void IntroLevelBackground::init()
{
  level->backgroundType = GRAVEYARD_BG;

  Point2 deviceBL = deviceScreenBox().norm_pos(HANDLE_BL);

  createSpineObj(deviceBL + Point2(953.0, 498.0 - 816.0),
                 bgObject0, "idle", BG_LAYER_CLOSE, 1.0);
}

void IntroLevelBackground::update()
{
  updateSpineObjects();
}

void IntroLevelBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x8b9aaeff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.33);
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.5);
  
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer2),
                                    deviceScreenBox().bottom(), 1.0, BG_LAYER_CLOSE);
}


// ==================================== GraveyardBackground ================================ //

GraveyardBackground::GraveyardBackground() :
  PizzaBackground(),

  bgFog1(),
  bgFog2(),
  fgFog(),

  oldCamX(0.0)
{
  
}

void GraveyardBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_grave_edges");
  loadImage(terrainFillImg, "terrain_grave_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_gravestone_fill");
  loadImage(blockWorldBorderImg, "block_gravestone_border");
  loadImage(blockWorldTrixImg, "block_gravestone_trix");
  
  // layers
  loadImage(bgLayer0, "bg_grave_0", HANDLE_BL);
  loadImage(bgLayer1, "bg_grave_1", HANDLE_BL);
  loadImage(bgLayer2, "bg_grave_2", HANDLE_BL);
  loadImage(bgLayer3, "bg_grave_3", HANDLE_BL);
  loadImage(bgLayer4, "bg_grave_4", HANDLE_BL);
  loadImage(bgLayer5, "bg_grave_5", HANDLE_BL);
  loadImage(bgLayer6, "bg_grave_6", HANDLE_BL);
  loadImage(bgLayer7, "bg_grave_fg_a", HANDLE_BL);
  loadImage(bgLayer8, "bg_grave_fg_b", HANDLE_BL);
  loadImage(bgLayer9, "bg_grave_fg_c", HANDLE_BL);
  loadImage(bgLayer10, "bg_grave_moon", HANDLE_TL, true);
}

void GraveyardBackground::init()
{
  Point2 deviceBL = deviceScreenBox().norm_pos(HANDLE_BL);
  
  bgFog1.init(getImg(bgLayer3), Point2(-45.0, 0.0), deviceBL, RM_WH);
  bgFog2.init(getImg(bgLayer5), Point2(-65.0, 0.0), deviceBL, RM_WH);
  fgFog.init(getImg(bgLayer6), Point2(-85.0, 0.0), deviceBL, RM_WH);

  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer7, bgLayer9));

  level->backgroundType = GRAVEYARD_BG;
}

void GraveyardBackground::update()
{
  Point1 camMoveX = level->rmboxCamera.getX() - oldCamX;

  // dust.scrollSpeed = Point2(-100.0 - camMoveX / RM::timePassed(), 0.0);
  bgFog1.update();
  bgFog2.update();
  fgFog.update();
  oldCamX = level->rmboxCamera.getX();
}

void GraveyardBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x4f5f7aff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  
  // the moon doesn't move
  CameraStd levelCam = *RMGraphics->get_camera();
  RMGraphics->pop_camera();
  getImg(bgLayer10)->draw(deviceScreenBox().norm_pos(HANDLE_BL) + Point2(507.0, -538.0));
  RMGraphics->push_camera(levelCam);
  
  // far graves and trees
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.23);

  // medium graves and trees
  drawLandscapeTilesStd(getImg(bgLayer2), deviceScreenBox().bottom(), 0.4);

  // bg fog 1
  CameraStd cam;
  cam.apply();
  bgFog1.redraw();
  level->rmboxCamera.apply();  // TODO: figure out why I can't apply the levelCam. doesn't work here

  // close graves
  drawLandscapeTilesStd(getImg(bgLayer4), deviceScreenBox().bottom(), 0.7);

  // bg fog 2
  cam.apply();
  bgFog2.redraw();
  level->rmboxCamera.apply();  // TODO: figure out why I can't apply the levelCam. doesn't work here
}

void GraveyardBackground::drawForeground()
{
  // fg graves
  drawFGObjects(1.5);
  
  // fg fog
  CameraStd cam;
  cam.apply();
  fgFog.redraw();
  level->rmboxCamera.apply();
}

// =============================== JurassicBackground ============================ //

JurassicBackground::JurassicBackground() :
  PizzaBackground()
{
  
}

void JurassicBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_jurassic_edges");
  loadImage(terrainFillImg, "terrain_jurassic_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_lavarock_fill");
  loadImage(blockWorldBorderImg, "block_lavarock_border");
  loadImage(blockWorldTrixImg, "block_lavarock_trix");
  
  // layers
  loadImage(bgLayer0, "jurassic_bg_00", HANDLE_BL);
  loadImage(bgLayer1, "jurassic_bg_01", HANDLE_BL);
  loadImage(bgLayer2, "jurassic_bg_02", HANDLE_BL);
  loadImage(bgLayer3, "jurassic_bg_03", HANDLE_BL);
  loadImage(bgLayer4, "jurassic_bg_04", HANDLE_BL);
  loadImage(bgLayer5, "jurassic_bg_05", HANDLE_BL);
  loadImage(bgLayer6, "jurassic_bg_06", HANDLE_BL);

  // trees
  loadImage(bgLayer7, "jurassic_fg_a", HANDLE_BC);
  loadImage(bgLayer8, "jurassic_fg_b", HANDLE_BC);
  loadImage(bgLayer9, "jurassic_fg_c", HANDLE_BC);
}

void JurassicBackground::init()
{
  level->backgroundType = JURASSIC_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer7, bgLayer9));
}

void JurassicBackground::update()
{

}

void JurassicBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x8b9aaeff));

  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.21);
  drawLandscapeTilesStd(getImg(bgLayer2), deviceScreenBox().bottom(), 0.3);
  drawLandscapeTilesStd(getImg(bgLayer3), deviceScreenBox().bottom(), 0.39);
  drawLandscapeTilesStd(getImg(bgLayer4), deviceScreenBox().bottom(), 0.52);
  drawLandscapeTilesStd(getImg(bgLayer5), deviceScreenBox().bottom(), 0.65);
  drawLandscapeTilesStd(getImg(bgLayer6), deviceScreenBox().bottom(), 0.8);

}

void JurassicBackground::drawForeground()
{
  drawFGObjects(1.5);
}

// =============================== CakeBackground ============================ //

CakeBackground::CakeBackground() :
  PizzaBackground()
{
  
}

void CakeBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_cake_edges");
  loadImage(terrainFillImg, "terrain_cake_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_cookie_fill");
  loadImage(blockWorldBorderImg, "block_cookie_border");
  loadImage(blockWorldTrixImg, "block_cookie_trix");
  
  // layers
  loadImage(bgLayer0, "cake_bg_0", HANDLE_BL);
  loadImage(bgLayer1, "cake_bg_1", HANDLE_BL);
  loadImage(bgLayer2, "cake_bg_2", HANDLE_BL);
  
  // pastries
  loadImage(bgLayer3, "cake_fg_a", HANDLE_BC);
  loadImage(bgLayer4, "cake_fg_b", HANDLE_BC);
  loadImage(bgLayer5, "cake_fg_c", HANDLE_BC);
  
  // candles
  loadSpine(bgObject0, "small_candle");
  loadSpine(bgObject1, "medl_candle");
}

void CakeBackground::init()
{
  level->backgroundType = CAKE_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer3, bgLayer5));
  
  Point2 deviceBL = deviceScreenBox().norm_pos(HANDLE_BL);
  
  // far candles
  createSpineObj(deviceBL + Point2(100.0, -294.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(204.0, -413.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(302.0, -390.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(300.0, -250.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(350.0, -271.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(448.0, -216.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(464.0, -231.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(495.0, -233.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(536.0, -179.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(572.0, -196.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(602.0, -232.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(625.0, -212.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(735.0, -309.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(762.0, -297.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(787.0, -321.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(803.0, -283.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(873.0, -308.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(936.0, -113.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  
  // close candles
  createSpineObj(deviceBL + Point2(276.0, -358.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(529.0, -278.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(751.0, -349.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
}

void CakeBackground::update()
{
  updateSpineObjects();
}

void CakeBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x8b9aaeff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  
  // far cakes
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer1),
                                    deviceScreenBox().bottom(), 0.4, BG_LAYER_FAR);
  
  // close cakes
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer2),
                                    deviceScreenBox().bottom(), 0.8, BG_LAYER_CLOSE);
}

void CakeBackground::drawForeground()
{
  drawFGObjects(1.5);
}

// =============================== DumpBackground ============================ //

DumpBackground::DumpBackground() :
  PizzaBackground()
{
  
}

void DumpBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_dump_edges");
  loadImage(terrainFillImg, "terrain_dump_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_trash_fill");
  loadImage(blockWorldBorderImg, "block_trash_border");
  loadImage(blockWorldTrixImg, "block_trash_trix");
  
  // layers
  loadImage(bgLayer0, "dump_bg_00", HANDLE_BL);
  loadImage(bgLayer1, "dump_bg_01", HANDLE_BL);
  loadImage(bgLayer2, "dump_bg_02", HANDLE_BL);
  loadImage(bgLayer3, "dump_bg_03", HANDLE_BL);
  loadImage(bgLayer4, "dump_bg_04", HANDLE_BL);
  loadImage(bgLayer5, "dump_bg_fg_a", HANDLE_BC);
  loadImage(bgLayer6, "dump_bg_fg_b", HANDLE_BC);
  loadImage(bgLayer7, "dump_bg_fg_c", HANDLE_BC);
}

void DumpBackground::init()
{
  level->backgroundType = DUMP_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer5, bgLayer7));
}

void DumpBackground::update()
{
  
}

void DumpBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x3a2d27ff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.3);
  drawLandscapeTilesStd(getImg(bgLayer2), deviceScreenBox().bottom(), 0.5);
  drawLandscapeTilesStd(getImg(bgLayer3), deviceScreenBox().bottom(), 0.7);
  drawLandscapeTilesStd(getImg(bgLayer4), deviceScreenBox().bottom(), 0.8);
}

void DumpBackground::drawForeground()
{
  drawFGObjects(1.5);
}

// =============================== SiliconBackground ============================ //

SiliconBackground::SiliconBackground() :
  PizzaBackground()
{
  
}

void SiliconBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_silicon_edges", HANDLE_TL, true);
  loadImage(terrainFillImg, "terrain_silicon_fill", HANDLE_TL, true);
  
  // blocks
  loadImage(blockWorldFillImg, "block_window_fill");
  loadImage(blockWorldBorderImg, "block_window_border");
  // window doesn't use a trix
  
  // layers
  loadImage(bgLayer0, "silicon_bg_0", HANDLE_BL);
  loadImage(bgLayer1, "silicon_bg_1", HANDLE_BL);
  loadImage(bgLayer2, "silicon_bg_2", HANDLE_BL);
  loadImage(bgLayer3, "silicon_bg_3", HANDLE_BL);
}

void SiliconBackground::init()
{
  level->backgroundType = SILICON_BG;
  
//  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer5, bgLayer7));
}

void SiliconBackground::update()
{
  
}

void SiliconBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x30563dff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.3);
  drawLandscapeTilesStd(getImg(bgLayer2), deviceScreenBox().bottom(), 0.5);
  drawLandscapeTilesStd(getImg(bgLayer3), deviceScreenBox().bottom(), 0.8);
}

void SiliconBackground::drawForeground()
{
//  drawFGObjects(1.5);
}

// =============================== BodyBackground ============================ //

BodyBackground::BodyBackground() :
  PizzaBackground()
{
  
}

void BodyBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_body_edges");
  loadImage(terrainFillImg, "terrain_body_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_bacteria_fill");
  loadImage(blockWorldBorderImg, "block_bacteria_border");
  // bacteria doesn't use a trix
  
  // layers
  loadImage(bgLayer0, "body_bg", HANDLE_TL);
  loadImage(bgLayer1, "body_fg_a", HANDLE_BC);
  loadImage(bgLayer2, "body_fg_b", HANDLE_BC);
  loadImage(bgLayer3, "body_fg_c", HANDLE_BC);
}

void BodyBackground::init()
{
  level->backgroundType = BODY_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer1, bgLayer3));
}

void BodyBackground::update()
{
  
}

void BodyBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0xff0000ff));
  
  drawTilingXYBG(bgLayer0, 1.0);
  // cout << "drawing from " << startIntervals << " to " << endIntervals << endl;
}

void BodyBackground::drawForeground()
{
  drawFGObjects(1.5);
}

// =============================== CatacombsBackground ============================ //

CatacombsBackground::CatacombsBackground() :
  PizzaBackground()
{
  
}

void CatacombsBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_catacombs_edges");
  loadImage(terrainFillImg, "terrain_catacombs_fill");
  
  // blocks
  loadImage(blockWorldFillImg, "block_skulls_fill");
  loadImage(blockWorldBorderImg, "block_skulls_border");
  loadImage(blockWorldTrixImg, "block_skulls_trix");
  
  // layers
  loadImage(bgLayer0, "bg_catacombs_0", HANDLE_TL);
  loadImage(bgLayer1, "bg_catacombs_1", HANDLE_TL);
  loadImage(bgLayer2, "bg_catacombs_2", HANDLE_TL);
  loadImage(bgLayer3, "bg_catacombs_fg_a", HANDLE_BL);
  loadImage(bgLayer4, "bg_catacombs_fg_b", HANDLE_BL);
  loadImage(bgLayer5, "bg_catacombs_fg_c", HANDLE_BL);
  loadImage(bgLayer6, "bg_catacombs_overlay", HANDLE_TL, true);
}

void CatacombsBackground::init()
{
  level->backgroundType = CATACOMBS_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer3, bgLayer5));
}

void CatacombsBackground::update()
{
  
}

void CatacombsBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0xff0000ff));
  
  drawTilingXYBG(bgLayer0, 0.2);
  drawTilingXYBG(bgLayer1, 0.6);
  drawTilingXYBG(bgLayer2, 1.0);
}

void CatacombsBackground::drawForeground()
{
  drawFGObjects(1.5);
  
  RMGraphics->pop_camera();
  getImg(bgLayer6)->draw(Point2(0.0, 0.0), RM_WH, 0.0, WHITE_SOLID);

  RMGraphics->push_camera(level->rmboxCamera);
}

// ==================================== PuppyBackground ================================ //

PuppyBackground::PuppyBackground() :
  PizzaBackground(),

  fgFog()
{
  
}

void PuppyBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_puppy_edges");
  loadImage(terrainFillImg, "terrain_puppy_fill");
  
  // layers
  loadImage(bgLayer0, "puppy_bg_0", HANDLE_BL);
  loadImage(bgLayer1, "puppy_bg_1", HANDLE_BL);
  loadImage(bgLayer2, "puppy_bg_2", HANDLE_BL);
  loadImage(bgLayer3, "puppy_bg_3", HANDLE_BL);
  loadImage(bgLayer4, "puppy_bg_4", HANDLE_BL);
  loadImage(bgLayer5, "puppy_fg_a", HANDLE_BL);
  loadImage(bgLayer6, "puppy_fg_b", HANDLE_BL);
  loadImage(bgLayer7, "puppy_fg_c", HANDLE_BL);
  loadImage(bgLayer8, "noise", HANDLE_TL);  // referred to by the level
}

void PuppyBackground::init()
{
  getImg(bgLayer8)->repeats();
  
  Point2 deviceBL = deviceScreenBox().norm_pos(HANDLE_BL);
  fgFog.init(getImg(bgLayer4), Point2(-85.0, 0.0), deviceBL, RM_WH);
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer5, bgLayer7));
  
  level->backgroundType = PUPPY_BG;
}

void PuppyBackground::update()
{
  // Point1 camMoveX = level->rmboxCamera.getX() - oldCamX;
  
  // dust.scrollSpeed = Point2(-100.0 - camMoveX / RM::timePassed(), 0.0);
  fgFog.update();
  // oldCamX = level->rmboxCamera.getX();
}

void PuppyBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x524b4aff));
  
  // bg layers
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  drawLandscapeTilesStd(getImg(bgLayer1), deviceScreenBox().bottom(), 0.3);
  drawLandscapeTilesStd(getImg(bgLayer2), deviceScreenBox().bottom(), 0.5);
  drawLandscapeTilesStd(getImg(bgLayer3), deviceScreenBox().bottom(), 0.8);
}

void PuppyBackground::drawForeground()
{
  // fg graves
  drawFGObjects(1.5);
  
  // fg fog
  CameraStd cam;
  cam.apply();
  fgFog.redraw();
  level->rmboxCamera.apply();
}

// =============================== FiremanBackground ============================ //

FiremanBackground::FiremanBackground() :
  PizzaBackground()
{
  
}

void FiremanBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "terrain_fireman_edges");
  loadImage(terrainFillImg, "terrain_fireman_fill");
  
  // layers
  loadImage(bgLayer0, "bg_fireman_0", HANDLE_BL);
  loadImage(bgLayer1, "bg_fireman_1", HANDLE_BL);
  loadImage(bgLayer2, "bg_fireman_2", HANDLE_BL);
  loadImage(bgLayer3, "bg_fireman_3", HANDLE_BL);

  // objects
  loadImage(bgLayer4, "bg_fireman_fg_a", HANDLE_BC);
  loadImage(bgLayer5, "bg_fireman_fg_b", HANDLE_BC);
  loadImage(bgLayer6, "bg_fireman_fg_c", HANDLE_BC);
  
  // candles
  loadSpine(bgObject0, "bg_fireman_flame_a");
  loadSpine(bgObject1, "bg_fireman_flame_b");
  loadSpine(bgObject2, "bg_fireman_flame_c");
}

void FiremanBackground::init()
{
  level->backgroundType = FIREMAN_BG;
  
  fgObjCreator(Point2(0.0, RM_WH.x * 3.0), Coord2(bgLayer4, bgLayer6));
  
  Point2 deviceBL = deviceScreenBox().norm_pos(HANDLE_BL);
  
  // far flames
  createSpineObj(deviceBL + Point2(96.0, -348.0),
                bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(244.0, -360.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(435.0, -360.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(581.0, -360.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(707.0, -359.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(860.0, -348.0),
                 bgObject0, "idle", BG_LAYER_FAR, RM::randf(0.8, 1.2));

  // closer flames
  createSpineObj(deviceBL + Point2(93.0, -328.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(313.0, -328.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(608.0, -330.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(735.0, -287.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));
  createSpineObj(deviceBL + Point2(922.0, -330.0),
                 bgObject1, "idle", BG_LAYER_CLOSE, RM::randf(0.8, 1.2));

  // closest flames
  createSpineObj(deviceBL + Point2(480.0, -139.0),
                 bgObject2, "idle", BG_LAYER_CLOSEST, RM::randf(0.8, 1.2));
}

void FiremanBackground::update()
{
  updateSpineObjects();
}

void FiremanBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x07090eff));
  
  // sky
  drawLandscapeTilesStd(getImg(bgLayer0), deviceScreenBox().bottom(), 0.1);
  
  // far bg with flames
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer1),
                                    deviceScreenBox().bottom(), 0.35, BG_LAYER_FAR, false);
  
  // closer bg with flames
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer2),
                                    deviceScreenBox().bottom(), 0.55, BG_LAYER_CLOSE, false);
  
  // closest bg with flames
  drawLandscapeTilesAndSpineObjsStd(getImg(bgLayer3),
                                    deviceScreenBox().bottom(), 0.8, BG_LAYER_CLOSEST, false);
}

void FiremanBackground::drawForeground()
{
  drawFGObjects(1.5);
}

// =============================== PlaneBG ============================ //

PlaneBG::PlaneBG() :
  PizzaBackground(),

  layers(),
  speedList(),

  masterSpeed(-512.0)
{
  
}

void PlaneBG::load()
{
  loadImage(bgLayer0, "bg_plane_0", HANDLE_BL);
  loadImage(bgLayer1, "bg_plane_1", HANDLE_BL);
  loadImage(bgLayer2, "bg_plane_2", HANDLE_BL);
  loadImage(bgLayer3, "bg_plane_3", HANDLE_BL);
  loadImage(bgLayer4, "bg_plane_4", HANDLE_BL);
  loadImage(bgLayer5, "bg_plane_5", HANDLE_BL);
  loadImage(bgLayer6, "bg_plane_6", HANDLE_BL);
  loadImage(bgLayer7, "bg_plane_7", HANDLE_BL);
  loadImage(bgLayer8, "bg_plane_8", HANDLE_BL);
  loadImage(bgLayer9, "bg_plane_overlay", HANDLE_BL);
}

void PlaneBG::init()
{
  speedList.add(0.01);
  speedList.add(0.03);
  speedList.add(0.06);
  speedList.add(0.12);
  speedList.add(0.25);
  speedList.add(0.50);
  speedList.add(0.30);
  speedList.add(0.70);
  speedList.add(1.00);
  speedList.add(2.00);
  
  // sky
  for (Coord1 i = 0; i < speedList.count; ++i)
  {
    layers.add(ScrollingImage());
    layers.last().init(getImg(bgLayer0 + i),
                       Point2(masterSpeed * speedList[i], 0.0), deviceScreenBox().norm_pos(HANDLE_BL),
                       RM_WH);
  }
}

void PlaneBG::update()
{
  for (Coord1 i = 0; i < layers.count; ++i)
  {
    layers[i].update();
  }
}

void PlaneBG::drawBackground()
{
  for (Coord1 i = 0; i < layers.count - 1; ++i)
  {
    layers[i].redraw();
  }
}

void PlaneBG::drawForeground()
{
  layers.last().redraw();
}

// =============================== LakeBackground ============================ //

LakeBackground::LakeBackground() :
  PizzaBackground()
{
  
}

void LakeBackground::load()
{
  // ground terrain
  loadImage(terrainBordersImg, "graveyard_terrain_borders");
  loadImage(terrainFillImg, "graveyard_terrain_fill");
  
  // layers
  loadImage(bgLayer0, "bg_lake_0", HANDLE_TL);
  loadImage(bgLayer1, "bg_lake_1", HANDLE_TL, true);
  loadImage(bgLayer2, "bg_lake_2", HANDLE_TL, true);
}

void LakeBackground::init()
{
  level->backgroundType = LAKE_BG;
}

void LakeBackground::update()
{
  
}

void LakeBackground::drawBackground()
{
  RMGraphics->clear(RM::color255(0x0000ffff));

  Point2 farBGSize = getImg(bgLayer0)->natural_size() - RM_WH;
  Point1 bgX = RM::double_lerp(level->rmboxCamera.getX(),
                               Point2(0.0, LAKE_WORLD_SIZE.x - RM_WH.x),
                               Point2(0.0, farBGSize.x));
  Point1 bgY = RM::double_lerp(level->rmboxCamera.getY(),
                               Point2(0.0, LAKE_WORLD_SIZE.y - RM_WH.y),
                               Point2(0.0, farBGSize.y));
  
  CameraStd parallaxCam;
  parallaxCam.size = level->rmboxCamera.size;
  parallaxCam.setXY(bgX, bgY);
  // parallaxCam.setXY(0.0, 0.0);

  parallaxCam.apply();
  getImg(bgLayer0)->draw();

  drawTilingXYBG(bgLayer1, 1.0);
}

void LakeBackground::drawForeground()
{
  drawTilingXYBG(bgLayer2, 1.5);
}

// =============================== PizzeriaBG ============================ //

PizzeriaBG::PizzeriaBG() :
  PizzaBackground(),

  fireZoom(1.0)
{
  
}

void PizzeriaBG::load()
{
  loadImage(bgLayer0, "shop_bg");
  loadImage(bgLayer1, "shop_arch");
  loadSpine(bgObject0, "shop_flame");
  loadSpine(bgObject1, "chef");
  
  // terrain
  loadImage(redrockCenterImg, "redrock_center");
  loadImage(redrockBorderImg, "redrock_border");
  loadImage(redrockTerrainBordersImg, "redrock_terrain_borders");
}

void PizzeriaBG::init()
{
  createSpineObj(Point2(512.0, 225.0), bgObject0, "burn", BG_LAYER_CLOSE);
//  createSpineObj(Point2(817.0, 321.0), bgObject1, "idle", BG_LAYER_PF);
  
  spineObjList[0]->setScale(3.0, 3.0);
}

void PizzeriaBG::update()
{
  updateSpineObjects();
}

void PizzeriaBG::drawBackground()
{
  RMGraphics->push_camera(Point2(0.0, 0.0), RM_WH * 0.5, Point2(fireZoom, fireZoom), 0.0);
  getImg(bgLayer0)->draw();

  drawSpineObjLayer(BG_LAYER_CLOSE);
  RMGraphics->pop_camera();

  // draw top arch
  CameraStd topHUDCam = level->rmboxCamera;
  topHUDCam.xy.y *= -1.0;
  RMGraphics->push_camera(topHUDCam);
  getImg(bgLayer1)->draw();
  RMGraphics->pop_camera();

  // darken everything
  RM_BOX.draw_solid(ColorP4(0.0, 0.0, 0.0, 0.2));
}

void PizzeriaBG::drawForeground()
{
  
}

void PizzeriaBG::zoomIntoEquip()
{
  
}

void PizzeriaBG::backToShop()
{
  
}
