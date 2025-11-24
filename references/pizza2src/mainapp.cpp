/*
 *  game.cpp
 *  GLTriangles
 *
 *  Created by Riverman Media on 7/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "mainapp.h"
#include "rmstdinput.h"
#include "rmglobals.h"
#include "rmfont.h"

#include "resourceman.h"
#include "rmdrawtest.h"
#include "rmbasegob.h"
#include "rmeffect.h"
#include "pgo.h"

#include "rmtests.h"
#include "rmdrawtest.h"
#include "rmbezier.h"

#include <map>

using std::cout;
using std::endl;

// =============================== MainApp ============================ //

void MainApp::init()
{
//  Pizza::DEBUG_IGNORE_LOADING = true;  ///////////////

  RM::init_global_constants(RMSystem->display_size().x, RMSystem->display_size().y);
  Pizza::initGameConstants();

  ResourceManager::loadCommon();

  Pizza::DEBUG_KEYSTROKES = true;
//  Pizza::DEBUG_LEVEL = 0;
//  Pizza::DEBUG_LEVEL = 153;
//  Pizza::resetAndSavePlayer(); ///////////////
//  RMSystem->print_save_dir();
//  Pizza::DRAW_DEBUG = true;
//  Pizza::DRAW_DEBUG_PHYS = true;
//  Pizza::DEBUG_MAP_PATHING = true;
//  activePlayer->getCurrency(CURRENCY_GOLD) = 800;
//  RMGraphics->set_render_target(RMGraphics->screenTgt);
//  runTests();
  /*
  Pizza::moreGamesMan = new GamesInterstitialManagerStd();
  Pizza::moreGamesMan->loadResources();
  Pizza::moreGamesMan->init(GamesInterstitialGUIStd::PIZZA_LITE, windowArea,
                             controller, getFont(futura130Font));
   */

//  TitleScene* scene = new TitleScene();
//  CinemaTestScene* scene = new CinemaTestScene();
  PlayScene* scene = new PlayScene();
//  MapScene* scene = new MapScene();
//  ResTestScene* scene = new ResTestScene();

  RMSceneMan->advanceScene(scene);
}

void MainApp::update()
{
  Pizza::platformCam.syncAsScreenCam(SAFE_WH, RM_WH, CameraStd::NEVER_LETTERBOX);

  RMVideoAdMan->update();
  
  // RM::push_time_mult(2.0);
  RMSceneMan->update();
  // RM::pop_time_mult();
}

void MainApp::redraw()
{  
  /////////////////////
  // cout << "start draw" << endl;
  RMGraphics->push_camera(Pizza::platformCam);

  RMGraphics->clear(ColorP4(0.5, 0.5, 0.5, 1.0));
  RMSceneMan->redraw();

  Pizza::platformCam.drawFullscreenLetterbox(RM_WH);
  
  RMGraphics->pop_camera();
  // cout << "end draw" << endl;
  /////////////////////
  
  // cout << "display size " << RMSystem->display_size() << endl;
  // Box(0.0, 0.0, RMSystem->display_size().x, RMSystem->display_size().y).draw_solid(ColorP4(1.0, 0.0, 0.0, 0.25));
  // RMSystem->display_safezone().draw_solid(ColorP4(1.0, 1.0, 1.0, 0.25));
}

// =============================== PizzaButton ============================ //

PizzaButton::PizzaButton() :
  Button(),

  upImg(NULL),
  downImg(NULL),
  disabledImg(NULL),

  clickSound(NULL),

  drawOffset(0.0, 0.0)
{

}

PizzaButton::PizzaButton(Image* setUpImg, Image* setDownImg, Image* setDisabledImg) :
  Button(),

  upImg(setUpImg),
  downImg(setDownImg),
  disabledImg(setDisabledImg),

  clickSound(NULL),

  drawOffset(0.0, 0.0)
{
  if (upImg != NULL)
  {
    setWH(upImg->natural_size());
  }
  else if (downImg != NULL)
  {
    setWH(downImg->natural_size());
  }
}

void PizzaButton::update()
{
  Button::update();
}

void PizzaButton::redraw()
{
  if (disabled == true && disabledImg != NULL)
  {
    setImage(disabledImg);
    drawMe(drawOffset);
  }
  else if (hoverPress() == true && downImg != NULL)
  {
    setImage(downImg);
    drawMe(drawOffset);
  }
  else if (hoverPress() == false && upImg != NULL)
  {
    setImage(upImg);
    drawMe(drawOffset);
  }
}

void PizzaButton::clickCallback()
{
  Button::clickCallback();

  // putting the sound after the click callback prevents the sfx button from playing
  // it when sfx gets turned off
  if (disabled == false && getAlpha() > 0.01)
  {
    // clickSound->play();
  }
}

ColorP4 PizzaButton::textColor()
{
  return hoverPress() ? GUI_HIGHLIGHT_COLOR : WHITE_SOLID;
}

// =============================== PizzaDirNode ============================ //


PizzaDirNode::PizzaDirNode() :
  DirectionalNode(),

  highlightPosOffset(0.0, 0.0),
  highlightSizeOffset(0.0, 0.0),

  absHighlightBox(0.0, 0.0, 0.0, 0.0)
{
  
}


// =============================== PizzaGUI ============================ //

PizzaGUI::PizzaGUI() :
  RivermanGUI(),
  DirectionGraph(),

  highlightBox(0.0, 0.0, -1.0, -1.0),
  boxFlashData(0.0),
  boxCornerGlowVal(0.0),

  boxGlowData(1.0, 1.0, -1.0),
  boxScaleData(0.0, 0.0, 1.0)
{

}

void PizzaGUI::update()
{
  updateActions();
}

void PizzaGUI::redraw()
{
  if (window.getImage() != NULL)
  {
    window.drawMe();
  }

  drawActions();
}

void PizzaGUI::transInStd()
{
  window.setAlpha(0.0);
  state = RM::TRANS_IN;
  script.enqueueX(new LinearFn(&window.color.w, 1.0, GUI_TRANS_DUR));

}

void PizzaGUI::transOutStd()
{
  state = RM::TRANS_OUT;
  script.enqueueX(new LinearFn(&window.color.w, 0.0, GUI_TRANS_DUR));
}

void PizzaGUI::addWidget(Widget* newWidget)
{
  RivermanGUI::addWidget(newWidget);
  newWidget->presser = controller;
}

void PizzaGUI::setFirstNode(Widget* widget)
{
  currNode = findNode(widget);
  rmassert(currNode != NULL);
  highlightBox = getTargetBox();
}

void PizzaGUI::updateDirHighlight()
{
  if (controller->usedDirsLast == false)
  {
    // this makes it so if the highlighter is invisible, it will first turn visible
    // before moving somewhere or activating a button
    return;
  }
  
  RM::bounce_arcsine(boxScaleData, RM::timePassed());
  RM::bounce_arcsine(boxGlowData, RM::timePassed() * 2.0);
  
  if (currNodeHasFocus == true)
  {
    boxFlashData += 4.0 * RM::timePassed();
    RM::wrap1_me(boxFlashData);
  }
  else
  {
    boxFlashData = 0.0;
  }
  
  Coord1 padDirJustPressed = controller->getDirection(JUST_PRESSED);
  
  if (state == RM::ACTIVE &&
      padDirJustPressed != ORIGIN)
  {
    Logical moved = tryMove(padDirJustPressed);
    
    if (moved == true)
    {
      // ResourceManager::downTick->play();
    }
  }
  
  if (state == RM::ACTIVE &&
      controller->getConfirmButtonStatus(JUST_PRESSED) == true)
  {
    if (getCurrDirWidget() != NULL)
    {
      getCurrDirWidget()->clickCallback();
    }
    else
    {
      cout << "null dir widget?" << endl;
    }
  }
  
  if (currNode == NULL)
  {
    return;
  }
  
  if (currNode->data != NULL)
  {
    Box targetBox = getTargetBox();
    
    Point2 currCenter = highlightBox.center();
    
    Point1 speed = 2048.0;
    RM::attract_me(currCenter, targetBox.center(), speed * RM::timePassed());
    RM::attract_me(highlightBox.wh, targetBox.wh, speed * RM::timePassed());
    highlightBox = Box::from_center(currCenter, highlightBox.wh);
  }
}

Box PizzaGUI::getTargetBox()
{
  Box targetBox(0.0, 0.0, 64.0, 64.0);
  
  if (currNode == NULL)
  {
    return targetBox;
  }
  
  if (currNode->data == NULL)
  {
    return targetBox;
  }
  
  targetBox = currNode->data->collisionBox();
  targetBox.xy += currNode->highlightPosOffset;
  targetBox.wh += currNode->highlightSizeOffset;
  
  if (currNode->absHighlightBox.wh.x > 0.0)
  {
    targetBox = currNode->absHighlightBox;
  }
  
  return targetBox;
}

void PizzaGUI::snapDirHighlighter()
{
  if (currNode == NULL)
  {
    return;
  }
  
  if (currNode->data == NULL)
  {
    return;
  }
  
  Box targetBox = getTargetBox();
  highlightBox = Box::from_center(targetBox.center(), targetBox.wh);
}

void PizzaGUI::drawDirHighlight()
{
  if (state != RM::ACTIVE ||
      boxFlashData > 0.5 ||
      controller->usedDirsLast == false)
  {
    return;
  }
  
  //  highlightBox.draw_outline(YELLOW_SOLID);
  // RivermanGUI::draw_window_pattern(ResourceManager::dirControllerHighlightSet,
  //                                  highlightBox.center(), highlightBox.wh + 16.0, HANDLE_C, WHITE_SOLID);
  
  Point1 scaleVal = boxScaleData.x * 2.0 - 1.0;
  Point2 size = highlightBox.wh;
  size += Point2(8.0, 8.0) + scaleVal * 4.0;
  
  ColorP4 currColor = RM::lerp(RM::color255(0x040e23ff),RM::color255(0x122248ff), boxGlowData.x);
  RivermanGUI::draw_window_pattern(getImgSet(selector3x3Fill),
                                   highlightBox.center(), size, HANDLE_C, currColor);
  
  currColor = RM::lerp(RM::color255(0x004effff),RM::color255(0xffffffff), boxGlowData.x);
  RivermanGUI::draw_window_pattern(getImgSet(selector3x3Add),
                                   highlightBox.center(), size, HANDLE_C, currColor);
  
  currColor = RM::lerp(RM::color255(0x005fe140),RM::color255(0x005fe1ff), boxGlowData.x);
  RivermanGUI::draw_window_pattern(getImgSet(selector3x3Normal),
                                   highlightBox.center(), size, HANDLE_C, currColor);
  
  // this is debug
  // Circle(highlightBox.center(), 1.0).draw_solid(GREEN_SOLID);  ////////////////
}

// =============================== IAPGUI ============================ //

IAPGUI::IAPGUI() :
  PizzaGUI(),

  loadingRotation(0.0),
  elapsed(0.0)
{
  window.setBox(RM_BOX);
  window.setColor(0.0, 0.0, 0.0, 0.5);
  
  Pizza::iapMan->gui = this;
  
  state = RM::ACTIVE;
}

void IAPGUI::update()
{
  loadingRotation += TWO_PI * RM::timePassed();
  RM::wrap1_me(loadingRotation, 0.0, TWO_PI);
  
  elapsed += RM::timePassed();
  
  if (elapsed >= 15.0 && state == RM::ACTIVE)
  {
    done();
  }
}

void IAPGUI::redraw()
{
  window.collisionBox().draw_solid(window.getColor());
  // getImg(loadingCircle)->draw_scale(RM_WH * 0.5, Point2(1.0, 1.0), loadingRotation, COLOR_FULL);
  Line(RM_WH * 0.5, RM::rotate(RM_WH * 0.5, RM_WH * 0.5 + Point2(32.0, 0.0), loadingRotation)).draw(WHITE_SOLID);
}

// called whether the purchase was successful or not
void IAPGUI::interactionFinished()
{
  if (state == RM::ACTIVE)
  {
    done();
  }
}

void IAPGUI::done()
{
  setNextPop();
  resolveGUI();
  Pizza::iapMan->gui = NULL;
}

// =============================== PizzaScene ============================ //

void PizzaScene::updateLoading()
{
  Logical loadingDone = resCache.isDoneLoading();

  if (loadingDone == false)
  {
    resCache.loadStride(4);

    if (resCache.isDoneLoading() == true)
    {
      initResources();
    }
  }

  //  tips->update();
  garbageCollectGUIs();  // this removes the tips when it's done

  if (loadingDone == true)
  {
    controller->update();
  }
}

void PizzaScene::drawLoading()
{
  //  tips->redraw();
  RM_BOX.draw_solid(ColorP4(0.0, 0.0, resCache.percentLoaded(), 1.0));
}

void PizzaScene::loadResources()
{
  state = RM::LOADING;
  loadMyRes();
  resCache.prepLoad();
}

void PizzaScene::initResources()
{
  initMyRes();
}

void PizzaScene::loadAndInitNow()
{
  state = RM::LOADING;
  loadMyRes();

  resCache.loadAllNow();

  initMyRes();
}

void PizzaScene::transIn()
{
  state = RM::TRANS_IN;
  script.enqueueX(ScreenFade::SceneIn(SCENE_TRANS_DUR));
}

void PizzaScene::transOut(Scene* next)
{
  nextScene = next;
  state = RM::TRANS_OUT;
  script.enqueueX(ScreenFade::SceneOut(SCENE_TRANS_DUR));
}

// =============================== TitleScene ============================ //

TitleScene::TitleScene() :
  PizzaScene()
{
  
}

TitleScene::~TitleScene()
{
  ResourceManager::unloadTitle();
}

void TitleScene::load()
{
  loadAndInitNow();
}

void TitleScene::loadMyRes()
{
  ResourceManager::loadTitle();
}

void TitleScene::initMyRes()
{
  ResourceManager::initTitle();
  
  transIn();
}

void TitleScene::updateMe()
{
  updateActions();
  
  if (state == RM::ACTIVE) controller->update();
}

void TitleScene::redraw()
{
  RMGraphics->clear(ColorP4(0.5, 0.5, 0.5, 1.0));
  
  drawActions();
}

// =============================== CinemaTestScene ============================ //

CinemaTestScene::CinemaTestScene() :
  PizzaScene(),

  animationScript(),
  mainSpineVR(),
  mainSpineAnimator(),

  typer(),
  relinedDialogueStrs(16)
{
  
}

CinemaTestScene::~CinemaTestScene()
{
  ResourceManager::unloadTestCinema();
}

