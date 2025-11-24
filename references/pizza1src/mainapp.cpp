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

using std::cout;
using std::endl;

// =============================== MainApp ============================ //

void MainApp::init()
{
  // Pizza::DEBUG_LANGUAGE = "es";  //////////////////
  
  Pizza::initGameConstants();
  ResourceManager::loadCommon();
  // Pizza::initMoreGamesMan();
  Pizza::extender = new PizzaExtender();
  Pizza::extender->init();

//  Sound::enable_playing(false);  /////////////////
//  Sound::enable_playing(Sound::TYPE_MUSIC, false);  /////////////////
//  Pizza::currWorldPair.set(0, 1);   ////////////////
//  Pizza::currLevelDef = Pizza::currLevelData().typeData;  /////
//  Pizza::currIngredientFound.set(0, 0);  /////////////////
//  Pizza::playerCash = 5500;   ////////////////
//  Pizza::unlockLevelsDebug();  /////////////////
//  ScoreManager::DEBUG_REPORT = true;
//  Pizza::unlockOutfitsDebug();  ////////////
//  Pizza::saveLevels();  /////////////////
//  Pizza::DEBUG_COLLISION = true;
//  Pizza::DEBUG_PHYSICS = true;
//  Pizza::DEBUG_INVINCIBLE = true;
//  Pizza::DEBUG_NO_INTERFACE = true;
//  ArrayList<String1> strFilenames;
//  strFilenames.add("pizza_strings.txt");
//  strFilenames.add("pizza_strings_fr.txt");
//  strFilenames.add("pizza_strings_it.txt");
//  strFilenames.add("pizza_strings_de.txt");
//  strFilenames.add("pizza_strings_es.txt");
//  RMFont::write_font_chars_from_filenames(strFilenames, "flyer_efigs_strip_new.txt", true);
//  RMSystem->print_save_dir();

  PizzaScene* scene = createFirstScene();
//  SplashScene* scene = new SplashScene();
//  TitleScene* scene = new TitleScene();
//  MapScene* scene = new MapScene();
//  ShopScene* scene = new ShopScene();
//  ShareScene* scene = new ShareScene();
//  PlayScene* scene = new PlayScene();
//  WheelScene* scene = new WheelScene();
//  UpgradeScene* scene = new UpgradeScene(true);
  
  RMSceneMan->advanceScene(scene);
}

void MainApp::update()
{
  Pizza::updateScreenCam();
  
  Pizza::extender->update();

  RMSceneMan->update();
  Pizza::scoreMan->update();
  ResourceManager::update();
}

void MainApp::redraw()
{
  RMGraphics->clear(ColorP4(0.0, 0.0, 0.0, 1.0));

  RMGraphics->push_camera(Pizza::platformCam);
  RMSceneMan->redraw();
  Pizza::extender->redraw();
  
  if (Pizza::controller->get_status(PRESSED) == true)
  {
    // Circle(Pizza::controller->getXY(), 2.0).draw_solid(GREEN_SOLID);
    // Circle(Pizza::controller->getXY(), 8.0).draw_outline(WHITE_SOLID);
  }
  
  RMGraphics->pop_camera();
}

PizzaScene* MainApp::createFirstScene()
{
  if (RMSystem->get_OS_family() == IOS_FAMILY)
  {
    return new TitleScene();
  }
  else
  {
    return new SplashScene();
  }
}

// =============================== PizzaExtender ============================ //

PizzaExtender::PizzaExtender() :
  masterScale(1.0),

  hudTimeout(0.1),
  disableTimeout(0.1),

  bg0(),
  bg1(),
  bg2(),
  bg3()
{
  hudTimeout.setInactive();
  disableTimeout.setInactive();
}

void PizzaExtender::init()
{
  Box leftBox = Pizza::leftLetterbox(RM_WH);
  masterScale = leftBox.height() / ResourceManager::extSkyImg->natural_size().y;
  
  initLayer(bg0, ResourceManager::extBG0Img, Point2(-8.0, 0.0));
  initLayer(bg1, ResourceManager::extBG1Img, Point2(-12.0, 0.0));
  initLayer(bg2, ResourceManager::extBG2Img, Point2(-16.0, 0.0));
  initLayer(bg3, ResourceManager::extBG3Img, Point2(-24.0, 0.0));
}

void PizzaExtender::update()
{
  bg0.update();
  bg1.update();
  bg2.update();
  bg3.update();
  
  hudTimeout.update();
  disableTimeout.update();
}

void PizzaExtender::redraw()
{
  Box leftBox = Pizza::leftLetterbox(RM_WH);
  Box rightBox = Pizza::rightLetterbox(RM_WH);
  Box biggerRight = rightBox;
  biggerRight.wh += 2.0;
  
  Point2 scale(masterScale, masterScale);
  
  if (leftBox.width() <= 0.0 ||
      disableTimeout.getActive() == true)
  {
    return;
  }

  // ======= start clipping
  RMGraphics->clip_world(leftBox);
  ResourceManager::extSkyImg->draw_scale(leftBox.norm_pos(HANDLE_TR) -
      Point2(ResourceManager::extSkyImg->natural_size().x * masterScale, 0.0),
      scale);

  RMGraphics->clip_world(biggerRight);
  ResourceManager::extSkyImg->draw_scale(rightBox.norm_pos(HANDLE_TL),
      Point2(masterScale, masterScale));

  drawLayer(bg0);
  drawLayer(bg1);
  drawLayer(bg2);
  drawLayer(bg3);
  RMGraphics->unclip();
  // ======= end clipping

  ResourceManager::extDividerLeftImg->draw_scale(leftBox.norm_pos(HANDLE_TR) + Point2(-2.0, 0.0), scale);
  ResourceManager::extDividerRightImg->draw_scale(rightBox.norm_pos(HANDLE_TL) + Point2(2.0, 0.0), scale);

  if (hudTimeout.getActive() == true &&
      leftBox.width() >= 48.0)
  {
    // draw time
    ResourceManager::extClockImg->draw_scale(leftBox.norm_pos(HANDLE_TC), scale);
    String2 timeStr("@");
    timeStr.time_string('@', Pizza::currFloatTime);
    
    ResourceManager::font->draw_in_box(timeStr, leftBox.norm_pos(HANDLE_TC) + Point2(0.0, 38.0),
                                       0.5, leftBox.width() - 4.0, HANDLE_C, WHITE_SOLID);
    
    // draw money
    ResourceManager::extBillImg->draw_scale(rightBox.norm_pos(HANDLE_TC), scale);
    String2 cashStr("@");
    cashStr.cash_string('@', Pizza::currGameVars[VAR_CASH]);
    cashStr.remove(0);
    ResourceManager::font->draw_in_box(cashStr, rightBox.norm_pos(HANDLE_TC) + Point2(0.0, 38.0),
                                       0.5, rightBox.width() - 4.0, HANDLE_C, WHITE_SOLID);
  }
}

void PizzaExtender::initLayer(ScrollingImage& currBG, Image* img, Point2 speed)
{
  Box leftBox = Pizza::leftLetterbox(RM_WH);
  Point2 bgSize = img->natural_size() * masterScale;
  currBG.setScale(masterScale, masterScale);
  currBG.init(img, speed, leftBox.norm_pos(HANDLE_BR) - bgSize, bgSize);
}

void PizzaExtender::drawLayer(ScrollingImage& currBG)
{
  Box leftBox = Pizza::leftLetterbox(RM_WH);
  Box rightBox = Pizza::rightLetterbox(RM_WH);
  Box biggerRight = rightBox;
  biggerRight.wh += 2.0;

  RMGraphics->clip_world(leftBox);
  Point2 bgSize = currBG.getImage()->natural_size() * masterScale;
  currBG.startPos = leftBox.norm_pos(HANDLE_BR) - bgSize;
  currBG.redraw();
  
  RMGraphics->clip_world(biggerRight);
  currBG.startPos = rightBox.norm_pos(HANDLE_BL) - Point2(0.0, bgSize.y);
  currBG.redraw();
}

void PizzaExtender::reportDrawHUD()
{
  hudTimeout.reset();
}

void PizzaExtender::reportDisable()
{
  disableTimeout.reset();
}

// =============================== PizzaScene ============================ //

PizzaScene::PizzaScene() : 
  Scene(),

  lastSceneRect(Pizza::platformTL),
  lastSceneRectDrawn(false),

  startWithMusic(true),
  stopWithMusic(true),

  releasesScreenshotEnd(false),
  darkAlpha(1.0),
  loadingAlpha(0.0),
  transInHint(RANDOM)
{
  
}

void PizzaScene::drawActions()
{
  drawGUIs();
  
  if (darkAlpha > 0.01)
  {
    CameraStd::screenBoxToWorld().draw_solid(ColorP4(0.0, 0.0, 0.0, darkAlpha * 0.75));
  }
  
  actions.redraw();
  drawLoading();
  script.redraw();
  if (lastSceneRect.getImage() != NULL) lastSceneRect.drawMe();
}

void PizzaScene::transIn()
{
  state = RM::TRANS_IN;
  
  if (ResourceManager::lastScreenImg == NULL) 
  {
    // script.enqueueX(ScreenFade::SceneIn(Pizza::SCENE_WIPE_DUR));
    // darkAlpha = 0.0;
    script.enqueueX(new LinearFn(&darkAlpha, 0.0, 0.15));
  }
  else 
  {
    lastSceneRect.setImage(ResourceManager::lastScreenImg);
    lastSceneRect.autoSize();
    transInEffect();
    script.enqueueX(new LinearFn(&darkAlpha, 0.0, 0.85));
  }
  
  if (soundCache[music] != NULL && startWithMusic == true) soundCache[music]->play();
}

void PizzaScene::transInEffect()
{
  switch (transInHint) 
  {
    case FROM_INTRO:
    case FROM_MAP:
    case FROM_SHOP:
      script.enqueueX(new DoorOpenEffect(ResourceManager::lastScreenImg, true));
      lastSceneRect.setImage(NULL);  // turn off regular draw 
      break;
      
//    case FROM_SHOP:
//      randomSpinIn();
//      break;
      
    case FROM_RESTART:
    case FROM_LOSE:
      script.enqueueX(new ScreenShatter(lastSceneRect));
      lastSceneRect.setImage(NULL);  // turn off regular draw
      break;
      
    default:
      randomTransIn();
      break;
  }
}

void PizzaScene::transOut(Scene* next)
{
  nextScene = next;
  state = RM::TRANS_OUT;

  if (soundCache[music] != NULL && stopWithMusic == true) 
  {
    script.enqueueX(new SoundVolumeFade(soundCache[music], 0.0, 0.25));
  }
  else
  {
    script.wait(0.25);
  }

  script.enqueueX(new SceneScreenshotAction(this, releasesScreenshotEnd));

  if (Pizza::useLoadImg == true)
  {
    actions.addX(new LinearFn(&loadingAlpha, 1.0, 0.24));
    Pizza::useLoadImg = false;
  }
}

void PizzaScene::randomTransIn()
{
  Point1 transTimeStd = 1.5;
  
  const Coord1 numEffects = 6;
  static Coord1 chooser = RM::randi(0, numEffects - 1);
  chooser++;
  chooser %= numEffects;
  
  switch (chooser) 
  {
    default:
    case 0: randomRotIn(); break;  // 8 
    case 1: randomRotBigIn(); break;  // 8
    case 2: randomFallIn(); break; // 4
    case 3: randomSpinIn(); break; // 2
    case 4: poofIn(); break; // 1
    case 5: 
      // 2
      script.enqueueX(new DoorOpenEffect(ResourceManager::lastScreenImg, RM::randl()));
      lastSceneRect.setImage(NULL);  // turn off regular draw
      break;
      /*
    case 6:
      // 9
      script.enqueueX(new ScreenShatter(lastSceneRect));
      lastSceneRect.setImage(NULL);  // turn off regular draw
      break;
       */
  }
}

void PizzaScene::randomRotIn()
{
  Coord1 cornerID = RM::randi(0, 3);
  Point1 rotDir = RM::randl() ? -1.0 : 1.0;
  Point2 cornerPt = lastSceneRect.getBox().corner(cornerID);
  
  lastSceneRect.setXY(cornerPt);
  lastSceneRect.getImage()->handle = Box(Point2(0.0, 0.0), lastSceneRect.getSize()).corner(cornerID);
  script.enqueueX(new CosineFn(&lastSceneRect.rotation, HALF_PI * rotDir, Pizza::SCENE_WIPE_DUR));
}

void PizzaScene::randomRotBigIn()
{
  Coord1 edgeID = RM::randi(0, 3);
  Point1 rotDir = RM::randl() ? -1.0 : 1.0;  
  Point2 edgePt = lastSceneRect.getBox().center_edge(edgeID);
  
  lastSceneRect.setXY(edgePt);
  lastSceneRect.getImage()->handle = Box(Point2(0.0, 0.0), lastSceneRect.getSize()).center_edge(edgeID);
  script.enqueueX(new CosineFn(&lastSceneRect.rotation, PI * rotDir, Pizza::SCENE_WIPE_DUR));
}

void PizzaScene::randomFallIn()
{
  // 0 = right, 1 = down, 2 = left, 3 = up
  Coord1 dirType = RM::randi(0, 3);
  Point1 endVal = 0.0;
  
  switch (dirType)
  {
    default:
    case 0: 
      endVal = lastSceneRect.getX() + lastSceneRect.getWidth();
      script.enqueueX(new AccelFn(&lastSceneRect.xy.x, endVal, Pizza::SCENE_WIPE_DUR)); 
      break;
    case 1: 
      endVal = lastSceneRect.getY() + lastSceneRect.getHeight();
      script.enqueueX(new AccelFn(&lastSceneRect.xy.y, endVal, Pizza::SCENE_WIPE_DUR)); 
      break;
    case 2: 
      endVal = lastSceneRect.getX() - lastSceneRect.getWidth();
      script.enqueueX(new AccelFn(&lastSceneRect.xy.x, endVal, Pizza::SCENE_WIPE_DUR)); 
      break;
    case 3: 
      endVal = lastSceneRect.getY() - lastSceneRect.getHeight();
      script.enqueueX(new AccelFn(&lastSceneRect.xy.y, endVal, Pizza::SCENE_WIPE_DUR)); 
      break;
  }
}

void PizzaScene::randomSpinIn()
{
  Point1 spinDir = RM::randl() ? -1.0 : 1.0;
  
  lastSceneRect.setXY(SCREEN_BOX.center());
  ResourceManager::lastScreenImg->handle_center();
  ActionList* list = new ActionList();
  list->addX(new LinearFn(&lastSceneRect.rotation, TWO_PI * 2.0 * spinDir, Pizza::SCENE_WIPE_DUR), false);
  list->addX(new CosineFn(&lastSceneRect.scale.x, 0.0, Pizza::SCENE_WIPE_DUR), false);
  list->addX(new CosineFn(&lastSceneRect.scale.y, 0.0, Pizza::SCENE_WIPE_DUR), true);
  script.enqueueX(list);
}

void PizzaScene::poofIn()
{
  lastSceneRect.getImage()->handle_center();
  lastSceneRect.setXY(SCREEN_BOX.center());

  PoofEffect* poof = new PoofEffect(PLAY_CENTER, lastSceneRect.getImage());
  poof->scaleTime = Pizza::SCENE_WIPE_DUR;
  poof->beforeFadeTime = 0.0;
  poof->fadeTime = Pizza::SCENE_WIPE_DUR;
  poof->startScale = 1.0;
  poof->init();
  script.enqueueX(poof);

  lastSceneRect.setImage(NULL);  // turn off regular draw
}

void PizzaScene::drawLoading()
{
  if (loadingAlpha < 0.01) return;

  Point2 xy = CameraStd::screenBoxToWorld().center();
  ColorP4 color(1.0, 1.0, 1.0, loadingAlpha);
  ResourceManager::loadingImg->draw_scale(xy, Point2(1.0, 1.0), 0.0, color);
}

void PizzaScene::releaseTransScreenshot()
{
  delete ResourceManager::lastScreenImg;
  ResourceManager::lastScreenImg = NULL;
  lastSceneRect.setImage(NULL);
}

