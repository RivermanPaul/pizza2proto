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
struct DWarpSequence;
struct DWarpAnimation;
struct VisRectangular;

enum ImageID
{
  bg,

  titleSky,
  titleSpine,
  titleSkull,
  titleJaw,
  titlePizza,
  titleLogo,
  titleRiverman,
  titlePresents,
  titlePizzaPad,
  
  mapStarCount,
  mapStarCountGlow,
  mapShop,
  mapShopDown,
  mapBubbleShop,
  
  instructionsHiScore,
  instructionsStar,

  shopBaseLeft,
  shopBaseRight,
  shopSelector,
  shopDone,
  shopShare,
  shopConfirm,
  shopYes,
  shopNo,
  shopQuestion,
  
  shareBackBones,
  shareSelectBase,
  shareSelector,
  shareShareBase,
  shareShareDown,
  shareFrontBones,
  shareLogo,
  shareRiverman,
  
  pizzaBase,
  pizzaClothes,
  pizzaEyes,
  pizzaMouth,
  pizzaGlow,
  slamShock,
  slamStreak,
  slamStar,
  blinkStar,
  burstRing,
  winBanner,
  loseBanner,
  losePx,
  skullBarFrameImg,
  skullBarFillImg,
  skullBarClock,
  secretOutfit,
  secretOutfitGlow,
  
  cupidArrow,
  nagaHeadClosed,
  nagaHeadOpen,
  nagaTail,
  nagaBone,
  nagaArm,
  roboElectricity,
  giantSkull,
  flamingSkull,
  wormImg,
  wormParticleImg,
  wormShadowImg,
  spinnerGlobeImg,
  sludge,
  sludgeParticle,
  shadowSmall,
  shadowMedium,
  shadowLarge,
  
  sumoBoss,
  alienLaser,
  shockBolt,
  sumoFireball,
  sumoMadRing,
  sumoKnife,
  sumoGust,
  sumoGem,
  sumoGemGlow,
  
  warningH,
  warningV,

  karateStart,
  karateMiddle,
  karateEnd,
  karateBlock,
  karateWood,
  karateSpear,
  brainStill,
  brainLead,
  
  slopeEnd,
  slopeStart,
  slope2_1,
  slope3_1,
  slope4_1,
  slope3_1_break,
  slope3_1_short,
  slopeBouncy,
  slopeFlatLong,
  slopeFlatShort,
  slopeSpikePlatform,
  slopeTriangle,
  slopeUphill,
  slopeSpikesDown,
  slopeSnowman,
  spikeball,
 
  balancePiece2x2,
  balancePieceBounceCircle,
  balancePieceBouncePlatform,
  balancePieceDownhill4_1,
  balancePieceDownhill4_3,
  balancePieceDownhill8_1,
  balancePieceDownhill12_4,
  balancePieceDownhill16_4,
  balancePieceFlatlong,
  balancePieceFlatmed,
  balancePieceNospike,
  balancePieceShort,
  balancePieceRiser,
  balancePieceRiseBlock,
  balancePiecePillar,
  balancePieceUphill4_1,
  balancePieceUphill4_3,
  balancePieceUphill8_1,
  balancePieceUphill8_3,
  balancePieceUphill12_2,
  balancePieceUphill16_4,
  balanceBallCircle,
  balanceBallOct,
  balanceGongStand,
  
  puppyDoghouseBack,
  puppyDoghouseFront,
  puppyBounce16,
  puppyCornerL,
  puppyCornerR,
  puppyDownhill4_2,
  puppyDownhill4_4,
  puppyDownhill8_2,
  puppyDownhill12_8,
  puppyEdgeL,
  puppyEdgeR,
  puppyEdgeTL,
  puppyEdgeTR,
  puppyFlat8,
  puppyFlat16,
  puppySpikeball,
  puppySpikes256,
  puppyPivotSquare,
  puppyPivotTri,
  puppyUphill4_2,
  puppyUphill4_4,
  puppyUphill8_2,
  puppyUphill12_8,
  puppyWall,
  madsun,
  madsunGlow,
  madsunFlame,
  caltropImg,
  