void CinemaTestScene::load()
{
  loadAndInitNow();
}

void CinemaTestScene::loadMyRes()
{
  ResourceManager::loadTestCinema();
}

void CinemaTestScene::initMyRes()
{
  ResourceManager::initTestCinema();
  
  // init cinema spine
  mainSpineAnimator.initWithAnim(getSpineSkeleton(introPalaceMedSpine),
                                 getSpineAnim(introPalaceMedSpine),
                                 "shot_14a_act_00", false, 0.1);

  // init typer and text
  Box screenBox = deviceScreenBox();
  typer.setXY(screenBox.norm_pos(HANDLE_BL) + Point2(32.0, -100.0));
  typer.setWH(screenBox.width() - 64.0, (screenBox.bottom() - 8.0) - typer.xy.y);
  typer.setText("   ", getFont(shackletonFont), HANDLE_TL);
  typer.init(0.05);

  // delivery car pulls onscreen and stops
  animationScript.enqueueX(new SpineAnimationAction(&mainSpineAnimator, "shot_14a_act_00", 0));
  
  // "DELIVERY BOY: Hey, delivery for Mr, uh.. Great Bone Fairy?"
  enqueueDialogue("shot_14a_dlg_00", 98);

  // "BONE FAIRY: Oh hey, that's me! But I don't remember ordering --"
  enqueueDialogue("shot_14a_dlg_01", 99);

  // delivery car speeds off and throws pizza box
  animationScript.enqueueX(new SpineAnimationAction(&mainSpineAnimator, "shot_14a_act_01", 0));

  transIn();
}

void CinemaTestScene::updateMe()
{
  if (state == RM::ACTIVE)
  {
    animationScript.update();
    mainSpineAnimator.updateSpineSkeleton();
  }
  
  typer.updateTyping();
  
  updateActions();
  
  if (state == RM::ACTIVE) controller->update();
}

void CinemaTestScene::redraw()
{
  RMGraphics->clear(ColorP4(0.5, 0.5, 0.5, 1.0));
 
  mainSpineAnimator.syncFromVisRect(mainSpineVR);
  mainSpineAnimator.renderSpineSkeleton();
  
  typer.drawTyping();
  
  drawActions();
}

void CinemaTestScene::enqueueDialogue(const Char* animName, Coord1 strIndex)
{
  relinedDialogueStrs.add(String2());
  relinedDialogueStrs.last() = getStr(strIndex);
  Point1 textScale = Str::reline_to_box(relinedDialogueStrs.last(),
                                        typer.getFont(), 1.0, typer.getSize());
  
  animationScript.enqueueX(new StartSpineCommand(&mainSpineAnimator, animName, 0, true));
  animationScript.enqueueX(new TyperResetCommand(&typer, &relinedDialogueStrs.last()));
  animationScript.enqueueX(new SetValueCommand<Point1>(&typer.scale.x, textScale));
  animationScript.enqueueX(new SetValueCommand<Point1>(&typer.scale.y, textScale));
  animationScript.enqueueX(new SetValueCommand<Point1>(&typer.color.w, 1.0));
  animationScript.enqueue(&typer);
  animationScript.wait(1.0);
  animationScript.enqueueX(new LinearFn(&typer.color.w, 0.0, 0.25));
}

void CinemaTestScene::spine_event_callback(spAnimationState* state, spEventType eventType,
                                           spTrackEntry* entry, spEvent* event)
{
  // note: the events seem to be coming in twice, i assume it has to do with the playback
  if (event != NULL &&
      event->data != NULL &&
      event->data->name != NULL)
  {
    String1 sfxStr = event->data->name;
    sfxStr.remove(0);
    sfxStr.remove(0);
    sfxStr.remove(0);
    sfxStr.remove(0);  // remove "sfx_"
    sfxStr += ".wav";
    
    Sound* sfx = ResourceManager::getSoundByFilename(sfxStr);
    
    if (sfx != NULL)
    {
      sfx->play();
    }
    else
    {
      cout << "CinemaTestScene::spine_event_callback - Sound " << sfxStr << " not found" << endl;
    }
    
  }
}

// =============================== RotationTestScene ============================ //

RotationTestScene::RotationTestScene() :
  PizzaScene(),

  //////////////// start
  skeleton(),
  animator(),

   saberVR()
  //////////////// end
{

}

RotationTestScene::~RotationTestScene()
{
  ResourceManager::unloadTitle();
}

void RotationTestScene::load()
{
  loadAndInitNow();
}

void RotationTestScene::loadMyRes()
{
  ResourceManager::loadTitle();
}

void RotationTestScene::initMyRes()
{
  ResourceManager::initTitle();

  //////////////// start
  skeleton.setXY(RM_WH * 0.5);
  skeleton.useFishRotation(true);
  
  animator.init(getSpineSkeleton(spineySpine), getSpineAnim(spineySpine));
  spAnimationState_setAnimationByName(animator.spineAnimState, 0, "walk", 1);
  
  saberVR.setXY(RM_WH * 0.5);
  saberVR.setImage(getImg(breakableObjA));
  saberVR.useFishRotation(true);
  //////////////// end

  transIn();
}

void RotationTestScene::updateMe()
{
  updateActions();

  //////////////// start
  static Logical flipToRotation = true;
  
  skeleton.setRotation(RM::angle(skeleton.getXY(), controller->getXY()));
  
  if (flipToRotation == true)
  {
    skeleton.hFlipForRotation();
  }
  
  animator.syncFromVisRect(skeleton);
  animator.updateSpineSkeleton();
  
  saberVR.setRotation(RM::angle(saberVR.getXY(), controller->getXY()));
  
  if (flipToRotation == true)
  {
    saberVR.hFlipForRotation();
  }
  
  if (controller->keyboard.get_status(JUST_PRESSED, kVK_Space) == true)
  {
    flipToRotation = !flipToRotation;
  }
  //////////////// end

  if (state == RM::ACTIVE) controller->update();
}

void RotationTestScene::redraw()
{
  RMGraphics->clear(ColorP4(0.5, 0.5, 0.5, 1.0));

  //////////////// start
  animator.renderSpineSkeleton();
  saberVR.drawMe();
  
  Circle(skeleton.getXY(), 2.0).draw_solid(GREEN_SOLID);
  //////////////// end

  drawActions();
}

// =============================== PlayScene ============================ //

PlayScene::PlayScene() :
  PizzaScene(),

  levelToPlayIndex(0),

  level(NULL),
  fullRestart(true)
{

}

PlayScene::~PlayScene()
{
  if (unloadType == UNLOAD_ALL)
  {
    // this has to be done here because the level's dtor doesn't make polymorphic calls
    level->unloadRes();
    ResourceManager::unloadPlay();
  }
  
  delete level;
}

void PlayScene::load()
{
  loadAndInitNow();
}

void PlayScene::loadMyRes()
{
  Pizza::resetGame();
  
  if (Pizza::DEBUG_LEVEL != -1)
  {
    levelToPlayIndex = Pizza::DEBUG_LEVEL;
  }
  
  activeGameplayData->currLevelIndex = levelToPlayIndex;
  activePlayer->getIntItem(LAST_PLAYED_LEVEL_KEY) = levelToPlayIndex;
  Pizza::savePlayerData();  // save which level they tried to play last
 
  level = new GLLevelTest();
//  level = PizzaLevelInterface::createLevel(levelList[levelToPlayIndex].modeData);
  level->levelID = activeGameplayData->currLevelIndex;
  level->levelRegion = levelList[levelToPlayIndex].region;

  level->preload();
  
  if (loadType == LOAD_ALL) level->loadRes();

  ResourceManager::loadPlay();
}

void PlayScene::initMyRes()
{
  ResourceManager::initPlay();

  level->initRes();

  transIn();
}

void PlayScene::updateMe()
{
  updateDebugKeystrokes();

  if (level->levelState == PhysicsLevelStd::LEVEL_PLAY ||
      guiList.count > 1) 
  {
    updateGUIs();
  }

  if (guiList.count <= 1)
  {
    level->update();
  }
  
  actions.update();
  script.update();

  if (state == RM::ACTIVE)
  {
    controller->update();
  }
}

void PlayScene::redraw()
{
  level->redraw();
  drawActions();
}

void PlayScene::startActive()
{
  level->levelStart();
}

void PlayScene::updateDebugKeystrokes()
{
  if (Pizza::DEBUG_KEYSTROKES == false)
  {
    return;
  }
  
  if (controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_Z) == true)
  {
    Pizza::DRAW_DEBUG = !Pizza::DRAW_DEBUG;
  }
  
  if (controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_X) == true)
  {
    Pizza::DRAW_DEBUG_PHYS = !Pizza::DRAW_DEBUG_PHYS;
    RMPhysics->enable_debug_draw(Pizza::DRAW_DEBUG_PHYS);
  }

}

// =============================== PizzeriaScene ============================ //

/*
PizzeriaScene::PizzeriaScene() :
  PizzaScene(),

  level(NULL)
{
  
}

PizzeriaScene::~PizzeriaScene()
{
  if (unloadType == UNLOAD_ALL)
  {
    // this has to be done here because the level's dtor doesn't make polymorphic calls
    level->unloadRes();
    ResourceManager::unloadPlay();
  }
  
  delete level;
}

void PizzeriaScene::load()
{
  loadAndInitNow();
}

void PizzeriaScene::loadMyRes()
{
  Pizza::resetGame();
  
  if (Pizza::DEBUG_LEVEL != -1)
  {
    levelToPlayIndex = Pizza::DEBUG_LEVEL;
  }
  
  activeGameplayData->currLevelIndex = levelToPlayIndex;
  activePlayer->getIntItem(LAST_PLAYED_LEVEL_KEY) = levelToPlayIndex;
  
  //  level = new ANLevelEasy2();
  level = PizzaLevelInterface::createLevel(levelList[levelToPlayIndex].modeData);
  
  level->preload();
  
  if (loadType == LOAD_ALL) level->loadRes();
  
  ResourceManager::loadPlay();
}

void PizzeriaScene::initMyRes()
{
  ResourceManager::initPlay();
  
  level->initRes();
  
  transIn();
}

void PizzeriaScene::updateMe()
{
  updateDebugKeystrokes();

  updateGUIs();

  actions.update();
  script.update();
  
  if (state == RM::ACTIVE)
  {
    controller->update();
  }
}

void PizzeriaScene::redraw()
{
  level->redraw();
  drawActions();
}

void PizzeriaScene::startActive()
{
  level->levelStart();
}

void PizzeriaScene::updateDebugKeystrokes()
{
  if (Pizza::DEBUG_KEYSTROKES == false)
  {
    return;
  }
  
  if (controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_Z) == true)
  {
    Pizza::DRAW_DEBUG = !Pizza::DRAW_DEBUG;
  }
  
  if (controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_X) == true)
  {
    Pizza::DRAW_DEBUG_PHYS = !Pizza::DRAW_DEBUG_PHYS;
    RMPhysics->enable_debug_draw(Pizza::DRAW_DEBUG_PHYS);
  }
  
}
 */


// =============================== MapTile ============================ //

MapTile::MapTile() :
  imgIndices(8)
{
  
}

Coord1 MapTile::levelNodeBits()
{
  Coord1 pathIndex = imgIndices[MAP_PATHING_LAYER];
  
  if (pathIndex >= 1 && pathIndex <= 15)
  {
    // the +1 is because we skip 0x0, the -1 is because that's the index of the
    //   first node in the tileset
    return (pathIndex + 1 - 1) & 0xf;
  }
  
  return -1;
}

Logical MapTile::isStandableNode()
{
  Coord1 pathIndex = imgIndices[MAP_PATHING_LAYER];
  
  return (pathIndex >= 1 && pathIndex <= 15);
}

Logical MapTile::isPathableNode()
{
  Coord1 pathIndex = imgIndices[MAP_PATHING_LAYER];

  return isStandableNode() || pathIndex == 22 || pathIndex == 23;
}

Logical MapTile::isVisitableType() const
{
  return imgIndices[MAP_LEVELNUMS_LAYER] != -1;
}

Logical MapTile::canWalkInDir(Coord1 dir8)
{
  Coord1 pathTile = imgIndices[MAP_PATHING_LAYER];
  
  // horizontal path
  if (pathTile == 23)
  {
    // cout << "hori fail" << endl;
    return dir8 == EAST || dir8 == WEST;
  }
  // vertical path
  else if (pathTile == 22)
  {
    // cout << "vert fail" << endl;
    return dir8 == NORTH || dir8 == SOUTH;
  }
  // standable node
  else if (pathTile >= 1 && pathTile <= 15)
  {
    Coord1 passthroughBits = levelNodeBits();
    
    if ((passthroughBits & (0x1 << dirToPassthroughIndex(dir8))) != 0)
    {
      // cout << "bits pass" << endl;
      return true;
    }
    else
    {
      if (dir8 == NORTH)
      {
        // cout << "bits fail " << std::hex << passthroughBits <<  << endl;
      }
      return false;
    }
  }
  
  // cout << "fallthrough" << endl;
  return false;
}

Coord1 MapTile::dirToPassthroughIndex(Coord1 dir8)
{
  switch (dir8)
  {
    default:
    case EAST: return 1;
    case SOUTH: return 2;
    case WEST: return 3;
    case NORTH: return 0;
  }
}

Logical MapTile::levelIsAssigned()
{
  return getLevelIndex() != -1;
}

Coord1 MapTile::getLevelIndex() const
{
  return imgIndices[MAP_LEVELNUMS_LAYER];
}

LevelData& MapTile::getLevelData() const
{
  return levelList[getLevelIndex()];
}

Logical operator<(const MapTile& tile1, const MapTile& tile2)
{
  if (tile1.isVisitableType() == false || tile2.isVisitableType() == false)
  {
    return false;
  }
  
//   return tile1.getLevelData().modeData.y < tile2.getLevelData().modeData.y;
//  return tile1.imgIndices[MAP_LEVELNUMS_LAYER] < tile2.imgIndices[MAP_LEVELNUMS_LAYER];
  return tile1.imgIndices[MAP_UNLOCKS_LAYER] < tile2.imgIndices[MAP_UNLOCKS_LAYER];
}

Logical operator>(const MapTile& tile1, const MapTile& tile2)
{
  if (tile1.isVisitableType() == false || tile2.isVisitableType() == false)
  {
    return false;
  }
  
//  return tile1.getLevelData().modeData.y > tile2.getLevelData().modeData.y;
//  return tile1.imgIndices[MAP_LEVELNUMS_LAYER] > tile2.imgIndices[MAP_LEVELNUMS_LAYER];
  return tile1.imgIndices[MAP_UNLOCKS_LAYER] > tile2.imgIndices[MAP_UNLOCKS_LAYER];
}

