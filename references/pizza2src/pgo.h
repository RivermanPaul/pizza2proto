/*
 *  rgo.h
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 11/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RGO_H
#define RGO_H

#include "rmvisible2d.h"
#include "rmbasegob.h"
#include "rmactions.h"
#include "rmeffect.h"
#include "rmbehaviors.h"
#include "globals.h"
#include "rmspine.h"
#include "rmalgoterrain.h"
#include "rmbezier.h"

struct PhysicsLevelStd;
struct SpineAnimator;
struct PizzaPlayer;
struct FiremanBalloon;
struct BoneBreakEffect;

struct SortDrawer
{
  virtual Point1 sortVal() const = 0;
  virtual void drawSorted() = 0;
};

struct RenderableParticleBreakerInf
{
  RenderableParticleBreakerInf() {}
  virtual ~RenderableParticleBreakerInf() {}

  // mandatory calls
  virtual VisRectangular* get_visrect() = 0;
  virtual Box get_local_draw_AABB() = 0;
  virtual void render_for_particles() = 0;
  virtual void particles_ready(BoneBreakEffect* effect) = 0;

  // optional calls
  virtual void predraw_call() {}
  virtual void postdraw_call() {}
};

struct PizzaSpineAnimatorStd
{
  SpineAnimator mySpineAnim;
  Point1 currSpineTimeMult;

  PizzaSpineAnimatorStd();
  virtual ~PizzaSpineAnimatorStd() {}
  
  virtual VisRectangular* get_visrect() = 0;

  virtual void initSpineAnim(SpineAnimator& anim, Coord1 spineResIndex, const String1& startTrackName, Logical looping);
  virtual void initSpineAnim(Coord1 spineResIndex, const String1& startTrackName, Logical looping = true);
  
  virtual void setSpineAnim(Coord1 trackIndex, const Char* animName, Logical looping);
  virtual void setSpineAnim(SpineAnimator& anim, Coord1 trackIndex, const Char* animName, Logical looping);
  
  virtual void updateSpineAnim();
  virtual void updateSpineAnim(Point1 timeMult);
  virtual void updateSpineAnim(SpineAnimator& anim);
  virtual void updateSpineAnim(SpineAnimator& anim, Point1 timeMult);
  
  virtual void renderSpineAnim();
  virtual void renderSpineAnim(SpineAnimator& anim);
  
  virtual Logical usingSpineAnim();
};

struct LightsaberTouchResult
{
  SaberTouchType touchType;
  Point2 touchPt;
  PizzaGOStd* attackingPgo;
  
  LightsaberTouchResult() : touchType(SABER_TOUCHING_NOTHING), touchPt(0.0, 0.0), attackingPgo(NULL) {}
};

/*
struct DragonFireballInf
{
  TimerFn fireTimer;
  Point2 vel;
  
  DragonFireballInf();
  virtual ~DragonFireballInf() {}
  virtual void updateFireball();
  virtual void redrawFireball();
  virtual Circle myCircle() = 0;
};
 */

struct ShooterSeasCallback
{
  ShooterSeasCallback() {}
  virtual ~ShooterSeasCallback() {}
  virtual void shootFire() {}
  virtual void shootLightning() {}
};

struct ShooterSeasoning : ActionListener
{
  ShooterSeasCallback* parent;
  
  TimerFn fireballTimer;
  TimerFn lightningTimer;
  
  ShooterSeasoning(ShooterSeasCallback* setParent);
  void update();
  void redraw();
  void callback(ActionEvent* caller);
};

// =============================== EnemyPathMover ============================ //

// the flying path movers use this
struct EnemyPathMover
{
  PizzaGOStd* parent;
  
  DataList<Point2> pathPoints;
  Coord1 targetPathIndex;
  Coord1 pathMoveDir;  // 1 is forward (usually right) on the path, -1 is opposite
  Point1 speed;
  
  Coord1 endType;  // loop or pingpong
  
  enum
  {
    PATHMOVE_LOOP,
    PATHMOVE_PINGPONG
  };
  
  EnemyPathMover(PizzaGOStd* myParent);
  void init(const DataList<Point2>& path, Coord1 setMoveDir);  // sets enemy to closest path node
  void update();
  Coord2 nextPtIndex(Coord1 startIndex, Coord1 dir);  // [next index, next dir]
};

// =============================== PizzaGOStd ============================ //

