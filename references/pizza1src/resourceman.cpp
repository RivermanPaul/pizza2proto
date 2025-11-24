/*
 *  graphicsman.cpp
 *  RMProject
 *
 *  Created by Paul Stevens on 9/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "resourceman.h"
#include "rmglobals.h"
#include "mainapp.h"
#include "rmdmorph.h"

DataList<Image*> imgCache(NUM_IMG_IDS, (Image*) NULL);
ArrayList<DataList<Image*> > imgsetCache(NUM_IMGSET_IDS, true);
DataList<DWarpSequence*> dwarpCache(NUM_DWARP_IDS, (DWarpSequence*) NULL);
DataList<DWarpAnimation*> warp3Cache(NUM_WARP3_IDS, (DWarpAnimation*) NULL);
DataList<Sound*> soundCache(NUM_SOUND_IDS, (Sound*) NULL);

namespace ResourceManager
{
  String1 workingStr(32);
  String2 efigsCharsStd(128);
  
  // common
  RMFont
  *font = NULL,
  *scoreFont = NULL,
  *winFont = NULL,
  *instructionsFont = NULL,
  *mapScoreFont = NULL;
  
  // shared
  Image 
  *lastScreenImg = NULL,
  *loadingImg = NULL,
  *extBG0Img = NULL,
  *extBG1Img = NULL,
  *extBG2Img = NULL,
  *extBG3Img = NULL,
  *extClockImg = NULL,
  *extBillImg = NULL,
  *extDividerLeftImg = NULL,
  *extDividerRightImg = NULL,
  *extSkyImg = NULL;

  // shop
  RMFont
  *shopCashFont = NULL,
  *shopItemFont = NULL;
  
  ArrayList<DataList<Image*> >
  shopItemSet(NUM_PIZZA_LAYERS, true);
  
  DataList<Image*> dirControllerHighlightSet(9);
  
  // play
  DataList<DWarpSequence*>
  plantWarps(3),
  fishBodyWarps(5),
  fishHeadWarps(5),
  angrySkullWarps(9),
  bounceSkullWarps(5);
  
  // THESE IMAGES ARE NOT OWNED, they just represent global animations
  DataList<Image*>
  pumpkinFly(4),
  cupidWingSet(11),
  wreckBoyAnim(4),
  wreckGirlAnim(4),
  wreckOldAnim(9),
  wreckThrowerFull,
  wreckArmFull,
  wreckCatcherThrow,
  wreckCatcherReverse,
  wreckClimberAttack,
  wreckClimberClimb,
  wreckWingHover,
  wreckWingAttack,
  wreckEasy1Full,
  wreckEasy2Full,
  wreckEasy3Full,
  wreckBatterFull,
  selector3x3Normal(9),
  selector3x3Fill(9),
  selector3x3Add(9);
  
  // common
  Sound
  *landing = NULL,
  *pizzaHit = NULL,
  *skeleBreak = NULL,
  *startSlam = NULL,
  *slamEnemy = NULL,
  *slamGround = NULL,
  *jump = NULL,
  *winLick = NULL,
  *loseLick = NULL,
  *clickStd = NULL,
  *clickPlay = NULL,
  *noClick = NULL,
  *enemyThrow = NULL,
  *enemyJumpSound = NULL,
  *rabbitJumpSound = NULL,
  *bounceSound = NULL,
  *downTick = NULL;
  
  Sound 
  *introMusic = NULL,
  *endingMusic = NULL,
  *carpetMusic = NULL;
  
  DataList<Sound*>
  crunchSet(3),
  coinSoundSet(5),
  collideSoundSet(4),
  bulletSet(4),
  timingResultSoundSet(4),
  timingStepSet(4);
  
  Point1 landWaiter(-0.01);
  
  void loadCommon()
  {
    RMSystem->read_utf8("flyer_efigs_strip.txt", efigsCharsStd);
    
    font = new RMFont(imgStr("flyer_30_efigs"), efigsCharsStd);
    font->widthAdjust = -7.0;

    Image::create_tiles_abs(imgStr("controller_selector"), dirControllerHighlightSet, 3, 3,
                            Point2(0.0, 0.0), true);

    loadingImg = Image::create_centered(imgStr("loading&"));
    
    Image::create_tiles_abs(imgStr("selector_normal"), selector3x3Normal, 3, 3, HANDLE_TL, false);
    Image::create_tiles_abs(imgStr("selector_add"), selector3x3Add, 3, 3, HANDLE_TL, true);
    Image::create_tiles_abs(imgStr("selector_fill"), selector3x3Fill, 3, 3, HANDLE_TL, true);
    
    // extender images
    extBG0Img = new Image(imgStr("extender_bg0"));
    extBG1Img = new Image(imgStr("extender_bg1"));
    extBG2Img = new Image(imgStr("extender_bg2"));
    extBG3Img = new Image(imgStr("extender_bg3"));
    extClockImg = new Image(imgStr("extender_clock"));
    extClockImg->handle_norm(HANDLE_TC);
    extBillImg = new Image(imgStr("extender_bill"));
    extBillImg->handle_norm(HANDLE_TC);
    extDividerLeftImg = new Image(imgStr("extender_divider_left"));
    extDividerLeftImg->handle_norm(HANDLE_TC);
    extDividerRightImg = new Image(imgStr("extender_divider_right"));
    extDividerRightImg->handle_norm(HANDLE_TC);
    extSkyImg = new Image(imgStr("extender_sky"));
    
    landing = Sound::create("land.wav");
    pizzaHit = Sound::create("ouch.wav");
    startSlam = Sound::create("smash_start.wav");
    slamEnemy = Sound::create("smash_enemy.wav");
    slamGround = Sound::create("smash_ground.wav");
    jump = Sound::create("jump.wav");
    downTick = Sound::create("click_down.wav");
    
    winLick = Sound::create("win.ima4");
    winLick->repeats(false);
    
    loseLick = Sound::create("lose.ima4");
    loseLick->repeats(false);
    
    for (Coord1 i = 0; i < 3; ++i)
    {
      crunchSet.add(Sound::create(String1("crunch$.wav", '$', (i+1))));
    }
    
    for (Char c = '1'; c <= '5'; ++c)
    {
      coinSoundSet.add(Sound::create(String1("coin$.wav", '$', c)));
    }
    
    for (Char c = '1'; c <= '4'; ++c)
    {
      collideSoundSet.add(Sound::create(String1("collide_$.wav", '$', c)));
    }
    
    clickStd = Sound::create("click_a.wav");
    clickPlay = Sound::create("click_b.wav");
    noClick = Sound::create("noclick.wav");
    enemyThrow = Sound::create("throw.wav");
    enemyJumpSound = Sound::create("enemy_jump.wav");
    rabbitJumpSound = Sound::create("rabbit.wav");
    bounceSound = Sound::create("bounce.wav");
  }
  
  void loadSplash()
  {
    if (RMSystem->get_OS_family() == MAC_FAMILY)
    {
      loadImgNorm(bg, "controls_mac");
    }
    else
    {
      loadImgNorm(bg, "controls");
    }
  }
  
  void unloadSplash()
  {
    unloadCaches();
  }
  
  void loadTitle()
  {
    dwarpCache[titleWarp] = new DWarpSequence("titlepizza.dwarp2", true);
    
    loadImgNorm(bg, "title_bg", Point2(0.0, 1.0));
    loadImgNorm(titleSky, "title_sky", Point2(0.0, 1.0));
    loadImgAbs(titleSpine, "title_spine", Point2(0.0, 20.0));
    loadImgAbs(titleSkull, "title_skull", Point2(152.0, 224.0));
    loadImgAbs(titleJaw, "title_jaw", Point2(152.0, 24.0));
    loadImgAbs(titlePizza, "title_pizza", Point2(256.0, 192.0));
    loadImgAbs(titleLogo, "title_logo", Point2(160.0, 112.0));
    loadImgAbs(titleRiverman, "title_riverman", Point2(148.0, 24.0));
    loadImgAbs(titlePresents, "title_presents&", Point2(36.0, 16.0));
    loadImgAbs(titlePizzaPad, "title_pizza_padding", Point2(300.0, 192.0));

    loadSound(music, "titlesplash.ima4");
  }
  
  void unloadTitle()
  {
    unloadCaches();
  }

  void loadMap()
  {
    scoreFont = new RMFont(imgStr("score_font"), '0', '9');
    scoreFont->widthAdjust = -5.0;

    instructionsFont = new RMFont(imgStr("level_font&"), '0' - 3, '9');
    instructionsFont->widthAdjust = -4.0;
    
    mapScoreFont = new RMFont(imgStr("score_font_big"), '0', '9');
    mapScoreFont->widthAdjust = -4.0;
    
    dwarpCache[mapChapterWarp] = new DWarpSequence("mapchapter2.dwarp2", true);
    dwarpCache[mapShopWarp] = new DWarpSequence("mappizzashop.dwarp2", true);
    dwarpCache[mapSkullWarp] = new DWarpSequence("mapskull.dwarp2", true);
    
    imgCache[bg] = new Image(imgStr("map_sky"));
    loadTilesAbs(mapLockSet, "map_icons_lock", 4, 1, Point2(28.0, 40.0));
    imgCache[mapStarCount] = new Image(imgStr("map_starcount"));
    imgCache[mapStarCountGlow] = new Image(imgStr("map_starcount_bright"));
    imgCache[mapShop] = Image::create_centered(imgStr("map_pizzashop"));
    imgCache[mapShopDown] = Image::create_centered(imgStr("map_pizzashop_down"));
    loadImgNorm(mapBubbleShop, "shop_tag&");
    
    loadImgAbs(winStreak, "win_streak", Point2(28.0, 4.0), true);
    loadTilesNorm(smokeSet, "smoke", 4, 1, HANDLE_C);
    
    Image::create_tiles(imgStr("map_arrows"), imgsetCache[mapArrowSet], 4, 1);
    Image::create_tiles(imgStr("map_buttons"), imgsetCache[mapSocialIcons], 4, 2);
    Image::create_tiles_abs(imgStr("map_icons_nums"), imgsetCache[mapNumbers], 3, 3, Point2(36.0, 36.0));
    Image::create_tiles_abs(imgStr("map_icons_stars&"), imgsetCache[mapStarSet], 3, 2, Point2(28.0, -16.0));

    loadImgAbs(instructionsHiScore, "instructions_hiscore&");
    loadImgAbs(instructionsStar, "instructions_star");
    loadTilesAbs(instructionsPlaySet, "instructions_play&", 1, 2);
    loadTilesAbs(instructionsCloseSet, "instructions_close&", 1, 2);

    // more games button
    loadTilesNorm(moregamesAnimSet, "more_riverman_game", 10, 6, HANDLE_TL, true);
    loadTilesNorm(moregamesButtonSet, "more_riverman_phone", 2, 1, HANDLE_TL, false);

    loadSound(music, "map.ima4");
    loadSound(lockBreakSound, "lockbreak.wav");
    loadSound(moreRivermanSound, "more_riverman.wav");
    loadSound(rivermanInterstitialSound, "riverman_interstitial.wav");
  }
  
  void unloadMap()
  {
    delete scoreFont;
    delete instructionsFont;
    delete mapScoreFont;
    
    unloadCaches();
  }
  
  void loadShop()
  {
    loadPizza();
    
    shopCashFont = new RMFont(imgStr("shop_cash_font"), '0' - 2, '9');
    shopCashFont->widthAdjust = -3;
    
    shopItemFont = new RMFont(imgStr("shop_item_font"), efigsCharsStd);
    shopItemFont->widthAdjust = -3;
    
    imgCache[bg] = new Image(imgStr("shop_sky"));
    
    if (RMSystem->get_OS_family() != APPLETV_FAMILY)
    {
      imgCache[shopBaseLeft] = new Image(imgStr("shop_base_left&"));
      imgCache[shopBaseRight] = new Image(imgStr("shop_base_right"));
    }
    else
    {
      imgCache[shopBaseLeft] = new Image(imgStr("shop_base_left_noshare&"));
      imgCache[shopBaseRight] = new Image(imgStr("shop_base_right_noshare"));
    }
    
    imgCache[shopSelector] = Image::create_handled_abs(imgStr("shop_selector"), Point2(22.0, 22.0));
    imgCache[shopDone] = new Image(imgStr("shop_button_done&"));
    imgCache[shopShare] = new Image(imgStr("shop_button_share&"));
    imgCache[shopConfirm] = new Image(imgStr("shop_buy_confirm&"));
    imgCache[shopYes] = new Image(imgStr("shop_buy_confirm_yes&"));
    imgCache[shopNo] = new Image(imgStr("shop_buy_confirm_no&"));
    
    imgsetCache[shopTabSet].ensure_count(NUM_PIZZA_LAYERS);
    imgsetCache[shopTabSet][CHEESE_LAYER] = new Image(imgStr("shop_tab_cheese&"));
    imgsetCache[shopTabSet][TOPPINGS_LAYER] = new Image(imgStr("shop_tab_toppings&"));
    imgsetCache[shopTabSet][EYES_LAYER] = new Image(imgStr("shop_tab_eyes&"));
    imgsetCache[shopTabSet][MOUTH_LAYER] = new Image(imgStr("shop_tab_mouth&"));
    imgsetCache[shopTabSet][CLOTHES_LAYER] = new Image(imgStr("shop_tab_clothes&"));
    
    loadImgNorm(shopQuestion, "shop_question");
    
    Image::create_tiles(imgStr("shop_selections_cheese&"), shopItemSet[CHEESE_LAYER], 4, 4);
    Image::create_tiles(imgStr("shop_selections_toppings&"), shopItemSet[TOPPINGS_LAYER], 4, 4);
    Image::create_tiles(imgStr("shop_selections_eyes&"), shopItemSet[EYES_LAYER], 4, 4);
    Image::create_tiles(imgStr("shop_selections_mouth&"), shopItemSet[MOUTH_LAYER], 4, 4);
    Image::create_tiles(imgStr("shop_selections_clothes&"), shopItemSet[CLOTHES_LAYER], 4, 4);
    
    loadSound(music, "tuscany.ima4");
  }
  
  void unloadShop()
  {
    delete shopCashFont;
    delete shopItemFont;
    
    for (Coord1 i = 0; i < shopItemSet.count; ++i) free_clear(shopItemSet[i]);
    
    unloadCaches();
  }
  
  void loadShare()
  {
    loadPizza();
    
    loadImgAbs(shareBackBones, "share_back_bones", Point2(192.0, 60.0));
    loadImgAbs(shareFrontBones, "share_front_bones", Point2(240.0, 66.0));
    loadImgNorm(shareSelectBase, "share_bg_select_base", Point2(0.0, 1.0));
    loadImgNorm(shareSelector, "share_bg_selector");
    loadImgNorm(shareShareBase, "share_button_base&");
    loadImgNorm(shareShareDown, "share_button_down&");
    loadImgNorm(shareLogo, "share_logo");
    loadImgNorm(shareRiverman, "share_riverman", Point2(1.0, 1.0));
    
    loadTilesNorm(shareCloseSet, "share_close", 1, 2, Point2(1.0, 0.0));

    loadSound(music, "tuscany.ima4");
    loadSound(cameraSound, "camera.wav");
  }
  
  void unloadShare()
  {
    unloadCaches();
  }
  
  void loadPlay(Logical shouldLoadPizza)
  {
    if (shouldLoadPizza == true) loadPizza();

    scoreFont = new RMFont(imgStr("score_font"), '0', '9');
    scoreFont->widthAdjust = -5.0;
    
    instructionsFont = new RMFont(imgStr("level_font&"), '0' - 3, '9');
    instructionsFont->widthAdjust = -4.0;

    imgCache[slamShock] = Image::create_handled_abs(imgStr("slam_shock"), Point2(118.0, 34.0));
    imgCache[slamStreak] = Image::create_handled_abs(imgStr("slam_streak"), Point2(136.0, 80.0));
    imgCache[slamStar] = Image::create_centered(imgStr("slam_star"));
    
    Image::create_tiles_abs(imgStr("sauce_drops"), imgsetCache[sauceDrops], 5, 1, Point2(16.0, 8.0));
    Image::create_tiles_abs(imgStr("sauce_splats"), imgsetCache[sauceSplats], 1, 3, Point2(16.0, 32.0));
    
    Image::create_tiles(imgStr("smoke"), imgsetCache[smokeSet], 4, 1, true);
    Image::create_tiles_abs(imgStr("sparks"), imgsetCache[sparkSet], 1, 4, Point2(48.0, 8.0));
    imgCache[blinkStar] = Image::create_centered(imgStr("blinkstar"));
    imgCache[burstRing] = Image::create_centered(imgStr("burstring"));

    loadTilesNorm(pennySet, "penny", 4, 2, Point2(0.5, 0.5));
    loadTilesNorm(nickelSet, "nickel", 4, 2, Point2(0.5, 0.5));
    loadTilesNorm(dimeSet, "dime", 4, 2, Point2(0.5, 0.5));
    loadTilesNorm(quarterSet, "quarter", 4, 2, Point2(0.5, 0.5));
    loadTilesNorm(coinSparkleSet, "coin_sparkles", 4, 4, HANDLE_C);
    
    loadImgNorm(secretOutfit, "clothes_icon", HANDLE_C);
    loadImgNorm(secretOutfitGlow, "clothes_icon_glow", HANDLE_C, true);
    
    loadTilesAbs(healthBarSet, "health", 2, 1, Point2(36.0, 36.0));
    
    loadImgNorm(winBanner, "level_complete&", Point2(0.5, 0.5));
    loadImgAbs(loseBanner, "fail_failed&", Point2(140.0, 56.0));
    loadTilesAbs(loseQuitSet, "fail_quit&", 1, 2);
    loadTilesAbs(loseInstructionsSet, "fail_instructions&", 1, 2);
    loadTilesAbs(loseRetrySet, "fail_retry&", 1, 2);
    
    loadImgAbs(pauseBaseTop, "ui_pause_base_top&", Point2(108.0, 160.0));
    loadImgAbs(pauseBaseMid, "ui_pause_base_middle&", Point2(224.0, 84.0));
    loadImgAbs(pauseBaseBot, "ui_pause_base_bottom&", Point2(116.0, -68.0));
    
    imgCache[pauseInstructions] = new Image(imgStr("ui_pause_instructions&"));
    imgCache[pausePlay] = new Image(imgStr("ui_pause_play&"));
    imgCache[pauseRestart] = new Image(imgStr("ui_pause_restart&"));
    imgCache[pauseQuit] = new Image(imgStr("ui_pause_quit&"));
    imgCache[pauseCalibrate] = new Image(imgStr("ui_pause_calibrate&"));
    imgCache[pauseCalibrateArrow] = Image::create_handled_abs(imgStr("ui_pause_calibrate_arrow"), Point2(28.0, 28.0));
    loadTilesNorm(soundBoxSet, "ui_pause_sound", 2, 2);
    
    Image::create_tiles_norm(imgStr("pause_hud"), imgsetCache[pauseSet], 1, 2, Point2(0.0, 0.0));
    
    Bitmap* whiteBmp = new Bitmap(Coord2(1, 1));
    whiteBmp->data[0] = (Byte4) 0xffffffff;
    imgCache[losePx] = new Image(Texture::create(*whiteBmp));
    imgCache[losePx]->multiply();
    delete whiteBmp;
    
    loadImgAbs(instructionsHiScore, "instructions_hiscore&");
    loadImgAbs(instructionsStar, "instructions_star");
    loadTilesAbs(instructionsPlaySet, "instructions_play&", 1, 2);
    loadTilesAbs(instructionsCloseSet, "instructions_close&", 1, 2);
    
    loadSound(outfitSound, "outfitfound.wav");
    loadSound(bombSound, "bomb.wav");
    loadSound(sunAttackSound, "sunattack.wav");
    loadSound(lightningSound, "lightning.wav");
  }
  
  void unloadPlay()
  {
    delete scoreFont;
    delete instructionsFont;
    
    unloadCaches();
  }
  
  void loadWheel()
  {
    winFont = new RMFont(imgStr("win_message_font_efigs"), efigsCharsStd);
    winFont->widthAdjust = -3.0;

    scoreFont = new RMFont(imgStr("score_font"), '0', '9');
    scoreFont->widthAdjust = -5.0;

    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_w")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_h")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_e1")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_e2")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_l")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_p")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_i")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_z1")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_z2")));
    imgsetCache[wheelTextSet].add(new Image(imgStr("spinner_text_lit_a")));
    
    Image::create_tiles_abs(imgStr("spinner_slice"), imgsetCache[wheelSliceSet], 1, 2, Point2(32.0, 48.0));
    Image::create_tiles(imgStr("spinner_bulb"), imgsetCache[wheelBulbSet], 1, 2, true);
    
    imgCache[wheelTextLeft] = new Image(imgStr("spinner_text_wheel_base"));
    imgCache[wheelTextRight] = new Image(imgStr("spnner_text_pizza_base"));
    imgCache[wheelCircle] = Image::create_centered(imgStr("spinner_wheel"));
    imgCache[wheelNeon] = Image::create_handled_abs(imgStr("spinner_wheel_neon"), Point2(4.0, 40.0));
    
    loadTilesNorm(wheelIconSet, "spinner_icons", 7, 1, HANDLE_C);
    loadTilesNorm(coinSparkleSet, "coin_sparkles", 4, 4, HANDLE_C);
    
    loadTilesAbs(winVictorySet, "win_victory&", 1, 2, Point2(104.0, 32.0));
    loadImgAbs(winCash, "win_cash");
    loadTilesAbs(winCheckSet, "win_check_x", 1, 2);
    loadTilesAbs(winStarSet, "win_star", 1, 2, Point2(34.0, 27.0));
    loadImgNorm(winRing, "win_burst_ring", Point2(0.5, 0.5), true);
    loadImgAbs(winStreak, "win_streak", Point2(28.0, 4.0), true);
    loadImgNorm(winMessage, "win_message", Point2(0.5, 0.5), true);
    loadTilesNorm(winReplaySet, "win_replay&", 1, 2);
    loadTilesNorm(winContinueSet, "win_continue&", 1, 2);
    loadImgNorm(winSparkle, "win_sparkle", HANDLE_C);
    
    // +1 is for 1-indexing, +2 to get one world ahead
    Coord1 currWorldNum = std::min(Pizza::currWorldPair.x + 2, NUM_WORLD_TYPES + 0);
    String1 bannerStr("chapter_$&", '$', currWorldNum);
    loadImgAbs(winChapterBanner, bannerStr.as_cstr(), Point2(184.0, 40.0));
    
    loadSound(music, "wheelofpizza.ima4");
    loadSound(wheelClickSound, "wheelclick.wav");
    loadSound(wheelMissSound, "wheelmiss.wav");
    loadSound(wheelStopSound, "wheelstop.wav");
    loadSound(wheelWinSmall, "wheelwinsmall.wav");
    loadSound(wheelWinBig, "wheelwinbig.wav");
    loadSound(messageSound, "message.wav");
    loadSound(starGotSound, "starget.wav");
    loadSound(starMissSound, "starmiss.wav");
    loadSound(goalSlideSound, "goalslide.wav");
  }
  
  void unloadWheel()
  {
    delete winFont;
    delete scoreFont;
    
    unloadCaches();
  }
  
  void loadUpgrade()
  {
    loadImgNorm(bg, "upsell_bg&");
    
    for (Char i = 0; i < 10; ++i)
    {
      String1 tileStr("upsell_slide_$&", '$', (i + 1));
      addImgAbs(upgradeSlideSet, tileStr.as_cstr(), Point2(-6.0, 78.0));
    }
    
    loadTilesAbs(upgradeYesSet, "upsell_full&", 1, 3);
    loadTilesAbs(upgradeNoSet, "upsell_no&", 1, 2);

    loadSound(music, "trailer.ima4");
  }
  
  void unloadUpgrade()
  {
    unloadCaches();
  }
  
  void loadPizza()
  {
    loadBase();
    loadEyes();
    loadMouth();
    loadClothes();
    loadToppings();
    
    // intro glow
    imgCache[pizzaGlow] = Image::create_centered(imgStr("pizza_intro_glow"), true);    
  }
  
  void loadBase(Logical unloadCurr, Logical loadDamage)
  {
    if (unloadCurr == true)
    {
      delete imgCache[pizzaBase];
      free_clear(imgsetCache[pizzaSauceSet]);
    }
    
    // base
    Coord1 baseIndex = Pizza::playerOutfit[CHEESE_LAYER];
    
    String1 baseStr("pizza_base_$", 32);
    baseStr.int_replace('$', baseIndex);
    baseStr += imgSuffix;
    
    imgCache[pizzaBase] = Image::create_centered(baseStr);
    
    // damage
    if (loadDamage == true)
    {
      String1 dmgStr("pizza_base_$_dmg", 32);
      dmgStr.int_replace('$', baseIndex);
      dmgStr += imgSuffix;
      
      Image::create_tiles(dmgStr, imgsetCache[pizzaSauceSet], 2, 2, true);      
    }
  }
  
  void loadEyes(Logical unloadCurr)
  {
    if (unloadCurr == true)
    {
      delete imgCache[pizzaEyes];
      delete dwarpCache[eyeWarp];
    }
    
    Coord1 eyeIndex = Pizza::playerOutfit[EYES_LAYER];
    
    String1 eyesStr("pizza_eyes_$", 32);
    eyesStr.int_replace('$', eyeIndex);
    eyesStr += imgSuffix;
    
    imgCache[pizzaEyes] = Image::create_handled_abs(eyesStr, EYE_HANDLES[eyeIndex]);
    
    dwarpCache[eyeWarp] = new DWarpSequence(String1("pizzaeyes$.dwarp2", '$', eyeIndex).as_cstr(), 
                                            true);
  }
  
  void loadMouth(Logical unloadCurr)
  {
    if (unloadCurr == true)
    {
      delete imgCache[pizzaMouth];
      delete dwarpCache[mouthWarp];
    }
    
    Coord1 mouthIndex = Pizza::playerOutfit[MOUTH_LAYER];
    
    String1 mouthStr("pizza_mouth_$", 32);
    mouthStr.int_replace('$', mouthIndex);
    mouthStr += imgSuffix;
    
    imgCache[pizzaMouth] = Image::create_handled_abs(mouthStr, MOUTH_HANDLES[mouthIndex]);
    
    dwarpCache[mouthWarp] = new DWarpSequence(String1("pizzamouth$.dwarp2", '$', mouthIndex).as_cstr(),
                                              true);

  }
  
  void loadClothes(Logical unloadCurr)
  {
    if (unloadCurr == true)
    {
      delete imgCache[pizzaClothes];
      delete dwarpCache[clothesWarp];
    }
      
    Coord1 clothesIndex = Pizza::playerOutfit[CLOTHES_LAYER];
    
    String1 clothesStr("pizza_clothes_$", 32);
    clothesStr.int_replace('$', clothesIndex);
    clothesStr += imgSuffix;
    
    imgCache[pizzaClothes] = Image::create_handled_abs(clothesStr, CLOTHES_HANDLES[clothesIndex]);

    dwarpCache[clothesWarp] = 
        new DWarpSequence(String1("pizzaclothes$.dwarp2", '$', clothesIndex).as_cstr(), true);
  }
  
  void loadToppings(Logical unloadCurr)
  {
    if (unloadCurr == true)
    {
      free_clear(imgsetCache[pizzaToppingSet]);
    }
      
    Coord1 toppingsIndex = Pizza::playerOutfit[TOPPINGS_LAYER];
    
    String1 toppingStr("pizza_topping_$", 32);
    toppingStr.int_replace('$', toppingsIndex);
    toppingStr += imgSuffix;
    
    Image::create_tiles(toppingStr, imgsetCache[pizzaToppingSet], 5, 1);
    
    for (Coord1 i = 0; i < TOPPINGS_PER_PIZZA; ++i)
    {
      imgsetCache[pizzaToppingSet][i]->handle = TOPPING_HANDLES[toppingsIndex][i];
    }
  }
  
  void unloadCaches()
  {
    for (Coord1 i = 0; i < imgCache.count; ++i)
    {
      delete imgCache[i];
      imgCache[i] = NULL;
    }

    for (Coord1 i = 0; i < imgsetCache.count; ++i)
    {
      free_clear(imgsetCache[i]);
    }

    for (Coord1 i = 0; i < dwarpCache.count; ++i)
    {
      delete dwarpCache[i];
      dwarpCache[i] = NULL;
    }
    
    for (Coord1 i = 0; i < soundCache.count; ++i)
    {
      delete soundCache[i];
      soundCache[i] = NULL;
    }
    
    for (Coord1 i = 0; i < warp3Cache.count; ++i)
    {
      delete warp3Cache[i];
      warp3Cache[i] = NULL;
    }
  }
  
  String1& imgStr(const Char* imgBase)
  {
    workingStr.clear();
    workingStr += imgBase;
    
    if (workingStr.last() == '&')
    {
      workingStr.remove_last();
      workingStr += langSuffix;
    }
    
    workingStr += imgSuffix;
    return workingStr;
  }
  
  void loadImgNorm(ImageID imgIndex, const Char* imgBase, Point2 normHandle, Logical overexpose)
  {
    imgCache[imgIndex] = Image::create_handled_norm(imgStr(imgBase), normHandle, overexpose);
  }
  
  void loadImgAbs(ImageID imgIndex, const Char* imgBase, Point2 absHandle, Logical overexpose)
  {
    imgCache[imgIndex] = Image::create_handled_abs(imgStr(imgBase), absHandle, overexpose);
  }
  
  void loadTilesNorm(ImageSetID setIndex, const Char* imgBase, 
                     Coord1 cols, Coord1 rows, Point2 normHandle, Logical overexpose)
  {
    Image::create_tiles_norm(imgStr(imgBase), imgsetCache[setIndex], cols, rows, normHandle, overexpose);
  }
  
  void loadTilesAbs(ImageSetID setIndex, const Char* imgBase, Coord1 cols, Coord1 rows, 
                    Point2 absHandle, Logical overexpose)
  {
    Image::create_tiles_abs(imgStr(imgBase), imgsetCache[setIndex], cols, rows, absHandle, overexpose);
  }
  
  void addImgNorm(ImageSetID setIndex, const Char* imgBase, Point2 normHandle, Logical overexpose)
  {
    imgsetCache[setIndex].add(Image::create_handled_norm(imgStr(imgBase), normHandle, overexpose));
  }
  
  void addImgAbs(ImageSetID setIndex, const Char* imgBase, Point2 absHandle, Logical overexpose)
  {
    imgsetCache[setIndex].add(Image::create_handled_abs(imgStr(imgBase), absHandle, overexpose));    
  }
  
  void loadWarp(DWarpID warpIndex, const Char* filename)
  {
    dwarpCache[warpIndex] = new DWarpSequence(filename, true);
  }
  
  void loadWarp3(Warp3AnimationID warpIndex, const Char* warpFilename, ImageSetID setID)
  {
    // warp3Cache[warpIndex] = new DWarpAnimation(warpFilename, imgsetCache[setID]);
    warp3Cache[warpIndex] = new DWarpAnimation();
    loadWarp3Helper(*warp3Cache[warpIndex], warpFilename, imgsetCache[setID]);
  }

  void loadWarp3Helper(DWarpAnimation& warp, const Char* warpFilename, const DataList<Image*>& imgSet)
  {
    String1 warpStr;
    ArrayList<String1> lines;
    ArrayList<String1> lineParts;

    RMSystem->read_file(warpFilename, warpStr);
    warpStr.split(lines, '\n');

    // DWARP3
    Coord1 lineIndex = 0;
    rmassert(lines[lineIndex] == "DWARP3");

    // TEXTURES X
    lines[++lineIndex].split(lineParts);
    rmassert(lineParts[0] == "TEXTURES");
    Coord1 textureCount = lineParts[1].as_int();
    rmassert(imgSet.count == textureCount);

    for (Coord1 i = 0; i < textureCount; ++i)
    {
      // TEXTURE_NAME
      lines[++lineIndex];  // this is each texture's name
    }

    // LAYERS X
    lines[++lineIndex].split(lineParts);
    rmassert(lineParts[0] == "LAYERS");
    warp.imageLayers.ensure_capacity(lineParts[1].as_int());
    Coord1 layerCount = lineParts[1].as_int();
    warp.warpLayers.ensure_capacity(layerCount);

    // playback style
    Logical loops = false;

    ++lineIndex;
    if (lines[lineIndex] == "LOOP")
    {
      loops = true;
    }
    else
    {
      rmassert(lines[lineIndex] == "PLAYONCE" || lines[lineIndex] == "PINGPONG");
      loops = false;
    }

    for (Coord1 layer = 0; layer < layerCount; ++layer)
    {
      warp.warpLayers.add(new DWarpSequence());

      // LAYER_NICKNAME TEX_INDEX HANDLE_X HANDLE_Y
      lines[++lineIndex].split(lineParts);
      warp.imageLayers.add(imgSet[lineParts[1].as_int()]);
      warp.warpLayers.last()->layerHandle.set(lineParts[2].as_int(),
                                              lineParts[3].as_int());
    }

    for (Coord1 layer = 0; layer < layerCount; ++layer)
    {
      // LAYER X
      lines[++lineIndex].split(lineParts);
      rmassert(lineParts[0] == "LAYER");
      rmassert(lineParts[1].as_int() == layer);

      // BASEMESH
      rmassert(lines[++lineIndex] == "BASEMESH");

      // VERTICES
      lines[++lineIndex].split(lineParts);
      rmassert(lineParts[0] == "VERTICES");
      Coord1 baseVertices = lineParts[1].as_int();

      for (Coord1 i = 0; i < baseVertices; ++i)
      {
        // X Y
        lines[++lineIndex].split(lineParts);

        Point2 pt(lineParts[0].as_double(), lineParts[1].as_double());
        warp.warpLayers[layer]->baseMesh.points.add(pt);
      }

      // POLYS X
      lines[++lineIndex].split(lineParts);
      rmassert(lineParts[0] == "POLYS");
      Coord1 polyCount = lineParts[1].as_int();

      for (Coord1 i = 0; i < polyCount; ++i)
      {
        // I0 I1 I2 I3
        lines[++lineIndex].split(lineParts);
        Short4 vertices(lineParts[1].as_int(), lineParts[2].as_int(),
                        lineParts[3].as_int(), -1);

        if (lineParts[0] == "TRI")
        {
          // cout << "Adding TRI " << vertices << endl;
        }
        else if (lineParts[0] == "QUAD")
        {
          vertices[3] = lineParts[4].as_int();
          // cout << "Adding QUAD " << vertices << endl;
        }

        warp.warpLayers[layer]->baseMesh.polys.add(vertices);
      }

      // FRAMECOUNT X
      lines[++lineIndex].split(lineParts);
      rmassert(lineParts[0] == "FRAMECOUNT");
      Coord1 frameCount = lineParts[1].as_int();
      warp.warpLayers[layer]->warpFrames.ensure_count(frameCount + (loops ? 1 : 0));

      // draw order data for compatibility with new drawing function
      warp.drawOrderSet.ensure_count(frameCount);

      for (Coord1 i = 0; i < frameCount; ++i)
      {
        warp.drawOrderSet[i].add(layer);
      }

      for (Coord1 frame = 0; frame < frameCount; ++frame)
      {
        warp.warpLayers[layer]->warpFrames[frame] = DWarpFrame();

        // FRAME X
        lines[++lineIndex].split(lineParts);
        rmassert(lineParts[0] == "FRAME");
        rmassert(lineParts[1].as_int() == frame);

        // VISIBLE (or HIDDEN)
        lines[++lineIndex].split(lineParts);
        if (lines[lineIndex] == "VISIBLE")
        {
          warp.warpLayers[layer]->warpFrames[frame].visible = true;
        }
        else
        {
          rmassert(lines[lineIndex] == "HIDDEN");
          warp.warpLayers[layer]->warpFrames[frame].visible = false;
        }

        // COLOR alpha red green bloe
        lines[++lineIndex].split(lineParts);
        rmassert(lineParts[0] == "COLOR");
        rmassert(lineParts.count == 5);

        ColorP4 layerColor(lineParts[2].as_double(), lineParts[3].as_double(),
                           lineParts[4].as_double(), lineParts[1].as_double());
        warp.warpLayers[layer]->warpFrames[frame].color = layerColor;

        // VERTICES X
        lines[++lineIndex].split(lineParts);
        rmassert(lineParts[0] == "VERTICES");
        Coord1 frameVertices = lineParts[1].as_int();
        warp.warpLayers[layer]->warpFrames[frame].points.ensure_capacity(frameVertices);
        // cout << "VERTICES " << frameVertices << endl;

        for (Coord1 i = 0; i < frameVertices; ++i)
        {
          // X Y
          lines[++lineIndex].split(lineParts);
          
          Point2 pt(lineParts[0].as_double(), lineParts[1].as_double());
          warp.warpLayers[layer]->warpFrames[frame].points.add(pt);
        }
      }
      
      if (loops == true)
      {
        warp.warpLayers[layer]->warpFrames.last() = DWarpFrame(warp.warpLayers[layer]->warpFrames.first());
      }
    }
  }


  void loadSound(SoundID soundIndex, const Char* filename)
  {
    soundCache[soundIndex] = Sound::create(filename);
  }
  
  // ================= sounds
  
  void update()
  {
    RM::flatten_me(landWaiter, -0.01, RM::timePassed());
  }
  
  void playCrunch() {RM::random(crunchSet)->play();}
  void playLanding(Logical overrideStall) {playCollide();}
  void playPizzaBoxBreak() {collideSoundSet.first()->play();}
  void playCollide() {RM::random(collideSoundSet)->play();}
  void playJump() {jump->play();}
  void playDamaged() {pizzaHit->play();}
  void playSlamStart() {startSlam->play();}
  void playSlamLand() {slamGround->play();}
  void playFishEat() {soundCache[fishEatSound]->play();}
  void playCoinGet() {RM::random(coinSoundSet)->play();}
  void playOutfitFound() {soundCache[outfitSound]->play();}
  void playClickStd() {clickStd->play();}
  void playClickPlay() {clickPlay->play();}
  void playCantClick() {noClick->play();}
  void playLockBreak() {soundCache[lockBreakSound]->play();}
  void playLightning() {soundCache[lightningSound]->play();}
  void playSunAttack() {soundCache[sunAttackSound]->play();}
  void playHarpoonShoot() {soundCache[harpoonSound]->play();}
  void playSwim() {soundCache[swimSound]->play();}
  void playSkeleRise() {soundCache[skeleRiseSound]->play();}
  void playDemonSwoop() {soundCache[demonSwoopSound]->play();}
  void playOstrichHop() {enemyJumpSound->play();}
  void playGiantThrow() {soundCache[giantThrowSound]->play();}
  void playCupidShoot() {soundCache[cupidSound]->play();}
  void playNagaCharge() {soundCache[nagaSound]->play();}
  void playWispShoot() {soundCache[wispShootSound]->play();}
  void playWispDie() {soundCache[wispDieSound]->play();}
  void playRobotPhase() {soundCache[roboPhaseSound]->play();}
  void playRobotFlame() {soundCache[roboFlameSound]->play();}
  void playRabbitJump() {rabbitJumpSound->play();}
  void playTailRise() {soundCache[skeleRiseSound]->play();}
  void playMoleThrow() {enemyThrow->play();}
  void playFireballJump() {soundCache[fireballJumpSound]->play();}
  void playMimicJump() {enemyJumpSound->play();}
  void playBark() {soundCache[puppyBark]->play();}
  void playMissile() {soundCache[missileSound]->play();}
  void playBounceComet() {soundCache[cometSound]->play();}
  void playSumoAngry() {soundCache[sumoAngrySound]->play();}
  void playSumoLaser() {soundCache[laserSound]->play();}
  void playSumoKnife() {soundCache[knifeSound]->play();}
  void playBombExplode() {soundCache[bombSound]->play();}
  void playGong() {soundCache[gongSound]->play();}
  void playFlagGet() {soundCache[flagSound]->play();}
  void playSlopeSwitch() {soundCache[flagSound]->play();}
  void playHitCivilian() {soundCache[civilianSound]->play();}
  void playWreckThrow() {enemyThrow->play();}
  void playWreckPowerup() {soundCache[wreckPowerSound]->play();}
  void playWreckUFOAppear() {soundCache[ufoAppearSound]->play();}
  void playWreckUFOShock() {soundCache[ufoBlastSound]->play();}
  void playPlaneFire() {RM::random(ResourceManager::bulletSet)->play();}
  void playPlaneBulletHit() {soundCache[bulletHit]->play();}
  void playNagaBreath() {soundCache[roboFlameSound]->play();}

  void playSpinnerAppear() {soundCache[spinnerSound]->play();}
  void playEelShock() {soundCache[eelSound]->play();}
  void playTimingTick() {soundCache[timingTickSound]->play();}
  void playTimingHit(Coord1 index) {timingResultSoundSet[index]->play();}
  void playTimingStep() {RM::random(timingStepSet)->play();}
  void playTimingGameStart() {soundCache[timingGameStartSound]->play();}
  void playTimingWin() {soundCache[timingWinSound]->play();}

  void playWheelClick() {soundCache[wheelClickSound]->play();}
  void playWheelStop() {soundCache[wheelStopSound]->play();}
  void playWheelGetSmall() {soundCache[wheelWinSmall]->play();}
  void playWheelGetBig() {soundCache[wheelWinBig]->play();}
  void playWheelMiss() {soundCache[wheelMissSound]->play();}
  void playWinStar() {soundCache[starGotSound]->play();}
  void playWinNotification() {soundCache[messageSound]->play();}
  void playStarMiss() {soundCache[starMissSound]->play();}
  void playGoalSlide() {soundCache[goalSlideSound]->play();}
  void playBossTaunt() {soundCache[bossTaunt]->play();}
  void playBossSink() {soundCache[bossSink]->play();}
  void playFlashSound() {soundCache[cameraSound]->play();}
  void playBounceSound() {bounceSound->play();}
}


// ============================== PizzaWarper =========================== //

PizzaWarper::PizzaWarper() :
  parent(NULL),
  sequence(NULL),
  contWarpFrame(),
  lastWarpFrame()
{

}

PizzaWarper::PizzaWarper(VisRectangular* setParent, DWarpSequence* setSequence) :
  parent(setParent),
  sequence(setSequence),
  contWarpFrame(),
  lastWarpFrame(sequence->warpFrames[0])
{
  last_to_cont();
}

void PizzaWarper::set(VisRectangular* setParent, DWarpSequence* setSequence)
{
  parent = setParent;
  sequence = setSequence;
  lastWarpFrame = sequence->warpFrames[0];
  last_to_cont();
}

void PizzaWarper::last_to_cont()
{
  contWarpFrame = lastWarpFrame;
  contWarpFrame.visible = true;
}

void PizzaWarper::draw(Point1 fullWeight)
{
  Coord1 currFrame = calcFrame(fullWeight);
  Point1 subWeight = frameWeight(fullWeight);

  draw_lerp(sequence->warpFrames[currFrame], sequence->warpFrames[currFrame + 1], subWeight);
}

void PizzaWarper::draw(Coord1 startFrame, Coord1 tgtFrame, Point1 weight)
{
  if (startFrame < 0 || tgtFrame < 0 || startFrame > sequence->num_frames() ||
      tgtFrame > sequence->num_frames())
  {
    cout << "PizzaWarper::draw(Coord1, Coord1, Point1) failed, frames invalid" << endl;
    return;
  }

  draw_lerp(sequence->warpFrames[startFrame], sequence->warpFrames[tgtFrame], weight);
}

void PizzaWarper::draw_lerp(const DWarpFrame& startFrame,
                            const DWarpFrame& tgtFrame,
                            Point1 weight)
{
  DWarpFrame lerpFrame;
  lerp(startFrame, tgtFrame, weight, lerpFrame);
  draw(lerpFrame);
}

void PizzaWarper::draw(const DWarpFrame& frame)
{
  Image* img = parent->getImage();

  RMGraphics->render_warp(img, sequence->baseMesh, frame,
                          parent->getXY(),
                          parent->getDrawScale() * img->natural_size(),
                          parent->getRotation(),
                          img->handle / img->natural_size(),
                          parent->getColor());

  lastWarpFrame = frame;
}

void PizzaWarper::draw_cont(Coord1 tgtFrame, Point1 rangeWeight)
{
  draw_lerp(contWarpFrame, sequence->warpFrames[tgtFrame], rangeWeight);
}

Coord1 PizzaWarper::numFrames()
{
  return sequence->num_frames();
}

Coord1 PizzaWarper::calcFrame(Point1 weight)
{
  Coord1 index = RM::segment_distance(0.0, weight, 1.0 / numFrames());
  RM::clamp_me(index, 0, numFrames() - 1);
  return index;
}

Point2 PizzaWarper::frameRange(Coord1 frameID)
{
  Point1 frameWidth = 1.0 / numFrames();
  return Point2(frameWidth * frameID, frameWidth * (frameID + 1));
}

Point1 PizzaWarper::frameWeight(Point1 sequenceWeight)
{
  Coord1 currFrame = calcFrame(sequenceWeight);
  return RM::clamp(RM::lerp_reverse(sequenceWeight, frameRange(currFrame)), 0.0, 1.0);
}