// =============================== BackgroundMap ============================ //

BackgroundMap::BackgroundMap() :
  Grid<MapTile>(),

  waveData(0.0, 0.0, 1.0),
  finalWaveOffset(0)
{
  load("pizzamap_terrain.txt");
}

void BackgroundMap::update()
{
  RM::bounce_accel(waveData, 1.0 * RM::timePassed());
  finalWaveOffset = RM::clamp(RM::float_to_int(waveData.x / (1.0 / 6.0)), 0, 4);
  finalWaveOffset *= 6;  // wave offsets are 0, 6, 12, 18, 24
}

void BackgroundMap::redraw()
{
  Box currCamBox = RMGraphics->get_camera()->myBox();
  Coord2 topLeftCoords = worldToGrid(currCamBox.norm_pos(HANDLE_TL));
  Coord2 botRightCoords = worldToGrid(currCamBox.norm_pos(HANDLE_BR)) + 1;
  
  for (Coord1 layer = 0; layer < get(0, 0).imgIndices.count; ++layer)
  {
    for (Coord1 row = std::max(topLeftCoords.y, 0); row < botRightCoords.y && row < size.y; ++row)
    {
      for (Coord1 col = std::max(topLeftCoords.x, 0); col < botRightCoords.x && col < size.x; ++col)
      {
        Coord1 tileIndex = get(col, row).imgIndices[layer];
        
        if (tileIndex == -1)
        {
          continue;
        }
        
        Point2 topLeft = gridToWorld(col, row);
        Point2 center = gridToWorldCenter(col, row);
        // cout << "col " << col << " row " << row << " indices " << tileIndex << endl;
        
        if ((tileIndex >= 256 && tileIndex <= 261) ||
            (tileIndex >= 288 && tileIndex <= 293) ||
            (tileIndex >= 320 && tileIndex <= 325) ||
            (tileIndex >= 352 && tileIndex <= 357))
        {
          // animate waves
          tileIndex += finalWaveOffset;
        }
        
        if (tileIndex != 65)
        {
          getImgSet(mapBackgroundSet)[tileIndex]->draw(topLeft);
        }
      }
    }
  }
}

void BackgroundMap::load(const Char* filename)
{
  String1 tilesStr;
  String1 currLine(64);
  Coord1 strIndex = 0;
  ArrayList<String1> lineParts;
  
  RMSystem->read_file(filename, tilesStr);
  
  // tileswide
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileswide");
  Coord1 numColumns = lineParts[1].as_int();
  
  // tileshigh
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileshigh");
  Coord1 numRows = lineParts[1].as_int();
  
  initData(numColumns, numRows);
  
  for (Coord1 row = 0; row < numRows; ++row)
  {
    for (Coord1 col = 0; col < numColumns; ++col)
    {
      get(col, row).coords.set(col, row);
    }
  }
  
  // tilewidth
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tilewidth");
  sqSize.x = lineParts[1].as_double();
  
  // tileheight
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileheight");
  sqSize.y = lineParts[1].as_double();
  
  // this line should be blank
  tilesStr.get_next_line(currLine, strIndex);
  
  Coord1 currLayer = 0;
  Coord1 currRow = 0;
  
  // cout << "dims " << size << " sq " << sqSize << endl;
  while (strIndex < tilesStr.length)
  {
    tilesStr.get_next_line(currLine, strIndex);
    // cout << "loop start, reading line " << currLine << " strIndex " << strIndex << " length " << tilesStr.length << endl;

    if (currLine.length >= 1)
    {
      if (currLine.index_of("layer") != -1)
      {
        currLine.split(lineParts);
        currLayer = lineParts[1].as_int();
        currRow = 0;
      }
      else if (currLine.index_of(",") != -1)
      {
        // currLine.remove_last();  // remove trailing ,
        currLine.split(lineParts, ',');
        
        for (Coord1 currCol = 0; currCol < lineParts.count; ++currCol)
        {
          get(currCol, currRow).imgIndices.add(lineParts[currCol].as_int());
          // cout << "adding " << lineParts[currCol].as_int() << " to (" << currCol << ", "
          // << currRow << "), layer " << get(currCol, currRow).imgIndices.count << endl;
        }
        
        currRow++;
      }
    }
    
    // cout << "loop end, reading line " << currLine << " strIndex " << strIndex << " length " << tilesStr.length << endl;
  }
  
  /*
   for (Coord1 row = 0; row < size.y; ++row)
   {
   for (Coord1 col = 0; col < size.x; ++col)
   {
   cout << get(col, row).imgIndices << " ";
   }
   
   // cout << endl;
   }
   */
}

// =============================== BatchedSpineDraw ============================ //

BatchedSpineDraw::BatchedSpineDraw() :
  animPtr(NULL),
  tileIndex(-1),
  drawXY(0.0, 0.0)
{
  
}

BatchedSpineDraw::BatchedSpineDraw(Coord1 setTileIndex, Point2 setDrawXY, SpineAnimator* setPtr) :
  animPtr(setPtr),
  tileIndex(setTileIndex),
  drawXY(setDrawXY)
{
  
}

void BatchedSpineDraw::redraw()
{
  animPtr->spineSkeleton->x = drawXY.x;
  animPtr->spineSkeleton->y = drawXY.y;
  animPtr->renderSpineSkeleton();
}

Logical operator<(const BatchedSpineDraw& bsd1, const BatchedSpineDraw& bsd2)
{
  return bsd1.tileIndex < bsd2.tileIndex;
}

Logical operator>(const BatchedSpineDraw& bsd1, const BatchedSpineDraw& bsd2)
{
  return bsd1.tileIndex > bsd2.tileIndex;
}


// =============================== MapCharacter ============================ //

MapCharacter::MapCharacter() :
  VisRectangular(),

  animator(),

  currNode(-1),
  targetNode(-1),

  movementScript()
{
  
}

void MapCharacter::update()
{
  movementScript.update();
  
  if (animator.getActive() == true)
  {
    animator.updateSpineSkeleton();
  }
}

void MapCharacter::redraw()
{
  if (animator.getActive() == true)
  {
    animator.syncFromVisRect(*this);
    animator.renderSpineSkeleton();
  }
  else if (getImage() != NULL)
  {
    drawMe();
  }
}

void MapCharacter::hideMe()
{
  setAlpha(0.0);
  setXY(-1000.0, -1000.0);
  currNode = -1;
  targetNode = -1;
  animator.syncFromVisRect(*this);
}

void MapCharacter::unhideMe()
{
  setAlpha(1.0);
  animator.syncFromVisRect(*this);
}

void MapCharacter::syncSpine()
{
  animator.syncFromVisRect(*this);
}


// =============================== MapPizza ============================ //

MapPizza::MapPizza() :
  face(NULL)
{
  face = new PizzaFace(this);
  
  setImage(getImg(pizzaBaseImg));

  setWH(48.0, 48.0);
  autoScale(2.0);
}

MapPizza::~MapPizza()
{
  delete face;
}

void MapPizza::update()
{
  Point2 oldXY = getXY();
  
  MapCharacter::update();
  
  Point1 rotationSign = 1.0;
  
  if (getXY().x >= oldXY.x + 1.0)
  {
    face->targetHFlip = false;
  }
  else if (getXY().x <= oldXY.x - 1.0)
  {
    face->targetHFlip = true;
    rotationSign = -1.0;
  }
  
  if (getXY().y <= oldXY.y - 1.0)
  {
    rotationSign = -1.0;
  }

  Point1 rollDistance = RM::distance_to(oldXY, getXY());
  
  if (rollDistance >= 1.0)
  {
    addRotation(rotationSign * RM::linear_to_rads(rollDistance, getWidth()));
  }
  
  face->updateFace();
}

void MapPizza::redraw()
{
  drawMe();
  
  face->drawFace();
}

// =============================== PizzaMap ============================ //

PizzaMap::PizzaMap() :
  DirectionGraph<PizzaMapNode, MapTile>(),
  Grid<MapTile>(),

  cameraFromScene(NULL),

  script(),
  actions(),

  pizza(),

  hammerBrosList(2),

  treeDrawer(),
  flowersDrawer(),
  deadTreeDrawer(),
  palmDrawer(),
  fernsDrawer(),
  volcanoDrawer(),
  candleDrawer(),
  bananaDrawer(),
  ledDrawer(),
  serverDrawer(),
  polypsDrawer(),
  palaceDrawer(),
  bridgeHDrawer(),
  bridgeVDrawer(),
  bossDrawer(),
  wave1Drawer(),
  wave2Drawer(),
  shopDrawer(),
  hammerBrosDrawer(),

  batchedSpines(1024),

  touchStartXY(0.0, 0.0),
  dragStarted(false)
{
  initMapSpineAnimator(treeDrawer, mapTreeSpine, "idle");
  initMapSpineAnimator(flowersDrawer, mapFlowersSpine, "idle");
  initMapSpineAnimator(deadTreeDrawer, mapDeadTreeSpine, "idle");
  initMapSpineAnimator(palmDrawer, mapPalmTreeSpine, "idle");
  initMapSpineAnimator(fernsDrawer, mapFernsSpine, "idle");
  initMapSpineAnimator(volcanoDrawer, mapVolcanoSpine, "idle");
  initMapSpineAnimator(candleDrawer, mapCandleSpine, "idle");
  initMapSpineAnimator(bananaDrawer, mapBananaSpine, "idle");
  initMapSpineAnimator(ledDrawer, mapLEDSpine, "idle");
  initMapSpineAnimator(serverDrawer, mapServerSpine, "idle");
  initMapSpineAnimator(polypsDrawer, mapPolypsSpine, "idle");
  initMapSpineAnimator(palaceDrawer, mapPalaceSpine, "world_wave");
  initMapSpineAnimator(bridgeHDrawer, mapBridgeHSpine, "idle");
  initMapSpineAnimator(bridgeVDrawer, mapBridgeVSpine, "idle");
  initMapSpineAnimator(bossDrawer, mapPalaceSpine, "skull_wave");
  initMapSpineAnimator(wave1Drawer, mapWave1Spine, "idle");
  initMapSpineAnimator(wave2Drawer, mapWave2Spine, "idle");
  initMapSpineAnimator(shopDrawer, mapShopSpine, "idle");
  initMapSpineAnimator(hammerBrosDrawer, mapHammerBrosSpine, "idle");

  load("pizzamap_object.txt");
  
  // create the paths and nodes
  for (Coord1 row = 0; row < size.y; ++row)
  {
    for (Coord1 col = 0; col < size.x; ++col)
    {
      MapTile& currTile = get(col, row);
      Coord1 tileIndex = currTile.imgIndices[MAP_PATHING_LAYER];
      
      if (tileIndex == -1)
      {
        continue;
      }
      else if (currTile.isStandableNode() == true)
      {
        addStandableNode(Coord2(col, row));
      }
    }
  }
  
  // debugPrintSublevels();   // use TOP operator<(const MapTile& tile1 and operator>
  // debugPrintIndices();   // use MIDDLE operator<(const MapTile& tile1 and operator>
  // debugPrintLevelUnlockValues();   // use LAST operator<(const MapTile& tile1 and operator>
  
  Coord1 lastPlayedLevel = std::max(activePlayer->getIntItem(LAST_PLAYED_LEVEL_KEY), 0);
  setCharacterNodeCenter(&pizza, lastPlayedLevel);
  
  for (Coord1 i = 0; i < hammerBrosList.capacity; ++i)
  {
    hammerBrosList.add(MapCharacter());
    initMapSpineAnimator(hammerBrosList[i].animator, mapHammerBrosSpine, "idle");
    
    String1 brosKey = "BROS_$X_KEY";
    brosKey.int_replace('$', i + 1);
    Coord1 broX = activePlayer->getIntItem(brosKey.as_cstr());
    
    brosKey.replace('X', 'Y');
    Coord1 broY = activePlayer->getIntItem(brosKey.as_cstr());
    
    if (broX != -1 && broY != -1)
    {
      Point2 brosWorldPos = gridToWorldCenter(broX, broY);
      hammerBrosList[i].setXY(brosWorldPos);
      hammerBrosList[i].currNode = getClosestNode(brosWorldPos);
      hammerBrosList[i].targetNode = hammerBrosList[i].currNode;
    }
    else
    {
      hammerBrosList[i].hideMe();
    }
  }

  // init_example();  /////////////////////////
}

void PizzaMap::debugPrintSublevels()
{
  // this prints out all the modes in order
  for (Coord1 mode = 0; mode < NUM_LEVEL_TYPES; ++mode)
  {
    DataList<MapTile*> sortTiles(64);
    
    for (Coord1 row = 0; row < size.y; ++row)
    {
      for (Coord1 col = 0; col < size.x; ++col)
      {
        MapTile& currTile = get(col, row);
        
        if (currTile.levelIsAssigned() == true &&
            currTile.getLevelData().modeData.x == mode)
        {
          sortTiles.add(&currTile);
        }
      }
    }
    
    cout << "mode total: " << sortTiles.count << endl;
    
    for (Coord1 i = 0; i < sortTiles.count; ++i)
    {
      insertion_sort_ptrs(sortTiles);
      
      String2 str;
      getLevelAbbreviation(sortTiles[i]->coords, str);
      // cout << str << " index " << sortTiles[i]->getLevelIndex() << " coords " << sortTiles[i]->coords << endl;
      cout << str << " index " << sortTiles[i]->getLevelIndex() << " unlock " << sortTiles[i]->imgIndices[MAP_UNLOCKS_LAYER] << endl;
    }
    
    cout << endl;
  }
}

void PizzaMap::debugPrintIndices()
{
  DataList<MapTile*> sortTiles(64);
  
  for (Coord1 row = 0; row < size.y; ++row)
  {
    for (Coord1 col = 0; col < size.x; ++col)
    {
      MapTile& currTile = get(col, row);
      
      if (currTile.levelIsAssigned() == true)
      {
        sortTiles.add(&currTile);
      }
    }
  }
  
  for (Coord1 i = 0; i < sortTiles.count; ++i)
  {
    insertion_sort_ptrs(sortTiles);
    
    String2 str;
    getLevelAbbreviation(sortTiles[i]->coords, str);
    // cout << str << " index " << sortTiles[i]->getLevelIndex() << " coords " << sortTiles[i]->coords << endl;
    cout << str << " index " << sortTiles[i]->getLevelIndex() << " unlock " << sortTiles[i]->imgIndices[MAP_UNLOCKS_LAYER] << endl;
  }
  
  cout << endl;
}