struct PizzaGOStd : PhysRectangular, BaseGOLogic, PizzaSpineAnimatorStd,
    RenderableParticleBreakerInf, SortDrawer, ActionListener
{
  PhysicsLevelStd* level;
  PizzaPlayer* player;
  Image* shadowImg;  // can be null
  
  Image* dmgOverlayImg;

  ActionQueue script;
  ActionList actions;

  PizzaPhase phase;
  Coord1 type;
  Coord1 value;
  Coord1 basePoints;
  
  Coord1 coinToDropType;
  Coord1 numCoinDrops;
  Logical coinToDropIsPhysical;
  
  Point2 oldXY;
  
  Point1
  sortY,  // higher sortYs draw last (as in, in front of everything)
  crushValue,  // normal force required to crush physical
  dmgTimer,
  maxHP,  // player has its own system
  currHP,  // player has its own system
  saberStrikePower,  // instantaneous
  saberDOTPower;  // per second
  
  // this is used by certain gladiator-style enemies to determine when they are walking,
  // at what angle, when they should turn, etc.
  Line myPlatformLine;
  Point2 platformOffset;  // this is set by certain enemies that don't handle bottom center
  
  Logical
  hitThisAttack,      // already hit the player with the current attack
  flies,              // for the shadow, if false draws at sortY
  crushesPhysical,    // if true sets destroyedPhysical to true with enough force
  destroyedPhysical;  // tells this object to blow up during its next update

  DataList<b2Fixture*> lightsaberFixtures;
  
  DataList<PizzaGOStd*> hidingObjects;  // this is especially for blocks

  EnemyPathMover pathMover; // fliers use this
  
  static const Coord1
  SPIKED_BIT = 0x1 << 0,
  SLOW_BIT = 0x1 << 1,
  STICK_BIT = 0x1 << 2,
  BREAKAWAY_BIT = 0x1 << 3, // for the ikaros breakable platforms
  DELETE_BELOWWORLD_BIT = 0x1 << 4,  // for the puppy crates
  BALANCE_BOUNCE_BIT = 0x1 << 5;
  
  enum CoinValue
  {
    PENNY,
    NICKEL,
    DIME,
    QUARTER
  };
  
  PizzaGOStd();
  virtual ~PizzaGOStd();

  virtual void loadMe();
  virtual void updateMe();
  virtual void redrawMe();
  
  virtual void destroyPhysical();
  virtual void destructionStarted() {}
  virtual void drawDStruct();
  virtual Logical backDebris() {return true;}

  virtual void facePlayer();
  virtual void faceDir(Point1 dir);  // dir is either negative or positive
  
  virtual void walkAlongPlatform(Point1 speed, Logical withRotation, Logical withAutoHFlip, Logical withTurning);
  virtual Logical shouldTurn(); // enemy is facing wall and runs into it with collision circle
  virtual Logical turnAtWalls();  // returns true if a turn occured
  virtual Logical clampAtWalls();
  virtual Logical playerOverMyPlatform();
  
  virtual void pizzaSlammed() {}
  virtual Logical touchingPlayerCircle();
  virtual Logical touchingPlayerCircle(const Circle& circle);
  
  virtual void magicTriggerOn();
  virtual void magicTriggerOff();
  
  virtual void emergeFromBlock();  // make sure to update script and actions 
  
  virtual void drawLightsaber() {}
  virtual void touchedFire(Point2 location, PizzaGOStd* attacker) {}
  virtual void touchedFireball(Point2 location, PizzaGOStd* attacker);  // default implementation for enemies
  virtual void strikeWithLightning();
  
  // nothing < 100.0 gets sent to this
  virtual void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  virtual Logical objectShouldCrush(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  
  // for item blocks
  virtual void objectBumped(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse) {}

  virtual Logical standingPhysical(Point2* contactPt = NULL);  // uses physics engine
  virtual LightsaberTouchResult touchingLightsaber();  // uses physics engine
  virtual LightsaberTouchResult touchingPlayerLightsaber();  // uses physics engine
  virtual void updateLightsaberDmg();
  
  virtual void setMaxHP(Point1 newMax);  // also sets currHP
  virtual void addDamage(Point1 dmgVal);

  virtual Point1 sortVal() const;
  virtual void drawSorted();  // this is called on the items in the "3d" draw list
  virtual Box getLocalDrawAABB();
  virtual Circle collisionCircle();
  virtual Box visibleAABB();
  virtual Logical onRMScreen();  // checks against RM_BOX, not device screen

  virtual void checkPhysicalDestruction();
  Logical touchingPGO(PizzaGOStd* targetPGO, Point2* location = NULL);  // uses physics engine

  virtual void dropCoin();
  
  virtual Coord1 facingToDir() {return getHFlip() == false ? 1 : -1;}  // pizza overrides

  virtual void hideInBreakable(PizzaGOStd* breakable, Coord1 collisionMaskBits);
  virtual void parentBroke(PizzaGOStd* breakable, Coord1 collisionMaskBits);

  // from RenderableParticleBreakerInf
  virtual VisRectangular* get_visrect();
  virtual Box get_local_draw_AABB();
  virtual void render_for_particles();
  virtual void particles_ready(BoneBreakEffect* breakEffect);
  virtual void predraw_call();
  virtual void postdraw_call();
  
  virtual void callback(ActionEvent* caller) {}
};

// these are for the y sort
Logical operator<(const SortDrawer& pgo1, const SortDrawer& pgo2);
Logical operator>(const SortDrawer& pgo1, const SortDrawer& pgo2);

struct PizzaSpoke
{
  Point1 localAngle;  // this doesn't change as the pizza rotates. relative to pizza's 0 rotation
  Point1 length;
  Point2 worldEndpoint;  // DOES change as the pizza rotates. in world coords
  Logical touchingTerrain;
  
  PizzaSpoke();
};

struct PizzaFace : ActionListener
{
  VisRectangular* parent;
  Point1 saucePercent;
  Point1 glowAlpha;  // won't draw glow is alpha is 0
  
  SpineAnimator eyesWarper;
  SpineAnimator mouthWarper;
  SpineAnimator noseWarper;
  SpineAnimator accWarper;
  
  ActionQueue faceScript;
  
  // relative to pizza handle, unrotated
  // [x, y, local rotation]
  DataList<Point3> toppingPlacementData;

  Coord1 faceState;
  Coord1 idleState;  // this is ignored if faceState is not IDLE
  Coord1 frameTarget;
  Logical targetHFlip;
  Logical currHFlip;
  
  Point1 currWeight;
  Point2 breatheData;
  
//  PizzaWarper clothesWarper;
  SpineAnimator clothesWarper;
  Point1 clothesWeight;
  ActionQueue clothesScript;
  
  // this is mainly to allow turning to take over the next frame's speed
  Logical useOverride;
  Point1 overrideDuration;
  
  // ORDER IMPLIES PRIORITY
  enum
  {
    // idleStates
    BREATHE,
    BLINK,
    TURN,
    
    // faceStates
    IDLE,
    SLAM,
    BUMP,
    HURT,
    ROAR_1,
    ROAR_2
  };
  
  PizzaFace(VisRectangular* setParent);
  void initMySpine(SpineAnimator& anim, Coord1 spineIndex);
  void resetFace();  // sets to current outfit
  void placeToppings();
  void updateFace();
  void drawFace();
  Point1 damagePercent();
  Logical isHurt();
  
  void actionTrigger(Coord1 actionType, Logical ignorePriority = false);
  void idleTrigger(Coord1 idleType);
  void callback(ActionEvent* caller);
};

/*
struct PizzaRenderer
{
  VisRectangular* parent;
  PizzaFace face;
  
  PizzaRenderer(VisRectangular* setParent);
  void update();
  void redraw();
};
 */

struct PizzaPlayer : PizzaGOStd, ShooterSeasCallback, b2RayCastCallback
{
  PizzaFace face;
  
  TimerFn standingTimer;  // when active, allows jumping to account for tiny bounces
  TimerFn nojumpTimer; // keeps you from jumping in quick succession despite contacts
  Logical isStanding; // this is updated every frame
  Logical isSlamming; // doing a slam move
  Logical usedDoubleJump;  // resets on landing
  Logical waterJustReported; // this is updated every frame

  Point2 oldXY;  // this helps the camera in some cases
  Point2 standingXY;
  Coord1 facing;  // -1 left, 1 right

  Point1 slamAlpha;
  Point1 slamRotation;
  Point1 slamYVel;  // this is maintained during the slam

  TimerFn starTimer;
  TimerFn starMaker;
  
  TimerFn gourdoFlyTimer;
  
  FlashState flasher;
  TimerFn invincibilityTimer;  // from seasoning
  
  ArrayList<PizzaSpoke> spokeList;
  Point1 fullSpokeLength;  // can change this before load() call
  
  DataList<FiremanBalloon*> balloonList;

  // it's fine to change these any time
  Point1
  rollForce,
  rollTorque;
  
  ShooterSeasoning shooters;
  
  b2Fixture* lastRaycastFixture;
  Point2 lastRaycastPt;
  Point2 lastRaycastNormal;
  Point1 lastRaycastFraction;
  
  PizzaPlayer();
  void load();
  void updateMe();
  void redrawMe();

  void drawSquishyBase();
  void tryJump();  // level calls this
  Logical canJump();
  Logical canSlam();
  Logical canDoubleJump();
  void bounceUp(Point1 percentOfJump);
  void slamDown();
  void updateStanding();
  void updateFlying();
  void gotGourdo();
  void tryMove(Point1 normMagnitude);  // level calls this, [-1.0, 1.0]
  void setFacing(Coord1 newFacing);  // -1 left 1 right
  void createLightsaber();
  void drawLightsaber();

  void attacked(const Point2& atkPoint, PizzaGOStd* attacker);
  void damaged(const Point2& atkPoint, PizzaGOStd* attacker);  // an attack that caused damage
  void touchedFire(Point2 location, PizzaGOStd* attacker);  // attacker might be null
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void reportInWater();
  
  virtual Coord1 facingToDir() {return face.targetHFlip == false ? 1 : -1;}  // pizza overrides

  void updateSpokes();
  void updateFireblockDetector();
  Box collisionBox();
  
  // ShooterSeasCallback
  void shootFire();
  void shootLightning();

  // from b2RayCastCallback
  float32 ReportFixture(b2Fixture* hitFixture, const b2Vec2& point,
                        const b2Vec2& normal, float32 fraction);
};

// =============================== UnarmedSkeleton ============================ //

struct UnarmedSkeleton : PizzaGOStd
{
  Point1 walkSpeed;
  
  TimerFn directionChangeTimer;
  Logical willBackAway;
  Logical movingForward;  // true means it's walking in the direction it's facing
  
  UnarmedSkeleton();
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();  // this is called on the items in the "3d" draw list
  void updateCollisions();
};

// =============================== SkeletonSpearman ============================ //

struct SkeletonSpearman : PizzaGOStd
{
  Point1 walkSpeed;  // the level might set this directly, to 0.0 for example
  Circle attackCircle;  // stored mainly for debug
  
  // value
  enum
  {
    VAL_SIDEWAYS,
    VAL_VERTICAL
  };
  
  SkeletonSpearman(Coord1 setValue);
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();  // this is called on the items in the "3d" draw list
  void updateCollisions();
};

// =============================== Spiny ============================ //

struct Spiny : PizzaGOStd
{
  Logical flipped;
  Coord1 airState;
  
  Point1 yVel;  // this is for flipping
  LinearFn rotFlipper;
  TimerFn flipTimer;  // for flipping itself
  
  Logical shouldFollowPlayer;  // default true
  
  // airState
  enum
  {
    ON_GROUND,
    TO_BACK,
    TO_FRONT
  };
  
  Spiny();
  void load();
  void updateMe();
  void redrawMe();
  
  void updatePop();
  void pizzaSlammed();
  void flipToBack();
  void flipToFront(Logical fromSlam);
  
  Point1 groundTouchY();
  
  void updateCollisions();
  void callback(ActionEvent* caller);
};

// =============================== DonutCanon ============================ //

struct DonutHoleCannonball : PizzaGOStd
{
  TimerFn breakTimer;

  DonutHoleCannonball();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct DonutCannon : PizzaGOStd
{
  SpineAnimator faceAnim;
  Point1 currSpeed;
  Point1 bodyRotation;
  
  static const Point1 WALK_SPEED;
  
  DonutCannon();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

// =============================== Ent ============================ //

struct EntRoot : PizzaGOStd
{
  Box visibleBox;
  Logical dangerous;
  
  EntRoot();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct Ent : PizzaGOStd
{
  Point1 currSpeed;
  
  Coord1 animState;

  enum AnimState
  {
    WALKING,
    ATTACKING,  // starts the instant the windup starts, end instant cooldown ends
    GROUNDED
  };
  
  static const Point1
  WALK_SPEED,
  WALL_PAD;

  Ent();
  void load();
  void updateMe();
  void redrawMe();
  
  Logical isStable();
  Logical fullyGrounded();
  void startUnstable();
  
  void updateCollisions();
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);

  Logical shouldTurn(); // enemy is facing wall and runs into it with collision circle

  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

// =============================== GrimReaper ============================ //

struct GrimReaper : PizzaGOStd
{
  Point1 walkSpeed;
  Circle attackCircle;  // stored mainly for debug
  
  GrimReaper();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  Circle collisionCircle();

  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

// =============================== SkeleCupid ============================ //

struct SkeleCupid : PizzaGOStd
{
  ActionQueue fireScript;
  TimerFn turnTimer;
  Point1 velX;
  
  Point1 bobOffset;
  Point2 bobData;
  
  SkeleCupid();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  Circle collisionCircle();
  
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct CupidArrow : PizzaGOStd
{
  ThumbTackBehavior stickBehavior;
  Point2 vel;
  
  CupidArrow(Point2 origin, Point2 setVel);
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

// =============================== Giant ============================ //

struct Giant : PizzaGOStd
{
  Point1 currSpeed;
  Logical headThrown;
  
  static const Point1 WALK_SPEED;

  Giant();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  void midCallback(Coord1 value);
  
  Point2 skullPositionWalk();
  Point2 skullPositionThrow();
};

struct GiantSkull : PizzaGOStd
{
  TimerFn breakTimer;

  GiantSkull();
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// =============================== Wisp ============================ //

struct Wisp : PizzaGOStd
{
  WispMoverQuad mover;
  TimerFn stateTimer;
  
  Coord1 currState;
  Coord1 blinkCount;  // 0-2
  Logical seenPlayer;  // the ai starts when it's onscreen for the first time
  
  enum
  {
    VISIBLE,
    INVISIBLE
  };
  
  Wisp();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct WispFlame : PizzaGOStd
{
  TimerFn lifeTimer;  // this makes sure it goes away eventually
  Point2 vel;
  
  WispFlame(Point2 center, Point2 target);
  void updateMe();
  void redrawMe();

  void dissipate();
  Circle collisionCircle();

  void callback(ActionEvent* caller);
};

// =============================== Utensil ============================ //

struct Utensil : PizzaGOStd
{
  TimerFn turnTimer;
  
  static const Point1 WALK_SPEED;
  
  Utensil();
  void load();
  void updateMe();
  void redrawMe();
  
  Circle backArmCircle();
  Circle frontArmCircle();

  void updateCollisions();
  
  void callback(ActionEvent* caller);
};

// =============================== Chip ============================ //

struct Chip : PizzaGOStd
{
  ThumbTackBehavior stickBehavior;
  TimerFn attackTimer;  // only runs when attached
  
  Coord1 attachmentState;
  Coord1 flyDir;
  
  Point1 maxDangerousAngle;  // multiplied by PI in either direction
  
  enum
  {
    FLYING,
    ATTACHED
  };
  
  Chip();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  void pizzaSlammed();
  
  Logical shouldTurn(); // enemy is facing wall and runs into it with collision circle
  Logical turnAtWalls();  // returns true if a turn occured
  void walkAlongPlatform(Point1 speed, Logical withRotation, Logical withAutoHFlip, Logical withTurning);

  Circle collisionCircle();

  void callback(ActionEvent* caller);
};

// =============================== JumpingSpider ============================ //

// currently only supported in gladiator mode so that i don't have to deal with forces
struct JumpingSpider : PizzaGOStd
{
  Point1 currSpeed;

  String1 startWalkName;
  String1 startJumpName;
  Point2 startFlipRange;

  String1 oppWalkName;
  String1 oppJumpName;
  Point2 oppFlipRange;

  static const Point1 WALK_SPEED;

  JumpingSpider();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  Circle collisionCircle();
  Circle attackCircle();

  void callback(ActionEvent* caller);
};

// =============================== Scientist ============================ //

struct Scientist : PizzaGOStd
{
  Point1 currSpeed;

  static const Point1 WALK_SPEED;
  
  Scientist();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();

  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct TestTube : PizzaGOStd
{
  TimerFn breakTimer;
  Point1 breakAngle;
  Logical createAcid;

  TestTube();
  void load();
  void updateMe();
  void redrawMe();

  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void destroyPhysical();

  void callback(ActionEvent* caller);
};

// currently only supports the player and named sumo boss, but could easily be expanded
struct AcidPatch : PizzaGOStd
{
  AcidPatch(Point2 center);
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// =============================== Dozer ============================ //

struct Dozer : PizzaGOStd
{
  b2Fixture* flippedFixture;
  TimerFn crushTimer;
  
  SpineAnimator
  shell1Animator,
  shell2Animator,
  wheelAnimator;
  
  Dozer();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  void syncHFlipCollision();
  
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void drawDStruct();
  Logical backDebris() {return false;}

  Circle attackCircle();
};

// =============================== Caveman ============================ //

struct Caveman : PizzaGOStd
{
  TimerFn turnTimer;
  Point1 velX;
  
  Logical attackOn;
  
  Point1 secondsPerRoll;
  Point1 wheelRadius;
  
  Caveman();
  void load();
  void updateMe();
  void redrawMe();

  void syncRolling();
  
  void updateCollisions();
  Circle collisionCircle();
  Circle attackCircle();

  void callback(ActionEvent* caller);
};

// =============================== Alien ============================ //

struct Alien : PizzaGOStd
{
  Point1 currSpeed;
  
  TimerFn teleportTimer;
  Point1 nextTeleportSide;

  static const Point1 WALK_SPEED;
  
  Alien();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  Circle collisionCircle();
  
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct AlienLaser : PizzaGOStd
{
  Point2 origin;
  Line endpoints;  // the x is always the start, the y is the end (current position)
  Point2 vel;

  AlienLaser(Point2 startXY, Point2 setVel);
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

// =============================== BombBat ============================ //

struct BombBat : PizzaGOStd
{
  SpineAnimator wingAnimator;
  Point2 wingPt;

  Point1 bobOffset;
  Point2 bobData;
  
  Point1 currSpeed;
  Logical flying;
  
  BombBat();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateFlying();
  void updateAsBomb();
  
  void syncWingPt();
  
  void updateCollisions();
  Circle collisionCircle();
  
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

// =============================== HammerBro ============================ //

// currently these assume the ground is flat because of the way they shuffle back and forth
// it would not be that hard to fix though
struct HammerBro : PizzaGOStd
{
  Point2 startXY;
  Point3 walkXData;
  Logical isWalking;  // false means jumping
  
  ActionQueue throwScript;
  
  static const Point1 WALK_SPEED;
  
  HammerBro(Coord1 setValue);
  void load();
  void updateMe();
  void redrawMe();
  
  void enqueueShuffle(Point1 duration);
  void enqueueThrow(Coord1 numShots);
  void enqueueJump();  // automatically resets to shuffle

  void updateCollisions();
  
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct Hammer : PizzaGOStd
{
  TimerFn breakTimer;
  
  Hammer();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct Boomerang : PizzaGOStd
{
  Point1 flyDur;
  
  Boomerang();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct Dynamite : PizzaGOStd
{
  TimerFn breakTimer;

  Dynamite();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void destroyPhysical();
  void callback(ActionEvent* caller);
};

// =============================== Chicken ============================ //

struct Chicken : PizzaGOStd
{
  Point1 bobOffset;
  Point2 bobData;
  
  Point1 currSpeed;
  
  static const Point1 WALK_SPEED;
  
  Chicken();
  void load();
  void updateMe();
  void redrawMe();
  
  Circle collisionCircle();
  void updateCollisions();
  void updatePathMove();
};

// =============================== Flameskull ============================ //

struct Flameskull : PizzaGOStd
{
  Flameskull();
  void load();
  void updateMe();
  void redrawMe();
  
  Circle collisionCircle();
  void updateCollisions();
  void updatePathMove();
};

// =============================== Mohawk ============================ //

struct Mohawk : PizzaGOStd
{
  Mohawk();
  void load();
  void updateMe();
  void redrawMe();
  
  Circle collisionCircle();
  void updateCollisions();
  void updatePathMove();
};


// =============================== BombExplosion ============================ //

// BombBat uses this
struct BombExplosion : PizzaGOStd
{
  BombExplosion();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  void callback(ActionEvent* caller);
};

// =============================== ParticleTestSkull ============================ //

struct ParticleTestSkull : PizzaGOStd
{
  SpineAnimator* animator;

  ParticleTestSkull();
  
  void load();
  void updateMe();
  void redrawMe();
  Box getLocalDrawAABB();  // this assumes the handle point is (0,0)
};

struct BoneBreakEffect : PhysicalPolyParticles, ActionEvent, ActionListener
{
  TimerFn solidTimer;
  TimerFn fadeTimer;
  
  static Coord1 CURR_BREAK_IMG_INDEX;
  
  BoneBreakEffect(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
  void updateMe();
  void redrawMe();
  void setCollisions(Coord1 groupBits, Coord1 maskBits);  // wreck and bounce uses this
  void setRestitutions(Point1 rest);
  void setVelFromPt(Point2 explodePt, Point1 speed);
  void setVelFromPtPos(Point2 explodePt, Point1 speed);  // makes sure all go downward
  void callback(ActionEvent* caller);
};

// =============================== TerrainQuad ============================ //

struct TerrainQuad : PizzaGOStd
{
  Logical handledCenter;  // the blocks DO NOT UPDATE in the physics world
  Logical updatesFromPhysical;  // for static and kinetic blocks
  
  // this is to help snowboard skeletons and build sequences
  // getXY() is assumed to return top left on a terrain quad, ONLY SET ON QUADS
  Point2 topRight;
  
  Coord1 puppyEdgeBits;  // 0 = top left, 1-7 = left edge, 8 = left corner, 16 = top right...

  Logical isBouncy;
  Logical startBounce;
  Point1 bounceScaleOffset;
  CosineFn bouncer;
  
  Logical respondsToCupcakeFlame;
  Logical continuousBurn;
  Point1 igniteTimer;
  Point1 burnoutTimer;
  Logical balloonStartedSeekingMe;  // this is for water balloons
  
  VisRectangular fireVR;
  AnimationBehavior fireAnim;
  Coord1 firemanSetIndex;  // such as firemanTeddySet
  
  Coord1 disintigrateState;  // -1 means does not respond to disintigration calls
  ActionQueue disintigrateScript;

  // for moving platforms
  Coord1 targetPathIndex;  // if -1, path move is disabled.  make sure to add at least 1 path point
  Coord1 pathMoveDir;  // -1 = left, 0 = stop, 1 = right
  DataList<Point2> pathPoints;
  Point1 pathMoveSpeed;
  
  // for spinner
  Logical isSpinner;
  Logical spinningNow;
  ActionQueue spinScript;
  
  // for magic trigger blocks
  Logical isMagicTrigger;
  TimerFn magicTimer;
  DataList<PizzaGOStd*> magicObjectList;
  
  // for one way spiker/breaker blocks
  Logical usesSpikeSides;
  Coord1 spikeSides;  // 0b1 = right 0b10 = down 0b100 = left 0b1000 = up
  Logical usesBreakSides;
  Coord1 breakSides;  // 0b1 = right 0b10 = down 0b100 = left 0b1000 = up

  // for ! blocks
  TimerFn surpriseRestockTimer;
  Coord1 surpriseItemType;
  ActionQueue animScript;
  ActionList animQueue;
  
  AlgoBlock algoBlockDrawer;
  Coord1 algoDrawType;
  Quad algoQuadPts;
  
  RotationAnchor anchor;  // used by some terrain items that can spin

  enum AlgoDrawType
  {
    ALGOBLOCK_UNSET,
    ALGOBLOCK_QUAD,
    ALGOBLOCK_TRI,
    ALGOBLOCK_DISK  // width serves as radius
  };
  
  enum DisintigrateState
  {
    DOESNT_DISINTIGRATE = -1,
    FULLY_FORMED,
    FADING_OUT,
    FULLY_OUT,
    FADING_IN
  };
  
  TerrainQuad();
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  
  XFormer drawAlgo(Logical suppressDraw = false);
  
  Box collisionBox();

  void setBreakaway();
  
  void setSpikey();
  void setSpikeySide(Coord1 dir4Side);
  void setBreakSide(Coord1 dir4Side);
  
  void setSpinning();

  void checkSurface();
  void playerTouch(Point2 location);
  void explosionOnscreen(const Circle& explosion);
  void objectBumped(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);

  void setPuppyEdges(Coord1 numLeft, Coord1 numRight);
  void setPuppyCorners(Logical leftCorner, Logical rightCorner);
  void setBalanceBounce();
  void drawPuppyEdges();
  void ignoreAngrySkulls();
  void blockDestroyed();
  void setBouncy(Point1 newRestitution);
  
  // returns true if touching the cupcake's candle
  LightsaberTouchResult touchingBossCandle();
  
  void updateCupcakeBurn();
  Logical isBurning();
  void quenchBurning();

  void setDisintigrate();
  void updateDisintigrate();
  
  void enablePathMove();
  void updatePathMotion();
  
  void enableMagicTrigger(Point1 onTime = 5.0);
  void updateMagicTrigger();
  
  void updateSpinning();
  
  void setRedrockImgs();

  Logical drawsAlgo();
  Box getLocalDrawAABB();
  Box visibleAABB();

  void drawDStruct();

  // returns this for call chaining
  TerrainQuad* setRedrockTL(Point2 topLeft, Point2 widthHeight);
  TerrainQuad* setSteelTL(Point2 topLeft, Point2 widthHeight);
  
  void setGenericBlockTL(Point2 topLeft, Point2 widthHeight);
  void setDrawerRandomizedBreakable();
  void setDrawerWood();
  void setDrawerSandstoneLarge();
  void setDrawerSandstoneLargeCracked();
  void setDrawerSandstoneSmall();
  void setDrawerSandstoneSmallCracked();
  void setDrawerSteel();
  void setDrawerSpikes();
  void setDrawerCookie();
  void setDrawerGravestone();
  void setDrawerLavarock();
  void setDrawerTrash();
  void setDrawerWindow();
  void setDrawerBacteria();
  void setDrawerSkulls();

  virtual void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  virtual Logical objectShouldCrush(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);

  virtual void callback(ActionEvent* caller);

  // Points should be top left first the counter-clockwise by convention
  static TerrainQuad* create_static_quad(const Point2& pt0, const Point2& pt1,
                                         const Point2& pt2, const Point2& pt3);
  
  // NOTE: modfies image handle to match center of mass
  static TerrainQuad* create_block_quad(const Point2& pt0, const Point2& pt1,
                                        const Point2& pt2, const Point2& pt3,
                                        Image* img);
  
  static TerrainQuad* create_block_circle(Point2 center, Point1 radius, Image* img);
  
  static TerrainQuad* create_static_circle(Point2 center, Point1 radius, Image* img);

  static TerrainQuad* create_pachinko_peg(Point2 center, Point1 radius = 8.0);
  
  // bouncer and spike ball
  static TerrainQuad* create_bouncer(Point2 center, Point1 radius, Point1 restitution = 0.75);

  // the vertical piece has same dimensions
  static TerrainQuad* create_pachinko_cross(Point2 center, Point2 horiSize);

  /*
  static PlatformRiser* create_static_riser(const Point2& pt0, const Point2& pt1,
                                            const Point2& pt2, const Point2& pt3);
   */
  // dynamic, uses full size. image should NOT be null
  static TerrainQuad* create_block_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);
  static TerrainQuad* create_block_TL(Point2 topLeft, Image* setImg);
  static TerrainQuad* create_static_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);

  // dynamic, gets size from image
  static TerrainQuad* create_block_center(Point2 center, Image* setImg);
  static TerrainQuad* create_block_center(Point2 center, Point2 size,
                                          Point1 rotation, Image* setImg);
  
  // dyamic, CCW ORDER, null image is fine
  static TerrainQuad* create_block_tri(const Tri& worldTri, Image* setImg);
  
  // static, uses full size. image can be null
  static TerrainQuad* create_ground_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);
  
  // for redrock
  static TerrainQuad* create_redrock_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);  // dynamic
  static TerrainQuad* create_redrock_static_quad(const Point2& pt0, const Point2& pt1,
                                                 const Point2& pt2, const Point2& pt3);  // points in CCW
  static TerrainQuad* create_redrock_static_quad(const Point2& topLeft, const Point2& botRight);
  static TerrainQuad* create_redrock_block(const Point2& pt0, const Point2& pt1,
                                           const Point2& pt2, const Point2& pt3);  // points in CCW, dynamic
  static TerrainQuad* create_fading_redrock(Point2 topLeft, Point2 widthHeight);
  
  // Quad should be in CCW order
  static TerrainQuad* create_static_quad_themed(const Quad& quad, Coord1 bgIndex);

  static TerrainQuad* create_halfspike_TL(Point2 topLeft, Point2 widthHeight, Coord1 spikeDir);

  // graves, the actual handle point is changed to the top left
  static TerrainQuad* create_gravestone_BC(Point2 bottomCenter, Image* setImg);
  
  // the image needs to be
  static TerrainQuad* create_trike_block_TL(Point2 topLeft, Image* setImg);

  static TerrainQuad* create_donut_C(Point2 center, Image* setImg);
  static TerrainQuad* create_loaf_C(Point2 center, Image* setImg);
  static TerrainQuad* create_roll_C(Point2 center, Image* setImg);

  static TerrainQuad* create_mushroom(Point2 topLeft, Coord1 shroomIndex);
  
  static TerrainQuad* create_invis_wall(Point2 topLeft, Point2 botRight);
  
  static TerrainQuad* createFiremanBlock(Point2 topLeft, Point2 bottomRight);
  static void pickFiremanBlock(TerrainQuad* burningBlock);

  static TerrainQuad* createSpinner(Point2 topLeft, Point2 bottomRight);

  // 128x128
  static TerrainQuad* createSurpriseBlock(Point2 topLeft, Coord1 itemType);
};

// =============================== TerrainGround ============================ //

struct TerrainGround : PizzaGOStd
{
  AlgoTerrain terrainDrawer;

  // the animation part isn't implemented yet
  Logical isBouncy;
  Logical startBounce;
  Point1 bounceScaleOffset;
  CosineFn bouncer;
  
  // applies for a regular imaga, not a spine right now
  Point2 drawOffset;
  
  // this should be updated every frame but isn't currently,
  //   don't know any terrainground will need that.
  Box worldAABB;
  
  TerrainGround(const DataList<Point2>& worldPts);
  virtual void updateMe();
  virtual void redrawMe();
  
  virtual void setBouncy(Point1 newRestitution);
  virtual void setSpikey();
  virtual void setLava();

  virtual Box visibleAABB();
  virtual void checkSurface();
  virtual void playerTouch(Point2 location);

  virtual void setGraveyardGround(const DataList<Point2>& worldPts);
  virtual void setJurassicGround(const DataList<Point2>& worldPts);
  virtual void setCakeGround(const DataList<Point2>& worldPts);
  virtual void setDumpGround(const DataList<Point2>& worldPts);
  virtual void setSiliconGround(const DataList<Point2>& worldPts);
  virtual void setBodyGround(const DataList<Point2>& worldPts);
  virtual void setCatacombsGround(const DataList<Point2>& worldPts);
  virtual void setFiremanGround(const DataList<Point2>& worldPts);
  virtual void setPuppyGround(const DataList<Point2>& worldPts);

  virtual void setSpikeyGround(const DataList<Point2>& worldPts);
  virtual void setRedrockGround(const DataList<Point2>& worldPts);

  static TerrainGround* createNoThemeGround(const DataList<Point2>& worldPtsCW, Coord1 bgIndex,
                                           Logical ensureClosedLoop);
  static TerrainGround* createThemedGround(const DataList<Point2>& worldPtsCW, Coord1 bgIndex,
                                           Logical ensureClosedLoop);
  static TerrainGround* createThemedGround(const DataList<Point2>& worldPtsCW, Coord2 ptIndexRange,
                                           Coord1 bgIndex);
  
  static TerrainGround* createSpikeyGround(const DataList<Point2>& worldPts, Logical ensureClosedLoop = true);
  static TerrainGround* createSpikeyCircle(const Circle& circle);
};

struct PizzeriaBuyButton : PizzaGOStd
{
  enum PressState
  {
    UP_STATE,
    DISABLE_STATE,
    DOWN_STATE
  };
  
  Coord1 buttonIndex;  // level sets this
  Coord1 pressState;  // up,
  
  Logical shouldAnimate;
  
  b2Fixture* baseFixture;
  
  PizzeriaBuyButton();

  // overrides
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  virtual void pizzaSlammed();

  virtual void setState(Coord1 newState);
};

// ================================= MotionBomb ========================================//

struct MotionBomb : PizzaGOStd
{
  TimerFn stableTimer;  // this is to give the level a chance to stabilize it
  TimerFn fuseTimer;
  
  MotionBomb();
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// =============================== WaterArea ============================ //

struct WaterArea : PizzaGOStd
{
  DataList<Point2> worldPts;
  Box myAABB;
  
  WaterSimBox simBox;
  
  WaterArea(const DataList<Point2>& setWorldPts);
  void load();
  void updateMe();
  void redrawMe();
  
  Box visibleAABB() {return myAABB;}
  
  void checkDisturb(Box playerBox);
};

// =============================== Decoration ============================ //

struct Decoration : PizzaGOStd
{
  Decoration();
  Decoration(Point2 position, Image* setImg);
  
  void updateMe() {}
  void redrawMe();
};

// =============================== LavaPatch ============================ //

// currently only supports the player and named sumo boss, but could easily be expanded
struct LavaPatch : PizzaGOStd
{
  LavaPatch(Point2 topLeft, Point2 setSize);
  LavaPatch(const Box& box);
  void load();
  void updateMe();
  void redrawMe();
};

// ================================= JengaPuffball ========================================//

struct JengaPuffball : PizzaGOStd
{
  Point2 startXY;
  
  JengaPuffball();
  void load();
  void updateMe();
  void redrawMe();
};

// ================================= JengaBomb ========================================//

struct JengaBomb : PizzaGOStd
{
  JengaBomb();
  void load();
  void updateMe();
  void redrawMe();
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
};

// ================================= AngrySkull ========================================//

struct AngrySkull : PizzaGOStd
{
  Logical scared;
  Point1 switchAnimCheckTimer;
  
  AngrySkull();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void checkScared();
  
  void destructionStarted();
  
  Box getLocalDrawAABB();
  Box collisionBox() {return Box::from_center(getXY(), getSize() * 2.0);}
  Box visibleAABB() {return collisionBox();}
};

struct Billy;

// ================================= PuppyCage ========================================//

// set the xy to the bottom center
struct PuppyCage : PizzaGOStd
{
  TimerFn impactTimer;  // stops multiple fast impacts
  Coord1 damage;
  Billy* puppy;  // does not own, just to notify of destruction
  
  PuppyCage();
  void load();
  void updateMe();
  void redrawMe();
  void tryDamageCage();
  void explosionOnscreen(const Circle& explosion);
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void strikeWithLightning();
  void drawBack();  // puppy calls this before it draws
  void drawDStruct();
};

// ================================= Billy ========================================//

struct Billy : PizzaGOStd
{
  ThumbTackBehavior tackBehavior;
  
  TimerFn
  turnTimer,
  barkOnTimer,
  barkOffTimer,
  wagOnTimer,
  wagOffTimer;
  
  Coord1 breedIndex;  // set this before load
  Coord1 aiState;
  Point2 stickTargetOffset;
  Point1 currSpeed;  // for jumping toward player and ending hop
  Point1 xHopSpeed;
  Point1 timeForHop;
  
  static const Point1 HOP_GRAVITY;
  
  // the level currently counts on this being chronological order
  enum
  {
    WAIT_FOR_BREAK,
    WAIT_FOR_PLAYER,
    JUMP_FOR_PLAYER,
    ON_PLAYER,
    JUMP_OFF_PLAYER,
    GO_TO_HOUSE,
    IN_HOUSE
  };
  
  Billy();
  void load();
  void updateMe();
  void redrawMe();
  void updateAI();
  
  void cageBroken();
  void puppyJumpHome();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

// ================================= SprintFlag ========================================//

struct SprintFlag : PizzaGOStd
{
  Logical touched;
  
  SprintFlag();
  void updateMe();
  void redrawMe();
  Circle collisionCircle();
  Box visibleAABB();
};

// ================================= ShopChef ========================================//

// set the xy to the bottom center
struct ShopChef : PizzaGOStd
{
  ShopChef();
  void load();
  void updateMe();
  void redrawMe();
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  void startClap();
};


struct SumoAI;
struct SumoArena;
struct SumoLevel;

// ================================= PirateShip ========================================//

// set the xy to the bottom center
struct PirateShip : PizzaGOStd
{
  SpineAnimator frontAnim;

  Point2 startXY;
  
  PirateShip();
  void load();
  void updateMe();
  void redrawMe();
  Box visibleAABB();
};

struct PirateChest : PizzaGOStd
{
  PirateShip* ship;
  Point2 offsetXY;  // from ship

  PirateChest();
  void load();
  void updateMe();
  void redrawMe();
  Box visibleAABB();
};

// ================================= BalloonBasket ========================================//

// set the xy to the bottom center
struct BalloonBasket : PizzaGOStd
{
  TimerFn impactTimer;
  
  BalloonBasket();
  void load();
  void updateMe();
  void redrawMe();
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  Box visibleAABB();
};

struct FiremanBalloon : PizzaGOStd
{
  ThumbTackBehavior tackBehavior;
  TerrainQuad* myBurningObj;
  
  Coord1 aiState;
  Point2 stickTargetOffset;
  
  ArcsineFn xTowardPlayer;
  ArcsineFn yTowardPlayer;

  Point1 rotateSpeed;
  
  LinearFn xTowardObject;
  ActionQueue yTowardObject;
  
  // the level currently counts on this being chronological order
  enum
  {
    FLOAT_TO_PLAYER,
    ON_PLAYER,
    FLY_AWAY,
    JUMP_TO_OBJECT,
    EXPLODED
  };
  
  FiremanBalloon();
  void load();
  void updateMe();
  void redrawMe();
  void updateAI();
  
  void seekObject(TerrainQuad* obj);
  void flyAway();
  
  Circle collisionCircle();
  Box visibleAABB();
  void callback(ActionEvent* caller);
};

// ================================= BossAttachment ========================================//

struct BossAttachment
{
  VisRectangular* parent;  // have to set this before calling any other functions
  b2Fixture* fixture;
  Image* img;
  Point2 bossOffset;
  Logical touched;
  
  BossAttachment();
  void redraw();
  Point2 calcWorldXY();
  Point1 calcWorldRotation();
};

// ================================= BossSpore ========================================//

struct BossSpore : PizzaGOStd
{
  Point2 vel;
  Point2 accel;
  
  BossSpore();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
};

// ================================= SporeSaber ========================================//

struct SporeSaber : PizzaGOStd
{
  SporeSaber();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
};

// ================================= SumoBoss ========================================//

struct SumoBoss : PizzaGOStd
{
  SumoAI* ai;  // OWNS
  Coord1 geometry;
  
  Coord1 coinsLeft;  // # coin bursts not single coins
  Coord1 coinStyle;
  Logical justSlammed;
  TimerFn canBeSlammedTimer;  // if active won't throw out coins to prevent super fast collisions
  
  Point1 strengthMult;  // 1.0 should match player
  Point1 dmgStrMult; // from being slammed
  Logical isStanding;
  
  Point1 touchPlayerTime;  // resets while not touching player
  TimerFn sparkTimer;
  
  TimerFn
  shootTimer,
  shockTimer,
  hurtTimer,  // this is for the pirate boss's face
  ringTimer;
  
  TimerFn lavaTimer;
  Logical bouncesOnLava;
  
  Point2 dmgAngleRange;
  Logical blower;
  
  Logical hasWeakRange;
  Point2 weakAngleRange;
  
  Logical usesGem;
  Point1 gemGlowAlpha;
  Point1 gemGlowSpeedMult;  // may be set externally
  Point2 gemGlowData;
  
  ArrayList<BossAttachment> attachments;
  
  Point1 warningAlpha;
  
  // PizzaWarper warper;
  Point1 warpWeight;
  Coord1 tgtFrame;
  
  Logical losing;
  Logical loseDraw;
  TimerFn losingTimer;   // this is the blinking part
  TimerFn losingTimer2;  // this starts it fading out
  TimerFn loseFlameTimer;
  Point1 loseSinkSpeed;
  
  enum
  {
    CIRCLE,
    DIAMOND,
    HEXAGON,
    OCTAGON
  };
  
  enum
  {
    DROP_STD,
    DROP_UP
  };

  SumoBoss();
  virtual ~SumoBoss();   // free AI
  
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  
  virtual void updateStd();
  virtual void updateLose();

  virtual void attachmentHit(b2Fixture* attachmentFixture, BossAttachment* attachment) {}
  virtual void checkDamage();

  void tryRoll(Point1 normMagnitude);   // may be negative
  void bounceUp(Point1 percentOfJump);
  
  void drawGem();
  void drawAttachments();
  
  void slamResponder();
  void createCoins();
  void updateSparks();
  void sparkAt(Point2 center, Coord1 biggestIndex);
  void updateCollisions();
  void explosionOnscreen(const Circle& explosion);
  void startLoseSequence(Logical longerLose);
  void blowPlayer();
  void drawLightsaber() {}

  void touchedFire(Point2 location, PizzaGOStd* attacker);
  void collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse);
  Box collisionBox();  // size is radius
  void callback(ActionEvent* caller);
};

struct SumoAI : ActionListener
{
  SumoBoss* parent;
  PizzaPlayer* player;
  
  Coord1 actionState;
  TimerFn actionTimer;
  
  Point1 baseStrength;
  TimerFn strengthTimer;  // changes the power and density
  Point2 strengthRange;
  
  TimerFn powerTimer;  // for extra power
  TimerFn powerWait;
  
  TimerFn gustTimer;  // for the particles
  Point1 blowAngleOffset;  // the center of the "mouth"
  Point1 blowAngleWidth;  // max angle distance FROM CENTER
  
  LinearFn warpMover;
  LinearFn smallWarper;
  
  TimerFn saberSpinTimer;

  // customization values
  Point1 endTolerance;  // default 0.1
  Coord1 saveChance;  // default 4 = 80%
  Coord1 pushChance;  // default 3 = 75%
  Coord1 jumpChance;  // default 9 = 10%, does INSTEAD of regular roll
  
  enum FaceFrames
  {
    NEUTRAL_FRAME,
    MAD_FRAME,
    SAD_FRAME,
    
    NUM_FRAMES
  };
  
  enum
  {
    IDLE,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP_LEFT,
    JUMP_RIGHT,
    JUMP_UP
  };
  
  SumoAI(SumoBoss* setParent);
  virtual void update();
  virtual Coord1 selectAction();
  
  Point1 arenaPercent();
  Point1 playerArenaPercent();
  void updateBlowing();
  void faceCheck();
  void callback(ActionEvent* caller);
  
  // customization functions
  void setStrRange(Point1 low, Point1 high);
  void setAISpeed(Point1 timePerUpdate);
};

struct SumoRayBoss : SumoBoss
{
  Coord1 nextMovePt;
  Point1 currSpeed;

  DataList<Point2> movePts;
  
  static const Point1 RAY_MAX_SPEED;
  
  SumoRayBoss();
  virtual void load();
  virtual void setMovePts();
  
  virtual Point2 getCurrMovePt();
  virtual void updateStd();
  virtual void updateSwimming();
  virtual void syncTail();
  virtual void callback(ActionEvent* caller);
};

struct SumoJunkBoss : SumoBoss
{
  Point2 waitRange;
  Point2 jumpVel;
  Point1 spinVal;
  
  SumoJunkBoss();
  virtual void load();
  virtual void updateStd();
  virtual void callback(ActionEvent* caller);
};

struct SumoFungusBoss : SumoBoss
{
  SumoFungusBoss() {}
  void attachmentHit(b2Fixture* attachmentFixture, BossAttachment* attachment);
};

struct SumoEdisonBoss : SumoBoss
{
  DataList<b2Fixture*> saberFixtures;
  
  Coord1 saberSide;  // 0 = left, 1 = right
  
  SumoEdisonBoss() : saberSide(0) {}
  void attachmentHit(b2Fixture* attachmentFixture, BossAttachment* attachment);
  void turnOffSaber(Coord1 side);
  void turnOnSaber(Coord1 side);
  void drawLightsaber();
};

// ================================= Batclops ========================================//

// the top of its y motion is its y value when loaded
struct Batclops : PizzaGOStd
{
  // set both of these before load()
  Point1 startY;
  Point2 activeXRange;  // low x, high x. uses this range + the screen box to disappear and reappear
  
  Point3 yOffsetData;
  
  Batclops();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  void spawnNext();
};

// ================================= DragonFireballStd ========================================//

struct DragonFireballStd : PizzaGOStd
{
  ActionQueue script;
  Point1 colorWeight;
  Point2 vel;
  
  DragonFireballStd();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

// ================================= PlayerLightningStd ========================================//

struct PlayerLightningStd : PizzaGOStd
{
  ActionQueue script;
  Point2 target;
  
  PlayerLightningStd();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// ================================= ForceBone ========================================//

struct ForceBone : PizzaGOStd
{
  // set before loading
  PizzaGOStd* parent;
  
  Coord1 sideOfParent;  // -1 or 1, set automatically during load
  Point2 parentOffset;
  Point2 vel;
  
  TimerFn hoverTimer;
  TimerFn flyTimer;  // dies automatically after this much time so that it doesn't go forever
  
  ForceBone();
  void load();
  void updateMe();
  void redrawMe();

  void throwAtPlayer();
  void parentDead();
  void updateCollisions();
};

// ================================= PizzaCoinStd ========================================//

struct PizzaCoinStd : PizzaGOStd
{
  // set lifestate something other than active to make the coins not collide with player
  AnimationBehavior flipbookAnimator;
  TimerFn animPauseTimer;  // for gems
  
  Logical usePhysics;
  
  Point3 gemScaleData;
  
  PizzaCoinStd();
  PizzaCoinStd(Point2 center, Coord1 setValue = 0);

  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();

  virtual Logical isCoin();
  virtual Logical isGem();

  virtual void gotCoin();
  
  virtual void callback(ActionEvent* caller);
};

// ================================= GroudoLantern ========================================//

struct GourdoLantern : PizzaGOStd
{
  // set lifestate something other than active to make the coins not collide with player
  GourdoLantern();
  
  virtual void updateMe();
  virtual void redrawMe();
  virtual Circle collisionCircle();
};


// ================================= MagnetCoin ========================================//

struct MagnetCoin : PizzaCoinStd
{
  TimerFn progressTimer;
  
  Point2 startXY;
  Point1 startDist;
  Point1 tgtDist;
  
  Point1 angleToPlayer;
  Point1 rotSpeed;
  
  MagnetCoin(Point2 center, Coord1 setValue = 0);
  void load();
  void updateMe();
  void updateMotions();
  void callback(ActionEvent* caller);
};

// ================================= PachinkoBall ======================================== //

struct PachinkoBall : PizzaGOStd
{
  PachinkoBall();
  
  void load();
  void updateMe();
  void redrawMe();

  void updateCollisions();
};

// ================================= PachinkoHole ======================================== //

// checks for the player, but the balls handle the holes on their own
struct PachinkoHole : PizzaGOStd
{
  Coord1 numStars;  // 0 means it's a lose hole
  
  PachinkoHole();
  void updateMe();
  void redrawMe();
};

// ================================= LakeGO ======================================== //

struct LakeLevel;
struct LakePlayer;
struct LakeFish;

struct LakeGO : VisRectangular, BaseGOLogic, PizzaSpineAnimatorStd,
    ActionListener
{
  enum TypeID
  {
    LAKE_TYPE_PLAYER,
    LAKE_TYPE_FISH,
    LAKE_TYPE_HARPOON,
    LAKE_TYPE_PEARL,
    LAKE_TYPE_BUBBLE,
  };
  
  enum SizeID
  {
    SIZE_16,
    SIZE_32,
    SIZE_64,
    SIZE_128,
    SIZE_256,
    SIZE_512,  // this is only for the win sequence
    
    NUM_FISH_SIZES
  };
  
  PizzaPhase phase;
  Coord1 type;
  Coord1 sizeIndex;
  
  LakeLevel* level;
  LakePlayer* player;
  
  TimerFn mouthTimer;  // prevents rapid mouth changes
  TimerFn poisonTimer;
  
  ActionQueue script;
  ActionList actions;
  
  Logical poison;
  
  LakeGO();
  virtual ~LakeGO() {}
  
  void update();
  virtual void redrawMe();
  
  void swallowedByPlayer();  // effect + trans out + eat
  Logical touchingPlayer();
  Logical touchingPlayer(const Circle& circle);
  Logical touchingPlayerMouth();
  Logical touchingPlayerMouth(const Circle& circle);
  Logical outsideWorld();
  
  virtual Logical playerNearEating();  // larger collision plus correct facing
  virtual Circle mouthCircle();
  virtual Circle collisionCircle();
  virtual VisRectangular* get_visrect();

  void callback(ActionEvent* caller) {}
};

struct LakeFace : ActionListener
{
  VisRectangular* parent;
  // PizzaWarper faceWarper;
  ActionQueue faceScript;
  
  Coord1 faceState;
  Coord1 idleState;  // this is ignored if faceState is not IDLE
  Coord1 frameTarget;
  Logical targetHFlip;
  Logical currHFlip;
  
  Point1 currWeight;
  Point2 breatheData;
  
  // this is mainly to allow turning to take over the next frame's speed
  Logical useOverride;
  Point1 overrideDuration;
  
  enum
  {
    SWIM_0,
    SWIM_1,
    TURN,
    
    IDLE,
    MOUTH_OPEN,
    SWALLOW,
    HURT,
    SWALLOW_TRANS,
    TRANSFORM
  };
  
  LakeFace(VisRectangular* setParent);
  void resetFace();  // sets to current outfit
  void updateFace();
  void drawFace();
  Point1 damagePercent();
  Logical isHurt();
  
  void actionTrigger(Coord1 actionType);
  void idleTrigger(Coord1 idleType);
  void callback(ActionEvent* caller);
};

struct LakePlayer : LakeGO
{
  LakeFace face;
  
  SineFn scaler;
  Image* fadeInImage;
  Point1 fadeInAlpha;

  FlashState flasher;
  TimerFn invincibilityTimer;  // from seasoning
  
  Point2 vxy;
  Point1 sizeMult;
  Point1 amountEaten;  // this is sizeIndex + 1 of each fish
  Point1 drawAngle;
  
  Logical lostStandard;
  
  LakePlayer();
  virtual ~LakePlayer() {}
  
  void load();
  void updateMe();
  void redrawMe();
  
  void updateStd();
  void updateMotion();
  void tryJump();
  void tryMove(Point1 mag);  // -1 to 1
  
  void smallerNearby();  // called by other fish to open mouth
  void ateFish(LakeGO* fish);
  
  // return true if attack damaged the player
  Logical attacked(Point2 atkPoint, Point1 dmg = 1.0);
  Logical tempInvincible();
  void loseToDamage();
  
  void sizeUp();
  Coord1 nextSize(Coord1 currSize);
  void setFishSize(Coord1 size);
  Image* getFishImg(Coord1 size);
  Coord1 facingToDir();
  void setFacing(Coord1 newFacing);
  
  Circle mouthCircle();
  
  void callback(ActionEvent* caller);
};

struct FishMover
{
  LakeFish* parent;
  LakePlayer* player;  // parent sets this manually on load()
  
  Point1 speed;
  Point1 yOffset;
  Point2 bobData;
  Point1 bobMult;
  
  FishMover(LakeFish* setParent);
  void update();
  void swimForward();
  void lazyTowardPlayer();
  void fastTowardPlayer();
  void awayFromPlayer();
  void swim(Point1 speedTarget, Point1 angleTarget, Point1 turnSpeed);
};

struct LakeFish : LakeGO
{
  FishMover mover;
  // PizzaWarper bodyWarper;
  // PizzaWarper headWarper;
  
  Coord1 spineIndex;
  
  String1
  normalSwimStr,
  chaseSwimStr,
  scaredSwimStr,
  eatStr;
  
  Point1 bodyWeight;
  Point2 bodyWeightData;
  
  Point1 headWeight;
  
  Image* headImg;
  Logical onscreen;  // updated every frame
  
  Point1 swimDir;
  Point1 currSpeed;
  Point1 currAngle;
  
  Logical
  isShark,
  willChase,
  willRun;
  
  Coord1 swimState;

  enum FishAnimState
  {
    SWIM_NORMAL,
    SWIM_CHASE,
    SWIM_RUN,
    SWIM_BITE
  };
  
  LakeFish(Coord1 size);
  
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  
  virtual Logical facingPlayer();
  virtual void updateCollisions();
  virtual Point1 normalSpeed();
  virtual Point1 maxSpeed();
  virtual void swallowedPlayer();
  
  virtual void reportMovementType(Coord1 animType);
  
  virtual Circle mouthCircle();

  virtual void callback(ActionEvent* caller);
};

struct LakeShark : LakeFish
{
  LakeShark(Coord1 size);
  Point1 normalSpeed() {return 200.0;};
  Point1 maxSpeed();
};

struct FishSwallow : ActionEvent, ActionListener
{
  LakeGO* parent;
  Point2 swallowPt;
  Point1 swallowTime;
  Point1 moveSpeed;
  LinearFn scaler;
  
  FishSwallow(LakeGO* setParent, Point2 setSwallowPt);
  void updateMe();
  void callback(ActionEvent* caller);
};

struct LakeSwordfish : LakeGO
{
  Point1 xVel;
  Point1 startY;
  Point2 bobData;
  
  TimerFn speedTimer;
  Logical fastSwim;
  
  Point2 oldXY;  // for rotation
  
  LakeSwordfish();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
  Circle mouthCircle();  // this is the sword
  void callback(ActionEvent* caller);
};

struct LakeUrchin : LakeGO
{
  JitterBehavior jitter;
  
  LakeUrchin();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  virtual Circle collisionCircle() {return mouthCircle();}

};

struct LakeLobster : LakeGO
{
  Point3 swimData;
  Point1 speedMult;
  Point1 startX;
  
  LakeLobster();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle mouthCircle();  // this is the claws
};

struct LakeJellyfish : LakeGO
{
  Logical electricOn;

  Point2 centerPt;
  Point2 oldXY;
  Point1 startAngle;  // only for ones that split off
  Point1 deathAngle;
  
  Point1 motionWeight;
  Point1 humpRadius;
  Point1 speedMult;
  Point1 moveAnimTgt;  // for movement
  
  LakeJellyfish();
  void load();
  void updateMe();
  void redrawMe();

  Point2 localMotionCoords(Point1 humpRad, Point1 functionWeight);  // [0.0, 1.0]
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct LakeStingray : LakeGO
{
  Point2
  oldXY,
  target;
  
  Point1
  speedMult;
  
  TimerFn targetTimeout;

  LakeStingray();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle mouthCircle();  // this is the tail
  Circle mouthCircle2();  // this is the tail

  void pickTarget();
  
  void callback(ActionEvent* caller);
};

struct LakeSpineclam : LakeGO
{
  JitterBehavior jitter;
  Logical isClosed;
  
  LakeSpineclam();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  Circle topCircle();  // when open
  void callback(ActionEvent* caller);
};

struct LakePearl : LakeGO
{
  JitterBehavior jitter;
  
  LakePearl();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct LakeBarrel : LakeGO
{
  TimerFn explodeTimer;
  Point1 fallSpeed;
  Point1 rotateSpeed;
  
  TimerFn blinkTimer;
  Coord1 blinkIndex;
  
  LakeBarrel();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  void explode();
  
  Circle nearbyCircle();
  Circle explodeCircle();
  void callback(ActionEvent* caller);
};

// ==================================== PlaneGO ==================================//

struct PlaneLevel;
struct PlanePlayer;

struct PlaneGO : PhysRectangular, BaseGOLogic, PizzaSpineAnimatorStd,
    RenderableParticleBreakerInf, ActionListener
{
  PlaneLevel* level;
  PlanePlayer* player;
  
  ActionList actions;
  ActionQueue script;
  
  Logical
  hitThisAttack,
  destroyedPhysical;  // tells this object to blow up during its next update

  Coord1
  phase,
  type,
  basePoints,
  topCoinGiven;
  
  Point1
  maxHP,  // player has its own system
  currHP;  // player has its own system
  
  Point2 lastHitPt;  // for the explosion effect
  
  // various enemies use this
  Point1 bobOffset;
  Point2 bobData;
  
  enum PlaneFlags
  {
    BREAKS_STD = 0x1,  // default on
    SMOKES_STD = 0x2,   // default on
    IGNORES_BULLETS = 0x4 // default off
  };
  
  PlaneGO();
  virtual ~PlaneGO() {}
  
  Logical touchingPlayerCircle();
  Logical touchingPlayerCircle(const Circle& c);
  Logical onScreen();
  
  Point1 randYRanged();
  void breakEffectStd(Coord1 breakCols, Coord1 breakRows);  // does +- 1 on each
  
  void debugCircle(const Circle& circle, const ColorP4 outlineColor);
  void drawDebugStd();  // draws collision circle in white
  Logical explodeTouchingPlayer();  // destroy this and attack player if touching
  void smokePuffStd();
  void updateDMGColor();
  void updateDMGStd();  // updates dmg color and checks collision with player
  void createCoinStd();
  
  Logical visualAABBFullyOnscreen();

  // virtuals
  virtual void redrawMe();
  virtual Circle collisionCircle();
  virtual Circle touchingMyCircle(const Circle& c);  // circle's rad is - if no collision

  // true if attack succeeded
  virtual Logical attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg = 1.0);
  
  virtual void createCoin() {createCoinStd();}
  virtual void damagedCallback(Point2 atkPoint, PlaneGO* attacker) {}  // called after destruction
  virtual void destroyStd();
  virtual void destroyMe() {destroyStd();}
  virtual Point2 yRange();  // calculated based on object height
  
  // related to destruction
  virtual void checkPhysicalDestruction();
  virtual void destroyPhysical();

  // from RenderableParticleBreakerInf
  virtual VisRectangular* get_visrect();
  virtual Box get_local_draw_AABB();
  virtual void render_for_particles();
  virtual void particles_ready(BoneBreakEffect* effect);
  virtual void predraw_call();
  virtual void postdraw_call();
  
  virtual void callback(ActionEvent* caller) {}
};

struct PlanePlayer : PlaneGO
{
  PizzaFace* face; // it's a pointer for the linker's sake
  
  // PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
  TimerFn cooldownTimer;
  Logical changePropIndex;
  Coord1 propIndex;
  Point2 oldXY;
  
  ActionList flashList;
  
  PlanePlayer();
  ~PlanePlayer();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateTilting();
  void updatePropeller();
  void tryMoveToPoint(Point1 tiltMag);
  void tryMoveDirection(Point1 tiltMag);
  void tryFire();
  Logical attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg = 1.0);
  Box collisionBox();
  Circle collisionCircle();

  void callback(ActionEvent* caller);
};

struct MuzzleFlash : ImageEffect
{
  VisRectangular* parent;
  
  MuzzleFlash(VisRectangular* setParent);
  void updateMe();
  void redrawMe();
};

// player sets vel and rotation
struct PlanePlayerBullet : PlaneGO
{
  Point2 vel;
  
  PlanePlayerBullet();
  void load();
  void updateMe();
  void redrawMe();
  void createExplosion(const Point2& enemyCircleXY);
  void updateCollisions();
};

struct PlaneMohawk : PlaneGO
{
  PlaneMohawk();
  void updateMe();
  void redrawMe();
  Circle collisionCircle();
};

struct PlaneCupid : PlaneGO
{
  PlaneCupid();
  void load();
  void updateMe();
  void redrawMe();
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};
 
struct PlaneCupidArrow : PlaneGO
{
  ThumbTackBehavior stickBehavior;
  Point2 vel;
  
  PlaneCupidArrow(Point2 origin, Point2 setVel);
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
//  void destroyMe() {setRemove();}
  void callback(ActionEvent* caller);
};

struct PlaneBombbat : PlaneGO
{
  Point1 speed;
  
  SpineAnimator wingAnimator;
  Point2 wingPt;
  
  PlaneBombbat();
  void load();
  void updateMe();
  void redrawMe();
  
  void syncWingPt();
  void checkPushOnscreenY();
  
  Circle collisionCircle();
  
  void createExplodeEffect();
  
  void destroyPhysical();

  // so the wings are part of particles
  Box get_local_draw_AABB();
};

struct PlaneChip : PlaneGO
{
  Point1 xVel;
  DeAccelFn yMover;
  
  PlaneChip();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
};

struct PlaneDonut : PlaneGO
{
  Point1 xVel;
  
  PlaneDonut();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct PlaneDonutHoleCannonball : PlaneGO
{
  Point2 vel;
  
  PlaneDonutHoleCannonball();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct PlaneChicken : PlaneGO
{
  Point2 vel;
  
  PlaneChicken();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
};

struct PlaneFlameskull : PlaneGO
{
  PlaneFlameskull();
  void updateMe();
  void redrawMe();
};

struct PlaneSpider : PlaneGO
{
  Logical startsBottom;
  
  PlaneSpider();
  void load();
  void updateMe();
  void redrawMe();
  Circle collisionCircle();
  void updateAI(Point2 oldXY);
  void callback(ActionEvent* caller);
};

struct PlaneWisp : PlaneGO
{
  WispMoverQuad mover;
  TimerFn stateTimer;
  
  Coord1 currState;
  Coord1 blinkCount;  // 0-2
  Logical seenPlayer;  // the ai starts when it's onscreen for the first time
  
  enum
  {
    VISIBLE,
    INVISIBLE
  };
  
  PlaneWisp();
  void load();
  void updateMe();
  void redrawMe();
  
  Logical attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg);

  void midCallback(Coord1 value);
  void callback(ActionEvent* caller);
};

struct PlaneWispFlame : PlaneGO
{
  TimerFn lifeTimer;  // this makes sure it goes away eventually
  Point2 vel;
  
  PlaneWispFlame(Point2 center, Point2 target);
  void updateMe();
  void redrawMe();
  
  void dissipate();
  Circle collisionCircle();
  
  void callback(ActionEvent* caller);
};

// ========================= ExorGO =========================== //

struct ExorcismLevel;

struct ExorGO : VisRectangular, BaseGOLogic
{
  ExorcismLevel* level;
  
  Coord1 type;
  Coord1 value;
  
  Coord2 gridXY;
  Point2 drawOffset;  // so it can float around
  
  WispMover offsetMover;
  AccelFn dropper;
  
  ExorGO();
  void updateFloating();
  Box collisionBox() {return boxFromC();}
  
  virtual Logical isRequired();
  virtual Logical isSelectable();
  virtual void touched() {}
  virtual void setCellType(Coord2 newTypes);
};

struct ExorCell : ExorGO, ActionListener
{
  ArcsineFn fader;
  
  ExorCell();
  void setCellType(Coord2 newTypes);
  
  void updateMe();
  void redrawMe();
  
  void touched();
  void checkTouchBottom();
  void absorbToBottom();
  
  void callback(ActionEvent* caller);
};
#endif