void PizzaScene::takeScreenshot()
{
//  Box transformedBox = Box::from_corners(CameraStd::worldToScreen(SCREEN_BOX.norm_pos(HANDLE_TL)),
//                                         CameraStd::worldToScreen(SCREEN_BOX.norm_pos(HANDLE_BR)));
  Box transformedBox = Box::from_corners(Pizza::platformCam.transform(SCREEN_BOX.norm_pos(HANDLE_TL)),
                                         Pizza::platformCam.transform(SCREEN_BOX.norm_pos(HANDLE_BR)));
  // cout << "screen box " << SCREEN_BOX << " transformed Box " << transformedBox << endl;

  Bitmap* screenBmp = RMGraphics->screenshot(transformedBox);
  ResourceManager::lastScreenImg = new Image(Texture::create(*screenBmp));
  ResourceManager::lastScreenImg->texture->scale_logical(
      1.0 / (Pizza::platformCam.zoom.x * screenBmp->scale));

  delete screenBmp;
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

void PizzaGUI::setFirstNode(Widget* widget)
{
  currNode = findNode(widget);
  rmassert(currNode != NULL);
  highlightBox = getTargetBox();
}

void PizzaGUI::updateDirHighlight()
{
  if (Pizza::controller->usedDirsLast == false)
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
  
  Coord1 padDirJustPressed = Pizza::controller->getDirection(JUST_PRESSED);
  
  if (state == RM::ACTIVE &&
      padDirJustPressed != ORIGIN)
  {
    Logical moved = tryMove(padDirJustPressed);
    
    if (moved == true)
    {
      ResourceManager::downTick->play();
    }
  }
  
  if (state == RM::ACTIVE &&
      Pizza::controller->getConfirmButtonStatus(JUST_PRESSED) == true)
  {
    if (getCurrDirWidget() != NULL)
    {
      getCurrDirWidget()->clickCallback();
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
      Pizza::controller->usedDirsLast == false)
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
  RivermanGUI::draw_window_pattern(ResourceManager::selector3x3Fill,
                                   highlightBox.center(), size, HANDLE_C, currColor);

  currColor = RM::lerp(RM::color255(0x004effff),RM::color255(0xffffffff), boxGlowData.x);
  RivermanGUI::draw_window_pattern(ResourceManager::selector3x3Add,
                                   highlightBox.center(), size, HANDLE_C, currColor);

  currColor = RM::lerp(RM::color255(0x005fe140),RM::color255(0x005fe1ff), boxGlowData.x);
  RivermanGUI::draw_window_pattern(ResourceManager::selector3x3Normal,
                                   highlightBox.center(), size, HANDLE_C, currColor);

  // this is debug
  // Circle(highlightBox.center(), 1.0).draw_solid(GREEN_SOLID);  ////////////////
}

// =============================== SceneScreenshotAction ============================ //

SceneScreenshotAction::SceneScreenshotAction(PizzaScene* setParent, Logical swaps) :
  ActionEvent(),
  parent(setParent),
  shotTaken(false),
  screenSwap(swaps)
{

}

void SceneScreenshotAction::updateMe()
{
  if (shotTaken == true) done();
}

void SceneScreenshotAction::redrawMe()
{
  if (screenSwap == true) parent->releaseTransScreenshot();
  parent->takeScreenshot();
  shotTaken = true;
}

// =============================== DoorOpenEffect ============================ //

DoorOpenEffect::DoorOpenEffect(Image* baseImg, Logical openHorizontal) :
  Effect(),
  doorImgs(2),
  hori(openHorizontal),
  leftDoor(),
  rightDoor(),
  leftOpen(&(hori ? leftDoor.xy.x : leftDoor.xy.y), 1.0, Pizza::SCENE_WIPE_DUR),
  rightOpen(&(hori ? rightDoor.xy.x : rightDoor.xy.y), 1.0, Pizza::SCENE_WIPE_DUR, this)
{  
  Image::create_tiles(baseImg->texture, doorImgs, hori ? 2 : 1, hori ? 1 : 2);
  
  leftDoor.setImage(doorImgs[0]);
  rightDoor.setImage(doorImgs[1]);

  if (hori == true)
  {
    leftDoor.setXY(Pizza::platformTL);
    leftOpen.reset(leftDoor.getX() - leftDoor.getImage()->natural_size().x);
    rightDoor.setXY(Point2(PLAY_W * 0.5, Pizza::platformTL.y));    
    rightOpen.reset(rightDoor.getX() + rightDoor.getImage()->natural_size().x);
  }
  else
  {
    leftDoor.setXY(Pizza::platformTL);
    leftOpen.reset(leftDoor.getY() - leftDoor.getImage()->natural_size().y);
    rightDoor.setXY(Point2(Pizza::platformTL.x, PLAY_H * 0.5));
    rightOpen.reset(rightDoor.getY() + rightDoor.getImage()->natural_size().y);
  }
}

DoorOpenEffect::~DoorOpenEffect()
{
  free_clear(doorImgs);
}

void DoorOpenEffect::updateMe()
{
  leftOpen.update();
  rightOpen.update();
}

void DoorOpenEffect::redrawMe()
{
  leftDoor.drawMe();
  rightDoor.drawMe();
}

void DoorOpenEffect::callback(ActionEvent* caller)
{
  done();
}


// =============================== SplashScene ============================ //

SplashScene::SplashScene() :
  PizzaScene()
{
  startWithMusic = false;
}

SplashScene::~SplashScene()
{
  ResourceManager::unloadSplash();
}

void SplashScene::load()
{
  ResourceManager::loadSplash();
  
  transIn();
}

void SplashScene::updateMe()
{
  updateActions();
  
  if (state == RM::ACTIVE &&
      Pizza::controller->getAnyConfirmStatus(PRESSED) == true)
  {
    transOut(new TitleScene());
  }
  
  Pizza::controller->update();
}

void SplashScene::redraw()
{
  imgCache[bg]->draw(Point2(-44.0, -32.0));

  drawText();
  
  drawActions();
}

void SplashScene::drawText()
{
  if (RMSystem->get_OS_family() == MAC_FAMILY)
  {
    drawTextMac();
  }
  else
  {
    drawTextAppleTV();
  }
}

void SplashScene::drawTextAppleTV()
{
  ColorP4 blueColor = RM::color255(0x6bb8ffff);
  Point1 smallScale = 0.6625;

  // MFi Controller
  ResourceManager::font->draw_in_box(LSTRINGS[122], Point2(98.0, 29.0), 1.0, 264.0, HANDLE_C, WHITE_SOLID);

  // (Recommended)
  ResourceManager::font->draw_in_box(LSTRINGS[123], Point2(98.0, 57.0), smallScale, 264.0, HANDLE_C, WHITE_SOLID);

  // Siri Remote
  ResourceManager::font->draw_in_box(LSTRINGS[124], Point2(383.0, 29.0), 1.0, 264.0, HANDLE_C, WHITE_SOLID);

  // Pause [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[125], Point2(158.0, 115.0), smallScale, 118.0, HANDLE_BC, blueColor);

  // TAP:
  ResourceManager::font->draw_in_box(LSTRINGS[129], Point2(326.0, 94.0), smallScale, 79.0, HANDLE_RC, blueColor);

  // Jump [right side]
  ResourceManager::font->draw_in_box(LSTRINGS[127], Point2(326.0, 118.0), smallScale, 79.0, HANDLE_RC, blueColor);

  // Slam [right side]
  ResourceManager::font->draw_in_box(LSTRINGS[128], Point2(326.0, 144.0), smallScale, 79.0, HANDLE_RC, blueColor);

  // Swipe:
  ResourceManager::font->draw_in_box(LSTRINGS[130], Point2(431.0, 95.0), smallScale, 79.0, HANDLE_LC, blueColor);

  // Move [right side]
  ResourceManager::font->draw_in_box(LSTRINGS[126], Point2(431.0, 119.0), smallScale, 79.0, HANDLE_LC, blueColor);

  // Cursor [right side]
  ResourceManager::font->draw_in_box(LSTRINGS[133], Point2(431.0, 144.0), smallScale, 79.0, HANDLE_LC, blueColor);

  // Pause [right side]
  ResourceManager::font->draw_in_box(LSTRINGS[125], Point2(327.0, 206.0), smallScale, 79.0, HANDLE_RC, blueColor);

  // Rotate to move
  ResourceManager::font->draw_in_box(LSTRINGS[131], Point2(383.0, 283.0), smallScale, 267.0, HANDLE_TC, blueColor);

  // Jump [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[127], Point2(158.0, 263.0), smallScale, 92.0, HANDLE_TC, blueColor);

  // Move [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[126], Point2(59.0, 263.0), smallScale, 92.0, HANDLE_TC, blueColor);

  // Slam [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[128], Point2(158.0, 283.0), smallScale, 92.0, HANDLE_TC, blueColor);
}

void SplashScene::drawTextMac()
{
  ColorP4 blueColor = RM::color255(0x6bb8ffff);
  Point1 smallScale = 0.6625;
  
  // MFi Controller
  ResourceManager::font->draw_in_box(LSTRINGS[122], Point2(98.0, 29.0), 1.0, 264.0, HANDLE_C, WHITE_SOLID);
  
  // (Recommended)
  ResourceManager::font->draw_in_box(LSTRINGS[123], Point2(98.0, 57.0), smallScale, 264.0, HANDLE_C, WHITE_SOLID);
  
  // Pause [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[125], Point2(158.0, 115.0), smallScale, 118.0, HANDLE_BC, blueColor);
  
  // Jump [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[127], Point2(158.0, 263.0), smallScale, 92.0, HANDLE_TC, blueColor);
  
  // Move [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[126], Point2(59.0, 263.0), smallScale, 92.0, HANDLE_TC, blueColor);
  
  // Slam [left side]
  ResourceManager::font->draw_in_box(LSTRINGS[128], Point2(158.0, 283.0), smallScale, 92.0, HANDLE_TC, blueColor);
  
  // Keyboard
  ResourceManager::font->draw_in_box(LSTRINGS[134], Point2(383.0, 29.0), 1.0, 264.0, HANDLE_C, WHITE_SOLID);

  // Jump / Slam
  String2 jumpOrSlam(16);
  jumpOrSlam = LSTRINGS[127];
  jumpOrSlam += " / ";
  jumpOrSlam += LSTRINGS[128];
  ResourceManager::font->draw_in_box(jumpOrSlam, Point2(382.0, 217.0), smallScale, 225.0, HANDLE_C, blueColor);

  // space
  ResourceManager::font->draw_in_box(LSTRINGS[135], Point2(382.0, 187.0), 0.5375, 79.0, HANDLE_C, WHITE_SOLID);

  // esc
  ResourceManager::font->draw_in_box(LSTRINGS[136], Point2(382.0, 261.0), 0.5125, 29.0, HANDLE_C, WHITE_SOLID);

  // Pause [right]
  ResourceManager::font->draw_in_box(LSTRINGS[125], Point2(382.0, 289.0), smallScale, 259.0, HANDLE_C, blueColor);

  // Move [right]
  ResourceManager::font->draw_in_box(LSTRINGS[132], Point2(382.0, 147.0), smallScale, 243.0, HANDLE_C, blueColor);
}


void SplashScene::startActive()
{

}


// =============================== TitleCinema ============================ //

TitleCinema::TitleCinema() :
  Effect(),

  actions(),
  script(this),

  riverman(Point2(240.0, 160.0)),
  presents(Point2(240.0, 221.0)),
  pizza(Point2(-256.0, 160.0)),
  skull(Point2(690.0, 204.0)),
  logo(Point2(240.0, 160.0)),

  shaker(1.0),
  pizzaWarper(),

  skullAngle(RM::degs_to_rads(5.0)),
  jawAngle(RM::degs_to_rads(-8.0)),
  fadeAlpha(0.0),

  warpWeight(0.0)
{
  riverman.setAlpha(0.0);
  presents.setAlpha(0.0);
  logo.setAlpha(0.0);
  logo.setScale(0.1, 0.1);
}

void TitleCinema::init()
{
  shaker.setMagnitude(2.5);
  pizzaWarper.set(&pizza, dwarpCache[titleWarp]);
  
  riverman.setImage(imgCache[titleRiverman]);
  riverman.setScale(2.5, 2.5);
  presents.setImage(imgCache[titlePresents]);
  pizza.setImage(imgCache[titlePizza]);
  skull.setImage(imgCache[titleSkull]);
  logo.setImage(imgCache[titleLogo]);
  
  PizzaScene* scene = NULL;
  
  if (Pizza::getLevelData(Coord2(0, 0)).starsEarned > 0)
  {
    scene = new MapScene();
  }
  else
  {
    scene = new PlayScene();    
  }
  
  scene->transInHint = PizzaScene::FROM_INTRO;
  
  script.wait(2.0);
  
  // T = 0.0
  startAction(new LinearFn(&riverman.color.w, 1.0, 1.0));
  startAction(new LinearFn(&riverman.scale.x, 1.0, 1.0));
  
  script.enqueueX(new PlaySoundCommand(soundCache[music]));
  script.wait(0.5);
  
  // T = 0.5
  startAction(new LinearFn(&presents.xy.y, 196.0, 0.5));
  startAction(new LinearFn(&presents.color.w, 1.0, 0.5));
  script.wait(0.5);
  
  // T = 1.0
  startAction(new LinearFn(&riverman.scale.x, 1.15, 0.8));
  script.wait(0.66);
  
  // T = 1.66
  startAction(new LinearFn(&pizza.xy.x, 178.0, 0.333));
  startAction(new LinearFn(&warpWeight, 1.75, 0.333));
  startAction(new LinearFn(&skull.xy.x, 450.0, 0.333));
  startAction(new LinearFn(&skullAngle, RM::degs_to_rads(2.0), 0.333));
  startAction(new LinearFn(&jawAngle, RM::degs_to_rads(-3.0), 0.333));
  script.wait(0.14);

  // T = 1.8
  startAction(new LinearFn(&riverman.scale.x, 0.0, 0.2));
  startAction(new LinearFn(&riverman.color.w, 0.0, 0.2));
  startAction(new LinearFn(&presents.scale.x, 0.0, 0.2));
  startAction(new LinearFn(&presents.color.w, 0.0, 0.2));
  script.wait(0.2);
  
  // T = 2.0
  startAction(new LinearFn(&pizza.xy.x, 240.0, 0.5));
  startAction(new LinearFn(&warpWeight, 2.0, 0.5));
  startAction(new LinearFn(&skull.xy.x, 396.0, 0.5));
  startAction(new LinearFn(&skullAngle, 0.0, 0.5));
  startAction(new LinearFn(&jawAngle, 0.0, 0.5));
  script.wait(0.5);
  
  // T = 2.5
  startAction(new LinearFn(&logo.scale.x, 0.75, 0.5));
  startAction(new LinearFn(&logo.color.w, 0.75, 0.5));
  startAction(new LinearFn(&pizza.xy.x, 123.0, 0.5));
  startAction(new LinearFn(&warpWeight, 2.9, 0.5));
  startAction(new LinearFn(&skull.xy.x, 515.0, 0.5));
  script.enqueueX(new ShakeCommand(&shaker));
  startAction(new LinearFn(&skullAngle, RM::degs_to_rads(15.0), 0.5));
  startAction(new LinearFn(&jawAngle, RM::degs_to_rads(-10.0), 0.5));
  script.wait(0.5);
  
  // T = 3.0
  startAction(new LinearFn(&logo.scale.x, 1.0, 0.5));
  startAction(new LinearFn(&logo.color.w, 1.0, 0.5));  
  startAction(new LinearFn(&pizza.xy.x, 80.0, 0.5));
  startAction(new LinearFn(&warpWeight, 3.0, 0.5));
  startAction(new LinearFn(&skull.xy.x, 546.0, 0.5));
  startAction(new LinearFn(&skullAngle, RM::degs_to_rads(17.0), 0.5));
  startAction(new LinearFn(&jawAngle, RM::degs_to_rads(-12.0), 0.5));
  script.wait(0.5);
  
  // T = 3.5
  startAction(new LinearFn(&logo.scale.x, 1.2, 1.5));  
  startAction(new LinearFn(&fadeAlpha, 1.0, 1.5));
  script.wait(0.5);
  
  // T = 4.0
  startAction(new LinearFn(&logo.color.w, 0.0, 1.0));
  script.wait(1.5);
  
  // T = 5.5
  script.enqueueX(new SceneTransitionCommand(scene));
}

void TitleCinema::updateMe()
{
  actions.update();
  script.update();
  
  shaker.update();
  riverman.scale.y = riverman.scale.x;
  presents.scale.y = riverman.scale.y;
  logo.scale.y = logo.scale.x;
}

void TitleCinema::redrawMe()
{
  riverman.drawMe();
  presents.drawMe();
  
  pizzaWarper.draw(warpWeight / 3.0);
  imgCache[titlePizzaPad]->draw(pizza.getXY());
  
  skull.addXY(shaker.offsets);
  skull.setRotation(0.0);
  skull.setImage(imgCache[titleSpine]);
  skull.drawMe();
  
  skull.setRotation(skullAngle);
  skull.setImage(imgCache[titleSkull]);
  skull.drawMe();
  
  skull.setRotation(jawAngle);
  skull.setImage(imgCache[titleJaw]);
  skull.drawMe();
  
  Box fillBox = CameraStd::screenBoxToWorld();
  if (fadeAlpha > 0.01) fillBox.draw_solid(ColorP4(0.0, 0.0, 0.0, fadeAlpha));
  
  logo.drawMe();
}

void TitleCinema::startAction(ActionEvent* ae)
{
  script.enqueueAddX(ae, &actions);
}

void TitleCinema::callback(ActionEvent* caller)
{
  eventCallback();
}

// =============================== TitleScene ============================ //

TitleScene::TitleScene() :
  PizzaScene(),
  cinema(),
  sky()
{
  startWithMusic = false;
}

TitleScene::~TitleScene()
{
  ResourceManager::unloadTitle();
}

void TitleScene::load()
{
  ResourceManager::loadTitle();
  soundCache[music]->repeats(false);

  cinema.init();
  cinema.setActive(false);
  
  sky.init(imgCache[titleSky], Point2(-15.0, 0.0), Point2(Pizza::platformTL.x, 352.0), Point2(SCREEN_W, PLAY_H));

  transIn();  
}

void TitleScene::updateMe()
{
  sky.update();
  cinema.update();
  updateActions();
  Pizza::controller->update();
}

void TitleScene::redraw()
{
  RMGraphics->push_camera(cinema.shaker.offsets, Point2(0.0, 0.0), Point2(1.0, 1.0), 0.0);
  sky.redraw();
  imgCache[bg]->draw(Point2(-60.0, 368.0));
  cinema.redraw();
  drawActions();
  RMGraphics->pop_camera();
}

void TitleScene::startActive()
{
  cinema.setActive(true);
}

// =============================== MapScene ============================ //

MapScene::MapScene() :
  PizzaScene(),
  worldGUIIn(NULL),
  worldGUIOut(NULL),
  zoomCam(),
  sky()
{
  worldGUIIn = NULL;
  worldGUIOut = NULL;
  Pizza::worldMapAlpha = 1.0;
}

MapScene::~MapScene()
{
  delete worldGUIIn;
  delete worldGUIOut;
  
  ResourceManager::unloadMap();
}

void MapScene::load()
{
  ResourceManager::loadMap();
  
  sky.init(imgCache[bg], Point2(-15.0, 0.0), Point2(Pizza::platformTL.x, -64.0),
           SCREEN_WH);
  sky.setColor(MAP_SKY_COLORS[Pizza::currWorldPair.x]);
  
  worldGUIIn = new WorldGUI(this, Pizza::currWorldPair.x, 0);

  MainGUI* mainGUI = new MainGUI();
  mainGUI->scene = this;
  mainGUI->resetNodeGraph();
  pushGUI(mainGUI);

  soundCache[moreRivermanSound]->pitch = 2.0;
  soundCache[rivermanInterstitialSound]->pitch = 2.0;

  // more games
  if (Pizza::getLevelData(Coord2(0, 0)).starsEarned >= 1 &&
      Pizza::getLevelData(Coord2(0, 1)).starsEarned >= 1 &&
      Pizza::getLevelData(Coord2(0, 2)).starsEarned >= 1)
  {
    if (Pizza::moreGamesMan != NULL &&
        Pizza::moreGamesMan->getTrackingCount() >= 3)
    {
      GamesInterstitialGUIStd* gui = Pizza::moreGamesMan->tryToUseGUI();

      if (gui != NULL)
      {
        pushGUI(gui);
        Pizza::moreGamesMan->resetTrackingAndSave();
      }
    }
  }

  Pizza::liteBtnDown = false;
  PizzaScene::transIn();
}

void MapScene::updateMe()
{
  sky.update();
  
  for (Coord1 i = 0; i < 3; ++i)
  {
    sky.color[i] = RM::lerp(sky.color[i], MAP_SKY_COLORS[Pizza::currWorldPair.x][i], 2.0 * RM::timePassed());
  }

  ///////////////////////
  /*
  static Logical on = false;

  if (on == false && Pizza::controller->clickStatus(JUST_PRESSED) == true)
  {
    GamesInterstitialGUIStd* moreGames = Pizza::moreGamesMan->tryToUseGUI();

    if (moreGames != NULL)
    {
      RMSceneMan->pushGUI(moreGames);
      on = true;
    }
  }
   */
  ///////////////////////

  updateWorld();
  updateActions();
  if (state == RM::ACTIVE) Pizza::controller->update();
}

void MapScene::redraw()
{
  RMGraphics->push_camera(zoomCam);
  RMGraphics->clear(WHITE_SOLID);
  sky.redraw();
  
  if (worldGUIIn != NULL) worldGUIIn->redraw();
  if (worldGUIOut != NULL) worldGUIOut->redraw();

  drawActions();
  drawLoading();
  RMGraphics->pop_camera();
}

void MapScene::startActive()
{
  releaseTransScreenshot();
  
  if (Pizza::gcLogonShown == false && RMSystem->get_OS_family() != MAC_FAMILY)
  {
    RMSystem->gamecenter_login();
    Pizza::gcLogonShown = true;
  }
}

void MapScene::updateWorld()
{
  if (worldGUIIn != NULL) worldGUIIn->update();
  
  if (worldGUIOut != NULL) 
  {
    worldGUIOut->update();

    if (worldGUIOut->state == RM::REMOVE) 
    {
      delete worldGUIOut;
      worldGUIOut = NULL;
    }
  }  
}

void MapScene::arrowPress(Coord1 dir)
{
  if (worldStable() == false) return;
  
  worldGUIOut = worldGUIIn;
  worldGUIOut->transOut(-dir);
  worldGUIIn = new WorldGUI(this, worldGUIOut->worldID + dir, dir);
  worldGUIIn->transIn(dir);
  
  Pizza::currWorldPair.x = worldGUIIn->worldID;
}

void MapScene::shopPress(Point2 shopTopLeft)
{
  Pizza::liteBtnDown = true;  // this disables full verion button
  zoomCam.handle = shopTopLeft + Point2(20.0, 32.0);
  
  ActionList* camActions = new ActionList();
  camActions->addX(new CosineFn(&zoomCam.zoom.x, 6.0, 0.5));
  camActions->addX(new CosineFn(&zoomCam.zoom.y, 6.0, 0.5), true);
  
  ShopScene* shop = new ShopScene();
  shop->transInHint = FROM_MAP;
  script.enqueueX(camActions);
  script.enqueueX(new SceneTransitionCommand(shop));
}

Logical MapScene::worldStable()
{
  return worldGUIIn->state == RM::ACTIVE;
}

// =============================== PizzaButton ============================ //

PizzaButton::PizzaButton(RivermanGUI* setParent) :
  Button(),
  upImg(NULL),
  downImg(NULL),
  drawOffset(0.0, 0.0),
  usesPlaySound(false),
  silentOverride(false)
{
  parentGUI = setParent;
  presser = Pizza::controller;
}

PizzaButton::PizzaButton(RivermanGUI* setParent, Point2 position, Image* setUpImg, Image* setDownImg) :
  Button(),
  upImg(NULL),
  downImg(NULL),
  drawOffset(0.0, 0.0),
  usesPlaySound(false),
  silentOverride(false)
{
  parentGUI = setParent;
  presser = Pizza::controller;  
  init(position, setUpImg, setDownImg);
}

void PizzaButton::init(Point2 position, Image* setUpImg, Image* setDownImg)
{
  setXY(position);
  upImg = setUpImg;
  downImg = setDownImg;
  setImage(downImg);
  if (getImage() != NULL) autoSize();
}

void PizzaButton::update()
{
  Button::update();
}

void PizzaButton::redraw()
{
  if (hoverPress() == true)
  {
    setImage(downImg);
  }
  else 
  {
    setImage(upImg);
  }

  if (getImage() != NULL) drawMe(drawOffset);
}

void PizzaButton::clickCallback()
{
  if (silentOverride == true)
  {

  }
  else if (usesPlaySound == false)
  {
    ResourceManager::playClickStd();
  }
  else
  {
    ResourceManager::playClickPlay();
  }
    
  Button::clickCallback();
}

// =============================== LevelButton ============================ //

LevelButton::LevelButton() :
  Button(),
  levelPos(WORLD_GRAVEYARD, 0),
  levelDef(LEVEL_GLADIATOR, GLADIATOR_TUTORIAL),
  isBoss(false),
  numStars(-1),

  regularImg(NULL),
  newAnimator(this, DataList<Image*>(imgsetCache[mapStarSet], 3, 5), 10.0 / 60.0),
  newBobVal(0.0),
  newBobData(0.5, 1.0),

  lockSwellScale(1.0),
  lockSwellData(0.5, 1.0),
  parentWorldGUI(NULL),

  pressScale(1.0),

  scareScript(this),
  warper(this, dwarpCache[mapSkullWarp]),
  weight(0.0)
{
  newAnimator.repeats();
  presser = Pizza::controller;
}

void LevelButton::init(Point2 position, Coord2 levelPosition, 
                       Image* regImg, WorldGUI* setParentWorldGUI)
{
  levelPos = levelPosition;
  levelDef = Pizza::getLevelData(levelPos).typeData;
  regularImg = regImg;
  parentWorldGUI = setParentWorldGUI;
  
  setXY(position);
  setWH(68.0, 68.0);
  
  Coord1 starCount = 0;
  Coord1 requiredStars = STARS_TO_UNLOCK[levelPosition.x][levelPosition.y];
  
  if (Pizza::playerStars >= requiredStars)
  {
    starCount = Pizza::getLevelData(levelPos).starsEarned;
  }
  else
  {
    starCount = -requiredStars;
  }
  
  setStars(starCount);
  
  if (levelDef.x == LEVEL_SUMO)
  {
    isBoss = true;
    callback(&scareScript);
  }
}

void LevelButton::update()
{
  Button::update();
  newAnimator.update();
  
  RM::bounce_arcsine(newBobVal, newBobData, Point2(-4.0, 4.0), RM::timePassed());
  RM::bounce_arcsine(lockSwellScale, lockSwellData, Point2(0.9, 1.1), RM::timePassed());
  
  Point1 pressTarget = hoverPress() ? 0.9 : 1.0;
  if (numStars < 0) pressTarget = 1.0;
  RM::flatten_me(pressScale, pressTarget, RM::timePassed());
  
  scareScript.update();
}

void LevelButton::redraw()
{  
  // locked
  if (numStars < 0)
  {
    setScale(pressScale, pressScale);
    
    setImage(imgsetCache[mapLockSet][0]);
    drawMe();
    
    setScale(0.75 * pressScale, 0.75 * pressScale);
    drawText(Point2(0.0, 12.0));
  }
  // need to touch lock
  else if (readyToUnlock() == true)
  {
    setScale(lockSwellScale, lockSwellScale);
    setImage(imgsetCache[mapLockSet][RM::randi(1, 3)]);
    drawMe();
  }
  else
  {
    setScale(pressScale, pressScale);
    
    if (isBoss == false) 
    {
      setImage(regularImg);
      drawMe();
      
      setImage(imgsetCache[mapNumbers][levelPos.y]);
      drawMe();
    }
    else
    {
      setImage(regularImg);
      warper.draw(weight);
      // drawMe();      
    }
    
    if (numStars == 0)
    {
      setImage(newAnimator.currImage());
      drawMe(Point2(0.0, newBobVal));
    }
    else 
    {
      setImage(imgsetCache[mapStarSet][numStars - 1]);
      drawMe();
    }
    
  }
}

void LevelButton::setStars(Coord1 setNumStars)
{
  numStars = setNumStars;
  setText(String2("$", '$', std::abs(numStars)), ResourceManager::font, Point2(0.5, 0.5));
}
           
Logical LevelButton::readyToUnlock()
{
  return numStars == 0 && Pizza::getLevelData(levelPos).lockBroken == false;
}

void LevelButton::clickCallback()
{
  // cout << "click" << endl;
  
  if (readyToUnlock() == true)
  {
    // cout << "unlock" << endl;
    setScale(pressScale, pressScale);
    setImage(imgsetCache[mapLockSet][0]);
    FishShatter* shatter = new FishShatter(*this, 5, 5);
    
    parentWorldGUI->actions.addX(shatter);
    
    setAlpha(0.0);
    parentWorldGUI->actions.addX(new LinearFn(&color.w, 1.0, 1.0));
    Pizza::getLevelData(levelPos).lockBroken = true;

    ResourceManager::playLockBreak();

    for (Coord1 sparkI = 0; sparkI < 10; ++sparkI) 
    {
      parentWorldGUI->actions.addX(new WinSpark(collisionBox().center()));
    }
    
    parentWorldGUI->actions.addX(new Smokeburst(collisionBox().center(), false));
  }
  else if (numStars >= 0) 
  {
    // cout << "open" << endl;
    ResourceManager::playClickPlay();
    Button::clickCallback();
  }
  else
  {
    ResourceManager::playCantClick();
  }
}

Box LevelButton::collisionBox()
{
  return boxFromC();
}

void LevelButton::callback(ActionEvent* caller)
{
  scareScript.wait(3.0);
  scareScript.enqueueX(new CosineFn(&weight, 1.0, 0.1));
  scareScript.wait(0.3);
  scareScript.enqueueX(new CosineFn(&weight, 0.0, 0.1));
}

// =============================== ShopButton ============================ //

ShopButton::ShopButton(RivermanGUI* setParent) : 
  Button(),
  warper(this, dwarpCache[mapShopWarp]),
  weight(0.0),
  bounceData(0.0, 1.0)
{
  presser = Pizza::controller;
  parentGUI = setParent;
  setImage(imgCache[mapShop]);
  autoSize(0.8);
}

void ShopButton::update()
{
  Button::update();

  RM::bounce_arcsine(weight, bounceData, Point2(0.0, 1.0), RM::timePassed());
}

void ShopButton::redraw()
{
  if (hoverPress() == true) setImage(imgCache[mapShopDown]);
  else setImage(imgCache[mapShop]);
    
  warper.draw(1, 2, weight);
  // drawMe();
}

Box ShopButton::collisionBox()
{
  return boxFromC();
}

void ShopButton::clickCallback()
{
  ResourceManager::playClickStd();
  Button::clickCallback();
}

// =============================== SoundCheckbox ============================ //

SoundCheckbox::SoundCheckbox(RivermanGUI* setParent) :
  Button()
{
  presser = Pizza::controller;
  parentGUI = setParent;
  updateImage();
}

void SoundCheckbox::update()
{
  Button::update();
  updateImage();
}

void SoundCheckbox::redraw()
{
  drawMe();
}

void SoundCheckbox::updateImage()
{
  Logical soundOn = Sound::group_enabled(Sound::TYPE_MUSIC);
  
  if (soundOn == true)
  {
    if (hoverPress() == false) setImage(imgsetCache[soundBoxSet][0]);
    else setImage(imgsetCache[soundBoxSet][1]);
  }
  else
  {
    if (hoverPress() == false) setImage(imgsetCache[soundBoxSet][2]);
    else setImage(imgsetCache[soundBoxSet][3]);
  }
}

void SoundCheckbox::clickCallback()
{
  Button::clickCallback();
  ResourceManager::playClickStd();
}

// =============================== MainGUI ============================ //

MainGUI::MainGUI() :
  PizzaGUI(),
  SystemMoreGamesListener(),

  scene(NULL),
  
  leftArrow(this, Point2(0.0, 120.0), imgsetCache[mapArrowSet][0], imgsetCache[mapArrowSet][2]),
  rightArrow(this, Point2(436.0, 120.0), imgsetCache[mapArrowSet][1], imgsetCache[mapArrowSet][3]),

  liteButton(this, Point2(-16.0, -32.0), NULL, NULL),

  shopButton(this),
  socialButtons(4, true),

  moregamesBtn(&imgsetCache[moregamesButtonSet], &imgsetCache[moregamesAnimSet]),

  starGlowAlpha(0.0),
  starGlowData(0.0, 1.0),

  drawBubble(Pizza::showShopTut),
  bubbleBobY(0.0),
  bubbleBobData(0.5, 1.0),

  gcMessageTimer(2.0),
  gcRecentlyPressed(false),

  yellowStarNum(false),

  lastDirPressed(0)
{
  leftArrow.autoSize();
  leftArrow.setAlpha(canGoLeft() ? 1.0 : 0.0);
  widgetList.add(&leftArrow);
  
  rightArrow.autoSize();
  rightArrow.setAlpha(canGoRight() ? 1.0 : 0.0);
  widgetList.add(&rightArrow);
  
  liteButton.setWH(256.0, 96.0);
  if (Pizza::lite == true) addWidget(&liteButton);

  shopButton.setXY(264.0, 296.0);
  widgetList.add(&shopButton);

  for (Coord1 i = 0; i < socialButtons.count; ++i)
  {
    socialButtons[i].init(Point2(288.0 + 48.0 * i, 272.0), imgsetCache[mapSocialIcons][i],
                          imgsetCache[mapSocialIcons][i + socialButtons.count]);
    socialButtons[i].parentGUI = this;
    socialButtons[i].setWH(40.0, 40.0);
    widgetList.add(&socialButtons[i]);
    
    if (i == SOCIAL_SHARE && RMSystem->get_OS_family() == APPLETV_FAMILY)
    {
      socialButtons[i].disable();
      socialButtons[i].setAlpha(0.0);
    }
  }
  
  if (Pizza::lite == true)
  {
    socialButtons[SOCIAL_ACHIEVEMENT].setAlpha(0.0);
    socialButtons[SOCIAL_ACHIEVEMENT].disable();
  }

  moregamesBtn.setXY(200.0, 276.0);
  moregamesBtn.setWH(40.0, 64.0);
  moregamesBtn.drawOffset.x += 8.0;
  moregamesBtn.presser = Pizza::controller;
  
  // if (RMSystem->get_OS_family() != IOS_FAMILY)
  {
    moregamesBtn.disable();
    moregamesBtn.setAlpha(0.0);
  }
  
  addWidget(&moregamesBtn);

  gcMessageTimer.setInactive();
  yellowStarNum = Pizza::starsInFirst100() >= 300;
  
  exclusive = false;
  state = RM::ACTIVE;
}

void MainGUI::update()
{
  Point1 leftTargetAlpha = 0.0;
  Point1 rightTargetAlpha = 0.0;

  if (scene->worldStable() == true)
  {
    if (canGoLeft() == true) leftTargetAlpha = 1.0;
    if (canGoRight() == true) rightTargetAlpha = 1.0;
  }
  
  checkForGCFail();
  
  RM::flatten_me(leftArrow.color.w, leftTargetAlpha, RM::timePassed() * 8.0);
  RM::flatten_me(rightArrow.color.w, rightTargetAlpha, RM::timePassed() * 8.0);
  
  leftArrow.disable();
  if (leftArrow.getAlpha() > 0.99) leftArrow.enable();

  rightArrow.disable();
  if (rightArrow.getAlpha() > 0.99) rightArrow.enable();
  
  RM::bounce_arcsine(starGlowAlpha, starGlowData, Point2(0.0, 1.0), RM::timePassed() * 0.5);
  RM::bounce_arcsine(bubbleBobY, bubbleBobData, Point2(-4.0, 4.0), RM::timePassed());
  
  syncMasterAlpha();
  
  updateActions();
  updateDirHighlight();
  // cout << "box " << highlightBox << endl;
  
  Pizza::liteBtnDown = liteButton.hoverPress();
}

void MainGUI::redraw()
{
  // this is in draw because it doesn't update while instructions is active
  if (Pizza::worldMapAlpha < 0.99) syncMasterAlpha();

  drawActions();
  drawDirHighlight();

  drawStars();
  drawScore();

  // unlockBtn.collisionBox().draw_solid(YELLOW_SOLID);

  if (drawBubble == true)
  {    
    imgCache[mapBubbleShop]->draw_scale(Point2(220.0, 240.0 + bubbleBobY), Point2(1.0, 1.0),
                                        0.0, ColorP4(1.0, 1.0, 1.0, Pizza::worldMapAlpha));
  }
}

void MainGUI::checkForGCFail()
{
  gcMessageTimer.update();

  if (gcRecentlyPressed == true && 
      Pizza::scoreMan->recentLoginFailed == true &&
      gcMessageTimer.getActive() == false)
  {
    failMsgStd(LSTRINGS[77], 88.0);
    failMsgStd(LSTRINGS[78], 108.0);

    gcMessageTimer.reset();
    gcRecentlyPressed = false;
    Pizza::scoreMan->recentLoginFailed = false;
  }
}

void MainGUI::drawStars()
{
  imgCache[mapStarCount]->draw_scale(Point2(380.0, 4.0), Point2(1.0, 1.0), 0.0,
                                            ColorP4(1.0, 1.0, 1.0, Pizza::worldMapAlpha));
  
  ColorP4 glowColor(1.0, 1.0, 1.0, std::min(Pizza::worldMapAlpha, starGlowAlpha));
  imgCache[mapStarCountGlow]->draw_scale(Point2(380.0, 4.0), Point2(1.0, 1.0), 0.0, glowColor);
}

void MainGUI::drawScore()
{
  String2 starStr(8);
  
  ColorP4 numberColor = ColorP4(1.0, 1.0, 1.0, Pizza::worldMapAlpha);
  
  if (yellowStarNum == true)
  {
    numberColor = RM::color255(255, 224, 0, 255);
    numberColor.w = Pizza::worldMapAlpha;
  }

  starStr.int_string(Pizza::playerStars);
  ResourceManager::font->draw(starStr, Point2(430.0, 8.0), Point2(0.75, 0.75),
                              numberColor);
  
  starStr.int_string(Pizza::playerScore);
  ResourceManager::mapScoreFont->draw(starStr, Point2(6.0, 280.0), Point2(1.0, 1.0), 
                                      numberColor);
}

void MainGUI::syncMasterAlpha()
{
  if (Pizza::worldMapAlpha > 0.99) return;
  
  leftArrow.setAlpha(std::min(leftArrow.getAlpha(), Pizza::worldMapAlpha));
  rightArrow.setAlpha(std::min(rightArrow.getAlpha(), Pizza::worldMapAlpha));
  
  for (Coord1 i = 0; i < socialButtons.count; ++i)
  {
    if (socialButtons[i].isEnabled() == true)
    {
      socialButtons[i].setAlpha(Pizza::worldMapAlpha);
    }
  }
  
  shopButton.setAlpha(Pizza::worldMapAlpha);
  if (Pizza::lite == true) socialButtons[SOCIAL_ACHIEVEMENT].setAlpha(0.0);
  
  if (moregamesBtn.isEnabled() == true)
  {
    moregamesBtn.setAlpha(Pizza::worldMapAlpha);
  }
}

Logical MainGUI::canGoLeft()
{
  return Pizza::isWorldUnlocked(Pizza::currWorldPair.x - 1);
}

Logical MainGUI::canGoRight()
{
  return Pizza::isWorldUnlocked(Pizza::currWorldPair.x + 1);
}

void MainGUI::opened_interface(Coord1 value)
{
  // moregames button
  if (value == MoreGamesButtonStd::RM_STORE_ID)
  {
    soundCache[moreRivermanSound]->play();
  }
  // interstitial interface
  else if (value == 0)
  {
    soundCache[music]->stop();
    soundCache[rivermanInterstitialSound]->play();
  }
  // download button from interstitial interface
  else
  {
    // do nothing
  }

  // cout << "opened_interface with value " << value << endl;
}

void MainGUI::closed_interface(Coord1 value)
{
  // moregames button
  if (value == MoreGamesButtonStd::RM_STORE_ID)
  {
    // do nothing
  }
  // interstitial interface
  else if (value == 0)
  {
    soundCache[music]->play();
  }
  // download button from interstitial interface
  else
  {
    soundCache[moreRivermanSound]->play();
  }

  // cout << "closed_interface with value " << value << endl;
}

void MainGUI::failMsgStd(const String2& msg, Point1 yCenterOffset)
{
  BonusText* failMsg = new BonusText(msg, ResourceManager::font, PLAY_CENTER + Point2(0.0, yCenterOffset));
  failMsg->scale.set(0.65, 0.65);
  failMsg->upwardVal = 0.0;
  failMsg->holdTime = 1.0;
  failMsg->init();
  actions.addX(failMsg);
}

void MainGUI::resetNodeGraph()
{
  if (scene == NULL)
  {
    return;
  }
  
  clearAllNodes();
  
  ArrayList<LevelButton>& levelButtons = scene->worldGUIIn->levelButtons;
  
  PizzaDirNode* leftNode = addNode(&leftArrow, &levelButtons.first(), &levelButtons[5], NULL, &levelButtons[0]);
  leftNode->highlightPosOffset += Point2(0.0, 4.0);
  leftNode->highlightSizeOffset += Point2(-4.0, -4.0);
  
  PizzaDirNode* rightNode = addNode(&rightArrow, NULL, &levelButtons[9], &levelButtons[4], &levelButtons[4]);
  rightNode->highlightPosOffset += Point2(4.0, 4.0);
  rightNode->highlightSizeOffset += Point2(-4.0, -4.0);
  

  // top row of level buttons
  for (Coord1 i = 0; i < 5; ++i)
  {
    Widget* rightNode = &levelButtons[i + 1];
    
    if (i == 4)
    {
      rightNode = &rightArrow;
    }
    
    Widget* leftNode = NULL;
    
    if (i == 0)
    {
      leftNode = &leftArrow;
    }
    else
    {
      leftNode = &levelButtons[i - 1];
    }

    addNode(&levelButtons[i], rightNode, &levelButtons[i + 5], leftNode, NULL);
  }
  
  // bottom row of level buttons
  for (Coord1 i = 5; i < 10; ++i)
  {
    Widget* rightNode = NULL;
    
    if (i <= 8)
    {
      rightNode = &levelButtons[i + 1];
    }
    else
    {
      rightNode = &rightArrow;
    }
    
    Widget* leftNode = NULL;
    
    if (i == 5)
    {
      leftNode = &leftArrow;
    }
    else
    {
      leftNode = &levelButtons[i - 1];
    }
    
    Widget* bottomNode = &shopButton;
    
    if (i == 8)
    {
      if (socialButtons[SOCIAL_ACHIEVEMENT].isEnabled() == true)
      {
        bottomNode = &socialButtons[0];
      }
      else
      {
        bottomNode = &socialButtons[1];
      }
    }
    
    if (i == 9)
    {
      if (socialButtons[SOCIAL_SHARE].isEnabled() == true)
      {
        bottomNode = &socialButtons[SOCIAL_SHARE];
      }
      else
      {
        bottomNode = &socialButtons[SOCIAL_CREDITS];
      }
    }
    
    addNode(&levelButtons[i], rightNode, bottomNode, leftNode, &levelButtons[i - 5]);
  }
  
  addNode(&rightArrow, &leftArrow, &levelButtons.last(), &levelButtons[4], &levelButtons[4]);
  
  addNode(&shopButton, &socialButtons.first(), NULL, &socialButtons.last(), &levelButtons[7]);

  for (Coord1 i = 0; i < socialButtons.count; ++i)
  {
    Widget* leftNode = NULL;
    
    if (i == 0)
    {
      leftNode = &shopButton;
    }
    else
    {
      leftNode = &socialButtons[i - 1];
    }
    
    Widget* rightNode = NULL;

    if (i == socialButtons.last_i())
    {
      rightNode = &shopButton;
    }
    else
    {
      rightNode = &socialButtons[i + 1];
    }
    
    Widget* upNode = NULL;

    if (i <= 1)
    {
      upNode = &levelButtons[8];
    }
    else
    {
      upNode = &levelButtons[9];
    }
    
    addNode(&socialButtons[i], rightNode, NULL, leftNode, upNode);
  }
  
  if (lastDirPressed == 0)
  {
    // didn't just hit an arrow, use the current level
    setFirstNode(&levelButtons[Pizza::currWorldPair.y]);
  }
  else if (lastDirPressed == -1)
  {
    if (canGoLeft() == true)
    {
      setFirstNode(&leftArrow);
    }
    else
    {
      setFirstNode(&levelButtons[0]);
    }
  }
  else if (lastDirPressed == 1)
  {
    if (canGoRight() == true)
    {
      setFirstNode(&rightArrow);
    }
    else
    {
      setFirstNode(&levelButtons[0]);
    }
  }
  
  if (currNode == NULL)
  {
    setFirstNode(&levelButtons[0]);
  }
  
  for (Coord1 i = 0; i < socialButtons.count; ++i)
  {
    PizzaDirNode* node = findNode(&socialButtons[i]);
    
    if (node != NULL)
    {
      node->highlightPosOffset += Point2(4.0, 4.0);
    }
  }
}

void MainGUI::click(Widget* caller)
{
  if (caller == &leftArrow)
  {
    lastDirPressed = -1;
    scene->arrowPress(-1);
    resetNodeGraph();
  }
  else if (caller == &rightArrow)
  {
    lastDirPressed = 1;
    scene->arrowPress(1);
    resetNodeGraph();
  }
  else if (caller == &socialButtons[0])
  {
    gcRecentlyPressed = true;
    Pizza::scoreMan->recentLoginFailed = false;
    RMSystem->analytics_event("View achievements");
    RMSystem->gamecenter_show_achievements();
  }
  else if (caller == &socialButtons[1])
  {
    gcRecentlyPressed = true;
    Pizza::scoreMan->recentLoginFailed = false;
    RMSystem->analytics_event("View leaderboards");
    RMSystem->gamecenter_show_leaderboards();
  }
  else if (caller == &socialButtons[2])
  {
    // share
    RMSystem->analytics_event("Share score");
    RMSystem->facebook_post(String2(LSTRINGS[106], '$', Pizza::playerScore));
  }
  else if (caller == &socialButtons[3])
  {
    RMSceneMan->pushGUI(new CreditsGUI());
  }
  else if (caller == &shopButton)
  {
    Pizza::showShopTut = false;
    scene->shopPress(shopButton.collisionBox().xy);
    state = RM::TRANS_OUT;
  }
  else if (caller == &liteButton)
  {
    Pizza::useLoadImg = true;
    RMSceneMan->sceneTransition(new UpgradeScene(true));
  }
  else if (caller == &moregamesBtn)
  {
    RMSystem->analytics_event("More games button");
    RMSystem->view_store(MoreGamesButtonStd::RM_STORE_ID, String1("more_games_button_v1"));
  }
  else
  {
    ArrayList<LevelButton>& levelButtons = scene->worldGUIIn->levelButtons;
    Coord1 foundBtnIndex = -1;
    
    for (Coord1 i = 0; i < levelButtons.count; ++i)
    {
      if (caller == &levelButtons[i])
      {
        foundBtnIndex = i;
        break;
      }
    }
    
    if (foundBtnIndex != -1)
    {
      levelButtons[foundBtnIndex].clickCallback();
    }
  }
}

// =============================== WorldGUI ============================ //

WorldGUI::WorldGUI(MapScene* setScene, Coord1 setWorldID, Coord1 startSide) :
  RivermanGUI(),

  scene(setScene),
  guiCam(),
  worldID(setWorldID),
  
  background(-124.0, 352.0),

  levelIcons(WORLD_LEVELS[worldID].count),
  levelButtons(10, true)
{
  guiCam.setX(648.0 * -startSide);

  String1 bgStr("map_base_$", imgSuffix);
  bgStr.int_replace('$', worldID + 1);
  
  background.setImage(Image::create_handled_norm(bgStr, Point2(0.0, 1.0)));
  
  // load icons
  Image::create_tiles_abs(ResourceManager::imgStr(String1("level_icons_$", '$', worldID).as_cstr()), 
                          levelIcons, 9, 1, Point2(28.0, 28.0));
  levelIcons.add(Image::create_handled_abs(ResourceManager::imgStr("map_icons_skull"), Point2(36.0, 44.0)));
  levelIcons.last()->ownsTexture = true;
  
  Point2 iconCoords[10] = 
  {
    Point2(80.0, 120.0),
    Point2(160.0, 120.0),
    Point2(240.0, 120.0),
    Point2(320.0, 120.0),
    Point2(400.0, 120.0),
    Point2(80.0, 200.0),
    Point2(160.0, 200.0),
    Point2(240.0, 200.0),
    Point2(320.0, 200.0),
    Point2(400.0, 200.0)    
  };
  
  for (Coord1 i = 0; i < levelButtons.count; ++i)
  {
    levelButtons[i].init(iconCoords[i], Coord2(worldID, i), levelIcons[i], this);
    levelButtons[i].parentGUI = this;
    widgetList.add(&levelButtons[i]);
  }
      
  actions.addX(new WorldBannerEffect(worldID, true));
  
  exclusive = false;
  state = RM::ACTIVE;
}

WorldGUI::~WorldGUI()
{
  free_clear(levelIcons);
  delete background.getImage();
}

void WorldGUI::update()
{
  script.update();
  actions.update();
  
  if (state == RM::ACTIVE && 
      RMSceneMan->getCurrScene()->guiList.count == 1)
  {
    updateWidgets();
  }
}

void WorldGUI::redraw()
{
  RMGraphics->push_camera(guiCam);
  background.drawMe();
  
  // scroll the icons at double speed
  Point1 camX = guiCam.getX();
  guiCam.setX(camX * 2.0);
  guiCam.apply();
  drawWidgets();
  guiCam.setX(camX);
  guiCam.apply();
  
  actions.redraw();
  script.redraw();
  
  RMGraphics->pop_camera();
}

void WorldGUI::transIn(Coord1 startFromSide)
{
  script.enqueueX(new AccelCurveFn(&guiCam.xy.x, 0.0, 0.5));
  state = RM::TRANS_IN;
}

void WorldGUI::transOut(Coord1 goToSide)
{
  script.enqueueX(new AccelCurveFn(&guiCam.xy.x, 648.0 * -goToSide, 0.5));
  state = RM::TRANS_OUT;
}

void WorldGUI::resolveGUI()
{
  state = RM::REMOVE;
}

void WorldGUI::click(Widget* caller)
{
  for (Coord1 i = 0; i < levelButtons.count; ++i)
  {
    if (caller == &levelButtons[i])
    {
      Pizza::currWorldPair.y = i;
      Pizza::currLevelDef = Pizza::currLevelData().typeData;
      RMSceneMan->pushGUI(new InstructionsGUI(levelButtons[i].getXY()));
    }
  }
}

// =============================== WorldBannerEffect ============================ //

WorldBannerEffect::WorldBannerEffect(Coord1 setWorldNum, Logical startIn) :
  VisRectangular(125.0, 28.0),
  Effect(),

  warper(this, dwarpCache[mapChapterWarp]),
  weight(0.0),

  alphaChange(&color.w, 1.0, 1.5),
  scaleChange(&scale.x, 1.0, 1.5, this),

  worldNum(setWorldNum),
  state(RM::TRANS_IN)
{
  if (Pizza::lite == true)
  {
    upImg = Image::create_centered(ResourceManager::imgStr("map_chapter_full_version&"));
    downImg = Image::create_centered(ResourceManager::imgStr("map_chapter_full_version_down&"));
  }
  else
  {
    String1 bannerStr(ResourceManager::imgStr("map_chapter_$&"));
    bannerStr.int_replace('$', worldNum + 1);
    
    upImg = Image::create_centered(bannerStr);
    downImg = upImg;
  }
  
  setScale(2.0, 2.0);
  setAlpha(0.0);
  
  if (startIn == true)
  {
    setScale(1.0, 1.0);
    setAlpha(1.0);
    alphaChange.setActive(false);
    scaleChange.setActive(false);
    state = RM::ACTIVE;
  }
}

WorldBannerEffect::~WorldBannerEffect()
{
  if (downImg != upImg) delete downImg;
  delete upImg;
}

void WorldBannerEffect::updateMe()
{
  alphaChange.update();
  scaleChange.update();
  setYScale(getXScale());
  
  weight += 0.1 * RM::timePassed();
  RM::wrap1_me(weight, 0.0, 1.0);
}

void WorldBannerEffect::redrawMe()
{
  if (Pizza::alreadyBeatenGame() == true)
  {
    setColor(RM::color255(255, 224, 0, 255));
  }
  
  // this is in draw because it doesn't update while instructions is active
  if (Pizza::worldMapAlpha < 0.99) setAlpha(Pizza::worldMapAlpha);
  else setAlpha(1.0);

  setImage(Pizza::liteBtnDown ? downImg : upImg);
  warper.draw(weight);
}

Box WorldBannerEffect::collisionBox()
{
  return boxFromC();
}

void WorldBannerEffect::callback(ActionEvent* caller)
{
  if (state == RM::TRANS_IN)
  {
    state = RM::ACTIVE;
  }
  else if (state == RM::TRANS_OUT)
  {
    done();
  }
}

// =============================== InstructionsGUI ============================ //

const Point1 InstructionsGUI::TRANS_TIME = 0.50;

InstructionsGUI::InstructionsGUI(Point2 center) :
  PizzaGUI(),

  comicImg(NULL),

  instructionsCam(),

  play(this),
  close(this),

  levelText(16, ResourceManager::instructionsFont, Point2(0.5, 0.0)),
  scoreText(16, ResourceManager::scoreFont, Point2(1.0, 0.0)),

  startCenter(center),
  startScale(0.12, 0.12)
{
  instructionsCam.zoom = startScale;
  instructionsCam.handle = startCenter;
  
  window.setXY(16.0, 16.0);
  window.setImage(new Image(ResourceManager::imgStr("ui_overlay")));
  window.setAlpha(0.0);
  
  String1 filename;
  Pizza::getInstructionsStr(Pizza::currLevelData(), filename);
  
  comicImg = new Image(filename);
  
  play.init(Point2(352.0, 244.0), imgsetCache[instructionsPlaySet][0], imgsetCache[instructionsPlaySet][1]);
  play.usesPlaySound = true;
  addWidget(&play);
  
  close.init(Point2(28.0, 244.0), imgsetCache[instructionsCloseSet][0], imgsetCache[instructionsCloseSet][1]);
  addWidget(&close);
  
  addNode(&play, &close, NULL, &close, NULL);
  addNode(&close, &play, NULL, &play, NULL);
  setFirstNode(&play);
  
  levelText.setXY(240.0, 38.0);
  levelText.getTextPtr()->set(".-$/%");  // "LEVEL $-%"
  levelText.getTextPtr()->int_replace('$', Pizza::currWorldPair.x + 1);
  levelText.getTextPtr()->int_replace('%', Pizza::currWorldPair.y + 1);
  levelText.textChanged();

  scoreText.setXY(439.0, 54.0);

  Coord1 levelScore = Pizza::currLevelData().bestScore;
  scoreText.getTextPtr()->int_string(levelScore);
  scoreText.textChanged();

  syncDrawers();
  
  actions.addX(new LinearFn(&Pizza::worldMapAlpha, 0.0, Pizza::GUI_TRANS_DUR));
  actions.addX(new LinearFn(&window.color.w, 1.0, Pizza::GUI_TRANS_DUR * 0.5));

  script.enqueueX(new CosineFn(&instructionsCam.zoom.x, 1.0, Pizza::GUI_TRANS_DUR));
  state = RM::TRANS_IN;
}

InstructionsGUI::~InstructionsGUI()
{
  delete window.getImage();
  delete comicImg;
}

void InstructionsGUI::update()
{
  updateActions();
  updateDirHighlight();
  syncDrawers();
}

void InstructionsGUI::redraw()
{
  RMGraphics->push_camera(instructionsCam);
  
  window.drawMe();
  
  comicImg->draw_scale(Point2(40.0, 88.0), Point2(1.0, 1.0), 0.0, 
                       ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  
  imgCache[instructionsHiScore]->draw_scale(Point2(384.0, 40.0), Point2(1.0, 1.0), 0.0, 
                                            ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  
  for (Coord1 i = 0; i < Pizza::currLevelData().starsEarned; ++i)
  {
    imgCache[instructionsStar]->draw_scale(Point2(40.0 + i * 24.0, 48.0), Point2(1.0, 1.0), 0.0, 
                                           ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  }
  
  levelText.drawText();
  scoreText.drawText();
  
  drawActions();
  drawDirHighlight();
  
  RMGraphics->pop_camera();
}

void InstructionsGUI::syncDrawers()
{
  instructionsCam.zoom.y = instructionsCam.zoom.x;
  play.setAlpha(window.getAlpha());
  close.setAlpha(window.getAlpha());
  levelText.setAlpha(window.getAlpha());
  scoreText.setAlpha(window.getAlpha());
}

void InstructionsGUI::click(Widget* caller)
{  
  if (caller == &play)
  {
    Pizza::useLoadImg = true;
    PlayScene* scene = new PlayScene();
    scene->transInHint = PizzaScene::FROM_MAP;    
    RMSceneMan->sceneTransition(scene);
    
    state = RM::TRANS_OUT;
  }
  else if (caller == &close)
  {
    actions.addX(new LinearFn(&Pizza::worldMapAlpha, 1.0, Pizza::GUI_TRANS_DUR));
    actions.addX(new LinearFn(&window.color.w, 0.0, Pizza::GUI_TRANS_DUR));
    
    script.enqueueX(new CosineFn(&instructionsCam.zoom.x, startScale.x, Pizza::GUI_TRANS_DUR));
    state = RM::TRANS_OUT;
  }
}

// =============================== CreditsGUI ============================ //

CreditsGUI::CreditsGUI() :
  PizzaGUI(),
  transPercent(0.0),
  creditsDrawer(Point2(28.0, 36.0), new Image(ResourceManager::imgStr("credits&")))
{
  window.setXY(16.0, 16.0);
  window.setImage(new Image(ResourceManager::imgStr("ui_overlay")));
  window.setAlpha(0.0);
  
  state = RM::TRANS_IN;
  actions.addX(new LinearFn(&Pizza::worldMapAlpha, 0.0, Pizza::GUI_TRANS_DUR));
  script.enqueueX(new LinearFn(&transPercent, 1.0, Pizza::GUI_TRANS_DUR));
}

CreditsGUI::~CreditsGUI()
{
  delete creditsDrawer.getImage();
  creditsDrawer.setImage(NULL);
  
  delete window.getImage();
  window.setImage(NULL);
}

void CreditsGUI::update()
{
  if (state == RM::ACTIVE &&
      Pizza::controller->getAnyConfirmStatus(JUST_PRESSED) == true)
  {
    state = RM::TRANS_OUT;
    setNextPop();
    actions.addX(new LinearFn(&Pizza::worldMapAlpha, 1.0, Pizza::GUI_TRANS_DUR));
    script.enqueueX(new LinearFn(&transPercent, 0.0, Pizza::GUI_TRANS_DUR));
  }

  updateActions();
}

void CreditsGUI::redraw()
{
  window.setAlpha(transPercent);
  if (window.getImage() != NULL) window.drawMe();
  
  creditsDrawer.setAlpha(transPercent);
  if (creditsDrawer.getImage() != NULL) creditsDrawer.drawMe();

  ColorP4 versionColor = ColorP4(1.0, 1.0, 1.0, transPercent);
  String1 versionStr;
  RMSystem->get_version_str(versionStr);

  ResourceManager::font->draw(String2(versionStr.as_cstr()), Pizza::platformTL,
                              Point2(0.65, 0.65), versionColor);
}


// =============================== ShopScene ============================ //

ShopScene::ShopScene() :
  PizzaScene(),
  sky()
{
  
}

ShopScene::~ShopScene()
{
  ResourceManager::unloadShop();
}

void ShopScene::load()
{
  ResourceManager::loadShop();
  
  sky.init(imgCache[bg], Point2(-15.0, 0.0), Point2(Pizza::platformTL.x, -32.0),
           Point2(SCREEN_W, imgCache[bg]->natural_size().y));
  
  RMSystem->analytics_event("Entered Shop");
  pushGUI(new ShopGUI());
  
  transIn();  
}

void ShopScene::updateMe()
{
  sky.update();
  updateActions();
  if (state == RM::ACTIVE) Pizza::controller->update();
}

void ShopScene::redraw()
{
  sky.redraw();
  imgCache[shopBaseLeft]->draw(-44.0, 200.0);
  imgCache[shopBaseRight]->draw(208.0, 0.0);
  
  drawActions();
}

void ShopScene::startActive()
{
  releaseTransScreenshot();
}

// =============================== ShopGUI ============================ //

ShopGUI::ShopGUI() :
  PizzaGUI(),

  pizza(Point2(120.0, 122.0)),
  pizzaDrawer(&pizza),

  done(this, Point2(8.0, 280.0), NULL, imgCache[shopDone]),
  share(this, Point2(124.0, 280.0), NULL, imgCache[shopShare]),
  tabClickers(NUM_PIZZA_LAYERS, true),
  itemClickers(16, true),

  cashDrawer(16, ResourceManager::shopCashFont),
  itemDrawer(32, ResourceManager::shopItemFont, Point2(0.5, 0.5)),

  selector(),
  selScaler(&selector.scale.x, 1.0, 0.3),
  selFader(&selector.color.w, 1.0, 0.15),

  currTab(0),
  currItem(0),

  postedBuy(false),
  postedRetro(false),

  clickCount(0)
{
  pizza.setScale(0.6, 0.6);
  
  done.setWH(108.0, 32.0);
  widgetList.add(&done);

  share.setWH(108.0, 32.0);
  widgetList.add(&share);
  
  if (RMSystem->get_OS_family() == APPLETV_FAMILY)
  {
    share.disable();
    share.setAlpha(0.0);
  }
  
  for (Coord1 i = 0; i < tabClickers.count; ++i)
  {
    tabClickers[i].setImage(imgsetCache[shopTabSet][i]);
    tabClickers[i].setWH(48.0, 40.0);
    tabClickers[i].setXY(232.0 + i * tabClickers[i].getWidth(), 8.0);
    tabClickers[i].parentGUI = this;
    tabClickers[i].presser = Pizza::controller;
  }
  
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    itemClickers[i].setWH(52.0, 48.0);
    itemClickers[i].setXY(248.0 + (i % 4) * itemClickers[i].getWidth(), 
                          110.0 + (i / 4) * itemClickers[i].getHeight());
    itemClickers[i].parentGUI = this;
    itemClickers[i].presser = Pizza::controller;
    widgetList.add(&itemClickers[i]);
  }
  
  setTab(CHEESE_LAYER);

  cashDrawer.setXY(Point2(6.0, 6.0));
  setPlayerCashText();
  
  itemDrawer.setXY(352.0, 100.0);
  setItemText();
  
  selector.setImage(imgCache[shopSelector]);
  selector.setXY(itemClickers[currItem].getXY() + Point2(26.0, 24.0));
  selScaler.setActive(false);
  selFader.setActive(false);

  addNode(&done, &share, NULL, &itemClickers[15], &tabClickers[0]);
  PizzaDirNode* shareNode = addNode(&share, &itemClickers[12], &tabClickers[0], &done, NULL);
  shareNode->highlightPosOffset.x -= 1.0;
  shareNode->highlightSizeOffset.x -= 6.0;
  
  // tabs
  addNode(&tabClickers[0], &tabClickers[1], &itemClickers[0], &share, &itemClickers[12]);
  addNode(&tabClickers[1], &tabClickers[2], &itemClickers[1], &tabClickers[0], &itemClickers[13]);
  addNode(&tabClickers[2], &tabClickers[3], &itemClickers[2], &tabClickers[1], &itemClickers[14]);
  addNode(&tabClickers[3], &tabClickers[4], &itemClickers[3], &tabClickers[2], &itemClickers[15]);
  addNode(&tabClickers[4], &done, &itemClickers[3], &tabClickers[3], &itemClickers[15]);
  
  // items
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    Widget* rightNode = NULL;
    Widget* downNode = NULL;
    Widget* leftNode = &share;
    Widget* upNode = NULL;
    
    if (i % 4 != 3)
    {
      rightNode = &itemClickers[i + 1];
    }
    else
    {
      rightNode = &done;
    }
    
    if (i / 4 != 3)
    {
      downNode = &itemClickers[i + 4];
    }
    else if (i >= 12)
    {
      downNode = &tabClickers[i % 4];
    }
    
    if (i % 4 != 0)
    {
      leftNode = &itemClickers[i - 1];
    }
    
    if (i < 4)
    {
      upNode = &tabClickers[i];
    }
    else
    {
      upNode = &itemClickers[i - 4];
    }
    
    PizzaDirNode* itemNode = addNode(&itemClickers[i], rightNode, downNode, leftNode, upNode);
    itemNode->highlightSizeOffset += Point2(-4.0, -4.0);
    itemNode->highlightPosOffset += Point2(2.0, 2.0);
  }
  
  setFirstNode(&done);
  
  Pizza::currGameVars[VAR_DAMAGE] = 0;
  state = RM::ACTIVE;
}

void ShopGUI::update()
{
  pizzaDrawer.updateFace();
  updateActions();
  updateDirHighlight();
  
  if (state == RM::ACTIVE) 
  {
    for (Coord1 i = 0; i < tabClickers.count; ++i)
    {
      tabClickers[i].update();
    }
  }
  
  selScaler.update();
  selFader.update();
  selector.setXY(itemClickers[currItem].getXY() + Point2(26.0, 24.0));
  selector.setYScale(selector.getXScale());
  
  setPlayerCashText();
  setItemText();
}

void ShopGUI::redraw()
{
  pizzaDrawer.drawFace();
  
  drawActions();
  tabClickers[currTab].drawMe(Point2(0.0 - tabClickers[0].getWidth() * currTab, 0.0));
  
  selector.drawMe();
  cashDrawer.drawText();
  itemDrawer.drawText();
  
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    if (Pizza::toppingExists(Coord2(currTab, i)) && 
        Pizza::isToppingUnlocked(Coord2(currTab, i)) == false)
    {      
      Coord1 itemPrice = TOPPING_DEFS[currTab][i].price;
      
      // secret item
      if (itemPrice == -1)
      {
        itemClickers[i].setText("", ResourceManager::shopCashFont, Point2(0.5, 0.5));
        imgCache[shopQuestion]->draw(itemClickers[i].getXY());
      }
      // regular item not yet bought
      else
      {
        itemClickers[i].setText("./", ResourceManager::shopCashFont, Point2(0.5, 0.5));
        itemClickers[i].getTextPtr()->int_replace('/', itemPrice / 100);        
        itemClickers[i].textChanged();
        
        Point1 clickerAlpha = itemClickers[i].getAlpha();
        itemClickers[i].setAlpha(1.0);
        itemClickers[i].drawText(itemClickers[i].getSize() * 0.5);
        itemClickers[i].setAlpha(clickerAlpha);    
      }
    }
  }

  drawDirHighlight();
  // unlockBtn.collisionBox().draw_solid(YELLOW_SOLID);
}

void ShopGUI::setPlayerCashText()
{
  cashDrawer.getTextPtr()->set("1");
  cashDrawer.getTextPtr()->cash_string('1', Pizza::playerCash);
  cashDrawer.getTextPtr()->replace('.', '/');
  cashDrawer.getTextPtr()->replace('$', '.');
  cashDrawer.textChanged();
}

void ShopGUI::setItemText()
{
  itemDrawer.setText(TOPPING_DEFS[currTab][currItem].name);
  itemDrawer.scaleToTextSize(Point2(196.0, 64.0));
}

void ShopGUI::setTab(Coord1 tabIndex)
{
  currTab = tabIndex;
  currItem = Pizza::playerOutfit[currTab];
  
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    itemClickers[i].setImage(ResourceManager::shopItemSet[currTab][i]);
    itemClickers[i].setAlpha(1.0);
    
    if (Pizza::toppingExists(Coord2(currTab, i)) == true) 
    {
      itemClickers[i].enable();
      Coord1 itemPrice = TOPPING_DEFS[currTab][i].price;
      Logical canBuy = itemPrice != -1 && Pizza::playerCash >= itemPrice;
      
      if (Pizza::isToppingUnlocked(Coord2(currTab, i)) == false && canBuy == false)
      {
        itemClickers[i].setAlpha(0.5);
      }
    }
    else
    {
      itemClickers[i].disable();
      if (Pizza::lite == true) itemClickers[i].setAlpha(0.0);
    }
  }

}

void ShopGUI::clickItem(Coord1 tabIndex, Coord1 itemIndex)
{
  if (Pizza::isToppingUnlocked(Coord2(tabIndex, itemIndex)) == false)
  {
    Coord1 toppingPrice = TOPPING_DEFS[tabIndex][itemIndex].price;
    
    if (toppingPrice == -1)
    {
      ResourceManager::playCantClick();
    }
    else if (Pizza::playerCash >= toppingPrice)
    {
      ResourceManager::playClickStd();
      RMSceneMan->pushGUI(new ShopConfirmGUI(this, Coord2(tabIndex, itemIndex), itemClickers[itemIndex]));      
    }
    else
    {
      ResourceManager::playCantClick();
    }
  }
  else
  {
    ResourceManager::playClickStd();
    selectTopping(tabIndex, itemIndex);
  }
}

void ShopGUI::itemBought(Coord2 itemDef)
{
  Pizza::playerCash -= TOPPING_DEFS[itemDef.x][itemDef.y].price;
  Pizza::unlockTopping(itemDef);
  
  if (postedBuy == false) Pizza::scoreMan->earnedAchievement(ACH_BOUGHT_ING);
  postedBuy = true;
  
  selectTopping(itemDef.x, itemDef.y);  // this saves player, including cash
  setTab(currTab);  // this resets price data and icon fades
}

void ShopGUI::selectTopping(Coord1 tabIndex, Coord1 itemIndex)
{
  currItem = itemIndex;
  Pizza::playerOutfit[tabIndex] = itemIndex;
  
  switch (tabIndex)
  {
    case CHEESE_LAYER:
      ResourceManager::loadBase(true, false);
      break;
    case TOPPINGS_LAYER:
      ResourceManager::loadToppings(true);
      break;
    case EYES_LAYER:
      ResourceManager::loadEyes(true);
      break;
    case MOUTH_LAYER:
      ResourceManager::loadMouth(true);
      break;
    case CLOTHES_LAYER:
      ResourceManager::loadClothes(true);
      break;
  }
  
  pizzaDrawer.resetFace();
  pizzaDrawer.actionTrigger(PizzaFace::BUMP);

  Pizza::savePlayer();
  
  selector.setAlpha(0.0);
  selector.setScale(2.0, 2.0);
  selScaler.reset();
  selFader.reset();
  
  RMSystem->analytics_event("Changed ingredient");

  Logical nonRetroUsed = false;
  
  for (Coord1 layerID = 0; layerID < TOPPING_DEFS.count; ++layerID)
  {
    if (Pizza::playerOutfit[layerID] != 10) 
    {
      nonRetroUsed = true;
      break;
    }
  }
  
  if (nonRetroUsed == false && postedRetro == false)
  {
    Pizza::scoreMan->earnedAchievement(ACH_ALL_RETRO);
    postedRetro = true;
  }
}

void ShopGUI::click(Widget* caller)
{
  if (caller == &done)
  {
    MapScene* scene = new MapScene();
    scene->transInHint = PizzaScene::FROM_SHOP;
    RMSceneMan->sceneTransition(scene);
    state = RM::TRANS_OUT;
  }
  else if (caller == &share)
  {
    RMSceneMan->sceneTransition(new ShareScene(), true);    
  }
  for (Coord1 i = 0; i < tabClickers.count; ++i)
  {    
    if (caller == &tabClickers[i])
    {
      if (clickCount == 10)
      {
        if (caller == &tabClickers[2])
        {
          Pizza::unlockOutfitsDebug();
          ResourceManager::winLick->play();
        }
        else
        {
          clickCount++;
        }
      }
      
      if (caller == &tabClickers.last())
      {
        clickCount++;
      }
      
      pizzaDrawer.actionTrigger(PizzaFace::BUMP);
      
      setTab(i);
      break;
    }
  }
  
  for (Coord1 i = 0; i < itemClickers.count; ++i)
  {
    if (caller == &itemClickers[i]) clickItem(currTab, i);
  }
}

// =============================== ShopConfirmGUI ============================ //

ShopConfirmGUI::ShopConfirmGUI(ShopGUI* setParent, Coord2 setItemData, const Clicker& item) :
  PizzaGUI(),

  parent(setParent),
  yesButton(this, Point2(132.0, 184.0), NULL, imgCache[shopYes]),
  noButton(this, Point2(244.0, 184.0), NULL, imgCache[shopNo]),
  icon(item.getXY(), item.getImage()),
  itemData(setItemData),
  clickedYes(false),
  textDrawer(64, ResourceManager::shopItemFont, Point2(0.5, 0.0))
{
  window.setImage(imgCache[shopConfirm]);
  window.autoSize();
  window.setXY(-window.getWidth(), 48.0);
  
  PizzaItem& currItem = TOPPING_DEFS[itemData.x][itemData.y];
  
  textDrawer.getTextPtr()->set(LSTRINGS[80]);
  textDrawer.getTextPtr()->cash_string('@', currItem.price);
  textDrawer.textChanged();
  
  icon.setAlpha(item.getAlpha());
  
  textDrawer.setXY(window.getXY() + Point2(144.0, 104.0));
  lockToRect(&textDrawer, &window);
  
  yesButton.setWH(104.0, 32.0);
  widgetList.add(&yesButton);
  
  noButton.setWH(104.0, 32.0);
  widgetList.add(&noButton);
  
  ActionList* transActions = new ActionList();
  transActions->addX(new ArcsineFn(&window.xy.x, 96.0, Pizza::GUI_TRANS_DUR));
  transActions->addX(new LinearFn(&icon.color.w, 1.0, Pizza::GUI_TRANS_DUR));
  transActions->addX(new ArcsineFn(&icon.xy.x, 214.0, Pizza::GUI_TRANS_DUR));
  transActions->addX(new ArcsineFn(&icon.xy.y, 88.0, Pizza::GUI_TRANS_DUR), true);  
  
  addNode(&yesButton, &noButton, NULL, &noButton, NULL);
  addNode(&noButton, &yesButton, NULL, &yesButton, NULL);
  setFirstNode(&yesButton);
  
  script.enqueueX(transActions);
  state = RM::TRANS_IN;
}

void ShopConfirmGUI::update()
{
  updateActions();
  updateDirHighlight();
}

void ShopConfirmGUI::redraw()
{
  window.drawMe();
  drawActions();
  icon.drawMe();
  textDrawer.drawText();
  drawDirHighlight();
}

void ShopConfirmGUI::startResolve()
{
  if (clickedYes == true) parent->itemBought(itemData);
}

void ShopConfirmGUI::click(Widget* caller)
{
  Point1 targetX = PLAY_W;
  
  if (caller == &yesButton)
  {
    RMSystem->analytics_event("Bought ingredient");
    clickedYes = true;
  }
  else if (caller == &noButton)
  {
    clickedYes = false;
    targetX = -window.getImage()->natural_size().x;
  }
  
  lockToRect(&icon, &window);
  
  script.enqueueX(new SineFn(&window.xy.x, targetX, Pizza::GUI_TRANS_DUR));
  state = RM::TRANS_OUT;
}

// =============================== ShareScene ============================ //

ShareScene::ShareScene() :
  PizzaScene()
{
  
}

ShareScene::~ShareScene()
{
  ResourceManager::unloadShare();
}

void ShareScene::load()
{
  ResourceManager::loadShare();
  
  RMSystem->analytics_event("Entered Share");
  PizzaScene::transIn();
  
  pushGUI(new ShareGUI());
}

void ShareScene::updateMe()
{
  updateActions();
  if (state == RM::ACTIVE) Pizza::controller->update();
}

void ShareScene::redraw()
{
  drawActions();
}

void ShareScene::startActive()
{
  releaseTransScreenshot();
}

// =============================== ShareGUI ============================ //

ShareGUI::ShareGUI() :
  PizzaGUI(),

  drawers(NUM_STATIC_DRAWERS, true),
  bgButtons(4, true),

  share(),

  pizza(),
  pizzaDrawer(&pizza),

  drawingBGID(-1),
  tallMode(false),
  shareState(NOTHING),
  whiteAlpha(0.0),
  screenshot(NULL),

  shouldPostAchNow(false),
  postedAchievement(false)
{
  Coord1 clothesID = Pizza::playerOutfit[CLOTHES_LAYER];
  tallMode = clothesID == 3 || clothesID == 4 || clothesID == 8 || 
      clothesID == 10 || clothesID == 11;
  
  drawers[BACKGROUND].setXY(-44.0, -32.0);
  drawers[BACKGROUND].setImage(NULL);  // this is to make SURE it's not deleting garbage
  setBackground(Pizza::shareBGID);
  
  drawers[BACK_BONES].setXY(240.0, 292.0);
  drawers[BACK_BONES].setImage(imgCache[shareBackBones]);
  
  pizza.setXY(240.0, 158.0);
  pizza.setScale(0.9, 0.9);
  
  drawers[FRONT_BONES].setXY(240.0, 292.0);
  drawers[FRONT_BONES].setImage(imgCache[shareFrontBones]);
  
  drawers[BG_ICON_BASE].setXY(Pizza::platScreenBox.corner(3));
  drawers[BG_ICON_BASE].setImage(imgCache[shareSelectBase]);
  
  drawers[SELECTOR].setXY(calcSelectorXY(Pizza::shareBGID));
  drawers[SELECTOR].setImage(imgCache[shareSelector]);

  share.init(Pizza::platScreenBox.corner(2) + Point2(-104.0, -48.0),
             imgCache[shareShareBase], imgCache[shareShareDown]);
  share.autoSize();
  lockAndLoad(&share);

  close.init(Pizza::platScreenBox.corner(1) + Point2(-48.0, 0.0), 
             imgsetCache[shareCloseSet][0], imgsetCache[shareCloseSet][1]);
  close.setWH(48.0, 48.0);
  close.drawOffset.x = 48.0;
  lockAndLoad(&close);
  
  drawers[RIVERMAN].setXY(Pizza::platScreenBox.corner(2));
  drawers[RIVERMAN].setImage(imgCache[shareRiverman]);
  drawers[RIVERMAN].setAlpha(0.0);

  drawers[PIZZA_LOGO].setXY(Pizza::platScreenBox.corner(0));
  drawers[PIZZA_LOGO].setImage(imgCache[shareLogo]);
  drawers[PIZZA_LOGO].setAlpha(0.0);
  
  for (Coord1 i = 0; i < bgButtons.count; ++i)
  {
    bgButtons[i].init(Point2(Pizza::platScreenBox.corner(3) + Point2(8.0 + i * 48.0, -48.0)), NULL, NULL);
    bgButtons[i].setWH(48.0, 48.0);
    lockAndLoad(&bgButtons[i]);
  }

  if (tallMode == true)
  {
    drawers[BACK_BONES].setXY(240.0, 308.0);
    drawers[BACK_BONES].setScale(0.8, 0.8);
    
    pizza.setY(188.0);
    pizza.setScale(0.8, 0.8);
    
    drawers[FRONT_BONES].setXY(240.0, 308.0);
    drawers[FRONT_BONES].setScale(0.8, 0.8);    
  }
  
  PizzaDirNode* currNode = addNode(&bgButtons[0], &bgButtons[1], &close, &share, &close);
  currNode->highlightPosOffset += Point2(-2.0, -2.0);
  currNode->highlightSizeOffset += Point2(-4.0, -4.0);

  currNode = addNode(&bgButtons[1], &bgButtons[2], &close, &bgButtons[0], &close);
  currNode->highlightPosOffset += Point2(-2.0, -2.0);
  currNode->highlightSizeOffset += Point2(-4.0, -4.0);
  
  currNode = addNode(&bgButtons[2], &bgButtons[3], &close, &bgButtons[1], &close);
  currNode->highlightPosOffset += Point2(-2.0, -2.0);
  currNode->highlightSizeOffset += Point2(-4.0, -4.0);

  currNode = addNode(&bgButtons[3], &share, &close, &bgButtons[2], &close);
  currNode->highlightPosOffset += Point2(-2.0, -2.0);
  currNode->highlightSizeOffset += Point2(-4.0, -4.0);
  
  currNode = addNode(&share, &bgButtons[0], &close, &bgButtons[3], &close);
  currNode->highlightPosOffset += Point2(-4.0, -2.0);
  currNode->highlightSizeOffset += Point2(-6.0, -6.0);
  
  currNode = addNode(&close, NULL, &share, NULL, &share);
  currNode->highlightPosOffset += Point2(12.0, 4.0);
  currNode->highlightSizeOffset += Point2(-16.0, -16.0);

  setFirstNode(&share);
  
  state = RM::ACTIVE;
}

ShareGUI::~ShareGUI()
{
  delete drawers[BACKGROUND].getImage();
  delete screenshot;
}

void ShareGUI::update()
{
  updateActions();
  
  if (shouldPostAchNow == true && postedAchievement == false)
  {
    Pizza::scoreMan->earnedAchievement(ACH_PHOTO);
    postedAchievement = true;
    shouldPostAchNow = false;
  }
  
  pizzaDrawer.updateFace();
  syncColors();
  if (drawingBGID != Pizza::shareBGID) setBackground(Pizza::shareBGID);
  drawers[SELECTOR].setXY(calcSelectorXY(Pizza::shareBGID));
  
  updateDirHighlight();
}

void ShareGUI::redraw()
{
  drawers[BACKGROUND].drawMe();
  drawers[BACK_BONES].drawMe();
  pizzaDrawer.drawFace();
  drawers[FRONT_BONES].drawMe();
  drawers[RIVERMAN].drawMe();
  drawers[PIZZA_LOGO].drawMe();
  Box(Pizza::platScreenBox.left(), PLAY_H - 56.0, Pizza::platScreenBox.width(), 
      Pizza::platScreenBox.bottom() - (PLAY_H - 56.0)).draw_solid(ColorP4(0.0, 0.0, 0.0, 0.5 * close.getAlpha()));
  drawers[BG_ICON_BASE].drawMe();
  drawers[SELECTOR].drawMe();
  
  drawWidgets();
  
  // camera flash
  if (whiteAlpha > 0.01)
  {
    CameraStd::screenBoxToWorld().draw_solid(ColorP4(1.0, 1.0, 1.0, whiteAlpha));    
  }
  
  drawDirHighlight();
  
  actions.redraw();
  script.redraw();
}

void ShareGUI::syncColors()
{
  drawers[BG_ICON_BASE].setAlpha(close.getAlpha());
  share.setAlpha(close.getAlpha());
  drawers[SELECTOR].setAlpha(close.getAlpha());
  
  drawers[PIZZA_LOGO].setAlpha(drawers[RIVERMAN].getAlpha());
  
  drawers[BACKGROUND].setGreen(drawers[BACKGROUND].getRed());
  drawers[BACKGROUND].setBlue(drawers[BACKGROUND].getRed());
}

Point2 ShareGUI::calcSelectorXY(Coord1 bgID)
{
  return Pizza::platScreenBox.corner(3) + Point2(8.0 + bgID * 48.0, -48.0);
}

void ShareGUI::changeBackground(Coord1 bgID)
{
  state = RM::PASSIVE;
  
  ActionList* fadeList1 = new ActionList();
  fadeList1->addX(new LinearFn(&close.color.w, 0.0, Pizza::GUI_TRANS_DUR), false);
  fadeList1->addX(new LinearFn(&drawers[BACKGROUND].color.x, 0.0, Pizza::GUI_TRANS_DUR), true);

  ActionList* fadeList2 = new ActionList();
  fadeList2->addX(new LinearFn(&close.color.w, 1.0, Pizza::GUI_TRANS_DUR), false);
  fadeList2->addX(new LinearFn(&drawers[BACKGROUND].color.x, 1.0, Pizza::GUI_TRANS_DUR), true);

  script.enqueueX(fadeList1);
  script.enqueueX(new SetValueCommand<Coord1>(&Pizza::shareBGID, bgID));
  script.enqueueX(fadeList2);
}

void ShareGUI::setBackground(Coord1 bgID)
{
  String1 imgBaseStr("share_bg_$", '$', bgID);
  delete drawers[BACKGROUND].getImage();
  drawers[BACKGROUND].setImage(new Image(ResourceManager::imgStr(imgBaseStr.as_cstr())));
  drawingBGID = bgID;
}

void ShareGUI::beginShare(ShareState shareType)
{
  shareState = shareType;
  state = RM::PASSIVE;
  
  ActionList* fadeList1 = new ActionList();
  fadeList1->addX(new LinearFn(&close.color.w, 0.0, Pizza::GUI_TRANS_DUR), false);
  fadeList1->addX(new LinearFn(&drawers[RIVERMAN].color.w, 1.0, Pizza::GUI_TRANS_DUR), true);
  
  script.enqueueX(fadeList1);
  script.wait(Pizza::GUI_TRANS_DUR);
  script.enqueueX(new DeleteCommand<Bitmap*>(screenshot));
  script.enqueueX(new ScreenshotAction(&screenshot));
  script.enqueueX(new SetValueCommand<Point1>(&whiteAlpha, 1.0));
  script.enqueueX(new FunctionCommand0(ResourceManager::playFlashSound));
  script.enqueueX(new CosineFn(&whiteAlpha, 0.0, 0.5));
}

void ShareGUI::click(Widget* caller)
{
  if (caller == &close)
  {
    ShopScene* nextShop = new ShopScene();
    nextShop->loadType = Scene::LOAD_LESS_AUTO;
    RMSceneMan->sceneTransition(nextShop);
  }
  else if (caller == &share)
  {
    RMSystem->analytics_event("Share photo");
    beginShare(SHARING);
  }
  
  for (Coord1 i = 0; i < bgButtons.count; ++i)
  {
    if (caller == &bgButtons[i] && Pizza::shareBGID != i)
    {
      changeBackground(i);
      break;
    }    
  }
}

void ShareGUI::callback(ActionEvent* caller)
{
  if (state == RM::PASSIVE)
  {
    Logical triedShare = false;
    
    if (shareState == SHARING)
    {
      RMSystem->facebook_post(LSTRINGS[85], screenshot);
      triedShare = true;
    }
    
    shareState = NOTHING;

    ActionList* fadeList1 = new ActionList();
    fadeList1->addX(new LinearFn(&close.color.w, 1.0, Pizza::GUI_TRANS_DUR), false);
    fadeList1->addX(new LinearFn(&drawers[RIVERMAN].color.w, 0.0, Pizza::GUI_TRANS_DUR), true);
    
    script.enqueueX(fadeList1);
    script.enqueueX(new SetValueCommand<Coord1>(&state, RM::ACTIVE));
    if (triedShare == true) script.enqueueX(new SetValueCommand<Logical>(&shouldPostAchNow, true));
  }
  else 
  {
    RivermanGUI::callback(caller);
  }
}

// =============================== PlayScene ============================ //

PlayScene::PlayScene() :
  PizzaScene(),
  level(NULL)
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
  Pizza::resetGame();
  
  level = PizzaLevelInterface::createLevel(Pizza::currLevelDef);
  
  if (loadType == LOAD_ALL)
  {
    level->loadRes();
    ResourceManager::loadPlay(level->shouldLoadPizza());
    ResourceManager::loadSound(music, level->musicFilename.as_cstr());
  }
    
  level->load();
  startWithMusic = level->startsWithMusic();
  
  // level settings
  if (RMPhysics != NULL) 
  {
    RMPhysics->enable_debug_draw(Pizza::DEBUG_PHYSICS);
  }
  
  if (Pizza::DEBUG_NO_INTERFACE == true)
  {
    level->scoreText.setAlpha(0.0);
  }
  
  PizzaScene::transIn();
  
  pushGUI(new GameplayGUI(level->pauseGUIEnabled()));
}

