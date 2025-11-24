//
//  globals.cpp
//  NewRMIPhone
//
//  Created by Paul Stevens on 12/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "globals.h"
#include "resourceman.h"
#include "mainapp.h"

using std::cout;
using std::endl;

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

PizzaController* controller = NULL;
PlayerDataMap* defaultPlayer = NULL;
PlayerDataMap* activePlayer = NULL;
TempGameplayData* activeGameplayData = NULL;

ArrayList<LevelData> levelList(128);
ArrayList<IngredientData> ingredientList(128);
DataList<Coord1> numSelectableIngs(NUM_INGREDIENT_LOCATIONS);

RenderTarget* offscreenBuffer(NULL);
Image* skullImg(NULL);

String2 MASTER_LSTRING;
ArrayList<String2> LSTRINGS(512);
String1 LANGUAGE_CODE("en");
String1 langSuffix("_en");
LanguageID LANGUAGE_ID = ENGLISH;
String2 EFIGS_CHARS;
String2 ASCII_CHARS;

// =============================== PizzaController ============================ //

PizzaController::PizzaController() :
  ButtonPresser(),

  pointer(),
  accelerometer(),

  calib(0.0),
  calibVert(0.0),

  pressTaken(false),
  usedDirsLast(false)
{
  pointer.singlePointer = true;
  
  // apple tv doesn't have a way to use pointers
  if (RMSystem->get_OS_family() == APPLETV_FAMILY ||
      RMSystem->get_OS_family() == MAC_FAMILY)
  {
    usedDirsLast = true;
  }
}

void PizzaController::update()
{
  updateTypeUsedLast();
  
  pointer.update();
  accelerometer.update();
  gamepad.update();
  keyboard.update();

  pressTaken = false;
}

Point2 PizzaController::getXY()
{
  return cursorToScreen(pointer.getXY());
}

Point2 PizzaController::getOldXY()
{
  return cursorToScreen(pointer.getOldXY());
}

Point2 PizzaController::cursorToScreen(Point2 cursorXY)
{
  CameraStd cursorCam = Pizza::platformCam;

  // for ios devices where the native mouse coords don't match the pixel size of the screen
  cursorCam.zoom /= RMSystem->get_screen_scale();

  cursorXY = cursorCam.transformInv(cursorXY);
  //  cout << "out: " << cursorXY << endl;

  return cursorXY;
}