void PizzaMap::debugPrintLevelUnlockValues()
{
  DataList<MapTile*> sortTiles(64);
  
  for (Coord1 row = 0; row < size.y; ++row)
  {
    for (Coord1 col = 0; col < size.x; ++col)
    {
      MapTile& currTile = get(col, row);
      
      if (currTile.levelIsAssigned() == true &&
          currTile.getLevelData().modeData.x != LEVEL_TYPE_GATE &&
          currTile.getLevelData().modeData.x != LEVEL_TYPE_PPARLOR)
      {
        sortTiles.add(&currTile);
      }
    }
  }
  
  for (Coord1 i = 0; i < sortTiles.count; ++i)
  {
    insertion_sort_ptrs(sortTiles);
    
    String2 str;
    getLevelAbbreviation(sortTiles[i]->coords, str);
    // cout << str << " index " << sortTiles[i]->getLevelIndex() << " coords " << sortTiles[i]->coords << endl;
    cout << str << " index " << sortTiles[i]->getLevelIndex() << " unlock " << sortTiles[i]->imgIndices[MAP_UNLOCKS_LAYER] << endl;
  }
  
  cout << endl;
}

void PizzaMap::setCharacterNodeCenter(MapCharacter* character, Coord1 levelIndex)
{
  character->currNode = findIndexNodeForLevel(levelIndex);
  character->targetNode = character->currNode;
  
  PizzaMapNode* startNode = nodeList[character->currNode];
  Point2 startCoords = gridToWorldCenter(startNode->data->coords);
  character->setXY(startCoords);
}

void PizzaMap::initMapSpineAnimator(SpineAnimator& animator, Coord1 spineResIndex,
                                    const Char* animName)
{
  animator.init(getSpineSkeleton(spineResIndex), getSpineAnim(spineResIndex));
  spSkeleton_setToSetupPose(animator.spineSkeleton);
  spSkeleton_updateWorldTransform(animator.spineSkeleton);
  spAnimationState_setAnimationByName(animator.spineAnimState, 0, animName, 1);
  animator.spineAnimState->data->defaultMix = 0.4;
}

void PizzaMap::addStandableNode(Coord2 coords)
{
  // contains the grid coords this connects to, or -1, -1
  DataList<Coord2> connectedCoords(4, true);
  connectedCoords.set_all(Coord2(-1, -1));
  
  for (Coord1 checkDir8 = EAST; checkDir8 < ORIGIN; checkDir8 += 2)
  {
    // make sure we can walk 1 square in this direction
    if (get(coords).canWalkInDir(checkDir8) == false)
    {
      // cout << "can walk 1 failed" << endl;
      continue;
    }

    // look 1 square over
    Coord2 checkCoords = coords + DIR_COORDS[checkDir8];

    // see if there is a standable node here
    if (inBounds(checkCoords) == true &&
        get(checkCoords).isStandableNode() == true)
    {
      connectedCoords[checkDir8 / 2] = checkCoords;
      continue;
    }
    
    // make sure we can walk 2 squares in this direction
    if (get(checkCoords).canWalkInDir(checkDir8) == false)
    {
      // cout << "can walk 2 failed" << endl;
      continue;
    }

    // look 2 squares over
    checkCoords = coords + DIR_COORDS[checkDir8] * 2;

    // see if there is a standable node here
    if (inBounds(checkCoords) == true &&
        get(checkCoords).isStandableNode() == true)
    {
      connectedCoords[checkDir8 / 2] = checkCoords;
      continue;
    }
  }
  
  addLevelNode(coords, connectedCoords);
}

void PizzaMap::update()
{
  treeDrawer.updateSpineSkeleton();
  flowersDrawer.updateSpineSkeleton();
  deadTreeDrawer.updateSpineSkeleton();
  palmDrawer.updateSpineSkeleton();
  fernsDrawer.updateSpineSkeleton();
  volcanoDrawer.updateSpineSkeleton();
  candleDrawer.updateSpineSkeleton();
  bananaDrawer.updateSpineSkeleton();
  ledDrawer.updateSpineSkeleton();
  serverDrawer.updateSpineSkeleton();
  polypsDrawer.updateSpineSkeleton();
  palaceDrawer.updateSpineSkeleton();
  bridgeHDrawer.updateSpineSkeleton();
  bridgeVDrawer.updateSpineSkeleton();
  bossDrawer.updateSpineSkeleton();
  wave1Drawer.updateSpineSkeleton();
  wave2Drawer.updateSpineSkeleton();
  shopDrawer.updateSpineSkeleton();
  hammerBrosDrawer.updateSpineSkeleton();

  script.update();
  actions.update();
  
  pizza.update();
  
  for (Coord1 i = 0; i < hammerBrosList.count; ++i)
  {
    hammerBrosList[i].update();
  }
  
  if (RMSceneMan->getCurrScene()->state == RM::ACTIVE)
  {
    updateControls();
  }
  
  updateCamera();
}

void PizzaMap::redraw()
{
  batchedSpines.clear();
  
  Box currCamBox = cameraFromScene->myBox();
  Coord2 topLeftCoords = worldToGrid(currCamBox.norm_pos(HANDLE_TL));
  Coord2 botRightCoords = worldToGrid(currCamBox.norm_pos(HANDLE_BR)) + 1;
  
  for (Coord1 layer = 0; layer < get(0, 0).imgIndices.count; ++layer)
  {
    if (layer == MAP_UNLOCKS_LAYER ||
        layer == MAP_REGION_LAYER)
    {
      continue;
    }
    
    for (Coord1 row = std::max(topLeftCoords.y, 0); row < botRightCoords.y && row < size.y; ++row)
    {
      for (Coord1 col = std::max(topLeftCoords.x, 0); col < botRightCoords.x && col < size.x; ++col)
      {
        Coord1 tileIndex = get(col, row).imgIndices[layer];
        
        if (tileIndex == -1)
        {
          continue;
        }
        
        Point2 topLeft = gridToWorld(col, row);
        Point2 center = gridToWorldCenter(col, row);
        // cout << "col " << col << " row " << row << " indices " << tileIndex << endl;
        
        if (layer == MAP_LEVELNUMS_LAYER)
        {
          // don't show the level number for a gate that's been opened
          if (getVisitSquareType(Coord2(col, row)).x == VISITABLE_GATE &&
              get(col, row).getLevelData().isLocked() == false)
          {
            // do nothing
          }
          // show the level number for a normal level or locked gate
          else
          {
            String2 levelName(16);
            getLevelAbbreviation(Coord2(col, row), levelName);
            
            getFont(futura130Font)->draw_in_box(levelName, center, Point2(0.6, 0.6),
                                                sqSize + Point2(-8.0, -8.0), 0.0, HANDLE_C, WHITE_SOLID);

          }
        }
        else if (layer == MAP_DYNAMIC_LEVELS_LAYER)
        {
          // regular level, which might be locked
          if (tileIndex == 36)
          {
            getImgSet(mapObjectSet)[tileIndex]->draw(topLeft);
            
            // draw the lock, if this level is locked
            if (get(col, row).levelIsAssigned() == true &&
                get(col, row).getLevelData().isLocked() == true)
            {
              getImgSet(mapObjectSet)[39]->draw(topLeft);
            }
          }
          // boss palace
          else if (tileIndex == 37)
          {
            // later this should change depeding on whether or not the boss is beaten
            drawSpineAnimator(tileIndex, topLeft, &bossDrawer);
            
            // draw the lock, if this level is locked
            if (get(col, row).levelIsAssigned() == true &&
                get(col, row).getLevelData().isLocked() == true)
            {
              getImgSet(mapObjectSet)[39]->draw(topLeft);
            }
          }
          // shop
          else if (tileIndex == 38)
          {
            drawSpineAnimator(tileIndex, topLeft, &shopDrawer);
            
            // draw the lock, if this shop is locked
            if (get(col, row).levelIsAssigned() == true &&
                get(col, row).getLevelData().isLocked() == true)
            {
              getImgSet(mapObjectSet)[39]->draw(topLeft);
            }
          }
          // gate
          else if (tileIndex == 40)
          {
            // draw only if locked
            if (get(col, row).levelIsAssigned() == true &&
                get(col, row).getLevelData().isLocked() == true)
            {
              getImgSet(mapObjectSet)[40]->draw(topLeft);
            }
          }
          else
          {
            // regular object tile
            getImgSet(mapObjectSet)[tileIndex]->draw(topLeft);
          }
        }
        else if (layer == MAP_OBJECT_LAYER)
        {
          if (tileIndex == 16)
          {
            drawSpineAnimator(tileIndex, topLeft, &treeDrawer);
          }
          else if (tileIndex == 17)
          {
            drawSpineAnimator(tileIndex, topLeft, &flowersDrawer);
          }
          else if (tileIndex == 18)
          {
            drawSpineAnimator(tileIndex, topLeft, &deadTreeDrawer);
          }
          else if (tileIndex == 19)
          {
            drawSpineAnimator(tileIndex, topLeft, &palmDrawer);
          }
          else if (tileIndex == 20)
          {
            drawSpineAnimator(tileIndex, topLeft, &fernsDrawer);
          }
          else if (tileIndex == 21)
          {
            drawSpineAnimator(tileIndex, topLeft, &volcanoDrawer);
          }
          else if (tileIndex == 24)
          {
            drawSpineAnimator(tileIndex, topLeft, &candleDrawer);
          }
          else if (tileIndex == 25)
          {
            drawSpineAnimator(tileIndex, topLeft, &bananaDrawer);
          }
          else if (tileIndex == 26)
          {
            drawSpineAnimator(tileIndex, topLeft, &ledDrawer);
          }
          else if (tileIndex == 27)
          {
            drawSpineAnimator(tileIndex, topLeft, &serverDrawer);
          }
          else if (tileIndex == 28)
          {
            drawSpineAnimator(tileIndex, topLeft, &polypsDrawer);
          }
          else if (tileIndex == 29)
          {
            // palace in boss mode
          }
          else if (tileIndex == 30)
          {
            drawSpineAnimator(tileIndex, topLeft, &bridgeHDrawer);
          }
          else if (tileIndex == 31)
          {
            drawSpineAnimator(tileIndex, topLeft, &bridgeVDrawer);
          }
          else if (tileIndex == 32)
          {
            // palace defeated
          }
          else if (tileIndex == 33)
          {
            drawSpineAnimator(tileIndex, topLeft, &wave1Drawer);
          }
          else if (tileIndex == 34)
          {
            drawSpineAnimator(tileIndex, topLeft, &wave2Drawer);
          }
          else if (tileIndex == 35)
          {
            drawSpineAnimator(tileIndex, topLeft, &wave2Drawer);
          }
          else
          {
            // regular object tile
            getImgSet(mapObjectSet)[tileIndex]->draw(topLeft);
          }
        }
        else
        {
          // regular object tile
          getImgSet(mapObjectSet)[tileIndex]->draw(topLeft);
        }
      }
    }
  }

  // doing a sort to batch the spines might be faster except that the sort itself is slow
  // cout << "drawing batched spines: " << batchedSpines.count << endl;
  // Point1 start = RMSystem->time();
  // insertion_sort_vals(batchedSpines);
  // cout << "took " << RMSystem->time() - start << endl;
  
  for (Coord1 i = 0; i < batchedSpines.count; ++i)
  {
    batchedSpines[i].redraw();
  }
  
  pizza.redraw();

  for (Coord1 i = 0; i < hammerBrosList.count; ++i)
  {
    hammerBrosList[i].redraw();
  }
  
  actions.redraw();
  script.redraw();
  // RMGraphics->clear(BLACK_SOLID);  //////////////////////////
  // draw_example();  //////////////////////////
}

void PizzaMap::drawSpineAnimator(Coord1 tileIndex, Point2 topLeft, SpineAnimator* animator)
{
  // batchedSpines.add(BatchedSpineDraw(tileIndex, topLeft, animator));
  animator->spineSkeleton->x = topLeft.x;
  animator->spineSkeleton->y = topLeft.y;
  animator->renderSpineSkeleton();
}

void PizzaMap::updateControls()
{
  checkDirButtonMovement();
  checkCursorMovement();

  if (Pizza::DEBUG_KEYSTROKES == true)
  {
    if (controller->keyboard.get_status(PRESSED, kVK_ANSI_Z) == true)
    {
      cameraFromScene->zoom.x -= 0.5 * RM::timePassed();
      cameraFromScene->zoom.y -= 0.5 * RM::timePassed();
    }
    else if (controller->keyboard.get_status(PRESSED, kVK_ANSI_X) == true)
    {
      cameraFromScene->zoom.x += 0.5 * RM::timePassed();
      cameraFromScene->zoom.y += 0.5 * RM::timePassed();
    }
    
    RM::clamp_me(cameraFromScene->zoom.x, 0.2, 2.0);
    RM::clamp_me(cameraFromScene->zoom.y, 0.2, 2.0);
  }
}

void PizzaMap::checkDirButtonMovement()
{
  // already moving, don't do anything
  if (pizza.movementScript.getActive() == true)
  {
    return;
  }

  Coord1 attemptDirIndex = ORIGIN;
  
  for (Coord1 dir = 0; dir < 8; dir += 2)
  {
    if (controller->getDirectionalButtonStatus(dir, PRESSED) == true)
    {
      attemptDirIndex = dir;
    }
  }
  
  // didn't press a movement key, don't do anything
  if (attemptDirIndex == ORIGIN)
  {
    return;
  }
  
  Coord2 startCoords = worldToGrid(pizza.getXY());
  Coord2 betweenCoords = startCoords + DIR_COORDS[attemptDirIndex] * 1;  //
  Coord2 attemptCoords = startCoords + DIR_COORDS[attemptDirIndex] * 2;  // all nodes are 2 squares apart
  
  // this just filters out things that aren't even nodes.
  if (get(betweenCoords).isPathableNode() == false ||
      get(attemptCoords).isStandableNode() == false)
  {
    return;
  }
  
  Coord2 visitNodeType = getVisitSquareType(attemptCoords);

  // this is a debug way to unlock gates, clicking does it too
  if (visitNodeType.x == VISITABLE_GATE &&
      get(attemptCoords).getLevelData().isLocked() == true)
  {
    destroyLockOrGateAndSave(attemptCoords);
    return;
  }
  
  Point2 targetCoords = gridToWorld(attemptCoords);
  
  tryMoveTowardLocation(targetCoords);
}