void PlayScene::updateMe()
{
  if (level->levelState == PizzaLevel::LEVEL_PLAY ||
      guiList.count > 1) 
  {
    updateGUIs();
  }
  
  if (guiList.count <= 1)
  {
    level->update();
  }
  
  if (level->levelState == PizzaLevel::LEVEL_PLAY &&
      (Pizza::currWorldPair.x != 0 || Pizza::currWorldPair.y != 0))
  {
    Pizza::extender->reportDrawHUD();
  }
  
  actions.update();
  script.update();
  Pizza::controller->update();
}

void PlayScene::redraw()
{
  level->redraw();
  drawActions();
  // drawLoading();
}

void PlayScene::startActive()
{
  releaseTransScreenshot();
  
  level->levelState = PizzaLevel::LEVEL_PLAY;
  level->startActive();
}

// =============================== GameplayGUI ============================ //

GameplayGUI::GameplayGUI(Logical enabled) :
  RivermanGUI(),
  pause(this, Point2(SCREEN_BOX.right() - imgsetCache[pauseSet][0]->natural_size().x,
                     SCREEN_BOX.top()),
        imgsetCache[pauseSet][0], imgsetCache[pauseSet][1])
{
  if (enabled == false)
  {
    disableGUI();
  }

  if (Pizza::DEBUG_NO_INTERFACE == true) pause.setAlpha(0.0);
  lockAndLoad(&pause);
  
  state = RM::ACTIVE;
}

