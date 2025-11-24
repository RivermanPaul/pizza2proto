//
//  globals.cpp
//  PizzaVsSkeletons
//
//  Created by Paul Stevens on 6/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "globals.h"
#include "resourceman.h"

using std::cout;
using std::endl;

struct PizzaController;

Point1 PLAY_X = 0.0;
Point1 PLAY_Y = 0.0;
Point1 PLAY_W = 0.0;
Point1 PLAY_H = 0.0;
Point1 PLAY_CX = 0.0;
Point1 PLAY_CY = 0.0;
Point2 PLAY_XY(0.0, 0.0);
Point2 PLAY_WH(0.0, 0.0);
Point2 PLAY_CENTER(0.0, 0.0);
Box PLAY_BOX(0.0, 0.0, 0.0, 0.0);

String1 imgSuffix(".png");
String1 langSuffix("_en");

ArrayList<ColorP4> MAP_SKY_COLORS;
Point2 CHUNK_ENDS[NUM_CHUNK_TYPES];
ArrayList<Point1> CUPID_FIRE_TIMES(10, true);

ArrayList<String1> modeStrs;
ArrayList<String1> worldStrs;

ArrayList<ArrayList<LevelData> > WORLD_LEVELS(NUM_WORLD_TYPES);
ArrayList<ArrayList<Coord1> > STARS_TO_UNLOCK(NUM_WORLD_TYPES);
 
String2 MASTER_LSTRING;
ArrayList<String2> LSTRINGS;
String1 LANGUAGE_CODE("en");
LanguageID LANGUAGE_ID = ENGLISH;

ArrayList<ArrayList<PizzaItem> > TOPPING_DEFS(NUM_PIZZA_LAYERS, true);

// ========================= LevelData ========================= //

LevelData::LevelData() :
  typeData(0, 0),
  goalTypes(GOAL_TIME, GOAL_TIME),
  goalValues(0, 0),
  lockBroken(false),
  instructionsBase(),

  starsEarned(0),
  bestScore(0)
{
  
}

LevelData::LevelData(Coord1 modeID, Coord1 levelID, Coord1 setGoalValA, Coord1 setGoalValB) :
  typeData(modeID, levelID),
  goalTypes(GOAL_TIME, GOAL_TIME),
  goalValues(setGoalValA, setGoalValB),
  lockBroken(false),
  instructionsBase(),

  starsEarned(0),
  bestScore(0)
{
  setModeDefaults();
}

void LevelData::setModeDefaults()
{  
  instructionsBase = "instructions_!@&";
  instructionsBase.replace('!', modeStrs[typeData.x].as_cstr());
  Coord1 insNum = 0;
  
  switch (typeData.x)
  {
    default: cout << "LevelData::setDefaultGoalTypes(): Invalid level type: " << typeData.x << endl;
    case LEVEL_GLADIATOR: 
      goalTypes.set(GOAL_TIME, GOAL_UNHARMED);
      insNum = typeData.y;
      if (typeData.y == 5 || typeData.y == 8) insNum = 1;
      break;
    case LEVEL_KARATE: 
      goalTypes.set(GOAL_BOARDS, GOAL_BOARDS);
      if (typeData.y == 10) insNum = typeData.y;
      break;
    case LEVEL_LAKE: 
      goalTypes.set(GOAL_TIME, GOAL_UNHARMED);
      if (typeData.y == 8 || typeData.y == 9) insNum = typeData.y;
      break;
    case LEVEL_SLOPES: 
      goalTypes.set(GOAL_TIME, GOAL_COINS);       
      if (typeData.y == 4 || typeData.y == 6) insNum = typeData.y;
      break;
    case LEVEL_WRECKING: 
      goalTypes.set(GOAL_ENEMIES, GOAL_UNHARMED); 
      if (typeData.y == 10) insNum = typeData.y;
      break;
    case LEVEL_BALANCE: 
      goalTypes.set(GOAL_COINS, GOAL_UNHARMED); 
      break;
    case LEVEL_BOUNCE: 
      goalTypes.set(GOAL_TIME, GOAL_UNHARMED); 
      if (typeData.y == 9) insNum = typeData.y;
      break;
    case LEVEL_PUPPY: 
      goalTypes.set(GOAL_TIME, GOAL_UNHARMED); 
      break;
    case LEVEL_ANGRY: 
      goalTypes.set(GOAL_TIME, GOAL_OBJECTS); 
      break;
    case LEVEL_SUMO: 
      goalTypes.set(GOAL_TIME, GOAL_WHEEL); 
      if (typeData.y == 9 || typeData.y == 10 || typeData.y == 11) insNum = typeData.y;
      break;
    case LEVEL_PLANE:
      goalTypes.set(GOAL_ENEMIES, GOAL_UNHARMED);
      insNum = 1;
      if (typeData.y <= 2) insNum = typeData.y;
      break;
    case LEVEL_TIMING:
      goalTypes.set(GOAL_METERS, GOAL_METERS);
      // NOTE: the standard goal equation is [numModules * 10, numModules * 20, numModules * 30]
      break;
  }
  
  instructionsBase.int_replace('@', insNum);
}

void LevelData::setStarsEarned(Coord1 stars)
{
  starsEarned = stars;
  if (starsEarned > 0) lockBroken = true;
}

ArrayList<String2>
goalEarnedStrs(LevelData::NUM_GOAL_TYPES, true),
goalRequiredStrs(LevelData::NUM_GOAL_TYPES, true);

// ========================= PizzaController ========================= //

PizzaController::PizzaController() :
  PointerDataStd(),
  AccelDataStd(),

  gamepad(),
  keyboard(),

  calib(0.0),
  calibVert(0.0),

  pressTaken(false),
  usedDirsLast(false)
{
  singlePointer = true;
  
  // apple tv doesn't have a way to use pointers
  if (RMSystem->get_OS_family() == APPLETV_FAMILY ||
      RMSystem->get_OS_family() == MAC_FAMILY)
  {
    usedDirsLast = true;
  }
}

void PizzaController::update()
{
  if (getDirection(JUST_PRESSED) != ORIGIN)
  {
    // cout << "dir " << getDirection(JUST_PRESSED) << endl;
  }
  
  updateTypeUsedLast();

  PointerDataStd::update();
  AccelDataStd::update();
  gamepad.update();  
  keyboard.update();
}

Point2 PizzaController::getXY()
{
  return cursorToScreen(PointerDataStd::getXY());
}

Point2 PizzaController::cursorToScreen(Point2 cursorXY)
{
  CameraStd cursorCam = Pizza::platformCam;
  
  // for ios devices where the mouse coords don't match the pixel size of the screen
  cursorCam.zoom /= RMSystem->get_screen_scale();
  
  cursorXY = Pizza::adjustCam.transformInv(cursorCam.transformInv(cursorXY));
  //  cout << "out: " << cursorXY << endl;
  
  return cursorXY;
}

Logical PizzaController::isOutsidePause()
{
  Box pauseBox(Pizza::platScreenBox.corner(1) - Point2(48.0, 0.0), Point2(48.0, 48.0));
  return pauseBox.collision(getXY()) == false;  
}

Logical PizzaController::clickedOutsidePause()
{
  return (clickStatus(JUST_PRESSED) == true && isOutsidePause()) ||
      getConfirmButtonStatus(JUST_PRESSED) == true;
}

Logical PizzaController::getAnyConfirmStatus(Coord1 status)
{
  return getConfirmButtonStatus(status) == true ||
      PointerDataStd::clickStatus(JUST_PRESSED) == true;
}