void PizzaMap::checkCursorMovement()
{
  if (pizza.movementScript.getActive() == true)
  {
    return;
  }

  if (controller->clickStatus(JUST_PRESSED))
  {
    touchStartXY = controller->getXY();
  }
  else if (controller->clickStatus(JUST_RELEASED) == true)
  {
    if (dragStarted == true)
    {
      // do nothing, the camera has already moved
    }
    else
    {
      Coord2 clickedSquare = worldToGrid(cameraFromScene->transformInv(controller->getXY()));
      Coord1 closestNodeIndex = getClosestNode(cameraFromScene->transformInv(controller->getXY()));
      Coord2 visitNodeType = getVisitSquareType(clickedSquare);
      
      // this is a debug way to unlock gates, button movement does it to
      if (visitNodeType.x == VISITABLE_GATE &&
          get(clickedSquare).getLevelData().isLocked() == true)
      {
        destroyLockOrGateAndSave(clickedSquare);
      }
      // player was either on or moving toward this square and it was clicked again
      else if (pizza.targetNode == closestNodeIndex &&
          nodeList[closestNodeIndex]->data->coords == clickedSquare)
      {
        tryVisitSquare(clickedSquare);
      }
      else
      {
        tryMoveTowardCursor();
      }
    }
    
    dragStarted = false;
  }
  else if (controller->clickStatus(PRESSED) == true)
  {
    if (RM::distance_to(controller->getXY(), touchStartXY) >= 8.0)
    {
      dragStarted = true;
    }
    
    if (pizza.movementScript.getActive() == false)
    {
      cameraFromScene->addXY(controller->getOldXY() - controller->getXY());
    }
  }
}

void PizzaMap::updateCamera()
{
  cameraFromScene->size = RM_WH;
  cameraFromScene->handle = RM_WH * 0.5;

  // center camera on player if he's moving
  if (pizza.movementScript.getActive() == true)
  {
    RM::flatten_me(cameraFromScene->xy.x, pizza.getX() - cameraFromScene->handle.x, 512.0 * RM::timePassed());
    RM::flatten_me(cameraFromScene->xy.y, pizza.getY() - cameraFromScene->handle.y, 512.0 * RM::timePassed());
  }
  
  cameraFromScene->clampToArea(getBox());
}

void PizzaMap::addLevelNode(Coord2 parentCoords, const DataList<Coord2>& connectionCoords)
{
  addNode(&get(parentCoords),
          connectionCoords[0].x == -1 ? NULL : &get(connectionCoords[0]),  // east
          connectionCoords[1].x == -1 ? NULL : &get(connectionCoords[1]),  // south
          connectionCoords[2].x == -1 ? NULL : &get(connectionCoords[2]),  // west
          connectionCoords[3].x == -1 ? NULL : &get(connectionCoords[3])); // north
}

void PizzaMap::tryMoveTowardCursor()
{
  tryMoveTowardLocation(cameraFromScene->transformInv(controller->getXY()));
}

void PizzaMap::tryMoveTowardLocation(Point2 xy)
{
  PizzaMapNode* closestReachable = findClosestReachableNode(nodeList[pizza.currNode], xy);
  
  if (closestReachable == nodeList[pizza.currNode])
  {
    return;
  }
  
  pizza.targetNode = closestReachable->listIndex;
  
  DataList<PizzaMapNode*> path;
  Logical connected = shortestPathTo(nodeList[pizza.currNode], nodeList[pizza.targetNode], path);
  moveCharacterTowardTargetNode(&pizza, path);
}

Coord1 PizzaMap::getClosestNode(Point2 mapXY)
{
  Point1 shortestDistance = 99999.0;
  Coord1 resultNodeIndex = 0;
  
  for (Coord1 i = 0; i < nodeList.count; ++i)
  {
    Point1 currDistance = RM::distance_to(mapXY, getNodeCenter(nodeList[i]));
    
    if (currDistance < shortestDistance)
    {
      shortestDistance = currDistance;
      resultNodeIndex = i;
    }
  }
  
  return resultNodeIndex;
}


void PizzaMap::moveCharacterTowardTargetNode(MapCharacter* character, const DataList<PizzaMapNode*>& path)
{
  character->movementScript.clear();
  
  for (Coord1 i = 0; i < path.count; ++i)
  {
    Point1 speed = 256.0;
    
    ActionList* xyMover = new ActionList();
    xyMover->addX(new ArcsineFn(&character->xy.x, getNodeCenter(path[i]).x, 0.125), false);
    xyMover->addX(new ArcsineFn(&character->xy.y, getNodeCenter(path[i]).y, 0.125), true);
    
    Coord1 nextNodeIndex = nodeList.find_index(path[i]);
    
    character->movementScript.enqueueX(new SetValueCommand<Coord1>(&character->currNode, nextNodeIndex));
    character->movementScript.enqueueX(xyMover);
    
    if (i != path.last_i())
    {
      // movementScript.wait(0.125);
    }
    
    if (Pizza::DEBUG_MAP_PATHING == true)
    {
      cout << "move to " << getNodeCenter(path[i]) << endl;
    }
  }
}

Logical PizzaMap::shortestPathTo(PizzaMapNode* startNode, PizzaMapNode* targetNode,
                                 DataList<PizzaMapNode*>& path)
{
  rmassert(startNode != NULL);
  rmassert(targetNode != NULL);
  
  path.clear();
  
  if (startNode == targetNode)
  {
    path.add(targetNode);
    return true;
  }
  
  DataList<PizzaMapNode*> unvisitedNodeList(nodeList);
  DataList<PizzaMapNode*> myPreviousNodeList(nodeList.count, true);
  DataList<Point1> distanceList(nodeList.count, true);

  Point1 BIG_DISTANCE = 999999.0;
  
  myPreviousNodeList.set_all(NULL);
  distanceList.set_all(BIG_DISTANCE);
  
  distanceList[startNode->listIndex] = 0.0;
  unvisitedNodeList.remove(unvisitedNodeList.find_index(startNode));
  
  if (Pizza::DEBUG_MAP_PATHING == true)
  {
    cout << "looking for  " << getNodeCenter(targetNode) << " " << std::hex
        << targetNode << std::dec <<
        " starting from  " << getNodeCenter(startNode) << " "
        << std::hex << startNode << std::dec << endl;
  }

  PizzaMapNode* visitNode = startNode;
  
  while (visitNode != targetNode)
  {
    if (Pizza::DEBUG_MAP_PATHING == true)
    {
      cout << "visiting " << getNodeCenter(visitNode) << " " << std::hex
          << visitNode << std::dec << endl;
    }
    
    PizzaMapNode* checkNode = NULL;

    // first update the distances to all the adjancent nodes
    for (Coord1 dir8 = 0; dir8 < 8; dir8 += 2)
    {
      checkNode = visitNode->links[dir8 / 2];
      
      // no node in this direction
      if (checkNode == NULL)
      {
        continue;
      }
      
      if (checkNode->data->isVisitableType() == true &&
          checkNode->data->getLevelData().modeData.x == LEVEL_TYPE_GATE &&
          checkNode->data->getLevelData().isLocked() == true)
      {
        continue;
      }

      // this node has already been visited
      if (unvisitedNodeList.find_index(checkNode) == -1)
      {
        continue;
      }
      
      if (Pizza::DEBUG_MAP_PATHING == true)
      {
        cout << "checking " << getNodeCenter(checkNode) << endl;
      }

      Point1 oldDistance = distanceList[checkNode->listIndex];
      
       // the +1.0 pretends they are all the same distance apart, which right now is true
      Point1 thisDistance = distanceList[visitNode->listIndex] + 1.0;
      
      if (thisDistance < oldDistance)
      {
        if (Pizza::DEBUG_MAP_PATHING == true)
        {
          cout << "updating distance to " << getNodeCenter(checkNode) <<
              " with distance " << thisDistance << " from " << getNodeCenter(visitNode) << endl;
        }
        
        distanceList[checkNode->listIndex] = thisDistance;
        myPreviousNodeList[checkNode->listIndex] = visitNode;
      }
    }
    
    Point1 shortestDistance = BIG_DISTANCE;
    
    // now find the unvisited node with the shortest distance
    for (Coord1 i = 0; i < unvisitedNodeList.count; ++i)
    {
      Point1 checkDistance = distanceList[unvisitedNodeList[i]->listIndex];
      
      if (checkDistance < shortestDistance)
      {
        shortestDistance = checkDistance;
        visitNode = unvisitedNodeList[i];
      }
    }
    
    if (shortestDistance >= BIG_DISTANCE - 1.0)
    {
      // the next node was not connected to the others
      return false;
    }

    unvisitedNodeList.remove(unvisitedNodeList.find_index(visitNode));
  }
  
  // now assemble the path, in reverse
  visitNode = targetNode;
  path.add(visitNode);
  
  if (Pizza::DEBUG_MAP_PATHING == true)
  {
    cout << "assembling path ending at " << getNodeCenter(targetNode) << endl;
  }
  
  while (visitNode != startNode)
  {
    visitNode = myPreviousNodeList[visitNode->listIndex];
    path.add(visitNode);
    
    if (Pizza::DEBUG_MAP_PATHING == true)
    {
      cout << "add " << getNodeCenter(visitNode) << endl;
    }
  }
  
  // remove the last node, since this is the starting node, then reverse
  path.remove_last();
  path.reverse();
  
  if (Pizza::DEBUG_MAP_PATHING == true)
  {
    cout << "final path: ";
    
    for (Coord1 i = 0; i < path.count; ++i)
    {
      cout << getNodeCenter(path[i]) << " ";
    }
    
    cout << endl;
  }
  
  return true;
}

void PizzaMap::findNodesAtDistance(PizzaMapNode* startNode, Point1 maxDistance,
                                   DataList<PizzaMapNode*>& nodes)
{
  rmassert(startNode != NULL);
  
  nodes.clear();
  
  if (maxDistance <= 0.0)
  {
    nodes.add(startNode);
    return;
  }
  
  DataList<PizzaMapNode*> unvisitedNodeList(nodeList);
  DataList<PizzaMapNode*> myPreviousNodeList(nodeList.count, true);
  DataList<Point1> distanceList(nodeList.count, true);
  
  Point1 BIG_DISTANCE = 999999.0;
  
  myPreviousNodeList.set_all(NULL);
  distanceList.set_all(BIG_DISTANCE);
  
  distanceList[startNode->listIndex] = 0.0;
  unvisitedNodeList.remove(unvisitedNodeList.find_index(startNode));
  
  PizzaMapNode* visitNode = startNode;
  
  while (unvisitedNodeList.count >= 1)
  {
    if (Pizza::DEBUG_MAP_PATHING == true)
    {
      cout << "visiting " << getNodeCenter(visitNode) << " " << std::hex
      << visitNode << std::dec << endl;
    }
    
    PizzaMapNode* checkNode = NULL;
    
    // first update the distances to all the adjancent nodes
    for (Coord1 dir8 = 0; dir8 < 8; dir8 += 2)
    {
      checkNode = visitNode->links[dir8 / 2];
      
      // no node in this direction
      if (checkNode == NULL)
      {
        continue;
      }
      
      if (checkNode->data->isVisitableType() == true &&
          checkNode->data->getLevelData().modeData.x == LEVEL_TYPE_GATE &&
          checkNode->data->getLevelData().isLocked() == true)
      {
        unvisitedNodeList.remove(unvisitedNodeList.find_index(checkNode));
        continue;
      }
      
      // this node has already been visited
      if (unvisitedNodeList.find_index(checkNode) == -1)
      {
        continue;
      }
      
      if (Pizza::DEBUG_MAP_PATHING == true)
      {
        cout << "checking " << getNodeCenter(checkNode) << endl;
      }
      
      Point1 oldDistance = distanceList[checkNode->listIndex];
      
      // the +1.0 pretends they are all the same distance apart, which right now is true
      Point1 thisDistance = distanceList[visitNode->listIndex] + 1.0;
      
      if (thisDistance < oldDistance)
      {
        if (Pizza::DEBUG_MAP_PATHING == true)
        {
          cout << "updating distance to " << getNodeCenter(checkNode) <<
          " with distance " << thisDistance << " from " << getNodeCenter(visitNode) << endl;
        }
        
        distanceList[checkNode->listIndex] = thisDistance;
        myPreviousNodeList[checkNode->listIndex] = visitNode;
      }
    }
    
    Point1 shortestDistance = BIG_DISTANCE;
    
    // now find the unvisited node with the shortest distance
    for (Coord1 i = 0; i < unvisitedNodeList.count; ++i)
    {
      Point1 checkDistance = distanceList[unvisitedNodeList[i]->listIndex];
      
      if (checkDistance < shortestDistance)
      {
        shortestDistance = checkDistance;
        visitNode = unvisitedNodeList[i];
      }
    }
    
    if (shortestDistance >= BIG_DISTANCE - 1.0)
    {
      // the next node was not connected to the others
      break;
    }
    
    unvisitedNodeList.remove(unvisitedNodeList.find_index(visitNode));
  }
  
  for (Coord1 i = 0; i < distanceList.count; ++i)
  {
    if (distanceList[i] <= maxDistance)
    {
      nodes.add(nodeList[i]);
    }
  }
  
  if (Pizza::DEBUG_MAP_PATHING == true)
  {
    cout << "starting at " << startNode->data->coords << " found " << nodes.count <<
        " at distance " << maxDistance << " or less" << endl;
    
    for (Coord1 i = 0; i < nodes.count; ++i)
    {
      cout << nodes[i]->data->coords << " ";
    }
    
    cout << endl;
  }
}

PizzaMapNode* PizzaMap::findClosestReachableNode(PizzaMapNode* startNode, Point2 targetXY)
{
  DataList<PizzaMapNode*> nodes;
  
  findNodesAtDistance(startNode, 100000.0, nodes);
  
  Point1 closestDistance = 100000.0;
  PizzaMapNode* closestNode = startNode;
  
  for (Coord1 i = 0; i < nodes.count; ++i)
  {
    Point2 currNodeXY = getNodeCenter(nodes[i]);
    Point1 currDistance = RM::distance_to(currNodeXY, targetXY);
    
    if (currDistance < closestDistance)
    {
      closestNode = nodes[i];
      closestDistance = currDistance;
    }
  }
  
  // cout << "closest reachable " << closestNode->data->coords << endl;
  
  return closestNode;
}

