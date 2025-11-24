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

// =============================== PizzaLevelInterface ============================ //

PizzaLevelInterface::PizzaLevelInterface() :
  levelID(),
  levelState(LEVEL_PASSIVE),

  tutorial(NULL),
  tutShown(false),

  camera(),
  shaker(0.3),
  randGen(4385378),
  scoreText(),
  endingScoreText(),

  healthBar(),
  healthStayTimer(2.5),
  prevPlayerDMG(0),
  healthChaseVal(1.0),
  healthYBounds(0.0, 0.0),
  healthYVal(0.0),
  healthYData(0.0, 1.0),
  healthPulseVal(0.0),
  healthPulseData(0.0, 1.0),

  coinFrame(0),
  coinsCreated(0),
  easySkeleID(RM::randi(0, 2)),

  worldTimeMult(1.0),

  musicFilename(16)
{
  tutorial = new PassiveTutorial(this);
  musicFilename = "tuscany.ima4";  // this is so you know when it's unset
}

PizzaLevelInterface::~PizzaLevelInterface()
{
  delete tutorial;
  
  if (RMPhysics != NULL)
  {
    delete RMPhysics;
    RMPhysics = NULL;
  }
}

PizzaLevelInterface* PizzaLevelInterface::createLevel(Coord2 levelDef)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  // levelDef.set(LEVEL_TIMING, TIMING_CIRCLES); ////////////////
  // levelDef.set(LEVEL_GLADIATOR, GLADIATOR_DISK); ////////////////
  // Pizza::currWorldPair.set(WORLD_MARKET, 0); //////////////////
  
  switch (levelDef.x)
  {
    default: cout << "ERROR: invalid level type" << endl;
      
    case LEVEL_GLADIATOR: resultLevel = createGladiatorLevel(levelDef.y); break;
    case LEVEL_ANGRY:     resultLevel = createAngryLevel(levelDef.y);     break;
    case LEVEL_BOUNCE:    resultLevel = createBounceLevel(levelDef.y);    break;
    case LEVEL_WRECKING:  resultLevel = createWreckLevel(levelDef.y);     break;
    case LEVEL_SLOPES:    resultLevel = createSlopesLevel(levelDef.y);    break;
    case LEVEL_KARATE:    resultLevel = createKarateLevel(levelDef.y);    break;
    case LEVEL_BALANCE:   resultLevel = createBalanceLevel(levelDef.y);   break;
    case LEVEL_LAKE:      resultLevel = createLakeLevel(levelDef.y);      break;
    case LEVEL_PUPPY:     resultLevel = createPuppyLevel(levelDef.y);     break;
    case LEVEL_SUMO:      resultLevel = createSumoLevel(levelDef.y);      break;
    case LEVEL_PLANE:     resultLevel = createPlaneLevel(levelDef.y);     break;
    case LEVEL_TIMING:    resultLevel = createTimingLevel(levelDef.y);    break;
  }
  
  resultLevel->levelID = levelDef.y;
  resultLevel->preload();
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createGladiatorLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Gladiator level type" << endl;
    case GLADIATOR_TUTORIAL:    resultLevel = new GLLevelTutorial();    break;
    case GLADIATOR_SPEARMEN:    resultLevel = new GLLevelSpearmen();    break;
    case GLADIATOR_GARGOYLE:    resultLevel = new GLLevelGargoyle();    break;
    case GLADIATOR_SPINY:       resultLevel = new GLLevelSpiny();       break;
    case GLADIATOR_STICKY:      resultLevel = new GLLevelSticky();      break;
    case GLADIATOR_OSTRICH_V:   resultLevel = new GLLevelOstrichV();    break;
    case GLADIATOR_GIANT:       resultLevel = new GLLevelGiant();       break;
    case GLADIATOR_FLAMESKULL:  resultLevel = new GLLevelFlameskull();  break;
    case GLADIATOR_OSTRICH_H:   resultLevel = new GLLevelOstrichH();    break;
    case GLADIATOR_CUPID:       resultLevel = new GLLevelCupid();       break;
    case GLADIATOR_ROBOT:       resultLevel = new GLLevelRobot();       break;
    case GLADIATOR_WISP:        resultLevel = new GLLevelWisp();        break;
    case GLADIATOR_GRAVITY:     resultLevel = new GLLevelGrav();        break;
    case GLADIATOR_NAGA:        resultLevel = new GLLevelNaga();        break;
      // case GLADIATOR_MIX_1:       resultLevel = new GLLevelMix1();        break;
    case GLADIATOR_STICKY_MIX:  resultLevel = new GLLevelStickyMix();   break;
    case GLADIATOR_ENDING:      resultLevel = new GLLevelEnding();      break;
    case GLADIATOR_LITE_ENDING: resultLevel = new GLLevelLiteEnding();  break;
    case GLADIATOR_RABBIT:      resultLevel = new GLLevelRabbit();      break;
    case GLADIATOR_TAIL:        resultLevel = new GLLevelTail();        break;
    case GLADIATOR_DISK:        resultLevel = new GLLevelDisk();        break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createKarateLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Karate level type" << endl;
    case KARATE_STILL:      resultLevel = new KALevelStill();       break;
    case KARATE_BOUNCE:     resultLevel = new KALevelBounce();      break;
    case KARATE_WALK:       resultLevel = new KALevelWalk();        break;
    case KARATE_FLY:        resultLevel = new KALevelFly();         break;
    case KARATE_HOP:        resultLevel = new KALevelHop();         break;
    case KARATE_SPEAR:      resultLevel = new KALevelSpear();       break;
    case KARATE_ILLUSION:   resultLevel = new KALevelIllusion();    break;
    case KARATE_CIRCLE:     resultLevel = new KALevelCircle();      break;
    case KARATE_GRAVITY:    resultLevel = new KALevelGravity();     break;
    case KARATE_MIX:        resultLevel = new KALevelMix();         break;
    case KARATE_MUMMY:      resultLevel = new KALevelMummy();       break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createSlopesLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Karate level type" << endl;
    case SLOPES_A: resultLevel = new SLLevelA(); break;
    case SLOPES_B: resultLevel = new SLLevelB(); break;
    case SLOPES_C: resultLevel = new SLLevelC(); break;
    case SLOPES_D: resultLevel = new SLLevelD(); break;
    case SLOPES_E: resultLevel = new SLLevelE(); break;
    case SLOPES_F: resultLevel = new SLLevelF(); break;
    case SLOPES_G: resultLevel = new SLLevelG(); break;
    case SLOPES_H: resultLevel = new SLLevelH(); break;
    case SLOPES_I: resultLevel = new SLLevelI(); break;
    case SLOPES_J: resultLevel = new SLLevelJ(); break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createBalanceLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Balance level type" << endl;
    case BALANCE_A: resultLevel = new BALevelA(); break;
    case BALANCE_B: resultLevel = new BALevelB(); break;
    case BALANCE_C: resultLevel = new BALevelC(); break;
    case BALANCE_D: resultLevel = new BALevelD(); break;
    case BALANCE_E: resultLevel = new BALevelE(); break;
    case BALANCE_F: resultLevel = new BALevelF(); break;
    case BALANCE_G: resultLevel = new BALevelG(); break;
    case BALANCE_H: resultLevel = new BALevelH(); break;
    case BALANCE_I: resultLevel = new BALevelI(); break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createBounceLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Bounce level type" << endl;
    case BOUNCE_EASY:       resultLevel = new BOLevelEasy();      break;
    case BOUNCE_ARROW:      resultLevel = new BOLevelArrow();     break;
    case BOUNCE_HEAVY:      resultLevel = new BOLevelHeavy();     break;
    case BOUNCE_SQUISH:     resultLevel = new BOLevelSquish();    break;
    case BOUNCE_BIG:        resultLevel = new BOLevelBig();       break;
    case BOUNCE_LIGHTNING:  resultLevel = new BOLevelLightning(); break;
    case BOUNCE_EXPLODE:    resultLevel = new BOLevelExplode();   break;
    case BOUNCE_SUN:        resultLevel = new BOLevelSun();       break;
    case BOUNCE_STREAKER:   resultLevel = new BOLevelStreaker();  break;
    case BOUNCE_ENERGY:     resultLevel = new BOLevelEnergy();    break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createPuppyLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Puppy level type" << endl;
    case PUPPY_EASY:      resultLevel = new PULevelEasy();        break;
    case PUPPY_SPIKEPIT:  resultLevel = new PULevelSpikepits();   break;
    case PUPPY_MOLE:      resultLevel = new PULevelMoles();       break;
    case PUPPY_SPIKEBALL: resultLevel = new PULevelSpikeballs();  break;
    case PUPPY_FALLPIT:   resultLevel = new PULevelFallpits();    break;
    case PUPPY_FIREBALL:  resultLevel = new PULevelFireballs();   break;
    case PUPPY_BOMB:      resultLevel = new PULevelBombs();       break;
    case PUPPY_MIMIC:     resultLevel = new PULevelMimics();      break;
    case PUPPY_ARROW:     resultLevel = new PULevelArrows();      break;
    case PUPPY_MIX:       resultLevel = new PULevelMix();         break;
    case PUPPY_SUN:       resultLevel = new PULevelSun();         break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createAngryLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Angry level type" << endl;
    case ANGRY_STONEHENGE:  resultLevel = new ANLevelStonehenge();  break;
    case ANGRY_CASTLE:      resultLevel = new ANLevelCastle();      break;
    case ANGRY_CABIN:       resultLevel = new ANLevelCabin();       break;
    case ANGRY_FORT:        resultLevel = new ANLevelFort();        break;
    case ANGRY_GLASS:       resultLevel = new ANLevelGlass();       break;
    case ANGRY_PYRAMID:     resultLevel = new ANLevelPyramid();     break;
    case ANGRY_PAGODA:      resultLevel = new ANLevelPagoda();      break;
    case ANGRY_IGLOO:       resultLevel = new ANLevelIgloo();       break;
    case ANGRY_CAVE:        resultLevel = new ANLevelCave();        break;
    case ANGRY_CLOUDCITY:   resultLevel = new ANLevelCloud();       break;
    case ANGRY_MARKET:      resultLevel = new ANLevelMarket();       break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createLakeLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Lake level type" << endl;
    case LAKE_EASY:       resultLevel = new LALevelEasy();      break;
    case LAKE_PEARL:      resultLevel = new LALevelPearl();     break;
    case LAKE_STARFISH:   resultLevel = new LALevelStarfish();  break;
    case LAKE_PUFFER:     resultLevel = new LALevelPuffer();    break;
    case LAKE_TIDE:       resultLevel = new LALevelTide();      break;
    case LAKE_SWORDFISH:  resultLevel = new LALevelSwordfish(); break;
    case LAKE_POISON:     resultLevel = new LALevelPoison();    break;
    case LAKE_HARPOON:    resultLevel = new LALevelHarpoon();   break;
    case LAKE_BARREL:     resultLevel = new LALevelBarrel();    break;
    case LAKE_EEL:        resultLevel = new LALevelEel();       break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createWreckLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Wreck level type" << endl;
    case WRECK_LINES:     resultLevel = new WRLevelLines();    break;
    case WRECK_CIVILIAN:  resultLevel = new WRLevelCivilian(); break;
    case WRECK_BASEBALL:  resultLevel = new WRLevelBaseball(); break;
    case WRECK_CLIMBER:   resultLevel = new WRLevelClimber();  break;
    case WRECK_JUNK:      resultLevel = new WRLevelJunk();     break;
    case WRECK_FLIER:     resultLevel = new WRLevelFlier();    break;
    case WRECK_HARDWALL:  resultLevel = new WRLevelHardwall(); break;
    case WRECK_UFO:       resultLevel = new WRLevelUFO();      break;
    case WRECK_CUTTER:    resultLevel = new WRLevelCutter();   break;
    case WRECK_MIX:       resultLevel = new WRLevelMix();      break;
    case WRECK_BATTER:    resultLevel = new WRLevelBatter();   break;
  }
  
  return resultLevel;
}


PizzaLevelInterface* PizzaLevelInterface::createSumoLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Sumo level type" << endl;
    case SUMO_GRAVEYARD:    resultLevel = new SULevelGraveyard();     break;
    case SUMO_BADLANDS:     resultLevel = new SULevelBadlands();      break;
    case SUMO_QUARRY:       resultLevel = new SULevelQuarry();        break;
    case SUMO_PIRATE:       resultLevel = new SULevelPirate();        break;
    case SUMO_STADIUM:      resultLevel = new SULevelStadium();       break;
    case SUMO_NUCLEAR:      resultLevel = new SULevelNuclear();       break;
    case SUMO_COMPUTER:     resultLevel = new SULevelComputer();      break;
    case SUMO_CARNIVAL:     resultLevel = new SULevelCarnival();      break;
    case SUMO_MOON:         resultLevel = new SULevelMoon();          break;
    case SUMO_MEAT:         resultLevel = new SULevelMeat();          break;
    case SUMO_CLOUD:        resultLevel = new SULevelCloud();         break;
    case SUMO_MARKET:       resultLevel = new SULevelMarket();          break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createPlaneLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Plane level type" << endl;
    case PLANE_CUPID:   resultLevel = new PLLevelCupid();     break;
    case PLANE_WISP:    resultLevel = new PLLevelWisp();      break;
    case PLANE_NAGA:    resultLevel = new PLLevelNaga();      break;
    case PLANE_NODE:    resultLevel = new PLLevelNode();      break;
  }
  
  return resultLevel;
}

PizzaLevelInterface* PizzaLevelInterface::createTimingLevel(Coord1 levelID)
{
  PizzaLevelInterface* resultLevel = NULL;
  
  switch (levelID)
  {
    default: cout << "ERROR: invalid Timing level type" << endl;
    case TIMING_CIRCLES:   resultLevel = new TILevelCircles();     break;
    case TIMING_SCALERS:   resultLevel = new TILevelScalers();     break;
    case TIMING_SLIDERS:   resultLevel = new TILevelSliders();     break;
  }
  
  return resultLevel;
}

void PizzaLevelInterface::addTextEffectStd(const String2& str, Point2 location, Point1 holdTime)
{
  // adds as a front action
  BonusText* textEffect = new BonusText(str, ResourceManager::font,
                                        location, false);
  textEffect->upwardVal = 0.0;
  textEffect->holdTime = holdTime;
  textEffect->scaleToTextSize(Point2(SCREEN_W - 32.0, SCREEN_H));
  textEffect->init();
  addFrontAction(textEffect);
}

void PizzaLevelInterface::addSauceEffect(const VisRectangular& pizzaPlayer, Point2 atkPoint)
{
  Point2 impactPoint = RM::attract(pizzaPlayer.getXY(), atkPoint, pizzaPlayer.getWidth());
  Point1 impactAngle = RM::angle(pizzaPlayer.getXY(), atkPoint);
  
  PoofEffect* sauceEffect = new PoofEffect(impactPoint, RM::random(imgsetCache[sauceSplats]));
  sauceEffect->scaleTime = 0.5;
  sauceEffect->beforeFadeTime = 0.25;
  sauceEffect->fadeTime = 0.25;
  sauceEffect->endScale = 3.0;
  sauceEffect->setColor255(166.0, 74.0, 66.0);
  sauceEffect->setRotation(impactAngle);
  sauceEffect->init();
  addAction(sauceEffect);
  
  SauceParticles* drops = new SauceParticles(impactPoint, impactAngle);
  addAction(drops);
}

void PizzaLevelInterface::initScoreText()
{
  scoreText.setText(8, ResourceManager::scoreFont, Point2(1.0, 0.0));
  scoreText.setText("");
  scoreText.setXY(PLAY_W - Pizza::platformTL.x - 12.0, PLAY_H - Pizza::platformTL.y - 35.0);
  
  endingScoreText.setText(16, ResourceManager::font, Point2(0.5, 0.0));
  endingScoreText.setText(String2(LSTRINGS[104], '$', Pizza::playerScore));
  endingScoreText.setXY(PLAY_CENTER + Point2(0.0, 80.0));
  endingScoreText.setScale(0.8, 0.8);
  endingScoreText.setAlpha(0.0);
}

Coord1 PizzaLevelInterface::getCoinFrame()
{
  Coord1 oldFrame = coinFrame;
  coinFrame++;
  coinFrame %= imgsetCache[pennySet].count;
  return oldFrame;
}

void PizzaLevelInterface::gotCoin(Point2 center, Coord1 cashVal)
{
  Pizza::currGameVars[VAR_CASH] += cashVal;
  Pizza::currGameVars[VAR_NUM_COINS]++;
  gotScore(cashVal);
  
  addAction(new CoinParticles(center));
  // for (Coord1 i = 0; i < 6; ++i) addAction(new SingleCoinSparkle(center));
  // for (Coord1 i = 0; i < 14; ++i) addAction(new CoinParticles(center));
  
  ResourceManager::playCoinGet();
}

void PizzaLevelInterface::gotScore(Coord1 scoreVal)
{
  Pizza::currGameVars[VAR_SCORE] += scoreVal;
  Pizza::currGameVars[VAR_SCORE] = std::max(0, Pizza::currGameVars[VAR_SCORE]);
  scoreText.getTextPtr()->int_string(Pizza::currGameVars[VAR_SCORE]);
  scoreText.textChanged();
}

Coord1 PizzaLevelInterface::getEasySkeleIndex()
{
  easySkeleID++;
  easySkeleID %= 3;
  return easySkeleID;
}

void PizzaLevelInterface::initHealth()
{
  Point1 low = Pizza::platformTL.y + 40.0;
  healthYBounds.set(low - 96.0, low);
  healthBar.setXY(Pizza::platformTL.x + 40.0, healthYBounds.x);
  healthBar.setWH(28.0, 28.0);
  healthBar.setAlpha(0.0);
  healthStayTimer.setInactive();
  prevPlayerDMG = Pizza::currGameVars[VAR_DAMAGE];
}

void PizzaLevelInterface::updateHealthBar()
{
  if (usesHealthBar() == false) return;
  
  Point1 barFraction = 1.0 - (Point1) Pizza::currGameVars[VAR_DAMAGE] / (Point1) MAX_DMG_STD;
  Logical barPulsing = barFraction <= 0.25;
  
  healthStayTimer.update();
  
  if (prevPlayerDMG != Pizza::currGameVars[VAR_DAMAGE] || barPulsing == true)
  {
    healthStayTimer.reset();
  }
  
  if (barPulsing == true)
  {
    RM::bounce_arcsine(healthPulseVal, healthPulseData, Point2(0.0, 1.0), 2.0 * RM::timePassed());
  }
  
  RM::flatten_me(healthChaseVal, barFraction, RM::timePassed());
  
  Point1 healthValTgt = healthStayTimer.getActive() ? 1.0 : 0.0;
  RM::flatten_me(healthYVal, healthValTgt, 4.0 * RM::timePassed());
  
  healthBar.setY(RM::lerp(healthYBounds, healthYVal));
  prevPlayerDMG = Pizza::currGameVars[VAR_DAMAGE];
}

void PizzaLevelInterface::drawHealthBar()
{
  if (usesHealthBar() == false) return;
  
  healthBar.setImage(imgsetCache[healthBarSet][0]);
  healthBar.setAlpha(healthYVal * healthMasterAlpha());
  if (Pizza::DEBUG_NO_INTERFACE == true) healthBar.setAlpha(0.0);
  healthBar.drawMe();
  
  ColorP4 pieColor = RM::lerp(HEALTH_COLOR_START, HEALTH_COLOR_PULSE, healthPulseVal);
  pieColor.w = healthBar.getAlpha();
  Circle pie(healthBar.getXY(), healthBar.getWidth());
  RMGraphics->draw_slice(pie, PI * 1.5, -healthChaseVal, pieColor);
  
  healthBar.setImage(imgsetCache[healthBarSet][1]);
  healthBar.drawMe();
}

void PizzaLevelInterface::winLevel()
{
  if (levelState != LEVEL_PLAY) return;
  
  levelState = LEVEL_WIN;
  addFrontAction(CreateWinBanner());
  
  addFrontAction(new DeAccelFn(&worldTimeMult, 0.25, 1.0));
  
  // cout << "time " << Pizza::currGameVars[VAR_SECONDS] << endl;
  // cout << "destroyed " << Pizza::currGameVars[VAR_DEFEATED] << endl;
  
  ActionQueue* winScript = new ActionQueue();
  winScript->enqueueX(new SoundVolumeFade(soundCache[music], 0.0, 0.25));
  winScript->enqueueX(new StopSoundCommand(soundCache[music]));
  winScript->enqueueX(new PlaySoundCommand(ResourceManager::winLick));
  winScript->wait(2.75);
  winScript->enqueueX(new SceneTransitionCommand(new WheelScene()));
  addFrontAction(winScript);
  
  Pizza::currGameVars[VAR_SCORE] += calcWinBonus();
  
  Coord1 world = Pizza::currWorldPair.x;
  Coord1 levelNum = Pizza::currWorldPair.y;
  
  if (Pizza::moreGamesMan != NULL)
  {
    Pizza::moreGamesMan->incrementTrackingAndSave();
  }
  
  if (world == 0 && levelNum == 9)
  {
    Pizza::scoreMan->earnedAchievement(ACH_10TH_LEVEL);
  }
  else if (world == 4 && levelNum == 9)
  {
    Pizza::scoreMan->earnedAchievement(ACH_50TH_LEVEL);
  }
  else if (world == 9 && levelNum == 9)
  {
    Pizza::scoreMan->earnedAchievement(ACH_100TH_LEVEL);
  }
}

void PizzaLevelInterface::loseLevel()
{
  if (levelState != LEVEL_PLAY) return;
  
  Point1 beforeGUIDir = 3.0;
  
  LoseRedEffect* red = new LoseRedEffect(beforeGUIDir);
  addFrontAction(red);
  
  levelState = LEVEL_LOSE;
  BannerSwoopEffect* loseEffect = new BannerSwoopEffect(imgCache[loseBanner],
                                                        Point2(PLAY_CX, 0.0), PLAY_CY);
  addFrontAction(loseEffect);
  
  // MapScene* scene = new MapScene();
  // scene->transInHint = PizzaScene::FROM_LOSE;
  
  ActionQueue* loseScript = new ActionQueue();
  loseScript->enqueueX(new SoundVolumeFade(soundCache[music], 0.0, 0.25));
  loseScript->enqueueX(new StopSoundCommand(soundCache[music]));
  loseScript->enqueueX(new PlaySoundCommand(ResourceManager::loseLick));
  loseScript->wait(beforeGUIDir - 0.25);
  loseScript->enqueueX(new PushGUICommand(new LoseGUI()));
  addFrontAction(loseScript);
}

void PizzaLevelInterface::startTutorial(PizzaTutorial* newTutorial)
{
  delete tutorial;
  tutorial = newTutorial;
}

void PizzaLevelInterface::updateTutorials()
{
  tutorial->update();
  
  if (tutorial->state == RM::REMOVE)
  {
    PizzaTutorial* nextTut = tutorial->nextTutorial;
    if (nextTut == NULL)
    {
      nextTut = new PassiveTutorial(this);
    }
    
    tutorial->nextTutorial = NULL;
    delete tutorial;
    tutorial = nextTut;
  }
}

Logical PizzaLevelInterface::lostToDMGStd()
{
  return Pizza::currGameVars[VAR_DAMAGE] >= MAX_DMG_STD;
}

Logical PizzaLevelInterface::canUnlockTopping(Coord2 toppingID)
{
  Logical result = Pizza::isToppingUnlocked(toppingID) == false;
  if (Pizza::lite == true) result = false;
  return result;
}

Logical PizzaLevelInterface::alreadyBeaten()
{
  return Pizza::currLevelData().starsEarned > 0;
}

CameraStd PizzaLevelInterface::platCam()
{
  CameraStd platCam = camera;
  platCam.xy += SCREEN_BOX.xy;
  platCam.handle -= SCREEN_BOX.xy;
  platCam.size = SCREEN_WH;
  return platCam;
}

Box PizzaLevelInterface::camBox()
{
  return platCam().myBox();
}

// =============================== PizzaObjman ============================ //

PizzaObjman::PizzaObjman() :
ObjmanStd<PizzaGO>(),
bgTerrainList(),
sortedDrawList(16),
debrisList()
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
  actions.update();
  
  Point1 time = RM::pop_time_mult();
  frontActions.update();
  RM::push_time_mult(time);
}

void PizzaObjman::updateObjects()
{
  for (Coord1 i = 0; i < bgTerrainList.count; ++i) bgTerrainList[i]->update();
  ObjmanStd<PizzaGO>::updateObjects();
}

void PizzaObjman::drawObjects()
{
  sortedDrawList.clear();
  
  ObjmanStd<PizzaGO>::drawObjects();
  
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

void PizzaObjman::addBGTerrain(PizzaGO* terrain)
{
  bgTerrainList.add(terrain);
}

// =============================== PizzaLevel ============================ //

PizzaLevel::PizzaLevel() :
  PizzaLevelInterface(),
  ActionListener(),
  b2ContactListener(),

  objman(),
  player(NULL),
  background(NULL),

  worldBox(PLAY_BOX),
  endpoint(0.0, 288.0),
  spikedTerrain(false),
  bounceTerrain(false),
  useOctBall(false),

  chunkWeights(NUM_CHUNK_TYPES, true),
  runningEnpoints(16),

  ballList(5),
  currBallID(-1),

  puppyList(10),
  currBreed(randGen.randi(0, 2)),
  puppiesToFind(0),
  puppiesToReturn(0),
  puppiesTotal(0),
  puppyStartPt(0.0, 0.0),
  puppyHousePt(0.0, 0.0),

  startingSkulls(0),
  skullsCrushed(0),
  totalDestructibles(0),

  tiltPercent(0.0),
  playerMotionFlags(0x0),

  junkTimer(2.0, true, this),
  textDrawer(32, ResourceManager::font, Point2(0.5, 1.0))
{
  for (Coord1 i = 0; i < chunkWeights.count; ++i) chunkWeights[i] = 0.0;
  junkTimer.setInactive();
  textDrawer.setXY(PLAY_CX, PLAY_BOX.bottom());
}

PizzaLevel::~PizzaLevel()
{
  delete player;
  delete background;
}

void PizzaLevel::load()
{
  RMPhysics = new RivermanPhysics(Point2(0.0, 500.0), true);
  RMPhysics->SetContactListener(this);
  
  initScoreText();
  initHealth();
  
  player = new PizzaPlayer();
  player->level = this;
  player->load();
  
  loadMe();
}

void PizzaLevel::update()
{
  if (tutorial->shouldUpdateGame() == true) updateGame();
  
  updateTutorials();
}

void PizzaLevel::redraw()
{
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  
  // == camera on
  RMGraphics->push_camera(CameraStd());
  
  if (background != NULL) background->drawBackground();
  camera.apply();
  
  // drawBottomBorder();
  drawBGTerrain();
  drawShadows();
  
  player->redraw();
  objman.drawObjects();
  objman.debrisList.redraw();
  
  objman.actions.redraw();
  if (background != NULL) background->drawForeground();
  camera.apply();
  
  RMPhysics->draw_debug();
  RMGraphics->pop_camera();
  // == camera off
  
  RMGraphics->pop_camera();  // undo shaker
  
  textDrawer.drawText();
  scoreText.drawText();
  drawHealthBar();
  drawHUD();
  
  tutorial->redraw();
  
  objman.frontActions.redraw();
  if (endingScoreText.getAlpha() > 0.01) endingScoreText.drawText();
}

void PizzaLevel::updateGame()
{
  //////////// special time
  RM::push_time_mult(worldTimeMult);
  
  RMPhysics->update();
  player->update();  // this should happen before the controls update to update standing
  objman.update();
  
  if (levelState == LEVEL_PLAY)
  {
    Pizza::runGameTime(RM::timePassed());
    junkTimer.update();
  }
  
  shaker.update();
  if (background != NULL) background->update();
  
  updateControls();
  if (levelState == LEVEL_PLAY) updateStatus();
  updateLevelAI();
  updateHealthBar();
  updateCamera();
  
  RM::pop_time_mult();
  ////////////
}

void PizzaLevel::startStanding(Point2 groundPt)
{
  player->setXY(groundPt.x, groundPt.y - player->getHeight());
  player->isStanding = true;
  player->updateFromWorld();
}

void PizzaLevel::reportPlayerLanded(Logical slam)
{
  if (slam == true)
  {
    reportSlam();
  }
}

void PizzaLevel::reportPlayerBump(Point1 impulseVal)
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

void PizzaLevel::reportSlam()
{
  for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGO* enemy = enemyNode->data;
    if (enemy->lifeState == RM::ACTIVE) enemy->pizzaSlammed();
  }
}

void PizzaLevel::reportSurface(Coord1 surfaceBits)
{
  playerMotionFlags |= surfaceBits;
}

void PizzaLevel::reportExplode(const Circle& explosion)
{
  Box maxBox = explosion.AABB();
  if (maxBox.collision(camera.myBox()) == true)
  {
    for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
         enemyNode != NULL;
         enemyNode = enemyNode->next)
    {
      PizzaGO* enemy = enemyNode->data;
      if (enemy->lifeState == RM::ACTIVE) enemy->explosionOnscreen(explosion);
    }
    
    for (LinkedListNode<PizzaGO*>* allyNode = objman.allyList.first;
         allyNode != NULL;
         allyNode = allyNode->next)
    {
      PizzaGO* ally = allyNode->data;
      if (ally->lifeState == RM::ACTIVE) ally->explosionOnscreen(explosion);
    }
  }
  
}

BalanceSkull* PizzaLevel::getCurrBall()
{
  return currBallID == -1 ? NULL : ballList[currBallID];
}

void PizzaLevel::drawBottomBorder()
{
  CameraStd tempCam = platCam();
  Point2 topLeft = tempCam.myBox().corner(3) + Point2(-6.0, 0.0) / tempCam.zoom;
  Point2 botRight = Point2(tempCam.myBox().corner(2));
  Box blackBox = Box::from_corners(topLeft, botRight + Point2(12.0, 6.0) / tempCam.zoom);
  blackBox.draw_solid(ColorP4(0.0, 0.0, 0.0, 1.0));
}

void PizzaLevel::drawShadowsStd(Point1 playerGroundY)
{
  // this is a hack to draw the sludges right now
  for (LinkedListNode<PizzaGO*>* allyNode = objman.allyList.first;
       allyNode != NULL;
       allyNode = allyNode->next)
  {
    PizzaGO* ally = allyNode->data;
    Point1 shadowY = ally->flies ? playerGroundY : ally->sortY;
    Image* shadow = ally->shadowImg;
    
    if (shadow != NULL)
    {
      shadow->draw_scale(Point2(ally->getX(), shadowY), ally->getScale(), 0.0, ally->getColor());
    }
  }
  
  for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGO* enemy = enemyNode->data;
    Point1 shadowY = enemy->flies ? playerGroundY : enemy->sortY;
    Image* shadow = enemy->shadowImg;
    
    if (shadow != NULL)
    {
      shadow->draw_scale(Point2(enemy->getX(), shadowY), enemy->getScale(), 0.0, enemy->getColor());
    }
  }
  
  if (player->shadowImg != NULL)
  {
    player->shadowImg->draw_scale(Point2(player->getX(), playerGroundY),
                                  player->getScale(), 0.0, player->getColor());
  }
  
}

void PizzaLevel::drawBGTerrain()
{
  objman.drawBGTerrain();
}

void PizzaLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  if (Pizza::controller->clickedOutsidePause() == true ||
      Pizza::controller->keyboard.get_status(JUST_PRESSED, kVK_Space) == true ||
      Pizza::controller->keyboard.get_status(JUST_PRESSED, kVK_UpArrow) == true ||
      Pizza::controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_W) == true)
  {
    // player asks level for jump mult here, level takes sticky into account
    player->tryJump();
  }
  
  tiltPercent = tiltMagnitude();
  
  player->tryMove(tiltPercent);
  
  // stickies slow the player here
  if ((playerMotionFlags & PizzaGO::SLOW_BIT) != 0)
  {
    Point2 vel = player->get_velocity();
    player->set_velocity(Point2(RM::flatten(vel.x, 0.0, 800.0 * RM::timePassed()), vel.y));
  }
  
  // this is for slopes to even out bumpy areas
  if (playerGroundStick() == true &&
      player->standingTimer.getActive() == true &&
      player->nojumpTimer.getActive() == false)
  {
    player->apply_force(Point2(0.0, 1000.0), player->getXY());
  }
  
  playerMotionFlags = 0x0;    // this has to happen after controls update
}

Point1 PizzaLevel::tiltMagnitude()
{
  Point1 result = 0.0;
  
  // cout << "tilt % " << Pizza::controller->get_accel().x << endl;
  Point1 topTolerance = 0.25;
  
  if (RMSystem->get_OS_family() == APPLETV_FAMILY)
  {
    topTolerance = 0.5;
  }
  
  Point1 accelXClamp = RM::clamp(Pizza::controller->get_accel().x - Pizza::controller->calib,
                                 -topTolerance, topTolerance);
  
  // controls for simulator
  if (RMSystem->on_iphone_simulator() == true)
  {
    if (Pizza::controller->get_status(PRESSED, 0) == true)
    {
      result = (Pizza::controller->getXY().x - Pizza::deviceScreenBox().center().x) / Pizza::deviceScreenBox().center().x;
    }
    else
    {
      result = 0.0;
    }
  }
  // handle joysticks, they take precedence because they are
  else if (RM::approxEq(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 0).x, 0.0) == false)
  {
    result = RM::clamp(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 0).x, -1.0, 1.0);
  }
  else if (RM::approxEq(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 1).x, 0.0) == false)
  {
    result = RM::clamp(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 1).x, -1.0, 1.0);
  }
  // the accelerometers comes before dpad input because you sort of use the dpad to jump on apple tv
  else if (Pizza::controller->getDirectionalButtonStatus(WEST, PRESSED) == true)
  {
    result = -1.0;
  }
  else if (Pizza::controller->getDirectionalButtonStatus(EAST, PRESSED) == true)
  {
    result = 1.0;
  }
  else if (std::abs(accelXClamp) >= 0.01)
  {
    result = accelXClamp / topTolerance;
  }
  
  return result;
}

Point1 PizzaLevel::tiltMagnitudeVert()
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
  
  Point1 accelZClamp = RM::clamp(Pizza::controller->get_accel()[axisID] - Pizza::controller->calib,
                                 -maxTolerance, maxTolerance);
  
  // controls for simulator
  if (RMSystem->on_iphone_simulator() == true)
  {
    if (Pizza::controller->get_status(PRESSED, 0) == true)
    {
      result = (Pizza::controller->getXY().y - PLAY_CY) / PLAY_CY;
    }
    else
    {
      result = 0.0;
    }
  }
  else if (RM::approxEq(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 0).y, 0.0) == false)
  {
    result = RM::clamp(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 0).y, -1.0, 1.0);
  }
  else if (RM::approxEq(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 1).y, 0.0) == false)
  {
    result = RM::clamp(Pizza::controller->gamepad.get_joystick(kGP_Joystick_L, 1).y, -1.0, 1.0);
  }
  else if (Pizza::controller->getDirectionalButtonStatus(SOUTH, PRESSED) == true)
  {
    result = 1.0;
  }
  else if (Pizza::controller->getDirectionalButtonStatus(NORTH, PRESSED) == true)
  {
    result = -1.0;
  }
  else if (std::abs(accelZClamp) >= 0.03 && RMSystem->get_OS_family() != IOS_FAMILY)
  {
    result = accelZClamp / maxTolerance;
  }
  
  return result;
}

void PizzaLevel::updateCamera()
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = closestEnemyX();
  
  updateZoomStd();
  updateFacingStd(enemyAnywhere, closestX);
  enemyCameraStd(enemyAnywhere, closestX);
}

SmallSkull* PizzaLevel::createAngrySkull(Point2 center)
{
  SmallSkull* skull = new SmallSkull(center);
  addEnemy(skull);
  startingSkulls++;
  return skull;
}

Point1 PizzaLevel::playerTXVel()
{
  return 400.0;
}

Point1 PizzaLevel::bounceMult()
{
  return ((playerMotionFlags & PizzaGO::STICK_BIT) != 0) ? 0.75 : 1.0;
}

void PizzaLevel::updateZoomStd(Point1 yPad)
{
  Point1 playerTop = player->collisionBox().xy.y + yPad;
  Point1 camCeiling = PLAY_Y;
  Point1 diff = playerTop - camCeiling;
  
  if (diff < 0.0)
  {
    Point1 camZoom = diff / (PLAY_H * 1.0);
    camZoom = 1.0 / (1.0 + -camZoom);
    camera.zoom.set(camZoom, camZoom);
  }
  else
  {
    camera.zoom.set(1.0, 1.0);
  }
}

void PizzaLevel::updateFacingStd(Logical hasEnemies, Point1 closestX)
{
  // update facing
  Coord1 facing = 1;
  if (hasEnemies == true)
  {
    facing = closestX < 0.0 ? -1 : 1;
  }
  else
  {
    facing = tiltPercent < 0.0 ? -1 : 1;
  }
  player->setFacing(facing);
}

void PizzaLevel::enemyCameraStd(Logical hasEnemies, Point1 closestX)
{
  Point1 cameraTarget = camera.xy.x;
  
  // enemy onscreen
  if (hasEnemies == true && std::abs(closestX) < PLAY_W)
  {
    Point1 between = RM::lerp(player->getX(), player->getX() + closestX, 0.5);
    cameraTarget = between - PLAY_CX;
  }
  // enemy offscreen
  else if (hasEnemies == true)
  {
    cameraTarget = player->facing == 1 ? player->getX() : player->getX() - PLAY_W;
  }
  // no enemies
  else
  {
    Point1 percentTarget = (tiltPercent + 1.0) * 0.5;  // 0.0-1.0
    cameraTarget = RM::lerp(player->getX() - PLAY_W, player->getX(), percentTarget);
  }
  
  camera.xy.x = RM::flatten(camera.xy.x, cameraTarget, RM::timePassed() * playerTXVel() * 1.2);
  camera.xy.y = 0.0;
  
  camera.handle.set(PLAY_CX, PLAY_H);
  // camera.handle.set(SCREEN_CX, SCREEN_BOX.bottom());
}

void PizzaLevel::zoomToMinY(Point1 yTarget)
{
  camera.zoom = minYZoom(yTarget);
}

Point2 PizzaLevel::minYZoom(Point1 yTarget)
{
  Point1 highestTop = yTarget;
  Point1 camCeiling = camera.getY();
  Point1 diff = highestTop - camCeiling;
  
  if (diff < 0.0)
  {
    Point1 camZoom = diff / camera.handle.y;
    camZoom = 1.0 / (1.0 + -camZoom);
    return Point2(camZoom, camZoom);
  }
  else
  {
    return Point2(1.0, 1.0);
  }
}

Point1 PizzaLevel::closestEnemyX()
{
  Point1 invalidVal = 100000.0;
  Point1 closestValue = -invalidVal;
  Point1 closestLeft = -invalidVal;
  Point1 closestRight = invalidVal;
  
  for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGO* enemy = enemyNode->data;
    if (enemy->type == PizzaGO::TYPE_TERRAIN) continue;
    
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
}

Point1 PizzaLevel::closestEnemyX(Point1 dir)
{
  Point1 closestValue = 100000.0 * dir;
  Logical enemyAnywhere = false;
  
  for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
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

void PizzaLevel::addEnemy(PizzaGO* enemy)
{
  enemy->level = this;
  enemy->player = player;
  objman.addEnemy(enemy);
  enemy->load();
}

void PizzaLevel::addAlly(PizzaGO* ally)
{
  ally->level = this;
  ally->player = player;
  objman.addAlly(ally);
  ally->load();
}

void PizzaLevel::addBGTerrain(PizzaGO* terrain)
{
  terrain->level = this;
  terrain->player = player;
  objman.addBGTerrain(terrain);
  terrain->load();
}

void PizzaLevel::addDebris(ActionEvent* debrisEffect)
{
  objman.debrisList.addX(debrisEffect);
}

void PizzaLevel::addSortDrawer(PizzaGO* pgo)
{
  objman.sortedDrawList.add(pgo);
}

void PizzaLevel::addAction(ActionEvent* ae)
{
  objman.actions.addX(ae);
}

void PizzaLevel::addFrontAction(ActionEvent* ae)
{
  objman.frontActions.addX(ae);
}

Logical PizzaLevel::enemyActive()
{
  for (LinkedListNode<PizzaGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    if (enemyNode->data->getActive() == true) return true;
  }
  
  return false;
}

UnlockFloater* PizzaLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet)
{
  if (canUnlockTopping(toppingID) == false) return NULL;
  
  UnlockFloater* floater = new UnlockFloater(toppingID);
  floater->setXY(center);
  addAlly(floater);
  if (startMagnet == true) floater->startMagnet();
  
  return floater;
}

void PizzaLevel::createChunk(Coord1 chunkType, Coord1 variationID)
{
  switch (chunkType)
  {
    case CHUNK_SKI_START: createSkiStart(); break;
    case CHUNK_SKI_2_1: createSki2_1Chunk(); break;
    case CHUNK_SKI_3_1: createSki3_1Chunk(); break;
    case CHUNK_SKI_4_1: createSki4_1Chunk(); break;
    case CHUNK_SKI_DROP: createSkiDropChunk(); break;
    case CHUNK_SKI_GAP: createSkiGapChunk(); break;
    case CHUNK_SKI_JUMP: createSkiJumpChunk(); break;
    case CHUNK_SKI_BOUNCEGAP: createSkiBouncegapChunk(); break;
    case CHUNK_SKI_BOUNCEJUMP: createSkiBouncejumpChunk(); break;
    case CHUNK_SKI_DUALCOIN: createSkiDualcoinChunk(); break;
    case CHUNK_SKI_SPIKEPLATFORMS: createSkiSpikeplatformsChunk(); break;
    case CHUNK_SKI_FLAMESKULL: createSkiFlameskullChunk(variationID); break;
    case CHUNK_SKI_HILLSKULL: createSkiHillskullChunk(); break;
    case CHUNK_SKI_JAGSKULL: createSkiJagskullChunk(); break;
    case CHUNK_SKI_LONGFALL: createSkiLongfallChunk(); break;
    case CHUNK_SKI_SPIKEFALL: createSkiSpikefallChunk(); break;
    case CHUNK_SKI_STRAIGHTBREAK: createSkiStraightbreakChunk(); break;
    case CHUNK_SKI_SWITCHSLOPE: createSkiSwitchslopeChunk(); break;
    case CHUNK_SKI_SWITCHGAP: createSkiSwitchgapChunk(); break;
    case CHUNK_SKI_SPIKEPATHS: createSkiSpikepathsChunk(); break;
    case CHUNK_SKI_FALLBREAK: createSkiFallbreakChunk(); break;
    case CHUNK_SKI_END: createSkiEndChunk(); break;
      
    case CHUNK_SPIKE_START: spikeStartChunk(); break;
    case CHUNK_SPIKE_BUMP: spikeBumpChunk(); break;
    case CHUNK_SPIKE_DOWNHILL: spikeDownhillChunk(); break;
    case CHUNK_SPIKE_DROP: spikeDropChunk(); break;
    case CHUNK_SPIKE_FLAT: spikeFlatChunk(); break;
    case CHUNK_SPIKE_UPHILL: spikeUphillChunk(); break;
    case CHUNK_SPIKE_BOUNCECIRCLES: spikeBouncecirclesChunk(); break;
    case CHUNK_SPIKE_BOUNCEDROPS: spikeBouncedropsChunk(); break;
    case CHUNK_SPIKE_LONGFALL: spikeLongfallChunk(); break;
    case CHUNK_SPIKE_NOSPIKE: spikeNospikeChunk(); break;
    case CHUNK_SPIKE_RISER: spikeRiserChunk(); break;
    case CHUNK_SPIKE_PILLAR: spikePillarChunk(); break;
    case CHUNK_SPIKE_SPIKEBALL: spikeSpikeballChunk(); break;
    case CHUNK_SPIKE_SPRING: spikeSpringChunk(); break;
    case CHUNK_SPIKE_STRAIGHTBOUNCE: spikeStraightbounceChunk(variationID); break;
    case CHUNK_SPIKE_TRANSFERBLOCK: spikeTransferblockChunk(); break;
    case CHUNK_SPIKE_TRANSFERDROP: spikeTransferdropChunk(); break;
    case CHUNK_SPIKE_TRANSFERSPIKE: spikeTransferspikeChunk(); break;
    case CHUNK_SPIKE_BOUNCESTEPS: spikeBouncestepsChunk(); break;
    case CHUNK_SPIKE_DOWNHILLBUMP: spikeDownhillbumpChunk(); break;
    case CHUNK_SPIKE_DUALRISE: spikeDualriseChunk(); break;
    case CHUNK_SPIKE_RISEBALLS: spikeRiseballsChunk(); break;
    case CHUNK_SPIKE_RISESTEPS: spikeRisestepsChunk(); break;
    case CHUNK_SPIKE_BALLDOWNHILL: spikeBalldownhillChunk(); break;
    case CHUNK_SPIKE_BALLJUMP: spikeBalljumpChunk(); break;
    case CHUNK_SPIKE_ZIGZAG: spikeZigzagChunk(); break;
    case CHUNK_SPIKE_END: spikeEndChunk(); break;
      
    case CHUNK_PUPPY_CRATE: puppyCrateChunk(); break;
    case CHUNK_PUPPY_HILL: puppyHillChunk(variationID); break;
    case CHUNK_PUPPY_DIP: puppyDipChunk(); break;
    case CHUNK_PUPPY_DOGHOUSE: puppyDoghouseChunk(); break;
    case CHUNK_PUPPY_SPIKEPITHILL: puppySpikeHillChunk(variationID); break;
    case CHUNK_PUPPY_SPIKEJUMP: puppySpikeJumpChunk(); break;
    case CHUNK_PUPPY_TRICKYJUMP: puppyTrickyJumpChunk(); break;
    case CHUNK_PUPPY_LAVAHILL: puppyLavahillChunk(); break;
    case CHUNK_PUPPY_BIGOBJHILL: puppyBigobjhillChunk(variationID); break;
    case CHUNK_PUPPY_BOUNCEPIT: puppyBouncepitChunk(variationID); break;
    case CHUNK_PUPPY_CRATEPIT: puppyCratepitChunk(); break;
    case CHUNK_PUPPY_DUALFIREBALL: puppyDualfireballChunk(); break;
    case CHUNK_PUPPY_GROUNDTRIANGLE: puppyGroundtriangleChunk(); break;
    case CHUNK_PUPPY_LAVAPOOL: puppyLavalpoolChunk(); break;
    case CHUNK_PUPPY_OBJSTEPS: puppyObjstepsChunk(variationID); break;
    case CHUNK_PUPPY_PILLARS: puppyPillarsChunk(variationID); break;
    case CHUNK_PUPPY_QUADSPIKE: puppyQuadspikeChunk(variationID); break;
    case CHUNK_PUPPY_SINGLETRI: puppySingletriChunk(); break;
    case CHUNK_PUPPY_SPIKEBALLBUMPS: puppySpikeballBumpsChunk(); break;
    case CHUNK_PUPPY_SPIKEBALLHILL: puppySpikeballhillChunk(); break;
    case CHUNK_PUPPY_SPIKEBALLPILLARS: puppySpikeballpillarChunk(variationID); break;
    case CHUNK_PUPPY_SPIKEBALLPIT: puppySpikeballpitChunk(); break;
    case CHUNK_PUPPY_SWINGSQUARE: puppySwingsquareChunk(); break;
    case CHUNK_PUPPY_TRIANGLEPIT: puppyTrianglepitChunk(); break;
    case CHUNK_PUPPY_WALLJUMP: puppyWalljumpChunk(); break;
      
    default: cout << "NO VALID CHUNK " << chunkType << endl; break;
  }
  
  if (endpoint.x < 1.0) runningEnpoints.add(endpoint);  // first endpoint
  
  endpoint += CHUNK_ENDS[chunkType];
  runningEnpoints.add(endpoint);
}

void PizzaLevel::createRandomChunk()
{
  Coord1 chunkIndex = RM::random_index_weighted(chunkWeights, randGen);
  createChunk(chunkIndex);
}

void PizzaLevel::createSki2_1Chunk()
{
  TerrainQuad* quad = createSlopeQuad(Point2(0.0, 0.0), Point2(0.0, 128.0),
                                      Point2(512.0, 384.0), Point2(512.0, 256.0),
                                      imgCache[slope2_1]);
  
  createSnowboarder(quad);
  createFlag(Point2(288.0, 144.0), false);
}

void PizzaLevel::createSki3_1Chunk()
{
  TerrainQuad* quad = createSlopeQuad(Point2(0.0, 0.0), Point2(0.0, 128.0),
                                      Point2(576.0, 320.0), Point2(576.0, 192.0),
                                      imgCache[slope3_1]);
  
  createSnowboarder(quad);
  createFlag(Point2(288.0, 96.0), false);
}

void PizzaLevel::createSki4_1Chunk()
{
  TerrainQuad* quad = createSlopeQuad(Point2(0.0, 0.0), Point2(0.0, 128.0),
                                      Point2(576.0, 256.0), Point2(576.0, 128.0),
                                      imgCache[slope4_1]);
  
  createSnowboarder(quad);
  createFlag(Point2(288.0, 64.0), false);
}

void PizzaLevel::createSkiStart()
{
  Point1 yHandle = 128.0;
  Point2 topRight = slopePieceFlatLong(Point2(0.0, 0.0), 0.0)->topRight - endpoint;
  topRight = slopePieceFlatLong(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece4_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece4_1(topRight, 0.0)->topRight - endpoint;
  
  createFlag(Point2(416.0, 128.0), true, yHandle);
  createFlag(Point2(864.0, 128.0), false, yHandle);
  createFlag(Point2(1280, 128.0), true, yHandle);
  createFlag(Point2(1568.0, 192.0), false, yHandle);
  createFlag(Point2(1858.0, 256.0), false, yHandle);
  createFlag(Point2(2144.0, 322.0), false, yHandle);
  
}

void PizzaLevel::createSkiDropChunk()
{
  Point1 yHandle = 96.0;
  
  Point2 topRight = slopePiece4_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  createFlag(topRight, false, 0.0);
  createFlag(Point2(288.0, 161.0), false, yHandle);
  topRight = slopePiece3_1Short(topRight, 0.0)->topRight - endpoint;
  createFlag(topRight, false, 0.0);
  
  createCoin(Point2(361.0, 31.0), yHandle);
  createCoin(Point2(454.0, 56.0), yHandle);
  createCoin(Point2(551.0, 77.0), yHandle);
  createCoin(Point2(637.0, 104.0), yHandle);
  createCoin(Point2(728.0, 128.0), yHandle);
  createCoin(Point2(821.0, 156.0), yHandle);
  createCoin(Point2(903.0, 203.0), yHandle);
  createCoin(Point2(966.0, 274.0), yHandle);
  createCoin(Point2(1018.0, 356.0), yHandle);
  createCoin(Point2(1062.0, 438.0), yHandle);
  createCoin(Point2(1097.0, 527.0), yHandle);
  createCoin(Point2(1133.0, 612.0), yHandle);
  createCoin(Point2(1161.0, 701.0), yHandle);
  createCoin(Point2(1193.0, 791.0), yHandle);
  createCoin(Point2(1218.0, 881.0), yHandle);
  createCoin(Point2(1244.0, 972.0), yHandle);
  
  TerrainQuad* last = slopePiece4_1(Point2(944.0, 1056.0), yHandle);
  createFlag(Point2(1234.0, 1121.0), false, yHandle);
  // too steep for snowboarder
}

void PizzaLevel::createSkiGapChunk()
{
  Point1 yHandle = 176.0;
  
  slopePiece3_1(Point2(0.0, yHandle), yHandle);
  createFlag(Point2(480.0, 338.0), false, yHandle);
  
  createCoin(Point2(502.0, 199.0), yHandle);
  createCoin(Point2(580.0, 157.0), yHandle);
  createCoin(Point2(668.0, 113.0), yHandle);
  createCoin(Point2(757.0, 82.0), yHandle);
  createCoin(Point2(847.0, 57.0), yHandle);
  createCoin(Point2(943.0, 44.0), yHandle);
  createCoin(Point2(1039.0, 45.0), yHandle);
  createCoin(Point2(1132.0, 58.0), yHandle);
  createCoin(Point2(1226.0, 79.0), yHandle);
  createCoin(Point2(1315.0, 112.0), yHandle);
  createCoin(Point2(1400.0, 152.0), yHandle);
  createCoin(Point2(1482.0, 199.0), yHandle);
  createCoin(Point2(1560.0, 253.0), yHandle);
  createCoin(Point2(1639.0, 313.0), yHandle);
  createCoin(Point2(1704.0, 375.0), yHandle);
  createCoin(Point2(1772.0, 440.0), yHandle);
  createCoin(Point2(1837.0, 509.0), yHandle);
  createCoin(Point2(1899.0, 579.0), yHandle);
  createCoin(Point2(1960.0, 667.0), yHandle);
  
  slopePiece3_1Short(Point2(1824.0, 768.0), yHandle);
  createFlag(Point2(1921.0, 803.0), false, yHandle);
  
  createCoin(Point2(2052.0, 691.0), yHandle);
  createCoin(Point2(2141.0, 725.0), yHandle);
  createCoin(Point2(2226.0, 765.0), yHandle);
  createCoin(Point2(2309.0, 813.0), yHandle);
  createCoin(Point2(2385.0, 865.0), yHandle);
  createCoin(Point2(2458.0, 925.0), yHandle);
  createCoin(Point2(2530.0, 985.0), yHandle);
  createCoin(Point2(2598.0, 1053.0), yHandle);
  createCoin(Point2(2661.0, 1121.0), yHandle);
  
  slopePiece3_1Short(Point2(2544.0, 1232.0), yHandle);
  createFlag(Point2(2639.0, 1266.0), false, yHandle);
  
  createCoin(Point2(2735.0, 1073.0), yHandle);
  createCoin(Point2(2821.0, 1032.0), yHandle);
  createCoin(Point2(2914.0, 996.0), yHandle);
  createCoin(Point2(3001.0, 977.0), yHandle);
  createCoin(Point2(3100.0, 966.0), yHandle);
  createCoin(Point2(3192.0, 965.0), yHandle);
  createCoin(Point2(3288.0, 978.0), yHandle);
  createCoin(Point2(3380.0, 1001.0), yHandle);
  createCoin(Point2(3471.0, 1034.0), yHandle);
  createCoin(Point2(3557.0, 1072.0), yHandle);
  createCoin(Point2(3637.0, 1116.0), yHandle);
  createCoin(Point2(3713.0, 1174.0), yHandle);
  
  slopePiece3_1Short(Point2(3616.0, 1296.0), yHandle);
  createFlag(Point2(3712.0, 1330.0), false, yHandle);
}

void PizzaLevel::createSkiJumpChunk()
{
  Point1 yHandle = 64.0;
  
  Point2 topRight = slopePiece2_1(Point2(0.0, 0.0), 0.0)->topRight - endpoint;
  topRight = slopePieceFlatShort(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceUphill(topRight, 0.0)->topRight - endpoint;
  
  createFlag(Point2(512.0, 318.0), false, yHandle);
  createFlag(Point2(767.0, 318.0), false, yHandle);
  createFlag(Point2(1088.0, 240.0), false, yHandle);
  
  createCoin(Point2(830.0, 151.0), yHandle);
  createCoin(Point2(911.0, 128.0), yHandle);
  createCoin(Point2(1005.0, 105.0), yHandle);
  createCoin(Point2(1095.0, 83.0), yHandle);
  createCoin(Point2(1188.0, 71.0), yHandle);
  createCoin(Point2(1285.0, 72.0), yHandle);
  createCoin(Point2(1381.0, 84.0), yHandle);
  createCoin(Point2(1471.0, 107.0), yHandle);
  createCoin(Point2(1561.0, 139.0), yHandle);
  createCoin(Point2(1649.0, 182.0), yHandle);
  createCoin(Point2(1731.0, 226.0), yHandle);
  
  createFlag(Point2(1728.0, 400.0), false, yHandle);
  createFlag(Point2(2240.0, 654.0), false, yHandle);
  
  topRight = slopePiece2_1(Point2(1728.0, 400.0), yHandle)->topRight - endpoint;
  TerrainQuad* last = slopePiece4_1(topRight, 0.0);
  
  createSnowboarder(last);
}

void PizzaLevel::createSkiBouncegapChunk()
{
  Point1 yHandle = 224.0;
  
  slopePiece4_1(Point2(0.0, yHandle), yHandle);
  createFlag(Point2(289.0, 291.0), false, yHandle);
  
  createCoin(Point2(412.0, 182.0), yHandle);
  createCoin(Point2(496.0, 135.0), yHandle);
  createCoin(Point2(580.0, 93.0), yHandle);
  createCoin(Point2(669.0, 60.0), yHandle);
  createCoin(Point2(760.0, 40.0), yHandle);
  createCoin(Point2(857.0, 25.0), yHandle);
  createCoin(Point2(952.0, 26.0), yHandle);
  createCoin(Point2(1047.0, 38.0), yHandle);
  createCoin(Point2(1141.0, 62.0), yHandle);
  createCoin(Point2(1230.0, 98.0), yHandle);
  createCoin(Point2(1314.0, 135.0), yHandle);
  createCoin(Point2(1397.0, 182.0), yHandle);
  createCoin(Point2(1476.0, 238.0), yHandle);
  createCoin(Point2(1548.0, 298.0), yHandle);
  createCoin(Point2(1616.0, 356.0), yHandle);
  createCoin(Point2(1685.0, 424.0), yHandle);
  createCoin(Point2(1751.0, 491.0), yHandle);
  createCoin(Point2(1814.0, 564.0), yHandle);
  createCoin(Point2(1872.0, 637.0), yHandle);
  createCoin(Point2(1930.0, 712.0), yHandle);
  createCoin(Point2(1984.0, 791.0), yHandle);
  createCoin(Point2(2040.0, 867.0), yHandle);
  createCoin(Point2(2090.0, 948.0), yHandle);
  createCoin(Point2(2141.0, 1029.0), yHandle);
  createCoin(Point2(2190.0, 1110.0), yHandle);
  createCoin(Point2(2235.0, 1190.0), yHandle);
  createCoin(Point2(2284.0, 1274.0), yHandle);
  createCoin(Point2(2328.0, 1360.0), yHandle);
  
  slopePieceBouncy(Point2(1984.0, 1424.0), yHandle);
  
  createCoin(Point2(2400.0, 1291.0), yHandle);
  createCoin(Point2(2487.0, 1229.0), yHandle);
  createCoin(Point2(2580.0, 1180.0), yHandle);
  createCoin(Point2(2670.0, 1148.0), yHandle);
  createCoin(Point2(2765.0, 1129.0), yHandle);
  createCoin(Point2(2861.0, 1131.0), yHandle);
  createCoin(Point2(2953.0, 1148.0), yHandle);
  createCoin(Point2(3048.0, 1183.0), yHandle);
  createCoin(Point2(3138.0, 1235.0), yHandle);
  createCoin(Point2(3222.0, 1297.0), yHandle);
  createCoin(Point2(3300.0, 1364.0), yHandle);
  
  TerrainQuad* last = slopePiece4_1(Point2(3088.0, 1456.0), yHandle);
  createSnowboarder(last);
  createFlag(Point2(3377.0, 1522.0), false, yHandle);
}

void PizzaLevel::createSkiBouncejumpChunk()
{
  Point1 yHandle = 224.0;
  
  slopePiece4_1(Point2(0.0, yHandle), yHandle);
  
  createCoin(Point2(439.0, 181.0), yHandle);
  createCoin(Point2(520.0, 138.0), yHandle);
  createCoin(Point2(604.0, 100.0), yHandle);
  createCoin(Point2(693.0, 65.0), yHandle);
  createCoin(Point2(786.0, 42.0), yHandle);
  createCoin(Point2(880.0, 30.0), yHandle);
  createCoin(Point2(977.0, 29.0), yHandle);
  createCoin(Point2(1069.0, 39.0), yHandle);
  createCoin(Point2(1165.0, 65.0), yHandle);
  createCoin(Point2(1255.0, 98.0), yHandle);
  createCoin(Point2(1339.0, 138.0), yHandle);
  createCoin(Point2(1422.0, 187.0), yHandle);
  createCoin(Point2(1498.0, 240.0), yHandle);
  createCoin(Point2(1573.0, 300.0), yHandle);
  createCoin(Point2(1640.0, 359.0), yHandle);
  createCoin(Point2(1710.0, 427.0), yHandle);
  createCoin(Point2(1776.0, 499.0), yHandle);
  createCoin(Point2(1835.0, 567.0), yHandle);
  createCoin(Point2(1898.0, 644.0), yHandle);
  
  slopePieceBouncy(Point2(1584.0, 720.0), yHandle);
  
  createCoin(Point2(1974.0, 576.0), yHandle);
  createCoin(Point2(2060.0, 515.0), yHandle);
  createCoin(Point2(2149.0, 466.0), yHandle);
  createCoin(Point2(2240.0, 433.0), yHandle);
  createCoin(Point2(2336.0, 414.0), yHandle);
  createCoin(Point2(2431.0, 417.0), yHandle);
  createCoin(Point2(2525.0, 434.0), yHandle);
  createCoin(Point2(2621.0, 469.0), yHandle);
  createCoin(Point2(2708.0, 516.0), yHandle);
  createCoin(Point2(2793.0, 577.0), yHandle);
  createCoin(Point2(2877.0, 648.0), yHandle);
  createCoin(Point2(2954.0, 732.0), yHandle);
  createCoin(Point2(3024.0, 822.0), yHandle);
  
  slopeSpikeball(Point2(2242.0, 737.0), yHandle);
  slopeSpikeball(Point2(2562.0, 762.0), yHandle);
  
  TerrainQuad* last = slopePiece4_1(Point2(2720.0, 896.0), yHandle);
  createSnowboarder(last);
  createFlag(Point2(3010.0, 963.0), false, yHandle);
}

void PizzaLevel::createSkiDualcoinChunk()
{
  Point1 yHandle = 16.0;
  
  TerrainQuad* first = slopePiece3_1(Point2(0.0, yHandle), yHandle);
  createSnowboarder(first);
  createFlag(Point2(288.0, 113.0), false, yHandle);
  
  // first 4
  createCoin(Point2(400.0, 29.0), yHandle);
  createCoin(Point2(514.0, 50.0), yHandle);
  createCoin(Point2(615.0, 71.0), yHandle);
  createCoin(Point2(712.0, 97.0), yHandle);
  
  // start pairs
  createCoin(Point2(818.0, 120.0), yHandle);
  createCoin(Point2(768.0, 184.0), yHandle);
  
  createCoin(Point2(897.0, 187.0), yHandle);
  createCoin(Point2(836.0, 246.0), yHandle);
  
  createCoin(Point2(960.0, 260.0), yHandle);
  createCoin(Point2(896.0, 310.0), yHandle);
  
  createCoin(Point2(1016.0, 339.0), yHandle);
  createCoin(Point2(950.0, 389.0), yHandle);
  
  createCoin(Point2(1073.0, 426.0), yHandle);
  createCoin(Point2(1007.0, 472.0), yHandle);
  
  createCoin(Point2(1122.0, 512.0), yHandle);
  createCoin(Point2(1052.0, 553.0), yHandle);
  
  createCoin(Point2(1167.0, 591.0), yHandle);
  createCoin(Point2(1095.0, 630.0), yHandle);
  
  createCoin(Point2(1208.0, 680.0), yHandle);
  createCoin(Point2(1133.0, 718.0), yHandle);
  
  createCoin(Point2(1251.0, 762.0), yHandle);
  createCoin(Point2(1182.0, 796.0), yHandle);
  
  // last 1
  createCoin(Point2(1249.0, 871.0), yHandle);
  
  TerrainQuad* last = slopePiece3_1(Point2(960.0, 912.0), yHandle);
  // createSnowboarder(last);
  
  createFlag(Point2(1248.0, 1009.0), false, yHandle);
}

void PizzaLevel::createSkiSpikeplatformsChunk()
{
  Point1 yHandle = 128.0;
  
  slopePiece3_1(Point2(0.0, yHandle), yHandle);
  
  Logical topIsGood = randGen.randl();
  // cout << "topgood " << topIsGood << endl;
  
  if (topIsGood == true)
  {
    createCoin(Point2(563.0, 174.0), yHandle);
    createCoin(Point2(644.0, 125.0), yHandle);
    createCoin(Point2(730.0, 85.0), yHandle);
    createCoin(Point2(820.0, 55.0), yHandle);
    createCoin(Point2(912.0, 29.0), yHandle);
    createCoin(Point2(1009.0, 19.0), yHandle);
    createCoin(Point2(1102.0, 17.0), yHandle);
    createCoin(Point2(1197.0, 28.0), yHandle);
    createCoin(Point2(1289.0, 55.0), yHandle);
    createCoin(Point2(1379.0, 89.0), yHandle);
    createCoin(Point2(1464.0, 129.0), yHandle);
    createCoin(Point2(1546.0, 177.0), yHandle);
    
    slopePiece2_1(Point2(1280.0, 224.0), yHandle);
    slopePieceSpikePlatform(Point2(1280.0, 1088.0), yHandle);
    
    createCoin(Point2(1828.0, 353.0), yHandle);
    createCoin(Point2(1919.0, 385.0), yHandle);
    createCoin(Point2(2003.0, 423.0), yHandle);
    createCoin(Point2(2085.0, 473.0), yHandle);
    createCoin(Point2(2160.0, 526.0), yHandle);
    createCoin(Point2(2233.0, 586.0), yHandle);
    createCoin(Point2(2307.0, 647.0), yHandle);
    createCoin(Point2(2373.0, 713.0), yHandle);
    createCoin(Point2(2441.0, 784.0), yHandle);
    createCoin(Point2(2503.0, 853.0), yHandle);
    createCoin(Point2(2561.0, 925.0), yHandle);
    createCoin(Point2(2621.0, 1002.0), yHandle);
    createCoin(Point2(2676.0, 1080.0), yHandle);
    createCoin(Point2(2730.0, 1154.0), yHandle);
    createCoin(Point2(2780.0, 1239.0), yHandle);
  }
  else
  {
    createCoin(Point2(563.0, 174.0), yHandle);
    createCoin(Point2(656.0, 199.0), yHandle);
    createCoin(Point2(748.0, 232.0), yHandle);
    createCoin(Point2(835.0, 274.0), yHandle);
    createCoin(Point2(916.0, 319.0), yHandle);
    createCoin(Point2(993.0, 375.0), yHandle);
    createCoin(Point2(1065.0, 431.0), yHandle);
    createCoin(Point2(1137.0, 496.0), yHandle);
    createCoin(Point2(1204.0, 561.0), yHandle);
    createCoin(Point2(1270.0, 630.0), yHandle);
    createCoin(Point2(1331.0, 703.0), yHandle);
    createCoin(Point2(1392.0, 777.0), yHandle);
    createCoin(Point2(1452.0, 853.0), yHandle);
    createCoin(Point2(1505.0, 928.0), yHandle);
    createCoin(Point2(1558.0, 1007.0), yHandle);
    createCoin(Point2(1614.0, 1078.0), yHandle);
    createCoin(Point2(1660.0, 1165.0), yHandle);
    createCoin(Point2(1708.0, 1247.0), yHandle);
    
    slopePieceSpikePlatform(Point2(1280.0, 224.0), yHandle);
    slopePiece2_1(Point2(1280.0, 1088.0), yHandle);
  }
  
  Point2 topRight = slopePieceFlatLong(Point2(1792.0, 1344.0), yHandle)->topRight - endpoint;
  topRight = slopePieceFlatLong(topRight, 0.0)->topRight - endpoint;
}

void PizzaLevel::createSkiFlameskullChunk(Coord1 variationID)
{
  Point1 yHandle = 384.0;
  
  Point2 topRight = slopePiece3_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  topRight = slopePiece3_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece4_1(topRight, 0.0)->topRight - endpoint;
  
  slopePieceTriangle(Point2(992.0, 64.0), yHandle);
  
  createCoin(Point2(360.0, 348.0), yHandle);
  createCoin(Point2(440.0, 302.0), yHandle);
  createCoin(Point2(527.0, 261.0), yHandle);
  createCoin(Point2(615.0, 229.0), yHandle);
  createCoin(Point2(708.0, 204.0), yHandle);
  createCoin(Point2(801.0, 193.0), yHandle);
  createCoin(Point2(897.0, 197.0), yHandle);
  createCoin(Point2(994.0, 206.0), yHandle);
  createCoin(Point2(1034.0, 295.0), yHandle);
  createCoin(Point2(1069.0, 382.0), yHandle);
  createCoin(Point2(1098.0, 472.0), yHandle);
  createCoin(Point2(1127.0, 561.0), yHandle);
  createCoin(Point2(1153.0, 648.0), yHandle);
  
  FlameSkull* skull = slopeFlameskull(Point2(483.0, 530.0), yHandle);
  createFlag(Point2(192.0, 449.0), false, yHandle);
  createFlag(Point2(1162.0, 769.0), false, yHandle);
  
  if (variationID == 1)
  {
    tryCreateUnlockable(skull->getXY() + Point2(48.0, 8.0), Coord2(2, 10));
  }
}

void PizzaLevel::createSkiHillskullChunk()
{
  Point1 yHandle = 32.0;
  
  Point2 topRight = slopePiece4_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece3_1Short(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceFlatShort(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceUphill(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* last = slopePiece4_1(topRight, 0.0);
  
  createFlag(Point2(578.0, 163.0), false, yHandle);
  createFlag(Point2(831.0, 292.0), false, yHandle);
  createFlag(Point2(1857.0, 403.0), false, yHandle);
  createSnowboarder(last);
  slopeFlameskull(Point2(1440.0, 461.0), yHandle);
  
  createCoin(Point2(1086.0, 264.0), yHandle);
  createCoin(Point2(1164.0, 209.0), yHandle);
  createCoin(Point2(1247.0, 161.0), yHandle);
  createCoin(Point2(1339.0, 128.0), yHandle);
  createCoin(Point2(1433.0, 106.0), yHandle);
  createCoin(Point2(1532.0, 110.0), yHandle);
  createCoin(Point2(1626.0, 129.0), yHandle);
  createCoin(Point2(1714.0, 163.0), yHandle);
  createCoin(Point2(1797.0, 208.0), yHandle);
  createCoin(Point2(1870.0, 265.0), yHandle);
  createCoin(Point2(1942.0, 326.0), yHandle);
}

void PizzaLevel::createSkiJagskullChunk()
{
  Point1 yHandle = 64.0;
  
  Point2 topRight = slopePiece2_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceUphill(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceUphill(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* last = slopePiece2_1(topRight, 0.0);
  
  createCoin(Point2(680.0, 248.0), yHandle);
  createCoin(Point2(749.0, 184.0), yHandle);
  createCoin(Point2(826.0, 130.0), yHandle);
  createCoin(Point2(917.0, 102.0), yHandle);
  createCoin(Point2(1014.0, 97.0), yHandle);
  createCoin(Point2(1104.0, 127.0), yHandle);
  createCoin(Point2(1184.0, 174.0), yHandle);
  createCoin(Point2(1255.0, 239.0), yHandle);
  createCoin(Point2(1315.0, 308.0), yHandle);
  createCoin(Point2(1375.0, 389.0), yHandle);
  
  createCoin(Point2(1493.0, 413.0), yHandle);
  createCoin(Point2(1562.0, 348.0), yHandle);
  createCoin(Point2(1637.0, 296.0), yHandle);
  createCoin(Point2(1728.0, 265.0), yHandle);
  createCoin(Point2(1825.0, 262.0), yHandle);
  createCoin(Point2(1916.0, 289.0), yHandle);
  createCoin(Point2(1998.0, 341.0), yHandle);
  createCoin(Point2(2066.0, 402.0), yHandle);
  createCoin(Point2(2130.0, 475.0), yHandle);
  createCoin(Point2(2188.0, 554.0), yHandle);
  
  createFlag(Point2(289.0, 211.0), false, yHandle);
  createFlag(Point2(1344.0, 498.0), false, yHandle);
  createFlag(Point2(2167.0, 675.0), false, yHandle);
  slopeFlameskull(Point2(1019.0, 554.0), yHandle);
  slopeFlameskull(Point2(1851.0, 731.0), yHandle);
  createSnowboarder(last);
}

void PizzaLevel::createSkiLongfallChunk()
{
  Point1 yHandle = 128.0;
  
  Point2 topRight = slopePiece3_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  slopePieceFlatShort(topRight, 0.0);
  
  createFlag(Point2(672.0, 321.0), false, yHandle);
  
  createCoin(Point2(656.0, 181.0), yHandle);
  createCoin(Point2(736.0, 132.0), yHandle);
  createCoin(Point2(820.0, 92.0), yHandle);
  createCoin(Point2(912.0, 59.0), yHandle);
  createCoin(Point2(1004.0, 39.0), yHandle);
  
  createCoin(Point2(1094.0, 24.0), yHandle);
  createCoin(Point2(1192.0, 26.0), yHandle);
  createCoin(Point2(1287.0, 38.0), yHandle);
  createCoin(Point2(1380.0, 63.0), yHandle);
  createCoin(Point2(1468.0, 94.0), yHandle);
  
  createCoin(Point2(1556.0, 134.0), yHandle);
  createCoin(Point2(1637.0, 182.0), yHandle);
  createCoin(Point2(1714.0, 238.0), yHandle);
  createCoin(Point2(1798.0, 296.0), yHandle);
  createCoin(Point2(1859.0, 357.0), yHandle);
  
  createCoin(Point2(1926.0, 424.0), yHandle);
  createCoin(Point2(1991.0, 491.0), yHandle);
  createCoin(Point2(2053.0, 561.0), yHandle);
  createCoin(Point2(2115.0, 636.0), yHandle);
  createCoin(Point2(2171.0, 713.0), yHandle);
  
  createCoin(Point2(2227.0, 787.0), yHandle);
  createCoin(Point2(2279.0, 866.0), yHandle);
  createCoin(Point2(2332.0, 946.0), yHandle);
  createCoin(Point2(2380.0, 1026.0), yHandle);
  createCoin(Point2(2429.0, 1109.0), yHandle);
  
  topRight = slopePiece3_1Short(Point2(2176.0, 1152.0), yHandle)->topRight - endpoint;
  TerrainQuad* last = slopePieceFlatLong(topRight, 0.0);
  
  createSnowboarder(last);
  createFlag(Point2(2496.0, 1216.0), false, yHandle);
  createFlag(Point2(2814.0, 1216.0), false, yHandle);
}

void PizzaLevel::createSkiSpikefallChunk()
{
  Point1 yHandle = 64.0;
  
  slopePiece2_1(Point2(0.0, yHandle), yHandle);
  createFlag(Point2(257.0, 194.0), false, yHandle);
  createFlag(Point2(510.0, 320.0), false, yHandle);
  
  createCoin(Point2(514.0, 166.0), yHandle);
  createCoin(Point2(601.0, 203.0), yHandle);
  createCoin(Point2(679.0, 261.0), yHandle);
  createCoin(Point2(741.0, 331.0), yHandle);
  createCoin(Point2(796.0, 407.0), yHandle);
  
  createCoin(Point2(851.0, 489.0), yHandle);
  createCoin(Point2(896.0, 571.0), yHandle);
  createCoin(Point2(940.0, 653.0), yHandle);
  createCoin(Point2(981.0, 741.0), yHandle);
  createCoin(Point2(1017.0, 828.0), yHandle);
  
  createCoin(Point2(1052.0, 915.0), yHandle);
  createCoin(Point2(1086.0, 1007.0), yHandle);
  createCoin(Point2(1117.0, 1092.0), yHandle);
  createCoin(Point2(1148.0, 1183.0), yHandle);
  createCoin(Point2(1177.0, 1272.0), yHandle);
  
  slopeSpikeball(Point2(963.0, 97.0), yHandle);
  slopeSpikeball(Point2(1216.0, 514.0), yHandle);
  slopeSpikeball(Point2(1410.0, 1057.0), yHandle);
  
  slopePiece2_1(Point2(1056.0, 1344.0), yHandle);
  slopePiece4_1(Point2(1568.0, 1600.0), yHandle);
  
  createFlag(Point2(1184.0, 1409.0), false, yHandle);
  createFlag(Point2(1344.0, 1488.0), false, yHandle);
  createFlag(Point2(1568.0, 1600.0), false, yHandle);
  createFlag(Point2(1856.0, 1668.0), false, yHandle);
}

void PizzaLevel::createSkiStraightbreakChunk()
{
  Point1 yHandle = 160.0;
  
  Point2 topRight = slopePiece3_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  topRight = slopePieceBreakaway(topRight, 0.0)->topRight - endpoint;
  topRight = slopePieceBreakaway(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* last = slopePiece3_1(topRight, 0.0);
  
  createCoin(Point2(536.0, 200.0), yHandle);
  createCoin(Point2(617.0, 154.0), yHandle);
  createCoin(Point2(703.0, 110.0), yHandle);
  createCoin(Point2(794.0, 80.0), yHandle);
  createCoin(Point2(883.0, 55.0), yHandle);
  createCoin(Point2(978.0, 44.0), yHandle);
  createCoin(Point2(1074.0, 44.0), yHandle);
  createCoin(Point2(1170.0, 57.0), yHandle);
  createCoin(Point2(1263.0, 80.0), yHandle);
  createCoin(Point2(1351.0, 112.0), yHandle);
  createCoin(Point2(1435.0, 153.0), yHandle);
  createCoin(Point2(1520.0, 200.0), yHandle);
  createCoin(Point2(1598.0, 254.0), yHandle);
  createCoin(Point2(1669.0, 313.0), yHandle);
  createCoin(Point2(1740.0, 374.0), yHandle);
  createCoin(Point2(1807.0, 442.0), yHandle);
  createCoin(Point2(1872.0, 512.0), yHandle);
  createCoin(Point2(1936.0, 583.0), yHandle);
  createCoin(Point2(1994.0, 657.0), yHandle);
  
  createSnowboarder(last);
  createFlag(Point2(290.0, 257.0), false, yHandle);
  createFlag(Point2(2017.0, 834.0), false, yHandle);
  
}

void PizzaLevel::createSkiSwitchslopeChunk()
{
  Point1 yHandle = 64.0;
  
  Point2 topRight = slopePiece2_1(Point2(0.0, yHandle), yHandle)->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* spiked = slopePieceSpikePlatform(topRight, 0.0);
  topRight = spiked->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  topRight = slopePiece2_1(topRight, 0.0)->topRight - endpoint;
  
  SkullSwitch* switcher = slopeSwitch(Point2(689.0, 252.0), yHandle);
  switcher->panel = spiked;
  
  createCoin(Point2(771.0, 206.0), yHandle);
  createCoin(Point2(854.0, 164.0), yHandle);
  createCoin(Point2(944.0, 132.0), yHandle);
  createCoin(Point2(1036.0, 109.0), yHandle);
  createCoin(Point2(1132.0, 97.0), yHandle);
  createCoin(Point2(1229.0, 99.0), yHandle);
  createCoin(Point2(1320.0, 108.0), yHandle);
  createCoin(Point2(1414.0, 133.0), yHandle);
  createCoin(Point2(1506.0, 167.0), yHandle);
  createCoin(Point2(1588.0, 208.0), yHandle);
  createCoin(Point2(1672.0, 253.0), yHandle);
  createCoin(Point2(1751.0, 309.0), yHandle);
  createCoin(Point2(1820.0, 368.0), yHandle);
  createCoin(Point2(1891.0, 430.0), yHandle);
  createCoin(Point2(1959.0, 497.0), yHandle);
  createCoin(Point2(2027.0, 565.0), yHandle);
  createCoin(Point2(2086.0, 635.0), yHandle);
  createCoin(Point2(2146.0, 707.0), yHandle);
  createCoin(Point2(2206.0, 785.0), yHandle);
  createCoin(Point2(2258.0, 863.0), yHandle);
  createCoin(Point2(2316.0, 942.0), yHandle);
  createCoin(Point2(2366.0, 1019.0), yHandle);
  createCoin(Point2(2415.0, 1101.0), yHandle);
  createCoin(Point2(2465.0, 1180.0), yHandle);
  
  createFlag(Point2(2528.0, 1327.0), false, yHandle);
}

void PizzaLevel::createSkiSwitchgapChunk()
{
  Point1 yHandle = 96.0;
  
  slopePiece4_1(Point2(0.0, yHandle), yHandle);
  createFlag(Point2(290.0, 162.0), false, yHandle);
  
  createCoin(Point2(335.0, 41.0), yHandle);
  createCoin(Point2(437.0, 50.0), yHandle);
  createCoin(Point2(529.0, 73.0), yHandle);
  createCoin(Point2(623.0, 94.0), yHandle);
  createCoin(Point2(714.0, 115.0), yHandle);
  createCoin(Point2(806.0, 150.0), yHandle);
  
  SkullSwitch* switcher = slopeSwitch(Point2(886.0, 187.0), yHandle);
  
  createCoin(Point2(969.0, 237.0), yHandle);
  createCoin(Point2(1050.0, 293.0), yHandle);
  createCoin(Point2(1120.0, 348.0), yHandle);
  createCoin(Point2(1194.0, 413.0), yHandle);
  createCoin(Point2(1259.0, 476.0), yHandle);
  
  TerrainQuad* spikes = slopePieceSpikePlatform(Point2(1008.0, 480.0), yHandle);
  Point2 topRight = spikes->topRight - endpoint;
  TerrainQuad* last = slopePiece4_1(topRight, 0.0);
  
  switcher->panel = spikes;
  createFlag(Point2(1809.0, 801.0), false, yHandle);
  createSnowboarder(last);
}

void PizzaLevel::createSkiSpikepathsChunk()
{
  Point1 yHandle = 224.0;
  
  slopePiece4_1(Point2(0.0, yHandle), yHandle);
  
  Logical topIsGood = randGen.randl();
  
  if (topIsGood == true)
  {
    createCoin(Point2(577.0, 143.0), yHandle);
    createCoin(Point2(664.0, 105.0), yHandle);
    createCoin(Point2(753.0, 74.0), yHandle);
    createCoin(Point2(845.0, 51.0), yHandle);
    createCoin(Point2(938.0, 39.0), yHandle);
    createCoin(Point2(1036.0, 39.0), yHandle);
    createCoin(Point2(1131.0, 49.0), yHandle);
    createCoin(Point2(1224.0, 74.0), yHandle);
    createCoin(Point2(1312.0, 107.0), yHandle);
    createCoin(Point2(1397.0, 149.0), yHandle);
    createCoin(Point2(1497.0, 192.0), yHandle);
    createCoin(Point2(1557.0, 251.0), yHandle);
    
    slopeSpikeball(Point2(802.0, 290.0), yHandle);
    
    createCoin(Point2(1657.0, 274.0), yHandle);
    createCoin(Point2(1754.0, 258.0), yHandle);
    createCoin(Point2(1842.0, 282.0), yHandle);
    createCoin(Point2(1934.0, 315.0), yHandle);
    createCoin(Point2(2017.0, 357.0), yHandle);
    createCoin(Point2(2104.0, 402.0), yHandle);
    createCoin(Point2(2177.0, 458.0), yHandle);
    createCoin(Point2(2251.0, 518.0), yHandle);
    createCoin(Point2(2322.0, 577.0), yHandle);
    createCoin(Point2(2390.0, 647.0), yHandle);
    createCoin(Point2(2455.0, 714.0), yHandle);
    createCoin(Point2(2519.0, 785.0), yHandle);
    createCoin(Point2(2577.0, 858.0), yHandle);
    createCoin(Point2(2636.0, 933.0), yHandle);
    createCoin(Point2(2692.0, 1011.0), yHandle);
    createCoin(Point2(2745.0, 1088.0), yHandle);
    createCoin(Point2(2795.0, 1168.0), yHandle);
  }
  else
  {
    createCoin(Point2(576.0, 208.0), yHandle);
    createCoin(Point2(668.0, 234.0), yHandle);
    createCoin(Point2(759.0, 265.0), yHandle);
    createCoin(Point2(846.0, 306.0), yHandle);
    createCoin(Point2(928.0, 353.0), yHandle);
    createCoin(Point2(1008.0, 407.0), yHandle);
    createCoin(Point2(1081.0, 468.0), yHandle);
    createCoin(Point2(1150.0, 530.0), yHandle);
    createCoin(Point2(1221.0, 595.0), yHandle);
    createCoin(Point2(1284.0, 663.0), yHandle);
    createCoin(Point2(1344.0, 734.0), yHandle);
    createCoin(Point2(1409.0, 808.0), yHandle);
    
    slopeSpikeball(Point2(978.0, 66.0), yHandle);
  }
  
  slopePieceTriangle(Point2(1424.0, 384.0), yHandle);
  
  TerrainQuad* snow1 = slopePiece4_1(Point2(1024.0, 864.0), yHandle);
  Point2 topRight = snow1->topRight - endpoint;
  createSnowboarder(snow1);
  
  TerrainQuad* snow2 = slopePiece4_1(topRight, 0.0);
  topRight = snow2->topRight - endpoint;
  createSnowboarder(snow2);
  
  TerrainQuad* snow3 = slopePiece4_1(topRight, 0.0);
  topRight = snow3->topRight - endpoint;
  createSnowboarder(snow3);
  
  TerrainQuad* snow4 = slopePiece4_1(topRight, 0.0);
  createSnowboarder(snow4);
}

void PizzaLevel::createSkiFallbreakChunk()
{
  Point1 yHandle = 128.0;
  
  slopePiece3_1(Point2(0.0, yHandle), yHandle);
  createFlag(Point2(192.0, 192.0), false, yHandle);
  createFlag(Point2(575.0, 322.0), false, yHandle);
  
  createCoin(Point2(164.0, 37.0), yHandle);
  createCoin(Point2(257.0, 65.0), yHandle);
  createCoin(Point2(348.0, 93.0), yHandle);
  createCoin(Point2(439.0, 120.0), yHandle);
  createCoin(Point2(530.0, 151.0), yHandle);
  createCoin(Point2(620.0, 180.0), yHandle);
  createCoin(Point2(709.0, 210.0), yHandle);
  createCoin(Point2(789.0, 260.0), yHandle);
  createCoin(Point2(860.0, 322.0), yHandle);
  createCoin(Point2(921.0, 393.0), yHandle);
  createCoin(Point2(982.0, 474.0), yHandle);
  createCoin(Point2(1034.0, 555.0), yHandle);
  createCoin(Point2(1083.0, 636.0), yHandle);
  createCoin(Point2(1127.0, 722.0), yHandle);
  
  Point2 topRight = slopePiece3_1(Point2(624.0, 704.0), yHandle)->topRight - endpoint;
  topRight = slopePieceBreakaway(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* last = slopePiece2_1(topRight, 0.0);
  
  createFlag(Point2(2063.0, 1223.0), false, yHandle);
  createSnowboarder(last);
  
  createCoin(Point2(1193.0, 661.0), yHandle);
  createCoin(Point2(1275.0, 608.0), yHandle);
  createCoin(Point2(1363.0, 574.0), yHandle);
  createCoin(Point2(1460.0, 568.0), yHandle);
  createCoin(Point2(1549.0, 600.0), yHandle);
  createCoin(Point2(1643.0, 649.0), yHandle);
  createCoin(Point2(1704.0, 715.0), yHandle);
  createCoin(Point2(1766.0, 785.0), yHandle);
  createCoin(Point2(1826.0, 863.0), yHandle);
  createCoin(Point2(1877.0, 944.0), yHandle);
  createCoin(Point2(1922.0, 1027.0), yHandle);
}

void PizzaLevel::createSkiEndChunk()
{
  Point1 yHandle = 64.0;
  
  Point2 topRight = slopePiece2_1(Point2(0.0, 0.0), 0.0)->topRight - endpoint;
  topRight = slopePiece4_1(topRight, 0.0)->topRight - endpoint;
  TerrainQuad* currQuad = slopePieceFlatLong(topRight, 0.0);
  slopePieceFlatLong(currQuad->topRight - endpoint, 0.0)->topRight - endpoint;
  
  createSnowboarder(currQuad);
  
  createFlag(Point2(290.0, 208.0), false, yHandle);
  createFlag(Point2(656.0, 352.0), false, yHandle);
  createFlag(Point2(800.0, 384.0), false, yHandle);
  createFlag(Point2(944.0, 416.0), false, yHandle);
  createFlag(Point2(1280.0, 448.0), true, yHandle);
  
  Snowman* snow = new Snowman();
  snow->setXY(Point2(1934.0, 448.0 - yHandle) + endpoint);
  addEnemy(snow);
}

void PizzaLevel::spikeStartChunk()
{
  Point1 yHandle = 320.0;
  
  Point2 topRight = spikePieceNospike(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceNospike(topRight)->topRight - endpoint;
  topRight = spikePieceNospike(topRight + Point2(0.0, 32.0))->topRight - endpoint;
  
  spikePiece2x2(Point2(512.0, 192.0), yHandle);
  spikeBalanceBall(Point2(720.0, 256.0), yHandle);
}

void PizzaLevel::spikeBumpChunk()
{
  const Coord1 NUM_TOPS = 7;
  Point2 topPoints[NUM_TOPS] =
  {
    Point2(0.0, 0.0),
    Point2(128.0, 0.0),
    Point2(192.0, -48.0),
    Point2(256.0, -64.0),
    Point2(320.0, -48.0),
    Point2(384.0, 0.0),
    Point2(512.0, 0.0)
  };
  
  Image* imgSet[NUM_TOPS - 1] =
  {
    imgCache[balancePieceShort],
    imgCache[balancePieceUphill4_3],
    imgCache[balancePieceUphill4_1],
    imgCache[balancePieceDownhill4_1],
    imgCache[balancePieceDownhill4_3],
    imgCache[balancePieceShort]
  };
  
  for (Coord1 i = 0; i < NUM_TOPS - 1; ++i)
  {
    createSlopeQuad(topPoints[i], Point2(topPoints[i].x, topPoints[i].y + 64.0),
                    Point2(topPoints[i+1].x, topPoints[i+1].y + 64.0), topPoints[i+1],
                    imgSet[i]);
  }
  
  Point1 yHandle = 512.0;
  createCoin(Point2(120.0, 240.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(160.0, 154.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(225.0, 87.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(310.0, 134.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(355.0, 225.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(386.0, 306.0), yHandle, PizzaGO::NICKEL);
}

void PizzaLevel::spikeDownhillChunk()
{
  const Coord1 NUM_TOPS = 4;
  Point2 topPoints[NUM_TOPS] =
  {
    Point2(0.0, 0.0),
    Point2(128.0, 16.0),
    Point2(320.0, 80.0),
    Point2(448.0, 96.0)
  };
  
  Image* imgSet[NUM_TOPS - 1] =
  {
    imgCache[balancePieceDownhill8_1],
    imgCache[balancePieceDownhill12_4],
    imgCache[balancePieceDownhill8_1]
  };
  
  for (Coord1 i = 0; i < NUM_TOPS - 1; ++i)
  {
    createSlopeQuad(topPoints[i], Point2(topPoints[i].x, topPoints[i].y + 64.0),
                    Point2(topPoints[i+1].x, topPoints[i+1].y + 64.0), topPoints[i+1],
                    imgSet[i]);
  }
}

void PizzaLevel::spikeDropChunk()
{
  createSlopeQuad(Point2(0.0, 0.0), Point2(0.0, 64.0),
                  Point2(128.0, 64.0), Point2(128.0, 0.0),
                  imgCache[balancePieceShort]);
  createSlopeQuad(Point2(128.0, 0.0), Point2(128.0, 64.0),
                  Point2(192.0, 48.0), Point2(192.0, -16.0),
                  imgCache[balancePieceUphill4_1]);
  
  createSlopeQuad(Point2(192.0, 80.0), Point2(192.0, 144.0),
                  Point2(448.0, 208.0), Point2(448.0, 144.0),
                  imgCache[balancePieceDownhill16_4]);
  
  Point1 yHandle = 320.0;
  createCoin(Point2(159.0, 26.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(236.0, 74.0), yHandle, PizzaGO::DIME);
  createCoin(Point2(282.0, 158.0), yHandle, PizzaGO::NICKEL);
}

void PizzaLevel::spikeFlatChunk()
{
  createSlopeQuad(Point2(0.0, 0.0), Point2(0.0, 64.0),
                  Point2(512.0, 64.0), Point2(512.0, 0.0),
                  imgCache[balancePieceFlatlong]);
}

void PizzaLevel::spikeUphillChunk()
{
  const Coord1 NUM_TOPS = 4;
  Point2 topPoints[NUM_TOPS] =
  {
    Point2(0.0, 0.0),
    Point2(128.0, -16.0),
    Point2(256.0, -64.0),
    Point2(448.0, -96.0)
  };
  
  Image* imgSet[NUM_TOPS - 1] =
  {
    imgCache[balancePieceUphill8_1],
    imgCache[balancePieceUphill8_3],
    imgCache[balancePieceUphill12_2]
  };
  
  for (Coord1 i = 0; i < NUM_TOPS - 1; ++i)
  {
    createSlopeQuad(topPoints[i], Point2(topPoints[i].x, topPoints[i].y + 64.0),
                    Point2(topPoints[i+1].x, topPoints[i+1].y + 64.0), topPoints[i+1],
                    imgSet[i]);
  }
}

void PizzaLevel::spikeBouncecirclesChunk()
{
  Point1 yhandle = 192.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceUphill8_1(Point2(0.0, yhandle), yhandle)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceUphill4_3(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceUphill4_3(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight, 0.0)->topRight - endpoint;
  topRight = spikePieceDownhill8_1(topRight, 0.0)->topRight - endpoint;
  
  spikePieceBounceCircle(Point2(572.0, 65.0), yhandle);
  spikePieceBounceCircle(Point2(767.0, 32.0), yhandle);
  spikePieceBounceCircle(Point2(960.0, 65.0), yhandle);
}

void PizzaLevel::spikeBouncedropsChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(Point2(topRight.x, topRight.y + 160.0))->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(Point2(topRight.x, topRight.y + 160.0))->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
  topRight = spikePieceUphill8_3(topRight)->topRight - endpoint;
}

void PizzaLevel::spikeLongfallChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceUphill16_4(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  
  topRight = spikePieceUphill16_4(Point2(topRight.x, topRight.y + 384.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  
  Point1 yHandle = 448.0;
  createCoin(Point2(522.0, 59.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(604.0, 106.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(651.0, 188.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(685.0, 276.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(711.0, 373.0), yHandle, PizzaGO::QUARTER);
}

void PizzaLevel::spikeNospikeChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  
  spikePieceNospike(Point2(512.0, -304.0));
}

void PizzaLevel::spikeRiserChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceRiser(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
}

void PizzaLevel::spikePillarChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePiecePillar(Point2(32.0, 0.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, 0.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, 32.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, 16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, -16.0))->topRight - endpoint;
  topRight = spikePiecePillar(topRight + Point2(32.0, 16.0))->topRight - endpoint;
  
  Point1 yHandle = 352.0;
  createCoin(Point2(60.0, 94.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(152.0, 78.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(246.0, 60.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(342.0, 44.0), yHandle, PizzaGO::DIME);
  
  createCoin(Point2(538.0, 107.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(630.0, 89.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(724.0, 72.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(818.0, 59.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(922.0, 38.0), yHandle, PizzaGO::DIME);
}

void PizzaLevel::spikeSpikeballChunk()
{
  Point1 yHandle = 576.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatlong(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
  
  slopeSpikeball(Point2(560.0, 378.0), yHandle);
}

void PizzaLevel::spikeSpringChunk()
{
  Point1 yHandle = 256.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  PlatformRiser* riser = spikePieceRiser(topRight);
  riser->targetY = riser->getY() - 192.0;
  
  topRight = spikePieceFlatmed(Point2(576.0, 64.0), yHandle)->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
}

void PizzaLevel::spikeStraightbounceChunk(Coord1 variationID)
{
  Point1 yHandle = 192.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceUphill16_4(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(Point2(576.0, 320.0), yHandle)->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
  
  if (variationID == 1)
  {
    tryCreateUnlockable(Point2(1232.0, 272.0 - yHandle) + endpoint, Coord2(0, 11));
  }
}

void PizzaLevel::spikeTransferblockChunk()
{
  Point1 yHandle = 240.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceUphill8_3(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  topRight = spikePieceUphill12_2(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  
  spikePiece2x2(Point2(512.0, 64.0), yHandle);
  spikeBalanceBall(Point2(736.0, 176.0), yHandle);
}

void PizzaLevel::spikeTransferdropChunk()
{
  Point1 yHandle = 192.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatlong(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight)->topRight - endpoint;
  
  topRight = spikePieceFlatmed(topRight + Point2(64.0, -160.0))->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  
  spikeBalanceBall(Point2(768.0, 56.0), yHandle);
}

void PizzaLevel::spikeTransferspikeChunk()
{
  Point1 yHandle = 192.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatlong(Point2(0.0, 0.0))->topRight - endpoint;
  
  slopeSpikeball(Point2(578.0, 100.0), yHandle);
  spikeBalanceBall(Point2(642.0, -64.0), yHandle);
  slopeSpikeball(Point2(706.0, 100.0), yHandle);
  
  topRight = spikePieceDownhill12_4(Point2(768.0, 64.0), yHandle)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
}

void PizzaLevel::spikeBouncestepsChunk()
{
  Point1 yHandle = 64.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatlong(Point2(0.0, 0.0))->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(topRight + Point2(0.0, 128.0))->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(topRight + Point2(0.0, 128.0))->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  
  topRight = spikePieceBouncePlatform(topRight + Point2(0.0, 128.0))->topRight - endpoint;
  topRight = spikePieceBouncePlatform(topRight)->topRight - endpoint;
  
  topRight = spikePieceFlatlong(topRight + Point2(0.0, 128.0))->topRight - endpoint;
}

void PizzaLevel::spikeDownhillbumpChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatshort(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceFlatshort(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = spikePieceUphill4_3(topRight)->topRight - endpoint;
  topRight = spikePieceFlatshort(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatshort(topRight)->topRight - endpoint;
  
  Point1 yHandle = 352.0;
  createCoin(Point2(333.0, 35.0), yHandle, PizzaGO::PENNY);
  createCoin(Point2(426.0, 75.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(518.0, 104.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(607.0, 132.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(697.0, 167.0), yHandle, PizzaGO::DIME);
  
  createCoin(Point2(923.0, 123.0), yHandle, PizzaGO::PENNY);
  createCoin(Point2(1012.0, 160.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(1103.0, 187.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(1197.0, 219.0), yHandle, PizzaGO::NICKEL);
  createCoin(Point2(1285.0, 252.0), yHandle, PizzaGO::DIME);
}

void PizzaLevel::spikeDualriseChunk()
{
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceRiser(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceDownhill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceRiser(topRight)->topRight - endpoint;
  
  topRight = spikePieceUphill8_3(topRight + Point2(0.0, -128.0))->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  
  Point1 yHandle = 448.0;
  createCoin(Point2(423.0, 33.0), yHandle, PizzaGO::DIME);
  createCoin(Point2(482.0, 109.0), yHandle, PizzaGO::DIME);
  createCoin(Point2(542.0, 180.0), yHandle, PizzaGO::DIME);
  createCoin(Point2(598.0, 260.0), yHandle, PizzaGO::DIME);
}

void PizzaLevel::spikeRiseballsChunk()
{
  Point1 yHandle = 192.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceUphill16_4(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceRiser(topRight)->topRight - endpoint;
  
  slopeSpikeball(Point2(640.0, 160.0), yHandle);
  slopeSpikeball(Point2(768.0, 160.0), yHandle);
  
  topRight = spikePieceRiser(topRight + Point2(256.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
}

void PizzaLevel::spikeRisestepsChunk()
{
  Point1 yHandle = 448.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceRiser(topRight)->topRight - endpoint;
  
  topRight = spikePieceRiser(topRight + Point2(0.0, -128.0))->topRight - endpoint;
  
  topRight = spikePieceRiser(topRight + Point2(0.0, -128.0))->topRight - endpoint;
  
  topRight = spikePieceFlatmed(topRight + Point2(0.0, -128.0))->topRight - endpoint;
}

void PizzaLevel::spikeBalldownhillChunk()
{
  Point1 yHandle = 64.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  
  topRight = slopeSpikeball(topRight + 64.0, 0.0)->getXY() - endpoint;
  topRight = slopeSpikeball(topRight + Point2(128.0, 48.0), 0.0)->getXY() - endpoint;
  topRight = slopeSpikeball(topRight + Point2(128.0, 48.0), 0.0)->getXY() - endpoint;
  
  topRight = spikePieceDownhill12_4(topRight + Point2(64.0, 0.0))->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatlong(topRight)->topRight - endpoint;
  
  Point1 yCoinHandle = 448.0;
  createCoin(Point2(203.0, 181.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(246.0, 92.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(317.0, 27.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(399.0, 78.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(443.0, 163.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(475.0, 249.0), yCoinHandle, PizzaGO::NICKEL);
  
  createCoin(Point2(595.0, 346.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(636.0, 259.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(707.0, 194.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(789.0, 246.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(833.0, 327.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(864.0, 414.0), yCoinHandle, PizzaGO::NICKEL);
  
}

void PizzaLevel::spikeBalljumpChunk()
{
  Point1 yHandle = 64.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceDownhill12_4(topRight)->topRight - endpoint;
  topRight = slopeSpikeball(topRight + Point2(64.0, 48.0), 0.0)->getXY() - endpoint;
  topRight = slopeSpikeball(topRight + Point2(128.0, -32.0), 0.0)->getXY() - endpoint;
  topRight = slopeSpikeball(topRight + Point2(128.0, -32.0), 0.0)->getXY() - endpoint;
  topRight = spikePieceFlatlong(topRight + Point2(64.0, 144.0), 0.0)->getXY() - endpoint;
  
  Point1 yCoinHandle = 320.0;
  createCoin(Point2(445.0, 116.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(537.0, 93.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(632.0, 72.0), yCoinHandle, PizzaGO::DIME);
  createCoin(Point2(724.0, 52.0), yCoinHandle, PizzaGO::QUARTER);
  createCoin(Point2(814.0, 32.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(898.0, 83.0), yCoinHandle, PizzaGO::NICKEL);
  createCoin(Point2(943.0, 168.0), yCoinHandle, PizzaGO::DIME);
  createCoin(Point2(977.0, 257.0), yCoinHandle, PizzaGO::QUARTER);
}

void PizzaLevel::spikeZigzagChunk()
{
  Point1 yHandle = 128.0;
  Point2 topRight(0.0, 0.0);
  
  topRight = spikePieceFlatmed(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceUphill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceDownhill16_4(topRight)->topRight - endpoint;
  topRight = spikePieceFlatmed(topRight)->topRight - endpoint;
  
  Point1 yCoinHandle = 576.0;
  createCoin(Point2(507.0, 242.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(547.0, 154.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(616.0, 93.0), yCoinHandle, PizzaGO::DIME);
  createCoin(Point2(699.0, 139.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(744.0, 224.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(776.0, 311.0), yCoinHandle, PizzaGO::PENNY);
  
  createCoin(Point2(1033.0, 242.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1074.0, 155.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1144.0, 91.0), yCoinHandle, PizzaGO::DIME);
  createCoin(Point2(1227.0, 136.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1272.0, 223.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1304.0, 311.0), yCoinHandle, PizzaGO::PENNY);
  
  createCoin(Point2(1525.0, 241.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1565.0, 154.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1635.0, 90.0), yCoinHandle, PizzaGO::DIME);
  createCoin(Point2(1717.0, 136.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1762.0, 225.0), yCoinHandle, PizzaGO::PENNY);
  createCoin(Point2(1794.0, 312.0), yCoinHandle, PizzaGO::PENNY);
}

void PizzaLevel::spikeEndChunk()
{
  Point1 yHandle = 256.0;
  
  Point2 topRight = spikePieceNospike(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = spikePieceNospike(topRight + Point2(0.0, 32.0))->topRight - endpoint;
  topRight = spikePieceNospike(topRight)->topRight - endpoint;
  
  BalanceGong* gong = new BalanceGong();
  gong->setXY(endpoint + Point2(880.0, 82.0 - yHandle));
  addBGTerrain(gong);
}

TerrainQuad* PizzaLevel::createSlopeQuad(const Point2& pt0, const Point2& pt1,
                                         const Point2& pt2, const Point2& pt3,
                                         Image* img)
{
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pt0 + endpoint, pt1 + endpoint,
                                                      pt2 + endpoint, pt3 + endpoint);
  quad->setImage(img);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::createSumoQuad(const Point2& pt0, const Point2& pt1,
                                        const Point2& pt2, const Point2& pt3,
                                        Image* img)
{
  TerrainQuad* quad = TerrainQuad::create_static_quad(pt0 + endpoint, pt1 + endpoint,
                                                      pt2 + endpoint, pt3 + endpoint);
  quad->setImage(img);
  addBGTerrain(quad);
  return quad;
}

// calculates the other corners based on image size and handle, assumes handle is at top left
TerrainQuad* PizzaLevel::createPuppyQuad(const Point2& topLeft, Point1 imgTopRightY,
                                         Image* img, Point1 layoutHandleY)
{
  Point2 finalTL = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      finalTL,
                                                      finalTL + Point2(0.0, img->natural_size().y - img->handle.y),
                                                      finalTL + img->natural_size() - img->handle,
                                                      finalTL + Point2(img->natural_size().x - img->handle.x, imgTopRightY - img->handle.y));
  quad->setImage(img);
  addBGTerrain(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePiece4_1(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 128.0) + pointOffset,
                                      Point2(576.0, 256.0) + pointOffset, Point2(576.0, 128.0) + pointOffset,
                                      imgCache[slope4_1]);
  return quad;
}

TerrainQuad* PizzaLevel::slopePiece3_1(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 128.0) + pointOffset,
                                      Point2(576.0, 320.0) + pointOffset, Point2(576.0, 192.0) + pointOffset,
                                      imgCache[slope3_1]);
  return quad;
}

TerrainQuad* PizzaLevel::slopePiece2_1(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 128.0) + pointOffset,
                                      Point2(512.0, 384.0) + pointOffset, Point2(512.0, 256.0) + pointOffset,
                                      imgCache[slope2_1]);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceBreakaway(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(576.0, 320.0) + pointOffset, Point2(576.0, 192.0) + pointOffset);
  quad->flags |= PizzaGO::BREAKAWAY_BIT;
  quad->setImage(imgCache[slope3_1_break]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePiece3_1Short(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 128.0) + pointOffset,
                                      Point2(192.0, 192.0) + pointOffset, Point2(192.0, 64.0) + pointOffset,
                                      imgCache[slope3_1_short]);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceBouncy(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(576.0, 256.0) + pointOffset, Point2(576.0, 128.0) + pointOffset);
  // quad->set_restitution(0.75);
  quad->setBouncy(0.75);
  quad->setImage(imgCache[slopeBouncy]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceFlatLong(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(640.0, 128.0) + pointOffset, Point2(640.0, 0.0) + pointOffset);
  quad->setImage(imgCache[slopeFlatLong]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceFlatShort(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(256.0, 128.0) + pointOffset, Point2(256.0, 0.0) + pointOffset);
  quad->setImage(imgCache[slopeFlatShort]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceSpikePlatform(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(512.0, 384.0) + pointOffset, Point2(512.0, 256.0) + pointOffset);
  quad->setImage(imgCache[slopeSpikePlatform]);
  quad->flags |= PizzaGO::SPIKED_BIT;
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceUphill(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 128.0) + pointOffset,
                                                      Point2(320.0, 48.0) + pointOffset, Point2(320.0, -80.0) + pointOffset);
  quad->setImage(imgCache[slopeUphill]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::slopePieceTriangle(const Point2& topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(120.0, 144.0) + pointOffset,
                                                      Point2(132.0, 144.0) + pointOffset, Point2(256.0, 0.0) + pointOffset);
  quad->setImage(imgCache[slopeTriangle]);
  addAlly(quad);
  return quad;
}

TerrainCircle* PizzaLevel::slopeSpikeball(const Point2& center, Point1 layoutHandleY)
{
  TerrainCircle* spikeBall = new TerrainCircle(imgCache[spikeball]);
  spikeBall->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  spikeBall->flags |= PizzaGO::SPIKED_BIT;
  addAlly(spikeBall);
  return spikeBall;
}

SkullSwitch* PizzaLevel::slopeSwitch(const Point2& center, Point1 layoutHandleY)
{
  SkullSwitch* spikeSwitch = new SkullSwitch();
  spikeSwitch->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  addAlly(spikeSwitch);
  return spikeSwitch;
}

SkiFlameSkull* PizzaLevel::slopeFlameskull(const Point2& center, Point1 layoutHandleY)
{
  SkiFlameSkull* skull = new SkiFlameSkull();
  skull->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  addAlly(skull);
  return skull;
}

SnowboardSkele* PizzaLevel::createSnowboarder(TerrainQuad* quad)
{
  SnowboardSkele* skele = new SnowboardSkele(quad->getXY(), quad->topRight);
  addEnemy(skele);
  return skele;
}

void PizzaLevel::createCoin(Point2 center, Point1 layoutYHandle)
{
  createCoin(center, layoutYHandle, PizzaGO::PENNY);
}

void PizzaLevel::createCoin(Point2 center, Point1 layoutYHandle, Coord1 val)
{
  coinsCreated++;
  // cout << "createCoin " << coinsCreated << endl;
  SlopeCoin* coin = new SlopeCoin(center + endpoint + Point2(0.0, -layoutYHandle), val);
  addAlly(coin);
}

void PizzaLevel::createFlag(Point2 location, Logical big, Point1 layoutHandleY)
{
  SlopeFlag* flag = new SlopeFlag(location + endpoint + Point2(0.0, -layoutHandleY), big);
  addEnemy(flag);
}

PizzaGO* PizzaLevel::createMeat(Point2 center)
{
  Coord1 junkType = RM::randi(0, 2);
  
  CircleDebris* meat = new CircleDebris(imgsetCache[junkSet][junkType]);
  meat->basePoints = 0;
  meat->setXY(center);
  
  if (junkType == 0) meat->setWH(12.0, 12.0);
  if (junkType == 1) meat->setWH(18.0, 18.0);
  if (junkType == 2) meat->setWH(13.0, 13.0);
  
  addEnemy(meat);
  return meat;
}

TerrainQuad* PizzaLevel::spikePiece2x2(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 128.0) + pointOffset,
                                      Point2(128.0, 128.0) + pointOffset, Point2(128.0, 0.0) + pointOffset,
                                      imgCache[balancePiece2x2]);
  quad->flags = 0x0;
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceBounceCircle(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = endpoint + topLeft + Point2(0.0, -layoutHandleY);
  // TerrainCircle* circle = new TerrainCircle();
  TerrainQuad* circle = TerrainQuad::create_static_circle(pointOffset,
                                                          imgCache[balancePieceBounceCircle]->natural_size().x * 0.5,
                                                          imgCache[balancePieceBounceCircle]);
  // circle->setImage(imgCache[balancePieceBounceCircle]);
  // circle->setXY(pointOffset);
  addAlly(circle);
  
  circle->flags &= ~PizzaGO::SPIKED_BIT;
  circle->setBouncy(0.75);
  return circle;
}

TerrainQuad* PizzaLevel::spikePieceBouncePlatform(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(192.0, 64.0) + pointOffset, Point2(192.0, 0.0) + pointOffset,
                                      imgCache[balancePieceBouncePlatform]);
  quad->flags &= ~PizzaGO::SPIKED_BIT;
  quad->flags |= PizzaGO::BALANCE_BOUNCE_BIT;
  // quad->set_restitution(0.9);
  quad->setBouncy(0.9);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceDownhill4_1(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(64.0, 80.0) + pointOffset, Point2(64.0, 16.0) + pointOffset,
                                      imgCache[balancePieceDownhill4_1]);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceDownhill4_3(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(64.0, 112.0) + pointOffset, Point2(64.0, 48.0) + pointOffset,
                                      imgCache[balancePieceDownhill4_3]);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceDownhill8_1(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(128.0, 80.0) + pointOffset, Point2(128.0, 16.0) + pointOffset,
                                      imgCache[balancePieceDownhill8_1]);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceDownhill12_4(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(192.0, 128.0) + pointOffset, Point2(192.0, 64.0) + pointOffset,
                                      imgCache[balancePieceDownhill12_4]);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceDownhill16_4(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(256.0, 128.0) + pointOffset, Point2(256.0, 64.0) + pointOffset,
                                      imgCache[balancePieceDownhill16_4]);
  return quad;
}

TerrainQuad* PizzaLevel::spikePieceFlatlong(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(512.0, 64.0) + pointOffset, Point2(512.0, 0.0) + pointOffset,
                         imgCache[balancePieceFlatlong]);
}

TerrainQuad* PizzaLevel::spikePieceFlatmed(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(256.0, 64.0) + pointOffset, Point2(256.0, 0.0) + pointOffset,
                         imgCache[balancePieceFlatmed]);
}

TerrainQuad* PizzaLevel::spikePieceFlatshort(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(128.0, 64.0) + pointOffset, Point2(128.0, 0.0) + pointOffset,
                         imgCache[balancePieceShort]);
}

TerrainQuad* PizzaLevel::spikePieceNospike(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                                      Point2(512.0, 64.0) + pointOffset, Point2(512.0, 0.0) + pointOffset,
                                      imgCache[balancePieceNospike]);
  quad->flags &= ~PizzaGO::SPIKED_BIT;
  return quad;
}

PlatformRiser* PizzaLevel::spikePieceRiser(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  PlatformRiser* quad = TerrainQuad::create_static_riser(
                                                         pointOffset + endpoint, pointOffset + Point2(0.0, 64.0) + endpoint,
                                                         pointOffset + Point2(320.0, 64.0) + endpoint, pointOffset + Point2(320.0, 0.0) + endpoint);
  quad->targetY = quad->getY() - 128.0;
  quad->setImage(imgCache[balancePieceRiser]);
  addAlly(quad);
  return quad;
}

PlatformRiser* PizzaLevel::spikePieceRiserSmall(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  PlatformRiser* quad = TerrainQuad::create_static_riser(
                                                         pointOffset + endpoint, pointOffset + Point2(0.0, 128.0) + endpoint,
                                                         pointOffset + Point2(128.0, 128.0) + endpoint, pointOffset + Point2(128.0, 0.0) + endpoint);
  quad->targetY = quad->getY() - 192;
  quad->setImage(imgCache[balancePieceRiseBlock]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::spikePiecePillar(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 384.0) + pointOffset,
                         Point2(64.0, 384.0) + pointOffset, Point2(64.0, 0.0) + pointOffset,
                         imgCache[balancePiecePillar]);
}

TerrainQuad* PizzaLevel::spikePieceUphill4_1(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(64.0, 48.0) + pointOffset, Point2(64.0, -16.0) + pointOffset,
                         imgCache[balancePieceUphill4_1]);
}

TerrainQuad* PizzaLevel::spikePieceUphill4_3(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(64.0, 16.0) + pointOffset, Point2(64.0, -48.0) + pointOffset,
                         imgCache[balancePieceUphill4_3]);
}

TerrainQuad* PizzaLevel::spikePieceUphill8_1(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(128.0, 48.0) + pointOffset, Point2(128.0, -16.0) + pointOffset,
                         imgCache[balancePieceUphill8_1]);
}

TerrainQuad* PizzaLevel::spikePieceUphill8_3(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(128.0, 16.0) + pointOffset, Point2(128.0, -48.0) + pointOffset,
                         imgCache[balancePieceUphill8_3]);
}

TerrainQuad* PizzaLevel::spikePieceUphill12_2(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(192.0, 32.0) + pointOffset, Point2(192.0, -32.0) + pointOffset,
                         imgCache[balancePieceUphill12_2]);
}

TerrainQuad* PizzaLevel::spikePieceUphill16_4(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + Point2(0.0, -layoutHandleY);
  return createSlopeQuad(pointOffset, Point2(0.0, 64.0) + pointOffset,
                         Point2(256.0, 0.0) + pointOffset, Point2(256.0, -64.0) + pointOffset,
                         imgCache[balancePieceUphill16_4]);
}

BalanceSkull* PizzaLevel::spikeBalanceBall(Point2 center, Point1 layoutHandleY)
{
  BalanceSkull* ball = new BalanceSkull();
  
  ball->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  ball->octagon = useOctBall;
  ball->value = ballList.count;
  ballList.add(ball);
  addAlly(ball);
  return ball;
}

TerrainQuad* PizzaLevel::puppyPieceBounce(Point2 topLeft, Point1 layoutHandleY)
{
  Point2 pointOffset = topLeft + endpoint + Point2(0.0, -layoutHandleY);
  TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                      pointOffset, Point2(0.0, 64.0) + pointOffset,
                                                      Point2(256.0, 64.0) + pointOffset, Point2(256.0, 0.0) + pointOffset);
  quad->setBouncy(0.75);
  quad->setImage(imgCache[puppyBounce16]);
  addAlly(quad);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceDownhill4_2(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 96.0,
                                      imgCache[puppyDownhill4_2], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceDownhill4_4(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 128.0,
                                      imgCache[puppyDownhill4_4], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceDownhill8_2(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 96.0,
                                      imgCache[puppyDownhill8_2], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceDownhill12_8(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 192.0,
                                      imgCache[puppyDownhill12_8], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceFlat8(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyFlat8], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceFlat8(Point2 topLeft,
                                         Logical leftCorner, Coord1 leftEdges,
                                         Coord1 rightEdges, Logical rightCorner,
                                         Point1 layoutHandleY)
{
  TerrainQuad* quad = puppyPieceFlat8(topLeft, layoutHandleY);
  quad->setPuppyEdges(leftEdges, rightEdges);
  quad->setPuppyCorners(leftCorner, rightCorner);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceFlat16(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyFlat16], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceFlat16(Point2 topLeft,
                                          Logical leftCorner, Coord1 leftEdges,
                                          Coord1 rightEdges, Logical rightCorner,
                                          Point1 layoutHandleY)
{
  TerrainQuad* quad = puppyPieceFlat16(topLeft, layoutHandleY);
  quad->setPuppyEdges(leftEdges, rightEdges);
  quad->setPuppyCorners(leftCorner, rightCorner);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceSpike(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 32.0,
                                      imgCache[puppySpikes256], layoutHandleY);
  quad->flags |= PizzaGO::SPIKED_BIT;
  return quad;
}

SquarePivot* PizzaLevel::puppyPiecePivotsquare(Point2 center, Point1 layoutHandleY)
{
  SquarePivot* sq = new SquarePivot();
  sq->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  addEnemy(sq);
  return sq;
}

TrianglePivot* PizzaLevel::puppyPiecePivottri(Point2 pivot, Point1 layoutHandleY)
{
  TrianglePivot* tri = new TrianglePivot();
  tri->setXY(pivot + endpoint + Point2(0.0, -layoutHandleY));
  addEnemy(tri);
  return tri;
}

TerrainQuad* PizzaLevel::puppyPieceUphill4_2(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyUphill4_2], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceUphill4_4(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyUphill4_4], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceUphill8_2(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyUphill8_2], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceUphill12_8(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 64.0,
                                      imgCache[puppyUphill12_8], layoutHandleY);
  return quad;
}

TerrainQuad* PizzaLevel::puppyPieceWall(Point2 topLeft, Point1 layoutHandleY)
{
  TerrainQuad* quad = createPuppyQuad(topLeft, 0.0,
                                      imgCache[puppyWall], layoutHandleY);
  return quad;
}


void PizzaLevel::puppyCrateChunk()
{
  Point1 yHandle = 288.0;
  
  Point2 topRight = puppyPieceUphill8_2(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  
  createPuppyObj(FAVOR_CRATE, Point2(224.0, 256.0), yHandle);
  
  createPuppyObj(FAVOR_CRATE, Point2(240.0, 192.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(337.0, 192.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(434.0, 192.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(498.0, 192.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(562.0, 192.0), yHandle);
  
  createPuppyObj(FAVOR_CRATE, Point2(224.0, 128.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(288.0, 128.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(384.0, 128.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(448.0, 128.0), yHandle);
  createPuppyObj(FAVOR_CRATE, Point2(544.0, 128.0), yHandle);
  
  createPuppyObj(FAVOR_CRATE, Point2(400.0, 64.0), yHandle);
}

void PizzaLevel::puppyDipChunk()
{
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill4_2(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
}

void PizzaLevel::puppyDoghouseChunk()
{
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  // doghouse back
  TerrainQuad* houseBack = TerrainQuad::create_static_quad(endpoint + Point2(480.0, -128.0),
                                                           endpoint + Point2(416.0, -64.0),
                                                           endpoint + Point2(640.0, -64.0),
                                                           endpoint + Point2(576.0, -128.0));
  houseBack->setImage(imgCache[puppyDoghouseBack]);
  addEnemy(houseBack);
  // doghouse middle collision, world will clean these up
  /*
   b2Body* houseBody = Physical::create_body(Point2(432.0, -64.0) + endpoint, false);
   Physical::create_quad_fixture(houseBody, Point2(0.0, 0.0), Point2(0.0, 64.0),
   Point2(192.0, 64.0), Point2(192.0, 0.0));
   */
  // addAction(new Decoration(endpoint + Point2(480.0, -128.0), imgCache[puppyDoghouseFront]));
  Point2 houseMidOff = Point2(432.0, -64.0);
  createSlopeQuad(houseMidOff, houseMidOff + Point2(0.0, 64.0),
                  houseMidOff + Point2(192.0, 64.0), houseMidOff + Point2(192.0, 0.0),
                  imgCache[puppyDoghouseFront]);
  /*
   Point2 houseMidOff = Point2(432.0, -64.0) + endpoint;
   TerrainQuad* front = TerrainQuad::create_static_quad(houseMidOff, houseMidOff + Point2(0.0, 64.0),
   houseMidOff + Point2(192.0, 64.0), houseMidOff + Point2(192.0, 0.0));
   front->setImage(imgCache[puppyDoghouseFront]);
   addAlly(front);
   */
  
  
  puppyHousePt = endpoint + Point2(480.0, 0.0);
  puppyStartPt = endpoint + Point2(732.0, 0.0);
}

void PizzaLevel::puppyHillChunk(Coord1 variation)
{
  Point1 yHandle = 128.0;
  
  Point2 topRight = puppyPieceFlat8(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  
  createPuppyObj(variation, Point2(576.0, 96.0), yHandle);
}

void PizzaLevel::puppySpikeHillChunk(Coord1 variation)
{
  Point1 yHandle = 448.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceUphill12_8(topRight);
  currQuad->setPuppyEdges(0, 2);
  
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint + Point2(32.0, 160.0),
                              true, 0, 0, true)->topRight - endpoint;
  
  topRight = puppyPieceFlat16(topRight + Point2(32.0, -160.0), false, 2, 0, false)->topRight - endpoint;
  topRight = puppyPieceUphill12_8(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill12_8(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill12_8(topRight)->topRight - endpoint;
  
  createCoin(Point2(376.0, 197.0), yHandle);
  createCoin(Point2(448.0, 138.0), yHandle);
  createCoin(Point2(531.0, 88.0), yHandle);
  createCoin(Point2(623.0, 54.0), yHandle);
  createCoin(Point2(716.0, 43.0), yHandle);
  createCoin(Point2(811.0, 59.0), yHandle);
  createCoin(Point2(899.0, 98.0), yHandle);
  createCoin(Point2(975.0, 151.0), yHandle);
  createCoin(Point2(1048.0, 215.0), yHandle);
  
  puppyPieceSpike(Point2(480.0, 336.0), yHandle);
  
  createPuppyObj(variation, Point2(1344.0, 192.0), yHandle);
}

void PizzaLevel::puppySpikeJumpChunk()
{
  Point1 yHandle = 448.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceUphill12_8(topRight);
  currQuad->setPuppyEdges(0, 3);
  
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint + Point2(32.0, 160.0),
                              true, 0, 0, true)->topRight - endpoint;
  
  currQuad = puppyPieceDownhill12_8(topRight + Point2(32.0, -160.0));
  currQuad->setPuppyEdges(3, 0);
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint)->topRight - endpoint;
  
  createCoin(Point2(376.0, 197.0), yHandle);
  createCoin(Point2(448.0, 138.0), yHandle);
  createCoin(Point2(531.0, 88.0), yHandle);
  createCoin(Point2(623.0, 54.0), yHandle);
  createCoin(Point2(716.0, 43.0), yHandle);
  createCoin(Point2(811.0, 59.0), yHandle);
  createCoin(Point2(899.0, 98.0), yHandle);
  createCoin(Point2(975.0, 151.0), yHandle);
  createCoin(Point2(1048.0, 215.0), yHandle);
  createCoin(Point2(1112.0, 286.0), yHandle);
  createCoin(Point2(1168.0, 360.0), yHandle);
  
  puppyPieceSpike(Point2(480.0, 336.0), yHandle);
}

void PizzaLevel::puppyTrickyJumpChunk()
{
  Point1 yHandle = 448.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceUphill12_8(topRight);
  currQuad->setPuppyEdges(0, 2);
  
  currQuad = puppyPieceFlat16(currQuad->topRight - endpoint + Point2(32.0, 160.0));
  currQuad->setPuppyCorners(true, false);
  
  currQuad = puppyPieceFlat16(currQuad->topRight - endpoint);
  currQuad->setPuppyCorners(false, true);
  
  currQuad = puppyPieceFlat8(currQuad->topRight - endpoint + Point2(32.0, -160.0));
  currQuad->setPuppyEdges(2, 2);
  
  currQuad = puppyPieceFlat16(currQuad->topRight - endpoint + Point2(32.0, 160.0));
  currQuad->setPuppyCorners(true, true);
  
  currQuad = puppyPieceFlat8(currQuad->topRight - endpoint + Point2(32.0, -160.0));
  currQuad->setPuppyEdges(2, 0);
  
  topRight = puppyPieceDownhill12_8(currQuad->topRight - endpoint)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(376.0, 197.0), yHandle);
  createCoin(Point2(448.0, 138.0), yHandle);
  createCoin(Point2(531.0, 88.0), yHandle);
  createCoin(Point2(623.0, 54.0), yHandle);
  createCoin(Point2(716.0, 43.0), yHandle);
  createCoin(Point2(811.0, 59.0), yHandle);
  createCoin(Point2(899.0, 98.0), yHandle);
  createCoin(Point2(975.0, 151.0), yHandle);
  createCoin(Point2(1048.0, 215.0), yHandle);
  
  puppyPieceSpike(Point2(480.0, 336.0), yHandle);
  puppyPieceSpike(Point2(736.0, 336.0), yHandle);
  
  createCoin(Point2(1147.0, 102.0), yHandle);
  createCoin(Point2(1223.0, 46.0), yHandle);
  createCoin(Point2(1312.0, 16.0), yHandle);
  createCoin(Point2(1408.0, 32.0), yHandle);
  createCoin(Point2(1488.0, 81.0), yHandle);
  createCoin(Point2(1558.0, 149.0), yHandle);
  createCoin(Point2(1611.0, 227.0), yHandle);
  
  puppyPieceSpike(Point2(1184.0, 336.0), yHandle);
}

void PizzaLevel::puppyLavahillChunk()
{
  Point1 yHandle = 256.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  puppyPieceLava(Point2(384.0, 192.0), true, yHandle);
  createFireball(Point2(448.0, 192.0), yHandle);
  
  puppyPieceLava(Point2(1024.0, 64.0), true, yHandle);
  createFireball(Point2(1088.0, 64.0), yHandle);
  
  puppyPieceLava(Point2(1664.0, 192.0), true, yHandle);
  createFireball(Point2(1728.0, 192.0), yHandle);
}

void PizzaLevel::puppyBigobjhillChunk(Coord1 variation)
{
  Point1 yHandle = 256.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  if (variation == FAVOR_CAGE)
  {
    createPuppyObj(FAVOR_CRATE, Point2(448.0, 192.0), yHandle);
    createPuppyObj(FAVOR_CRATE, Point2(768.0, 128.0), yHandle);
    createPuppyObj(FAVOR_CAGE, Point2(1088.0, 64.0), yHandle);
    createPuppyObj(FAVOR_CRATE, Point2(1408.0, 128.0), yHandle);
    createPuppyObj(FAVOR_CRATE, Point2(1728.0, 192.0), yHandle);
  }
  else
  {
    createPuppyObj(variation, Point2(448.0, 192.0), yHandle);
    createPuppyObj(variation, Point2(768.0, 128.0), yHandle);
    createPuppyObj(variation, Point2(1088.0, 64.0), yHandle);
    createPuppyObj(variation, Point2(1408.0, 128.0), yHandle);
    createPuppyObj(variation, Point2(1728.0, 192.0), yHandle);
  }
}

void PizzaLevel::puppyBouncepitChunk(Coord1 variation)
{
  Point1 yHandle = 224.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill12_8(topRight)->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceFlat8(topRight);
  currQuad->setPuppyEdges(0, 4);
  
  puppyPieceBounce(Point2(704.0, 196.0), yHandle);
  
  currQuad = puppyPieceFlat16(Point2(1088.0, 96.0), yHandle);
  currQuad->setPuppyEdges(4, 4);
  
  createPuppyObj(variation, Point2(1216.0, 96.0), yHandle);
  
  puppyPieceBounce(Point2(1472.0, 196.0), yHandle);
  
  currQuad = puppyPieceFlat8(Point2(1856.0, 96.0), yHandle);
  currQuad->setPuppyEdges(4, 0);
  
  topRight = puppyPieceDownhill12_8(currQuad->topRight - endpoint)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
}

void PizzaLevel::puppyCratepitChunk()
{
  Point1 yHandle = 192.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceFlat16(topRight);
  currQuad->setPuppyEdges(0, 2);
  
  createPuppyObj(FAVOR_CRATE, Point2(592.0, 0.0));
  createPuppyObj(FAVOR_CRATE, Point2(656.0, 0.0));
  createPuppyObj(FAVOR_CRATE, Point2(736.0, 0.0));
  createPuppyObj(FAVOR_CRATE, Point2(624.0, -64.0));
  createPuppyObj(FAVOR_CRATE, Point2(688.0, -64.0));
  
  currQuad = puppyPieceDownhill12_8(Point2(1168.0, 64.0), yHandle);
  currQuad->setPuppyEdges(4, 0);
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint)->topRight - endpoint;
}

void PizzaLevel::puppyDualfireballChunk()
{
  Point1 yHandle = 336.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(215.0, 131.0), yHandle);
  createCoin(Point2(298.0, 78.0), yHandle);
  createCoin(Point2(385.0, 48.0), yHandle);
  createCoin(Point2(480.0, 37.0), yHandle);
  createCoin(Point2(575.0, 55.0), yHandle);
  createCoin(Point2(664.0, 92.0), yHandle);
  createCoin(Point2(743.0, 145.0), yHandle);
  
  puppyPieceLava(Point2(288.0, 0.0), false);
  createFireball(Point2(384.0, 0.0));
  createFireball(Point2(576.0, 0.0));
}

void PizzaLevel::puppyGroundtriangleChunk()
{
  Point1 yHandle = 272.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(254.0, 133.0), yHandle);
  createCoin(Point2(337.0, 55.0), yHandle);
  createCoin(Point2(481.0, 33.0), yHandle);
  createCoin(Point2(624.0, 56.0), yHandle);
  createCoin(Point2(706.0, 134.0), yHandle);
  
  puppyPiecePivottri(Point2(480.0, 0.0));
}

void PizzaLevel::puppyLavalpoolChunk()
{
  Point1 yHandle = 336.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(215.0, 131.0), yHandle);
  createCoin(Point2(298.0, 78.0), yHandle);
  createCoin(Point2(385.0, 48.0), yHandle);
  createCoin(Point2(480.0, 37.0), yHandle);
  createCoin(Point2(575.0, 55.0), yHandle);
  createCoin(Point2(664.0, 92.0), yHandle);
  createCoin(Point2(743.0, 145.0), yHandle);
  
  puppyPieceLava(Point2(288.0, 0.0), false);
  createFireball(Point2(480.0, 0.0));
}

void PizzaLevel::puppyObjstepsChunk(Coord1 variation)
{
  Point1 yHandle = 304.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0), false, 0, 0, true)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight + Point2(32.0, -96.0), false, 1, 0, true)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight + Point2(32.0, -96.0), false, 1, 0, true)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight, false, 0, 1, false)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight + Point2(32.0, 96.0), true, 0, 1, false)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight + Point2(32.0, 96.0), true, 0, 0, false)->topRight - endpoint;
  
  if (variation == FAVOR_CAGE)
  {
    createPuppyObj(FAVOR_CRATE, Point2(416.0, 208.0), yHandle);
    createPuppyObj(FAVOR_CRATE, Point2(704.0, 112.0), yHandle);
    createPuppyObj(FAVOR_CAGE, Point2(960.0, 112.0), yHandle);
    createPuppyObj(FAVOR_CRATE, Point2(1248.0, 208.0), yHandle);
  }
  else
  {
    createPuppyObj(variation, Point2(416.0, 208.0), yHandle);
    createPuppyObj(variation, Point2(704.0, 112.0), yHandle);
    createPuppyObj(variation, Point2(960.0, 112.0), yHandle);
    createPuppyObj(variation, Point2(1248.0, 208.0), yHandle);
  }
}

void PizzaLevel::puppyPillarsChunk(Coord1 variation)
{
  Point1 yHandle = 224.0;
  
  puppyPieceFlat16(Point2(0.0, 0.0), false, 0, 3, false);
  
  puppyPieceFlat8(Point2(608.0, 192.0), false, 2, 2, false, yHandle);
  puppyPieceFlat8(Point2(1088.0, 96.0), false, 4, 4, false, yHandle);
  puppyPieceFlat8(Point2(1568.0, 192.0), false, 2, 2, false, yHandle);
  
  puppyPieceFlat16(Point2(2048.0, 0.0), false, 2, 0, false);
  
  if (variation == FAVOR_CAGE)
  {
    createPuppyObj(FAVOR_CRATE, Point2(672.0, 192.0), yHandle);
    createPuppyObj(FAVOR_NOTHING, Point2(1152.0, 96.0), yHandle);
    createPuppyObj(FAVOR_CAGE, Point2(1632.0, 192.0), yHandle);
  }
  else
  {
    createPuppyObj(variation, Point2(672.0, 192.0), yHandle);
    createPuppyObj(variation, Point2(1152.0, 96.0), yHandle);
    createPuppyObj(variation, Point2(1632.0, 192.0), yHandle);
  }
}

void PizzaLevel::puppyQuadspikeChunk(Coord1 variation)
{
  Point1 yHandle = 320.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill4_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(266.0, 175.0), yHandle);
  createCoin(Point2(337.0, 117.0), yHandle);
  createCoin(Point2(422.0, 62.0), yHandle);
  createCoin(Point2(515.0, 27.0), yHandle);
  createCoin(Point2(607.0, 20.0), yHandle);
  
  puppyPieceSpikeball(Point2(482.0, 272.0), yHandle);
  puppyPieceSpikeball(Point2(574.0, 242.0), yHandle);
  
  createPuppyObj(variation, Point2(832.0, 288.0), yHandle);
  
  puppyPieceSpikeball(Point2(1088.0, 242.0), yHandle);
  puppyPieceSpikeball(Point2(1184.0, 272.0), yHandle);
  
  createCoin(Point2(1054.0, 20.0), yHandle);
  createCoin(Point2(1145.0, 27.0), yHandle);
  createCoin(Point2(1239.0, 62.0), yHandle);
  createCoin(Point2(1322.0, 117.0), yHandle);
  createCoin(Point2(1393.0, 175.0), yHandle);
}

void PizzaLevel::puppySingletriChunk()
{
  Point1 yHandle = 512.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight, false, 0, 2, false)->topRight - endpoint;
  
  createCoin(Point2(455.0, 369.0), yHandle);
  createCoin(Point2(530.0, 309.0), yHandle);
  createCoin(Point2(611.0, 257.0), yHandle);
  createCoin(Point2(699.0, 221.0), yHandle);
  createCoin(Point2(794.0, 215.0), yHandle);
  createCoin(Point2(888.0, 231.0), yHandle);
  
  puppyPiecePivottri(Point2(1056.0, 416.0), yHandle);
  TerrainQuad* invis = puppyPieceFlat8(Point2(992.0, 416.0), yHandle);
  invis->setImage(NULL);
  
  createCoin(Point2(1082.0, 177.0), yHandle);
  createCoin(Point2(1156.0, 116.0), yHandle);
  createCoin(Point2(1239.0, 62.0), yHandle);
  createCoin(Point2(1325.0, 28.0), yHandle);
  createCoin(Point2(1423.0, 22.0), yHandle);
  createCoin(Point2(1513.0, 37.0), yHandle);
  createCoin(Point2(1604.0, 77.0), yHandle);
  createCoin(Point2(1680.0, 129.0), yHandle);
  createCoin(Point2(1754.0, 189.0), yHandle);
  createCoin(Point2(1818.0, 260.0), yHandle);
  createCoin(Point2(1875.0, 334.0), yHandle);
  createCoin(Point2(1929.0, 417.0), yHandle);
  
  topRight = puppyPieceFlat16(Point2(1376.0, 0.0), false, 2, 0, false)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
}

void PizzaLevel::puppySpikeballBumpsChunk()
{
  Point1 yHandle = 336.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0), false, 0, 0, true)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight + Point2(32.0, -96.0), false, 1, 0, false)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight, false, 0, 0, true)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight + Point2(32.0, -96.0), false, 1, 0, false)->topRight - endpoint;
  topRight = puppyPieceDownhill4_4(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill4_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  puppyPieceSpikeball(Point2(242.0, 288.0), yHandle);
  
  createCoin(Point2(168.0, 70.0), yHandle);
  createCoin(Point2(259.0, 36.0), yHandle);
  createCoin(Point2(354.0, 26.0), yHandle);
  createCoin(Point2(446.0, 46.0), yHandle);
  createCoin(Point2(533.0, 84.0), yHandle);
  
  puppyPieceSpikeball(Point2(1042.0, 288.0), yHandle);
  
  createCoin(Point2(967.0, 70.0), yHandle);
  createCoin(Point2(1059.0, 36.0), yHandle);
  createCoin(Point2(1154.0, 26.0), yHandle);
  createCoin(Point2(1246.0, 46.0), yHandle);
  createCoin(Point2(1334.0, 84.0), yHandle);
}

void PizzaLevel::puppySpikeballhillChunk()
{
  Point1 yHandle = 128.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceUphill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  puppyPieceSpikeball(Point2(768.0, 48.0), yHandle);
}

void PizzaLevel::puppySpikeballpillarChunk(Coord1 variation)
{
  Point1 yHandle = 160.0;
  
  Coord1 stdVariation = variation != FAVOR_CAGE_SPECIAL ? variation : FAVOR_CAGE;
  
  puppyPieceFlat16(Point2(0.0, 0.0), false, 0, 2, false);
  
  puppyPieceSpikeball(Point2(434.0, 0.0));
  
  puppyPieceFlat8(Point2(608.0, 80.0), false, 3, 3, false, yHandle);
  
  puppyPieceFlat8(Point2(1088.0, 128.0), false, 2, 2, false, yHandle);
  createPuppyObj(stdVariation, Point2(1152.0, 128.0), yHandle);
  
  puppyPieceFlat8(Point2(1568.0, 64.0), false, 3, 3, false, yHandle);
  
  puppyPieceFlat8(Point2(1920.0, 128.0), false, 2, 2, false, yHandle);
  
  puppyPieceSpikeball(Point2(2194.0, 0.0));
  
  puppyPieceFlat16(Point2(2336.0, 0.0), false, 2, 0, false);
  
  if (variation == FAVOR_CAGE_SPECIAL)
  {
    tryCreateUnlockable(Point2(2512.0, 96.0 - yHandle) + endpoint, Coord2(3, 11));
  }
}

void PizzaLevel::puppySpikeballpitChunk()
{
  Point1 yHandle = 432.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceUphill12_8(topRight);
  currQuad->setPuppyEdges(0, 4);
  
  createCoin(Point2(375.0, 183.0), yHandle);
  createCoin(Point2(446.0, 122.0), yHandle);
  createCoin(Point2(530.0, 70.0), yHandle);
  createCoin(Point2(622.0, 36.0), yHandle);
  createCoin(Point2(717.0, 26.0), yHandle);
  createCoin(Point2(812.0, 42.0), yHandle);
  createCoin(Point2(897.0, 82.0), yHandle);
  createCoin(Point2(975.0, 136.0), yHandle);
  createCoin(Point2(1048.0, 197.0), yHandle);
  createCoin(Point2(1112.0, 268.0), yHandle);
  createCoin(Point2(1169.0, 340.0), yHandle);
  
  puppyPieceSpikeball(Point2(545.0, 323.0), yHandle);
  puppyPieceSpikeball(Point2(672.0, 274.0), yHandle);
  puppyPieceSpikeball(Point2(800.0, 323.0), yHandle);
  puppyPieceSpikeball(Point2(610.0, 400.0), yHandle);
  puppyPieceSpikeball(Point2(736.0, 400.0), yHandle);
  
  currQuad = puppyPieceDownhill12_8(Point2(896.0, 304.0), yHandle);
  currQuad->setPuppyEdges(4, 0);
  
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint)->topRight - endpoint;
}

void PizzaLevel::puppySwingsquareChunk()
{
  Point1 yHandle = 496.0;
  
  puppyPieceFlat16(Point2(0.0, 0.0), false, 0, 1, false);
  
  createCoin(Point2(192.0, 345.0), yHandle);
  createCoin(Point2(264.0, 286.0), yHandle);
  createCoin(Point2(344.0, 232.0), yHandle);
  createCoin(Point2(437.0, 199.0), yHandle);
  createCoin(Point2(530.0, 191.0), yHandle);
  createCoin(Point2(623.0, 208.0), yHandle);
  
  puppyPiecePivotsquare(Point2(640.0, 416.0), yHandle);
  
  createCoin(Point2(696.0, 147.0), yHandle);
  createCoin(Point2(777.0, 92.0), yHandle);
  createCoin(Point2(866.0, 64.0), yHandle);
  createCoin(Point2(958.0, 53.0), yHandle);
  createCoin(Point2(1055.0, 71.0), yHandle);
  createCoin(Point2(1139.0, 105.0), yHandle);
  createCoin(Point2(1218.0, 156.0), yHandle);
  createCoin(Point2(1291.0, 222.0), yHandle);
  
  puppyPiecePivotsquare(Point2(1296.0, 416.0), yHandle);
  
  createCoin(Point2(1385.0, 190.0), yHandle);
  createCoin(Point2(1493.0, 198.0), yHandle);
  createCoin(Point2(1597.0, 233.0), yHandle);
  createCoin(Point2(1680.0, 288.0), yHandle);
  createCoin(Point2(1750.0, 344.0), yHandle);
  
  puppyPieceFlat16(Point2(1684.0, 0.0), false, 1, 0, false);
}

void PizzaLevel::puppyTrianglepitChunk()
{
  Point1 yHandle = 640.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  TerrainQuad* currQuad = puppyPieceUphill12_8(topRight);
  currQuad->setPuppyEdges(0, 2);
  
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint + Point2(32.0, 160.0),
                              true, 0, 0, false)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight, false, 0, 0, true)->topRight - endpoint;
  
  puppyPieceSpike(Point2(480.0, 528.0), yHandle);
  puppyPieceSpike(Point2(736.0, 528.0), yHandle);
  puppyPieceSpike(Point2(992.0, 528.0), yHandle);
  puppyPieceSpike(Point2(1248.0, 528.0), yHandle);
  puppyPieceSpike(Point2(1504.0, 528.0), yHandle);
  puppyPieceSpike(Point2(1760.0, 528.0), yHandle);
  
  createCoin(Point2(410.0, 390.0), yHandle);
  createCoin(Point2(482.0, 327.0), yHandle);
  createCoin(Point2(566.0, 274.0), yHandle);
  createCoin(Point2(658.0, 243.0), yHandle);
  createCoin(Point2(752.0, 235.0), yHandle);
  createCoin(Point2(844.0, 250.0), yHandle);
  createCoin(Point2(931.0, 288.0), yHandle);
  
  TrianglePivot* currTri = puppyPiecePivottri(Point2(1024.0, 448.0), yHandle);
  TerrainQuad* currFlat = puppyPieceFlat8(currTri->getXY() - Point2(64.0, 0.0) - endpoint);
  currFlat->setImage(NULL);
  
  createCoin(Point2(1023.0, 200.0), yHandle);
  createCoin(Point2(1087.0, 133.0), yHandle);
  createCoin(Point2(1161.0, 72.0), yHandle);
  createCoin(Point2(1254.0, 44.0), yHandle);
  createCoin(Point2(1344.0, 61.0), yHandle);
  createCoin(Point2(1425.0, 109.0), yHandle);
  createCoin(Point2(1494.0, 177.0), yHandle);
  createCoin(Point2(1552.0, 251.0), yHandle);
  
  currTri = puppyPiecePivottri(Point2(1632.0, 448.0), yHandle);
  currFlat = puppyPieceFlat8(currTri->getXY() - Point2(64.0, 0.0) - endpoint);
  currFlat->setImage(NULL);
  
  createCoin(Point2(1637.0, 204.0), yHandle);
  createCoin(Point2(1698.0, 137.0), yHandle);
  createCoin(Point2(1777.0, 82.0), yHandle);
  createCoin(Point2(1865.0, 50.0), yHandle);
  createCoin(Point2(1960.0, 66.0), yHandle);
  createCoin(Point2(2039.0, 114.0), yHandle);
  createCoin(Point2(2107.0, 181.0), yHandle);
  createCoin(Point2(2164.0, 254.0), yHandle);
  createCoin(Point2(2213.0, 338.0), yHandle);
  createCoin(Point2(2261.0, 423.0), yHandle);
  createCoin(Point2(2303.0, 508.0), yHandle);
  
  currQuad = puppyPieceDownhill12_8(topRight + Point2(32.0, -160.0));
  currQuad->setPuppyEdges(2, 0);
  
  topRight = puppyPieceFlat16(currQuad->topRight - endpoint)->topRight - endpoint;
  
}

void PizzaLevel::puppyWalljumpChunk()
{
  Point1 yHandle = 336.0;
  
  Point2 topRight = puppyPieceFlat16(Point2(0.0, 0.0))->topRight - endpoint;
  topRight = puppyPieceUphill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat8(topRight)->topRight - endpoint;
  topRight = puppyPieceDownhill8_2(topRight)->topRight - endpoint;
  topRight = puppyPieceFlat16(topRight)->topRight - endpoint;
  
  createCoin(Point2(234.0, 182.0), yHandle);
  createCoin(Point2(305.0, 121.0), yHandle);
  createCoin(Point2(389.0, 71.0), yHandle);
  createCoin(Point2(479.0, 36.0), yHandle);
  createCoin(Point2(575.0, 24.0), yHandle);
  createCoin(Point2(669.0, 43.0), yHandle);
  createCoin(Point2(753.0, 80.0), yHandle);
  createCoin(Point2(832.0, 133.0), yHandle);
  createCoin(Point2(905.0, 198.0), yHandle);
  
  puppyPieceLava(Point2(400.0, 304.0), true, yHandle);
  puppyPieceLava(Point2(624.0, 304.0), true, yHandle);
  
  createFireball(Point2(464.0, 304.0), yHandle);
  createFireball(Point2(688.0, 304.0), yHandle);
  
  puppyPieceWall(Point2(544.0, 176.0), yHandle);
}

PuppyMissile* PizzaLevel::arrowFromOffscreen(Coord1 dir, Logical puppyRemoval)
{
  Box camBox = camera.myBox();
  
  Point2 coords(0.0, 0.0);
  Point2 vel(0.0, 0.0);
  
  if (dir == -1 || dir == 1)
  {
    Point1 side = dir == -1 ? camBox.left() : camBox.right();
    Point2 vertRange(0.4, 0.6);
    if (puppyRemoval == true) vertRange.set(0.525, 0.65);
    
    coords.set(side + dir * PLAY_W, RM::randf(vertRange.x * PLAY_H, vertRange.y * PLAY_H));
    vel = Point2(200.0 * -dir, 0.0);
  }
  else
  {
    coords.set(RM::lerp(camBox.left(), camBox.right(), RM::randf(0.2, 0.8)), -PLAY_H * 2.5);
    vel.set(0.0, 200.0);
  }
  
  PuppyMissile* arrow = new PuppyMissile(coords, vel);
  addEnemy(arrow);
  return arrow;
}

SkullBomb* PizzaLevel::createBomb(Point2 xy, Point2 vel)
{
  SkullBomb* bomb = new SkullBomb();
  bomb->setXY(xy);
  addEnemy(bomb);
  bomb->set_velocity(vel);
  return bomb;
}

void PizzaLevel::puppyWithCage(Point2 bottomCenter)
{
  Billy* dog = new Billy();
  dog->breedIndex = currBreed;
  dog->setXY(bottomCenter + endpoint);
  addEnemy(dog);
  puppyList.add(dog);
  
  PuppyCage* cage = new PuppyCage();
  cage->puppy = dog;
  cage->setXY(dog->getXY());
  addAlly(cage);
  
  currBreed++;
  currBreed %= 3;
  puppiesToFind++;
  puppiesToReturn = puppiesToFind;
  puppiesTotal = puppiesToFind;
}

Mimic* PizzaLevel::createMimic(Point2 bottomCenter)
{
  Mimic* mim = new Mimic();
  mim->setXY(bottomCenter + Point2(0.0, -mim->getHeight() * 0.5) + endpoint);
  addEnemy(mim);
  return mim;
}

JumpingFireball* PizzaLevel::createFireball(Point2 topCenter, Point1 layoutYHandle)
{
  JumpingFireball* fireball = new JumpingFireball();
  fireball->setXY(topCenter + endpoint + Point2(0.0, -layoutYHandle));
  fireball->setMovement(256.0, 1.0, 3.0);
  addEnemy(fireball);
  return fireball;
}

Mole* PizzaLevel::createMole(Point2 bottomCenter)
{
  Mole* newMole = new Mole();
  newMole->setXY(bottomCenter + Point2(0.0, newMole->getHeight()) + endpoint);
  addEnemy(newMole);
  return newMole;
}

TerrainCircle* PizzaLevel::puppyPieceSpikeball(Point2 center, Point1 layoutHandleY)
{
  TerrainCircle* spikeBall = new TerrainCircle(imgCache[puppySpikeball]);
  spikeBall->setXY(center + endpoint + Point2(0.0, -layoutHandleY));
  spikeBall->setWH(48.0, 48.0);
  spikeBall->flags |= PizzaGO::SPIKED_BIT;
  addAlly(spikeBall);
  return spikeBall;
}

LavaPatch* PizzaLevel::puppyPieceLava(Point2 leftCenter, Logical small, Point1 layoutHandleY)
{
  LavaPatch* patch = new LavaPatch(leftCenter + endpoint + Point2(0.0, -layoutHandleY), small);
  addEnemy(patch);
  return patch;
}

void PizzaLevel::createPuppyObj(Coord1 objType, Point2 bottomCenter, Point1 layoutHandleY)
{
  switch (objType)
  {
    default:
    case FAVOR_NOTHING:
      break;
    case FAVOR_CAGE:
      puppyWithCage(bottomCenter + Point2(0.0, -layoutHandleY));
      break;
    case FAVOR_CRATE:
    {
      TerrainQuad* crate = TerrainQuad::create_block_TL(
                                                        bottomCenter - Point2(32.0, 64.0) + endpoint + Point2(0.0, -layoutHandleY),
                                                        Point2(64.0, 64.0), imgsetCache[puppyCrateSet][randGen.randi(0, 1)]);
      crate->flags |= PizzaGO::DELETE_BELOWWORLD_BIT;
      addEnemy(crate);
      break;
    }
    case FAVOR_MOLE:
      createMole(bottomCenter + Point2(0.0, -layoutHandleY));
      break;
    case FAVOR_MIMIC:
      createMimic(bottomCenter + Point2(0.0, -layoutHandleY));
      break;
  }
}

TerrainQuad* PizzaLevel::createPuppyJunkCrate(Point2 center)
{
  TerrainQuad* crate = TerrainQuad::create_block_center(center,
                                                        imgsetCache[puppyCrateSet][randGen.randi(0, 1)]);
  crate->setRotation(RM::rand_angle());
  crate->updateFromWorld();
  crate->set_ang_vel(RM::randf(0.0, TWO_PI) * (RM::randl() ? 1.0 : -1.0));
  addEnemy(crate);
  return crate;
}

PizzaGO* PizzaLevel::createSticky(Point2 topCenter, Point1 rotation)
{
  StickyPatch* sticky = new StickyPatch(topCenter, rotation);
  addAlly(sticky);
  return sticky;
}

void PizzaLevel::createTopography(const Point2* topPoints, Coord1 numPoints, Image* firstImg)
{
  for (Coord1 i = 0; i < numPoints - 1; ++i)
  {
    Image* img = i == 0 ? firstImg : NULL;
    createSlopeQuad(topPoints[i], Point2(topPoints[i].x, topPoints[i].y + 64.0),
                    Point2(topPoints[i+1].x, topPoints[i+1].y + 64.0), topPoints[i+1],
                    img);
  }
}

void PizzaLevel::invisibleWallsStd(const Box& wallBox, Point1 wallThickness)
{
  // left wall
  addAlly(TerrainQuad::create_ground_TL(
                                        Point2(wallBox.left() - wallThickness, wallBox.top()),
                                        Point2(wallThickness, wallBox.height()),
                                        NULL));
  
  // right wall
  addAlly(TerrainQuad::create_ground_TL(
                                        Point2(wallBox.right(), wallBox.top()),
                                        Point2(wallThickness, wallBox.height()),
                                        NULL));
  
  // top wall
  addAlly(TerrainQuad::create_ground_TL(
                                        Point2(wallBox.left(), wallBox.top() - wallThickness),
                                        Point2(wallBox.width(), wallThickness),
                                        NULL));
}

void PizzaLevel::invisibleWallsLR(Point2 leftGroundPt, Point2 rightGroundPt)
{
  Point1 wallWidth = 128.0;
  Point1 wallHeight = 1024.0;
  
  // left wall
  TerrainQuad* leftWall = TerrainQuad::create_ground_TL(
                                                        Point2(leftGroundPt.x - wallWidth, leftGroundPt.y - wallHeight),
                                                        Point2(wallWidth, wallHeight),
                                                        NULL);
  addAlly(leftWall);
  leftWall->flags = 0x0;  // for spiked
  
  // right wall
  TerrainQuad* rightWall = TerrainQuad::create_ground_TL(
                                                         Point2(rightGroundPt.x, rightGroundPt.y - wallHeight),
                                                         Point2(wallWidth, wallHeight),
                                                         NULL);
  addAlly(rightWall);
  rightWall->flags = 0x0;  // for spiked
  
}

PizzaBackground* PizzaLevel::createBackgroundStd(Coord1 worldID)
{
  switch (worldID)
  {
    default: cout << "createBackgroundStd invalid world " << worldID << endl;
    case WORLD_GRAVEYARD: return new GraveyardBG(this); break;
    case WORLD_BADLANDS: return new BadlandsBG(this); break;
    case WORLD_QUARRY: return new QuarryBG(this); break;
    case WORLD_PIRATE: return new PirateBG(this); break;
    case WORLD_STADIUM: return new StadiumBG(this); break;
      
    case WORLD_NUCLEAR: return new NuclearBG(this); break;
    case WORLD_COMPUTER: return new ComputerBG(this); break;
    case WORLD_CARNIVAL: return new CarnivalBG(this); break;
    case WORLD_MOON: return new MoonBG(this); break;
    case WORLD_MEAT: return new MeatBG(this); break;
      
    case WORLD_CLOUD: return new CloudBG(this); break;
    case WORLD_MARKET: return new MarketBG(this); break;
  }
}

// ======================= From b2ContactListener ======================= //

void PizzaLevel::BeginContact(b2Contact* contact)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();
  
  if (userData1 == NULL || userData2 == NULL)
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }
  
  PizzaGO* pgo1 = static_cast<PizzaGO*>(userData1);
  PizzaGO* pgo2 = static_cast<PizzaGO*>(userData2);
}

void PizzaLevel::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
  
}

void PizzaLevel::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();
  
  if (userData1 == NULL || userData2 == NULL)
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }
  
  PizzaGO* pgo1 = static_cast<PizzaGO*>(userData1);
  PizzaGO* pgo2 = static_cast<PizzaGO*>(userData2);
  
  Point1 finalImpulse = b2dsca_to_world(std::abs(impulse->normalImpulses[0]));
  if (impulse->count == 2)
  {
    finalImpulse = std::max(finalImpulse, b2dsca_to_world(std::abs(impulse->normalImpulses[1])));
  }
  
  if (finalImpulse > 100.0)
  {
    pgo1->collidedPhysical(pgo2, finalImpulse);
    pgo2->collidedPhysical(pgo1, finalImpulse);
  }
}

void PizzaLevel::EndContact(b2Contact* contact)
{
  
}

PizzaPlayer* PizzaLevel::findPlayer(PizzaGO* pgo1, PizzaGO* pgo2)
{
  if (pgo1 == player) return static_cast<PizzaPlayer*>(pgo1);
  if (pgo2 == player) return static_cast<PizzaPlayer*>(pgo2);
  return NULL;
}

template <typename PGOType>
PGOType* PizzaLevel::findObj(PizzaGO* obj1, PizzaGO* obj2, Coord1 type)
{
  if (obj1->type == type) return static_cast<PGOType*>(obj1);
  if (obj2->type == type) return static_cast<PGOType*>(obj2);
  return NULL;
}

// =============================== PizzaBackground ============================ //

PizzaBackground::PizzaBackground(PizzaLevel* setLevel) :
  StdBackground(),
  level(setLevel),
  totalWidth(0.0),
  trackBounds(272.0, 304.0),
  bgBottomLeft(0.0, 352.0),
  foregroundDisabled(false)
{
  
}

void PizzaBackground::drawLandscapeTilesStd(Image* img, Point1 scaleFactor)
{
  CameraStd bgCam = level->camera;
  
  Box screenBox = bgCam.myBox();
  Point1 invScale = 1.0 / scaleFactor;
  Point1 regularSize = img->natural_size().x;
  
  bgCam.setHParallax(level->camera, scaleFactor);
  bgCam.xy.y = 0.0;
  bgCam.apply();
  Box layerBox = bgCam.myBox();
  // cout << "level left " << screenBox.left() << " bg left " << layerBox.left() << endl;
  
  // the -64 and +64 are for shake padding
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left() - 64.0, regularSize) - 1;
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right() + 64.0, regularSize) + 1;
  
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    // * 1.5 seems correct at far zoom outs but close 1.0 is better?
    Point1 xOff = Pizza::platformTL.x * 1.0 + i * regularSize;
    Point2 drawXY(bgBottomLeft + Point2(xOff, 0.0));
    img->draw(drawXY);
  }
}

void PizzaBackground::drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY)
{
  drawLandscapeTilesSlopes(img, scaleFactor, drawY, level->camera);
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
  // layerBox.xy += Pizza::platformTL;
  // cout << "layer box " << layerBox << endl;
  // cout << "level left " << screenBox.left() << " bg left " << layerBox.left() << endl;
  
  Coord1 startInterval = RM::segment_distance(0.0, layerBox.left(), regularSize);
  Coord1 endInterval = RM::segment_distance(0.0, layerBox.right(), regularSize);
  
  for (Coord1 i = startInterval; i <= endInterval; ++i)
  {
    Point1 xOff = Pizza::platformTL.x + i * regularSize;
    Point2 drawXY(xOff, drawY);
    img->draw(drawXY);
  }
}

// =============================== GraveyardBG ============================ //

GraveyardBG::GraveyardBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void GraveyardBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("graveyard_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("graveyard_moon"), Point2(0.0, 0.0), true));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("graveyard_far_graves"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("graveyard_close_graves"), Point2(0.0, 1.0)));
}

void GraveyardBG::drawBackground()
{
  RMGraphics->clear(RM::color255(61, 174, 167));
  
  CameraStd bgCam = level->camera;
  Box screenBox = bgCam.myBox();
  Point2 masterXY = bgCam.xy;
  Point2 masterZoom = bgCam.zoom;
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // moon
  bgCam.xy = Point2(0.0, 0.0);
  bgCam.setParallaxZoom(masterZoom, 0.05);
  bgCam.apply();
  imgsetCache[bgLayerSet][1]->draw(Point2(272.0, 4.0));
  
  // far graves
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.5);   // supposed to be 0.4
  
  // close graves
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][3], 0.65);
}

// =============================== BadlandsBG ============================ //

BadlandsBG::BadlandsBG(PizzaLevel* setLevel) :
  PizzaBackground(setLevel),
  spireXVals(8),
  spireImgIDs(8)
{
  Point1 currSpireX = 0.0;
  
  for (Coord1 i = 0; i < 8; ++i)
  {
    currSpireX += RM::randf(0.2, 0.4) * PLAY_W * 3.0;
    spireXVals.add(currSpireX);
    spireImgIDs.add(RM::randi(0, 2));
  }
}

void BadlandsBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("badlands_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("badlands_moon"), Point2(0.0, 0.0), true));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("badlands_mountains"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("badlands_roots"), Point2(0.0, 1.0)));
  
  Image::create_tiles_norm(imgStr("badlands_spires"), imgsetCache[bgObjectSet], 3, 1, Point2(0.5, 1.0));
}

void BadlandsBG::drawBackground()
{
  RMGraphics->clear(RM::color255(40, 37, 45));
  
  CameraStd bgCam = level->camera;
  Point2 worldBottomLeft(0.0, 352.0);
  Box screenBox = bgCam.myBox();
  Point2 masterXY = bgCam.xy;
  Point2 masterZoom = bgCam.zoom;
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // moon
  bgCam.xy = Point2(0.0, 0.0);
  bgCam.setParallaxZoom(masterZoom, 0.05);
  bgCam.apply();
  imgsetCache[bgLayerSet][1]->draw(Point2(288.0, 0.0));
  
  // mountains
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.3);
  
  // roots
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][3], 0.5);
}

void BadlandsBG::drawForeground()
{
  if (foregroundDisabled == true) return;
  
  CameraStd bgCam = level->camera;
  Point2 worldBottomLeft(0.0, 352.0);
  
  bgCam.applyHParallax(level->camera, 1.25);
  
  for (Coord1 i = 0; i < spireXVals.count; ++i)
  {
    imgsetCache[bgObjectSet][spireImgIDs[i]]->draw(spireXVals[i], worldBottomLeft.y);
  }
}

// =============================== QuarryBG ============================ //

QuarryBG::QuarryBG(PizzaLevel* setLevel, Logical withMiddleLayers) :
PizzaBackground(setLevel),
useMidLayers(withMiddleLayers)
{
  
}

void QuarryBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("quarry_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("quarry_far"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("quarry_mid"), Point2(0.0, 1.0)));
}

void QuarryBG::drawBackground()
{
  RMGraphics->clear(RM::color255(176, 193, 255));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  if (useMidLayers == true)
  {
    // far layer
    drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.3);
    
    // mid layer
    drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.75);
  }
}

// =============================== PirateBG ============================ //

PirateBG::PirateBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void PirateBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("pirate_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_abs(imgStr("pirate_island"), Point2(0.0, 56.0 + (352.0 - 176.0))));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("pirate_ocean"), Point2(0.0, 1.0)));
}

void PirateBG::drawBackground()
{
  RMGraphics->clear(RM::color255(56, 110, 157));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // island
  drawIslandTiles(imgsetCache[bgLayerSet][1], 0.3);
  
  // ocean
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.3);
  
}

void PirateBG::drawIslandTiles(Image* img, Point1 scaleFactor)
{
  CameraStd bgCam = level->camera;
  Box screenBox = bgCam.myBox();
  Point1 invScale = 1.0 / scaleFactor;
  
  bgCam.setHParallax(level->camera, scaleFactor);
  bgCam.setY(0.0);
  bgCam.apply();
  
  Point1 regularSize = 512.0;
  Point1 startX = std::floor((screenBox.xy.x) / (regularSize * invScale)) * regularSize + 340.0;
  
  while (startX < screenBox.xy.x / invScale + screenBox.wh.x)
  {
    img->draw(bgBottomLeft + Point2(startX, 0.0));
    startX += regularSize;
  }
}

// =============================== StadiumBG ============================ //

StadiumBG::StadiumBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void StadiumBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("stadium_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("stadium_signs"), Point2(0.0, 1.0)));
}

void StadiumBG::drawBackground()
{
  RMGraphics->clear(RM::color255(21, 12, 33));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.25);
  
  // signs
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.5);
  
}

// ==================================== NuclearBG ================================ //

NuclearBG::NuclearBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void NuclearBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("nuclear_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("nuclear_mid"), Point2(0.0, 1.0)));
}

void NuclearBG::drawBackground()
{
  RMGraphics->clear(RM::color255(175, 115, 86));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.05);
  
  // mid
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.5);
}

// ==================================== ComputerBG ================================ //

ComputerBG::ComputerBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void ComputerBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("motherboard_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("motherboard_mid"), Point2(0.0, 1.0)));
}

void ComputerBG::drawBackground()
{
  RMGraphics->clear(RM::color255(27, 32, 21));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.25);
  
  // mid
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.5);
}

// ==================================== CarnivalBG ================================ //

CarnivalBG::CarnivalBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void CarnivalBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("carnival_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("carnival_rides"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("carnival_tents"), Point2(0.0, 1.0)));
}

void CarnivalBG::drawBackground()
{
  RMGraphics->clear(RM::color255(28, 39, 52));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // rides
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.25);
  
  // tents
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.5);
}

// ==================================== MoonBG ================================ //

MoonBG::MoonBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void MoonBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("moon_space"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("moon_earth"), Point2(1.0, 0.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("moon_mid"), Point2(0.0, 1.0)));
}

void MoonBG::drawBackground()
{
  CameraStd bgCam = level->camera;
  
  RMGraphics->clear(RM::color255(4, 1, 0));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // earth
  bgCam.xy.set(0.0, 0.0);
  bgCam.zoom.set(1.0, 1.0);
  bgCam.apply();
  
  imgsetCache[bgLayerSet][1]->draw(Point2(192.0, 80.0));
  
  // mid
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.5);
}

// ==================================== MeatBG ================================ //

MeatBG::MeatBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel),
spireXVals(NUM_MEATS, true),
spireImgIDs(NUM_MEATS, true)
{
  Point1 currSpireX = -PLAY_W;
  for (Coord1 i = 0; i < NUM_MEATS; ++i)
  {
    currSpireX += RM::randf(0.4, 0.65) * PLAY_W;
    spireXVals[i] = currSpireX;
    spireImgIDs[i] = RM::randi(0, 3);
  }
}

void MeatBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("meatlocker_wall"), Point2(0.0, 1.0)));
  loadTilesAbs(bgObjectSet, "meatlocker_fg_meat", 4, 1, Point2(48.0, 512.0));
}

void MeatBG::drawBackground()
{
  CameraStd bgCam = level->camera;
  
  RMGraphics->clear(RM::color255(0, 0, 0));
  
  // wall
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 1.0);
}

void MeatBG::drawForeground()
{
  if (foregroundDisabled == true) return;
  
  CameraStd bgCam = level->camera;
  Point2 worldBottomLeft(0.0, 352.0);
  
  bgCam.setX(bgCam.getX() * 1.5);
  bgCam.apply();
  
  for (Coord1 i = 0; i < spireXVals.count; ++i)
  {
    imgsetCache[bgObjectSet][spireImgIDs[i]]->draw(spireXVals[i], worldBottomLeft.y);
  }
}

// =============================== CloudBG ============================ //

CloudBG::CloudBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void CloudBG::load()
{
  addImgNorm(bgLayerSet, "cloud_sky", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "cloud_far", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "cloud_mid", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "cloud_fg", Point2(0.0, 1.0));
}

void CloudBG::drawBackground()
{
  RMGraphics->clear(RM::color255(68, 109, 167));
  
  // sky
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.1);
  
  // far clouds
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.25);
  
  // mid clouds
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.5);
}

void CloudBG::drawForeground()
{
  // front clouds
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][3], 1.25);
}

// =============================== MarketBG ============================ //

MarketBG::MarketBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel),
fruitXVals(8),
fruitImgIDs(8)
{
  Point1 currSpireX = 0.0;
  
  for (Coord1 i = 0; i < 8; ++i)
  {
    currSpireX += RM::randf(0.2, 0.4) * PLAY_W * 3.0;
    fruitXVals.add(currSpireX);
    fruitImgIDs.add(RM::randi(0, 2));
  }
}

void MarketBG::load()
{
  addImgNorm(bgLayerSet, "market_buildings", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "market_stalls", Point2(0.0, 1.0));
  
  loadTilesNorm(bgObjectSet, "market_baskets", 3, 1, Point2(0.5, 1.0));
}

void MarketBG::drawBackground()
{
  RMGraphics->clear(RM::color255(232, 194, 166));
  
  CameraStd bgCam = level->camera;
  Point2 worldBottomLeft(0.0, 352.0);
  Box screenBox = bgCam.myBox();
  Point2 masterXY = bgCam.xy;
  Point2 masterZoom = bgCam.zoom;
  
  // buildings
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.8);
  
  // stalls
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.9);
}

void MarketBG::drawForeground()
{
  if (foregroundDisabled == true) return;
  
  CameraStd bgCam = level->camera;
  Point2 worldBottomLeft(0.0, 352.0);
  
  bgCam.applyHParallax(level->camera, 1.1);
  
  for (Coord1 i = 0; i < fruitXVals.count; ++i)
  {
    imgsetCache[bgObjectSet][fruitImgIDs[i]]->draw(fruitXVals[i], worldBottomLeft.y);
  }
}

// ==================================== KarateBG ================================ //

KarateBG::KarateBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void KarateBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("karate_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("karate_pagoda"), Point2(0.0, 1.0)));
}

void KarateBG::drawBackground()
{
  RMGraphics->clear(RM::color255(141, 191, 255));
  
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.25);
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.5);
}

// ==================================== BounceBG ================================ //

BounceBG::BounceBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel)
{
  
}

void BounceBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("bounce_stars"),
                                                         Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("bounce_nebula"),
                                                         Point2(0.0, 1.0), true));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("bounce_hill"),
                                                         Point2(0.0, 1.0)));
}

void BounceBG::drawBackground()
{
  CameraStd bgCam;
  bgCam.apply();
  imgsetCache[bgLayerSet][0]->draw(-44.0, 352.0);
  
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.25);
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][2], 0.5);
}

// ==================================== PuppyBG ================================ //

PuppyBG::PuppyBG(PizzaLevel* setLevel) :
  PizzaBackground(setLevel),
  dust(),
  oldCamX(0.0)
{
  
}

void PuppyBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("puppy_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("puppy_buildings"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("puppy_smoke"), Point2(0.0, 1.0)));
  
  dust.init(imgsetCache[bgLayerSet][2], Point2(-100.0, 0.0), Point2(bgBottomLeft.x + Pizza::platformTL.x,
                                                                    bgBottomLeft.y - Pizza::platformTL.y),
            // imgsetCache[bgLayerSet][2]->natural_size());
            SCREEN_WH);
}

void PuppyBG::update()
{
  Point1 camMoveX = level->camera.getX() - oldCamX;
  
  dust.scrollSpeed = Point2(-100.0 - camMoveX / RM::timePassed(), 0.0);
  dust.update();
  oldCamX = level->camera.getX();
}

void PuppyBG::drawBackground()
{
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][0], 0.25);
  drawLandscapeTilesStd(imgsetCache[bgLayerSet][1], 0.5);
}

void PuppyBG::drawForeground()
{
  CameraStd cam;
  cam.apply();
  dust.redraw();
}

// =============================== Snowflake ============================ //

const Point2 Snowflake::X_SPEED_RANGE(30.0, 60.0);
const Point2 Snowflake::Y_SPEED_RANGE(50.0, 100.0);

Snowflake::Snowflake() :
VisRectangular(),
Effect(),

weatherSet(&imgsetCache[bgObjectSet]),
level(NULL),

sizePercent(1.0),
vel(0.0, 0.0),
rotVel(RM::randf(0.20, 0.40) * (RM::randl() ? 1.0 : -1.0)),
checkTimer(1.0, this)
{
  
}

void Snowflake::updateMe()
{
  if (getImage() == NULL) reset(true);
  
  addXY(vel * RM::timePassed());
  // addXY(Pizza::cameraMoved * sizePercent * 0.5);
  addRotation(rotVel * TWO_PI * RM::timePassed());
  RM::wrap1_me(rotation, 0.0, TWO_PI);
  
  checkTimer.update();
}

void Snowflake::redrawMe()
{
  if (getImage() == NULL) reset(true);
  drawMe();
}

void Snowflake::reset(Logical firstReset)
{
  setImage(RM::random(*weatherSet));
  
  sizePercent = RM::randf(0.25, 1.0);
  vel = Point2(RM::randf(X_SPEED_RANGE) * (RM::randl() ? -1.0 : 1.0),
               RM::randf(Y_SPEED_RANGE)) * sizePercent;
  setScale(sizePercent, sizePercent);
  autoSize();
  
  Box camBox = level->camBox();
  
  if (firstReset == true)
  {
    setX(RM::randf(camBox.left() + getWidth(), camBox.right() - getWidth()));
    setY(RM::randf(camBox.top() - getHeight(), camBox.bottom() + getHeight()));
  }
  else
  {
    setX(RM::lerp(camBox.left(), camBox.left() + camBox.wh.x * 1.5, RM::randf(0.0, 1.0)));
    setY(camBox.bottom() + getHeight());
    
    if (Pizza::cameraMoved.y <= 0.0) setY(camBox.top() - getHeight());
    
    // reposition on the y if it's in front of the camera
    if (getX() > camBox.right())
    {
      setY(RM::lerp(camBox.top(), camBox.bottom() + camBox.wh.y * 1.5, RM::randf(0.0, 1.0)));
    }
  }
}

void Snowflake::callback(ActionEvent* caller)
{
  if (caller == &checkTimer)
  {
    if (Box::collision(boxFromC(), level->camBox()) == false)
    {
      reset(false);
    }
    checkTimer.reset();
  }
}


// =============================== SlopesBG ============================ //

SlopesBG::SlopesBG(PizzaLevel* setLevel) :
PizzaBackground(setLevel),
snowflakes(8, true)
{
  for (Coord1 i = 0; i < snowflakes.count; ++i)
  {
    snowflakes[i].level = level;
  }
}

void SlopesBG::load()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("slopes_sky"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("slopes_mid_clouds"), Point2(0.0, 0.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("slopes_front_clouds"), Point2(0.0, 0.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("slopes_mountains"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("slopes_rocks"), Point2(0.0, 1.0)));
  Image::create_tiles(imgStr("snowflakes"), imgsetCache[bgObjectSet], 4, 1, true);
}

void SlopesBG::update()
{
  for (Coord1 i = 0; i < snowflakes.count; ++i)
  {
    snowflakes[i].update();
  }
}

void SlopesBG::drawBackground()
{
  // sky
  drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][0], 0.1, 240.0);
  
  // mid clouds
  drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][1], 0.2, 16.0);
  
  // front clouds
  drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][2], 0.3, -32.0);
  
  // mountains
  drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][3], 0.5, 352.0);
  
  // rocks
  drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][4], 0.75, 352.0);
}

void SlopesBG::drawForeground()
{
  for (Coord1 i = 0; i < snowflakes.count; ++i)
  {
    snowflakes[i].redraw();
  }
}

// ==================================== GladiatorLevel ================================ //

GladiatorLevel::GladiatorLevel() :
  PizzaLevel(),

  enemiesEnabled(8),
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
  
  musicFilename = "gladiator$.ima4";
  musicFilename.int_replace('$', (Pizza::currLevelDef.y % 3) + 1);
}

void GladiatorLevel::preload()
{
  background = createBackgroundStd(Pizza::currWorldPair.x);
}

void GladiatorLevel::loadRes()
{
  background->load();
  
  String1 terrainStr(16);
  terrainStr = modeStrs[LEVEL_GLADIATOR];
  terrainStr += '_';
  terrainStr += worldStrs[Pizza::currWorldPair.x];
  addImgNorm(bgLayerSet, terrainStr.as_cstr(), Point2(0.0, 1.0));
  
  for (Coord1 i = 0; i < enemiesEnabled.count; ++i)
  {
    loadEnemy(enemiesEnabled[i]);
  }
  
  loadImgNorm(sludge, "sticky", Point2(0.5, 0.2));
  loadImgNorm(sludgeParticle, "goo_particle", Point2(0.5, 0.5));
  
  imgCache[shadowSmall] = Image::create_centered(imgStr("shadow_small"));
  imgCache[shadowMedium] = Image::create_centered(imgStr("shadow_med"));
  imgCache[shadowLarge] = Image::create_centered(imgStr("shadow_large"));
}

void GladiatorLevel::unloadRes()
{
  // if any of the enemies need special free instructions make sure to
  // do the same in PlaneLevel
}

void GladiatorLevel::loadEnemy(EnemyType enemyID)
{
  switch (enemyID)
  {
    default:
    case ENEMY_SPEARMAN_SIDE:
      Image::create_tiles_abs(imgStr("spearman_front_walk"), imgsetCache[skeletonSpearSide],
                              4, 3, Point2(55.0, 118.0));
      break;
    case ENEMY_SPEARMAN_VERT:
      Image::create_tiles_abs(imgStr("spearman_up_walk"), imgsetCache[skeletonSpearVert],
                              4, 3, Point2(46.0, 172.0));
      break;
    case ENEMY_GARGOYLE:
    {
      Image::create_tiles_abs(imgStr("demon"), imgsetCache[pumpkinImgs],
                              5, 1, Point2(82.0, 58.0));
      DataList<Image*> justFly(imgsetCache[pumpkinImgs], 0, 3);
      
      Image::create_pingpong(justFly, pumpkinFly);
      loadSound(demonSwoopSound, "demonswoop.wav");
    }
      break;
    case ENEMY_SPINY:
      Image::create_tiles(imgStr("spiky"), imgsetCache[spinyImgs], 4, 3, true);
      break;
    case ENEMY_SKELOSTRICH_SIDE:
      Image::create_tiles_abs(imgStr("ostrich_front_walk"), imgsetCache[ostrichWalkSide],
                              4, 3, Point2(63.0, 118.0));
      Image::create_tiles_abs(imgStr("ostrich_front_fly"), imgsetCache[ostrichFlySide],
                              3, 2, Point2(71.0, 114.0));
      break;
      
    case ENEMY_SKELOSTRICH_VERT:
      Image::create_tiles_abs(imgStr("ostrich_up_walk"), imgsetCache[ostrichWalkVert],
                              4, 3, Point2(60.0, 176.0));
      break;
    case ENEMY_CUPID:
      Image::create_tiles_abs(imgStr("cupid_idle"), imgsetCache[cupidIdleSet], 4, 2, Point2(53.0, 38.0));
      imgCache[cupidArrow] = Image::create_handled_abs(imgStr("cupid_arrow"), Point2(38.0, 10.0));
      
      Image::create_tiles_abs(imgStr("cupid_wings"), imgsetCache[cupidWingSrc], 6, 1, Point2(53.0, 38.0));
      Image::create_pingpong(imgsetCache[cupidWingSrc], cupidWingSet);
      
      Image::create_tiles_abs(imgStr("cupid_shoot"), imgsetCache[cupidShootSet], 5, 2, Point2(53.0, 38.0));
      loadSound(cupidSound, "cupid.wav");
      break;
    case ENEMY_NAGA:
      imgCache[nagaHeadClosed] = Image::create_handled_abs(imgStr("naga_head_closed"), Point2(9.0, 13.0));
      imgCache[nagaHeadOpen] = Image::create_handled_abs(imgStr("naga_head_open"), Point2(12.0, 16.0));
      imgCache[nagaTail] = Image::create_handled_abs(imgStr("naga_tail"), Point2(41.0, 14.0));
      imgCache[nagaBone] = Image::create_handled_abs(imgStr("naga_vertebrae"), Point2(9.0, 16.0));
      imgCache[nagaArm] = Image::create_handled_abs(imgStr("naga_vertebrae_arm"), Point2(14.0, 16.0));
      imgCache[madsunFlame] = Image::create_centered(imgStr("flame"), true);
      loadSound(nagaSound, "naga.wav");
      break;
    case ENEMY_WISP:
      imgsetCache[wispSet].add(Image::create_centered(imgStr("wisp_normal")));
      imgsetCache[wispSet].add(Image::create_centered(imgStr("wisp_fade")));
      imgsetCache[wispSet].add(Image::create_centered(imgStr("wisp_blink")));
      Image::create_tiles_abs(imgStr("wispfire"), imgsetCache[wispFlameSet], 1, 2, Point2(27.0, 10.0));
      Image::create_tiles(imgStr("coin_sparkles"), imgsetCache[wispParticles], 4, 2, true);
      dwarpCache[wispWarp] = new DWarpSequence("wisp.dwarp2", true);
      loadSound(wispShootSound, "wispshoot.wav");
      loadSound(wispDieSound, "wispdie.wav");
      break;
    case ENEMY_ROBOT:
      Image::create_tiles_abs(imgStr("robo_walk"), imgsetCache[roboWalkImgs], 4, 2, Point2(60.0, 152.0));
      Image::create_tiles_abs(imgStr("robo_attack"), imgsetCache[roboAtkImgs], 3, 1, Point2(60.0, 152.0));
      Image::create_tiles_abs(imgStr("robo_fireball"), imgsetCache[roboFireballImgs], 3, 1,
                              Point2(4.0, 72.0), true);
      imgCache[roboElectricity] = Image::create_centered(imgStr("robo_electricity"), true);
      loadSound(roboPhaseSound, "robo_phase.wav");
      loadSound(roboFlameSound, "flame.wav");
      break;
      
    case ENEMY_GIANT:
      Image::create_tiles_abs(imgStr("giant_walk"), imgsetCache[giantWalkImgs], 4, 2, Point2(88.0, 196.0));
      Image::create_tiles_abs(imgStr("giant_attack"), imgsetCache[giantAtkImgs], 4, 1, Point2(88.0, 196.0));
      imgCache[giantSkull] = Image::create_handled_abs(imgStr("giant_skull"), Point2(26.0, 26.0));
      Image::create_tiles(imgStr("explosion"), imgsetCache[bombExplodeSet], 3, 1, true, true);
      loadSound(giantThrowSound, "giant_throw.wav");
      break;
    case ENEMY_FLAMESKULL:
      imgCache[flamingSkull] = Image::create_centered(imgStr("flameskull"));
      Image::create_tiles_abs(imgStr("flameskull_flame"), imgsetCache[flameskullFlameSet], 1, 3,
                              Point2(116.0, 64.0), true);
      break;
    case ENEMY_UNARMED:
      Image::create_tiles_abs(imgStr("unarmed"), imgsetCache[skeletonUnarmed],
                              4, 3, Point2(48.0, 102.0));
      loadSound(skeleRiseSound, "introskelappear.wav");
      break;
    case ENEMY_END_SKULLS:
      
      break;
    case ENEMY_RABBIT:
      loadTilesAbs(rabbitSet, "rabbit", 5, 1, Point2(83.0, 88.0));
      break;
    case ENEMY_TAIL:
      loadImgNorm(warningH, "warning_h", HANDLE_RC);
      loadImgNorm(wormImg, "worm", Point2(0.5, 0.0));
      loadImgNorm(wormShadowImg, "worm_shadow", HANDLE_C);
      loadImgNorm(wormParticleImg, "worm_particle", HANDLE_C);
      loadWarp(wormWarp, "worm.dwarp2");
      loadSound(skeleRiseSound, "introskelappear.wav");
      break;
    case ENEMY_DISK:
      loadImgAbs(spinnerGlobeImg, "spike_spinner_color", Point2(48.0, 24.0));
      loadTilesAbs(spinnerSpikeSet, "spike_spinner", 5, 2, Point2(48.0, 24.0));
      loadSound(spinnerSound, "spinner.wav");
      break;
  }
}

void GladiatorLevel::loadMe()
{
  createWorldStd();
  worldCreated();
  
  // this is to fix the camera in the beginning
  createWave();
  currWave++;
  
  camera.xy.x = player->getX();
  camera.handle.set(SCREEN_CX, SCREEN_H + Pizza::platformTL.y);
}

void GladiatorLevel::createWorld(const Box& setWorldBox)
{
  worldBox = setWorldBox;
  
  groundY = PLAY_H - 32.0;
  Point1 wallThickness = 128.0;
  
  TerrainQuad* ground = TerrainQuad::create_ground_TL(Point2(0.0, groundY),
                                                      Point2(worldBox.width(), 32.0),
                                                      NULL);
  addAlly(ground);
  
  invisibleWallsStd(worldBox);
}

void GladiatorLevel::createWorldStd()
{
  createWorld(Box(0.0, -PLAY_H * 2.0, PLAY_W * 3.0, PLAY_H * 3.0));
  
  // ground->set_friction(1.0);
  startStanding(Point2(192.0, getGroundY()));
  
  updateCamera();
}

void GladiatorLevel::updateStatus()
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

void GladiatorLevel::enemyDefeated(PizzaGO* enemy)
{
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
}

void GladiatorLevel::pizzaDamaged()
{
  currCombo = 0;
}

void GladiatorLevel::drawShadows()
{
  drawShadowsStd(groundY);
}

void GladiatorLevel::drawBGTerrain()
{
  background->drawLandscapeTilesStd(imgsetCache[bgLayerSet].last(), 1.0);
}

void GladiatorLevel::updateCamera()
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = closestEnemyX();
  
  updateZoomStd();
  updateFacingStd(enemyAnywhere, closestX);
  enemyCameraStd(enemyAnywhere, closestX);
  
  camera.clampHorizontal(Point2(worldBox.left() - Pizza::platformTL.x / camera.zoom.x,
                                worldBox.right() + Pizza::platformTL.x / camera.zoom.x));
}

void GladiatorLevel::updateLevelAI()
{
  if (levelState != LEVEL_PLAY) return;
  if (enemyActive() == true) return;
  
  Box camBox = camera.myBox();
  Box pizzaBox = player->collisionBox();
  
  Logical leftAvailable = camBox.left() - camSpawnPad > worldBox.left() + worldSpawnPad;
  Logical rightAvailable = camBox.right() + camSpawnPad < worldBox.right() - worldSpawnPad;
  
  if (leftAvailable == false && rightAvailable == false) return;
  
  Coord1 botRand = leftAvailable ? 0 : 1;
  Coord1 topRand = rightAvailable ? 1 : 0;
  Coord1 randSide = RM::randi(botRand, topRand);
  
  // cout << "wave " << currWave << endl;
  currSide = randSide;
  
  if (currWave <= numWaves)
  {
    createWave();
    currWave++;
  }
}

Point1 GladiatorLevel::spawnXStd(Coord1 side, Point1 lerpMin)
{
  Point1 createPad = 128.0;
  Box camBox = camera.myBox();
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
  return spawnYStd(index, number, background->trackBounds);
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

PizzaGO* GladiatorLevel::createSingle(Coord1 enemyType, Coord2 countData)
{
  PizzaGO* skele = NULL;
  
  Point2 flyBoundsStd(PLAY_Y + PLAY_H * 0.5, PLAY_Y + PLAY_H * 0.75);
  Point2 groundPosStd(spawnXStd(currSide), spawnYStd(countData.x, countData.y));
  Point2 groundPosFar(spawnXStd(currSide, 0.75), spawnYStd(countData.x, countData.y));
  Point2 flyPosStd(spawnXStd(currSide), spawnYStd(countData.x, countData.y, flyBoundsStd));
  Point2 position(0.0, 0.0);
  
  switch (enemyType)
  {
      // 0
    default:
    case ENEMY_SPEARMAN_SIDE:
      skele = new SkeletonSpearman(SkeletonSpearman::VAL_SIDEWAYS);
      position = groundPosStd;
      break;
    case ENEMY_SPEARMAN_VERT:
      skele = new SkeletonSpearman(SkeletonSpearman::VAL_VERTICAL);
      position = groundPosStd;
      break;
    case ENEMY_GARGOYLE:
      skele = new PumpkinSwooper();
      position = Point2(spawnXStd(currSide), 40.0);
      break;
    case ENEMY_SPINY:
      skele = new Spiny();
      position = groundPosStd;
      break;
    case ENEMY_SKELOSTRICH_SIDE:
      skele = new Skelostrich(Skelostrich::VAL_SIDEWAYS);
      position = groundPosFar;
      break;
      
      // 5
    case ENEMY_SKELOSTRICH_VERT:
      skele = new Skelostrich(Skelostrich::VAL_VERTICAL);
      position = groundPosFar;
      break;
    case ENEMY_CUPID:
      position = flyPosStd;
      skele = new SkeleCupid();
      break;
    case ENEMY_NAGA:
      position = flyPosStd;
      skele = new Naga();
      break;
    case ENEMY_WISP:
      position = flyPosStd;
      skele = new Wisp();
      break;
    case ENEMY_ROBOT:
      skele = new Robot();
      position = groundPosStd;
      break;
      
      // 10
    case ENEMY_GIANT:
      skele = new Giant();
      position = groundPosStd;
      break;
    case ENEMY_FLAMESKULL:
      skele = new FlameSkull();
      position = groundPosStd - Point2(0.0, skele->getHeight());
      break;
    case ENEMY_UNARMED:
      skele = new SkeletonTutorial();
      position = groundPosStd;
      break;
    case ENEMY_END_SKULLS:
      break;
    case ENEMY_RABBIT:
      skele = new RabbitAngry();
      position = groundPosFar;
      break;
      
      // 15
    case ENEMY_TAIL:
      skele = new TailSpiny();
      position = groundPosFar;
      break;
    case ENEMY_DISK:
      skele = new DiskEnemy();
      position = Point2(spawnXStd(currSide, RM::randf(0.75, 1.0)), 148.0 + RM::randf(0.0, 32.0));
      break;
  }
  
  skele->setXY(position);
  if (enemyType != ENEMY_FLAMESKULL) addEnemy(skele);
  else addAlly(skele);
  
  return skele;
}

PizzaGO* GladiatorLevel::createSingle(Coord1 enemyType, Coord1 groupID)
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
  createSticky(Point2(RM::lerp(worldBox.left(), worldBox.right(), courseXPercent), getGroundY() - 32.0));
}

// ==================================== TutorialScript

TutorialScript::TutorialScript(GLLevelTutorial* setLevel) :
ActionEvent(),
ActionListener(),

level(setLevel),
script(this),
actions(),

genTimer0(1.0),
genTimer1(1.0),
genTimer2(1.0),
textLines(4)
{
  textLines.add(VisRectangular(Point2(240.0, 29.0), imgsetCache[cinemaTextSet][0]));
  textLines.add(VisRectangular(Point2(240.0, 67.0), imgsetCache[cinemaTextSet][1]));
  textLines.add(VisRectangular(Point2(240.0, 99.0), imgsetCache[cinemaTextSet][2]));
  textLines.add(VisRectangular(Point2(240.0, 131.0), imgsetCache[cinemaTextSet][3]));
  
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].setAlpha(0.0);
  
  Point1 fadeDur = 0.75;
  Point1 riseDur = 2.0;
  
  genTimer0.reset(riseDur);
  genTimer0.setActive(false);
  
  genTimer1.reset(riseDur);
  genTimer1.setActive(false);
  
  genTimer2.reset(riseDur);
  genTimer2.setActive(false);
  
  level->skeles.add(new SkeletonTutorial());
  level->skeles.last()->setXY(level->camera.myBox().center().x + 53.0, PLAY_H - 50.0);
  level->skeles.last()->animation.setFrame(0);
  
  level->skeles.add(new SkeletonTutorial());
  level->skeles.last()->setXY(level->camera.myBox().center().x + 120.0, PLAY_H - 20.0);
  level->skeles.last()->animation.setFrame(3);
  
  level->skeles.add(new SkeletonTutorial());
  level->skeles.last()->setXY(level->camera.myBox().center().x + 200.0, PLAY_H - 40.0);
  level->skeles.last()->animation.setFrame(5);
  
  for (Coord1 i = 0; i < level->skeles.count; ++i)
  {
    level->addEnemy(level->skeles[i]);
    level->skeles[i]->setIntro();
  }
  
  // 0.0
  script.enqueueAddX(new LinearFn(&level->camera.zoom.x, 1.0, 9.0), &actions);
  script.enqueueAddX(new LinearFn(&level->camera.zoom.y, 1.0, 9.0), &actions);
  script.wait(1.0);
  
  // 1.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 3.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 5.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  
  script.enqueueAddX(new LinearFn(&level->skeles[0]->xy.y, level->skeles[0]->getY() -
                                  level->skeles[0]->getImage()->natural_size().y, riseDur), &actions);
  script.enqueueAddX(new LinearFn(&level->skeles[0]->color.w, 1.0, riseDur), &actions);
  script.enqueueX(new ShakeCommand(&level->skeles[0]->shaker));
  script.enqueueX(new SetValueCommand<Logical>(&genTimer0.active, true));
  script.enqueueX(new FunctionCommand0(&ResourceManager::playSkeleRise));
  script.wait(1.5);
  
  script.enqueueAddX(new LinearFn(&level->skeles[1]->xy.y, level->skeles[1]->getY() -
                                  level->skeles[1]->getImage()->natural_size().y, riseDur), &actions);
  script.enqueueAddX(new LinearFn(&level->skeles[1]->color.w, 1.0, riseDur), &actions);
  script.enqueueX(new ShakeCommand(&level->skeles[1]->shaker));
  script.enqueueX(new SetValueCommand<Logical>(&genTimer1.active, true));
  script.enqueueX(new FunctionCommand0(&ResourceManager::playSkeleRise));
  script.wait(0.5);
  
  // 7.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(0.75);
  
  script.enqueueAddX(new LinearFn(&level->skeles[2]->xy.y, level->skeles[2]->getY() -
                                  level->skeles[2]->getImage()->natural_size().y, riseDur), &actions);
  script.enqueueAddX(new LinearFn(&level->skeles[2]->color.w, 1.0, riseDur), &actions);
  script.enqueueX(new ShakeCommand(&level->skeles[2]->shaker));
  script.enqueueX(new SetValueCommand<Logical>(&genTimer2.active, true));
  script.enqueueX(new FunctionCommand0(&ResourceManager::playSkeleRise));
  script.wait(1.25);
  
  // 9.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&level->camera.xy.x, 0.0, 11.0), &actions);
  script.wait(2.0);
  
  // 11.0
  script.enqueueX(new SetValueCommand<Image*>(&textLines[0].image, imgsetCache[cinemaTextSet][4]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[1].image, imgsetCache[cinemaTextSet][5]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[2].image, imgsetCache[cinemaTextSet][6]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[3].image, imgsetCache[cinemaTextSet][7]));
  
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 13.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 15.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 17.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 19.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.wait(1.0);
  
  // 20.0
}

void TutorialScript::updateMe()
{
  if (genTimer0.getActive() == true && RM::randi(0, 4) == 0)
  {
    SkeletonTutorial* skele = level->skeles[0];
    Point2 skeleXY = Point2(skele->getX(), skele->sortY);
    Point1 xVal = skele->getX() + RM::randf(-32.0, 32.0);
    level->addAction(new Pebble(Point2(xVal, skeleXY.y)));
  }
  
  if (genTimer1.getActive() == true && RM::randi(0, 4) == 0)
  {
    SkeletonTutorial* skele = level->skeles[1];
    Point2 skeleXY = Point2(skele->getX(), skele->sortY);
    Point1 xVal = skele->getX() + RM::randf(-32.0, 32.0);
    level->addAction(new Pebble(Point2(xVal, skeleXY.y)));
  }
  
  if (genTimer2.getActive() == true && RM::randi(0, 4) == 0)
  {
    SkeletonTutorial* skele = level->skeles[2];
    Point2 skeleXY = Point2(skele->getX(), skele->sortY);
    Point1 xVal = skele->getX() + RM::randf(-32.0, 32.0);
    level->addAction(new Pebble(Point2(xVal, skeleXY.y)));
  }
  
  genTimer0.update();
  genTimer1.update();
  genTimer2.update();
  
  actions.update();
  script.update();
}

void TutorialScript::redrawMe()
{
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].drawMe();
  actions.redraw();
}

void TutorialScript::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    for (Coord1 i = 0; i < level->skeles.count; ++i)
    {
      level->skeles[i]->lifeState = RM::PASSIVE;
    }
    
    level->cinemaDone();
  }
}

// ==================================== GLLevelTutorial

GLLevelTutorial::GLLevelTutorial() :
GladiatorLevel(),

cinemaState(CINEMA),

box(NULL),
skeles(3),

growScript(this),
growStartScale(0.2),
growDur(4.0),
growScale(growStartScale),
growOffset(0.0),
growData(0.5, 1.0),

tapJump(NULL),
tiltRoll(NULL),
crushEnemies(NULL),

tapJumpData(0.0, 1.0)
{
  numWaves = 4;  // the first one doesn't count
}

void GLLevelTutorial::loadRes()
{
  background->load();
  
  String1 terrainStr(16);
  terrainStr = modeStrs[LEVEL_GLADIATOR];
  terrainStr += '_';
  terrainStr += worldStrs[Pizza::currWorldPair.x];
  terrainStr += imgSuffix;
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(terrainStr, Point2(0.0, 1.0)));
  
  loadEnemy(ENEMY_UNARMED);
  loadEnemy(ENEMY_SPEARMAN_SIDE);
  
  String1 lineTemplateStr(24);
  lineTemplateStr = imgStr("intro_verse$_@&");
  
  for (Coord1 i = 0; i < 4; ++i)
  {
    String1 verseStrA(24);
    verseStrA = lineTemplateStr;
    verseStrA.int_replace('$', 1);
    verseStrA.replace('@', 'a' + i);
    
    imgsetCache[cinemaTextSet].add(Image::create_centered(verseStrA));
  }
  
  for (Coord1 i = 0; i < 4; ++i)
  {
    String1 verseStrB(24);
    verseStrB = lineTemplateStr;
    verseStrB.int_replace('$', 2);
    verseStrB.replace('@', 'a' + i);
    
    imgsetCache[cinemaTextSet].add(Image::create_centered(verseStrB));
  }
  
  if (RMSystem->get_OS_family() == MAC_FAMILY)
  {
    imgsetCache[cinemaTextSet].add(Image::create_centered(imgStr("intro_taptojump_mac&")));
    imgsetCache[cinemaTextSet].add(Image::create_centered(imgStr("intro_tilttoroll_mac&")));
  }
  else
  {
    imgsetCache[cinemaTextSet].add(Image::create_centered(imgStr("intro_taptojump&")));
    imgsetCache[cinemaTextSet].add(Image::create_centered(imgStr("intro_tilttoroll&")));
  }
  
  imgsetCache[cinemaTextSet].add(Image::create_centered(imgStr("intro_crush&")));
  
  imgCache[shadowSmall] = Image::create_centered(imgStr("shadow_small"));
  imgCache[shadowMedium] = Image::create_centered(imgStr("shadow_med"));
  imgCache[shadowLarge] = Image::create_centered(imgStr("shadow_large"));
  
  Image::create_tiles_abs(imgStr("pizzabox"), imgsetCache[pizzaBoxSet], 3, 1, Point2(40.0, 40.0));
  Image::create_tiles_norm(imgStr("pizzabox_glow"), imgsetCache[boxGlowSet], 4, 1, Point2(0.5, 0.5));
  Image::create_tiles(imgStr("clods"), imgsetCache[junkSet], 4, 1, true);
  
  introMusic = Sound::create("intro.ima4");
}

void GLLevelTutorial::unloadRes()
{
  delete introMusic;
  introMusic = NULL;
}

void GLLevelTutorial::loadMe()
{
  createWorld(Box(0.0, -PLAY_H * 2.0, 1024.0, PLAY_H * 3.0));
  
  player->setIntro();
  
  box = new BoxPlayer(this);
  box->setXY(PLAY_W * 0.5, getGroundY() - box->getHeight() * 0.5);
  addAlly(box);
  
  camera.setX(512.0);
  camera.zoom.set(0.75, 0.75);
  camera.handle.set(SCREEN_CX, SCREEN_H + Pizza::platformTL.y);
  
  introMusic->repeats(false);
  introMusic->play();
  
  addFrontAction(new TutorialScript(this));
}

void GLLevelTutorial::updateCamera()
{
  if (cinemaState == PIZZA_GROW)
  {
    // RM::flatten_me(camera.xy.x, player->getX(), 100.0 * RM::timePassed());
    // the camera is moving in a script here
  }
  else if (cinemaState == FULL_PLAY)
  {
    GladiatorLevel::updateCamera();
  }
}

void GLLevelTutorial::updateLevelAI()
{
  if (cinemaState == BOX_PLAY)
  {
    RM::bounce_arcsine(tapJump->color.w, tapJumpData, Point2(0.0, 1.0), 0.4 * RM::timePassed());
  }
  else if (cinemaState == PIZZA_GROW)
  {
    growScale = RM::flatten(growScale, 1.0, ((1.0 - growStartScale) / growDur) * RM::timePassed());
    RM::bounce_arcsine(growOffset, growData, Point2(-1.0, 1.0), 2.0 * RM::timePassed());
    player->setScale(growScale + growOffset * 0.05, growScale + growOffset * 0.05);
    
    growScript.update();
    player->face.updateFace();
    
    player->setY(getGroundY());  // the next lines will adjust this y
    
    if (player->getX() - player->getWidth() * player->getXScale() < worldBox.left())
    {
      player->setX(player->getWidth() * player->getXScale() + worldBox.left());
    }
    
    if (player->getY() + player->getHeight() * player->getYScale() > getGroundY())
    {
      player->setY(getGroundY() - player->getHeight() * player->getYScale());
    }
  }
  else if (cinemaState == FULL_PLAY)
  {
    GladiatorLevel::updateLevelAI();
  }
}

void GLLevelTutorial::updateStatus()
{
  if (cinemaState == FULL_PLAY)
  {
    if (currWave >= numWaves && enemyActive() == false)
    {
      winLevel();
    }
  }
}

void GLLevelTutorial::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  if (cinemaState == CINEMA)
  {
    // do nothing
  }
  else if (cinemaState == BOX_PLAY)
  {
    if (Pizza::controller->clickedOutsidePause() == true)
    {
      box->tryJump(tiltMagnitude());
    }
    
    box->tryMove(tiltMagnitude());
  }
  else if (cinemaState == FULL_PLAY)
  {
    PizzaLevel::updateControls();
  }
}

void GLLevelTutorial::cinemaDone()
{
  cinemaState = BOX_PLAY;
  
  tapJump = new ImageEffect();
  tapJump->setImage(imgsetCache[cinemaTextSet][8]);
  tapJump->setAlpha(0.0);
  tapJump->setXY(PLAY_CX, 83.0);
  /*
   tapJump->script.wait(1.0);
   tapJump->enqueueX(new LinearFn(&tapJump->color.w, 1.0, 0.75));
   tapJump->script.wait(4.0);
   tapJump->enqueueX(new LinearFn(&tapJump->color.w, 0.0, 0.75));
   */
  addFrontAction(tapJump);
}

void GLLevelTutorial::boxDone()
{
  cinemaState = PIZZA_GROW;
  
  player->setXY(box->getX(), getGroundY());
  player->setScale(growScale, growScale);
  player->setAlpha(1.0);
  player->face.actionTrigger(PizzaFace::ROAR_1);
  
  tapJump->enqueueX(new LinearFn(&tapJump->color.w, 0.0, 0.75));  // this will remove it when done
  addAction(new StarParticles(box->getXY()));
  
  // this is programmed to end just after the grow is finished
  ActionQueue* glowScript = new ActionQueue();
  glowScript->enqueueX(new LinearFn(&player->face.glowAlpha, 1.0, growDur * (1.0 / 8.0)));
  glowScript->wait(growDur * (7.0 / 8.0));
  glowScript->enqueueX(new LinearFn(&player->face.glowAlpha, 0.0, growDur * (1.0 / 8.0)));
  addAction(glowScript);
  
  ActionQueue* musicScript = new ActionQueue();
  musicScript->enqueueX(new SoundVolumeFade(introMusic, 0.0, 0.25));
  musicScript->enqueueX(new StopSoundCommand(introMusic));
  musicScript->enqueueX(new PlaySoundCommand(soundCache[music]));
  addAction(musicScript);
  
  growScript.enqueueAddX(new ArcsineFn(&camera.xy.x, player->getX(), growDur), &objman.actions);
  growScript.wait(growDur);
}

void GLLevelTutorial::createWave()
{
  // spear side, spear vert
  switch (currWave)
  {
    case 0:
      crushEnemies = new ImageEffect();
      crushEnemies->setImage(imgsetCache[cinemaTextSet][10]);
      crushEnemies->setAlpha(0.0);
      crushEnemies->setXY(PLAY_CX, 131.0);
      crushEnemies->script.wait(1.0);
      crushEnemies->enqueueX(new LinearFn(&crushEnemies->color.w, 1.0, 0.75));
      crushEnemies->script.wait(2.5);
      crushEnemies->enqueueX(new LinearFn(&crushEnemies->color.w, 0.0, 0.75));
      addFrontAction(crushEnemies);
      
      createGroup(ENEMY_UNARMED, 2);
      break;
    case 1:
      createGroup(ENEMY_UNARMED, 3);
      break;
    case 2:
      createGroup(ENEMY_UNARMED, 2);
      break;
    case 3:
      createGroup(ENEMY_SPEARMAN_SIDE, 1);
      break;
  }
}

Logical GLLevelTutorial::pauseGUIEnabled()
{
  return alreadyBeaten();
}

void GLLevelTutorial::callback(ActionEvent* caller)
{
  if (caller == &growScript)
  {
    cinemaState = FULL_PLAY;
    
    player->endIntro();
    player->setScale(1.0, 1.0);
    startStanding(Point2(player->getX(), getGroundY()));
    player->face.idleTrigger(PizzaFace::BREATHE);
    
    for (Coord1 i = 0; i < skeles.count; ++i) skeles[i]->setActive();
    
    tiltRoll = new ImageEffect();
    tiltRoll->setImage(imgsetCache[cinemaTextSet][9]);
    tiltRoll->setAlpha(0.0);
    tiltRoll->setXY(PLAY_CX, 83.0);
    tiltRoll->script.wait(1.0);
    tiltRoll->enqueueX(new LinearFn(&tiltRoll->color.w, 1.0, 0.75));
    tiltRoll->script.wait(2.0);
    tiltRoll->enqueueX(new LinearFn(&tiltRoll->color.w, 0.0, 0.75));
    addFrontAction(tiltRoll);
  }
}

// ==================================== Pebble

Pebble::Pebble(Point2 center) :
VisRectangular(center, RM::random(imgsetCache[junkSet])),
Effect(),
ActionListener(),
visibleTimer(0.35, this),
fader(&color.w, 0.0, 0.15, this),
vel(RM::randf(-200.0, 200.0), RM::randf(-150.0, -250.0))
{
  fader.setActive(false);
}

void Pebble::updateMe()
{
  vel += Point2(0.0, 800.0) * RM::timePassed();
  addXY(vel * RM::timePassed());
  visibleTimer.update();
  fader.update();
}

void Pebble::redrawMe()
{
  drawMe();
}

void Pebble::callback(ActionEvent* caller)
{
  if (caller == &visibleTimer)
  {
    fader.setActive();
  }
  else if (caller == &fader)
  {
    done();
  }
}

// =============================== DustEffect ============================ //

DustEffect::DustEffect(Point2 impactXY, Point1 impactAngle) :
ActionEvent(),
dustParticles()
{
  dustParticles.addX(new DustParticle(impactXY, impactAngle + HALF_PI * 1.2, 0.75));
  dustParticles.addX(new DustParticle(impactXY, impactAngle - HALF_PI * 1.2, 0.75));
}

void DustEffect::updateMe()
{
  dustParticles.update();
  if (dustParticles.count == 0) done();
}

void DustEffect::redrawMe()
{
  dustParticles.redraw();
}

// =============================== DustParticle ============================ //

DustParticle::DustParticle(Point2 startXY, Point1 flyAngle, Point1 finalScale) :
VisRectangular(startXY, Point2(16.0, 16.0), imgsetCache[smokeSet][1]),
ActionEvent(),
origin(startXY),
distance(0.0),
angle(flyAngle),
scaler(&scale.x, finalScale, 0.75),
mover(&distance, 64.0, 0.5),
fader(&color.w, 0.0, 0.5, this)
{
  setScale(0.0, 0.0);
}

void DustParticle::updateMe()
{
  setXY(RM::pol_to_cart(distance, angle) + origin);
  scaler.update();
  scale.y = scale.x;
  mover.update();
  fader.update();
}

void DustParticle::redrawMe()
{
  drawMe();
}

void DustParticle::callback(ActionEvent* caller)
{
  done();
}


// ==================================== GLLevelSpearmen

GLLevelSpearmen::GLLevelSpearmen() :
  GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
}

void GLLevelSpearmen::createWave()
{
  // spear side, spear vert
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_SPEARMAN_SIDE);
      break;
    case 1:
      createGroup(ENEMY_SPEARMAN_SIDE);
      break;
    case 2:
      createGroup(ENEMY_SPEARMAN_VERT);
      break;
    case 3:
      setProb(ENEMY_SPEARMAN_SIDE, 2.0, 1, 1);
      setProb(ENEMY_SPEARMAN_VERT, 2.0, 1, 1);
      randomWave();
      break;
    case 7:
      setProb(ENEMY_SPEARMAN_VERT, 2.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(ENEMY_SPEARMAN_SIDE, 1, 2);
      setProb(ENEMY_SPEARMAN_VERT, 2, 2);
      randomWave();
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelGargoyle

GLLevelGargoyle::GLLevelGargoyle() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(ENEMY_GARGOYLE);
}

void GLLevelGargoyle::createWave()
{
  // spear side, spear vert, garg
  switch (currWave)
  {
    case 0:
    case 1:
      createGroup(ENEMY_GARGOYLE);
      break;
    case 2:
      createGroup(ENEMY_SPEARMAN_SIDE, 2);
      break;
    case 3:
      createGroup(ENEMY_SPEARMAN_VERT);
      break;
    case 4:
      createGroup(ENEMY_GARGOYLE, 1);
      break;
    case 5:
      setProb(ENEMY_SPEARMAN_SIDE, 2.0, 1, 1);
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 1, 2);
      setProb(ENEMY_GARGOYLE, 2.0, 1, 1);
      randomWave();
      break;
    case 10:
      setProb(ENEMY_SPEARMAN_SIDE, 2.0, 1, 2);
      randomWave();
      break;
    case 12:
      createGroup(ENEMY_GARGOYLE, 2);
      break;
    case 13:
      setProb(ENEMY_GARGOYLE, 1, 2);
      randomWave();
      break;
      
    default:
      randomWave();
      break;
  }
}

void GLLevelGargoyle::updateLevelAI()
{
  GladiatorLevel::updateLevelAI();
  
  if (alreadyBeaten() == false && tutShown == false &&
      currCombo == 1 && player->canJump() == true)
  {
    startTutorial(new EnemyJumpTutorial(this));
    tutShown = true;
  }
}

// ==================================== GLLevelSpiny

GLLevelSpiny::GLLevelSpiny() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SPINY);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
}

void GLLevelSpiny::createWave()
{
  // spiny, spear vert, garg
  switch (currWave)
  {
    case 0:
      tryCreateUnlockable(Point2(worldBox.right() - 64.0, 96.0), Coord2(0, 10));
      createGroup(ENEMY_SPINY);
      break;
    case 1:
      createGroup(ENEMY_SPINY);
      break;
    case 2:
      createGroup(ENEMY_SPEARMAN_VERT, 2);
      break;
    case 3:
      createGroup(ENEMY_SPINY);
      break;
    case 4:
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 1, 1);
      setProb(ENEMY_SPEARMAN_SIDE, 1.5, 1, 1);
      setProb(ENEMY_SPINY, 2.0, 1, 1);
      randomWave();
      break;
    case 9:
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 1, 2);
      setProb(ENEMY_SPEARMAN_SIDE, 1.5, 1, 2);
      setProb(ENEMY_SPINY, 2.0, 1, 2);
      randomWave();
      break;
    case 12:
      setProb(ENEMY_SPEARMAN_VERT, 1, 3);
      setProb(ENEMY_SPEARMAN_SIDE, 1, 3);
      setProb(ENEMY_SPINY, 1, 3);
      randomWave();
      break;
      
    default:
      randomWave();
      break;
  }
}

void GLLevelSpiny::pizzaDamaged()
{
  if (alreadyBeaten() == false && tutShown == false &&
      currWave == 1)
  {
    startTutorial(new SlamTutorial(this));
    tutShown = true;
  }
  
  GladiatorLevel::pizzaDamaged();
}

// ==================================== GLLevelSticky

GLLevelSticky::GLLevelSticky() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(ENEMY_SPINY);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
}

void GLLevelSticky::worldCreated()
{
  placeSticky(0.35);
  placeSticky(0.7);
}

void GLLevelSticky::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_SPEARMAN_VERT, 1);
      break;
    case 1:
      createGroup(ENEMY_SPINY, 1);
      break;
    case 2:
      createGroup(ENEMY_SPEARMAN_SIDE, 1);
      break;
    case 3:
      setProb(ENEMY_SPEARMAN_SIDE, 1.0, 1, 1);
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 1, 2);
      setProb(ENEMY_SPINY, 1.0, 1, 2);
      randomWave();
      break;
    case 6:
      createGroup(ENEMY_SPINY, 2);
      createGroup(ENEMY_SPEARMAN_SIDE);
      break;
    case 10:
      setProb(ENEMY_SPEARMAN_SIDE, 1.0, 1, 2);
      randomWave();
      break;
    case 15:
      createGroup(ENEMY_SPEARMAN_VERT, 2);
      createGroup(ENEMY_SPINY, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelOstrichV

GLLevelOstrichV::GLLevelOstrichV() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(ENEMY_GARGOYLE);
}

void GLLevelOstrichV::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_SKELOSTRICH_VERT);
      break;
      
    case 1:
      setProb(ENEMY_SPEARMAN_SIDE, 2.0, 2, 3);
      setProb(ENEMY_GARGOYLE, 2.0, 2, 2);
      setProb(ENEMY_SKELOSTRICH_VERT, 3.0, 1, 2);
      randomWave();
      break;
    case 5:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      createGroup(ENEMY_GARGOYLE, 1);
      break;
    case 10:
      setProb(ENEMY_SPEARMAN_SIDE, 3, 3);
      setProb(ENEMY_GARGOYLE, 2, 3);
      setProb(ENEMY_SKELOSTRICH_VERT, 2, 2);
      randomWave();
      break;
    case 11:
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      createGroup(ENEMY_GARGOYLE, 2);
      break;
    case 15:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      createGroup(ENEMY_GARGOYLE, 2);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelGiant

GLLevelGiant::GLLevelGiant() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_GIANT);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(ENEMY_SPINY);
}

void GLLevelGiant::worldCreated()
{
  // placeSticky(0.6);
}

void GLLevelGiant::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_GIANT);
      break;
    case 1:
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 2, 3);
      setProb(ENEMY_SPINY, 1.0, 1, 2);
      setProb(ENEMY_GIANT, 2.0, 1, 1);
      randomWave();
      break;
    case 5:
      setProb(ENEMY_SPINY, 2, 3);
      setProb(ENEMY_SPEARMAN_VERT, 3, 4);
      randomWave();
      break;
    case 9:
      createGroup(ENEMY_GIANT, 1);
      createGroup(ENEMY_SPEARMAN_VERT, 2);
      break;
    case 10:
      setProb(ENEMY_GIANT, 1, 2);
      randomWave();
      break;
    case 15:
      createGroup(ENEMY_GIANT, 2);
      createGroup(ENEMY_SPINY, 1);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelFlameskull

GLLevelFlameskull::GLLevelFlameskull() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_FLAMESKULL);
  enemiesEnabled.add(ENEMY_SPEARMAN_SIDE);
  enemiesEnabled.add(ENEMY_SPINY);
  enemiesEnabled.add(ENEMY_GIANT);
  
  musicFilename = "pirate.ima4";
}

void GLLevelFlameskull::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_FLAMESKULL);
      createGroup(ENEMY_SPEARMAN_SIDE);
      break;
    case 1:
      createGroup(ENEMY_SPINY);
      createGroup(ENEMY_SPEARMAN_SIDE);
      break;
    case 2:
      setProb(ENEMY_SPEARMAN_SIDE, 1.0, 2, 3);
      setProb(ENEMY_SPINY, 1.0, 2, 3);
      setProb(ENEMY_GIANT, 1.0, 1, 2);
      randomWave();
      break;
    case 3:
      createGroup(ENEMY_FLAMESKULL);
      createGroup(ENEMY_SPEARMAN_SIDE, 2);
      break;
    case 5:
      createGroup(ENEMY_FLAMESKULL);
      createGroup(ENEMY_SPINY, 2);
      break;
    case 7:
      createGroup(ENEMY_FLAMESKULL, 2);
      createGroup(ENEMY_SPEARMAN_SIDE, 3);
      break;
    case 8:
      createGroup(ENEMY_FLAMESKULL, 1);
      createGroup(ENEMY_GIANT, 1);
      break;
    case 10:
      setProb(ENEMY_SPEARMAN_SIDE, 2, 4);
      setProb(ENEMY_SPINY, 2, 4);
      randomWave();
      break;
    case 12:
      createGroup(ENEMY_SPINY, 2);
      createGroup(ENEMY_GIANT, 1);
      break;
    case 15:
      createGroup(ENEMY_FLAMESKULL, 2);
      createGroup(ENEMY_SPEARMAN_SIDE, 2);
      createGroup(ENEMY_SPINY, 1);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelOstrichH

GLLevelOstrichH::GLLevelOstrichH() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_SKELOSTRICH_SIDE);
  enemiesEnabled.add(ENEMY_GARGOYLE);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
  
  camSpawnPad = 196.0;
}


void GLLevelOstrichH::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_SKELOSTRICH_SIDE);
      break;
    case 1:
      createGroup(ENEMY_GARGOYLE, 2);
      break;
    case 2:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 1);
      break;
    case 3:
      setProb(ENEMY_GARGOYLE, 1.0, 1, 2);
      setProb(ENEMY_SKELOSTRICH_VERT, 1.0, 1, 2);
      setProb(ENEMY_SKELOSTRICH_SIDE, 2.0, 1, 2);
      randomWave();
      break;
    case 7:
      createGroup(ENEMY_GARGOYLE, 1);
      createGroup(ENEMY_SKELOSTRICH_SIDE, 1);
      break;
    case 8:
      setProb(ENEMY_GARGOYLE, 2, 3);
      setProb(ENEMY_SKELOSTRICH_VERT, 2, 2);
      setProb(ENEMY_SKELOSTRICH_SIDE, 2, 2);
      randomWave();
      break;
    case 11:
      createGroup(ENEMY_GARGOYLE, 1);
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      break;
    case 13:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 3);
      break;
    case 15:
      createGroup(ENEMY_GARGOYLE, 1);
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelCupid

GLLevelCupid::GLLevelCupid() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_CUPID);
  enemiesEnabled.add(ENEMY_GIANT);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
}

void GLLevelCupid::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_CUPID);
      break;
    case 1:
      createGroup(ENEMY_GIANT);
      break;
    case 2:
      createGroup(ENEMY_CUPID);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 3:
      setProb(ENEMY_GIANT, 1.0, 1, 2);
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 2, 3);
      setProb(ENEMY_CUPID, 2.0, 1, 2);
      randomWave();
      break;
    case 5:
      createGroup(ENEMY_CUPID);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 7:
      createGroup(ENEMY_SPEARMAN_VERT, 1);
      createGroup(ENEMY_CUPID, 1);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 8:
      setProb(ENEMY_SPEARMAN_VERT, 3, 3);
      setProb(ENEMY_GIANT, 1, 2);
      setProb(ENEMY_CUPID, 2, 3);
      randomWave();
      break;
    case 11:
      createGroup(ENEMY_GIANT, 1);
      createGroup(ENEMY_SPEARMAN_VERT, 2);
      break;
    case 12:
      createGroup(ENEMY_GIANT, 1);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 15:
      createGroup(ENEMY_GIANT, 1);
      createGroup(ENEMY_CUPID, 2);
      break;
    default:
      randomWave();
      break;
  }
}


// ==================================== GLLevelRobot

GLLevelRobot::GLLevelRobot() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_ROBOT);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
  enemiesEnabled.add(ENEMY_GARGOYLE);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
}

void GLLevelRobot::worldCreated()
{
  
}

void GLLevelRobot::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_ROBOT);
      break;
    case 1:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      break;
    case 2:
      createGroup(ENEMY_ROBOT);
      break;
    case 3:
      createGroup(ENEMY_GARGOYLE, 3);
      break;
    case 4:
      setProb(ENEMY_GARGOYLE, 1.0, 2, 4);
      setProb(ENEMY_SKELOSTRICH_VERT, 1.0, 2, 3);
      setProb(ENEMY_ROBOT, 2.0, 1, 2);
      randomWave();
      break;
    case 7:
      createGroup(ENEMY_GARGOYLE, 1);
      createGroup(ENEMY_ROBOT, 1);
      break;
    case 9:
      setProb(ENEMY_GARGOYLE, 3, 4);
      setProb(ENEMY_SKELOSTRICH_VERT, 3, 3);
      setProb(ENEMY_ROBOT, 2.0, 2, 2);
      randomWave();
      break;
    case 10:
      createGroup(ENEMY_GARGOYLE, 2);
      createGroup(ENEMY_FLAMESKULL, 1);
      break;
    case 12:
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      createGroup(ENEMY_ROBOT, 1);
      break;
    case 15:
      createGroup(ENEMY_GARGOYLE, 1);
      createGroup(ENEMY_ROBOT, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelWisp

GLLevelWisp::GLLevelWisp() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_WISP);
  enemiesEnabled.add(ENEMY_SPINY);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_SIDE);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
}

void GLLevelWisp::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_WISP);
      break;
    case 1:
      createGroup(ENEMY_WISP);
      break;
    case 2:
      setProb(ENEMY_SPINY, 1.0, 2, 4);
      setProb(ENEMY_SKELOSTRICH_SIDE, 1.0, 2, 3);
      setProb(ENEMY_WISP, 1.3, 1, 1);
      randomWave();
      break;
    case 4:
      createGroup(ENEMY_WISP);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 7:
      createGroup(ENEMY_WISP);
      createGroup(ENEMY_SPINY);
      break;
    case 8:
      setProb(ENEMY_SPINY, 1.0, 3, 5);
      setProb(ENEMY_SKELOSTRICH_SIDE, 1.0, 2, 4);
      randomWave();
      break;
    case 10:
      createGroup(ENEMY_SPINY, 3);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 11:
      createGroup(ENEMY_WISP);
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      break;
    case 15:
      createGroup(ENEMY_FLAMESKULL);
      createGroup(ENEMY_WISP, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelGrav

GLLevelGrav::GLLevelGrav() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_CUPID);
  enemiesEnabled.add(ENEMY_ROBOT);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
  
  musicFilename = "bounce.ima4";
}

void GLLevelGrav::worldCreated()
{
  RMPhysics->set_gravity_rm(RMPhysics->get_gravity_rm() * Point2(1.0, 0.65));
}

void GLLevelGrav::createWave()
{
  switch (currWave)
  {
    case 0:
      setProb(ENEMY_CUPID, 1.0, 2, 4);
      setProb(ENEMY_ROBOT, 1.0, 2, 3);
      setProb(ENEMY_SKELOSTRICH_VERT, 1.0, 2, 3);
      randomWave();
      break;
    case 3:
      createGroup(ENEMY_FLAMESKULL);
      randomWave();
      break;
    case 5:
      createGroup(ENEMY_CUPID);
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      break;
    case 6:
      createGroup(ENEMY_CUPID, 2);
      createGroup(ENEMY_FLAMESKULL, 2);
      break;
    case 9:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 10:
      createGroup(ENEMY_ROBOT);
      createGroup(ENEMY_CUPID, 2);
      break;
    case 12:
      createGroup(ENEMY_FLAMESKULL, 2);
      break;
    case 15:
      createGroup(ENEMY_FLAMESKULL, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelNaga

GLLevelNaga::GLLevelNaga() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_NAGA);
  enemiesEnabled.add(ENEMY_GARGOYLE);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
}

void GLLevelNaga::worldCreated()
{
  RMPhysics->set_gravity_rm(RMPhysics->get_gravity_rm() * Point2(1.0, 0.65));
}

void GLLevelNaga::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_NAGA);
      break;
    case 1:
      createGroup(ENEMY_NAGA);
      break;
    case 2:
      setProb(ENEMY_GARGOYLE, 1.0, 2, 4);
      setProb(ENEMY_SKELOSTRICH_VERT, 1.0, 2, 3);
      setProb(ENEMY_NAGA, 2.0, 1, 1);
      randomWave();
      break;
    case 7:
      createGroup(ENEMY_NAGA);
      createGroup(ENEMY_GARGOYLE);
      break;
    case 8:
      setProb(ENEMY_GARGOYLE, 1.0, 3, 5);
      setProb(ENEMY_SKELOSTRICH_VERT, 1.0, 2, 4);
      randomWave();
      break;
    case 11:
      createGroup(ENEMY_NAGA);
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      break;
    case 15:
      createGroup(ENEMY_NAGA, 2);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelMix1

GLLevelMix1::GLLevelMix1() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_CUPID);
  enemiesEnabled.add(ENEMY_SKELOSTRICH_SIDE);
  enemiesEnabled.add(ENEMY_GIANT);
}

void GLLevelMix1::createWave()
{
  switch (currWave)
  {
    case 0:
      setProb(ENEMY_CUPID, 1.0, 2, 4);
      setProb(ENEMY_SKELOSTRICH_SIDE, 1.0, 2, 3);
      setProb(ENEMY_GIANT, 2.0, 1, 2);
      randomWave();
      break;
    case 4:
      createGroup(ENEMY_CUPID, 2);
      createGroup(ENEMY_GIANT, 2);
      break;
    case 7:
      setProb(ENEMY_CUPID, 1.0, 3, 5);
      setProb(ENEMY_SKELOSTRICH_SIDE, 1.0, 3, 4);
      setProb(ENEMY_GIANT, 2.0, 2, 3);
      randomWave();
      break;
    case 8:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      createGroup(ENEMY_GIANT, 2);
      break;
    case 12:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      createGroup(ENEMY_CUPID, 2);
      break;
    case 15:
      createGroup(ENEMY_GIANT);
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      createGroup(ENEMY_CUPID);
      break;
      
    default:
      randomWave();
      break;
  }
  
  createGroup(ENEMY_FLAMESKULL);
}

// ==================================== GLLevelStickyMix

GLLevelStickyMix::GLLevelStickyMix() :
GladiatorLevel()
{
  enemiesEnabled.add(ENEMY_ROBOT);
  enemiesEnabled.add(ENEMY_WISP);
  enemiesEnabled.add(ENEMY_NAGA);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
}

void GLLevelStickyMix::worldCreated()
{
  // placeSticky(0.1);
  // placeSticky(0.4);
  // placeSticky(0.75);
}

void GLLevelStickyMix::createWave()
{
  
  switch (currWave)
  {
    case 0:
      setProb(ENEMY_ROBOT, 1.0, 2, 3);
      setProb(ENEMY_WISP, 1.0, 1, 1);
      setProb(ENEMY_NAGA, 1.0, 1, 1);
      randomWave();
      break;
    case 2:
      createGroup(ENEMY_FLAMESKULL);
      randomWave();
      break;
    case 4:
      createGroup(ENEMY_WISP);
      createGroup(ENEMY_ROBOT);
      break;
    case 5:
      createGroup(ENEMY_FLAMESKULL);
      randomWave();
      break;
    case 7:
      setProb(ENEMY_ROBOT, 3, 3);
      randomWave();
      break;
    case 8:
      createGroup(ENEMY_NAGA);
      createGroup(ENEMY_ROBOT);
      break;
    case 10:
      createGroup(ENEMY_FLAMESKULL);
      randomWave();
      break;
    case 12:
      createGroup(ENEMY_FLAMESKULL, 2);
      randomWave();
      break;
    case 15:
      createGroup(ENEMY_FLAMESKULL);
      createGroup(ENEMY_NAGA);
      createGroup(ENEMY_WISP);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelRabbit

GLLevelRabbit::GLLevelRabbit() :
GladiatorLevel()
{
  musicFilename = "gladcloud.ima4";
  enemiesEnabled.add(ENEMY_RABBIT);
  enemiesEnabled.add(ENEMY_SPEARMAN_VERT);
  enemiesEnabled.add(ENEMY_GIANT);
}

void GLLevelRabbit::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_RABBIT);
      break;
    case 1:
      createGroup(ENEMY_RABBIT);
      break;
    case 2:
      setProb(ENEMY_SPEARMAN_VERT, 3.0, 2, 3);
      setProb(ENEMY_GIANT, 1.0, 1, 2);
      setProb(ENEMY_RABBIT, 2.0, 1, 1);
      randomWave();
      break;
    case 6:
      createGroup(ENEMY_RABBIT, 2);
      break;
    case 7:
      createGroup(ENEMY_RABBIT);
      createGroup(ENEMY_SPEARMAN_VERT);
      break;
    case 8:
      setProb(ENEMY_SPEARMAN_VERT, 1.0, 3, 4);
      setProb(ENEMY_GIANT, 1.0, 1, 2);
      setProb(ENEMY_RABBIT, 1.0, 2, 2);
      randomWave();
      break;
    case 11:
      createGroup(ENEMY_RABBIT, 2);
      createGroup(ENEMY_GIANT);
      break;
    case 15:
      createGroup(ENEMY_RABBIT, 1);
      createGroup(ENEMY_GIANT, 1);
      createGroup(ENEMY_SPEARMAN_VERT, 1);
      break;
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelTail

GLLevelTail::GLLevelTail() :
GladiatorLevel()
{
  musicFilename = "gladcloud.ima4";
  enemiesEnabled.add(ENEMY_SKELOSTRICH_SIDE);
  enemiesEnabled.add(ENEMY_CUPID);
  enemiesEnabled.add(ENEMY_TAIL);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
}

void GLLevelTail::createWave()
{
  
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_TAIL);
      break;
    case 1:
      createGroup(ENEMY_TAIL);
      break;
    case 2:
      createGroup(ENEMY_CUPID, 2);
      break;
    case 3:
      createGroup(ENEMY_CUPID, 1);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 4:
      createGroup(ENEMY_TAIL, 1);
      break;
    case 5:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      break;
    case 6:
      createGroup(ENEMY_TAIL, 2);
      break;
    case 7:
      createGroup(ENEMY_CUPID, 3);
      break;
    case 8:
      createGroup(ENEMY_SKELOSTRICH_SIDE);
      createGroup(ENEMY_TAIL);
      break;
    case 9:
      createGroup(ENEMY_SKELOSTRICH_SIDE, 2);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 10:
      createGroup(ENEMY_CUPID, 2);
      break;
    case 11:
      createGroup(ENEMY_CUPID);
      createGroup(ENEMY_SKELOSTRICH_SIDE);
      break;
    case 12:
      createGroup(ENEMY_TAIL);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 13:
      createGroup(ENEMY_FLAMESKULL, 2);
      createGroup(ENEMY_CUPID, 2);
      break;
    case 14:
      createGroup(ENEMY_TAIL);
      createGroup(ENEMY_CUPID);
      break;
    case 15:
      createGroup(ENEMY_TAIL, 3);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ==================================== GLLevelDisk

GLLevelDisk::GLLevelDisk() :
GladiatorLevel()
{
  musicFilename = "market.ima4";
  
  enemiesEnabled.add(ENEMY_SKELOSTRICH_VERT);
  enemiesEnabled.add(ENEMY_ROBOT);
  enemiesEnabled.add(ENEMY_DISK);
  enemiesEnabled.add(ENEMY_FLAMESKULL);
}

void GLLevelDisk::createWave()
{
  switch (currWave)
  {
    case 0:
      createGroup(ENEMY_DISK);
      break;
    case 1:
      createGroup(ENEMY_DISK, 2);
      break;
    case 2:
      createGroup(ENEMY_ROBOT, 2);
      break;
    case 3:
      createGroup(ENEMY_DISK, 4);
      break;
    case 4:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      break;
    case 5:
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      createGroup(ENEMY_FLAMESKULL);
      break;
    case 6:
      createGroup(ENEMY_DISK, 5);
      break;
    case 7:
      createGroup(ENEMY_ROBOT, 1);
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      break;
    case 8:
      createGroup(ENEMY_DISK, 4);
      createGroup(ENEMY_FLAMESKULL, 2);
      break;
    case 9:
      createGroup(ENEMY_SKELOSTRICH_VERT, 3);
      break;
    case 10:
      createGroup(ENEMY_ROBOT, 2);
      createGroup(ENEMY_DISK, 3);
      break;
    case 11:
      createGroup(ENEMY_ROBOT, 3);
      break;
    case 12:
      createGroup(ENEMY_SKELOSTRICH_VERT, 3);
      break;
    case 13:
      createGroup(ENEMY_SKELOSTRICH_VERT, 2);
      createGroup(ENEMY_FLAMESKULL, 1);
      break;
    case 14:
      createGroup(ENEMY_DISK, 6);
      break;
    case 15:
      createGroup(ENEMY_SKELOSTRICH_VERT, 1);
      createGroup(ENEMY_DISK, 3);
      createGroup(ENEMY_FLAMESKULL, 1);
      createGroup(ENEMY_ROBOT, 1);
      break;
      
    default:
      randomWave();
      break;
  }
}

// ==================================== EndingScript

EndingScript::EndingScript(GLLevelEnding* setLevel) :
  level(setLevel),

  ActionEvent(),
  ActionListener(),

  script(this),
  actions(),

  textLines(4)
{
  textLines.add(VisRectangular(Point2(240.0, 30.0), imgsetCache[endingVerseSet][0]));
  textLines.add(VisRectangular(Point2(240.0, 67.0), imgsetCache[endingVerseSet][1]));
  textLines.add(VisRectangular(Point2(240.0, 99.0), imgsetCache[endingVerseSet][2]));
  textLines.add(VisRectangular(Point2(240.0, 131.0), imgsetCache[endingVerseSet][3]));
  
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].setAlpha(0.0);
  
  Point1 fadeDur = 0.75;
  
  // 0.0
  script.wait(1.0);
  
  // 1.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 3.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 5.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 7.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 9.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 11.0
  script.enqueueX(new SetValueCommand<Image*>(&textLines[0].image, imgsetCache[endingVerseSet][4]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[1].image, imgsetCache[endingVerseSet][5]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[2].image, imgsetCache[endingVerseSet][6]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[3].image, imgsetCache[endingVerseSet][7]));
  
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 13.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 15.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 17.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 19.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.wait(1.0);
  
  // 20.0
}

void EndingScript::updateMe()
{
  actions.update();
  script.update();
}

void EndingScript::redrawMe()
{
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].drawMe();
  actions.redraw();
}

void EndingScript::callback(ActionEvent* caller)
{
  done();
}

// ==================================== GLLevelEnding

GLLevelEnding::GLLevelEnding() :
GladiatorLevel(),

endState(PLAYING_VERSES),
currBoneType(0)
{
  enemiesEnabled.add(ENEMY_END_SKULLS);
  musicFilename = "ending.ima4";
}

void GLLevelEnding::preload()
{
  background = createBackgroundStd(WORLD_GRAVEYARD);
}

void GLLevelEnding::loadRes()
{
  background->load();
  addImgNorm(bgLayerSet, "gladiator_graveyard", Point2(0.0, 1.0));
  
  for (Char boneChar = 'a'; boneChar <= 'p'; ++boneChar)
  {
    String1 name("ending_bones_$", '$', boneChar);
    addImgNorm(endingBoneSet, name.as_cstr());
  }
  
  for (Char verseChar = 'a'; verseChar <= 'd'; ++verseChar)
  {
    String1 name1("ending_verse1_$&", '$', verseChar);
    addImgNorm(endingVerseSet, name1.as_cstr(), Point2(0.5, 0.5));
  }
  
  for (Char verseChar = 'a'; verseChar <= 'd'; ++verseChar)
  {
    String1 name2("ending_verse2_$&", '$', verseChar);
    addImgNorm(endingVerseSet, name2.as_cstr(), Point2(0.5, 0.5));
  }
  
  loadTilesNorm(endingSkullSet, "ending_skull", 1, 3, Point2(0.5, 0.5));
  loadImgNorm(endingTheEnd, "ending_end&", Point2(0.5, 0.5));
  
  imgCache[shadowSmall] = Image::create_centered(imgStr("shadow_small"));
  imgCache[shadowMedium] = Image::create_centered(imgStr("shadow_med"));
  imgCache[shadowLarge] = Image::create_centered(imgStr("shadow_large"));
  
  endingMusic = Sound::create("the_end.ima4");
  endingMusic->repeats(false);
}

void GLLevelEnding::unloadRes()
{
  delete endingMusic;
  endingMusic = NULL;
}

void GLLevelEnding::loadMe()
{
  GladiatorLevel::loadMe();
  
  soundCache[music]->repeats(false);
  
  // these will get cleaned up by b2d
  b2Body* circleBody1 = Physical::create_body(Point2(worldBox.left(), groundY), false);
  b2Fixture* circleFix1 = Physical::create_circle_fixture(circleBody1, 64.0);
  
  b2Body* circleBody2 = Physical::create_body(Point2(worldBox.right(), groundY), false);
  b2Fixture* circleFix2 = Physical::create_circle_fixture(circleBody2, 64.0);
  
  EndingScript* endScript = new EndingScript(this);
  endScript->setListener(this);
  addFrontAction(endScript);
}

void GLLevelEnding::createWave()
{
  Point2 posStd(spawnXStd(currSide), RM::randf(-128.0, 128.0));
  
  if (endState == PLAYING_VERSES)
  {
    Coord1 numBones = 5;
    for (Coord1 i = 0; i < numBones; ++i)
    {
      EndingDebris* bone = new EndingDebris(currBoneType);
      bone->setXY(posStd);
      addEnemy(bone);
      
      currBoneType++;
      currBoneType %= imgsetCache[endingBoneSet].count;
    }
  }
  else if (endState == FIGHTING_BIGSKULL)
  {
    EndingSkull* boss = new EndingSkull();
    boss->setXY(posStd);
    addEnemy(boss);
  }
}

void GLLevelEnding::enemyDefeated(PizzaGO* enemy)
{
  if (endState == FIGHTING_BIGSKULL)
  {
    endState = SHOWING_END;
    levelState = LEVEL_PASSIVE;
    
    soundCache[music]->stop();
    endingMusic->play();
    
    Point1 dur = 3.0;
    
    ImageEffect* youwin = new ImageEffect(imgCache[endingTheEnd]);
    youwin->setXY(camera.transform(enemy->getXY()));
    youwin->setScale(0.05, 0.05);
    youwin->setListener(this);
    
    youwin->addX(new LinearFn(&youwin->rotation, TWO_PI * 3.0, dur));
    youwin->addX(new CosineFn(&youwin->scale.x, 1.0, dur * 0.5));
    youwin->addX(new CosineFn(&youwin->scale.y, 1.0, dur * 0.5));
    youwin->addX(new LinearFn(&youwin->xy.y, PLAY_CY, dur));
    youwin->addX(new LinearFn(&youwin->xy.x, PLAY_CX, dur));
    
    // this will make it fade in just AFTER youwin is done and click is possible
    ActionQueue* scoreScript = new ActionQueue();
    scoreScript->enqueueX(new TimerFn(dur + 1.0));
    scoreScript->enqueueX(new LinearFn(&endingScoreText.color.w, 1.0, 1.0));
    
    addFrontAction(new DeAccelFn(&worldTimeMult, 0.25, 1.0));
    addFrontAction(new ScreenFade(dur, true));
    addFrontAction(youwin);
    addFrontAction(scoreScript);
    addFrontAction(new TimerFn(dur + 0.1, this));  // this causes the callback
  }
}

void GLLevelEnding::updateLevelAI()
{
  if (endState == READY_FOR_CLICK &&
      Pizza::controller->getAnyConfirmStatus(JUST_PRESSED) == true)
  {
    endState = TOTALLY_FINISHED;
    RMSceneMan->sceneTransition(new TitleScene());
  }
  
  GladiatorLevel::updateLevelAI();
}

void GLLevelEnding::callback(ActionEvent* caller)
{
  if (endState == PLAYING_VERSES)
  {
    endState = FIGHTING_BIGSKULL;
  }
  else if (endState == SHOWING_END)
  {
    endState = READY_FOR_CLICK;
    Pizza::playerCash += Pizza::currGameVars[VAR_CASH];
    Pizza::savePlayer();
  }
}

// ==================================== LiteEndingScript

LiteEndingScript::LiteEndingScript(GLLevelLiteEnding* setLevel) :
ActionEvent(),
ActionListener(),

script(this),
actions(),

textLines(4)
{
  textLines.add(VisRectangular(Point2(240.0, 30.0), imgsetCache[endingVerseSet][0]));
  textLines.add(VisRectangular(Point2(240.0, 68.0), imgsetCache[endingVerseSet][1]));
  textLines.add(VisRectangular(Point2(240.0, 95.0), imgsetCache[endingVerseSet][2]));
  textLines.add(VisRectangular(Point2(240.0, 130.0), imgsetCache[endingVerseSet][3]));
  
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].setAlpha(0.0);
  
  Point1 fadeDur = 0.75;
  
  // 0.0
  script.wait(1.0);
  
  // 1.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 3.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 5.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 7.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 9.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 11.0
  script.enqueueX(new SetValueCommand<Image*>(&textLines[0].image, imgsetCache[endingVerseSet][4]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[1].image, imgsetCache[endingVerseSet][5]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[2].image, imgsetCache[endingVerseSet][6]));
  script.enqueueX(new SetValueCommand<Image*>(&textLines[3].image, imgsetCache[endingVerseSet][7]));
  script.enqueueX(new SetValueCommand<Point1>(&textLines[0].xy.y, 30.0));
  script.enqueueX(new SetValueCommand<Point1>(&textLines[1].xy.y, 66.0));
  script.enqueueX(new SetValueCommand<Point1>(&textLines[2].xy.y, 94.0));
  script.enqueueX(new SetValueCommand<Point1>(&textLines[3].xy.y, 126.0));
  
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 13.0
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 15.0
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 17.0
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 1.0, fadeDur), &actions);
  script.wait(2.0);
  
  // 19.0
  script.enqueueAddX(new LinearFn(&textLines[0].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[1].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[2].color.w, 0.0, fadeDur), &actions);
  script.enqueueAddX(new LinearFn(&textLines[3].color.w, 0.0, fadeDur), &actions);
  script.wait(1.0);
  
  // 20.0
}

void LiteEndingScript::updateMe()
{
  actions.update();
  script.update();
}

void LiteEndingScript::redrawMe()
{
  for (Coord1 i = 0; i < textLines.count; ++i) textLines[i].drawMe();
  actions.redraw();
}

void LiteEndingScript::callback(ActionEvent* caller)
{
  done();
}

// ==================================== GLLevelLiteEnding

GLLevelLiteEnding::GLLevelLiteEnding() :
GladiatorLevel(),

endState(PLAYING_VERSES),
currBoneType(0)
{
  musicFilename = "free_end.ima4";
}

void GLLevelLiteEnding::preload()
{
  background = createBackgroundStd(WORLD_GRAVEYARD);
}

void GLLevelLiteEnding::loadRes()
{
  background->load();
  addImgNorm(bgLayerSet, "gladiator_graveyard", Point2(0.0, 1.0));
  
  for (Char verseChar = 'a'; verseChar <= 'd'; ++verseChar)
  {
    String1 name1("free_end_verse1_$&", '$', verseChar);
    addImgNorm(endingVerseSet, name1.as_cstr(), Point2(0.5, 0.5));
  }
  
  for (Char verseChar = 'a'; verseChar <= 'd'; ++verseChar)
  {
    String1 name2("free_end_verse2_$&", '$', verseChar);
    addImgNorm(endingVerseSet, name2.as_cstr(), Point2(0.5, 0.5));
  }
  
  imgCache[shadowSmall] = Image::create_centered(imgStr("shadow_small"));
  imgCache[shadowMedium] = Image::create_centered(imgStr("shadow_med"));
  imgCache[shadowLarge] = Image::create_centered(imgStr("shadow_large"));
  
  loadEnemy(ENEMY_SPEARMAN_SIDE);
  loadEnemy(ENEMY_SPEARMAN_VERT);
}

void GLLevelLiteEnding::unloadRes()
{
  
}

void GLLevelLiteEnding::loadMe()
{
  GladiatorLevel::loadMe();
  
  soundCache[music]->repeats(false);
  
  LiteEndingScript* endScript = new LiteEndingScript(this);
  endScript->setListener(this);
  addFrontAction(endScript);
}

void GLLevelLiteEnding::createWave()
{
  if (endState == PLAYING_VERSES)
  {
    switch (currWave)
    {
      case 0:
        setProb(ENEMY_SPEARMAN_SIDE, 1.0, 1, 1);
        setProb(ENEMY_SPEARMAN_VERT, 1.0, 1, 1);
        randomWave();
        break;
      default:
        randomWave();
        break;
    }
  }
}

void GLLevelLiteEnding::enemyDefeated(PizzaGO* enemy)
{
  if (endState == FIGHTING_FINAL)
  {
    levelState = LEVEL_PASSIVE;
    
    addFrontAction(ScreenFade::SceneOut(1.0));
    
    ActionQueue* endScript = new ActionQueue();
    endScript->wait(1.5);
    endScript->enqueueX(new SceneTransitionCommand(new UpgradeScene(false)));
    addFrontAction(endScript);
  }
}

void GLLevelLiteEnding::updateLevelAI()
{
  if (endState == WAITING_FOR_FINAL && enemyActive() == true)
  {
    endState = FIGHTING_FINAL;
  }
  
  GladiatorLevel::updateLevelAI();
}

void GLLevelLiteEnding::callback(ActionEvent* caller)
{
  if (endState == PLAYING_VERSES)
  {
    endState = WAITING_FOR_FINAL;
  }
}

// ==================================== SumoLevel ================================ //

SumoLevel::SumoLevel() :
PizzaLevel(),
boss(NULL),
camYPad(64.0),

tauntStartTimer(0.5, this),
bombTimer(4.0, true, this),
arrowTimer(4.0, true, this),
grassTouchLeft(30),

coinTimer(10.0, this),
coinGroupsLeft(6),

useRods(false),
rodList(10),
rodMoveVals(10),
rodMoveData(10),

windyAchTried(false),
winTimer(4.5, this)
{
  bombTimer.setInactive();
  arrowTimer.setInactive();
  winTimer.setInactive();
  
  endpoint.set(0.0, 352.0);
  
  musicFilename = "boss.ima4";
}

void SumoLevel::preload()
{
  background = createBackgroundStd(levelID);
}

void SumoLevel::loadRes()
{
  background->load();
  
  String1 bossBase("boss", worldStrs[levelID]);
  imgCache[sumoBoss] = Image::create_centered(imgStr(bossBase.as_cstr()));
  dwarpCache[sumoWarp] = new DWarpSequence(String1(bossBase.as_cstr(), ".dwarp2").as_cstr(), true);
  
  String1 terrainStr(16);
  terrainStr = modeStrs[LEVEL_SUMO];
  terrainStr += '_';
  terrainStr += worldStrs[levelID];
  addImgAbs(sumoTerrainSet, terrainStr.as_cstr());
  
  Image::create_tiles_abs(imgStr("bomb"), imgsetCache[bombSet], 2, 1, Point2(26.0, 42.0));
  Image::create_tiles(imgStr("explosion"), imgsetCache[bombExplodeSet], 3, 1, true, true);
  
  imgCache[warningV] = Image::create_handled_norm(imgStr("warning_v"), HANDLE_TC);
  imgCache[sumoKnife] = Image::create_handled_abs(imgStr("knife"), Point2(44.0, 16.0));
  
  loadImgNorm(warningH, "warning_h", HANDLE_RC);
  imgCache[alienLaser] = Image::create_handled_abs(imgStr("laser"), Point2(102.0, 6.0), true);
  
  imgCache[shockBolt] = Image::create_handled_norm(imgStr("bolt"), Point2(0.5, 0.0));
  imgCache[shockBolt]->repeats();
  
  loadImgNorm(sumoMadRing, "boss_power", HANDLE_C);
  loadImgAbs(sumoFireball, "boss_fireball", Point2(78.0, 16.0));
  loadTilesNorm(sumoGrassSet, "grass_particle", 4, 1, HANDLE_C);
  loadImgNorm(sumoGust, "wind_particle", HANDLE_C);
  loadImgAbs(sumoGem, "bossmarket_gem", Point2(80.0, 160.0));
  loadImgAbs(sumoGemGlow, "bossmarket_gem_glow", Point2(80.0, 160.0));
  
  Image::create_tiles(imgStr("meat"), imgsetCache[junkSet], 3, 1);
  imgsetCache[junkSet][0]->handle.set(23.0, 38.0);
  imgsetCache[junkSet][1]->handle.set(24.0, 32.0);
  imgsetCache[junkSet][2]->handle.set(38.0, 35.0);
  
  loadSound(sumoAngrySound, "sumoangry.wav");
  loadSound(knifeSound, "knife.wav");
  loadSound(laserSound, "laser.wav");
  
  if (levelID != SUMO_MEAT)
  {
    loadSound(bossTaunt, "boss_taunt.wav");
    loadSound(bossSink, "boss_die.wav");
  }
  else
  {
    String1 tauntStr("boss_!_taunt.wav");
    tauntStr.replace('!', worldStrs[levelID]);
    loadSound(bossTaunt, tauntStr.as_cstr());
    
    String1 sinkStr("boss_!_die.wav");
    sinkStr.replace('!', worldStrs[levelID]);
    loadSound(bossSink, sinkStr.as_cstr());
  }
}

void SumoLevel::unloadRes()
{
  
}

void SumoLevel::loadMe()
{
  createBoss();  // this is before the arena so that the arena can set its coords
  
  Point2 arenaEnds = createArena();
  worldBox.xy.x = arenaEnds.x;
  worldBox.wh.x = arenaEnds.y - arenaEnds.x;
  
  boss->updateFromWorld();
  
  // the screenCX should be 0.0 if the bg should be lined up with 0.0 during zoom out
  camera.handle.set(PLAY_CX, 320.0);
}

void SumoLevel::createBoss()
{
  boss = new SumoBoss();
  boss->setXY(1024.0, 112.0);
  addEnemy(boss);
}

void SumoLevel::updateCamera()
{
  Logical enemyAnywhere = enemyActive();
  Point1 closestX = boss->getX() - player->getX();
  
  zoomToMinY(std::min(player->collisionBox().xy.y, boss->collisionBox().xy.y) + camYPad);
  updateFacingStd(enemyAnywhere, closestX);
  enemyCameraStd(enemyAnywhere, closestX);
}

void SumoLevel::updateStatus()
{
  Point1 botLoseY = RM::lerp(boss->collisionBox().top(), boss->collisionBox().bottom(), 0.75);
  
  if (botLoseY > PLAY_H)
  {
    levelState = LEVEL_WIN;
    
    shaker.timedShake(10.0);
    player->body->SetActive(false);
    
    Logical longLose = levelID == SUMO_MEAT;
    winTimer.setActive();
    if (longLose == true) winTimer.reset(7.0);
    
    // the standard win sequence will fade it out again and stop it
    addAction(new SoundVolumeFade(soundCache[music], 0.0, winTimer.stopTime * 1.0));
    ResourceManager::playBossSink();
    
    if (levelID == SUMO_COMPUTER && Pizza::currGameVars[VAR_DAMAGE] == 0)
    {
      Pizza::scoreMan->earnedAchievement(ACH_DEBUGGER);
    }
    
    boss->startLoseSequence(longLose);
  }
  else if (player->collisionBox().xy.y > PLAY_H)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void SumoLevel::updateLevelAI()
{
  if (levelState == LEVEL_PLAY)
  {
    tauntStartTimer.update();
    arrowTimer.update();
    bombTimer.update();
    coinTimer.update();
    checkWindyAch();
  }
  
  winTimer.update();
  
  if (useRods == true) updateRods();
}

void SumoLevel::checkWindyAch()
{
  if (windyAchTried == false && player->getY() < -550.0)
  {
    Pizza::scoreMan->earnedAchievement(ACH_WINDY);
    windyAchTried = true;
  }
}

void SumoLevel::reportSurface(Coord1 surfaceBits)
{
  PizzaLevel::reportSurface(surfaceBits);
  if ((playerMotionFlags & PizzaGO::SLOW_BIT) != 0)
  {
    grassTouchLeft--;
  }
  
  if (grassTouchLeft <= 0)
  {
    addAction(new GrassSplash(Point2(player->getX(), player->collisionCircle().bottom() - 8.0)));
    grassTouchLeft = RM::randi(10, 30);
  }
}

void SumoLevel::updateRods()
{
  for (Coord1 i = 0; i < rodList.count; ++i)
  {
    Point1 oldRodVal = rodMoveVals[i];
    RM::bounce_arcsine(rodMoveVals[i], rodMoveData[i], Point2(0.0, 128.0), RM::timePassed());
    rodList[i]->set_velocity(-Point2(0.0, rodMoveVals[i] - oldRodVal) / RM::timePassed());
  }
}

void SumoLevel::addRod(TerrainQuad* rod)
{
  rodList.add(rod);
  rodMoveVals.add(0.0);
  rodMoveData.add(Point2(randGen.randf(0.0, 1.0), 1.0));
}

Coord1 SumoLevel::calcWinBonus()
{
  return 1250 + 250 * levelID;
}

void SumoLevel::callback(ActionEvent* caller)
{
  if (caller == &tauntStartTimer)
  {
    ResourceManager::playBossTaunt();
  }
  else if (caller == &bombTimer)
  {
    createBomb(Point2(player->getX() + RM::randf(-64.0, 64.0),
                      camera.myBox().top() - 64.0), Point2(RM::randf(-24.0, 24.0), 0.0));
  }
  else if (caller == &arrowTimer)
  {
    Box camBox = camera.myBox();
    
    Point2 coords(RM::lerp(camBox.left(), camBox.right(), RM::randf(0.2, 0.8)), -PLAY_H * 2.5);
    Point2 vel(0.0, 200.0);
    
    SumoKnife* knife = new SumoKnife(coords, vel);
    addEnemy(knife);
  }
  else if (caller == &junkTimer)
  {
    Point2 createLocation = Point2(RM::randf(player->getX() - 100.0, player->getX() + 100.0),
                                   camera.myBox().top() - 64.0);
    createMeat(createLocation);
  }
  else if (caller == &coinTimer)
  {
    Point2 groupCenter = Point2(RM::randf(worldBox.left() + 64.0, worldBox.right() - 64.0),
                                camera.myBox().top() - 128.0);
    
    for (Coord1 i = 0; i < 3; ++i)
    {
      Point2 coinCoords = groupCenter + Point2(RM::randf(-48.0, 48.0), RM::randf(-64.0, 64.0));
      PhysicalCoin* coin = new PhysicalCoin(coinCoords, SlopeCoin::randValueAll());
      addAlly(coin);
      coin->set_restitution(RM::randf(0.3, 0.75));
      coin->set_velocity(Point2(RM::randf(-64.0, 64.0), 0.0));
    }
    
    coinGroupsLeft--;
    
    if (coinGroupsLeft == 0) coinTimer.setInactive();
    else coinTimer.reset(5.0);
  }
  else if (caller == &winTimer)
  {
    levelState = LEVEL_PLAY;  // this is needed to trigger winLevel()
    winLevel();
  }
}

// ==================================== SULevelGraveyard

Point2 SULevelGraveyard::createArena()
{
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 64.0);
  
  Point2 topPoints[8] =
  {
    Point2(0.0, -192.0),
    Point2(64.0, -160.0),
    Point2(192.0, -160.0),
    Point2(320.0, -224.0),
    Point2(640.0, -224.0),
    Point2(768.0, -160.0),
    Point2(896.0, -160.0),
    Point2(960.0, -192.0)
  };
  Point1 bottomY = 0.0;
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    TerrainQuad* quad = createSlopeQuad(topPoints[i], Point2(topPoints[i].x, -32.0),
                                        Point2(topPoints[i+1].x, -32.0), topPoints[i+1],
                                        img);
    // quad->flags = PizzaGO::SLOW_BIT;
  }
  
  boss->setXY(832.0, endpoint.y - 224.0);
  startStanding(Point2(128.0, endpoint.y - 224.0));
  
  return Point2(topPoints[0].x, topPoints[RM::count(topPoints) - 1].x);
}


void SULevelGraveyard::loadMe()
{
  SumoLevel::loadMe();
  
  boss->getImage()->handle = Point2(148.0, 148.0);
  boss->ai->setStrRange(0.5, 1.0);
  boss->ai->saveChance = 1;
}

// ==================================== SULevelBadlands

Point2 SULevelBadlands::createArena()
{
  Point1 bottomY = 0.0;
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 160.0);
  
  // leftmost quad
  TerrainQuad* quad = createSlopeQuad(Point2(0.0, 0.0), Point2(128.0, bottomY),
                                      Point2(128.0, -80.0), Point2(64.0, -128.0),
                                      imgsetCache[sumoTerrainSet][0]);
  
  Point2 topPoints[] =
  {
    Point2(128.0, -80.0),
    Point2(400.0, -80.0),
    Point2(464.0, -128.0),
    Point2(528.0, -80.0),
    Point2(832.0, -80.0),
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    TerrainQuad* quad2 = createSumoQuad(topPoints[i], Point2(topPoints[i].x, bottomY),
                                        Point2(topPoints[i+1].x, bottomY), topPoints[i+1],
                                        NULL);
  }
  
  // rightmost quad
  TerrainQuad* quad3 = createSumoQuad(Point2(832.0, -80.0), Point2(832.0, bottomY),
                                      Point2(960.0, bottomY), Point2(896.0, -128.0),
                                      NULL);
  
  startStanding(Point2(192.0, endpoint.y - 224.0));
  
  boss->setXY(768.0, 352.0 - 224.0);
  boss->getImage()->handle = Point2(152.0, 148.0);
  boss->ai->setStrRange(0.5, 1.5);
  boss->ai->saveChance = 2;
  
  return Point2(64.0, 896.0);
}

// ==================================== SULevelQuarry

Point2 SULevelQuarry::createArena()
{
  Point1 bottomY = 0.0;
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 64.0);
  
  Point2 topPoints[] =
  {
    Point2(0.0, -128.0),
    Point2(96.0, -64.0),
    Point2(192.0, -112.0),
    Point2(288.0, -144.0),
    Point2(480.0, -160.0),
    Point2(672.0, -144.0),
    Point2(768.0, -112.0),
    Point2(864.0, -64.0),
    Point2(960.0, -128.0),
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    
    TerrainQuad* quad2 = createSumoQuad(topPoints[i], Point2(topPoints[i].x, bottomY),
                                        Point2(topPoints[i+1].x, bottomY), topPoints[i+1],
                                        img);
  }
  
  startStanding(Point2(112.0, endpoint.y - 242.0));
  boss->setXY(848.0, 352.0 - 240.0);
  
  return Point2(topPoints[0].x, topPoints[RM::count(topPoints) - 1].x);
}

void SULevelQuarry::createBoss()
{
  boss = new SumoBoss();
  boss->getImage()->handle = Point2(148.0, 128.0);
  boss->geometry = SumoBoss::HEXAGON;
  addEnemy(boss);
}

// ==================================== SULevelPirate

SULevelPirate::SULevelPirate() :
SumoLevel(),
boat(NULL),
rotVal(0.0),
rotData(0.5, 1.0)
{
  camYPad = -48.0;
  musicFilename = "pirate.ima4";
}

Point2 SULevelPirate::createArena()
{
  boat = new TerrainQuad();
  boat->setImage(imgsetCache[sumoTerrainSet][0]);
  boat->getImage()->handle.set(512.0, 208.0);
  
  boat->setXY(512.0, 272.0);
  boat->body = boat->create_body(boat->getXY(), false);
  boat->body->SetType(b2_kinematicBody);
  
  // IMPORTANT: "boat->fixture" changes as each call is made, no need to free them all
  Point2 topPoints[] =
  {
    Point2(-480.0, -160.0),
    Point2(-416.0, -96.0),
    Point2(0.0, -96.0),
    Point2(416.0, -96.0),
    Point2(480.0, -160.0)
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    boat->fixture = boat->create_quad_fixture(boat->body,
                                              topPoints[i],
                                              Point2(topPoints[i].x, 64.0),
                                              Point2(topPoints[i+1].x, 64.0),
                                              topPoints[i+1]);
    boat->fixture->SetUserData(boat);
    boat->set_collisions(1 << PizzaGO::TYPE_TERRAIN, 0xffff);
    boat->set_friction(0.5);
  }
  
  addBGTerrain(boat);
  
  boss->setXY(boat->getXY() + Point2(336.0, -300.0));
  startStanding(boat->getXY() + Point2(-336.0, -300.0));
  
  boss->getImage()->handle = Point2(148.0, 148.0);
  boss->ai->setStrRange(0.5, 1.75);
  bombTimer.reset(3.5);
  
  return Point2(boat->getX() + topPoints[1].x, boat->getX() + 416.0);
}

void SULevelPirate::updateLevelAI()
{
  SumoLevel::updateLevelAI();
  
  if (levelState == LEVEL_PLAY)
  {
    Point1 oldVal = rotVal;
    RM::bounce_arcsine(rotVal, rotData, Point2(-1.0, 1.0), 0.5 * RM::timePassed());
    boat->set_ang_vel((rotVal - oldVal) * TWO_PI);
  }
}

void SULevelPirate::callback(ActionEvent* caller)
{
  SumoLevel::callback(caller);
  
}

// ==================================== SULevelStadium

SULevelStadium::SULevelStadium() :
SumoLevel(),
triedDispenseItem(false)
{
  
}

Point2 SULevelStadium::createArena()
{
  Point1 bottomY = 0.0;
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 32);
  
  Point2 topPoints[] =
  {
    Point2(0.0, -192.0),
    Point2(64.0, -128.0),
    Point2(480.0, -128.0),
    Point2(896.0, -128.0),
    Point2(960.0, -192.0)
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    
    TerrainQuad* quad2 = TerrainQuad::create_static_quad(topPoints[i] + endpoint,
                                                         Point2(topPoints[i].x, bottomY)  + endpoint,
                                                         Point2(topPoints[i+1].x, bottomY) + endpoint,
                                                         topPoints[i+1] + endpoint);
    quad2->setImage(img);
    quad2->flags |= PizzaGO::SLOW_BIT;
    addEnemy(quad2);
  }
  
  startStanding(Point2(144.0, endpoint.y - 272.0));
  boss->setXY(816.0, endpoint.y - 272.0);
  boss->getImage()->handle = Point2(152.0, 152.0);
  boss->ai->setStrRange(0.5, 1.5);
  boss->ai->powerWait.setActive(true);
  
  return Point2(topPoints[0].x, topPoints[RM::count(topPoints) - 1].x);
}

void SULevelStadium::sumoSlammedEmpty()
{
  if (triedDispenseItem == false)
  {
    tryCreateUnlockable(boss->getXY(), Coord2(4, 10), true);
    triedDispenseItem = true;
  }
}

// ==================================== SULevelNuclear

Point2 SULevelNuclear::createArena()
{
  imgsetCache[sumoTerrainSet][0]->handle_norm(Point2(0.5, 0.5));
  imgsetCache[sumoTerrainSet][0]->handle.y += 8.0;
  
  for (Coord1 i = 0; i < 5; ++i)
  {
    TerrainQuad* quad = TerrainQuad::create_ground_TL(
                                                      Point2(0.0 + i * 256.0, endpoint.y - 128.0),
                                                      imgsetCache[sumoTerrainSet][0]->natural_size(),
                                                      imgsetCache[sumoTerrainSet][0]);
    quad->body->SetType(b2_kinematicBody);
    addRod(quad);
    addAlly(quad);
  }
  
  startStanding(Point2(player->getWidth(), -256.0));
  boss->setXY(1024.0 - boss->getWidth() * 2.0, -128.0);
  boss->getImage()->handle = Point2(148.0, 172.0);
  boss->dmgAngleRange.set(PI * 1.34, PI * 1.5);
  boss->coinStyle = SumoBoss::DROP_UP;
  
  useRods = true;
  updateRods();
  
  Point1 halfWidth = imgsetCache[sumoTerrainSet][0]->natural_size().x * 0.5;
  return Point2(0.0, objman.allyList.getLast()->getX() + halfWidth);
}

// ==================================== SULevelComputer

Point2 SULevelComputer::createArena()
{
  Point1 bottomY = 0.0;
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 208.0);
  imgsetCache[sumoTerrainSet][0]->overexpose();
  
  Point2 topPoints[] =
  {
    Point2(0.0, -96.0),
    Point2(80.0, -160.0),
    Point2(160.0, -128.0),
    Point2(368.0, -224.0),
    Point2(608.0, -176.0),
    Point2(752.0, -272.0),
    Point2(832.0, -240.0)
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    
    TerrainQuad* quad2 = createSlopeQuad(topPoints[i], Point2(topPoints[i].x, bottomY),
                                         Point2(topPoints[i+1].x, bottomY), topPoints[i+1],
                                         img);
  }
  
  createSlopeQuad(Point2(832.0, -240.0), Point2(832.0, 0.0),
                  Point2(960.0, 0.0), Point2(912.0, -272.0), NULL);
  
  startStanding(Point2(160.0, endpoint.y - 288.0));
  boss->setXY(832.0, endpoint.y - 400.0);
  
  return Point2(topPoints[1].x, topPoints[RM::count(topPoints) - 1].x);
}

void SULevelComputer::createBoss()
{
  boss = new SumoBoss();
  boss->getImage()->handle = Point2(148.0, 148.0);
  boss->geometry = SumoBoss::OCTAGON;
  boss->shockTimer.setActive();
  addEnemy(boss);
}

// ==================================== SULevelCarnival

Point2 SULevelCarnival::createArena()
{
  arrowTimer.setActive(true);
  imgsetCache[sumoTerrainSet][0]->handle_norm(Point2(0.5, 0.0));
  imgsetCache[sumoTerrainSet][0]->handle += 4.0;
  
  for (Coord1 i = 0; i < 4; ++i)
  {
    BounceSegment* bouncy = new BounceSegment(Point2(0.0 + i * 256.0, endpoint.y - 96.0));
    bouncy->setImage(imgsetCache[sumoTerrainSet][0]);
    addAlly(bouncy);
  }
  
  startStanding(Point2(player->getWidth() - 64.0, 128.0));
  boss->setXY(1024.0 - boss->getWidth() * 2.0, 128.0);
  boss->getImage()->handle = Point2(156.0, 148.0);
  boss->ai->jumpChance = 4;
  
  Point1 halfWidth = imgsetCache[sumoTerrainSet][0]->natural_size().x * 0.5;
  return Point2(-halfWidth, objman.allyList.getLast()->getX() + halfWidth);
}


// ==================================== SULevelMoon

Point2 SULevelMoon::createArena()
{
  Point1 bottomY = 0.0;
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 96.0);
  
  // these points are relative to the bottom left of the terrain in the image
  Point2 topPoints[] =
  {
    Point2(0.0, -128.0),
    Point2(64.0, -192.0),
    Point2(128.0, -128.0),
    Point2(320.0, -80.0),
    Point2(480.0, -64.0),
    Point2(640.0, -80.0),
    Point2(832.0, -128.0),
    Point2(896.0, -192.0),
    Point2(960.0, -128.0),
  };
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    
    TerrainQuad* quad2 = createSlopeQuad(topPoints[i], Point2(topPoints[i].x, bottomY),
                                         Point2(topPoints[i+1].x, bottomY), topPoints[i+1],
                                         img);
  }
  
  startStanding(Point2(224.0, endpoint.y - 256.0));
  boss->setXY(688.0, endpoint.y - 242.0);
  boss->getImage()->handle = Point2(148.0, 148.0);
  boss->shootTimer.setActive(true);
  boss->ai->jumpChance = 7;
  
  RMPhysics->set_gravity_rm(RMPhysics->get_gravity_rm() * Point2(1.0, 0.65));
  
  return Point2(topPoints[1].x, topPoints[RM::count(topPoints) - 2].x);
}

// ==================================== SULevelMeat

Point2 SULevelMeat::createArena()
{
  imgsetCache[sumoTerrainSet][0]->handle.set(0.0, 208.0);
  
  ArrayList<Point2> pentPts(5);
  pentPts.add(Point2(0.0, -160.0));
  pentPts.add(Point2(0.0, 0.0));
  pentPts.add(Point2(96.0, 0.0));
  pentPts.add(Point2(96.0, -160.0));
  pentPts.add(Point2(48.0, -208.0));
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    TerrainQuad* quad = new TerrainQuad();
    quad->setImage(imgsetCache[sumoTerrainSet][0]);
    quad->setXY(Point2(0.0 + i * 160.0, endpoint.y + 96.0));
    quad->body = quad->create_body(quad->getXY(), false);
    quad->body->SetType(b2_kinematicBody);
    
    quad->fixture = quad->create_shape_fixture(quad->body, pentPts);
    quad->set_collisions(1 << PizzaGO::TYPE_TERRAIN, 0xffff);
    quad->fixture->SetUserData(quad);
    
    addRod(quad);
    addAlly(quad);
  }
  
  startStanding(Point2(player->getWidth(), -128.0));
  boss->setXY(1024.0 - boss->getWidth() * 2.0, -128.0);
  boss->getImage()->handle = Point2(160.0, 148.0);
  boss->coinStyle = SumoBoss::DROP_UP;
  boss->ai->endTolerance = 0.15;
  boss->ai->powerWait.setActive(true);
  boss->ai->strengthRange.set(0.5, 3.0);
  boss->ai->jumpChance = 8;
  boss->ai->saveChance = 7;
  
  junkTimer.setActive();
  
  useRods = true;
  updateRods();
  
  Point1 halfWidth = imgsetCache[sumoTerrainSet][0]->natural_size().x * 0.5;
  return Point2(48.0, objman.allyList.getLast()->getX() + halfWidth);
}

// ==================================== SULevelCloud

Point2 SULevelCloud::createArena()
{
  imgsetCache[sumoTerrainSet].last()->handle_center();
  
  SumoBridge* bridge = new SumoBridge();
  bridge->setXY(endpoint - Point2(0.0, 160.0));
  addAlly(bridge);
  
  startStanding(Point2(128.0, endpoint.y - 224.0));
  
  boss->getImage()->handle.set(152.0, 152.0);
  boss->setXY(832.0, endpoint.y - 224.0);
  boss->ai->jumpChance = 4;
  boss->blower = true;
  
  return Point2(bridge->getX(), bridge->getRightEdge());
}

// ==================================== SULevelMarket

SULevelMarket::SULevelMarket() :
SumoLevel(),
weaksHit(0)
{
  
}

Point2 SULevelMarket::createArena()
{
  imgsetCache[sumoTerrainSet][0]->handle.set(32.0, 16.0);
  
  Point2 topPoints[] =
  {
    Point2(0.0, -192.0),
    Point2(64.0, -128.0),
    Point2(192.0, -160.0),
    Point2(384.0, -96.0),
    Point2(480.0, -128.0),
    Point2(640.0, -112.0),
    Point2(768.0, -160.0),
    Point2(896.0, -128.0),
    Point2(960.0, -192.0)
  };
  
  Point1 bottomY = 0.0;
  
  for (Coord1 i = 0; i < RM::count(topPoints) - 1; ++i)
  {
    Image* img = i == 0 ? imgsetCache[sumoTerrainSet][0] : NULL;
    
    TerrainQuad* quad2 = createSlopeQuad(topPoints[i], Point2(topPoints[i].x, bottomY),
                                         Point2(topPoints[i+1].x, bottomY), topPoints[i+1],
                                         img);
  }
  
  boss->setXY(832.0, endpoint.y - 224.0);
  boss->getImage()->handle = Point2(160.0, 144.0);
  startStanding(Point2(128.0, endpoint.y - 224.0));
  
  return Point2(topPoints[1].x, topPoints[RM::count(topPoints) - 2].x);
}

void SULevelMarket::createBoss()
{
  boss = new SumoBoss();
  boss->geometry = SumoBoss::OCTAGON;
  boss->hasWeakRange = true;
  boss->weakAngleRange.set(PI * 1.35, PI * 1.65);
  boss->dmgAngleRange.set(PI * 0.35, PI * 0.65);
  boss->usesGem = true;
  addEnemy(boss);
  boss->ai->endTolerance = 0.15;
  boss->ai->strengthRange.set(0.5, 3.0);
  boss->ai->jumpChance = 8;
  boss->ai->saveChance = 99;
}

void SULevelMarket::sumoWeakpointHit()
{
  Coord1 defeatHits = 5;
  weaksHit++;
  
  if (weaksHit < defeatHits)
  {
    Point2 gemPoint = RM::pol_to_cart(boss->getWidth(), PI * 1.5 + boss->getRotation()) + boss->getXY();
    addAction(new Smokeburst(gemPoint, true));
    
    Image* currBossImg = boss->getImage();
    boss->setImage(imgCache[sumoGem]);
    FishShatter* shatter = new FishShatter(*boss, 4, 4);
    addAction(shatter);
    boss->setImage(currBossImg);
    
    boss->gemGlowSpeedMult *= 2.0;
  }
  else if (weaksHit == defeatHits)
  {
    Image* currBossImg = boss->getImage();
    boss->setImage(imgCache[sumoGem]);
    FishShatter* shatter = new FishShatter(*boss, 4, 4);
    addAction(shatter);
    boss->setImage(currBossImg);
    
    boss->ai->setStrRange(0.1, 0.2);
    boss->ai->saveChance = 1;
    
    boss->usesGem = false;
  }
}

// ==================================== AngryLevel ================================ //

AngryLevel::AngryLevel() :
PizzaLevel(),
timeAllowed(60),

skullBarFrame(Point2(0.0, 0.0) + Pizza::platformTL),
skullBarFill(Point2(49.0, 16.0) + Pizza::platformTL),
skullBarWeight(0.0)
{
  musicFilename = "angry.ima4";
}

void AngryLevel::preload()
{
  createBackground();
}

void AngryLevel::loadRes()
{
  background->load();
  
  for (Char skullID = 'a'; skullID <= 'i'; ++skullID)
  {
    String1 imgSkullStr("angry_skull_$", '$', skullID);
    imgsetCache[angrySkullSet].add(Image::create_centered(imgStr(imgSkullStr.as_cstr())));
    
    String1 warpSkullStr("angryskull$.dwarp2", '$', skullID);
    angrySkullWarps.add(new DWarpSequence(warpSkullStr.as_cstr(), true));
  }
  
  imgCache[skullBarFrameImg] = Image::create_handled_norm(imgStr("angry_bar_base"), Point2(0.0, 1.0));
  imgCache[skullBarFillImg] = new Image(imgStr("angry_bar_fill"));
  imgCache[skullBarClock] = Image::create_handled_norm(imgStr("angry_bar_clock"), Point2(0.0, 1.0));
  
  loadImgs();
}

void AngryLevel::unloadRes()
{
  free_clear(angrySkullWarps);
}

void AngryLevel::loadMe()
{
  skullBarFrame.setXY(Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y));
  skullBarFrame.setImage(imgCache[skullBarFrameImg]);
  skullBarFill.setXY(skullBarFrame.getXY() + Point2(49.0, -32.0));
  skullBarFill.setImage(imgCache[skullBarFillImg]);
  skullBarFill.setH(skullBarFill.getImage()->natural_size().y);
  
  createWorld();
  
  // the screenCX should be 0.0 if the bg should be lined up with 0.0 during zoom out
  camera.xy.x = player->getX();
  camera.handle.set(SCREEN_CX, SCREEN_H + Pizza::platformTL.y);
}

void AngryLevel::updateStatus()
{
  if (skullsCrushed >= startingSkulls)
  {
    winLevel();
  }
  else if (timeAllowed - Pizza::currFloatTime <= 0.0)
  {
    loseLevel();
  }
}

void AngryLevel::updateLevelAI()
{
  RM::flatten_me(skullBarWeight, (Point1) skullsCrushed / startingSkulls, RM::timePassed());
}

void AngryLevel::updateCamera()
{
  PizzaLevel::updateCamera();
  
  camera.clampHorizontal(Point2(worldBox.left() - Pizza::platformTL.x / camera.zoom.x,
                                worldBox.right() + Pizza::platformTL.x / camera.zoom.x));
}

void AngryLevel::drawHUD()
{
  Point1 fillWidth = RM::clamp(skullBarWeight, 0.0, 1.0) * skullBarFill.getImage()->natural_size().x;
  skullBarFrame.drawMe();
  skullBarFill.setW(fillWidth);
  skullBarFill.drawMeRepeating();
  
  ColorP4 clockColor = RM::color255(71, 163, 214);
  if (timeAllowed - Pizza::currFloatTime < 10.0) clockColor.set(1.0, 0.0, 0.0, 1.0);
  
  RMGraphics->draw_slice(Circle(skullBarFrame.getXY() + Point2(28.0, -24.0), 16.0),
                         PI * 1.5, Pizza::currFloatTime / timeAllowed,
                         clockColor);
  
  imgCache[skullBarClock]->draw(skullBarFrame.getXY());
}

void AngryLevel::addBlock(PizzaGO* block)
{
  addEnemy(block);
}

void AngryLevel::enemyDefeated(PizzaGO* enemy)
{
  Pizza::currGameVars[VAR_DEFEATED]++;
  
  /*
   String1 str("$/%", '$', Pizza::currGameVars[VAR_DEFEATED]);
   str.int_replace('%', totalDestructibles);
   BonusText* textEffect = new BonusText(str.as_cstr(), ResourceManager::font,
   enemy->getXY(), false);
   textEffect->upwardVal = -96.0;
   textEffect->setScale(0.75, 0.75);
   textEffect->holdTime = 0.75;
   textEffect->init();
   addAction(textEffect);
   */
}

Coord1 AngryLevel::calcWinBonus()
{
  return levelID * 250;
}

// ==================================== ANLevelStonehenge

void ANLevelStonehenge::createBackground()
{
  background = new GraveyardBG(this);
}

void ANLevelStonehenge::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_stonehenge_ground_a"), Point2(0.0, 64.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_stonehenge_ground_b"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_stonehenge_ground_c"), Point2(0.0, 32.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_stonehenge_altar_feet")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_stonehenge_altar_flat")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_stonehenge_altar_top")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_stonehenge_block_h")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_stonehenge_block_v")));
}

void ANLevelStonehenge::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(212.0, 704.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 640.0) + handle);
  groundPts.add(Point2(64.0, 704.0) + handle);
  groundPts.add(Point2(320.0, 704.0) + handle);
  groundPts.add(Point2(448.0, 672.0) + handle);
  groundPts.add(Point2(512.0, 608.0) + handle);
  groundPts.add(Point2(2624.0, 608.0) + handle);
  groundPts.add(Point2(2688.0, 672.0) + handle);
  groundPts.add(Point2(2816.0, 704.0) + handle);
  groundPts.add(Point2(3008.0, 704.0) + handle);
  groundPts.add(Point2(3072.0, 640.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(Point2(0.0, 640.0) + handle, imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 608.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 608.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  Point2 blockHPts[] =
  {
    Point2(608.0, 336.0),
    Point2(1088.0, 336.0),
    Point2(1376.0, 336.0),
    Point2(2080.0, 336.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockHPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockHPts[i] + handle,
                                                      imgsetCache[angryBlockSet][3]->natural_size(), imgsetCache[angryBlockSet][3]);
    addBlock(block);
  }
  
  Point2 blockVPts[] =
  {
    Point2(640.0, 384.0),
    Point2(800.0, 384.0),
    Point2(1120.0, 384.0),
    Point2(1344.0, 384.0),
    Point2(1568.0, 384.0),
    Point2(2048.0, 384.0),
    Point2(2272.0, 384.0),
  };
  
  for (Coord1 i = 0; i < RM::count(blockVPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(blockVPts[i] + handle,
                                                      imgsetCache[angryBlockSet][4]->natural_size(), imgsetCache[angryBlockSet][4]);
    addBlock(block);
  }
  
  
  Point2 feetPts[] =
  {
    Point2(1424.0, 544.0),
    Point2(1424.0, 576.0),
    Point2(1536.0, 544.0),
    Point2(1536.0, 576.0),
    Point2(1760.0, 576.0),
    Point2(1888.0, 576.0)
  };
  
  for (Coord1 i = 0; i < RM::count(feetPts); ++i)
  {
    TerrainQuad* block = TerrainQuad::create_block_TL(feetPts[i] + handle,
                                                      imgsetCache[angryBlockSet][0]->natural_size(), imgsetCache[angryBlockSet][0]);
    addBlock(block);
  }
  
  Point2 skullPts[] =
  {
    Point2(760.0, 580.0),
    Point2(1300.0, 308.0),
    Point2(1236.0, 583.0),
    Point2(1496.0, 581.0),
    Point2(1860.0, 500.0),
    Point2(2212.0, 309.0),
    Point2(2358.0, 582.0),
    Point2(2819.0, 680.0)
  };
  
  TerrainQuad* altTop1 = TerrainQuad::create_block_TL(Point2(1440.0, 512.0) + handle,
                                                      imgsetCache[angryBlockSet][2]->natural_size(), imgsetCache[angryBlockSet][2]);
  addBlock(altTop1);
  
  TerrainQuad* altTop2 = TerrainQuad::create_block_TL(Point2(1776.0, 528.0) + handle,
                                                      imgsetCache[angryBlockSet][2]->natural_size(), imgsetCache[angryBlockSet][2]);
  addBlock(altTop2);
  
  TerrainQuad* altFlat = TerrainQuad::create_block_TL(Point2(1744.0, 560.0) + handle,
                                                      imgsetCache[angryBlockSet][1]->natural_size(), imgsetCache[angryBlockSet][1]);
  addBlock(altFlat);
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
  
  addTextEffectStd(LSTRINGS[94], PLAY_CENTER);
}

void ANLevelStonehenge::updateLevelAI()
{
  AngryLevel::updateLevelAI();
  if (levelState == LEVEL_PLAY) tutTimer.update();
}

void ANLevelStonehenge::callback(ActionEvent* caller)
{
  if (caller == &tutTimer && alreadyBeaten() == false)
  {
    addTextEffectStd(LSTRINGS[RMSystem->get_OS_family() == MAC_FAMILY ? 137 : 95], PLAY_CENTER + Point2(0.0, -96.0));
  }
}

// ==================================== ANLevelCastle

void ANLevelCastle::createBackground()
{
  background = new BadlandsBG(this);
}

void ANLevelCastle::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_castle_ground_a"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_castle_ground_b"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_castle_ground_c"), Point2(0.0, 96.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_block_big_window")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_block_big")));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_castle_block_top"), Point2(80.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_block_v_short")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_block_v_tall")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_drawbridge")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_castle_tower_shaft")));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_castle_tower_top"),
                                                           Point2(32.0, 0.0)));
}

void ANLevelCastle::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(176.0, 642.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 576.0) + handle);
  groundPts.add(Point2(64.0, 640.0) + handle);
  groundPts.add(Point2(384.0, 640.0) + handle);
  groundPts.add(Point2(448.0, 704.0) + handle);
  groundPts.add(Point2(640.0, 704.0) + handle);
  groundPts.add(Point2(704.0, 644.0) + handle);
  groundPts.add(Point2(2368.0, 644.0) + handle);
  groundPts.add(Point2(2432.0, 704.0) + handle);
  groundPts.add(Point2(2624.0, 704.0) + handle);
  groundPts.add(Point2(2688.0, 640.0) + handle);
  groundPts.add(Point2(3008.0, 640.0) + handle);
  groundPts.add(Point2(3072.0, 576.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 640.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 640.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  tryCreateUnlockable(Point2(1792.0, -128.0) + handle, Coord2(1, 11));
  
  // drawbridges
  addBlock(TerrainQuad::create_block_TL(Point2(352.0, 608.0) + handle,
                                        imgsetCache[angryBlockSet][5]->natural_size(), imgsetCache[angryBlockSet][5]));
  addBlock(TerrainQuad::create_block_TL(Point2(2336.0, 608.0) + handle,
                                        imgsetCache[angryBlockSet][5]->natural_size(), imgsetCache[angryBlockSet][5]));
  
  // walls
  for (Coord1 i = 0; i < 4; ++i)
  {
    Point2 tallTL = Point2(800.0 + 112.0 * i, 384.0) + handle;
    
    addBlock(TerrainQuad::create_block_TL(tallTL,
                                          Point2(48.0, 256.0), imgsetCache[angryBlockSet][4]));
    if (i < 3)
    {
      addBlock(TerrainQuad::create_block_TL(tallTL + Point2(64.0, 64.0),
                                            Point2(64.0, 208.0), imgsetCache[angryBlockSet][3]));
    }
  }
  
  for (Coord1 i = 0; i < 2; ++i)
  {
    Point2 tallTL = Point2(2784.0 + 112.0 * i, 384.0) + handle;
    
    addBlock(TerrainQuad::create_block_TL(tallTL,
                                          Point2(48.0, 256.0), imgsetCache[angryBlockSet][4]));
    if (i < 1)
    {
      addBlock(TerrainQuad::create_block_TL(tallTL + Point2(64.0, 64.0),
                                            Point2(64.0, 208.0), imgsetCache[angryBlockSet][3]));
    }
  }
  
  // towers
  for (Coord1 i = 0; i < 4; ++i)
  {
    Point2 blockTL = Point2(1280.0, 128.0 + 128.0 * i) + handle;
    
    addBlock(TerrainQuad::create_block_TL(blockTL,
                                          Point2(64.0, 128.0),
                                          imgsetCache[angryBlockSet][6]));
    
    addBlock(TerrainQuad::create_block_TL(Point2(2208.0 + handle.x, blockTL.y),
                                          Point2(64.0, 128.0),
                                          imgsetCache[angryBlockSet][6]));
  }
  
  Point2 triTL = Point2(1312.0, 64.0) + handle;
  addBlock(TerrainQuad::create_block_tri(Tri(triTL, triTL + Point2(-32.0, 64.0), triTL + Point2(32.0, 64.0)),
                                         imgsetCache[angryBlockSet][7]));
  
  triTL = Point2(2240.0, 64.0) + handle;
  addBlock(TerrainQuad::create_block_tri(Tri(triTL, triTL + Point2(-32.0, 64.0), triTL + Point2(32.0, 64.0)),
                                         imgsetCache[angryBlockSet][7]));
  
  // castle trapezoids
  Point2 trapTLs[] =
  {
    Point2(1552.0, 128.0),
    Point2(1760.0, 16.0),
    Point2(1968.0, 128.0)
  };
  
  for (Coord1 i = 0; i < RM::count(trapTLs); ++i)
  {
    Point2 topCorner = trapTLs[i] + handle;
    
    TerrainQuad* quad = TerrainQuad::create_block_quad(
                                                       topCorner, topCorner + Point2(-80.0, 64.0),
                                                       topCorner + Point2(128.0, 64.0), topCorner + Point2(48.0, 0.0),
                                                       imgsetCache[angryBlockSet][2]);
    addBlock(quad);
  }
  
  // skulls are before castle body for windows
  Point2 skullTLs[] =
  {
    Point2(448.0, 656.0),
    Point2(912.0, 336.0),
    Point2(1072.0, 384.0),
    Point2(1536.0, 256.0),
    Point2(1584.0, 594.0),
    Point2(1888.0, 112.0),
    Point2(1954.0, 482.0),
    Point2(2034.0, 482.0),
    Point2(2274.0, 595.0),
    Point2(2434.0, 658.0),
    Point2(2498.0, 658.0),
    Point2(2834.0, 386.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullTLs); ++i)
  {
    createAngrySkull(skullTLs[i] + handle + Point2(24.0, 24.0));
  }
  
  // castle body
  for (Coord1 row = 0; row < 5; ++row)
  {
    for (Coord1 col = 0; col < 3; ++col)
    {
      if (row == 0 && col == 0) continue;
      if (row == 0 && col == 2) continue;
      
      Image* img = imgsetCache[angryBlockSet][1];
      Point2 blockTL = Point2(1472.0, 80.0) + handle;
      Point2 size = img->natural_size();
      
      TerrainQuad* block = TerrainQuad::create_block_TL(blockTL +
                                                        Point2(col * size.x, row * size.y),
                                                        size, img);
      if ((row == 1 && col == 0) ||
          (row == 3 && col == 2) ||
          (row == 4 && col == 0))
      {
        block->setImage(imgsetCache[angryBlockSet][0]);
        block->ignoreAngrySkulls();
      }
      
      addBlock(block);
    }
  }
}

// ==================================== ANLevelCabin

void ANLevelCabin::createBackground()
{
  background = new PirateBG(this);
}

void ANLevelCabin::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cabin_ground_a"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cabin_ground_b"), Point2(0.0, 144.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cabin_ground_c"), Point2(0.0, 96.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_cabin_log_front")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_cabin_log_long")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_cabin_log_short")));
}

void ANLevelCabin::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(168.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 608.0) + handle);
  groundPts.add(Point2(64.0, 672.0) + handle);
  groundPts.add(Point2(608.0, 672.0) + handle);
  groundPts.add(Point2(608.0, 624.0) + handle);
  groundPts.add(Point2(704.0, 624.0) + handle);
  groundPts.add(Point2(704.0, 672.0) + handle);
  groundPts.add(Point2(944.0, 672.0) + handle);
  groundPts.add(Point2(944.0, 624.0) + handle);
  groundPts.add(Point2(1040.0, 624.0) + handle);
  groundPts.add(Point2(1040.0, 672.0) + handle);
  groundPts.add(Point2(1120.0, 672.0) + handle);
  groundPts.add(Point2(1280.0, 624.0) + handle);
  groundPts.add(Point2(1408.0, 544.0) + handle);
  groundPts.add(Point2(1504.0, 512.0) + handle);
  groundPts.add(Point2(3008.0, 512.0) + handle);
  groundPts.add(Point2(3072.0, 448.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 624.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 512.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  Point2 logPts[] =
  {
    Point2(728.0, 650.0),
    Point2(824.0, 650.0),
    Point2(872.0, 650.0),
    Point2(920.0, 650.0),
    Point2(752.0, 608.0),
    Point2(800.0, 608.0),
    Point2(848.0, 608.0),
    Point2(896.0, 608.0),
    Point2(776.0, 566.0),
    Point2(872.0, 566.0),
    Point2(800.0, 524.0),
    Point2(848.0, 524.0),
    
    Point2(1064.0, 650.0),
    Point2(1112.0, 650.0),
    Point2(1160.0, 636.0),
    Point2(1206.0, 622.0),
    Point2(1253.0, 608.0),
    Point2(1339.0, 560.0),
    
    Point2(1640.0, 250.0),
    Point2(1640.0, 298.0),
    Point2(1640.0, 346.0),
    Point2(1640.0, 442.0),
    Point2(1640.0, 490.0),
    Point2(2152.0, 298.0),
    Point2(2152.0, 346.0),
    Point2(2152.0, 394.0),
    Point2(2152.0, 442.0),
    Point2(2152.0, 490.0)
  };
  
  for (Coord1 i = 0; i < RM::count(logPts); ++i)
  {
    TerrainQuad* roundLog = TerrainQuad::create_block_circle(
                                                             logPts[i] + handle, 24.0, imgsetCache[angryBlockSet][0]);
    addBlock(roundLog);
    roundLog->body->SetAwake(false);
  }
  
  // long top block
  TerrainQuad* longLog = TerrainQuad::create_block_TL(Point2(1552.0, 176.0) + handle,
                                                      imgsetCache[angryBlockSet][1]->natural_size(),
                                                      imgsetCache[angryBlockSet][1]);
  longLog->body->SetAwake(false);
  addBlock(longLog);
  
  // short blocks
  Point3 shortLogDefs[] =
  {
    Point3(1590.0, 370.0, 90.0),
    Point3(1686.0, 370.0, 90.0),
    Point3(2102.0, 370.0, 90.0),
    Point3(2200.0, 370.0, 90.0),
    
    Point3(2559.0, 458.0, -12.3),
    Point3(2604.0, 400.0, -12.3),
    Point3(2747.0, 317.0, -8.5),
    Point3(2886.0, 426.0, 26.6),
  };
  
  for (Coord1 i = 0; i < RM::count(shortLogDefs); ++i)
  {
    Point2 shortTL = Point2(shortLogDefs[i].x, shortLogDefs[i].y) + handle;
    
    addBlock(TerrainQuad::create_block_center(shortTL, Point2(288.0, 48.0),
                                              RM::degs_to_rads(shortLogDefs[i].z),
                                              imgsetCache[angryBlockSet][2]));
  }
  
  // skulls
  Point2 skullPts[] =
  {
    Point2(820.0, 478.0),
    Point2(822.0, 564.0),
    Point2(774.0, 650.0),
    Point2(1294.0, 582.0),
    Point2(1640.0, 392.0),
    Point2(1766.0, 488.0),
    Point2(1956.0, 150.0),
    Point2(2152.0, 250.0),
    Point2(2644.0, 488.0)
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== ANLevelFort

void ANLevelFort::createBackground()
{
  background = new StadiumBG(this);
}

void ANLevelFort::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_fort_ground_a"), Point2(0.0, 96.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_fort_ground_b"), Point2(0.0, 80.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_fort_ground_c"), Point2(0.0, 176.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_2x2")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_h_8")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_h_16")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_h_32")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_v_4")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_fort_board_v_8")));
}

void ANLevelFort::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(160.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 608.0) + handle);
  groundPts.add(Point2(64.0, 672.0) + handle);
  groundPts.add(Point2(576.0, 672.0) + handle);
  groundPts.add(Point2(704.0, 656.0) + handle);
  groundPts.add(Point2(864.0, 576.0) + handle);
  groundPts.add(Point2(1024.0, 544.0) + handle);
  groundPts.add(Point2(1856.0, 544.0) + handle);
  groundPts.add(Point2(1984.0, 528.0) + handle);
  groundPts.add(Point2(2144.0, 448.0) + handle);
  groundPts.add(Point2(2288.0, 416.0) + handle);
  groundPts.add(Point2(3008.0, 416.0) + handle);
  groundPts.add(Point2(3072.0, 352.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 544.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 496.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  IndexPt boardDefs[] =
  {
    // 2x2
    IndexPt(0, 1152.0, 384.0),
    IndexPt(0, 2496.0, 384.0),
    
    // h 8
    IndexPt(1, 1824.0, 288.0),
    IndexPt(1, 2832.0, 224.0),
    IndexPt(1, 2832.0, 320.0),
    
    // h 16
    IndexPt(2, 928.0, 352.0),
    IndexPt(2, 2272.0, 288.0),
    IndexPt(2, 2464.0, 256.0),
    
    // h 32
    IndexPt(3, 544.0, 448.0),
    IndexPt(3, 1072.0, 256.0),
    IndexPt(3, 1504.0, 160.0),
    IndexPt(3, 1824.0, 384.0),
    
    // v 4
    IndexPt(4, 544.0, 480.0),
    IndexPt(4, 928.0, 384.0),
    IndexPt(4, 1024.0, 480.0),
    IndexPt(4, 1072.0, 288.0),
    IndexPt(4, 1504.0, 192.0),
    IndexPt(4, 1824.0, 320.0),
    IndexPt(4, 1920.0, 320.0),
    IndexPt(4, 1984.0, 320.0),
    IndexPt(4, 2272.0, 320.0),
    IndexPt(4, 2496.0, 320.0),
    IndexPt(4, 2832.0, 256.0),
    IndexPt(4, 2832.0, 352.0),
    IndexPt(4, 2928.0, 256.0),
    IndexPt(4, 2928.0, 352.0),
    
    // v 8
    IndexPt(5, 544.0, 544.0),
    IndexPt(5, 624.0, 320.0),
    IndexPt(5, 1152.0, 416.0),
    IndexPt(5, 1552.0, 288.0),
    IndexPt(5, 1552.0, 416.0),
    IndexPt(5, 1824.0, 416.0),
    IndexPt(5, 1984.0, 192.0),
    IndexPt(5, 2688.0, 288.0),
  };
  
  for (Coord1 i = 0; i < RM::count(boardDefs); ++i)
  {
    Image* img = imgsetCache[angryBlockSet][boardDefs[i].index];
    TerrainQuad* block = TerrainQuad::create_block_TL(boardDefs[i].xy + handle,
                                                      img->natural_size(),
                                                      img);
    addBlock(block);
  }
  
  Point2 skullPts[] =
  {
    Point2(638.0, 294.0),
    Point2(1080.0, 518.0),
    Point2(1128.0, 324.0),
    Point2(1520.0, 516.0),
    Point2(1560.0, 232.0),
    Point2(1878.0, 360.0),
    Point2(2040.0, 360.0),
    Point2(2436.0, 264.0),
    Point2(2846.0, 200.0),
    Point2(2936.0, 200.0)
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== ANLevelGlass

void ANLevelGlass::createBackground()
{
  background = new MoonBG(this);
}

void ANLevelGlass::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_glass_ground_a"), Point2(0.0, 64.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_glass_ground_b"), Point2(0.0, 48.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_glass_ground_c"), Point2(0.0, 96.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_glass_block_v_8")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_glass_h_32")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_glass_triangle")));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_glass_well_top"), Point2(96.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_glass_well")));
}

void ANLevelGlass::createWorld()
{
  RMPhysics->set_gravity_rm(RMPhysics->get_gravity_rm() * Point2(1.0, 0.65));
  
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(160.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 592.0) + handle);
  groundPts.add(Point2(64.0, 672.0) + handle);
  groundPts.add(Point2(704.0, 672.0) + handle);
  groundPts.add(Point2(832.0, 640.0) + handle);
  groundPts.add(Point2(960.0, 576.0) + handle);
  groundPts.add(Point2(1088.0, 544.0) + handle);
  groundPts.add(Point2(1600.0, 544.0) + handle);
  groundPts.add(Point2(1728.0, 576.0) + handle);
  groundPts.add(Point2(1856.0, 640.0) + handle);
  groundPts.add(Point2(1984.0, 672.0) + handle);
  groundPts.add(Point2(3008.0, 672.0) + handle);
  groundPts.add(Point2(3072.0, 608.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 560.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 672.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  IndexPt boardDefs[] =
  {
    // v 8
    IndexPt(0, 448.0, 416.0),
    IndexPt(0, 448.0, 544.0),
    IndexPt(0, 640.0, 416.0),
    IndexPt(0, 640.0, 544.0),
    IndexPt(0, 1088.0, 416.0),
    IndexPt(0, 1536.0, 416.0),
    IndexPt(0, 1984.0, 352.0),
    IndexPt(0, 1984.0, 544.0),
    IndexPt(0, 2432.0, 352.0),
    IndexPt(0, 2464.0, 544.0),
    IndexPt(0, 2944.0, 544.0),
    
    // h 32
    IndexPt(1, 1088.0, 352.0),
    IndexPt(1, 1984.0, 480.0),
    IndexPt(1, 2496.0, 480.0),
    
    // well
    IndexPt(4, 528.0, 608)
  };
  
  for (Coord1 i = 0; i < RM::count(boardDefs); ++i)
  {
    Image* img = imgsetCache[angryBlockSet][boardDefs[i].index];
    TerrainQuad* block = TerrainQuad::create_block_TL(boardDefs[i].xy + handle,
                                                      img->natural_size(),
                                                      img);
    addBlock(block);
  }
  
  // well top
  addBlock(TerrainQuad::create_block_quad(Point2(544.0, 336.0) + handle,
                                          Point2(448.0, 416.0) + handle,
                                          Point2(704.0, 416.0) + handle,
                                          Point2(608.0, 336.0) + handle,
                                          imgsetCache[angryBlockSet][3]));
  
  Point2 roofTopPts[] =
  {
    Point2(1344.0, 160.0),
    Point2(2240.0, 160.0),
    Point2(2752.0, 288.0)
  };
  
  for (Coord1 i = 0; i < RM::count(roofTopPts); ++i)
  {
    Tri pts(roofTopPts[i] + handle,
            roofTopPts[i] + Point2(-256.0, 192.0) + handle,
            roofTopPts[i] + Point2(256.0, 192.0) + handle);
    TerrainQuad* roof = TerrainQuad::create_block_tri(pts, imgsetCache[angryBlockSet][2]);
    roof->getImage()->handle = roof->getXY() - (roofTopPts[i] + handle) + Point2(256.0, 0.0);
    addBlock(roof);
  }
  
  Point2 skullPts[] =
  {
    Point2(608.0, 584.0),
    Point2(728.0, 634.0),
    Point2(1326.0, 516.0),
    Point2(2174.0, 644.0),
    Point2(2404.0, 449.0),
    Point2(2524.0, 420.0),
    Point2(2550.0, 648.0),
    Point2(2614.0, 0.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== ANLevelPyramid

void ANLevelPyramid::createBackground()
{
  background = new QuarryBG(this, false);
}

void ANLevelPyramid::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pyramid_ground_a"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pyramid_ground_b"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pyramid_ground_c"), Point2(0.0, 96.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pyramid_block_special")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pyramid_block")));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_pyramid_capstone"),
                                                           Point2(64.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_pyramid_side_left"),
                                                           Point2(192.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_pyramid_side_right"),
                                                           Point2(0.0, 0.0)));
}

void ANLevelPyramid::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(160.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 608.0) + handle);
  groundPts.add(Point2(64.0, 672.0) + handle);
  groundPts.add(Point2(3008.0, 672.0) + handle);
  groundPts.add(Point2(3072.0, 608.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 672.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 672.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  Coord1 filledBits = 0x0;
  createPyramid(Point2(576.0, 416.0) + handle, 2, filledBits);
  
  filledBits = 0x1;
  createPyramid(Point2(1376.0, 288.0) + handle, 3, filledBits);
  
  filledBits = (0x1 << 2) | (0x1 << 3) | (0x1 << 4);
  createPyramid(Point2(2432.0, 160.0) + handle, 4, filledBits);
  
  Point2 skullPts[] =
  {
    Point2(632.0, 648.0),
    Point2(1254.0, 648.0),
    Point2(1396.0, 648.0),
    Point2(1464.0, 648.0),
    Point2(2236.0, 518.0),
    Point2(2368.0, 518.0),
    Point2(2532.0, 392.0),
    Point2(2640.0, 648.0),
    Point2(2790.0, 648.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

void ANLevelPyramid::createPyramid(Point2 topCenter, Coord1 numRows, UInt2 filledBits)
{
  Coord1 currBlock = 0;
  Point2 blockSize = imgsetCache[angryBlockSet][0]->natural_size();
  
  for (Coord1 row = 0; row < numRows; ++row)
  {
    Point2 leftHandle = topCenter + Point2(row * -blockSize.x * 0.5, row * blockSize.y);
    Point2 rightHandle = topCenter + Point2(row * blockSize.x * 0.5, row * blockSize.y);
    
    TerrainQuad* leftSide = TerrainQuad::create_block_quad(leftHandle,
                                                           leftHandle + Point2(-64.0, 0.0),
                                                           leftHandle + Point2(-192.0, 128.0),
                                                           leftHandle + Point2(0.0, 128.0),
                                                           imgsetCache[angryBlockSet][3]);
    addBlock(leftSide);
    
    TerrainQuad* rightSide = TerrainQuad::create_block_quad(rightHandle,
                                                            rightHandle + Point2(0.0, 128.0),
                                                            rightHandle + Point2(192.0, 128.0),
                                                            rightHandle + Point2(64.0, 0.0),
                                                            imgsetCache[angryBlockSet][4]);
    addBlock(rightSide);
    
    // middle
    for (Coord1 col = 0; col < row; ++col)
    {
      Logical filledBlock = ((0x1 << currBlock) & filledBits) != 0;
      Point2 blockTL = topCenter + Point2(-blockSize.x * 0.5 * row + col * blockSize.x,
                                          row * blockSize.y);
      
      TerrainQuad* midBlock = TerrainQuad::create_block_TL(blockTL, blockSize,
                                                           imgsetCache[angryBlockSet][1]);
      if (filledBlock == false)
      {
        midBlock->setImage(imgsetCache[angryBlockSet][0]);
        midBlock->ignoreAngrySkulls();
      }
      
      addBlock(midBlock);
      ++currBlock;
    }
  }
  
  Tri capstoneTri(Point2(topCenter + Point2(0.0, -64.0)),
                  Point2(topCenter + Point2(-64.0, 0.0)),
                  Point2(topCenter + Point2(64.0, 0.0)));
  addBlock(TerrainQuad::create_block_tri(capstoneTri, imgsetCache[angryBlockSet][2]));
}

// ==================================== ANLevelIgloo

void ANLevelIgloo::createBackground()
{
  background = new SlopesBG(this);
}

void ANLevelIgloo::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_igloo_ground_a"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_igloo_ground_b"), Point2(0.0, 96.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_igloo_ground_c"), Point2(0.0, 96.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_igloo_block_diag_left"),
                                                           Point2(128.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_igloo_block_diag_right"),
                                                           Point2(0.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_igloo_block_special")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_igloo_block")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_igloo_snowball")));
}

void ANLevelIgloo::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(176.0, 640.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 576.0) + handle);
  groundPts.add(Point2(64.0, 640.0) + handle);
  groundPts.add(Point2(3008.0, 640.0) + handle);
  groundPts.add(Point2(3072.0, 576.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 640.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 640.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  // skulls come first so they are behind clear ice
  Point2 skullPts[] =
  {
    Point2(576.0, 480.0),
    Point2(832.0, 600.0),
    Point2(1664.0, 608.0),
    Point2(1984.0, 480.0),
    Point2(2240.0, 354.0),
    Point2(2496.0, 608.0),
    Point2(2240.0, 100.0),
    Point2(2753.0, 354.0),
    Point2(2975.0, 608.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
  
  Point2 blockSize(128.0, 128.0);
  
  // small igloo
  for (Coord1 i = 0; i < 4; ++i)
  {
    Point2 blockTL = Point2(512.0, 512.0) + handle;
    
    TerrainQuad* iceblock = TerrainQuad::create_block_TL(blockTL + Point2(blockSize.x * i, 0.0),
                                                         blockSize, imgsetCache[angryBlockSet][3]);
    if (i == 2)
    {
      iceblock->setImage(imgsetCache[angryBlockSet][2]);
      iceblock->ignoreAngrySkulls();
    }
    
    addBlock(iceblock);
  }
  
  addBlock(TerrainQuad::create_block_TL(Point2(768.0, 384.0) + handle,
                                        blockSize, imgsetCache[angryBlockSet][3]));
  createIglooCorners(Point2(768.0, 384.0) + handle, 1, 1);
  
  // large igloo
  for (Coord1 i = 0; i < 32; ++i)
  {
    Coord1 col = i % 8;
    Coord1 row = i / 8;
    
    if (row == 0 && (col == 0 || col == 1 || col >= 4)) continue;
    else if (row == 1 && (col == 0 || col >= 5)) continue;
    
    Point2 blockTL = Point2(1792.0, 128.0) + Point2(col * blockSize.x, row * blockSize.y) + handle;
    
    TerrainQuad* iceblock = TerrainQuad::create_block_TL(blockTL, blockSize,
                                                         imgsetCache[angryBlockSet][3]);
    
    if ((row == 1 && col == 3) || (row == 2 && col == 1) || (row == 3 && col == 5))
    {
      iceblock->setImage(imgsetCache[angryBlockSet][2]);
      iceblock->ignoreAngrySkulls();
    }
    
    addBlock(iceblock);
  }
  
  // snowball
  addBlock(TerrainQuad::create_block_circle(Point2(1344.0, 450.0) + handle, 192.0,
                                            imgsetCache[angryBlockSet][4]));
  
  createIglooCorners(Point2(2048.0, 128.0) + handle, 2, 2);
}

void ANLevelIgloo::createIglooCorners(Point2 leftTL, Coord1 blockGap, Coord1 rows)
{
  Point2 blockSize(128.0, 128.0);
  
  for (Coord1 row = 0; row < rows; ++row)
  {
    // left
    Point2 leftBase = leftTL + Point2(-blockSize.x * row, blockSize.y * row);
    Tri leftTri(leftBase,
                leftBase + Point2(-blockSize.x, blockSize.y),
                leftBase + Point2(0.0, blockSize.y));
    TerrainQuad* leftCorner = TerrainQuad::create_block_tri(leftTri, imgsetCache[angryBlockSet][0]);
    addBlock(leftCorner);
    
    // right
    Point2 rightBase = leftTL + Point2(blockGap * blockSize.x, 0.0) +
    Point2(blockSize.x * row, blockSize.y * row);
    Tri rightTri(rightBase,
                 rightBase + Point2(0.0, blockSize.y),
                 rightBase + Point2(blockSize.x, blockSize.y));
    TerrainQuad* rightCorner = TerrainQuad::create_block_tri(rightTri, imgsetCache[angryBlockSet][1]);
    addBlock(rightCorner);
  }
}

// ==================================== ANLevelPagoda

void ANLevelPagoda::createBackground()
{
  background = new KarateBG(this);
}

void ANLevelPagoda::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pagoda_ground_a"), Point2(0.0, 32.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pagoda_ground_b"), Point2(0.0, 40.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_pagoda_ground_c"), Point2(0.0, 40.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_block_special")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_block")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_divider")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_lantern")));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_pagoda_roof"),
                                                           Point2(128.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_shrine_feet")));
  imgsetCache[angryBlockSet].add(Image::create_centered(imgStr("angry_pagoda_shrine_slab")));
}

void ANLevelPagoda::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(176.0, 640.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 576.0) + handle);
  groundPts.add(Point2(64.0, 640.0) + handle);
  groundPts.add(Point2(384.0, 640.0) + handle);
  groundPts.add(Point2(576.0, 576.0) + handle);
  groundPts.add(Point2(992.0, 576.0) + handle);
  groundPts.add(Point2(1184.0, 640.0) + handle);
  groundPts.add(Point2(1888.0, 640.0) + handle);
  groundPts.add(Point2(2080.0, 576.0) + handle);
  groundPts.add(Point2(2496.0, 576.0) + handle);
  groundPts.add(Point2(2688.0, 640.0) + handle);
  groundPts.add(Point2(3008.0, 640.0) + handle);
  groundPts.add(Point2(3072.0, 576.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 584.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 584.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  // skulls come first to put them behind blocks
  Point2 skullPts[] =
  {
    Point2(630.0, 294.0),
    Point2(808.0, 554.0),
    Point2(934.0, 422.0),
    Point2(1395.0, 488.0),
    Point2(1670.0, 488.0),
    Point2(2132.0, 166.0),
    Point2(2132.0, 554.0),
    Point2(2198.0, 554.0),
    Point2(2324.0, 296.0),
    Point2(2502.0, 422.0),
    Point2(2848.0, 296.0)
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
  
  createPagoda(Point2(784.0, 224.0) + handle, 3, (0x1 << 4) | (0x1 << 6));
  createPagoda(Point2(2288.0, 96.0) + handle, 4, (0x1 << 3) | (0x1 << 8));
  
  IndexPt blockDefs[] =
  {
    // lantern
    IndexPt(3, 1504.0, 384.0),
    IndexPt(3, 2816.0, 320.0),
    IndexPt(3, 2816.0, 448.0),
    
    // feet
    IndexPt(5, 1344.0, 576.0),
    IndexPt(5, 1664.0, 576.0),
    IndexPt(5, 2816.0, 576.0),
    
    // slab
    IndexPt(6, 1376.0, 512.0)
  };
  
  for (Coord1 i = 0; i < RM::count(blockDefs); ++i)
  {
    Image* img = imgsetCache[angryBlockSet][blockDefs[i].index];
    TerrainQuad* block = TerrainQuad::create_block_TL(blockDefs[i].xy + handle,
                                                      img->natural_size(),
                                                      img);
    addBlock(block);
  }
  
}

void ANLevelPagoda::createPagoda(Point2 topCenter, Coord1 numLevels, UInt2 unfilledBits)
{
  Point2 blockSize(128.0, 96.0);
  Point2 dividerSize(480.0, 32.0);
  Coord1 currBlock = 0;
  
  TerrainQuad* roof = TerrainQuad::create_block_quad(topCenter + Point2(-64.0, -64.0),
                                                     topCenter + Point2(-192.0, 0.0),
                                                     topCenter + Point2(192.0, 0.0),
                                                     topCenter + Point2(64.0, -64.0),
                                                     imgsetCache[angryBlockSet][4]);
  addBlock(roof);
  
  for (Coord1 row = 0; row < numLevels; ++row)
  {
    // dividers
    if (row >= 1)
    {
      Point2 dividerTL = topCenter + Point2(-dividerSize.x * 0.5,
                                            blockSize.y + (blockSize.y + dividerSize.y) * (row - 1));
      
      TerrainQuad* block = TerrainQuad::create_block_TL(dividerTL, dividerSize,
                                                        imgsetCache[angryBlockSet][2]);
      addBlock(block);
    }
    
    // window blocks
    for (Coord1 col = 0; col < 3; ++col)
    {
      if (row == 0 && col == 2) break;
      
      Point2 blockTL = Point2(topCenter + Point2(-blockSize.x + blockSize.x * col, 0.0));
      
      if (row > 0)
      {
        blockTL = Point2(topCenter + Point2(-blockSize.x * 1.5 + blockSize.x * col,
                                            (blockSize.y + 32.0) * row));
      }
      
      Logical filledBlock = ((0x1 << currBlock) & unfilledBits) != 0;
      TerrainQuad* block = TerrainQuad::create_block_TL(blockTL, blockSize,
                                                        imgsetCache[angryBlockSet][1]);
      if (filledBlock == true)
      {
        block->setImage(imgsetCache[angryBlockSet][0]);
        block->ignoreAngrySkulls();
      }
      
      addBlock(block);
      
      currBlock++;
    }
  }
}

// ==================================== ANLevelCave

void ANLevelCave::createBackground()
{
  background = new MeatBG(this);
}

void ANLevelCave::loadImgs()
{
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cave_ground_a"), Point2(0.0, 64.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cave_ground_b"), Point2(0.0, 128.0)));
  imgsetCache[angryTerrainSet].add(Image::create_handled_abs(imgStr("angry_cave_ground_c"), Point2(0.0, 32.0)));
  
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_cave_rock_a"),
                                                           Point2(48.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_cave_rock_b"),
                                                           Point2(16.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_cave_rock_c"),
                                                           Point2(0.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_cave_rock_d"),
                                                           Point2(16.0, 0.0)));
  imgsetCache[angryBlockSet].add(Image::create_handled_abs(imgStr("angry_cave_rock_e"),
                                                           Point2(112.0, 0.0)));
}

void ANLevelCave::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(160.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(16);
  groundPts.add(Point2(0.0, 576.0) + handle);
  groundPts.add(Point2(64.0, 640.0) + handle);
  groundPts.add(Point2(448.0, 640.0) + handle);
  groundPts.add(Point2(576.0, 544.0) + handle);
  groundPts.add(Point2(1216.0, 544.0) + handle);
  groundPts.add(Point2(1344.0, 448.0) + handle);
  groundPts.add(Point2(2240.0, 448.0) + handle);
  groundPts.add(Point2(2368.0, 544.0) + handle);
  groundPts.add(Point2(3008.0, 544.0) + handle);
  groundPts.add(Point2(3072.0, 480.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 544.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 448.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  IndexPt blockPts[] =
  {
    IndexPt(0, 719.0, 464.0),
    IndexPt(0, 2456.0, 382.0),
    IndexPt(0, 1471.0, 244.0),
    IndexPt(0, 1472.0, 144.0),
    IndexPt(0, 1903.0, 384.0),
    IndexPt(0, 2368.0, 161.0),
    IndexPt(0, 2368.0, 425.0),
    IndexPt(0, 2487.0, 50.0),
    IndexPt(0, 2575.0, 264.0),
    IndexPt(0, 2890.0, 422.0),
    
    IndexPt(1, 752.0, 428.0),
    IndexPt(1, 800.0, 256.0),
    IndexPt(1, 1582.0, 192.0),
    IndexPt(1, 1696.0, 354.0),
    IndexPt(1, 1888.0, 144.0),
    IndexPt(1, 2104.0, 340.0),
    IndexPt(1, 2592.0, 8.0),
    IndexPt(1, 2731.0, 348.0),
    IndexPt(1, 2760.0, 108.0),
    
    IndexPt(2, 640.0, 432.0),
    IndexPt(2, 848.0, 384.0),
    IndexPt(2, 1360.0, 336.0),
    IndexPt(2, 1520.0, 128.0),
    IndexPt(2, 1728.0, 290.0),
    
    IndexPt(3, 784.0, 326.0),
    IndexPt(3, 1488.0, 320.0),
    IndexPt(3, 1808.0, 64.0),
    IndexPt(3, 2254.0, 144.0),
    IndexPt(3, 1544.0, 128.0),
    IndexPt(3, 2682.0, 244.0),
    IndexPt(3, 2860.0, 139.0),
    IndexPt(3, 2990.0, 272.0),
    
    IndexPt(4, 735.0, 303.0),
    IndexPt(4, 1631.0, 272.0),
    IndexPt(4, 1742.0, 47.0),
    IndexPt(4, 1840.0, 162.0),
    IndexPt(4, 1984.0, 274.0),
    IndexPt(4, 2318.0, 280.0),
    IndexPt(4, 2398.0, 58.0),
    IndexPt(4, 2478.0, 220.0),
    IndexPt(4, 2510.0, 335.0),
    IndexPt(4, 2709.0, 101.0),
    IndexPt(4, 2692.0, 376.0),
    IndexPt(4, 2912.0, 272.0),
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    Point2 blockXY = blockPts[i].xy + handle;
    TerrainQuad* block = NULL;
    
    switch (blockPts[i].index)
    {
      default:
      case 0:
      {
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-48.0, 32.0),
                                               blockXY + Point2(0.0, 64.0),
                                               blockXY + Point2(16.0, 48.0),
                                               imgsetCache[angryBlockSet][blockPts[i].index]);
        break;
      }
      case 1:
      {
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-16.0, 64.0),
                                               blockXY + Point2(80.0, 80.0),
                                               blockXY + Point2(64.0, 16.0),
                                               imgsetCache[angryBlockSet][blockPts[i].index]);
        break;
      }
      case 2:
      {
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(0.0, 32.0),
                                               blockXY + Point2(96.0, 16.0),
                                               blockXY + Point2(80.0, 0.0),
                                               imgsetCache[angryBlockSet][blockPts[i].index]);
        break;
      }
      case 3:
      {
        block = TerrainQuad::create_block_tri(Tri(blockXY,
                                                  blockXY + Point2(-16.0, 96.0),
                                                  blockXY + Point2(48.0, 80.0)),
                                              imgsetCache[angryBlockSet][blockPts[i].index]);
        break;
      }
      case 4:
      {
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-112.0, 48.0),
                                               blockXY + Point2(-80.0, 112.0),
                                               blockXY + Point2(16.0, 80.0),
                                               imgsetCache[angryBlockSet][blockPts[i].index]);
        break;
      }
    }
    
    block->set_restitution(0.0);
    block->set_friction(1.0);
    addBlock(block);
  }
  
  Point2 skullPts[] =
  {
    Point2(759.0, 520.0),
    Point2(1556.0, 424.0),
    Point2(1686.0, 202.0),
    Point2(1798.0, 360.0),
    Point2(2452.0, 170.0),
    Point2(2527.0, 515.0),
    Point2(2615.0, 286.0),
    Point2(2770.0, 247.0),
    Point2(2783.0, 466.0)
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== ANLevelCloud

void ANLevelCloud::createBackground()
{
  background = new CloudBG(this);
}

void ANLevelCloud::loadImgs()
{
  addImgAbs(angryTerrainSet, "angry_cloudcity_ground_a", Point2(0.0, 80.0));
  addImgAbs(angryTerrainSet, "angry_cloudcity_ground_b", Point2(0.0, 96.0));
  addImgAbs(angryTerrainSet, "angry_cloudcity_ground_c", Point2(0.0, 64.0));
  
  addImgNorm(angryBlockSet, "angry_cloudcity_block_big", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_cloudcity_block_med", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_cloudcity_block_tall", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_cloudcity_block_wide", HANDLE_C);
  
  addImgAbs(angryBlockSet, "angry_cloudcity_topper_big_bottom", Point2(0.0, 0.0));
  addImgAbs(angryBlockSet, "angry_cloudcity_topper_big_top", Point2(128.0, 0.0));
  addImgNorm(angryBlockSet, "angry_cloudcity_topper_med", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_cloudcity_topper_small", HANDLE_C);
  
  addImgAbs(angryBlockSet, "angry_cloudcity_trap_down", Point2(0.0, 0.0));
  addImgAbs(angryBlockSet, "angry_cloudcity_trap_up", Point2(48.0, 0.0));
}

void ANLevelCloud::createWorld()
{
  Point2 handle(-16.0, 352.0 - 768.0);
  
  startStanding(Point2(256.0, 672.0) + handle);
  
  ArrayList<Point2> groundPts(18);
  groundPts.add(Point2(0.0, 608.0) + handle);
  groundPts.add(Point2(64.0, 672.0) + handle);
  groundPts.add(Point2(512.0, 672.0) + handle);
  groundPts.add(Point2(560.0, 624.0) + handle);
  groundPts.add(Point2(896.0, 624.0) + handle);
  groundPts.add(Point2(944.0, 592.0) + handle);
  groundPts.add(Point2(1344.0, 592.0) + handle);
  groundPts.add(Point2(1472.0, 560.0) + handle);
  groundPts.add(Point2(1936.0, 560.0) + handle);
  groundPts.add(Point2(2048.0, 608.0) + handle);
  groundPts.add(Point2(2112.0, 608.0) + handle);
  groundPts.add(Point2(2240.0, 672.0) + handle);
  groundPts.add(Point2(2368.0, 672.0) + handle);
  groundPts.add(Point2(2432.0, 608.0) + handle);
  groundPts.add(Point2(2688.0, 608.0) + handle);
  groundPts.add(Point2(2752.0, 672.0) + handle);
  groundPts.add(Point2(3008.0, 672.0) + handle);
  groundPts.add(Point2(3072.0, 600.0) + handle);
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 592.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 608.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  IndexPt blockPts[] =
  {
    // block big
    IndexPt(0, 608.0, 240.0),
    IndexPt(0, 2496.0, 224.0),
    
    // block med
    IndexPt(1, 656.0, 464.0),
    IndexPt(1, 1664.0, 224.0),
    IndexPt(1, 1664.0, 336.0),
    IndexPt(1, 1664.0, 448.0),
    IndexPt(1, 2544.0, 448.0),
    IndexPt(1, 2848.0, 560.0),
    
    // block tall
    IndexPt(2, 1088.0, 80.0),
    IndexPt(2, 1088.0, 352.0),
    IndexPt(2, 2064.0, 368.0),
    IndexPt(2, 2432.0, 368.0),
    
    // block wide
    IndexPt(3, 2048.0, 336.0),
    
    // topper big bottom
    IndexPt(4, 1504.0, 160.0),
    
    // topper big top
    IndexPt(5, 1632.0, 96.0),
    
    // topper med
    IndexPt(6, 576.0, 160.0),
    IndexPt(6, 2768.0, 528.0),
    
    // topper small
    IndexPt(7, 1040.0, 48.0),
    IndexPt(7, 1040.0, 320.0),
    
    // trop down
    IndexPt(8, 608.0, 416.0),
    IndexPt(8, 2496.0, 400.0),
    
    // trap up
    IndexPt(9, 656.0, 192.0),
    IndexPt(9, 656.0, 576.0),
    IndexPt(9, 2544.0, 560.0),
    IndexPt(9, 2544.0, 176.0),
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    Point2 blockXY = blockPts[i].xy + handle;
    Image* blockImg = imgsetCache[angryBlockSet][blockPts[i].index];
    TerrainQuad* block = NULL;
    
    switch (blockPts[i].index)
    {
      default:
        cout << "Cloud city unrecognized block type" << endl;
      case 0:
      case 1:
      case 2:
      case 3:
      case 6:
      case 7:
        block = TerrainQuad::create_block_TL(blockXY, blockImg);
        break;
        
      case 4:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(128.0, 64.0),
                                               blockXY + Point2(288.0, 64.0),
                                               blockXY + Point2(416.0, 0.0),
                                               blockImg);
        break;
      case 5:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-128.0, 64.0),
                                               blockXY + Point2(288.0, 64.0),
                                               blockXY + Point2(160.0, 0.0),
                                               blockImg);
        break;
      case 8:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(48.0, 48.0),
                                               blockXY + Point2(144.0, 48.0),
                                               blockXY + Point2(192.0, 0.0),
                                               blockImg);
        break;
      case 9:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-48.0, 48.0),
                                               blockXY + Point2(144.0, 48.0),
                                               blockXY + Point2(96.0, 0.0),
                                               blockImg);
        break;
    }
    
    block->set_restitution(0.0);
    block->set_friction(1.0);
    addBlock(block);
  }
  
  Point2 skullPts[] =
  {
    Point2(775.0, 135.0),
    Point2(1095.0, 22.0),
    Point2(1062.0, 294.0),
    Point2(1145.0, 294.0),
    Point2(1680.0, 70.0),
    Point2(2118.0, 310.0),
    Point2(2390.0, 311.0),
    Point2(2441.0, 312.0),
    Point2(2615.0, 148.0),
    Point2(2822.0, 505.0),
    Point2(2896.0, 505.0),
    Point2(2966.0, 505.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== ANLevelMarket

void ANLevelMarket::createBackground()
{
  background = new MarketBG(this);
}

void ANLevelMarket::loadImgs()
{
  addImgAbs(angryTerrainSet, "angry_market_ground_a", Point2(0.0, 16.0));
  addImgAbs(angryTerrainSet, "angry_market_ground_b", Point2(0.0, 72.0));
  addImgAbs(angryTerrainSet, "angry_market_ground_c", Point2(0.0, 72.0));
  
  addImgNorm(angryBlockSet, "angry_market_fruit_a", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_fruit_b", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_fruit_c", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_barrel", HANDLE_C);
  
  addImgNorm(angryBlockSet, "angry_market_crate", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_post", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_post_big", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_roof", HANDLE_C);
  addImgNorm(angryBlockSet, "angry_market_stand", HANDLE_C);
  
  addImgAbs(angryBlockSet, "angry_market_tent", Point2(32.0, 0.0));
  addImgAbs(angryBlockSet, "angry_market_wagon", Point2(0.0, 0.0));
}

void ANLevelMarket::createWorld()
{
  Point2 handle(-16.0, 352.0 - 608.0);
  
  startStanding(Point2(184.0, 296.0) + handle);
  
  DataList<Point2> groundPts(18);
  groundPts.add(Point2(0.0, 392.0) + handle);
  groundPts.add(Point2(64.0, 448.0) + handle);
  groundPts.add(Point2(1504.0, 448.0) + handle);
  groundPts.add(Point2(1600.0, 544.0) + handle);
  groundPts.add(Point2(3008.0, 544.0) + handle);
  groundPts.add(Point2(3072.0, 480.0) + handle);
  
  addAlly(new TerrainGround(groundPts));
  
  worldBox = Box(Point2(0.0, -SCREEN_H * 3.0), Point2(groundPts.last().x, SCREEN_H * 4.0));
  invisibleWallsStd(worldBox);
  
  addAction(new Decoration(groundPts[0], imgsetCache[angryTerrainSet][0]));
  addAction(new Decoration(Point2(1024.0, 448.0) + handle, imgsetCache[angryTerrainSet][1]));
  addAction(new Decoration(Point2(2048.0, 544.0) + handle, imgsetCache[angryTerrainSet][2]));
  
  IndexPt blockPts[] =
  {
    // limes (fruit a)
    IndexPt(0, 888.0, 424.0),
    IndexPt(0, 936.0, 224.0),
    IndexPt(0, 960.0, 296.0),
    IndexPt(0, 1032.0, 424.0),
    IndexPt(0, 1768.0, 408.0),
    IndexPt(0, 1816.0, 408.0),
    IndexPt(0, 2696.0, 478.0),
    IndexPt(0, 2744.0, 478.0),
    
    // oranges (fruit b)
    IndexPt(1, 960.0, 386.0),
    IndexPt(1, 984.0, 424.0),
    IndexPt(1, 2672.0, 432.0),
    IndexPt(1, 2720.0, 432.0),
    IndexPt(1, 2720.0, 520.0),
    
    // apples (fruit c)
    IndexPt(2, 936.0, 424.0),
    IndexPt(2, 1008.0, 384.0),
    IndexPt(2, 1368.0, 312.0),
    IndexPt(2, 1416.0, 312.0),
    IndexPt(2, 1416.0, 234.0),
    IndexPt(2, 1440.0, 272.0),
    IndexPt(2, 2648.0, 480.0),
    IndexPt(2, 2696.0, 392.0),
    
    // barrel
    IndexPt(3, 768.0, 416.0),
    IndexPt(3, 2032.0, 512.0),
    IndexPt(3, 2096.0, 512.0),
    IndexPt(3, 2128.0, 456.0),
    IndexPt(3, 2160.0, 512.0),
    
    // crate
    IndexPt(4, 512.0, 384.0),
    IndexPt(4, 576.0, 384.0),
    IndexPt(4, 544.0, 320.0),
    IndexPt(4, 2304.0, 480.0),
    IndexPt(4, 2368.0, 480.0),
    IndexPt(4, 2432.0, 480.0),
    IndexPt(4, 2320.0, 416.0),
    IndexPt(4, 2400.0, 416.0),
    IndexPt(4, 2336.0, 352.0),
    
    // post
    IndexPt(5, 448.0, 272.0),
    IndexPt(5, 704.0, 272.0),
    IndexPt(5, 832.0, 272.0),
    IndexPt(5, 1088.0, 272.0),
    
    // post_big
    IndexPt(6, 1680.0, 240.0),
    IndexPt(6, 1952.0, 240.0),
    IndexPt(6, 2240.0, 240.0),
    IndexPt(6, 2528.0, 240.0),
    IndexPt(6, 2800.0, 240.0),
    
    // roof
    IndexPt(7, 1680.0, 80.0),
    IndexPt(7, 1968.0, 80.0),
    IndexPt(7, 2256.0, 80.0),
    IndexPt(7, 2544.0, 80.0),
    
    // stand
    IndexPt(8, 1728.0, 432.0),
    
    // tent
    IndexPt(9, 480.0, 224.0),
    IndexPt(9, 864.0, 224.0),
    
    // wagon
    IndexPt(10, 1216.0, 336.0),
  };
  
  for (Coord1 i = 0; i < RM::count(blockPts); ++i)
  {
    Point2 blockXY = blockPts[i].xy + handle;
    Image* blockImg = imgsetCache[angryBlockSet][blockPts[i].index];
    TerrainQuad* block = NULL;
    
    switch (blockPts[i].index)
    {
      default:
        cout << "Cloud city unrecognized block type" << endl;
      case 0:
      case 1:
      case 2:
      case 3:
      {
        block = TerrainQuad::create_block_circle(blockXY, 24.0, blockImg);
        block->body->SetAwake(false);
        break;
      }
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
        block = TerrainQuad::create_block_TL(blockXY, blockImg);
        break;
        
        // tent
      case 9:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(-32.0, 48.0),
                                               blockXY + Point2(240.0, 48.0),
                                               blockXY + Point2(208.0, 0.0),
                                               blockImg);
        break;
        
        // wagon
      case 10:
        block = TerrainQuad::create_block_quad(blockXY,
                                               blockXY + Point2(128.0, 112.0),
                                               blockXY + Point2(272.0, 112.0),
                                               blockXY + Point2(272.0, 0.0),
                                               blockImg);
        break;
    }
    
    block->set_restitution(0.0);
    block->set_friction(1.0);
    addBlock(block);
  }
  
  Point2 skullPts[] =
  {
    Point2(912.0, 378.0),
    Point2(982.0, 340.0),
    
    Point2(1392.0, 272.0),
    Point2(1464.0, 312.0),
    
    Point2(2102.0, 56.0),
    Point2(2152.0, 56.0),
    Point2(2678.0, 56.0),
    
    Point2(1880.0, 408.0),
    Point2(2064.0, 464.0),
    Point2(2360.0, 328.0),
    Point2(2422.0, 392.0),
    Point2(2624.0, 520.0),
    Point2(2672.0, 520.0),
    Point2(2678.0, 520.0),
  };
  
  for (Coord1 i = 0; i < RM::count(skullPts); ++i)
  {
    createAngrySkull(skullPts[i] + handle);
  }
}

// ==================================== SlopeLevel ================================ //

SlopeLevel::SlopeLevel() :
PizzaLevel(),
numChunks(12),
camYOffset(64.0),
maxCameraBottom(0.0, 0.0),
currPlayerChunk(0)
{
  musicFilename = "slopes.ima4";
}

void SlopeLevel::preload()
{
  background = new SlopesBG(this);
}

// overrides
void SlopeLevel::loadRes()
{
  background->load();
  
  Image::create_tiles_abs(imgStr("slope_flag_big"), imgsetCache[bigFlagSet], 5, 1, Point2(65.0, 128.0));
  Image::create_tiles_abs(imgStr("slope_flag_small"), imgsetCache[flagSet], 5, 1, Point2(32.0, 64.0));
  Image::create_tiles_abs(imgStr("snowboarder"), imgsetCache[snowboardSet], 4, 2, Point2(81.0, 128.0));
  Image::create_tiles_abs(imgStr("switch"), imgsetCache[skullSwitchSet], 2, 1, Point2(32.0, 48.0));
  imgCache[slope2_1] = Image::create_handled_abs(imgStr("slope_2_1"), Point2(0.0, 64.0));
  imgCache[slope3_1] = Image::create_handled_abs(imgStr("slope_3_1"), Point2(0.0, 64.0));
  imgCache[slope4_1] = Image::create_handled_abs(imgStr("slope_4_1"), Point2(0.0, 64.0));
  imgCache[slope3_1_break] = Image::create_handled_abs(imgStr("slope_3_1_breakaway"), Point2(0.0, 64.0));
  imgCache[slope3_1_short] = Image::create_handled_abs(imgStr("slope_3_1_short"), Point2(0.0, 64.0));
  imgCache[slopeBouncy] = Image::create_handled_abs(imgStr("slope_bouncy"), Point2(0.0, 64.0));
  imgCache[slopeFlatLong] = Image::create_handled_abs(imgStr("slope_flat_long"), Point2(0.0, 64.0));
  imgCache[slopeFlatShort] = Image::create_handled_abs(imgStr("slope_flat_short"), Point2(0.0, 64.0));
  imgCache[slopeSpikePlatform] = Image::create_handled_abs(imgStr("slope_snow_spikes_up"), Point2(0.0, 64.0));
  imgCache[slopeSpikesDown] = Image::create_handled_abs(imgStr("slope_snow_spikes_down"), Point2(0.0, 64.0));
  imgCache[slopeTriangle] = Image::create_handled_abs(imgStr("slope_triangle"), Point2(32.0, 64.0));  // top left
  imgCache[slopeUphill] = Image::create_handled_abs(imgStr("slope_uphill"), Point2(0.0, 128.0));
  imgCache[spikeball] = Image::create_centered(imgStr("spikeball"));
  
  imgCache[flamingSkull] = Image::create_centered(imgStr("flameskull"));
  Image::create_tiles_abs(imgStr("flameskull_flame"), imgsetCache[flameskullFlameSet], 1, 3,
                          Point2(116.0, 64.0), true);
  
  imgCache[slopeSnowman] = Image::create_handled_abs(imgStr("snowman"), Point2(80.0, 214.0));
  Image::create_tiles_abs(imgStr("snowman_scarf"), imgsetCache[scarfSet], 3, 1, Point2(22.0, 9.0));
  
  loadSound(flagSound, "flagget.wav");
}

void SlopeLevel::unloadRes()
{
  
}

void SlopeLevel::loadMe()
{
  setInitialWeights();
  
  // invisible starting wall
  addEnemy(TerrainQuad::create_ground_TL(Point2(-64.0, -128.0), Point2(64.0, 512.0), NULL));
  createChunk(CHUNK_SKI_START);
  
  startStanding(Point2(196.0, 300.0));
  maxCameraBottom = Point2(player->getX(), camera.getY() + PLAY_H);
  
  for (Coord1 i = 0; i < numChunks; ++i)
  {
    generateNextChunk(i);
    // createChunk(randGen.randi(CHUNK_SKI_START + 1, CHUNK_SKI_END - 1));
    // createChunk(CHUNK_SKI_FLAMESKULL);
  }
  
  createChunk(CHUNK_SKI_END);
  
  // invisible ending wall
  addEnemy(TerrainQuad::create_ground_TL(endpoint + Point2(0.0, -512.0), Point2(64.0, 544.0), NULL));
  
  camera.zoom.set(0.75, 0.75);
  updateCamera();
}

void SlopeLevel::generateNextChunk(Coord1 chunkNum)
{
  createRandomChunk();
}

Point1 SlopeLevel::playerTXVel()
{
  return 600.0;
}

void SlopeLevel::updateCamera()
{
  // avoids weird situations with too many constraints on the camera
  if (levelState == LEVEL_LOSE) return;
  
  Point2 cameraStart = camera.getXY();
  
  Point1 cameraXTarget = player->getX();
  if (tiltPercent < 0.0) cameraXTarget = player->getX() + tiltPercent * PLAY_W * 0.33;
  
  Point2 playerMove = player->getXY() - player->oldXY;
  
  camera.xy.x += playerMove.x;
  if (std::abs(cameraXTarget - camera.xy.x) > 32.0)
  {
    camera.xy.x = RM::flatten(camera.xy.x, cameraXTarget, PLAY_W * 0.75
                              * std::abs(tiltPercent) * RM::timePassed());
    
  }
  
  Box camBox = camera.myBox();
  Point1 camXDiff = camBox.right() - maxCameraBottom.x;
  if (camXDiff > 0.0) maxCameraBottom.y += camXDiff / 6.0;
  
  maxCameraBottom.x = std::max(camBox.right(), maxCameraBottom.x);
  maxCameraBottom.y = std::max(camBox.bottom(), maxCameraBottom.y);
  
  RM::flatten_me(camYOffset, player->isStanding ? 64.0 : -64.0, 128.0 * 0.6 * RM::timePassed());
  camera.xy.y = player->getY() - player->getHeight() + camYOffset;
  // cout << "camera.xy.x " << camera.xy.x << endl;
  
  camera.handle.set(player->getX() - camera.getX(), 0.0);
  
  Point1 zoomTarget = player->isStanding ? 0.75 : 0.50;
  camera.zoom.x = RM::flatten(camera.zoom.x, zoomTarget, 0.25 * 0.6 * RM::timePassed());
  camera.zoom.y = camera.zoom.x;
  
  Point1 alternativeMin = camera.calcMinZoom(Point2(player->getX() + 1.0, maxCameraBottom.y), true);
  camera.setZoom(std::min(camera.getZoom().x, alternativeMin));
  
  camera.clampHorizontal(Point2(-Pizza::platformTL.x * 2.0 / camera.zoom.x, endpoint.x) + Pizza::platformTL.x / camera.zoom.x);
  
  camera.clampVertical(Point2(-1024.0 + -Pizza::platformTL.y / camera.zoom.y,
                              endpoint.y + 160.0 + Pizza::platformTL.x / camera.zoom.y));
  Pizza::cameraMoved = camera.getXY() - cameraStart;
}

void SlopeLevel::updateStatus()
{
  // cout << "xy " << player->getXY() << " vel " << player->get_velocity() << endl;
  if (currPlayerChunk < runningEnpoints.count)
  {
    if (player->collisionCircle().right() > runningEnpoints[currPlayerChunk + 1].x)
    {
      currPlayerChunk++;
    }
  }
  
  Logical belowEnd = player->collisionCircle().top() >
  runningEnpoints[std::min(currPlayerChunk + 1, runningEnpoints.last_i())].y;
  
  if (belowEnd == true)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void SlopeLevel::setInitialWeights()
{
  chunkWeights[CHUNK_SKI_4_1] = 1.0;
}

// =============================== SLLevelA

SLLevelA::SLLevelA() :
SlopeLevel(),
slowX(0.0),
readyX(0.0),
jumpX(0.0),
showedSlow(false),
showedReady(false),
showedJump(false)
{
  numChunks = 17;
}


void SLLevelA::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_4_1); break;
    case 1: createChunk(CHUNK_SKI_4_1); break;
    case 2: createChunk(CHUNK_SKI_2_1); break;
    case 3: createChunk(CHUNK_SKI_3_1); break;
    case 4: createChunk(CHUNK_SKI_JUMP); break;
    case 5: createChunk(CHUNK_SKI_2_1); break;
    case 6: createChunk(CHUNK_SKI_3_1); readyX = endpoint.x; break;
    case 7: createChunk(CHUNK_SKI_3_1); jumpX = endpoint.x + 512.0; break;
    case 8: createChunk(CHUNK_SKI_LONGFALL); break;
    case 9: createChunk(CHUNK_SKI_4_1); break;
    case 10: createChunk(CHUNK_SKI_2_1); break;
    case 11: createChunk(CHUNK_SKI_JUMP); slowX = endpoint.x; break;
    case 12: createChunk(CHUNK_SKI_2_1); break;
    case 13: createChunk(CHUNK_SKI_3_1); break;
    case 14: createChunk(CHUNK_SKI_DROP); break;
    case 15: createChunk(CHUNK_SKI_JUMP); break;
    case 16: createChunk(CHUNK_SKI_3_1); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

void SLLevelA::updateLevelAI()
{
  if (levelState != LEVEL_PLAY)
  {
    // don't do it, avoids weird things with losing
  }
  else if (alreadyBeaten() == true)
  {
    // don't show tutorial
  }
  else if (player->getX() > readyX && showedReady == false)
  {
    addTextEffectStd(LSTRINGS[96], Point2(PLAY_CX, PLAY_CY - 64.0), 1.0);
    showedReady = true;
  }
  else if (player->getX() > jumpX && showedJump == false)
  {
    addTextEffectStd(LSTRINGS[97], Point2(PLAY_CX, PLAY_CY), 1.0);
    showedJump = true;
  }
  else if (player->getX() > slowX && showedSlow == false)
  {
    addTextEffectStd(LSTRINGS[RMSystem->get_OS_family() == MAC_FAMILY ? 138 : 98], Point2(PLAY_CX, PLAY_CY));
    showedSlow = true;
  }
}

// =============================== SLLevelB

void SLLevelB::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_4_1); break;
    case 1: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 2: createChunk(CHUNK_SKI_JUMP); break;
    case 3: createChunk(CHUNK_SKI_3_1); break;
    case 4: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 5: createChunk(CHUNK_SKI_LONGFALL); break;
    case 6: createChunk(CHUNK_SKI_2_1); break;
    case 7: createChunk(CHUNK_SKI_DROP); break;
    case 8: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 9: createChunk(CHUNK_SKI_3_1); break;
    case 10: createChunk(CHUNK_SKI_4_1); break;
    case 11: createChunk(CHUNK_SKI_LONGFALL); break;
    case 12: createChunk(CHUNK_SKI_LONGFALL); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelC

void SLLevelC::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 1: createChunk(CHUNK_SKI_JUMP); break;
    case 2: createChunk(CHUNK_SKI_DROP); break;
    case 3: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 4: createChunk(CHUNK_SKI_LONGFALL); break;
    case 5: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 6: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 7: createChunk(CHUNK_SKI_LONGFALL); break;
    case 8: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 9: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 10: createChunk(CHUNK_SKI_DROP); break;
    case 11: createChunk(CHUNK_SKI_FLAMESKULL); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelD

void SLLevelD::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 1: createChunk(CHUNK_SKI_2_1); break;
    case 2: createChunk(CHUNK_SKI_3_1); break;
    case 3: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 4: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 5: createChunk(CHUNK_SKI_FLAMESKULL, 1); break;
    case 6: createChunk(CHUNK_SKI_DROP); break;
    case 7: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 8: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 9: createChunk(CHUNK_SKI_LONGFALL); break;
    case 10: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 11: createChunk(CHUNK_SKI_BOUNCEGAP); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelE

void SLLevelE::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_SWITCHSLOPE); break;
    case 1: createChunk(CHUNK_SKI_DROP); break;
    case 2: createChunk(CHUNK_SKI_JUMP); break;
    case 3: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 4: createChunk(CHUNK_SKI_SWITCHSLOPE); break;
    case 5: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 6: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 7: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 8: createChunk(CHUNK_SKI_SWITCHSLOPE); break;
    case 9: createChunk(CHUNK_SKI_JUMP); break;
    case 10: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 11: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelF

void SLLevelF::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_FALLBREAK); break;
    case 1: createChunk(CHUNK_SKI_DROP); break;
    case 2: createChunk(CHUNK_SKI_LONGFALL); break;
    case 3: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 4: createChunk(CHUNK_SKI_FALLBREAK); break;
    case 5: createChunk(CHUNK_SKI_GAP); break;
    case 6: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 7: createChunk(CHUNK_SKI_FALLBREAK); break;
    case 8: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 9: createChunk(CHUNK_SKI_JUMP); break;
    case 10: createChunk(CHUNK_SKI_GAP); break;
    case 11: createChunk(CHUNK_SKI_GAP); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelG

void SLLevelG::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_SPIKEFALL); break;
    case 1: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 2: createChunk(CHUNK_SKI_LONGFALL); break;
    case 3: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 4: createChunk(CHUNK_SKI_SPIKEFALL); break;
    case 5: createChunk(CHUNK_SKI_BOUNCEJUMP); break;
    case 6: createChunk(CHUNK_SKI_JUMP); break;
    case 7: createChunk(CHUNK_SKI_SPIKEFALL); break;
    case 8: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 9: createChunk(CHUNK_SKI_DROP); break;
    case 10: createChunk(CHUNK_SKI_BOUNCEJUMP); break;
    case 11: createChunk(CHUNK_SKI_BOUNCEJUMP); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelH

void SLLevelH::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_SPIKEPATHS); break;
    case 1: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 2: createChunk(CHUNK_SKI_SWITCHSLOPE); break;
    case 3: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 4: createChunk(CHUNK_SKI_SPIKEPATHS); break;
    case 5: createChunk(CHUNK_SKI_HILLSKULL); break;
    case 6: createChunk(CHUNK_SKI_JUMP); break;
    case 7: createChunk(CHUNK_SKI_SPIKEPATHS); break;
    case 8: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 9: createChunk(CHUNK_SKI_GAP); break;
    case 10: createChunk(CHUNK_SKI_HILLSKULL); break;
    case 11: createChunk(CHUNK_SKI_HILLSKULL); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelI

void SLLevelI::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_SPIKEPLATFORMS); break;
    case 1: createChunk(CHUNK_SKI_FALLBREAK); break;
    case 2: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 3: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 4: createChunk(CHUNK_SKI_SPIKEPLATFORMS); break;
    case 5: createChunk(CHUNK_SKI_SWITCHGAP); break;
    case 6: createChunk(CHUNK_SKI_GAP); break;
    case 7: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 8: createChunk(CHUNK_SKI_SPIKEPLATFORMS); break;
    case 9: createChunk(CHUNK_SKI_BOUNCEJUMP); break;
    case 10: createChunk(CHUNK_SKI_SWITCHGAP); break;
    case 11: createChunk(CHUNK_SKI_SWITCHGAP); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// =============================== SLLevelJ

void SLLevelJ::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SKI_JAGSKULL); break;
    case 1: createChunk(CHUNK_SKI_BOUNCEGAP); break;
    case 2: createChunk(CHUNK_SKI_GAP); break;
    case 3: createChunk(CHUNK_SKI_FLAMESKULL); break;
    case 4: createChunk(CHUNK_SKI_HILLSKULL); break;
    case 5: createChunk(CHUNK_SKI_BOUNCEJUMP); break;
    case 6: createChunk(CHUNK_SKI_SPIKEFALL); break;
    case 7: createChunk(CHUNK_SKI_SWITCHSLOPE); break;
    case 8: createChunk(CHUNK_SKI_DROP); break;
    case 9: createChunk(CHUNK_SKI_SPIKEPATHS); break;
    case 10: createChunk(CHUNK_SKI_SPIKEPLATFORMS); break;
    case 11: createChunk(CHUNK_SKI_LONGFALL); break;
    case 12: createChunk(CHUNK_SKI_JUMP); break;
    case 13: createChunk(CHUNK_SKI_DROP); break;
    case 14: createChunk(CHUNK_SKI_JAGSKULL); break;
    case 15: createChunk(CHUNK_SKI_JAGSKULL); break;
    case 16: createChunk(CHUNK_SKI_SWITCHGAP); break;
    case 17: createChunk(CHUNK_SKI_SPIKEFALL); break;
    case 18: createChunk(CHUNK_SKI_DUALCOIN); break;
    case 19: createChunk(CHUNK_SKI_SPIKEPLATFORMS); break;
    case 20: createChunk(CHUNK_SKI_STRAIGHTBREAK); break;
    case 21: createChunk(CHUNK_SKI_GAP); break;
    case 22: createChunk(CHUNK_SKI_FALLBREAK); break;
    case 23: createChunk(CHUNK_SKI_LONGFALL); break;
    case 24: createChunk(CHUNK_SKI_GAP); break;
      
    default: createChunk(CHUNK_SKI_4_1); break;
  }
}

// ==================================== KarateLevel ================================ //

KarateLevel::KarateLevel() :
PizzaLevel(),
numBoards(10),
numMids(-1),  // means unset
midList(10),
boardList(numBoards),
groundY(0.0),

standTimer(1.0 / 30.0, this),
justJumpedTimer(0.5),
tutTimer(0.5, this),

numBoost(0),
boardsBroken(-1)
{
  endpoint.set(0.0, 0.0);
  standTimer.setInactive();
  tutTimer.setInactive();
  musicFilename = "karate.ima4";
}

void KarateLevel::preload()
{
  background = new KarateBG(this);
}

void KarateLevel::loadRes()
{
  background->load();
  
  imgCache[karateStart] = Image::create_handled_abs(imgStr("karate_start"), Point2(0.0, 176.0));
  imgCache[karateMiddle] = Image::create_handled_abs(imgStr("karate_middle"), Point2(0.0, 64.0));
  imgCache[karateEnd] = Image::create_handled_abs(imgStr("karate_end"), Point2(0.0, 144.0));
  imgCache[karateBlock] = Image::create_centered(imgStr("karate_block"));
  imgCache[karateWood] = Image::create_centered(imgStr("karate_wood"));
  imgCache[karateSpear] = Image::create_handled_abs(imgStr("karate_spear"), Point2(16.0, 12.0));
  imgCache[brainStill] = Image::create_handled_abs(imgStr("brain_stationary"), Point2(72.0, 98.0));
  loadImgAbs(brainLead, "brain_lead", Point2(72.0, 98.0));
  Image::create_tiles_abs(imgStr("brain_bounce"), imgsetCache[brainBounce], 2, 1, Point2(72.0, 114.0));
  Image::create_tiles_abs(imgStr("brain_circle"), imgsetCache[brainCircle], 3, 1, Point2(136.0, 119.0));
  Image::create_tiles_abs(imgStr("brain_fly"), imgsetCache[brainFly], 3, 1, Point2(136.0, 119.0));
  Image::create_tiles_abs(imgStr("brain_walk"), imgsetCache[brainWalk], 4, 1, Point2(76.0, 101.0));
}

void KarateLevel::unloadRes()
{
  
}

void KarateLevel::loadMe()
{
  startStanding(Point2(128.0, 256.0));
  numBoards = RM::clamp(levelID + 1, 2, 10);  // ensures first level has 2
  if (numMids == -1) numMids = numBoards + 1;
  
  // ===== start level terrain
  Point2 begin(0.0, 256.0);
  
  const Coord1 NUM_START_TOPS = 5;
  Point2 startTops[NUM_START_TOPS] =
  {
    Point2(0.0, 0.0),
    Point2(320.0, 0.0),
    Point2(480.0, -64.0),
    Point2(784.0, -64.0),
    Point2(960.0, -128.0)
  };
  
  for (Coord1 i = 0; i < NUM_START_TOPS - 1; ++i)
  {
    TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                        startTops[i] + begin, Point2(startTops[i].x, 64.0) + begin,
                                                        Point2(startTops[i + 1].x, 64.0) + begin, startTops[i + 1] + begin);
    
    if (i == 0) quad->setImage(imgCache[karateStart]);
    
    addBGTerrain(quad);
  }
  
  // middle sections
  Point2 midSize(KARATE_MIDDLE_WIDTH, 64.0);
  groundY = begin.y;
  
  for (Coord1 i = 0; i < numMids; ++i)
  {
    Point2 startMid = Point2(startTops[NUM_START_TOPS - 1].x + i * midSize.x, 0.0) + begin;
    
    TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                        startMid, startMid + Point2(0.0, midSize.y),
                                                        startMid + midSize, startMid + Point2(midSize.x, 0.0));
    
    quad->setImage(imgCache[karateMiddle]);
    midList.add(quad);
    addBGTerrain(quad);
  }
  
  // end base
  Point2 startEnd = Point2(startTops[NUM_START_TOPS - 1].x + numMids * midSize.x, 0.0) + begin;
  
  Point2 endTops[] =
  {
    Point2(0.0, 0.0),
    Point2(64.0, -16.0),
    Point2(416.0, -16.0),
    Point2(480.0, 0.0)
  };
  
  for (Coord1 i = 0; i < RM::count(endTops) - 1; ++i)
  {
    TerrainQuad* quad = TerrainQuad::create_static_quad(
                                                        startEnd + endTops[i], startEnd + Point2(endTops[i].x, 64.0),
                                                        startEnd + Point2(endTops[i + 1].x, 64.0), startEnd + endTops[i + 1]);
    
    if (i == 0) quad->setImage(imgCache[karateEnd]);
    else if (i == RM::count(endTops) - 2) worldBox.wh.x = (startEnd + Point2(endTops[i + 1].x, 64.0)).x;
    
    addBGTerrain(quad);
  }
  
  // concrete blocks
  for (Coord1 i = 0; i < 2; ++i)
  {
    TerrainQuad* block = TerrainQuad::create_ground_TL(
                                                       startEnd + Point2(64.0 + i * 288.0, -80.0), Point2(64.0, 64.0),
                                                       imgCache[karateBlock]);
    
    addBGTerrain(block);
  }
  
  // boards
  for (Coord1 i = 0; i < numBoards; ++i)
  {
    Point2 boardCenter = startEnd + Point2(96.0, -112.0 + (-32.0 * i));
    
    TerrainQuad* board = TerrainQuad::create_block_TL(
                                                      boardCenter, imgCache[karateWood]->natural_size(),
                                                      imgCache[karateWood]);
    
    board->crushesPhysical = false;  // the level crushes them manually
    boardList.add(board);
    
    addAlly(board);
  }
  
  worldBox.xy.set(0.0, -PLAY_H * 10.0);
  worldBox.wh.y = PLAY_H * 11.0;
  
  Point1 wallThickness = 128.0;
  
  // left wall
  addAlly(TerrainQuad::create_ground_TL(Point2(worldBox.left() - wallThickness, worldBox.top()),
                                        Point2(wallThickness, worldBox.height()),
                                        NULL));
  
  // top wall
  addAlly(TerrainQuad::create_ground_TL(Point2(worldBox.left(), worldBox.top() - wallThickness),
                                        Point2(worldBox.width(), wallThickness),
                                        NULL));
  createBrains();
}

void KarateLevel::updateCamera()
{
  camera.handle.set(SCREEN_CX, SCREEN_H + Pizza::platformTL.y);
  
  camera.xy = Point2(player->getX() - 160.0, 0.0);
  updateZoomStd();
  
  // camera.setZoom(0.05);
  camera.clampHorizontal(Point2(worldBox.left() - Pizza::platformTL.x / camera.zoom.x,
                                worldBox.right() + 2048.0));
}

void KarateLevel::updateLevelAI()
{
  // cout << player->getXY() << endl;
  
  if (player->canJump() == true)
  {
    standTimer.setActive(true);
  }
  else
  {
    // not standing
    standTimer.reset();
  }
  standTimer.update();
  justJumpedTimer.update();
  tutTimer.update();
}

void KarateLevel::updateStatus()
{
  if (player->touchingPGO(boardList.last()) == true)
  {
    boardsBroken = boardsToBreak();
    Pizza::currGameVars[VAR_DEFEATED] = boardsBroken;
    
    for (Coord1 i = 0; i < boardsBroken; ++i)
    {
      // changing the desctruction to 2,2 is fast enough on iphone 4
      Coord1 currBoard = numBoards - 1 - i;
      
      Coord1 chunkHigh = 4;
      if (levelID >= 3) chunkHigh--;
      if (levelID >= 6) chunkHigh--;
      
      boardList[currBoard]->destroyPhysicalStd(Coord2(2, chunkHigh), imgCache[karateWood]);
    }
    
    resetBounce();
    
    if (boardsBroken > 0) winLevel();
    else loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
  else if (player->collisionCircle().top() > worldBox.bottom())
  {
    loseLevel();
  }
}

Logical KarateLevel::slamDisabled()
{
  return justJumpedTimer.getActive();
}

Coord1 KarateLevel::boardsToBreak()
{
  Coord1 result = numBoost;
  if (player->isSlamming == true) result++;
  
  result = RM::clamp(result, 0, boardList.count);
  return result;
}

void KarateLevel::pizzaDamaged()
{
  numBoost = 0;
}

void KarateLevel::landedOnEnemy(PizzaGO* enemy)
{
  numBoost++;
  
  Coord1 coinVal = RM::clamp(numBoost - 1, (Coord1) PizzaGO::PENNY, (Coord1) PizzaGO::QUARTER);
  addAlly(new MagnetCoin(enemy->getXY(), coinVal));
  gotScore(25 * numBoost);
  
  if (numBoost >= 2)
  {
    BonusText* textEffect = new BonusText(String2("x$", '$', numBoost), ResourceManager::font,
                                          enemy->getXY(), false);
    textEffect->upwardVal = -96.0;
    textEffect->holdTime = 1.0;
    textEffect->init();
    addAction(textEffect);
  }
  
  justJumpedTimer.reset();
}

void KarateLevel::resetBounce()
{
  // also called when the player jumps a brain for the 2+ time
  numBoost = 0;
  justJumpedTimer.reset();
}

Point1 KarateLevel::bounceMult()
{
  return 1.0 + 0.25 * numBoost;
}

Point1 KarateLevel::getGroundY(Point1 xCoord)
{
  return groundY;
}

Coord1 KarateLevel::calcWinBonus()
{
  return 1000;
}

void KarateLevel::callback(ActionEvent* caller)
{
  if (caller == &standTimer)
  {
    standTimer.reset();
    
    if (player->isStanding == true && numBoost > 0 && boardsBroken == -1)
    {
      resetBounce();
      standTimer.setActive(false);
    }
  }
  if (caller == &tutTimer && alreadyBeaten() == false)
  {
    addTextEffectStd(LSTRINGS[99], PLAY_CENTER + Point2(0.0, -96.0));
  }
}

// ==================================== KALevelStill

KALevelStill::KALevelStill() :
KarateLevel()
{
  numMids = 2;
}

void KALevelStill::createBrains()
{
  Point1 xList[] = {1192.0, 1696.0};
  
  for (Coord1 i = 0; i < 2; ++i)
  {
    Point2 midPieceTL = midList[i]->getXY();
    
    KarateBrain* spring = new KarateBrain();
    spring->setXY(xList[i], midPieceTL.y - spring->getHeight());
    spring->mover = new BrainMover(spring);
    addAlly(spring);
  }
  
  Point1 yHandle = 352.0;
  
  createCoin(Point2(782.0, 439.0), yHandle);
  createCoin(Point2(872.0, 403.0), yHandle);
  createCoin(Point2(960.0, 371.0), yHandle);
  createCoin(Point2(1056.0, 360.0), yHandle);
  createCoin(Point2(1146.0, 380.0), yHandle);
  createCoin(Point2(1187.0, 470.0), yHandle);
  
  createCoin(Point2(1240.0, 349.0), yHandle);
  createCoin(Point2(1297.0, 276.0), yHandle);
  createCoin(Point2(1362.0, 207.0), yHandle);
  createCoin(Point2(1451.0, 173.0), yHandle);
  createCoin(Point2(1524.0, 207.0), yHandle);
  createCoin(Point2(1605.0, 276.0), yHandle);
  createCoin(Point2(1664.0, 353.0), yHandle);
  createCoin(Point2(1700.0, 441.0), yHandle);
  
  createCoin(Point2(1723.0, 347.0), yHandle);
  createCoin(Point2(1754.0, 261.0), yHandle);
  createCoin(Point2(1797.0, 170.0), yHandle);
  createCoin(Point2(1849.0, 95.0), yHandle);
  createCoin(Point2(1928.0, 41.0), yHandle);
  createCoin(Point2(2012.0, 88.0), yHandle);
  createCoin(Point2(2065.0, 165.0), yHandle);
  createCoin(Point2(2106.0, 254.0), yHandle);
  createCoin(Point2(2139.0, 344.0), yHandle);
  createCoin(Point2(2158.0, 435.0), yHandle);
  /*
   for (Coord1 i = 0; i < midList.count; ++i)
   {
   Point2 midPieceTL = midList[i]->getXY();
   Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
   
   KarateBrain* spring = new KarateBrain();
   spring->setXY(RM::lerp(endpoints, 0.5), midPieceTL.y - spring->getHeight());
   spring->mover = new BrainMover(spring);
   addAlly(spring);
   }
   */
}

void KALevelStill::resetBounce()
{
  KarateLevel::resetBounce();
  
  if (tutShown == false && boardsBroken < 1 &&
      alreadyBeaten() == false)
  {
    startTutorial(new KarateTutorial(this));
    tutShown = true;
  }
}

// ==================================== KALevelBounce 2

void KALevelBounce::createBrains()
{
  Point1 midWeights[] = {0.0, 0.2, 0.4};
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    spring->setXY(RM::lerp(endpoints, midWeights[i]), midPieceTL.y - spring->getHeight());
    spring->mover = new BrainMoverHop(spring, Point2(spring->getX(), spring->getX()));
    addAlly(spring);
  }
}

// ==================================== KALevelWalk 3

void KALevelWalk::createBrains()
{
  tutTimer.setActive();
  
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.1, 0.5),
    Point2(0.3, 0.7),
    Point2(0.2, 0.8)
  };
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    spring->setXY(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y - spring->getHeight());
    Point2 flyEnd = Point2(RM::lerp(endpoints, midWeights[i].y), midPieceTL.y - spring->getHeight());
    spring->mover = new BrainMoverWalk(spring, spring->getXY(), flyEnd);
    addAlly(spring);
  }
}

// ==================================== KALevelFly 4

void KALevelFly::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.5, 0.5),
    Point2(0.2, 0.5),
    Point2(0.3, 0.3),
    Point2(0.5, 0.9)
  };
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    if (i % 2 == 1)
    {
      KarateBrain* spring = new KarateBrain();
      spring->setXY(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y - spring->getHeight());
      spring->mover = new BrainMover(spring);
      addAlly(spring);
    }
    else
    {
      KarateBrain* spring = new KarateBrain();
      spring->setXY(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y - 200.0);
      Point2 flyEnd = Point2(RM::lerp(endpoints, midWeights[i].y), midPieceTL.y - 200.0);
      
      BrainMoverWalk* mover = new BrainMoverWalk(spring, spring->getXY(), flyEnd);
      mover->setFlying();
      spring->mover = mover;
      addAlly(spring);
    }
  }
}

// ==================================== KALevelHop 5

void KALevelHop::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.1, 0.4),
    Point2(0.3, 0.5),
    Point2(0.3, 0.7),
    Point2(0.1, 0.4),
    Point2(0.1, 0.9),
  };
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    Point2 springEndpoints(RM::lerp(endpoints, midWeights[i].x), RM::lerp(endpoints, midWeights[i].y));
    spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
    spring->mover = new BrainMoverHop(spring, Point2(springEndpoints.x, springEndpoints.y));
    addAlly(spring);
  }
}

// ==================================== KALevelSpear 6

void KALevelSpear::createBrains()
{
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    Point2 flyStart(RM::lerp(endpoints, 0.25), midPieceTL.y - 200.0);
    Point2 flyEnd(RM::lerp(endpoints, 0.75), midPieceTL.y - spring->getHeight());
    
    if (i % 2 == 1) std::swap(flyStart.y, flyEnd.y);
    
    spring->setXY(flyStart);
    BrainMoverWalk* mover = new BrainMoverWalk(spring, spring->getXY(), flyEnd);
    mover->setFlying();
    spring->mover = mover;
    addAlly(spring);
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.9), midPieceTL.y - 96.0);
    addEnemy(spear);
  }
}

// ==================================== KALevelIllusion 7

void KALevelIllusion::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.1, 0.4),
    Point2(0.3, 0.7),
    Point2(0.4, 0.5),
    Point2(0.3, 0.7),
    Point2(0.2, 0.7),
    Point2(0.4, 0.6),
    Point2(0.45, 0.65)
  };
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    spring->enableIllusions();
    
    Point2 springEndpoints(RM::lerp(endpoints, midWeights[i].x), RM::lerp(endpoints, midWeights[i].y));
    spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
    spring->mover = new BrainMoverHop(spring, Point2(springEndpoints.x, springEndpoints.y));
    addAlly(spring);
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.9), midPieceTL.y - 128.0);
    addEnemy(spear);
  }
  
  tryCreateUnlockable(Point2(4400.0, -1100.0), Coord2(2, 11));
}

// ==================================== KALevelCircle 8

void KALevelCircle::createBrains()
{
  // second coord is the absolute y (relative to the ground)
  Point2 midWeights[] =
  {
    Point2(0.0, -160.0),
    Point2(0.1, -160.0),
    Point2(0.4, -160.0),
    Point2(0.4, -160.0),
    Point2(0.5, -160.0),
    Point2(0.4, -160.0),
    Point2(0.5, -160.0),
    Point2(0.6, -160.0),
    Point2(0.3, -160.0)
  };
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    
    Circle circle(Point2(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y + midWeights[i].y), 96.0);
    spring->setXY(circle.xy);
    BrainMoverCircle* circleMover = new BrainMoverCircle(spring, circle);
    circleMover->currAngle = randGen.randf(0.0, TWO_PI);
    spring->mover = circleMover;
    addAlly(spring);
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.9), midPieceTL.y - 160.0);
    addEnemy(spear);
  }
}

// ==================================== KALevelGravity

void KALevelGravity::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.5, 0.5),
    Point2(0.3, 0.7),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.3, 0.7),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.3, 0.7),
    Point2(0.35, 0.65)
  };
  
  KarateSpear* lastSpear = NULL;
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    Point2 springEndpoints(RM::lerp(endpoints, midWeights[i].x), RM::lerp(endpoints, midWeights[i].y));
    
    if (i % 3 == 1)
    {
      spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
      spring->mover = new BrainMover(spring);
    }
    else if (i % 3 == 2)
    {
      spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
      
      spring->mover = new BrainMoverWalk(spring,
                                         Point2(springEndpoints.x, spring->getY()),
                                         Point2(springEndpoints.y, spring->getY()));
    }
    else if (i % 3 == 0)
    {
      spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
      
      spring->mover = new BrainMoverHop(spring, Point2(springEndpoints.x, springEndpoints.y));
    }
    
    addAlly(spring);
    
    if (i > 1)
    {
      KarateSpear* spear2 = new KarateSpear();
      spear2->setXY(RM::lerp(endpoints, 0.025), midPieceTL.y - 196.0);
      addEnemy(spear2);
      
      lastSpear->neighbor = spear2;
      spear2->neighbor = lastSpear;
    }
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.975), midPieceTL.y - 196.0);
    addEnemy(spear);
    lastSpear = spear;
  }
}

// ==================================== KALevelMix

void KALevelMix::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.35, 0.65),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65)
  };
  
  KarateSpear* lastSpear = NULL;
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    Point2 springEndpoints(RM::lerp(endpoints, midWeights[i].x), RM::lerp(endpoints, midWeights[i].y));
    
    if (i == numBoards)
    {
      spring->enableIllusions(0.25);
      Circle circle(Point2(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y + -160.0), 96.0);
      spring->setXY(circle.xy);
      BrainMoverCircle* circleMover = new BrainMoverCircle(spring, circle);
      circleMover->currAngle = randGen.randf(0.0, TWO_PI);
      spring->mover = circleMover;
    }
    else if (i % 3 == 1)
    {
      spring->setXY(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y - 64.0);
      Point2 flyEnd = Point2(RM::lerp(endpoints, midWeights[i].y), midPieceTL.y - 128.0);
      
      BrainMoverWalk* mover = new BrainMoverWalk(spring, spring->getXY(), flyEnd);
      mover->setFlying();
      spring->mover = mover;
    }
    else if (i % 3 == 2)
    {
      spring->enableIllusions(0.35);
      spring->setXY(springEndpoints.x, midPieceTL.y - spring->getHeight());
      
      spring->mover = new BrainMoverHop(spring, Point2(springEndpoints.x, springEndpoints.y));
    }
    else if (i % 3 == 0)
    {
      Circle circle(Point2(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y + -160.0), 96.0);
      spring->setXY(circle.xy);
      BrainMoverCircle* circleMover = new BrainMoverCircle(spring, circle);
      circleMover->currAngle = randGen.randf(0.0, TWO_PI);
      spring->mover = circleMover;
    }
    
    addAlly(spring);
    
    if (i > 1)
    {
      KarateSpear* spear2 = new KarateSpear();
      spear2->setXY(RM::lerp(endpoints, 0.025), midPieceTL.y - 256.0);
      addEnemy(spear2);
      
      lastSpear->neighbor = spear2;
      spear2->neighbor = lastSpear;
    }
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.975), midPieceTL.y - 256.0);
    addEnemy(spear);
    lastSpear = spear;
  }
}

// ==================================== KALevelMummy

void KALevelMummy::createBrains()
{
  Point2 midWeights[] =
  {
    Point2(0.0, 0.0),
    Point2(0.35, 0.65),
    Point2(0.45, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65),
    Point2(0.35, 0.65),
    Point2(0.5, 0.5),
    Point2(0.35, 0.65)
  };
  
  KarateSpear* lastSpear = NULL;
  
  for (Coord1 i = 0; i < midList.count; ++i)
  {
    if (i == 0) continue;
    
    Point2 midPieceTL = midList[i]->getXY();
    Point2 endpoints(midPieceTL.x, midPieceTL.x + KARATE_MIDDLE_WIDTH);
    
    KarateBrain* spring = new KarateBrain();
    Point2 springEndpoints(RM::lerp(endpoints, midWeights[i].x), RM::lerp(endpoints, midWeights[i].y));
    
    if (i % 2 == 0)
    {
      spring->enableIllusions(0.25);
      Circle circle(Point2(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y + -160.0), 96.0);
      spring->setXY(circle.xy);
      BrainMoverCircle* circleMover = new BrainMoverCircle(spring, circle);
      circleMover->currAngle = randGen.randf(0.0, TWO_PI);
      spring->mover = circleMover;
    }
    else
    {
      spring->setXY(RM::lerp(endpoints, midWeights[i].x), midPieceTL.y - 48.0);
      spring->mover = new BrainMover(spring);
      spring->setMummy();
    }
    
    addAlly(spring);
    
    if (i > 1)
    {
      KarateSpear* spear2 = new KarateSpear();
      spear2->setXY(RM::lerp(endpoints, 0.025), midPieceTL.y - 256.0);
      addEnemy(spear2);
      
      lastSpear->neighbor = spear2;
      spear2->neighbor = lastSpear;
    }
    
    KarateSpear* spear = new KarateSpear();
    spear->setXY(RM::lerp(endpoints, 0.975), midPieceTL.y - 256.0);
    addEnemy(spear);
    lastSpear = spear;
  }
}

// ==================================== SlopeLevel ================================ //

SpikeLevel::SpikeLevel() :
PizzaLevel(),
numChunks(10)
{
  spikedTerrain = true;
  musicFilename = "balance.ima4";
}

void SpikeLevel::preload()
{
  background = createBackgroundStd(Pizza::currWorldPair.x);
}

// overrides
void SpikeLevel::loadRes()
{
  background->load();
  
  imgCache[balanceBallCircle] = Image::create_centered(imgStr("balance_skull"));
  imgCache[balanceBallOct] = Image::create_centered(imgStr("balance_skull_oct"));
  imgCache[balancePiece2x2] = Image::create_handled_abs(imgStr("balance_block_2x2"), Point2(0.0, 64.0));
  imgCache[balancePieceBounceCircle] = Image::create_centered(imgStr("balance_bounce_circle"));
  imgCache[balancePieceBouncePlatform] = Image::create_handled_abs(imgStr("balance_bounce"), Point2(0.0, 64.0));
  imgCache[balancePieceDownhill4_1] = Image::create_handled_abs(imgStr("balance_downhill_4_1"), Point2(0.0, 48.0));
  imgCache[balancePieceDownhill4_3] = Image::create_handled_abs(imgStr("balance_downhill_4_3"), Point2(0.0, 48.0));
  imgCache[balancePieceDownhill8_1] = Image::create_handled_abs(imgStr("balance_downhill_8_1"), Point2(0.0, 64.0));
  imgCache[balancePieceDownhill12_4] = Image::create_handled_abs(imgStr("balance_downhill_12_4"), Point2(0.0, 64.0));
  imgCache[balancePieceDownhill16_4] = Image::create_handled_abs(imgStr("balance_downhill_16_4"), Point2(0.0, 64.0));
  imgCache[balancePieceFlatlong] = Image::create_handled_abs(imgStr("balance_flat_long"), Point2(0.0, 64.0));
  imgCache[balancePieceFlatmed] = Image::create_handled_abs(imgStr("balance_flat_med"), Point2(0.0, 64.0));
  imgCache[balancePieceNospike] = Image::create_handled_abs(imgStr("balance_flat_nospike"), Point2(0.0, 64.0));
  imgCache[balancePieceShort] = Image::create_handled_abs(imgStr("balance_flat_short"), Point2(0.0, 64.0));
  imgCache[balancePieceRiser] = Image::create_handled_abs(imgStr("balance_riser"), Point2(0.0, 64.0));
  imgCache[balancePieceRiseBlock] = Image::create_handled_abs(imgStr("balance_rise_block_2x2"), Point2(0.0, 64.0));
  imgCache[balancePiecePillar] = Image::create_handled_abs(imgStr("balance_spike_pillar"), Point2(32.0, 32.0));
  imgCache[balancePieceUphill4_1] = Image::create_handled_abs(imgStr("balance_uphill_4_1"), Point2(0.0, 64.0));
  imgCache[balancePieceUphill4_3] = Image::create_handled_abs(imgStr("balance_uphill_4_3"), Point2(0.0, 96.0));
  imgCache[balancePieceUphill8_1] = Image::create_handled_abs(imgStr("balance_uphill_8_1"), Point2(0.0, 64.0));
  imgCache[balancePieceUphill8_3] = Image::create_handled_abs(imgStr("balance_uphill_8_3"), Point2(0.0, 80.0));
  imgCache[balancePieceUphill12_2] = Image::create_handled_abs(imgStr("balance_uphill_12_2"), Point2(0.0, 80.0));
  imgCache[balancePieceUphill16_4] = Image::create_handled_abs(imgStr("balance_uphill_16_4"), Point2(0.0, 128.0));
  imgCache[spikeball] = Image::create_centered(imgStr("spikeball"));
  
  loadImgAbs(balanceGongStand, "balance_gong_stand");
  loadTilesAbs(balanceGongSet, "balance_gong", 3, 1);
  
  Image::create_tiles(imgStr("meat"), imgsetCache[junkSet], 3, 1);
  imgsetCache[junkSet][0]->handle.set(23.0, 38.0);
  imgsetCache[junkSet][1]->handle.set(24.0, 32.0);
  imgsetCache[junkSet][2]->handle.set(38.0, 35.0);
  
  loadSound(gongSound, "gong.wav");
}

void SpikeLevel::unloadRes()
{
  
}

void SpikeLevel::loadMe()
{
  background->foregroundDisabled = true;
  setInitialWeights();
  
  createChunk(CHUNK_SPIKE_START);
  
  for (Coord1 i = 0; i < numChunks; ++i)
  {
    // createChunk(CHUNK_SPIKE_BOUNCESTEPS);
    // createChunk(CHUNK_SPIKE_BOUNCECIRCLES);
    generateNextChunk(i);
  }
  
  createChunk(CHUNK_SPIKE_END);
  
  worldBox.wh.x = endpoint.x;
  invisibleWallsLR(Point2(0.0, 288.0), endpoint + Point2(-256.0, 0.0));
  
  startStanding(Point2(224.0, 288.0));
  
  Point2 cameraTarget(0.0, 0.0);
  Point1 minY = minYZoom(player->collisionBox().xy.y + 64.0).x;
  camera.zoom.x = RM::flatten(camera.zoom.x, minY, 1.0 * RM::timePassed());
  camera.zoom.y = camera.zoom.x;
  
  camera.xy.x = RM::lerp(player->getX(), ballList[0]->getX(), 0.5) - PLAY_CX;
  camera.xy.y = ballList[0]->getY() + ballList[0]->getHeight() + 64.0 - PLAY_H;
}

void SpikeLevel::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_FLAT); break;
    default: createRandomChunk(); break;
  }
}

void SpikeLevel::updateCamera()
{
  if (getCurrBall() == NULL)
  {
    updateCameraStart();
  }
  else
  {
    updateCameraStd();
  }
}

void SpikeLevel::updateCameraStart()
{
  camera.handle.set(128.0, SCREEN_H + Pizza::platformTL.y);
  
  Point2 cameraTarget(0.0, 0.0);
  
  if (getCurrBall() == NULL && ballList[0]->onScreen() == false)
  {
    cameraTarget.x = player->getX();
  }
  else if (getCurrBall() == NULL && ballList[0]->onScreen() == true)
  {
    cameraTarget.x = RM::lerp(player->getX(), ballList[0]->getX(), 0.5) - PLAY_CX;
    cameraTarget.y = ballList[0]->getY() + ballList[0]->getHeight() + 64.0 - PLAY_H;
  }
  
  Point1 minY = minYZoom(player->collisionBox().xy.y + 64.0).x;
  camera.zoom.x = RM::flatten(camera.zoom.x, minY, 1.0 * RM::timePassed());
  camera.zoom.y = camera.zoom.x;
  
  camera.xy.x = RM::flatten(camera.xy.x, cameraTarget.x, 400.0 * RM::timePassed());
  camera.xy.y = RM::flatten(camera.xy.y, cameraTarget.y, 400.0 * RM::timePassed());
}

void SpikeLevel::updateCameraStd()
{
  camera.handle.set(128.0, SCREEN_H + Pizza::platformTL.y);
  
  Point2 cameraTarget(0.0, 0.0);
  cameraTarget.x = getCurrBall()->getX() - 128.0;
  cameraTarget.y = getCurrBall()->getY() + getCurrBall()->getHeight() + 64.0 - PLAY_H;
  
  camera.xy.x = RM::flatten(camera.xy.x, cameraTarget.x, 400.0 * RM::timePassed());
  camera.xy.y = RM::flatten(camera.xy.y, cameraTarget.y, 400.0 * RM::timePassed());
  
  Point1 minY = minYZoom(player->collisionBox().xy.y + 64.0).x;
  camera.zoom.x = RM::flatten(camera.zoom.x, minY, 1.0 * RM::timePassed());
  camera.zoom.y = camera.zoom.x;
}

void SpikeLevel::updateLevelAI()
{
  if (levelState == LEVEL_PLAY && getCurrBall() != NULL) getCurrBall()->tryRoll(1.0);
}

void SpikeLevel::updateStatus()
{
  Point1 loseEdge = camera.myBox().left() + Pizza::platformTL.x / camera.zoom.x;
  
  if (player->collisionCircle().right() < loseEdge)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void SpikeLevel::balanceBallTouched(BalanceSkull* ball)
{
  // this won't let you go back to the old ball accidentally
  currBallID = std::max(currBallID, ball->value);
}

void SpikeLevel::gongHit()
{
  BoneBreakEffect* breaker = new BoneBreakEffect(*getCurrBall(), 4, 4);
  breaker->setVelFromPt(Point2(getCurrBall()->collisionCircle().right(), getCurrBall()->getY()), 300.0);
  addDebris(breaker);
  
  addAction(new Smokeburst(getCurrBall()->getXY(), false));
  
  getCurrBall()->body->SetActive(false);
  getCurrBall()->setAlpha(0.0);
  
  winLevel();
}

Coord1 SpikeLevel::calcWinBonus()
{
  return 750 + 250 * levelID;
}

void SpikeLevel::setInitialWeights()
{
  chunkWeights[CHUNK_SPIKE_FLAT] = 1.0;
}

void SpikeLevel::callback(ActionEvent* caller)
{
  if (caller == &junkTimer)
  {
    Point1 randX = RM::randf(player->collisionBox().corner(1).x, camera.myBox().corner(1).x);
    Point2 createLocation = Point2(randX, camera.myBox().top() - 64.0);
    createMeat(createLocation);
  }
}

Coord1 SlopeLevel::calcWinBonus()
{
  return levelID * 125;
}

// =============================== BALevelA

void BALevelA::setInitialWeights()
{
  numChunks = 13;
}

void BALevelA::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_FLAT); break;
    case 1: createChunk(CHUNK_SPIKE_FLAT); break;
    case 2: createChunk(CHUNK_SPIKE_FLAT); break;
    case 3: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 4: createChunk(CHUNK_SPIKE_FLAT); break;
    case 5: createChunk(CHUNK_SPIKE_BUMP); break;
    case 6: createChunk(CHUNK_SPIKE_FLAT); break;
    case 7: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 8: createChunk(CHUNK_SPIKE_DROP); break;
    case 9: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 10: createChunk(CHUNK_SPIKE_BUMP); break;
    case 11: createChunk(CHUNK_SPIKE_DROP); break;
    case 12: createChunk(CHUNK_SPIKE_DROP); break;
    default: createRandomChunk(); break;
  }
}

// =============================== BALevelB

void BALevelB::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_FLAT); break;
    case 1: createChunk(CHUNK_SPIKE_BUMP); break;
    case 2: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 3: createChunk(CHUNK_SPIKE_ZIGZAG); break;
    case 4: createChunk(CHUNK_SPIKE_RISER); break;
    case 5: createChunk(CHUNK_SPIKE_BUMP); break;
    case 6: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 7: createChunk(CHUNK_SPIKE_ZIGZAG); break;
    case 8: createChunk(CHUNK_SPIKE_RISER); break;
    case 9: createChunk(CHUNK_SPIKE_RISER); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelC

BALevelC::BALevelC() :
SpikeLevel()
{
  useOctBall = true;
}

void BALevelC::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 1: createChunk(CHUNK_SPIKE_DOWNHILLBUMP); break;
    case 2: createChunk(CHUNK_SPIKE_PILLAR); break;
    case 3: createChunk(CHUNK_SPIKE_SPRING); break;
    case 4: createChunk(CHUNK_SPIKE_PILLAR); break;
    case 5: createChunk(CHUNK_SPIKE_BUMP); break;
    case 6: createChunk(CHUNK_SPIKE_DOWNHILLBUMP); break;
    case 7: createChunk(CHUNK_SPIKE_SPRING); break;
    case 8: createChunk(CHUNK_SPIKE_PILLAR); break;
    case 9: createChunk(CHUNK_SPIKE_SPRING); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelD

void BALevelD::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_UPHILL); break;
    case 1: createChunk(CHUNK_SPIKE_NOSPIKE); break;
    case 2: createChunk(CHUNK_SPIKE_LONGFALL); break;
    case 3: createChunk(CHUNK_SPIKE_DUALRISE); break;
    case 4: createChunk(CHUNK_SPIKE_UPHILL); break;
    case 5: createChunk(CHUNK_SPIKE_ZIGZAG); break;
    case 6: createChunk(CHUNK_SPIKE_NOSPIKE); break;
    case 7: createChunk(CHUNK_SPIKE_DUALRISE); break;
    case 8: createChunk(CHUNK_SPIKE_LONGFALL); break;
    case 9: createChunk(CHUNK_SPIKE_LONGFALL); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelE

void BALevelE::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_DOWNHILLBUMP); break;
    case 1: createChunk(CHUNK_SPIKE_STRAIGHTBOUNCE); break;
    case 2: createChunk(CHUNK_SPIKE_LONGFALL); break;
    case 3: createChunk(CHUNK_SPIKE_BALLJUMP); break;
    case 4: createChunk(CHUNK_SPIKE_TRANSFERDROP); break;
    case 5: createChunk(CHUNK_SPIKE_ZIGZAG); break;
    case 6: createChunk(CHUNK_SPIKE_STRAIGHTBOUNCE, 1); break;
    case 7: createChunk(CHUNK_SPIKE_TRANSFERDROP); break;
    case 8: createChunk(CHUNK_SPIKE_BUMP); break;
    case 9: createChunk(CHUNK_SPIKE_BALLJUMP); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelF

BALevelF::BALevelF() :
SpikeLevel()
{
  useOctBall = true;
}

void BALevelF::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_DROP); break;
    case 1: createChunk(CHUNK_SPIKE_BOUNCECIRCLES); break;
    case 2: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 3: createChunk(CHUNK_SPIKE_BUMP); break;
    case 4: createChunk(CHUNK_SPIKE_TRANSFERBLOCK); break;
    case 5: createChunk(CHUNK_SPIKE_NOSPIKE); break;
    case 6: createChunk(CHUNK_SPIKE_BOUNCECIRCLES); break;
    case 7: createChunk(CHUNK_SPIKE_TRANSFERBLOCK); break;
    case 8: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 9: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelG

void BALevelG::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_PILLAR); break;
    case 1: createChunk(CHUNK_SPIKE_BALLDOWNHILL); break;
    case 2: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 3: createChunk(CHUNK_SPIKE_SPRING); break;
    case 4: createChunk(CHUNK_SPIKE_TRANSFERSPIKE); break;
    case 5: createChunk(CHUNK_SPIKE_NOSPIKE); break;
    case 6: createChunk(CHUNK_SPIKE_BALLDOWNHILL); break;
    case 7: createChunk(CHUNK_SPIKE_TRANSFERSPIKE); break;
    case 8: createChunk(CHUNK_SPIKE_DOWNHILL); break;
    case 9: createChunk(CHUNK_SPIKE_BALLJUMP); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelH

void BALevelH::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      createChunk(CHUNK_SPIKE_BUMP);
      RMPhysics->set_gravity_rm(RMPhysics->get_gravity_rm() * Point2(1.0, 0.65));
      break;
    case 1: createChunk(CHUNK_SPIKE_SPIKEBALL); break;
    case 2: createChunk(CHUNK_SPIKE_BOUNCEDROPS); break;
    case 3: createChunk(CHUNK_SPIKE_TRANSFERBLOCK); break;
    case 4: createChunk(CHUNK_SPIKE_RISEBALLS); break;
    case 5: createChunk(CHUNK_SPIKE_BOUNCEDROPS); break;
    case 6: createChunk(CHUNK_SPIKE_SPIKEBALL); break;
    case 7: createChunk(CHUNK_SPIKE_RISEBALLS); break;
    case 8: createChunk(CHUNK_SPIKE_ZIGZAG); break;
    case 9: createChunk(CHUNK_SPIKE_RISEBALLS); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// =============================== BALevelI

BALevelI::BALevelI() :
SpikeLevel()
{
  useOctBall = true;
  numChunks = 13;
}

void BALevelI::generateNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0: createChunk(CHUNK_SPIKE_BOUNCECIRCLES); break;
    case 1: createChunk(CHUNK_SPIKE_LONGFALL); break;
    case 2: createChunk(CHUNK_SPIKE_NOSPIKE); break;
    case 3: createChunk(CHUNK_SPIKE_TRANSFERBLOCK); break;
    case 4: createChunk(CHUNK_SPIKE_SPIKEBALL); break;
    case 5: createChunk(CHUNK_SPIKE_BOUNCEDROPS); break;
    case 6: createChunk(CHUNK_SPIKE_TRANSFERSPIKE); break;
    case 7: createChunk(CHUNK_SPIKE_BALLJUMP); break;
    case 8: createChunk(CHUNK_SPIKE_SPIKEBALL); break;
    case 9: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 10: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 11: createChunk(CHUNK_SPIKE_RISESTEPS); break;
    case 12: createChunk(CHUNK_SPIKE_SPIKEBALL); break;
    default: createRandomChunk(); break;  // shouldn't get called
  }
}

// ==================================== BounceLevel ================================ //

const Point1 BounceLevel::BOUNCE_MAX_VEL = 800.0;

BounceLevel::BounceLevel() :
PizzaLevel(),
sun(NULL),
arrowTimer(0.5, this),
turnTimer(0.5),
enemyGoal(75),
enemyNum(0),
arrowCount(0),
warnedOnce(false),

skullBarFrame(Point2(0.0, 0.0) + Pizza::platformTL),
skullBarFill(Point2(49.0, 16.0) + Pizza::platformTL),
skullBarWeight(0.0),
arrowYOff(-Pizza::platformTL.y)
{
  bounceTerrain = true;
  musicFilename = "bounce.ima4";
  turnTimer.setInactive();
}

void BounceLevel::preload()
{
  background = new BounceBG(this);
}

// overrides
void BounceLevel::loadRes()
{
  background->load();
  
  Image::create_tiles_abs(imgStr("bounce_platforms"), imgsetCache[bounceSegmentSet], 3, 1, Point2(72.0, 16.0));
  imgCache[warningH] = Image::create_handled_norm(imgStr("warning_h"), HANDLE_RC);
  Image::create_tiles(imgStr("explosion"), imgsetCache[bombExplodeSet], 3, 1, true, true);
  
  Image::create_tiles_abs(imgStr("comet"), imgsetCache[bounceCometSet], 1, 4, Point2(56.0, 16.0), true);
  loadImgAbs(bounceGiantComet, "giant_comet", Point2(176.0, 96.0), true);
  imgCache[bounceWormhole] = Image::create_centered(imgStr("wormhole"), true);
  
  for (Char skullID = 'a'; skullID <= 'e'; ++skullID)
  {
    imgsetCache[bounceSkullSet].add(
                                    Image::create_centered(imgStr(String1("bounce_skull_$", '$', skullID).as_cstr())));
    bounceSkullWarps.add(
                         new DWarpSequence(String1("bounceskull$.dwarp2", '$', skullID).as_cstr(), true));
  }
  
  imgCache[bounceSkullHeavy] = Image::create_centered(imgStr("bounce_skull_heavy"));
  
  imgCache[bounceSkullBig] = Image::create_centered(imgStr("bounce_skull_big"));
  dwarpCache[bounceBigWarp] = new DWarpSequence("bounceskullbig.dwarp2", true);
  
  Image::create_tiles_abs(imgStr("bounce_skull_bomb"), imgsetCache[bounceBombSet], 2, 2, Point2(40.0, 64.0));
  
  imgsetCache[bounceLightningSet].add(Image::create_handled_norm(imgStr("bounce_lightning_a"),
                                                                 Point2(0.0, 0.5), true));
  imgsetCache[bounceLightningSet].last()->repeats();
  imgsetCache[bounceLightningSet].add(Image::create_handled_norm(imgStr("bounce_lightning_b"),
                                                                 Point2(0.0, 0.5), true));
  imgsetCache[bounceLightningSet].last()->repeats();
  
  Image::create_tiles_abs(imgStr("bounce_platform_squish"), imgsetCache[bounceSquishSet], 1, 2, Point2(72.0, 16.0));
  
  imgCache[madsun] = Image::create_handled_abs(imgStr("sun_skull"), Point2(64.0, 64.0));
  imgCache[madsunGlow] = Image::create_handled_abs(imgStr("sun_skull_glow"), Point2(64.0, 64.0), true);
  imgCache[madsunFlame] = Image::create_centered(imgStr("flame"), true);
  dwarpCache[sunWarp] = new DWarpSequence("sunskull.dwarp2", true);
  
  imgCache[skullBarFrameImg] = Image::create_handled_norm(imgStr("bounce_bar_base"), Point2(0.0, 1.0));
  imgCache[skullBarFillImg] = new Image(imgStr("bounce_bar_fill"));
  
  imgCache[caltropImg] = Image::create_handled_abs(imgStr("mole_spikeball"), Point2(20.0, 18.0));
  
  loadSound(cometSound, "comet.wav");
}

void BounceLevel::unloadRes()
{
  free_clear(bounceSkullWarps);
}

void BounceLevel::loadMe()
{
  skullBarFrame.setXY(Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y));
  skullBarFrame.setImage(imgCache[skullBarFrameImg]);
  skullBarFill.setXY(skullBarFrame.getXY() + Point2(49.0, -32.0));
  skullBarFill.setImage(imgCache[skullBarFillImg]);
  skullBarFill.setH(skullBarFill.getImage()->natural_size().y);
  
  player->setY(-PLAY_H * 1.0);
  player->updateFromWorld();
  
  createWorld();
  
  // establish world size
  PizzaGO* lastSeg = objman.bgTerrainList.last();
  worldBox = Box(0.0, 0.0, lastSeg->getX() + lastSeg->getWidth() * 0.5, SCREEN_H);
  worldBox.grow_up(SCREEN_H * 3.0);
  
  addTextEffectStd(String2(LSTRINGS[100], '$', enemyGoal), PLAY_CENTER);
  
  updateCamera();
}

void BounceLevel::updateCamera()
{
  camera.handle.set(PLAY_CX, SCREEN_H + Pizza::platformTL.y);
  
  updateZoomStd(-64.0);
  camera.xy = Point2(player->getX() - PLAY_CX, 0.0);
  if (sun != NULL) sun->updatePostCamera();
}

void BounceLevel::updateStatus()
{
  if (player->collisionCircle().top() > PLAY_H)
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void BounceLevel::updateLevelAI()
{
  // enforce max -y vel
  Point2 playerVel = player->get_velocity();
  if (player->get_velocity().y < -BOUNCE_MAX_VEL)
  {
    player->set_velocity(Point2(playerVel.x, -BOUNCE_MAX_VEL));
  }
  // enforce max +y vel
  else if (player->get_velocity().y > BOUNCE_MAX_VEL)
  {
    player->set_velocity(Point2(playerVel.x, BOUNCE_MAX_VEL));
  }
  
  RM::flatten_me(skullBarWeight, (Point1) Pizza::currGameVars[VAR_DEFEATED] / enemyGoal,
                 1.0 * RM::timePassed());
  
  /////////////////// The playing line
  if (levelState != LEVEL_PLAY) return;
  
  Coord1 tgtFacing = tiltPercent < 0.0 ? -1 : 1;
  // if (player->facingToDir() == tgtFacing) turnTimer.setInactive();
  
  turnTimer.update();
  if (turnTimer.getActive() == false)
  {
    player->setFacing(tgtFacing);
    turnTimer.reset();
  }
  
  enemyMaker();
  arrowTimer.update();
}

void BounceLevel::drawHUD()
{
  Point1 fillWidth = RM::clamp(skullBarWeight, 0.0, 1.0) * skullBarFill.getImage()->natural_size().x;
  skullBarFrame.drawMe();
  skullBarFill.setW(fillWidth);
  skullBarFill.drawMeRepeating();
}

void BounceLevel::enemyMaker()
{
  if (objman.enemyList.count < 5)
  {
    addEnemy(createNextEnemy());
    ++enemyNum;
  }
}

void BounceLevel::enemyDefeated(PizzaGO* enemy)
{
  if (levelState != LEVEL_PLAY) return;
  
  Pizza::currGameVars[VAR_DEFEATED]++;
  gotScore(enemy->basePoints);
  
  if (RM::randf() < 0.5) createNextCoin();
  
  // textDrawer.getTextPtr()->int_string(Pizza::currGameVars[VAR_DEFEATED]);
  // textDrawer.textChanged();
  
  if (Pizza::currGameVars[VAR_DEFEATED] >= enemyGoal)
  {
    if (sun != NULL) sun->puppyDone(player->getXY(), 0.5);
    winLevel();
  }
}

PizzaGO* BounceLevel::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  Coord1 segID = (4 + enemyNum * 2) % objman.bgTerrainList.count;
  Point2 frogCenter(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  frog->setXY(frogCenter);
  
  return frog;
}

PhysicalCoin* BounceLevel::createNextCoin()
{
  Coord1 segID = RM::randi(0, objman.bgTerrainList.count - 1);
  Point2 coinCenter(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  Point1 dir = RM::randl() ? 1 : -1;
  if (true == true)
  {
    coinCenter.x += 64.0 * dir;
  }
  else
  {
    dir = 0;
  }
  
  Coord1 maxCoinType = std::min(levelID, (Coord1) PizzaGO::QUARTER);
  Coord1 coinType = SlopeCoin::randValueAll(maxCoinType);
  
  PhysicalCoin* coin = new PhysicalCoin(coinCenter, coinType);
  addAlly(coin);
  
  if (dir != 0)
  {
    coin->set_velocity(Point2(16.0 * -dir, 0.0));
  }
  
  return coin;
}

BounceComet* BounceLevel::createBounceComet(Logical startLeft, Point1 timeToNext)
{
  return createBounceComet(startLeft, 96.0 + arrowYOff, timeToNext);
}

BounceComet* BounceLevel::createBounceEnergy(Logical startLeft, Point1 timeToNext)
{
  return createBounceComet(startLeft, 96.0 + arrowYOff, timeToNext, true);
}

BounceComet* BounceLevel::createBounceComet(Logical startLeft, Point1 yVal, Point1 timeToNext,
                                            Logical energyBall)
{
  Point1 xVal = startLeft ? worldBox.left() - PLAY_W : worldBox.right() + PLAY_W;
  
  BounceComet* comet = NULL;
  
  if (energyBall == false)
  {
    comet = new BounceComet(Point2(xVal, yVal),
                            Point2(200.0 * (startLeft ? 1.0 : -1.0), 0.0));
  }
  else
  {
    comet = new BounceEnergyBall(Point2(xVal, yVal),
                                 Point2(150.0 * (startLeft ? 1.0 : -1.0), 0.0));
  }
  
  if (warnedOnce == false)
  {
    warnedOnce = true;
    comet->warning = true;
  }
  
  ResourceManager::playBounceComet();
  
  Wormhole* hole = new Wormhole(timeToNext);
  hole->setXY(comet->getXY());
  addAlly(hole);
  
  addAlly(comet);
  return comet;
}

Coord1 BounceLevel::calcWinBonus()
{
  return 1000;
}

void BounceLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== BOLevelEasy

void BOLevelEasy::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    BounceSegment2* segment = new BounceSegment2(Point2(72.0 + 144.0 * i, baseY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

// ==================================== BOLevelArrow

void BOLevelArrow::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    BounceSegment2* segment = new BounceSegment2(Point2(72.0 + 144.0 * i, baseY + 32.0 * (i % 2)));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

void BOLevelArrow::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    createBounceComet(false, 5.0);
    arrowTimer.reset(5.0);
  }
}

// ==================================== BOLevelHeavy

void BOLevelHeavy::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    Point1 segX = 72.0 + 144.0 * 1.75 * i;
    
    BounceSegment2* segment = new BounceSegment2(Point2(segX, baseY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelHeavy::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  if (randGen.randi(0, 3) == 0) frog->heavy = true;
  
  return frog;
}

void BOLevelHeavy::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    createBounceComet(true, 3.0);
    arrowTimer.reset(3.0);
  }
}

// ==================================== BOLevelSquish

void BOLevelSquish::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 10; ++i)
  {
    BounceSegment2* segment = new BounceSegment2(Point2(72.0 + 144.0 * i, baseY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    segment->squishes = true;
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelSquish::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  if (randGen.randi(0, 7) == 0) frog->heavy = true;
  
  return frog;
}

void BOLevelSquish::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    createBounceComet(false, 4.0);
    arrowTimer.reset(4.0);
  }
}

// ==================================== BOLevelBig

void BOLevelBig::createWorld()
{
  Coord1 ySteps[] = {0, -1, -2, -3, -2, -1, 0};
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < RM::count(ySteps); ++i)
  {
    Point1 segX = 72.0 + 144.0 * 1.75 * i;
    Point1 segY = baseY + 32.0 * ySteps[i];
    
    BounceSegment2* segment = new BounceSegment2(Point2(segX, segY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelBig::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  if (randGen.randi(0, 3) == 0) frog->big = true;
  
  return frog;
}

void BOLevelBig::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    createBounceComet(true, 64.0 + arrowYOff, 3.75);
    
    if (arrowCount == 0) arrowTimer.reset(0.25);
    else if (arrowCount == 1) arrowTimer.reset(3.5);
    
    arrowCount++;
    arrowCount %= 2;
  }
}

// ==================================== BOLevelLightning

void BOLevelLightning::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  BounceSegment2* segment = NULL;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    segment = new BounceSegment2(Point2(72.0 + 144.0 * i * 1.5,
                                        baseY + 32.0 * (i % 2)));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
  
  tryCreateUnlockable(segment->getXY() + Point2(512.0, -380.0), Coord2(1, 10));
}

PizzaGO* BOLevelLightning::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  Coord1 randFrog = randGen.randi(0, 15);
  if (randFrog < 2) frog->heavy = true;
  else if (randFrog == 2) frog->big = true;
  
  return frog;
}

void BOLevelLightning::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    BounceShock* shock = new BounceShock();
    shock->setY(96.0 + arrowYOff);
    addAlly(shock);
    
    arrowTimer.reset(3.0);
  }
}

// ==================================== BOLevelExplode

void BOLevelExplode::createWorld()
{
  Coord1 ySteps[] = {0, -3, -2, -1, 0, -3, 0};
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < RM::count(ySteps); ++i)
  {
    Point1 segX = 72.0 + 144.0 * 1.75 * i;
    Point1 segY = baseY + 32.0 * ySteps[i];
    
    BounceSegment2* segment = new BounceSegment2(Point2(segX, segY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelExplode::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  Coord1 randFrog = randGen.randi(0, 31);
  if (randFrog < 2) frog->heavy = true;
  else if (randFrog < 8) frog->explodes = true;
  
  return frog;
}

void BOLevelExplode::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    if (arrowCount == 0)
    {
      createBounceComet(false, 80.0 + arrowYOff, 3.0);
      arrowTimer.reset(0.25);
    }
    else if (arrowCount == 1)
    {
      createBounceComet(false, 80.0 - 32.0 + arrowYOff, 3.0);
      arrowTimer.reset(0.25);
    }
    else if (arrowCount == 2)
    {
      createBounceComet(false, 80.0 - 64.0 + arrowYOff, 3.0);
      arrowTimer.reset(2.5);
    }
    
    arrowCount++;
    arrowCount %= 3;
  }
}

// ==================================== BOLevelSun

void BOLevelSun::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    BounceSegment2* segment = new BounceSegment2(Point2(72.0 + 144.0 * i * 2.0,
                                                        baseY + 32.0 * (i % 2)));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    segment->squishes = true;
    addBGTerrain(segment);
  }
  
  sun = new AngrySun();
  sun->hflip(true);
  sun->targetY = PLAY_H - 176.0;
  sun->swapsSides = true;
  addAlly(sun);
}

PizzaGO* BOLevelSun::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = enemyNum % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  Coord1 randFrog = randGen.randi(0, 15);
  if (randFrog == 0) frog->heavy = true;
  else if (randFrog == 1) frog->big = true;
  else if (randFrog <= 8) frog->explodes = true;
  
  return frog;
}

void BOLevelSun::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    
  }
}

// ==================================== BOLevelStreaker

void BOLevelStreaker::createWorld()
{
  Coord1 ySteps[] = {0, -1, 0, -1, 0, -1, 0};
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < RM::count(ySteps); ++i)
  {
    Point1 segX = 72.0 + 144.0 * 1.75 * i;
    Point1 segY = baseY + 32.0 * ySteps[i];
    
    BounceSegment2* segment = new BounceSegment2(Point2(segX, segY));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelStreaker::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  Coord1 randFrog = randGen.randi(0, 2);
  if (randFrog == 1) frog->heavy = true;
  else if (randFrog == 2) frog->explodes = true;
  
  return frog;
}

void BOLevelStreaker::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    BounceComet* comet = NULL;
    
    if (arrowCount == 0)
    {
      comet = createBounceComet(true, 80.0 + arrowYOff, 3.0);
      arrowTimer.reset(0.25);
    }
    else if (arrowCount == 1)
    {
      comet = createBounceComet(true, 80.0 - 32.0 + arrowYOff, 3.0);
      arrowTimer.reset(0.25);
    }
    else
    {
      comet = createBounceComet(true, 80.0 + arrowYOff, 3.0);
      arrowTimer.reset(2.0);
    }
    
    comet->streaker = true;
    
    arrowCount++;
    arrowCount %= 3;
  }
}

// ==================================== BOLevelEnergy

void BOLevelEnergy::createWorld()
{
  Point1 baseY = PLAY_H - 176.0 - Pizza::platformTL.y;
  
  for (Coord1 i = 0; i < 7; ++i)
  {
    BounceSegment2* segment = new BounceSegment2(Point2(72.0 + 144.0 * i, baseY + 32.0 * (i % 2)));
    segment->setImage(imgsetCache[bounceSegmentSet][i % imgsetCache[bounceSegmentSet].count]);
    addBGTerrain(segment);
  }
}

PizzaGO* BOLevelEnergy::createNextEnemy()
{
  BounceFrog* frog = new BounceFrog();
  
  Coord1 segID = (2 + enemyNum) % objman.bgTerrainList.count;
  frog->setXY(objman.bgTerrainList[segID]->getX(), camera.myBox().top() - 128.0);
  
  return frog;
}

void BOLevelEnergy::callback(ActionEvent* caller)
{
  if (caller == &arrowTimer)
  {
    createBounceComet(true, 80.0 + arrowYOff, 4.0, true);
    arrowTimer.reset(4.0);
    
    // arrowCount++;
    // arrowCount %= 3;
  }
}

// ==================================== PuppyLevel ================================ //

PuppyLevel::PuppyLevel() :
  PizzaLevel(),

  objectiveDir(1.0),

  crateTimer(4.0, this),
  cratesMade(0),

  arrowTimer(4.0, true, this),
  bombTimer(4.0, true, this),

  billyState(WAIT_FOR_RETURN),
  billyJumpTimer(0.75, this),
  currJumpingBilly(0),

  sun(NULL)
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
  background = new PuppyBG(this);
}

// overrides
void PuppyLevel::loadRes()
{
  background->load();
  
  imgCache[puppyDoghouseBack] = Image::create_handled_abs(imgStr("puppy_doghouse_back"), Point2(80.0, 32.0));
  imgCache[puppyDoghouseFront] = Image::create_handled_abs(imgStr("puppy_doghouse_front"), Point2(32.0, 96.0));
  imgCache[puppyBounce16] = new Image(imgStr("puppy_bounce_16"));
  imgCache[puppyCornerL] = Image::create_handled_abs(imgStr("puppy_corner_l"), Point2(32.0, 32.0));
  imgCache[puppyCornerR] = Image::create_handled_abs(imgStr("puppy_corner_r"), Point2(0.0, 32.0));
  imgCache[puppyDownhill4_2] = Image::create_handled_abs(imgStr("puppy_downhill_4_2"), Point2(0.0, 64.0));
  imgCache[puppyDownhill4_4] = Image::create_handled_abs(imgStr("puppy_downhill_4_4"), Point2(0.0, 64.0));
  imgCache[puppyDownhill8_2] = Image::create_handled_abs(imgStr("puppy_downhill_8_2"), Point2(0.0, 64.0));
  imgCache[puppyDownhill12_8] = Image::create_handled_abs(imgStr("puppy_downhill_12_8"), Point2(0.0, 64.0));
  imgCache[puppyEdgeL] = Image::create_handled_abs(imgStr("puppy_edge_left"), Point2(32.0, 0.0));
  imgCache[puppyEdgeR] = Image::create_handled_abs(imgStr("puppy_edge_right"), Point2(0.0, 0.0));
  imgCache[puppyEdgeTL] = Image::create_handled_abs(imgStr("puppy_edge_topleft"), Point2(32.0, 64.0));
  imgCache[puppyEdgeTR] = Image::create_handled_abs(imgStr("puppy_edge_topright"), Point2(0.0, 64.0));
  imgCache[puppyFlat8] = Image::create_handled_abs(imgStr("puppy_flat_8"), Point2(0.0, 64.0));
  imgCache[puppyFlat16] = Image::create_handled_abs(imgStr("puppy_flat_16"), Point2(0.0, 64.0));
  imgCache[puppySpikes256] = Image::create_handled_abs(imgStr("puppy_spikes_256"), Point2(0.0, 32.0));
  imgCache[puppyPivotSquare] = Image::create_handled_abs(imgStr("puppy_swing_square"), Point2(64.0, 64.0));
  imgCache[puppyPivotTri] = Image::create_handled_abs(imgStr("puppy_swing_triangle"), Point2(160.0, 160.0));
  imgCache[puppyUphill4_2] = Image::create_handled_abs(imgStr("puppy_uphill_4_2"), Point2(0.0, 96.0));
  imgCache[puppyUphill4_4] = Image::create_handled_abs(imgStr("puppy_uphill_4_4"), Point2(0.0, 128.0));
  imgCache[puppyUphill8_2] = Image::create_handled_abs(imgStr("puppy_uphill_8_2"), Point2(0.0, 96.0));
  imgCache[puppyUphill12_8] = Image::create_handled_abs(imgStr("puppy_uphill_12_8"), Point2(0.0, 192.0));
  imgCache[puppyWall] = new Image(imgStr("puppy_wall"));
  
  Image::create_tiles_norm(imgStr("puppy_flamepool_small"), imgsetCache[puppyLavaSetSmall],
                           1, 8, Point2(0.0, 0.5));
  Image::create_tiles_norm(imgStr("puppy_flamepool"), imgsetCache[puppyLavaSetLarge],
                           1, 8, Point2(0.0, 0.5));
  
  imgsetCache[puppyCrateSet].add(Image::create_centered(imgStr("puppy_crate_a")));
  imgsetCache[puppyCrateSet].add(Image::create_centered(imgStr("puppy_crate_b")));
  
  Image::create_tiles_abs(imgStr("bomb"), imgsetCache[bombSet], 2, 1, Point2(26.0, 42.0));
  Image::create_tiles(imgStr("explosion"), imgsetCache[bombExplodeSet], 3, 1, true, true);
  
  imgCache[warningH] = Image::create_handled_norm(imgStr("warning_h"), Point2(1.0, 0.5));
  imgCache[warningV] = Image::create_handled_norm(imgStr("warning_v"), Point2(0.5, 0.0));
  
  loadTilesAbs(puppyMissileImgs, "missile", 2, 2, Point2(76.0, 20.0));
  
  Image::create_tiles_norm(imgStr("puppy_cage"), imgsetCache[puppyCage], 3, 1, Point2(0.5, 1.0));
  loadTilesNorm(puppyAImgs, "puppy_a", 11, 1, Point2(0.5, 1.0));
  loadTilesNorm(puppyBImgs, "puppy_b", 11, 1, Point2(0.5, 1.0));
  loadTilesNorm(puppyCImgs, "puppy_c", 11, 1, Point2(0.5, 1.0));
  
  Image::create_tiles_abs(imgStr("mole"), imgsetCache[moleImgs], 5, 4, Point2(82.0, 89.0));
  imgCache[caltropImg] = Image::create_handled_abs(imgStr("mole_spikeball"), Point2(20.0, 18.0));
  
  imgCache[puppySpikeball] = Image::create_handled_abs(imgStr("puppy_spikeball"), Point2(48.0, 48.0));
  
  Image::create_tiles_abs(imgStr("fireball"), imgsetCache[puppyFireballSet], 4, 1, Point2(50.0, 24.0));
  Image::create_tiles(imgStr("firesplash"), imgsetCache[puppyFireSplashSet], 4, 1, true);
  
  Image::create_tiles_abs(imgStr("fake_cage"), imgsetCache[mimicSet], 4, 1, Point2(66.0, 72.0));
  
  imgCache[madsun] = Image::create_handled_abs(imgStr("sun_skull"), Point2(64.0, 64.0));
  imgCache[madsunGlow] = Image::create_handled_abs(imgStr("sun_skull_glow"), Point2(64.0, 64.0), true);
  imgCache[madsunFlame] = Image::create_centered(imgStr("flame"), true);
  dwarpCache[sunWarp] = new DWarpSequence("sunskull.dwarp2", true);
  
  Image::create_tiles(imgStr("meat"), imgsetCache[junkSet], 3, 1);
  imgsetCache[junkSet][0]->handle.set(23.0, 38.0);
  imgsetCache[junkSet][1]->handle.set(24.0, 32.0);
  imgsetCache[junkSet][2]->handle.set(38.0, 35.0);
  
  imgCache[skullBarFrameImg] = Image::create_handled_norm(imgStr("puppy_hud"), Point2(0.0, 1.0));
  
  loadSound(fireballJumpSound, "fireball_jump.wav");
  loadSound(missileSound, "missile.wav");
  loadSound(puppyBark, "puppy.wav");
}

void PuppyLevel::unloadRes()
{
  
}

void PuppyLevel::loadMe()
{
  createChunk(CHUNK_PUPPY_DOGHOUSE);
  startStanding(Point2(200.0, 312.0));
  
  createLevel();
  addTextEffectStd(LSTRINGS[101], Point2(PLAY_CX, PLAY_CY));
  
  worldBox = Box(0.0, -PLAY_H * 2.0, endpoint.x, PLAY_H * 3.0);
  invisibleWallsStd(worldBox);
}

void PuppyLevel::createLevel()
{
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);
}

void PuppyLevel::updateCamera()
{
  updateZoomStd();
  
  // cam target normally based on dir
  Point1 camTargetX = objectiveDir > 0.0 ?
  player->getX() - 64.0 : player->getX() + 64.0 - PLAY_W;
  
  if (levelState == LEVEL_WIN)
  {
    camTargetX = RM::lerp(puppyStartPt.x, puppyHousePt.x, 0.5) - PLAY_CX + 64.0;
  }
  
  for (Coord1 i = 0; i < puppyList.count; ++i)
  {
    if (puppyList[i]->aiState < Billy::ON_PLAYER &&
        puppyList[i]->onScreen() == true)
    {
      camTargetX = RM::lerp(player->getX(), puppyList[i]->getX(), 0.5) - PLAY_CX;
    }
  }
  
  player->setFacing(objectiveDir > 0.0 ? 1 : -1);
  
  camera.xy.x = RM::flatten(camera.xy.x, camTargetX, RM::timePassed() * playerTXVel() * 1.2);
  camera.xy.y = 0.0;
  
  camera.handle.set(PLAY_CX, SCREEN_H + Pizza::platformTL.y);
  camera.clampHorizontal(Point2(worldBox.left() - Pizza::platformTL.x / camera.zoom.x,
                                worldBox.right() + Pizza::platformTL.x / camera.zoom.x));
  
  if (sun != NULL) sun->updatePostCamera();
}

void PuppyLevel::updateLevelAI()
{
  if (billyState == WAIT_FOR_RETURN)
  {
    crateTimer.update();
    arrowTimer.update();
    bombTimer.update();
    
    if (puppiesToFind == 0 && player->getX() < puppyStartPt.x + 128.0)
    {
      if (sun != NULL) sun->puppyDone(puppyHousePt + Point2(128.0, -128.0));
      
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

void PuppyLevel::updateStatus()
{
  if (player->collisionCircle().top() > PLAY_BOX.bottom())
  {
    loseLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

void PuppyLevel::drawHUD()
{
  imgCache[skullBarFrameImg]->draw(Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y));
  
  String2 text(8);
  text = "$/%";
  text.int_replace('$', puppiesTotal - puppiesToFind);
  text.int_replace('%', puppiesTotal);
  ResourceManager::font->draw(text, Point2(Pizza::platformTL.x, PLAY_H - Pizza::platformTL.y) +
                              Point2(63.0, -44.0), Point2(1.0, 1.0), COLOR_FULL);
}

void PuppyLevel::puppyFound(Billy* puppy)
{
  puppiesToFind--;
  gotScore(350);
  
  if (puppiesToFind <= 0)
  {
    objectiveDir *= -1.0;
    directionChanged();
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
  }
  
  puppyWasFound();
}

void PuppyLevel::puppyReturned(Billy* puppy)
{
  puppiesToReturn--;
  if (puppiesToReturn <= 0)
  {
    levelState = LEVEL_PLAY;  // this hack forces the win to trigger
    winLevel();
  }
}

Coord1 PuppyLevel::calcWinBonus()
{
  return 250 * levelID;
}

void PuppyLevel::callback(ActionEvent* caller)
{
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
}

// ==================================== PULevelEasy

void PULevelEasy::createLevel()
{
  createChunk(CHUNK_PUPPY_DIP);  // NEW
  createChunk(CHUNK_PUPPY_CRATE); // NEW
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // NEW 1
  createChunk(CHUNK_PUPPY_GROUNDTRIANGLE); // NEW
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CRATE); // NEW
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // NEW 2
  createChunk(CHUNK_PUPPY_CRATE); // NEW
  createChunk(CHUNK_PUPPY_DIP); // NEW
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CAGE);  // NEW 3
}

void PULevelEasy::puppyWasFound()
{
  if (tutShown == false && alreadyBeaten() == false)
  {
    addTextEffectStd(LSTRINGS[103], PLAY_CENTER + Point2(0.0, -96.0));
    tutShown = true;
  }
}

void PULevelEasy::directionChanged()
{
  crateTimer.setActive();
}

// ==================================== PULevelSpikepits

void PULevelSpikepits::createLevel()
{
  createChunk(CHUNK_PUPPY_HILL, FAVOR_NOTHING);
  createChunk(CHUNK_PUPPY_SPIKEJUMP);  // NEW
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_SPIKEJUMP);
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_TRICKYJUMP);  // NEW
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_CAGE);  // NEW 3
}

void PULevelSpikepits::directionChanged()
{
  crateTimer.setActive();
}

// ==================================== PULevelMoles

void PULevelMoles::createLevel()
{
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_SPIKEJUMP);
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 3
}

// ==================================== PULevelSpikeballs

void PULevelSpikeballs::createLevel()
{
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_SPIKEBALLHILL);  // NEW
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_TRICKYJUMP);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_SPIKEBALLBUMPS, FAVOR_NOTHING);  // NEW
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_QUADSPIKE, FAVOR_CAGE);  // NEW 3
}

// ==================================== PULevelFallpits

void PULevelFallpits::createLevel()
{
  createChunk(CHUNK_PUPPY_HILL, FAVOR_NOTHING);
  createChunk(CHUNK_PUPPY_CRATEPIT);  // NEW
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_SPIKEBALLBUMPS, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_GROUNDTRIANGLE);
  createChunk(CHUNK_PUPPY_BOUNCEPIT, FAVOR_CAGE);  // NEW 2
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_SPIKEJUMP);
  createChunk(CHUNK_PUPPY_SPIKEBALLPILLARS, FAVOR_CAGE_SPECIAL);  // NEW 3
}

// ==================================== PULevelFireballs

void PULevelFireballs::createLevel()
{
  createChunk(CHUNK_PUPPY_LAVAPOOL);  // NEW
  createChunk(CHUNK_PUPPY_SPIKEBALLHILL);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_LAVAHILL);  // NEW
  createChunk(CHUNK_PUPPY_QUADSPIKE, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_DUALFIREBALL);  // NEW
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CAGE);  // 3
}

// ==================================== PULevelBombs

void PULevelBombs::createLevel()
{
  bombTimer.setActive();
  
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_SINGLETRI);  // NEW
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_LAVAPOOL);
  createChunk(CHUNK_PUPPY_BOUNCEPIT, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_SPIKEBALLPILLARS, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_SPIKEBALLPIT);  // NEW
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_CAGE);  // 3
}

// ==================================== PULevelMimics

void PULevelMimics::createLevel()
{
  createChunk(CHUNK_PUPPY_TRICKYJUMP);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_MIMIC);
  createChunk(CHUNK_PUPPY_QUADSPIKE, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_TRIANGLEPIT); // NEW
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_MIMIC);
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_CRATEPIT);
  createChunk(CHUNK_PUPPY_PILLARS, FAVOR_MIMIC);  // NEW
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // 3
}

// ==================================== PULevelArrows

void PULevelArrows::createLevel()
{
  arrowTimer.setActive();
  
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_GROUNDTRIANGLE);
  createChunk(CHUNK_PUPPY_SPIKEPITHILL, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CRATE);
  createChunk(CHUNK_PUPPY_BOUNCEPIT, FAVOR_MIMIC);
  createChunk(CHUNK_PUPPY_PILLARS, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_SPIKEBALLBUMPS, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_SWINGSQUARE);  // NEW
  createChunk(CHUNK_PUPPY_SPIKEBALLPILLARS, FAVOR_CAGE);  // 3
}

// ==================================== PULevelMix

void PULevelMix::createLevel()
{
  arrowTimer.setActive();
  bombTimer.setActive();
  
  createChunk(CHUNK_PUPPY_SPIKEJUMP);
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_BOUNCEPIT, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_SPIKEBALLHILL);
  createChunk(CHUNK_PUPPY_WALLJUMP);  // NEW
  createChunk(CHUNK_PUPPY_HILL, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_PILLARS, FAVOR_MIMIC);
  createChunk(CHUNK_PUPPY_LAVAHILL);
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_CAGE);  // 3
  
}

// ==================================== PULevelSun

void PULevelSun::createLevel()
{
  junkTimer.setActive();
  
  createChunk(CHUNK_PUPPY_CRATE);
  createChunk(CHUNK_PUPPY_CRATEPIT);
  createChunk(CHUNK_PUPPY_PILLARS, FAVOR_CAGE);  // 1
  createChunk(CHUNK_PUPPY_TRICKYJUMP);
  createChunk(CHUNK_PUPPY_SINGLETRI);
  createChunk(CHUNK_PUPPY_PILLARS, FAVOR_CAGE);  // 2
  createChunk(CHUNK_PUPPY_OBJSTEPS, FAVOR_MOLE);
  createChunk(CHUNK_PUPPY_BIGOBJHILL, FAVOR_MIMIC);
  createChunk(CHUNK_PUPPY_QUADSPIKE, FAVOR_CAGE);  // 3
  createChunk(CHUNK_PUPPY_TRIANGLEPIT);
  createChunk(CHUNK_PUPPY_SWINGSQUARE);
  createChunk(CHUNK_PUPPY_SPIKEBALLPILLARS, FAVOR_CAGE);  // 4
  createChunk(CHUNK_PUPPY_DUALFIREBALL);
  createChunk(CHUNK_PUPPY_WALLJUMP);
  createChunk(CHUNK_PUPPY_SPIKEBALLPILLARS, FAVOR_CAGE);  // 5
  
  // last so it draws last
  sun = new AngrySun();
  sun->hflip(true);
  addAlly(sun);
}

// ==================================== LakePlant ================================ //

LakePlant::LakePlant() :
VisRectangular(),
warper(),
timer(RM::randf(0.5, 1.0), this),
weight(0.0),
frameTarget(0)
{
  
}

void LakePlant::init(Coord1 imgIndex, Point2 location)
{
  setXY(location);
  setImage(imgsetCache[lakePlants][imgIndex]);
  warper.set(this, plantWarps[imgIndex]);
  chooseTarget();
}

void LakePlant::update()
{
  timer.update();
  weight = timer.progress();
}

void LakePlant::redraw()
{
  warper.draw_cont(frameTarget, weight);
}

void LakePlant::chooseTarget()
{
  frameTarget = (frameTarget + RM::randi(1, warper.sequence->num_frames() - 2))
  % (warper.sequence->num_frames() + 1);
}

void LakePlant::callback(ActionEvent* caller)
{
  timer.reset(RM::randf(0.5, 1.0));
  warper.last_to_cont();
  chooseTarget();
}

// ==================================== LakeObjman ================================ //

LakeObjman::LakeObjman() :
ObjmanStd<LakeGO>(),
tide(0.0)
{
  
}

void LakeObjman::updateObjects()
{
  for (LinkedListNode<LakeGO*>* enemyNode = enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    LakeGO* enemy = enemyNode->data;
    
    Point1 tideMult = 1.0;
    if (enemy->type == LakeGO::LAKE_TYPE_HARPOON) tideMult = 0.0;
    if (enemy->type == LakeGO::LAKE_TYPE_BUBBLE) tideMult = 2.0;
    
    enemy->addX(tide * tideMult * RM::timePassed());
    enemy->update();
  }
  
  for (LinkedListNode<LakeGO*>* allyNode = allyList.first;
       allyNode != NULL;
       allyNode = allyNode->next)
  {
    LakeGO* ally = allyNode->data;
    
    Point1 tideMult = 1.0;
    if (ally->type == LakeGO::LAKE_TYPE_HARPOON) tideMult = 0.0;
    if (ally->type == LakeGO::LAKE_TYPE_BUBBLE) tideMult = 2.0;
    
    ally->addX(tide * tideMult * RM::timePassed());
    ally->update();
  }
}

// ==================================== LakeBackground ================================ //

IndexPt::IndexPt(Coord1 setIndex, Point1 setX, Point1 setY) :
  index(setIndex),
  xy(setX, setY)
{
  
}

const Point1 LakeBackground::WORLD_BOTTOM = 1536.0;

const Point2 LakeBackground::CLOSE_CORAL_PTS[3] =
{
  Point2(0.0, 1536.0),
  Point2(1024.0, 1536.0),
  Point2(2048.0, 1536.0)
};

const Point2 LakeBackground::FAR_PLANT_PTS[9] =
{
  Point2(118.0, 45.0),
  Point2(257.0, 437.0),
  Point2(568.0, 581.0),
  Point2(685.0, 360.0),
  Point2(1261.0, 438.0),
  Point2(1370.0, 706.0),
  Point2(1710.0, 360.0),
  Point2(1816.0, 245.0),
  Point2(2017.0, 705.0)
};

const Point2 LakeBackground::CLOSE_PLANT_PTS[10] =
{
  Point2(56.0, 1433.0),
  Point2(191.0, 1312.0),
  Point2(470.0, 1465.0),
  Point2(808.0, 1290.0),
  Point2(867.0, 1203.0),
  Point2(1495.0, 1565.0),
  Point2(1808.0, 1361.0),
  Point2(2110.0, 1434.0),
  Point2(2743.0, 1721.0),
  Point2(2865.0, 1276.0)
};

const Point2 LakeBackground::FG_PLANT_PTS[10] =
{
  Point2(205.0, 1905.0),
  Point2(468.0, 2020.0),
  Point2(1213.0, 1988.0),
  Point2(1827.0, 1884.0),
  Point2(2931.0, 1911.0),
  Point2(3043.0, 1971.0),
  Point2(3830.0, 1912.0)
};

const IndexPt LakeBackground::FAR_CORAL_IPTS[10] =
{
  IndexPt(0, 0.0, 768.0),
  IndexPt(0, 1024.0, 768.0),
  IndexPt(1, 1312.0, 768.0),
  IndexPt(1, 288.0, 768.0),
  IndexPt(2, 496.0, 768.0),
  IndexPt(2, 1520.0, 768.0),
  IndexPt(3, 656.0, 768.0),
  IndexPt(3, 1680.0, 768.0),
  IndexPt(4, 928.0, 768.0),
  IndexPt(4, 1952.0, 768.0)
};

const IndexPt LakeBackground::FG_CORAL_IPTS[10] =
{
  IndexPt(0, 108.0, 2048.0),
  IndexPt(0, 1348.0, 2048.0),
  IndexPt(0, 2848.0, 2048.0),
  IndexPt(0, 3729.0, 2048.0),
  IndexPt(1, 405.0, 2048.0),
  IndexPt(1, 1080.0, 2048.0),
  IndexPt(1, 2293.0, 2048.0),
  IndexPt(2, 760.0, 2048.0),
  IndexPt(2, 1654.0, 2048.0),
  IndexPt(2, 3358.0, 2048.0)
};

// ==================================== LakeBackground class

LakeBackground::LakeBackground(LakeLevel* setLevel) :
  StdBackground(),
  level(setLevel),
  farPlants(RM::count(FAR_PLANT_PTS), true),
  closePlants(RM::count(CLOSE_PLANT_PTS), true),
  fgPlants(RM::count(FG_PLANT_PTS), true)
{
  
}

void LakeBackground::load()
{
  for (Coord1 i = 0; i < farPlants.count; ++i)
  {
    farPlants[i].init(0, FAR_PLANT_PTS[i]);
    farPlants[i].addXY(Pizza::platformTL * 2.0 + Point2(0.0, 16.0));
  }
  
  for (Coord1 i = 0; i < closePlants.count; ++i)
  {
    closePlants[i].init(1, CLOSE_PLANT_PTS[i]);
    closePlants[i].addXY(-Pizza::platformTL);
  }
  
  for (Coord1 i = 0; i < fgPlants.count; ++i)
  {
    fgPlants[i].init(2, FG_PLANT_PTS[i]);
    // fgPlants[i].addXY(-Pizza::platformTL);
  }
}

void LakeBackground::update()
{
  // update plants
  for (Coord1 i = 0; i < farPlants.count; ++i) farPlants[i].update();
  for (Coord1 i = 0; i < closePlants.count; ++i) closePlants[i].update();
  for (Coord1 i = 0; i < fgPlants.count; ++i) fgPlants[i].update();
}

void LakeBackground::drawBackground()
{
  CameraStd adjustCam = level->camera;
  adjustCam.xy -= SCREEN_BOX.xy;
  adjustCam.handle += SCREEN_BOX.xy;
  adjustCam.size = SCREEN_WH;
  CameraStd bgCam = level->platCam();
  
  // clear to blue
  RMGraphics->clear(ColorP4(0.0, 0.0, 1.0, 1.0));
  
  // ocean bg
  bgCam.applyParallax(adjustCam, LAKE_WORLD_SIZE, Point2(1024.0, 512.0)
                      + Pizza::platformTL * 2.0 / level->camera.zoom, 0.25);
  imgCache[lakeBG]->draw();
  
  // far coral, far plants
  bgCam.applyParallax(adjustCam, LAKE_WORLD_SIZE, Point2(1536.0, 768.0)
                      + Pizza::platformTL * 2.0 / level->camera.zoom, 0.5);
  
  for (Coord1 i = 0; i < RM::count(FAR_CORAL_IPTS); ++i)
  {
    Image* img = imgsetCache[lakeFarCoral][FAR_CORAL_IPTS[i].index];
    img->draw(FAR_CORAL_IPTS[i].xy + Pizza::platformTL * 2.0 + Point2(0.0, 16.0));
  }
  
  for (Coord1 i = 0; i < farPlants.count; ++i)
  {
    farPlants[i].redraw();
  }
  
  // close coral, close plants
  bgCam.applyParallax(adjustCam, LAKE_WORLD_SIZE, Point2(3072.0, 1536.0)
                      + Pizza::platformTL * 2.0 / level->camera.zoom, 1.0);
  
  for (Coord1 i = 0; i < RM::count(CLOSE_CORAL_PTS); ++i)
  {
    imgCache[lakeCloseCoral]->draw(CLOSE_CORAL_PTS[i] - Pizza::platformTL);
  }
  
  for (Coord1 i = 0; i < closePlants.count; ++i)
  {
    closePlants[i].redraw();
  }
}

void LakeBackground::drawForeground()
{
  CameraStd bgCam = level->camera;
  
  // fg coral, fg plants
  bgCam.applyParallax(level->camera, LAKE_WORLD_SIZE, Point2(4096.0, 2048.0)
                      + Pizza::platformTL * 2.0 / level->camera.zoom, 1.25);
  
  for (Coord1 i = 0; i < RM::count(FG_CORAL_IPTS); ++i)
  {
    imgsetCache[lakeFGCoral][FG_CORAL_IPTS[i].index]->draw(FG_CORAL_IPTS[i].xy );
  }
  
  for (Coord1 i = 0; i < fgPlants.count; ++i)
  {
    fgPlants[i].redraw();
  }
  
  
  bgCam.setXY(0.0, 0.0);
  bgCam.zoom.set(1.0, 1.0);
  bgCam.apply();
  
  Point1 lightAlpha = 1.0 - RM::lerp_reverse(level->player->getY(), 0.0, LAKE_WORLD_SIZE.y);
  imgCache[lakeLight]->draw_scale(Point2(PLAY_W + 16.0, -16.0),
                                  Point2(1.0, 1.0), 0.0, ColorP4(1.0, 1.0, 1.0, lightAlpha));
}

void LakeBackground::drawOnscreen(Image* img, Point2 location)
{
  
}

// ==================================== LakeLevel ================================ //

LakeLevel::LakeLevel() :
  PizzaLevelInterface(),
  ActionListener(),
  objman(),
  player(NULL),
  bg(this),

  camera(),
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
  camera.size = SCREEN_WH;
}

void LakeLevel::loadRes()
{
  dwarpCache[lakeFaceWarp] = new DWarpSequence("pizzaslice.dwarp2", true);
  
  plantWarps.add(new DWarpSequence("lakeplantsmall.dwarp2", true));
  plantWarps.add(new DWarpSequence("lakeplantmed.dwarp2", true));
  plantWarps.add(new DWarpSequence("lakeplantbig.dwarp2", true));
  
  imgCache[lakeBG] = Image::create_handled_norm(imgStr("lake_bg"), Point2(0.0, 0.0));
  imgCache[lakeCloseCoral] = Image::create_handled_norm(imgStr("lake_close_coral"), Point2(0.0, 1.0));
  imgCache[lakeLight] = Image::create_handled_norm(imgStr("lake_light"), Point2(1.0, 0.0));
  
  imgsetCache[lakeFGCoral].add(Image::create_handled_norm(imgStr("lake_fg_coral_a"), Point2(0.0, 1.0)));
  imgsetCache[lakeFGCoral].add(Image::create_handled_norm(imgStr("lake_fg_coral_b"), Point2(0.0, 1.0)));
  imgsetCache[lakeFGCoral].add(Image::create_handled_norm(imgStr("lake_fg_coral_c"), Point2(0.0, 1.0)));
  
  imgsetCache[lakeFarCoral].add(Image::create_handled_norm(imgStr("lake_far_coral_a"), Point2(0.0, 1.0)));
  imgsetCache[lakeFarCoral].add(Image::create_handled_norm(imgStr("lake_far_coral_b"), Point2(0.0, 1.0)));
  imgsetCache[lakeFarCoral].add(Image::create_handled_norm(imgStr("lake_far_coral_c"), Point2(0.0, 1.0)));
  imgsetCache[lakeFarCoral].add(Image::create_handled_norm(imgStr("lake_far_coral_d"), Point2(0.0, 1.0)));
  imgsetCache[lakeFarCoral].add(Image::create_handled_norm(imgStr("lake_far_coral_e"), Point2(0.0, 1.0)));
  
  imgsetCache[lakePlants].add(Image::create_handled_abs(imgStr("lake_plant_small"), Point2(22.0, 74.0)));
  imgsetCache[lakePlants].add(Image::create_handled_abs(imgStr("lake_plant_med"), Point2(27.0, 128.0)));
  imgsetCache[lakePlants].add(Image::create_handled_abs(imgStr("lake_plant_big"), Point2(47.0, 225.0)));
  
  Image::create_tiles(imgStr("hearts"), imgsetCache[lakeHeartSet], 2, 1, true);
  
  imgCache[lakeHaroon] = Image::create_handled_abs(imgStr("harpoon"), Point2(16.0, 96.0));
  imgCache[lakeHarpoonRope] = Image::create_handled_norm(imgStr("harpoon_rope"), Point2(0.0, 0.5));
  imgCache[warningV] = Image::create_handled_norm(imgStr("warning_v"), Point2(0.5, 0.0));
  
  Image::create_tiles(imgStr("poison"), imgsetCache[lakePoisonSet], 4, 1, true);
  
  imgCache[lakePizza32] = Image::create_centered(imgStr("pizza_slice_32"));
  imgCache[lakePizza64] = Image::create_centered(imgStr("pizza_slice_64"));
  imgCache[lakePizza128] = Image::create_centered(imgStr("pizza_slice_128"));
  imgCache[lakePizza256] = Image::create_centered(imgStr("pizza_slice_256"));
  
  imgsetCache[lakeFishBodySet].add(Image::create_handled_abs(imgStr("fish_body_16"), Point2(8.0, 8.0)));
  imgsetCache[lakeFishBodySet].add(Image::create_handled_abs(imgStr("fish_body_32"), Point2(16.0, 12.0)));
  imgsetCache[lakeFishBodySet].add(Image::create_handled_abs(imgStr("fish_body_64"), Point2(36.0, 20.0)));
  imgsetCache[lakeFishBodySet].add(Image::create_handled_abs(imgStr("fish_body_128"), Point2(68.0, 32.0)));
  imgsetCache[lakeFishBodySet].add(Image::create_handled_abs(imgStr("fish_body_256"), Point2(128.0, 64.0)));
  
  imgsetCache[lakeFishHeadSet].add(Image::create_handled_abs(imgStr("fish_head_16"), Point2(0.0, 8.0)));
  imgsetCache[lakeFishHeadSet].add(Image::create_handled_abs(imgStr("fish_head_32"), Point2(0.0, 8.0)));
  imgsetCache[lakeFishHeadSet].add(Image::create_handled_abs(imgStr("fish_head_64"), Point2(-8.0, 12.0)));
  imgsetCache[lakeFishHeadSet].add(Image::create_handled_abs(imgStr("fish_head_128"), Point2(-20.0, 20.0)));
  imgsetCache[lakeFishHeadSet].add(Image::create_handled_abs(imgStr("fish_head_256"), Point2(-32.0, 44.0)));
  
  fishBodyWarps.add(new DWarpSequence("fishbody16.dwarp2", true));
  fishBodyWarps.add(new DWarpSequence("fishbody32.dwarp2", true));
  fishBodyWarps.add(new DWarpSequence("fishbody64.dwarp2", true));
  fishBodyWarps.add(new DWarpSequence("fishbody128.dwarp2", true));
  fishBodyWarps.add(new DWarpSequence("fishbody256.dwarp2", true));
  
  fishHeadWarps.add(new DWarpSequence("fishhead16.dwarp2", true));
  fishHeadWarps.add(new DWarpSequence("fishhead32.dwarp2", true));
  fishHeadWarps.add(new DWarpSequence("fishhead64.dwarp2", true));
  fishHeadWarps.add(new DWarpSequence("fishhead128.dwarp2", true));
  fishHeadWarps.add(new DWarpSequence("fishhead256.dwarp2", true));
  
  imgCache[lakePufferDeflated] = Image::create_centered(imgStr("puffer_deflated"));
  imgCache[lakePufferTrans] = Image::create_centered(imgStr("puffer_transition"));
  imgCache[lakePufferInflated] = Image::create_centered(imgStr("puffer_inflated"));
  dwarpCache[pufferDeflatedWarp] = new DWarpSequence("pufferdeflated.dwarp2", true);
  dwarpCache[pufferInflatedWarp] = new DWarpSequence("pufferinflated.dwarp2", true);
  
  Image::create_tiles(imgStr("starfish"), imgsetCache[lakeStarfishSet], 2, 1, true);
  
  imgCache[lakeSwordfishImg] = Image::create_handled_abs(imgStr("swordfish"), Point2(54.0, 26.0));
  dwarpCache[swordfishWarp] = new DWarpSequence("swordfish.dwarp2", true);
  
  addImgNorm(lakeEelSet, "eel_0", HANDLE_C);
  addImgNorm(lakeEelSet, "eel_1", HANDLE_C);
  addImgNorm(lakeEelSet, "eel_2", HANDLE_C);
  addImgNorm(lakeEelSet, "eel_3", HANDLE_C);
  loadWarp(eelWarp, "eel.dwarp2");
  
  imgCache[lakePearlImg] = Image::create_centered(imgStr("pearl"));
  Image::create_tiles(imgStr("pearl_sparkles"), imgsetCache[lakePearlSparkleSet], 4, 1, true, true);
  
  loadTilesNorm(lakeBarrelSet, "lake_barrel", 2, 1, HANDLE_C);
  
  Image::create_tiles(imgStr("explosion"), imgsetCache[bombExplodeSet], 3, 1, true, true);
  
  imgsetCache[lakeBubbleSet].add(Image::create_centered(imgStr("bubble_4")));
  imgsetCache[lakeBubbleSet].add(Image::create_centered(imgStr("bubble_8")));
  imgsetCache[lakeBubbleSet].add(Image::create_centered(imgStr("bubble_16")));
  imgsetCache[lakeBubbleSet].add(Image::create_centered(imgStr("bubble_32")));
  imgsetCache[lakeBubbleSet].add(Image::create_centered(imgStr("bubble_64")));
  
  loadSound(fishEatSound, "fisheat.wav");
  loadSound(swimSound, "swim.wav");
  loadSound(harpoonSound, "harpoon.wav");
  loadSound(eelSound, "electric_eel.wav");
}

LakeLevel::~LakeLevel()
{
  delete player;
}

void LakeLevel::unloadRes()
{
  free_clear(plantWarps);
  free_clear(fishBodyWarps);
  free_clear(fishHeadWarps);
}

void LakeLevel::load()
{
  initScoreText();
  
  bg.load();
  
  player = new LakePlayer();
  player->level = this;
  player->setXY(LAKE_PLAYER_START);
  player->load();
  
  updateCamera();
  
  populateWorld();
}

void LakeLevel::update()
{
  // ========== modified time
  RM::push_time_mult(worldTimeMult);
  
  bg.update();
  objman.updateObjects();
  objman.actions.update();
  objman.garbageCollector();
  
  player->addX(objman.tide * RM::timePassed());
  player->update();
  
  if (levelState == LEVEL_PLAY)
  {
    RM::bounce_arcsine(objman.tide, tideData, Point2(-tideMax, tideMax), 0.1 * RM::timePassed());
    Pizza::runGameTime(RM::timePassed());
    harpoonTimer.update();
  }
  
  updateControls();
  bubbleTimer.update();
  updateCamera();
  
  RM::pop_time_mult();
  // ==========
  
  objman.frontActions.update();
}

void LakeLevel::redraw()
{
  // this cancels out the top left move from the scene
  RMGraphics->push_camera(-Pizza::platformTL, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  RMGraphics->push_camera(CameraStd());
  // ========== camera on
  bg.drawBackground();
  
  camera.apply();
  objman.drawEnemies();
  player->redraw();
  objman.drawAllies();
  objman.actions.redraw();
  
  bg.drawForeground();
  
  // ========== camera off
  RMGraphics->pop_camera();
  RMGraphics->pop_camera();
  
  drawHearts();
  scoreText.drawText();
  objman.frontActions.redraw();
  
  // debug tide draw
  //  RMGraphics->draw_line(Point2(PLAY_CX, PLAY_H - 32.0), Point2(PLAY_CX + objman.tide, PLAY_H - 32.0),
  //                        BLACK_SOLID, WHITE_SOLID);
}

void LakeLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  if (Pizza::controller->clickedOutsidePause() == true)
  {
    player->tryJump();
  }
  
  player->tryMove(PizzaLevel::tiltMagnitude());
  player->setFacing(PizzaLevel::tiltMagnitude() < 0.0 ? -1 : 1);
}

void LakeLevel::updateCamera()
{
  shaker.update();
  
  // this is to facilitate other things moving the player
  if (levelState == LEVEL_LOSE) return;
  
  camera.setXY(player->getXY() - SCREEN_CENTER);
  camera.handle = SCREEN_CENTER;
  
  cameraStaller.update();
  
  if (cameraStaller.getActive() == false)
  {
    camera.zoom.x = RM::flatten(camera.zoom.x, 1.0 - 0.166 * (player->sizeIndex - LakeGO::SIZE_32),
                                0.166 * RM::timePassed());
  }
  
  camera.zoom.y = camera.zoom.x;
  
  Box clampBox = LAKE_WORLD_BOX;
  clampBox.wh += Pizza::platformTL * 2.0 / camera.zoom;
  
  if (Pizza::rightLetterbox(RM_WH).width() > 0.0)
  {
    // not sure why it's * 1.5 but this works on the X
    clampBox.wh.x += Pizza::rightLetterbox(RM_WH).width() * 1.5 / camera.zoom.x;
  }
  
  camera.clampToArea(clampBox);
}

void LakeLevel::playerGrew(Coord1 newSize)
{
  cameraStaller.reset();
}


void LakeLevel::addEnemy(LakeGO* fish)
{
  fish->level = this;
  fish->player = player;
  objman.addEnemy(fish);
  fish->load();
}

void LakeLevel::addAlly(LakeGO* ally)
{
  ally->level = this;
  ally->player = player;
  objman.addAlly(ally);
  ally->load();
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
  
  if (pearlsEaten == 3)
  {
    Pizza::scoreMan->earnedAchievement(ACH_3_PEARLS);
  }
  
  shaker.setMagnitude(fish->sizeIndex + 2.0);
  shaker.timedShake(0.3 + fish->sizeIndex * 0.1);
}

void LakeLevel::playerDamaged(Logical instantLose)
{
  if (levelState != LEVEL_PLAY) return;
  
  Coord1 numDamage = 1;
  if (instantLose == true)
  {
    numDamage = std::max(maxHearts - Pizza::currGameVars[VAR_DAMAGE], 0);
  }
  
  ResourceManager::playDamaged();
  
  for (Coord1 i = 0; i < numDamage; ++i)
  {
    Coord1 currHearts = std::max(maxHearts - Pizza::currGameVars[VAR_DAMAGE], 0);
    
    PoofEffect* heartPoof = new PoofEffect(heartPos(currHearts - 1), imgsetCache[lakeHeartSet][0]);
    heartPoof->startScale = 1.0;
    heartPoof->endScale = 0.0;
    heartPoof->init();
    addFrontAction(heartPoof);
    
    Pizza::currGameVars[VAR_DAMAGE]++;
  }
  
  if (Pizza::currGameVars[VAR_DAMAGE] >= maxHearts) loseLevel();
}

void LakeLevel::drawHearts()
{
  Coord1 currHearts = std::max(maxHearts - Pizza::currGameVars[VAR_DAMAGE], 0);
  
  for (Coord1 i = 0; i < maxHearts; ++i)
  {
    Logical filled = (i + 1) <= currHearts;
    Image* heartImg = imgsetCache[lakeHeartSet][(filled ? 0 : 1)];
    heartImg->draw(heartPos(i));
  }
}

Point2 LakeLevel::heartPos(Coord1 heartID)
{
  return Point2(28.0 + 40.0 * heartID, 24.0) + Pizza::platformTL;
}

void LakeLevel::placeLakeGOStd(LakeGO* lgo)
{
  Box createBox = LAKE_WORLD_BOX;
  createBox.xy.y += 64.0;
  createBox.grow_down(-128.0);
  
  Point2 target(RM::randf(createBox.left(), createBox.right()),
                RM::randf(createBox.top(), createBox.bottom()));
  lgo->setXY(target);
  
  Box fishBox = lgo->collisionCircle().AABB();
  fishBox.xy -= 32.0;
  fishBox.grow_right(64.0);
  fishBox.grow_down(64.0);
  
  if (fishBox.collision(camera.myBox()) == true)
  {
    // this pushes it a screen away left or right. if it goes out of the
    // world bounds it will just get removed and another will generate
    lgo->addX((SCREEN_W + 64.0) * (lgo->getX() > player->getX() ? 1.0 : -1.0));
  }
}

LakeFish* LakeLevel::createNormalFish(Coord1 size)
{
  LakeFish* fish = new LakeFish(size);
  if (randGen.randf(0.0, 1.0) < runnerPercent) fish->willRun = true;
  if (randGen.randf(0.0, 1.0) < chasePercent) fish->willChase = true;
  if (randGen.randf(0.0, 1.0) < poisonPercent && size != LakeGO::SIZE_16) fish->poison = true;
  
  placeLakeGOStd(fish);
  addEnemy(fish);
  return fish;
}

LakeFish* LakeLevel::createNormalFish(const LakeFish& original)
{
  LakeFish* fish = new LakeFish(original.sizeIndex);
  fish->poison = original.poison;
  fish->willRun = original.willRun;
  fish->willChase = original.willChase;
  
  placeLakeGOStd(fish);
  addEnemy(fish);
  return fish;
}

LakePuffer* LakeLevel::createPuffer()
{
  LakePuffer* puffer = new LakePuffer();
  placeLakeGOStd(puffer);
  addEnemy(puffer);
  return puffer;
}

LakeSwordfish* LakeLevel::createSwordfish()
{
  LakeSwordfish* swordfish = new LakeSwordfish();
  placeLakeGOStd(swordfish);
  addEnemy(swordfish);
  return swordfish;
}

Starfish* LakeLevel::createStarfish()
{
  Starfish* star = new Starfish();
  placeLakeGOStd(star);
  addEnemy(star);
  return star;
}

LakeBarrel* LakeLevel::createBarrel()
{
  LakeBarrel* barrel = new LakeBarrel();
  placeLakeGOStd(barrel);
  addEnemy(barrel);
  return barrel;
}

LakeEel* LakeLevel::createEel()
{
  LakeEel* eel = new LakeEel();
  placeLakeGOStd(eel);
  addEnemy(eel);
  return eel;
}

LakePearl* LakeLevel::createPearl()
{
  LakePearl* pearl = new LakePearl();
  placeLakeGOStd(pearl);
  addEnemy(pearl);
  return pearl;
}

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

void LakeLevel::callback(ActionEvent* caller)
{
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
}

// ==================================== LALevelEasy

void LALevelEasy::populateWorld()
{
  Coord1 sizeNums[] = {25, 10, 8, 7, 5};
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  tryCreateUnlockable(LAKE_WORLD_BOX.corner(2) - Point2(96.0, 96.0),
                      Coord2(CHEESE_LAYER, 13));
}

// ==================================== LALevelPearl

void LALevelPearl::populateWorld()
{
  Coord1 sizeNums[] = {20, 10, 8, 7, 5};
  runnerPercent = 0.5;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 3; ++i) createPearl();
}

// ==================================== LALevelStarfish

void LALevelStarfish::populateWorld()
{
  Coord1 sizeNums[] = {20, 10, 8, 7, 5};
  runnerPercent = 0.65;
  chasePercent = 0.5;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 7; ++i) createStarfish();
}

// ==================================== LALevelPuffer

void LALevelPuffer::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 8, 7};
  runnerPercent = 0.75;
  chasePercent = 0.65;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 4; ++i) createStarfish();
  for (Coord1 i = 0; i < 7; ++i) createPuffer();
}

// ==================================== LALevelTide

void LALevelTide::populateWorld()
{
  Coord1 sizeNums[] = {15, 15, 10, 8, 7};
  runnerPercent = 0.85;
  chasePercent = 0.75;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 5; ++i) createStarfish();
  for (Coord1 i = 0; i < 5; ++i) createPuffer();
  
  tideMax = 96.0;
}

// ==================================== LALevelSwordfish

void LALevelSwordfish::populateWorld()
{
  Coord1 sizeNums[] = {12, 12, 12, 9, 8};
  runnerPercent = 0.85;
  chasePercent = 0.75;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 4; ++i) createStarfish();
  for (Coord1 i = 0; i < 4; ++i) createPuffer();
  for (Coord1 i = 0; i < 10; ++i) createSwordfish();
  
  tideMax = 32.0;
}

// ==================================== LALevelPoison

void LALevelPoison::populateWorld()
{
  Coord1 sizeNums[] = {14, 14, 14, 11, 10};
  runnerPercent = 0.85;
  chasePercent = 0.75;
  poisonPercent = 0.33;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 4; ++i) createStarfish();
  for (Coord1 i = 0; i < 4; ++i) createPuffer();
  for (Coord1 i = 0; i < 4; ++i) createSwordfish();
  
  tryCreateUnlockable(LAKE_WORLD_BOX.corner(0) + Point2(96.0, 96.0), Coord2(3, 10));
  
  tideMax = 0.0;
}

// ==================================== LALevelHarpoon

void LALevelHarpoon::populateWorld()
{
  Coord1 sizeNums[] = {14, 14, 14, 14, 14};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  poisonPercent = 0.33;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 5; ++i) createStarfish();
  for (Coord1 i = 0; i < 5; ++i) createPuffer();
  for (Coord1 i = 0; i < 5; ++i) createSwordfish();
  
  tideMax = 64.0;
  harpoonTimer.setActive();
}

// ==================================== LALevelBarrel

void LALevelBarrel::populateWorld()
{
  Coord1 sizeNums[] = {20, 14, 13, 11, 10};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  poisonPercent = 0.33;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 2; ++i) createPearl();
  for (Coord1 i = 0; i < 3; ++i) createPuffer();
  for (Coord1 i = 0; i < 3; ++i) createSwordfish();
  for (Coord1 i = 0; i < 8; ++i) createBarrel();
  
  tideMax = 32.0;
}

// ==================================== LALevelEel

void LALevelEel::populateWorld()
{
  Coord1 sizeNums[] = {20, 14, 13, 11, 10};
  runnerPercent = 1.0;
  chasePercent = 1.0;
  poisonPercent = 0.33;
  
  for (Coord1 sizeType = 0; sizeType <= LakeGO::SIZE_256; ++sizeType)
  {
    for (Coord1 i = 0; i < sizeNums[sizeType]; ++i) createNormalFish(sizeType);
  }
  
  for (Coord1 i = 0; i < 1; ++i) createPearl();
  for (Coord1 i = 0; i < 2; ++i) createPuffer();
  // for (Coord1 i = 0; i < 3; ++i) createSwordfish();
  for (Coord1 i = 0; i < 7; ++i) createStarfish();
  for (Coord1 i = 0; i < 4; ++i) createBarrel();
  for (Coord1 i = 0; i < 10; ++i) createEel();
  
  tideMax = 0.0;
}

// ==================================== WreckBackground ================================ //

WreckBackground::WreckBackground(WreckLevel* setLevel) :
StdBackground(),
level(setLevel),
clouds(3, true),
isInitialized(false)
{
  
}

void WreckBackground::init()
{
  for (Coord1 i = 0; i < clouds.count; ++i)
  {
    Image* img = imgsetCache[bgObjectSet][i];
    Point2 scrollSpeed = Point2((4.0 - i) * -4.0, 0.0);
    clouds[i].init(img, scrollSpeed,
                   Point2(0.0, RM::lerp(PLAY_H, 0.0, i / 3.0)),
                   Point2(SCREEN_W, img->natural_size().y));
    clouds[i].logicalImgSize.x = SCREEN_W + img->natural_size().x;
    clouds[i].scrollOffset.x = RM::randf(0.0, SCREEN_W);
  }
}

void WreckBackground::update()
{
  if (isInitialized == false)
  {
    isInitialized = true;
    init();
  }
  
  for (Coord1 i = 0; i < clouds.count; ++i)
  {
    clouds[i].update();
    clouds[i].startPos.y += level->getChainVel() * (0.6 + i * 0.1) * RM::timePassed();
    
    Point1 bottom = clouds[i].startPos.y + clouds[i].logicalImgSize.y;
    Box camBox = level->camera.myBox();
    
    if (bottom < camBox.top())
    {
      clouds[i].startPos.y = camBox.bottom() + RM::randf(0.0, PLAY_Y * 0.5);
      clouds[i].scrollOffset.x = RM::randf(0.0, SCREEN_W - clouds[i].getImage()->natural_size().x);
    }
  }
}

void WreckBackground::drawBackground()
{
  CameraStd bgCam = level->camera;
  Point2 levelSize = level->worldSize;
  
  // clear to black
  RMGraphics->clear(ColorP4(0.0, 0.0, 0.0, 1.0));
  
  // sky
  bgCam.applyParallax(level->camera, levelSize, Point2(512.0, 512.0), 1.0);
  
  imgsetCache[bgLayerSet][0]->draw(-44.0, Pizza::platformTL.y);
  
  // far buildings
  bgCam.applyParallax(level->camera, levelSize, Point2(SCREEN_W, levelSize.y * 0.2), 1.0);
  
  imgsetCache[bgLayerSet][1]->draw(-16.0, levelSize.y * 0.2);
  
  // clouds
  level->camera.apply();
  for (Coord1 i = 0; i < clouds.count; ++i)
  {
    clouds[i].redraw();
  }
  
  // mid buildings
  bgCam.applyParallax(level->camera, levelSize, Point2(SCREEN_W, levelSize.y * 0.5), 1.0);
  
  imgsetCache[bgLayerSet][2]->draw(-16.0, levelSize.y * 0.5);
  imgsetCache[bgLayerSet][3]->draw(128.0, levelSize.y * 0.5);
  imgsetCache[bgLayerSet][4]->draw(364.0, levelSize.y * 0.5);
  
  // street
  bgCam.applyParallax(level->camera, levelSize, Point2(SCREEN_W, levelSize.y), 1.0);
  
  imgsetCache[bgLayerSet][5]->draw(-16.0, levelSize.y + 32.0);
  imgsetCache[bgLayerSet][6]->draw(120.0, levelSize.y + 32.0 - 64.0);
  imgsetCache[bgLayerSet][7]->draw(332.0, levelSize.y + 32.0 - 64.0);
}

void WreckBackground::drawForeground()
{
  // these are black bars to cover the edges on iphone 5 during big shakes
  Box screenBox = CameraStd::screenBoxToWorld();
  ColorP4 barColor = RM::color255(206.0, 231.0, 249.0, 255.0);
  
  //Box leftBox = Box::from_norm(screenBox.corner(0), Point2(32.0, screenBox.wh.y), HANDLE_TR);
  Box leftBox = Box::from_norm(Point2(-44.0, screenBox.xy.y), Point2(32.0, screenBox.wh.y), HANDLE_TR);
  leftBox.draw_solid(barColor);
  
  // Box rightBox = Box::from_norm(Point2(-44.0 + , 0.0, Point2(32.0, screenBox.wh.y), HANDLE_TL);
  // rightBox.draw_solid(barColor);
}

// ==================================== WreckLevel ================================ //

WreckLevel::WreckLevel() :
PizzaLevelInterface(),
ActionListener(),
b2ContactListener(),

objman(),
player(NULL),
chain(NULL),
bg(NULL),

debrisList(),
windowsDestroyed(0),

numLevels(24),
bottomLevel(0),
bottomY(0.0),

buildingSize(96.0, 128.0),
worldSize(PLAY_W * 10.0, buildingSize.y * numLevels),
emptyRange(0.0, 0.0),
chunkHeight(LEVELS_PER_CHUNK * buildingSize.y),

buildingCache(LEVELS_PER_CHUNK * 2, true),
currObjOffset(-16.0, 0.0),

yTriggers(8),
currLocation(0),

junkTimer(3.0, true, this),
junkType(EASY_JUNK),

powerTimer(3.0),
sparkTimer(0.15, this),
currMaxVel(50.0)
{
  shaker.setMagnitude(3.0);
  shaker.setDuration(0.75);
  junkTimer.setInactive();
  powerTimer.setInactive();
  
  emptyRange.set(PLAY_BOX.left() + buildingSize.x, PLAY_BOX.right() - buildingSize.x);
  
  musicFilename = "wreck.ima4";
}

WreckLevel::~WreckLevel()
{
  delete bg;
  delete player;
}

void WreckLevel::preload()
{
  bg = new WreckBackground(this);
}

void WreckLevel::loadRes()
{
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_sky"), Point2(0.0, 0.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_far_buildings"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_mid_buildings_a"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_mid_buildings_b"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_mid_buildings_c"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_street_a"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_street_b"), Point2(0.0, 1.0)));
  imgsetCache[bgLayerSet].add(Image::create_handled_norm(imgStr("wreck_street_c"), Point2(0.0, 1.0)));
  
  Image::create_tiles_norm(imgStr("wreck_clouds"), imgsetCache[bgObjectSet], 3, 1, Point2(0.5, 0.0));
  
  loadTilesAbs(wreckWindowAll, "wreck_building", 1, 8, Point2(16.0, 0.0));
  loadImgAbs(wreckBuildingPad, "wreck_building_pad", Point2(44.0, 0.0));
  loadImgAbs(wreckEmptyPad, "wreck_empty_building_pad", Point2(44.0, 0.0));
  
  imgCache[wreckWindowStrong] = Image::create_handled_abs(imgStr("wreck_bars_building"), Point2(16.0, 0.0));
  
  imgCache[wreckRoofLeft] = Image::create_handled_abs(imgStr("wreck_roof_a"), Point2(16.0, 40.0));
  imgCache[wreckRoofRight] = Image::create_handled_abs(imgStr("wreck_roof_b"), Point2(16.0, 40.0));
  loadImgAbs(wreckRoofPad, "wreck_roof_pad", Point2(44.0, 40.0));
  
  Image::create_tiles_abs(imgStr("wreck_fire_building"),
                          imgsetCache[wreckWindowFireImgs], 4, 1, Point2(16.0, 0.0));
  imgCache[wreckBottom] = Image::create_handled_abs(imgStr("wreck_empty_building"), Point2(16.0, 0.0));
  
  imgCache[wreckChain] = Image::create_centered(imgStr("wreck_chain"));
  
  Image::create_tiles_abs(imgStr("wreck_girl"), imgsetCache[wreckGirlSrc], 4, 1, Point2(-32.0, -48.0));
  wreckGirlAnim.clear();
  wreckGirlAnim.add(imgsetCache[wreckGirlSrc][0]);
  wreckGirlAnim.add(imgsetCache[wreckGirlSrc][1]);
  wreckGirlAnim.add(imgsetCache[wreckGirlSrc][2]);
  wreckGirlAnim.add(imgsetCache[wreckGirlSrc][1]);
  
  Image::create_tiles_abs(imgStr("wreck_guy"), imgsetCache[wreckBoySrc], 4, 1, Point2(-16.0, -44.0));
  wreckBoyAnim.clear();
  wreckBoyAnim.add(imgsetCache[wreckBoySrc][0]);
  wreckBoyAnim.add(imgsetCache[wreckBoySrc][1]);
  wreckBoyAnim.add(imgsetCache[wreckBoySrc][2]);
  wreckBoyAnim.add(imgsetCache[wreckBoySrc][1]);
  
  Image::create_tiles_abs(imgStr("wreck_old"), imgsetCache[wreckOldSrc], 5, 1, Point2(-32.0, -48.0));
  wreckOldAnim.clear();
  wreckOldAnim.add(imgsetCache[wreckOldSrc][0]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][1]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][2]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][3]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][2]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][3]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][2]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][1]);
  wreckOldAnim.add(imgsetCache[wreckOldSrc][0]);
  
  Image::create_tiles_abs(imgStr("wreck_skeleton"), imgsetCache[wreckSkele],
                          5, 1, Point2(-40.0, -48.0));
  Image::create_pingpong(imgsetCache[wreckSkele], wreckThrowerFull);
  Image::create_tiles_abs(imgStr("wreck_skeleton_arm"), imgsetCache[wreckThrowArmSet],
                          5, 1, Point2(55.0, 94.0));
  Image::create_pingpong(imgsetCache[wreckThrowArmSet], wreckArmFull);
  imgCache[wreckBone] = Image::create_centered(imgStr("wreck_throw_bone"));
  
  Image::create_tiles_abs(imgStr("wreck_useless_skeleton"), imgsetCache[wreckEasy1Set],
                          5, 1, Point2(-16.0, -44.0));
  Image::create_pingpong(imgsetCache[wreckEasy1Set], wreckEasy1Full);
  
  Image::create_tiles_abs(imgStr("wreck_useless_skeleton_b"), imgsetCache[wreckEasy2Set],
                          5, 1, Point2(-32.0, -8.0));
  Image::create_pingpong(imgsetCache[wreckEasy2Set], wreckEasy2Full);
  
  Image::create_tiles_abs(imgStr("wreck_useless_skeleton_c"), imgsetCache[wreckEasy3Set],
                          5, 1, Point2(-16.0, -44.0));
  Image::create_pingpong(imgsetCache[wreckEasy3Set], wreckEasy3Full);
  
  loadTilesAbs(wreckBatterSrc, "wreck_mallet", 5, 1, Point2(-8.0, -24.0));
  Image::create_pingpong(imgsetCache[wreckBatterSrc], wreckBatterFull);
  
  imgCache[wreckNode] = Image::create_centered(imgStr("wreck_node"));
  
  imgsetCache[wreckElectricSet].clear();
  imgsetCache[wreckElectricSet].ensure_capacity(4);
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_0"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_1"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_2"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_3"), Point2(0.0, 0.5)));
  for (Coord1 i = 0; i < imgsetCache[wreckElectricSet].count; ++i)
  {
    // imgsetCache[wreckElectricSet][i]->overexpose();
    imgsetCache[wreckElectricSet][i]->repeats();
  }
  
  loadTilesAbs(wreckBaseballSkele, "wreck_baseball_skeleton", 5, 1, Point2(-16.0, -48.0));
  wreckCatcherReverse = imgsetCache[wreckBaseballSkele];
  wreckCatcherReverse.reverse();
  wreckCatcherThrow = imgsetCache[wreckBaseballSkele];
  wreckCatcherThrow.remove_last();
  
  imgCache[wreckBaseball] = Image::create_centered(imgStr("wreck_baseball"));
  
  Image::create_tiles_abs(imgStr("wreck_climber"), imgsetCache[wreckClimberSrc],
                          4, 2, Point2(70.0, 65.0));
  wreckClimberAttack.copy_from(imgsetCache[wreckClimberSrc], 0, 3);
  Image::create_pingpong(wreckClimberAttack);
  wreckClimberClimb.copy_from(imgsetCache[wreckClimberSrc], 4, 7);
  
  Image::create_tiles_abs(imgStr("wreck_wingskull"), imgsetCache[wreckFlierSrc], 3, 2, Point2(48.0, 24.0));
  wreckWingHover.copy_from(imgsetCache[wreckFlierSrc], 0, 2);
  Image::create_pingpong(wreckWingHover);
  wreckWingAttack.copy_from(imgsetCache[wreckFlierSrc], 3, 5);
  Image::create_pingpong(wreckWingAttack);
  
  Image::create_tiles_abs(imgStr("wreck_sickle_skeleton"), imgsetCache[wreckCutSkeleImgs],
                          1, 5, Point2(24.0, 32.0));
  imgCache[wreckSickle] = Image::create_handled_abs(imgStr("wreck_sickle"), Point2(32.0, 32.0));
  
  Image::create_tiles_abs(imgStr("wreck_ufo"), imgsetCache[wreckUFOSet], 1, 2, Point2(132.0, 84.0));
  imgCache[wreckUFOBlast] = Image::create_handled_norm(imgStr("wreck_ufo_blast"), Point2(0.5, 0.0));
  imgCache[wreckUFOBlastBot] = Image::create_handled_norm(imgStr("wreck_ufo_blast_bottom"), Point2(0.5, 0.0));
  
  imgCache[warningV] = Image::create_handled_norm(imgStr("warning_v"), Point2(0.5, 0.0));
  
  loadTilesNorm(wreckPowerupSet, "wreck_powerup", 3, 1, HANDLE_C);
  loadTilesNorm(wreckPowerupSparkSet, "bolt_particles", 3, 1, HANDLE_C);
  
  imgsetCache[junkSet].add(Image::create_centered(imgStr("wreck_junk_light")));
  imgsetCache[junkSet].add(Image::create_centered(imgStr("wreck_junk_heavy")));
  
  imgCache[wreckObstacle] = Image::create_centered(imgStr("wreck_rect"));
  
  loadTilesNorm(wreckShieldSet, "wreck_shield", 7, 1, HANDLE_C);
  
  loadSound(civilianSound, "civilianhit.wav");
  loadSound(ufoAppearSound, "ufoappear.wav");
  loadSound(ufoBlastSound, "ufo_blast.wav");
  loadSound(wreckPowerSound, "wreck_powerup.wav");
}

void WreckLevel::unloadRes()
{
  
}

void WreckLevel::load()
{
  RMPhysics = new RivermanPhysics(Point2(0.0, 500.0), true);
  RMPhysics->SetContactListener(this);
  
  initHealth();
  initScoreText();
  
  player = new WreckPlayer();
  player->level = this;
  player->load();
  
  chain = new WreckChain();
  addAlly(chain);
  
  buildToBottom();
  
  camera.handle = PLAY_CENTER;
  updateCamera();
}

void WreckLevel::update()
{
  if (tutorial->shouldUpdateGame() == true) updateGame();
  
  updateTutorials();
}

void WreckLevel::redraw()
{
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  
  // ========== camera on
  RMGraphics->push_camera(CameraStd());
  
  bg->drawBackground();
  camera.apply();
  
  debrisList.redraw();
  objman.drawObjects();
  objman.actions.redraw();
  player->redraw();
  bg->drawForeground();
  RMPhysics->draw_debug();
  
  RMGraphics->pop_camera();
  // ========== camera off
  RMGraphics->pop_camera();
  
  drawHealthBar();
  scoreText.drawText();
  tutorial->redraw();
  objman.frontActions.redraw();
}

void WreckLevel::updateGame()
{
  // ========== modified time
  RM::push_time_mult(worldTimeMult);
  
  RMPhysics->update();
  
  debrisList.update();
  objman.updateObjects();
  objman.actions.update();
  objman.garbageCollector();
  
  player->update();
  checkLocations();
  
  updateControls();
  shaker.update();
  updateCamera();
  
  if (levelState == LEVEL_PLAY)
  {
    Pizza::runGameTime(RM::timePassed());
    junkTimer.update();
    updateStatus();
  }
  
  buildToBottom();
  updateHealthBar();
  bg->update();
  
  RM::pop_time_mult();
  // ==========
  
  objman.frontActions.update();
}

void WreckLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  powerTimer.update();
  if (powerActive() == true) sparkTimer.update();
  currMaxVel = RM::flatten(currMaxVel, powerTimer.getActive() == false ? 50.0 : 65.0, 30.0 * RM::timePassed());
  
  Point1 powerMult = 1.0;
  player->setColor(COLOR_FULL);
  
  if (powerTimer.getActive() == true)
  {
    powerMult = 1.5;
  }
  
  if (Pizza::controller->clickedOutsidePause() == true)
  {
    player->tryJump(powerMult);
  }
  
  player->tryMove(PizzaLevel::tiltMagnitude() * powerMult);
}

void WreckLevel::updateCamera()
{
  shaker.update();
  
  // prevents an overshoot right after a win
  if (levelState == LEVEL_PLAY) camera.xy = Point2(0.0, chain->links.first().getY() + 160.0);
}

void WreckLevel::updateStatus()
{
  if (camera.xy.y + PLAY_H >= worldSize.y)
  {
    camera.setY(worldSize.y - PLAY_H);
    
    winLevel();
  }
  else if (lostToDMGStd() == true)
  {
    loseLevel();
  }
}

Coord1 WreckLevel::calcWinBonus()
{
  return 1000 + 125 * levelID;
}

void WreckLevel::addAction(ActionEvent* ae)
{
  objman.addAction(ae);
}

void WreckLevel::addDebris(ActionEvent* ae)
{
  debrisList.addX(ae);
}

void WreckLevel::addFrontAction(ActionEvent* ae)
{
  objman.addFrontAction(ae);
}

WreckBuilding* WreckLevel::createBuilding(Logical leftSide, Coord1 numOffset,
                                          Logical burning, Logical strong)
{
  WreckBuilding* currBuilding = new WreckBuilding(leftSide, bottomY + numOffset * buildingSize.y);
  if (burning == true) currBuilding->setSpikeWindow();
  if (strong == true) currBuilding->setHardWindow();
  addEnemy(currBuilding, false);
  return currBuilding;
}

WreckSkeleEasy* WreckLevel::createSkeleEasy(WreckBuilding* building)
{
  WreckSkeleEasy* skele = new WreckSkeleEasy(building->getHFlip() == false, building->getY());
  building->occupant = skele;
  addEnemy(skele);
  return skele;
}

WreckSkele* WreckLevel::createBoneThrower(WreckBuilding* building)
{
  WreckSkele* skele = new WreckSkele(building->getHFlip() == false, building->getY());
  building->occupant = skele;
  addEnemy(skele);
  return skele;
}

WreckCutterSkele* WreckLevel::createCutter(WreckBuilding* building)
{
  WreckCutterSkele* skele = new WreckCutterSkele(building->getHFlip() == false, building->getY());
  building->occupant = skele;
  addEnemy(skele);
  return skele;
}

WreckBatter* WreckLevel::createBatter(WreckBuilding* building)
{
  WreckBatter* skele = new WreckBatter(building->getHFlip() == false, building->getY());
  building->occupant = skele;
  addEnemy(skele);
  return skele;
}

WreckGirl* WreckLevel::createGirl(WreckBuilding* building)
{
  WreckGirl* person = new WreckGirl(building->getHFlip() == false, building->getY());
  person->setX(building->getX() + 16.0 * (building->getHFlip() == false ? -1.0 : 1.0));
  building->occupant = person;
  addEnemy(person);
  return person;
}

void WreckLevel::createSkeleCatcher(WreckBuilding* leftBuilding, WreckBuilding* rightBuilding,
                                    Logical leftHasBall)
{
  WreckSkeleCatcher* leftSkele = new WreckSkeleCatcher(leftBuilding->getHFlip() == false,
                                                       leftBuilding->getY());
  leftBuilding->occupant = leftSkele;
  addEnemy(leftSkele);
  
  WreckSkeleCatcher* rightSkele = new WreckSkeleCatcher(rightBuilding->getHFlip() == false,
                                                        rightBuilding->getY());
  rightBuilding->occupant = rightSkele;
  addEnemy(rightSkele);
  
  leftHasBall ? leftSkele->catchBaseball() : rightSkele->catchBaseball();
}

WreckCreature* WreckLevel::createClimber(Point1 yPos, Logical onLeft)
{
  WreckCreature* creature = new WreckCreature();
  creature->setXY(onLeft ? emptyRange.x : emptyRange.y, yPos);
  addEnemy(creature);
  return creature;
}

WreckShocker* WreckLevel::createHLine(Point2 xSpacePercents, Point1 yPos)
{
  WreckShocker* shocks = new WreckShocker(Point2(RM::lerp(emptyRange, xSpacePercents.x), yPos),
                                          Point2(RM::lerp(emptyRange, xSpacePercents.y), yPos));
  addEnemy(shocks);
  return shocks;
}

WreckShocker* WreckLevel::createLine(Point2 pos1, Point2 pos2)
{
  WreckShocker* shocks = new WreckShocker(Point2(RM::lerp(emptyRange, pos1.x), pos1.y),
                                          Point2(RM::lerp(emptyRange, pos2.x), pos2.y));
  addEnemy(shocks);
  return shocks;
}

WreckPowerup* WreckLevel::createPowerOff(Point2 center)
{
  WreckPowerup* power = new WreckPowerup();
  power->setXY(center + currObjOffset);
  addAlly(power);
  return power;
}

WreckCoin* WreckLevel::createCoinOff(Point2 center)
{
  WreckCoin* wcoin = new WreckCoin();
  wcoin->setXY(center + currObjOffset);
  addAlly(wcoin);
  return wcoin;
}

WreckScreenShock* WreckLevel::createFullShock(Point1 yCoord)
{
  WreckScreenShock* shock = new WreckScreenShock();
  shock->setY(yCoord);
  addEnemy(shock);
  return shock;
}

WreckFloatSquare* WreckLevel::createFloater(Point2 center, Point1 rotation)
{
  WreckFloatSquare* floater = new WreckFloatSquare();
  floater->setXY(center);
  floater->setRotation(rotation);
  addEnemy(floater);
  return floater;
}

WreckBird* WreckLevel::createFlier(Point2 center)
{
  WreckBird* flier = new WreckBird();
  flier->setXY(center);
  addAlly(flier);
  return flier;
}

WreckCoin* WreckLevel::createCoin(Point2 xy, Coord1 val)
{
  WreckCoin* coin = new WreckCoin();
  coin->value = val;
  coin->setXY(xy);
  addAlly(coin);
  return coin;
}

WreckUnlockable* WreckLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet)
{
  if (canUnlockTopping(toppingID) == false) return NULL;
  
  WreckUnlockable* floater = new WreckUnlockable(toppingID);
  floater->setXY(center);
  addEnemy(floater, false);  // this puts it behind building
  if (startMagnet == true) floater->startMagnet();
  
  return floater;
}

void WreckLevel::arcLeftCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(96.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(96.0, 16.0) + offset);
  createCoin(Point2(48.0, 64.0) + offset);
  createCoin(Point2(16.0, 128.0) + offset);
  createCoin(Point2(16.0, 192.0) + offset);
  createCoin(Point2(48.0, 256.0) + offset);
  createCoin(Point2(96.0, 306.0) + offset);
}

void WreckLevel::arcRightCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(64.0, 64.0) + offset);
  createCoin(Point2(96.0, 128.0) + offset);
  createCoin(Point2(96.0, 192.0) + offset);
  createCoin(Point2(64.0, 256.0) + offset);
  createCoin(Point2(16.0, 306.0) + offset);
}

void WreckLevel::clumpCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(48.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(48.0, 16.0) + offset);
  createCoin(Point2(16.0, 32.0) + offset);
  createCoin(Point2(48.0, 48.0) + offset);
  createCoin(Point2(80.0, 32.0) + offset);
}

void WreckLevel::diagLeftCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(160.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(48.0, 16.0) + offset);
  createCoin(Point2(112.0, 64.0) + offset);
  createCoin(Point2(64.0, 112.0) + offset);
  createCoin(Point2(16.0, 160.0) + offset);
}

void WreckLevel::diagRightCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(64.0, 64.0) + offset);
  createCoin(Point2(112.0, 112.0) + offset);
  createCoin(Point2(160.0, 160.0) + offset);
}

void WreckLevel::diamondCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(64.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(64.0, 16.0) + offset);
  createCoin(Point2(16.0, 64.0) + offset);
  createCoin(Point2(64.0, 64.0) + offset);
  createCoin(Point2(112.0, 64.0) + offset);
  createCoin(Point2(64.0, 112.0) + offset);
}

void WreckLevel::bowCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) + Point2(-16.0, 0.0);
  createCoin(Point2(0.0, 0.0) + offset);
  createCoin(Point2(48.0, -16.0) + offset);
  createCoin(Point2(96.0, 0.0) + offset);
  createCoin(Point2(144.0, -16.0) + offset);
  createCoin(Point2(192.0, 0.0) + offset);
}

void WreckLevel::flatCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(80.0, 16.0) + offset);
  createCoin(Point2(144.0, 16.0) + offset);
  createCoin(Point2(208.0, 16.0) + offset);
}

void WreckLevel::ringCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(48.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(48.0, 16.0) + offset);
  createCoin(Point2(112.0, 16.0) + offset);
  createCoin(Point2(16.0, 48.0) + offset);
  createCoin(Point2(144.0, 48.0) + offset);
  createCoin(Point2(16.0, 112.0) + offset);
  createCoin(Point2(144.0, 112.0) + offset);
  createCoin(Point2(48.0, 144.0) + offset);
  createCoin(Point2(112.0, 144.0) + offset);
}

void WreckLevel::smallArcLeftCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(32.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(32.0, 16.0) + offset);
  createCoin(Point2(16.0, 48.0) + offset);
  createCoin(Point2(16.0, 80.0) + offset);
  createCoin(Point2(32.0, 112.0) + offset);
}

void WreckLevel::smallArcRightCoins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(32.0, 48.0) + offset);
  createCoin(Point2(32.0, 80.0) + offset);
  createCoin(Point2(16.0, 112.0) + offset);
}

void WreckLevel::straight3Coins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(16.0, 80.0) + offset);
  createCoin(Point2(16.0, 144.0) + offset);
}

void WreckLevel::straight6Coins(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) - Point2(16.0, 16.0) + Point2(-16.0, 0.0);
  createCoin(Point2(16.0, 16.0) + offset);
  createCoin(Point2(16.0, 80.0) + offset);
  createCoin(Point2(16.0, 144.0) + offset);
  createCoin(Point2(16.0, 208.0) + offset);
  createCoin(Point2(16.0, 272.0) + offset);
  createCoin(Point2(16.0, 336.0) + offset);
}

void WreckLevel::singleCoin(Point1 x, Point1 y)
{
  Point2 offset = Point2(x, y) + Point2(-16.0, 0.0);
  createCoin(offset);
}

void WreckLevel::addEnemy(WreckGO* enemy, Logical addLast)
{
  enemy->level = this;
  enemy->player = player;
  if (addLast == true) objman.addEnemy(enemy);
  else objman.enemyList.addFirstX(enemy);
  enemy->load();
}

void WreckLevel::addAlly(WreckGO* ally)
{
  ally->level = this;
  ally->player = player;
  objman.addAlly(ally);
  ally->load();
}

Point1 WreckLevel::getChainVel()
{
  return levelState == LEVEL_PLAY ? currMaxVel : 0.0;
}

Coord1 WreckLevel::cutAttack(const Circle& atkCircle)
{
  // offscreen, don't bother
  if (levelState != LEVEL_PLAY) return 0;
  if (atkCircle.AABB().collision(camera.myBox()) == false) return 0;
  
  Coord1 closestIndex = -1;
  Point1 closestVal = 10000.0;
  
  for (Coord1 i = 0; i < chain->links.count; ++i)
  {
    Circle linkCircle = chain->linkCircle(i);
    Point1 currDistance = RM::distance_to(linkCircle.xy, atkCircle.xy);
    
    if (linkCircle.collision(atkCircle) == true &&
        currDistance < closestVal)
    {
      closestVal = currDistance;
      closestIndex = i;
    }
  }
  
  if (player->collisionCircle().collision(atkCircle) == true &&
      RM::distance_to(player->getXY(), atkCircle.xy) < closestVal)
  {
    player->attacked(atkCircle.xy);
    return 1;
  }
  else if (closestIndex != -1)
  {
    loseLevel();
    RMPhysics->DestroyJoint(chain->joints[closestIndex]);
    chain->joints[closestIndex] = NULL;
    return 2;
  }
  
  return 0;
}

void WreckLevel::gotPowerup()
{
  powerTimer.reset();
}

Logical WreckLevel::powerActive()
{
  return powerTimer.getActive();
}

WreckGO* WreckLevel::objectAtPt(Coord1 objType, Point2 pt)
{
  for (LinkedListNode<WreckGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    WreckGO* enemy = enemyNode->data;
    
    if (enemy->lifeState == RM::ACTIVE &&
        enemy->type == objType &&
        Circle::collision(pt, enemy->collisionCircle()) == true)
    {
      return enemy;
    }
  }
  
  return NULL;
}

WreckGO* WreckLevel::buildingAtPt(Point2 pt)
{
  for (LinkedListNode<WreckGO*>* enemyNode = objman.enemyList.first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    WreckGO* enemy = enemyNode->data;
    
    if (enemy->lifeState == RM::ACTIVE &&
        enemy->type == WreckGO::BUILDING &&
        Box::collision(pt, enemy->collisionBox()) == true)
    {
      return enemy;
    }
  }
  
  return NULL;
}

void WreckLevel::smallShake()
{
  shaker.setMagnitude(3.0);
  shaker.timedShake(0.3);
}

void WreckLevel::bigShake()
{
  shaker.setMagnitude(6.0);
  shaker.timedShake(0.6);
}

void WreckLevel::addLocation(Point1 yCoord)
{
  yTriggers.add(yCoord);
}

void WreckLevel::checkLocations()
{
  if (currLocation >= yTriggers.count) return;
  
  if (player->getY() >= yTriggers[currLocation])
  {
    locationTrigger(currLocation);
    ++currLocation;
  }
}

void WreckLevel::buildToBottom()
{
  while (bottomY < camera.getY() + PLAY_H * 1.25 &&
         bottomLevel < numLevels)
  {
    buildWindowChunk();
  }
}

void WreckLevel::buildWindowChunk()
{
  for (Coord1 i = 0; i < LEVELS_PER_CHUNK; ++i)
  {
    if (bottomLevel == 0 && i == 0)
    {
      continue;
    }
    
    WreckBuilding* leftBuilding = createBuilding(true, i);
    buildingCache[i] = leftBuilding;
    
    WreckBuilding* rightBuilding = createBuilding(false, i);
    buildingCache[i + LEVELS_PER_CHUNK] = rightBuilding;
    
    // roofs on the top
    if (bottomLevel == 0 && i == 1)
    {
      Image* leftRoofImg = imgCache[wreckRoofLeft];
      Image* rightRoofImg = imgCache[wreckRoofRight];
      if (levelID % 2 == 1) std::swap(leftRoofImg, rightRoofImg);
      
      addAction(new Decoration(leftBuilding->getXY(), leftRoofImg));
      addAction(new Decoration(leftBuilding->getXY(), imgCache[wreckRoofPad]));
      
      Decoration* rightDec = new Decoration(rightBuilding->getXY(), rightRoofImg);
      rightDec->hflip();
      addAction(rightDec);
      
      Decoration* rightPad = new Decoration(rightBuilding->getXY(), imgCache[wreckRoofPad]);
      rightPad->hflip();
      addAction(rightPad);
    }
    // base on the bottom
    else if (bottomLevel == numLevels - LEVELS_PER_CHUNK && i == LEVELS_PER_CHUNK - 1)
    {
      leftBuilding->setImage(imgCache[wreckBottom]);
      leftBuilding->padImg = imgCache[wreckEmptyPad];
      rightBuilding->setImage(imgCache[wreckBottom]);
      rightBuilding->padImg = imgCache[wreckEmptyPad];
    }
  }
  
  bottomY += LEVELS_PER_CHUNK * buildingSize.y;
  buildNextChunk(bottomLevel / LEVELS_PER_CHUNK);
  bottomLevel += LEVELS_PER_CHUNK;
}

void WreckLevel::objectDefeated(WreckGO* obj)
{
  Pizza::currGameVars[VAR_DEFEATED]++;
}

void WreckLevel::buildingDefeated(WreckGO* obj)
{
  windowsDestroyed++;
  
  if (levelID == 0 && windowsDestroyed == 44)
  {
    Pizza::scoreMan->earnedAchievement(ACH_SHAVE);
  }
}

void WreckLevel::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
  }
}

void WreckLevel::callback(ActionEvent* caller)
{
  if (caller == &junkTimer)
  {
    WreckDebris* junk = new WreckDebris();
    junk->setX(randGen.randf(emptyRange.x + 32.0, emptyRange.y - 32.0));
    
    if (junkType == HARD_JUNK) junk->setSpiked();
    else if (junkType == BOTH_JUNK && randGen.randi(0, 1) == 0) junk->setSpiked();
    
    addEnemy(junk);
  }
  else if (caller == &sparkTimer)
  {
    addAction(new WreckSpark(player->getXY()));
    sparkTimer.reset(RM::randf(0.05, 0.1));
  }
}

// ======================= From b2ContactListener

void WreckLevel::BeginContact(b2Contact* contact)
{
  
}

void WreckLevel::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
  
}

void WreckLevel::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();
  
  if (userData1 == NULL || userData2 == NULL)
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }
  
  WreckGO* wgo1 = static_cast<WreckGO*>(userData1);
  WreckGO* wgo2 = static_cast<WreckGO*>(userData2);
  
  Point2 worldImpulse = impulse_to_wpt(impulse);
  Point1 finalImpulse = std::max(worldImpulse.x, worldImpulse.y);
  
  if (finalImpulse > 10.0)
  {
    wgo1->collidedPhysical(wgo2, finalImpulse);
    wgo2->collidedPhysical(wgo1, finalImpulse);
  }
}

void WreckLevel::EndContact(b2Contact* contact)
{
  
}

// ==================================== WRLevelLines

void WRLevelLines::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      smallArcRightCoins(334.0, 400.0);
      smallArcLeftCoins(176.0, 528.0);
      flatCoins(160.0, 960.0);
      arcRightCoins(272, 1072);
      arcLeftCoins(224, 1600);
      diagLeftCoins(320, 2048);
      diagRightCoins(174, 2240);
      clumpCoins(254, 2640);
      diamondCoins(256, 2766);
      flatCoins(158, 2896);
      break;
    case 1:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(1));
      createSkeleEasy(leftBuilding(2));
      break;
    case 2:
      createSkeleEasy(rightBuilding(0));
      createBoneThrower(leftBuilding(2));
      break;
    case 3:
      createSkeleEasy(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createSkeleEasy(rightBuilding(2));
      break;
    case 4:
      createSkeleEasy(leftBuilding(0));
      createHLine(Point2(0.6, 0.9), bottomY - chunkHeight * 0.5);
      break;
    case 5:
      createBoneThrower(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createSkeleEasy(leftBuilding(2));
      break;
    case 6:
      createSkeleEasy(rightBuilding(0));
      createHLine(Point2(0.1, 0.4), bottomY - chunkHeight * 0.5);
      createSkeleEasy(rightBuilding(2));
      break;
    case 7:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      break;
  }
}

void WRLevelLines::playerDamaged()
{
  if (alreadyBeaten() == false && tutShown == false)
  {
    startTutorial(new WreckJumpTutorial(this));
    tutShown = true;
  }
}

// ==================================== WRLevelCivilian

void WRLevelCivilian::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      singleCoin(256, 288);
      singleCoin(256, 354);
      diagLeftCoins(320, 448);
      straight3Coins(256, 704);
      diagRightCoins(224, 1120);
      arcRightCoins(256, 1632);
      clumpCoins(256, 2128);
      ringCoins(224, 2784);
      
      createPowerOff(Point2(256.0, 896.0));
      
      createPowerOff(Point2(368.0, 1344.0));
      
      createPowerOff(Point2(256.0, 2249.0));
      createPowerOff(Point2(256.0, 2328.0));
      createPowerOff(Point2(144.0, 2387.0));
      
      createPowerOff(Point2(368.0, 2508.0));
      createPowerOff(Point2(256.0, 2618.0));
      createPowerOff(Point2(256.0, 2710.0));
      
      createGirl(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 1:
      createSkeleEasy(rightBuilding(0));
      createBoneThrower(leftBuilding(1));
      createSkeleEasy(leftBuilding(2));
      createGirl(rightBuilding(2));
      break;
    case 2:
      createSkeleEasy(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createBoneThrower(leftBuilding(2));
      createGirl(rightBuilding(2));
      break;
    case 3:
      createGirl(leftBuilding(0));
      createBoneThrower(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      
      createHLine(Point2(0.1, 0.4), bottomY - buildingSize.y * 2.0);
      break;
    case 4:
      createGirl(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createGirl(rightBuilding(1));
      
      createHLine(Point2(0.1, 0.4), bottomY - buildingSize.y * 1.0);
      break;
    case 5:
      createBoneThrower(leftBuilding(0));
      createGirl(leftBuilding(1));
      break;
    case 6:
      createBoneThrower(leftBuilding(0));
      createBoneThrower(rightBuilding(1));
      createBoneThrower(leftBuilding(2));
      break;
    case 7:
      createBoneThrower(rightBuilding(0));
      break;
  }
}

// ==================================== WRLevelBaseball

void WRLevelBaseball::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      singleCoin(304, 224);
      singleCoin(352, 272);
      flatCoins(160, 720);
      arcRightCoins(304, 1008);
      arcLeftCoins(224, 1744);
      flatCoins(160, 2208);
      ringCoins(224, 2448);
      ringCoins(224, 2802);
      
      createSkeleEasy(rightBuilding(2));
      createHLine(Point2(0.1, 0.4), bottomY);
      break;
    case 1:
      createSkeleEasy(leftBuilding(0));
      createGirl(rightBuilding(0));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 2:
      createSkeleEasy(leftBuilding(0));
      createHLine(Point2(0.35, 0.65), bottomY - buildingSize.y * 2.0);
      createGirl(leftBuilding(1));
      createGirl(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 3:
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      
      createPowerOff(Point2(256.0, 1358.0));
      createHLine(Point2(0.1, 0.9), bottomY - buildingSize.y * 1.0);
      createPowerOff(Point2(160.0, 1456.0));
      createPowerOff(Point2(352.0, 1456.0));
      
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 4:
      createSkeleEasy(rightBuilding(0));
      createBoneThrower(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createSkeleEasy(leftBuilding(2));
      createGirl(rightBuilding(2));
      break;
    case 5:
      createSkeleEasy(rightBuilding(0));
      
      createPowerOff(Point2(256.0, 2080.0));
      createSkeleCatcher(leftBuilding(1), rightBuilding(1), true);
      
      createPowerOff(Point2(256.0, 2158.0));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      
      break;
    case 6:
      createSkeleEasy(leftBuilding(0));
      createHLine(Point2(0.6, 0.9), bottomY - buildingSize.y * 2.5);
      createGirl(leftBuilding(1));
      createGirl(rightBuilding(1));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 7:
      createSkeleEasy(leftBuilding(0));
      createGirl(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      break;
  }
}

// ==================================== WRLevelClimber

void WRLevelClimber::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      singleCoin(368, 592);
      singleCoin(142, 832);
      singleCoin(176, 912);
      straight3Coins(352, 1104);
      straight3Coins(256, 1360);
      smallArcLeftCoins(224, 1808);
      smallArcRightCoins(286, 1808);
      diagRightCoins(144, 1968);
      flatCoins(160, 2514);
      flatCoins(152, 2864);
      flatCoins(168, 2914);
      
      createClimber(bottomY, false);
      break;
    case 1:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(1));
      createGirl(leftBuilding(2));
      break;
    case 2:
      createSkeleEasy(leftBuilding(0));
      
      createBoneThrower(leftBuilding(1));
      createLine(Point2(0.1, bottomY - buildingSize.y * 2.0),
                 Point2(0.1, bottomY - buildingSize.y * 1.0));
      createPowerOff(Point2(172.0, 960.0));
      
      createGirl(rightBuilding(1));
      createSkeleEasy(rightBuilding(2));
      break;
    case 3:
      createGirl(leftBuilding(0));
      createClimber(bottomY - buildingSize.y * 2.5, true);
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      createGirl(rightBuilding(2));
      break;
    case 4:
      createSkeleCatcher(leftBuilding(0), rightBuilding(0), false);
      createClimber(bottomY - buildingSize.y * 0.5, true);
      createClimber(bottomY - buildingSize.y * 0.5, false);
      break;
    case 5:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      createGirl(leftBuilding(2));
      break;
    case 6:
      createHLine(Point2(0.1, 0.4), bottomY - buildingSize.y * 2.5);
      createBoneThrower(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createHLine(Point2(0.6, 0.9), bottomY);
      break;
    case 7:
      createPowerOff(Point2(256.0, 2752.0));
      createClimber(bottomY - buildingSize.y * 2.5, false);
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createGirl(rightBuilding(1));
      break;
  }
}

// ==================================== WRLevelJunk

void WRLevelJunk::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      singleCoin(256, 272);
      singleCoin(368, 336);
      singleCoin(144, 576);
      arcRightCoins(246, 592);
      singleCoin(208, 1392);
      singleCoin(304, 1392);
      singleCoin(208, 1504);
      singleCoin(304, 1506);
      diagLeftCoins(320, 1824);
      ringCoins(224, 2192);
      diamondCoins(256, 2576);
      singleCoin(256, 2896);
      
      junkTimer.setActive();
      
      createGirl(leftBuilding(2));
      createFloater(Point2(emptyRange.x + 80.0, bottomY - buildingSize.y * 0.5), HALF_PI * 0.5);
      createSkeleEasy(rightBuilding(2));
      break;
    case 1:
      createBoneThrower(leftBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createGirl(rightBuilding(1));
      createSkeleEasy(rightBuilding(2));
      createPowerOff(Point2(368.0, 736.0));
      createFloater(Point2(emptyRange.x + 48.0, bottomY), 0.0);
      break;
    case 2:
      createBoneThrower(leftBuilding(0));
      createGirl(rightBuilding(1));
      createSkeleEasy(leftBuilding(1));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 3:
      createGirl(rightBuilding(0));
      createBoneThrower(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createFloater(Point2(RM::lerp(emptyRange, 0.5),
                           bottomY - buildingSize.y * 0.5), HALF_PI * 0.5);
      createGirl(rightBuilding(2));
      break;
    case 4:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      
      createPowerOff(Point2(192.0, 1600.0));
      createLine(Point2(0.5, bottomY - buildingSize.y * 3.0),
                 Point2(0.5, bottomY - buildingSize.y * 2.0));
      createPowerOff(Point2(304.0, 1600.0));
      
      createSkeleEasy(leftBuilding(2));
      break;
    case 5:
      createSkeleEasy(leftBuilding(0));
      createGirl(rightBuilding(0));
      createClimber(bottomY - buildingSize.y * 2.5, false);
      createSkeleEasy(rightBuilding(1));
      createGirl(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 6:
      createSkeleEasy(leftBuilding(0));
      createFloater(Point2(RM::lerp(emptyRange, 0.75),
                           bottomY - buildingSize.y * 2.0), HALF_PI * 0.5);
      createGirl(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createBoneThrower(rightBuilding(2));
      break;
    case 7:
      createBoneThrower(leftBuilding(0));
      createFloater(Point2(RM::lerp(emptyRange, 0.25),
                           bottomY - buildingSize.y * 2.0), 0.0);
      createSkeleEasy(rightBuilding(1));
      break;
  }
}

// ==================================== WRLevelFlier

void WRLevelFlier::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      straight6Coins(256, 336);
      straight6Coins(256, 720);
      straight6Coins(256, 1104);
      
      singleCoin(256, 1488);
      singleCoin(256, 1552);
      singleCoin(256, 1616);
      singleCoin(256, 1680);
      
      straight6Coins(256, 1792);
      
      singleCoin(256, 2176);
      singleCoin(256, 2240);
      
      straight6Coins(256, 2352);
      straight3Coins(256, 2736);
      
      createFlier(Point2(emptyRange.x + 48.0, bottomY));
      break;
    case 1:
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createGirl(rightBuilding(1));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 2:
      createGirl(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createFloater(Point2(RM::lerp(emptyRange, 0.25),
                           bottomY - buildingSize.y * 1.5), 0.0);
      createPowerOff(Point2(352.0, 960.0));
      createFlier(Point2(emptyRange.x + 48.0, bottomY - buildingSize.y * 0.5));
      break;
    case 3:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createGirl(leftBuilding(1));
      createBoneThrower(rightBuilding(1));
      createClimber(bottomY - buildingSize.y * 0.5, false);
      break;
    case 4:
      createSkeleEasy(leftBuilding(0));
      createBoneThrower(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createPowerOff(Point2(256.0, 1736.0));
      createFloater(Point2(emptyRange.y - 40.0,
                           bottomY - buildingSize.y * 0.5), 0.0);
      createFlier(Point2(emptyRange.x + 48.0, bottomY - buildingSize.y * 1.0));
      createSkeleEasy(rightBuilding(2));
      break;
    case 5:
      createGirl(rightBuilding(0));
      createSkeleCatcher(leftBuilding(1), rightBuilding(1), true);
      createSkeleEasy(leftBuilding(2));
      createHLine(Point2(0.1, 0.4), bottomY);
      break;
    case 6:
      createGirl(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createFlier(Point2(emptyRange.x + 32.0, bottomY - buildingSize.y * 0.5));
      createFlier(Point2(emptyRange.y - 32.0, bottomY - buildingSize.y * 0.5));
      break;
    case 7:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      break;
  }
}

// ==================================== WRLevelHardwall

void WRLevelHardwall::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      clumpCoins(174, 320);
      clumpCoins(288, 560);
      clumpCoins(192, 816);
      clumpCoins(320, 1184);
      clumpCoins(224, 1584);
      clumpCoins(304, 1696);
      clumpCoins(336, 1936);
      clumpCoins(192, 2048);
      clumpCoins(320, 2480);
      clumpCoins(208, 2674);
      clumpCoins(336, 2864);
      
      leftBuilding(2)->setHardWindow();
      break;
    case 1:
      createSkeleEasy(leftBuilding(0));
      rightBuilding(0)->setHardWindow();
      createSkeleEasy(rightBuilding(0));
      createBoneThrower(leftBuilding(2));
      createGirl(rightBuilding(2));
      break;
    case 2:
      createSkeleEasy(rightBuilding(0));
      createClimber(bottomY - buildingSize.y * 2.5, false);
      leftBuilding(1)->setSpikeWindow();
      createHLine(Point2(0.6, 0.9), bottomY - buildingSize.y * 2.0);
      createSkeleEasy(leftBuilding(2));
      break;
    case 3:
      createPowerOff(Point2(256.0, 1152.0));
      createPowerOff(Point2(192.0, 1216.0));
      createPowerOff(Point2(352.0, 1280.0));
      
      leftBuilding(0)->setHardWindow();
      rightBuilding(0)->setHardWindow();
      createSkeleCatcher(leftBuilding(0), rightBuilding(0), true);
      createBoneThrower(leftBuilding(1));
      // createFloater(Point2(emptyRange.x + 40.0, bottomY - buildingSize.y * 1.5), 0.0);
      createGirl(rightBuilding(2));
      createFlier(Point2(emptyRange.y - 32.0, bottomY));
      addLocation(bottomY);
      break;
    case 4:
      leftBuilding(0)->setHardWindow();
      leftBuilding(1)->setSpikeWindow();
      createClimber(bottomY - buildingSize.y * 1.0, true);
      createBoneThrower(rightBuilding(1));
      createGirl(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 5:
      createPowerOff(Point2(192.0, 2208.0));
      createSkeleEasy(rightBuilding(0));
      createSkeleCatcher(leftBuilding(1), rightBuilding(1), false);
      rightBuilding(1)->setHardWindow();
      createBoneThrower(rightBuilding(2));
      createFloater(Point2(RM::lerp(emptyRange, 0.75), bottomY), 0.0);
      break;
    case 6:
      createSkeleEasy(leftBuilding(0));
      rightBuilding(1)->setSpikeWindow();
      rightBuilding(2)->setHardWindow();
      createGirl(rightBuilding(2));
      createClimber(bottomY, true);
      createFlier(Point2(emptyRange.y - 32.0, bottomY));
      break;
    case 7:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(leftBuilding(1));
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      break;
  }
}

void WRLevelHardwall::locationTrigger(Coord1 triggerNum)
{
  junkTimer.setActive();
}

// ==================================== WRLevelUFO

void WRLevelUFO::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      smallArcLeftCoins(160, 272);
      smallArcRightCoins(352, 848);
      diamondCoins(256, 1472);
      diamondCoins(256, 2128);
      clumpCoins(320, 2864);
      
      createSkeleEasy(leftBuilding(2));
      break;
    case 1:
      createGirl(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createFullShock(bottomY - buildingSize.y * 2.0);
      createSkeleEasy(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 2:
      createBoneThrower(leftBuilding(1));
      createBoneThrower(rightBuilding(1));
      createFullShock(bottomY);
      break;
    case 3:
      createSkeleEasy(leftBuilding(0));
      createGirl(rightBuilding(0));
      createSkeleEasy(leftBuilding(1));
      rightBuilding(1)->setSpikeWindow();
      createHLine(Point2(0.6, 0.9), bottomY - buildingSize.y * 1.0);
      break;
    case 4:
      createSkeleCatcher(leftBuilding(0), rightBuilding(0), false);
      createSkeleEasy(leftBuilding(1));
      createFullShock(bottomY - buildingSize.y * 1.5);
      createClimber(bottomY - buildingSize.y * 2.0, false);
      createPowerOff(Point2(256.0, 1664.0));
      createPowerOff(Point2(256.0, 1776.0));
      leftBuilding(2)->setHardWindow();
      break;
    case 5:
      createBoneThrower(leftBuilding(1));
      rightBuilding(1)->setSpikeWindow();
      createGirl(leftBuilding(2));
      createBoneThrower(rightBuilding(2));
      break;
    case 6:
      createBoneThrower(leftBuilding(0));
      rightBuilding(0)->setSpikeWindow();
      createFullShock(bottomY - buildingSize.y * 2.0);
      rightBuilding(1)->setHardWindow();
      createSkeleEasy(rightBuilding(1));
      leftBuilding(2)->setHardWindow();
      createClimber(bottomY - buildingSize.y * 0.5, false);
      break;
    case 7:
      createClimber(bottomY - buildingSize.y * 2.5, true);
      createSkeleEasy(rightBuilding(0));
      createGirl(leftBuilding(1));
      createBoneThrower(rightBuilding(1));
      break;
  }
}

// ==================================== WRLevelCutter

void WRLevelCutter::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      diagLeftCoins(288, 304);
      diagRightCoins(144, 816);
      straight6Coins(256, 1632);
      ringCoins(176, 2128);
      ringCoins(224, 2352);
      ringCoins(272, 2562);
      diamondCoins(192, 2768);
      
      createSkeleEasy(leftBuilding(2));
      createCutter(rightBuilding(2));
      break;
    case 1:
      createSkeleEasy(leftBuilding(0));
      createClimber(bottomY - buildingSize.y * 2.0, false);
      rightBuilding(0)->setHardWindow();
      leftBuilding(1)->setSpikeWindow();
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), false);
      break;
    case 2:
      createBoneThrower(leftBuilding(0));
      createCutter(rightBuilding(0));
      leftBuilding(1)->setHardWindow();
      createSkeleEasy(leftBuilding(1));
      createFlier(Point2(emptyRange.y - 32.0, 1018.0));
      createGirl(rightBuilding(2));
      createPowerOff(Point2(256.0, 1152.0));
      break;
    case 3:
      rightBuilding(0)->setHardWindow();
      createCutter(rightBuilding(0));
      createBoneThrower(leftBuilding(1));
      createFullShock(bottomY - buildingSize.y * 1.5);
      createBoneThrower(rightBuilding(2));
      break;
    case 4:
      createCutter(leftBuilding(2));
      createGirl(rightBuilding(2));
      break;
    case 5:
      leftBuilding(0)->setHardWindow();
      createBoneThrower(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createSkeleCatcher(leftBuilding(2), rightBuilding(2), true);
      break;
    case 6:
      createPowerOff(Point2(256.0, 2416.0));
      leftBuilding(1)->setHardWindow();
      createCutter(leftBuilding(1));
      createCutter(rightBuilding(1));
      rightBuilding(2)->setSpikeWindow();
      break;
    case 7:
      createSkeleEasy(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createClimber(bottomY - buildingSize.y * 2.5, false);
      createSkeleEasy(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      break;
  }
}

// ==================================== WRLevelMix

void WRLevelMix::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      flatCoins(160, 242);
      arcRightCoins(224, 354);
      flatCoins(160, 784);
      flatCoins(160, 928);
      ringCoins(272, 1488);
      ringCoins(240, 2384);
      flatCoins(160, 2672);
      ringCoins(224, 2736);
      clumpCoins(256, 2782);
      
      leftBuilding(1)->setHardWindow();
      createGirl(leftBuilding(1));
      rightBuilding(1)->setHardWindow();
      createGirl(rightBuilding(1));
      leftBuilding(2)->setHardWindow();
      createSkeleEasy(leftBuilding(2));
      createSkeleEasy(rightBuilding(2));
      break;
    case 1:
      rightBuilding(0)->setHardWindow();
      createPowerOff(Point2(352.0, 496.0));
      createSkeleCatcher(leftBuilding(1), rightBuilding(1), true);
      createCutter(leftBuilding(2));
      rightBuilding(2)->setSpikeWindow();
      break;
    case 2:
      createBoneThrower(rightBuilding(0));
      leftBuilding(1)->setHardWindow();
      createCutter(leftBuilding(1));
      createSkeleEasy(rightBuilding(1));
      createPowerOff(Point2(256.0, 1136.0));
      leftBuilding(2)->setSpikeWindow();
      createGirl(rightBuilding(2));
      tryCreateUnlockable(Point2(64.0 - 16.0, 1102.0), Coord2(4, 11));
      break;
    case 3:
      createGirl(leftBuilding(0));
      createFlier(Point2(emptyRange.x + 40.0, bottomY - buildingSize.y * 2.0));
      createFlier(Point2(RM::lerp(emptyRange, 0.5), bottomY - buildingSize.y * 2.0));
      createFlier(Point2(emptyRange.y - 40.0, bottomY - buildingSize.y * 2.0));
      createCutter(rightBuilding(1));
      createHLine(Point2(0.35, 0.65), bottomY - buildingSize.y);
      leftBuilding(2)->setHardWindow();
      createSkeleEasy(leftBuilding(2));
      break;
    case 4:
      rightBuilding(0)->setSpikeWindow();
      createSkeleCatcher(leftBuilding(1), rightBuilding(1), false);
      createFullShock(bottomY - buildingSize.y * 0.5);
      break;
    case 5:
      leftBuilding(0)->setHardWindow();
      createBoneThrower(leftBuilding(0));
      createBoneThrower(rightBuilding(0));
      createSkeleEasy(rightBuilding(2));
      createClimber(bottomY, false);
      addLocation(bottomY);
      break;
    case 6:
      createCutter(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createCutter(leftBuilding(2));
      createCutter(rightBuilding(2));
      break;
    case 7:
      leftBuilding(0)->setSpikeWindow();
      rightBuilding(0)->setSpikeWindow();
      createGirl(leftBuilding(1));
      createGirl(rightBuilding(1));
      break;
  }
}

void WRLevelMix::locationTrigger(Coord1 triggerNum)
{
  junkType = HARD_JUNK;
  junkTimer.setActive();
}

// ==================================== WRLevelBatter

void WRLevelBatter::buildNextChunk(Coord1 chunkNum)
{
  switch (chunkNum)
  {
    case 0:
      diagRightCoins(144, 592);
      flatCoins(160, 848);
      straight3Coins(256, 1072);
      singleCoin(256, 1328);
      singleCoin(176, 1360);
      arcRightCoins(288, 1552);
      diagRightCoins(208, 2240);
      flatCoins(160, 2592);
      clumpCoins(336, 2736);
      bowCoins(160, 2894);
      bowCoins(160, 2960);
      
      createSkeleEasy(leftBuilding(2));
      break;
    case 1:
      createBatter(leftBuilding(0));
      createSkeleEasy(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      leftBuilding(2)->setSpikeWindow();
      break;
    case 2:
      createSkeleCatcher(leftBuilding(0), rightBuilding(0), true);
      createClimber(bottomY - buildingSize.y, false);
      createGirl(leftBuilding(2));
      rightBuilding(2)->setHardWindow();
      createGirl(rightBuilding(2));
      break;
    case 3:
      createClimber(bottomY - buildingSize.y * 3.0, false);
      createPowerOff(Point2(256.0, bottomY - buildingSize.y * 2.0));
      leftBuilding(1)->setHardWindow();
      createBatter(leftBuilding(1));
      createPowerOff(Point2(256.0, bottomY - buildingSize.y * 1.0));
      createSkeleEasy(leftBuilding(2));
      createCutter(rightBuilding(2));
      break;
    case 4:
      createSkeleCatcher(leftBuilding(0), rightBuilding(0), false);
      createSkeleEasy(rightBuilding(2));
      createFullShock(bottomY);
      break;
    case 5:
      leftBuilding(0)->setHardWindow();
      createSkeleEasy(leftBuilding(0));
      createBatter(rightBuilding(0));
      createSkeleEasy(rightBuilding(1));
      break;
    case 6:
      createPowerOff(Point2(368.0, bottomY - buildingSize.y * 2.0));
      leftBuilding(1)->setSpikeWindow();
      createCutter(leftBuilding(1));
      createBatter(rightBuilding(1));
      createPowerOff(Point2(368.0, bottomY - buildingSize.y * 1.0));
      leftBuilding(2)->setSpikeWindow();
      createBatter(leftBuilding(2));
      break;
    case 7:
      createSkeleEasy(rightBuilding(0));
      createGirl(leftBuilding(0));
      createGirl(leftBuilding(1));
      break;
  }
}

// =============================== PlaneBG ============================ //

PlaneBG::PlaneBG() :
  StdBackground(),
  layers(5, true)
{
  
}

void PlaneBG::load()
{
  addImgNorm(bgLayerSet, "plane_sky", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "plane_farclouds", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "plane_closeclouds", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "plane_farmountains", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "plane_closemountains", Point2(0.0, 1.0));
}

void PlaneBG::init()
{
  Point1 masterSpeed = -512.0;
  
  Coord1 i = 0;
  layers[i].init(imgsetCache[bgLayerSet][i],
                 Point2(masterSpeed * 0.15, 0.0), Point2(Pizza::platformTL.x, 240.0),
                 Point2(SCREEN_W, SCREEN_H));
  
  i++;
  layers[i].init(imgsetCache[bgLayerSet][i],
                 Point2(masterSpeed * 0.25, 0.0), Point2(Pizza::platformTL.x, 272.0),
                 Point2(SCREEN_W, SCREEN_H));
  
  i++;
  layers[i].init(imgsetCache[bgLayerSet][i],
                 Point2(masterSpeed * 0.5, 0.0), Point2(Pizza::platformTL.x, 320.0),
                 Point2(SCREEN_W, SCREEN_H));
  
  i++;
  layers[i].init(imgsetCache[bgLayerSet][i],
                 Point2(masterSpeed * 0.75, 0.0), Point2(Pizza::platformTL.x, 352.0),
                 Point2(SCREEN_W, SCREEN_H));
  
  i++;
  layers[i].init(imgsetCache[bgLayerSet][i],
                 Point2(masterSpeed * 1.0, 0.0), Point2(Pizza::platformTL.x, 352.0),
                 Point2(SCREEN_W, SCREEN_H));
}

void PlaneBG::update()
{
  for (Coord1 i = 0; i < layers.count; ++i) layers[i].update();
}

void PlaneBG::drawBackground()
{
  for (Coord1 i = 0; i < layers.count; ++i) layers[i].redraw();
}

// ==================================== PlaneLevel ================================ //

PlaneLevel::PlaneLevel() :
  PizzaLevelInterface(),
  ActionListener(),
  b2ContactListener(),

  objman(),
  player(NULL),
  bg(),

  debrisList(),
  spawnQueue(this),

  wavePercents(NUM_PLENEMIES, true),
  enemiesPerGroup(NUM_PLENEMIES, true),
  groupsPerWave(1, 1),

  lastWave(9),
  currWave(-1)
{
  randGen.reset(0x24ed517f);
  musicFilename = "skyfighter.ima4";
  
  wavePercents.set_all(0.0);
  enemiesPerGroup.set_all(Coord2(1, 1));
}

PlaneLevel::~PlaneLevel()
{
  delete player;
}

void PlaneLevel::preload()
{
  
}

void PlaneLevel::loadRes()
{
  bg.load();
  
  loadImgAbs(planeFront, "plane_front", Point2(52.0, 32.0));
  loadWarp(planeWarp, "planefront.dwarp2");
  loadTilesAbs(planePropSet, "plane_prop", 2, 1, Point2(-24.0, 12.0));
  loadTilesAbs(planeMuzzleSet, "plane_muzzleflash", 3, 1, Point2(-20.0, 20.0));
  loadImgAbs(planeBullet, "plane_bullet", Point2(44.0, 8.0));
  
  loadTilesNorm(bombExplodeSet, "explosion", 3, 1, HANDLE_C, true);
  
  GladiatorLevel::loadEnemy(GladiatorLevel::ENEMY_CUPID);
  GladiatorLevel::loadEnemy(GladiatorLevel::ENEMY_FLAMESKULL);
  GladiatorLevel::loadEnemy(GladiatorLevel::ENEMY_GARGOYLE);
  loadTilesAbs(brainFly, "brain_fly", 3, 1, Point2(136.0, 73.0));
  GladiatorLevel::loadEnemy(GladiatorLevel::ENEMY_WISP);
  loadTilesAbs(rabbitSet, "rabbit", 5, 1, Point2(83.0, 59.0));
  GladiatorLevel::loadEnemy(GladiatorLevel::ENEMY_NAGA);
  
  imgCache[wreckNode] = Image::create_centered(imgStr("wreck_node"));
  
  imgsetCache[wreckElectricSet].clear();
  imgsetCache[wreckElectricSet].ensure_capacity(4);
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_0"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_1"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_2"), Point2(0.0, 0.5)));
  imgsetCache[wreckElectricSet].add(Image::create_handled_norm(imgStr("wreck_electric_3"), Point2(0.0, 0.5)));
  for (Coord1 i = 0; i < imgsetCache[wreckElectricSet].count; ++i)
  {
    // imgsetCache[wreckElectricSet][i]->overexpose();
    imgsetCache[wreckElectricSet][i]->repeats();
  }
  
  for (Char c = '1'; c <= '4'; ++c)
  {
    bulletSet.add(Sound::create(String1("bullet$.wav", '$', c)));
  }
  
  loadSound(bulletHit, "enemydamage.wav");
  loadSound(roboFlameSound, "flame.wav");
}

void PlaneLevel::unloadRes()
{
  free_clear(bulletSet);
}

void PlaneLevel::load()
{
  RMPhysics = new RivermanPhysics(Point2(0.0, 0.0), true);
  RMPhysics->SetContactListener(this);
  
  bg.init();
  initScoreText();
  initHealth();
  
  player = new PlanePlayer();
  player->level = this;
  player->setXY(camBox().left() + 64.0, PLAY_CY);
  player->load();
  
  spawnQueue.wait(1.0);
}


void PlaneLevel::update()
{
  if (tutorial->shouldUpdateGame() == true) updateGame();
  
  updateTutorials();
}

void PlaneLevel::redraw()
{
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  
  // ========== camera on
  RMGraphics->push_camera(CameraStd());
  
  bg.drawBackground();
  camera.apply();
  
  debrisList.redraw();
  objman.drawObjects();
  objman.actions.redraw();
  player->redraw();
  RMPhysics->draw_debug();
  
  RMGraphics->pop_camera();
  // ========== camera off
  
  RMGraphics->pop_camera();
  
  drawHealthBar();
  scoreText.drawText();
  tutorial->redraw();
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
  player->update();
  
  waveGenerator();
  
  shaker.update();
  updateCamera();
  
  if (levelState == LEVEL_PLAY)
  {
    Pizza::runGameTime(RM::timePassed());
    updateStatus();
  }
  
  updateHealthBar();
  bg.update();
  
  RM::pop_time_mult();
  // ==========
  
  objman.frontActions.update();
}

void PlaneLevel::waveGenerator()
{
  if (levelState != LEVEL_PLAY) return;
  
  spawnQueue.update();
  
  if (objman.enemyList.count == 0 &&
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
    queueGroup((PlaneType) type, groupSize);
  }
}

void PlaneLevel::enemyDefeated(PlaneGO* enemy)
{
  if (enemy->basePoints == 0) return;
  
  Pizza::currGameVars[VAR_DEFEATED]++;
  
  gotScore(enemy->basePoints);
}

void PlaneLevel::queueGroup(PlaneType type, Coord1 groupSize)
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
    if (Pizza::controller->clickStatus(PRESSED) == true)
    {
      player->tryFire();
      player->tryMoveToPoint(Pizza::controller->getXY().y);
    }
    else
    {
      player->tryMoveDirection(PizzaLevel::tiltMagnitudeVert());
      
      if (Pizza::controller->getConfirmButtonStatus(PRESSED) == true)
      {
        player->tryFire();
      }
    }
  }
  // mac
  else if (RMSystem->get_OS_family() == MAC_FAMILY)
  {
    if (Pizza::controller->clickStatus(PRESSED) == true)
    {
      player->tryFire();
      player->tryMoveToPoint(Pizza::controller->getXY().y);
    }
    else
    {
      player->tryMoveDirection(PizzaLevel::tiltMagnitudeVert());

      if (Pizza::controller->getConfirmButtonStatus(PRESSED) == true)
      {
        player->tryFire();
      }
    }
  }
  // apple tv
  else
  {
    player->tryMoveDirection(PizzaLevel::tiltMagnitudeVert());
    
    if (Pizza::controller->getConfirmButtonStatus(PRESSED) == true)
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

PlaneUnlockable* PlaneLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet)
{
  if (canUnlockTopping(toppingID) == false) return NULL;
  
  PlaneUnlockable* floater = new PlaneUnlockable(toppingID);
  floater->setXY(center);
  addAlly(floater);
  
  if (startMagnet == true) floater->startMagnet();
  
  return floater;
}

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

void PlaneLevel::addEnemy(PlaneGO* enemy, Logical addLast)
{
  enemy->level = this;
  enemy->player = player;
  if (addLast == true) objman.addEnemy(enemy);
  else objman.enemyList.addFirstX(enemy);
  enemy->load();
}

void PlaneLevel::addAlly(PlaneGO* ally)
{
  ally->level = this;
  ally->player = player;
  objman.addAlly(ally);
  ally->load();
}

void PlaneLevel::callback(ActionEvent* caller)
{
  
}

// ==================================== CreatePLGroupCMD

CreatePLGroupCMD::CreatePLGroupCMD(PlaneLevel* setLevel, PlaneLevel::PlaneType setType,
                                   Coord1 setGroupSize) :
Command(),
level(setLevel),
type(setType),
groupSize(setGroupSize)
{
  
}

void CreatePLGroupCMD::execute()
{
  if (type == PlaneLevel::PLENEMY_FLAMESKULL)
  {
    Point1 yBuffer = 64.0;
    Point2 firstXY = Point2(PLAY_W + 48.0, RM::randf(yBuffer, level->camBox().bottom() - yBuffer));
    
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
      level->spawnQueue.enqueueX(new CreatePLEnemyCMD(level, type));
      level->spawnQueue.wait(0.75);
    }
  }
}

// ==================================== CreatePLEnemyCMD

CreatePLEnemyCMD::CreatePLEnemyCMD(PlaneLevel* setLevel, PlaneLevel::PlaneType setType,
                                   Point2 setXY) :
level(setLevel),
type(setType),
xy(setXY)
{
  
}

void CreatePLEnemyCMD::execute()
{
  PlaneGO* pgo = NULL;
  Box screenBounds = level->camBox();
  
  switch (type)
  {
    default:
    case PlaneLevel::PLENEMY_FLAMESKULL:
      pgo = new PlaneFlameskull();
      pgo->setXY(xy);
      break;
    case PlaneLevel::PLENEMY_CUPID:
      pgo = new PlaneCupid();
      pgo->setX(screenBounds.right() - 32.0);
      pgo->setY(RM::randl() ? screenBounds.top() - 48.0 : screenBounds.bottom() + 48.0);
      break;
    case PlaneLevel::PLENEMY_DEMON:
      pgo = new PlaneDemon();
      pgo->setX(RM::lerp(screenBounds.norm_pos_x(0.75), screenBounds.norm_pos_x(1.25), RM::randf()));
      pgo->setY(screenBounds.top() - 32.0);
      break;
    case PlaneLevel::PLENEMY_BRAIN:
      pgo = new PlaneBrain();
      pgo->setX(screenBounds.right());
      pgo->setY(RM::lerp(screenBounds.top() + 100.0, screenBounds.bottom() - 100.0, RM::randf()));
      break;
    case PlaneLevel::PLENEMY_WISP:
      pgo = new PlaneWisp();
      pgo->setX(screenBounds.right() + 96.0);
      pgo->setY(RM::lerp(screenBounds.top() + 32.0, screenBounds.bottom() - 32.0, RM::randf()));
      break;
    case PlaneLevel::PLENEMY_RABBIT:
      pgo = new PlaneRabbit();
      pgo->setX(RM::lerp(screenBounds.norm_pos_x(0.75), screenBounds.norm_pos_x(1.25), RM::randf()));
      pgo->setY(screenBounds.bottom() + 32.0);
      break;
    case PlaneLevel::PLENEMY_NAGA:
      pgo = new PlaneNaga();
      pgo->setX(screenBounds.right() + 32.0);
      pgo->setY(RM::lerp(screenBounds.top() + 64.0, screenBounds.bottom() - 64.0, RM::randf()));
      break;
    case PlaneLevel::PLENEMY_NODES:
    {
      PlaneNodes* nodes = new PlaneNodes();
      Point2 nodeBounds(screenBounds.top() + 64.0,
                        screenBounds.bottom() - 64.0 - PlaneNodes::MIN_BEAM_HEIGHT);
      Point1 beamTop = RM::randf(nodeBounds);
      nodes->setX(screenBounds.right() + 32.0);
      nodes->setY(beamTop);
      nodes->beamHeight = RM::randf(PlaneNodes::MIN_BEAM_HEIGHT,
                                    nodeBounds.y + PlaneNodes::MIN_BEAM_HEIGHT - beamTop);
      pgo = nodes;
      break;
    }
  }
  
  level->addEnemy(pgo);
}

// ==================================== PLLevelCupid

void PLLevelCupid::createWave()
{
  switch (currWave)
  {
    case 0:
      lastWave = 15;
      
      queueGroup(PLENEMY_FLAMESKULL, 3);
      break;
    case 1:
      queueGroup(PLENEMY_FLAMESKULL, 3);
      queueGroup(PLENEMY_FLAMESKULL, 3);
      break;
    case 2:
      queueGroup(PLENEMY_FLAMESKULL, 3);
      queueGroup(PLENEMY_FLAMESKULL, 5);
      break;
    case 3:
      queueGroup(PLENEMY_DEMON, 1);
      break;
    case 4:
      queueGroup(PLENEMY_DEMON, 2);
      break;
    case 5:
      groupsPerWave.set(2, 2);
      
      wavePercents[PLENEMY_FLAMESKULL] = 1.0;
      enemiesPerGroup[PLENEMY_FLAMESKULL].set(3, 5);
      
      wavePercents[PLENEMY_DEMON] = 1.0;
      enemiesPerGroup[PLENEMY_DEMON].set(1, 3);
      
      createRandomWave();
      break;
    case 10:
      queueGroup(PLENEMY_CUPID, 1);
      break;
    case 11:
      queueGroup(PLENEMY_CUPID, 2);
      break;
    case 12:
      groupsPerWave.set(3, 3);
      wavePercents[PLENEMY_CUPID] = 1.0;
      enemiesPerGroup[PLENEMY_CUPID].set(1, 3);
      enemiesPerGroup[PLENEMY_DEMON].set(2, 3);
      createRandomWave();
      break;
    case 15:
      queueGroup(PLENEMY_CUPID, 2);
      queueGroup(PLENEMY_FLAMESKULL, 5);
      queueGroup(PLENEMY_DEMON, 2);
      break;
    default:
      createRandomWave();
      break;
  }
}

// ==================================== PLLevelWisp

void PLLevelWisp::createWave()
{
  switch (currWave)
  {
    case 0:
      queueGroup(PLENEMY_BRAIN, 1);
      break;
    case 1:
      queueGroup(PLENEMY_BRAIN, 2);
      queueGroup(PLENEMY_FLAMESKULL, 5);
      break;
    case 2:
      queueGroup(PLENEMY_CUPID, 2);
      queueGroup(PLENEMY_DEMON, 3);
      queueGroup(PLENEMY_CUPID, 2);
      break;
    case 3:
      queueGroup(PLENEMY_FLAMESKULL, 5);
      queueGroup(PLENEMY_BRAIN, 2);
      queueGroup(PLENEMY_DEMON, 2);
      break;
    case 4:
      queueGroup(PLENEMY_WISP, 1);
      break;
    case 5:
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_DEMON, 2);
      break;
    case 6:
      queueGroup(PLENEMY_CUPID, 2);
      queueGroup(PLENEMY_BRAIN, 3);
      break;
    case 7:
      queueGroup(PLENEMY_WISP, 2);
      queueGroup(PLENEMY_FLAMESKULL, 5);
      queueGroup(PLENEMY_FLAMESKULL, 5);
      queueGroup(PLENEMY_WISP, 1);
      break;
    case 8:
      queueGroup(PLENEMY_DEMON, 7);
      break;
    case 9:
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_BRAIN, 1);
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_DEMON, 2);
      queueGroup(PLENEMY_WISP, 1);
      break;
  }
}

// ==================================== PLLevelNaga

void PLLevelNaga::createWave()
{
  switch (currWave)
  {
    case 0:
      queueGroup(PLENEMY_RABBIT, 1);
      break;
    case 1:
      queueGroup(PLENEMY_RABBIT, 2);
      queueGroup(PLENEMY_CUPID, 1);
      break;
    case 2:
      queueGroup(PLENEMY_BRAIN, 1);
      queueGroup(PLENEMY_DEMON, 2);
      queueGroup(PLENEMY_WISP, 1);
      break;
    case 3:
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_FLAMESKULL, 3);
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_FLAMESKULL, 3);
      break;
    case 4:
      queueGroup(PLENEMY_NAGA, 1);
      break;
    case 5:
      queueGroup(PLENEMY_NAGA, 2);
      break;
    case 6:
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_WISP, 2);
      queueGroup(PLENEMY_RABBIT, 1);
      break;
    case 7:
      queueGroup(PLENEMY_BRAIN, 2);
      queueGroup(PLENEMY_NAGA, 2);
      tryCreateUnlockable(camBox().norm_pos(Point2(1.0, 1.0)) + Point2(64.0, -32.0),
                          Coord2(CLOTHES_LAYER, 12));
      break;
    case 8:
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_DEMON, 1);
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_DEMON, 1);
      queueGroup(PLENEMY_RABBIT, 1);
      queueGroup(PLENEMY_DEMON, 1);
      queueGroup(PLENEMY_RABBIT, 1);
      break;
    case 9:
      queueGroup(PLENEMY_NAGA, 1);
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_CUPID, 1);
      queueGroup(PLENEMY_NAGA, 1);
      break;
  }
}

// ==================================== PLLevelNode

void PLLevelNode::createWave()
{
  switch (currWave)
  {
    case 0:
      queueGroup(PLENEMY_NODES, 1);
      break;
    case 1:
      queueGroup(PLENEMY_FLAMESKULL, 3);
      queueGroup(PLENEMY_NODES, 1);
      break;
    case 2:
      queueGroup(PLENEMY_CUPID, 2);
      queueGroup(PLENEMY_BRAIN, 2);
      queueGroup(PLENEMY_FLAMESKULL, 3);
      break;
    case 3:
      queueGroup(PLENEMY_NODES, 2);
      queueGroup(PLENEMY_DEMON, 2);
      queueGroup(PLENEMY_NAGA, 1);
      break;
    case 4:
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_NODES, 2);
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_NODES, 2);
      break;
    case 5:
      queueGroup(PLENEMY_BRAIN, 3);
      queueGroup(PLENEMY_FLAMESKULL, 2);
      queueGroup(PLENEMY_BRAIN, 3);
      break;
    case 6:
      queueGroup(PLENEMY_NAGA, 3);
      break;
    case 7:
      queueGroup(PLENEMY_NODES, 5);
      break;
    case 8:
      queueGroup(PLENEMY_BRAIN, 3);
      queueGroup(PLENEMY_RABBIT, 3);
      break;
    case 9:
      queueGroup(PLENEMY_CUPID, 1);
      queueGroup(PLENEMY_NODES, 2);
      queueGroup(PLENEMY_WISP, 1);
      queueGroup(PLENEMY_NODES, 2);
      queueGroup(PLENEMY_NAGA, 1);
      queueGroup(PLENEMY_NODES, 2);
      break;
  }
}

// ==================================== TimingLevel ================================ //

TimingBG::TimingBG(TimingLevel* setLevel) :
level(setLevel)
{
  
}

void TimingBG::load()
{
  addImgNorm(bgLayerSet, "timing_sky", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "timing_bldgs_a", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "timing_bldgs_b", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "timing_bldgs_c", Point2(0.0, 1.0));
  addImgNorm(bgLayerSet, "timing_bldgs_d", Point2(0.0, 1.0));
}

void TimingBG::drawBackground()
{
  // sky
  PizzaBackground::drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][0], 0.05, 208.0, level->camera);
  
  // buildings
  PizzaBackground::drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][1], 0.1, 224.0, level->camera);
  PizzaBackground::drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][2], 0.2, 252.0, level->camera);
  PizzaBackground::drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][3], 0.4, 304.0, level->camera);
  PizzaBackground::drawLandscapeTilesSlopes(imgsetCache[bgLayerSet][4], 0.8, 352.0, level->camera);
}

// ==================================== TimingLevel ================================ //

TimingLevel::TimingLevel() :
PizzaLevelInterface(),

objman(),
player(NULL),
carpet(NULL),
bg(this),

camera(),
camState(TIMING_CAM_PLAY),
camZoomer(&camera.zoom.x, 1.6, 2.5),
startZoomOutCamXY(0.0, 0.0),
startZoomOutPlayerX(0.0),

actions(),
frontActions(),
script(this),

platformList(16),
currPlatformIndex(-1),

moduleList(10),
currModuleIndex(-1),
currModule(NULL),
nextModule(NULL),

runningProgress(0)
{
  musicFilename = "timing.ima4";
}

TimingLevel::~TimingLevel()
{
  free_clear(moduleList);
}

void TimingLevel::preload()
{
  
}

void TimingLevel::loadRes()
{
  bg.load();
  
  loadImgAbs(timingBlock, "timing_tent", Point2(96.0, 32.0));
  loadImgNorm(timingCarpet, "carpet");
  loadImgAbs(timingStart, "timing_start", Point2(221.0, 32.0));
  loadTilesNorm(timingSignSet, "timing_sign", 3, 1, HANDLE_BC);
  
  loadTilesAbs(timingGenieSet, "genie", 4, 2, Point2(80.0, 24.0));
  loadImgAbs(timingGenieFlame, "genie_flame");
  
  DataList<Image*> genieFlameTemp;
  genieFlameTemp.add(imgCache[timingGenieFlame]);
  // warp3Cache[genieWarp] = new DWarpAnimation("genie_flame.dwarp3", genieFlameTemp);
  warp3Cache[genieWarp] = new DWarpAnimation();
  loadWarp3Helper(*warp3Cache[genieWarp], "genie_flame.dwarp3", genieFlameTemp);
  
  loadImgAbs(timingTap, "timing_tap&", Point2(41.0, 48.0));
  loadImgAbs(timingCircleBase, "timing_circles_base", Point2(124.0, 68.0));
  loadImgAbs(timingHBarBase, "timing_hbar_base", Point2(132.0, 22.0));
  loadImgNorm(timingPieBase, "timing_pie_base", HANDLE_C);
  
  addImgAbs(timingPizzaLayers, "timing_pizza_leg_back");
  addImgAbs(timingPizzaLayers, "timing_pizza_body");
  addImgAbs(timingPizzaLayers, "timing_pizza_arm");
  addImgAbs(timingPizzaLayers, "timing_pizza_leg_front");
  
  loadWarp3(timingFallWarp, "pizzabadjump.dwarp3", timingPizzaLayers);
  loadWarp3(timingJumpWarp, "pizzajump.dwarp3", timingPizzaLayers);
  loadWarp3(timingRunWarp, "pizzarun.dwarp3", timingPizzaLayers);
  loadWarp3(timingWinStartWarp, "pizzavictorytransition.dwarp3", timingPizzaLayers);
  loadWarp3(timingVictoryWarp, "pizzavictory.dwarp3", timingPizzaLayers);
  
  DataList<Image*> carpetTemp;
  carpetTemp.add(imgCache[timingCarpet]);
  // warp3Cache[carpetWarp] = new DWarpAnimation("carpet.dwarp3", carpetTemp);
  warp3Cache[carpetWarp] = new DWarpAnimation();
  loadWarp3Helper(*warp3Cache[carpetWarp], "carpet.dwarp3", carpetTemp);
  
  loadImgNorm(winSparkle, "win_sparkle", HANDLE_C);
  
  // ignore the first one in the set, it just owns the texture
  addImgAbs(timingBuildingSet, "timing_building");
  imgsetCache[timingBuildingSet].add(new Image(imgsetCache[timingBuildingSet][0]->texture,
                                               0.0, 0.0, 64.0, 382.0));
  imgsetCache[timingBuildingSet].last()->handle = Point2(64.0, 32.0);
  
  imgsetCache[timingBuildingSet].add(new Image(imgsetCache[timingBuildingSet][0]->texture,
                                               64.0, 0.0, 128.0, 382.0));
  imgsetCache[timingBuildingSet].last()->handle = Point2(0.0, 32.0);
  
  imgsetCache[timingBuildingSet].add(new Image(imgsetCache[timingBuildingSet][0]->texture,
                                               192.0, 0.0, 64.0, 382.0));
  imgsetCache[timingBuildingSet].last()->handle = Point2(0.0, 32.0);
  
  for (Coord1 i = 0; i < 4; ++i)
  {
    String1 nameBuild(32);
    
    nameBuild = "timing_circles_pip_$";
    nameBuild.int_replace('$', i);
    addImgNorm(timingCirclePipSet, nameBuild.as_cstr(), HANDLE_C);
    
    nameBuild = "timing_hbar_pip_$";
    nameBuild.int_replace('$', i);
    addImgNorm(timingHBarPipSet, nameBuild.as_cstr(), HANDLE_C);
    
    nameBuild = "timing_pie_pip_$";
    nameBuild.int_replace('$', i);
    addImgNorm(timingPiePipSet, nameBuild.as_cstr(), HANDLE_LC);
  }
  
  loadTilesNorm(timingScalersPipSet, "timing_scale_circles", 4, 2, HANDLE_C);
  loadTilesNorm(timingTriSlideSet, "timing_slide_tri", 4, 2, HANDLE_C);
  
  addImgNorm(timingResultSet, "timing_result_ok&", HANDLE_C);
  addImgNorm(timingResultSet, "timing_result_good&", HANDLE_C);
  addImgNorm(timingResultSet, "timing_result_great&", HANDLE_C);
  addImgNorm(timingResultSet, "timing_result_perfect&", HANDLE_C);
  
  loadSound(timingWinSound, "timing_ending.wav");
  loadSound(timingTickSound, "timing_cross.wav");
  loadSound(timingGameStartSound, "timing_onscreen.wav");
  
  for (Coord1 i = 0; i < 4; ++i)
  {
    String1 nameBuild("step$.wav", '$', (i+1));
    timingStepSet.add(Sound::create(nameBuild.as_cstr()));
  }
  
  timingResultSoundSet.add(Sound::create("timing_fail.wav"));
  timingResultSoundSet.add(Sound::create("timing_ok.wav"));
  timingResultSoundSet.add(Sound::create("timing_good.wav"));
  timingResultSoundSet.add(Sound::create("timing_great.wav"));
  timingResultSoundSet.add(Sound::create("timing_perfect.wav"));
  
  carpetMusic = Sound::create("magic_carpet.ima4");
  carpetMusic->repeats();
}

void TimingLevel::unloadRes()
{
  delete carpetMusic;
  carpetMusic = NULL;
  
  free_clear(timingResultSoundSet);
  free_clear(timingStepSet);
}

void TimingLevel::load()
{
  initScoreText();
  
  prepModules();
  createWorld();  // call after prepModules(), needs the module count
  
  // initial run to building edge
  Point1 distance = 448.0 - player->getX();
  script.wait(1.0);
  script.enqueueX(new LinearFn(&player->xy.x, 400.0, distance / 384.0));
}

void TimingLevel::update()
{
  // ========== modified time
  RM::push_time_mult(worldTimeMult);
  
  updateControls();
  updateModules();
  
  if (currModule == NULL || currModule->blockGame() == false)
  {
    objman.updateObjects();
  }
  
  actions.update();
  script.update();
  updateCamera();
  
  if (levelState == LEVEL_PLAY)
  {
    Pizza::runGameTime(RM::timePassed());
    updateStatus();
  }
  
  RM::pop_time_mult();
  // ==========
  
  frontActions.update();
}

void TimingLevel::redraw()
{
  RMGraphics->clear(RM::color255(182, 206, 238));
  
  RMGraphics->push_camera(shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  // ========== camera on
  RMGraphics->push_camera(camera);
  
  bg.drawBackground();
  camera.apply();
  objman.drawObjects();
  
  RMGraphics->pop_camera();
  drawModules();
  RMGraphics->push_camera(camera);
  
  actions.redraw();
  script.redraw();
  
  RMGraphics->pop_camera();
  // ========== camera off
  RMGraphics->pop_camera();
  
  scoreText.drawText();
  tutorial->redraw();
  frontActions.redraw();
}

void TimingLevel::createWorld()
{
  Point1 startY = 192.0;
  
  // pizza, first so that others can set to it, but added last for drawing
  player = new TimingPizza();
  player->setXY(80.0, startY);
  
  // carpet
  carpet = new TimingCarpet();
  carpet->setXY(-1024.0, startY);
  addObj(carpet);
  
  // start building
  Image* startImg = imgCache[timingStart];
  TimingGO* startBuilding = new TimingBounceObj();
  startBuilding->setImage(startImg);
  startBuilding->setWH(440.0, 64.0);
  startBuilding->setXY(startBuilding->getWidth() * 0.5 - 48.0, startY);  // the -16 is for ipad padding
  addObj(startBuilding);
  
  // block generation
  Point1 standardSpacing = 352.0;
  Point1 groundY = 352.0;
  Point1 runningX = 656.0;
  Coord1 numBlocks = moduleList.count * 3;
  
  for (Coord1 i = 0; i < numBlocks; ++i)
  {
    Coord1 rand = RM::randi(0, 2);
    
    if (i < 3 || rand == 0)
    {
      // building
      TimingBuilding* building = new TimingBuilding();
      building->numMiddles = RM::randi(2, 4);
      building->setXY(runningX, startY);
      platformList.add(building);
      addObj(building);
      
      runningX += standardSpacing * RM::randf(0.8, 1.4) + building->getWidth();
    }
    else if (rand == 1)
    {
      // bouncer
      TimingGO* landingObj = new TimingBounceObj();
      landingObj->setImage(imgCache[timingBlock]);
      landingObj->setXY(runningX, startY);
      platformList.add(landingObj);
      addObj(landingObj);
      
      runningX += standardSpacing * RM::randf(0.8, 1.4);
    }
    else
    {
      // genie
      TimingGO* landingObj = new TimingGenie();
      landingObj->setXY(runningX, startY);
      platformList.add(landingObj);
      addObj(landingObj);
      
      runningX += standardSpacing * RM::randf(0.8, 1.4);
    }
  }
  
  // sign generation
  for (Coord1 i = 0; i < 3; ++i)
  {
    Coord1 obsIndex = (i+1) * moduleList.count - 1;
    
    Point1 signX = RM::lerp(platformList[obsIndex - 1]->platXRange().y, platformList[obsIndex]->getX(), 0.5);
    if (i == 2) signX = platformList[obsIndex]->getX() - standardSpacing * 0.5;
    
    // cout << "obs " << obsIndex << " platcount " << platformList.count << endl;
    TimingGO* sign = new TimingGO();
    sign->type = TimingGO::TTYPE_SIGN;
    sign->setXY(signX, 352.0);
    sign->setImage(imgsetCache[timingSignSet][i]);
    addObj(sign);
  }
  
  addObj(player);  // add last for draw order
}

void TimingLevel::updateStatus()
{
  
}

void TimingLevel::updateControls()
{
  if (levelState != LEVEL_PLAY) return;
  
  if (Pizza::controller->clickedOutsidePause() == true)
  {
    if (currModule != NULL)
    {
      currModule->touchAction();
    }
  }
}

void TimingLevel::updateModules()
{
  if (currModule != NULL)
  {
    currModule->update();
  }
  
  if (nextModule != NULL)
  {
    currModule = nextModule;
    currModule->transIn();
    nextModule = NULL;
  }
}

void TimingLevel::drawModules()
{
  if (currModule != NULL)
  {
    currModule->redraw();
  }
}

void TimingLevel::addModule(TimingModule* newModule)
{
  newModule->level = this;
  moduleList.add(newModule);
}

void TimingLevel::moduleGameDone()
{
  gotScore(resultToScore(currModule->getResultImgIndex()));
  pizzaJump();
}

void TimingLevel::pizzaJump()
{
  Coord1 numJumps = currModule->getPipIndex();
  
  // null on starting platform
  if (currPlatformIndex > 0)
  {
    platformList[currPlatformIndex]->pizzaTouched();
  }
  
  // miss
  if (numJumps == 0)
  {
    script.enqueueX(new DeAccelFn(&player->xy.y, player->getY() - 64.0, 0.5));
    script.enqueueX(new AccelFn(&player->xy.y, 500.0, 1.5));
    addAction(new LinearFn(&player->xy.x, player->getX() + 196.0, 2.0));
    
    player->playJumpAnim(2.0, false);
    
    loseLevel();
  }
  // regular jump
  else
  {
    Coord1 nextPlatformIndex = currPlatformIndex + numJumps;
    TimingGO* nextPlatform = platformList[nextPlatformIndex];
    
    Point1 distance = nextPlatform->getX() - player->getX();
    Point1 jumpTime = std::max(distance / 512.0, 1.0);
    
    script.enqueueX(new DeAccelFn(&player->xy.y, nextPlatform->getY() - 128.0, jumpTime * 0.5));
    script.enqueueX(new AccelFn(&player->xy.y, nextPlatform->getY(), jumpTime * 0.5));
    addAction(new LinearFn(&player->xy.x, nextPlatform->getX(), jumpTime));
    
    player->playJumpAnim(jumpTime, true);
    ResourceManager::playJump();
    
    // run on the next platform if it's a building, and it's not an "invisible" one at the end
    if (nextPlatform->type == TimingGO::TTYPE_BUILDING &&
        currModuleIndex < moduleList.last_i())
    {
      script.enqueueX(new SetValueCommand<Coord1>(&player->animState, TimingPizza::ANIM_RUN));
      script.enqueueX(new LinearFn(&player->xy.x, nextPlatform->getRightEdge(), nextPlatform->getWidth() / 384.0));
    }
    
    // second to last jump
    if (currModuleIndex == moduleList.last_i() - 1)
    {
      // make platforms after this jump invisible
      for (Coord1 i = nextPlatformIndex + 1; i < platformList.count; ++i)
      {
        TimingGO* checkPlat = platformList[i];
        checkPlat->setAlpha(0.0);
      }
    }
    // last jump, inform carpet
    else if (currModuleIndex == moduleList.last_i())
    {
      resolveVictory(nextPlatformIndex);
      camState = TIMING_CAM_STARTWIN;
      
      camZoomer.reset_time(jumpTime * 0.75);
      addAction(new CosineFn(&worldTimeMult, 0.35, jumpTime * 0.75));
      script.enqueueX(new SetValueCommand<Point1>(&worldTimeMult, 1.0));
      
      carpet->catchPizza(nextPlatform->getXY(), jumpTime);
    }
    
    currPlatformIndex = nextPlatformIndex;
  }
}

void TimingLevel::resolveVictory(Coord1 finalPlatIndex)
{
  // the +1 is because the 0th platform is after a 10m jump
  Pizza::currGameVars[VAR_DEFEATED] = (finalPlatIndex + 1) * 10;
}

void TimingLevel::advanceModule()
{
  currModuleIndex++;
  
  if (currModuleIndex <= moduleList.last_i())
  {
    nextModule = moduleList[currModuleIndex];
  }
  else
  {
    nextModule = NULL;
  }
}

void TimingLevel::updateCamera()
{
  switch (camState)
  {
    case TIMING_CAM_PLAY:
      camera.handle = Point2(144.0, PLAY_H);
      camera.xy.x = std::max(camera.xy.x, player->getX() - PLAY_CX);
      break;
    case TIMING_CAM_STARTWIN:
    {
      camZoomer.update();
      Point2 normalCamXY(player->getX() - PLAY_CX, 0.0);
      Point2 finalXY = camera.topleftOfNorm(player->getXY() + Point2(0.0, -32.0), HANDLE_C);
      
      Point2 currTargetXY = RM::lerp(normalCamXY, finalXY, camZoomer.progress());
      camera.setXY(currTargetXY - Point2(0.0, carpet->carpetDipOffset));
      break;
    }
    case TIMING_CAM_WIN:
      camZoomer.update();
      Point2 currTargetXY = RM::lerp(startZoomOutCamXY, Point2(startZoomOutPlayerX - PLAY_CX, -4.0),
                                     camZoomer.progress());
      camera.setXY(currTargetXY - Point2(0.0, carpet->carpetDipOffset));
      break;
  }
  
  camera.zoom.y = camera.zoom.x;
}

void TimingLevel::startZoomOutCam()
{
  camState = TIMING_CAM_WIN;
  camZoomer.reset(1.0, 1.5);
  startZoomOutPlayerX = player->getX();
  startZoomOutCamXY = camera.getXY();
}

void TimingLevel::checkModuleStars()
{
  if (currModule == NULL) return;
  
  runningProgress += currModule->getPipIndex();
}

void TimingLevel::addObj(TimingGO* obj)
{
  objman.addEnemy(obj);
  obj->level = this;
  obj->player = player;
  obj->load();
}

void TimingLevel::addAction(ActionEvent* ae)
{
  actions.addX(ae);
}

void TimingLevel::addFrontAction(ActionEvent* ae)
{
  frontActions.addX(ae);
}

TimingUnlockable* TimingLevel::tryCreateUnlockable(Point2 center, Coord2 toppingID)
{
  if (canUnlockTopping(toppingID) == false) return NULL;
  
  TimingUnlockable* floater = new TimingUnlockable(toppingID);
  floater->setXY(center);
  addObj(floater);
  
  return floater;
}

Coord1 TimingLevel::resultToScore(Coord1 resultID)
{
  switch (resultID)
  {
    default: return 0; break;
    case 0: return 100; break;
    case 1: return 200; break;
    case 2: return 350; break;
    case 3: return 500; break;
  }
}

void TimingLevel::callback(ActionEvent* caller)
{
  if (caller == &script && levelState == LEVEL_PLAY)
  {
    advanceModule();
  }
}

// ==================================== TimingModule ================================ //

TimingModule::TimingModule() :
ActionListener(),

level(NULL),
script(this),
actions(),

state(RM::TRANS_IN),
base(),
resultVR(PLAY_CENTER, imgsetCache[timingResultSet][0]),

progress(-1.0),
progressData(0.0, 1.0),
waitForDraw(false)
{
  resultVR.setAlpha(0.0);
}

void TimingModule::update()
{
  if (state == RM::REMOVE) return;
  
  script.update();
  actions.update();
  
  resultVR.matchXScale();
  
  if (state == RM::ACTIVE && waitForDraw == false)
  {
    updateFunction();
  }
  
  if (state == RM::ACTIVE && RM::approxEq(progress, 0.0) == true)
  {
    waitForDraw = true;
    ResourceManager::playTimingTick();
  }
}

void TimingModule::redraw()
{
  if (state == RM::REMOVE) return;
  
  redrawMe();
  resultVR.drawMe();
  waitForDraw = false;
  // cout << "drawing at " << getPipIndex() << " with prog " << std::abs(progress) << endl;
}

void TimingModule::touchAction()
{
  if (state == RM::ACTIVE)
  {
    transOut();
    level->moduleGameDone();
  }
}

void TimingModule::transIn()
{
  transInStd();
}

void TimingModule::transInStd()
{
  ResourceManager::playTimingGameStart();
  base.setAlpha(0.0);
  script.enqueueX(new LinearFn(&base.color.w, 1.0, 1.0, NULL));
}

void TimingModule::transOut()
{
  transOutStd();
}

void TimingModule::transOutStd()
{
  state = RM::TRANS_OUT;
  Point1 totalTransTime = 1.0;
  
  if (getPipIndex() > 0)
  {
    TimingCoin* coin = new TimingCoin();
    coin->value = getResultImgIndex();
    level->addObj(coin);
    ResourceManager::playTimingHit(getResultImgIndex() + 1);
    
    resultVR.setY(PLAY_CY + 32.0);
    resultVR.scale.set(2.0, 2.0);
    resultVR.setImage(imgsetCache[timingResultSet][getResultImgIndex()]);
    
    actions.addX(new LinearFn(&base.color.w, 0.0, totalTransTime * 0.66));
    actions.addX(new LinearFn(&resultVR.color.w, 1.0, totalTransTime * 0.25));
    
    script.enqueueX(new DeAccelFn(&resultVR.scale.x, 1.0, totalTransTime * 0.15));
    script.enqueueX(new LinearFn(&resultVR.xy.y, PLAY_CY - 32.0, totalTransTime * 0.85));
    script.enqueueAddX(new LinearFn(&resultVR.color.w, 0.0, totalTransTime * 0.15), &actions);
    script.enqueueX(new DeAccelFn(&resultVR.scale.x, 2.0, totalTransTime * 0.15));
  }
  else
  {
    ResourceManager::playTimingHit(0);
    
    script.enqueueX(new LinearFn(&base.color.w, 0.0, totalTransTime));
  }
}

Coord1 TimingModule::getPipIndex()
{
  Coord1 pipIndex = 0;
  Point1 progDist = std::abs(progress);
  
  if (progDist <= 0.05) pipIndex = 3;
  else if (progDist <= 0.101) pipIndex = 2;
  else if (progDist <= 0.17) pipIndex = 1;
  
  return pipIndex;
}

Coord1 TimingModule::getResultImgIndex()
{
  if (std::abs(progress) <= 0.01) return 3;
  
  return getPipIndex() - 1;
}

void TimingModule::startActive()
{
  
}

void TimingModule::stateHandler(ActionEvent* e)
{
  if (e == &script && state == RM::TRANS_IN)
  {
    state = RM::ACTIVE;
    startActive();
  }
  else if (e == &script && state == RM::TRANS_OUT)
  {
    resolveModule();
  }
}

void TimingModule::resolveModule()
{
  state = RM::REMOVE;
}

void TimingModule::callback(ActionEvent* caller)
{
  stateHandler(caller);
}

// ==================================== HBarModule ================================ //

HBarModule::HBarModule() :
TimingModule()
{
  base.setBox(PLAY_CENTER, Point2(126.0, 8.0));  // half-width
  base.setImage(imgCache[timingHBarBase]);
}

void HBarModule::updateFunction()
{
  progress += RM::timePassed();
  RM::bounce_linear(progress, progressData, Point2(-1.0, 1.0), 0.5 * RM::timePassed());
}

void HBarModule::redrawMe()
{
  base.drawMe();
  
  Coord1 pipIndex = getPipIndex();
  Image* currPip = imgsetCache[timingHBarPipSet][pipIndex];
  currPip->draw_scale(base.getXY() + Point2(progress * base.getWidth(), 0.0),
                      Point2(1.0, 1.0), 0.0, base.getColor());
  
  if (pipIndex != 0)
  {
    imgCache[timingTap]->draw_scale(base.getXY() + Point2(0.0, -20.0),
                                    Point2(1.0, 1.0), 0.0, base.getColor());
  }
}

// ==================================== PieModule ================================ //

PieModule::PieModule() :
TimingModule()
{
  base.setBox(PLAY_CENTER, Point2(126.0, 8.0));  // half-width
  base.setImage(imgCache[timingPieBase]);
}

void PieModule::updateFunction()
{
  progress += 1.0 * RM::timePassed();
  RM::wrap1_me(progress, -1.0, 1.0);
}

void PieModule::redrawMe()
{
  base.drawMe();
  
  Coord1 pipIndex = getPipIndex();
  Image* currPip = imgsetCache[timingPiePipSet][pipIndex];
  currPip->draw_scale(base.getXY(), Point2(1.0, 1.0), PI * progress + 1.5 * PI, base.getColor());
  
  if (pipIndex != 0)
  {
    imgCache[timingTap]->draw_scale(base.getXY() + Point2(0.0, -83.0),
                                    Point2(1.0, 1.0), 0.0, base.getColor());
  }
}

// ==================================== CirclesModule ================================ //

CirclesModule::CirclesModule() :
TimingModule()
{
  base.setBox(PLAY_CENTER, Point2(126.0, 8.0));  // half-width
  base.setImage(imgCache[timingCircleBase]);
}

void CirclesModule::updateFunction()
{
  progress += 1.0 * RM::timePassed();
  RM::wrap1_me(progress, -1.0, 1.0);
}

void CirclesModule::redrawMe()
{
  base.drawMe();
  
  Coord1 pipIndex = getPipIndex();;
  Image* currPip = imgsetCache[timingCirclePipSet][pipIndex];
  
  currPip->draw_scale(base.getXY() + Point2(-56.0, 0.0) + RM::pol_to_cart(56.0, progress * PI),
                      Point2(1.0, 1.0), 0.0, base.getColor());
  currPip->draw_scale(base.getXY() + Point2(56.0, 0.0) + RM::pol_to_cart(56.0, PI - progress * PI),
                      Point2(1.0, 1.0), 0.0, base.getColor());
  
  if (pipIndex != 0)
  {
    imgCache[timingTap]->draw_scale(base.getXY() + Point2(0.0, -52.0),
                                    Point2(1.0, 1.0), 0.0, base.getColor());
  }
}


// ==================================== ScalersModule ================================ //

ScalersModule::ScalersModule() :
TimingModule()
{
  
}

void ScalersModule::updateFunction()
{
  progress += RM::timePassed();
  RM::bounce_linear(progress, progressData, Point2(-1.0, 1.0), 0.5 * RM::timePassed());
}

void ScalersModule::redrawMe()
{
  Coord1 pipIndex = getPipIndex();;
  Image* leftPip = imgsetCache[timingScalersPipSet][pipIndex * 2];
  Image* rightPip = imgsetCache[timingScalersPipSet][pipIndex * 2 + 1];
  Point1 scaleAdd = progress * 0.5;
  
  leftPip->draw_scale(PLAY_CENTER, Point2(1.0, 1.0) + scaleAdd, 0.0, base.getColor());
  rightPip->draw_scale(PLAY_CENTER, Point2(1.0, 1.0) - scaleAdd, 0.0, base.getColor());
  
  if (pipIndex != 0)
  {
    imgCache[timingTap]->draw_scale(PLAY_CENTER + Point2(0.0, -80.0),
                                    Point2(1.0, 1.0), 0.0, base.getColor());
  }
}

// ==================================== SlidersModule ================================ //

SlidersModule::SlidersModule() :
TimingModule()
{
  
}

void SlidersModule::updateFunction()
{
  progress += RM::timePassed();
  RM::bounce_linear(progress, progressData, Point2(-1.0, 1.0), 0.5 * RM::timePassed());
  
}

void SlidersModule::redrawMe()
{
  Coord1 pipIndex = getPipIndex();;
  Image* leftPip = imgsetCache[timingTriSlideSet][pipIndex * 2];
  Image* rightPip = imgsetCache[timingTriSlideSet][pipIndex * 2 + 1];
  Point2 centerDist = Point2(128.0 * progress, 0.0);
  
  leftPip->draw_scale(PLAY_CENTER + centerDist, Point2(1.0, 1.0), 0.0, base.getColor());
  rightPip->draw_scale(PLAY_CENTER - centerDist, Point2(1.0, 1.0), 0.0, base.getColor());
  
  if (pipIndex != 0)
  {
    imgCache[timingTap]->draw_scale(PLAY_CENTER + Point2(0.0, -72.0),
                                    Point2(1.0, 1.0), 0.0, base.getColor());
  }
}

// ==================================== TILevelCircles

void TILevelCircles::prepModules()
{
  addModule(new HBarModule());
  addModule(new PieModule());
  addModule(new CirclesModule());
}

void TILevelCircles::moduleGameDone()
{
  TimingLevel::moduleGameDone();
  
  if (currModuleIndex == moduleList.last_i())
  {
    Logical allPerfect = true;
    
    for (Coord1 i = 0; i < moduleList.count; ++i)
    {
      if (moduleList[i]->getResultImgIndex() != 3)
      {
        allPerfect = false;
        break;
      }
    }
    
    if (allPerfect == true)
    {
      Pizza::scoreMan->earnedAchievement(ACH_CARPET);
    }
  }
}

// ==================================== TILevelScalers

void TILevelScalers::prepModules()
{
  addModule(new PieModule());
  addModule(new CirclesModule());
  addModule(new HBarModule());
  addModule(new ScalersModule());
}

// ==================================== TILevelSliders

void TILevelSliders::prepModules()
{
  addModule(new CirclesModule());
  addModule(new HBarModule());
  addModule(new ScalersModule());
  addModule(new PieModule());
  addModule(new SlidersModule());
}

void TILevelSliders::createWorld()
{
  TimingLevel::createWorld();
  
  tryCreateUnlockable(platformList.first()->getXY() + Point2(0.0, -32.0), Coord2(2, 12));
}

// ==================================== CreateWinBanner

ActionEvent* CreateWinBanner()
{
  Coord1 chooser = RM::randi(0, 2);
  
  if (chooser == 0)
  {
    return new BannerSpinEffect(imgCache[winBanner], PLAY_CENTER);
  }
  else if (chooser == 1)
  {
    return new BannerSwoopEffect(imgCache[winBanner], Point2(PLAY_CX, 0.0), PLAY_CY);
  }
  else
  {
    BannerFlipEffect* flipBanner = new BannerFlipEffect(imgCache[winBanner], PLAY_CY, PLAY_CX);
    flipBanner->addX(Pizza::platformTL.x);
    return flipBanner;
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
  window.setXY(PLAY_CENTER);
  window.setFont(ResourceManager::font);
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
    if (Pizza::controller->clickedOutsidePause() == true)
    {
      click(NULL);
    }
  }
}

void PizzaTutorial::redraw()
{
  drawActions();
  
  if (transProgress > 0.01)
  {
    Point2 boxXY = Box::from_center(PLAY_CENTER, boxSize).xy;
    ColorP4 finalColor = boxColor;
    finalColor.w = transProgress;
    
    imgCache[losePx]->draw(boxXY, boxSize, 0.0, finalColor);
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

// ==================================== PizzaTutorial ==================================== //

PassiveTutorial::PassiveTutorial(PizzaLevelInterface* setLevel) :
PizzaTutorial(setLevel)
{
  updateGame = true;
}

// ==================================== SlamTutorial ==================================== //

SlamTutorial::SlamTutorial(PizzaLevelInterface* setLevel) :
PizzaTutorial(setLevel)
{
  transIn();
  boxSize.set(384.0, 224.0);
  boxColor = RM::color255(222, 94, 77);
  setWindowText(LSTRINGS[RMSystem->get_OS_family() == MAC_FAMILY ? 139 : 109]);
}

// ==================================== EnemyJumpTutorial ==================================== //

EnemyJumpTutorial::EnemyJumpTutorial(PizzaLevel* setLevel) :
PizzaTutorial(setLevel),
regularLevel(setLevel)
{
  transIn();
  boxSize.set(384.0, 224.0);
  boxColor = RM::color255(82, 125, 130);
  setWindowText(LSTRINGS[RMSystem->get_OS_family() == MAC_FAMILY ? 140: 110]);
}

void EnemyJumpTutorial::startResolve()
{
  regularLevel->player->tryJump();
}

// ==================================== KarateTutorial ==================================== //

KarateTutorial::KarateTutorial(PizzaLevelInterface* setLevel) :
PizzaTutorial(setLevel)
{
  transIn();
  boxSize.set(352.0, 160.0);
  boxColor = RM::color255(120, 99, 153);
  setWindowText(LSTRINGS[111]);
}

// ==================================== WreckJumpTutorial ==================================== //

WreckJumpTutorial::WreckJumpTutorial(PizzaLevelInterface* setLevel) :
PizzaTutorial(setLevel)
{
  transIn();
  boxSize.set(384.0, 224.0);
  boxColor = RM::color255(222, 94, 77);
  setWindowText(LSTRINGS[RMSystem->get_OS_family() == MAC_FAMILY ? 141 : 112]);
}
