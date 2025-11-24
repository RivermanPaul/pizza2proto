/*
 *  level.h
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 11/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LEVEL_H
#define LEVEL_H

#include "rmvector.h"
#include "rmactions.h"
#include "rmgui.h"
#include "rmbackground.h"
#include "pgo.h"
#include "resourceman.h"

struct PizzaObjman;
struct PizzaLevel;
struct LakeLevel;
struct PizzaBackground;
struct PizzaTutorial;

struct PizzaLevelInterface
{
  Coord1 levelID;
  Coord1 levelState;  // passive, active, win, lose

  PizzaTutorial* tutorial;
  Logical tutShown;  // most only have 1
   
  CameraStd camera;
  ShakeRegister shaker;
  RandomSimple randGen;
  VisibleText scoreText;
  VisibleText endingScoreText;  // only used in ending

  VisRectangular healthBar;
  TimerFn healthStayTimer;
  Coord1 prevPlayerDMG;   // monitored for health bar to move
  Point1 healthChaseVal;  // % of max life
  Point2 healthYBounds;
  Point1 healthYVal;    // [0.0 - 1.0], matches alpha
  Point2 healthYData;
  Point1 healthPulseVal;  // [0.0 - 1.0] 
  Point2 healthPulseData;
  
  Coord1 coinFrame;
  Coord1 coinsCreated;
  Coord1 easySkeleID; // for wreck to pick the easy animation
  
  Point1 worldTimeMult;
  
  String1 musicFilename;  // set this before the resources are loaded
  
  // levelState constants
  static const Coord1 
  LEVEL_PLAY = 0,
  LEVEL_PASSIVE = 1,
  LEVEL_LOSE = 2,
  LEVEL_WIN = 3;
  
  static const Coord1
  MAX_DMG_STD = 8;
  
  PizzaLevelInterface();  // levelType is already determined
  virtual ~PizzaLevelInterface();  // deletes tutorial
  
  virtual void loadRes() = 0;
  virtual void unloadRes() = 0;
  virtual void load() = 0;  // call this AFTER loadRes()
  
  virtual void update() = 0;
  virtual void redraw() = 0;
  
  virtual void preload() {}  // create background here
  virtual void startActive() {}
  virtual void addAction(ActionEvent* ae) = 0;  // required for win/lose
  virtual void addFrontAction(ActionEvent* ae) = 0;   // required for win/lose
  
  // adds as a front action
  virtual void addTextEffectStd(const String2& str, Point2 location, Point1 holdTime = 2.0);
  virtual void addSauceEffect(const VisRectangular& pizzaPlayer, Point2 atkPoint);
  virtual void initScoreText();
  
  virtual Coord1 getCoinFrame();
  virtual void gotCoin(Point2 center, Coord1 cashVal);
  virtual void gotScore(Coord1 scoreVal);  // scoreVal can be -
  
  virtual Coord1 getEasySkeleIndex();
  
  virtual void winLevel();
  virtual void loseLevel();
  virtual void startTutorial(PizzaTutorial* newTutorial);
  virtual void updateTutorials();
  virtual Logical pauseGUIEnabled() {return true;}
  virtual Logical startsWithMusic() {return true;}
  virtual Logical shouldLoadPizza() {return true;}  // false on lake
  
  virtual Logical usesHealthBar() {return true;}
  virtual void initHealth(); // call this after loading the resources
  virtual Point1 healthMasterAlpha() {return 1.0;}  // sun, plane levels override
  virtual void updateHealthBar();
  virtual void drawHealthBar();
  
  virtual Logical lostToDMGStd();
  virtual Logical canUnlockTopping(Coord2 toppingID);
  virtual Logical alreadyBeaten();  // returns stars > 0
  virtual Coord1 calcWinBonus() {return 0;} // essentially the 1* bonus
  virtual CameraStd platCam();  // takes platform into account
  virtual Box camBox();  // takes platform into account
  
  // statics
  static PizzaLevelInterface* createLevel(Coord2 levelDef); // [levelType, levelID]
  
  static PizzaLevelInterface* createGladiatorLevel(Coord1 levelID);
  static PizzaLevelInterface* createKarateLevel(Coord1 levelID);
  static PizzaLevelInterface* createSlopesLevel(Coord1 levelID);
  static PizzaLevelInterface* createBalanceLevel(Coord1 levelID);
  static PizzaLevelInterface* createBounceLevel(Coord1 levelID);
  static PizzaLevelInterface* createPuppyLevel(Coord1 levelID);
  static PizzaLevelInterface* createAngryLevel(Coord1 levelID);
  static PizzaLevelInterface* createLakeLevel(Coord1 levelID);
  static PizzaLevelInterface* createWreckLevel(Coord1 levelID);
  static PizzaLevelInterface* createSumoLevel(Coord1 levelID);
  static PizzaLevelInterface* createPlaneLevel(Coord1 levelID);
  static PizzaLevelInterface* createTimingLevel(Coord1 levelID);
};

struct ChangeLevelStatusCMD : Command
{
  PizzaLevelInterface* level;
  Coord1 newLevelState;
  
  ChangeLevelStatusCMD(PizzaLevelInterface* setLevel, Coord1 newState) :
    level(setLevel),
    newLevelState(newState)
  {
    
  }
  
  void execute()
  {
    if (newLevelState == PizzaLevelInterface::LEVEL_WIN) level->winLevel();
    else if (newLevelState == PizzaLevelInterface::LEVEL_LOSE) level->loseLevel();
  }
};

// ========================= ObjmanStd =========================== //

template <typename Type>
struct ObjmanStd
{
  LinkedList<Type*> allyList;
  LinkedList<Type*> enemyList;
  
  ActionList actions;
  ActionList frontActions;  // draw on top of game  
  
  Coord1 removeCounter;
  
  ObjmanStd();
  virtual ~ObjmanStd() {}
  
  virtual void update();
  virtual void updateObjects();
  virtual void drawObjects();
  
  void drawEnemies();
  void drawAllies();
  
  void addAlly(Type* ally);
  void addEnemy(Type* enemy);
  
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  void garbageCollector();
};

template <typename Type>
Logical removeBGO(Type* pgo);

template <typename Type>
ObjmanStd<Type>::ObjmanStd() :
  allyList(),
  enemyList(),

  actions(),
  frontActions(),

  removeCounter(0)
{
  
}

template <typename Type>
void ObjmanStd<Type>::update()
{
  updateObjects();
  garbageCollector();
  actions.update();
  frontActions.update();
}

template <typename Type>
void ObjmanStd<Type>::updateObjects()
{
  for (LinkedListNode<Type*>* enemyNode = enemyList.first; 
       enemyNode != NULL; 
       enemyNode = enemyNode->next)
  {
    enemyNode->data->update();
  }
  
  for (LinkedListNode<Type*>* allyNode = allyList.first; 
       allyNode != NULL; 
       allyNode = allyNode->next)
  {
    allyNode->data->update();
  }  
}

template <typename Type>
void ObjmanStd<Type>::drawObjects()
{
  drawEnemies();
  drawAllies();
}

template <typename Type>
void ObjmanStd<Type>::drawEnemies()
{
  for (LinkedListNode<Type*>* enemyNode = enemyList.first; 
       enemyNode != NULL; 
       enemyNode = enemyNode->next)
  {
    enemyNode->data->redraw();
  }
}

template <typename Type>
void ObjmanStd<Type>::drawAllies()
{
  for (LinkedListNode<Type*>* allyNode = allyList.first; 
       allyNode != NULL; 
       allyNode = allyNode->next)
  {
    allyNode->data->redraw();
  }
}

template <typename Type>
void ObjmanStd<Type>::addAlly(Type* ally)
{
  allyList.addX(ally);
}

template <typename Type>
void ObjmanStd<Type>::addEnemy(Type* enemy)
{
  enemyList.addX(enemy);
}

template <typename Type>
void ObjmanStd<Type>::addAction(ActionEvent* ae)
{
  actions.addX(ae);
}

template <typename Type>
void ObjmanStd<Type>::addFrontAction(ActionEvent* ae)
{
  frontActions.addX(ae);  
}

template <typename Type>
void ObjmanStd<Type>::garbageCollector()
{
  // stagger garbage collection for enemies and bullets, still 10 frames
  ++removeCounter;
  if (removeCounter == 5)
  {
    enemyList.removeMatches(&removeBGO);
  }
  else if (removeCounter == 10)
  {
    allyList.removeMatches(&removeBGO);
    removeCounter = 0;
  }
}

template <typename Type>
Logical removeBGO(Type* bgo)
{
  return bgo->lifeState == RM::REMOVE;
}

// ======================== Pizza Standard ================== //

struct PizzaObjman : ObjmanStd<PizzaGO>
{
  ArrayList<PizzaGO*> bgTerrainList;
  ArrayList<PizzaGO*> sortedDrawList;
  ActionList debrisList;

  PizzaObjman();
  ~PizzaObjman();
  
  void update();
  void updateObjects();
  void drawObjects();
  void drawBGTerrain();
  
  void addBGTerrain(PizzaGO* terrain);
};

struct PizzaLevel : PizzaLevelInterface, ActionListener, b2ContactListener
{
  PizzaObjman objman;
  PizzaPlayer* player;
  PizzaBackground* background;
    
  Box worldBox;  // some levels use this for bounds checking
  Point2 endpoint;   // this is for levels that build in chunks
  Logical spikedTerrain;  // for the spiked level, it's a global
  Logical bounceTerrain;  // for the bounce level, it's a global
  Logical useOctBall;  // true enables for all balls in balance
  
  DataList<Point1> chunkWeights;  // used in bounce, spike, and puppy
  DataList<Point2> runningEnpoints;
  
  DataList<BalanceSkull*> ballList;
  Coord1 currBallID;
  
  DataList<Billy*> puppyList;
  Coord1 currBreed;
  Coord1 puppiesToFind;
  Coord1 puppiesToReturn;
  Coord1 puppiesTotal;
  Point2 puppyStartPt;  // they hop off of you to here
  Point2 puppyHousePt;  // then they prance into here
  
  Coord1 startingSkulls;  // for starting effect
  Coord1 skullsCrushed;
  Coord1 totalDestructibles;  // for angry, includes skulls

  Point1 tiltPercent;  // [-1.0, 1.0] helps camera
  Coord1 playerMotionFlags;  // correspond to the flag bits of PizzaGO

  TimerFn junkTimer;
  VisibleText textDrawer;  // this is for level metrics like time/defeated

  PizzaLevel();
  virtual ~PizzaLevel();
      
  void reportPlayerLanded(Logical slam); // trigger shake
  void reportPlayerBump(Point1 impulseVal);
  void reportSlam();
  void reportExplode(const Circle& explosion);
  
  void addSortDrawer(PizzaGO* pgo);
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  void addEnemy(PizzaGO* enemy);
  void addAlly(PizzaGO* ally);
  void addBGTerrain(PizzaGO* terrain);
  void addDebris(ActionEvent* debrisEffect);

  void drawBottomBorder();  // for ipad shaking
  void drawShadowsStd(Point1 playerGroundY);
  
  void startStanding(Point2 groundPt);
  void updateZoomStd(Point1 yPad = 64.0);  // zooms based on player top vs screen top
  void updateFacingStd(Logical hasEnemies, Point1 closestX);  // responds to closest enemy or tilt
  void enemyCameraStd(Logical hasEnemies, Point1 closestX);  // responds to closest enemy or tilt
  void zoomToMinY(Point1 yTarget);  // assumes handle is already set, zooms Y to keep point onscreen
  Point2 minYZoom(Point1 yTarget);  // same as above but just returns the zoom value

  Point1 closestEnemyX(); // return pizza X if there are no enemies, NOT FLAMESKULLS NOT TERRAIN
  Point1 closestEnemyX(Point1 dir); // return pizza X if there are no enemies
  Logical enemyActive(); // return true if an enemy is onscreen
  Point2 pranceStartPt() {return puppyStartPt;}
  Point2 pranceEndPt() {return puppyHousePt;}
  SmallSkull* createAngrySkull(Point2 center);
  
  // nullifies unlockObj and sets currTopppingUnlock now
  UnlockFloater* tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet = false);
  
  // these are for slopes and spikes levels
  void createChunk(Coord1 chunkType, Coord1 variationID = 0);
  void createRandomChunk();
  
  void createSkiStart();
  void createSki2_1Chunk();
  void createSki3_1Chunk();
  void createSki4_1Chunk();
  void createSkiDropChunk();
  void createSkiGapChunk();
  void createSkiJumpChunk();
  void createSkiBouncegapChunk();
  void createSkiBouncejumpChunk();
  void createSkiDualcoinChunk();
  void createSkiSpikeplatformsChunk();
  void createSkiFlameskullChunk(Coord1 variationID);  // 0 = normal, 1 = possible unlock item
  void createSkiHillskullChunk();
  void createSkiJagskullChunk();
  void createSkiLongfallChunk();
  void createSkiSpikefallChunk();
  void createSkiStraightbreakChunk();
  void createSkiSwitchslopeChunk();
  void createSkiSwitchgapChunk();
  void createSkiSpikepathsChunk();
  void createSkiFallbreakChunk();
  void createSkiEndChunk();
  
  TerrainQuad* slopePiece4_1(const Point2& topLeft, Point1 layoutHandleY);  
  TerrainQuad* slopePiece3_1(const Point2& topLeft, Point1 layoutHandleY);  
  TerrainQuad* slopePiece2_1(const Point2& topLeft, Point1 layoutHandleY);  
  TerrainQuad* slopePieceBreakaway(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePiece3_1Short(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceBouncy(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceFlatLong(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceFlatShort(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceSpikePlatform(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceUphill(const Point2& topLeft, Point1 layoutHandleY);
  TerrainQuad* slopePieceTriangle(const Point2& topLeft, Point1 layoutHandleY);  // really a quad
  TerrainCircle* slopeSpikeball(const Point2& center, Point1 layoutHandleY);
  SkullSwitch* slopeSwitch(const Point2& center, Point1 layoutHandleY);
  SkiFlameSkull* slopeFlameskull(const Point2& center, Point1 layoutHandleY);  // stationary
  
  void spikeStartChunk();
  void spikeBumpChunk();
  void spikeDownhillChunk();
  void spikeDropChunk();
  void spikeFlatChunk();
  void spikeUphillChunk();
  void spikeBouncecirclesChunk();
  void spikeBouncedropsChunk();
  void spikeLongfallChunk();
  void spikeNospikeChunk();
  void spikeRiserChunk();
  void spikePillarChunk();
  void spikeSpikeballChunk();
  void spikeSpringChunk();
  void spikeStraightbounceChunk(Coord1 variationID);  // 1 for unlockable
  void spikeTransferblockChunk();
  void spikeTransferdropChunk();
  void spikeTransferspikeChunk();
  void spikeBouncestepsChunk();
  void spikeDownhillbumpChunk();
  void spikeDualriseChunk();
  void spikeRiseballsChunk();
  void spikeRisestepsChunk();
  void spikeBalldownhillChunk();
  void spikeBalljumpChunk();
  void spikeZigzagChunk();
  void spikeEndChunk();
  
  TerrainQuad* spikePiece2x2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceBounceCircle(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceBouncePlatform(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceDownhill4_1(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceDownhill4_3(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceDownhill8_1(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceDownhill12_4(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceDownhill16_4(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceFlatlong(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceFlatmed(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceFlatshort(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceNospike(Point2 topLeft, Point1 layoutHandleY = 0.0);
  PlatformRiser* spikePieceRiser(Point2 topLeft, Point1 layoutHandleY = 0.0);
  PlatformRiser* spikePieceRiserSmall(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePiecePillar(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill4_1(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill4_3(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill8_1(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill8_3(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill12_2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* spikePieceUphill16_4(Point2 topLeft, Point1 layoutHandleY = 0.0);
  BalanceSkull* spikeBalanceBall(Point2 center, Point1 layoutHandleY = 0.0);
  
  
  void puppyCrateChunk();
  void puppyDipChunk();
  void puppyDoghouseChunk();
  void puppyHillChunk(Coord1 variation);
  void puppySpikeHillChunk(Coord1 variation);
  void puppySpikeJumpChunk();
  void puppyTrickyJumpChunk();
  void puppyLavahillChunk();
  void puppyBigobjhillChunk(Coord1 variation);
  void puppyBouncepitChunk(Coord1 variation);
  void puppyCratepitChunk();
  void puppyDualfireballChunk();
  void puppyGroundtriangleChunk();
  void puppyLavalpoolChunk();
  void puppyObjstepsChunk(Coord1 variation);
  void puppyPillarsChunk(Coord1 variation);
  void puppyQuadspikeChunk(Coord1 variation);
  void puppySingletriChunk();
  void puppySpikeballBumpsChunk();
  void puppySpikeballhillChunk();
  void puppySpikeballpillarChunk(Coord1 variation);
  void puppySpikeballpitChunk();
  void puppySwingsquareChunk();
  void puppyTrianglepitChunk();
  void puppyWalljumpChunk();
  
  TerrainQuad* puppyPieceBounce(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceDownhill4_2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceDownhill4_4(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceDownhill8_2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceDownhill12_8(Point2 topLeft, Point1 layoutHandleY = 0.0);
  
  TerrainQuad* puppyPieceFlat8(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceFlat8(Point2 topLeft, 
                               Logical leftCorner, Coord1 leftEdges, 
                               Coord1 rightEdges, Logical rightCorner, 
                               Point1 layoutHandleY = 0.0);  
  TerrainQuad* puppyPieceFlat16(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceFlat16(Point2 topLeft, 
                                Logical leftCorner, Coord1 leftEdges, 
                                Coord1 rightEdges, Logical rightCorner, 
                                Point1 layoutHandleY = 0.0);
  
  TerrainQuad* puppyPieceSpike(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceUphill4_2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceUphill4_4(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceUphill8_2(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceUphill12_8(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainQuad* puppyPieceWall(Point2 topLeft, Point1 layoutHandleY = 0.0);
  TerrainCircle* puppyPieceSpikeball(Point2 center, Point1 layoutHandleY = 0.0);
  LavaPatch* puppyPieceLava(Point2 leftCenter, Logical small, Point1 layoutHandleY = 0.0);
  SquarePivot* puppyPiecePivotsquare(Point2 center, Point1 layoutHandleY = 0.0);  
  TrianglePivot* puppyPiecePivottri(Point2 pivot, Point1 layoutHandleY = 0.0);  
  
  // to ALLY list, adds endpoint to each
  TerrainQuad* createSlopeQuad(const Point2& pt0, const Point2& pt1, 
                               const Point2& pt2, const Point2& pt3,
                               Image* img = NULL);
  // to TERRAIN list
  TerrainQuad* createSumoQuad(const Point2& pt0, const Point2& pt1, 
                              const Point2& pt2, const Point2& pt3,
                              Image* img = NULL);
  
  // calculates the other corners based on image size and handle, assumes handle is at top left
  // imgTopRightY is the pixel coordinates of the top right corner, ignoring handle
  // includes endpoints
  TerrainQuad* createPuppyQuad(const Point2& worldTopLeft, Point1 imgTopRightY, Image* img, 
                               Point1 layoutHandleY = 0.0);
  
  SkullBomb* createBomb(Point2 xy, Point2 vel);
  
  // firstImg can be null
  void createTopography(const Point2* topPoints, Coord1 numPoints, 
                        Image* firstImg = NULL);
  void invisibleWallsStd(const Box& wallBox, Point1 wallThickness = 128.0);
  void invisibleWallsLR(Point2 leftGroundPt, Point2 rightGroundPt);
  
    // -1 start left, 0 start top, 1 start right
  PuppyMissile* arrowFromOffscreen(Coord1 dir, Logical puppyRemoval = false);
  
  // all include endpoint
  void puppyWithCage(Point2 bottomCenter);
  Mimic* createMimic(Point2 bottomCenter);
  JumpingFireball* createFireball(Point2 topCenter, Point1 layoutYHandle = 0.0);
  Mole* createMole(Point2 bottomCenter);

  // includes endpoint, use PuppyVariation type
  void createPuppyObj(Coord1 objType, Point2 bottomCenter, Point1 layoutHandleY = 0.0);
  TerrainQuad* createPuppyJunkCrate(Point2 center);
  
  PizzaGO* createSticky(Point2 topCenter, Point1 rotation = 0.0);
  SnowboardSkele* createSnowboarder(TerrainQuad* quad);
  void createCoin(Point2 center, Point1 layoutYHandle = 0.0);
  void createCoin(Point2 center, Point1 layoutYHandle, Coord1 val);
  void createFlag(Point2 location, Logical big, Point1 layoutYHandle = 0.0);
  PizzaGO* createMeat(Point2 center);
  
  static Point1 tiltMagnitude();  // -1.0 to 1.0
  static Point1 tiltMagnitudeVert();  // -1.0 to 1.0
  PizzaBackground* createBackgroundStd(Coord1 worldID);
  
  // ============== Override these
  virtual void loadRes() = 0; // call this first, loads any level-specific images including bg
  virtual void unloadRes() = 0;
  
  virtual void load();  // call this AFTER resources have loaded, calls loadMe()
  virtual void loadMe() {}    // create background, bg, set camera
  
  virtual void update();  
  virtual void redraw();

  virtual void updateGame();
  virtual void updateControls();
  virtual void updateLevelAI() {}  // only if level is active
  virtual void updateStatus() {}
  virtual void updateCamera();
  
  virtual void drawHUD() {}
  virtual void drawBGTerrain();
  virtual void drawShadows() {}
  virtual Point1 getGroundY(Point1 xCoord = 0.0) {return 480.0;} // GladiatorLevel and KarateLevel overrides
  virtual BalanceSkull* getCurrBall();  // for SpikeLevel
  virtual void balanceBallTouched(BalanceSkull* ball) {} // for SpikeLevel
  virtual Point1 maxSkullXVel() {return 150.0;}  // for SpikeLevel
  virtual void reportSurface(Coord1 surfaceBits);  // sumo stadium overrides for grass
  
  virtual Point1 playerTXVel();  // terminal x velocity
  virtual Logical slamDisabled() {return false;}  // karate uses this
  virtual Point1 bounceMult();  // for sticky and karate level
  virtual Logical jumpEnabled() {return true;}  // for bounce level to disable
  virtual Logical playerGroundStick() {return false;}  // for slopes, to stick to ground
  virtual void resetBounce() {} // for karate level
  
  virtual void enemyDefeated(PizzaGO* enemy) {}  // bounce, gladiator, angry use this
  virtual void pizzaDamaged() {} // karate uses this to reset bounce, gladiator for combos/tutorials
  virtual void landedOnEnemy(PizzaGO* enemy) {}  // for karate level
  virtual void puppyFound(Billy* puppy) {}  // for puppy level
  virtual void puppyReturned(Billy* puppy) {} // for puppy level
  virtual void sumoSlammedEmpty() {}  // for boss to dispense item
  virtual void sumoWeakpointHit() {}  // for weak level
  virtual void gongHit() {} // balance
  virtual void callback(ActionEvent* caller) {}

  // b2ContactListener
  void BeginContact(b2Contact* contact);
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
  void EndContact(b2Contact* contact);
  
  PizzaPlayer* findPlayer(PizzaGO* pgo1, PizzaGO* pgo2);
  
  template <typename PGOType>
  static PGOType* findObj(PizzaGO* obj1, PizzaGO* obj2, Coord1 type);
};

// ==================================== Backgrounds ================================ //

struct PizzaBackground : StdBackground
{
  PizzaLevel* level;
  Point1 totalWidth;
  Point2 trackBounds;
  Point2 bgBottomLeft;
  Logical foregroundDisabled;  // balance uses this, individual bgs must implement
  
  PizzaBackground(PizzaLevel* setLevel);
  
  void drawLandscapeTilesStd(Image* img, Point1 scaleFactor);
  void drawLandscapeTiles(Image* img, Point1 scaleFactor, Point1 scrollFactor);  // for computer
  void drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY);
  
  // timing uses this one
  static void drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY,
                                       const CameraStd& levelCam);
};

struct GraveyardBG : PizzaBackground
{
  GraveyardBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct BadlandsBG : PizzaBackground
{
  ArrayList<Point1> spireXVals;
  ArrayList<Coord1> spireImgIDs;
  
  BadlandsBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
  void drawForeground();
};

struct QuarryBG : PizzaBackground
{
  Logical useMidLayers;  // no on angry pyramid
  
  QuarryBG(PizzaLevel* setLevel, Logical withMiddleLayers = true);
  void load();
  void drawBackground();
};

struct PirateBG : PizzaBackground
{
  PirateBG(PizzaLevel* setLevel);
  void load();
  void drawIslandTiles(Image* img, Point1 scaleFactor);
  void drawBackground();
};

struct StadiumBG : PizzaBackground
{
  StadiumBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct NuclearBG : PizzaBackground
{
  NuclearBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct ComputerBG : PizzaBackground
{
  ComputerBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct CarnivalBG : PizzaBackground
{
  CarnivalBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct MoonBG : PizzaBackground
{
  MoonBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct MeatBG : PizzaBackground
{
  ArrayList<Point1> spireXVals;
  ArrayList<Coord1> spireImgIDs;
  
  static const Coord1 NUM_MEATS = 12;

  MeatBG(PizzaLevel* setLevel);
  void load();
  
  void drawBackground();
  void drawForeground();
};

struct CloudBG : PizzaBackground
{
  CloudBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
  void drawForeground();
};

struct MarketBG : PizzaBackground
{
  DataList<Point1> fruitXVals;
  DataList<Coord1> fruitImgIDs;

  MarketBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
  void drawForeground();  
};

// === start special bgs : PizzaBackground

struct KarateBG : PizzaBackground
{
  KarateBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct BounceBG : PizzaBackground
{
  BounceBG(PizzaLevel* setLevel);
  void load();
  void drawBackground();
};

struct PuppyBG : PizzaBackground
{
  ScrollingImage dust;
  Point1 oldCamX;
  
  PuppyBG(PizzaLevel* setLevel);
  void load();
  void update();
  void drawBackground();
  void drawForeground();
};

struct Snowflake : VisRectangular, Effect, ActionListener
{
  DataList<Image*>* weatherSet;  // does not own
  PizzaLevel* level;
  
  Point1 sizePercent;  // should be [0.25-1.0]
  Point2 vel;
  Point1 rotVel;
  TimerFn checkTimer;
  
  static const Point2 X_SPEED_RANGE; // assumes sizePercent 1.0
  static const Point2 Y_SPEED_RANGE; // assumes sizePercent 1.0
  
  Snowflake();
  
  void updateMe();
  void redrawMe();
  void reset(Logical firstReset);
  void callback(ActionEvent* caller);
};

struct SlopesBG : PizzaBackground
{
  ArrayList<Snowflake> snowflakes;
  
  SlopesBG(PizzaLevel* setLevel);
  void load();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== Gladiator ================================ //

struct GladiatorLevel : PizzaLevel
{
  enum EnemyType
  {
    ENEMY_SPEARMAN_SIDE,
    ENEMY_SPEARMAN_VERT,
    ENEMY_GARGOYLE,
    ENEMY_SPINY,
    ENEMY_SKELOSTRICH_SIDE,
    
    ENEMY_SKELOSTRICH_VERT,
    ENEMY_CUPID,
    ENEMY_NAGA,
    ENEMY_WISP,
    ENEMY_ROBOT,
    
    ENEMY_GIANT,
    ENEMY_FLAMESKULL,
    ENEMY_UNARMED,
    ENEMY_END_SKULLS,  // this is for the ending
    ENEMY_RABBIT,
    
    ENEMY_TAIL,
    ENEMY_DISK,
    
    NUM_ENEMY_TYPES
  };
  
  ArrayList<EnemyType> enemiesEnabled;
  ArrayList<Point1> enemyWeights;
  ArrayList<Coord2> enemyCounts;
  Point1 groundY;
  
  Coord1 numWaves;
  Coord1 currWave;
  Coord1 currSide;
  Coord1 currCombo;
  
  // it's fine to change these in subclasses
  Point1 worldSpawnPad;
  Point1 camSpawnPad; 
  
  GladiatorLevel();
  virtual ~GladiatorLevel() {}  

  static void loadEnemy(EnemyType enemyID);
  void randomWave();
  
  void createWorldStd();
  void createGroup(Coord1 enemyType, Coord1 count = 1);
  PizzaGO* createSingle(Coord1 enemyType, Coord2 countData); // countData[i, num]
  PizzaGO* createSingle(Coord1 enemyType, Coord1 groupID = 1);
  Point1 spawnXStd(Coord1 side, Point1 lerpMin = 0.0);  // side 0 left 1 right
  Point1 spawnYStd(Coord1 index, Coord1 number);  // for enemies in track bounds
  Point1 spawnYStd(Coord1 index, Coord1 number, Point2 bounds);  // for enemies in track bounds
  void placeSticky(Point1 courseXPercent);
  
  void setProb(Coord1 enemyType, Point1 weight);
  void setProb(Coord1 enemyType, Coord1 lowCount, Coord1 highCount);
  void setProb(Coord1 enemyType, Point1 weight, Coord1 lowCount, Coord1 highCount);
    
  // new virtuals
  virtual void createWorld(const Box& setWorldBox);
  virtual void worldCreated() {}
  virtual void createWave() {}
  
  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void loadMe();
  
  virtual void updateCamera();
  virtual void updateLevelAI();
  
  virtual void updateStatus();
  
  virtual void enemyDefeated(PizzaGO* enemy);
  virtual void pizzaDamaged();
  
  void drawBGTerrain();
  void drawShadows();
  Point1 getGroundY(Point1 xCoord = 0.0);
  Logical playerGroundStick() {return true;}
};

struct GLLevelTutorial;

// THIS IS MEANT TO BE ON THE HEAP, created during level->loadMe()
struct TutorialScript : ActionEvent, ActionListener
{
  GLLevelTutorial* level;
  ActionQueue script;
  ActionList actions;
  
  TimerFn genTimer0;
  TimerFn genTimer1;
  TimerFn genTimer2;
  ArrayList<VisRectangular> textLines;
  
  TutorialScript(GLLevelTutorial* setLevel);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct GLLevelTutorial : GladiatorLevel
{
  Coord1 cinemaState;
  
  BoxPlayer* box;
  ArrayList<SkeletonTutorial*> skeles;
  
  ActionQueue growScript;
  Point1 growStartScale;
  Point1 growDur;
  Point1 growScale;
  Point1 growOffset;
  Point2 growData;
  
   // does not own any of these
  ImageEffect
  *tapJump,
  *tiltRoll,
  *crushEnemies;
  
  Point2 tapJumpData;
  
  enum
  {
    CINEMA,
    BOX_PLAY,
    PIZZA_GROW,
    FULL_PLAY
  };
  
  GLLevelTutorial();
  
  void cinemaDone();
  void boxDone();

  // overrides 
  void loadRes();
  void unloadRes();  // deletes music
  void loadMe();
  
  void updateControls();
  void updateCamera();
  void updateLevelAI();
  void updateStatus(); 
  void createWave();
  
  Logical startsWithMusic() {return false;}
  Logical pauseGUIEnabled();
  Logical usesHealthBar() {return false;}
  void callback(ActionEvent* caller);
};

struct Pebble : VisRectangular, Effect, ActionListener
{
  TimerFn visibleTimer;
  LinearFn fader;
  
  Point2 vel;
  
  Pebble(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct DustEffect : ActionEvent
{
  ActionList dustParticles;
  
  DustEffect(Point2 impactXY, Point1 impactAngle);
  void updateMe();
  void redrawMe();
};

struct DustParticle : VisRectangular, ActionEvent, ActionListener
{
  Point2 origin;
  Point1 distance;
  Point1 angle;
  
  DeAccelFn scaler;
  DeAccelFn mover;
  LinearFn fader;
  
  DustParticle(Point2 startXY, Point1 flyAngle, Point1 finalScale);
  
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct GLLevelSpearmen : GladiatorLevel
{
  GLLevelSpearmen();
  void createWave();
};

struct GLLevelGargoyle : GladiatorLevel
{  
  GLLevelGargoyle();
  void createWave();
  void updateLevelAI();
};

struct GLLevelSpiny : GladiatorLevel
{
  GLLevelSpiny();
  void createWave(); 
  void pizzaDamaged();
};

struct GLLevelOstrichV : GladiatorLevel
{
  GLLevelOstrichV();
  void createWave();  
};

struct GLLevelSticky : GladiatorLevel
{
  GLLevelSticky();
  void worldCreated();
  void createWave();  
};

struct GLLevelGiant : GladiatorLevel
{
  GLLevelGiant();
  void worldCreated();
  void createWave();  
};

struct GLLevelOstrichH : GladiatorLevel
{
  GLLevelOstrichH();
  void createWave();
};

struct GLLevelCupid : GladiatorLevel
{
  GLLevelCupid();
  void createWave();
};

struct GLLevelFlameskull : GladiatorLevel
{
  GLLevelFlameskull();
  void createWave();
};

struct GLLevelRobot : GladiatorLevel
{
  GLLevelRobot();
  void worldCreated();
  void createWave();  
};

struct GLLevelWisp : GladiatorLevel
{
  GLLevelWisp();
  void createWave();  
};

struct GLLevelGrav : GladiatorLevel
{
  GLLevelGrav();
  void worldCreated();
  void createWave();
};

struct GLLevelNaga : GladiatorLevel
{
  GLLevelNaga();
  void worldCreated();
  void createWave();  
};

struct GLLevelMix1 : GladiatorLevel
{
  GLLevelMix1();
  void createWave();
};

struct GLLevelStickyMix : GladiatorLevel
{
  GLLevelStickyMix();
  void worldCreated();
  void createWave();
};

struct GLLevelRabbit : GladiatorLevel
{
  GLLevelRabbit();
  void createWave();
};

struct GLLevelTail : GladiatorLevel
{
  GLLevelTail();
  void createWave();
};

struct GLLevelDisk : GladiatorLevel
{
  GLLevelDisk();
  void createWave();
};

struct GLLevelEnding;

// THIS IS MEANT TO BE ON THE HEAP, created during level->loadMe()
struct EndingScript : ActionEvent, ActionListener
{
  GLLevelEnding* level;
  
  ActionQueue script;
  ActionList actions;
  ArrayList<VisRectangular> textLines;
  
  EndingScript(GLLevelEnding* setLevel);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct GLLevelEnding : GladiatorLevel
{  
  Coord1 endState;
  Coord1 currBoneType;
  
  enum
  {
    PLAYING_VERSES,
    FIGHTING_BIGSKULL,
    SHOWING_END,
    READY_FOR_CLICK,
    TOTALLY_FINISHED
  };
    
  GLLevelEnding();
  void preload();
  void loadRes();
  void unloadRes();
  void loadMe();
  
  void updateLevelAI();
  void createWave();
  void updateStatus() {}
  void enemyDefeated(PizzaGO* enemy);  // only final skull should call this
  Logical pauseGUIEnabled() {return false;}
  Logical usesHealthBar() {return false;}

  void callback(ActionEvent* caller);
};

struct GLLevelLiteEnding;

// THIS IS MEANT TO BE ON THE HEAP, created during level->loadMe()
struct LiteEndingScript : ActionEvent, ActionListener
{
  ActionQueue script;
  ActionList actions;
  ArrayList<VisRectangular> textLines;
  
  LiteEndingScript(GLLevelLiteEnding* setLevel);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct GLLevelLiteEnding : GladiatorLevel
{  
  Coord1 endState;
  Coord1 currBoneType;
  
  enum
  {
    PLAYING_VERSES,
    WAITING_FOR_FINAL,
    FIGHTING_FINAL
  };
  
  GLLevelLiteEnding();
  void preload();
  void loadRes();
  void unloadRes();
  void loadMe();
  
  void updateLevelAI();
  void createWave();
  void updateStatus() {}
  void enemyDefeated(PizzaGO* enemy);  // only final skull should call this
  Logical pauseGUIEnabled() {return false;}
  Logical usesHealthBar() {return false;}
  
  void callback(ActionEvent* caller);
};

// ==================================== SumoLevel ================================ //

struct SumoLevel : PizzaLevel
{
  SumoBoss* boss;
  Point1 camYPad;  // pirate changes this slightly for bombs
  
  TimerFn tauntStartTimer;
  TimerFn bombTimer;
  TimerFn arrowTimer;
  Coord1 grassTouchLeft;  // frames of touching grass before throwing 

  TimerFn coinTimer;
  Coord1 coinGroupsLeft;
  
  Logical useRods;
  ArrayList<TerrainQuad*> rodList;  // DOES NOT OWN
  ArrayList<Point1> rodMoveVals;
  ArrayList<Point2> rodMoveData;

  Logical windyAchTried;  // will try once per play
  TimerFn winTimer;
  
  SumoLevel();
  virtual ~SumoLevel() {}
  
  void checkWindyAch();
  
  // new virtuals
  virtual Point2 createArena() = 0;  // returns [leftmostX, rightmostX]
  virtual void createBoss();
  
  // overrides, pass through virtuals  
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  
  virtual void loadMe();
  virtual void updateLevelAI();
  
  void reportSurface(Coord1 surfaceBits);
  void addRod(TerrainQuad* rod);
  void updateRods();
  void updateCamera();
  void updateStatus();
  Coord1 calcWinBonus();
  void callback(ActionEvent* caller);
};

struct SULevelGraveyard : SumoLevel
{
  Point2 createArena();
  void loadMe();
};

struct SULevelBadlands : SumoLevel
{
  Point2 createArena();
};

struct SULevelQuarry : SumoLevel
{
  Point2 createArena();
  void createBoss();
};

struct SULevelPirate : SumoLevel
{
  TerrainQuad* boat;  // DOES NOT OWN
  Point1 rotVal;
  Point2 rotData;
  
  SULevelPirate();
  void updateLevelAI();
  
  Point2 createArena();
  void callback(ActionEvent* caller);
};

struct SULevelStadium : SumoLevel
{
  Logical triedDispenseItem;  // for the unlockable
  
  SULevelStadium();
  Point2 createArena();
  void sumoSlammedEmpty();
};

struct SULevelNuclear : SumoLevel
{  
  Point2 createArena();
};

struct SULevelComputer : SumoLevel
{
  Point2 createArena();
  void createBoss();
};

struct SULevelCarnival : SumoLevel
{
  SULevelCarnival() : SumoLevel() {musicFilename = "carnival.ima4";}
  Point2 createArena();
};

struct SULevelMoon : SumoLevel
{
  Point2 createArena();
};

struct SULevelMeat : SumoLevel
{
  Point2 createArena();
};

struct SULevelCloud : SumoLevel
{
  Point2 createArena();
};

struct SULevelMarket : SumoLevel
{
  Coord1 weaksHit;
  
  SULevelMarket();
  
  Point2 createArena();
  void createBoss();
  void sumoWeakpointHit();
};

// ================================ Angry ============================= //

struct AngryLevel : PizzaLevel
{
  Coord1 timeAllowed;
  
  VisRectangular skullBarFrame;
  VisRectangular skullBarFill;
  Point1 skullBarWeight;
  
  AngryLevel();
  
  void addBlock(PizzaGO* block);
  
  // new virtuals
  virtual void createBackground() {}
  virtual void loadImgs() {}
  virtual void createWorld() {}
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void loadMe();
  
  void updateCamera();
  void updateStatus();
  virtual void updateLevelAI();  // stonehenge overrides for tutorial
  void drawHUD();
  void enemyDefeated(PizzaGO* enemy);
  virtual Coord1 calcWinBonus();
};

struct ANLevelStonehenge : AngryLevel
{
  TimerFn tutTimer;
  
  ANLevelStonehenge() : AngryLevel(), tutTimer(8.0, this) {}
  void createBackground();
  void loadImgs();
  void createWorld();
  void updateLevelAI();
  void callback(ActionEvent* caller);
};

struct ANLevelCastle : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();
};

struct ANLevelCabin : AngryLevel
{
  ANLevelCabin() : AngryLevel() {musicFilename = "pirate.ima4";}
  void createBackground();
  void loadImgs();
  void createWorld();
};

struct ANLevelPyramid : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();

  // filled bits are top to bot left to right only valid rectangles
  void createPyramid(Point2 topCenter, Coord1 numRows, UInt2 filledBits);
};

struct ANLevelFort : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();
};

struct ANLevelIgloo : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();
  
  void createIglooCorners(Point2 leftTL, Coord1 blockGap, Coord1 rows);
};

struct ANLevelGlass : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();  
};

struct ANLevelPagoda : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();
  
  void createPagoda(Point2 topCenter, Coord1 numLevels, UInt2 unfilledBits);
};

struct ANLevelCave : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld(); 
};

struct ANLevelCloud : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld(); 
};

struct ANLevelMarket : AngryLevel
{
  void createBackground();
  void loadImgs();
  void createWorld();
};

// ================================ Slopes ============================= //

struct SlopeLevel : PizzaLevel
{
  Coord1 numChunks;
  Point1 camYOffset;  // helps camera pan during jump
  Point2 maxCameraBottom;  // not including zoom
  Coord1 currPlayerChunk;  // where the player is to determine losing
  
  SlopeLevel();
  
  virtual void setInitialWeights();
  virtual void generateNextChunk(Coord1 chunkNum);  // use this to customize specific chunks
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
    
  void loadMe();
  Point1 playerTXVel();
  Logical playerGroundStick() {return true;}
  void updateCamera();
  void updateStatus();
  Coord1 calcWinBonus();
};

struct SLLevelA : SlopeLevel
{
  Point1 
  slowX,
  readyX,
  jumpX;
  
  Logical 
  showedSlow,
  showedReady,
  showedJump;
  
  SLLevelA();
  void generateNextChunk(Coord1 chunkNum);
  void updateLevelAI();
};

struct SLLevelB : SlopeLevel
{
  SLLevelB() : SlopeLevel() {numChunks = 13;}
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelC : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelD : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelE : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelF : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelG : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelH : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelI : SlopeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct SLLevelJ : SlopeLevel
{
  SLLevelJ() : SlopeLevel() {numChunks = 25;}
  void generateNextChunk(Coord1 chunkNum);
};

// ================================ KarateLevel ============================= //
  
struct KarateLevel : PizzaLevel
{
  Coord1 numBoards;
  Coord1 numMids;  // default = numBoards + 1
  ArrayList<TerrainQuad*> midList;  // does not own
  ArrayList<TerrainQuad*> boardList;  // does not own
  Point1 groundY;
  
  TimerFn standTimer;  // if player stands for this time, boost resets
  TimerFn justJumpedTimer;  // this is to disable slam
  TimerFn tutTimer;  // KALevelBounce uses 
  
  Coord1 numBoost;
  Coord1 boardsBroken;  // starts -1
  
  KarateLevel();
  
  Coord1 boardsToBreak();  // call this right upon striking  
  
  // new virtuals
  virtual void createBrains() = 0;

  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  virtual void loadMe();
  void updateCamera();
  void updateLevelAI();
  void updateStatus();
  
  Logical slamDisabled();
  void pizzaDamaged();
  virtual void resetBounce();  // first level overrides for tutorial
  virtual Point1 bounceMult();  // mummy brain level overrides
  void landedOnEnemy(PizzaGO* enemy);
  Point1 getGroundY(Point1 xCoord);  // returns the y of the middle sections
  Coord1 calcWinBonus();
  void callback(ActionEvent* caller);
};

struct KALevelStill : KarateLevel
{
  KALevelStill();
  void createBrains();
  void resetBounce();
};

struct KALevelBounce : KarateLevel
{
  void createBrains();
};

struct KALevelWalk : KarateLevel
{
  void createBrains();
};

struct KALevelFly : KarateLevel
{
  void createBrains();
};

struct KALevelHop : KarateLevel
{
  void createBrains();
};

struct KALevelSpear : KarateLevel
{
  void createBrains();
};

struct KALevelIllusion : KarateLevel
{
  void createBrains();
};

struct KALevelCircle : KarateLevel
{
  void createBrains();
};

struct KALevelGravity : KarateLevel
{
  void createBrains();
};

struct KALevelMix : KarateLevel
{
  void createBrains();
};

struct KALevelMummy : KarateLevel
{
  void createBrains();
};

// ================================ SpikeLevel ============================= //

struct SpikeLevel : PizzaLevel
{
  Coord1 numChunks;
  
  SpikeLevel();
  virtual ~SpikeLevel() {}
  
  virtual void generateNextChunk(Coord1 chunkNum);
  virtual void setInitialWeights();

  void updateCameraStart();
  void updateCameraStd();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void loadMe();
  void updateLevelAI();
  void updateCamera();
  void updateStatus();
  void balanceBallTouched(BalanceSkull* ball);
  void gongHit();
  Coord1 calcWinBonus();
  void callback(ActionEvent* caller);
};
  
struct BALevelA : SpikeLevel
{
  void setInitialWeights();
  void generateNextChunk(Coord1 chunkNum);
  Point1 maxSkullXVel() {return 100.0;}
};

struct BALevelB : SpikeLevel
{
  void generateNextChunk(Coord1 chunkNum);
  Point1 maxSkullXVel() {return 125.0;}
};

struct BALevelC : SpikeLevel
{
  BALevelC();
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelD : SpikeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelE : SpikeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelF : SpikeLevel
{
  BALevelF();
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelG : SpikeLevel
{
  BALevelG() : SpikeLevel() {musicFilename = "carnival.ima4";}
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelH : SpikeLevel
{
  void generateNextChunk(Coord1 chunkNum);
};

struct BALevelI : SpikeLevel
{
  BALevelI();
  void generateNextChunk(Coord1 chunkNum);
};


// ================================ Bounce ============================= //


struct BounceLevel : PizzaLevel
{
  AngrySun* sun;
  TimerFn arrowTimer;
  TimerFn turnTimer;
  Coord1 enemyGoal;
  Coord1 enemyNum;
  Coord1 arrowCount;  // used for patterns
  Logical warnedOnce;  // warn for the first arrow
  
  VisRectangular skullBarFrame;
  VisRectangular skullBarFill;
  Point1 skullBarWeight;
  Point1 arrowYOff;  // this is because the platforms spawn higher on ipad
  
  static const Point1 BOUNCE_MAX_VEL;
  
  BounceLevel();

  BounceComet* createBounceComet(Logical startLeft, Point1 timeToNext);
  BounceComet* createBounceEnergy(Logical startLeft, Point1 timeToNext);
  BounceComet* createBounceComet(Logical startLeft, Point1 yVal, Point1 timeToNext,
                                 Logical energyBall = false);
  
  // new virtuals
  virtual void createWorld() = 0;
  virtual void enemyMaker();
  virtual PizzaGO* createNextEnemy();
  virtual PhysicalCoin* createNextCoin();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void loadMe();
  void updateCamera();
  void updateStatus();
  void updateLevelAI();
  void drawHUD();
  void enemyDefeated(PizzaGO* enemy);
  Coord1 calcWinBonus();

  virtual void callback(ActionEvent* caller);
};

struct BOLevelEasy : BounceLevel
{
  void createWorld();
};

struct BOLevelArrow : BounceLevel
{
  void createWorld();
  void callback(ActionEvent* caller);
};

struct BOLevelHeavy : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);
};

struct BOLevelSquish : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);  
};

struct BOLevelBig : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);
};

struct BOLevelLightning : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);
};

struct BOLevelExplode : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);  
};

struct BOLevelSun : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  Point1 healthMasterAlpha() {return 0.7;}
  void callback(ActionEvent* caller);    
};

struct BOLevelStreaker : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);  
};

struct BOLevelEnergy : BounceLevel
{
  void createWorld();
  PizzaGO* createNextEnemy();
  void callback(ActionEvent* caller);
};

// ================================ PuppyLevel ============================= //

struct PuppyLevel : PizzaLevel
{
  Point1 objectiveDir;
  
  TimerFn crateTimer; // for crate rain
  Coord1 cratesMade;
  
  TimerFn arrowTimer;
  TimerFn bombTimer;
  
  Coord1 billyState;
  TimerFn billyJumpTimer;
  Coord1 currJumpingBilly; // this is the NEXT billy to jump off the pizza
  
  AngrySun* sun;
  
  enum
  {
    WAIT_FOR_RETURN,
    BILLYS_RETURNING
  };
  
  PuppyLevel();
  
  // new virtuals
  virtual void createLevel();
  virtual void directionChanged() {}  // called after turn, mostly used to turn on crate timer
  virtual void puppyWasFound() {}
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void loadMe();
  void updateCamera();
  void updateLevelAI();
  void updateStatus();
  void drawHUD();
  
  void puppyFound(Billy* puppy);   // called when it collides with pizza
  void puppyReturned(Billy* puppy);  // called when it finishes prance into dog house
  virtual Coord1 calcWinBonus();
  
  void callback(ActionEvent* caller);
};

struct PULevelEasy : PuppyLevel
{
  void createLevel();
  void puppyWasFound();
  void directionChanged();
};

struct PULevelSpikepits : PuppyLevel
{
  void createLevel();
  void directionChanged();
};

struct PULevelMoles : PuppyLevel
{
  void createLevel();
};

struct PULevelSpikeballs : PuppyLevel
{
  void createLevel();
};

struct PULevelFallpits : PuppyLevel
{
  void createLevel();
};

struct PULevelFireballs : PuppyLevel
{
  void createLevel();
};

struct PULevelBombs : PuppyLevel
{
  void createLevel();
};

struct PULevelMimics : PuppyLevel
{
  void createLevel();
};

struct PULevelArrows : PuppyLevel
{
  void createLevel();
};

struct PULevelMix : PuppyLevel
{
  void createLevel();
};

struct PULevelSun : PuppyLevel
{
  void createLevel();
  Point1 healthMasterAlpha() {return 0.7;}
};

// ==================================== LakeLevel ================================ //

struct LakePlant : VisRectangular, ActionListener
{
  PizzaWarper warper;
  TimerFn timer;
  Point1 weight;
  Coord1 frameTarget;
  
  LakePlant();
  void init(Coord1 imgIndex, Point2 location);
  void update();
  void redraw();
  void chooseTarget();
  void callback(ActionEvent* caller);
};

struct IndexPt
{
  Coord1 index;
  Point2 xy;
  
  IndexPt(Coord1 setIndex, Point1 setX, Point1 setY);
};

struct LakeObjman : ObjmanStd<LakeGO>
{
  Point1 tide;
  
  LakeObjman();
  void updateObjects();
};

struct LakeBackground : StdBackground
{
  LakeLevel* level;

  ArrayList<LakePlant>
  farPlants,
  closePlants,
  fgPlants;  

  static const Point1 WORLD_BOTTOM;
  
  static const Point2 
  CLOSE_CORAL_PTS[],
  FAR_PLANT_PTS[],
  CLOSE_PLANT_PTS[],
  FG_PLANT_PTS[];
  
  static const IndexPt 
  FAR_CORAL_IPTS[],
  FG_CORAL_IPTS[];

  LakeBackground(LakeLevel* setLevel);
  
  void load();
  void update();
  void drawBackground();
  void drawForeground();
  
  void drawOnscreen(Image* img, Point2 location);
};

struct LakeLevel : PizzaLevelInterface, ActionListener
{
  LakeObjman objman;
  LakePlayer* player;
  LakeBackground bg;
  
  CameraStd camera;
  TimerFn cameraStaller;  // makes the zoom wait slightly after size up

  TimerFn bubbleTimer;
  TimerFn harpoonTimer;
  Point1 tideMax;
  Point2 tideData;
  
  Coord1 maxHearts;
  Coord1 pearlsEaten;  // for achievement
  
  // all 0.0-1.0
  Point1 runnerPercent;
  Point1 chasePercent;
  Point1 poisonPercent;
  
  LakeLevel();
  virtual ~LakeLevel();
  
  void playerGrew(Coord1 newSize);
  void updateControls();
  void updateCamera();
  
  void addEnemy(LakeGO* fish);
  void addAlly(LakeGO* ally);
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  void ateFish(LakeGO* fish);
  void playerDamaged(Logical instantLose = false);
  void drawHearts();
  Point2 heartPos(Coord1 heartID);
  
  void placeLakeGOStd(LakeGO* lgo);  // sets the position offscreen
  LakeFish* createNormalFish(Coord1 size);
  LakeFish* createNormalFish(const LakeFish& original);
  LakePuffer* createPuffer();
  LakeSwordfish* createSwordfish();
  Starfish* createStarfish();
  LakeBarrel* createBarrel();
  LakeEel* createEel();
  LakePearl* createPearl();
  LakeBubble* createBubble();
  LakeCoin* createCoin();
  LakeUnlockable* tryCreateUnlockable(Point2 center, Coord2 toppingID);
  
  virtual Coord1 calcWinBonus();
  void callback(ActionEvent* caller);
  
  // new virtuals
  virtual void populateWorld() = 0;
  
  // overrides
  void loadRes();
  void unloadRes();
  void load();
  Logical shouldLoadPizza() {return false;}
  Logical usesHealthBar() {return false;}
  
  void update();
  void redraw();
};

struct LALevelEasy : LakeLevel
{
  void populateWorld();
};

struct LALevelPearl : LakeLevel
{
  void populateWorld();
};

struct LALevelStarfish : LakeLevel
{
  void populateWorld();
};

struct LALevelPuffer : LakeLevel
{
  void populateWorld();
};

struct LALevelTide : LakeLevel
{
  void populateWorld();
};

struct LALevelSwordfish : LakeLevel
{
  void populateWorld();
};

struct LALevelPoison : LakeLevel
{
  void populateWorld();
};

struct LALevelHarpoon : LakeLevel
{
  void populateWorld();
};

struct LALevelBarrel : LakeLevel
{
  void populateWorld();
};

struct LALevelEel : LakeLevel
{
  void populateWorld();
};

// ==================================== WreckLevel ================================ //

struct WreckLevel;

struct WreckObjman : ObjmanStd<WreckGO>
{
  WreckObjman() : ObjmanStd<WreckGO>() {}
};

struct WreckBackground : StdBackground
{
  WreckLevel* level;
  ArrayList<ScrollingImage> clouds;
  Logical isInitialized;
  
  WreckBackground(WreckLevel* setLevel);  
  void init();

  void update();
  void drawBackground();
  void drawForeground();
};

struct WreckLevel : PizzaLevelInterface, ActionListener, b2ContactListener
{
  WreckObjman objman;
  WreckPlayer* player;
  WreckChain* chain;
  WreckBackground* bg;
  
  ActionList debrisList;
  Coord1 windowsDestroyed;  // for the achievement
  
  Coord1 numLevels;
  Coord1 bottomLevel; // number of vertical windows created
  Point1 bottomY;  // this is the bottom of the created area

  Point2 buildingSize;
  Point2 worldSize;
  Point2 emptyRange;  // x area not filled by buildings
  Point1 chunkHeight;
  
  ArrayList<WreckBuilding*> buildingCache;  // 0-2 left 3-5 right
  Point2 currObjOffset;  // added to all free objects made with the "create" methods

  ArrayList<Point1> yTriggers;
  Coord1 currLocation;

  TimerFn junkTimer;
  Coord1 junkType;  // see JunkType
  
  TimerFn powerTimer;
  TimerFn sparkTimer;
  Point1 currMaxVel;
  
  static const Coord1 LEVELS_PER_CHUNK = 3;
  static const Logical LEFT = true, RIGHT = false;
  
  enum JunkType
  {
    EASY_JUNK,
    BOTH_JUNK,
    HARD_JUNK
  };
  
  WreckLevel();
  virtual ~WreckLevel();
  
  void updateControls();
  void updateCamera();
  void updateStatus();
  
  void addAction(ActionEvent* ae);
  void addDebris(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  Point1 getChainVel();
  Coord1 cutAttack(const Circle& atkCircle);  // miss = 0 damage player = 1 sever chain = 2
  void gotPowerup();
  Logical powerActive();
  WreckGO* objectAtPt(Coord1 objType, Point2 pt);  // returns the first found, uses circles
  WreckGO* buildingAtPt(Point2 pt);  // returns the first found, uses rects
  void smallShake();
  void bigShake();
  
  void addLocation(Point1 yCoord);  // add them IN ORDER
  void checkLocations();
  
  void buildToBottom();
  void buildWindowChunk();  // 3 vertical windows worth
  
  WreckBuilding* createBuilding(Logical leftSide, Coord1 numOffset,
                                Logical burning = false, Logical strong = false);
  WreckSkeleEasy* createSkeleEasy(WreckBuilding* building);
  WreckSkele* createBoneThrower(WreckBuilding* building);
  WreckCutterSkele* createCutter(WreckBuilding* building);
  WreckGirl* createGirl(WreckBuilding* building);
  void createSkeleCatcher(WreckBuilding* leftBuilding, WreckBuilding* rightBuilding, 
                          Logical leftHasBall);
  WreckBatter* createBatter(WreckBuilding* building);
  WreckCreature* createClimber(Point1 yPos, Logical onLeft);
  WreckShocker* createHLine(Point2 xSpacePercents, Point1 yPos);
  WreckShocker* createLine(Point2 pos1, Point2 pos2);  // pos = [xPercent, yValue]
  WreckPowerup* createPowerOff(Point2 center);
  WreckCoin* createCoinOff(Point2 center);
  WreckScreenShock* createFullShock(Point1 yCoord);
  WreckFloatSquare* createFloater(Point2 center, Point1 rotation);
  WreckBird* createFlier(Point2 center);
  WreckCoin* createCoin(Point2 xy, Coord1 val = 0);
  WreckUnlockable* tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet = false);
  
  void arcLeftCoins(Point1 x, Point1 y);
  void arcRightCoins(Point1 x, Point1 y);
  void clumpCoins(Point1 x, Point1 y);
  void diagLeftCoins(Point1 x, Point1 y);
  void diagRightCoins(Point1 x, Point1 y);
  void diamondCoins(Point1 x, Point1 y);
  void bowCoins(Point1 x, Point1 y);
  void flatCoins(Point1 x, Point1 y);
  void ringCoins(Point1 x, Point1 y);
  void smallArcLeftCoins(Point1 x, Point1 y);
  void smallArcRightCoins(Point1 x, Point1 y);
  void straight3Coins(Point1 x, Point1 y);
  void straight6Coins(Point1 x, Point1 y);
  void singleCoin(Point1 x, Point1 y);
  
  void addEnemy(WreckGO* enemy, Logical addLast = true);
  void addAlly(WreckGO* ally);
  
  WreckBuilding* leftBuilding(Coord1 fromTop) {return buildingCache[fromTop];};
  WreckBuilding* rightBuilding(Coord1 fromTop) {return buildingCache[fromTop + LEVELS_PER_CHUNK];};
  
  // new virtuals
  virtual void updateGame();
  virtual void objectDefeated(WreckGO* obj);  // for enemies (not buildings)
  virtual void buildingDefeated(WreckGO* obj);  // only buildings
  virtual void playerDamaged() {}
  
  // bottomLevel is NOT updated yet, but bottomY is
  virtual void buildNextChunk(Coord1 chunkNum);
  virtual void locationTrigger(Coord1 triggerNum) {}
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  void load();
  
  void update();
  void redraw();
  
  virtual Coord1 calcWinBonus();
  void callback(ActionEvent* caller);
  
  // b2ContactListener
  void BeginContact(b2Contact* contact);
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
  void EndContact(b2Contact* contact);
};

struct WRLevelLines : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
  void playerDamaged();
};

struct WRLevelCivilian : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelBaseball : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelClimber : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelJunk : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelFlier : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelHardwall : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
  void locationTrigger(Coord1 triggerNum);
};

struct WRLevelUFO : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelCutter : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};

struct WRLevelMix : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
  void locationTrigger(Coord1 triggerNum);
};

struct WRLevelBatter : WreckLevel
{
  void buildNextChunk(Coord1 chunkNum);
};


// ==================================== PlaneLevel ================================ //

struct PlaneLevel;

struct PlaneObjman : ObjmanStd<PlaneGO>
{
  PlaneObjman() : ObjmanStd<PlaneGO>() {}
};

struct PlaneBG : StdBackground
{
  ArrayList<ScrollingImage> layers;
  
  PlaneBG();
  void load();
  void init();  // call this sometime after load() but before update and draw
  void update();
  void drawBackground();
};

struct PlaneLevel : PizzaLevelInterface, ActionListener, b2ContactListener
{
  enum PlaneType 
  {
    PLENEMY_FLAMESKULL,
    PLENEMY_CUPID,
    PLENEMY_DEMON,
    PLENEMY_BRAIN,
    PLENEMY_WISP,
    PLENEMY_RABBIT,
    PLENEMY_NAGA,
    PLENEMY_NODES,
    
    NUM_PLENEMIES
  };
  
  PlaneObjman objman;
  PlaneBG bg;
  PlanePlayer* player;  // owns
  
  ActionList debrisList;
  ActionQueue spawnQueue;
  
  ArrayList<Point1> wavePercents;  // default 0.0
  ArrayList<Coord2> enemiesPerGroup;  // [min, max] default 1
  Coord2 groupsPerWave;  // [min, max] default 1
  
  Coord1 lastWave;  // waves are [0...lastWave]
  Coord1 currWave;

  PlaneLevel();
  virtual ~PlaneLevel();
  
  void updateGame();
  void updateControls();
  void updateCamera();
  void updateStatus();
  
  void waveGenerator();
  void createRandomWave();
  void queueGroup(PlaneType type, Coord1 groupSize);
  void smallShake();

  PlaneUnlockable* tryCreateUnlockable(Point2 center, Coord2 toppingID, Logical startMagnet = false);
  
  void addAction(ActionEvent* ae);
  void addDebris(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  void addEnemy(PlaneGO* enemy, Logical addLast = true);
  void addAlly(PlaneGO* ally);

  // new virtuals
  virtual void createWave() = 0;
  virtual void playerDamaged() {}
  virtual void enemyDefeated(PlaneGO* enemy);
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  void load();
  
  void update();
  void redraw();
    
  virtual Coord1 calcWinBonus();
  virtual Point1 healthMasterAlpha() {return 0.5;}
  Logical shouldLoadPizza() {return false;}
  void callback(ActionEvent* caller);
  
  // b2ContactListener
  void BeginContact(b2Contact* contact) {}
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {}
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}
  void EndContact(b2Contact* contact) {}
};

struct CreatePLGroupCMD : Command
{
  PlaneLevel* level;
  PlaneLevel::PlaneType type;
  Coord1 groupSize;
  
  CreatePLGroupCMD(PlaneLevel* setLevel, PlaneLevel::PlaneType setType, Coord1 setGroupSize);
  void execute();
};

struct CreatePLEnemyCMD : Command
{
  PlaneLevel* level;
  PlaneLevel::PlaneType type;
  Point2 xy;  // only flameskull uses this
  
  CreatePLEnemyCMD(PlaneLevel* setLevel, PlaneLevel::PlaneType setType, 
                   Point2 setXY = Point2(0.0, 0.0));
  void execute();
};

struct PLLevelCupid : PlaneLevel
{
  void createWave();
};

struct PLLevelWisp : PlaneLevel
{
  void createWave();  
};

struct PLLevelNaga : PlaneLevel
{
  void createWave();
};

struct PLLevelNode : PlaneLevel
{
  void createWave();
};

// ==================================== TimingLevel ================================ //

struct TimingModule;
struct TimingGO;
struct TimingLevel;

struct TimingObjman : ObjmanStd<TimingGO>
{
  TimingObjman() : ObjmanStd<TimingGO>() {}
};

struct TimingBG : StdBackground
{
  TimingLevel* level;
  
  TimingBG(TimingLevel* setLevel);
  void load();
  void drawBackground();
};

struct TimingLevel : PizzaLevelInterface, ActionListener
{
  TimingLevel();
  virtual ~TimingLevel();
  
  TimingObjman objman;
  TimingPizza* player;
  TimingCarpet* carpet;
  TimingBG bg;

  CameraStd camera;
  Coord1 camState;
  LinearFn camZoomer;
  Point2 startZoomOutCamXY;
  Point1 startZoomOutPlayerX;

  ActionList actions;
  ActionList frontActions;
  ActionQueue script;   // this controls the pizza
  
  DataList<TimingGO*> platformList;  // not owned
  Coord1 currPlatformIndex;

  DataList<TimingModule*> moduleList;  // OWNS
  Coord1 currModuleIndex;
  TimingModule* currModule;
  TimingModule* nextModule;
  
  Coord1 runningProgress;
  
  enum TimingCamState
  {
    TIMING_CAM_PLAY,
    TIMING_CAM_STARTWIN,
    TIMING_CAM_WIN
  };
  
  // new non virtuals
  void addModule(TimingModule* newModule);
  void advanceModule();
  void updateControls();
  void updateStatus();
  void updateModules();
  void drawModules();
  void checkModuleStars();
  void updateCamera();
  void pizzaJump();
  void startZoomOutCam();
  void resolveVictory(Coord1 finalPlatIndex);
  Coord1 resultToScore(Coord1 resultID);  // pass it getResultImgIndex()
  void addObj(TimingGO* obj);
  TimingUnlockable* tryCreateUnlockable(Point2 center, Coord2 toppingID);
  
  // new virtuals
  virtual void prepModules() = 0;
  virtual void createWorld();
  virtual void moduleGameDone();
  
  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void load();
  
  virtual void update();
  virtual void redraw();
  
  virtual void addAction(ActionEvent* ae);
  virtual void addFrontAction(ActionEvent* ae);
  
  virtual Coord1 calcWinBonus() {return 500.0 * levelID;}
  virtual Logical shouldLoadPizza() {return false;}
  virtual void callback(ActionEvent* caller);
};

struct TimingModule : ActionListener
{
  TimingLevel* level;
  ActionQueue script;
  ActionList actions;
  
  Coord1 state;
  VisRectangular base;
  VisRectangular resultVR;
  
  Point1 progress;  // typically -1.0 to 1.0, 0.0 is target
  Point2 progressData;  // this is some helper data that not all use
  Logical waitForDraw; // this is to make sure it starts at 0
  
  TimingModule();
  virtual ~TimingModule() {}
  void update();
  void redraw();
  
  virtual void touchAction();
  
  virtual void startActive();
  virtual void transIn();
  virtual void transInStd();
  virtual void transOut();
  virtual void transOutStd();
  virtual void updateFunction() = 0;
  virtual void redrawMe() = 0;
  virtual Logical blockGame() {return state == RM::TRANS_IN || state == RM::ACTIVE;}
  virtual void resolveModule();
  virtual Coord1 getPipIndex();
  virtual Coord1 getResultImgIndex();
  virtual void callback(ActionEvent* caller);
  virtual void stateHandler(ActionEvent* e);
};

struct HBarModule : TimingModule
{  
  HBarModule();
  void updateFunction();
  void redrawMe();
};

struct PieModule : TimingModule
{
  PieModule();
  void updateFunction();
  void redrawMe();
};

struct CirclesModule : TimingModule
{
  CirclesModule();
  void updateFunction();
  void redrawMe();
};

struct ScalersModule : TimingModule
{
  ScalersModule();
  void updateFunction();
  void redrawMe();
};

struct SlidersModule : TimingModule
{
  SlidersModule();
  void updateFunction();
  void redrawMe();
};

struct TILevelCircles : TimingLevel
{
  void prepModules();
  void moduleGameDone();
};

struct TILevelScalers : TimingLevel
{
  void prepModules();
};

struct TILevelSliders : TimingLevel
{
  void prepModules();
  void createWorld();
};

// ==================================== Tutorial ================================ //

ActionEvent* CreateWinBanner();

struct PizzaTutorial : RivermanGUI
{
  PizzaLevelInterface* level;
  PizzaTutorial* nextTutorial;
  Logical updateGame;
  
  Point2 boxSize; // each one should set this
  ColorP4 boxColor;
  Point1 transProgress;
  
  PizzaTutorial(PizzaLevelInterface* setLevel);
  virtual ~PizzaTutorial();
  virtual void update();
  virtual void redraw();
  
  virtual void setWindowText(const String2& text);  // scales text
  virtual void transIn();
  virtual void transOut();
  Logical shouldUpdateGame() {return updateGame;}
  virtual void resolveGUI();
  virtual void click(Widget* caller);
};

struct PassiveTutorial : PizzaTutorial
{
  PassiveTutorial(PizzaLevelInterface* setLevel);
  void update() {}
  void redraw() {}
};

struct SlamTutorial : PizzaTutorial
{
  SlamTutorial(PizzaLevelInterface* setLevel);
};

struct EnemyJumpTutorial : PizzaTutorial
{
  PizzaLevel* regularLevel;
  
  EnemyJumpTutorial(PizzaLevel* setLevel);
  void startResolve();
};

struct KarateTutorial : PizzaTutorial
{
  KarateTutorial(PizzaLevelInterface* setLevel);
};

struct WreckJumpTutorial : PizzaTutorial
{
  WreckJumpTutorial(PizzaLevelInterface* setLevel);  
};

#endif