  bounceSkullBig,
  bounceSkullHeavy,
  bounceWormhole,
  bounceGiantComet,

  lakeBG,
  lakeCloseCoral,
  lakeLight,
  lakePufferDeflated,
  lakePufferTrans,
  lakePufferInflated,
  lakePearlImg,
  lakeSwordfishImg,
  lakePizza32,
  lakePizza64,
  lakePizza128,
  lakePizza256,
  lakeHaroon,
  lakeHarpoonRope,
  
  wreckWindowStrong,
  wreckRoofLeft,
  wreckRoofRight,
  wreckBuildingPad,
  wreckEmptyPad,
  wreckRoofPad,
  wreckChain,
  wreckBone,
  wreckNode,
  wreckBaseball,
  wreckObstacle,
  wreckBottom,
  wreckUFOBlast,
  wreckUFOBlastBot,
  wreckSickle,
  
  planeFront,
  planeBullet,
  
  timingCircleBase,
  timingHBarBase,
  timingPieBase,
  timingTap,
  timingBlock,
  timingCarpet,
  timingStart,
  timingGenieFlame,
  
  pauseBaseBot,
  pauseBaseMid,
  pauseBaseTop,
  pauseCalibrate,
  pauseCalibrateArrow,
  pauseInstructions,
  pausePlay,
  pauseQuit,
  pauseRestart,
    
  wheelCircle,
  wheelTextLeft,
  wheelTextRight,
  wheelNeon,  
  winRing,
  winCash,
  winMessage,
  winSparkle,
  winStreak,
  winChapterBanner,

  endingTheEnd,
  
  NUM_IMG_IDS
};

enum ImageSetID
{
  mapLockSet,
  mapArrowSet,
  mapSocialIcons,
  mapNumbers,
  mapStarSet,
  moregamesAnimSet,
  moregamesButtonSet,

  shopTabSet,

  shareCloseSet,

  pizzaSauceSet,
  pizzaToppingSet,
  bgLayerSet,
  bgObjectSet,
  pennySet,
  nickelSet,
  dimeSet,
  quarterSet,
  sauceDrops,
  sauceSplats,
  smokeSet,
  sparkSet,
  coinSparkleSet,
  pizzaBoxSet,
  boxGlowSet, 
  skullGlowSet,
  junkSet,
  sumoTerrainSet,
  healthBarSet,
  pauseSet,
  soundBoxSet,
  loseInstructionsSet,
  loseQuitSet,
  loseRetrySet,
  instructionsCloseSet,
  instructionsPlaySet,

  skeletonUnarmed,
  skeletonSpearSide,
  skeletonSpearVert,
  pumpkinImgs,
  spinyImgs,
  ostrichWalkSide,
  ostrichWalkVert,
  ostrichFlySide,
  cupidIdleSet,
  cupidShootSet,
  cupidWingSrc,  
  wispSet,
  wispFlameSet,
  wispParticles,
  roboWalkImgs,
  roboAtkImgs,
  roboFireballImgs,
  giantWalkImgs,
  giantAtkImgs,
  flameskullFlameSet,
  rabbitSet,
  spinnerSpikeSet,
  cinemaTextSet,
  
  sumoGrassSet,

  bigFlagSet,
  flagSet,
  scarfSet,
  snowboardSet,
  skullSwitchSet,
  
  balanceGongSet,

  puppyAImgs,
  puppyBImgs,
  puppyCImgs,
  puppyMissileImgs,
  puppyCage,
  puppyFireballSet,
  puppyFireSplashSet,
  moleImgs,
  mimicSet,
  bombSet,
  bombExplodeSet,
  puppyCrateSet,
  puppyLavaSetSmall,
  puppyLavaSetLarge,
  
  bounceSegmentSet,
  bounceSkullSet,
  bounceBombSet,
  bounceCometSet,
  bounceLightningSet,
  bounceSquishSet,
  
  brainBounce,
  brainCircle,
  brainFly,
  brainWalk,
  
  angryTerrainSet,
  angryBlockSet,
  angrySkullSet,
  
