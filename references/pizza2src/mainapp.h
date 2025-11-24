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
//#include "rmscene.h"

#include "rmgui.h"
#include "examples.h"
#include "level.h"
#include "globals.h"
#include "rmgrid.h"

struct PizzaFace;

struct MainApp : RivermanApplication
{
  void init();
  void update();
  void redraw();
};

struct PizzaScene : Scene
{
  PizzaScene() : Scene() {}
  virtual ~PizzaScene() {}

  virtual void loadResources();
  virtual void initResources();
  virtual void loadAndInitNow();
  virtual void loadMyRes() {}
  virtual void initMyRes() {}
  virtual void updateLoading();
  virtual void drawLoading();
  virtual void loadingGUIDone() {}

  virtual void transIn();
  virtual void transOut(Scene* next);
};

struct PizzaButton : Button
{
  Image* upImg;
  Image* downImg;
  Image* disabledImg;

  Sound* clickSound;

  Point2 drawOffset;  // from collision

  PizzaButton();

  // either can be null, down is used for hover
  // by default, matches the size of a non-null image draw size
  PizzaButton(Image* setUpImg, Image* setDownImg, Image* setDisabledImg = NULL);

  void update();
  void redraw();
  void clickCallback();
  ColorP4 textColor();
};

// =============================== PizzaGUI ============================ //

struct PizzaDirNode : DirectionalNode<PizzaDirNode, Widget>
{
  Point2 highlightPosOffset;
  Point2 highlightSizeOffset;
  
  Box absHighlightBox;  // used if the width > 1.0, otherwise uses the offsets
  
  PizzaDirNode();
  Coord1 getStopType() {return NODE_SELECT;}
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

  void transInStd();
  void transOutStd();

  virtual void setFirstNode(Widget* widget);
  virtual void updateDirHighlight();
  virtual Box getTargetBox();
  virtual void snapDirHighlighter();
  virtual void drawDirHighlight();
  
  // overrides
  void update();
  void redraw();
  void addWidget(Widget* newWidget);
};

struct IAPGUI : PizzaGUI
{
  Point1 loadingRotation;
  Point1 elapsed;
  
  IAPGUI();
  
  void update();
  void redraw();
  
  // called whether the purchase was successful or not
  void interactionFinished();
  void done();
  
  void click(Widget* caller) {}
};

struct TitleScene : PizzaScene
{
  TitleScene();
  virtual ~TitleScene();
  
  // virtuals
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  virtual void updateMe();
  virtual void redraw();
};

struct CinemaTestScene : PizzaScene, SpineEventListener
{
  ActionQueue animationScript;
  VisRectangular mainSpineVR;
  SpineAnimator mainSpineAnimator;
  
  TextTyperEffect typer;
  ArrayList<String2> relinedDialogueStrs;
  Coord1 currDialogueIndex;
  
  CinemaTestScene();
  virtual ~CinemaTestScene();
  
  virtual void enqueueDialogue(const Char* animName, Coord1 strIndex);
  
  // overrides
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  virtual void updateMe();
  virtual void redraw();
  virtual void spine_event_callback(spAnimationState* state, spEventType eventType,
                                    spTrackEntry* entry, spEvent* event);
};

struct RotationTestScene : PizzaScene
{
  VisRectangular skeleton;
  SpineAnimator animator;
  
  VisRectangular saberVR;
  
  RotationTestScene();
  virtual ~RotationTestScene();

  // virtuals
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  virtual void updateMe();
  virtual void redraw();
};

struct PlayScene : PizzaScene
{
  Coord1 levelToPlayIndex;
  
  PizzaLevelInterface* level;
  Logical fullRestart;
  
  PlayScene();
  virtual ~PlayScene();
  
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  void updateMe();
  void redraw();

  void startActive();
  
  void updateDebugKeystrokes();
};

/*
struct PizzeriaScene : PizzaScene
{
  Coord1 levelToPlayIndex;

  PizzaLevelInterface* level;
  
  PizzeriaScene();
  virtual ~PizzeriaScene();
  
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  void updateMe();
  void redraw();
  
  void startActive();
  void updateDebugKeystrokes();
};
 */

struct MapTile
{
  Coord2 coords;
  DataList<Coord1> imgIndices;
  
  MapTile();
  
