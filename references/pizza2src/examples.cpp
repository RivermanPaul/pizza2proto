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
#include "rmeffect.h"

using std::cout;
using std::endl;

// =================================== ExampleParticleBurst ============================== //

DataList<Image*> ExampleParticleBurst::particleImgs(0);

ExampleParticleBurst::ExampleParticleBurst(Point2 setXY) :
  ParticleEffect(),
  xy(setXY)
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
    std::cout << "ran" << std::endl;
  } 
  
  // 2. Create burst
  
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(15, 35), Point2(2.0, 3.0), xy);
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

// =================================== ExampleMeshShatter ============================== //

ExampleMeshShatter::ExampleMeshShatter(const VisRectangular& visRect, DWarpMesh* mesh) :
  ParticleEffect()
{
  // fast version
  ProceduralBurst* burst = new ProceduralBurst(0, Point2(1.0, 2.0), visRect.getXY());
  burst->triSet.create(visRect, mesh);
  burst->set_num_particles(burst->triSet.triangles.count);
  
  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(0.0, 15.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-0.1, 0.1);
  
  burst->scaleBounds.set(0.75, 2.0);
  
  emitters.add(burst);
}

// =================================== createRandWinBanner ============================== //

ActionEvent* createRandWinBanner()
{
  Coord1 chooser = RM::randi(0, 2);
  
  // set the images to something real
  if (chooser == 0)
  {
    // return new BannerSpinEffect(ResourceManager::winBanner, SCREEN_CENTER);
  }
  else if (chooser == 1)
  {
    // return new BannerSwoopEffect(ResourceManager::winBanner, Point2(SCREEN_CX, 0.0), SCREEN_CY);
  }
  else
  {
    // return new BannerFlipEffect(ResourceManager::winBanner, SCREEN_CY, SCREEN_CX);    
  }
  
  return NULL;
}

// =============================== CrossSellButton ============================ //

ExampleCrossSellButton::ExampleCrossSellButton(const DataList<Image*>& setImgs) : 
  Button(),
  ActionListener(),

  imgs(setImgs),

  topImgAlpha(1.0),
  topImgIndex(1),

  holdTimer(3.0, this),
  fader(&topImgAlpha, 1.0, 0.5, this)
{
  setImage(imgs[0]);
  autoSize();
}

void ExampleCrossSellButton::init(Point2 topLeft, RivermanGUI* setParent, ButtonPresser* setPresser)
{
  setXY(topLeft);
  parentGUI = setParent;
  presser = setPresser;
}

void ExampleCrossSellButton::update()
{
  if (isEnabled() == false) return;
  
  collisionUpdate();
  
  holdTimer.update();
  fader.update();
}

void ExampleCrossSellButton::redraw()
{
  if (isEnabled() == false) return;
  
  Point1 masterAlpha = getAlpha();
  
  setAlpha(masterAlpha);
  setImage(imgs[0]);
  drawMe();
  
  setAlpha(masterAlpha * calcBotImgAlpha());
  setImage(imgs[calcBotImgIndex()]);
  drawMe();
  
  setAlpha(masterAlpha * topImgAlpha);
  setImage(imgs[topImgIndex]);
  drawMe();
  
  if (hoverPress() == true)
  {
    setAlpha(masterAlpha);
    setImage(imgs.last());
    drawMe();
  }
  
  setAlpha(masterAlpha);
}

Coord1 ExampleCrossSellButton::calcBotImgIndex()
{
  Coord1 result = topImgIndex - 1;
  if (result == 0) result = imgs.last_i() - 1;
  
  return result;
}

Point1 ExampleCrossSellButton::calcBotImgAlpha()
{
  return 1.0 - topImgAlpha;
}

void ExampleCrossSellButton::callback(ActionEvent* caller)
{
  if (caller == &holdTimer)
  {
    topImgIndex++;
    if (topImgIndex == imgs.last_i()) topImgIndex = 1;
    
    topImgAlpha = 0.0;
    fader.reset();
  }
  else if (caller == &fader)
  {
    holdTimer.reset();
  }
}