void PizzaMap::randomizeHammerBros()
{
  DataList<PizzaMapNode*> path;
  DataList<PizzaMapNode*> candidateNodes;
  findNodesAtDistance(nodeList[pizza.targetNode], 3.0, candidateNodes);
  
  for (Coord1 i = 0; i < hammerBrosList.count; ++i)
  {
    Coord2 nextNodeCoords = Coord2(-1, -1);
    
    // this is so its current coords get written if there is no change
    if (hammerBrosList[i].isHidden() == false)
    {
      nextNodeCoords = worldToGrid(hammerBrosList[i].getXY());
    }

    // 50% chance to toggle hidden state
    if (RM::randl() == true)
    {
      // bro is hidden, show it
      if (hammerBrosList[i].isHidden() == true)
      {
        hammerBrosList[i].currNode = pizza.currNode;
        hammerBrosList[i].targetNode = pizza.currNode;
        hammerBrosList[i].setAlpha(1.0);
        hammerBrosList[i].setXY(pizza.getXY());
        hammerBrosList[i].syncSpine();
        
        PizzaMapNode* startNode = nodeList[hammerBrosList[i].currNode];
        PizzaMapNode* randNode = RM::random(candidateNodes);
        nextNodeCoords = randNode->data->coords;
        shortestPathTo(startNode, randNode, path);
        moveCharacterTowardTargetNode(&hammerBrosList[i], path);

        // cout << "showing bro " << i << " moving to " << randNode->data->coords << endl;
      }
      // bro is shown, hide it
      else
      {
        nextNodeCoords.set(-1, -1);
        hammerBrosList[i].hideMe();
        
        // cout << "hiding bro " << i << endl;
      }
    }
    else
    {
      // cout << "bro " << i << " hidden " << hammerBrosList[i].isHidden() <<
      //     " no state change, maybe move" << endl;
      
      // already shown, 50% chance to move somewhere
      if (hammerBrosList[i].isHidden() == false && RM::randl() == true)
      {
        PizzaMapNode* randNode = RM::random(candidateNodes);
        nextNodeCoords = randNode->data->coords;
        PizzaMapNode* startNode = nodeList[hammerBrosList[i].currNode];
        shortestPathTo(startNode, randNode, path);
        moveCharacterTowardTargetNode(&hammerBrosList[i], path);
        
        // cout << "moving bro " << i << " from " << worldToGrid(hammerBrosList[i].getXY())
        //     << " to " << randNode->data->coords << endl;
      }
    }
    
    String1 brosKey = "BROS_$X_KEY";
    brosKey.int_replace('$', i + 1);
    // cout << "writing " << brosKey << " with " << nextNodeCoords.x << endl;
    activePlayer->getIntItem(brosKey.as_cstr()) = nextNodeCoords.x;
    
    brosKey.replace('X', 'Y');
    // cout << "writing " << brosKey << " with " << nextNodeCoords.y << endl;
    activePlayer->getIntItem(brosKey.as_cstr()) = nextNodeCoords.y;
  }
  
   Pizza::savePlayerData();
}

void PizzaMap::load(const Char* filename)
{
  String1 tilesStr;
  String1 currLine(64);
  Coord1 strIndex = 0;
  ArrayList<String1> lineParts;
  
  RMSystem->read_file(filename, tilesStr);
  
  // tileswide
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileswide");
  Coord1 numColumns = lineParts[1].as_int();
  
  // tileshigh
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileshigh");
  Coord1 numRows = lineParts[1].as_int();
  
  initData(numColumns, numRows);
  
  for (Coord1 row = 0; row < numRows; ++row)
  {
    for (Coord1 col = 0; col < numColumns; ++col)
    {
      get(col, row).coords.set(col, row);
    }
  }
  
  // tilewidth
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tilewidth");
  sqSize.x = lineParts[1].as_double();
  
  // tileheight
  tilesStr.get_next_line(currLine, strIndex).split(lineParts);
  rmassert(lineParts[0] == "tileheight");
  sqSize.y = lineParts[1].as_double();
  
  // this line should be blank
  tilesStr.get_next_line(currLine, strIndex);
  
  Coord1 currLayer = 0;
  Coord1 currRow = 0;
  
  // cout << "dims " << size << " sq " << sqSize << endl;
  while (strIndex < tilesStr.length)
  {
    tilesStr.get_next_line(currLine, strIndex);
    // cout << "loop start, reading line " << currLine << " strIndex " << strIndex << " length " << tilesStr.length << endl;
    
    if (currLine.length >= 1)
    {
      if (currLine.index_of("layer") != -1)
      {
        currLine.split(lineParts);
        currLayer = lineParts[1].as_int();
        currRow = 0;
      }
      else if (currLine.index_of(",") != -1)
      {
        // currLine.remove_last();  // remove trailing ,
        currLine.split(lineParts, ',');
        
        for (Coord1 currCol = 0; currCol < lineParts.count; ++currCol)
        {
          get(currCol, currRow).imgIndices.add(lineParts[currCol].as_int());
          // cout << "adding " << lineParts[currCol].as_int() << " to (" << currCol << ", "
          // << currRow << "), layer " << get(currCol, currRow).imgIndices.count << endl;
        }
        
        currRow++;
      }
    }
    
    // cout << "loop end, reading line " << currLine << " strIndex " << strIndex << " length " << tilesStr.length << endl;
  }
}

Point2 PizzaMap::getNodeCenter(PizzaMapNode* node)
{
  return gridToWorldCenter(node->data->coords);
}

void PizzaMap::getLevelAbbreviation(Coord2 gxy, String2& result)
{
  LevelData& levelData = get(gxy).getLevelData();
  
  switch (levelData.modeData.x)
  {
    default: result += "? $"; break;
    case LEVEL_TYPE_GATE: result += "GA $"; break;
    case LEVEL_TYPE_GLADIATOR: result += "GL $"; break;
    case LEVEL_TYPE_SUMO: result += "SU $"; break;
    case LEVEL_TYPE_ANGRY: result += "AN $"; break;
    case LEVEL_TYPE_PIRATE: result += "PI $"; break;
    case LEVEL_TYPE_PUPPY: result += "PU $"; break;
    case LEVEL_TYPE_PLANE: result += "PL $"; break;
    case LEVEL_TYPE_LAKE: result += "LA $"; break;
    case LEVEL_TYPE_SPRINT: result += "SP $"; break;
    case LEVEL_TYPE_FIREMAN: result += "FI $"; break;
    case LEVEL_TYPE_PPARLOR: result += "PP $"; break;
  }
 
  result.int_replace('$', levelData.modeData.y);
}

Coord2 PizzaMap::getVisitSquareType(Coord2 gxy)
{
  Coord1 outerType = VISITABLE_NULL;
  Coord1 innerType = -1;
  
  Coord1 visitableTileIndex = get(gxy).imgIndices[MAP_DYNAMIC_LEVELS_LAYER];
  
  switch (visitableTileIndex)
  {
    default:
    case -1: outerType = VISITABLE_NULL; break;
      
    case 36:
    case 37:
    case 39: outerType = VISITABLE_LEVEL; break;
      
    case 38: outerType = VISITABLE_PARLOR; break;
      
    case 40: outerType = VISITABLE_GATE; break;
      
    case 41: outerType = VISITABLE_SKELE; break;
  }
  
  innerType = get(gxy).getLevelIndex();
  
  return Coord2(outerType, innerType);
}

Coord1 PizzaMap::getVisitBG(Coord2 gxy)
{
  Coord1 regionIndex = get(gxy).imgIndices[MAP_REGION_LAYER];

  regionIndex = RM::clamp(regionIndex, (Coord1) GRAVEYARD_BG, (Coord1) CATACOMBS_BG);
  
  return regionIndex;
}

void PizzaMap::tryVisitSquare(Coord2 gxy)
{
  Coord2 visitType = getVisitSquareType(gxy);
  
  switch (visitType.x)
  {
    case VISITABLE_LEVEL:
    case VISITABLE_PARLOR:
    {
      LevelData& visitLevel = get(gxy).getLevelData();
      visitLevel.region = getVisitBG(gxy);
      cout << "visit region " << visitLevel.region << endl;
      
      if (visitLevel.isLocked() == false)
      {
        PlayScene* scene = new PlayScene();
        scene->levelToPlayIndex = visitType.y;
        RMSceneMan->sceneTransition(scene);
      }
      else
      {
        destroyLockOrGateAndSave(gxy);
      }
      
      break;
    }
      
    default:
      break;
  }
}

void PizzaMap::destroyLockOrGateAndSave(Coord2 gxy)
{
  LevelData& visitLevel = get(gxy).getLevelData();

  visitLevel.unlockForActivePlayer();
  
  VisRectangular lockProxy;
  lockProxy.setImage(getImgSet(mapObjectSet)[39]);
  lockProxy.setXY(gridToWorld(gxy));
  PizzaLevelInterface::createTriangleShatter(&lockProxy,
      Coord2(5, 5), Point2(2.0, 3.0), &actions);
  
  Pizza::savePlayerData();
}

Coord1 PizzaMap::findIndexNodeForLevel(Coord1 levelIndex)
{
  for (Coord1 i = 0; i < nodeList.count; ++i)
  {
    Coord2 nodeCoords = nodeList[i]->data->coords;
    Coord2 visitType = getVisitSquareType(nodeCoords);
    
    if (levelIndex == visitType.y)
    {
      return i;
    }
  }
  
  return -1;
}

// =============================== MapScene ============================ //

MapScene::MapScene() :
  PizzaScene(),

  bgMap(NULL),
  map(NULL),

  sceneCam(),

  anyLevelJustBeaten(false)
{
  
}

MapScene::~MapScene()
{
  delete bgMap;
  delete map;
  
  ResourceManager::unloadMap();
}

void MapScene::load()
{
  loadAndInitNow();
}

void MapScene::loadMyRes()
{
  ResourceManager::loadMap();
  
//  padTilesets();  ///////////////////////////////
}

void MapScene::initMyRes()
{
  ResourceManager::initMap();
  
  bgMap = new BackgroundMap();
  
  map = new PizzaMap();
  map->cameraFromScene = &sceneCam;
  
  if (anyLevelJustBeaten == true)
  {
    map->randomizeHammerBros();
  }
  
  sceneCam.size = RM_WH;
  sceneCam.setXY(sceneCam.topleftOfNorm(map->pizza.getXY(), HANDLE_C));
  
  transIn();
}

void MapScene::updateMe()
{
  bgMap->update();
  map->update();
  
  updateGUIs();
  actions.update();
  script.update();
  
  if (state == RM::ACTIVE)
  {
    // debug way to reset player data
    if (state == RM::ACTIVE &&
        Pizza::DEBUG_KEYSTROKES == true &&
        controller->keyboard.get_status(JUST_PRESSED, kVK_ANSI_R))
    {
      Pizza::resetAndSavePlayer();
      RMSceneMan->sceneTransition(new MapScene());
    }
    
    controller->update();
  }
}

void MapScene::redraw()
{
  RMGraphics->push_camera(sceneCam);
  bgMap->redraw();
  map->redraw();
  RMGraphics->pop_camera();
  
  drawActions();
}

void MapScene::startActive()
{

}

void MapScene::padTilesets()
{
  RMSystem->print_save_dir();
  
  Bitmap* tilesetBMP = Bitmap::create("map_terrain_1x.png");
  Bitmap* paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(24, 24) * 1, 1);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_terrain_padded_1x.png");
  delete tilesetBMP;
  delete paddedBMP;
  
  tilesetBMP = Bitmap::create("map_terrain_2x.png");
  paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(24, 24) * 2, 2);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_terrain_padded_2x.png");
  delete tilesetBMP;
  delete paddedBMP;
  
  tilesetBMP = Bitmap::create("map_terrain_4x.png");
  paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(24, 24) * 4, 4);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_terrain_padded_4x.png");
  delete tilesetBMP;
  delete paddedBMP;
  
  tilesetBMP = Bitmap::create("map_objects_1x.png");
  paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(48, 48) * 1, 1);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_objects_padded_1x.png");
  delete tilesetBMP;
  delete paddedBMP;

  tilesetBMP = Bitmap::create("map_objects_2x.png");
  paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(48, 48) * 2, 2);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_objects_padded_2x.png");
  delete tilesetBMP;
  delete paddedBMP;

  tilesetBMP = Bitmap::create("map_objects_4x.png");
  paddedBMP = Bitmap::create_padded_tilemap(*tilesetBMP, Coord2(48, 48) * 4, 4);
  RMSystem->write_bitmap_to_png(*paddedBMP, "map_objects_padded_4x.png");
  delete tilesetBMP;
  delete paddedBMP;
}


// =============================== ResTestScene ============================ //

ResTestScene::ResTestScene() :
  PizzaScene(),

  testImg(NULL)
{
  
}

ResTestScene::~ResTestScene()
{
  ResourceManager::unloadMap();
}

void ResTestScene::load()
{
  loadAndInitNow();
}

void ResTestScene::loadMyRes()
{
  testImg = new Image("riverman_res.png");
  testImg->handle = Point2(53.0, 60.0);
}

void ResTestScene::initMyRes()
{
  ResourceManager::initMap();
  
  transIn();
}

void ResTestScene::updateMe()
{
  updateGUIs();
  actions.update();
  script.update();
  
  if (state == RM::ACTIVE)
  {
    controller->update();
  }
}

void ResTestScene::redraw()
{
  testImg->draw();
  
  drawActions();
}

void ResTestScene::startActive()
{
  
}

// ========================= ShopGUI =========================== //

ShopGUI::ShopGUI(PizzeriaLevel* setLevel, Logical startOnscreen) :
  PizzaGUI(),

  level(setLevel),

  equipBtn()
{
  Box safeBox = deviceSafeBox();
  equipBtn.setBox(safeBox.norm_pos(HANDLE_TL), Point2(255.0, 67.0));
  
  addWidget(&equipBtn);
  
  PizzaDirNode* currNode = addNode(&equipBtn, NULL, NULL, NULL, NULL);
  currNode->highlightPosOffset += Point2(4.0, 4.0);
  currNode->highlightSizeOffset += Point2(-4.0, -4.0);

  setFirstNode(&equipBtn);
  
  if (startOnscreen == true)
  {
    state = RM::ACTIVE;
  }
  else
  {
    script.enqueueX(new LinearFn(&equipBtn.xy.y, equipBtn.getY(), GUI_TRANS_DUR));
    equipBtn.addY(-RM_WH.y);

    state = RM::TRANS_IN;
  }
}

void ShopGUI::update()
{
  updateActions();
  updateDirHighlight();
}

void ShopGUI::redraw()
{
  getImg(shopPizzaIconImg)->draw(equipBtn.getXY() + Point2(4.0, 4.0));
  
  getFont(shackletonFont)->draw_in_box(getStr(1), equipBtn.getXY() + Point2(61.0, 29.0),
                                       1.0, 299.0, HANDLE_LC, equipBtn.textColor());

  drawActions();
  drawDirHighlight();
}