void GameplayGUI::update()
{
  if (RMSceneMan->peekGUI() == this &&
      pause.isEnabled() == true &&
      (Pizza::controller->getCancelButtonStatus(JUST_PRESSED) == true))
  {
    pause.clickCallback();
  }
  
  updateActions();
}

void GameplayGUI::redraw()
{
  drawActions();
}

void GameplayGUI::disableGUI()
{
  pause.upImg = NULL;
  pause.downImg = NULL;
  pause.disable();
}

void GameplayGUI::click(Widget* caller)
{
  RMSceneMan->pushGUI(new PauseMenu());
}

// =============================== PauseMenu ============================ //

PauseMenu::PauseMenu() :
  PizzaGUI(),

  play(this, Point2(80.0, 76.0), NULL, imgCache[pausePlay]),
  restart(this, Point2(268.0, 80.0), NULL, imgCache[pauseRestart]),
  quit(this, Point2(296.0, 128.0), NULL, imgCache[pauseQuit]),
  instructions(this, Point2(16.0, 124.0), NULL, imgCache[pauseInstructions]),
  calibrate(this, Point2(168.0, 260.0), NULL, imgCache[pauseCalibrate]),

  sounds(this),

  topBase(Point2(240.0, 160.0), imgCache[pauseBaseTop]),
  midBase(Point2(240.0, 160.0), imgCache[pauseBaseMid]),
  botBase(Point2(240.0, 160.0), imgCache[pauseBaseBot]),
  calibrateArrow(Point2(132.0, 240.0), imgCache[pauseCalibrateArrow]),

  darkAlpha(0.0),
  accelChaser(0.0),

  clickCount(0)
{
  topBase.setAlpha(0.0);
  
  play.setWH(play.getImage()->natural_size());
  play.usesPlaySound = true;
  lockAndLoad(&play);

  restart.setWH(restart.getImage()->natural_size());
  restart.usesPlaySound = true;
  lockAndLoad(&restart);
  
  quit.setWH(quit.getImage()->natural_size());
  lockAndLoad(&quit);
  
  instructions.setWH(instructions.getImage()->natural_size());
  lockAndLoad(&instructions);

  calibrate.setWH(calibrate.getImage()->natural_size());
  lockAndLoad(&calibrate);
  
  sounds.setXY(Pizza::platformTL);
  lockAndLoad(&sounds);
  
  addNode(&play, &restart, &instructions, &sounds, &sounds);
  addNode(&restart, NULL, &quit, &play, &calibrate);
  addNode(&instructions, &quit, &calibrate, &quit, &play);
  addNode(&quit, &instructions, &calibrate, &instructions, &restart);
  addNode(&calibrate, NULL, NULL, NULL, &instructions);
  PizzaDirNode* currNode = addNode(&sounds, &play, &play, NULL, NULL);
  currNode->highlightPosOffset += Point2(4.0, 4.0);
  currNode->highlightSizeOffset += Point2(-16.0, 0.0);
  
  setFirstNode(&play);
  
  transInStd();
}