Logical PizzaController::getAnyConfirmStatus(Coord1 status)
{
  return getConfirmButtonStatus(status) == true ||
     (pointer.clickStatus(JUST_PRESSED) == true &&
       pressTaken == false);
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

Logical PizzaController::clickStatus(Coord1 type)
{
  return pressTaken == false && pointer.clickStatus(type);
}

void PizzaController::blockClicker()
{
  pressTaken = true;
}


// =============================== iCloudListener ============================ //

iCloudListener::iCloudListener() :
  SystemICloudListener()
{

}

void iCloudListener::read_icloud_finished(Logical success)
{
  cout << "iCloudListener tried to read, result = " << success << endl;

  if (success == true)
  {
    // RandomSimple randGen(0);
    // cout << "raw message: " << readCache << " length " << readCache.length << endl;
    // readCache.convert_from_es1(randGen);
    // cout << "unencrypted message: " << readCache << " length " << readCache.length << endl;
  }
}

void iCloudListener::write_icloud_finished(Logical success)
{
  cout << "iCloudListener tried to write, result = " << success << endl;
}

// =============================== IAPManager ============================ //

IAPManager::IAPManager() :
  SystemIAPListener(),

  gui(NULL)
{
  
}

void IAPManager::iap_purchased()
{
  // getSound(coinGetSnd)->play();
  // Skippy::gameUnlocked = true;
  // Skippy::savePlayerData();
  
  if (gui != NULL)
  {
    gui->interactionFinished();
  }
}

void IAPManager::iap_failed()
{
  if (gui != NULL)
  {
    gui->interactionFinished();
  }
}

// =============================== RMBoolItem ============================ //

void RMBoolItem::read(const String1& utf8Str)
{
  data = utf8Str.as_int() == 1;
}

void RMBoolItem::write(String1& output)
{
  output.int_string(data);
}

// =============================== RMIntItem ============================ //

void RMIntItem::read(const String1& utf8Str)
{
  data = utf8Str.as_int();
}

void RMIntItem::write(String1& output)
{
  output.int_string(data);
}


// =============================== RMFloatItem ============================ //

void RMFloatItem::read(const String1& utf8Str)
{
  data = utf8Str.as_double();
}

void RMFloatItem::write(String1& output)
{
  output.double_string(data, 8, 8);
}

// =============================== RMString2Item ============================ //

void RMString2Item::read(const String1& utf8Str)
{
  data.unicode_from_utf8(utf8Str);
}

void RMString2Item::write(String1& output)
{
  // TODO: unicode_to_utf8, have never needed this yet
  output = data;
}


// =============================== MapComparator ============================ //


Logical MapComparator::operator()(String1* left, String1* right) const
{
  Coord1 minLength = std::min(left->length, right->length);
  
  for (Coord1 i = 0; i < minLength; i++)
  {
    if (left->get(i) < right->get(i))
    {
      return true;
    }
    else if (left->get(i) > right->get(i))
    {
      return false;
    }
  }
  
  if (left->length < right->length)
  {
    return true;
  }
  
  return false;
}

// =============================== Pizza ============================ //

PlayerDataMap::PlayerDataMap() :
  cppMap(),
  strHelper(16)
{
  
}

PlayerDataMap::~PlayerDataMap()
{
  freeAll();
}


/*
void PlayerDataMap::deep_copy_from(PlayerDataMap& source)
{
  PizzaCppMap::iterator itr = source.cppMap.begin();
  
  while (itr != source.cppMap.end())
  {
    // std::cout << *(itr->first) << " :: " << *((Coord1*) itr->second->getData()) << std::endl;
    String1* newKey = new String1(*(itr->first));
    RMGenericItem* newValue = new RMGenericItem(*(itr->second));
    cout << "value is " << *((Coord1*) newValue->getData()) << endl;
    
    cppMap[newKey] = newValue;

    itr++;
  }
}
 */

void PlayerDataMap::initDefaultPlayer()
{
  addIntItem(FILE_TYPE_INT_KEY, 1);
  addBoolItem(MUSIC_BOOL_KEY, false);
  addBoolItem(SFX_BOOL_KEY, false);
  addStr2Item(CURRENT_VERSION_STR2_KEY, "1.0.0 (30)");
  addStr2Item(TRIED_RATE_VERSION_STR2_KEY, "never");
  addFloatItem(TOTAL_TIME_FLOAT_KEY, 0.0);
  addIntItem(COINS_KEY, 500);
  addIntItem(GEMS_KEY, 100);

  addIntItem(BASE_INGREDIENT_KEY, ING_BASE_CHEESE);
  addIntItem(EYES_INGREDIENT_KEY, ING_EYES_REGULAR);
  addIntItem(NOSE_INGREDIENT_KEY, ING_NONE);
  addIntItem(MOUTH_INGREDIENT_KEY, ING_MOUTH_REGULAR);
  addIntItem(ACCESSORY_INGREDIENT_KEY, ING_NONE);
  addIntItem(TOPPING_0_INGREDIENT_KEY, ING_TOPPING_PEPPERONI);
  addIntItem(TOPPING_1_INGREDIENT_KEY, ING_TOPPING_PEPPERONI);
  addIntItem(TOPPING_2_INGREDIENT_KEY, ING_TOPPING_PEPPERONI);
  addIntItem(SEASONING_0_KEY, ING_NONE);
  addIntItem(SEASONING_1_KEY, ING_NONE);

  addIntItem(BROS_1X_KEY, -1);
  addIntItem(BROS_1Y_KEY, -1);
  addIntItem(BROS_2X_KEY, -1);
  addIntItem(BROS_2Y_KEY, -1);

  addIntItem(LAST_PLAYED_LEVEL_KEY, 0);

  String1 nameHelper(16);
  
  for (Coord1 i = 0; i < levelList.count; ++i)
  {
    addFloatItem(levelStr(i, L_TIME_KEY), 999.0);
    addBoolItem(levelStr(i, L_FIRST_STAR_KEY), false);
    addBoolItem(levelStr(i, L_SECOND_STAR_KEY), false);
    addBoolItem(levelStr(i, L_THIRD_STAR_KEY), false);
    addIntItem(levelStr(i, L_TOTAL_SCORE_KEY), 0);
    addBoolItem(levelStr(i, L_LOCKED_KEY), (i == 0 ? false : true));
  }
  
  for (Coord1 i = 0; i < NUM_INGREDIENTS; ++i)
  {
    nameHelper.replace('$',  + i);
    
    Logical owned = false;
    
    if (i == ING_NONE ||
        i == ING_BASE_CHEESE ||
        i == ING_TOPPING_PEPPERONI ||
        i == ING_EYES_REGULAR ||
        i == ING_MOUTH_REGULAR)
    {
      owned = true;
    }
    
    addBoolItem(ingredientStr(i, I_UNLOCKED_KEY), true);
    addBoolItem(ingredientStr(i, I_OWNED_KEY), owned);
  }
}

void PlayerDataMap::addBoolItem(const String1& key, Logical defaultData)
{
  String1 keyCopy;
  PizzaCppMap::iterator searchItr = cppMap.find(&keyCopy);
  
  if (searchItr != cppMap.end())
  {
    // node already exists
    cout << "Trying to add key " << key << " which already exists" << endl;
    rmassert(false);
    return;
  }
  
  cppMap[new String1(key)] = new RMBoolItem(defaultData);
}

void PlayerDataMap::addIntItem(const String1& key, Coord1 defaultData)
{
  String1 keyCopy;
  PizzaCppMap::iterator searchItr = cppMap.find(&keyCopy);
  
  if (searchItr != cppMap.end())
  {
    // node already exists
    cout << "Trying to add key " << key << " which already exists" << endl;
    rmassert(false);
    return;
  }
  
  cppMap[new String1(key)] = new RMIntItem(defaultData);
}

void PlayerDataMap::addFloatItem(const String1& key, Point1 defaultData)
{
  String1 keyCopy;
  PizzaCppMap::iterator searchItr = cppMap.find(&keyCopy);
  
  if (searchItr != cppMap.end())
  {
    // node already exists
    cout << "Trying to add key " << key << " which already exists" << endl;
    rmassert(false);
    return;
  }
  
  cppMap[new String1(key)] = new RMFloatItem(defaultData);
}

void PlayerDataMap::addStr2Item(const String1& key, const Char* defaultData)
{
  String1 keyCopy;
  PizzaCppMap::iterator searchItr = cppMap.find(&keyCopy);
  
  if (searchItr != cppMap.end())
  {
    // node already exists
    cout << "Trying to add key " << key << " which already exists" << endl;
    rmassert(false);
    return;
  }
  
  cppMap[new String1(key)] = new RMString2Item(defaultData);
}

RMGenericItem* PlayerDataMap::getItem(const Char* key)
{
  strHelper = key;
  
  PizzaCppMap::iterator searchItr = cppMap.find(&strHelper);
  
  if (searchItr != cppMap.end())
  {
    // found it
    return searchItr->second;
  }
  
  return NULL;
}

Logical& PlayerDataMap::getBoolItem(const Char* key)
{
  RMGenericItem* item = getItem(key);
  rmassert(item->type == BOOL_TYPE);
  return *reinterpret_cast<Logical*>(item->getData());
}

Coord1& PlayerDataMap::getIntItem(const Char* key)
{
  RMGenericItem* item = getItem(key);
  rmassert(item->type == INT_TYPE);
  return *reinterpret_cast<Coord1*>(item->getData());
}

Point1& PlayerDataMap::getFloatItem(const Char* key)
{
  RMGenericItem* item = getItem(key);
  rmassert(item->type == FLOAT_TYPE);
  return *reinterpret_cast<Point1*>(item->getData());
}

String2& PlayerDataMap::getString2Item(const Char* key)
{
  RMGenericItem* item = getItem(key);
  rmassert(item->type == STRING2_TYPE);
  return *reinterpret_cast<String2*>(item->getData());
}

Point1& PlayerDataMap::getLevelFloat(Coord1 levelIndex, const String1& keyPrefix)
{
  return getFloatItem(levelStr(levelIndex, keyPrefix).as_cstr());
}

Logical& PlayerDataMap::getLevelBool(Coord1 levelIndex, const String1& keyPrefix)
{
  return getBoolItem(levelStr(levelIndex, keyPrefix).as_cstr());
}

Coord1& PlayerDataMap::getLevelInt(Coord1 levelIndex, const String1& keyPrefix)
{
  return getIntItem(levelStr(levelIndex, keyPrefix).as_cstr());
}

Logical& PlayerDataMap::getIngredientBool(Coord1 ingIndex, const String1& keyPrefix)
{
  return getBoolItem(ingredientStr(ingIndex, keyPrefix).as_cstr());
}

String1& PlayerDataMap::levelStr(Coord1 levelIndex, const String1& dataKey)
{
  strHelper = LEVEL_PREFIX_KEY;
  strHelper += '$';
  strHelper.int_replace('$', levelIndex);
  strHelper += "_";
  strHelper += dataKey;
  return strHelper;
}

String1& PlayerDataMap::ingredientStr(Coord1 ingIndex, const String1& dataKey)
{
  strHelper = INGREDIENT_PREFIX_KEY;
  strHelper += '$';
  strHelper.int_replace('$', ingIndex);
  strHelper += "_";
  strHelper += dataKey;
  return strHelper;
}

Coord1 PlayerDataMap::countTotalStars()
{
  Coord1 result = 0;
  
  for (Coord1 i = 0; i < levelList.count; ++i)
  {
    result += getBoolItem(levelStr(i, L_FIRST_STAR_KEY).as_cstr()) == true ? 1 : 0;
    result += getBoolItem(levelStr(i, L_SECOND_STAR_KEY).as_cstr()) == true ? 1 : 0;
    result += getBoolItem(levelStr(i, L_THIRD_STAR_KEY).as_cstr()) == true ? 1 : 0;
  }
  
  return result;
}

Coord1& PlayerDataMap::getCurrency(Coord1 type)
{
  switch (type)
  {
    default:
    case CURRENCY_GOLD: return getIntItem(COINS_KEY);
    case CURRENCY_KEYS: return getIntItem(GEMS_KEY);
  }
}

Coord1& PlayerDataMap::getIngLocation(Coord1 locationID, Coord1 subID)
{
  switch (locationID)
  {
    default:
    case LOCATION_BASE: return getIntItem(BASE_INGREDIENT_KEY);
    case LOCATION_EYES: return getIntItem(EYES_INGREDIENT_KEY);
    case LOCATION_NOSE: return getIntItem(NOSE_INGREDIENT_KEY);
    case LOCATION_MOUTH: return getIntItem(MOUTH_INGREDIENT_KEY);
    case LOCATION_ACCESSORY: return getIntItem(ACCESSORY_INGREDIENT_KEY);

    case LOCATION_TOPPING:
      switch (subID)
      {
        default:
        case 0: return getIntItem(TOPPING_0_INGREDIENT_KEY);
        case 1: return getIntItem(TOPPING_1_INGREDIENT_KEY);
        case 2: return getIntItem(TOPPING_2_INGREDIENT_KEY);
      }
      
    case LOCATION_SEASONING:
      switch (subID)
      {
        default:
        case 0: return getIntItem(SEASONING_0_KEY);
        case 1: return getIntItem(SEASONING_1_KEY);
      }
  }
}

void PlayerDataMap::write(String1& output, Logical onlyChanged)
{
  output.clear();
  output.ensure_capacity(1024);
  
  String1 defaultValueStr(16);
  String1 myValueStr(16);
  
  PizzaCppMap::iterator itr = cppMap.begin();
  
  while (itr != cppMap.end())
  {
    itr->second->write(myValueStr);
    
    PizzaCppMap::iterator searchItr = defaultPlayer->cppMap.find(itr->first);
    
    // found the default key, compare the values
    if (searchItr != defaultPlayer->cppMap.end())
    {
      searchItr->second->write(defaultValueStr);
      
      if (onlyChanged == false ||
          defaultValueStr != myValueStr)
      {
        output += '\"';
        output += *(itr->first);
        output += "\" = \"";
        output += myValueStr;
        output += "\";\n";
      }
    }
    
    // remove the trailing new line
    itr++;
  }

  output.remove_last();
}

void PlayerDataMap::read(const String1& input)
{
  ArrayList<String1> tempLines;
  input.split(tempLines, '\n');
  String1 valueStr(32);
  
  for (Coord1 i = 0; i < tempLines.count; ++i)
  {
    Coord1 startIndex = 1;
    Coord1 endIndex = tempLines[i].index_of("=") - 3;

    String1* keyStr = new String1();
    keyStr->as_substring(tempLines[i], startIndex, endIndex);
    
    startIndex = tempLines[i].index_of("=") + 3;
    endIndex = tempLines[i].last_i() - 2;
    
    valueStr.as_substring(tempLines[i], startIndex, endIndex);
    
    // use the default value object to generate the new object
    PizzaCppMap::iterator searchItr = defaultPlayer->cppMap.find(keyStr);
    
    // found the default definition
    if (searchItr != defaultPlayer->cppMap.end())
    {
      RMGenericItem* defaultValue = searchItr->second;
      
      RMGenericItem* newValue = defaultValue->createNew();
      newValue->read(valueStr);
      cppMap[keyStr] = newValue;
    }
    // couldn't find the key definition in the default player.
    // ignore this pair.
    else
    {
      delete keyStr;
    }
  }
}

void PlayerDataMap::freeAll()
{
  cppMap.clear();
}

// =============================== LevelData ============================ //

LevelData::LevelData() :
  index(0),
  localizedName("Level_A"),

  modeData(LEVEL_TYPE_GLADIATOR, 0),
  region(GRAVEYARD_BG),

  unlockRequirement(0),

  ingredientDefIndices(4)
{
  
}

Logical LevelData::isLocked()
{
  return activePlayer->getBoolItem(activePlayer->levelStr(index, L_LOCKED_KEY).as_cstr()) == true;
}

void LevelData::unlockForActivePlayer()
{
  activePlayer->getBoolItem(activePlayer->levelStr(index, L_LOCKED_KEY).as_cstr()) = false;
}

// =============================== TempGameplayData ============================ //

TempGameplayData::TempGameplayData() :
  currLevelIndex(0),

  currTime(0.0),
  maxPlayerHP(5.0),
  currPlayerHP(5.0),

  collectedCoins(LAST_GEM_TYPE + 1, true),

  objectsIgnited(0),
  objectsExtinguished(0)
{
  collectedCoins.set_all(0);
}

void TempGameplayData::addHP(Point1 value)
{
  currPlayerHP += value;
  currPlayerHP = std::min(currPlayerHP, maxPlayerHP);
}

// =============================== IngredientData ============================ //

IngredientData::IngredientData() :
  index(-1),
  baseType(-1),
  location(LOCATION_BASE),
  upgradeLevel(0),
  price(CURRENCY_GOLD, 0),

  localizedName("Cheese"),

  dataVals(0.0, 0.0, 0.0, 0.0)
{
  
}

Logical IngredientData::playerUnlocked()
{
  return activePlayer->getIngredientBool(index, I_UNLOCKED_KEY);
}

Logical IngredientData::playerBought()
{
  return activePlayer->getIngredientBool(index, I_OWNED_KEY);
}

Logical IngredientData::playerCanAfford()
{
  return activePlayer->getCurrency(price.x) >= price.y;
}


// =============================== Pizza ============================ //

namespace Pizza 
{
  iCloudListener cloudResponder;
  GamesInterstitialManagerStd* moreGamesMan = NULL;
  IAPManager* iapMan = NULL;

  CameraStd
  platformCam;

  Logical gcLogonShown = false,

  DRAW_DEBUG = false,
  DRAW_DEBUG_PHYS = false,
  DEBUG_INVINCIBLE = false,
  DEBUG_KEYSTROKES = false,
  DEBUG_MAP_PATHING = false,
  DEBUG_IGNORE_SAVING = false,
  DEBUG_IGNORE_LOADING = false;

  Coord1 DEBUG_LEVEL = -1;

  void initGameConstants()
  {
    RivermanSystem::fallbackIsEmptyStyle = false;
    PHYSICS_SCALE = 200.0;

    processStrings();
    setLevelData();
    setIngredientData();
    
    iapMan = new IAPManager();

    controller = new PizzaController();
    
    defaultPlayer = new PlayerDataMap();
    defaultPlayer->initDefaultPlayer();

    activePlayer = new PlayerDataMap();
    activePlayer->initDefaultPlayer();
    loadPlayerData();
    
    // this has to come AFTER the active player is loaded so it can read from
    activeGameplayData = new TempGameplayData();
    
    offscreenBuffer = RMGraphics->create_render_target(RMSystem->display_size());
    // cout << "display size " << RMSystem->display_size() << " safe box " << RMSystem->display_safezone() << endl;
    
    /*
    cout << "real size " << offscreenBuffer->real_pixel_size() << endl;
    
    String1 masterStr;
    activePlayer->getBoolItem(activePlayer->levelStr(15, L_LOCKED_KEY).as_cstr()) = true;
    activePlayer->getBoolItem(activePlayer->levelStr(5, L_LOCKED_KEY).as_cstr()) = true;
    activePlayer->write(masterStr);
    cout << masterStr << endl;
     */
    
    // TODO: for some reason it's not returning the right value when i try to access
    // using the [] method unless I set the value here

    /*
    PizzaCppMap::iterator itr = newPlayer->cppMap.begin();
    
    while (itr != newPlayer->cppMap.end())
    {
      String1 valueStr;
      itr->second->write(valueStr);
      String1 helper(FILE_TYPE_INT_KEY);
      
      if (*itr->first == helper)
      {
        cout << "key: " << (*(itr->first)) << " value: " << valueStr << " data ptr " << itr->second->getData()
          << " dereference " << *((Coord1*) itr->second->getData()) << endl;
      }
      
      itr++;
    }
     */

    // in case it draws before the first update or anything needs the camera for calculations
    // platformCam.syncAsScreenCam(Point2(480.0, 320.0), RM_WH, CameraStd::NEVER_LETTERBOX);
    platformCam.syncAsScreenCam(SAFE_WH, RM_WH, CameraStd::NEVER_LETTERBOX);
    // cout << "plat cam " << platformCam << endl;

    ResourceManager::imgSizeSuffix = String1("_$x", '$', RMSystem->desired_texture_scale());
  }

  void loadPlayerData()
  {
    if (DEBUG_IGNORE_LOADING == true)
    {
      return;
    }
    
    /*
    String1 masterStr;
    RMSystem->read_file(PLAYER_FILENAME, masterStr);

    // loading from free version in handled within this
    loadPlayerFromStr(masterStr, LOAD_FROM_LOCAL);

    if (Kung::isFreeVersion() == false)
    {
      // this makes it so you can always reinstall the game and play fresh
      // as long as you don't play the lite version in between
      RMSystem->write_pasteboard(PLAYER_PASTEBOARD_LITE, "");
    }
     */
    
    String1 masterStr;
    RMSystem->read_file(PLAYER_FILENAME, masterStr);
    
    activePlayer->read(masterStr);
  }

  void savePlayerData()
  {
    if (DEBUG_IGNORE_SAVING == true)
    {
      return;
    }
    
    String1 masterStr;

    activePlayer->write(masterStr);
    RMSystem->write_file(PLAYER_FILENAME, masterStr);
  }
  
  void resetAndSavePlayer()
  {
    delete activePlayer;
    activePlayer = new PlayerDataMap();
    activePlayer->initDefaultPlayer();
    savePlayerData();
  }

  void resetGame()
  {
    *activeGameplayData = TempGameplayData();
    activeGameplayData->maxPlayerHP = 5.0 + getEquippedSeasoningLevel(SEAS_HP_0);
    activeGameplayData->currPlayerHP = activeGameplayData->maxPlayerHP;
  }

  void gotAchievement(Coord1 val)
  {
    RMSystem->gamecenter_achievement(val, 100.0, true);
  }

  void processStrings()
  {
    // NOTE: these match the indices of RM::LanguageID
    ArrayList<String1> supportedSet;
    supportedSet.add("en");
    supportedSet.add("fr");
    supportedSet.add("it");
    supportedSet.add("de");
    supportedSet.add("es");

    LANGUAGE_CODE = supportedSet[RM::pick_language_std(supportedSet)];

    langSuffix = "_";
    langSuffix += LANGUAGE_CODE;

    String1 stringFilename("pizza_strings");
    stringFilename += langSuffix;
    stringFilename += ".txt";

    RMSystem->read_utf8("pizza_efigs_strip.txt", EFIGS_CHARS);
    RMSystem->read_utf8("pizza_ascii_strip.txt", ASCII_CHARS);

    RMSystem->read_utf8(stringFilename, MASTER_LSTRING);
    Str::kvsToLines(MASTER_LSTRING, LSTRINGS);
  }
  
  void setLevelData()
  {
    // 0
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_UNARMED);  // 0 START GLADIATOR MAIN BLOCK
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPEARMEN_SIDE);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPEARMEN_VERT);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_BOMBBAT_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_BOMBBAT_2);
    
    // 5
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPIDER_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPIDER_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CAVEMAN_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CAVEMAN_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_ENT_1);
    
    // 10
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_ENT_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_DONUT_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_DONUT_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_UTENSIL_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_UTENSIL_2);
    
    // 15
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_GIANT_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_GIANT_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_DOZER_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_DOZER_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_WISP_1);
    
    // 20
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_WISP_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CHIP_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CHIP_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SCIENTIST_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SCIENTIST_2);
    
    // 25
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_ALIEN_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_ALIEN_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CUPID_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_CUPID_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPINY_1);
    
    // 30
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_SPINY_2);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_REAPER_1);
    addLevelData(LEVEL_TYPE_GLADIATOR, GL_LEVEL_REAPER_2);
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_GRAVEYARD); // 33 START SUMO MAIN BLOCK
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_JURASSIC);
    
    // 35
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_CUPCAKE);
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_DUMP);
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_EDISON);
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_FUNGUS);
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_EMPEROR);
    
    // 40
    addLevelData(LEVEL_TYPE_SUMO, SUMO_LEVEL_SNOWBALL);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_MOHAWK);  // 41 START PLANE MAIN BLOCK
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_CHICKEN);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_BOMBBAT);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_SPIDER);
    
    // 45
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_DONUT);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_FLAMESKULL);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_SPIDER_2);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_FLAMESKULL_2);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_CHIP);
    
    // 50
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_DONUT_2);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_WISP);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_CHIP_2);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_CUPID);
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_WISP_2);
    
    // 55
    addLevelData(LEVEL_TYPE_PLANE, PL_LEVEL_CUPID_2);
    addParlorData(PARLOR_LEVEL_GRAVEYARD_1,
                  ING_TOPPING_MUSHROOM, ING_ACC_TOUPEE, SEAS_HP_0);  // 56 start toad houses
    addParlorData(PARLOR_LEVEL_GRAVEYARD_2,
                  ING_BASE_PESTO, SEAS_INVINCIBILITY_0, SEAS_MOVE_SPEED_1);
    addParlorData(PARLOR_LEVEL_GRAVEYARD_3,
                  ING_TOPPING_BACON, ING_MOUTH_HUMAN, SEAS_STOMPER_0);
    addParlorData(PARLOR_LEVEL_GRAVEYARD_4,
                  ING_NOSE_CAT, SEAS_JUMP_HEIGHT_0, SEAS_ACCEL_2);

    // 60
    addParlorData(PARLOR_LEVEL_JURASSIC_1,
                  ING_MOUTH_DOG, ING_EYES_CYCLOPS, SEAS_FLOATY_0);
    addParlorData(PARLOR_LEVEL_JURASSIC_2,
                  ING_ACC_BEANIE, ING_EYES_HUMAN, SEAS_BANKER_0);
    addParlorData(PARLOR_LEVEL_JURASSIC_3,
                  ING_EYES_DOG, SEAS_SHIELD_0, SEAS_DRAGON_0);
    addParlorData(PARLOR_LEVEL_JURASSIC_4,
                  ING_NOSE_RHINO, ING_MOUTH_DEVIL, SEAS_MOVE_SPEED_0);
    addParlorData(PARLOR_LEVEL_BAKERY_1,
                  ING_EYES_COAL, SEAS_HEAL_0, SEAS_EMPEROR_0);
    
    // 65
    addParlorData(PARLOR_LEVEL_BAKERY_2,
                  ING_BASE_WOOD, ING_NOSE_HUMAN, SEAS_DOUBLE_JUMP_0);
    addParlorData(PARLOR_LEVEL_BAKERY_3,
                  ING_TOPPING_PINEAPPLE, SEAS_ACCEL_0, SEAS_HEAL_2);
    addParlorData(PARLOR_LEVEL_BAKERY_4,
                  ING_TOPPING_ANCHOVIES, ING_EYES_GOOGLEY, SEAS_FLOATY_1);
    addParlorData(PARLOR_LEVEL_DUMP_1,
                  ING_BASE_ZEBRA, ING_MOUTH_VAMPIRE, SEAS_ACCEL_1);
    addParlorData(PARLOR_LEVEL_DUMP_2,
                  ING_ACC_DOG_EARS, SEAS_BANKER_1, SEAS_DOUBLE_JUMP_2);
    
    // 70
    addParlorData(PARLOR_LEVEL_DUMP_3,
                  ING_NOSE_DOG, ING_ACC_DREADLOCKS, SEAS_HP_1);
    addParlorData(PARLOR_LEVEL_DUMP_4,
                  ING_TOPPING_SAUSAGE, SEAS_INVINCIBILITY_1, SEAS_SHIELD_2);
    addParlorData(PARLOR_LEVEL_EDISON_1,
                  ING_ACC_CAT_EARS, ING_BASE_UFO, SEAS_DOUBLE_JUMP_1);
    addParlorData(PARLOR_LEVEL_EDISON_2,
                  ING_BASE_SNOWCONE, ING_NOSE_ELEPHANT, SEAS_DRAGON_1);
    addParlorData(PARLOR_LEVEL_EDISON_3,
                  ING_BASE_MEATBALL, SEAS_STOMPER_1, SEAS_STOMPER_2);
    
    // 75
    addParlorData(PARLOR_LEVEL_EDISON_4,
                  ING_EYES_CAT, ING_TOPPING_BUGS, SEAS_HP_2);
    addParlorData(PARLOR_LEVEL_BODY_1,
                  ING_TOPPING_PEPPERS, ING_ACC_PROPELLOR, SEAS_EMPEROR_1);
    addParlorData(PARLOR_LEVEL_BODY_2,
                  ING_TOPPING_PAPER_CLIPS, SEAS_INVINCIBILITY_2, SEAS_MOVE_SPEED_2);
    addParlorData(PARLOR_LEVEL_BODY_3,
                  ING_TOPPING_GUMMY_WORMS, ING_TOPPING_XMAS_LIGHTS, SEAS_BANKER_2);
    addParlorData(PARLOR_LEVEL_BODY_4,
                  ING_TOPPING_OLIVES, SEAS_HEAL_1, SEAS_DRAGON_2);
    
    // 80
    addParlorData(PARLOR_LEVEL_SKULL_1,
                  ING_ACC_WOMENS_MARCH, ING_ACC_TRIHAWK, SEAS_JUMP_HEIGHT_1);
    addParlorData(PARLOR_LEVEL_SKULL_2,
                  ING_BASE_SOCCER, ING_EYES_DEVIL, SEAS_JUMP_HEIGHT_2);
    addParlorData(PARLOR_LEVEL_SKULL_3,
                  ING_NOSE_CARROT, SEAS_FLOATY_2, SEAS_EMPEROR_2);
    addParlorData(PARLOR_LEVEL_SKULL_4,
                  ING_MOUTH_CAT, ING_ACC_WEDDING_DRESS, SEAS_BANKER_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_GRAVEYARD_1);  // 84 start gates
    
    // 85
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_GRAVEYARD_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_GRAVEYARD_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_GRAVEYARD_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_GRAVEYARD_5);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_JURASSIC_1);
    
    // 90
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_JURASSIC_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_JURASSIC_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_JURASSIC_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_JURASSIC_5);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BAKERY_1);
    
    // 95
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BAKERY_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BAKERY_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BAKERY_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BAKERY_5);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_1);
    
    // 100
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_5);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_DUMP_6);
    
    // 105
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_1);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_5);
    
    // 110
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_6);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_7);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_EDISON_8);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_1);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_2);
    
    // 115
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_5);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_6);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_BODY_7);
    
    // 120
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_1);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_2);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_3);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_4);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_5);
    
    // 125
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_6);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_7);
    addLevelData(LEVEL_TYPE_GATE, GATE_LEVEL_SKULL_8);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_EASY); // 128 start lake levels
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_LOBSTER);

    // 130
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_LOBSTER_2);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_PEARL);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_JELLYFISH);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_JELLYFISH_2);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_BARREL);

    // 135
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_BARREL_2);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_URCHIN);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_SPINECLAM);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_SPINECLAM_2);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_SWORDFISH);

    // 140
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_STINGRAY);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_SHARK);
    addLevelData(LEVEL_TYPE_LAKE, LAKE_LEVEL_SHARK_2);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_0);  // 143 start angry
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_1);

    // 145
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_2);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_3);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_4);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_5);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_6);

    // 150
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_7);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_8);
    addLevelData(LEVEL_TYPE_ANGRY, AN_LEVEL_9);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_0);  // 153 start sprint
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_1);

    // 155
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_2);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_3);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_4);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_5);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_6);

    // 160
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_7);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_8);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_0);  // 162 start puppy
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_1);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_2);

    // 165
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_3);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_4);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_5);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_6);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_7);

    // 170
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_8);
    addLevelData(LEVEL_TYPE_PUPPY, PU_LEVEL_9);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_0);  // 172 start pirate
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_1);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_2);

    // 175
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_3);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_4);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_5);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_6);
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_7);

    // 180
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_8);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_0);  // 181 start fireman
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_1);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_2);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_3);

    // 185
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_4);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_5);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_6);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_7);
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_8);
    
    // 190
    addLevelData(LEVEL_TYPE_FIREMAN, FI_LEVEL_9);
    addLevelData(LEVEL_TYPE_SPRINT, SP_LEVEL_9);  // another sprint level
    addLevelData(LEVEL_TYPE_PIRATE, PI_LEVEL_9);   // another pirate level
  }
  
  void addLevelData(Coord1 modeType, Coord1 modeLevel)
  {
    levelList.add(LevelData());
    levelList.last().index = levelList.last_i();
    levelList.last().localizedName = String2("Level $", '$', levelList.last().index);
    levelList.last().modeData.set(modeType, modeLevel);
  }
  
  void addParlorData(Coord1 parlorID, Coord1 ing1, Coord1 ing2, Coord1 ing3)
  {
    addLevelData(LEVEL_TYPE_PPARLOR, parlorID);
    levelList.last().ingredientDefIndices.add(ing1);
    levelList.last().ingredientDefIndices.add(ing2);
    levelList.last().ingredientDefIndices.add(ing3);
  }
  
  void setIngredientData()
  {
    numSelectableIngs.ensure_count(NUM_INGREDIENT_LOCATIONS);
    numSelectableIngs.set_all(1);
    numSelectableIngs[LOCATION_TOPPING] = 3;
    numSelectableIngs[LOCATION_SEASONING] = 2;

    Coord1 currStrID = 3;
    
    // DO NOT CHANGE THE ORDER, add to the end
    addIngredientData(ING_NONE, LOCATION_BASE, 101);  // placeholder
    
    // crust
    addIngredientData(ING_BASE_CHEESE, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_PESTO, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_WOOD, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_MEATBALL, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_UFO, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_ZEBRA, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_SNOWCONE, LOCATION_BASE, currStrID++);
    addIngredientData(ING_BASE_SOCCER, LOCATION_BASE, currStrID++);
    
    // topping
    addIngredientData(ING_TOPPING_PEPPERONI, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_MUSHROOM, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_PEPPERS, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_OLIVES, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_PINEAPPLE, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_SAUSAGE, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_BACON, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_ANCHOVIES, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_BUGS, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_GUMMY_WORMS, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_PAPER_CLIPS, LOCATION_TOPPING, currStrID++);
    addIngredientData(ING_TOPPING_XMAS_LIGHTS, LOCATION_TOPPING, currStrID++);
    
    // eyes
    addIngredientData(ING_EYES_REGULAR, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_GOOGLEY, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_COAL, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_HUMAN, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_DEVIL, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_DOG, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_CAT, LOCATION_EYES, currStrID++);
    addIngredientData(ING_EYES_CYCLOPS, LOCATION_EYES, currStrID++);
    
    // mouth
    addIngredientData(ING_MOUTH_REGULAR, LOCATION_MOUTH, currStrID++);
    addIngredientData(ING_MOUTH_VAMPIRE, LOCATION_MOUTH, currStrID++);
    addIngredientData(ING_MOUTH_HUMAN, LOCATION_MOUTH, currStrID++);
    addIngredientData(ING_MOUTH_DOG, LOCATION_MOUTH, currStrID++);
    addIngredientData(ING_MOUTH_CAT, LOCATION_MOUTH, currStrID++);
    addIngredientData(ING_MOUTH_DEVIL, LOCATION_MOUTH, currStrID++);
    
    // nose
    currStrID = 85;
    addIngredientData(ING_NOSE_DOG, LOCATION_NOSE, currStrID++);
    addIngredientData(ING_NOSE_CAT, LOCATION_NOSE, currStrID++);
    addIngredientData(ING_NOSE_HUMAN, LOCATION_NOSE, currStrID++);
    addIngredientData(ING_NOSE_RHINO, LOCATION_NOSE, currStrID++);
    addIngredientData(ING_NOSE_CARROT, LOCATION_NOSE, currStrID++);
    addIngredientData(ING_NOSE_ELEPHANT, LOCATION_NOSE, currStrID++);
    
    // accessories
    currStrID = 37;
    addIngredientData(ING_ACC_BEANIE, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_PROPELLOR, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_WOMENS_MARCH, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_DOG_EARS, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_CAT_EARS, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_TOUPEE, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_WEDDING_DRESS, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_DREADLOCKS, LOCATION_ACCESSORY, currStrID++);
    addIngredientData(ING_ACC_TRIHAWK, LOCATION_ACCESSORY, currStrID++);
    
    // seasonings
    addSeasoningData(SEAS_HP_0, SEAS_HP_0, 0, currStrID++);
    addSeasoningData(SEAS_HP_1, SEAS_HP_0, 1, currStrID++);
    addSeasoningData(SEAS_HP_2, SEAS_HP_0, 2, currStrID++);
    addSeasoningData(SEAS_FLOATY_0, SEAS_FLOATY_0, 0, currStrID++);
    addSeasoningData(SEAS_FLOATY_1, SEAS_FLOATY_0, 1, currStrID++);
    addSeasoningData(SEAS_FLOATY_2, SEAS_FLOATY_0, 2, currStrID++);
    addSeasoningData(SEAS_MOVE_SPEED_0, SEAS_MOVE_SPEED_0, 0, currStrID++);
    addSeasoningData(SEAS_MOVE_SPEED_1, SEAS_MOVE_SPEED_0, 1, currStrID++);
    addSeasoningData(SEAS_MOVE_SPEED_2, SEAS_MOVE_SPEED_0, 2, currStrID++);
    addSeasoningData(SEAS_ACCEL_0, SEAS_ACCEL_0, 0, currStrID++);
    addSeasoningData(SEAS_ACCEL_1, SEAS_ACCEL_0, 1, currStrID++);
    addSeasoningData(SEAS_ACCEL_2, SEAS_ACCEL_0, 2, currStrID++);
    addSeasoningData(SEAS_INVINCIBILITY_0, SEAS_INVINCIBILITY_0, 0, currStrID++);
    addSeasoningData(SEAS_INVINCIBILITY_1, SEAS_INVINCIBILITY_0, 1, currStrID++);
    addSeasoningData(SEAS_INVINCIBILITY_2, SEAS_INVINCIBILITY_0, 2, currStrID++);
    addSeasoningData(SEAS_DOUBLE_JUMP_0, SEAS_DOUBLE_JUMP_0, 0, currStrID++);
    addSeasoningData(SEAS_DOUBLE_JUMP_1, SEAS_DOUBLE_JUMP_0, 1, currStrID++);
    addSeasoningData(SEAS_DOUBLE_JUMP_2, SEAS_DOUBLE_JUMP_0, 2, currStrID++);
    addSeasoningData(SEAS_JUMP_HEIGHT_0, SEAS_JUMP_HEIGHT_0, 0, currStrID++);
    addSeasoningData(SEAS_JUMP_HEIGHT_1, SEAS_JUMP_HEIGHT_0, 1, currStrID++);
    addSeasoningData(SEAS_JUMP_HEIGHT_2, SEAS_JUMP_HEIGHT_0, 2, currStrID++);
    addSeasoningData(SEAS_STOMPER_0, SEAS_STOMPER_0, 0, currStrID++);
    addSeasoningData(SEAS_STOMPER_1, SEAS_STOMPER_0, 1, currStrID++);
    addSeasoningData(SEAS_STOMPER_2, SEAS_STOMPER_0, 2, currStrID++);
    addSeasoningData(SEAS_HEAL_0, SEAS_HEAL_0, 0, currStrID++);
    addSeasoningData(SEAS_HEAL_1, SEAS_HEAL_0, 1, currStrID++);
    addSeasoningData(SEAS_HEAL_2, SEAS_HEAL_0, 2, currStrID++);
    addSeasoningData(SEAS_SHIELD_0, SEAS_SHIELD_0, 0, currStrID++);
    addSeasoningData(SEAS_SHIELD_1, SEAS_SHIELD_0, 1, currStrID++);
    addSeasoningData(SEAS_SHIELD_2, SEAS_SHIELD_0, 2, currStrID++);
    addSeasoningData(SEAS_BANKER_0, SEAS_BANKER_0, 0, currStrID++);
    addSeasoningData(SEAS_BANKER_1, SEAS_BANKER_0, 1, currStrID++);
    addSeasoningData(SEAS_BANKER_2, SEAS_BANKER_0, 2, currStrID++);
    addSeasoningData(SEAS_DRAGON_0, SEAS_DRAGON_0, 0, currStrID++);
    addSeasoningData(SEAS_DRAGON_1, SEAS_DRAGON_0, 1, currStrID++);
    addSeasoningData(SEAS_DRAGON_2, SEAS_DRAGON_0, 2, currStrID++);
    addSeasoningData(SEAS_EMPEROR_0, SEAS_EMPEROR_0, 0, currStrID++);
    addSeasoningData(SEAS_EMPEROR_1, SEAS_EMPEROR_0, 1, currStrID++);
    addSeasoningData(SEAS_EMPEROR_2, SEAS_EMPEROR_0, 2, currStrID++);

    /*
    addIngredientData(ING_NONE, LOCATION_EYES, 101);
    addIngredientData(ING_NONE, LOCATION_NOSE, 101);
    addIngredientData(ING_NONE, LOCATION_MOUTH, 101);
    addIngredientData(ING_NONE, LOCATION_ACCESSORY, 101);
    addIngredientData(ING_NONE, LOCATION_TOPPING, 101);
    addIngredientData(ING_NONE, LOCATION_SEASONING, 101);
     */

    // add any new ones to the here
  }
  
  void addIngredientData(Coord1 setType, Coord1 setLocation, Coord1 strIndex)
  {
    ingredientList.add(IngredientData());
    ingredientList.last().index = ingredientList.last_i();
    ingredientList.last().location = setLocation;
    ingredientList.last().price = Coord2(ingredientList.last_i() % 2 == 0 ? CURRENCY_GOLD : CURRENCY_KEYS,
                                         ingredientList.last().index * 10);
    ingredientList.last().localizedName = getStr(strIndex);
  }

  void addSeasoningData(Coord1 setType, Coord1 setBaseType, Coord1 setUpgradeLevel, Coord1 strIndex)
  {
    ingredientList.add(IngredientData());
    ingredientList.last().index = ingredientList.last_i();
    ingredientList.last().baseType = setBaseType;
    ingredientList.last().location = LOCATION_SEASONING;
    ingredientList.last().upgradeLevel = setUpgradeLevel;
    ingredientList.last().price = Coord2(ingredientList.last_i() % 2 == 0 ? CURRENCY_GOLD : CURRENCY_KEYS,
                                         ingredientList.last().index * 10);
    ingredientList.last().localizedName = getStr(strIndex);
    ingredientList.last().dataVals = Point4(0.0, 0.0, 0.0, 0.0);
  }
  
  Coord1 getEquippedSeasoningLevel(Coord1 baseIngredientID)
  {
    Coord2 equippedSeasonings(-1, -1);
    equippedSeasonings.x = activePlayer->getIntItem(SEASONING_0_KEY);
    equippedSeasonings.y = activePlayer->getIntItem(SEASONING_1_KEY);
    
    Coord1 baseIngredientType1 = ingredientList[equippedSeasonings.x].baseType;
    Coord1 baseIngredientType2 = ingredientList[equippedSeasonings.y].baseType;
    
    Coord1 result = 0;
    
    // both equipped match the target type, use the larger of the two
    if (baseIngredientType1 == baseIngredientID &&
        baseIngredientType2 == baseIngredientID)
    {
      result = std::max(ingredientList[equippedSeasonings.x].upgradeLevel,
                        ingredientList[equippedSeasonings.y].upgradeLevel) + 1;
    }
    // only seas1 matches the target type
    else if (baseIngredientType1 == baseIngredientID)
    {
      result = ingredientList[equippedSeasonings.x].upgradeLevel + 1;
    }
    // only seas2 matches the target type
    else if (baseIngredientType2 == baseIngredientID)
    {
      result = ingredientList[equippedSeasonings.y].upgradeLevel + 1;
    }
    
    return result;
  }
}

Box deviceScreenBox()
{
  return Box(Pizza::platformCam.xy, Pizza::platformCam.size);
}

Box deviceSafeBox()
{
  Box screenBox = deviceScreenBox();
  return screenBox;
}