  Coord1 levelNodeBits();  // returns the bits or -1
  Logical isStandableNode();  // you can stand on it
  Logical isPathableNode();  // you can stand on it or walk through it
  Logical canWalkInDir(Coord1 dir8);
  Logical isVisitableType() const;
  Coord1 dirToPassthroughIndex(Coord1 dir8);
  
  Logical levelIsAssigned();
  Coord1 getLevelIndex() const;
  LevelData& getLevelData() const;
};

// this is only meant for level tiles
Logical operator<(const MapTile& tile1, const MapTile& tile2);
Logical operator>(const MapTile& tile1, const MapTile& tile2);

struct PizzaMapNode : DirectionalNode<PizzaMapNode, MapTile>
{
  PizzaMapNode() {}
};

struct BackgroundMap : Grid<MapTile>
{
  Point3 waveData;  // pingpongs to animate wave
  Coord1 finalWaveOffset;
  
  BackgroundMap();
  void update();
  void redraw();
  void load(const Char* filename);
};

struct BatchedSpineDraw
{
  SpineAnimator* animPtr;
  Coord1 tileIndex;
  Point2 drawXY;
  
  BatchedSpineDraw();
  BatchedSpineDraw(Coord1 setTileIndex, Point2 setDrawXY, SpineAnimator* setPtr);
  void redraw();
};

// these are for the sort
Logical operator<(const BatchedSpineDraw& bsd1, const BatchedSpineDraw& bsd2);
Logical operator>(const BatchedSpineDraw& bsd1, const BatchedSpineDraw& bsd2);

// hammer bros: after beating a level, 50% chance to toggle hidden
struct MapCharacter : VisRectangular
{
  SpineAnimator animator;
  
  Coord1
  currNode,
  targetNode;
  
  ActionQueue movementScript;
  
  MapCharacter();
  virtual ~MapCharacter() {}
  
  // let the map init it
  virtual void update();
  virtual void redraw();
  
  virtual Logical isHidden() {return currNode == -1 || targetNode == -1;}
  virtual void hideMe();
  virtual void unhideMe();
  virtual void syncSpine();
};

struct MapPizza : MapCharacter
{
  PizzaFace* face; // it's a pointer for the linker's sake
  
  MapPizza();
  virtual ~MapPizza();
  virtual void update();
  virtual void redraw();
};

// The levels and the save file don't know where anything is on the map.
// It's up to the map to find them. This is so that everything can move
//   around on the map without changing any data in the save file or code.
struct PizzaMap : DirectionGraph<PizzaMapNode, MapTile>, Grid<MapTile>
{
  CameraStd* cameraFromScene;
 
  ActionQueue script;
  ActionList actions;

  MapPizza pizza;
  ArrayList<MapCharacter> hammerBrosList;
  
  SpineAnimator
  treeDrawer,
  flowersDrawer,
  deadTreeDrawer,
  palmDrawer,
  fernsDrawer,
  volcanoDrawer,
  candleDrawer,  
  bananaDrawer,
  ledDrawer,
  serverDrawer,
  polypsDrawer,
  palaceDrawer,
  bridgeHDrawer,
  bridgeVDrawer,
  bossDrawer,
  wave1Drawer,
  wave2Drawer,
  shopDrawer,
  hammerBrosDrawer;
  
  ArrayList<BatchedSpineDraw> batchedSpines;
  
  // these are for the camera controls
  Point2 touchStartXY;
  Logical dragStarted;
  
  PizzaMap();
  
  void update();
  void redraw();
  
  void initMapSpineAnimator(SpineAnimator& animator, Coord1 spineResIndex,
                            const Char* animName);
  void drawSpineAnimator(Coord1 tileIndex, Point2 topLeft, SpineAnimator* animator);
  
  void debugPrintSublevels();
  void debugPrintIndices();
  void debugPrintLevelUnlockValues();

  void setCharacterNodeCenter(MapCharacter* character, Coord1 levelIndex);

  void updateControls();
  void updateCamera();
  
  void addLevelNode(Coord2 parentCoords, const DataList<Coord2>& connectionCoords);
  
  void addStandableNode(Coord2 coords);
  
  void checkDirButtonMovement();
  void checkCursorMovement();
  
  void tryMoveTowardCursor();
  void tryMoveTowardLocation(Point2 xy);
  Coord1 getClosestNode(Point2 mapXY);
  