  lakeFGCoral,
  lakeFarCoral,
  lakePlants,
  lakeFishBodySet,
  lakeFishHeadSet,
  lakeStarfishSet,
  lakeEelSet,
  lakePoisonSet,
  lakePearlSparkleSet,
  lakeBubbleSet,
  lakeHeartSet,
  lakeBarrelSet,
  
  wreckWindowAll,
  wreckSkele,
  wreckThrowArmSet,
  wreckElectricSet,
  wreckBaseballSkele,
  wreckFlierSrc,
  wreckCutSkeleImgs,
  wreckWindowFireImgs,
  wreckBoySrc,
  wreckGirlSrc,
  wreckOldSrc,
  wreckBatterSrc,
  wreckClimberSrc,
  wreckUFOSet,
  wreckEasy1Set,
  wreckEasy2Set,
  wreckEasy3Set,
  wreckCutterSet,
  wreckPowerupSet,
  wreckPowerupSparkSet,
  wreckShieldSet,
  
  planePropSet,
  planeMuzzleSet,
  
  timingCirclePipSet,
  timingHBarPipSet,
  timingPiePipSet,
  timingScalersPipSet,
  timingTriSlideSet,
  timingPizzaLayers,
  timingBuildingSet,
  timingGenieSet,
  timingSignSet,
  timingResultSet,
  
  wheelTextSet,
  wheelBulbSet,
  wheelSliceSet,
  wheelIconSet,
  
  winCheckSet,
  winStarSet,
  winVictorySet,
  winReplaySet,
  winContinueSet,
  
  endingVerseSet,
  endingBoneSet,
  endingSkullSet,
  
  upgradeSlideSet,
  upgradeYesSet,
  upgradeNoSet,
  
  NUM_IMGSET_IDS
};

enum DWarpID
{
  titleWarp,
  
  mapChapterWarp,
  mapShopWarp,
  mapSkullWarp,
  
  eyeWarp,
  mouthWarp,
  clothesWarp,
  
  wispWarp,
  wormWarp,
  sumoWarp,
  lakeFaceWarp,
  pufferDeflatedWarp,
  pufferInflatedWarp,
  swordfishWarp,
  eelWarp,
  bounceBigWarp,
  sunWarp,
  planeWarp,
  
  NUM_DWARP_IDS
};

enum Warp3AnimationID
{
  timingFallWarp,
  timingJumpWarp,
  timingRunWarp,
  timingWinStartWarp,
  timingVictoryWarp,
  carpetWarp,
  genieWarp,
  
  NUM_WARP3_IDS
};

enum SoundID
{
  music,  // remember: introMusic, endingMusic, and carpetMusic are separate
  
  // common to play scene
  outfitSound,
  bombSound,
  sunAttackSound,
  lightningSound,

  // misc scenes
  cometSound,
  fireballJumpSound,
  missileSound,
  puppyBark,
  flagSound,
  gongSound,
  cameraSound,
  moreRivermanSound,
  rivermanInterstitialSound,

  // gladiator
  skeleRiseSound,
  demonSwoopSound,
  cupidSound,
  giantThrowSound,
  nagaSound,
  roboPhaseSound,
  roboFlameSound,
  wispShootSound,
  wispDieSound,
  spinnerSound,
  
  // map
  lockBreakSound,
  
  // lake
  fishEatSound,
  swimSound,
  harpoonSound,
  eelSound,
  
  // wreck
  civilianSound,
  ufoAppearSound,
  ufoBlastSound,
  wreckPowerSound,
  
  // sumo
  sumoAngrySound,
  knifeSound,
  laserSound,
  bossTaunt,
  bossSink,
  
  // plane
  bulletHit,
  
  // timing
  timingTickSound,
  timingGameStartSound,
  timingWinSound,
  
  // win
  wheelClickSound,
  wheelMissSound,
  wheelStopSound,
  wheelWinSmall,
  wheelWinBig,
  messageSound,
  starGotSound,
  starMissSound,
  goalSlideSound,
  
  NUM_SOUND_IDS
};

extern DataList<Image*> imgCache;
extern ArrayList<DataList<Image*> > imgsetCache;
extern DataList<DWarpSequence*> dwarpCache;
extern DataList<DWarpAnimation*> warp3Cache;
extern DataList<Sound*> soundCache;

