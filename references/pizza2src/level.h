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

#include "pgo.h"
#include "rmactions.h"
#include "globals.h"
#include "rmbackground.h"

struct PizzaTutorial;
struct PizzaBackground;

struct PizzaLevelInterface : ActionListener, SpineEventListener
{
  Coord1 levelID;
  Coord1 levelRegion;  // this usually determines background
  
  Coord1 levelState;  // passive, active, win, lose

  RandomSimple randGen;

  PizzaTutorial* tutorial;
  Coord1 tutShown;  // assumes there is a natural order to them. this could also be a bitmap

  CameraStd rmboxCamera;
  ShakeRegister shaker;
  
  Point1 worldTimeMult;
  
  String1 musicFilename;  // set this before the resources are loaded

  // used to help build levels
  DataList<Point2> currPointList;
  Point2 endpoint;

  // used when there are enemies to load
  DataList<Coord1> enemiesEnabled;
  
  Coord1 backgroundType;

  TimerFn healTimer;  // for the seasoning
  
  TimerFn shieldDownTimer;
  Logical shieldActive;
  
  // equip gui turns these off
  Logical updateGOBs;
  Logical drawGOBs;

  enum
  {
    LEVEL_PLAY,
    LEVEL_PASSIVE,
    LEVEL_LOSE,
    LEVEL_WIN
  };
  
  PizzaLevelInterface();
  virtual ~PizzaLevelInterface();  // deletes tutorial and physics

  virtual void preload() {}  // called before loadRes
  virtual void loadRes() = 0;
  virtual void unloadRes() = 0;
  virtual void initRes() = 0;  // call this AFTER loadRes()

  virtual void update() = 0;
  virtual void redraw() = 0;

  virtual void winLevel();
  virtual void loseLevel();
  
  virtual void levelStart() {levelState = LEVEL_PLAY;}
  
  virtual Logical lostToDMGStd();
  virtual Point2 rmcamboxToScreen(Point2 rmcamboxPt);
  
  virtual void addLevelPt(Point1 xVal, Point1 yVal);  // adds to currPointList, adds endpoint
  virtual void clearPtList();
  
  virtual void loadEnabledEnemies();
  virtual void loadGenericChunkObjects();
  virtual void loadEnemy(Coord1 enemyIndex);

  virtual void updateSeasonings();  // for heal, shield, bank
  
  virtual void updateHealing();
  virtual void updateBanking() {}

  virtual void updateShielding();
  virtual Logical tryHitShield();  // returns true if shield blocked it
  
  // from SpineEventListener
  virtual void spine_event_callback(spAnimationState* state, spEventType eventType,
                                    spTrackEntry* entry, spEvent* event) {}
  
  virtual void callback(ActionEvent* caller) {}

  // statics
  static void createSparksEffect(Point2 xy, Point1 scale, Point1 angle, ActionList* list);
  
  static void createTriangleShatter(VisRectangular* object, Coord2 dimensions, Point2 durationRange,
                                    ActionList* list);
  
  static PizzaLevelInterface* createLevel(Coord2 levelType);
  
  static PizzaBackground* createRegionalBackground(Coord1 region, PizzaLevelInterface* level);
};

// ========================= ObjmanStd =========================== //

template <typename Type>
struct ObjmanStd
{
  ArrayList<LinkedList<Type*> > phaseList;
  
  ActionList actions;
  ActionList frontActions;  // draw on top of game  
  
  Coord1 removeCounter;
  
  ObjmanStd();
  virtual ~ObjmanStd() {}
  void init(Coord1 numPhases);
  
  virtual void update();
  virtual void updateObjects();
  virtual void drawObjects();  // draws all phases in order
  virtual void drawPhase(Coord1 phaseID);
  virtual void drawPhases(Coord2 phaseRange);  // inclusive [start, end]
  
  virtual void addPGO(Type* pgo, PizzaPhase phase);
  
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  void garbageCollector();
};

template <typename Type>
Logical removeBGO(Type* pgo);

template <typename Type>
ObjmanStd<Type>::ObjmanStd() :
  phaseList(),

  actions(),
  frontActions(),

  removeCounter(0)
{
  
}

template <typename Type>
void ObjmanStd<Type>::init(Coord1 numPhases)
{
  phaseList.ensure_count(numPhases);
}

template <typename Type>
void ObjmanStd<Type>::update()
{
  updateObjects();
  actions.update();
  frontActions.update();
}

template <typename Type>
void ObjmanStd<Type>::updateObjects()
{
  for (Coord1 phaseID = 0; phaseID < phaseList.count; ++phaseID)
  {
    LinkedList<Type*>& currList = phaseList[phaseID];
    
    for (LinkedListNode<Type*>* pgoNode = currList.first; 
         pgoNode != NULL; 
         pgoNode = pgoNode->next)
    {
      pgoNode->data->update();
    }
    
    // stagger garbage collection, 1 phase per update
    if (phaseID == removeCounter)
    {
      currList.removeMatches(&removeBGO);
    }
  }
  
  removeCounter++;
  removeCounter %= phaseList.count;
}

template <typename Type>
void ObjmanStd<Type>::drawObjects()
{
  drawPhases(Coord2(PHASE_PLAYER + 1, phaseList.last_i()));
}

template <typename Type>
void ObjmanStd<Type>::drawPhase(Coord1 phaseID)
{
  drawPhases(Coord2(phaseID, phaseID));
}

template <typename Type>
void ObjmanStd<Type>::drawPhases(Coord2 phaseRange)
{
  phaseRange.x = std::max(phaseRange.x, 0);
  phaseRange.y = std::min(phaseRange.y, phaseList.count - 1);

  for (Coord1 phaseID = phaseRange.x; phaseID <= phaseRange.y; ++phaseID)
  {
    LinkedList<Type*>& currList = phaseList[phaseID];
    
    for (LinkedListNode<Type*>* pgoNode = currList.first; 
         pgoNode != NULL; 
         pgoNode = pgoNode->next)
    {
      pgoNode->data->redraw();
    }
  }
}

