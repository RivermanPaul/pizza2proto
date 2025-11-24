/*
 *  game.h
 *  GLTriangles
 *
 *  Created by Riverman Media on 7/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DEF_GAME
#define DEF_GAME

#include "rmapplication.h"
#include "rminputlisteners.h"
#include "rmscene.h"
#include "rmstdinput.h"
#include "rmgui.h"

#include "globals.h"
#include "level.h"

struct PizzaGO;
struct PointerDataStd;
struct WorldGUI;
struct MainGUI;
struct PizzaScene;

// =============================== MainApp ============================ //

struct MainApp : RivermanApplication
{
  void init();
  void update();
  void redraw();
  PizzaScene* createFirstScene();
};

// =============================== PizzaExtender ============================ //

struct PizzaExtender
{
  Point1 masterScale;
  
  TimerFn hudTimeout;
  TimerFn disableTimeout;
  
  ScrollingImage
  bg0,
  bg1,
  bg2,
  bg3;
  
  PizzaExtender();
  void init();
  void update();
  void redraw();
  void initLayer(ScrollingImage& currBG, Image* img, Point2 speed);
  void drawLayer(ScrollingImage& currBG);
  void reportDrawHUD();
  void reportDisable();
};

// =============================== PizzaScene ============================ //

struct PizzaScene : Scene
{
  VisRectangular lastSceneRect;
  Logical lastSceneRectDrawn;
  
  Logical startWithMusic;  // true plays music in transIn
  Logical stopWithMusic;  // false keeps music playing, this is for share gui

  Logical releasesScreenshotEnd;  // wheel scene uses, means release right before taking next one
  Point1 darkAlpha;
  Point1 loadingAlpha;
  Coord1 transInHint;

  // for transInHint
  enum 
  {
    FROM_INTRO,
    FROM_MAP,
    FROM_SHOP,
    FROM_RESTART,
    FROM_LOSE,
    RANDOM
  };
  
  PizzaScene();
  virtual ~PizzaScene() {}
  
  virtual void drawActions();
  
  virtual void transIn();
  virtual void transOut(Scene* next);
  virtual void transInEffect();
  
  void randomTransIn();
  void randomRotIn();
  void randomRotBigIn();
  void randomFallIn();
  void randomSpinIn();
  void poofIn();

  void drawLoading();  // fine to call all the time, won't render at low alpha
  void takeScreenshot();
  void releaseTransScreenshot();  // it's fine to call this even if it wasn't set
};

// =============================== PizzaGUI ============================ //

struct PizzaDirNode : DirectionalNode<PizzaDirNode, Widget>
{
  Point2 highlightPosOffset;
  Point2 highlightSizeOffset;
  
  Box absHighlightBox;  // used if the width > 1.0, otherwise uses the offsets
  
  PizzaDirNode();
  Coord1 getStopType() {return data->isEnabled() ? NODE_SELECT : NODE_PASSTHROUGH;}
};

struct PizzaGUI : RivermanGUI, DirectionGraph<PizzaDirNode, Widget>
{
  Box highlightBox;  // the current box
  
  Point1 boxFlashData;  // < 0.5 = ON
  Point1 boxCornerGlowVal;  // 0.0 - 0.3999

  Point3 boxGlowData;
  Point3 boxScaleData;

  PizzaGUI();
  virtual ~PizzaGUI() {}
  
  virtual void setFirstNode(Widget* widget);
  virtual void updateDirHighlight();
  virtual Box getTargetBox();
  virtual void snapDirHighlighter();
  virtual void drawDirHighlight();
};


// =============================== Effects ============================ //

struct SceneScreenshotAction : ActionEvent
{
  PizzaScene* parent;
  Logical shotTaken;
  Logical screenSwap;  // wheel uses this

  SceneScreenshotAction(PizzaScene* setParent, Logical swaps);
  void updateMe();
  void redrawMe();
};

struct DoorOpenEffect : Effect, ActionListener
{
  DataList<Image*> doorImgs;
  Logical hori;
  VisRectangular leftDoor;
  VisRectangular rightDoor;
  CosineFn leftOpen;
  CosineFn rightOpen;
  
  DoorOpenEffect(Image* baseImg, Logical openHorizontal);   // 0 = regular, 1 = vert
  ~DoorOpenEffect();
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// =============================== SplashScene ============================ //

struct SplashScene : PizzaScene
{
  SplashScene();
  ~SplashScene();
  void load();
  void updateMe();
  void redraw();
  void startActive();
  
  void drawText();
  void drawTextAppleTV();
  void drawTextMac();
};

// =============================== TitleScene ============================ //

struct TitleCinema : Effect, ActionListener
{
  ActionList actions;
  ActionQueue script;
    
  VisRectangular
  riverman,
  presents,
  pizza,
  skull,
  logo;
  
  ShakeRegister shaker;
  PizzaWarper pizzaWarper;
  
  Point1 
  skullAngle,
  jawAngle,
  fadeAlpha,
  warpWeight;

  TitleCinema();
  void init();
  void updateMe();
  void redrawMe();
  void startAction(ActionEvent* ae);
  void callback(ActionEvent* caller);
};

struct TitleScene : PizzaScene
{
  ScrollingImage sky;
  TitleCinema cinema;
  
  TitleScene();
  ~TitleScene();
  void load();
  void updateMe();
  void redraw();
  void startActive();
};

// =============================== MapScene ============================ //

struct MapScene : PizzaScene
{
  WorldGUI* worldGUIIn;
  WorldGUI* worldGUIOut;

  CameraStd zoomCam;  // for zooming into the shop
  ScrollingImage sky;
  
  MapScene();
  ~MapScene();

  void load();

  void updateMe();
  void redraw();
  
  void startActive();
  void updateWorld();
  void arrowPress(Coord1 dir);  // -1 or 1
  void shopPress(Point2 shopTopLeft);
  Logical worldStable();
};

struct PizzaButton : Button
{
  Image* upImg;
  Image* downImg;
  Point2 drawOffset;
  Logical usesPlaySound;
  Logical silentOverride;  // makes the button silent
  
  PizzaButton(RivermanGUI* setParent = NULL);
  
  // calls init
  PizzaButton(RivermanGUI* setParent, Point2 position, Image* setUpImg, Image* setDownImg);
  
  // upImg and/or downImg can be NULL
  void init(Point2 position, Image* setUpImg, Image* setDownImg);
  void update();
  void redraw();
  void clickCallback();
};

struct WorldGUI;

struct LevelButton : Button, ActionListener
{
  Coord2 levelPos;  // [worldNum, levelNum]
  Coord2 levelDef;  // [modeType, levelType]
  Logical isBoss;  // for the dwarp
  Coord1 numStars;
  
  Image* regularImg;
  AnimationBehavior newAnimator;
  Point1 newBobVal;
  Point2 newBobData;
  
  Point1 lockSwellScale;
  Point2 lockSwellData;
  WorldGUI* parentWorldGUI;

  Point1 pressScale;
  
  ActionQueue scareScript;
  PizzaWarper warper;
  Point1 weight;  // warpWeight for the skull
  
  LevelButton();
  void init(Point2 position, Coord2 levelPosition, 
            Image* regImg, WorldGUI* setParentWorldGUI);  // CALL BEFORE UPDATE/REDRAW
  void update();
  void redraw();
  void setStars(Coord1 setNumStars); // -x = locked, 0 = new, 1, 2, 3 stars
  void resetScare();
  Box collisionBox();
  void clickCallback();
  Logical readyToUnlock();
  void callback(ActionEvent* caller);
};

struct ShopButton : Button
{
  PizzaWarper warper;
  Point1 weight;
  Point2 bounceData;
  
  ShopButton(RivermanGUI* setParent);
  void update();
  void redraw();
  Box collisionBox();
  void clickCallback();
};

struct SoundCheckbox : Button
{
  SoundCheckbox(RivermanGUI* setParent);
  void update();
  void redraw();
  void updateImage();
  void clickCallback();
};

struct MainGUI : PizzaGUI, SystemMoreGamesListener
{
  MapScene* scene;
  
  PizzaButton leftArrow;
  PizzaButton rightArrow;
  
  PizzaButton liteButton;
  PizzaButton unlockBtn;
  
  ShopButton shopButton;
  ArrayList<PizzaButton> socialButtons;

  MoreGamesButtonStd moregamesBtn;

  Point1 starGlowAlpha;
  Point2 starGlowData;
  
  Logical drawBubble;
  Point1 bubbleBobY;
  Point2 bubbleBobData;
  
  TimerFn gcMessageTimer;  // if active, don't display the message again
  Logical gcRecentlyPressed;  // a game center button was pressed this session

  Logical yellowStarNum;
  
  Coord1 lastDirPressed;  // -1, 0, 1
  
  enum
  {
    SOCIAL_ACHIEVEMENT,
    SOCIAL_LEADERBOARD,
    SOCIAL_SHARE,
    SOCIAL_CREDITS
  };
  
  MainGUI();
  
  void update();
  void redraw();
  
  void drawStars();
  void drawScore();
  
  void checkForGCFail();
  void syncMasterAlpha();
  void fadeOut();
  void fadeIn();

  Logical canGoLeft();
  Logical canGoRight();
  void failMsgStd(const String2& msg, Point1 yCenterOffset);

  // from SystemMoreGamesListener
  void opened_interface(Coord1 value);
  void closed_interface(Coord1 value);

  void resetNodeGraph();  // called every time the world changes

  void click(Widget* caller);
};

struct WorldGUI : RivermanGUI
{
  MapScene* scene;
  CameraStd guiCam;
  Coord1 worldID;
  
  VisRectangular background;
  
  DataList<Image*> levelIcons;  // OWNED
  ArrayList<LevelButton> levelButtons;

  WorldGUI(MapScene* setScene, Coord1 setWorldID, Coord1 startSide);  // -1, 0, 1
  ~WorldGUI();
  
  void update();
  void redraw();
  
  void transIn(Coord1 startFromSide);  // -1 or 1
  void transOut(Coord1 goToSide);  // -1 or 1
  void resolveGUI();
  
  void click(Widget* caller);
};

struct WorldBannerEffect : VisRectangular, Effect, ActionListener
{
  Image* upImg;
  Image* downImg;
  
  PizzaWarper warper;
  Point1 weight;
  
  SineFn alphaChange;
  SineFn scaleChange;
  
  Coord1 worldNum;
  Coord1 state;
  
  WorldBannerEffect(Coord1 setWorldNum, Logical startIn);
  ~WorldBannerEffect();
  void updateMe();
  void redrawMe();
  Box collisionBox();
  void callback(ActionEvent* caller);
};

// set Pizza::currWorldPair BEFORE the ctor
struct InstructionsGUI : PizzaGUI
{
  Image* comicImg;  // OWNS

  CameraStd instructionsCam;
  
  PizzaButton
  play,
  close;
  
  VisibleText 
  levelText,
  scoreText;
  
  Point2 
  startCenter,
  startScale;
  
  const static Point1 TRANS_TIME;
  
  InstructionsGUI(Point2 center);
  ~InstructionsGUI();  // deletes background and comic
  void update();
  void redraw();
  void syncDrawers();
  void click(Widget* caller);
};

struct CreditsGUI : PizzaGUI
{
  Point1 transPercent;
  VisRectangular creditsDrawer;

  CreditsGUI();
  ~CreditsGUI();  // unloads the credits image and black bg
  void update();
  void redraw();
};

// =============================== ShopScene ============================ //

struct ShopScene : PizzaScene
{
  ScrollingImage sky;
  
  ShopScene();
  ~ShopScene();
  
  void load();
  void updateMe();
  void redraw();
  
  void startActive();
  // void transInEffect();
};

struct ShopGUI : PizzaGUI
{
  VisRectangular pizza;
  PizzaFace pizzaDrawer;
  
  PizzaButton done;
  PizzaButton share;
  ArrayList<Clicker> tabClickers;  // 5
  ArrayList<Clicker> itemClickers;  // 16

  VisibleText cashDrawer;
  VisibleText itemDrawer;
  
  VisRectangular selector;
  SineFn selScaler;
  SineFn selFader;
  
  Coord1 currTab;
  Coord1 currItem;
  
  Logical postedBuy; // only generate achievement once per restart of scene
  Logical postedRetro;  // only generate achievement once per restart of scene

  Coord1 clickCount;

  ShopGUI();
  void update();
  void redraw();
  
  void setPlayerCashText();
  void setItemText();
  
  void setTab(Coord1 tabIndex);
  void clickItem(Coord1 tabIndex, Coord1 itemIndex);
  void selectTopping(Coord1 tabIndex, Coord1 itemIndex);
  void itemBought(Coord2 itemDef);
  
  void click(Widget* caller);
};

struct ShopConfirmGUI : PizzaGUI
{
  ShopGUI* parent;
  PizzaButton yesButton;
  PizzaButton noButton;
  
  VisRectangular icon;
  Coord2 itemData;  // category, item
  Logical clickedYes;
  
  VisibleText textDrawer;
  
  ShopConfirmGUI(ShopGUI* setParent, Coord2 setItemData, const Clicker& item);
  void update();
  void redraw();
  void startResolve();
  void click(Widget* caller);  
};

// =============================== ShareScene ============================ //

struct ShareScene : PizzaScene
{
  ShareScene();
  ~ShareScene();
  void load();
  void updateMe();
  void redraw();
  void startActive();
};

struct ShareGUI : PizzaGUI
{
  ArrayList<VisRectangular> drawers;
  ArrayList<PizzaButton> bgButtons;
  
  PizzaButton 
  share,
  close;
  
  VisRectangular pizza;
  PizzaFace pizzaDrawer;
  
  Coord1 drawingBGID;  // helps with unloading
  Logical tallMode;
  Coord1 shareState;
  Point1 whiteAlpha;
  Bitmap* screenshot;
  
  Logical shouldPostAchNow;  // true will post on next update
  Logical postedAchievement;  // only post once per scene instance
  
  enum
  {
    BACKGROUND,
    BG_ICON_BASE,
    SHARE_BASE,
    SELECTOR,
    FRONT_BONES,
    BACK_BONES,
    RIVERMAN,
    PIZZA_LOGO,
    
    NUM_STATIC_DRAWERS
  };
  
  // these are substates of PASSIVE
  enum ShareState
  {
    NOTHING,
    CHANGE_BG,
    SHARING
  };
  
  ShareGUI();
  ~ShareGUI();  // deletes background image and screenshot
  void update();
  void redraw();
  
  void syncColors();
  Point2 calcSelectorXY(Coord1 bgID);
  void changeBackground(Coord1 bgID);
  void setBackground(Coord1 bgID);
  void beginShare(ShareState shareType);
  
  void click(Widget* caller);
  void callback(ActionEvent* caller);
};

// =============================== PlayScene ============================ //

struct PlayScene : PizzaScene
{
  PizzaLevelInterface* level;
    
  PlayScene();
  ~PlayScene();
  
  void load();
  void updateMe();
  void redraw();

  void startActive();
  // void transInEffect();
};

struct GameplayGUI : RivermanGUI
{
  PizzaButton pause;
  
  GameplayGUI(Logical enabled);
  void update();
  void redraw();
  void disableGUI();
  void click(Widget* caller);
};

struct PauseMenu : PizzaGUI
{
  PizzaButton 
  play,
  restart,
  quit,
  instructions,
  calibrate;
  
  SoundCheckbox sounds;
  
  VisRectangular 
  topBase,
  midBase,
  botBase,
  calibrateArrow;
  
  Point1 darkAlpha;
  Point1 accelChaser;
  
  Coord1 clickCount;
  
  PauseMenu();
  
  void update();
  void redraw();
  
  void syncBases();
  
  void transInStd();
  void transOutStd();
  void transOutInstructions();
  void transInInstructions();
  
  void resume(RivermanGUI* caller);
  void click(Widget* caller);
};

struct PauseInstructions : RivermanGUI
{
  RivermanGUI* parent;
  Image* comicImg;  // OWNS
  
  VisibleText 
  levelText,
  scoreText;
  
  PauseInstructions(RivermanGUI* setParent);
  ~PauseInstructions();
  void update();
  void redraw();
  void startResolve();
};

struct LoseGUI : PizzaGUI
{
  PizzaButton 
  quitBtn,
  instructionsBtn,
  retryBtn;
  
  static const Point1 botOffset;
  
  LoseGUI();
  void update();
  void redraw();
  void resume(RivermanGUI* caller);
  void click(Widget* caller);
};

// =============================== WheelScene ============================ //

struct WheelScene;

struct WheelOfPizza : ActionListener
{
  WheelScene* scene;
  VisRectangular wheel;
  VisRectangular slice;
  ActionQueue endScript;
  
  Logical touched;
  Point1 currSpeed;
  LinearFn speedMover;
  
  ArrayList<Logical> lettersOn;
  ArrayList<Logical> bulbsOn;
  
  ActionQueue lightScript;
  Coord1 lightState;
  
  Coord1 letterIndex;
  Coord1 bulbIndex;
  Coord1 wonItemIndex;  // starts -1 until an item is won
  
  ArrayList<Coord1> wheelItemIDs;
  ArrayList<VisRectangular> wheelObjects;
  ArrayList<Coord1> workingList;  // this is to help with the randomization
  
  enum
  {
    SPINNING,
    WON_SMALL,
    WON_BIG,
    MISS
  };
  
  enum WheelItemID
  {
    // these match SlopeCoin::types
    WHEEL_PENNY,
    WHEEL_NICKEL,
    WHEEL_DIME,
    WHEEL_QUARTER,
    
    WHEEL_BIG_CASH,
    WHEEL_OUTFIT,
    WHEEL_STAR,
    
    WHEEL_NOTHING
  };
  
  static const Coord1 NUM_DIVISIONS = 10;
  static const Coord1 NUM_LIGHTS = 40;
  static const Point2 LETTER_POSITIONS[];
  static const Point2 WHEEL_CENTER;
  
  WheelOfPizza(WheelScene* setScene);
  void init();  // sets graphics
  void update();
  void redraw();
  
  void updateItems();
  void drawItems();
  
  void playerTouched();
  void updateLights();
  Coord1 calcSegment();
  Coord1 calcLight();
  
  void setWonSmall();
  void setWonBig();
  void setMiss();  

  void wheelStopped();
  void insertItem(WheelItemID itemID);  // don't call this until images are loaded
  void lightCallback();
  void clearLights();
  void callback(ActionEvent* caller);
};

struct WheelScene : PizzaScene
{
  WheelOfPizza wheel;
  CameraStd wheelCam;
  Logical wheelActive;
  
  WheelScene();
  ~WheelScene();
  
  void load();
  void updateMe();
  void redraw();
  
  void startActive();
  void transInEffect();
  void wheelDone();  // wheel calls this
};

struct WinGUI : PizzaGUI
{
  VisibleText
  levelNumText,
  cashText,
  scoreText,
  messageTextDrawer;
  
  VisRectangular 
  victoryNormal,
  victoryBright,
  cashRect,
  messageRect;
  
  ArrayList<VisibleText> 
  earnedTextSet,
  goalTextSet;
  
  ArrayList<VisRectangular> 
  goalCheckSet,
  starSet;
  
  Coord1 currMsg;
  ArrayList<String2> messageList;
  
  PizzaButton
  replayBtn,
  continueBtn;
  
  static const Point1 
  topOffset,
  leftOffset,
  rightOffset,
  botOffset;
  
  Point1
  darkAlpha,
  bannerAlpha;  
  
  Byte4 starBits;
  Logical levelWasBeaten;
  Coord1 newStars;
  
  Logical
  newHiScore,
  newTopWorld,
  newIngredient,
  gameBeaten,  // doesn't have to be for the first time
  gameBeatenLite,  // doesn't have to be for the first time
  newlyBeatenFull,   // first time
  newlyBeatenLite;   // first time
  
  WinGUI();
  void update();
  void redraw();
  
  void levelWon();  // call this at start of ctor
  void transIn();
  void syncPositions();
  void setStarText(Coord1 goalIndex);  // 0-2
  Logical starWasEarned(Coord1 starIndex);  // 0-2, call after levelWon
  void enqueueMessage(const String2& msg);
  void click(Widget* caller);
};

struct WinSpark : VisRectangular, Effect, ActionListener
{
  LinearFn scaler;
  LinearFn fader;
  Point1 speedMult;
  
  static const Point1 SPEED;
  
  WinSpark(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct WinStarSparkle : VisRectangular, Effect, ActionListener
{
  LinearFn scaler;
  Point1 rotationMult;
  Logical scaleIn;
  Point2 offset;
  
  WinStarSparkle(Point2 center);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

// =============================== UpgradeScene ============================ //

struct UpgradeScene : PizzaScene
{
  Logical fromMap;
  Coord1 nextSlide;
  
  ActionList slideList;
  TimerFn createTimer;
  
  UpgradeScene(Logical setFromMap);
  ~UpgradeScene();
  
  void load();
  void updateMe();
  void redraw();
  void startActive();
  void callback(ActionEvent* caller);
};

struct LiteUpgradeGUI : RivermanGUI
{
  PizzaButton 
  noThanks,
  buyNow;
  
  Logical fromMap;
  Point1 glowAlpha;
  Point2 glowData;
  
  LiteUpgradeGUI(Logical setFromMap);
  void update();
  void redraw();
  void click(Widget* caller);
};

#endif