namespace ResourceManager
{
  extern String1 workingStr;
  extern String2 efigsCharsStd;
  
  extern RMFont
  *font,
  *scoreFont,
  *winFont,
  *instructionsFont,
  *mapScoreFont;
  
  // shared
  extern Image
  *lastScreenImg,
  *loadingImg,
  
  *extBG0Img,
  *extBG1Img,
  *extBG2Img,
  *extBG3Img,
  *extClockImg,
  *extBillImg,
  *extDividerLeftImg,
  *extDividerRightImg,
  *extSkyImg;
  
  // shop
  extern RMFont
  *shopCashFont,
  *shopItemFont;
  
  extern ArrayList<DataList<Image*> >
  shopItemSet;
  
  extern DataList<DWarpSequence*>
  plantWarps,
  fishBodyWarps,
  fishHeadWarps,
  angrySkullWarps,
  bounceSkullWarps;  // does not include the big one
  
  // THESE IMAGES ARE NOT OWNED, they just represent global animations
  extern DataList<Image*>
  pumpkinFly,  
  cupidWingSet,
  wreckBoyAnim,
  wreckGirlAnim,
  wreckOldAnim,
  wreckThrowerFull,
  wreckArmFull,
  wreckCatcherThrow,
  wreckCatcherReverse,
  wreckClimberAttack,
  wreckClimberClimb,
  wreckWingHover,
  wreckWingAttack,
  wreckEasy1Full,
  wreckEasy2Full,
  wreckEasy3Full,
  wreckBatterFull,
  selector3x3Normal,
  selector3x3Fill,
  selector3x3Add;
  
  // common sounds
  extern Sound
  *landing,
  *pizzaHit,
  *startSlam,
  *slamEnemy,
  *slamGround,
  *jump,
  *winLick,
  *loseLick,
  *clickStd,
  *clickPlay,
  *noClick,
  *enemyThrow,
  *enemyJumpSound,
  *rabbitJumpSound,
  *bounceSound,
  *downTick;

  // these need to be here b/c of restarting
  extern Sound 
  *introMusic,
  *endingMusic,
  *carpetMusic;
  
  extern DataList<Sound*>
  crunchSet,
  coinSoundSet,
  collideSoundSet,
  bulletSet,
  timingResultSoundSet,
  timingStepSet;
  
  extern Point1 landWaiter;
  
  void loadCommon();
  
  void loadSplash();
  void unloadSplash();
  
  void loadTitle();
  void unloadTitle();
  
  void loadMap();
  void unloadMap();
  
  void loadShop();
  void unloadShop();
  
  void loadShare();
  void unloadShare();
  
  void loadPlay(Logical shouldLoadPizza);  // false for lake
  void unloadPlay();   // false for lake
  
  void loadWheel();
  void unloadWheel();
  
  void loadUpgrade();
  void unloadUpgrade();
  
  // helpers
  void loadPizza();
  
  void loadBase(Logical unloadCurr = false, Logical loadDamage = true);
  void loadEyes(Logical unloadCurr = false);
  void loadMouth(Logical unloadCurr = false);
  void loadClothes(Logical unloadCurr = false);
  void loadToppings(Logical unloadCurr = false);
  
  String1& imgStr(const Char* imgBase);
  
  void loadImgNorm(ImageID imgIndex, const Char* imgBase, 
                   Point2 normHandle = Point2(0.0, 0.0), Logical overexpose = false);
  void loadImgAbs(ImageID imgIndex, const Char* imgBase, 
                  Point2 absHandle = Point2(0.0, 0.0), Logical overexpose = false);
  
  void loadTilesNorm(ImageSetID setIndex, const Char* imgBase, 
                     Coord1 cols, Coord1 rows, Point2 normHandle = Point2(0.0, 0.0), 
                     Logical overexpose = false);
  void loadTilesAbs(ImageSetID setIndex, const Char* imgBase, 
                    Coord1 cols, Coord1 rows, Point2 absHandle = Point2(0.0, 0.0),
                    Logical overexpose = false);
  
