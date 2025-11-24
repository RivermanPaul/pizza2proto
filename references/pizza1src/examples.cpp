/*
 *  examples.cpp
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 9/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "examples.h"
#include "resourceman.h"

using std::cout;
using std::endl;

// =================================== ExampleParticleBurst ============================== //

DataList<Image*> ExampleParticleBurst::particleImgs(0);

ExampleParticleBurst::ExampleParticleBurst(Point2 setXY) :
  ParticleEffect()
{
  // 1. Load the texture and the images
  
  // Supposedly this leaks but I think it's a false positive because it's a static and only runs once.
  // Normally this would be done in ResourceManager
  if (ExampleParticleBurst::particleImgs.count == 0)
  {
    Texture* particleTex = Texture::create("twist_sparks.png");
    Image::create_tiles(particleTex, ExampleParticleBurst::particleImgs, 4, 2);
    for (Coord1 i = 0; i < ExampleParticleBurst::particleImgs.count; ++i)
    {
      ExampleParticleBurst::particleImgs[i]->overexpose();
      ExampleParticleBurst::particleImgs[i]->handle_center();
    }
  } 
  
  // 2. Create burst
  
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(15, 35), Point2(2.0, 3.0), setXY);
  burst->images.append(particleImgs);
  
  burst->speedBounds.set(0.0, 150.0);
  burst->angleBounds.set(PI * 1.15, PI * 1.85);
  
  burst->constantRotate = true;
  // burst->ballisticRotate = true;
  burst->rotateSpeedBounds.set(HALF_PI, TWO_PI);
  
  burst->gravity.set(0.0, 40.0);  
  
  // burst->alphaOscillate = true;
  // burst->alphaOscillateBounds.set(0.25, 0.5);
  
  burst->scaleBounds.set(0.75, 2.0);
  
  // 3. Add burst to emitter list
  
  emitters.add(burst);
}

// =================================== ExampleTriangleShatter ============================== //

ExampleTriangleShatter::ExampleTriangleShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  manageTime(NULL),
  timePtr(NULL),
  ParticleEffect()
{
  // fast version
  ProceduralBurst* burst = new ProceduralBurst(visRect, rows, cols, Point2(1.0, 2.0));
  manageTime = &burst->manageTime;
  timePtr = &burst->currTime;

  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(0.0, 150.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-TWO_PI, TWO_PI);
  
  burst->gravity.set(0.0, 40.0);  
  burst->scaleBounds.set(0.75, 2.0);
  
  emitters.add(burst);
  
  // slow version
  /*
  ProceduralBurst* burst = new ProceduralBurst(visRect, rows, cols, Point2(5.0, 7.0));
  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(0.0, 15.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-0.1, 0.1);
  
  burst->scaleBounds.set(0.75, 2.0);
  
  emitters.add(burst);
   */
}

// =================================== ExampleScrollingBackground ============================== //

Image* ExampleScrollingBackground::bgTile = NULL;

ExampleScrollingBackground::ExampleScrollingBackground() :
  StdBackground(),
  scrolling()
{
  // Normally this would be done in ResourceManager
  if (ExampleScrollingBackground::bgTile == NULL)
  {
    ExampleScrollingBackground::bgTile = new Image("twist_bg.png");
  }
  
  // Depending on where the images are loaded, this could be called during init or the load phase
  load();
}

void ExampleScrollingBackground::load()
{
  // set(Image*, startVeloctity, startPosition, scrollingArea)
  scrolling.init(ExampleScrollingBackground::bgTile, Point2(-45.0, 0.0), Point2(0.0, 0.0), SCREEN_WH);
}

void ExampleScrollingBackground::update()
{
  scrolling.update();
}

void ExampleScrollingBackground::drawBackground()
{
  scrolling.redraw();
}

// =================================== ExampleAnimator ============================== //

DataList<Image*> ExampleAnimator::animImgs;