template <typename Type>
void ObjmanStd<Type>::addPGO(Type* pgo, PizzaPhase phase)
{
  phaseList[phase].addX(pgo);
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
Logical removeBGO(Type* bgo)
{
  return bgo->lifeState == RM::REMOVE;
}

template <typename Type>
void ObjmanStd<Type>::garbageCollector()
{
  for (Coord1 i = 0; i < phaseList.count; ++i)
  {
    phaseList[i].removeMatches(&removeBGO);
  }
}

// ========================= PizzaObjman =========================== //

struct PizzaBackground;

struct PizzaObjman : ObjmanStd<PizzaGOStd>
{
  ArrayList<PizzaGOStd*> bgTerrainList;
  ArrayList<PizzaGOStd*> sortedDrawList;
  
  ActionList debrisList,
  frontDebrisList;

  PizzaObjman();
  ~PizzaObjman();
  
  void update();
  void updateObjects();
  void drawObjects();
  void drawBGTerrain();
  
  void addBGTerrain(PizzaGOStd* terrain);
};

// ========================= PhysicsLevelStd =========================== //

struct PhysicsLevelStd : PizzaLevelInterface, b2ContactListener, b2RayCastCallback
{
  PizzaObjman objman;
  PizzaPlayer* player;
  PizzaBackground* background;

  Box worldBox;  // some levels use this for bounds checking
  Logical spikedTerrain;  // for the spiked level, it's a global
  Logical bounceTerrain;  // for the bounce level, it's a global
  Logical useOctBall;  // true enables for all balls in balance
  
  Coord1 totalDestructibles;  // for angry, includes skulls

  DataList<Point1> chunkWeights;  // used in bounce, spike, and puppy
  DataList<Point2> groundCameraPts;
  DataList<Point2> ceilingCameraPts;
  
  Point1 tiltPercent;  // [-1.0, 1.0] helps camera
  Point1 cameraTowardGroundMult;  // default: 0.85
  
  // this is for angry
  Coord1 skullsCreated;
  Coord1 skullsDestroyed;
  
  // for puppy
  DataList<Billy*> puppyList;
  Coord1 currBreed;
  Coord1 puppiesToFind;
  Coord1 puppiesToReturn;
  Coord1 puppiesTotal;
  Point2 puppyStartPt;  // they hop off of you to here
  Point2 puppyHousePt;  // then they prance into here
  Coord1 objectiveDir;

  // for sprint
  Point2 flagStartPt;
  
  // for sumo
  SumoBoss* boss;
  
  // for pirate
  Box pirateWinBox;
  PirateChest* pirateChest;
  PirateShip* pirateShip;
  WaterArea* shipWater;
  
  // for b2RayCastCallback
  b2Fixture* lastRaycastFixture;
  Point2 lastRaycastPt;
  Point2 lastRaycastNormal;
  Point1 lastRaycastFraction;
  
  PhysicsLevelStd();
  virtual ~PhysicsLevelStd() {}
  
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);

  virtual void updateControls();

  // virtuals
  virtual void loadRes();
  virtual void unloadRes();
  virtual void initRes();  // call this AFTER loadRes()
  virtual void initSublevel() {}

  virtual void update();
  virtual void redraw();
  
  virtual void updateGame();
  virtual void updateCamera(Logical snapIntoPlace);
  virtual void updateLevelAI() {}  // only if level is active
  
  virtual void updateCameraPuppy(Logical snapIntoPlace = false);
  virtual void updateCameraFree(Logical snapIntoPlace = false);
  virtual void updateCameraSumo(Logical snapIntoPlace = false);
  virtual void updateCamBottomToGround(Logical snapIntoPlace = false);
  
  virtual void colorizeAfterTerrain() {}  // only puppy does this
  
  // called after the background and terrtain draws
  virtual void drawShadows();
  virtual void drawShadowsChunkMode();
  
  virtual void drawPuppyCageBacks();
  
  // some levels override this for special destruction effects
  virtual void objectCrushedStd(PizzaGOStd* pgo) {pgo->destroyedPhysical = true;}

  virtual void drawPirateShipFront() {}
  virtual void drawAfterObjects() {}
  
  // zooms based on player top vs screen top
  void updateZoomStd(Point1 yPad);
  
  // zooms based on player top vs bottom center of camera and ground y
  void updateZoomPuppy(Point1 playerYPad);
  void updateZoomFree(Point1 playerYPad);

  // zooms based on player top and boss top vs bottom center of camera and ground y
  void updateZoomSumo(Point1 yPad);
  
  void clampRMBoxCamScreenXToWorld();  // uses worldBox
  void clampRMBoxCamScreenXToRange(Point1 leftX, Point1 rightX);
  
  virtual Point1 playerTXVel();
  virtual Point1 playerXVelNorm() {return player->get_velocity().x / playerTXVel();}
  virtual Point1 bounceMult() {return 1.0;}
  virtual Logical jumpEnabled() {return true;}  // for bounce level to disable
  virtual Logical slamDisabled() {return false;}  // karate uses this
  virtual Logical playerGroundStick() {return false;}  // for slopes, to stick to ground

  virtual void startStanding(Point2 groundPt);

  void updateFacingStd(Logical hasEnemies, Point1 closestX);  // responds to closest enemy or tilt
  void enemyCameraXStd(Logical hasEnemies, Point1 closestX, Logical snapIntoPlace);  // responds to closest enemy or tilt
  void zoomToMinY(Point1 yTarget);  // assumes handle is already set, zooms Y to keep point onscreen
  Point2 minYZoom(Point1 yTarget);  // same as above but just returns the zoom value
  
  virtual Logical lostToFallingStd();

  virtual void addDebris(ActionEvent* debrisEffect);
  virtual void addFrontDebris(ActionEvent* debrisEffect);

  virtual void drawHUD();

  virtual Point1 closestEnemyX(); // return pizza X if there are no enemies, NOT FLAMESKULLS NOT TERRAIN
  virtual Point1 closestEnemyX(Point1 dir); // return pizza X if there are no enemies
  virtual Logical enemyActive(); // return true if an enemy is onscreen
  virtual Point1 getGroundY(Point1 xCoord = 0.0) {return 480.0;} // GladiatorLevel and KarateLevel overrides
  
  virtual void enemyDefeated(PizzaGOStd* enemy) {}  // bounce, gladiator, angry use this
  
  virtual void reportPlayerLanded(Logical slam); // trigger shake
  virtual void reportPlayerBump(Point1 impulseVal);
  virtual void reportSlam();

  virtual void addPGO(PizzaGOStd* pgo);
  
  static PizzaGOStd* createEnemy(Coord1 enemyType);  // ONLY generates the enemy, does NOT call addPGO or change xy
  
  // chunk creation helper functions
  void createBlocksFromTLandBR();
  void createBlocksFromTLandWH();  // assumes WH was added with addLevelPt
  void createSteelFromTLandBR();
  void createSteelFromTLandWH();  // assumes WH was added with addLevelPt
  void createStaticBreakableBlocksFromTLandBR();
  void createSpikeBlocksFromQuads();
  void createBurningBlocksFromTLandBR();
  void createBurningBlocksFromBCandWH();
  void createBurningBlocksFromBC(Point1 squareSize);
  void createSpinnersFromTLandBR();
  void createCoinsFromCenter(Point2 offset = Point2(0.0, 0.0));
  void createCoinsFromGroundPts();
  void createSkullsCentered(Point2 offset = Point2(0.0, 0.0));
  void createSkullsGroundPts();
  void createPuppiesGroundPts();
  void createGemsFromCenter(Point2 offset = Point2(0.0, 0.0));
  void createGemsFromGroundPts();
  void createCoinOrGemCenter(Point2 offset = Point2(0.0, 0.0));
  void createCoinOrGemFromGroundPts();
  void createEnemiesFromBCLine(Coord1 subType); // each enemy is 3 points: BC, line left, line right
  void createHiddenSkullsInLastBlockFromC(Logical useSkulls);
  void createHiddenPhysCoinsInLastBlockFromC();
  void createThemedGroundFromCWPts();
  void createChickensFromLines();  // [ch1Start, ch1End, ch2Start, ch2End...]
  void createItemBlocksFromTL(Coord1 itemType);
  void createGourdoBlocksFromTL();
  void createFadeBlocksFromTLandBR();
  void createHalfSpikeBlocksFromTL_BR_side();  // [TL, BR, any point in front of spikes]
  void createBouncersFromCandRC();  // [C, RC]
  void createFlyerFromSingleLine(Coord1 flyerType);  // this can be one LONG line
  void createFlyersFromLines(Coord1 flyerType);  // these lines are 2 pts each
  void createSpikeCirclesFromCandRC();
  void createSpikeGroundCW();
  WaterArea* createWaterFromCWQuads();
  void createObjectiveFromGroundPt(Coord1 levelType);
  
  // for all modes
  void starterChunk();
  
  // puppy chunks
  void puppyDoghouseChunk();
  void cutleryChunk(Coord1 levelType, Coord1 valleyEnemyType, Coord1 hillEnemyType);
  void pitpitChunk(Logical useCage, Logical useSkull, Logical useBBlock, Coord1 enemyType);
  void maplebarsChunk(Coord1 levelType, Coord1 enemyType);
  void spearpitChunk(Logical useCage, Logical useSkulls, Logical useBurners, Coord1 topEnemy);  // uses spearmen vert
  void spikeyMountainChunk(Coord1 levelType, Coord1 groundEnemyType, Coord1 flyerType);
  void puppyMoat(Logical useCage, Logical useGems, Coord1 waterEnemy, Logical useBBlocks, Coord1 standingEnemyType);
  void puppySpikeFlight(Coord1 levelType, Logical useRightItem, Coord1 enemyType);
  void puppySpinnerSpan(Logical useCage, Logical useGems,
                        Logical useSkull, Logical useBBlocks, Coord1 enemyType);

  // angry chunks
  void angryBouncePyramid(Logical useSkulls, Coord1 flyerType);
  void angryMagicPyramid(Logical useSkulls, Coord1 lineFlyerType, Coord1 groundType);  // uses FLAMEKSULL
  void angrySkyhenge(Logical useSkulls, Logical useCages);
  void angrySpikeblock(Logical useSkulls);
  void angryToppleTower(Logical useSkulls, Logical useGems);
  void angryCrossing(Logical useSkulls, Logical useGem, Logical useBBlocks, Logical useCage);
  void angrySpinners(Logical useSkulls, Logical useCage, Logical useGem,
                     Logical useBBlocks, Coord1 flyerType);
  void angryPit(Logical useSkulls, Logical useCage, Logical useGem, Coord1 enemyType);
  
  // sprint chunks
  void sprintBombtower(Logical useSkulls, Logical useBombs);
  void sprintSpearmanPitA(Logical useGems, Coord1 enemyType);
  void sprintSpikejump(Logical useGems);
  void sprintSpikeys(Logical useGems, Coord1 groundType, Coord1 lineFlyType);  // PI
  void sprintCavemen(Logical useGems, Logical useCages,
                     Logical useSkulls, Logical useBBlocks, Coord1 enemyType);
  void sprintBounceChasm(Logical useGems);
  void sprintDozerDig(Logical useGems, Logical useCages,
                      Logical useBurningBlocks, Coord1 enemyType);
  void sprintJumpChoice(Logical useGems);
  void sprintEndingRight();
  void flyerChunk(Coord1 flyer1Type, Coord1 flyer2Type, Coord1 flyer3Type);
  void waterChunk();

  // pirate chunks
  void pirateShipChunk();
  void dipBallChunk(Logical useCage, Logical useBlocks, Logical useBurningBlocks,
                    Logical useSkulls, Coord1 enemyType);
  void bouncerAChunk(Logical useCage, Logical useGem,
                     Logical useBBlock, Logical useSkull);
  void pirateFBlocks(Coord1 powerupType, Coord1 enemyType);
  void pirateGourdoLantern(Logical useGems, Logical useRightGourdo, Coord1 lineFlyerType);  // PI
  void pirateWater(Logical useGems, Coord1 lineFlyerType);
  
  // fireman chunks
  void flameBalloonBucketChunk(Logical useBucket, Logical useCoins, Logical useGems,
                               Logical useSkulls, Logical useBlocks, Logical useBurningBlocks,
                               Logical useCage, Coord1 enemyType);
  void flameBounceChunk(Logical useBurningBlocks, Logical useSkulls, Logical useBlocks,
                        Logical useGems, Logical useCage);
  void flamePitsChunk(Logical useBurningBlocks, Logical useGems, Logical useCage,
                      Logical useSkulls, Coord1 lineFlyerType);
  void flameSpearGuardsChunk(Logical useBurningBlocks, Logical useGems, Logical useCage,
                             Logical useSkulls, Logical useBlocks, Coord1 enemyType);
  void flameSpikeyStepsChunk(Logical useBurningBlocks, Logical useSkulls, Logical useBlocks,
                             Logical useGems, Coord1 enemyType); // support AN, PI, FI
  void flameFlyer(Logical useBurners, Logical useCages, Logical useGems, Logical useSkulls,
                  Coord1 lineFlyerType);
  void flameSpikeblocks(Logical useBurners, Logical useCages, Logical useGems,
                        Logical useSkulls, Coord1 enemyType);
  void flameSpinners(Logical useBurners, Logical useGems, Coord1 enemyType);
  
  // pizzeria chunks
  void pizzeriaChunk();

  virtual Coord1 standardEnemyDrop() {return RANDOM_COIN;}
  
  virtual Coord1 maxPlayerBalloons() {return 5;}

  PizzaGOStd* createSpecialObject(Coord2 objectType);
  
  // [num objects, average X point, highest y point]
  virtual Point3 evalObjectsOfInterestPuppy();
  virtual Point3 evalObjectsOfInterestFree();

  virtual Logical objectCloseToPlayerPuppy(PizzaGOStd* obj);
  virtual Logical objectCloseToPlayerFree(PizzaGOStd* obj);

  // Point1 getCameraGroundY(Point1 xVal);
  Point1 getCameraGroundY(Point2 xRange);
  Point1 getCameraBottomY();
  void addCameraGroundPts(DataList<Point2>& groundPts);  // groundPts is the actual ground
  void drawCameraGround();

  // ceilPts is the actual ceiling
  // always adds a very low point at the very end because it's assumed that each call
  // to this function creates line segments rather than being a continuous line like the ground
  void addCameraCeilingPts(DataList<Point2>& ceilPts);
  Point1 getCameraTopY();
  Point1 getCameraCeilY(Point2 xRange);
  void drawCameraTop();

  virtual Point2 pranceStartPt() {return puppyStartPt;}
  virtual Point2 pranceEndPt() {return puppyHousePt;}
  virtual void puppyFound(Billy* puppy); // for puppy level
  virtual void puppyReturned(Billy* puppy); // for puppy level
  virtual void puppyWasFound() {}
  virtual void directionChanged() {}

  // firstImg can be null
  void createTopography(const Point2* topPoints, Coord1 numPoints,
                        Image* firstImg = NULL);
  void invisibleWallsStd(const Box& wallBox, Point1 wallThickness = 128.0);
  void invisibleWallsLR(Point2 leftGroundPt, Point2 rightGroundPt);
  
  TerrainQuad* createSlopeQuad(const Point2& pt0, const Point2& pt1,
                               const Point2& pt2, const Point2& pt3,
                               Image* img = NULL);
  
  void particleCollision(PhysPolyParticle* particle, Point1 impulse);
  
  // PizzaPlayer* findPlayer(PizzaGO* pgo1, PizzaGO* pgo2);

  virtual void shopButtonPressed(Coord1 buttonIndex) {}
  
  // from b2RayCastCallback
  float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                        const b2Vec2& normal, float32 fraction);
  // b2ContactListener
  void BeginContact(b2Contact* contact);
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
  void EndContact(b2Contact* contact);
  
  PizzaPlayer* findPlayer(PizzaGOStd* pgo1, PizzaGOStd* pgo2);
  
  template <typename PGOType>
  static PGOType* findObj(PizzaGOStd* obj1, PizzaGOStd* obj2, Coord1 type);
  
  // from SpineEventListener
  virtual void spine_event_callback(spAnimationState* state, spEventType eventType,
                                    spTrackEntry* entry, spEvent* event);
  
  // STATICS
  static Point1 tiltMagnitude();  // -1.0 to 1.0
  static Point1 tiltMagnitudeVert();  // -1.0 to 1.0
};