Logical PizzaController::getConfirmButtonStatus(Coord1 status)
{
  return gamepad.get_button_status(status, kGP_Button_Rightface_Down, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Down, 1) == true ||
      keyboard.get_status(status, kVK_Space) == true ||
      keyboard.get_status(status, kVK_Return) == true ||
      keyboard.get_status(status, kVK_ANSI_KeypadEnter) == true;
}

Logical PizzaController::getCancelButtonStatus(Coord1 status)
{
  return gamepad.get_button_status(status, kGP_Button_Rightface_Right, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Right, 1) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Left, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Left, 1) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Up, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Rightface_Up, 1) == true ||
      keyboard.get_status(status, kVK_Escape) == true ||
      keyboard.get_status(status, kVK_ANSI_Q) == true;

}

void PizzaController::updateTypeUsedLast()
{
  if (getConfirmButtonStatus(PRESSED) == true ||
      getDirection(PRESSED) != ORIGIN)
  {
    usedDirsLast = true;
  }
  else if (clickStatus(PRESSED) == true)
  {
    usedDirsLast = false;
  }
}

Logical PizzaController::getDirectionStatus(Coord1 dir8, Coord1 status, Coord1 controllerID)
{
  if (getDirectionalButtonStatus(dir8, status) == true)
  {
    return true;
  }
  
  Point1 tolerance = 0.5;
  Logical wasNotGT = false;
  Logical gtNow = false;
  Point2 prevXY = gamepad.get_prev_joystick(kGP_Joystick_L, controllerID);
  Point2 currXY = gamepad.get_joystick(kGP_Joystick_L, controllerID);
  
  switch (dir8)
  {
    default:
    case EAST:
      wasNotGT = prevXY.x < tolerance;
      gtNow = currXY.x >= tolerance;
      break;
    case SOUTH:
      wasNotGT = prevXY.y < tolerance;
      gtNow = currXY.y >= tolerance;
      break;
    case WEST:
      wasNotGT = prevXY.x > -tolerance;
      gtNow = currXY.x <= -tolerance;
      break;
    case NORTH:
      wasNotGT = prevXY.y > -tolerance;
      gtNow = currXY.y <= -tolerance;
      break;
  }
  
  switch (status)
  {
    default:
    case PRESSED: return gtNow == true; break;
    case JUST_PRESSED: return wasNotGT == true && gtNow == true; break;
    case WAS_PRESSED: return wasNotGT == false; break;
    case JUST_RELEASED: return wasNotGT == true && gtNow == false; break;
  }
}


Logical PizzaController::getDirectionalButtonStatus(Coord1 dir8, Coord1 status)
{
  switch (dir8)
  {
    default:
    case EAST:
      return gamepad.get_button_status(status, kGP_Button_Leftface_Right, 0) == true ||
          gamepad.get_button_status(status, kGP_Button_Leftface_Right, 1) ||
          keyboard.get_status(status, kVK_ANSI_D) == true ||
          keyboard.get_status(status, kVK_RightArrow) == true;
      break;
    case SOUTH:
      return gamepad.get_button_status(status, kGP_Button_Leftface_Down, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Leftface_Down, 1) ||
          keyboard.get_status(status, kVK_ANSI_S) == true ||
          keyboard.get_status(status, kVK_DownArrow) == true;
      break;
    case WEST:
      return gamepad.get_button_status(status, kGP_Button_Leftface_Left, 0) == true ||
          gamepad.get_button_status(status, kGP_Button_Leftface_Left, 1) ||
          keyboard.get_status(status, kVK_ANSI_A) == true ||
          keyboard.get_status(status, kVK_LeftArrow) == true;
      break;
    case NORTH:
      return gamepad.get_button_status(status, kGP_Button_Leftface_Up, 0) == true ||
      gamepad.get_button_status(status, kGP_Button_Leftface_Up, 1) ||
          keyboard.get_status(status, kVK_ANSI_W) == true ||
          keyboard.get_status(status, kVK_UpArrow) == true;
      break;
  }
}

Coord1 PizzaController::getDirection(Coord1 status)
{
  for (Coord1 currDir = EAST; currDir < 8; currDir += 2)
  {
    if (getDirectionStatus(currDir, status, 0) == true)
    {
      return currDir;
    }
    
    if (getDirectionStatus(currDir, status, 1) == true)
    {
      return currDir;
    }

  }
  
  return ORIGIN;
}

struct PizzaExtender;

// ========================= Pizza ========================= //

namespace Pizza 
{
  Logical
  iPad = false,
  lite = false,
  widescreen = false;
  
  PizzaController* controller = NULL;
  GamesInterstitialManagerStd* moreGamesMan = NULL;
  PizzaExtender* extender = NULL;

  ScoreManager* scoreMan = NULL;
  Coord1 LEADERBOARD_ID = 1200;
  Coord1 ACH_OFFSET = 1200;
  Logical gcLogonShown = false;
  Logical useLoadImg = false;
  
  Point1 worldMapAlpha = 0.0;
  Logical liteBtnDown = false;
  Point2 platformTL(0.0, 0.0);

  CameraStd
  platformCam,
  adjustCam;
  
  Box platScreenBox; // includes ipad offset, not scale
  Coord1 shareBGID = 0;
  Logical showShopTut = false;
  
  // DEBUG constants
  Logical 
  DEBUG_COLLISION = false,
  DEBUG_PHYSICS = false,
  DEBUG_INVINCIBLE = false,
  DEBUG_NO_INTERFACE = false;

  const Char* DEBUG_LANGUAGE = NULL;
  
  // gameplay vars
  Coord2 currWorldPair(0, 0);
  Coord2 currLevelDef(0, 0);
  
  Coord1 playerCash = 0;
  ArrayList<Coord1> playerOutfit(NUM_PIZZA_LAYERS, 0x0);
  ArrayList<Coord1> playerUnlocks(NUM_PIZZA_LAYERS, 0x0);
  
  Coord1 playerScore = 0;
  Coord1 playerStars = 0;

  Point2 cameraPosition(0.0, 0.0);
  Point2 cameraMoved(0.0, 0.0);
  
  ArrayList<Coord1> currGameVars(NUM_VAR_TYPES, true);
  Point1 currFloatTime = 0.0;
  Coord2 currIngredientFound(-1, -1);
  Coord2 currIngredientUnlocked(-1, -1);
  
  void initGameConstants()
  {
    // Set these play constants based on iPhone vs. iPad, ad-free vs. iAd
    PLAY_W = 480.0;
    PLAY_H = 320.0;    
    PLAY_X = 0.0;
    PLAY_Y = 0.0;
    
    PLAY_XY = Point2(PLAY_X, PLAY_Y);
    PLAY_WH = Point2(PLAY_W, PLAY_H);
    PLAY_CX = RM::lerp(PLAY_X, PLAY_X + PLAY_W, 0.5);
    PLAY_CY = RM::lerp(PLAY_Y, PLAY_Y + PLAY_H, 0.5);
    PLAY_CENTER = Point2(PLAY_CX, PLAY_CY);
    PLAY_BOX = Box(PLAY_XY, PLAY_WH);
    
    lite = isFreeVersion();

    const Char* appID = "475212472";
    if (lite == true) appID = "490737748";

    // report_adcolony_install("5cb229cf8faba14138a38ae5f56422ee", appID);

    processStrings();
    currGameVars.set_all(0);
    initWorldConstants();
    setLevels();
    initToppingConstants();
    
    Pizza::controller = new PizzaController();

    updateScreenCam();
    
    Coord1 desiredTexScale = RMSystem->desired_texture_scale();
    imgSuffix = ".png";
    if (desiredTexScale > 1) imgSuffix = String1("_$x.png", '$', desiredTexScale);

    if (lite == true)
    {
      ACH_OFFSET = 1300;
      LEADERBOARD_ID = 1300;
    }
    else if (RMSystem->get_OS_family() == APPLETV_FAMILY)
    {
      ACH_OFFSET = 1800;
      LEADERBOARD_ID = 1800;
    }
    else if (RMSystem->get_OS_family() == MAC_FAMILY)
    {
      ACH_OFFSET = 2000;
      LEADERBOARD_ID = 2000;
    }
    
    loadPlayer();
    loadLevels();
    
    updatePlayerStats();    
    currWorldPair.x = topWorldUnlocked();
    
    scoreMan = new ScoreManager();
    scoreMan->load();
  }