void PauseMenu::update()
{
  RM::flatten_me(accelChaser, PizzaLevel::tiltMagnitude(), 1.0 * RM::timePassed()); 
  calibrateArrow.setX(RM::lerp(Point2(132.0, 348.0), (accelChaser + 1.0) * 0.5));
  
  updateActions();
  syncBases();
  
  updateDirHighlight();
}

void PauseMenu::redraw()
{
  CameraStd::screenBoxToWorld().draw_solid(ColorP4(0.0, 0.0, 0.0, darkAlpha));
  
  if (state != RM::PASSIVE)
  {    
    topBase.drawMe();
    midBase.drawMe();
    botBase.drawMe();
    drawActions();
    
    if (state == RM::ACTIVE) calibrateArrow.drawMe();
    
    drawDirHighlight();
  }
}

void PauseMenu::syncBases()
{
  topBase.setYScale(topBase.getXScale());
  midBase.setAlpha(topBase.getAlpha());
  midBase.setScale(topBase.getXScale(), topBase.getXScale());
  botBase.setAlpha(topBase.getAlpha());
  botBase.setScale(topBase.getXScale(), topBase.getXScale());
  sounds.setAlpha(topBase.getAlpha());
  calibrateArrow.setAlpha(topBase.getAlpha());
}

void PauseMenu::transInStd()
{
  state = RM::TRANS_IN;
  
  topBase.setScale(0.25, 0.25);
  syncBases();
  
  ActionList* list = new ActionList();
  list->addX(new LinearFn(&darkAlpha, 0.5, Pizza::GUI_TRANS_DUR), false);
  list->addX(new LinearFn(&topBase.color.w, 1.0, Pizza::GUI_TRANS_DUR), false);
  list->addX(new LinearFn(&topBase.scale.x, 1.0, Pizza::GUI_TRANS_DUR), true);
  script.enqueueX(list);
}