// ==================================== LevelChunk ================================ //

struct PhysLevelChunkStd
{
  PhysicsLevelStd* level;
  
  Point2 startPt;
  
  DataList<Coord1> myEnemyList;  // tells the level to load these
  
  PhysLevelChunkStd();
  virtual ~PhysLevelChunkStd() {}
  
  virtual void createChunk() = 0;
  virtual void createCoinsFromPts();
  virtual void addCameraGroundFromPts();
};

// ==================================== TestLevel ================================ //

struct TestLevel : PhysicsLevelStd
{
  TestLevel();
  void initRes();
  void createWorld(const Box& setWorldBox);
};

// ==================================== Gladiator ================================ //

struct GladiatorLevel : PhysicsLevelStd
{
  DataList<Point1> enemyWeights;
  DataList<Coord2> enemyCounts;
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
  
  void randomWave();
  
  void createGroup(Coord1 enemyType, Coord1 count = 1);
  PizzaGOStd* createSingle(Coord1 enemyType, Coord2 countData); // countData[i, num]
  PizzaGOStd* createSingle(Coord1 enemyType, Coord1 groupID = 1);
  Point1 spawnXStd(Coord1 side, Point1 lerpMin = 0.0);  // side 0 left 1 right
  Point1 spawnYStd(Coord1 index, Coord1 number);  // for enemies in track bounds
  Point1 spawnYStd(Coord1 index, Coord1 number, Point2 bounds);  // for enemies in track bounds
  void placeSticky(Point1 courseXPercent);
  