void ShopGUI::click(Widget* caller)
{
  if (caller == &equipBtn)
  {
    level->zoomInToEquip(GUI_TRANS_DUR);

    script.enqueueX(new LinearFn(&equipBtn.xy.y, equipBtn.getY() - RM_WH.y, GUI_TRANS_DUR));
    setNext(new EquipGUI(level));
    
    state = RM::TRANS_OUT;
  }
}

// ========================= EquipGUI =========================== //

EquipGUI::EquipGUI(PizzeriaLevel* setLevel) :
  PizzaGUI(),

  level(setLevel),

  tabButtons(NUM_INGREDIENT_LOCATIONS),
  tabSelected(0),

  categoryTxt(32, getFont(museoFont), HANDLE_TL),

  itemClickers(12),
  currIngredientSet(64),

  pageUpArrow(),
  pageDownArrow(),
  itemPage(0),

  selectorPositions(-1, -1, -1),
  nextSelector(0),
  selectorAddAlphaData(0.0, 0.0, 1.0),

  doneBtn(),

  camera(),

  pizzaVR(),
  face(NULL)
{
  Box safeBox = deviceSafeBox();
  Point2 menuTL(125.0, 52.0);
  
  // up arrow
  Point1 arrowX = menuTL.x + getImg(equipMenuBaseImg)->natural_size().x * 0.5;
  pageUpArrow.setXY(arrowX, 93.0);
  pageUpArrow.setImage(getImg(equipMoreArrowImg));
  pageUpArrow.autoSize();
  pageUpArrow.vflip(true);
  // pageUpArrow.setAlpha(0.0);
  addWidget(&pageUpArrow);
  
  // down arrow
  pageDownArrow.setXY(arrowX, 463.0);
  pageDownArrow.setImage(getImg(equipMoreArrowImg));
  pageDownArrow.autoSize();
  pageDownArrow.vflip(false);
  // pageDownArrow.setAlpha(0.0);
  addWidget(&pageDownArrow);
  
  // intialize tabs
  Point2 tabSize(38.0, 38.0);

  for (Coord1 i = 0; i < NUM_INGREDIENT_LOCATIONS; ++i)
  {
    tabButtons.add(PizzaButton(NULL, getImgSet(equipMenuTabsSet)[i]));
    tabButtons.last().setXY(menuTL.x + 2.0 + tabSize.x * i, menuTL.y + 2.0);
    tabButtons.last().setWH(tabSize);
    addWidget(&tabButtons[i]);
  }
  
  // initialize category text
  categoryTxt.setXY(133.0, 101.0);
  categoryTxt.setScale(0.6, 0.6);
  categoryTxt.setColor(RM::color255(0x611605FF));
  
  // initialize item buttons
  Point2 itemSize(90.0, 90.0);
  Point2 itemsTopLeft = menuTL + Point2(2.0, 75.0);
  
  for (Coord1 i = 0; i < ITEM_COLS * ITEM_ROWS; ++i)
  {
    itemClickers.add(Clicker());
    itemClickers.last().setXY(itemsTopLeft + itemSize * Point2(i % ITEM_COLS, i / ITEM_COLS));
    itemClickers.last().setWH(itemSize);
    itemClickers.last().setImage(getImg(equipItemImg));
    addWidget(&itemClickers.last());
  }
  
  changeTab(0);
  
  doneBtn.setBox(safeBox.norm_pos(HANDLE_BR) + Point2(-154.0, -63.0), Point2(154.0, 63.0));
  addWidget(&doneBtn);

  // set up node graph
  PizzaDirNode* currNode = addNode(&doneBtn, &itemClickers[9], NULL, &itemClickers.last(), NULL);
//  currNode->highlightPosOffset += Point2(4.0, 4.0);
  currNode->highlightSizeOffset += Point2(-6.0, -6.0);

  // items
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    Widget* rightNode = NULL;
    Widget* downNode = NULL;
    Widget* leftNode = &doneBtn;
    Widget* upNode = NULL;
    
    if (i % ITEM_COLS != ITEM_COLS - 1)
    {
      rightNode = &itemClickers[i + 1];
    }
    else
    {
      rightNode = &doneBtn;
    }
    
    if (i / ITEM_COLS != ITEM_ROWS - 1)
    {
      downNode = &itemClickers[i + ITEM_COLS];
    }
    else if (i >= 9)
    {
      Coord1 col = i % ITEM_COLS;
      Coord1 tab = col * 2 + (col == 2 ? 1 : 0);
      downNode = &tabButtons[tab];
    }
    
    if (i % ITEM_COLS != 0)
    {
      leftNode = &itemClickers[i - 1];
    }
    
    if (i < ITEM_COLS)
    {
      Coord1 col = i % ITEM_COLS;
      Coord1 tab = col * 2 + (col == 2 ? 1 : 0);
      upNode = &tabButtons[tab];
    }
    else
    {
      upNode = &itemClickers[i - ITEM_COLS];
    }
    
    PizzaDirNode* itemNode = addNode(&itemClickers[i], rightNode, downNode, leftNode, upNode);
    // itemNode->highlightSizeOffset += Point2(-4.0, -4.0);
    // itemNode->highlightPosOffset += Point2(2.0, 2.0);
  }
  
  Coord1 lastItemRowIndex = ITEM_COLS * (ITEM_ROWS - 1);
  addNode(&tabButtons[0], &tabButtons[1], &itemClickers[0], &doneBtn,
          &itemClickers[lastItemRowIndex + 0]);
  addNode(&tabButtons[1], &tabButtons[2], &itemClickers[0], &tabButtons[0],
          &itemClickers[lastItemRowIndex + 0]);
  addNode(&tabButtons[2], &tabButtons[3], &itemClickers[1], &tabButtons[1],
          &itemClickers[lastItemRowIndex + 1]);
  addNode(&tabButtons[3], &tabButtons[4], &itemClickers[1], &tabButtons[2],
          &itemClickers[lastItemRowIndex + 1]);
  addNode(&tabButtons[4], &tabButtons[5], &itemClickers[1], &tabButtons[3],
          &itemClickers[lastItemRowIndex + 1]);
  addNode(&tabButtons[5], &tabButtons[6], &itemClickers[2], &tabButtons[4],
          &itemClickers[lastItemRowIndex + 2]);
  addNode(&tabButtons[6], &doneBtn, &itemClickers[2], &tabButtons[5],
          &itemClickers[lastItemRowIndex + 2]);

  setFirstNode(&doneBtn);

  pizzaVR.setXY(416.0 + 191.0, 48.0 + 211.0);
  pizzaVR.setScale(0.72, 0.72);
  pizzaVR.setImage(getImg(pizzaBaseImg));

  face = new PizzaFace(&pizzaVR);
  
  if (level == NULL)
  {
    state = RM::ACTIVE;
  }
  else
  {
    transIn();
  }
}

EquipGUI::~EquipGUI()
{
  delete face;
}

void EquipGUI::update()
{
  face->updateFace();
  
  RM::bounce_linear(selectorAddAlphaData, 1.0 * RM::timePassed());
  
  updateActions();
  updateDirHighlight();
}

void EquipGUI::redraw()
{
  // ========= cam on
  RMGraphics->push_camera(camera);
  Box screenBox = deviceSafeBox();

  // menu base
  getImg(equipMenuBaseImg)->draw(125.0, 52.0);
  
  // category text
  categoryTxt.drawTextBoxed(260.0);
  
  // peel
  getImg(equipPeelImg)->draw(418.0, 48.0);
  
  // face
  pizzaVR.drawMe();
  face->drawFace();
  
  // done button
  getImg(equipDoneCheckImg)->draw(screenBox.norm_pos(HANDLE_BR) + Point2(-141.0, -53.0));
  
  getFont(shackletonFont)->draw_in_box(getStr(91), screenBox.norm_pos(HANDLE_BR) + Point2(-93.0, -28.0),
                                       1.0, 80.0, HANDLE_LC, doneBtn.textColor());

  drawWidgets();
  drawSelectors();
  drawDirHighlight();
  
  pageUpArrow.collisionBox().draw_outline(WHITE_SOLID);
  pageDownArrow.collisionBox().draw_outline(WHITE_SOLID);

  actions.redraw();
  script.redraw();
  
  RMGraphics->pop_camera();
  // ========= cam off
}

void EquipGUI::drawSelectors()
{
  for (Coord1 i = 0; i < selectorPositions.count(); ++i)
  {
    if (selectorPositions[i] == -1)
    {
      continue;
    }
    
    Point2 itemTL = itemClickers[selectorPositions[i]].collisionBox().xy;
    
    Coord1 boxPos = 0;
    
    if (i >= 1 && selectorPositions[0] == selectorPositions[i])
    {
      boxPos++;
    }
    
    if (i >= 2 && selectorPositions[1] == selectorPositions[i])
    {
      boxPos++;
    }
    
    Point2 numberTL = itemTL + Point2(2.0, 2.0) + Point2(16.0 * boxPos, 0.0);
    
    // draw box normal
    Image* boxImg = getImgSet(equipSelectSquaresSet)[i];
    boxImg->options.blend = RenderOptions::BLEND_NORMAL;
    boxImg->draw_scale(itemTL, Point2(1.0, 1.0), 0.0, WHITE_SOLID);

    // draw box overexpose
    ColorP4 boxColor = ColorP4(1.0, 1.0, 1.0, selectorAddAlphaData.x);
    boxImg->options.blend = RenderOptions::BLEND_OVEREXPOSE;
    boxImg->draw_scale(itemTL, Point2(1.0, 1.0), 0.0, boxColor);
    
    // draw number normal
    boxImg = getImgSet(equipSelectNumbersSet)[i];
    boxImg->options.blend = RenderOptions::BLEND_NORMAL;
    boxImg->draw_scale(numberTL, Point2(1.0, 1.0), 0.0, WHITE_SOLID);

    // draw number normal
    boxColor = ColorP4(1.0, 1.0, 1.0, selectorAddAlphaData.x);
    boxImg->options.blend = RenderOptions::BLEND_OVEREXPOSE;
    boxImg->draw_scale(numberTL, Point2(1.0, 1.0), 0.0, boxColor);
  }
}

void EquipGUI::transIn()
{
  state = RM::TRANS_IN;
  
  camera.addY(-RM_WH.y);
  script.enqueueX(new SineFn(&camera.xy.y, 0.0, GUI_TRANS_DUR));
}

void EquipGUI::transOut()
{
  state = RM::TRANS_OUT;

  script.enqueueX(new SineFn(&camera.xy.y, -RM_WH.y, GUI_TRANS_DUR));
}

void EquipGUI::startActive()
{
  
}

void EquipGUI::changeTab(Coord1 newTab)
{
  tabButtons[tabSelected].upImg = NULL;

  tabSelected = newTab;
  tabButtons[tabSelected].upImg = getImgSet(equipMenuTabsSet)[tabSelected];

  Coord1 categoryTextIndex = 92 + tabSelected;
  
  if (tabSelected == LOCATION_SEASONING)
  {
    categoryTextIndex = 100;
  }
  
  categoryTxt.setText(getStr(categoryTextIndex));
  
  nextSelector = 0;
  
  syncIngredientButtons(newTab);
  changePage(0);
}

void EquipGUI::syncIngredientButtons(Coord1 newTab)
{
  currIngredientSet.clear();
  
  // add "None" as long as it's not for base and cheese
  if (newTab != LOCATION_BASE)
  {
    currIngredientSet.add(&ingredientList[0]);
  }
  
  // it starts at 1 because 0 is a placeholder
  for (Coord1 ingIndex = 1; ingIndex < ingredientList.count; ++ingIndex)
  {
    IngredientData* currIngredient = &ingredientList[ingIndex];
    
    if (currIngredient->location == newTab)
    {
      currIngredientSet.add(currIngredient);
    }
  }
}

void EquipGUI::changePage(Coord1 newPage)
{
  itemPage = newPage;

  syncSelectorPositions(tabSelected);
  
  // set all the item button icons
  for (Coord1 btnIndex = 0; btnIndex < itemClickers.count; ++btnIndex)
  {
    Coord1 itemSetIndex = btnIndex + maxItemsPerPage() * itemPage;
    
    if (itemSetIndex >= currIngredientSet.count)
    {
      itemClickers[btnIndex].disable();
      itemClickers[btnIndex].setAlpha(0.0);
      continue;
    }
    
    itemClickers[btnIndex].enable();
    itemClickers[btnIndex].setAlpha(1.0);
    
    // set the clicker's icon to the one for currIngredientSet[itemSetIndex]
    
    
  }
}

void EquipGUI::syncSelectorPositions(Coord1 tab)
{
  Coord3 selectorIngIndicies(-1, -1, -1);
  
  selectorIngIndicies.x = activePlayer->getIngLocation(tab, 0);
  
  if (tab == LOCATION_TOPPING || tab == LOCATION_SEASONING)
  {
    selectorIngIndicies.y = activePlayer->getIngLocation(tab, 1);
  }

  if (tab == LOCATION_TOPPING)
  {
    selectorIngIndicies.z = activePlayer->getIngLocation(tab, 2);
  }
  
  // cout << "equipped ingredient indices are " << selectorIngIndicies << endl;
  // cout << "tab ingredients are "  << endl;
  
  for (Coord1 i = 0; i < currIngredientSet.count; ++i)
  {
    // cout << currIngredientSet[i]->index << " ";
  }
  
  // cout << endl;

  selectorPositions.set(-1, -1, -1);

  // for each selected ingredient, try to find it in the set of ingredients for this tab
  for (Coord1 selectorIndex = 0; selectorIndex < selectorIngIndicies.count(); ++selectorIndex)
  {
    // nothing selected in this slot, skip it
    if (selectorIngIndicies[selectorIndex] == -1)
    {
      continue;
    }
    
    // try to find the match in the ingredient list
    for (Coord1 ingSetIndex = 0; ingSetIndex < currIngredientSet.count; ++ingSetIndex)
    {
      // cout << "equip position " << selectorIndex << " is ingredient id "
      //     << selectorIngIndicies[selectorIndex] << ", currIngredientSet[" << ingSetIndex
      //     << "] = " << currIngredientSet[ingSetIndex]->index << endl;

      if (selectorIngIndicies[selectorIndex] == currIngredientSet[ingSetIndex]->index)
      {
        // cout << "match found, checking page " << endl;

        // found the match, need to see if it's on this page
        if (ingSetIndex >= (itemPage * maxItemsPerPage()) &&
            ingSetIndex < ((itemPage + 1) * maxItemsPerPage()))
        {
          // it is on this page, put it in the right place
          selectorPositions[selectorIndex] = ingSetIndex % maxItemsPerPage();
          // cout << "match found for selector " << selectorIndex << ", new selectors " << selectorPositions << endl;
        }
      }
    }
  }
}