  void addImgNorm(ImageSetID setIndex, const Char* imgBase, 
                  Point2 normHandle = Point2(0.0, 0.0), Logical overexpose = false); 
  void addImgAbs(ImageSetID setIndex, const Char* imgBase, 
                 Point2 absHandle = Point2(0.0, 0.0), Logical overexpose = false);
  
  void loadWarp(DWarpID warpIndex, const Char* filename);

  void loadWarp3(Warp3AnimationID warpIndex, const Char* warpFilename, ImageSetID setID);
  void loadWarp3Helper(DWarpAnimation& warp, const Char* warpFilename, const DataList<Image*>& imgSet);

  void loadSound(SoundID soundIndex, const Char* filename);
  
  void unloadCaches();
  
  // for sounds
  void update();
  
  void playJump();
  void playDamaged();
  void playSlamStart();
  void playSlamLand();
  void playCrunch();
  void playLanding(Logical overrideStall = false);
  void playPizzaBoxBreak();
  void playCollide();
  void playFishEat();
  void playCoinGet();
  void playOutfitFound();
  void playClickStd();
  void playClickPlay();
  void playCantClick();
  void playLockBreak();
  void playLightning();
  void playSunAttack();
  void playHarpoonShoot();
  void playSwim();
  void playSkeleRise();
  void playDemonSwoop();
  void playOstrichHop();
  void playGiantThrow();
  void playCupidShoot();
  void playNagaCharge();
  void playWispShoot();
  void playWispDie();
  void playRobotPhase();
  void playRobotFlame();
  void playRabbitJump();
  void playTailRise();
  void playMoleThrow();
  void playFireballJump();
  void playMimicJump();
  void playBark();
  void playMissile();
  void playBounceComet();
  void playSumoAngry();
  void playSumoLaser();
  void playSumoKnife();
  void playBossSink();
  void playBombExplode();
  void playGong();
  void playFlagGet();
  void playSlopeSwitch();
  void playHitCivilian();
  void playWreckThrow();
  void playWreckPowerup();
  void playWreckUFOAppear();
  void playWreckUFOShock();
  void playPlaneFire();
  void playPlaneBulletHit();
  void playNagaBreath();
  
  void playSpinnerAppear();  // for gladiator
  void playEelShock();
  void playTimingTick();
  void playTimingHit(Coord1 index);  // 0 miss, 1-3*, 4 perfect
  void playTimingStep();
  void playTimingGameStart();
  void playTimingWin();
  
  void playWheelClick();
  void playWheelStop();
  void playWheelGetSmall();
  void playWheelGetBig();
  void playWheelMiss();
  void playWinStar();
  void playWinNotification();
  void playStarMiss();
  void playGoalSlide();
  void playBossTaunt();
  void playBossSink();
  void playFlashSound();
  void playBounceSound();
}

// ============================== PizzaWarper =========================== //

struct PizzaWarper
{
  VisRectangular* parent;
  DWarpSequence* sequence;
  DWarpFrame contWarpFrame;
  DWarpFrame lastWarpFrame;  // set every time it draws

  PizzaWarper();
  PizzaWarper(VisRectangular* setParent, DWarpSequence* setSequence);
  void set(VisRectangular* setParent, DWarpSequence* setSequence);

  void last_to_cont();  // last warp goes to cont

  void draw(Point1 fullWeight);  // uses all frames, starting at 0
  void draw(Coord1 startFrame, Coord1 tgtFrame, Point1 rangeWeight);  // uses ONLY 2 frames

  void draw_lerp(const DWarpFrame& startFrame, const DWarpFrame& tgtPts, Point1 weight);

  void draw(const DWarpFrame& frame);  // points are not transformed yet

  // the _cont methods use contWarpPts
  void draw_cont(Coord1 tgtFrame, Point1 weight);

  Coord1 numFrames();
  Coord1 calcFrame(Point1 weight);  // returns bottom frame
  Point2 frameRange(Coord1 frameID);  // returns [botWeight, topWeight]
  Point1 frameWeight(Point1 sequenceWeight);
};

#endif