  void setProb(Coord1 enemyType, Point1 weight);
  void setProb(Coord1 enemyType, Coord1 lowCount, Coord1 highCount);
  void setProb(Coord1 enemyType, Point1 weight, Coord1 lowCount, Coord1 highCount);
  
  void createWaveNewEnemyStd();
  void createWaveNewHarderStd();
  
  // new virtuals
  virtual void createWorld(const Box& setWorldBox);
  virtual void createWorldStd();
  virtual void worldCreated() {}
  
  virtual void createWave() {createWaveNewEnemyStd();}

  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void initSublevel();
  
  virtual void updateCamera(Logical snapIntoPlace);
  virtual void updateLevelAI();
  
  virtual void enemyDefeated(PizzaGOStd* enemy);
  virtual void pizzaDamaged();
  
  void drawBGTerrain();

  Point1 getGroundY(Point1 xCoord = 0.0);
  Logical playerGroundStick() {return true;}
};

// do NOT delete the pgo before this calls done()
struct CreateRenderedParticlesAction : ActionEvent
{
  RenderableParticleBreakerInf* pgo;
  Image* img;
  
  Logical captured;
  Box localAABB;
  
  CreateRenderedParticlesAction(RenderableParticleBreakerInf* setPGO);
  virtual ~CreateRenderedParticlesAction();
  
  virtual void updateMe();
  virtual void redrawMe();
};

// graveyard 1
struct GLLevelUnarmed : GladiatorLevel
{
  GLLevelUnarmed();
  void preload();
  void worldCreated();
  void createWave();
  void createWorldStd();
};

// graveyard 2
struct GLLevelSpearmenSide : GladiatorLevel
{
  GLLevelSpearmenSide();
  void createWave();
};

// graveyard 3
struct GLLevelSpearmenVert : GladiatorLevel
{
  GLLevelSpearmenVert();
  void createWave();
};

// graveyard 4
struct GLLevelBombbat : GladiatorLevel
{
  GLLevelBombbat();
};

// graveyard 5
struct GLLevelBombbat2 : GladiatorLevel
{
  GLLevelBombbat2();
  void createWave() {createWaveNewHarderStd();}
};

// jurassic park 1
struct GLLevelSpider : GladiatorLevel
{
  GLLevelSpider();
};

// jurassic park 2
struct GLLevelSpider2 : GladiatorLevel
{
  GLLevelSpider2();
  void createWave() {createWaveNewHarderStd();}
};

// jurassic park 3
struct GLLevelCaveman : GladiatorLevel
{
  GLLevelCaveman();
};

// jurassic park 4
struct GLLevelCaveman2 : GladiatorLevel
{
  GLLevelCaveman2();
  void createWave() {createWaveNewHarderStd();}
};

// jurassic park 5
struct GLLevelEnt : GladiatorLevel
{
  GLLevelEnt();
};

// jurassic park 6
struct GLLevelEnt2 : GladiatorLevel
{
  GLLevelEnt2();
  void createWave() {createWaveNewHarderStd();}
};

// bakery 1
struct GLLevelDonut : GladiatorLevel
{
  GLLevelDonut();
};

// bakery 2
struct GLLevelDonut2 : GladiatorLevel
{
  GLLevelDonut2();
  void createWave() {createWaveNewHarderStd();}
};

// bakery 3
struct GLLevelUtensil : GladiatorLevel
{
  GLLevelUtensil();
};

// bakery 4
struct GLLevelUtensil2 : GladiatorLevel
{
  GLLevelUtensil2();
  void createWave() {createWaveNewHarderStd();}
};

// dump 1
struct GLLevelGiant : GladiatorLevel
{
  GLLevelGiant();
};

// dump 2
struct GLLevelGiant2 : GladiatorLevel
{
  GLLevelGiant2();
  void createWave() {createWaveNewHarderStd();}
};

// dump 3
struct GLLevelDozer : GladiatorLevel
{
  GLLevelDozer();
};