Coord1 EquipGUI::maxItemsPerPage()
{
  return itemClickers.count;
}

void EquipGUI::clickItem(Coord1 buttonIndex)
{
  selectorPositions[nextSelector] = buttonIndex;
  
  Coord1 selectorUsed = nextSelector;
  nextSelector++;
  nextSelector %= numSelectableIngs[tabSelected];
  
  Coord1 ingIndex = buttonIndex + itemPage * maxItemsPerPage();
  IngredientData* currIng = currIngredientSet[ingIndex];
  activePlayer->getIngLocation(tabSelected, selectorUsed) = currIng->index;
  Pizza::savePlayerData();
  cout << "saving " << currIng->localizedName << endl;  //////

  ResourceManager::unloadPizzaPlayer();
  
  // swap the ingredients here
  ResourceManager::loadPizzaPlayer();
  resCache.loadAllNow();
  ResourceManager::initPizzaPlayer();

  // change faces and base
  pizzaVR.setImage(getImg(pizzaBaseImg));
  face->resetFace();
  level->player->face.resetFace();
 
  // [mostly done!] actually change displayed outfit
  // .. support directional nodes for page arrows [wait for final art]
  // .. hook up button icons
}

Coord1 EquipGUI::pagesInCurrentTab()
{
  return (currIngredientSet.count / itemClickers.count) + 1;
}

void EquipGUI::click(Widget* caller)
{
  if (caller->isEnabled() == false)
  {
    // this happens when clicking an empty item
    return;
  }
  
  Coord1 possiblePages = pagesInCurrentTab();
  
  // check arrows
  if (caller == &pageUpArrow)
  {
    Coord1 newPage = (itemPage - 1);
    
    if (newPage == -1)
    {
      newPage = pagesInCurrentTab() - 1;
    }
    
    changePage(newPage);
    return;
  }
  else if (caller == &pageDownArrow)
  {
    Coord1 newPage = (itemPage + 1) % possiblePages;
    changePage(newPage);
    return;
  }
  
  // check tabs
  for (Coord1 i = 0; i < tabButtons.count; ++i)
  {
    if (caller == &tabButtons[i])
    {
      changeTab(i);
      return;
    }
  }
  
  // check item buttons
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    if (caller == &itemClickers[i])
    {
      clickItem(i);
      return;
    }
  }
  
  if (caller == &doneBtn)
  {
    if (level == NULL)
    {
      RMSceneMan->sceneTransition(new MapScene());
    }
    else
    {
      setNext(new ShopGUI(level, false));

      level->zoomOutToBuy(GUI_TRANS_DUR);

      transOut();
    }
  }
}


// ========================= AlgoDraw Tests =========================== //

DataList<Point2> polyA;
DataList<Point2> polyB;
DataList<Point2> polyC;
AlgoTerrainSurfaceDef up(TERRAIN_UP);
AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
AlgoTerrainSurfaceDef spikeUni(Point2(1,0));
ArrayList<AlgoTerrainSurfaceDef> spikeSurfaces;
ArrayList<AlgoTerrainSurfaceDef> cliffSurfaces;
AlgoTerrain terrainA;
AlgoTerrain terrainB;
AlgoTerrain terrainC;
LightingEnvironment lightEnv;
Point1 jacrumTime = 0.0;

AlgoBlock brick;
AlgoBlock glass;
AlgoBlock stone;
AlgoBlock wood;
AlgoBlock steel;

//example init
void init_example()
{
  
  ////Lights//////
  
  LightSource ambient;
  ambient.lightType = ambient.AMBIENT;
  ambient.color = ColorP4(0.4,0.6,0.8,1.0);
  
  LightSource roundLight;
  roundLight.lightType = roundLight.ROUND;
  roundLight.pos = Point2(200,200);
  roundLight.direction = Point2(200,0);
  roundLight.color = ColorP4(1,0.5,0.5,1);
  
  LightSource linearLight;
  linearLight.lightType = linearLight.LINEAR;
  linearLight.color = ColorP4(1.0,0.8,0.4,1.0);
  linearLight.pos = Point2(200,400);
  linearLight.direction = Point2(0,-300);
  
  lightEnv.lights.add(ambient);
  lightEnv.lights.add(roundLight);
  lightEnv.lights.add(linearLight);
  
  //AlgoBlock ///////
  
  brick.borderImg = getImg(brick9TopImg);
  brick.centerImg = getImg(brickCenterImg);
  brick.debugDraw = false;
  brick.edgeOffset = 2;
  brick.edgeScale = 0.5;
  brick.fillOffset = -8;
  brick.fillScale = 0.5;
  
  glass.borderImg = getImg(glass9TopImg);
  glass.centerImg = getImg(glassCenterImg);
  glass.triXImg = getImg(glassTriXImg);
  glass.debugDraw = false;
  glass.edgeOffset = 0;
  glass.fillOffset = 0;
  glass.fillScale = 1.0;
  
  stone.borderImg = getImg(stone9TopImg);
  stone.centerImg = getImg(stoneCenterImg);
  stone.debugDraw = false;
  stone.edgeOffset = 2;
  stone.edgeScale = 0.5;
  stone.fillOffset = -8;
  stone.fillScale = 0.5;
  
  wood.borderImg = getImg(wood9TopImg);
  wood.centerImg = getImg(woodCenterImg);
  wood.debugDraw = false;
  wood.edgeOffset = 2;
  wood.edgeScale = 0.5;
  wood.fillOffset = -8;
  wood.fillScale = 0.5;
  
  steel.borderImg = getImg(steel9TopImg);
  steel.centerImg = getImg(steelCenterImg);
  steel.debugDraw = false;
  steel.edgeOffset = 2;
  steel.edgeScale = 0.5;
  steel.fillOffset = -8;
  steel.fillScale = 0.5;
  
  //AlgoTerrain ///////
  
  polyA.add(Point2(0.0, 320.0));
  polyA.add(Point2(96.0, 416.0));
  polyA.add(Point2(584.0, 416.0));
  polyA.add(Point2(820.0, 364.0));
  polyA.add(Point2(900.0, 312.0));
  polyA.add(Point2(4184.0, 312.0));
  polyA.add(Point2(4260.0, 364.0));
  polyA.add(Point2(4492.0, 416.0));
  polyA.add(Point2(4984.0, 412.0));
  polyA.add(Point2(5080.0, 320.0));
  polyA.add(Point2(5080.0, 540.0));  // for the terrain ground
  polyA.add(Point2(0.0, 540.0));  // for the terrain ground
  
  //closed unfilled spike circle
  Point2 center(450,150);
  Point1 radius = 125;
  for(int a=0; a < 8; a++)
  {
    Point1 angle = ((Point1) a / 8.0) * TWO_PI;
    polyC.add(Point2(radius,angle).pol_to_cart() + center);
  }
  
  //AlgoTerrainSurfaceDef up(TERRAIN_UP);
  up.smooth = 5;
  up.topOffset = 10;
  up.leftOffset = 15;
  up.rightOffset = 15;
  up.hasCaps = true;
  up.img = getImg(redrockTerrainBordersImg);
  up.numTiles = 6;
  up.tiles = Box(0,0,768,48);
  
  //AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
  right.smooth = 1;
  right.rightOffset = 0;
  right.leftOffset = 0;
  right.topOffset = 10;
  right.hasCaps = false;
  right.img = getImg(redrockTerrainBordersImg);
  right.numTiles = 1;
  right.tiles = Box(144,80,48,32);
  
  //AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
  down.smooth = 5;
  down.topOffset = 5;
  down.leftOffset = 15;
  down.rightOffset = 15;
  down.hasCaps = true;
  down.img = getImg(redrockTerrainBordersImg);
  down.numTiles = 4;
  down.tiles = Box(0,48,512,32);
  
  //AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
  left.smooth = 1;
  left.rightOffset = 0;
  left.leftOffset = 0;
  left.topOffset = 10;
  left.hasCaps = false;
  left.img = getImg(redrockTerrainBordersImg);
  left.numTiles = 1;
  left.tiles = Box(8,80,48,40);
  
  //AlgoTerrainSurfaceDef spikeUni(Point2(1,0));
  spikeUni.smooth = 5;
  spikeUni.topOffset = 15;
  spikeUni.leftOffset = 10;
  spikeUni.rightOffset = 5;
  spikeUni.hasCaps = true;
  spikeUni.img = getImg(spikebarImg);
  spikeUni.numTiles = 4;
  spikeUni.tiles = Box(0,0,192,80);
  
  //ArrayList<AlgoTerrainSurfaceDef> spikeSurfaces;
  spikeSurfaces.add(spikeUni);
  
  //add direction definitions in draw order!
  //ArrayList<AlgoTerrainSurfaceDef> cliffSurfaces;
  cliffSurfaces.add(right);
  cliffSurfaces.add(left);
  cliffSurfaces.add(down);
  cliffSurfaces.add(up);
  
  //AlgoTerrain terrainA;
  terrainA.fillOffset = -10;
  terrainA.texScale = 1.0;
  //terrainA.fillResolution = 1024;
  terrainA.fillImg = getImg(redrockCenterImg);
  terrainA.init(polyA,cliffSurfaces);
  
  
  //AlgoTerrain terrainC;
  terrainC.isFilled = false;
  terrainC.texScale = 0.5;
  terrainC.init(polyC,spikeSurfaces);
  
}


//Draw test AlgoBlocks
void draw_example()
{
  //clear BG to gray
  Box(Point2(0,0), Point2(1000,1000)).draw_solid(ColorP4(0.5,0.5,0.5,1.0));
  //getImg(yaryortImg)->draw();
  
  //move lights around
  jacrumTime += 1.0 / 60.0;
  lightEnv.lights[1].pos = Point2(200.00 + 200.0 * sin(jacrumTime), 150);
  lightEnv.lights[2].pos = Point2(200.00, 400 + 100.0 * cos(jacrumTime * 2));
  
  terrainA.draw(Point2(-400,-100), Point2(1,1), 0, lightEnv, false);
  //terrainB.draw(Point2(0,0), Point2(1.0,1.0), 0, lightEnv);
  terrainC.draw(Point2(0,0), Point2(1.0,1.0), 0, lightEnv, false);
  
  //terrainA.debug_draw(Point2(0,0), Point2(0.25,0.25), 0);
  //terrainB.debug_draw(Point2(0,0), Point2(1.0,1.0), 0);
  //terrainC.debug_draw(Point2(0,0), Point2(1.0,1.0), 0);
  
  
  //make some quads
  Quad q1(Point2(-32,-32), Point2(32,-32), Point2(32,32), Point2(-32,32));
  Quad q2(Point2(-8,-8), Point2(8,-8), Point2(8,8), Point2(-8,8));
  Quad q3(Point2(-16,-16), Point2(48,-32), Point2(32,32), Point2(-16,16));
  Quad q4(Point2(-64,-16), Point2(64,-16), Point2(64,16), Point2(-64,16));
  Quad q5(Point2(-8,-8), Point2(8,-12), Point2(64,64), Point2(-64,73));
  Quad q6(Point2(-32,-32), Point2(32,-48), Point2(108,73), Point2(0,92));
  Quad q7(Point2(-32,-64), Point2(0,-64), Point2(64,64), Point2(32,64));
  
  brick.draw(q1, Point2(100,100), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  glass.draw(q2, Point2(180,100), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  stone.draw(q3, Point2(250,100), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  wood.draw(q4, Point2(400,100), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  steel.draw(q5, Point2(500,100), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  brick.draw(q6, Point2(100,200), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  glass.draw(q7, Point2(225,200), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  
  //make some tris
  Tri t1(Point2(0, -24), Point2(32, 24), Point2(-32, 24));
  Tri t2(Point2(-64, -48), Point2(64, -48), Point2(0, 48));
  Tri t3(Point2(-8, -64), Point2(32, 0), Point2(-8, 64));
  Tri t4(Point2(0, -12), Point2(16, 12), Point2(-16, 12));
  Tri t5(Point2(0, -64), Point2(150, 0), Point2(0, 0));
  
  stone.draw(t1, Point2(300,170), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  stone.draw(t2, Point2(380,170), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  steel.draw(t3, Point2(24,170), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  brick.draw(t4, Point2(200,64), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  glass.draw(t5, Point2(16,335), Point2(1,1), 0, WHITE_SOLID, lightEnv, false);
  
  //draw some circles
  stone.draw_disc(16, Point2(310,225), Point2(1,1), 0, WHITE_SOLID, lightEnv, 4, false);
  wood.draw_disc(32, Point2(375,260), Point2(1,1), 0, WHITE_SOLID, lightEnv, 2, false);
  steel.draw_disc(64, Point2(500,275), Point2(1,1), 0, WHITE_SOLID, lightEnv, 2, false);
  
  
  
  
  /*
   //Quad subdivision test
   Box(Point2(0,0), Point2(1000,1000)).draw_solid(ColorP4(0.5,0.5,0.5,1.0));
   getImg(brickCenterImg)->draw(Quad(100,100,200,150,250,200,50,200), Quad(0,0,1,0,1,1,0,1), ColorQuad(WHITE_SOLID,WHITE_SOLID,WHITE_SOLID,WHITE_SOLID));
   
   AD::draw_lit_auto_divided_quad(getImg(brickCenterImg), Quad(300,100,400,100,450,200,250,200), Quad(0,0,1,0,1,1,0,1), ColorQuad(WHITE_SOLID,WHITE_SOLID,WHITE_SOLID,WHITE_SOLID),lightEnv, 3, true);
   
   AD::draw_lit_auto_divided_quad(getImg(brickCenterImg), Quad(100,200,200,250,250,300,50,300), Quad(0,0,1,0,1,1,0,1), ColorQuad(WHITE_SOLID,WHITE_SOLID,WHITE_SOLID,WHITE_SOLID), lightEnv, 3, true);
   
   AD::draw_lit_auto_divided_quad(getImg(brickCenterImg), Quad(300,200,400,250,450,300,225,325), Quad(0,0,1,0,1,1,0,1), ColorQuad(WHITE_SOLID,WHITE_SOLID,WHITE_SOLID,WHITE_SOLID), lightEnv, 3, true);
   */
}

