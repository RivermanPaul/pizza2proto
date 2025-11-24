/*
 *  pgo.h
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
#include "rmparticle.h"
#include "rmdmorph.h"
#include "resourceman.h"

struct PizzaLevel;
struct PizzaPlayer;

struct SortDrawer
{
  virtual Point1 sortVal() const = 0;
  virtual void drawSorted() = 0;
};

struct PizzaGO : PhysRectangular, BaseGOLogic, SortDrawer
{
  PizzaLevel* level;
  PizzaPlayer* player;
  Image* shadowImg;  // can be null
  
  Coord1 type;
  Coord1 value;
  Coord1 basePoints;

  Point1 sortY;  // higher sortYs draw last (as in, in front of everything)
  Point1 crushValue;  // normal force required to crush physical
  
  Logical hitThisAttack;  // already hit the player with the current attack
  Logical flies;   // for the shadow, if false draws at sortY
  Logical crushesPhysical;  // if true sets destroyedPhysical to true with enough force
  Logical destroyedPhysical;  // tells this object to blow up during its next update
  
  static const Coord1
  TYPE_PLAYER = 1,
  TYPE_SKELETON = 2,
  TYPE_TERRAIN = 3,
  TYPE_FROG = 4,
  TYPE_DEBRIS = 5,
  TYPE_SUMO_BOSS = 6,
  TYPE_PUPPY = 7,
  TYPE_ROLLBALL = 8,
  TYPE_PASSIVE = 9;  // aerial junk uses this until it lands, also coins
  
  // this is the "flags" of BaseGOLogic
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
  
  PizzaGO();
  virtual ~PizzaGO() {}
  
  virtual void updateMe();
  virtual void redrawMe();
  
  Logical touchingPGO(PizzaGO* targetPGO, Point2* location = NULL);  // uses physics engine
  Logical standingPhysical(Point2* contactPt = NULL);  // uses physics engine
  Logical shouldDrawArrowOverhead();  // for the bounce level
  Logical shouldDrawArrowSides();  // for the bounce level
  Logical shouldTurn(); // enemy is facing wall and runs into it with collision circle
  Logical turnAtWalls();  // returns true if a turn occured
  
  Box collisionBoxWalker();   // assumes handled bottom center, size is half box size
  
  // destroys body, calls breakstd, removes
  void destroyPhysicalStd(Coord2 breakRange, Image* breakImg);
  
  void smokePuffStd();
  void breakStd(Coord2 breakRange, Image* breakImg, Logical withSound = true);  // this is just the effect
  void facePlayer();
  void faceDir(Point1 dir);  // dir is either negative or positive
  void addSortDraw();
  Logical playerCollision();  // uses collisionCircle()
  Logical playerCollision(Circle c);
  Logical onScreen();  // checks collision with camera box
  Logical onScreenFixed();
  Logical onScreenAABB();  // for objects that can rotate
  Point1 sortVal() const;
  
  void createMagnetCoin();
  void createMagnetCoin(Coord1 coinVal);  // pass in a -val and it does nothing
  void createMagnetCoin(Point2 origin, Coord1 coinVal);

  // ======== virtuals 
  virtual Coord1 facingToDir();  // returns -1.0 or 1.0 depending on hflip, pizza overrides
  virtual void drawSorted();  // this is called on the items in the "3d" draw list
  virtual void pizzaSlammed() {}  
  virtual void checkPhysicalDestruction();
  virtual void destroyPhysical();
  virtual void puppyJumpHome(const Circle& explosion) {}
  virtual void explosionOnscreen(const Circle& explosion) {}  // due to bomb in puppy/sumo
  virtual void puppyTurn(Point1 newDir) {} // change sun, reset timing 
  
  // nothing < 100.0 gets sent to this
  virtual void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  
  virtual Circle collisionCircle();
};

// these are for the y sort
Logical operator<(const SortDrawer& pgo1, const SortDrawer& pgo2);
Logical operator>(const SortDrawer& pgo1, const SortDrawer& pgo2);

struct PizzaFace : ActionListener
{
  VisRectangular* parent;
  Point1 saucePercent;
  Point1 glowAlpha;  // won't draw glow is alpha is 0

  PizzaWarper faceWarper;
  PizzaWarper mouthWarper;
  ActionQueue faceScript;
    
  Coord1 faceState;
  Coord1 idleState;  // this is ignored if faceState is not IDLE
  Coord1 frameTarget;
  Logical targetHFlip;
  Logical currHFlip;
  
  Point1 currWeight;
  Point2 breatheData;
  
  PizzaWarper clothesWarper;
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
  void resetFace();  // sets to current outfit
  void updateFace();
  void drawFace();
  Point1 damagePercent();
  Logical isHurt();
  
  void actionTrigger(Coord1 actionType, Logical ignorePriority = false);
  void idleTrigger(Coord1 idleType);
  void callback(ActionEvent* caller);
};

struct PizzaPlayer : PizzaGO, ActionListener
{
  PizzaFace face;
  
  TimerFn standingTimer;  // when active, allows jumping to account for tiny bounces
  TimerFn nojumpTimer; // keeps you from jumping in quick succession despite contacts
  Logical isStanding; // this is updated every frame 
  Logical isSlamming; // doing a slam move

  Point2 oldXY;  // this helps the camera in some cases
  Point2 standingXY;
  Coord1 facing;  // -1 left, 1 right
  
  Point1 slamAlpha;
  Point1 slamRotation;
  Point1 slamYVel;  // this is maintained during the slam
  
  TimerFn starTimer;
  TimerFn starMaker;
  
  PizzaPlayer();
  void updateMe();
  void redrawMe();
  
  void updateStanding();
  void setFacing(Coord1 newFacing);  // -1 left 1 right
  void tryMove(Point1 normMagnitude);  // level calls this, [-1.0, 1.0]
  Coord1 facingToDir();
  
  Logical canJump();
  Logical canSlam();
  Logical starActive();
  void drawSlamStreak();
  void tryJump();  // level calls this
  void bounceUp(Point1 percentOfJump);
  void slamDown();

  void attacked(const Point2& atkPoint, PizzaGO* attacker);
  void damaged(const Point2& atkPoint, PizzaGO* attacker);  // an attack that caused damage
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  
  void setIntro();
  void endIntro();
  
  Box collisionBox();
  void callback(ActionEvent* caller);
  
  static void drawPizza(VisRectangular& pizza, Point1 saucePercent);
};

struct SkeletonSpearman : PizzaGO
{
  AnimationBehavior animation;
  Point1 walkSpeed;
  Circle attackCircle;  // stored mainly for debug
  
  // value 
  static const Coord1
  VAL_SIDEWAYS = 0,
  VAL_VERTICAL = 1;
  
  SkeletonSpearman(Coord1 setValue);
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();  // this is called on the items in the "3d" draw list
  void updateCollisions();
  Circle collisionCircle();
};

struct SkeletonTutorial : PizzaGO
{  
  AnimationBehavior animation;
  ShakeRegister shaker;
  Point1 walkSpeed;

  SkeletonTutorial();
  void load();
  void updateMe();
  void redrawMe();
  
  void setIntro();
  void drawSorted();
  void updateCollisions();
  Circle collisionCircle();
};

struct GLLevelTutorial;

struct BoxPlayer : PizzaGO, ActionListener
{
  GLLevelTutorial* tutLevel;
  Coord1 dmg;
  TimerFn dmgTimer;  // makes sure it doesn't trigger twice for 1 jump
  
  Point2 shakeOffset;
  TimerFn shakeTimer;
  LinearFn rotator;
  LinearFn yMover;
  
  Coord1 glowFrame;
  Point1 glowAlpha;
  
  BoxPlayer(GLLevelTutorial* setTut);
  void load();
  void updateMe();
  void redrawMe();

  void tryMove(Point1 tiltPercent);
  void tryJump(Point1 tiltPercent);
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  void callback(ActionEvent* caller);
};

struct PumpkinSwooper : PizzaGO, ActionListener
{
  AnimationBehavior animation;
  TimerFn xSwoopTimer;  // when active, he is x swooping
  Point2 swoopData;
  TimerFn swoopWaitTimer;
  
  Logical seenPlayer;
  Logical swooping;
  Point1 startY;
  Point1 bobOffset;
  Point2 bobData;
  
  PumpkinSwooper();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Box collisionBox();
  
  void callback(ActionEvent* caller);
};

// its position is where its feet are, but its size is radius
// wherever you set its y to before load() is where it assumes is ground
struct Spiny : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  
  Logical flipped;
  Coord1 airState;
  
  Point1 groundY;
  Point1 yVel;  // this is for flipping
  LinearFn rotFlipper;
  TimerFn flipTimer;  // for flipping itself
  
  // airState
  static const Coord1 
  ON_GROUND = 0,
  TO_BACK = 1,
  TO_FRONT = 2;
  
  static const Point1 DRAW_OFFSET;
  
  Spiny();
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();
  void updatePop();
  void pizzaSlammed();
  void flipToBack();
  void flipToFront(Logical fromSlam);
  
  void updateCollisions();
  void callback(ActionEvent* caller);
};

// its position is where its feet are, but its size is radius and circle is up
// wherever you set its y to before load() is where it assumes is ground
struct Skelostrich : PizzaGO, ActionListener
{
  AnimationBehavior walkAnim;
  AnimationBehavior flyAnim;   // up spear doesn't use this
  
  Circle attackCircle;  // it's a member for debug
  Coord1 airState;
  Logical seenPlayer;
  
  Point1 walkSpeed;

  TimerFn jumpTimer;
  Point1 groundY;
  Point1 yVel;  // this is for flipping

  // value 
  static const Coord1
  VAL_SIDEWAYS = 0,
  VAL_VERTICAL = 1;
  
  // aiState
  static const Coord1 
  ON_GROUND = 0,
  JUMPING = 1;  
  
  Skelostrich(Coord1 setValue);
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();
  void jump();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct SkeleCupid : PizzaGO, ActionListener
{
  AnimationBehavior idleAnim;
  AnimationBehavior wingAnim;
  AnimationBehavior fireAnim;
  AnimationBehavior* bodyAnim;
  
  TimerFn fireTimer;
  TimerFn turnTimer;
  Point1 velX;
  
  Point1 bobOffset;
  Point2 bobData;
  
  SkeleCupid();
  void load();
  void updateMe();
  void redrawMe();
  
  void beginFire();
  void fireArrow();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct CupidArrow : PizzaGO, ActionListener
{
  ThumbTackBehavior stickBehavior;
  Point1 xVel;
  Point1 rotationOffset;
  Point2 rotationData;
  
  CupidArrow(Point2 origin, Point1 setVelX);
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};


struct Naga : PizzaGO, ActionListener
{
  ArrayList<VisRectangular> pieces;
  Point1 tailLength;
  
  Point1 sinePercent;  // 0-1
  Point1 currAmplitude;
  Point1 currNumPeriods;
  
  TimerFn attackTimer;
  Point1 currSpeed;
  Logical attacking;
  
  Point1 flameGen;
  ActionList flameList;
  
  static const Point1 
  TAIL_LENGTH,
  TAIL_AMPLITUDE_START,
  TAIL_PERIODS_START,
  BASE_BONE_SIZE,
  MAX_ATTACK_SPEED,
  ATTACK_ACCEL;
  
  Naga();
  void load();
  void updateMe();
  void redrawMe();
  
  void updatePieces(Logical firstUpdate);
  void updateCollisions();
  void drawPieces();
  void generateFlames();
  void destroy();
  Circle collisionCircle();  // this is just the head
  void callback(ActionEvent* caller);
};

struct Wisp : PizzaGO, ActionListener
{
  PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
  WispMover mover;
  TimerFn stateTimer;
  
  Coord1 currState;
  Coord1 blinkCount;  // 0-2
  Logical seenPlayer;  // the ai starts when it's onscreen for the first time
  
  enum
  {
    VISIBLE,
    INVISIBLE,
    INVIS_BLINK,
    NUM_WISP_STATES
  };
  
  Wisp();
  void load();
  void updateMe();
  void redrawMe();
  
  void updateCollisions();
  
  void setVisible();
  void setInvisible();
  void setBlink();
  void fire();
  
  void callback(ActionEvent* caller);
};

struct WispFlame : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  TimerFn lifeTimer;  // this makes sure it goes away eventually
  Point2 vel;

  WispFlame(Point2 center, Point2 target);
  void updateMe();
  void redrawMe();
  void dissipate();
  void callback(ActionEvent* caller);
};

struct WispExplode : ParticleEffect
{
  WispExplode(Point2 center);
};

struct RobotShatter : ParticleEffect
{
  ProceduralBurst* burst;
  
  RobotShatter();
  void init(const VisRectangular& robot);
  void reset(const VisRectangular& robot);
  Point1* getTimePtr();
};

struct Robot : PizzaGO, ActionListener
{
  Point1 walkSpeed;
  Logical seenPlayer;
  
  AnimationBehavior 
  walkAnim,
  atkAnim,
  atkFinishAnim,
  *currAnim;
  
  RobotShatter shatter;
  ActionQueue shatterScript;
  
  VisRectangular flame;
  Point1 flameAlpha;
  Coord1 flameFrame;
  
  TimerFn attackTimer;  // updates while walking
  TimerFn flameTimer;   // updates while flame is actually active
  Logical attacking;
  
  Robot();
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();
  void updateCollisions();
  void attack();
  Logical phasedIn();
  
  Circle collisionCircle();  // this is the main body
  Circle attackCircle();
  Circle keyCircle();
  
  void callback(ActionEvent* caller);
};

// this is bound to the giant until one of them has a collision
struct GiantSkull : PizzaGO, ActionListener
{
  TimerFn breakTimer;
  
  GiantSkull();
  void load();
  void updateMe();
  void redrawMe();  
  void callback(ActionEvent* caller);
};

struct Giant : PizzaGO, ActionListener
{
  PizzaGO* skull;  // OWNS until collision occurs with giant or skull

  AnimationBehavior 
  walkAnim,
  throwAnim,
  afterThrowAnim,
  *currAnim;
  
  TimerFn throwTimer;
  
  static const Point1 WALK_SKULL_YOFFSET[];
  static const Point2 ATK_SKULL_OFFSET[];
  static const Point1 ATK_SKULL_ANGLES[];
  
  Giant();
  ~Giant();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void syncSkull();
  void disconnectSkull();
  void updateCollisions();
  
  void drawSorted();
  Circle collisionCircle();
  Circle skullCircle();  // this is only used if (skull != NULL)
  Point1 skullRotation(); // this is only used if (skull != NULL)

  void callback(ActionEvent* caller);
};

struct FlameSkull : PizzaGO
{
  Point1 glowAlpha;
  Coord1 glowFrame;
  
  FlameSkull();
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  virtual void updateCollisions();
  virtual void updateGlow();
  void poofOut();
  virtual void destroySkull();
};

struct RabbitAngry : PizzaGO, ActionListener
{
  Point1 speedMult;  // this is to randomize them
  AnimationBehavior animator;
  DeAccelFn upMover;
  AccelFn downMover;
  
  RabbitAngry();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();
  void updateCollisions();
  Circle attackCircle();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct TailSpiny : PizzaGO, ActionListener
{  
  PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
  Point1
  speedMult,
  headOffset,
  warningAlpha,
  moveHSpeed;
 
  Coord1 moveState;
  
  CosineFn vertMover;
  TimerFn waitTimer;
  
  enum
  {
    SEEK_PLAYER,
    WAIT_TO_UP,
    GOING_UP,
    WAIT_TO_DOWN,
    GOING_DOWN,
    
    NUM_TAIL_STATES
  };
  
  TailSpiny();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void drawSorted();
  void updateMovement();
  void updateCollisions();
  void seekPlayer(Logical speedingUp);
  
  Circle attackCircle();
  Circle collisionCircle();  // this is the middle of the body
  void callback(ActionEvent* caller);  
};

struct DiskEnemy : PizzaGO
{
  Point1 startY;
  Point1 hoverVal;
  Point1 speedMult;
  Point2 hoverData;
  Coord1 globeColorIndex;
  Logical playedSound;
  AnimationBehavior animator;  // this is after speed mult 
    
  DiskEnemy();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
};

struct EndingDebris : PizzaGO, ActionListener
{
  TimerFn lifeTimer;
  Coord1 boneType;
  
  EndingDebris(Coord1 setBoneType);
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct EndingSkull : PizzaGO
{
  TimerFn dmgTimer;  // prevent rapid damage
  Coord1 damage;
  Logical readyToDamage;
  
  EndingSkull();
  void load();
  void updateMe();
  void redrawMe();
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
};

struct SumoAI;
struct SumoArena;
struct SumoLevel;

struct SumoBoss : PizzaGO, ActionListener
{
  SumoAI* ai;  // OWNS
  Coord1 geometry;
  
  Coord1 coinsLeft;  // # coin bursts not single coins
  Coord1 coinStyle;
  Logical justSlammed;
  TimerFn canSlamTimer;  // if active won't throw out coins to prevent super fast collisions
  
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
  
  Point2 dmgAngleRange;
  Logical blower;
  
  Logical hasWeakRange;
  Point2 weakAngleRange;
  
  Logical usesGem;
  Point1 gemGlowAlpha;
  Point1 gemGlowSpeedMult;  // may be set externally
  Point2 gemGlowData;

  Point1 warningAlpha;
  
  PizzaWarper warper;
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
  
  void load();
  void updateMe();
  void redrawMe();
  
  void tryRoll(Point1 normMagnitude);   // may be negative
  void bounceUp(Point1 percentOfJump);
  
  void updateStd();
  void updateLose();
  
  void drawGem();
  void slamResponder();
  void createCoins();
  void updateSparks();
  void sparkAt(Point2 center, Coord1 biggestIndex);
  void updateCollisions();
  void explosionOnscreen(const Circle& explosion);
  void startLoseSequence(Logical longerLose);
  void blowPlayer();
  
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
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

struct SumoKnife : PizzaGO, ActionListener
{
  ThumbTackBehavior stickBehavior;

  Point2 vel;
  Logical wasAboveCam;
  Logical playedSound;
  
  Point1 rotationOffset;
  Point2 rotationData;
  
  Logical warning;
  Point1 warningAlpha;
  
  SumoKnife(Point2 origin, Point2 setVel);
  void updateMe();
  void redrawMe();
  void updateActive();
  void updateOut();
  Logical shouldWarn();
  void updateCollisions();
  Circle collisionCircle();  
  void callback(ActionEvent* caller);
};

struct SumoBossFlame : PizzaGO, ActionListener
{
  LinearFn fader;
  Point1 speed;
  
  SumoBossFlame(Point2 start, Point1 angle);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct SumoShockEffect : VisRectangular, Effect, ActionListener
{
  PizzaGO* boss;
  CameraStd* levelCam;
  SineFn fader;
  
  SumoShockEffect(PizzaGO* setBoss);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct SumoLoseFlame : VisRectangular, Effect, ActionListener
{
  LinearFn scaler;
  LinearFn fader;
  Point1 speedMult;
  
  static const Point1 SPEED;
  
  SumoLoseFlame(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct SumoRing : VisRectangular, Effect, ActionListener
{
  VisRectangular* parent;
  Point2 bossOffset;
  LinearFn scaler;
  
  SumoRing(VisRectangular* setParent);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct SumoBridge : PizzaGO
{
  ArrayList<PhysRectangular> links;
  ArrayList<b2Joint*> joints;
  
  RotationAnchor leftAnchor;
  RotationAnchor rightAnchor;
  
  // set XY to top left before calling load()
  SumoBridge();
  virtual ~SumoBridge();
  void load();
  void updateMe();
  void redrawMe();
};

struct CircleDebris : PizzaGO
{
  TimerFn stayTimer;  // won't try to disappear until after this time
  TimerFn maxTimer;  // destroys self after this time
  
  CircleDebris(Image* img);
  void load();
  void updateMe();
};

struct SmallSkull : PizzaGO, ActionListener
{
  PizzaWarper warper;
  ActionQueue blinkScript;
  Point1 blinkWeight;
  
  SmallSkull(Point2 center);  // size is radius
  void load();
  void updateMe();
  void redrawMe();
  Box collisionBox();
  void callback(ActionEvent* caller);
};

struct SnowboardSkele : PizzaGO
{
  AnimationBehavior animator;
  
  // size is full size
  LinearFn xMover;
  LinearFn yMover;
  
  SnowboardSkele(Point2 start, Point2 end);
  void updateMe();
  void redrawMe();
  Circle collisionCircle();
};

// the winning item of ski
struct Snowman : PizzaGO
{
  VisRectangular scarf;
  AnimationBehavior scarfAnim;
  
  Snowman();
  void updateMe();
  void redrawMe();
  Circle collisionCircle();
};

// not a physics obj
struct SkiFlameSkull : FlameSkull
{
  Point1 glowAlpha;
  Coord1 glowFrame;
  
  SkiFlameSkull();
  void load();
  void updateMe();
  void updateCollisions();
};

struct SlopeFlag : PizzaGO
{
  Logical big;
  AnimationBehavior animator;
  
  SlopeFlag(Point2 setBottom, Logical setBig);
  void updateMe();
  void redrawMe();
  Box collisionBox();
};

struct SlopeCoin : PizzaGO
{  
  AnimationBehavior animator;
  
  SlopeCoin(Point2 center, Coord1 setValue = 0);  // 0-3
  virtual void load();
  
  virtual void updateMe();
  virtual void redrawMe();  
  
  void gotCoin();
  
  static Coord1 randValueAll(Coord1 bestCoin = PizzaGO::QUARTER);
  static Coord1 randValueEven(Coord1 bestCoin = PizzaGO::QUARTER);
  static Coord1 toCash(Coord1 coinVal);
  static void setAnimator(VisRectangular* parent, AnimationBehavior& memberAnimator,
                          Coord1 coinVal, Coord1 coinFrame);
};

struct MagnetCoin : SlopeCoin, ActionListener
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

struct PhysicalCoin : SlopeCoin, ActionListener
{
  Logical removeOutsideWorld;
  TimerFn waiter;
  LinearFn fader;
  
  PhysicalCoin(Point2 center, Coord1 setValue = 0);
  void load();
  void updateMe();
  void callback(ActionEvent* caller);
};

struct UnlockFloater : PizzaGO
{
  Coord2 toppingID;
  
  Point1 glowRotation;
  Point1 bobOffset;
  Point2 bobData;
  
  Logical startedMagnet;
  Point2 startXY;
  Point1 currDistPercent;
  
  UnlockFloater(Coord2 setToppingID);
  void updateMe();
  void redrawMe();
  void updateFloater();
  void updateMagnet();
  void startMagnet();  // can call this after load
};

struct BrainMover;

struct KarateBrain : PizzaGO, ActionListener
{
  BrainMover* mover;  // OWNS, create during load()
  ActionQueue scaleScript;
  TimerFn illusionTimer;
  Point2 oldXY;
  
  Point1 innerRadius;
  TimerFn bounceTimer;
  Logical firstTouch;
  Logical mummyBounce;  // reports mummy bounce to level
  
  KarateBrain();
  virtual ~KarateBrain();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void enableIllusions(Point1 speed = 0.5);
  void setMummy();
  void callback(ActionEvent* caller);
};

struct BrainMover
{
  KarateBrain* parent;
  
  BrainMover(KarateBrain* setParent);
  virtual ~BrainMover() {}
  virtual void moveBrain() {}     // only moves logically
  virtual void animateBrain() {}  // only sets the image
};

struct BrainMoverWalk : BrainMover, ActionListener
{
  ActionList actions;
  ActionQueue script;
  
  Point2 start;
  Point2 target;
  Point1 linearSpeed;
  
  Logical walks;
  AnimationBehavior flyAnim;
  
  BrainMoverWalk(KarateBrain* setParent, Point2 setStart, Point2 setTarget);
  void moveBrain();  
  
  void setFlying();
  void animateBrain();
  void updateWalkFrame();
  void updateFlyFrame();
  void callback(ActionEvent* caller);
};

struct BrainMoverHop : BrainMover, ActionListener
{
  Point2 xBounds;
  Coord1 moveDir;
  
  Point1 linearSpeed;
  LinearFn sideMove;
  DeAccelFn upMove;
  AccelFn downMove;
  
  BrainMoverHop(KarateBrain* setParent, Point2 setXBounds);
  void moveBrain();
  void animateBrain();
  void callback(ActionEvent* caller);
};

struct BrainMoverCircle : BrainMover
{
  AnimationBehavior animator;
  Circle moveCircle;
  Point1 currAngle;
  
  BrainMoverCircle(KarateBrain* setParent, const Circle& circle);
  void moveBrain();
  void animateBrain();
};

struct KarateSpear : PizzaGO
{
  KarateSpear* neighbor;  // for the double retraction
  CosineFn yMover;
  
  KarateSpear();
  void load();
  void retract();
  void updateMe();
  void redrawMe();
};

struct KarateIllusion : ImageEffect
{
  KarateIllusion(const VisRectangular& parent);
};

struct JumpSensor : PizzaGO, ActionListener
{
  TimerFn lifeTimer;
  
  JumpSensor(const Circle& circle);
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// this is the ball you roll on in the spike levels
// size is radius, value is its id
struct BalanceSkull : PizzaGO
{  
  Point2 oldXY;
  Logical octagon;
  
  BalanceSkull();
  
  void load();
  void updateMe();
  void redrawMe();
  
  void tryRoll(Point1 normMagnitude);
  
  Circle collisionCircle();
};

struct BalanceGong : PizzaGO
{
  Logical readyToRing;
  Logical rung;
  Coord1 gongRingFrame;
  
  BalanceGong();
  void load();
  void updateMe();
  void redrawMe();
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
};

struct BounceFrog : PizzaGO, ActionListener
{
  PizzaWarper warper;
  Point1 warpWeight;
  
  TimerFn explodeTimer;
  ActionQueue blinkScript;
  
  Logical big;
  Logical heavy;
  Logical explodes;
  
  BounceFrog();
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  virtual void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  void callback(ActionEvent* caller);
};

struct PuppyMissile : PizzaGO
{
  AnimationBehavior animator;
  Point2 vel;
  
  Point1 rotationOffset;
  Point2 rotationData;
  
  Logical warning;
  Point1 warningAlpha;
  
  Logical wasOnscreen;
  Logical playedSound;
  
  PuppyMissile(Point2 origin, Point2 setVel);
  void updateMe();
  void redrawMe();
  Logical shouldWarn();
  void updateCollisions();
  Circle collisionCircle();  
};

struct BounceComet : PizzaGO, ActionListener
{
  Point1 fadeAlpha;
  LinearFn fader;  // value also used to scale
  
  Logical warning;  // defaults to false, opposite of regular arrow
  Point1 warningAlpha;
  
  Point1 glowAlpha;
  Coord1 glowFrame;
  
  Point2 vel;
  Point1 velMult;
  Point2 velMultData;
  Logical streaker;

  BounceComet(Point2 origin, Point2 setVel);
  virtual void updateMe();
  virtual void redrawMe();
  virtual void updateWarning();
  virtual void updateGlowFrame();
  virtual void updateCollisions();
  virtual void drawWarning();
  virtual Logical shouldWarn();
  virtual void startDissipate();
  virtual void callback(ActionEvent* caller);
};

struct BounceEnergyBall : BounceComet
{
  Logical drawnLast;
  
  BounceEnergyBall(Point2 origin, Point2 setVel);
  void updateMe();
  void redrawMe();
  void startDissipate();
};

struct Wormhole : PizzaGO, ActionListener
{
  TimerFn waiter;
  LinearFn fader;
  
  Wormhole(Point1 arrowWaitTime);  // fading is deremined from this
  void updateMe();
  void callback(ActionEvent* caller);
};

// no need to set the x
struct BounceShock : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  ActionQueue script;  
  Logical warningNow;
  Point1 warningAlpha;
  
  BounceShock();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct Billy;

struct PuppyCage : PizzaGO
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
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
};

struct Billy : PizzaGO, ActionListener
{
  DataList<Image*>* fullImgSet;
  AnimationBehavior jumpAnim;
  AnimationBehavior walkAnim;
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

struct SkullBomb : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  TimerFn explodeTimer;
  
  SkullBomb();
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// start/end scale, scale time, before fade time, and fade time are all customizable
struct BombExplosion : VisRectangular, Effect, ActionListener
{
  Point1 glowAlpha;
  Coord1 glowFrame;
  Point1 fadeAlpha;
  
  DeAccelFn scaler;
  TimerFn beforeFadeTimer;  // total time = beforeFadeTimer + fader
  LinearFn fader;
  
  BombExplosion(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct Mimic : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  ActionQueue jumpScript;
  TimerFn jumpTimer;
  
  Point2 oldXY;
  Point1 startY;
  Logical hasCoin;
  
  Mimic();
  void load();
  void updateMe();
  void redrawMe();
  
  Logical onGround();
  void updateCollisions();
  void puppyTurn(Point1 objectiveDir);
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  Circle attackCircle();
  void callback(ActionEvent* caller);
};

struct JumpingFireball : PizzaGO, ActionListener
{
  AnimationBehavior animator;
  
  DeAccelFn upMover;
  AccelFn downMover;
  TimerFn waitTimer;
  
  Point1 baseWaitTime;
  Point2 startXY;
    
  JumpingFireball();
  void setMovement(Point1 upVal, Point1 upTime, Point1 waitTime);  // call this before first update
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct Mole : PizzaGO, ActionListener
{
  SineFn upMover;
  CosineFn downMover;
  
  TimerFn waitThrowTimer;
  TimerFn riseTimer;
  
  Coord1 moleState;
  Logical hasCoin;
  Point2 oldXY;
  Point2 startXY;
  
  AnimationBehavior 
  upAnim,
  downAnim,
  throwAnim,
  resetAnim,
  downForwardAnim,
  *currAnim;
  
  enum
  {
    MOLE_DOWN,
    MOLE_GOING_UP,
    MOLE_UP,
    MOLE_GOING_DOWN
  };
  
  Mole();
  void load();
  void updateMe();
  void redrawMe();
  void puppyTurn(Point1 objectiveDir);
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  void callback(ActionEvent* caller);
};

struct Caltrop : PizzaGO, ActionListener
{
  TimerFn lifeTimer;  // prevents too many from existing onscreen at once
  
  Caltrop();
  void load();  // call before setting the velocity
  void updateMe();
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  void callback(ActionEvent* caller);
};

struct TrianglePivot : PizzaGO
{
  RotationAnchor anchor;
  
  TrianglePivot();
  void load();
  void updateMe();
  void puppyTurn(Point1 objectiveDir);
};

struct SquarePivot : PizzaGO
{
  RotationAnchor anchor;
  
  SquarePivot();
  void load();
  void updateMe();
  void puppyTurn(Point1 objectiveDir);
};

// change the sun's facing (hflip) to get it on the other side
struct AngrySun : PizzaGO, ActionListener
{
  PizzaWarper warper;
  Point1 warpWeight;
  Coord1 warpTarget;
  
  ActionQueue script;
  Coord1 sunState;
  
  Point1 flameGen;
  ActionList flameList;
  
  Point2 relativeCamXY;
  Point1 targetY;  // set this to aim for a different Y
  Logical swapsSides;  // swap sides after each attack
  Logical harmless;
  
  Logical wasLeftOfPlayer;
  Logical hitThisPass;
  Logical angerNextFrame;   // hack to get the sun to angry in the middle of a state
  Coord1 coinsLeft;
  
  Logical bobbing;
  Point1 bobOffset;
  Point2 bobData;
  
  enum
  {
    WAITING_OFFSCREEN,
    COMING_ONSCREEN,
    THREATENING,
    ATTACKING,
    MOVING_OFFSCREEN,
    
    NUM_SUNSTATES
  };
  
  AngrySun();
  void updateMe();
  void redrawMe();
  
  void updatePostCamera();
  void updateCollisions();
  void updateCoinToss();
  void generateFlames();
  Coord1 facingToCorner();
  void startAngry();
  void puppyTurn(Point1 objectiveDir);
  void puppyDone(Point2 sunEnd, Point1 duration = 1.0);  
  void bounceDone(Point2 playerOffset);
  
  void callback(ActionEvent* caller);
};

struct SunSpark : VisRectangular, Effect, ActionListener
{
  Point1 totalDur;
  Point1 angVel;
  
  LinearFn fader;
  LinearFn scaler;
  
  Logical fadeIn;
  
  SunSpark(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct StickyPatch : PizzaGO
{
  StickyPatch(Point2 leftCenter, Point1 angle);
  void load();
  void updateMe();
  void redrawMe();
};

struct LavaPatch : PizzaGO
{
  AnimationBehavior animator;
  
  LavaPatch(Point2 leftCenter, Logical small);
  void load();
  void updateMe();
  void redrawMe();
};

struct PlatformRiser;

struct TerrainQuad : PizzaGO, ActionListener
{
  // this is to help snowboard skeletons and build sequences
  // getXY() is assumed to return top left on a terrain quad, ONLY SET ON QUADS
  Point2 topRight;
  
  Coord1 puppyEdgeBits;  // 0 = top left, 1-7 = left edge, 8 = left corner, 16 = top right...

  Logical isBouncy;
  Logical startBounce;
  CosineFn bouncer;  // bouncy ones use this

  TerrainQuad();
  virtual void load();
  virtual void updateMe();
  virtual void redrawMe();
  
  void setBreakaway();
  void checkSurface();
  void playerTouch(Point2 location);
  void explosionOnscreen(const Circle& explosion);
  
  void setPuppyEdges(Coord1 numLeft, Coord1 numRight);
  void setPuppyCorners(Logical leftCorner, Logical rightCorner);
  void setBalanceBounce();
  void drawPuppyEdges();
  void ignoreAngrySkulls();
  void blockDestroyed();
  void setBouncy(Point1 newRestitution);
  
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);
  void callback(ActionEvent* caller);
  
  // Points should be top left first the counter-clockwise by convention
  static TerrainQuad* create_static_quad(const Point2& pt0, const Point2& pt1, 
                                         const Point2& pt2, const Point2& pt3);
  
  // NOTE: modfies image handle to match center of mass
  static TerrainQuad* create_block_quad(const Point2& pt0, const Point2& pt1, 
                                        const Point2& pt2, const Point2& pt3,
                                        Image* img);

  static TerrainQuad* create_block_circle(Point2 center, Point1 radius, Image* img);
  
  static TerrainQuad* create_static_circle(Point2 center, Point1 radius, Image* img);
  
  static PlatformRiser* create_static_riser(const Point2& pt0, const Point2& pt1, 
                                            const Point2& pt2, const Point2& pt3);
  
   // dynamic, uses full size. image should NOT be null
  static TerrainQuad* create_block_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);
  static TerrainQuad* create_block_TL(Point2 topLeft, Image* setImg);
  
  // dynamic, gets size from image
  static TerrainQuad* create_block_center(Point2 center, Image* setImg);
  static TerrainQuad* create_block_center(Point2 center, Point2 size, 
                                          Point1 rotation, Image* setImg);
  
  // dyamic, CCW ORDER, null image is fine
  static TerrainQuad* create_block_tri(const Tri& worldTri, Image* setImg);
  
  // static, uses full size. image can be null
  static TerrainQuad* create_ground_TL(Point2 topLeft, Point2 widthHeight, Image* setImg);
};

struct TerrainGround : PizzaGO
{
  TerrainGround(const ArrayList<Point2>& worldPts);
};

struct PlatformRiser : TerrainQuad
{
  Point1 targetY;
  Point1 riseSpeed;
  
  PlatformRiser();
  void load();
  void updateMe();
};

struct TerrainCircle : PizzaGO
{
  Logical dynamic;
  
  TerrainCircle(Image* setImg = NULL);
  void load();
  void updateMe();
  void redrawMe();
};

// for breakaway terrain pieces (does not use physics)
struct TerrainShatter : ParticleEffect
{
  TerrainShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
};

struct SpikePanel;

struct SkullSwitch : PizzaGO
{
  TerrainQuad* panel;  // DOES NOT OWN
  Logical swapped;
  LinearFn rotator;
  
  SkullSwitch();
  void updateMe();
  void redrawMe();
};

struct SpikePanel : PizzaGO
{
  LinearFn scaler;
  
  SpikePanel();
  void updateMe();
  void redrawMe();
  void spikeTrigger();
};

// this is used in sumo
struct BounceSegment : PizzaGO, ActionListener
{
  ActionQueue bounceScript;
  Logical squishes;  // set BEFORE load()
  Logical squishedNow;
  Point1 naturalRestitution;

  BounceSegment(Point2 topCenter);
  void load();
  void updateMe();
  void redrawMe();
  
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);  // for bounce 
  void callback(ActionEvent* caller);
};

// used in bounce castle
struct BounceSegment2 : PizzaGO, ActionListener
{
  ActionQueue bounceScript;
  Logical squishes;   // set BEFORE load()
  Logical squishedNow;
  Point1 naturalRestitution;
  
  BounceSegment2(Point2 topCenter);
  void load();
  void updateMe();
  void redrawMe();
  
  void collidedPhysical(PizzaGO* pgo, Point1 normalImpulse);  // for bounce 
  void callback(ActionEvent* caller);
};

struct BoneBreakEffect : PhysicalTriangleParticles, ActionEvent, ActionListener
{
  TimerFn solidTimer;
  TimerFn fadeTimer;
  
  BoneBreakEffect(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
  void updateMe();
  void redrawMe();
  void setCollisions(Coord1 groupBits, Coord1 maskBits);  // wreck and bounce uses this
  void setRestitutions(Point1 rest);
  void setVelFromPt(Point2 explodePt, Point1 speed);
  void setVelFromPtPos(Point2 explodePt, Point1 speed);  // makes sure all go downward
  void callback(ActionEvent* caller);
};

struct Decoration : VisRectangular, ActionEvent
{
  Decoration(Point2 position, Image* setImg);
  void updateMe();
  void redrawMe();
  Box imageBox();  // NOT CURRENTLY USED 
};

// ================================= Lake ========================================//

struct LakeLevel;
struct LakePlayer;
struct LakeFish;

struct LakeGO : VisRectangular, BaseGOLogic
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
  
  Coord1 type;
  Coord1 sizeIndex;
  
  LakeLevel* level;
  LakePlayer* player;
  
  TimerFn mouthTimer;  // prevents rapid mouth changes
  TimerFn poisonTimer;

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
};

struct LakeFace : ActionListener
{
  VisRectangular* parent;
  PizzaWarper faceWarper;
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

struct LakePlayer : LakeGO, ActionListener
{
  LakeFace face;
  
  SineFn scaler;
  Image* fadeInImage;
  Point1 fadeInAlpha;
  TimerFn dmgTimer;  // can't be hurt while active
  
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
  Logical attacked(Point2 atkPoint, Logical instantLose = false);
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

struct LakeFish : LakeGO, ActionListener
{
  FishMover mover;
  PizzaWarper bodyWarper;
  PizzaWarper headWarper;
  
  Point1 bodyWeight;
  Point2 bodyWeightData;
  
  Point1 headWeight;
  
  Image* headImg;
  Logical onscreen;  // updated every frame
  
  Point1 swimDir;
  Point1 currSpeed;
  Point1 currAngle;
  
  Logical willChase;
  Logical willRun;
  
  LakeFish(Coord1 size);
  
  void load();
  void updateMe();
  void redrawMe();
  
  Logical facingPlayer();
  void updateCollisions();
  Point1 normalSpeed();
  Point1 maxSpeed();
  void swallowedPlayer();
  
  Circle mouthCircle();
  void callback(ActionEvent* caller);
};

struct LakePuffer : LakeGO, ActionListener
{
  TimerFn waiter;
  TimerFn transTimer;
  Coord1 puffState;
  
  PizzaWarper deflatedWarper;
  PizzaWarper inflatedWarper;
  Point1 warpWeight;
  Point2 warpData;
  
  Point1 startY;
  Point2 bobData;
  
  enum
  {
    FULL_UNPUFF,
    FULL_PUFF
  };
  
  LakePuffer();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct LakeSwordfish : LakeGO, ActionListener
{
  Point1 xVel;
  Point1 startY;
  Point2 bobData;
  
  TimerFn speedTimer;
  Logical fastSwim;
  
  PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
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

struct Starfish : LakeGO, ActionListener
{
  Point2 origin;
  Point1 currAngle;
  Point1 circleRad;
  
  Point1 distance;
  Point2 distData;
  
  TimerFn eyesOpenTimer;
  TimerFn eyesClosedTimer;
  
  Starfish();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct Harpoon : LakeGO, ActionListener
{
  TimerFn warningTimer;
  Point1 warningAlpha;
  Point2 targetPt;
  Logical hitPlayer;
  
  Harpoon(Point2 setTarget);  // adds some to y value
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct LakeBarrel : LakeGO, ActionListener
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

struct LakeEel : LakeGO, ActionListener
{
  PizzaWarper warper;
  AnimationBehavior animator;
  Point1 warpVal;
  
  ArcsineFn xMover;
  ArcsineFn yMover;
  TimerFn waitTimer;
  
  Point2 oldXY;
  Point2 targetXY;
  Point1 shockAlpha;
  
  LakeEel();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct LakePearl : LakeGO, ActionListener
{
  TimerFn sparkleOnTimer;
  Point2 sparkleOffset;
  
  LakePearl();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct LakeBubble : LakeGO
{
  Coord1 value;  // 0-4
  Point1 ySpeed;
  Point1 xOffset;
  
  Point2 bobData;
  Point1 bobMult;
  
  Point1 baseScale;  // this is its "natural" scale
  Point1 squishPercent;
  Point2 squishData;
  
  static Point1 SIZE_MULTS[];
  
  LakeBubble();
  LakeBubble(Point2 center, Coord1 size);
  void setSize(Coord1 size);
  void updateMe();
};

struct LakeCoin : LakeGO
{
  AnimationBehavior animator;
  Coord1 value;
  
  Point1 speed;
  Point1 xOffset;
  Point2 bobData;
  Point1 bobMult;
  
  LakeCoin();
  void load();
  void updateMe();
};

struct LakeUnlockable : LakeGO
{
  Coord2 toppingID;
  
  Point1 glowRotation;
  Point1 bobOffset;
  Point2 bobData;
  
  Logical startedMagnet;
  Point2 startXY;
  Point1 currDistPercent;
  
  LakeUnlockable(Coord2 setToppingID);
  void updateMe();
  void redrawMe();
  void updateFloater();
  void updateMagnet();
  void startMagnet();  // can call this after load
};

struct PoisonPuff : PoofEffect
{
  Point2 vel;
  
  PoisonPuff(Point2 center);
  void updateMe();
};

struct FishShatter : ParticleEffect
{
  ProceduralBurst* burst;
  
  FishShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
  
  // faster, shorter version
  static FishShatter* createForBounce(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
  static FishShatter* createForPlane(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
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

// ==================================== Wreck ==================================//

struct WreckLevel;
struct WreckPlayer;

struct WreckGO : PhysRectangular, BaseGOLogic
{
  WreckLevel* level;
  WreckPlayer* player;
  
  Coord1 type;
  Logical hitThisAttack;
  Coord1 basePoints;
  
  enum
  {
    UNSET,
    BUILDING,
    SKELE_BONETHROWER,
    CIVILIAN,
    SKELE_CATCHER
  };
  
  WreckGO();
  virtual ~WreckGO() {}
  
  void removeHighStd();
  void removeLowStd();  // for things that fall
  Logical touchingPlayerPhys(Point2* location = NULL);
  Logical touchingPlayerCircle();
  Logical touchingPlayerCircle(const Circle& c);
  Logical onScreen();
  void destroyStd();
  
  // virtuals
  virtual void redrawMe();
  virtual void collidedPhysical(WreckGO* wgo, Point1 impulse) {}
  virtual void ballCollision() {} // this is for the occupants
  virtual void catchBaseball() {} // for catchers
  virtual Circle collisionCircle();
};

struct WreckChain : WreckGO
{
  ArrayList<PhysRectangular> links;
  ArrayList<b2Joint*> joints;
  
  Physical leftWall;  // invisible
  Physical rightWall; // invisible
  Physical topWall;
  
  WreckChain();
  ~WreckChain();
  
  void load();
  void updateMe();
  void redrawMe();
  
  Circle linkCircle(Coord1 linkID);
  Circle jointCircle(Coord1 jointID);
};

struct WreckPlayer : WreckGO
{ 
  PizzaFace face;
  AnimationBehavior powerAnim;
  Point1 powerAlpha;
  
  WreckPlayer();
  
  void load();
  void redrawMe();
  void updateMe();
  
  void tryMove(Point1 mag);  // -1.0 to 1.0
  void tryJump(Point1 multiplier);
  void attacked(Point2 atkPoint, Point1 magnitude = 100.0);
  Circle collisionCircle();
  Box collisionBox();
};

struct WreckBuilding : WreckGO
{
  AnimationBehavior fireAnim;
  Image* padImg;
  WreckGO* occupant;  // may be null, forwards collision and upper removal
  
  Coord1 variation;  // applies to regular windows
  Logical hardWindow;  // can't be destroyed
  Logical spikedWindow;  // damage on touch
  
  WreckBuilding(Logical leftSide, Point1 top);  // this doesn't set the position
  void load();
  
  void updateMe();
  void redrawMe();
  void setHardWindow();
  void setSpikeWindow();
  Box collisionBox();
  void collidedPhysical(WreckGO* wgo, Point1 impulse);
};

struct WreckSkele : WreckGO
{
  AnimationBehavior throwAnim;
  
  static Point2 ARM_OFFSETS[];
  
  WreckSkele(Logical leftSide, Point1 top);
  void updateMe();
  void redrawMe();
  void ballCollision();
};

struct WreckSkeleEasy : WreckGO
{
  AnimationBehavior animator;
  
  WreckSkeleEasy(Logical leftSide, Point1 top);
  void load();
  void updateMe();
  void ballCollision();
};

struct WreckBatter : WreckGO, ActionListener
{
  AnimationBehavior animator;
  Logical hitThisAttack;
  Point1 idleChooser;
  
  WreckBatter(Logical leftSide, Point1 top);
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle attackCircle();
  void ballCollision();
  void callback(ActionEvent* caller);
};

struct WreckGirl : WreckGO, ActionListener
{
  AnimationBehavior animator;
  CosineFn slider;
  Coord1 value;

  enum
  {
    WRECK_GIRL,
    WRECK_BOY,
    WRECK_OLD
  };  
  
  WreckGirl(Logical leftSide, Point1 top);
  void load();
  void updateMe();
  void redrawMe();
  void ballCollision();
  void callback(ActionEvent* caller);
};

struct WreckBone : WreckGO
{
  Point2 vel;
  
  WreckBone(Point2 start);
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  Circle collisionCircle();
};

struct WreckBaseball;

struct WreckSkeleCatcher : WreckGO, ActionListener
{
  WreckBaseball* myBall;  // null if not in hand
  AnimationBehavior throwAnim;
  AnimationBehavior catchAnim;
  
  WreckSkeleCatcher(Logical leftSide, Point1 top);
  void updateMe();
  void redrawMe();
  void catchBaseball();  // also call this when you start with the ball
  void ballCollision();
  
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct WreckBaseball : WreckGO, ActionListener
{
  LinearFn sideMover;
  DeAccelFn upMover;
  AccelFn downMover;
  
  Logical falling;
  Point2 oldXY;
  Point2 fallVel;
  
  WreckBaseball();
  void updateMe();
  void setTarget(Point1 tgtX);  // call this before updating
  void callback(ActionEvent* caller);
};

struct WreckCutterSkele : WreckGO, ActionListener
{
  AnimationBehavior animator;
  Logical thrown;
  
  WreckCutterSkele(Logical leftSide, Point1 top);
  void updateMe();
  void ballCollision();
  void callback(ActionEvent* caller);
};

struct WreckCutterBlade : WreckGO
{
  WreckCutterBlade();
  void load();
  void updateMe();
};

struct WreckShocker : WreckGO
{
  VisRectangular secondNode;
  Point1 animWeight;
  
  WreckShocker(Point2 xy0, Point2 xy1);
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
};

struct WreckBird : WreckGO, ActionListener
{
  AnimationBehavior hoverAnim;
  AnimationBehavior attackAnim;
  
  CosineFn xMover;
  CosineFn yMover;
  TimerFn waiter;
  
  WreckBird();
  void updateMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

// set the y, player crossing this triggers warning
struct WreckScreenShock : WreckGO, ActionListener
{
  VisRectangular UFO;
  VisRectangular blast;
  ActionQueue script;
  
  Point1 UFOYOffset;  
  Logical started; // player crosses y, triggers
  
  WreckScreenShock();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct WreckPowerup : WreckGO, ActionListener
{
  AnimationBehavior animator;
  LinearFn fader;
  
  WreckPowerup();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void callback(ActionEvent* caller);
};

struct WreckCreature : WreckGO
{
  AnimationBehavior attackAnim;
  AnimationBehavior climbAnim;
  
  Coord1 moveState;
  Point1 yVel;
  
  enum
  {
    WAITING,
    MOVING_DOWN
  };
  
  WreckCreature();
  void updateMe();
  void redrawMe();
  Circle attackCircle();
  void updateCollisions();
};

// only need to set x
struct WreckDebris : WreckGO, ActionListener
{
  TimerFn warningTimer;  // for spiked ones
  Point1 warningAlpha;
  
  WreckDebris();
  void load();
  void updateMe();
  void redrawMe();
  void setSpiked();
  void callback(ActionEvent* caller);
};

struct WreckFloatSquare : WreckGO
{
  WreckFloatSquare();
  void load();
  void collidedPhysical(WreckGO* wgo, Point1 impulse);
  void updateMe();
};

struct WreckCoin : WreckGO
{
  AnimationBehavior animator;
  Coord1 value;

  WreckCoin();
  void load();
  void updateMe();
};

struct WreckUnlockable : WreckGO
{
  Coord2 toppingID;
  
  Point1 glowRotation;
  Point1 bobOffset;
  Point2 bobData;
  
  Logical startedMagnet;
  Point2 startXY;
  Point1 currDistPercent;
  
  WreckUnlockable(Coord2 setToppingID);
  void updateMe();
  void redrawMe();
  void updateFloater();
  void updateMagnet();
  void startMagnet();  // can call this after load
};

struct WreckSpark : VisRectangular, Effect, ActionListener
{
  TimerFn waiter;
  LinearFn fader;
  Point1 speedMult;
  
  static const Point1 SPEED;
  
  WreckSpark(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// ==================================== PlaneGO ==================================//

struct PlaneLevel;
struct PlanePlayer;

struct PlaneGO : VisRectangular, BaseGOLogic
{
  PlaneLevel* level;
  PlanePlayer* player;
  
  Logical hitThisAttack;
  
  Coord1 
  type,
  basePoints,
  topCoinGiven;
  
  Point1 hitPoints;
  
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
  void explodeTouchingPlayer();  // destroy this and attack player if touching
  void smokePuffStd();
  void updateDMGColor();
  void updateDMGStd();  // updates dmg color and checks collision with player
  void createCoinStd();
  
  // virtuals
  virtual void redrawMe();
  virtual Circle collisionCircle();
  virtual Circle touchingMyCircle(const Circle& c);  // circle's rad is - if no collision
  
  // true if attack succeeded
  virtual Logical attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg = 1.0);
  
  virtual void createCoin() {createCoinStd();}
  virtual void damagedCallback(Point2 atkPoint, PlaneGO* attacker) {}  // called after destruction
  virtual void destroyStd();
  virtual Point2 yRange();  // calculated based on object height
};

struct PlanePlayer : PlaneGO, ActionListener
{
  PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
  TimerFn cooldownTimer;
  Logical changePropIndex;
  Coord1 propIndex;
  Point2 oldXY;
  
  ActionList flashList;
  
  PlanePlayer();
  void updateMe();
  void redrawMe();
  
  void updateTilting();
  void updatePropeller();
  void tryMoveToPoint(Point1 tiltMag);
  void tryMoveDirection(Point1 tiltMag);
  void tryFire();
  Logical attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg = 1.0);
  Box collisionBox();
  
  void callback(ActionEvent* caller);
};

struct MuzzleFlash : ImageEffect
{
  VisRectangular* parent;
  
  MuzzleFlash(VisRectangular* setParent);
  void updateMe();
};

// player sets vel and rotation
struct PlanePlayerBullet : PlaneGO
{
  Point2 vel;
  
  PlanePlayerBullet();
  void updateMe();
  void redrawMe();
  void createExplosion(const Point2& enemyCircleXY);
  void updateCollisions();
};

struct PlaneFlameskull : PlaneGO
{
  Point1 glowAlpha;
  Coord1 glowFrame;
  
  PlaneFlameskull();
  void updateMe();
  void redrawMe();
  
  void updateGlow();
  void poofOut();
  void destroyStd();
};

struct PlaneCupid : PlaneGO, ActionListener
{
  AnimationBehavior 
  idleAnim,
  wingAnim,
  fireAnim,
  *bodyAnim;
  
  ArcsineFn mover;
  
  Point1 bobOffset;
  Point2 bobData;
  
  PlaneCupid();
  void load();
  void updateMe();
  void redrawMe();
  void fire();
  void destroyStd();
  void callback(ActionEvent* caller);
};

struct PlaneCupidArrow : PlaneGO, ActionListener
{
  ThumbTackBehavior stickBehavior;
  Point1 xVel;
  Point1 rotationOffset;
  Point2 rotationData;
  
  PlaneCupidArrow(Point2 origin, Point1 setVelX);
  void updateMe();
  void updateCollisions();
  Circle collisionCircle();
  void callback(ActionEvent* caller);
};

struct PlaneDemon : PlaneGO, ActionListener
{
  AnimationBehavior animation;
  LinearFn xMover;
  DeAccelFn yMover;
  
  PlaneDemon();
  void load();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct PlaneBrain : PlaneGO, ActionListener
{
  AnimationBehavior animator;
  Circle orbitCircle;
  Point1 orbitAngle;
  Point1 orbitSpeed;
  LinearFn xMover;
  LinearFn yMover;
  
  PlaneBrain();
  void load();
  void updateMe();
  void callback(ActionEvent* caller);
};

struct PlaneWisp : PlaneGO, ActionListener
{
  PizzaWarper warper;
  Point1 warpWeight;
  Point2 warpData;
  
  WispMover mover;
  TimerFn stateTimer;
  
  Coord1 currState;
  Coord1 blinkCount;  // 0-2

  PlaneWisp();
  void load();
  void updateMe();
  void redrawMe();
  
  void setVisible();
  void setInvisible();
  void setBlink();
  void fire();
  void destroyStd();

  void callback(ActionEvent* caller);
};

struct PlaneWispFlame : PlaneGO, ActionListener
{
  AnimationBehavior animator;
  LinearFn fader;
  Point2 vel;
  
  PlaneWispFlame(Point2 center);
  void load();
  void updateMe();
  void destroyStd();
  void callback(ActionEvent* caller);
};

struct PlaneRabbit : PlaneGO, ActionListener
{
  AnimationBehavior animation;
  LinearFn xMover;
  DeAccelFn yMover;

  PlaneRabbit();
  void load();
  void updateMe();
  void callback(ActionEvent* caller);
};

struct PlaneNaga : PlaneGO, ActionListener
{
  ArrayList<VisRectangular> pieces;
  Point1 tailLength;
  
  Point1 sinePercent;  // 0-1
  Point1 currAmplitude;
  Point1 currNumPeriods;
  
  DeAccelFn onscreenMover;
  TimerFn chargeWaiter;
  AccelFn chargeMover;
  Coord1 aiState;
  Point1 currSpeed;
  
  Point1 flameGen;
  ActionList flameList;
  
  enum 
  {
    MOVE_ONSCREEN,
    WAIT_TO_CHARGE,
    CHARGING
  };
  
  PlaneNaga();
  void load();
  void updateMe();
  void redrawMe();
  
  void updatePieces(Logical firstUpdate);
  void drawPieces();
  void generateFlames();
  void updateCollisions();
  void destroyStd();
  void callback(ActionEvent* caller);
};

struct PlaneNagaFireball : PlaneGO, ActionListener
{
  Point1 flameGen;
  ActionList flameList;
  LinearFn xMover;
  
  PlaneNagaFireball(Point2 center);
  void load();
  void updateMe();
  void redrawMe();
  
  void generateFlames();
  Circle collisionCircle() {return Circle(Point2(getX() + 24.0, getY()), getWidth());}
  void callback(ActionEvent* caller);
};

struct PlaneNodes : PlaneGO
{
  // set XY before load, which is the TOP node
  Point1 beamHeight;
  Point1 animWeight;
  
  static const Point1 MIN_BEAM_HEIGHT;
  
  PlaneNodes();
  void load();
  void updateMe();
  void redrawMe();
  void updateCollisions();
  void destroyStd();
  void damagedCallback(Point2 atkPoint, PlaneGO* attacker);
  Circle getBottomCircle();
  Circle touchingMyCircle(const Circle& c);
};

struct PlaneCoin : PlaneGO
{  
  AnimationBehavior animator;
  Coord1 value;
  
  PlaneCoin(Point2 center, Coord1 setValue = 0);  // 0-3
  virtual void load();
  
  virtual void updateMe();
  virtual void redrawMe();  
  
  void gotCoin();
};

struct PlaneUnlockable : PlaneGO
{
  Coord2 toppingID;
  
  Point1 glowRotation;
  Point1 bobOffset;
  Point2 bobData;
  
  Logical startedMagnet;
  Point2 startXY;
  Point1 currDistPercent;
  
  PlaneUnlockable(Coord2 setToppingID);
  void updateMe();
  void redrawMe();
  void updateFloater();
  void updateMagnet();
  void startMagnet();  // can call this after load
};

// ==================================== TimingGO ================================== //

struct TimingLevel;
struct TimingPizza;

struct TimingGO : VisRectangular, BaseGOLogic
{
  TimingLevel* level;
  TimingPizza* player;
  Coord1 type;
  
  enum TimingGOType
  {
    TTYPE_UNKNOWN,
    TTYPE_PLAYER,
    TTYPE_BOUNCER,
    TTYPE_BUILDING,
    TTYPE_CARPET,
    TTYPE_SIGN
  };
  
  TimingGO();
  virtual void load() {}
  virtual void updateMe();
  virtual void redrawMe();
  virtual Point2 platXRange() {return Point2(getX(), getX());}
  virtual void pizzaTouched() {}
};

struct TimingPizza : TimingGO
{
  Coord1 animState;
  Point1 walkStartX;
  Point1 animWeight;  
  LinearFn jumper;  // just controls the animation
  
  enum AnimType
  {
    ANIM_RUN,
    ANIM_JUMP,
    ANIM_FALL,
    ANIM_START_WIN,
    ANIM_LOOP_WIN
  };
  
  TimingPizza();
  void load();
  void updateMe();
  void redrawMe();
  void playJumpAnim(Point1 time, Logical goodJump);
  void playWinAnim();
};

struct TimingBounceObj : TimingGO, ActionListener
{
  DeAccelFn scaler;
  
  TimingBounceObj();
  void updateMe();
  void pizzaTouched();
  void callback(ActionEvent* caller);
};

struct TimingGenie : TimingGO
{
  AnimationBehavior animator;
  Point1 flameAlpha;
  Point1 flameWeight;
  Logical touched;
  
  TimingGenie();
  void updateMe();
  void redrawMe();
  void pizzaTouched();
};

// set the  manually, xy is run path top left
struct TimingBuilding : TimingGO
{
  Coord1 numMiddles;  // 128 px each, width is set during load
  
  TimingBuilding();
  void load();
  void redrawMe();
  Point2 platXRange() {return Point2(getX(), getX() + getWidth());}
};

struct TimingCarpet : TimingGO, ActionListener
{
  ActionQueue script;
  ActionList actions;
  
  Coord1 carpetState;
  Point1 animWeight;
  
  Point1 sparkleGen;
  Point1 startDipY;
  Point1 carpetDipOffset;
  
  enum
  {
    WAITING_FOR_JUMP,
    CATCHING_PIZZA,
    JUST_DANCE,
    WIN_FLYING,
    WIN_FINISH
  };
  
  TimingCarpet();
  void updateMe();
  void redrawMe();
  void generateSparkles();
  void catchPizza(Point2 targetXY, Point1 timeLeft);
  void callback(ActionEvent* caller);
};

struct CarpetSparkle : VisRectangular, Effect, ActionListener
{
  TimingGO* parent;
  
  LinearFn scaler;
  Logical scaleIn;
  
  Point2 offset;
  Point1 rotationMult;

  CarpetSparkle(TimingGO* setCarpet, Point2 startingOffset);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct TimingCoin : TimingGO, ActionListener
{
  AnimationBehavior animator;
  LinearFn fader;
  
  Coord1 value;
  Coord1 spins;

  TimingCoin();
  void load();
  void updateMe();
  void callback(ActionEvent* caller);
};

struct TimingUnlockable : TimingGO
{
  Coord2 toppingID;
  
  Point1 glowRotation;
  Point1 bobOffset;
  Point2 bobData;
  
  Logical startedMagnet;
  Point2 startXY;
  Point1 currDistPercent;
  
  TimingUnlockable(Coord2 setToppingID);
  void updateMe();
  void redrawMe();
  void updateFloater();
  void startMagnet();  // can call this after load
};

// ==================================== Misc Effects ================================== //

struct SauceSplash : PoofEffect
{
  
};

struct SauceParticles : ParticleEffect
{
  Point2 xy;
  
  SauceParticles(Point2 setXY, Point1 angle);
};

struct StarParticles : ParticleEffect
{
  ProceduralBurst* burst;
  Point2 xy;
  
  StarParticles(Point2 setXY, Point1 angle);
  StarParticles(Point2 setXY);  // burst all directions, for intro effect
};

struct FlagStars : ParticleEffect
{
  ProceduralBurst* burst;
  
  FlagStars(Point2 center, Logical big);
  void updateMe();
};

struct SnowflakeBurst : ParticleEffect
{
  ProceduralBurst* burst;  
  SnowflakeBurst(Point2 center, Logical big);  // big is for the snowman
};

struct Smokeburst : ParticleEffect
{
  ProceduralBurst* burst;  
  Smokeburst(Point2 center, Logical big);  // big for bigger enemies
};

struct CoinParticles : ParticleEffect
{
  CoinParticles(Point2 center);
};

struct SingleCoinSparkle : VisRectangular, ActionEvent, ActionListener
{
  LinearFn scaler;
  Logical scaleIn;
  Point1 rotateSpeed;
  
  SingleCoinSparkle(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct RingEffect : PoofEffect
{
  RingEffect(Point2 center);
};

struct FireSplash : ParticleEffect
{
  FireSplash(Point2 center);
};

struct GooSplash : ParticleEffect
{
  GooSplash(Point2 center);
};

struct RockSplash : ParticleEffect
{
  RockSplash(Point2 center);
};

struct GrassSplash : ParticleEffect
{
  GrassSplash(Point2 center);
};

struct ScreenShatter : ParticleEffect
{
  ScreenShatter(const VisRectangular& visRect);
};

struct LoseRedEffect : VisRectangular, Effect
{
  Point1 progress;
  CosineFn progressMover;
  
  LoseRedEffect(Point1 duration);
  void updateMe();
  void redrawMe();
};

#endif