// dump 4
struct GLLevelDozer2 : GladiatorLevel
{
  GLLevelDozer2();
  void createWave() {createWaveNewHarderStd();}
};

// silicon 1
struct GLLevelWisp : GladiatorLevel
{
  GLLevelWisp();
};

// silicon 2
struct GLLevelWisp2 : GladiatorLevel
{
  GLLevelWisp2();
  void createWave() {createWaveNewHarderStd();}
};

// silicon 3
struct GLLevelChip : GladiatorLevel
{
  GLLevelChip();
};

// silicon 4
struct GLLevelChip2 : GladiatorLevel
{
  GLLevelChip2();
  void createWave() {createWaveNewHarderStd();}
};

// body 1
struct GLLevelScientist : GladiatorLevel
{
  GLLevelScientist();
};

// body 2
struct GLLevelScientist2 : GladiatorLevel
{
  GLLevelScientist2();
  void createWave() {createWaveNewHarderStd();}
};

// body 3
struct GLLevelAlien : GladiatorLevel
{
  GLLevelAlien();
};

// body 4
struct GLLevelAlien2 : GladiatorLevel
{
  GLLevelAlien2();
  void createWave() {createWaveNewHarderStd();}
};

// catacombs 1
struct GLLevelCupid : GladiatorLevel
{
  GLLevelCupid();
};

// catacombs 2
struct GLLevelCupid2 : GladiatorLevel
{
  GLLevelCupid2();
  void createWave() {createWaveNewHarderStd();}
};

// catacombs 3
struct GLLevelSpiny : GladiatorLevel
{
  GLLevelSpiny();
};

// catacombs 4
struct GLLevelSpiny2 : GladiatorLevel
{
  GLLevelSpiny2();
  void createWave() {createWaveNewHarderStd();}
};

// catacombs 5
struct GLLevelReaper : GladiatorLevel
{
  GLLevelReaper();
};

// catacombs 6
struct GLLevelReaper2 : GladiatorLevel
{
  GLLevelReaper2();
  void createWave() {createWaveNewHarderStd();}
};

struct GLLevelTest : GladiatorLevel
{
  Coord1 testEnemyType;
  
  GLLevelTest();
  void createWave();
};

// ==================================== JengaLevel ================================ //

struct JengaLevel : PhysicsLevelStd
{
  Point1 groundY;

  JengaLevel();
  
  void createWorldStd(const Box& setWorldBox, Logical withGround = true);

  // new virtuals
  virtual void createWorld() {}
  virtual void worldCreated() {}

  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void initSublevel();
  
  virtual void updateCamera(Logical snapIntoPlace);
  virtual void updateLevelAI();
  
  virtual void updateStatus();
  
  virtual void pizzaDamaged();
  
  void drawBGTerrain();
  Point1 getGroundY(Point1 xCoord = 0.0);
};

// ==================================== JELevelPyramid ================================ //

struct JELevelPyramid : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelPusher ================================ //

struct JELevelPusher : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelColumns ================================ //

struct JELevelColumns : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelClouds ================================ //

struct JELevelClouds : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelBombs ================================ //

struct JELevelBombs : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelBombfield ================================ //

struct JELevelBombfield : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelBombAsym ================================ //

struct JELevelBombAsym : JengaLevel
{
  void createWorld();
};

// ==================================== JELevelBombAsym ================================ //

struct JELevelAerials : JengaLevel
{
  void createWorld();
};

// ==================================== AngryLevel ================================ //

struct AngryLevel : PhysicsLevelStd
{
  Point1 timeAllowed;
  
  AngryLevel();
  
  Coord1 closestSkullDir();

  // new virtuals
  virtual void createLevel();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();
  
  virtual Coord1 calcWinBonus();
  
  void callback(ActionEvent* caller);
};

struct ANLevel0 : AngryLevel
{
  ANLevel0();
  void createLevel();
};

struct ANLevel1 : AngryLevel
{
  ANLevel1();
  void createLevel();
};

struct ANLevel2 : AngryLevel
{
  ANLevel2();
  void createLevel();
};

struct ANLevel3 : AngryLevel
{
  ANLevel3();
  void createLevel();
};

struct ANLevel4 : AngryLevel
{
  ANLevel4();
  void createLevel();
};

struct ANLevel5 : AngryLevel
{
  ANLevel5();
  void createLevel();
};

struct ANLevel6 : AngryLevel
{
  ANLevel6();
  void createLevel();
};

struct ANLevel7 : AngryLevel
{
  ANLevel7();
  void createLevel();
};

struct ANLevel8 : AngryLevel
{
  ANLevel8();
  void createLevel();
};

struct ANLevel9 : AngryLevel
{
  ANLevel9();
  void createLevel();
};

struct ANLevelTest : AngryLevel
{
  ANLevelTest();
  void createLevel();
};

// ==================================== PizzeriaLevel ================================ //

struct PizzeriaBG;

struct PizzeriaLevel : PhysicsLevelStd
{
  PizzeriaBG* parlorBG;
  ShopChef* chef;
  
  String2 wrappedChefMsg;
  Point2 chefMsgSize;
  Point1 chefMsgScale;
  
  DataList<PizzeriaBuyButton*> buttons;
  
  DataList<IngredientData*> ingredientPtrs;
  
  PizzeriaLevel();
  
  void zoomInToEquip(Point1 transTime);
  void zoomOutToBuy(Point1 transTime);

  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();

  void updateButtonState(Coord1 index);
  void syncAllButtons();
  
  Point1 playerTXVel() {return 250.0;}
  virtual Point1 bounceMult() {return 0.75;}

  virtual void shopButtonPressed(Coord1 buttonIndex);

  void callback(ActionEvent* caller);
};

// ==================================== SumoLevel ================================ //

struct SumoLevel : PhysicsLevelStd
{
  Point1 groundY;
  
  SumoLevel();
  
  void createWorldStd(const Box& setWorldBox, Logical withGround = true);
  
  // points should be arranged like
  // [saberCenter1x, center1y]
  // [saberCenter2x, center2y]
  // [saber1Length, saber1Angle]
  // [saber2Length, saber2Angle]
  void createBossSabersFromCurrPoints(Point1 thickness = 16.0);
  
  // new virtuals
  virtual void createWorld() {}
  virtual void worldCreated() {}
  
  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void initSublevel();
  
  virtual void updateCamera(Logical snapIntoPlace);
  virtual void updateLevelAI();
  
  void drawBGTerrain();
  void drawAfterObjects();  // draws lightsabers

  Point1 getGroundY(Point1 xCoord = 0.0);
};

// ==================================== SumoLevelEasy ================================ //

struct SumoLevelEasy : SumoLevel
{
  void loadRes();
  void createWorld();
};

// ==================================== SumoLevelGraveyard ================================ //

struct SumoLevelGraveyard : SumoLevel
{
  void loadRes();
  void createWorld();
};

// ==================================== SumoLevelTrike ================================ //

struct SumoLevelTrike : SumoLevel
{
  DataList<PizzaGOStd*> blockList;
  Logical groundLevelChanged;
  