  void initMoreGamesMan()
  {
    if (RMSystem->get_OS_family() != IOS_FAMILY)
    {
      return;
    }
    
    Box windowArea = Box::from_corners(platformCam.transform(Point2(44.0, 56.0)) / RMSystem->get_screen_scale().x,
                                       platformCam.transform(Point2(44.0, 56.0) + Point2(480.0, 232.0)) / RMSystem->get_screen_scale().y);
    
    moreGamesMan = new GamesInterstitialManagerStd();
    moreGamesMan->loadResources();
    moreGamesMan->init(isFreeVersion() ? GamesInterstitialGUIStd::PIZZA_LITE : GamesInterstitialGUIStd::PIZZA_FULL,
                       windowArea, controller, ResourceManager::font);  }

  void processStrings()
  {
    // NOTE: these match the indices of LanguageID
    ArrayList<String1> supportedSet;
    supportedSet.add("en");
    supportedSet.add("fr");
    supportedSet.add("it");
    supportedSet.add("de");
    supportedSet.add("es");
    
    RM::pick_language_std(supportedSet, LANGUAGE_CODE);
    if (DEBUG_LANGUAGE != NULL) LANGUAGE_CODE = DEBUG_LANGUAGE;
    
    LANGUAGE_ID = (LanguageID) supportedSet.find_index(String1(LANGUAGE_CODE));
    
    langSuffix = "_";
    langSuffix += LANGUAGE_CODE;
    if (LANGUAGE_ID == ENGLISH) langSuffix = "";
    
    String1 stringFilename("pizza_strings");
    stringFilename += langSuffix;
    stringFilename += ".txt";
    
    RMSystem->read_utf8(stringFilename, MASTER_LSTRING);
    
    MASTER_LSTRING.split(LSTRINGS, '\n');
    LSTRINGS.insert("FILLER LINE", 0);  // this is to make line numbers match array indices
    for (Coord1 i = 0; i < LSTRINGS.count; ++i) LSTRINGS[i].replace('+', '\n');
  }
  
  void initWorldConstants()
  {
    modeStrs.ensure_count(NUM_LEVEL_TYPES);
    modeStrs[LEVEL_GLADIATOR] = "gladiator";
    modeStrs[LEVEL_KARATE] = "karate";
    modeStrs[LEVEL_LAKE] = "lake";
    modeStrs[LEVEL_SLOPES] = "slopes";
    modeStrs[LEVEL_WRECKING] = "wreck";
    modeStrs[LEVEL_BALANCE] = "balance";
    modeStrs[LEVEL_BOUNCE] = "bounce";
    modeStrs[LEVEL_PUPPY] = "puppy";
    modeStrs[LEVEL_ANGRY] = "angry";
    modeStrs[LEVEL_SUMO] = "sumo";
    modeStrs[LEVEL_PLANE] = "plane";
    modeStrs[LEVEL_TIMING] = "timing";
    
    worldStrs.ensure_count(NUM_WORLD_TYPES);
    worldStrs[WORLD_GRAVEYARD] = "graveyard";
    worldStrs[WORLD_BADLANDS] = "badlands";
    worldStrs[WORLD_QUARRY] = "quarry";
    worldStrs[WORLD_PIRATE] = "pirate";
    worldStrs[WORLD_STADIUM] = "stadium";
    worldStrs[WORLD_NUCLEAR] = "nuclear";
    worldStrs[WORLD_COMPUTER] = "motherboard";
    worldStrs[WORLD_CARNIVAL] = "carnival";
    worldStrs[WORLD_MOON] = "moon";
    worldStrs[WORLD_MEAT] = "meatlocker";
    worldStrs[WORLD_CLOUD] = "cloud";
    worldStrs[WORLD_MARKET] = "market";
    
    MAP_SKY_COLORS.ensure_count(NUM_WORLD_TYPES);
    MAP_SKY_COLORS[WORLD_GRAVEYARD] = RM::color255(62.0, 75.0, 118.0);
    MAP_SKY_COLORS[WORLD_BADLANDS] = RM::color255(65.0, 61.0, 91.0);
    MAP_SKY_COLORS[WORLD_QUARRY] = RM::color255(148,173,235);
    MAP_SKY_COLORS[WORLD_PIRATE] = RM::color255(91,168,214);
    MAP_SKY_COLORS[WORLD_STADIUM] = RM::color255(46,72,129);
    MAP_SKY_COLORS[WORLD_NUCLEAR] = RM::color255(171,82,43);
    MAP_SKY_COLORS[WORLD_COMPUTER] = RM::color255(26,46,0);
    MAP_SKY_COLORS[WORLD_CARNIVAL] = RM::color255(15,31,65);
    MAP_SKY_COLORS[WORLD_MOON] = RM::color255(15,31,65);
    MAP_SKY_COLORS[WORLD_MEAT] = RM::color255(0,0,0);
    MAP_SKY_COLORS[WORLD_CLOUD] = RM::color255(136, 182, 207);
    MAP_SKY_COLORS[WORLD_MARKET] = RM::color255(136, 182, 207);
  }
  
