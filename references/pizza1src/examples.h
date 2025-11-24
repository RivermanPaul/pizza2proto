/*
 *  examples.h
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 9/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef EXAMPLES_H
#define EXAMPLES_H

#include "rmparticle.h"
#include "rmbackground.h"
#include "rmvisible2d.h"
#include "rmbehaviors.h"

// To use ANY EXAMPLES:
// 1. Make sure the example images, data and sounds have not been removed from the project
// 2. #include examples.h

// NOTES:
// -Most examples use static variables to store images so that it's only loaded once
// even if the class is instantiated multiple times. This means that a ~ is not generally
// necessary, but the shared images/resources would need to be freed in normal uses.
// -Do not instantiate these as global or static variables because the file system won't be
// set up yet to load the textures.

// 1. Instantiate as a class member (or local)
// 2. update() and redraw() or add to a list/script
struct ExampleParticleBurst : ParticleEffect
{
  static DataList<Image*> particleImgs;  // this is not normally needed
  
  ExampleParticleBurst(Point2 setXY);
};

// 1. Create a VisRectaganular, set its xy, scale, rotation, AND image
//    (Start drawing visRect)
// 2. Instantiate ExampleTriangleShatter using visRect
//    (Stop drawing visRect)
// 3. update() and redraw() shatter effect or add to list/script 
struct ExampleTriangleShatter : ParticleEffect
{
  Logical* manageTime; // set to true to manage time manually for special usages (e.g., rewind)
  Point1* timePtr; // give this to a robot function to manage the time
  
  ExampleTriangleShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
};

// 1. Instantiate as a class member
// 2. update() and (during redraw) drawBackground()
struct ExampleScrollingBackground : StdBackground
{
  ScrollingImage scrolling;
  static Image* bgTile;
  
  ExampleScrollingBackground();
  virtual void load();
  virtual void update();
  virtual void drawBackground();
};

// 1. Instantiate as a class member (or local)
// 2. update() and redraw() or add to a list/script
struct ExampleAnimator : VisRectangular, ActionEvent, ActionListener
{
  AnimationBehavior animator;
  static DataList<Image*> animImgs;
  
  ExampleAnimator(Logical setrepeat = false);
  void updateMe();
  void redrawMe();
  void callback(ActionEvent* caller);
};

struct ExampleB2DStruct : PhysicalTriangleParticles, ActionEvent
{
  ExampleB2DStruct(const VisRectangular& visRect, Coord1 cols, Coord1 rows);
  void updateMe();
  void redrawMe();
};

struct ExampleB2DListener : b2ContactListener
{
  void BeginContact(b2Contact* contact) {}
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {}
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
  void EndContact(b2Contact* contact) {}
};

struct ExampleScoreManager : SystemGameCenterListener
{  
  // settings
  String1 saveFilename;
  Point1 repostSeconds;
  
  Coord1 achievementOffset;  // SET THIS
  Coord1 leaderboardOffset;  // SET THIS
  
  ArrayList<Coord1> topScores;
  Coord1 scoresPosted;
  
  ArrayList<Point1> achievements;
  Coord1 achPosted;
  
  Logical needsSave;  // indicates that a score or achievement has been earned since the last save
  Point1 currTime;
  
  static const Logical DEBUG_REPORT = false;
  static const Coord1 MAX_SCORES = 4;
  static const Coord1 MAX_ACHIEVEMENTS = 30;
  
  ExampleScoreManager();
  
  void load();
  void save();
  
  void update();
  
  void checkAndRepost();
  Coord1 topScore();
  
  // this is boardID, not the localized version
  // returns true if it's the new high score.
  Logical earnedScore(Coord1 localID, Coord1 score);
  
  // returns true if the achievement was just finished for the first time,
  //   according to local cached data
  Logical earnedAchievement(Coord1 localID, Point1 percent = 100.0);
  
  // ========== from SystemGameCenterListener
  void score_posted(Coord1 boardID, Coord1 score);
  void achievement_posted(Coord1 achID, Point1 percent);  
};

#endif