void PauseMenu::transOutStd()
{
  ActionList* list = new ActionList();
  list->addX(new LinearFn(&darkAlpha, 0.0, Pizza::GUI_TRANS_DUR), false);
  list->addX(new LinearFn(&topBase.color.w, 0.0, Pizza::GUI_TRANS_DUR), false);
  list->addX(new LinearFn(&topBase.scale.x, 0.25, Pizza::GUI_TRANS_DUR), true);
  script.enqueueX(list);
  
  state = RM::TRANS_OUT;
}

void PauseMenu::transInInstructions()
{
  state = RM::TRANS_IN;

  script.enqueueX(new LinearFn(&topBase.color.w, 1.0, Pizza::GUI_TRANS_DUR));
  script.enqueueX(new SetValueCommand<Coord1>(&state, RM::ACTIVE));
}

void PauseMenu::transOutInstructions()
{
  state = RM::TRANS_OUT;
  
  script.enqueueX(new LinearFn(&topBase.color.w, 0.0, Pizza::GUI_TRANS_DUR));
  script.enqueueX(new SetValueCommand<Coord1>(&state, RM::PASSIVE));
  script.enqueueX(new PushGUICommand(new PauseInstructions(this)));
}

void PauseMenu::resume(RivermanGUI* caller)
{
  transInInstructions();
}

void PauseMenu::click(Widget* caller)
{
  if (caller == &play)
  {
    setNextPop();
    transOutStd();
  }
  else if (caller == &restart)
  {
    if (ResourceManager::carpetMusic != NULL)
    {
      ResourceManager::carpetMusic->stop();
    }

    RMSceneMan->sceneTransition(new PlayScene(), true);
  }
  else if (caller == &quit)
  {
    if (clickCount == 10)
    {
      ResourceManager::winLick->play();
      Pizza::unlockLevelsDebug();
    }
    
    Pizza::useLoadImg = true;
    MapScene* map = new MapScene();
    map->transInHint = PizzaScene::FROM_LOSE;
    RMSceneMan->sceneTransition(map);
  }
  else if (caller == &instructions)
  {
    if (clickCount == 10)
    {
      clickCount++;
    }
    
    transOutInstructions();
  }
  else if (caller == &calibrate)
  {
    RMSystem->analytics_event("Calibrate");
    
    Pizza::controller->calib = Pizza::controller->get_accel_x();
    accelChaser = 0.0;
    calibrateArrow.setX(RM::lerp(Point2(132.0, 348.0), (accelChaser + 1.0) * 0.5));
  }
  else if (caller == &sounds)
  {
    clickCount++;
    
    Sound::enable_playing(!Sound::group_enabled(Sound::TYPE_MUSIC));
    
    if (Sound::group_enabled(Sound::TYPE_MUSIC) == false)
    {
      RMSystem->analytics_event("Mute sounds");
      
      if (soundCache[music] != NULL) soundCache[music]->stop();
      if (ResourceManager::introMusic != NULL) ResourceManager::introMusic->stop();
      if (ResourceManager::endingMusic != NULL) ResourceManager::endingMusic->stop();
      if (ResourceManager::carpetMusic != NULL) ResourceManager::carpetMusic->stop();
    }
    else
    {
      if (soundCache[music] != NULL) soundCache[music]->play();
      // if (ResourceManager::introMusic != NULL) ResourceManager::introMusic->play();
      // if (ResourceManager::endingMusic != NULL) ResourceManager::endingMusic->play();      
    }
  }
}

// =============================== PauseInstructions ============================ //

PauseInstructions::PauseInstructions(RivermanGUI* setParent) :
  RivermanGUI(),
  parent(setParent),
  comicImg(NULL),

  levelText(16, ResourceManager::instructionsFont, Point2(0.5, 0.0)),
  scoreText(16, ResourceManager::scoreFont, Point2(1.0, 0.0))
{
  window.setXY(16.0, 16.0);
  window.setAlpha(0.0);
  
  levelText.setXY(240.0, 38.0);
  levelText.getTextPtr()->set(".-$/%");  // "LEVEL $-%"
  levelText.getTextPtr()->int_replace('$', Pizza::currWorldPair.x + 1);
  levelText.getTextPtr()->int_replace('%', Pizza::currWorldPair.y + 1);
  levelText.textChanged();
  
  scoreText.setXY(439.0, 54.0);
  
  Coord1 levelScore = Pizza::currLevelData().bestScore;
  scoreText.getTextPtr()->int_string(levelScore);
  scoreText.textChanged();
  
  state = RM::TRANS_IN;
  script.enqueueX(new LinearFn(&window.color.w, 1.0, Pizza::GUI_TRANS_DUR));
}

PauseInstructions::~PauseInstructions()
{
  delete window.getImage();
  delete comicImg;
}

void PauseInstructions::update()
{
  // this should be before updateActions so it doesn't happen after resolve
  if (window.getImage() == NULL)
  {
    window.setImage(new Image(ResourceManager::imgStr("ui_overlay")));
    
    String1 filename;
    Pizza::getInstructionsStr(Pizza::currLevelData(), filename);

    comicImg = new Image(filename);
  }
  
  if (state == RM::ACTIVE &&
      Pizza::controller->getAnyConfirmStatus(JUST_PRESSED) == true)
  {
    state = RM::TRANS_OUT;
    script.enqueueX(new LinearFn(&window.color.w, 0.0, Pizza::GUI_TRANS_DUR));
  }
  
  updateActions();
  
  levelText.setAlpha(window.getAlpha());
  scoreText.setAlpha(window.getAlpha());
}