  SumoLevelTrike() : SumoLevel(), groundLevelChanged(false) {}
  void loadRes();
  void createWorld();
  void updateLevelAI();
};

// ==================================== SumoLevelCupcake ================================ //

struct SumoLevelCupcake : SumoLevel
{
  void loadRes();
  void createWorld();
};

// ==================================== SumoLevelEmperor ================================ //

struct SumoLevelEmperor : SumoLevel
{
  TimerFn bossThrowTimer;
  Coord1 currBossSaber;
  
  SumoLevelEmperor();
  
  void loadRes();
  void createWorld();
  void updateLevelAI();
  void drawAfterObjects(); // draw special emporer sabers
};

// ==================================== SumoLevelRay ================================ //

struct SumoLevelRay : SumoLevel
{
  TimerFn bubbleTimer;
  Coord1 lastBubbleSide;  // -1 or 1
  
  SumoLevelRay();
  void loadRes();
  void createWorld();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void createBubble(Point2 center, Point1 scale);
  void callback(ActionEvent* caller); 
}; 

// ==================================== SumoLevelFungus ================================ //

struct SumoLevelFungus : SumoLevel
{
  void loadRes();
  void createWorld();
  void drawAfterObjects(); // draw special emporer sabers
};

// ==================================== SumoLevelSnowball ================================ //

struct SumoLevelSnowball : SumoLevel
{
  SumoLevelSnowball();
  
  void loadRes();
  void createWorld();
};

// ==================================== SumoLevelJunkboys ================================ //

struct SumoLevelJunkboys : SumoLevel
{
  SumoLevelJunkboys();
  
  void loadRes();
  void createWorld();
};

// ==================================== SumoLevelEdison ================================ //

struct SumoLevelEdison : SumoLevel
{
  SumoLevelEdison();
  
  void loadRes();
  void createWorld();
  void objectCrushedStd(PizzaGOStd* pgo);
};

// ==================================== PachinkoLevel ================================ //

struct PachinkoLevel : PhysicsLevelStd
{
  ActionQueue startScript;
  ActionQueue ballDropScript;
  
  PachinkoLevel();
  
  // new virtuals
  virtual void createWorld() {}
  virtual void worldCreated() {}
  
  // overrides
  virtual void preload();
  virtual void loadRes();
  virtual void unloadRes();
  virtual void initSublevel();
  
  virtual void updateCamera(Logical snapIntoPlace);
  virtual void updateLevelAI();
  
  virtual void updateStatus();
  
  virtual void pizzaDamaged();
  
  void drawBGTerrain();
  Point1 bounceMult() {return 0.3;}
  virtual Point1 playerTXVel() {return 200.0;}

  void levelStart();

  virtual void callback(ActionEvent* caller);
};

// ==================================== PALevelEasy ================================ //

struct PALevelEasy : PachinkoLevel
{
  void createWorld();
};


// ================================ PuppyLevel ============================= //

struct PuppyLevel : PhysicsLevelStd
{
  TimerFn crateTimer; // for crate rain
  Coord1 cratesMade;
  
  TimerFn arrowTimer;
  TimerFn bombTimer;
  
  Coord1 billyState;
  TimerFn billyJumpTimer;
  Coord1 currJumpingBilly; // this is the NEXT billy to jump off the pizza
  
  // AngrySun* sun;
  
  enum
  {
    WAIT_FOR_RETURN,
    BILLYS_RETURNING
  };
  
  PuppyLevel();
  
  // new virtuals
  virtual void createLevel();

  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();

  void colorizeAfterTerrain();

  virtual Coord1 calcWinBonus();
  
  void callback(ActionEvent* caller);
};

struct PULevel0 : PuppyLevel
{
  PULevel0();
  void createLevel();
};

struct PULevel1 : PuppyLevel
{
  PULevel1();
  void createLevel();
};

struct PULevel2 : PuppyLevel
{
  PULevel2();
  void createLevel();
};

struct PULevel3 : PuppyLevel
{
  PULevel3();
  void createLevel();
};

struct PULevel4 : PuppyLevel
{
  PULevel4();
  void createLevel();
};

struct PULevel5 : PuppyLevel
{
  PULevel5();
  void createLevel();
};

struct PULevel6 : PuppyLevel
{
  PULevel6();
  void createLevel();
};

struct PULevel7 : PuppyLevel
{
  PULevel7();
  void createLevel();
};

struct PULevel8 : PuppyLevel
{
  PULevel8();
  void createLevel();
};

struct PULevel9 : PuppyLevel
{
  PULevel9();
  void createLevel();
};

struct PULevelTest : PuppyLevel
{
  PULevelTest();
  void createLevel();
};

// ================================ SprintLevelStd ============================= //

struct SprintLevelStd : PhysicsLevelStd
{
  Point1 timeAllowed;
  
  SprintLevelStd();
  
  // new virtuals
  virtual void createLevel();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();
  
  virtual Coord1 calcWinBonus();
  
  void callback(ActionEvent* caller);
};

struct SPLevel0 : SprintLevelStd
{
  SPLevel0();
  void createLevel();
};

struct SPLevel1 : SprintLevelStd
{
  SPLevel1();
  void createLevel();
};

struct SPLevel2 : SprintLevelStd
{
  SPLevel2();
  void createLevel();
};

struct SPLevel3 : SprintLevelStd
{
  SPLevel3();
  void createLevel();
};

struct SPLevel4 : SprintLevelStd
{
  SPLevel4();
  void createLevel();
};

struct SPLevel5 : SprintLevelStd
{
  SPLevel5();
  void createLevel();
};

struct SPLevel6 : SprintLevelStd
{
  SPLevel6();
  void createLevel();
};

struct SPLevel7 : SprintLevelStd
{
  SPLevel7();
  void createLevel();
};

struct SPLevel8 : SprintLevelStd
{
  SPLevel8();
  void createLevel();
};

struct SPLevel9 : SprintLevelStd
{
  SPLevel9();
  void createLevel();
};

struct SPLevelTest : SprintLevelStd
{
  SPLevelTest();
  void createLevel();
};

// ================================ PirateLevel ============================= //

struct PirateLevel : PhysicsLevelStd
{
  Logical startedWin;
  Point1 timeAllowed;
  
  PirateLevel();
  
  // new virtuals
  virtual void createLevel();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();
  
  virtual Coord1 calcWinBonus();
  virtual Coord1 standardEnemyDrop() {return RANDOM_GEM;}