ExampleAnimator::ExampleAnimator(Logical setrepeat) :
  VisRectangular(0.0, 0.0, 64.0, 64.0, NULL),
  ActionEvent(NULL),
  ActionListener(),
  animator()
{
  // Normally this would be done in ResourceManager
  if (ExampleAnimator::animImgs.count == 0)
  {
    Texture* tex = Texture::create("twist_sparks.png");
    Image::create_tiles(tex, ExampleAnimator::animImgs, 4, 2);
    for (Coord1 i = 0; i < ExampleAnimator::animImgs.count; ++i)
    {
      ExampleAnimator::animImgs[i]->overexpose();
    }
  }
  
  animator.init(this, ExampleAnimator::animImgs, 0.2, this);
  animator.repeats();
}

void ExampleAnimator::updateMe()
{
  animator.update();
}

void ExampleAnimator::redrawMe()
{
  drawMe();
}

void ExampleAnimator::callback(ActionEvent* caller)
{
  if (animator.repeating == false) done();
  else (eventCallback());
}

// =================================== ExampleB2DStruct ============================== //

ExampleB2DStruct::ExampleB2DStruct(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  PhysicalTriangleParticles()
{
  create(visRect, cols, rows);
}

void ExampleB2DStruct::updateMe()
{

}

void ExampleB2DStruct::redrawMe()
{
  drawParticles(Point2(0.0, 0.0));
}

// =================================== ExampleListener ============================== //

// this is an example post solve function to trigger a collision callback with impulse
void ExampleB2DListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
  void* userData1 = contact->GetFixtureA()->GetUserData();
  void* userData2 = contact->GetFixtureB()->GetUserData();
  
  if (userData1 == NULL || userData2 == NULL) 
  {
    // cout << "NULL USER DATA" << endl;
    return;
  }
  
//  WreckGO* wgo1 = static_cast<WreckGO*>(userData1);
//  WreckGO* wgo2 = static_cast<WreckGO*>(userData2);
  
  Point2 worldImpulse = impulse_to_wpt(impulse);
  Point1 finalImpulse = std::max(worldImpulse.x, worldImpulse.y);
  
  if (finalImpulse > 10.0) 
  {
//    wgo1->collidedPhysical(wgo2, finalImpulse);
//    wgo2->collidedPhysical(wgo1, finalImpulse);
  }
}

// =============================== ScoreManager ============================ //

ExampleScoreManager::ExampleScoreManager() :
  SystemGameCenterListener(),

  saveFilename("PS012.sav"),
  repostSeconds(60.0),

  topScores(MAX_SCORES),
  scoresPosted(0xffffffff),

  achievements(MAX_ACHIEVEMENTS),
  achPosted(0xffffffff),

  needsSave(false),
  currTime(0.0)
{
  for (Coord1 i = 0; i < MAX_SCORES; ++i) topScores.add(0);
  for (Coord1 i = 0; i < MAX_ACHIEVEMENTS; ++i) achievements.add(0.0);
}

void ExampleScoreManager::update()
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

Coord1 ExampleScoreManager::topScore()
{
  return topScores[0];
}

void ExampleScoreManager::checkAndRepost()
{
  for (Coord1 i = 0; i < MAX_SCORES; ++i)
  {
    if ((scoresPosted & (0x1 << i)) == 0) 
    {
      RMSystem->gamecenter_post_int(i + leaderboardOffset, topScores[i]);
    }
  }
  for (Coord1 i = 0; i < MAX_ACHIEVEMENTS; ++i)
  {
    if ((achPosted & (0x1 << i)) == 0) 
    {
      RMSystem->gamecenter_achievement(i + achievementOffset, achievements[i], false);      
    }
  }
}

Logical ExampleScoreManager::earnedScore(Coord1 localID, Coord1 score)
{
  Coord1 globalID = localID + leaderboardOffset;
  
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

Logical ExampleScoreManager::earnedAchievement(Coord1 localID, Point1 percent)
{
  Logical showBanner = false;
  Coord1 globalID = localID + achievementOffset;
  
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

void ExampleScoreManager::save()
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

void ExampleScoreManager::load()
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

void ExampleScoreManager::score_posted(Coord1 boardID, Coord1 score)
{
  Coord1 localizedID = boardID - leaderboardOffset;
  
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

void ExampleScoreManager::achievement_posted(Coord1 achID, Point1 percent)
{
  Coord1 localizedID = achID - achievementOffset;
  
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

