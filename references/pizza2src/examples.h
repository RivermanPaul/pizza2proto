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
#include "rmgui.h"

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
  Point2 xy;
  static DataList<Image*> particleImgs;
  
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

struct ExampleMeshShatter : ParticleEffect
{
  ExampleMeshShatter(const VisRectangular& visRect, DWarpMesh* mesh);
};

ActionEvent* createRandWinBanner();

struct ExampleCrossSellButton : Button, ActionListener
{
  const DataList<Image*>& imgs;
  
  Point1 topImgAlpha;
  Coord1 topImgIndex;
  
  TimerFn holdTimer;
  LinearFn fader;
  
  ExampleCrossSellButton(const DataList<Image*>& setImgs);
  void init(Point2 topLeft, RivermanGUI* setParent, ButtonPresser* setPresser);
  void update();
  void redraw();
  
  Coord1 calcBotImgIndex();
  Point1 calcBotImgAlpha();
  
  void callback(ActionEvent* caller);
};

#endif