  virtual void drawPirateShipFront();

  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct GemSpiraler : ActionEvent, ActionListener
{
  PizzaCoinStd* gem;
  PirateChest* chest;
  
  Point1 currDistance;
  Point1 currAngle;

  LinearFn distanceMover;
  LinearFn angleMover;
  
  GemSpiraler(PizzaCoinStd* setGem, PirateChest* setChest);
  void updateMe();
  void callback(ActionEvent* caller);
};

struct PILevel0 : PirateLevel
{
  PILevel0();
  void createLevel();
};

struct PILevel1 : PirateLevel
{
  PILevel1();
  void createLevel();
};

struct PILevel2 : PirateLevel
{
  PILevel2();
  void createLevel();
};

struct PILevel3 : PirateLevel
{
  PILevel3();
  void createLevel();
};

struct PILevel4 : PirateLevel
{
  PILevel4();
  void createLevel();
};

struct PILevel5 : PirateLevel
{
  PILevel5();
  void createLevel();
};

struct PILevel6 : PirateLevel
{
  PILevel6();
  void createLevel();
};

struct PILevel7 : PirateLevel
{
  PILevel7();
  void createLevel();
};

struct PILevel8 : PirateLevel
{
  PILevel8();
  void createLevel();
};

struct PILevel9 : PirateLevel
{
  PILevel9();
  void createLevel();
};

struct PILevelTest : PirateLevel
{
  PILevelTest();
  void createLevel();
};

// ================================ FiremanLevelStd ============================= //

struct FiremanLevelStd : PhysicsLevelStd
{
  Point1 timeAllowed;
  
  FiremanLevelStd();
  
  // new virtuals
  virtual void createLevel();
  
  // overrides
  void preload();
  void loadRes();
  void unloadRes();
  
  void initSublevel();
  void updateCamera(Logical snapIntoPlace);
  void updateLevelAI();
  void drawHUD();
  
  virtual Coord1 calcWinBonus();
  Coord1 closestBalloonsDir();  // returns -1 or 0 or 1
  
  void callback(ActionEvent* caller);
};

struct FILevel0 : FiremanLevelStd
{
  FILevel0();
  void createLevel();
};

struct FILevel1 : FiremanLevelStd
{
  FILevel1();
  void createLevel();
};

struct FILevel2 : FiremanLevelStd
{
  FILevel2();
  void createLevel();
};

struct FILevel3 : FiremanLevelStd
{
  FILevel3();
  void createLevel();
};

struct FILevel4 : FiremanLevelStd
{
  FILevel4();
  void createLevel();
};

struct FILevel5 : FiremanLevelStd
{
  FILevel5();
  void createLevel();
};

struct FILevel6 : FiremanLevelStd
{
  FILevel6();
  void createLevel();
};

struct FILevel7 : FiremanLevelStd
{
  FILevel7();
  void createLevel();
};

struct FILevel8 : FiremanLevelStd
{
  FILevel8();
  void createLevel();
};

struct FILevel9 : FiremanLevelStd
{
  FILevel9();
  void createLevel();
};

struct FILevelTest : FiremanLevelStd
{
  FILevelTest();
  void createLevel();
};

// ==================================== LakeLevel ================================ //

struct LakeBackground;

struct LakeObjman : ObjmanStd<LakeGO>
{
  Point1 tide;
  
  LakeObjman() : ObjmanStd<LakeGO>(), tide(0.0) {}
};

struct LakeLevel : PizzaLevelInterface
{
  LakeObjman objman;
  LakePlayer* player;
  LakeBackground* bg;
  
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
  void updateCamera(Logical snapIntoPlace);

  void addPGO(LakeGO* pgo);
  void addAction(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  void ateFish(LakeGO* fish);
  void playerDamaged(Point1 dmg);
  void drawHUD();
  Point2 heartPos(Coord1 heartID);
  
  void placeLakeGOStd(LakeGO* lgo);  // sets the position offscreen
  LakeFish* createNormalFish(Coord1 size);
  LakeFish* createNormalFish(const LakeFish& original);
  void placeAndAddLGO(LakeGO* lgo);  // calls placeLakeGOStd and addPGO
  /*
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
   */
  void callback(ActionEvent* caller);
  
  // new virtuals
  virtual void populateWorld() = 0;
  
  // overrides
  void preload();
  void loadRes();
  void initRes();
  void initSublevel();
  void unloadRes();
  Logical shouldLoadPizza() {return false;}
  Logical usesHealthBar() {return false;}
  
  void update();
  void redraw();
};

struct LALevelEasy : LakeLevel
{
  void populateWorld();
};

struct LALevelLobster : LakeLevel
{
  void populateWorld();
};

struct LALevelLobster2 : LakeLevel
{
  void populateWorld();
};

struct LALevelPearl : LakeLevel
{
  void populateWorld();
};

struct LALevelJellyfish : LakeLevel
{
  void populateWorld();
};

struct LALevelJellyfish2 : LakeLevel
{
  void populateWorld();
};

struct LALevelBarrel : LakeLevel
{
  void populateWorld();
};

struct LALevelBarrel2 : LakeLevel
{
  void populateWorld();
};

struct LALevelUrchin : LakeLevel
{
  void populateWorld();
};

struct LALevelSpineclam : LakeLevel
{
  void populateWorld();
};

struct LALevelSpineclam2 : LakeLevel
{
  void populateWorld();
};

struct LALevelSwordfish : LakeLevel
{
  void populateWorld();
};

struct LALevelStingray : LakeLevel
{
  void populateWorld();
};

struct LALevelShark : LakeLevel
{
  void populateWorld();
};

struct LALevelShark2 : LakeLevel
{
  void populateWorld();
};

struct LALevelTest : LakeLevel
{
  void populateWorld();
};


// ==================================== PlaneLevel ================================ //

struct PlaneLevel;
struct PlaneGO;
struct PlanePlayer;
struct PlaneBG;

struct PlaneObjman : ObjmanStd<PlaneGO>
{
  PlaneObjman() : ObjmanStd<PlaneGO>() {}
};

struct PlaneLevel : PizzaLevelInterface, b2ContactListener
{
  PlaneObjman objman;
  PlaneBG* bg;
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
  void queueGroup(Coord1 type, Coord1 groupSize);
  void smallShake();
  void drawHUD();
  
  void addAction(ActionEvent* ae);
  void addDebris(ActionEvent* ae);
  void addFrontAction(ActionEvent* ae);
  
  void addPGO(PlaneGO* enemy);
  
  void loadEnemy(Coord1 enemyType);
  static PizzaGOStd* createEnemy(Coord1 enemyType);  // ONLY generates the enemy, does NOT call addPGO or change xy
  
  // new virtuals
  virtual void createWave() = 0;
  virtual void playerDamaged() {}
  virtual void enemyDefeated(PlaneGO* pgo);
  
  // overrides
  void preload();
  void loadRes();
  void initRes();
  void initSublevel() {}
  void unloadRes();
  
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
  Coord1 type;
  Coord1 groupSize;
  
  CreatePLGroupCMD(PlaneLevel* setLevel, Coord1 setType, Coord1 setGroupSize);
  void execute();
};

struct CreatePLEnemyCMD : Command
{
  PlaneLevel* level;
  Coord1 type;
  Point2 xy;  // only flameskull uses this
  