void PauseInstructions::redraw()
{
  if (window.getImage() == NULL) return;
  
  window.drawMe();
    
  comicImg->draw_scale(Point2(40.0, 104.0), Point2(1.0, 1.0), 0.0, 
                       ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  
  imgCache[instructionsHiScore]->draw_scale(Point2(384.0, 40.0), Point2(1.0, 1.0), 0.0, 
                                            ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  
  for (Coord1 i = 0; i < Pizza::currLevelData().starsEarned; ++i)
  {
    imgCache[instructionsStar]->draw_scale(Point2(40.0 + i * 24.0, 48.0), Point2(1.0, 1.0), 0.0, 
                                           ColorP4(1.0, 1.0, 1.0, window.getAlpha()));
  }
  
  levelText.drawText();
  scoreText.drawText();
}

void PauseInstructions::startResolve()
{
  window.image = NULL;
  parent->resume(this);
}

// =============================== LoseGUI ============================ //

const Point1 LoseGUI::botOffset = 128.0;

LoseGUI::LoseGUI() :
  PizzaGUI(),

  quitBtn(this, Pizza::platScreenBox.corner(3) + Point2(0.0, -48.0 + botOffset), 
          imgsetCache[loseQuitSet][0], imgsetCache[loseQuitSet][1]),
  instructionsBtn(this, Pizza::platScreenBox.corner(3) + Point2(112.0, -48.0 + botOffset), 
                  imgsetCache[loseInstructionsSet][0], imgsetCache[loseInstructionsSet][1]),
  retryBtn(this, Pizza::platScreenBox.corner(2) + Point2(-136.0, -48.0 + botOffset), 
           imgsetCache[loseRetrySet][0], imgsetCache[loseRetrySet][1])
{
  state = RM::TRANS_IN;
  retryBtn.usesPlaySound = true;

  if (Pizza::widescreen == true)
  {
    instructionsBtn.addX(52.0);
  }
  
  addWidget(&quitBtn);
  addWidget(&instructionsBtn);
  addWidget(&retryBtn);
  
  PizzaDirNode* currNode = addNode(&retryBtn, &quitBtn, NULL, &instructionsBtn, NULL);
  currNode->highlightSizeOffset -= Point2(4.0, 4.0);
  
  currNode = addNode(&instructionsBtn, &retryBtn, NULL, &quitBtn, NULL);
  currNode->highlightSizeOffset -= Point2(4.0, 4.0);
  currNode->highlightPosOffset += Point2(8.0, 0.0);

  currNode = addNode(&quitBtn, &instructionsBtn, NULL, &retryBtn, NULL);
  currNode->highlightSizeOffset -= Point2(4.0, 4.0);
  currNode->highlightPosOffset += Point2(4.0, 0.0);
  
  setFirstNode(&retryBtn);
  
  script.enqueueX(new LinearFn(&quitBtn.xy.y, quitBtn.getY() - botOffset, Pizza::GUI_TRANS_DUR));
}

void LoseGUI::update()
{
  updateActions();
  instructionsBtn.setY(quitBtn.getY());
  retryBtn.setY(quitBtn.getY());
  
  updateDirHighlight();
}

void LoseGUI::redraw()
{
  drawActions();
  drawDirHighlight();
}

void LoseGUI::resume(RivermanGUI* caller)
{
  script.enqueueX(new LinearFn(&quitBtn.xy.y, quitBtn.getY() - botOffset, Pizza::GUI_TRANS_DUR));
  script.enqueueX(new SetValueCommand<Coord1>(&state, RM::ACTIVE));
}

void LoseGUI::click(Widget* caller)
{
  if (caller == &quitBtn)
  {
    MapScene* map = new MapScene();
    map->transInHint = PizzaScene::FROM_LOSE;
    RMSceneMan->sceneTransition(map);
  }
  else if (caller == &instructionsBtn)
  {
    state = RM::PASSIVE;
    
    script.enqueueX(new LinearFn(&quitBtn.xy.y, quitBtn.getY() + botOffset, Pizza::GUI_TRANS_DUR));
    script.enqueueX(new PushGUICommand(new PauseInstructions(this)));
  }
  else if (caller == &retryBtn)
  {
    RMSceneMan->sceneTransition(new PlayScene(), true);
  }
}

// =============================== WheelOfPizza ============================ //

const Point2 WheelOfPizza::LETTER_POSITIONS[] = 
{
  Point2(32, 152),
  Point2(32, 104),
  Point2(48, 64),
  Point2(64, 28),
  Point2(92, 12),
  
  Point2(356, 248),
  Point2(380, 232),
  Point2(388, 192),
  Point2(396, 136),
  Point2(392, 80)
};

const Point2 WheelOfPizza::WHEEL_CENTER(240.0, 160.0);

WheelOfPizza::WheelOfPizza(WheelScene* setScene) :
  scene(setScene),
  wheel(Point2(240.0, 160.0)),
  slice(Point2(240.0, 160.0)),
  endScript(this),

  touched(false),
  currSpeed(PI * 2.3),
  // speedMover(&currSpeed, 0.0, 3.49, this),
  speedMover(&currSpeed, 0.0, 1.746, this),

  lettersOn(),
  bulbsOn(),

  lightScript(this),
  lightState(SPINNING),

  letterIndex(0),
  bulbIndex(0),
  
  wheelItemIDs(NUM_DIVISIONS, true),
  wheelObjects(NUM_DIVISIONS, true),
  workingList(10)
{
  bulbsOn.ensure_count(NUM_LIGHTS);
  lettersOn.ensure_count(10);
  clearLights();
  
  speedMover.setInactive();
  lightScript.wait(1.0 / 30.0);
  
  wheelItemIDs.set_all(WHEEL_NOTHING);
  for (Coord1 i = 0; i < wheelObjects.count; ++i)
  {
    wheelObjects[i].setXY(WHEEL_CENTER + RM::pol_to_cart(121.0, 0.2 * i * PI));
  }
  
}

void WheelOfPizza::init()
{
  wheel.setImage(imgCache[wheelCircle]);
  slice.setImage(imgsetCache[wheelSliceSet][0]);
  
  insertItem(WHEEL_PENNY);
  insertItem(WHEEL_NICKEL);
  insertItem(WHEEL_DIME);
  insertItem(WHEEL_QUARTER);
  insertItem(WHEEL_BIG_CASH);
}

void WheelOfPizza::update()
{
  Coord1 oldSegment = calcSegment();
  speedMover.update();
  slice.addRotation(currSpeed * RM::timePassed());
  Coord1 newSegment = calcSegment();
  
  if (oldSegment != newSegment)
  {
    ResourceManager::playWheelClick();
  }
  
  lightScript.update();
  updateLights();
  
  updateItems();
  
  if (touched == false &&
      Pizza::controller->getAnyConfirmStatus(JUST_PRESSED) == true)
  {
    touched = true;
    playerTouched();
  }
  
  endScript.update();
}

void WheelOfPizza::redraw()
{
  imgCache[wheelTextLeft]->draw(16.0, 0.0);
  imgCache[wheelTextRight]->draw(304.0, 64.0);
  
  // letters
  for (Coord1 i = 0; i < lettersOn.count; ++i)
  {
    if (lettersOn[i] == false) continue;
    
    imgsetCache[wheelTextSet][i]->draw(LETTER_POSITIONS[i]);
  }
  
  wheel.drawMe();
  
  // lights
  for (Coord1 i = 0; i < bulbsOn.count; ++i) 
  {
    Point2 center = WHEEL_CENTER + Point2(RM::pol_to_cart(133.0, RM::degs_to_rads(9.0 * i)));
    Image* lightImg = imgsetCache[wheelBulbSet][bulbsOn[i] ? 1 : 0];
    
    lightImg->draw_scale(center, Point2(1.0, 1.0), 0.0, COLOR_FULL);
  }
  
  Point1 singleSliceRads = TWO_PI / NUM_DIVISIONS;
  imgCache[wheelNeon]->draw_scale(WHEEL_CENTER, Point2(1.0, 1.0), 
                                  singleSliceRads * calcSegment(), COLOR_FULL);
  
  slice.setAlpha(1.0);
  slice.setImage(imgsetCache[wheelSliceSet][0]);
  slice.drawMe();

  slice.setAlpha(1.0 - speedMover.progress());
  slice.setImage(imgsetCache[wheelSliceSet][1]);
  slice.drawMe();
  
  drawItems();
}

void WheelOfPizza::updateItems()
{
  for (Coord1 i = 0; i < wheelObjects.count; ++i)
  {
    if (wheelItemIDs[i] == WHEEL_NOTHING) continue;
    
    Coord1 sliceSeg = calcSegment();
    Point1 speedMult = 1.0;
    Point1 targetScale = 1.0;
    
    if (i == sliceSeg)
    {
      speedMult = 4.0;
      targetScale = 1.25;
    }
    
    RM::flatten_me(wheelObjects[i].scale.x, targetScale, speedMult * RM::timePassed());
    wheelObjects[i].scale.y = wheelObjects[i].scale.x;
  }
}

void WheelOfPizza::drawItems()
{
  for (Coord1 i = 0; i < wheelObjects.count; ++i)
  {
    if (wheelItemIDs[i] == WHEEL_NOTHING) continue;

    wheelObjects[i].xy += 3.0;
    wheelObjects[i].setColor(0.0, 0.0, 0.0, 1.0);
    wheelObjects[i].drawMe();
    wheelObjects[i].xy -= 3.0;
    
    wheelObjects[i].setColor(1.0, 1.0, 1.0, 1.0);
    wheelObjects[i].drawMe();
  }
}

void WheelOfPizza::playerTouched()
{
  ResourceManager::playWheelStop();
  speedMover.setActive();
  // cout << "start rot " << RM::standardize_rads(slice.getRotation()) << endl;
}

void WheelOfPizza::updateLights()
{
  switch (lightState) 
  {
    default:
    case SPINNING:
      // bulbsOn[RM::wrap1(bulbIndex - 3, 0, bulbsOn.count)] = false;
      bulbsOn[bulbIndex] = false;
      bulbIndex = calcLight();
      bulbsOn[bulbIndex] = true;
      break;
    case WON_SMALL:
      break;
    case WON_BIG:
      break;
    case MISS:
      break;
  }
}

Coord1 WheelOfPizza::calcSegment()
{
  Point1 sliceRot = RM::standardize_rads(slice.getRotation());
  Point1 singleSliceRads = TWO_PI / NUM_DIVISIONS;
  
  if (sliceRot > TWO_PI - singleSliceRads * 0.5 ||
      sliceRot < singleSliceRads * 0.5)
  {
    return 0;
  }
  
  return (sliceRot - singleSliceRads * 0.5) / (singleSliceRads) + 1;
}

Coord1 WheelOfPizza::calcLight()
{
  Point1 sliceRot = RM::standardize_rads(slice.getRotation());
  Point1 singleLightRads = TWO_PI / NUM_LIGHTS;
  
  if (sliceRot > TWO_PI - singleLightRads * 0.5 ||
      sliceRot < singleLightRads * 0.5)
  {
    return 0;
  }
  
  return (sliceRot - singleLightRads * 0.5) / (singleLightRads) + 1;
}



void WheelOfPizza::lightCallback()
{
  switch (lightState) 
  {
    default:
    case SPINNING:
      lettersOn[letterIndex] = !lettersOn[letterIndex];
      letterIndex++;
      letterIndex %= lettersOn.count;
      
      lightScript.wait(1.0 / 15.0);
      break;
    
    case WON_SMALL:
      bulbsOn[bulbIndex] = !bulbsOn[bulbIndex];
      bulbIndex++;
      bulbIndex %= bulbsOn.count;
      
      lettersOn[letterIndex] = !lettersOn[letterIndex];
      letterIndex++;
      letterIndex %= lettersOn.count;
      
      lightScript.wait(1.0 / 20.0);      
      break;
      
    case WON_BIG:
      for (Coord1 i = 0; i < lettersOn.count; ++i) lettersOn[i] = (((i + bulbIndex) % 2) == 0);
      for (Coord1 i = 0; i < bulbsOn.count; ++i) bulbsOn[i] = (((i + bulbIndex) % 2) == 0);
      lightScript.wait(1.0 / 6.0);
      
      bulbIndex++;
      bulbIndex %= 2;
      break;
      
    case MISS:
      bulbsOn[bulbIndex] = false;
      bulbIndex++;
      bulbIndex %= bulbsOn.count;
      bulbsOn[bulbIndex] = true;

      lettersOn[letterIndex] = false;
      letterIndex++;
      letterIndex %= lettersOn.count;
      lettersOn[letterIndex] = true;

      lightScript.wait(1.0 / 3.0);      
      break;
  }
}

void WheelOfPizza::insertItem(WheelItemID itemID)
{
  workingList.clear();
  for (Coord1 i = 0; i < wheelItemIDs.count; ++i)
  {
    if (wheelItemIDs[i] == WHEEL_NOTHING)
    {
      workingList.add(i);
    }
  }
  
  Coord1 emptyIndex = RM::random(workingList);
  wheelItemIDs[emptyIndex] = itemID;
  
  switch (itemID) 
  {
    default: cout << "WheelOfPizza::insertItem " << itemID << " not recognized" << endl;
    case WHEEL_PENNY: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][0]); break;
    case WHEEL_NICKEL: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][1]); break;
    case WHEEL_DIME: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][2]); break;
    case WHEEL_QUARTER: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][3]); break;
    case WHEEL_BIG_CASH: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][5]); break;
    case WHEEL_OUTFIT: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][4]); break;
    case WHEEL_STAR: wheelObjects[emptyIndex].setImage(imgsetCache[wheelIconSet][6]); break;
  }
}

void WheelOfPizza::clearLights()
{
  lettersOn.set_all(false);
  bulbsOn.set_all(false);
  
  lightScript.clear();
  
  bulbIndex = 0;
  letterIndex = 0;
}

void WheelOfPizza::setWonSmall()
{
  lightState = WON_SMALL;
  ResourceManager::playWheelGetSmall();
  for (Coord1 i = 0; i < lettersOn.count; ++i) lettersOn[i] = ((i % 2) == 0);
  for (Coord1 i = 0; i < bulbsOn.count; ++i) bulbsOn[i] = ((i % 2) == 0);
  bulbIndex = (calcLight() + 1) % bulbsOn.count;
  lightCallback();
}

void WheelOfPizza::setWonBig()
{
  lightState = WON_BIG;
  ResourceManager::playWheelGetBig();
  lightCallback();
}

void WheelOfPizza::setMiss()
{
  lightState = MISS;
  bulbIndex = calcLight();
  ResourceManager::playWheelMiss();
  lightCallback();
}

void WheelOfPizza::wheelStopped()
{
  clearLights();
  
  Coord1 itemWon = wheelItemIDs[calcSegment()];
  
  switch (itemWon) 
  {
    default: cout << "WheelOfPizza::wheelStopped " << itemWon << " not recognized" << endl;
    case WHEEL_PENNY:
    case WHEEL_NICKEL:
    case WHEEL_DIME:
    case WHEEL_QUARTER:
    {      
      Coord1 cashVal = SlopeCoin::toCash(itemWon);
      Pizza::currGameVars[VAR_CASH] += cashVal;
      Pizza::currGameVars[VAR_SCORE] += cashVal;
      setWonSmall();
      break;
    }
    case WHEEL_BIG_CASH:
    {
      Coord1 cashVal = 100;
      Pizza::currGameVars[VAR_CASH] += cashVal;
      Pizza::currGameVars[VAR_SCORE] += cashVal;
      Pizza::scoreMan->earnedAchievement(ACH_WHEEL_DOLLAR);
      setWonBig();
      break;
    }
    case WHEEL_OUTFIT:
      Pizza::currIngredientUnlocked = Pizza::currIngredientFound;
      Pizza::scoreMan->earnedAchievement(ACH_SECRET_ING);
      setWonBig();
      break;
    case WHEEL_STAR:
      Pizza::currGameVars[VAR_WHEEL_EARNED] = 1;
      setWonBig();
      break;
    case WHEEL_NOTHING:
      setMiss();
      break;
  }
  
  endScript.wait(2.0);
}

void WheelOfPizza::callback(ActionEvent* caller)
{
  if (caller == &speedMover)
  {
    // cout << "final rot " << RM::standardize_rads(slice.getRotation()) << endl;
    wheelStopped();
  }
  else if (caller == &lightScript)
  {
    lightCallback();
  }
  else if (caller == &endScript)
  {
    scene->wheelDone();
  }
}

// =============================== WheelScene ============================ //

WheelScene::WheelScene() :
  PizzaScene(),
  wheel(this),
  wheelCam(),
  wheelActive(true)
{
  wheelCam.addY(SCREEN_H);
  releasesScreenshotEnd = true;
}

WheelScene::~WheelScene()
{
  ResourceManager::unloadWheel();  
}

void WheelScene::load()
{
  ResourceManager::loadWheel();
    
  wheel.init();
  if (Pizza::currIngredientFound.x != -1) wheel.insertItem(WheelOfPizza::WHEEL_OUTFIT);
  if (Pizza::currLevelData().goalTypes.y == LevelData::GOAL_WHEEL) wheel.insertItem(WheelOfPizza::WHEEL_STAR);
  
  PizzaScene::transIn();
  script.clear();  // this takes out the darkening
  darkAlpha = 0.0;
  
  ActionList* transInActions = new ActionList();
  transInActions->addX(new LinearFn(&darkAlpha, 0.5, 0.5));
  transInActions->addX(new CosineFn(&wheelCam.xy.y, 0.0, 0.5), true);
  
  script.enqueueX(transInActions);
}

void WheelScene::updateMe()
{
  updateActions();
  if (wheelActive == true) wheel.update();
  if (state == RM::ACTIVE) Pizza::controller->update();
}

void WheelScene::redraw()
{
  if (lastSceneRect.getImage() != NULL) 
  {
    lastSceneRect.drawMe();
  }
  
  CameraStd::screenBoxToWorld().draw_solid(ColorP4(0.0, 0.0, 0.0, darkAlpha));
  
  if (wheelActive == true)
  {
    RMGraphics->push_camera(wheelCam);
    wheel.redraw();
    RMGraphics->pop_camera();    
  }
  
  drawGUIs();
  actions.redraw();
  drawLoading();
  script.redraw();
  // drawLoading();
}

void WheelScene::transInEffect()
{
  
}

void WheelScene::startActive()
{

}

void WheelScene::wheelDone()
{
  script.enqueueX(new CosineFn(&wheelCam.xy.y, SCREEN_H, 0.5));
  script.enqueueX(new SetValueCommand<Logical>(&wheelActive, false));
  script.enqueueX(new PushGUICommand(new WinGUI()));
  script.enqueueX(new SetValueCommand<Coord1>(&state, RM::ACTIVE));
}


// =============================== WinGUI ============================ //

const Point1 
WinGUI::topOffset = -32.0,
WinGUI::leftOffset = -480.0,
WinGUI::rightOffset = 480.0,
WinGUI::botOffset = 128.0;

WinGUI::WinGUI() :
  PizzaGUI(),

  levelNumText("!-@", ResourceManager::font, Point2(0.5, 0.0)),
  cashText("$", ResourceManager::font),
  scoreText("0", ResourceManager::scoreFont, Point2(1.0, 0.0)),
  messageTextDrawer("", ResourceManager::winFont, Point2(0.5, 0.5)),

  victoryNormal(Point2(240.0, 48.0), imgsetCache[winVictorySet][0]),
  victoryBright(Point2(240.0, 48.0), imgsetCache[winVictorySet][1]),
  cashRect(Pizza::platScreenBox.corner(0) + Point2(0.0, topOffset), imgCache[winCash]),
  messageRect(Point2(240.0, 250.0), imgCache[winMessage]),

  earnedTextSet(3, true),
  goalTextSet(3, true),

  goalCheckSet(3, true),
  starSet(3, true),

  currMsg(-1),
  messageList(4),

  replayBtn(this),
  continueBtn(this),

  darkAlpha(0.0),
  bannerAlpha(0.0),

  starBits(0x0),
  levelWasBeaten(false),
  newStars(0),

  newHiScore(false),
  newTopWorld(false),
  newIngredient(false),
  gameBeaten(false),
  gameBeatenLite(false),
  newlyBeatenFull(false),
  newlyBeatenLite(false)
{ 
  levelWon();
  
  cashText.setXY(cashRect.getXY() + Point2(40.0, 0.0));
  cashText.setScale(0.73, 0.73);
  cashText.getTextPtr()->cash_string('$', Pizza::currGameVars[VAR_CASH]);
  cashText.textChanged();
  cashText.setColor(RM::color255(211, 227, 131));
  
  scoreText.setX(Pizza::platScreenBox.corner(1).x - 4.0);
  scoreText.getTextPtr()->int_replace('0', Pizza::currGameVars[VAR_SCORE]);
  scoreText.textChanged();
  
  levelNumText.setX(Pizza::platScreenBox.center().x);
  levelNumText.setScale(0.73, 0.73);
  levelNumText.setColor(RM::color255(255, 114, 0));
  levelNumText.getTextPtr()->int_replace('!', Pizza::currWorldPair.x + 1);
  levelNumText.getTextPtr()->int_replace('@', Pizza::currWorldPair.y + 1);
  levelNumText.textChanged();
  
  victoryNormal.setAlpha(0.0);
  victoryBright.setAlpha(0.0);
  victoryNormal.setScale(2.0, 2.0);
  victoryBright.setScale(2.0, 2.0);
  
  messageRect.setAlpha(0.0);
  messageRect.setScale(0.0, 0.0);
  
  messageTextDrawer.setAlpha(0.0);
  messageTextDrawer.setScale(0.0, 0.0);
  messageTextDrawer.setXY(messageRect.getXY());
  
  for (Coord1 i = 0; i < earnedTextSet.count; ++i)
  {
    Point2 baseXY = Point2(60.0, 76.0 + 52.0 * i);
    
    goalCheckSet[i].setXY(baseXY + Point2(leftOffset, 0.0));
    goalCheckSet[i].setImage(imgsetCache[winCheckSet][starWasEarned(i) ? 0 : 1]);
    
    earnedTextSet[i].setXY(baseXY + Point2(30.0 + leftOffset, 3.0));
    earnedTextSet[i].setFont(ResourceManager::font);
    
    starSet[i].setXY(baseXY + Point2(334.0 + rightOffset, 23.0));
    starSet[i].setImage(imgsetCache[winStarSet][1]);
    
    goalTextSet[i].setXY(baseXY + Point2(33.0 + leftOffset, 32.0));
    goalTextSet[i].setScale(0.5, 0.5);    
    goalTextSet[i].setFont(ResourceManager::font);
    
    setStarText(i);
  }
  
  replayBtn.init(Pizza::platScreenBox.corner(3) + Point2(0.0, -44.0 + botOffset),
                 imgsetCache[winReplaySet][0], imgsetCache[winReplaySet][1]);
  replayBtn.usesPlaySound = true;
  replayBtn.autoSize();
  widgetList.add(&replayBtn);
  
  continueBtn.init(Pizza::platScreenBox.corner(2) + Point2(-168.0, -48.0 + botOffset),
                   imgsetCache[winContinueSet][0], imgsetCache[winContinueSet][1]);
  continueBtn.usesPlaySound = true;
  continueBtn.autoSize();
  widgetList.add(&continueBtn);
  
  syncPositions();
  
  PizzaDirNode* currNode = addNode(&replayBtn, &continueBtn, NULL, &continueBtn, NULL);
  currNode->highlightPosOffset += Point2(4.0, -4.0);
  
  currNode = addNode(&continueBtn, &replayBtn, NULL, &replayBtn, NULL);
  currNode->highlightPosOffset += Point2(-4.0, -4.0);
  currNode->highlightSizeOffset += Point2(0.0, -6.0);

  setFirstNode(&continueBtn);
  
  transIn();
}