  void initToppingConstants()
  {
    TOPPING_DEFS[CHEESE_LAYER].ensure_capacity(16);
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[1], 0));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[2], 300));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[3], 300));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[4], 500));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[5], 500));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[6], 700));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[7], 700));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[8], 1000));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[9], 1200));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[10], 1500));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[11], -1));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[12], -1));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[114], 1000));
    TOPPING_DEFS[CHEESE_LAYER].add(PizzaItem(LSTRINGS[120], -1));

    TOPPING_DEFS[TOPPINGS_LAYER].ensure_capacity(16);
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[14], 0));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[15], 0));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[16], 200));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[17], 300));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[18], 300));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[19], 300));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[20], 400));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[21], 400));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[22], 600));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[23], 700));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[24], -1));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[25], -1));
    TOPPING_DEFS[TOPPINGS_LAYER].add(PizzaItem(LSTRINGS[119], 3000));

    TOPPING_DEFS[EYES_LAYER].ensure_capacity(16);
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[27], 0));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[28], 0));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[29], 300));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[30], 400));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[31], 700));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[32], 800));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[33], 900));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[34], 900));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[35], 1200));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[36], 1500));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[37], -1));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[38], -1));
    TOPPING_DEFS[EYES_LAYER].add(PizzaItem(LSTRINGS[117], -1));
    
    TOPPING_DEFS[MOUTH_LAYER].ensure_capacity(16);
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[40], 0));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[41], 0));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[42], 300));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[43], 400));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[44], 700));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[45], 800));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[46], 900));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[47], 900));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[48], 1200));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[49], 1500));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[50], -1));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[51], -1));
    TOPPING_DEFS[MOUTH_LAYER].add(PizzaItem(LSTRINGS[116], 1500));
    
    TOPPING_DEFS[CLOTHES_LAYER].ensure_capacity(16);
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[53], 0));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[54], 300));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[55], 300));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[56], 400));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[57], 600));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[58], 600));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[59], 900));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[60], 900));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[61], 1200));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[62], 1500));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[63], -1));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[64], -1));
    TOPPING_DEFS[CLOTHES_LAYER].add(PizzaItem(LSTRINGS[115], -1));
  }
  
  void setLevels()
  {
    // 0 Graveyard
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_TUTORIAL, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_SPEARMEN, 65));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_SPINY, 62));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_STONEHENGE, 45, 28));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_A, 60, 74));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_B, 60, 171));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_GARGOYLE, 80));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_EASY, 90));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_SPIKEPIT, 92));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_GRAVEYARD, 60));
    
    // 1 Badlands
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_A, 15));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_B, 38));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_C, 70, 204));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_STICKY, 85));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_EASY, 90));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_ARROW, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_CASTLE, 41, 50));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_OSTRICH_V, 80));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_MOLE, 86));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_BADLANDS, 60));
    
    // 2 Quarry
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_GIANT, 87));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_STILL, 1, 2));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_BOUNCE, 1, 2));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_D, 92, 269));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_C, 53));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_SPIKEBALL, 100));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_HEAVY, 99));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_PYRAMID, 29, 40));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_FALLPIT, 91));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_QUARRY, 50));
    
    // 3 Pirate
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_EASY, 90));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_PEARL, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_CABIN, 40, 46));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_FIREBALL, 92));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_D, 41));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_WALK, 2, 3));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_FLAMESKULL, 105));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_E, 70, 232));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_FLY, 2, 4));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_PIRATE, 45));

    // 4 Stadium
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_LINES, 10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_CIVILIAN, 10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_OSTRICH_H, 68));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_F, 86, 344));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_HOP, 3, 5));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_E, 55));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_STARFISH, 45));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_FORT, 39, 44));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_SQUISH, 87));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_STADIUM, 45));

    // 5 Nuclear
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_F, 9));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_BASEBALL, 16));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_BIG, 90));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_IGLOO, 36, 43));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_G, 78, 274));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_PUFFER, 51));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_CUPID, 92));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_CLIMBER, 13));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_SPEAR, 3, 6));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_NUCLEAR, 40));

    // 6 Computer
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_ROBOT, 85));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_PAGODA, 31, 44));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_BOMB, 107));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_JUNK, 15));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_H, 81, 242));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_LIGHTNING, 86));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_FLIER, 16));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_MIMIC, 107));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_TIDE, 45));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_COMPUTER, 38));

    // 7 Carnival
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_G, 41));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_ILLUSION, 4, 7));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_EXPLODE, 120));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_HARDWALL, 17));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_SWORDFISH, 40));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_WISP, 160));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_CIRCLE, 4, 8));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_I, 82, 258));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_ARROW, 114));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_CARNIVAL, 36));

    // 8 Moon
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_GLASS, 26, 27));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_POISON, 36));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_H, 24));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_UFO, 11));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_GRAVITY, 68));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_GRAVITY, 5, 9));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_MIX, 120));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_NAGA, 83));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_CUTTER, 15));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_MOON, 50));

    // 9 Meat
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BALANCE, BALANCE_I, 13));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SLOPES, SLOPES_J, 149, 557));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_CAVE, 36, 53));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_SUN, 124));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_HARPOON, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_MIX, 15));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_MIX, 5, 10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PUPPY, PUPPY_SUN, 270));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_STICKY_MIX, 130));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_MEAT, 30));

    // 10 Cloud
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PLANE, PLANE_CUPID, 65));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PLANE, PLANE_WISP, 45));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_RABBIT, 82));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_BARREL, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_CLOUDCITY, 26, 37));
    WORLD_LEVELS.last().add(LevelData(LEVEL_WRECKING, WRECK_BATTER, 14));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PLANE, PLANE_NAGA, 30));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_TAIL, 60));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_STREAKER, 88));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_CLOUD, 41));
    
    // 11 market
    WORLD_LEVELS.add(ArrayList<LevelData>(10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_TIMING, TIMING_CIRCLES, 60, 90));
    WORLD_LEVELS.last().add(LevelData(LEVEL_TIMING, TIMING_SCALERS, 80, 120));
    WORLD_LEVELS.last().add(LevelData(LEVEL_GLADIATOR, GLADIATOR_DISK, 70));
    WORLD_LEVELS.last().add(LevelData(LEVEL_PLANE, PLANE_NODE, 28));
    WORLD_LEVELS.last().add(LevelData(LEVEL_BOUNCE, BOUNCE_ENERGY, 65));
    WORLD_LEVELS.last().add(LevelData(LEVEL_KARATE, KARATE_MUMMY, 5, 10));
    WORLD_LEVELS.last().add(LevelData(LEVEL_LAKE, LAKE_EEL, 50));
    WORLD_LEVELS.last().add(LevelData(LEVEL_ANGRY, ANGRY_MARKET, 49, 66));
    WORLD_LEVELS.last().add(LevelData(LEVEL_TIMING, TIMING_SLIDERS, 100, 150));
    WORLD_LEVELS.last().add(LevelData(LEVEL_SUMO, SUMO_MARKET, 40));
    
    Point1 currCount = 0.0;
    Point1 countHedge = 0.001;
    
    STARS_TO_UNLOCK.ensure_count(WORLD_LEVELS.count);
    for (Coord1 world = 0; world < STARS_TO_UNLOCK.count; ++world)
    {
      STARS_TO_UNLOCK[world].ensure_count(WORLD_LEVELS[world].count);
      
      for (Coord1 level = 0; level < STARS_TO_UNLOCK[world].count; ++level)
      {
        if (world == 0 && level == 9) currCount += 1.0 + countHedge;  // 10th level goal = 10
        else if (world == 6 && level == 9) currCount -= 1.0 + countHedge;  // 70th goal = 100
        else if (world == 9 && level == 9) currCount += 3.0 + countHedge;  // 100th goal = 165
        else if (world == 10 && level == 0) currCount -= 2.0 + countHedge; // 101st goal = 166
        else if (world == 10 && level == 9) currCount -= 1.0 + countHedge; // 110th goal = 185
        else if (world == 11 && level == 0) currCount -= 1.0 + countHedge; // 111st goal = 186
        else if (world == 11 && level == 9) currCount -= 1.0 + countHedge; // 120st goal = 205
        
        STARS_TO_UNLOCK[world][level] = static_cast<Coord1>(currCount);
        // cout << STARS_TO_UNLOCK[world][level] << endl;
        
        if (world >= 9) currCount += 2.25 + countHedge;
        else if (world >= 6) currCount += 2.00 + countHedge;
        else if (world >= 3) currCount += 1.75 + countHedge;
        else currCount += 1.0 + countHedge;
      }      
    }

    goalEarnedStrs[LevelData::GOAL_TIME] = LSTRINGS[66];
    goalEarnedStrs[LevelData::GOAL_UNHARMED] = LSTRINGS[67];
    goalEarnedStrs[LevelData::GOAL_BOARDS] = LSTRINGS[68];
    goalEarnedStrs[LevelData::GOAL_COINS] = LSTRINGS[69];
    goalEarnedStrs[LevelData::GOAL_OBJECTS] = LSTRINGS[70];
    goalEarnedStrs[LevelData::GOAL_ENEMIES] = LSTRINGS[71];
    goalEarnedStrs[LevelData::GOAL_METERS] = LSTRINGS[118];
    goalEarnedStrs[LevelData::GOAL_WHEEL] = LSTRINGS[72];
    
    goalRequiredStrs[LevelData::GOAL_TIME] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_UNHARMED] = "";
    goalRequiredStrs[LevelData::GOAL_BOARDS] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_COINS] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_OBJECTS] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_ENEMIES] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_METERS] = LSTRINGS[73];
    goalRequiredStrs[LevelData::GOAL_WHEEL] = "";
    
    CHUNK_ENDS[CHUNK_SKI_START].set(2432.0, 256.0);
    CHUNK_ENDS[CHUNK_SKI_4_1].set(576.0, 128.0);
    CHUNK_ENDS[CHUNK_SKI_3_1].set(576.0, 192.0);
    CHUNK_ENDS[CHUNK_SKI_2_1].set(512.0, 256.0);
    CHUNK_ENDS[CHUNK_SKI_DROP].set(1520.0, 1088.0);
    CHUNK_ENDS[CHUNK_SKI_GAP].set(3808.0, 1184.0);
    CHUNK_ENDS[CHUNK_SKI_JUMP].set(2816.0, 720.0);
    CHUNK_ENDS[CHUNK_SKI_BOUNCEGAP].set(3664.0, 1360.0);  
    CHUNK_ENDS[CHUNK_SKI_BOUNCEJUMP].set(3296.0, 800.0);
    CHUNK_ENDS[CHUNK_SKI_DUALCOIN].set(1536.0, 1088.0);
    CHUNK_ENDS[CHUNK_SKI_SPIKEPLATFORMS].set(3072.0, 1216.0);
    CHUNK_ENDS[CHUNK_SKI_FLAMESKULL].set(1728.0, 512.0);
    CHUNK_ENDS[CHUNK_SKI_HILLSKULL].set(2432.0, 496.0);
    CHUNK_ENDS[CHUNK_SKI_JAGSKULL].set(2688.0, 864.0);
    CHUNK_ENDS[CHUNK_SKI_LONGFALL].set(3008.0, 1088.0);
    CHUNK_ENDS[CHUNK_SKI_SPIKEFALL].set(2144.0, 1664.0);
    CHUNK_ENDS[CHUNK_SKI_STRAIGHTBREAK].set(2304.0, 768.0);
    CHUNK_ENDS[CHUNK_SKI_SWITCHSLOPE].set(2560.0, 1280.0);
    CHUNK_ENDS[CHUNK_SKI_SWITCHGAP].set(2096.0, 768.0);
    CHUNK_ENDS[CHUNK_SKI_SPIKEPATHS].set(3328.0, 1152.0);
    CHUNK_ENDS[CHUNK_SKI_FALLBREAK].set(2288.0, 1216.0);
    CHUNK_ENDS[CHUNK_SKI_END].set(2368.0, 384.0);
    
    CHUNK_ENDS[CHUNK_SPIKE_START].set(1536.0, 32.0);
    CHUNK_ENDS[CHUNK_SPIKE_FLAT].set(512.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_BUMP].set(512.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_DOWNHILL].set(448.0, 96.0);
    CHUNK_ENDS[CHUNK_SPIKE_DROP].set(448.0, 144.0);
    CHUNK_ENDS[CHUNK_SPIKE_UPHILL].set(448.0, -96.0);    
    CHUNK_ENDS[CHUNK_SPIKE_BOUNCECIRCLES].set(1536.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_BOUNCEDROPS].set(1792.0, 32.0);
    CHUNK_ENDS[CHUNK_SPIKE_LONGFALL].set(1024.0, 128.0);
    CHUNK_ENDS[CHUNK_SPIKE_NOSPIKE].set(1536.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_RISER].set(832.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_PILLAR].set(1056.0, -32.0);
    CHUNK_ENDS[CHUNK_SPIKE_SPIKEBALL].set(1024.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_SPRING].set(1024.0, -128.0);
    CHUNK_ENDS[CHUNK_SPIKE_STRAIGHTBOUNCE].set(1856.0, 128.0);
    CHUNK_ENDS[CHUNK_SPIKE_TRANSFERBLOCK].set(1408.0, -32.0);
    CHUNK_ENDS[CHUNK_SPIKE_TRANSFERDROP].set(1344.0, 32.0);
    CHUNK_ENDS[CHUNK_SPIKE_TRANSFERSPIKE].set(1216.0, -64.0);
    CHUNK_ENDS[CHUNK_SPIKE_BOUNCESTEPS].set(2176.0, 512.0);
    CHUNK_ENDS[CHUNK_SPIKE_DOWNHILLBUMP].set(1408.0, 160.0);
    CHUNK_ENDS[CHUNK_SPIKE_DUALRISE].set(1280.0, -112.0);
    CHUNK_ENDS[CHUNK_SPIKE_RISEBALLS].set(1664.0, -128.0);
    CHUNK_ENDS[CHUNK_SPIKE_RISESTEPS].set(1472.0, -384.0);
    CHUNK_ENDS[CHUNK_SPIKE_BALLDOWNHILL].set(1536.0, 288.0);
    CHUNK_ENDS[CHUNK_SPIKE_BALLJUMP].set(1344.0, 192.0);
    CHUNK_ENDS[CHUNK_SPIKE_ZIGZAG].set(2048.0, 0.0);
    CHUNK_ENDS[CHUNK_SPIKE_END].set(1536.0, 32.0);
    
    CHUNK_ENDS[CHUNK_PUPPY_DOGHOUSE].set(1024.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_HILL].set(896.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_CRATE].set(960.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_DIP].set(768.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEPITHILL].set(1856.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEJUMP].set(1216.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_TRICKYJUMP].set(2048.0, 0.0);    
    CHUNK_ENDS[CHUNK_PUPPY_LAVAHILL].set(2176.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_BIGOBJHILL].set(2176.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_BOUNCEPIT].set(2432.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_CRATEPIT].set(1616.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_DUALFIREBALL].set(1024.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_GROUNDTRIANGLE].set(1024.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_LAVAPOOL].set(1024.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_OBJSTEPS].set(1664.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_PILLARS].set(2304.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_QUADSPIKE].set(1664.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SINGLETRI].set(2144.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEBALLBUMPS].set(1600.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEBALLHILL].set(1536.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEBALLPILLARS].set(2592.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SPIKEBALLPIT].set(1344.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_SWINGSQUARE].set(1936.0, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_TRIANGLEPIT].set(2464, 0.0);
    CHUNK_ENDS[CHUNK_PUPPY_WALLJUMP].set(1152.0, 0.0);
    
    for (Coord1 i = 0; i < CUPID_FIRE_TIMES.count; ++i)
    {
      CUPID_FIRE_TIMES[i] = 0.2;
      if (i == 5) CUPID_FIRE_TIMES[i] = 0.4;
      else if (i > 5) CUPID_FIRE_TIMES[i] = 0.1;
    }
}
  
  void resetGame()
  {
    cameraPosition.set(0.0, 0.0);
    cameraMoved.set(0.0, 0.0);
    currIngredientFound.set(-1, -1);
    currIngredientUnlocked.set(-1, -1);
    
    currGameVars.set_all(0);
    currFloatTime = 0.0;
  }
  
  Logical isWorldUnlocked(Coord1 worldID)
  {   
    if (lite == true) return worldID == 0;
    
    if (worldID < 0 || worldID >= WORLD_LEVELS.count) return false;
    if (worldID == 0) return true;
    
    return WORLD_LEVELS[worldID - 1].last().starsEarned > 0;
  }
  
  Logical isLevelUnlocked(Coord2 levelID)
  {
    return playerStars >= STARS_TO_UNLOCK[levelID.x][levelID.y];
  }
  
  Coord1 topWorldUnlocked()
  {
    for (Coord1 i = WORLD_LEVELS.count - 1; i >= 0; --i)
    {
      if (isWorldUnlocked(i) == true) return i;
    }
    return 0;
  }
  
  Logical toppingExists(Coord2 toppingID)
  {
    if (toppingID.x < 0 || toppingID.y < 0) return false;
    if (toppingID.x >= TOPPING_DEFS.count) return false;
    if (toppingID.y >= TOPPING_DEFS[toppingID.x].count) return false;
    if (lite == true && toppingID.y >= 4) return false;
    
    return true;
  }

  Logical isToppingUnlocked(Coord2 toppingID)
  {
    return (playerUnlocks[toppingID.x] & (0x1 << toppingID.y)) != 0;
  }
  
  void unlockTopping(Coord2 toppingID)
  {
    Pizza::playerUnlocks[toppingID.x] |= 0x1 << toppingID.y;
  }
  
  Byte4 starBitsForWonLevel()
  {
    LevelData& levelDef = currLevelData();
    Coord1 starsBitsEarned = 0x1;
    
    for (Coord1 goalIndex = 0; goalIndex < levelDef.goalTypes.count(); ++goalIndex)
    {
      Coord1 goalType = levelDef.goalTypes[goalIndex];
      Coord1 goalVal = levelDef.goalValues[goalIndex];
      Coord1 earnedValIndex = goalTypeToVarIndex(goalType);
      Coord1 earnedVal = currGameVars[earnedValIndex];
      
      Logical earnedThisStar = false;
      
      switch (goalType)
      {
        default: cout << "starBitsForWonLevel: " << goalType << " not recognized." << endl;
        case LevelData::GOAL_TIME: if (earnedVal <= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_UNHARMED: if (earnedVal == 0) earnedThisStar = true; break;
        case LevelData::GOAL_BOARDS: if (earnedVal >= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_COINS: if (earnedVal >= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_OBJECTS: if (earnedVal >= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_ENEMIES: if (earnedVal >= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_METERS: if (earnedVal >= goalVal) earnedThisStar = true; break;
        case LevelData::GOAL_WHEEL: if (earnedVal > 0) earnedThisStar = true; break;
      }
      
      if (earnedThisStar == true) starsBitsEarned |= 0x1 << (goalIndex + 1);
    }
    
    return starsBitsEarned;
  }
  
  void runGameTime(Point1 time)
  {
    currFloatTime += time;
    currGameVars[VAR_SECONDS] = RM::pt_to_coord(currFloatTime);
  }
  
  Coord1 goalTypeToVarIndex(Coord1 goalType)
  {
    switch (goalType)
    {
      default: cout << "goalTypeToVarIndex: " << goalType << " not recognized." << endl;
      case LevelData::GOAL_TIME: return VAR_SECONDS;
      case LevelData::GOAL_UNHARMED: return VAR_DAMAGE;
      case LevelData::GOAL_BOARDS: return VAR_DEFEATED;
      case LevelData::GOAL_COINS: return VAR_NUM_COINS;
      case LevelData::GOAL_OBJECTS: return VAR_DEFEATED;
      case LevelData::GOAL_ENEMIES: return VAR_DEFEATED;
      case LevelData::GOAL_METERS: return VAR_DEFEATED;
      case LevelData::GOAL_WHEEL: return VAR_WHEEL_EARNED;
    }
  }

  LevelData& currLevelData()
  {
    return getLevelData(currWorldPair);
  }
  
  LevelData& getLevelData(Coord2 worldPair)
  {
    return WORLD_LEVELS[worldPair.x][worldPair.y];
  }
  
  Coord2 lookupLevelPos(Coord2 levelDef)
  {
    Coord2 result(-1, -1);
    Logical found = false;
    
    for (Coord1 world = 0; world < WORLD_LEVELS.count; ++world)
    {
      for (Coord1 level = 0; level < WORLD_LEVELS[world].count; ++level)
      {
        if (WORLD_LEVELS[world][level].typeData == levelDef)
        {
          result.set(world, level);
          found = true;
          break;
        }
      }
      
      if (found == true) break;
    }
    
    return result;
  }

  Coord1 calcStarBonus(Coord1 worldID, Coord1 numStars)
  {
    Coord1 bonus = 0;
    
    if (numStars == 2) bonus = 500 + 100 * worldID;
    else if (numStars == 3) bonus = 1000 + 200 * worldID;
    
    return bonus;
  }
  
  Coord1 calcTimeBonus(const LevelData& level, Coord1 seconds)
  {
    Coord1 bonus = 0;
    
    if (level.goalTypes.x == LevelData::GOAL_TIME)
    {
      bonus = (level.goalValues.x - seconds) * 25;
    }
    else if (level.goalTypes.y == LevelData::GOAL_TIME)
    {
      bonus = (level.goalValues.y - seconds) * 25;
    }
    
    bonus = std::max(0, bonus);
    return bonus;
  }
  
  Logical alreadyBeatenGame()
  {
    return WORLD_LEVELS[9][9].starsEarned > 0;
  }
  
  void getInstructionsStr(const LevelData& level, String1& output)
  {
    output = level.instructionsBase.as_cstr();
    // cout << output << endl;
    
    if (RMSystem->get_OS_family() == MAC_FAMILY ||
        RMSystem->get_OS_family() == APPLETV_FAMILY)
    {
      Logical localized = output.index_of("&") == output.last_i();
      
      if (localized == true)
      {
        output.remove_last();
      }
      
      if (RMSystem->get_OS_family() == MAC_FAMILY)
      {
        output += "_mac";
      }
      else if (RMSystem->get_OS_family() == APPLETV_FAMILY)
      {
        output += "_tv";
      }
      
      // to indicate localization
      output += '&';
      output = ResourceManager::imgStr(output.as_cstr());
      
      String1 foundName;
      
      // no special platform version, use the iphone version
      if (RMSystem->find_leq_filename(output.as_cstr(), foundName) == -1)
      {
        output = Pizza::currLevelData().instructionsBase.as_cstr();
        output = ResourceManager::imgStr(output.as_cstr());
      }
    }
    else
    {
      output = ResourceManager::imgStr(output.as_cstr());
    }
  }
  
  void unlockLevelsDebug()
  {
    for (Coord1 currWorld = 0; currWorld < WORLD_LEVELS.count; ++currWorld)
    // for (Coord1 currWorld = 0; currWorld < 10; ++currWorld)
    {
      for (Coord1 currLevel = 0; currLevel < WORLD_LEVELS[currWorld].count; ++currLevel)
      {
        getLevelData(Coord2(currWorld, currLevel)).starsEarned = 3;
      }
    }
    
    updatePlayerStats();
    currWorldPair.x = topWorldUnlocked();
  }
  
  void unlockOutfitsDebug()
  {
    for (Coord1 i = 0; i < playerUnlocks.count; ++i)
    {
      playerUnlocks[i] = 0xffff;
    }
  }
  
  void loadPlayer()
  {
    String1 masterStr(64);
    
    RMSystem->read_file(playerFilename, masterStr);
    readPlayer(masterStr);
    
    RMSystem->read_pasteboard(PLAYER_PASTEBOARD_LITE, masterStr);
    
    if (lite == false && masterStr.length != 0) 
    {
      // cout << "lite player exists, reading..." << endl;
      readPlayer(masterStr);
      savePlayer();
      RMSystem->write_pasteboard(PLAYER_PASTEBOARD_LITE, String1(""));
    }
  }
  
  void readPlayer(const String1& fullPlayerStr)
  {
    ArrayList<String1> masterLines;
    fullPlayerStr.split(masterLines, '\n');
    
    ArrayList<Coord1> unlocksFromFile;
    
    playerCash = std::max(playerCash, masterLines[0].as_int());
    Str::string_to_ints(masterLines[1], playerOutfit);
    Str::string_to_ints(masterLines[2], unlocksFromFile);
    
    // merge unlock data
    for (Coord1 i = 0; i < unlocksFromFile.count; ++i) playerUnlocks[i] |= unlocksFromFile[i];
  }
  
  void savePlayer()
  {
    String1 playerStr;
    writePlayer(playerStr);
    RMSystem->write_file(playerFilename, playerStr);
    
    if (lite == true)
    {
      RMSystem->write_pasteboard(PLAYER_PASTEBOARD_LITE, playerStr);
    }
  }
  
  void writePlayer(String1& destination)
  {
    destination.ensure_capacity(64);
    String1 bufferStr(32);
    
    bufferStr.int_string(Pizza::playerCash);
    destination += bufferStr;
    destination += '\n';
    
    bufferStr.clear();
    Str::ints_to_string(Pizza::playerOutfit, bufferStr);
    destination += bufferStr;
    destination += '\n';
    
    bufferStr.clear();
    Str::ints_to_string(Pizza::playerUnlocks, bufferStr);
    destination += bufferStr;
  }
  
  void loadLevels()
  {
    String1 masterStr(2048);
    RMSystem->read_file(levelFilename, masterStr);
    readLevels(masterStr);
    
    RMSystem->read_pasteboard(LEVEL_PASTEBOARD_LITE, masterStr);
    
    if (lite == false && masterStr.length > 0)
    {
      // cout << "free level file exists, reading..." << endl;
      RMSystem->analytics_event("Resume from Free version");
      readLevels(masterStr);
      saveLevels();
      RMSystem->write_pasteboard(LEVEL_PASTEBOARD_LITE, String1(""));
    }
  }
  
  void readLevels(const String1& fullLevelStr)
  {
    if (fullLevelStr.last() != '#')
    {
      cout << "Error loading level file, saving default level data" << endl;
      saveLevels();
      return;
    }
    
    ArrayList<String1> masterLines(128);
    fullLevelStr.split(masterLines, '\n');
    
    ArrayList<Coord1> valBuffer(4);
    
    for (Coord1 currLine = 0; currLine < masterLines.count; ++currLine)
    {
      if ((masterLines[currLine])[0] == '#')
      {
        break;
      }
      
      Str::string_to_ints(masterLines[currLine], valBuffer);
      LevelData& currLevel = WORLD_LEVELS[valBuffer[0]][valBuffer[1]];
      
      // merge stars and best score
      currLevel.setStarsEarned(std::max(currLevel.starsEarned, valBuffer[2]));
      currLevel.bestScore = std::max(currLevel.bestScore, valBuffer[3]);
    }
  }
  
  void saveLevels()
  {
    String1 levelStr;
    writeLevels(levelStr);
    RMSystem->write_file(levelFilename, levelStr);
    
    if (lite == true)
    {
      RMSystem->write_pasteboard(LEVEL_PASTEBOARD_LITE, levelStr);
    }
  }
 
  void writeLevels(String1& destination)
  {
    destination.ensure_capacity(2048);
    String1 lineBuffer(32);
    
    for (Coord1 currWorld = 0; currWorld < WORLD_LEVELS.count; ++currWorld)
    {
      for (Coord1 currLevel = 0; currLevel < WORLD_LEVELS[currWorld].count; ++currLevel)
      {
        lineBuffer = "! @ # $\n";
        lineBuffer.int_replace('!', currWorld);
        lineBuffer.int_replace('@', currLevel);
        lineBuffer.int_replace('#', WORLD_LEVELS[currWorld][currLevel].starsEarned);
        lineBuffer.int_replace('$', WORLD_LEVELS[currWorld][currLevel].bestScore);
        
        destination += lineBuffer;
      }
    }
    
    destination += '#';
  }
  
  void updatePlayerStats()
  {
    playerScore = 0;
    playerStars = 0;
    
    for (Coord1 currWorld = 0; currWorld < WORLD_LEVELS.count; ++currWorld)
    {
      for (Coord1 currLevel = 0; currLevel < WORLD_LEVELS[currWorld].count; ++currLevel)
      {
        LevelData& currLevelData = WORLD_LEVELS[currWorld][currLevel];
        
        Coord1 levelStars = currLevelData.starsEarned;
        if (levelStars > 0) playerStars += levelStars;  // could be negative so don't always add
        
        playerScore += currLevelData.bestScore;
      }
    }
  }
  
  Coord1 starsInFirst100()
  {
    Coord1 mainGameStars = 0;
    
    for (Coord1 currWorld = 0; currWorld <= WORLD_MEAT; ++currWorld)
    {
      for (Coord1 currLevel = 0; currLevel < WORLD_LEVELS[currWorld].count; ++currLevel)
      {
        LevelData& currLevelData = WORLD_LEVELS[currWorld][currLevel];
        Coord1 levelStars = currLevelData.starsEarned;
        
        if (levelStars > 0) mainGameStars += levelStars;  // could be negative so don't always add
      }
    }
    
    return mainGameStars;
  }
  
  Box deviceScreenBox()
  {
    return Box(platformCam.xy, platformCam.size);
  }
  
  void updateScreenCam()
  {
    platformCam.syncAsScreenCam(PLAY_WH, PLAY_WH, CameraStd::AVOID_LETTERBOXING);
    // cout << "platformCam xy " << platformCam.xy << " size " << platformCam.size << endl;
    
    // SCREEN_BOX = Box(platformCam.xy, platformCam.size);
    Point1 boxWidth = RM::clamp(platformCam.size.x, PLAY_W, RM_WH.x);
    Point1 boxHeight = RM::clamp(platformCam.size.y, PLAY_H, RM_WH.y);
    SCREEN_BOX = Box::from_center(platformCam.untranslatedBox().center(), Point2(boxWidth, boxHeight));
    
    SCREEN_W = SCREEN_BOX.width();
    SCREEN_H = SCREEN_BOX.height();
    SCREEN_WH = Point2(SCREEN_W, SCREEN_H);
    SCREEN_CENTER = SCREEN_WH * (Point1) 0.5;
    
    platformTL = SCREEN_BOX.xy;
    platScreenBox = SCREEN_BOX;
  }

  void letterboxContent(Point2 maxContent)
  {
    Box logicalScreenBox = platformCam.untranslatedBox();
    ColorP4 boxColor = BLACK_SOLID;
    
    if (platformCam.xy.x < 0.0)
    {
      // horizontal letterbox
      Box firstLetterbox = Box::from_corners(logicalScreenBox.xy,
                                             SCREEN_BOX.norm_pos(HANDLE_BL));
      Box secondLetterbox = Box::from_corners(Point2(SCREEN_BOX.right(), SCREEN_BOX.top()),
                                              logicalScreenBox.norm_pos(HANDLE_BR));
      
      firstLetterbox.draw_solid(boxColor);
      secondLetterbox.draw_solid(boxColor);
      
      // cout << "horizontal: first " << firstLetterbox << " second " << secondLetterbox << endl;
    }
    
    if (platformCam.xy.y < 0.0)
    {
      // vertical letterbox
      Box firstLetterbox = Box::from_corners(logicalScreenBox.xy,
                                             SCREEN_BOX.norm_pos(HANDLE_TR));
      Box secondLetterbox = Box::from_corners(Point2(logicalScreenBox.left(), SCREEN_BOX.bottom()),
                                              logicalScreenBox.norm_pos(HANDLE_BR));
      firstLetterbox.draw_solid(boxColor);
      secondLetterbox.draw_solid(boxColor);
      
      // cout << "vertical: first " << firstLetterbox << " second " << secondLetterbox << endl;
    }
    
//    Box transformedBox = Box::from_corners(CameraStd::worldToScreen(SCREEN_BOX.norm_pos(HANDLE_TL)),
//                                           CameraStd::worldToScreen(SCREEN_BOX.norm_pos(HANDLE_BR)));
//    cout << "total trans " << transformedBox << endl;
//    
//    Box platformBox = Box::from_corners(platformCam.worldToScreen(SCREEN_BOX.norm_pos(HANDLE_TL)),
//                                        platformCam.worldToScreen(SCREEN_BOX.norm_pos(HANDLE_BR)));
//    cout << "total trans " << transformedBox << endl;
//    cout << "platform trans " << platformBox << endl;
    
    // logicalScreenBox.draw_outline(GREEN_SOLID);
    // SCREEN_BOX.draw_outline(BLUE_SOLID);
  }
  
  Box leftLetterbox(Point2 maxContent)
  {
    if (platformCam.xy.x >= 0.0)
    {
      return Box(-1.0, -1.0, -1.0, -1.0);
    }
    
    Box logicalScreenBox = platformCam.untranslatedBox();
    Box firstLetterbox = Box::from_corners(logicalScreenBox.xy,
                                           SCREEN_BOX.norm_pos(HANDLE_BL));
    return firstLetterbox;
  }
  
  Box rightLetterbox(Point2 maxContent)
  {
    if (platformCam.xy.x >= 0.0)
    {
      return Box(-1.0, -1.0, -1.0, -1.0);
    }
    
    Box logicalScreenBox = platformCam.untranslatedBox();
    Box secondLetterbox = Box::from_corners(Point2(SCREEN_BOX.right(), SCREEN_BOX.top()),
                                            logicalScreenBox.norm_pos(HANDLE_BR));
    return secondLetterbox;
  }
  
  Box contentBox(Point2 maxContent)
  {
    return Box::from_corners(leftLetterbox(maxContent).norm_pos(HANDLE_TR),
                             rightLetterbox(maxContent).norm_pos(HANDLE_BL));
  }
}

// =============================== ScoreManager ============================ //

Logical ScoreManager::DEBUG_REPORT = false;

ScoreManager::ScoreManager() :
  SystemGameCenterListener(),

  saveFilename("PS012.sav"),
  repostSeconds(60.0),

  topScores(MAX_SCORES),
  scoresPosted(0xffffffff),

  achievements(MAX_ACHIEVEMENTS),
  achPosted(0xffffffff),

  needsSave(false),
  currTime(0.0),

  recentLoginFailed(false)
{
  for (Coord1 i = 0; i < MAX_SCORES; ++i) topScores.add(0);
  for (Coord1 i = 0; i < MAX_ACHIEVEMENTS; ++i) achievements.add(0.0);
}

void ScoreManager::update()
{
  currTime += RM::timePassed();
  if (currTime >= repostSeconds)
  {
    checkAndRepost();
    currTime -= repostSeconds;
  }
  
  if (needsSave == true)
  {
    save();
    needsSave = false;
  }
}

Coord1 ScoreManager::topScore()
{
  return topScores[0];
}

void ScoreManager::checkAndRepost()
{
  for (Coord1 i = 0; i < MAX_SCORES; ++i)
  {
    if ((scoresPosted & (0x1 << i)) == 0) 
    {
      RMSystem->gamecenter_post_int(i + Pizza::LEADERBOARD_ID, topScores[i]);
    }
  }
  for (Coord1 i = 0; i < MAX_ACHIEVEMENTS; ++i)
  {
    if ((achPosted & (0x1 << i)) == 0) 
    {
      RMSystem->gamecenter_achievement(i + Pizza::ACH_OFFSET, achievements[i], false);      
    }
  }
}

Logical ScoreManager::earnedScore(Coord1 localID, Coord1 score)
{
  Coord1 globalID = localID + Pizza::LEADERBOARD_ID;
  
  RMSystem->gamecenter_post_int(globalID, score);
  if (DEBUG_REPORT == true) cout << "Try post score: board " << globalID << " score " << score << endl;
    
  if (score > topScores[0])
  {
    topScores[0] = score;
    scoresPosted &= ~(0x1 << localID);
    needsSave = true;
    return true;
  }
  return false;
}

Logical ScoreManager::earnedAchievement(Coord1 localID, Point1 percent)
{
  if (Pizza::lite == true) return false;
  
  Logical showBanner = false;
  Coord1 globalID = localID + Pizza::ACH_OFFSET;
  
  if (DEBUG_REPORT == true) cout << "Try post achievement: id " << globalID << " %" << percent << endl;
  
  if (percent > achievements[localID])
  {
    achievements[localID] = percent;
    achPosted &= ~(0x1 << localID);
    needsSave = true;
    if (percent > 99.0) showBanner = true;
  }
  
  RMSystem->gamecenter_achievement(globalID, percent, showBanner);
  return showBanner;
}

void ScoreManager::save()
{
  String1 saveStr(128);
  
  String1 bufferStr(64);
  Str::ints_to_string(topScores, bufferStr);
  saveStr += bufferStr;
  saveStr += '\n';
  
  bufferStr.clear();
  bufferStr.int_string(scoresPosted);
  saveStr += bufferStr;
  saveStr += '\n';
  
  bufferStr.clear();
  Str::doubles_to_string(achievements, bufferStr);
  saveStr += bufferStr;
  saveStr += '\n';
  
  bufferStr.clear();
  bufferStr.int_string(achPosted);
  saveStr += bufferStr;
  
  if (DEBUG_REPORT == true) cout << "Saving...\n" << saveStr << endl;
  RMSystem->write_file(saveFilename, saveStr);
}

void ScoreManager::load()
{
  String1 masterStr(128);
  RMSystem->read_file(saveFilename, masterStr);
  if (DEBUG_REPORT == true) cout << "Loading...\n" << masterStr << endl;
  
  ArrayList<String1> lines(4);
  masterStr.split(lines, '\n');
  
  Str::string_to_ints(lines[0], topScores);
  scoresPosted = lines[1].as_int();
  Str::string_to_doubles(lines[2], achievements);
  achPosted = lines[3].as_int();
}

void ScoreManager::score_posted(Coord1 boardID, Coord1 score)
{
  Coord1 localizedID = boardID - Pizza::LEADERBOARD_ID;
  
  // there's only one board for space Ika
  if (score >= topScores[localizedID]) 
  {
    scoresPosted |= (0x1 << localizedID);
    needsSave = true;
  }
  
  if (DEBUG_REPORT == true)
  {
    cout << "Score post successful: board " << boardID << " score " << score << endl;
    cout << "Scores posted " << std::hex << scoresPosted << std::dec << endl;    
  }
}

void ScoreManager::achievement_posted(Coord1 achID, Point1 percent)
{
  Coord1 localizedID = achID - Pizza::ACH_OFFSET;
  
  if (RM::approxGt(achievements[localizedID], percent, 0.01) == true)
  {
    achPosted |= (0x1 << localizedID);
    needsSave = true;
  }
  
  if (DEBUG_REPORT == true) 
  {
    cout << "Achievement post successful: id " << achID << " %" << percent << endl;
    cout << "Achievements posted " << std::hex << achPosted << std::dec << endl;
  }
}

void ScoreManager::gc_tried_logon(Logical success)
{
  recentLoginFailed = success == false;
  if (DEBUG_REPORT == true) cout << "recentLoginFailed " << recentLoginFailed << endl; 
}