  CreatePLEnemyCMD(PlaneLevel* setLevel, Coord1 setType,
                   Point2 setXY = Point2(0.0, 0.0));
  void execute();
};

struct PLLevelMohawk : PlaneLevel
{
  void createWave();
};

struct PLLevelChicken : PlaneLevel
{
  void createWave();
};

struct PLLevelBombbat : PlaneLevel
{
  void createWave();
};

struct PLLevelSpider : PlaneLevel
{
  void createWave();
};

struct PLLevelDonut : PlaneLevel
{
  void createWave();
};

struct PLLevelFlameskull : PlaneLevel
{
  void createWave();
};

struct PLLevelSpider2 : PlaneLevel
{
  void createWave();
};

struct PLLevelFlameskull2 : PlaneLevel
{
  void createWave();
};

struct PLLevelChip : PlaneLevel
{
  void createWave();
};

struct PLLevelDonut2 : PlaneLevel
{
  void createWave();
};

struct PLLevelWisp : PlaneLevel
{
  void createWave();
};

struct PLLevelChip2 : PlaneLevel
{
  void createWave();
};

struct PLLevelCupid : PlaneLevel
{
  void createWave();
};

struct PLLevelWisp2 : PlaneLevel
{
  void createWave();
};

struct PLLevelCupid2 : PlaneLevel
{
  void createWave();
};

// ==================================== WinCommand ================================ //

struct LevelOverCommand : Command
{
  PizzaLevelInterface* level;
  
  LevelOverCommand(PizzaLevelInterface* setLevel);
  void execute();
};

// ==================================== Tutorial ================================ //

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

// ==================================== BGSpineObject ================================ //

struct BGSpineObject : VisRectangular
{
  SpineAnimator mySpineAnim;
  Point1 speedMult;  // just a convenience value to animate faster or slower
  
  // use BGLayerSuggestion, it's up to the individual background to decide where each layer actually draws
  Coord1 layerSuggestion;
  
  BGSpineObject();
  
  // these are all copied directly from PizzaGOStd
  virtual void initSpineAnim(SpineAnimator& anim, Coord1 spineResIndex, const String1& startTrackName, Logical looping);
  virtual void initSpineAnim(Coord1 spineResIndex, const String1& startTrackName, Logical looping = true);
  
  virtual void updateSpineAnim(SpineAnimator& anim);
  virtual void renderSpineAnim(SpineAnimator& anim);
  virtual void updateSpineAnim();
  virtual void renderSpineAnim();
  virtual Logical usingSpineAnim();
  virtual Logical onRMScreen();  // checks against RM_BOX, not device screen
};

// ==================================== PizzaBackground ================================ //

struct PizzaBackground : StdBackground
{
  PizzaLevelInterface* level;
  
  DataList<BGSpineObject*> spineObjList;

  Point1 totalWidth;
  Point2 trackBounds;
  Logical foregroundDisabled;  // balance uses this, individual bgs must implement

  // backgrounds with foreground objects use this
  ArrayList<Point1> fgObjXVals;
  ArrayList<Coord1> fgObjImgIDs;
  
  PizzaBackground();
  virtual ~PizzaBackground();
  
  CameraStd applyHParallaxCam(Point1 scaleFactor);
  
  // zooms out with level cam
  void drawLandscapeTilesStd(Image* img, Point1 drawY, Point1 scaleFactor);  // zooms out with level cam

  // also draws a spineObjLayer, pass in -1 to not draw any spines
  void drawLandscapeTilesAndSpineObjsStd(Image* img, Point1 drawY,
                                         Point1 scaleFactor, Coord1 spineObjLayer,
                                         Logical objsAreFront = true);

  void drawLandscapeTilesStd(const ImageGrid& grid, Point1 drawY, Point1 scaleFactor);  // zooms out with level cam
  void drawLandscapeTilesFar(Image* img, Point1 drawY, Point1 scrollFactor);  // won't zoom out at all, only scroll

  void drawLandscapeTiles(Image* img, Point1 scaleFactor, Point1 scrollFactor);  // for computer
  void drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY);
  
  void drawTilingXYBG(Coord1 imgIndex, Point1 zoomRatio);
  
  // adds to the list automatically
  BGSpineObject* createSpineObj(Point2 xy, Coord1 spineIndex, const Char* startAnim,
                                Coord1 layerHint, Point1 speed = 1.0);

  void updateSpineObjects();
  void drawSpineObjLayer(Coord1 layer);
  void drawSpineObjLayer(Coord1 layer, Point1 xOffset);

  // use with algorithmic foreground objects
  void fgObjCreator(Point2 xRange, Coord2 imgIndexRange);
  void drawFGObjects(Point1 zoomFactor);

  // timing uses this one
  static void drawLandscapeTilesSlopes(Image* img, Point1 scaleFactor, Point1 drawY,
                                       const CameraStd& levelCam);
};

// ==================================== UFOBackground ================================ //

struct UFOBackground : PizzaBackground
{
  UFOBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== TestPanoBackground ================================ //

struct TestPanoBackground : PizzaBackground
{
  ImageGrid mainLayerGrid;
  ImageGrid objectLayerGrid;
  
  DataList<Point2> backgroundObjs;  // [objX, objIndex]
  
  TestPanoBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
  
  void objectCreator();
};

// ==================================== IntroLevelBackground ================================ //

struct IntroLevelBackground : PizzaBackground
{
  IntroLevelBackground();
  void load();
  void init();
  void update();
  void drawBackground();
};

// ==================================== GraveyardBackground ================================ //

struct GraveyardBackground : PizzaBackground
{
  ScrollingImage
  bgFog1,
  bgFog2,
  fgFog;
  
  Point1 oldCamX;

  GraveyardBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
  
};

// ==================================== JurassicBackground ================================ //

struct JurassicBackground : PizzaBackground
{
  JurassicBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== CakeBackground ================================ //

struct CakeBackground : PizzaBackground
{
  CakeBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== DumpBackground ================================ //

struct DumpBackground : PizzaBackground
{
  DumpBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== SiliconBackground ================================ //

struct SiliconBackground : PizzaBackground
{
  SiliconBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== BodyBackground ================================ //

struct BodyBackground : PizzaBackground
{
  BodyBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== CatacombsBackground ================================ //

struct CatacombsBackground : PizzaBackground
{
  CatacombsBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== PuppyBackground ================================ //

struct PuppyBackground : PizzaBackground
{
  ScrollingImage
  fgFog;
  
  PuppyBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== FiremanBackground ================================ //

struct FiremanBackground : PizzaBackground
{
  FiremanBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== PlaneBG ================================ //

struct PlaneBG : PizzaBackground
{
  ArrayList<ScrollingImage> layers;
  DataList<Point1> speedList;

  Point1 masterSpeed;
  
  PlaneBG();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== LakeBackground ================================ //

struct LakeBackground : PizzaBackground
{
  LakeBackground();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
};

// ==================================== PizzeriaBG ================================ //

struct PizzeriaBG : PizzaBackground
{
  Point1 fireZoom;  // equip gui zooms in
  
  PizzeriaBG();
  void load();
  void init();
  void update();
  void drawBackground();
  void drawForeground();
  
  void zoomIntoEquip();
  void backToShop();
};


#endif