void WinGUI::update()
{
  updateActions();
  updateDirHighlight();
  syncPositions();
}

void WinGUI::redraw()
{
  levelNumText.drawText();
  cashText.drawText();
  scoreText.drawText();
  
  messageTextDrawer.drawText();
   
  victoryNormal.drawMe();
  victoryBright.drawMe();
  cashRect.drawMe();
  messageRect.drawMe();
  
  for (Coord1 i = 0; i < earnedTextSet.count; ++i)
  {
    goalCheckSet[i].drawMe();
    earnedTextSet[i].drawText();
    starSet[i].drawMe();
    goalTextSet[i].drawText();
  }
  
  drawActions();
  drawDirHighlight();
  
  if (darkAlpha > 0.01)
  {
    CameraStd::screenBoxToWorld().draw_solid(ColorP4(0.0, 0.0, 0.0, darkAlpha));
  }
  if (bannerAlpha > 0.01)
  {
    imgCache[winChapterBanner]->draw_scale(PLAY_CENTER, Point2(1.0, 1.0), 0.0, 
                                           ColorP4(1.0, 1.0, 1.0, bannerAlpha));
  }
}

void WinGUI::levelWon()
{
  LevelData& currLevel = Pizza::currLevelData();
  Coord1 oldTopWorld = Pizza::topWorldUnlocked();
  Coord1 oldStars = Pizza::playerStars;
  Coord1 oldStarsMain = Pizza::starsInFirst100();

  if (currLevel.starsEarned >= 1) levelWasBeaten = true;
  
  starBits = Pizza::starBitsForWonLevel();
  Coord1 starCount = 0;
  for (Coord1 i = 0; i < 10; ++i) starCount += (((0x1 << i) & starBits) != 0) ? 1 : 0;
  
  Pizza::currGameVars[VAR_SCORE] += Pizza::calcStarBonus(Pizza::currWorldPair.x, starCount);
  Pizza::currGameVars[VAR_SCORE] += Pizza::calcTimeBonus(currLevel, Pizza::currGameVars[VAR_SECONDS]);
  
  if (Pizza::currGameVars[VAR_SCORE] > currLevel.bestScore)
  {
    Pizza::playerScore -= currLevel.bestScore;
    currLevel.bestScore = Pizza::currGameVars[VAR_SCORE];
    Pizza::playerScore += currLevel.bestScore;
    newHiScore = true;
  }
  
  if (starCount > currLevel.starsEarned)
  {
    newStars = starCount - std::max(0, currLevel.starsEarned);
    currLevel.setStarsEarned(starCount);
  }
  
  if (Pizza::currIngredientUnlocked.x != -1)
  {
    Pizza::unlockTopping(Pizza::currIngredientUnlocked);
    RMSystem->analytics_event("Earned secret ingredient");
    newIngredient = true;
  }

  if (Pizza::lite == true &&
      Pizza::currWorldPair.x == 0 && Pizza::currWorldPair.y == 9)
  {
    if (newStars > 0) 
    {
      newlyBeatenLite = true;
      RMSystem->analytics_event("Game finished FREE");
    }
    
    gameBeatenLite = true;
  }
  else if (Pizza::lite == false &&
           Pizza::currWorldPair.x == 9 && Pizza::currWorldPair.y == 9)
  {
    if (newStars > 0) 
    {
      newlyBeatenFull = true;
      RMSystem->analytics_event("Game finished FULL");
    }
    
    gameBeaten = true;
  }
  
  Pizza::playerCash += Pizza::currGameVars[VAR_CASH];
  Pizza::savePlayer();
  
  Pizza::saveLevels();
  
  Pizza::updatePlayerStats();
  
  if (newHiScore == true) 
  {
    RMSystem->analytics_event("New hi-score on old level");
    Pizza::scoreMan->earnedScore(0, Pizza::playerScore);
  }
  
  newTopWorld = Pizza::topWorldUnlocked() > oldTopWorld;
  if (Pizza::lite == true) newTopWorld = false;
  // newTopWorld = true; /////////////////////

  if (oldStars <= 0 && Pizza::playerStars >= 1)
  {
    Pizza::showShopTut = true;
  }
  
  if (Pizza::playerStars >= 50)
  {
    if (oldStars < 50)
    {
      RMSystem->analytics_event("Earned 50 stars");
    }
    
    Pizza::scoreMan->earnedAchievement(ACH_50_STARS);
  }
  
  if (Pizza::starsInFirst100() >= 300)
  {
    if (oldStarsMain < 300)
    {
      RMSystem->analytics_event("Earned 300 stars");
    }
    
    Pizza::scoreMan->earnedAchievement(ACH_300_STARS);
  }
  
  // this is just for analytics
  if (newTopWorld == true) 
  {
    String1 finishedStr("World $ finished", '$', Pizza::currWorldPair.x);
    RMSystem->analytics_event(finishedStr);
  }
  if (newStars > 0 && Pizza::currWorldPair.x == WORLD_CLOUD && Pizza::currWorldPair.y == 0)
  {
    RMSystem->analytics_event("Beat first level in Chapter 11");
  }
}

void WinGUI::transIn()
{  
  state = RM::TRANS_IN;
  
  Point1 stampTime = 0.5;
  
  ActionList* stamp1 = new ActionList();
  stamp1->addX(new LinearFn(&victoryNormal.scale.x, 1.0, stampTime));
  stamp1->addX(new LinearFn(&victoryBright.scale.x, 1.0, stampTime));
  stamp1->addX(new LinearFn(&victoryNormal.color.w, 1.0, stampTime));
  stamp1->addX(new LinearFn(&victoryBright.color.w, 1.0, stampTime), true);

  ActionList* stamp2 = new ActionList();
  stamp2->addX(new LinearFn(&victoryBright.scale.x, 2.0, stampTime));
  stamp2->addX(new LinearFn(&victoryBright.color.w, 0.0, stampTime));
  stamp2->addX(new LinearFn(&cashRect.xy.y, cashRect.getY() - topOffset, stampTime), true);
  
  script.enqueueX(stamp1);
  script.enqueueX(new FunctionCommand0(&ResourceManager::playGoalSlide));
  script.enqueueX(stamp2);

  for (Coord1 i = 0; i < earnedTextSet.count; ++i)
  {
    ActionList* moveList = new ActionList();
    moveList->addX(new CosineFn(&goalCheckSet[i].xy.x, goalCheckSet[i].getX() - leftOffset, 0.5), false);
    moveList->addX(new CosineFn(&starSet[i].xy.x, starSet[i].getX() - rightOffset, 0.5), true);
    
    script.enqueueX(moveList);
    
    if (starWasEarned(i) == true)
    {
      ImageEffect* filledStar = new ImageEffect(imgsetCache[winStarSet][0]);
      filledStar->setXY(starSet[i].getXY() - Point2(rightOffset, 0.0));
      filledStar->setAlpha(0.0);
      actions.addX(filledStar);
      
      ActionList* fadeList = new ActionList();
      fadeList->addX(new CosineFn(&starSet[i].color.w, 1.0, 0.5));
      fadeList->addX(new CosineFn(&filledStar->color.w, 1.0, 0.5), true);
      
      PoofEffect* ring = new PoofEffect(filledStar->getXY(), imgCache[winRing]);
      ring->startScale = 0.5;
      ring->endScale = 6.0;
      ring->beforeFadeTime = 0.5;
      ring->fadeTime = 1.5;
      ring->scaleTime = 2.0;
      ring->init(); 
      
      script.enqueueAddX(ring, &actions);
            
      for (Coord1 sparkI = 0; sparkI < 50; ++sparkI) 
      {
        script.enqueueAddX(new WinSpark(ring->getXY()), &actions);
      }
      
      script.enqueueX(new FunctionCommand0(&ResourceManager::playWinStar));
      script.enqueueX(fadeList);
      
      for (Coord1 sparkI = 0; sparkI < 3; ++sparkI) 
      {
        script.enqueueAddX(new WinStarSparkle(ring->getXY()), &actions);
      }
    }
    else
    {
      // didn't get star
      script.wait(0.2);
      script.enqueueX(new FunctionCommand0(&ResourceManager::playStarMiss));
      script.wait(0.2);
    }

    script.wait(0.5);
  }
  
  if (levelWasBeaten == true && newStars == 1) enqueueMessage(String2(LSTRINGS[87], '$', newStars));
  else if (levelWasBeaten == true && newStars >= 2) enqueueMessage(String2(LSTRINGS[88], '$', newStars));
  
  if (levelWasBeaten == true && newHiScore == true) enqueueMessage(LSTRINGS[89]);
  if (newIngredient == true) enqueueMessage(LSTRINGS[90]);
  if (newTopWorld == true) enqueueMessage(LSTRINGS[91]);
  
  script.enqueueX(new CosineFn(&replayBtn.xy.y, replayBtn.getY() - botOffset, 0.5));
}

void WinGUI::syncPositions()
{
  victoryNormal.setYScale(victoryNormal.getXScale());
  victoryBright.setYScale(victoryBright.getXScale());
  
  cashText.setY(cashRect.getY());
  levelNumText.setY(cashRect.getY());
  scoreText.setY(cashRect.getY());
  
  continueBtn.setY(replayBtn.getY());
  
  messageRect.setScale(messageRect.getAlpha(), messageRect.getAlpha());
  
  if (currMsg >= 0) messageTextDrawer.setText(messageList[currMsg]);
  messageTextDrawer.scaleToTextSize(Point2(208.0, 64.0));
  Point2 textScale = messageTextDrawer.getScale();
  messageTextDrawer.setScale(textScale * messageRect.getScale());
  messageTextDrawer.setAlpha(messageRect.getAlpha());
  
  for (Coord1 i = 0; i < earnedTextSet.count; ++i)
  {    
    earnedTextSet[i].setXY(goalCheckSet[i].getXY() + Point2(30.0, 3.0));
    goalTextSet[i].setXY(goalCheckSet[i].getXY() + Point2(33.0, 32.0));
  }
}

void WinGUI::setStarText(Coord1 goalIndex)
{
  Point2 textMaxSize(240.0, 64.0);
  
  if (goalIndex == 0)
  {
    earnedTextSet[goalIndex].setText(LSTRINGS[92]);
    earnedTextSet[goalIndex].scaleToTextSize(textMaxSize);
    goalCheckSet[goalIndex].setImage(imgsetCache[winCheckSet][0]);
    goalTextSet[goalIndex].setText("");
    return;
  }

  LevelData& levelDef = Pizza::currLevelData();
  Coord1 currGoalType = levelDef.goalTypes[goalIndex - 1];
  Coord1 currEarnedVal = Pizza::currGameVars[Pizza::goalTypeToVarIndex(currGoalType)];
  Coord1 currGoalVal = levelDef.goalValues[goalIndex - 1];
  VisibleText& currEarnedText = earnedTextSet[goalIndex];
  VisibleText& currGoalText = goalTextSet[goalIndex];
  
  currEarnedText.setText(goalEarnedStrs[currGoalType]);
  currGoalText.setText(goalRequiredStrs[currGoalType]);
  
  if (currGoalType == LevelData::GOAL_TIME)
  {
    currEarnedText.getTextPtr()->time_string('$', currEarnedVal);
    currGoalText.getTextPtr()->time_string('$', currGoalVal);
  }
  else      
  {
    currEarnedText.getTextPtr()->int_replace('$', currEarnedVal);
    currGoalText.getTextPtr()->int_replace('$', currGoalVal);      
  }
  
  currEarnedText.textChanged();
  currEarnedText.scaleToTextSize(textMaxSize);

  currGoalText.textChanged();
}

Logical WinGUI::starWasEarned(Coord1 starIndex)
{
  return (starBits & (0x1 << starIndex)) != 0;
}

void WinGUI::enqueueMessage(const String2& msg)
{
  messageList.add(msg);
  
  script.enqueueX(new SetValueCommand<Coord1>(&currMsg, messageList.count - 1));
  script.enqueueX(new CosineFn(&messageRect.color.w, 1.0, 0.5));  // this also scales
  script.enqueueX(new FunctionCommand0(&ResourceManager::playWinNotification));
  script.wait(1.0);
  script.enqueueX(new CosineFn(&messageRect.color.w, 0.0, 0.3));  // this also scales
}

void WinGUI::click(Widget* caller)
{
  Pizza::useLoadImg = true;  // new top world + continue cancels it
  
  if (caller == &continueBtn)
  {
    if (gameBeatenLite == true)
    {
      // go to ending
      Pizza::currLevelDef.set(LEVEL_GLADIATOR, GLADIATOR_LITE_ENDING);
      RMSceneMan->sceneTransition(new PlayScene());
    }
    else if (gameBeaten == true)
    {
      // take them to cloud world after ending
      if (newTopWorld == true) Pizza::currWorldPair.x++;
      
      // go to ending
      Pizza::currLevelDef.set(LEVEL_GLADIATOR, GLADIATOR_ENDING);
      RMSceneMan->sceneTransition(new PlayScene());
      
    }
    else if (newTopWorld == true)
    {
      // win banner then map
      state = RM::PASSIVE;  // passive so it keeps drawing

      // the min is just in place for debugging
      Pizza::currWorldPair.x = std::min(Pizza::currWorldPair.x + 1, NUM_WORLD_TYPES - 1);
      Pizza::currWorldPair.y = 0;
      Pizza::useLoadImg = false;
      
      actions.addX(new SoundVolumeFade(soundCache[music], 0.0, 1.0));
      
      script.enqueueX(new LinearFn(&darkAlpha, 1.0, 1.0));
      script.wait(0.5);
      script.enqueueX(new LinearFn(&bannerAlpha, 1.0, 2.0));
      script.wait(1.0);
      script.enqueueX(new SceneTransitionCommand(new MapScene()));
    }
    else
    {
      RMSceneMan->sceneTransition(new MapScene());
    }    
  }
  else if (caller == &replayBtn)
  {
    RMSceneMan->sceneTransition(new PlayScene());    
  }
}

// =============================== WinSpark ============================ //

const Point1 WinSpark::SPEED = 300.0;

WinSpark::WinSpark(Point2 center) :
  VisRectangular(),
  Effect(),
  ActionListener(),

  scaler(&scale.x, 2.0 + RM::randf(0.0, 1.0), 2.0 + RM::randf(0.0, 1.0)),
  fader(&color.w, 0.0, 2.0, this),
  speedMult(RM::randf(0.45, 1.25))
{
  setXY(center);
  setRotation(RM::randf(0.0, TWO_PI));
  setScale(0.5, 0.5);
  setImage(imgCache[winStreak]);
}

void WinSpark::updateMe()
{
  addXY(RM::pol_to_cart(SPEED * speedMult * RM::timePassed(), getRotation()));
  scaler.update();
  fader.update();
  setYScale(getXScale());
}

void WinSpark::redrawMe()
{
  drawMe();
}

void WinSpark::callback(ActionEvent* caller)
{
  if (caller == &fader)
  {
    done();
  }
}

// =============================== WinStarSparkle ============================ //
// Note: also used on timing mode's carpet

WinStarSparkle::WinStarSparkle(Point2 center) :
  VisRectangular(center, imgCache[winSparkle]),
  Effect(),
  ActionListener(),
  
  scaler(&scale.x, 1.0, RM::randf(0.31, 0.7), this),
  rotationMult(RM::randl() ? -1.0 : 1.0),
  scaleIn(true),
  offset(RM::pol_to_cart(RM::randf(16.0, 32.0), RM::randf(0.0, TWO_PI)))
{
  setScale(0.0, 0.0);
}

void WinStarSparkle::updateMe()
{
  addRotation(TWO_PI * rotationMult * RM::timePassed());
  scaler.update();
  matchXScale();
}

void WinStarSparkle::redrawMe()
{
  drawMe(offset);
}

void WinStarSparkle::callback(ActionEvent* caller)
{
  if (scaleIn == true)
  {
    scaler.reset(0.0);
  }
  else
  {
    offset = RM::pol_to_cart(RM::randf(16.0, 32.0), RM::randf(0.0, TWO_PI));
    rotationMult = RM::randl() ? -1.0 : 1.0;
    scaler.reset(1.0, RM::randf(0.3, 0.7));
  }
  
  scaleIn = !scaleIn;
}

// =============================== UpgradeScene ============================ //

UpgradeScene::UpgradeScene(Logical setFromMap) :
  PizzaScene(),
  fromMap(setFromMap),
  nextSlide(0),

  slideList(),
  createTimer(8.0 * 0.333, true, this)
{
  
}

UpgradeScene::~UpgradeScene()
{
  ResourceManager::unloadUpgrade();
}

void UpgradeScene::load()
{
  ResourceManager::loadUpgrade();
  
  if (fromMap == true) RMSystem->analytics_event("Viewing upsell from map");
  else RMSystem->analytics_event("Viewing upsell after win");
  
  PizzaScene::transIn();
  
  callback(&createTimer);
  pushGUI(new LiteUpgradeGUI(fromMap));
}

void UpgradeScene::updateMe()
{
  createTimer.update();
  slideList.update();
  
  updateActions();
  if (state == RM::ACTIVE) Pizza::controller->update();
}

void UpgradeScene::redraw()
{
  imgCache[bg]->draw(-44.0, -32.0);
  slideList.redraw();
  drawActions();
  // drawLoading();
}

void UpgradeScene::startActive()
{
  releaseTransScreenshot();
}

void UpgradeScene::callback(ActionEvent* caller)
{
  if (caller == &createTimer)
  {
    Point1 slideDur = 8.0;
    
    ImageEffect* slide = new ImageEffect(imgsetCache[upgradeSlideSet][nextSlide]);
    slide->setXY(496.0, -20.0);
    slide->setScale(0.73, 0.73);
    slide->addX(new LinearFn(&slide->xy.x, -448.0, slideDur));
    slide->addX(new LinearFn(&slide->xy.y, 263.0, slideDur));
    slide->addX(new LinearFn(&slide->scale.x, 1.34, slideDur));
    slide->addX(new LinearFn(&slide->scale.y, 1.34, slideDur), true);    
    slideList.addX(slide);
    
    nextSlide++;
    nextSlide %= imgsetCache[upgradeSlideSet].count;
  }
  else
  {
    PizzaScene::callback(caller);
  }
}

// =============================== LiteUpgradeGUI ============================ //

LiteUpgradeGUI::LiteUpgradeGUI(Logical setFromMap) :
  RivermanGUI(),

  noThanks(this, Pizza::platScreenBox.corner(3) + Point2(0.0, -48.0), 
           imgsetCache[upgradeNoSet][0], imgsetCache[upgradeNoSet][1]),
  buyNow(this, Pizza::platScreenBox.corner(2) + Point2(-280.0, -72.0), 
         imgsetCache[upgradeYesSet][0], imgsetCache[upgradeYesSet][2]),

  fromMap(setFromMap),
  glowAlpha(0.0),
  glowData(0.0, 1.0)
{
  addWidget(&noThanks);
  addWidget(&buyNow);
  
  state = RM::ACTIVE;
}

void LiteUpgradeGUI::update()
{
  RM::bounce_arcsine(glowAlpha, glowData, Point2(0.0, 1.0), 2.0 * RM::timePassed());
  updateActions();
}

void LiteUpgradeGUI::redraw()
{
  drawActions();
  
  if (buyNow.hoverPress() == false)
  {
    imgsetCache[upgradeYesSet][1]->draw(buyNow.getXY(), glowAlpha);
  }
}

void LiteUpgradeGUI::click(Widget* caller)
{
  if (caller == &noThanks)
  {
    Pizza::useLoadImg = true;

    if (fromMap == true)
    {
      RMSceneMan->sceneTransition(new MapScene());
    }
    else
    {
      RMSceneMan->sceneTransition(new TitleScene());
    }
  }
  else if (caller == &buyNow)
  {
    RMSystem->analytics_event("Clicked upsell link");
    // RMSystem->open_url(FULL_VERSION_URL);
    RMSystem->view_store(475212472, String1("more_games_button_v1"));
  }
}