  void moveCharacterTowardTargetNode(MapCharacter* character, const DataList<PizzaMapNode*>& path);

  // does not include the startNode in the path.  DOES include targetNode
  Logical shortestPathTo(PizzaMapNode* startNode, PizzaMapNode* targetNode, DataList<PizzaMapNode*>& nodes);

  // finds all nodes at this distance or less, may return start node
  void findNodesAtDistance(PizzaMapNode* startNode, Point1 maxDistance, DataList<PizzaMapNode*>& nodes);

  PizzaMapNode* findClosestReachableNode(PizzaMapNode* startNode, Point2 targetXY);

  void randomizeHammerBros();

  Point2 getNodeCenter(PizzaMapNode* node);
  
  void getLevelAbbreviation(Coord2 gxy, String2& result);
  
  // [VisitSquareType, levelIndex]
  Coord2 getVisitSquareType(Coord2 gxy);
  
  Coord1 getVisitBG(Coord2 gxy);

  void tryVisitSquare(Coord2 gxy);
  
  void destroyLockOrGateAndSave(Coord2 gxy);
  
  // [VisitSquareType, index], returns nodeIndex
  Coord1 findIndexNodeForLevel(Coord1 levelIndex);
  
  void load(const Char* filename);
};

struct MapScene : PizzaScene
{
  BackgroundMap* bgMap;
  PizzaMap* map;
  
  CameraStd sceneCam;

  // set to true any time a regular level or toad house was beaten
  //   to randomize the hammer bros
  Logical anyLevelJustBeaten;
  
  MapScene();
  virtual ~MapScene();
  
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  void updateMe();
  void redraw();
  
  void startActive();
  
  void padTilesets();  // this only needs to be called if the tileset changes
};

struct ResTestScene : PizzaScene
{
  Image* testImg;
  
  ResTestScene();
  ~ResTestScene();
  
  virtual void load();
  virtual void loadMyRes();
  virtual void initMyRes();
  void updateMe();
  void redraw();
  
  void startActive();
};

// ========================= ShopGUI =========================== //

// this is just the equip button currently
struct ShopGUI : PizzaGUI
{
  PizzeriaLevel* level;
  
  PizzaButton equipBtn;
  
  ShopGUI(PizzeriaLevel* setLevel, Logical startOnscreen);
  void update();
  void redraw();
  void click(Widget* caller);
};

// ========================= EquipGUI =========================== //

struct PageArrowClicker : Clicker
{
  Box collisionBox() {return boxFromC();}
};

struct EquipGUI : PizzaGUI
{
  PizzeriaLevel* level;  // level can be NULL

  ArrayList<PizzaButton> tabButtons;
  Coord1 tabSelected;
  
  VisibleText categoryTxt;
  
  ArrayList<Clicker> itemClickers;
  DataList<IngredientData*> currIngredientSet;
  
  PageArrowClicker pageUpArrow;
  PageArrowClicker pageDownArrow;
  Coord1 itemPage;
  
  Coord3 selectorPositions;  // -1 means invalid or selector is not on this page
  Coord1 nextSelector;  // usually 0, 0-1 for seasonings, 0-2 for toppings
  Point3 selectorAddAlphaData;
  
  PizzaButton doneBtn;

  CameraStd camera;
  
  VisRectangular pizzaVR;
  PizzaFace* face;
  
  static const Coord1
  ITEM_COLS = 3,
  ITEM_ROWS = 4;
  
  EquipGUI(PizzeriaLevel* setLevel);  // level can be NULL
  virtual ~EquipGUI();
  
  virtual void update();
  virtual void redraw();
  
  virtual void drawSelectors();
  
  virtual void transIn();
  virtual void transOut();
  virtual void startActive();
  
  void changeTab(Coord1 newTab);
  void syncIngredientButtons(Coord1 newTab);
  void changePage(Coord1 newPage);
  void syncSelectorPositions(Coord1 tab);
  Coord1 maxItemsPerPage();
  Coord1 pagesInCurrentTab();  // call this after setting currIngredientSet
  void clickItem(Coord1 buttonIndex);
  
  virtual void click(Widget* caller);
};

// ========================= AlgoDraw Tests =========================== //

// algorithmic drawing test functions
void init_example();
void draw_example();
 
#endif
