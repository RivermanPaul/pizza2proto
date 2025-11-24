/*
 *  pgo.cpp
 *  NewRMIPhone
 *
 *  Created by Paul Stevens on 11/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "pgo.h"
#include "rmactions.h"
#include "resourceman.h"
#include "rmglobals.h"
#include "mainapp.h"
#include "rmstdinput.h"
#include "rmeffect.h"

using std::cout;
using std::endl;

// =============================== PizzaGO ============================ //

PizzaGO::PizzaGO() :
  PhysRectangular(),
  BaseGOLogic(),

  level(NULL),
  player(NULL),
  shadowImg(NULL),

  type(-1),
  value(-1),
  basePoints(0),
 
  sortY(0.0),
  crushValue(999.0),

  hitThisAttack(false),
  flies(false),
  crushesPhysical(false),
  destroyedPhysical(false)
{
  setActive();
}

void PizzaGO::updateMe()
{
  
}

void PizzaGO::redrawMe()
{
  if (getImage() != NULL) drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    // collisionCircle().draw_outline(WHITE_SOLID);
  }
}

Logical PizzaGO::touchingPGO(PizzaGO* targetPGO, Point2* location)
{
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    if (edge->contact->IsTouching() == true && edge->contact->IsEnabled() == true) 
    {
      PizzaGO* pgo1 = static_cast<PizzaGO*>(edge->contact->GetFixtureA()->GetUserData());
      PizzaGO* pgo2 = static_cast<PizzaGO*>(edge->contact->GetFixtureB()->GetUserData());
      
      if (pgo1 == targetPGO || pgo2 == targetPGO) 
      {
        if (location != NULL) (*location) = edge_to_wpt(edge);
        return true;
      }
    }
  }
  
  return false;
}

Logical PizzaGO::standingPhysical(Point2* contactPt)
{
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    Point2 pt0 = edge_to_wpt(edge);
    Point1 contactAngle = RM::angle(getXY(), pt0);
    
    b2Fixture* fixtureA = edge->contact->GetFixtureA();
    b2Fixture* fixtureB = edge->contact->GetFixtureB();
    
    // problem: sumo boss won't jump off player unless we add player,
    // but can't do that without check that player can't use itself as terrain
    // actually this works b/c boss CAN use itself as terrain, player can't
    Coord1 standableBits = (1 << TYPE_TERRAIN) | 
        (1 << TYPE_DEBRIS) | (1 << TYPE_SUMO_BOSS);
    
    Logical leftIsTerrain =
        (fixtureA->GetFilterData().categoryBits & standableBits) != 0;
    Logical rightIsTerrain =
        (fixtureB->GetFilterData().categoryBits & standableBits) != 0;
    Logical eitherTerrain = leftIsTerrain || rightIsTerrain;
        
    if (eitherTerrain == true &&
        edge->contact->IsTouching() == true && 
        edge->contact->IsEnabled() == true &&
        contactAngle > PI * 0.1 && contactAngle < PI * 0.9) 
    {
      if (contactPt != NULL) *contactPt = pt0;
      return true;
    }
  }
  
  return false;
}

Logical PizzaGO::shouldDrawArrowOverhead()
{
  Box camBox = level->camera.myBox();
  Box imgBox = imageBox();
  Box worldArea = Box(level->worldBox);
  worldArea.grow_up(PLAY_H * 2.0);
  
  Logical onscreen = imgBox.collision(camBox);
  Logical inWorld = imgBox.collision(worldArea);
  Logical living = lifeState == RM::ACTIVE;
  
  return !onscreen && inWorld && living;
}

Logical PizzaGO::shouldDrawArrowSides()
{
  Box camBox = level->camera.myBox();
  Box imgBox = imageBox();
  Box worldArea = Box(level->worldBox);
  worldArea.grow_left(PLAY_W);
  worldArea.grow_right(PLAY_W);
  
  Logical onscreen = imgBox.collision(camBox);
  Logical living = getActive();
  
  return !onscreen && living;
}

Logical PizzaGO::shouldTurn()
{
  if (collisionCircle().left() < level->worldBox.left() && getHFlip() == true) return true;
  if (collisionCircle().right() > level->worldBox.right() && getHFlip() == false) return true;
  return false;
}

Logical PizzaGO::turnAtWalls()
{
  Logical turned = shouldTurn();
  if (turned == true) toggleHFlip();
  return turned;
}

void PizzaGO::addSortDraw()
{
  level->addSortDrawer(this);
}

Logical PizzaGO::playerCollision()
{  
  return collisionCircle().collision(player->collisionCircle());
}

Logical PizzaGO::playerCollision(Circle c)
{
  return c.collision(player->collisionCircle());
}

Logical PizzaGO::onScreen()
{
  return getImage() != NULL && Box::collision(imageBox(), level->camera.myBox());
}

Logical PizzaGO::onScreenFixed()
{
  return getImage() != NULL && Box::collision(imageBox(), level->camBox());
}

Logical PizzaGO::onScreenAABB()
{
  Point1 bigDimension = std::max(getWidth(), getHeight());
  Box bigBox(getX() - bigDimension, getY() - bigDimension, bigDimension * 2.0, bigDimension * 2.0);

  CameraStd cam = level->camera;
  cam.xy += Pizza::platformTL;
  cam.size = SCREEN_WH;
  
  return getImage() != NULL && Box::collision(bigBox, cam.myBox());
}

void PizzaGO::facePlayer()
{
  hflip(getX() > player->getX());
}

void PizzaGO::faceDir(Point1 dir)
{
  hflip(dir < 0.0);
}

Coord1 PizzaGO::facingToDir()
{
  return VisRectangular::facingToDir();
}

void PizzaGO::drawSorted()
{
  drawMe();
}

void PizzaGO::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  // the level play check is for objects responding to forces right when the
  // level starts
  if (crushesPhysical && 
      normalImpulse >= crushValue &&
      level->levelState == PizzaLevel::LEVEL_PLAY) 
  {
    // cout << normalImpulse << endl;
    destroyedPhysical = true;
  }
}

void PizzaGO::checkPhysicalDestruction()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (destroyedPhysical == true)
  {
    destroyPhysical();
  }
}

void PizzaGO::destroyPhysical()
{
  destroyPhysicalStd(Coord2(2, 3), getImage());
}

void PizzaGO::breakStd(Coord2 breakRange, Image* breakImg, Logical withSound)
{
  if (withSound == true) ResourceManager::playCrunch();
  
  Coord2 breakGrid(RM::randi(breakRange), RM::randi(breakRange));
  setImage(breakImg);
  
  BoneBreakEffect* breaker = new BoneBreakEffect(*this, breakGrid.x, breakGrid.y);
  // breaker->setVelFromPt(collisionCircle().xy, 200.0);
  breaker->setVelFromPtPos(collisionCircle().xy, 300.0);
  level->addDebris(breaker);
}

void PizzaGO::destroyPhysicalStd(Coord2 breakRange, Image* breakImg)
{
  if (body != NULL) RMPhysics->DestroyBody(body);
  body = NULL;  // avoids double delete

  breakStd(breakRange, breakImg);  
  lifeState = RM::REMOVE;
}

void PizzaGO::smokePuffStd()
{
  level->addAction(new Smokeburst(collisionCircle().xy, false));
}

Box PizzaGO::collisionBoxWalker()
{
  return Box(xy.x - wh.x, xy.y - wh.y * 2.0, wh.x * 2.0, wh.y * 2.0);
}

Circle PizzaGO::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

Point1 PizzaGO::sortVal() const
{
  return sortY;
}

void PizzaGO::createMagnetCoin()
{
  createMagnetCoin(collisionCircle().xy, 0);
}

void PizzaGO::createMagnetCoin(Coord1 coinVal)
{
  createMagnetCoin(collisionCircle().xy, coinVal);
}

void PizzaGO::createMagnetCoin(Point2 origin, Coord1 coinVal)
{
  if (coinVal < 0) return;
  
  level->addAlly(new MagnetCoin(origin, coinVal));
}

Logical operator<(const SortDrawer& pgo1, const SortDrawer& pgo2)
{
  return pgo1.sortVal() < pgo2.sortVal();
}

Logical operator>(const SortDrawer& pgo1, const SortDrawer& pgo2)
{
  return pgo1.sortVal() > pgo2.sortVal();  
}

// =============================== PizzaFace ============================ //

PizzaFace::PizzaFace(VisRectangular* setParent) :
  ActionListener(),

  parent(setParent),
  saucePercent(0.0),
  glowAlpha(0.0),

  faceWarper(parent, dwarpCache[eyeWarp]),
  mouthWarper(parent, dwarpCache[mouthWarp]),
  faceScript(this),

  faceState(IDLE),
  idleState(BREATHE),
  frameTarget(0),
  targetHFlip(false),
  currHFlip(false),

  currWeight(0.0),
  breatheData(0.0, 1.0),

  clothesWarper(parent, dwarpCache[clothesWarp]),
  clothesWeight(0.0),
  clothesScript(),

  useOverride(false),
  overrideDuration(0.2)
{
  resetFace();
  idleTrigger(BREATHE);
}

void PizzaFace::resetFace()
{
  faceWarper.set(parent, dwarpCache[eyeWarp]);
  mouthWarper.set(parent, dwarpCache[mouthWarp]);
  clothesWarper.set(parent, dwarpCache[clothesWarp]);
  
  faceWarper.lastWarpFrame = faceWarper.sequence->warpFrames[1];
  mouthWarper.lastWarpFrame = mouthWarper.sequence->warpFrames[1];
  
  faceWarper.contWarpFrame = faceWarper.sequence->warpFrames[1];
  mouthWarper.contWarpFrame = mouthWarper.sequence->warpFrames[1];
}

void PizzaFace::updateFace()
{
  faceScript.update();
  clothesScript.update();
  saucePercent = RM::flatten(saucePercent, damagePercent(), 0.25 * RM::timePassed());  
}

void PizzaFace::drawFace()
{
  Logical parentHFlip = parent->getHFlip();

  // glow
  if (glowAlpha > 0.01)
  {
    parent->setImage(imgCache[pizzaGlow]);
    parent->scale *= 2.0;
    Point1 trueAlpha = parent->getAlpha();
    parent->setAlpha(glowAlpha);

    parent->drawMe();
    
    parent->setAlpha(trueAlpha);
    parent->scale /= 2.0;
  }
  
  // base/damage
  Coord1 numSauces = imgsetCache[pizzaSauceSet].count;
  Point1 singleSaucePercent = 1.0 / numSauces;
  
  Coord1 botSauceIndex = (Coord1) (saucePercent / singleSaucePercent) - 1;
  botSauceIndex = RM::clamp(botSauceIndex, -1, numSauces - 2);
  Coord1 topSauceIndex = botSauceIndex + 1;
  
  if (botSauceIndex < 0)
  {
    parent->setImage(imgCache[pizzaBase]);
    parent->drawMe();
  }
  else
  {
    parent->setImage(imgsetCache[pizzaSauceSet][botSauceIndex]);
    parent->drawMe();
  }
  
  // damage overlay
  if (imgsetCache[pizzaSauceSet].count >= 1)
  {
    // this makes it faster and guards against how sauce isn't loaded after changing
    // ingredients in the shop
    Point1 parentAlpha = parent->getAlpha();
    Point1 topSauceAlpha = (saucePercent - (topSauceIndex * singleSaucePercent)) / singleSaucePercent;

    parent->setAlpha(topSauceAlpha);
    parent->setImage(imgsetCache[pizzaSauceSet][topSauceIndex]);
    if (parent->getAlpha() > 0.01) parent->drawMe();    // the alpha check is for the shop
    parent->setAlpha(parentAlpha);
  }

  // cout << "damage " << damagePercent() << " saucePercent " << saucePercent << " Is " <<
  //     Coord2(botSauceIndex, topSauceIndex) << " %s " << topSauceAlpha << endl;
  
  // toppings
  for (Coord1 i = 0; i < imgsetCache[pizzaToppingSet].count; ++i)
  {
    parent->setImage(imgsetCache[pizzaToppingSet][i]);
    parent->drawMe();
  }
  
  // prep unrotated
  Point1 myRotation = parent->getRotation();
  parent->setRotation(0.0);
  parent->hflip(currHFlip);
  
  // eyes
  parent->setImage(imgCache[pizzaEyes]);
  faceWarper.draw_cont(frameTarget, currWeight);
  
  // mouth
  parent->setImage(imgCache[pizzaMouth]);
  mouthWarper.draw_cont(frameTarget, currWeight);
  
  // clothes
  parent->setImage(imgCache[pizzaClothes]);
  clothesWarper.draw(clothesWeight);
  
  parent->setRotation(myRotation);
  parent->hflip(parentHFlip);
}

Point1 PizzaFace::damagePercent()
{
  return std::min(Pizza::currGameVars[VAR_DAMAGE] / 8.0, 1.0);
}

Logical PizzaFace::isHurt()
{
  return faceState == HURT;
}

void PizzaFace::actionTrigger(Coord1 actionType, Logical ignorePriority)
{
  if (faceState >= actionType && ignorePriority == false) return;

  faceScript.clear();
  faceWarper.last_to_cont();
  mouthWarper.last_to_cont();
  currWeight = 0.0;
  faceState = actionType;
  
  switch (faceState)
  {      
    case SLAM:
      frameTarget = 5;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(10.0);
      break;
    case BUMP:
      frameTarget = 4;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.15));
      break;
    case HURT:
      frameTarget = 6;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(0.4);
      break;
    case ROAR_1:
      frameTarget = 7;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, 2.0));
      break;
    case ROAR_2:
      frameTarget = 8;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, 2.0));
      break;
  }
  
  useOverride = false;
}

void PizzaFace::idleTrigger(Coord1 idleType)
{
  faceScript.clear();
  faceWarper.last_to_cont();
  mouthWarper.last_to_cont();
  currWeight = 0.0;
  
  if (idleType == BLINK && RM::randi(0, 5) != 0) idleType = BREATHE;
  idleState = idleType;
  faceState = IDLE;
  
  switch (idleState)
  {
    case BREATHE:
      frameTarget = frameTarget != 1 ? 1 : 2;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.5));
      break;
    case BLINK:
      frameTarget = 3;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.enqueueX(new LinearFn(&currWeight, 0.0, 0.2));
      break;
    case TURN:
      frameTarget = 9;
      overrideDuration = 0.2;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.1));
      faceScript.enqueueX(new SetValueCommand<Logical>(&currHFlip, targetHFlip));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&frameTarget, 0));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&idleState, BLINK));
      faceScript.enqueueX(new SetValueCommand<Logical>(&useOverride, true));
      
      clothesScript.enqueueX(new LinearFn(&clothesWeight, 1.0, 0.1));
      clothesScript.enqueueX(new LinearFn(&clothesWeight, 0.0, 0.1));
      break;
  }  

  useOverride = false;
}

void PizzaFace::callback(ActionEvent* caller)
{
  if (caller == &faceScript)
  {
    if (faceState == ROAR_1)
    {
      actionTrigger(ROAR_2);
    }
    else if (faceState == ROAR_2)
    {
      // do nothing, hold it
    }
    else if (targetHFlip == currHFlip)
    {
      // cout << "idle" << endl;
      idleTrigger(BLINK);
    }
    else
    {
      // cout << "turn" << endl;
      idleTrigger(TURN);
    }
  }
}

// =============================== PizzaPlayer ============================ //

PizzaPlayer::PizzaPlayer() :
  PizzaGO(),

  face(this),

  standingTimer(0.2),
  nojumpTimer(0.22),  // must be at least 1/60 greater than standing timer
  isStanding(false),
  isSlamming(false),

  oldXY(0.0, 0.0),
  facing(1),

  slamAlpha(0.0),
  slamRotation(0.0),
  slamYVel(0.0),
  
  starTimer(5.0),
  starMaker(0.5, true, this)
{    
  type = TYPE_PLAYER;
  
  setXY(Point2(100.0, 100.0));
  setWH(Point2(128.0, 128.0)); // width = radius
  oldXY = getXY();
  
  setImage(imgCache[pizzaBase]);
  shadowImg = imgCache[shadowLarge];
  autoSize(0.45);
    
  init_circle(getXY(), getWidth(), true);
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(0.5);
  fixture->SetUserData(this);
  
  starTimer.setInactive();
}

void PizzaPlayer::updateMe()
{
  if (lifeState == RM::PASSIVE) return;

  oldXY = getXY();
  updateFromPhysical();
  updateStanding();
  
  Point1 slamTarget = isSlamming ? 1.0 : 0.0;
  slamAlpha = RM::flatten(slamAlpha, slamTarget, 6.0 * RM::timePassed());
  
  // enforce max velocity
  Point2 vel = get_velocity();
  if (std::abs(vel.x) > level->playerTXVel()) 
  {
    set_velocity((Point2(level->playerTXVel() * RM::sign(vel.x), vel.y)));
  }
  
  // maintain slam velocity
  if (isSlamming == true)
  {
    slamYVel += RMPhysics->get_gravity_rm().y * RM::timePassed();
    set_velocity(Point2(get_velocity().x, slamYVel));
  }
  
  // starTimer.reset();
  if (starActive() == true) starMaker.update();
  
  face.updateFace();
}

void PizzaPlayer::redrawMe()
{
  // if (lifeState == RM::PASSIVE) return;
  
  face.drawFace();
  drawSlamStreak();
  
  if (Pizza::DEBUG_COLLISION == true)  
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void PizzaPlayer::drawSlamStreak()
{
  // draw Slam
  Point1 myRotation = getRotation();
  Point1 myAlpha = getAlpha();
  Point2 myScale = getScale();
  setImage(imgCache[slamStreak]);
  setRotation(slamRotation);
  setScale(2.0, 2.0);
  setAlpha(slamAlpha);
  drawMe();
  setRotation(myRotation);
  setScale(myScale);
  setAlpha(myAlpha);
}

void PizzaPlayer::updateStanding()
{
  standingTimer.update();
  nojumpTimer.update();
  
  Logical oldStanding = isStanding;
  
  if (standingPhysical(&standingXY) == true)
  {
    // standing
    isStanding = true;
    standingTimer.reset();
  }
  else
  {
    isStanding = false;
  }
  
  // cout << "isStanding " << isStanding << " standTimer " <<
  // standingTimer.getActive() << " jumpTimer " << jumpTimer.getActive() << endl;
  
  if (oldStanding == false && canJump() == true)
  {
    // just landed
    if (isSlamming == true)
    {
      StarParticles* leftStars = new StarParticles(standingXY, RM::angle(getXY(), standingXY) - HALF_PI);
      level->addAction(leftStars);      

      StarParticles* rightStars = new StarParticles(standingXY, RM::angle(getXY(), standingXY) + HALF_PI);
      level->addAction(rightStars);    
      
      ResourceManager::playSlamLand();
    }

    level->reportPlayerLanded(isSlamming);
    isSlamming = false;
  }  
}

void PizzaPlayer::setFacing(Coord1 newFacing)
{
  facing = newFacing;
  if (facing == -1) face.targetHFlip = true;
  else face.targetHFlip = false;
}

Coord1 PizzaPlayer::facingToDir()
{
  return face.targetHFlip == false ? 1 : -1;
}

void PizzaPlayer::tryJump()
{
  if (canJump() == true)
  {
    ResourceManager::playJump();
    if ((level->playerMotionFlags & STICK_BIT) != 0)
    {
      level->addAction(new GooSplash(Point2(getX(), collisionCircle().bottom())));
    }
    if (face.faceState == PizzaFace::SLAM)
    {
      face.actionTrigger(PizzaFace::BUMP);  // override slam
    }
    
    bounceUp(level->bounceMult());  // takes sticky into account
  }
  else if (canSlam() == true)
  {
    ResourceManager::playSlamStart();
    slamDown();
  }
}

void PizzaPlayer::tryMove(Point1 normMagnitude)
{
  Point2 centerOfMass = b2dpt_to_world(body->GetWorldCenter());
  
  Point1 armLength = getWidth() * 0.9 * RM::sign(normMagnitude);
  Point1 torqueMult = 1.0;
  if (isStanding == false) torqueMult = 0.0;
  
  // this is to turn faster
  Point1 turnMult = 1.0;  
  if ((normMagnitude < 0.0 && get_velocity().x > 0.0) ||
      (normMagnitude > 0.0 && get_velocity().x < 0.0)) 
  {
    turnMult *= 2.5; 
  }
  
  Point1 finalRollForce = PLAYER_ROLL_FORCE * normMagnitude * turnMult;
  Point1 finalRollTorque = PLAYER_ROLL_TORQUE * torqueMult * std::abs(normMagnitude);
  
  apply_force(Point2(finalRollForce, finalRollTorque), 
              centerOfMass + Point2(armLength * torqueMult, 0.0));
}

void PizzaPlayer::bounceUp(Point1 percentOfJump)
{
  Point2 currVel = get_velocity();
  set_velocity(Point2(currVel.x, PLAYER_JUMP_VEL * percentOfJump));
  
  isStanding = false;
  standingTimer.setActive(false);
  nojumpTimer.reset();
}

void PizzaPlayer::slamDown()
{
  Point2 currVel = get_velocity();
  slamYVel = std::max(-PLAYER_JUMP_VEL * 1.35 * level->bounceMult(), currVel.y * 1.5);
  
  set_velocity(Point2(currVel.x, slamYVel));
  slamRotation = RM::angle(get_velocity());
  isSlamming = true;
  
  nojumpTimer.setInactive();
  
  face.actionTrigger(PizzaFace::SLAM);
}

Logical PizzaPlayer::canJump()
{
  Logical result = 
      level->jumpEnabled() == true &&
      (isStanding == true || standingTimer.getActive() == true) && 
      nojumpTimer.getActive() == false &&
      lifeState == RM::ACTIVE;

  return result;
}

Logical PizzaPlayer::canSlam()
{
  Logical result = 
      isSlamming == false &&
      isStanding == false &&
      level->slamDisabled() == false;
  
  return result;  
}

Logical PizzaPlayer::starActive()
{
  return starTimer.getActive();
}

void PizzaPlayer::attacked(const Point2& atkPoint, PizzaGO* attacker)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return;
  if (level->levelState != PizzaLevel::LEVEL_PLAY) return;
  if (starActive() == true) return;
  if (face.isHurt() == true) return;
  
  level->pizzaDamaged();
  face.actionTrigger(PizzaFace::HURT);
  isSlamming = false;  // cancel the slam

  level->addSauceEffect(*this, atkPoint);  
  damaged(atkPoint, attacker);
}

void PizzaPlayer::damaged(const Point2& atkPoint, PizzaGO* attacker)
{
  Point2 bounceVector = RM::ring_edge(atkPoint, getXY(), 450.0);
  bounceVector.x = RM::clamp(bounceVector.x, -300.0, 300.0);
  set_velocity(bounceVector);
  // set_velocity(get_velocity() + bounceVector);
  // cout << "bounce " << get_velocity().y << endl;
    
  ResourceManager::playDamaged();
  Pizza::currGameVars[VAR_DAMAGE]++;
  // if (Pizza::currGameVars[VAR_DAMAGE] > 8) Pizza::currGameVars[VAR_DAMAGE] = 0.0;
}

void PizzaPlayer::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (normalImpulse >= 1000.0) 
  {
    ResourceManager::playLanding();
    face.actionTrigger(PizzaFace::BUMP);
  }
  else if (normalImpulse > 250.0 && face.faceState == PizzaFace::SLAM)
  {
    // this cancels a slam face on a glancing blow
    face.actionTrigger(PizzaFace::BUMP);
  }
                                             
  
  level->reportPlayerBump(normalImpulse);  
}

void PizzaPlayer::setIntro()
{
  // affects update, draw, and canJump
  setAlpha(0.0);
  setPassive();
  body->SetActive(false);
}

void PizzaPlayer::endIntro()
{
  setActive();
  body->SetActive(true);
  updateFromWorld();
}

Box PizzaPlayer::collisionBox()
{
  return Box(getXY() - getSize(), getSize() * 2.0);
}

void PizzaPlayer::callback(ActionEvent* caller)
{
  if (caller == &starMaker)
  {
    level->addAction(new CoinParticles(getXY()));
  }
}

// =============================== SkeletonSpearman ============================ //

SkeletonSpearman::SkeletonSpearman(Coord1 setValue) :
  PizzaGO(),
  animation(),
  walkSpeed(RM::randf(40.0, 60.0)),
  attackCircle(Point2(0.0, 0.0), 16.0)
{
  value = setValue;
  basePoints = 10;
}

void SkeletonSpearman::load()
{
  facePlayer();
  sortY = getY();

  DataList<Image*>& imgSet = value == VAL_SIDEWAYS ? imgsetCache[skeletonSpearSide] :
      imgsetCache[skeletonSpearVert];
  shadowImg = value == VAL_SIDEWAYS ? imgCache[shadowMedium] :
      imgCache[shadowSmall];

  setWH(value == VAL_SIDEWAYS ? 40.0 : 32.0, 48.0);

  animation.init(this, imgSet, 0.14);
  animation.repeats();
  animation.randomize();
  
  setImage(imgSet[0]);
}

void SkeletonSpearman::updateMe()
{
  animation.update();
  addX(walkSpeed * facingToDir() * RM::timePassed());
  turnAtWalls();
  updateCollisions();
}

void SkeletonSpearman::redrawMe()
{
  addSortDraw();  
}

void SkeletonSpearman::drawSorted()
{
  drawMe();

  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 1.0, 1.0, 1.0));
    attackCircle.draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void SkeletonSpearman::updateCollisions()
{
  if (value == VAL_SIDEWAYS)
  {
    attackCircle = Circle(getXY() + Point2(60.0 * facingToDir(), -55.0), 20.0);
  }
  else if (value == VAL_VERTICAL)
  {
    attackCircle = Circle(getXY() + Point2(-8.0 * facingToDir(), -130.0), 14.0);
  }
  
  Logical attackHitPlayer = Circle::collision(attackCircle, player->collisionCircle());
  if (value == VAL_VERTICAL && player->standingTimer.getActive() == true) 
  {
    attackHitPlayer = false;
  }
  else if (value == VAL_SIDEWAYS && player->collisionCircle().bottom() < level->getGroundY() - 32.0) 
  // else if (value == VAL_SIDEWAYS && player->standingTimer.getActive() == false)
  {
    attackHitPlayer = false;
  }
  
  if (hitThisAttack == true && attackHitPlayer == false)
  {
    hitThisAttack = false;
  }
  else if (hitThisAttack == false && attackHitPlayer == true)
  {
    hitThisAttack = true;
    player->attacked(attackCircle.xy, this);
  }
  else if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
  {
    createMagnetCoin();
    destroyPhysicalStd(Coord2(3, 4), getImage());
    smokePuffStd();
    level->enemyDefeated(this);
  }           
}

Circle SkeletonSpearman::collisionCircle()
{
  if (value == VAL_SIDEWAYS)
  {
    return Circle(getXY() + Point2(0.0, -64.0), 36.0);
  }
  else
  {
    return Circle(getXY() + Point2(-8.0 * facingToDir(), -82.0), 30.0);    
  }
}

// =============================== SkeletonTutorial ============================ //

SkeletonTutorial::SkeletonTutorial() :
  PizzaGO(),
  animation(),
  shaker(2.0),
  walkSpeed(RM::randf(20.0, 30.0))
{
  basePoints = 5;
  setWH(32.0, 48.0);  
  shaker.setMagnitude(3.0);
  shadowImg = imgCache[shadowMedium];
  animation.init(this, imgsetCache[skeletonUnarmed], 0.28);
  animation.randomize(true);
  animation.repeats();
}

void SkeletonTutorial::load()
{
  facePlayer();
  sortY = getY();
}

void SkeletonTutorial::updateMe()
{
  if (lifeState == RM::ACTIVE) 
  {
    animation.update();
    addX(walkSpeed * facingToDir() * RM::timePassed()); 
    turnAtWalls();
    updateCollisions();
  }
  else
  {
    shaker.update();   
  }
}

void SkeletonTutorial::redrawMe()
{
  addSortDraw();  
}

void SkeletonTutorial::setIntro()
{
  addY(getImage()->natural_size().y);
  lifeState = RM::TRANS_IN;
  setAlpha(0.0);
}

void SkeletonTutorial::drawSorted()
{
  Point1 realAlpha = getAlpha();
  setAlpha(1.0);
  
  Box clippingBox = imageBox();
  clippingBox.xy.y = sortY - getImage()->natural_size().y;
  clippingBox.grow_down(8.0);
  
  if (lifeState == RM::TRANS_IN) RMGraphics->clip_world(clippingBox);
  drawMe(shaker.offsets);
  if (lifeState == RM::TRANS_IN) RMGraphics->unclip();
  
  setAlpha(realAlpha);

  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 1.0, 1.0, 1.0));
  }
}

void SkeletonTutorial::updateCollisions()
{
  if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
  {
    smokePuffStd();
    destroyPhysicalStd(Coord2(3, 4), getImage());
    level->enemyDefeated(this);
  }           
}

Circle SkeletonTutorial::collisionCircle()
{
  return Circle(getXY() + Point2(17.0 * facingToDir(), -64.0), 48.0);    
}

// =============================== BoxPlayer ============================ //

BoxPlayer::BoxPlayer(GLLevelTutorial* setTut) :
  PizzaGO(),
  tutLevel(setTut),
  dmg(0),
  dmgTimer(1.0),

  shakeOffset(0.0, 0.0),
  shakeTimer(0.5, this),
  rotator(&rotation, 0.0, 8.0 / 60.0, this),
  yMover(&shakeOffset.y, 0.0, 8.0 / 60.0),

  glowFrame(0),
  glowAlpha(0.0)
{
  setImage(imgsetCache[pizzaBoxSet][0]);
  shadowImg = imgCache[shadowSmall];
  rotator.setActive(false);
  setWH(56.0, 56.0);
  lifeState = RM::TRANS_IN;
}

void BoxPlayer::load()
{
  sortY = level->getGroundY();
  init_box(getXY(), getSize(), true);
  fixture->SetUserData(this);
  change_density(2.5);
  set_collisions(1 << TYPE_PLAYER, 0xffff);
}

void BoxPlayer::updateMe()
{
  if (lifeState == RM::TRANS_IN)
  {
    shakeTimer.update();
    rotator.update();
    yMover.update();
    
  }
  else if (lifeState == RM::ACTIVE)
  {
    updateFromPhysical();    
    dmgTimer.update();
  }
  else if (lifeState == RM::TRANS_OUT)
  {
    destroyPhysicalStd(Coord2(4, 4), getImage());
    return;
  }
  
}

void BoxPlayer::redrawMe()
{
  drawMe(shakeOffset);
}

void BoxPlayer::tryMove(Point1 tiltPercent)
{
  tiltPercent = std::max(std::abs(tiltPercent), 0.5) * RM::sign(tiltPercent);
  set_ang_vel(get_ang_vel() + tiltPercent * TWO_PI * 0.3 * RM::timePassed());  
}

void BoxPlayer::tryJump(Point1 tiltPercent)
{
  if (standingPhysical() == false) return;
  
  if (lifeState == RM::TRANS_IN)
  {
    setActive();
    shakeOffset.set(0.0, 0.0);
    setRotation(0.0);
  }
  
  Point2 currVel = get_velocity();

  tiltPercent = std::max(std::abs(tiltPercent), 0.25) * RM::sign(tiltPercent);
  set_velocity(Point2(tiltPercent * 25.0, -350.0));
}

void BoxPlayer::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (dmgTimer.getActive() == true) return;
  
  dmg++;
  dmgTimer.reset();
  
  level->addAction(new DustEffect(Point2(getX(), level->getGroundY()), HALF_PI));
  ResourceManager::landing->play();
  
  if (dmg < 3)
  {
    setImage(imgsetCache[pizzaBoxSet][dmg]);
  }
  else
  {
    lifeState = RM::TRANS_OUT;
    ResourceManager::playPizzaBoxBreak();
    tutLevel->boxDone();
  }
}

void BoxPlayer::callback(ActionEvent* caller)
{
  if (caller == &shakeTimer)
  {
    if (onScreenFixed() == true) ResourceManager::playLanding(true);
    
    setRotation(RM::randf(RM::degs_to_rads(-10.0), RM::degs_to_rads(10.0)));
    Point1 lowYOff = std::abs(std::sin(getRotation()) * 28.0);
    shakeOffset.y = RM::randf(-lowYOff, -10.0);
    yMover.reset();
    rotator.reset();
  }
  else if (caller == &rotator)
  {
    shakeTimer.reset(RM::randf(0.0, 0.5));
    shakeOffset.y = 0.0;
  }
}

// =============================== PumpkinSwooper ============================ //

PumpkinSwooper::PumpkinSwooper() :
  PizzaGO(),

  animation(this, ResourceManager::pumpkinFly, 0.1),
  xSwoopTimer(3.0, this),
  swoopData(0.0, 1.0),
  swoopWaitTimer(RM::randf(1.8, 2.2), this),

  seenPlayer(false),
  swooping(false),
  startY(0.0),
  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0)
{
  basePoints = 20;
  flies = true;
  setWH(17.0, 17.0);   // size is half size
  
  xSwoopTimer.setActive(false);
  shadowImg = imgCache[shadowSmall];
  
  animation.repeats();
  animation.randomize();
}

void PumpkinSwooper::load()
{
  startY = getY();
  facePlayer();
}

void PumpkinSwooper::updateMe()
{
  if (seenPlayer == false)
  {
    if (onScreen() == true)
    {
      seenPlayer = true;
    }
  }
  else      
  {
    swoopWaitTimer.update();
  }
  
  xSwoopTimer.update();
  turnAtWalls();
  
  if (xSwoopTimer.getActive() == false)
  {
    // face player if not swooping
    facePlayer();
    RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), RM::timePassed() * 2.0);
    animation.update();
  }
  else 
  {
    addX(85.0 * facingToDir() * RM::timePassed());
    RM::bounce_arcsine(xy.y, swoopData, Point2(startY, startY + 156.0), RM::timePassed() * 0.66);
    setImage(imgsetCache[pumpkinImgs].last());
  }

  updateCollisions();
}

void PumpkinSwooper::redrawMe()
{
  drawMe(Point2(0.0, bobOffset));
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
  }
}

void PumpkinSwooper::updateCollisions()
{
  Logical attackHitPlayer = Circle::collision(collisionCircle(), player->collisionCircle());
  
  if (hitThisAttack == true && attackHitPlayer == false)
  {
    hitThisAttack = false;
  }
  else if (hitThisAttack == false && attackHitPlayer == true)
  {
    hitThisAttack = true;
    Point1 angle = RM::angle(player->getXY(), getXY());
    
    if (angle > PI * 0.15 && angle < PI * 0.85)
    {
      createMagnetCoin(NICKEL);
      smokePuffStd();
      destroyPhysicalStd(Coord2(3, 4), getImage());
      level->enemyDefeated(this);
    }
    else
    {
      // player hit
      player->attacked(getXY(), this);
    }    
  }
}

Box PumpkinSwooper::collisionBox()
{
  return boxFromC();
}

void PumpkinSwooper::callback(ActionEvent* caller)
{
  if (caller == &swoopWaitTimer)
  {
    ResourceManager::playDemonSwoop();
    xSwoopTimer.reset();
    swoopData.set(RM::randf(0.0, 0.1), 1.0);
  }
  else if (caller == &xSwoopTimer)
  {
    swoopWaitTimer.reset(RM::randf(1.9, 2.1));
  }
}

// =============================== Spiny ============================ //

const Point1 Spiny::DRAW_OFFSET = -24.0;

Spiny::Spiny() : 
  PizzaGO(),

  animator(this, imgsetCache[spinyImgs], 0.065),

  flipped(false),
  airState(ON_GROUND),

  groundY(0.0),
  yVel(0.0),
  rotFlipper(&rotation, -PI, 0.5),
  flipTimer(4.5, this)
{
  basePoints = 20;
  setWH(48.0, 48.0);  // this is radius and extends under ground

  shadowImg = imgCache[shadowSmall];
  animator.repeats();
  animator.randomize();
  
  rotFlipper.setActive(false);
  flipTimer.setActive(false);
}

void Spiny::load()
{
  facePlayer();
  groundY = getY();
  sortY = groundY;
}

void Spiny::updateMe()
{
  animator.update();
  
  if (flipped == false && airState == ON_GROUND)
  {
    addX(facingToDir() * 100.0 * RM::timePassed());
    turnAtWalls();
    if (getHFlip() == true && getX() < player->getX() - 256.0) hflip(false);
    if (getHFlip() == false && getX() > player->getX() + 256.0) hflip(true);
  }
  else 
  {
    updatePop();
  }
  
  updateCollisions();
}

void Spiny::redrawMe()
{
  addSortDraw();
}

void Spiny::drawSorted()
{
  drawMe(Point2(0.0, DRAW_OFFSET));
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
  }  
}

void Spiny::updatePop()
{
  rotFlipper.update();
  flipTimer.update();

  if (airState != ON_GROUND)
  {
    yVel += 900.0 * RM::timePassed();
    addY(yVel * RM::timePassed());
  }
  
  // landed on ground on back
  if (airState == TO_BACK &&
      yVel > 0.0 && getY() > groundY)
  {
    airState = ON_GROUND;
    setY(groundY);
  }
  // landed on ground on front
  else if (airState == TO_FRONT &&
           yVel > 0.0 && getY() > groundY)
  {
    airState = ON_GROUND;
    setY(groundY);
  }
}

void Spiny::pizzaSlammed()
{
  if (std::abs(player->getX() - getX()) > PLAY_W * 0.75) return;
  
  // flip from front to back
  if (flipped == false && airState == ON_GROUND)
  {
    flipToBack();
  }
  // flip from back to front
  else if (flipped == true && airState == ON_GROUND)
  {
    flipToFront(true);
  }
}

void Spiny::flipToBack()
{
  flipped = true;
  airState = TO_BACK;
  yVel = -300.0;
  
  Point1 rotTarget = -PI;
  if (getHFlip() == true) rotTarget = PI;
  
  flipTimer.reset();
  rotFlipper.reset(getRotation() + rotTarget);
  animator.setTimes(0.035);
}

void Spiny::flipToFront(Logical fromSlam)
{
  flipped = false;
  airState = TO_FRONT;
  yVel = -300.0;
  
  Point1 rotTarget = PI;
  if (getHFlip() == true) rotTarget = -PI;
  
  if (fromSlam == true) rotTarget *= -1.0;
  
  rotFlipper.reset(getRotation() + rotTarget);
  animator.setTimes(0.065);
}

void Spiny::updateCollisions()
{
  if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
  {
    if (flipped == true || (flipped == false && airState == TO_FRONT)) 
    {
      addY(DRAW_OFFSET);
      
      smokePuffStd();
      createMagnetCoin(NICKEL);
      destroyPhysicalStd(Coord2(3, 4), getImage());
      level->enemyDefeated(this);
      
      addY(-DRAW_OFFSET);
    }
    else if (flipped == false && hitThisAttack == false)
    {
      hitThisAttack = true;
      player->attacked(getXY(), this);  // note: the collisionBox is not yet accurate
    }
  }
  else
  {
    hitThisAttack = false;
  }
}

void Spiny::callback(ActionEvent* caller)
{
  if (caller == &flipTimer)
  {
    if (flipped == true) flipToFront(false);
  }
}

// =============================== Skelostrich ============================ //

Skelostrich::Skelostrich(Coord1 setValue) :
  PizzaGO(),
  
  walkAnim(),
  flyAnim(),
  
  attackCircle(getXY(), 24.0),
  airState(ON_GROUND),
  seenPlayer(false),

  walkSpeed(RM::randf(130.0, 160.0)),

  jumpTimer(RM::randf(1.5, 2.3), this),
  groundY(0.0),
  yVel(0.0)
{
  value = setValue;
  setWH(48.0, 48.0);
}

void Skelostrich::load()
{ 
  DataList<Image*>* imageSet = &imgsetCache[ostrichWalkSide];
  if (value == VAL_VERTICAL)
  {
    basePoints = 20;
    imageSet = &imgsetCache[ostrichWalkVert];
    jumpTimer.setActive(false);
  }
  else
  {
    basePoints = 30;
    flyAnim.init(this, imgsetCache[ostrichFlySide], 0.065);
  }
  
  setImage(imageSet->first());
  shadowImg = imgCache[shadowMedium];
  
  walkAnim.init(this, *imageSet, 0.065);
  walkAnim.repeats();
  walkAnim.randomize();
  
  flyAnim.repeats();  
  
  facePlayer();
  groundY = getY();
  sortY = groundY;
  attackCircle = Circle(getXY(), 24.0);
}

void Skelostrich::updateMe()
{
  addX(facingToDir() * walkSpeed * RM::timePassed());
  turnAtWalls();
  
  if (airState == ON_GROUND)
  {
    walkAnim.update();
    
    // don't start jump timer until you see the player for the first time
    if (seenPlayer == false)
    {
      if (onScreen() == true) seenPlayer = true;
    }
    else      
    {
      jumpTimer.update();
    }
  }
  else if (airState == JUMPING)
  {
    flyAnim.update();
    yVel += 400.0 * RM::timePassed();
    addY(yVel * RM::timePassed());
    if (yVel > 0.0 && getY() > groundY)
    {
      // just landed
      airState = ON_GROUND;
      jumpTimer.reset();
    }
  }
  
  updateCollisions();
}

void Skelostrich::redrawMe()
{
  addSortDraw();
}

void Skelostrich::drawSorted()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    attackCircle.draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void Skelostrich::jump()
{
  yVel = -300.0;
  airState = JUMPING;
  ResourceManager::playOstrichHop();
}

void Skelostrich::updateCollisions()
{
  if (value == VAL_SIDEWAYS)
  {
    attackCircle = Circle(getXY() + Point2(70.0 * facingToDir(), -65.0), 20.0);
  }
  else
  {
    attackCircle = Circle(getXY() + Point2(5.0 * facingToDir(), -148.0), 20.0);
  }
  
  Logical attackCollision = Circle::collision(attackCircle, player->collisionCircle());
  
  if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
  {
    smokePuffStd();
    createMagnetCoin(NICKEL);
    destroyPhysicalStd(Coord2(3, 4), getImage());
    level->enemyDefeated(this);
  }
  else if (attackCollision == true && 
           hitThisAttack == false &&
           (value == VAL_SIDEWAYS || player->isStanding == false))  // so up won't hit unless player jumps
  {
    player->attacked(attackCircle.xy, this);
    hitThisAttack = true;
  }
  
  if (attackCollision == false) hitThisAttack = false;
}

Circle Skelostrich::collisionCircle()
{
  if (value == VAL_SIDEWAYS)
  {
    return Circle(getXY() + Point2(2.0 * facingToDir(), -64.0), 50.0);
  }
  else
  {
    return Circle(getXY() + Point2(14.0 * facingToDir(), -78.0), 50.0);
  }
}

void Skelostrich::callback(ActionEvent* caller)
{
  if (caller == &jumpTimer)
  {
    jump();
  }
}

// =============================== SkeleCupid ============================ //

SkeleCupid::SkeleCupid() :
  PizzaGO(),
  ActionListener(),
  
  idleAnim(this, imgsetCache[cupidIdleSet], 0.06),
  wingAnim(this, ResourceManager::cupidWingSet, 0.06),
  fireAnim(this, imgsetCache[cupidShootSet], 0.15, this),
  bodyAnim(&idleAnim),

  fireTimer(2.0, this),
  turnTimer(1.0, this),
  velX(0.0),

  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0)
{
  basePoints = 40;
  flies = true;
  idleAnim.apply();
  shadowImg = imgCache[shadowSmall];
  wingAnim.repeats();
  idleAnim.repeats();
}

void SkeleCupid::load()
{
  facePlayer();
}

void SkeleCupid::updateMe()
{  
  turnTimer.update();
  Point1 oldX = getX();
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());

  if (onScreen() == true) 
  {
    fireTimer.update();
    Point1 playerMax = level->playerTXVel();
    
    Point1 finalVelX = player->get_velocity().x * 1.2;
    if (RM::sign(player->get_velocity().x) == facingToDir()) finalVelX = 0.0;
    finalVelX += 0.35 * playerMax * facingToDir();
    
    addX(finalVelX * RM::timePassed());  
  }
  else
  {
    addX(250.0 * facingToDir() * RM::timePassed());
  }
  
  // clamp inside walls
  Point1 width = collisionCircle().radius;
  xy.x = RM::clamp(xy.x, level->worldBox.left() + width, level->worldBox.right() - width);

  velX = getX() - oldX;
 
  Coord1 startIndex = bodyAnim->currIndex();
  
  bodyAnim->update();
  
  Point1 wingMult = RM::clamp(std::abs((velX / RM::timePassed()) / 250.0), 1.0, 3.0);
  wingAnim.setSpeedMult(wingMult);
  wingAnim.update();
  
  if (fireTimer.getActive() == false &&
      startIndex < 5 &&
      bodyAnim->currIndex() == 5)
  {
    fireArrow();
  }
      
  updateCollisions();
}

void SkeleCupid::redrawMe()
{
  setImage(bodyAnim->currImage());
  drawMe(Point2(0.0, bobOffset));

  setImage(wingAnim.currImage());
  drawMe(Point2(0.0, bobOffset));
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void SkeleCupid::beginFire()
{
  fireAnim.reset();
  bodyAnim = &fireAnim;
}

void SkeleCupid::fireArrow()
{
  Point2 firePoint = getXY() + Point2(22.0 * facingToDir(), -5.0);
  
  ResourceManager::playCupidShoot();
  
  CupidArrow* arrow = new CupidArrow(firePoint, 200.0 * facingToDir() + velX);
  level->addAlly(arrow);  // ally so it doesn't affect facing or spawning
}

void SkeleCupid::updateCollisions()
{
  if (playerCollision() == true)
  {
    smokePuffStd();
    createMagnetCoin(DIME);
    destroyPhysicalStd(Coord2(3, 4), bodyAnim->currImage());
    level->enemyDefeated(this);
  }
}

Circle SkeleCupid::collisionCircle()
{
  return Circle(getXY(), 24.0);
}

void SkeleCupid::callback(ActionEvent* caller)
{
  if (caller == &fireTimer)
  {
    beginFire();
  }
  else if (caller == &turnTimer)
  {
    facePlayer();
    turnTimer.reset();
  }
  else if (caller == &fireAnim)
  {
    bodyAnim = &idleAnim;
    fireTimer.reset(3.5);
  }
}

// =============================== CupidArrow ============================ //

CupidArrow::CupidArrow(Point2 origin, Point1 setVelX) :
  PizzaGO(),
  ActionListener(),

  stickBehavior(this),
  xVel(setVelX),
  rotationOffset(0.0),
  rotationData(0.0, 1.0)
{
  stickBehavior.setListener(this);
  setImage(imgCache[cupidArrow]);
  setXY(origin);
  faceDir(setVelX);
}

void CupidArrow::updateMe()
{
  if (lifeState == RM::ACTIVE)
  {
    addX(xVel * RM::timePassed());
    if (onScreenFixed() == false) lifeState = RM::REMOVE;
    updateCollisions();
    
    RM::bounce_arcsine(rotationOffset, rotationData, 
                    Point2(-PI * 0.02, PI * 0.02), RM::timePassed() * 4.0);    
    
    setRotation(rotationOffset);
  }
  else if (lifeState == RM::PASSIVE)
  {
    stickBehavior.update();
    setAlpha(1.0 - RM::clamp((stickBehavior.percentDone() - 0.5) / 0.5, 0.0, 1.0));
  }
  
}

void CupidArrow::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void CupidArrow::updateCollisions()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (playerCollision() == true)
  {
    player->attacked(getXY(), this);

    stickBehavior.reset(player, 1.5);
    lifeState = RM::PASSIVE;
  }
}

Circle CupidArrow::collisionCircle()
{
  return Circle(getXY(), 7.0);
}

void CupidArrow::callback(ActionEvent* caller)
{
  if (caller == &stickBehavior)
  {
    setRemove();
  }
}

// =============================== Naga ============================ //

const Point1 
Naga::TAIL_LENGTH = 256.0,
Naga::TAIL_AMPLITUDE_START = 32.0,
Naga::TAIL_PERIODS_START = 1.0,
Naga::BASE_BONE_SIZE = 16.0,
Naga::MAX_ATTACK_SPEED = 500.0,
Naga::ATTACK_ACCEL = 1000.0;

Naga::Naga() :
  PizzaGO(),
  ActionListener(),

  pieces(),
  tailLength(TAIL_LENGTH),

  sinePercent(0.0),
  currAmplitude(TAIL_AMPLITUDE_START),
  currNumPeriods(TAIL_PERIODS_START),

  attackTimer(2.0, this),
  currSpeed(0.0),
  attacking(false),

  flameGen(0.0),
  flameList()
{
  basePoints = 85;
  flies = true;
  shadowImg = imgCache[shadowMedium];
  setImage(imgCache[nagaHeadOpen]);
  setWH(32.0, 32.0);
  attackTimer.setActive(false);
}

void Naga::load()
{
  facePlayer();

  pieces.ensure_capacity(24);
  Point1 currPiecePlace = tailLength;
  Point2 runningXY = getXY();
  
  while (currPiecePlace > 0.0)
  {
    Point1 pieceScale = RM::lerp(0.5, 1.0, currPiecePlace / tailLength);
    
    pieces.add(VisRectangular());
    pieces.last().setImage(imgCache[nagaBone]);    
    pieces.last().setWH(BASE_BONE_SIZE * pieceScale, BASE_BONE_SIZE * pieceScale);
    pieces.last().setScale(pieceScale, pieceScale);
    pieces.last().hflip(getHFlip());
    
    runningXY += Point2(pieces.last().getWidth(), 0.0);
    pieces.last().setXY(runningXY);
    
    // cout << currPiecePlace << " " << pieces.last().getWidth() << endl;
    currPiecePlace -= BASE_BONE_SIZE * pieceScale;
    
    if (currPiecePlace <= 0.0)
    {
      pieces.last().setImage(imgCache[nagaTail]);
    }
  }
  
  updatePieces(true);
}

void Naga::updateMe()
{
  sinePercent = RM::wrap1(sinePercent + RM::timePassed(), 0.0, 1.0);
  // currAmplitude += 5.0 * RM::timePassed();
  flameList.update();
  
  if (attackTimer.getActive() == false && onScreen() == true) attackTimer.setActive(true);
  if (attacking == false) generateFlames();
  attackTimer.update();
  
  RM::flatten_me(currSpeed, attacking ? MAX_ATTACK_SPEED : 0.0, RM::timePassed() * ATTACK_ACCEL);
  addX(currSpeed * facingToDir() * RM::timePassed());
  turnAtWalls();
  
  updatePieces(false);
  updateCollisions();
}

void Naga::redrawMe()
{
  setImage(attacking ? imgCache[nagaHeadClosed] : imgCache[nagaHeadOpen]);
  drawMe();
  drawPieces();
  flameList.redraw();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    
    for (Coord1 i = 0; i < pieces.count; ++i)
    {
      if (i == 0 || i % 3 != 0) continue;
      
      Circle(pieces[i].getXY(), pieces[i].getWidth()).draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
    }
  }
}

void Naga::updatePieces(Logical firstUpdate)
{
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    VisRectangular& currPiece = pieces[i];
    VisRectangular& lastPiece = i == 0 ? *this : pieces[i-1];
    
    Point1 piecePercent = RM::lerp_reverse(currPiece.getXScale(), 0.5, 1.0);
    Point1 curveVal = std::fmod(piecePercent + sinePercent, 1.0) * TWO_PI * currNumPeriods;
    
    // Point1 waveMult = 1.0;
    Point1 waveMult = std::sqrt(1.0 - piecePercent);
    Point1 maxDistance = (lastPiece.getWidth() * 0.5 + currPiece.getWidth() * 0.5);
    
    if (i == 0) 
    {
      waveMult = 0.1;  // this is just an approximation based on current values
      maxDistance = 16.0;
    }
    
    Point1 oldX = currPiece.getX();
    Point1 pieceX = RM::clamp(currPiece.getX(), lastPiece.getX() - maxDistance, 
                              lastPiece.getX() + maxDistance);
    if (firstUpdate == true) pieceX = lastPiece.getX() + maxDistance * -facingToDir();
    
    if (pieceX != oldX) currPiece.hflip(oldX > pieceX);
    if (firstUpdate == true) currPiece.hflip(getHFlip());
    
    currPiece.setX(pieceX);
    currPiece.setY(getY() + std::sin(curveVal) * currAmplitude * waveMult);
    currPiece.setRotation(std::cos(curveVal) * PI * 0.2 * waveMult);
  }
  
  setRotation(pieces.first().getRotation());
}

void Naga::updateCollisions()
{
  Circle headCircle = collisionCircle();
  Circle playerCircle = player->collisionCircle();

  Logical pieceHit = false;
  Logical headHit = false;
  
  // NOTE: this is the same as the debug draw loop
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    if (i == 0 || i % 3 != 0) continue;
    
    Circle pieceCircle(pieces[i].getXY(), pieces[i].getWidth());
    
    if (pieceCircle.collision(player->collisionCircle()) == true)
    {
      if (hitThisAttack == false)
      {
        player->attacked(pieceCircle.xy, this);
      }
      
      pieceHit = true;
      hitThisAttack = true;
      break;
    }
  }

  
  if (headCircle.collision(player->collisionCircle()) == true)
  {
    if (hitThisAttack == false)
    {
      Point1 collisionAngle = RM::angle(playerCircle.xy, headCircle.xy);

      if (attacking == true &&
          collisionAngle >= PI * 0.3 && collisionAngle <= PI * 0.7)
      {
        createMagnetCoin(QUARTER);
        smokePuffStd();
        destroy();
        level->enemyDefeated(this);
      }
      else
      {
        player->attacked(headCircle.xy, this);
      }
    }
    
    headHit = false;
    hitThisAttack = true;
  }
  
  if (pieceHit == false && headHit == false) hitThisAttack = false;
}

void Naga::drawPieces()
{
  for (Coord1 i = pieces.count - 1; i >= 0; --i)
  {
    // i think it has to do the rotation compensation because the curve
    // equation changes in the other direction
    Point1 currRot = pieces[i].getRotation();
    pieces[i].setRotation(pieces[i].getHFlip() ? TWO_PI - currRot : currRot);
    pieces[i].drawMe();
    
    if (i % 4 == 3) 
    {
      Image* currImg = pieces[i].getImage();
      pieces[i].setImage(imgCache[nagaArm]);
      pieces[i].drawMe();
      pieces[i].setImage(currImg);
    }
    pieces[i].setRotation(currRot);
  }  
  drawMe();
}

void Naga::generateFlames()
{
  flameGen += RM::randf(0.0, 0.5) * RM::timePassed();
    
  while (flameGen > 1.0 / 60.0)
  {
    flameList.addX(new SunSpark(getXY() + Point2(16.0 * facingToDir(), 16.0)));
    flameGen -= 1.0 / 60.0;
  }
}

void Naga::destroy()
{
  destroyPhysicalStd(Coord2(2, 3), getImage());
  
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    level->addDebris(new BoneBreakEffect(pieces[i], 1, 1));
    
    if (i % 4 == 3)
    {
      pieces[i].setImage(imgCache[nagaArm]);
      level->addDebris(new BoneBreakEffect(pieces[i], 1, 1));      
    }
  }
}

Circle Naga::collisionCircle()
{
  return Circle(getXY(), getWidth() * 0.5);
}

void Naga::callback(ActionEvent* caller)
{
  if (attacking == false)
  {
    ResourceManager::playNagaCharge();
    facePlayer();
    attacking = true; 
    attackTimer.reset(2.0);
  }
  else
  {
    attacking = false;
    attackTimer.reset(2.0);    
  }
}


// =============================== Wisp ============================ //

Wisp::Wisp() :
  PizzaGO(),
  ActionListener(),

  warper(this, dwarpCache[wispWarp]),
  warpWeight(0.0),
  warpData(0.0, 1.0),

  mover(this),
  stateTimer(2.0, this),

  currState(INVISIBLE),
  blinkCount(0),
  seenPlayer(false)
{
  basePoints = 70;
  setImage(imgsetCache[wispSet][0]);
  setWH(8.0, 8.0);   // half size
  flies = true;
}

void Wisp::load()
{
  Point1 moveTime = RM::randf(1.5, 2.0);
  Box bounds(getX() - PLAY_W * 0.5, PLAY_Y + getHeight(), 
             PLAY_W, level->getGroundY() - getHeight() - (PLAY_Y + getHeight()));
  RM::clamp_me(bounds.xy.x, level->worldBox.left(), level->worldBox.right() - bounds.width());
  mover.init(bounds, Point2(moveTime, moveTime));
  
  setInvisible();
}

void Wisp::updateMe()
{
  // don't do anything until you see the player for the first time
  if (seenPlayer == false)
  {
    if (onScreen() == true) seenPlayer = true;
    else return;
  }
    
  RM::bounce_linear(warpWeight, warpData, Point2(0.0, 1.0), RM::timePassed());
  
  stateTimer.update();
  mover.update();
  facePlayer();
  
  updateCollisions();
}

void Wisp::redrawMe()
{
  warper.draw(warpWeight);
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(0.0, 0.0, 1.0, 1.0));
  }
}

void Wisp::updateCollisions()
{
  if (currState == VISIBLE &&
      playerCollision() == true)
  {
    ResourceManager::playWispDie();
    createMagnetCoin(QUARTER);
    level->addAction(new WispExplode(getXY()));
    level->enemyDefeated(this);
    setRemove();
  }
}

void Wisp::setVisible()
{
  currState = VISIBLE;
  setImage(imgsetCache[wispSet][0]);
  stateTimer.reset(2.0);  
}

void Wisp::setInvisible()
{
  currState = INVISIBLE;
  setImage(imgsetCache[wispSet][1]);
  stateTimer.reset(1.0);
}

void Wisp::setBlink()
{
  currState = INVIS_BLINK;
  setImage(imgsetCache[wispSet][2]);
  stateTimer.reset(1.5);
}

void Wisp::fire()
{
  Point1 targetX = RM::clamp(player->getX(), getX() - 196.0, getX() + 196.0);
  Point1 targetY = std::max(level->getGroundY() - 96.0, getY() + 96.0);
  
  ResourceManager::playWispShoot();
  
  WispFlame* flame = new WispFlame(getXY(), Point2(targetX, targetY));
  level->addAlly(flame);
}

void Wisp::callback(ActionEvent* caller)
{
  if (caller == &stateTimer)
  {
    switch (currState)
    {
      default:
      case VISIBLE:
        setInvisible();
        break;
      case INVISIBLE:
        if (blinkCount >= 2) setVisible();
        else setBlink();
        blinkCount++;
        blinkCount %= 3;
        break;
      case INVIS_BLINK:
        fire();
        setInvisible();
        break;
    }
  }
}

// =============================== WispFlame ============================ //

WispFlame::WispFlame(Point2 center, Point2 target) :
  PizzaGO(),
  animator(this, imgsetCache[wispFlameSet], 0.1),
  lifeTimer(5.0, this),
  vel(RM::ring_edge(center, target, 100.0))
{
  animator.repeats();
  setBox(center.x, center.y, 6.0, 6.0);
  setRotation(RM::angle(vel));
}

void WispFlame::updateMe()
{
  animator.update();
  
  lifeTimer.update();
  if (lifeState == RM::PASSIVE) setAlpha(1.0 - lifeTimer.progress());
  
  addXY(vel * RM::timePassed());
  
  if (lifeState == RM::ACTIVE)
  {
    if (getY() >= level->getGroundY()) 
    {
      dissipate();
    }
    else if (playerCollision() == true)
    {
      player->attacked(getXY(), this);
      dissipate();
    }
  }
}

void WispFlame::redrawMe()
{
  drawMe();
}

void WispFlame::dissipate()
{
  if (lifeState != RM::ACTIVE) return;

  setPassive();
  vel.set(0.0, 0.0);
  lifeTimer.reset(0.2);
}

void WispFlame::callback(ActionEvent* caller)
{
  if (lifeState == RM::ACTIVE)
  {
    dissipate();
  }
  else
  {
    setRemove();
  }
}

// =============================== WispExplode ============================ //

WispExplode::WispExplode(Point2 setXY) :
  ParticleEffect()
{

  // 2. Create burst
  
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(10, 20), Point2(1.5, 2.0), setXY);
  burst->images.append(imgsetCache[wispParticles]);
  
  burst->speedBounds.set(0.0, 150.0);
  burst->angleBounds.set(0.0, TWO_PI);
  
  burst->constantRotate = true;
  // burst->ballisticRotate = true;
  burst->rotateSpeedBounds.set(HALF_PI, TWO_PI);
  
  burst->gravity.set(0.0, 0.0);  
  
  // burst->alphaOscillate = true;
  // burst->alphaOscillateBounds.set(0.25, 0.5);
  
  burst->scaleBounds.set(0.75, 1.0);
  
  // 3. Add burst to emitter list
  
  emitters.add(burst);
}

// =============================== RobotShatter ============================ //

RobotShatter::RobotShatter() :
  ParticleEffect(),
  burst(NULL)
{
  
}

void RobotShatter::init(const VisRectangular& robot)
{
  burst = new ProceduralBurst(robot, 3, 4, Point2(5.0, 5.0));
  burst->images.append(imgsetCache[wispParticles]);
  
  burst->speedBounds.set(0.0, 150.0);
  burst->angleBounds.set(0.0, TWO_PI);
  
  burst->constantRotate = true;
  // burst->ballisticRotate = true;
  burst->rotateSpeedBounds.set(HALF_PI, TWO_PI);
  
  burst->gravity.set(0.0, 0.0);  
  
  // burst->alphaOscillate = true;
  // burst->alphaOscillateBounds.set(0.25, 0.5);
  
  burst->scaleBounds.set(0.75, 1.0);
  
  // 3. Add burst to emitter list
  
  burst->manageTime = true;
  
  emitters.add(burst);
}

void RobotShatter::reset(const VisRectangular& robot)
{
  burst->triSet.reset(robot);
}

Point1* RobotShatter::getTimePtr()
{
  return &burst->currTime;
}

// =============================== Robot ============================ //

Robot::Robot() :
  PizzaGO(),
  ActionListener(),

  walkSpeed(RM::randf(80.0, 120.0)),
  seenPlayer(false),

  walkAnim(this, imgsetCache[roboWalkImgs], 0.12 * walkSpeed / 100.0),
  atkAnim(this, imgsetCache[roboAtkImgs], 0.1, this),  // these times are unset
  atkFinishAnim(),
  currAnim(&walkAnim),

  shatter(),
  shatterScript(this),

  flame(Point2(0.0, 0.0), imgsetCache[roboFireballImgs][0]),
  flameAlpha(0.0),
  flameFrame(0),

  attackTimer(RM::randf(1.5, 2.0), this),
  flameTimer(1.0, this),
  attacking(false)
{
  basePoints = 50;
  walkAnim.apply();
  walkAnim.repeats();
  
  ArrayList<Point1> atkTimes(4, true);
  for (Coord1 i = 0; i < atkTimes.count; ++i) atkTimes[i] = 0.2;
  atkTimes.last() = 0.1;
  atkAnim.setTimes(atkTimes);
  
  DataList<Image*> atkFinishImgs(1, true);
  atkFinishImgs[0] = imgsetCache[roboAtkImgs][1];
  // atkFinishImgs[1] = ResourceManager::roboAtkImgs[1];
  // atkFinishImgs[2] = ResourceManager::roboAtkImgs[0];
  
  atkFinishAnim.init(this, atkFinishImgs, 0.15, this);
  
  flameTimer.setActive(false);
  
  shadowImg = imgCache[shadowMedium];
  
  shatter.init(*this);
  setWH(24.0, 24.0);  // radius
}

void Robot::load()
{
  facePlayer();
  sortY = getY();
}

void Robot::updateMe()
{  
  if (phasedIn() == true) 
  {
    currAnim->update();
    
    if (currAnim == &walkAnim)
    {
      addX(walkSpeed * facingToDir() * RM::timePassed());
      turnAtWalls();
      
      // don't start attack ai until onscreen for the first time
      if (seenPlayer == false)
      {
        if (onScreen() == true) 
        {
          seenPlayer = true;
        }
      }
      else      
      {
        attackTimer.update();
      }
    }
    else
    {
      flameTimer.update();
      
      // scale fireball
      if (attacking == true)
      {
        Point1 flameProgress = 0.0;
        Point1 timerProgress = flameTimer.progress();
        
        if (timerProgress < 0.2) flameProgress = RM::lerp_reverse(timerProgress, 0.0, 0.2);
        else if (timerProgress < 0.8) flameProgress = 1.0;
        else flameProgress = 1.0 - RM::lerp_reverse(timerProgress, 0.8, 1.0);
        
        flame.setScale(flameProgress, flameProgress);
      }
    }
  }
  else 
  {
    shatterScript.update();
    shatter.update();
  }
  
  // animate fireball
  flameAlpha += 8.0 * RM::timePassed();
  Coord1 wrapped = RM::wrap1_me(flameAlpha, 0.0, 1.0);
  if (wrapped != 0) 
  {
    flameFrame++;
    flameFrame %= imgsetCache[roboFireballImgs].count;
  }
  
  updateCollisions();
}

void Robot::redrawMe()
{
  if (phasedIn() == true) addSortDraw();
  else shatter.redraw();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    keyCircle().draw_outline(YELLOW_SOLID);
    if (attacking == true) attackCircle().draw_outline(RED_SOLID);
  }
}

void Robot::drawSorted()
{  
  drawMe();
  
  if (attacking == true)
  {
    flame.setXY(getXY() + Point2(54.0 * facingToDir(), -76.0));
    flame.hflip(getHFlip());
    
    flame.setAlpha(1.0 - flameAlpha);
    flame.setImage(imgsetCache[roboFireballImgs][flameFrame]);
    flame.drawMe();
    
    flame.setAlpha(flameAlpha);
    flame.setImage(imgsetCache[roboFireballImgs][(flameFrame + 1) % 
                                                     imgsetCache[roboFireballImgs].count]);
    flame.drawMe();
  }  
}

void Robot::updateCollisions()
{
  if (phasedIn() == false) 
  {
    hitThisAttack = false;
    return;
  }
  
  if (attacking == true &&
      playerCollision(attackCircle()) == true)
  {
    player->attacked(attackCircle().xy, this);
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
  
  if (playerCollision() == true)
  {
    attacking = false;
    attackTimer.reset(2.0);
    
    PoofEffect* poof = new PoofEffect(collisionCircle().xy, imgCache[roboElectricity]);
    poof->startScale = 0.0;
    poof->endScale = 1.0;
    poof->beforeFadeTime = 1.0;
    poof->fadeTime = 1.0;
    poof->init();
    level->addAction(poof);

    ResourceManager::playRobotPhase();

    shatter.reset(*this);
    shatterScript.enqueueX(new DeAccelFn(shatter.getTimePtr(), 1.5, 1.0));
    shatterScript.enqueueX(new AccelFn(shatter.getTimePtr(), 0.0, 1.0));
  }
  else if (playerCollision(keyCircle()) == true)
  {
    attacking = false;
    attackTimer.reset(2.0);  // it won't fire again
    
    smokePuffStd();
    createMagnetCoin(DIME);
    destroyPhysicalStd(Coord2(3, 4), getImage());
    level->enemyDefeated(this);
  }
}

void Robot::attack()
{
  setImage(currAnim->images.last());
}

Logical Robot::phasedIn()
{
  return shatterScript.length() <= 0;
}

Circle Robot::collisionCircle()
{
  return Circle(Point2(getX() + 36.0 * facingToDir(), getY() - 84.0), getWidth());
}

Circle Robot::attackCircle()
{
  return Circle(Point2(getX() + 140.0 * facingToDir(), getY() - 72.0), 64.0);
}

Circle Robot::keyCircle()
{
  return Circle(Point2(getX() - 24.0 * facingToDir(), getY() - 88.0), 24.0);
}

void Robot::callback(ActionEvent* caller)
{
  if (caller == &attackTimer)
  {
    currAnim = &atkAnim;
    currAnim->reset();
  }
  else if (caller == &atkAnim)
  {
    ResourceManager::playRobotFlame();
    attacking = true;
    flameTimer.reset();
  }
  else if (caller == &flameTimer)
  {
    currAnim = &atkFinishAnim;
    currAnim->reset();
    attacking = false;
  }
  else if (caller == &atkFinishAnim)
  {
    currAnim = &walkAnim;
    attackTimer.reset();
    facePlayer();
  }

}

// =============================== GiantSkull ============================ //

GiantSkull::GiantSkull() :
  PizzaGO(),
  breakTimer(2.5, this)
{
  setWH(22.0, 22.0);
  setImage(imgCache[giantSkull]);
  shadowImg = imgCache[shadowSmall];
}

void GiantSkull::load()
{
  sortY = level->getGroundY();
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger
  
  change_density(3.0);
  set_collisions(0x1 << TYPE_SKELETON, 0x1 << TYPE_TERRAIN | 0x1 << TYPE_DEBRIS);
  set_ang_vel(PI * facingToDir());
  set_velocity(Point2(300.0 * facingToDir(), 0.0));
  set_restitution(0.7);
  fixture->SetUserData(this);
}

void GiantSkull::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  if (lifeState == RM::ACTIVE &&
      playerCollision() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysicalStd(Coord2(3, 3), getImage());
    
    level->addAction(new BombExplosion(getXY()));
  }
}

void GiantSkull::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void GiantSkull::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  destroyPhysicalStd(Coord2(3, 3), getImage());
}

// =============================== Giant ============================ //

const Point1 Giant::WALK_SKULL_YOFFSET[] =
{
  -154.0,
  -155.0,
  -156.0,
  -155.0,
  
  -154.0,
  -153.0,
  -152.0,
  -153.0
};

const Point2 Giant::ATK_SKULL_OFFSET[] =
{
  Point2(-45.0, -164.0),
  Point2(10.0, -167.0),
  Point2(60.0, -156.0),
  Point2(102.0, -135.0)
};

// this is in degrees, convert to rads
const Point1 Giant::ATK_SKULL_ANGLES[] =
{
  -30.0,
  10.0,
  10.0,
  30.0
};

Giant::Giant() :
  PizzaGO(),
  ActionListener(),
  skull(NULL), // set during load()
  walkAnim(this, imgsetCache[giantWalkImgs], 0.15),
  throwAnim(),
  afterThrowAnim(),
  currAnim(&walkAnim),
  throwTimer(1.25, this)
{
  basePoints = 40;
  
  DataList<Image*> afterThrowImgs(1, true);
  afterThrowImgs[0] = imgsetCache[giantAtkImgs][1];
  afterThrowAnim.init(this, afterThrowImgs, 0.1, this);
  shadowImg = imgCache[shadowLarge];
  
  ArrayList<Point1> throwTimes(imgsetCache[giantAtkImgs].count);
  throwTimes.add(0.3);
  throwTimes.add(0.1);
  throwTimes.add(0.1);
  throwTimes.add(0.1);
  throwAnim.init(this, imgsetCache[giantAtkImgs], throwTimes, this);
  
  walkAnim.repeats();
  currAnim->apply();
  setWH(64.0, 64.0);  // radius
}

Giant::~Giant()
{
  // this would only happen if the scene ended with the skull in hand
  delete skull;
}

void Giant::load()
{
  facePlayer();
  sortY = getY();
  
  skull = new GiantSkull();
  skull->sortY = getY();
  syncSkull();
}

void Giant::updateMe()
{
  currAnim->update();
  
  if (currAnim == &walkAnim)
  {
    addX(30.0 * facingToDir() * RM::timePassed());
  }
  
  turnAtWalls();
  
  if (onScreen() == true)
  {
    throwTimer.update();
  }
  
  updateCollisions();

  if (skull != NULL)
  {
    syncSkull();
  }
}

void Giant::redrawMe()
{
  addSortDraw();
}

void Giant::syncSkull()
{
  skull->setXY(skullCircle().xy);
  skull->setRotation(skullRotation());
  skull->hflip(getHFlip());
}

void Giant::disconnectSkull()
{
  if (skull == NULL) return;
  
  ResourceManager::playGiantThrow();
  level->addAlly(skull);
  skull = NULL;
  currAnim = &walkAnim;
}

void Giant::updateCollisions()
{
  if (playerCollision() == true)
  {
    disconnectSkull();
    
    createMagnetCoin(DIME);
    smokePuffStd();
    destroyPhysicalStd(Coord2(4, 6), getImage());
    level->enemyDefeated(this);
  }
}

void Giant::drawSorted()
{
  drawMe();
  
  if (skull != NULL)
  {
    skull->drawMe();
  }
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    if (skull != NULL) skullCircle().draw_outline(RED_SOLID);
  }
}

Circle Giant::collisionCircle()
{
  return Circle(getXY() + Point2(12.0 * facingToDir(), -112.0), getWidth());
}

Circle Giant::skullCircle()
{
  Point2 center = getXY();
  
  if (currAnim == &walkAnim)
  {
    center += Point2(18.0 * facingToDir(), WALK_SKULL_YOFFSET[currAnim->currIndex()]);
  }
  else
  {
    center += Point2(ATK_SKULL_OFFSET[currAnim->currIndex()].x * facingToDir(), 
                     ATK_SKULL_OFFSET[currAnim->currIndex()].y);
  }
  
  return Circle(center, skull->getWidth());
}

Point1 Giant::skullRotation()
{
  Point1 rot = 0.0;
  
  if (currAnim == &throwAnim)
  {
    rot = ATK_SKULL_ANGLES[currAnim->currIndex()];
  }
  
  return RM::degs_to_rads(rot);
}

void Giant::callback(ActionEvent* caller)
{
  if (caller == &throwTimer && skull != NULL)
  {
    currAnim = &throwAnim;
  }
  else if (caller == &throwAnim)
  {
    disconnectSkull();
    currAnim = &afterThrowAnim;
  }
  else if (caller == &afterThrowAnim)
  {
    currAnim = &walkAnim;
  }
}

// =============================== FlameSkull ============================ //

FlameSkull::FlameSkull() :
  PizzaGO(),
  glowAlpha(0.0),
  glowFrame(0)
{
  basePoints = 10;
  setImage(imgCache[flamingSkull]);
  autoSize(0.5);  // size is radius
}

void FlameSkull::load()
{
  facePlayer();
}

void FlameSkull::updateMe()
{
  Point1 speed = 100.0;
  addX(speed * facingToDir() * RM::timePassed());
  addRotation((speed / getWidth()) * facingToDir() * RM::timePassed());
  
  updateGlow();  
  updateCollisions();
}

void FlameSkull::redrawMe()
{
  Point1 rot = getRotation();
  
  setRotation(0.0);
  setAlpha(1.0 - glowAlpha);
  setImage(imgsetCache[flameskullFlameSet][glowFrame]);
  drawMe();
  
  setAlpha(glowAlpha);
  setImage(imgsetCache[flameskullFlameSet][(glowFrame + 1) % 
               imgsetCache[flameskullFlameSet].count]);
  drawMe();
  
  setRotation(rot);
  setAlpha(1.0);
  setImage(imgCache[flamingSkull]);
  drawMe();
}

void FlameSkull::updateGlow()
{
  glowAlpha += 8.0 * RM::timePassed();
  glowFrame += RM::wrap1_me(glowAlpha, 0.0, 1.0) == 0 ? 0 : 1;
  glowFrame %= imgsetCache[flameskullFlameSet].count;
}

void FlameSkull::updateCollisions()
{
  Logical destroy = false;
  Logical earnCoin = false;

  if (playerCollision() == true)
  {
    player->attacked(getXY(), this);
    destroy = true;
  }
  
  if (player->facing == 1 && getHFlip() == false && onScreenFixed() == false)
  {
    destroy = true;
  }
  else if (player->facing == -1 && getHFlip() == true && onScreenFixed() == false)
  {
    destroy = true;
  }
  else if (getX() < player->getX() && getHFlip() == true) 
  {
    destroy = true;
    earnCoin = true;
  }
  else if (getX() > player->getX() && getHFlip() == false)
  {
    destroy = true;
    earnCoin = true;
  }
  else if (shouldTurn() == true)
  {
    destroy = true;
  }
  
  if (destroy == true) destroySkull();
  if (earnCoin == true) createMagnetCoin(NICKEL);
}

void FlameSkull::poofOut()
{
  smokePuffStd();

  PoofEffect* flamePoof = new PoofEffect(getXY(), imgsetCache[flameskullFlameSet][0]);
  flamePoof->hflip(getHFlip());
  flamePoof->beforeFadeTime = 0.0;
  flamePoof->fadeTime = 0.5;
  flamePoof->startScale = 1.0;
  flamePoof->endScale = 0.0;
  flamePoof->init();
  
  level->addAction(flamePoof);
}

void FlameSkull::destroySkull()
{
  destroyPhysicalStd(Coord2(2, 2), imgCache[flamingSkull]);
  poofOut();
  
  level->enemyDefeated(this);
}

// =============================== RabbitAngry ============================ //

RabbitAngry::RabbitAngry() : 
  PizzaGO(),
  ActionListener(),

  speedMult(RM::randf(1.0, 1.3)),
  animator(this, imgsetCache[rabbitSet], 0.06 * speedMult),
  upMover(&xy.y, 0.0, 0.75 * speedMult, this),
  downMover(&xy.y, 0.0, 0.75 * speedMult, this)
{
  basePoints = 40.0;
  setWH(24.0, 24.0);
  shadowImg = imgCache[shadowMedium];
}

void RabbitAngry::load()
{
  sortY = getY();
  
  upMover.reset(getY() - 150.0);
  downMover.reset(getY());
  downMover.setInactive();
  facePlayer();
}

void RabbitAngry::updateMe()
{
  addX(96.0 * facingToDir() * RM::timePassed());
  
  upMover.update();
  downMover.update();
  animator.update();
  
  updateCollisions();
}

void RabbitAngry::redrawMe()
{
  addSortDraw();
}

void RabbitAngry::drawSorted()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle().draw_outline(RED_SOLID);
  }
}

void RabbitAngry::updateCollisions()
{
  Point1 playerAngle = RM::angle(player->collisionCircle().xy, attackCircle().xy);
  
  Logical attackHitPlayer = playerCollision(attackCircle()) &&
      RM::angle_distance(playerAngle, HALF_PI) >= PI * 0.18;

  if (attackHitPlayer == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(attackCircle().xy, this);
    }
    
    hitThisAttack = true;
  }
  else 
  {
    hitThisAttack = false;
  }
  
  if (playerCollision() == true)
  {
    createMagnetCoin(DIME);
    smokePuffStd();
    destroyPhysicalStd(Coord2(3, 4), getImage());
    level->enemyDefeated(this);
  }
}

Circle RabbitAngry::attackCircle()
{
  return Circle(getXY() + Point2(33.0 * facingToDir(), -39.0), 16.0);
}

Circle RabbitAngry::collisionCircle()
{
  return Circle(getXY() + Point2(-9.0 * facingToDir(), -46.0), getWidth());
}

void RabbitAngry::callback(ActionEvent* caller)
{
  if (caller == &upMover)
  {
    downMover.reset();
  }
  else if (caller == &downMover)
  {
    if (onScreen() == true) ResourceManager::playRabbitJump();
    facePlayer();
    upMover.reset();
    animator.reset();
  }
}

// =============================== TailSpiny ============================ //

TailSpiny::TailSpiny() : 
  PizzaGO(),
  ActionListener(),

  warper(this, dwarpCache[wormWarp]),
  warpWeight(0.0),
  warpData(0.0, 1.0),

  speedMult(RM::randf(0.8, 1.0)),
  headOffset(0.0),
  warningAlpha(0.0),
  moveHSpeed(0.0),

  moveState(SEEK_PLAYER),

  vertMover(&headOffset, 0.0, 0.5 * (1.0 / speedMult), this),
  waitTimer(0.75 * (1.0 / speedMult), this)
{
  basePoints = 85;
  setWH(16.0, 16.0);
  setImage(imgCache[wormImg]);
  shadowImg = imgCache[wormShadowImg];
}

void TailSpiny::load()
{
  sortY = getY();
}

void TailSpiny::updateMe()
{  
  updateMovement();
  updateCollisions();
  warpWeight += 0.1 * RM::timePassed();
  RM::bounce_linear(warpWeight, warpData, Point2(0.0, 1.0), 2.0 * speedMult * RM::timePassed());
}

void TailSpiny::redrawMe()
{
  addSortDraw();
}

void TailSpiny::drawSorted()
{
  imgCache[warningH]->draw_scale(getXY(), Point2(1.0, 1.0), HALF_PI, 
                                 ColorP4(1.0, 1.0, 1.0, warningAlpha));
  
  RMGraphics->clip_world(Box::from_norm(getXY(), Point2(64.0, 196.0), Point2(0.5, 1.0)));
  
  addY(headOffset);
  warper.draw(warpWeight);
  addY(-headOffset);
  
  RMGraphics->unclip();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle().draw_outline(RED_SOLID);
  }
}

void TailSpiny::updateMovement()
{
  switch (moveState) 
  {
    default: cout << "TailSpiny::moveState is invalid" << endl;
    case SEEK_PLAYER:
      seekPlayer(true);
      
      if (std::abs(player->getX() - getX()) < 96.0 * speedMult)
      {
        waitTimer.reset(0.15 * (1.0 / speedMult));
        moveState = WAIT_TO_UP;
      }
      break;
      
    case WAIT_TO_UP:
      seekPlayer(false);
      waitTimer.update();
      break;
      
    case GOING_UP:
      vertMover.update();
      break;
      
    case WAIT_TO_DOWN:
      waitTimer.update();
      break;
      
    case GOING_DOWN:
      vertMover.update();
      break;
  }
  
  Logical shouldWarn = moveState == SEEK_PLAYER || moveState == WAIT_TO_UP;
  RM::flatten_me(warningAlpha, shouldWarn ? 1.0 : 0.0, 4.0 * RM::timePassed());
}

void TailSpiny::seekPlayer(Logical speedingUp)
{
  facePlayer();
  
  moveHSpeed += 512.0 * speedMult * (speedingUp ? 1.0 : -1.0) * RM::timePassed();
  RM::clamp_me(moveHSpeed, 0.0, 196.0);
  addX(moveHSpeed * facingToDir() * RM::timePassed());
}

void TailSpiny::updateCollisions()
{
  Logical playerStanding = player->standingTimer.getActive();
  Logical attackHitPlayer = playerCollision(attackCircle());
  
  if (moveState == WAIT_TO_DOWN || moveState == GOING_DOWN)
  {
    if (playerStanding == true) attackHitPlayer = false;
  }
  
  if (attackHitPlayer == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(attackCircle().xy, this);
    }
    
    hitThisAttack = true;
  }
  else 
  {
    hitThisAttack = false;
  }
  
  if (moveState == WAIT_TO_DOWN && 
      playerCollision() == true && 
      player->standingTimer.getActive() == true)
  {
    smokePuffStd();
    createMagnetCoin(QUARTER);
    
    addY(headOffset);
    destroyPhysicalStd(Coord2(3, 4), getImage());
    addY(-headOffset);
    
    level->enemyDefeated(this);
  }
}

Circle TailSpiny::attackCircle()
{
  return Circle(getXY() + Point2(0.0, 20.0 + headOffset), 16.0);
}

Circle TailSpiny::collisionCircle()
{
  return Circle(getXY() + Point2(0.0, 64.0 + headOffset), 24.0);
}

void TailSpiny::callback(ActionEvent* caller)
{
  moveState++;
  moveState %= NUM_TAIL_STATES;
  
  switch (moveState) 
  {
    default: cout << "TailSpiny::moveState is invalid" << endl;
    case SEEK_PLAYER:
      moveHSpeed = 0.0;
      break;
      
    case WAIT_TO_UP:
      // this won't get called, handled manually
      break;
      
    case GOING_UP:
      ResourceManager::playTailRise();
      level->addAction(new RockSplash(getXY()));
      vertMover.reset(-196.0, 0.35);
      break;
      
    case WAIT_TO_DOWN:
      waitTimer.reset(1.5 * (1.0 / speedMult));
      break;
      
    case GOING_DOWN:
      vertMover.reset(sortY, 0.35 * (1.0 / speedMult));
      break;
  }
}

// =============================== DiskEnemy ============================ //

DiskEnemy::DiskEnemy() :
  PizzaGO(),
  startY(0.0),
  hoverVal(0.0),
  speedMult(RM::randf(0.8, 1.2)),
  hoverData(RM::randf(), RM::randl() ? -1.0 : 1.0),
  globeColorIndex(RM::randi(0, RM::count(SPIN_DISK_COLORS) - 1)),
  playedSound(false),
  animator(this, imgsetCache[spinnerSpikeSet], 0.07 * speedMult)
{
  basePoints = 25;
  flies = true;
  shadowImg = imgCache[shadowSmall];
  
  animator.repeats();
  setWH(17.0, 17.0);
}

void DiskEnemy::load()
{
  startY = getY();
  facePlayer();
}

void DiskEnemy::updateMe()
{
  animator.updateMe();
  addX(facingToDir() * 256.0 * speedMult * RM::timePassed());
  RM::bounce_arcsine(hoverVal, hoverData, Point2(-8.0, 8.0), speedMult * RM::timePassed());
  setY(startY + hoverVal);
  
  if (playedSound == false && onScreen() == true)
  {
    ResourceManager::playSpinnerAppear();
    playedSound = true;
  }
  
  turnAtWalls();
  updateCollisions();
}

void DiskEnemy::redrawMe()
{
  setImage(imgCache[spinnerGlobeImg]);
  setColor(SPIN_DISK_COLORS[globeColorIndex]);
  drawMe();
  
  animator.apply();
  setColor(COLOR_FULL);
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void DiskEnemy::updateCollisions()
{
  Logical attackHitPlayer = Circle::collision(collisionCircle(), player->collisionCircle());
  
  if (hitThisAttack == true && attackHitPlayer == false)
  {
    hitThisAttack = false;
  }
  else if (hitThisAttack == false && attackHitPlayer == true)
  {
    hitThisAttack = true;
    Point1 angle = RM::angle(player->getXY(), getXY());
    
    if (angle > PI * 0.25 && angle < PI * 0.75)
    {
      createMagnetCoin(NICKEL);
      smokePuffStd();
      destroyPhysicalStd(Coord2(2, 2), getImage());
      
      setColor(SPIN_DISK_COLORS[globeColorIndex]);
      breakStd(Coord2(2, 2), imgCache[spinnerGlobeImg]);
      level->enemyDefeated(this);
    }
    else
    {
      // player hit
      player->attacked(getXY(), this);
    }
  }
}

// =============================== EndingDebris ============================ //

EndingDebris::EndingDebris(Coord1 setBoneType) :
  PizzaGO(),
  ActionListener(),
  lifeTimer(0.5, this),
  boneType(setBoneType)
{
  setWH(36.0, 36.0);
  setImage(imgsetCache[endingBoneSet][boneType]);
  type = TYPE_SUMO_BOSS;  // this allows jumping while treated as camera enemy
}

void EndingDebris::load()
{
  ArrayList<Point2> bonePts(4);
  switch (boneType)
  {
    case 0:
      bonePts.add(Point2(22.0, 24.0));
      bonePts.add(Point2(18.0, 64.0));
      bonePts.add(Point2(91.0, 63.0));
      bonePts.add(Point2(75.0, 37.0));
      break;
    case 1:
      bonePts.add(Point2(16.0, 8.0));
      bonePts.add(Point2(40.0, 36.0));
      bonePts.add(Point2(80.0, 32.0));
      break;
    case 2:
      bonePts.add(Point2(10.0, 21.0));
      bonePts.add(Point2(10.0, 52.0));
      bonePts.add(Point2(49.0, 51.0));
      bonePts.add(Point2(39.0, 21.0));
      break;
    case 3:
      bonePts.add(Point2(14.0, 14.0));
      bonePts.add(Point2(10.0, 48.0));
      bonePts.add(Point2(71.0, 48.0));
      bonePts.add(Point2(56.0, 9.0));
      break;
    case 4:
      bonePts.add(Point2(26.0, 6.0));
      bonePts.add(Point2(24.0, 44.0));
      bonePts.add(Point2(74.0, 46.0));
      bonePts.add(Point2(81.0, 16.0));
      break;

    case 5:
      bonePts.add(Point2(6.0, 11.0));
      bonePts.add(Point2(5.0, 31.0));
      bonePts.add(Point2(40.0, 31.0));
      bonePts.add(Point2(32.0, 6.0));
      break;
    case 6:
      bonePts.add(Point2(10.0, 11.0));
      bonePts.add(Point2(4.0, 35.0));
      bonePts.add(Point2(46.0, 36.0));
      bonePts.add(Point2(36.0, 5.0));
      break;
    case 7:
      bonePts.add(Point2(8.0, 5.0));
      bonePts.add(Point2(3.0, 27.0));
      bonePts.add(Point2(32.0, 27.0));
      bonePts.add(Point2(32.0, 5.0));
      break;
    case 8:
      bonePts.add(Point2(9.0, 8.0));
      bonePts.add(Point2(9.0, 41.0));
      bonePts.add(Point2(61.0, 40.0));
      bonePts.add(Point2(41.0, 8.0));
      break;
    case 9:
      bonePts.add(Point2(4.0, 32.0));
      bonePts.add(Point2(58.0, 31.0));
      bonePts.add(Point2(43.0, 3.0));
      break;

    case 10:
      bonePts.add(Point2(11.0, 16.0));
      bonePts.add(Point2(12.0, 32.0));
      bonePts.add(Point2(35.0, 34.0));
      bonePts.add(Point2(37.0, 9.0));
      break;
    case 11:
      bonePts.add(Point2(4.0, 5.0));
      bonePts.add(Point2(4.0, 13.0));
      bonePts.add(Point2(54.0, 10.0));
      bonePts.add(Point2(54.0, 5.0));
      break;
    case 12:
      bonePts.add(Point2(8.0, 4.0));
      bonePts.add(Point2(8.0, 11.0));
      bonePts.add(Point2(48.0, 12.0));
      bonePts.add(Point2(48.0, 3.0));
      break;
    case 13:
      bonePts.add(Point2(8.0, 8.0));
      bonePts.add(Point2(10.0, 19.0));
      bonePts.add(Point2(75.0, 19.0));
      bonePts.add(Point2(76.0, 7.0));
      break;
    case 14:
      bonePts.add(Point2(13.0, 8.0));
      bonePts.add(Point2(5.0, 28.0));
      bonePts.add(Point2(52.0, 27.0));
      bonePts.add(Point2(40.0, 5.0));
      break;
    case 15:
      bonePts.add(Point2(9.0, 44.0));
      bonePts.add(Point2(50.0, 45.0));
      bonePts.add(Point2(39.0, 7.0));
      break;
  }

  init_shape(getXY(), bonePts, true);
  
  crushesPhysical = true;
  crushValue = 100.0;
  
  set_b2d_callback_data(this);
}

void EndingDebris::updateMe()
{
  updateFromPhysical();
  checkPhysicalDestruction();
  
  if (destroyedPhysical == true)
  {
    createMagnetCoin(SlopeCoin::randValueAll());
    level->addAction(new Smokeburst(getXY(), false));
  }
  
  if (playerCollision() == true)
  {
    lifeTimer.update();
  }
}

void EndingDebris::redrawMe()
{
  drawMe();
}

void EndingDebris::callback(ActionEvent* caller)
{
  if (getActive() == true)
  {
    destroyPhysical();
  }
}

// =============================== EndingSkull ============================ //

EndingSkull::EndingSkull() :
  PizzaGO(),
  dmgTimer(0.2),
  damage(0),
  readyToDamage(false)
{
  type = TYPE_SUMO_BOSS;  // this allows jumping while treated as camera enemy
  setImage(imgsetCache[endingSkullSet][0]);
  autoSize(0.5);
}

void EndingSkull::load()
{
  init_circle(getXY(), getWidth() - 32.0, true);
  set_friction(1.0);
  set_b2d_callback_data(this);
  set_collisions(1 << type, 0xffff);
}

void EndingSkull::updateMe()
{
  updateFromPhysical();
  
  dmgTimer.update();
  
  if (readyToDamage == true && dmgTimer.getActive() == false)
  {
    damage++;
    smokePuffStd();
    
    if (damage <= 2)
    {
      FishShatter* shatter = new FishShatter(*this, 4, 4);
      level->addAction(shatter);

      level->addAction(new Smokeburst(getXY(), true));
      
      setImage(imgsetCache[endingSkullSet][0]);
      dmgTimer.reset();
    }
    else
    {
      destroyPhysicalStd(Coord2(5, 5), getImage());
      level->enemyDefeated(this);
    }
  }
  
  readyToDamage = false;
}

void EndingSkull::redrawMe()
{
  drawMe();
}

void EndingSkull::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (pgo == player && normalImpulse > 1000.0)
  {
    readyToDamage = true;
  }
}

// =============================== CircleDebris ============================ //

CircleDebris::CircleDebris(Image* img) :
  PizzaGO(),
  stayTimer(2.0),
  maxTimer(10.0)
{
  type = TYPE_PASSIVE;
  crushesPhysical = true;
  crushValue = 650.0;
  setImage(img);
}

void CircleDebris::load()
{
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << TYPE_PASSIVE, 0xffff);  // type is 0 until it is on the ground for jumping
  fixture->SetUserData(this);
  set_restitution(0.2);
  set_ang_vel(RM::randf(-PI, PI));
}

void CircleDebris::updateMe()
{
  updateFromPhysical();
  checkPhysicalDestruction();
  
  if (lifeState == RM::ACTIVE) 
  {
    // this allows you to jump off it once it hits the ground, but not in the air
    if (type != TYPE_DEBRIS && standingPhysical() == true) 
    {
      set_collisions(0x1 << TYPE_DEBRIS, 0xffff);
      type = TYPE_DEBRIS;
    }
  }

  stayTimer.update();
  maxTimer.update();
  if (stayTimer.getActive() == false && onScreenAABB() == false) setRemove();
  if (maxTimer.getActive() == false && lifeState == RM::ACTIVE) destroyPhysicalStd(Coord2(1, 2), getImage());
}

// =============================== SumoBoss ============================ //

SumoBoss::SumoBoss() :
  PizzaGO(),
  ActionListener(),

  ai(NULL),
  geometry(CIRCLE),

  coinsLeft(10),
  coinStyle(DROP_STD),
  justSlammed(false),
  canSlamTimer(0.35),

  strengthMult(1.0),
  dmgStrMult(1.0),
  isStanding(true),

  touchPlayerTime(0.0),
  sparkTimer(0.1),

  shootTimer(5.0, true, this),
  shockTimer(6.0, true, this),
  hurtTimer(2.0, this),
  ringTimer(0.15, true, this),

  dmgAngleRange(0.0, 0.0),
  blower(false),

  hasWeakRange(false),
  weakAngleRange(0.0, 0.0),

  usesGem(false),
  gemGlowAlpha(0.0),
  gemGlowSpeedMult(1.0),
  gemGlowData(0.0, 1.0),

  warningAlpha(0.0),
  
  warper(this, dwarpCache[sumoWarp]),
  warpWeight(0.0),
  tgtFrame(SumoAI::MAD_FRAME),

  losing(false),
  loseDraw(false),
  losingTimer(3.0, this),
  losingTimer2(3.0, this),
  loseFlameTimer(0.2, this),
  loseSinkSpeed(32.0)
{
  type = TYPE_SUMO_BOSS;
  hflip();
  
  setImage(imgCache[sumoBoss]);
  setWH(144.0, 144.0);
  
  shootTimer.setInactive();
  shockTimer.setInactive();
  hurtTimer.setInactive();
  losingTimer.setInactive();
  losingTimer2.setInactive();
  sparkTimer.setInactive();
}

SumoBoss::~SumoBoss()
{
  delete ai;
}

void SumoBoss::load()
{
  if (geometry == CIRCLE) init_circle(getXY(), getWidth(), true);
  else if (geometry == HEXAGON) init_poly(getXY(), 6, getWidth(), true);
  else init_poly(getXY(), 8, getWidth(), true);
  
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(0.5);
  fixture->SetUserData(this);

  ai = new SumoAI(this);  // done here so that player is set
}

void SumoBoss::updateMe()
{
  if (losing == true)
  {
    updateLose();
  }
  else
  {
    updateStd();
  }  
}

void SumoBoss::redrawMe()
{
  warper.draw_cont(tgtFrame, warpWeight);
  if (usesGem == true) drawGem();
  
  // warnings
  if (shockTimer.getActive() == true && warningAlpha > 0.01)
  {
    imgCache[warningH]->draw_scale(Point2(getX(), collisionCircle().top() - 32.0), 
                                   Point2(1.0, 1.0), PI * 1.5, ColorP4(1.0, 1.0, 1.0, warningAlpha));
    imgCache[warningH]->draw_scale(Point2(getX(), collisionCircle().bottom() + 32.0), 
                                   Point2(1.0, 1.0), PI * 0.5, ColorP4(1.0, 1.0, 1.0, warningAlpha));
  }
  
  if (shootTimer.getActive() == true && warningAlpha > 0.01)
  {
    Point1 angleToPlayer = RM::angle(getXY(), player->getXY());
    imgCache[warningH]->draw_scale(getXY() + RM::pol_to_cart(getWidth() + 32.0, angleToPlayer), 
                                   Point2(1.0, 1.0), angleToPlayer, ColorP4(1.0, 1.0, 1.0, warningAlpha));    
  }
}

void SumoBoss::updateStd()
{
  shootTimer.update();
  shockTimer.update();
  hurtTimer.update();
  canSlamTimer.update();
  
  // warnings
  if (shockTimer.getActive() == true && shockTimer.progress() > 0.5)
  {
    RM::flatten_me(warningAlpha, 1.0, 4.0 * RM::timePassed());
  }
  else if (shootTimer.getActive() == true && shootTimer.progress() > 0.65)
  {
    RM::flatten_me(warningAlpha, 1.0, 4.0 * RM::timePassed());    
  }
  else
  {    
    RM::flatten_me(warningAlpha, 0.0, 4.0 * RM::timePassed());
  }
  
  if (ai->powerTimer.getActive() == true)
  {
    ringTimer.update();
  }
  
  ai->update();
  updateFromPhysical();
  slamResponder();
  updateCollisions();
  updateSparks();
  RM::bounce_linear(gemGlowAlpha, gemGlowData, Point2(0.0, 1.0), gemGlowSpeedMult * RM::timePassed());
  
  // enforce max velocity
  Point2 vel = get_velocity();
  if (std::abs(vel.x) > level->playerTXVel()) 
  {
    set_velocity((Point2(level->playerTXVel() * RM::sign(vel.x), vel.y)));
  }
}

void SumoBoss::updateLose()
{
  losingTimer.update();
  losingTimer2.update();
  loseFlameTimer.update();
  
  RM::flatten_me(warningAlpha, 0.0, 4.0 * RM::timePassed());

  if (losingTimer.getActive() == true)
  {
    loseDraw = RM::randf() < losingTimer.progress();
    
    if (loseDraw == true)
    {
      setColor(RM::color255(255, 32, 0));
      getImage()->overexpose();
    }
    else
    {
      setColor(1.0, 1.0, 1.0);
      getImage()->overexpose(false);    
    }    
  }
  else
  {
    ColorP4 baseColor = RM::color255(255, 32, 0);
    setColor(RM::lerp(baseColor, ColorP4(0.0, 0.0, 0.0, 1.0), losingTimer2.progress()));
  }

  addY(loseSinkSpeed * RM::timePassed());
}

void SumoBoss::drawGem()
{
  VisRectangular::drawImg(this, imgCache[sumoGem]);
  
  Point1 realAlpha = getAlpha();
  setAlpha(gemGlowAlpha);
  VisRectangular::drawImg(this, imgCache[sumoGemGlow]);
  setAlpha(realAlpha);
}

void SumoBoss::tryRoll(Point1 normMagnitude)
{
  Point2 centerOfMass = b2dpt_to_world(body->GetWorldCenter());
  
  Point1 armLength = getWidth() * 0.9 * RM::sign(normMagnitude);
  Point1 torqueMult = 1.0;
  if (isStanding == false) torqueMult = 0.0;
  
  // this is to turn faster
  Point1 turnMult = 1.0;  
  if ((normMagnitude < 0.0 && get_velocity().x > 0.0) ||
      (normMagnitude > 0.0 && get_velocity().x < 0.0)) 
  {
    turnMult *= 2.5; 
  }
  
  Point1 finalRoll = PLAYER_ROLL_FORCE * normMagnitude * 
      turnMult * strengthMult * dmgStrMult;
  Point1 finalTorque = PLAYER_ROLL_TORQUE * torqueMult * 
      std::abs(normMagnitude) * strengthMult * dmgStrMult;
  
  apply_force((Point2(finalRoll, finalTorque)), 
              centerOfMass + Point2(armLength * torqueMult, 0.0));
}

void SumoBoss::bounceUp(Point1 percentOfJump)
{
  Point2 currVel = get_velocity();
  set_velocity(Point2(currVel.x, PLAYER_JUMP_VEL * percentOfJump));
}

void SumoBoss::updateSparks()
{
  Point2 location(0.0, 0.0);
  if (touchingPGO(player, &location) == true)
  {
    touchPlayerTime += RM::timePassed();
    touchPlayerTime = std::min(touchPlayerTime, 1.99);
    Coord1 biggestSpark = RM::clamp((Coord1) floor(touchPlayerTime / 0.5), 
                                    0, imgsetCache[sparkSet].count - 1);
    
    if (sparkTimer.getActive() == false) 
    {
      sparkAt(location, biggestSpark);
      sparkTimer.reset();
    }
  }
  else
  {
    touchPlayerTime = 0.0;
  }
  
  sparkTimer.update();
}

void SumoBoss::sparkAt(Point2 center, Coord1 biggestIndex)
{
  Point1 baseAngle = RM::randf(0.0, TWO_PI);
  Coord1 sparkIndex = RM::randi(0, biggestIndex);
  
  SparkEffect* spark = new SparkEffect(center, baseAngle, (sparkIndex + 1) * 8.0, 
                                       imgsetCache[sparkSet][sparkIndex]);
  level->addAction(spark);
}

void SumoBoss::slamResponder()
{
  if (justSlammed == true && canSlamTimer.getActive() == false)
  {
    if (hasWeakRange == true)
    {
      // has weakpoint
      Point1 localHitAngle = RM::standardize_rads(RM::angle(getXY(), player->getXY()) - getRotation());
      
      if (localHitAngle >= weakAngleRange.x && localHitAngle <= weakAngleRange.y)
      {
        createCoins();
        level->sumoWeakpointHit();
      }
    }
    else
    {
      // no weakpoint
      createCoins();
      
      dmgStrMult -= 0.01;
      dmgStrMult = std::max(0.9, dmgStrMult);
      
      canSlamTimer.reset();
    }
  }
  
  justSlammed = false;
}

void SumoBoss::createCoins()
{
  if (coinsLeft <= 0) 
  {
    level->sumoSlammedEmpty();
    return;
  }
  
  for (Coord1 i = 0; i < coinsLeft; ++i)
  {
    Point1 randMag = RM::randf(0.0, 96.0);
    Point1 randAngle = RM::randf(0.0, TWO_PI);
    
    PhysicalCoin* coin = new PhysicalCoin(RM::pol_to_cart(randMag, randAngle) + getXY(), 
                                          SlopeCoin::randValueAll());
    level->addAlly(coin);
    
    if (coinStyle == DROP_STD)
    {
      coin->set_velocity(RM::pol_to_cart(RM::randf(0.0, 750.0), RM::randf(0.0, TWO_PI)));
    }    
    else
    {
      coin->set_velocity(RM::pol_to_cart(RM::randf(400.0, 750.0), RM::randf(PI * 1.25, PI * 1.75)));
    }
  }
  
  coinsLeft--;
}

void SumoBoss::updateCollisions()
{
  if (dmgAngleRange.x < 0.01 && dmgAngleRange.y < 0.01) return;
  
  Point2 location(0.0, 0.0);
  
  if (touchingPGO(player, &location) == true)
  {
    Point1 angle = RM::standardize_rads(RM::angle(getXY(), location) - getRotation());
    if (angle > dmgAngleRange.x && angle < dmgAngleRange.y) player->attacked(location, this);
  }
}

void SumoBoss::explosionOnscreen(const Circle& explosion)
{
  if (collisionCircle().collision(explosion) == true)
  {
    apply_impulse(RM::ring_edge(explosion.xy, getXY(), 5000.0), explosion.xy);
    hurtTimer.reset();
    ai->faceCheck();
  }
}

void SumoBoss::startLoseSequence(Logical longerLose)
{
  losing = true;
  losingTimer.setActive();
  loseFlameTimer.setActive();
  
  if (longerLose == true)
  {
    losingTimer.reset(5.0);
    loseSinkSpeed *= 0.6;
  }
  
  body->SetActive(false);
}

void SumoBoss::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (player->isSlamming == true && pgo == player && player->getY() < getY())
  {
    justSlammed = true;
  }
}

void SumoBoss::blowPlayer()
{
  Point2 polDistance = RM::cart_to_pol(player->getXY() - getXY());
  Point2 normCart = RM::pol_to_cart(1.0, polDistance.y);
  Point1 magMult = RM::clamp(256.0 / polDistance.x, 0.0, 1.0);
  player->apply_force(normCart * 10000.0 * magMult);
}

Box SumoBoss::collisionBox()
{
  return Box(getXY() - getSize(), getSize() * 2.0);
}

void SumoBoss::callback(ActionEvent* caller)
{
  if (caller == &shootTimer)
  {
    ResourceManager::playSumoLaser();

    SumoBossFlame* flame = new SumoBossFlame(getXY(), RM::angle(getXY(), player->getXY()));
    level->addEnemy(flame);
  }
  else if (caller == &shockTimer)
  {
    ResourceManager::playLightning();
    
    Box laserBox = Box::from_center(getXY(), Point2(16.0, PLAY_H * 3.0));
    if (laserBox.collision(player->collisionBox()) == true)
    {
      player->attacked(getXY(), this);
    }
    
    level->addAction(new SumoShockEffect(this));
  }
  else if (caller == &loseFlameTimer)
  {
    level->addAction(new SumoLoseFlame(getXY()));
    loseFlameTimer.reset(RM::randf(0.15, 0.25));
  }
  else if (caller == &losingTimer)
  {
    losingTimer2.setActive();
    
    setColor(RM::color255(255, 32, 0));
    getImage()->overexpose();
  }
  else if (caller == &ringTimer)
  {
    level->addAction(new SumoRing(this));
  }
}

// =============================== SumoAI ============================ //

SumoAI::SumoAI(SumoBoss* setParent) :
  ActionListener(),
  parent(setParent),
  player(parent->player),

  actionState(IDLE),
  actionTimer(0.5, this),

  baseStrength(1.0),
  strengthTimer(5.0, this),
  strengthRange(0.5, 2.5),  // strengthMult is set to 1.5 * this
 
  powerTimer(2.0, this),
  powerWait(4.0, this),

  gustTimer(0.1, this),
  blowAngleOffset(0.8 * PI),
  blowAngleWidth(PI * 0.14),

  warpMover(&parent->warpWeight, 1.0, 0.20, this),
  smallWarper(&parent->warpWeight, 0.2, 0.20, this),

  endTolerance(0.1),
  saveChance(4),
  pushChance(3),
  jumpChance(9)
{
  powerTimer.setActive(false);
  powerWait.setActive(false);
  warpMover.setActive(false);
}

void SumoAI::update()
{
  if (parent->level->levelState != PizzaLevel::LEVEL_PLAY) return;
      
  strengthTimer.update();
  actionTimer.update();
  powerTimer.update();
  powerWait.update();
  warpMover.update();
  smallWarper.update();
  updateBlowing();
  
  switch (actionState)
  {
    default:
    case JUMP_UP:
    case IDLE: 
      break;
    
    case JUMP_LEFT:
    case MOVE_LEFT: parent->tryRoll(-1.0); 
      break;
      
    case JUMP_RIGHT:
    case MOVE_RIGHT: parent->tryRoll(1.0); 
      break;
  }
}

Coord1 SumoAI::selectAction()
{
  Point1 percent = arenaPercent();
  Point1 playerPercent = playerArenaPercent();
  
  Coord1 rollAction = IDLE;
  
  Logical randSave = RM::randi(0, saveChance) != 0;
  Logical randPush = RM::randi(0, pushChance) != 0;
  Logical jumpNotRoll = RM::randi(0, jumpChance) == 0;
  
  // Resist falling off left
  if (randSave == true && percent < endTolerance)
  {
    rollAction = MOVE_RIGHT;
  }
  // Resist falling off right
  else if (randSave == true && percent > 1.0 - endTolerance)
  {
    rollAction = MOVE_LEFT;
  }
  // push player right
  else if (randPush == true && percent < playerPercent)
  {
    rollAction = MOVE_RIGHT;
  }
  // push player left
  else if (randPush == true && percent > playerPercent)
  {
    rollAction = MOVE_LEFT;
  }
  
  if (jumpNotRoll == true)
  {
    if (rollAction == MOVE_LEFT) return JUMP_LEFT;
    else if (rollAction == MOVE_RIGHT) return JUMP_RIGHT;
    else return JUMP_UP;
  }
    
  return rollAction;
}

void SumoAI::setStrRange(Point1 low, Point1 high)
{
  strengthRange.set(low, high);
  callback(&strengthTimer);
}

void SumoAI::setAISpeed(Point1 timePerUpdate)
{
  actionTimer.reset(timePerUpdate);
}

Point1 SumoAI::arenaPercent()
{
  Point1 result = RM::lerp_reverse(parent->getX(), 
                                   parent->level->worldBox.left(), 
                                   parent->level->worldBox.right());
  return result;
}

Point1 SumoAI::playerArenaPercent()
{
  Point1 result = RM::lerp_reverse(player->getX(), 
                                   parent->level->worldBox.left(), 
                                   parent->level->worldBox.right());
  return result;
}

void SumoAI::updateBlowing()
{
  if (parent->blower == false) return;
  
  Point1 mouthAngle = blowAngleOffset + parent->getRotation();
  Point1 angleToPlayer = RM::angle(parent->getXY(), player->getXY());
  
  gustTimer.update();
  
  if (RM::angle_distance(mouthAngle, angleToPlayer) < blowAngleWidth)
  {
    parent->blowPlayer();
  }
}

void SumoAI::faceCheck()
{
  Coord1 newTarget = NEUTRAL_FRAME;
  
  // sad near edge
  Logical sad = (arenaPercent() > 0.95) && (playerArenaPercent() < arenaPercent());
  sad = sad || ((arenaPercent() < 0.05) && (playerArenaPercent() > arenaPercent()));
  sad = sad || parent->hurtTimer.getActive();
  
  // mad while pushing hard
  Logical mad = powerTimer.getActive() || (baseStrength > RM::lerp(strengthRange, 0.75));
  
  parent->warper.last_to_cont();
  parent->warpWeight = 0.0;
  
  if (sad == true)
  {
    newTarget = SAD_FRAME;
    // cout << "start sad" << endl;
  }
  else if (mad == true)
  {
    newTarget = MAD_FRAME;
    // cout << "start mad" << endl;
  }
  else
  {
    // cout << "start neutral" << endl;
  }
  
  // didn't change states, do a small random warp
  if (newTarget == parent->tgtFrame)
  {
    parent->tgtFrame += RM::randi(1, 2);
    parent->tgtFrame %= NUM_FRAMES;
    smallWarper.reset(0.2);
  }
  else
  {
    parent->tgtFrame = newTarget;
    warpMover.reset();
  }
}

void SumoAI::callback(ActionEvent* caller)
{
  if (caller == &actionTimer)
  {
    actionState = selectAction();
    actionTimer.reset();
    
    // this is done here so that it only tries once, greatly reduces logic
    if ((actionState == JUMP_LEFT || actionState == JUMP_RIGHT || actionState == JUMP_UP) &&
        parent->standingPhysical() == true)
    {
      parent->bounceUp(1.0);
    }    
  }
  else if (caller == &strengthTimer)
  {
    parent->strengthMult -= baseStrength;
    baseStrength = RM::randf(strengthRange);
    parent->strengthMult += baseStrength;
    
    parent->change_density(baseStrength * 1.5);  // this is so it moves at roughly the same speed
    
    strengthTimer.reset();
  }
  else if (caller == &powerTimer)
  {
    parent->strengthMult -= 2.5;
    powerWait.reset();
  }
  else if (caller == &powerWait)
  {
    ResourceManager::playSumoAngry();
    parent->strengthMult += 2.5;
    powerTimer.reset();
  }
  else if (caller == &warpMover)
  {
    faceCheck();
  }
  else if (caller == &smallWarper)
  {
    // came FROM small warp
    if (parent->warpWeight < 0.01)
    {
      faceCheck();
    }
    else
    {
      smallWarper.reset(0.0);
    }
  }
  else if (caller == &gustTimer)
  {
    Point1 gustAngle = parent->getRotation() + blowAngleOffset + RM::randf(-blowAngleWidth, blowAngleWidth);
    Point2 blowTgt = parent->getXY() + RM::pol_to_cart(158.0 + RM::randf(192.0, 512.0), gustAngle);
    Point1 effectTime = RM::randf(0.3, 0.6);
    
    ImageEffect* gust = new ImageEffect(imgCache[sumoGust]);
    gust->setXScale(RM::randf(0.5, 1.0));
    gust->matchXScale();
    gust->setAlpha(0.0);
    gust->setXY(parent->getXY() + RM::pol_to_cart(158.0, gustAngle));
    gust->setRotation(gustAngle);
    
    gust->addX(new DeAccelFn(&gust->xy.x, blowTgt.x, effectTime));
    gust->addX(new DeAccelFn(&gust->xy.y, blowTgt.y, effectTime));
    
    gust->enqueueX(new LinearFn(&gust->color.w, 1.0, effectTime * 0.2));
    gust->enqueueX(new TimerFn(effectTime * 0.6));
    gust->enqueueX(new LinearFn(&gust->color.w, 0.0, effectTime * 0.2));
    
    parent->level->addAction(gust);
    
    gustTimer.reset(RM::randf(0.04, 0.06));
  }
}

// =============================== SumoKnife ============================ //

SumoKnife::SumoKnife(Point2 origin, Point2 setVel) :
  PizzaGO(),

  stickBehavior(this),

  vel(setVel),
  wasAboveCam(false),
  playedSound(false),

  rotationOffset(0.0),
  rotationData(0.0, 1.0),

  warning(true),
  warningAlpha(0.0)
{
  stickBehavior.setListener(this);

  setXY(origin);
  setImage(imgCache[sumoKnife]);
  
  setRotation(RM::cart_to_pol(vel).y);  
}

void SumoKnife::updateMe()
{
  if (getActive() == true)
  {
    updateActive();
  }
  else if (lifeState == RM::PASSIVE)
  {
    updateOut();
  }
}

void SumoKnife::redrawMe()
{  
  if (getActive() == true && warningAlpha > 0.01)
  {
    Box camBox = level->camBox();
    
    Image* arrowImg = imgCache[warningH];
    Point2 arrowXY = Point2(camBox.right(), getY());
    
    arrowXY.set(getX(), camBox.top());
    arrowImg = imgCache[warningV];
    
    arrowImg->draw_scale(arrowXY,
                         Point2(1.0, 1.0) / level->camera.zoom, 0.0, ColorP4(1.0, 1.0, 1.0, warningAlpha));
  }
  
  addRotation(rotationOffset);
  drawMe();
  addRotation(-rotationOffset);
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void SumoKnife::updateActive()
{
  Point1 oldY = getY();
  addXY(vel * RM::timePassed());
  updateCollisions();
  
  RM::flatten_me(warningAlpha, shouldWarn() ? 1.0 : 0.0, 4.0 * RM::timePassed());
  
  if (playedSound == false &&
      wasAboveCam == true && getY() >= level->camBox().top())
  {
    ResourceManager::playSumoKnife();
    playedSound = true;
  }
  
  wasAboveCam = getY() < level->camBox().top();
  
  // remove if offscreen in direction of flight
  if (vel.y > 0.0 && getY() > level->worldBox.bottom() + 128.0)
  {
    setRemove();
  }
  
  RM::bounce_arcsine(rotationOffset, rotationData, 
                  Point2(-PI * 0.02, PI * 0.02), RM::timePassed() * 4.0);   
}

void SumoKnife::updateOut()
{
  stickBehavior.update();
  setAlpha(1.0 - RM::clamp((stickBehavior.percentDone() - 0.5) / 0.5, 0.0, 1.0));
}

Logical SumoKnife::shouldWarn()
{
  Box camBox = level->camBox();
  
  Logical leftScreen = vel.y > 0.0 && getY() < camBox.top();
  
  return warning == true && leftScreen == true;
}

void SumoKnife::updateCollisions()
{
  if (lifeState == RM::ACTIVE &&
      playerCollision() == true)
  {
    player->attacked(getXY(), this);
    
    stickBehavior.reset(player, 1.5);
    setPassive();
  }
}

Circle SumoKnife::collisionCircle()
{
  return Circle(getXY(), 7.0);
}

void SumoKnife::callback(ActionEvent* caller)
{
  if (caller == &stickBehavior)
  {
    setRemove();
  }
}

// =============================== SumoBossFlame ============================ //

SumoBossFlame::SumoBossFlame(Point2 start, Point1 angle) :
  PizzaGO(),
  ActionListener(),
  fader(&color.w, 1.0, 0.25, this),
  speed(300.0)
{
  setXY(start);
  setWH(128.0, 12.0);
  setImage(imgCache[alienLaser]);
  setRotation(angle);
  setAlpha(0.0);
}

void SumoBossFlame::updateMe()
{
  fader.update();
  addXY(RM::pol_to_cart(speed, getRotation()) * RM::timePassed());
  
  if (onScreenAABB() == false)
  {
    setRemove();
  }
  else if (lifeState == RM::ACTIVE && playerCollision(Circle(getXY(), 4.0)) == true)
  {
    player->attacked(getXY(), this);
    lifeState = RM::TRANS_OUT;
    fader.reset(0.0);
  }
}

void SumoBossFlame::redrawMe()
{
  drawMe();
}

void SumoBossFlame::callback(ActionEvent* caller)
{
  if (lifeState == RM::TRANS_OUT)
  {
    setRemove();
  }
}

// =============================== SumoShockEffect ============================ //

SumoShockEffect::SumoShockEffect(PizzaGO* setBoss) :
  VisRectangular(),
  Effect(),
  ActionListener(),
  boss(setBoss),
  levelCam(&boss->level->camera),
  fader(&color.w, 0.0, 0.4, this)
{
  setImage(imgCache[shockBolt]);
  setXY(boss->getX(), levelCam->myBox().top() - 64.0);
  setWH(getImage()->natural_size().x, levelCam->myBox().wh.y + 128.0);
}

void SumoShockEffect::updateMe()
{
  fader.update();
  setX(boss->getX());
}

void SumoShockEffect::redrawMe()
{
  drawMeRepeating();
}

void SumoShockEffect::callback(ActionEvent* caller)
{
  done();
}

// =============================== SumoLoseFlame ============================ //

const Point1 SumoLoseFlame::SPEED = 150.0;

SumoLoseFlame::SumoLoseFlame(Point2 center) :
  VisRectangular(),
  Effect(),
  ActionListener(),

  scaler(&scale.x, 2.0 + RM::randf(0.0, 1.0), 2.0 + RM::randf(0.0, 1.0)),
  fader(&color.w, 0.0, 2.0, this),
  speedMult(RM::randf(0.45, 1.25))
{
  setRotation(RM::randf(PI * 1.1, PI * 1.9));
  setXY(center + RM::pol_to_cart(RM::randf(48.0, 96.0), getRotation()));
  setScale(0.5, 0.5);
  setImage(imgCache[sumoFireball]);
}

void SumoLoseFlame::updateMe()
{
  addXY(RM::pol_to_cart(SPEED * speedMult * RM::timePassed(), getRotation()));
  scaler.update();
  fader.update();
  setYScale(getXScale());
}

void SumoLoseFlame::redrawMe()
{
  drawMe();
}

void SumoLoseFlame::callback(ActionEvent* caller)
{
  if (caller == &fader)
  {
    done();
  }
}

// =============================== SumoRing ============================ //

SumoRing::SumoRing(VisRectangular* setParent) :
  VisRectangular(setParent->getXY(), imgCache[sumoMadRing]),
  Effect(),
  ActionListener(),
  parent(setParent),
  bossOffset(RM::pol_to_cart(RM::randf(0.0, 96.0), RM::rand_angle())),
  scaler(&scale.x, 2.0, 1.5, this)
{
  setScale(0.5, 0.5);
  setAlpha(0.0);
}

void SumoRing::updateMe()
{
  bossOffset += -64.0 * RM::timePassed();
  setXY(parent->getXY() + bossOffset);
  
  scaler.update();
  matchXScale();
  
  if (scaler.progress() < 0.5) setAlpha(scaler.progress() * 2.0);
  else setAlpha(1.0 - (scaler.progress() - 0.5) / 0.5);
}

void SumoRing::redrawMe()
{
  drawMe();
}

void SumoRing::callback(ActionEvent* caller)
{
  done();
}

// =============================== SumoBridge ============================ //

SumoBridge::SumoBridge() : 
  PizzaGO(),

  links(10, true),
  joints(9, true),

  leftAnchor(),
  rightAnchor()
{
  type = TYPE_TERRAIN;
  setWH(960.0, 32.0);
}

SumoBridge::~SumoBridge()
{
  for (Coord1 i = 0; i < joints.count; ++i)
  {
    RMPhysics->DestroyJoint(joints[i]);
  }
}

void SumoBridge::load()
{
  Point2 linkSize(getWidth() / links.count, getHeight());
  
  for (Coord1 i = 0; i < links.count; ++i)
  {
    Point2 center = getXY() + Point2((i + 0.5) * linkSize.x, 0.0);
    links[i].setBox(center, linkSize);
    links[i].setImage(imgsetCache[sumoTerrainSet].last());
    
    links[i].init_box(center, linkSize, true);
    links[i].set_collisions(0x1 << TYPE_TERRAIN, 0xffff);
    links[i].fixture->SetUserData(this);
    links[i].updateFromPhysical();
  }  
  
  leftAnchor.init(links.first().body, links.first().xy - Point2(linkSize.x * 0.5, 0.0));
  rightAnchor.init(links.last().body, links.last().xy + Point2(linkSize.x * 0.5, 0.0));
  
  for (Coord1 i = 0; i < joints.count; ++i)
  {
    joints[i] = Physical::join_revolute(links[i].body, links[i + 1].body, 
                                        links[i].xy + Point2(linkSize.x * 0.5, 0.0));
  }
}

void SumoBridge::updateMe()
{
  for (Coord1 i = 0; i < links.count; ++i)
  {
    links[i].updateFromPhysical();
  }
}

void SumoBridge::redrawMe()
{
  for (Coord1 i = 0; i < links.count; ++i)
  {
    links[i].drawMe();
  }
}

// =============================== SmallSkull ============================ //

SmallSkull::SmallSkull(Point2 center) :
  PizzaGO(),
  warper(),
  blinkScript(this),
  blinkWeight(0.0)
{
  basePoints = 50;
  type = TYPE_SKELETON;
  value = RM::randi(0, 8);
  
  setXY(center);
  setWH(24.0, 24.0);
  setImage(imgsetCache[angrySkullSet][value]);
  warper.set(this, ResourceManager::angrySkullWarps[value]);
  
  init_circle(center, getWidth(), true);
  set_b2d_callback_data(this);
  set_restitution(0.0);
  set_collisions(0x1 << PizzaGO::TYPE_SKELETON, 0xffff);
  
  setRotation(RM::randf(-PI / 6.0, PI / 6.0));
  updateFromWorld();
  
  crushesPhysical = true;
  crushValue = 650.0;
  
  callback(&blinkScript);
}

void SmallSkull::load()
{
  level->totalDestructibles++;
}

void SmallSkull::updateMe()
{
  blinkScript.update();
  updateFromPhysical();
  checkPhysicalDestruction();
  
  if (destroyedPhysical == true) 
  {
    createMagnetCoin(NICKEL);
    smokePuffStd();

    level->skullsCrushed++;
    level->enemyDefeated(this);
    level->gotScore(basePoints);
    
    /*
    String1 str("$/@", '$', level->skullsCrushed);
    str.int_replace('@', level->startingSkulls);
    BonusText* textEffect = new BonusText(str.as_cstr(), ResourceManager::font, 
                                          getXY(), false);
    textEffect->upwardVal = -96.0;
    textEffect->setScale(0.75, 0.75);
    textEffect->holdTime = 0.75;
    textEffect->init();
    level->addAction(textEffect);
     */
  }
}

void SmallSkull::redrawMe()
{
  warper.draw(blinkWeight);
}

Box SmallSkull::collisionBox()
{
  return Box(getXY() - getSize(), getSize() * 2.0);
}

void SmallSkull::callback(ActionEvent* caller)
{
  if (caller == &blinkScript)
  {
    blinkScript.wait(RM::randf(0.5, 1.0));
    blinkScript.enqueueX(new LinearFn(&blinkWeight, 1.0, 0.2));
    blinkScript.enqueueX(new LinearFn(&blinkWeight, 0.0, 0.2));
  }
}

// =============================== SlopeFlag ============================ //

SlopeFlag::SlopeFlag(Point2 setBottom, Logical setBig) :
  PizzaGO(),
  big(setBig),
  animator()
{
  basePoints = big ? 100 : 10;
  
  DataList<Image*>& imgs = big ? imgsetCache[bigFlagSet] : imgsetCache[flagSet];
  animator.init(this, DataList<Image*>(imgs, 0, 3), 0.12);
  animator.repeats();
  
  setXY(setBottom);
  autoSize(0.5);
}

void SlopeFlag::updateMe()
{
  if (getActive() == false) return;
  
  animator.update();
  
  if (playerCollision() == true)
  {
    setImage((big ? imgsetCache[bigFlagSet] : imgsetCache[flagSet]).last());
    
    ResourceManager::playFlagGet();
    
    RingEffect* ring = new RingEffect(getXY());
    ring->endScale = big ? 2.0 : 1.0;
    ring->init();
    level->addAction(ring);
    
    level->gotScore(basePoints);
    level->addAction(new FlagStars(getXY(), big));
    
    setPassive();
  }
}

void SlopeFlag::redrawMe()
{
  drawMe();
}

Box SlopeFlag::collisionBox()
{
  return boxFromBC();
}

// =============================== SlopeCoin ============================ //

SlopeCoin::SlopeCoin(Point2 center, Coord1 setValue) :
  PizzaGO(),
  animator()
{
  value = setValue;
  setXY(center);
}

void SlopeCoin::load()
{
  setAnimator(this, animator, value, level->getCoinFrame());
}

void SlopeCoin::updateMe()
{
  animator.update();
  
  if (playerCollision() == true)
  {
    gotCoin();
  }
}

void SlopeCoin::redrawMe()
{
  drawMe();
}

void SlopeCoin::gotCoin()
{
  level->gotCoin(getXY(), toCash(value));
  setRemove();
}

Coord1 SlopeCoin::toCash(Coord1 coinVal)
{
  switch (coinVal) 
  {
    default:
    case PENNY: return 1;
    case NICKEL: return 5;
    case DIME: return 10;
    case QUARTER: return 25;
  }
}

void SlopeCoin::setAnimator(VisRectangular* parent, AnimationBehavior& memberAnimator,
                            Coord1 coinVal, Coord1 coinFrame)
{
  DataList<Image*>* imgs = NULL;
  
  switch (coinVal)
  {
    case PENNY: 
      imgs = &imgsetCache[pennySet];
      parent->setWH(16.0, 16.0);
      break;
    case NICKEL:
      imgs = &imgsetCache[nickelSet];
      parent->setWH(18.0, 18.0);
      break;
    case DIME:
      imgs = &imgsetCache[dimeSet];
      parent->setWH(21.0, 21.0);
      break;
    case QUARTER:
      imgs = &imgsetCache[quarterSet];
      parent->setWH(22.0, 22.0);
      break;
  }
  
  memberAnimator.init(parent, *imgs, 0.06);
  memberAnimator.setFrame(coinFrame);
  memberAnimator.repeats();
}

Coord1 SlopeCoin::randValueAll(Coord1 bestCoin)
{
  Coord1 topRand = 24;
  if (bestCoin == NICKEL) topRand = 29;
  else if (bestCoin == DIME) topRand = 31;
  else if (bestCoin == QUARTER) topRand = 32;
  
  Coord1 randCoin = RM::randi(0, topRand);
  
  if (randCoin <= 24)
  {
    return PENNY;
  }
  else if (randCoin <= 29)
  {
    return NICKEL;
  }
  else if (randCoin <= 31)
  {
    return DIME;
  }
  else
  {
    return QUARTER;
  }
}

Coord1 SlopeCoin::randValueEven(Coord1 bestCoin)
{
  return RM::randi(PENNY, bestCoin);
}

// =============================== MagnetCoin ============================ //

MagnetCoin::MagnetCoin(Point2 center, Coord1 setValue) :
  SlopeCoin(center, setValue),

  progressTimer(RM::randf(0.75, 1.25), this),

  startXY(0.0, 0.0),
  startDist(0.0),
  tgtDist(RM::randf(16.0, 96.0)),

  angleToPlayer(0.0),
  rotSpeed(RM::randf(PI * 0.4, PI * 0.6))
{
  rotSpeed *= RM::randl() ? 1.0 : -1.0;
}

void MagnetCoin::load()
{
  SlopeCoin::load();
  startXY = getXY();
  startDist = RM::distance_to(startXY, player->getXY());
  angleToPlayer = RM::angle(player->getXY(), getXY());
}

void MagnetCoin::updateMe()
{
  animator.update();

  // double update if player is about to win
  if (level->levelState == PizzaLevel::LEVEL_WIN) updateMotions();
  updateMotions();
}

void MagnetCoin::updateMotions()
{
  progressTimer.update();
  
  Point1 newDistance = RM::lerp(startDist, tgtDist, progressTimer.progress());
  angleToPlayer = angleToPlayer + rotSpeed * RM::timePassed();
  setXY(RM::pol_to_cart(newDistance, angleToPlayer) + player->getXY());
}

void MagnetCoin::callback(ActionEvent* caller)
{
  if (caller == &progressTimer)
  {
    gotCoin();
  }
}

// =============================== PhysicalCoin ============================ //

PhysicalCoin::PhysicalCoin(Point2 center, Coord1 setValue) :
  SlopeCoin(center, setValue),
  removeOutsideWorld(true),
  waiter(10.0, this),
  fader(&color.w, 0.0, 0.5, this)
{
  fader.setInactive();
}

void PhysicalCoin::load()
{
  SlopeCoin::load();

  init_circle(getXY(), getWidth() * 0.5, true);
  set_collisions(0x1 << TYPE_PASSIVE, 0x1 << TYPE_TERRAIN);
  set_restitution(0.90);
  set_b2d_callback_data(this);
}

void PhysicalCoin::updateMe()
{
  animator.update();
  updateFromPhysical();
  setRotation(0.0);
  
  if (playerCollision() == true)
  {
    gotCoin();
  }
  else if (removeOutsideWorld == true &&
           collisionCircle().top() > level->worldBox.bottom())
  {
    setRemove();
  }
  
  waiter.update();
  fader.update();
}

void PhysicalCoin::callback(ActionEvent* caller)
{
  if (caller == &waiter)
  {
    fader.setActive();
  }
  else if (caller == &fader)
  {
    setRemove();
  }
}

// =============================== UnlockFloater ============================ //

UnlockFloater::UnlockFloater(Coord2 setToppingID) :
  PizzaGO(),

  toppingID(setToppingID),

  glowRotation(0.0),
  bobOffset(0.0),
  bobData(0.0, 1.0),
  
  startedMagnet(false),
  startXY(0.0, 0.0),
  currDistPercent(0.0)
{
  setImage(imgCache[secretOutfit]);
  setWH(16.0, 16.0);
}

void UnlockFloater::updateMe()
{
  glowRotation += TWO_PI * 0.1 * RM::timePassed();
  if (startedMagnet == false) updateFloater();
  else updateMagnet();
}

void UnlockFloater::redrawMe()
{
  setImage(imgCache[secretOutfitGlow]);
  setRotation(glowRotation);
  drawMe();
  
  setImage(imgCache[secretOutfit]);
  setRotation(0.0);
  drawMe();
}

void UnlockFloater::updateFloater()
{
  addY(-bobOffset);
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 
                  0.5 * RM::timePassed());
  addY(bobOffset);
  
  if (playerCollision() == true)
  {
    startMagnet();
  }
}

void UnlockFloater::updateMagnet()
{
  Point1 speedMult = level->levelState == PizzaLevel::LEVEL_PLAY ? 1.0 : 2.0;
  Logical atPlayer = RM::flatten_me(currDistPercent, 1.0, speedMult * RM::timePassed());
  Point1 totalDist = RM::distance_to(startXY, player->getXY());
  
  setXY(RM::attract(startXY, player->getXY(), totalDist * currDistPercent));
  
  if (atPlayer == true)
  {
    ResourceManager::playOutfitFound();
    Pizza::currIngredientFound = toppingID;
    level->addAction(new FlagStars(getXY(), false));
    setRemove();
  }
}

void UnlockFloater::startMagnet()
{
  startedMagnet = true;
  startXY = getXY();
}

// =============================== SnowboardSkele ============================ //

SnowboardSkele::SnowboardSkele(Point2 start, Point2 end) :
  PizzaGO(),
  animator(this, imgsetCache[snowboardSet], 0.06),
  xMover(&xy.x, end.x, 1.0),
  yMover(&xy.y, end.y, 1.0)
{
  animator.repeats();

  setBox(start, getImage()->natural_size());
  setRotation(RM::angle(start, end));
  
  Point1 time = RM::distance_to(start, end) / 200.0;
  xMover.reset(end.x, time);
  yMover.reset(end.y, time);
}

void SnowboardSkele::updateMe()
{
  if (onScreenFixed() == true)
  {
    animator.update();
    xMover.update();
    yMover.update();
  }
  
  if (playerCollision() == true)
  {
    destroyPhysicalStd(Coord2(2, 3), getImage());
    level->addAction(new SnowflakeBurst(collisionCircle().xy, false));
    level->gotScore(10);
  }
}

void SnowboardSkele::redrawMe()
{
  if (onScreenFixed() == true) drawMe();
  
  if (Pizza::DEBUG_COLLISION == true) 
  {
    collisionCircle().draw_outline(COLOR_FULL);
  }
}

Circle SnowboardSkele::collisionCircle()
{
  return Circle(getXY() - Point2(0.0, 96.0), 16.0);
}

// =============================== SkiFlameSkull ============================ //

SkiFlameSkull::SkiFlameSkull() :
  FlameSkull()
{
  
}

void SkiFlameSkull::load()
{
  facePlayer();
}

void SkiFlameSkull::updateMe()
{
  updateGlow();
  updateCollisions();
}

void SkiFlameSkull::updateCollisions()
{
  Logical destroy = false;

  if (playerCollision() == true)
  {
    player->attacked(getXY(), this);
    destroy = true;
  }
  
  if (getX() < player->getX() && getHFlip() == true) destroy = true;
  else if (getX() > player->getX() && getHFlip() == false) destroy = true;
  
  if (destroy == true)
  {
    createMagnetCoin(NICKEL);
    level->gotScore(10);
    destroySkull();
  }
}

// =============================== Snowman ============================ //

Snowman::Snowman() :
  PizzaGO(),
  scarf(),
  scarfAnim(&scarf, imgsetCache[scarfSet], 0.06)
{
  scarfAnim.repeats();
  
  setImage(imgCache[slopeSnowman]);
  autoSize(0.5);
}

void Snowman::updateMe()
{
  scarf.setXY(getXY() + Point2(11.0, -135.0));
  scarfAnim.update();
  
  if (playerCollision() == true)
  {
    level->gotScore(250);
    destroyPhysicalStd(Coord2(3, 4), getImage());
    level->addAction(new SnowflakeBurst(collisionCircle().xy, true));
    level->winLevel();
  }
}

void Snowman::redrawMe()
{
  drawMe();
  scarf.drawMe();
  // collisionCircle().draw_outline(BLUE_SOLID);
}

Circle Snowman::collisionCircle()
{
  return Circle(getXY() - Point2(0.0, 128.0), getWidth());
}


// =============================== KarateBrain ============================ //

KarateBrain::KarateBrain() :
  PizzaGO(),

  mover(NULL),
  scaleScript(),
  illusionTimer(0.5, true, this),
  oldXY(0.0, 0.0),

  innerRadius(32.0),
  bounceTimer(0.3, this),
  firstTouch(true),
  mummyBounce(false)
{
  illusionTimer.setActive(false);  // set true to turn them on
  setWH(50.0, 50.0);
  setImage(imgCache[brainStill]);
}

KarateBrain::~KarateBrain()
{
  delete mover;
}

void KarateBrain::load()
{
  oldXY = getXY();
  facePlayer();
  
  init_circle(getXY(), innerRadius, false);
  set_body_type(b2_kinematicBody);
  set_collisions(0x1 << TYPE_TERRAIN, 0xff);
  set_restitution(0.0);
  set_friction(1.0);
  set_b2d_callback_data(this);
}

void KarateBrain::updateMe()
{
  updateFromPhysical();
  oldXY = getXY();
  Logical playerTouchPhysical = touchingPGO(player);
  
  if (playerCollision() == false) mover->moveBrain();
  if (getX() - oldXY.x > 0.1) faceDir(1.0);
  else if (getX() - oldXY.x < -0.1) faceDir(-1.0);
  
  if (playerTouchPhysical == true && bounceTimer.getActive() == false)
  {    
    if (firstTouch == true)
    {
      level->landedOnEnemy(this);
    }
    else
    {
      level->resetBounce();  // this just resets bounce count
    }

    Point1 finalBounceMult = mummyBounce ? 1.25 : level->bounceMult();
    Point1 finalSpeed = finalBounceMult * std::abs(PLAYER_JUMP_VEL);
    
    player->set_velocity(Point2(player->get_velocity().x, -finalSpeed));
    
    ResourceManager::playBounceSound();
    player->nojumpTimer.reset();  // prevents "double" jump
    player->isSlamming = false;
    bounceTimer.reset();
    firstTouch = false;
  }
  
  velFromWorld(oldXY);
  mover->animateBrain();
  
  if (playerCollision() == true)
  {
    Point1 tgtRad = RM::distance_to(getXY(), player->getXY()) - player->getWidth();
    Point1 newScale = tgtRad / getWidth();
    setScale(newScale, newScale);
    
    if (scaleScript.length() == 0)
    {
      scaleScript.enqueueX(new DeAccelFn(&scale.x, 1.25, 0.1));
      scaleScript.enqueueX(new AccelFn(&scale.x, 1.0, 0.2));
    }
  }
  else
  {
    scaleScript.update();
  }
  
  scale.y = scale.x;
  
  bounceTimer.update();
  if (onScreen() == true) illusionTimer.update();
}

void KarateBrain::redrawMe()
{  
  drawMe(Point2(0.0, 46.0));
}

void KarateBrain::enableIllusions(Point1 speed)
{
  illusionTimer.reset(speed);
}

void KarateBrain::setMummy()
{
  mummyBounce = true;
  setImage(imgCache[brainLead]);
}

void KarateBrain::callback(ActionEvent* caller)
{
  if (caller == &illusionTimer)
  {
    level->addAction(new KarateIllusion(*this));
  }
}

// =============================== BrainMover ============================ //

BrainMover::BrainMover(KarateBrain* setParent) :
  parent(setParent)
{
  parent->setImage(imgCache[brainStill]);
}

// =============================== BrainMoverWalk ============================ //

BrainMoverWalk::BrainMoverWalk(KarateBrain* setParent, Point2 setStart, Point2 setTarget) :
  BrainMover(setParent),

  actions(),
  script(this),

  start(setStart),
  target(setTarget),
  linearSpeed(125.0),
  
  walks(true),
  flyAnim()
{  
  updateWalkFrame();
  callback(&script);
}

void BrainMoverWalk::moveBrain()
{
  actions.update();
  script.update();
}

void BrainMoverWalk::setFlying()
{
  walks = false;
  
  DataList<Image*> pingpong;
  Image::create_pingpong(imgsetCache[brainFly], pingpong);
  flyAnim.init(parent, pingpong, 0.1);
  flyAnim.repeats();
}

void BrainMoverWalk::animateBrain()
{
  if (walks == true)
  {
    updateWalkFrame();
  }
  else
  {
    updateFlyFrame();
  }
}

void BrainMoverWalk::updateWalkFrame()
{
  Coord1 frameNum = RM::segment_distance(start.x, parent->getX(), 16.0);
  frameNum = std::abs(frameNum) % imgsetCache[brainWalk].count;

  if (parent->getHFlip() == false) 
  {
    parent->setImage(imgsetCache[brainWalk][frameNum]);
  }
  else
  {
    frameNum = imgsetCache[brainWalk].count - 1 - frameNum;
    parent->setImage(imgsetCache[brainWalk][frameNum]);
  }
}

void BrainMoverWalk::updateFlyFrame()
{
  flyAnim.update();
}

void BrainMoverWalk::callback(ActionEvent* caller)
{
  Point1 horiTime = std::abs(target.x - parent->xy.x) / linearSpeed;
  Point1 vertTime = std::abs(target.y - parent->xy.y) / linearSpeed;
  Point1 maxTime = std::max(horiTime, vertTime);
  
  script.enqueueAddX(new LinearFn(&parent->xy.x, target.x, maxTime), &actions);
  script.enqueueAddX(new LinearFn(&parent->xy.y, target.y, maxTime), &actions);
  script.wait(maxTime + 1.0);
  script.enqueueAddX(new LinearFn(&parent->xy.x, start.x, maxTime), &actions);
  script.enqueueAddX(new LinearFn(&parent->xy.y, start.y, maxTime), &actions);
  script.wait(maxTime + 1.0);
}

// =============================== BrainMoverHop ============================ //

BrainMoverHop::BrainMoverHop(KarateBrain* setParent, Point2 setXBounds) :
  BrainMover(setParent),
  ActionListener(),
  xBounds(setXBounds),
  moveDir(1),
  linearSpeed(75.0),
  sideMove(&parent->xy.x, xBounds.x, 1.0, this),
  upMove(&parent->xy.y, parent->getY() - 128.0, 0.5, this),
  downMove(&parent->xy.y, parent->getY(), 0.5, this)
{
  parent->setImage(imgCache[brainStill]);

  downMove.setActive(false);
  
  Point1 horiTime = std::abs(xBounds.x - xBounds.y) / linearSpeed;
  horiTime = std::max(horiTime, 1.0);
  
  sideMove.reset_time(horiTime);
  upMove.reset_time(horiTime * 0.5);
  downMove.reset_time(horiTime * 0.5);
}

void BrainMoverHop::moveBrain()
{
  upMove.update();
  downMove.update();
  sideMove.update();
}

void BrainMoverHop::animateBrain()
{
  if (std::abs(parent->get_velocity().y) > 200.0) parent->setImage(imgsetCache[brainBounce][1]);
  else parent->setImage(imgsetCache[brainBounce][0]); 
}

void BrainMoverHop::callback(ActionEvent* caller)
{
  if (caller == &upMove)
  {
    downMove.reset();
  }
  else if (caller == &downMove)
  {
    upMove.reset();
    moveDir *= -1;
    sideMove.reset(moveDir == 1 ? xBounds.x : xBounds.y);    
  }
}

// =============================== BrainMOverCircle ============================ //

BrainMoverCircle::BrainMoverCircle(KarateBrain* setParent, const Circle& circle) :
  BrainMover(setParent),
  animator(),
  moveCircle(circle),
  currAngle(RM::randf(0.0, TWO_PI))
{
  DataList<Image*> pingpong;
  Image::create_pingpong(imgsetCache[brainCircle], pingpong);
  animator.init(parent, pingpong, 0.1);
  animator.repeats();
}

void BrainMoverCircle::moveBrain()
{
  currAngle += TWO_PI * 0.25 * RM::timePassed();
  RM::wrap1_me(currAngle, 0.0, TWO_PI);
  parent->setXY(RM::pol_to_cart(moveCircle.radius, currAngle) + moveCircle.xy);
}

void BrainMoverCircle::animateBrain()
{
  animator.update();  
}

// =============================== KarateSpear ============================ //

KarateSpear::KarateSpear() :
  PizzaGO(),
  neighbor(NULL),
  yMover(&xy.y, 0.0, 0.4)
{
  setWH(8.0, 8.0);
  setImage(imgCache[karateSpear]);
  yMover.setInactive();
}

void KarateSpear::load()
{
  
}

void KarateSpear::updateMe()
{
  Logical collided = playerCollision();
  yMover.update();
  
  if (lifeState == RM::ACTIVE && collided == true && hitThisAttack == false)
  {
    player->attacked(getXY(), this);
    
    retract();
    if (neighbor != NULL) neighbor->retract();
  }
  
  hitThisAttack = collided;
}

void KarateSpear::retract()
{
  if (lifeState == RM::PASSIVE) return;
  
  lifeState = RM::PASSIVE;
  yMover.reset(getY() + 512.0);
}

void KarateSpear::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

// =============================== KarateIllusion ============================ //

KarateIllusion::KarateIllusion(const VisRectangular& parent) :
  ImageEffect()
{
  setXY(parent.getXY());
  setImage(parent.getImage());
  setScale(parent.getScale());
  hflip(parent.getHFlip());
  
  Point1 partTime = 0.5;
  addX(new LinearFn(&xy.x, getX() + RM::randf(-128.0, 128.0), partTime));
  addX(new LinearFn(&xy.y, getY() + RM::randf(-128.0, 128.0), partTime));
  addX(new LinearFn(&color.x, 0.5, partTime));
  addX(new LinearFn(&color.y, 0.5, partTime));
  
  script.wait(partTime);
  script.enqueueX(new LinearFn(&color.w, 0.0, 0.5));
}

// =============================== JumpSensor ============================ //

JumpSensor::JumpSensor(const Circle& circle) :
  PizzaGO(),
  lifeTimer(0.1, false, this)
{
  setXY(circle.xy);
  setWH(circle.radius, circle.radius);
}

void JumpSensor::load()
{
  init_circle(getXY(), getWidth(), false);
  fixture->SetSensor(true);
  fixture->SetUserData(this);
  set_collisions(0xff, 0x1 << TYPE_PLAYER);
}

void JumpSensor::updateMe()
{
  // lifeTimer.update();
  if (touchingPGO(player) == true)
  {
    
  }
}

void JumpSensor::redrawMe()
{
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    b2WorldManifold mani = contact_to_manifold(edge->contact);
    
    Point2 pt0 = b2dpt_to_world(mani.points[0]);
    Point2 pt1 = b2dpt_to_world(mani.points[1]);
    Point1 contactAngle = RM::angle(getXY(), pt0);
    
    b2Fixture* fixtureA = edge->contact->GetFixtureA();
    b2Fixture* fixtureB = edge->contact->GetFixtureB();
    
    // problem: sumo boss won't jump off player unless we add player,
    // but can't do that without check that player can't use itself as terrain
    // actually this works b/c boss CAN use itself as terrain, player can't
    Coord1 standableBits = (1 << TYPE_TERRAIN) | 
    (1 << TYPE_DEBRIS) | (1 << TYPE_SUMO_BOSS);
    
    Logical leftIsTerrain =
    (fixtureA->GetFilterData().categoryBits & standableBits) != 0;
    Logical rightIsTerrain =
    (fixtureB->GetFilterData().categoryBits & standableBits) != 0;
    Logical eitherTerrain = leftIsTerrain || rightIsTerrain;
        
    if (eitherTerrain == true &&
        edge->contact->IsTouching() == true && 
        edge->contact->IsEnabled() == true) 
    {
      Box b(pt1, Point2(64.0, 64.0));
      b.draw_solid(ColorP4(0.0, 0.0, 1.0, 0.75));
    }
  }
}

void JumpSensor::callback(ActionEvent* caller)
{
  RMPhysics->DestroyBody(body);
  setRemove();
}

// =============================== BalanceSkull ============================ //

BalanceSkull::BalanceSkull() :
  PizzaGO(),
  oldXY(0.0, 0.0),
  octagon(false)
{
  setImage(imgCache[balanceBallCircle]);
  setWH(64.0, 64.0);   // radius
  autoScale(2.0);
}

void BalanceSkull::load()
{
  if (octagon == false)
  {
    init_circle(getXY(), getWidth(), true);
  }
  else
  {
    init_poly(getXY(), 8, getWidth(), true);
    setImage(imgCache[balanceBallOct]);    
  }
  
  set_collisions(1 << TYPE_SUMO_BOSS, 0xffff);
  change_density(5.0);
  set_b2d_callback_data(this);
  oldXY = xy;
}

void BalanceSkull::updateMe()
{
  oldXY = xy;
  updateFromPhysical();
  if (touchingPGO(player) == true) level->balanceBallTouched(this);
  
  // enforce max velocity
  Point2 vel = get_velocity();
  if (vel.x > level->maxSkullXVel()) 
  {
    set_velocity(Point2(level->maxSkullXVel(), vel.y));
  }
  
}

void BalanceSkull::redrawMe()
{
  drawMe();
}

void BalanceSkull::tryRoll(Point1 normMagnitude)
{
  Point2 centerOfMass = b2dpt_to_world(body->GetWorldCenter());
  
  Point1 armLength = getWidth() * 0.9 * RM::sign(normMagnitude);

  apply_force((Point2(PLAYER_ROLL_FORCE * normMagnitude * 3.0, 
                      PLAYER_ROLL_TORQUE * 3.0)), 
                centerOfMass + Point2(armLength, 0.0));

}

Circle BalanceSkull::collisionCircle()
{
  return Circle(getXY() - Point2(0.0, getHeight()), getWidth());
}

// =============================== BalanceGong ============================ //

BalanceGong::BalanceGong() :
  PizzaGO(),
  readyToRing(false),
  rung(false),
  gongRingFrame(0)
{
  
}

void BalanceGong::load()
{
  init_box_TL(getXY() + Point2(64.0, 64.0), Point2(64.0, 128.0), false);
  set_collisions(1 << TYPE_SUMO_BOSS, 1 << TYPE_SUMO_BOSS);
  set_restitution(0.75);
  set_b2d_callback_data(this);
}

void BalanceGong::updateMe()
{
  if (readyToRing == true && rung == false)
  {
    ResourceManager::playGong();
    level->gongHit();
    rung = true;
  }
}

void BalanceGong::redrawMe()
{
  if (rung == false)
  {
    gongRingFrame = 0;
  }
  else
  {
    gongRingFrame++;
    if (gongRingFrame == 3) gongRingFrame = 1;
  }
  
  setImage(imgCache[balanceGongStand]);
  drawMe();
  
  setImage(imgsetCache[balanceGongSet][gongRingFrame]);
  drawMe(Point2(32.0, 64.0));
}

void BalanceGong::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  readyToRing = true;
}

// =============================== BounceFrog ============================ //

BounceFrog::BounceFrog() :
  PizzaGO(),
  ActionListener(),

  warper(),
  warpWeight(0.0),

  explodeTimer(1.0, this),
  blinkScript(),

  big(false),
  heavy(false),
  explodes(false)
{
  type = TYPE_FROG;
  setWH(64.0, 64.0);
  setImage(imgsetCache[bounceSkullSet][0]);
  explodeTimer.setInactive();
}

void BounceFrog::load()
{
  if (big == true)
  {
    wh *= 3.0;
  }
  
  init_circle(getXY(), getWidth() * 0.5, true);
  set_b2d_callback_data(this);
  set_restitution(1.0);
  set_friction(0.2);
  set_ang_vel(RM::randf(HALF_PI, PI) * (RM::randl() ? -1.0 : 1.0));
  // set_collisions(0x1 << TYPE_TERRAIN, 0xff);
  
  if (heavy == true)
  {
    basePoints = 25;
    setImage(imgCache[bounceSkullHeavy]);
  }
  else if (big == true)
  {
    basePoints = 60;
    change_density(0.75);
    setImage(imgCache[bounceSkullBig]);
    warper.set(this, dwarpCache[bounceBigWarp]);
  }
  else if (explodes == true)
  {
    basePoints = 35;
    setImage(imgsetCache[bounceBombSet][RM::randi(0, 1)]);
  }
  else
  {
    // regular
    basePoints = 7;
    value = RM::randi(0, 4);
    setImage(imgsetCache[bounceSkullSet][value]);
    warper.set(this, ResourceManager::bounceSkullWarps[value]);
  }
}

void BounceFrog::updateMe()
{
  if (lifeState == RM::TRANS_OUT)
  {    
    StarParticles* stars = new StarParticles(getXY(), RM::angle(player->getXY(), getXY()));
    stars->burst->speedBounds *= 3.0;
    level->addAction(stars);

    // level->addAction(FishShatter::createForBounce(*this, 2, 2));
    BoneBreakEffect* breaker = new BoneBreakEffect(*this, 2, 2);
    breaker->setCollisions(0x1 << TYPE_DEBRIS, 0x1 << TYPE_TERRAIN);
    breaker->setVelFromPtPos(player->getXY(), player->get_velocity().length() * 1.1);
    breaker->setRestitutions(0.4);
    level->addDebris(breaker);

    ResourceManager::playCrunch();
    
    level->enemyDefeated(this);
    RMPhysics->DestroyBody(body);
    body = NULL;
    setRemove();
    
    return;
  }
  
  if (standingPhysical() == true)
  {
    set_velocity(Point2(get_velocity().x, PLAYER_JUMP_VEL * 1.5));
  }
  
  updateFromPhysical();
  blinkScript.update();
  
  if (onScreenFixed() == false && 
      collisionBox().collision(level->worldBox) == false)
  {
    Box camBox = level->camBox();
    Point1 xEnd = getX() > camBox.center().x ? camBox.right() : camBox.left();
    Point2 explodePt(xEnd, RM::clamp(getY(), camBox.top() + 96.0, camBox.bottom() - 96.0));
    
    StarParticles* stars = new StarParticles(explodePt, RM::angle(explodePt, player->getXY()));
    stars->burst->speedBounds *= 3.0;
    level->addAction(stars);
        
    level->enemyDefeated(this);
    setRemove();
  }
  else
  {
    explodeTimer.update();
    Point1 progress = explodeTimer.progress();
    
    if (progress < 0.05)
    {
      // do nothing
    }
    else if (explodeTimer.progress() < 0.5)
    {
      setImage(imgsetCache[bounceBombSet][3]);
    }
    else 
    {
      setImage(imgsetCache[bounceBombSet][2]);
    }
  }
}

void BounceFrog::redrawMe()
{
  if (heavy == true || explodes == true)
  {
    drawMe();
  }
  else
  {
    warper.draw(warpWeight);
  }
}

void BounceFrog::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  Point1 angleToPlayer = RM::angle(getXY(), player->getXY());
  
  if (pgo == player && 
      angleToPlayer > PI * 1.2 && angleToPlayer < PI * 1.8 &&
      heavy == false && big == false && explodes == false)
  {
    lifeState = RM::TRANS_OUT;
  }
  else if (pgo == player && explodes == true)
  {
    explodeTimer.setActive();
  }
  else if (pgo == player && 
           explodes == false && heavy == false &&
           blinkScript.length() == 0)
  {
    blinkScript.enqueueX(new LinearFn(&warpWeight, 1.0, 0.4));
    blinkScript.enqueueX(new LinearFn(&warpWeight, 0.0, 0.4));
  }
}

void BounceFrog::callback(ActionEvent* caller)
{
  if (caller == &explodeTimer)
  {
    level->enemyDefeated(this);

    FishShatter* shatter = new FishShatter(*this, 2, 2);
    level->addAction(shatter);
    
    BombExplosion* explode = new BombExplosion(getXY());
    level->addAction(explode);

    if (onScreen() == true) ResourceManager::playBombExplode();

    if (playerCollision(Circle(getXY(), 48.0)) == true)
    {
      player->attacked(getXY(), this);
    }
    
    setRemove();
  }
}

// =============================== PuppyMissile ============================ //

PuppyMissile::PuppyMissile(Point2 origin, Point2 setVel) :
  PizzaGO(),

  animator(this, imgsetCache[puppyMissileImgs], 0.05),
  vel(setVel),

  rotationOffset(0.0),
  rotationData(0.0, 1.0),

  warning(true),
  warningAlpha(0.0),

  wasOnscreen(false),
  playedSound(false)
{
  setXY(origin);
  animator.repeats();

  if (vel.x < -0.01) 
  {
    hflip();
  }
  else
  {
    setRotation(RM::cart_to_pol(vel).y);
  }  
}

void PuppyMissile::updateMe()
{
  animator.update();
  addXY(vel * RM::timePassed());
  updateCollisions();
  
  RM::flatten_me(warningAlpha, shouldWarn() ? 1.0 : 0.0, 4.0 * RM::timePassed());
  
  Logical onscreenNow = onScreen();
  
  // remove if offscreen in direction of flight
  if (onscreenNow == false)
  {
    if ((vel.x > 0.0 && getX() > level->worldBox.right()) || 
        (vel.x < 0.0 && getX() < level->worldBox.left()) ||
        (vel.y > 0.0 && getY() > level->worldBox.bottom() + 128.0))
    {
      lifeState = RM::REMOVE;
    }
  }
  
  if (playedSound == false &&
      onscreenNow == true && wasOnscreen == false)
  {
    ResourceManager::playMissile();
    playedSound = true;
  }
  
  // remove if going the same direction as the player in puppy mode
  if ((vel.x > 0.0 && player->facingToDir() > 0.0) ||
      (vel.x < 0.0 && player->facingToDir() < 0.0)) 
  {
    lifeState = RM::REMOVE;
  }
  
  if (lifeState == RM::ACTIVE)
  {
    RM::bounce_arcsine(rotationOffset, rotationData, 
                    Point2(-PI * 0.02, PI * 0.02), RM::timePassed() * 4.0);    
  }
  
  wasOnscreen = onscreenNow;
}

void PuppyMissile::redrawMe()
{  
  if (warningAlpha > 0.01)
  {
    Box camBox = level->camBox();
 
    Image* arrowImg = imgCache[warningH];
    Point2 arrowXY = Point2(camBox.right(), getY());
    Logical hFlipWarning = false;
    
    if (vel.x > 0.1) 
    {
      arrowXY.x = camBox.left();
      hFlipWarning = true;
    }
    else if (vel.y > 0.01) 
    {
      arrowXY.set(getX(), camBox.top());
      arrowImg = imgCache[warningV];
    }
    
    Point2 flippedScale = RM::flip_scale(Point2(1.0, 1.0) / level->camera.zoom, hFlipWarning);
    arrowImg->draw_scale(arrowXY,
        flippedScale, 0.0, ColorP4(1.0, 1.0, 1.0, warningAlpha));
  }
  
  addRotation(rotationOffset);
  drawMe();
  addRotation(-rotationOffset);

  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

Logical PuppyMissile::shouldWarn()
{
  Box camBox = level->camBox();

  Logical leftScreen = (vel.x > 0.0 && getX() < camBox.left()) || 
    (vel.x < 0.0 && getX() > camBox.right()) ||
    (vel.y > 0.0 && getY() < camBox.top());
  
  return warning == true && leftScreen == true;
}

void PuppyMissile::updateCollisions()
{
  if (lifeState == RM::ACTIVE &&
      playerCollision() == true)
  {
    player->attacked(getXY(), this);
    setRemove();
    
    ResourceManager::playBombExplode();
    level->addAction(new BombExplosion(getXY()));
    level->addAction(FishShatter::createForBounce(*this, 2, 2));
  }
}

Circle PuppyMissile::collisionCircle()
{
  return Circle(getXY(), 7.0);
}


// =============================== BounceComet ============================ //

BounceComet::BounceComet(Point2 origin, Point2 setVel) :
  PizzaGO(),
  ActionListener(),

  fadeAlpha(0.0),
  fader(&fadeAlpha, 1.0, 0.4, this),

  warning(false),
  warningAlpha(0.0),

  glowAlpha(0.0),
  glowFrame(0),

  vel(setVel),
  velMult(1.0),
  velMultData(0.0, 1.0),
  streaker(false)
{
  setAlpha(0.0);
  setScale(0.0, 0.0);
  setImage(imgsetCache[bounceCometSet].first());
  setXY(origin);
  setWH(10.0, 10.0);
  setRotation(RM::cart_to_pol(vel).y);
}

void BounceComet::updateMe()
{
  fader.update();
  updateWarning();
  
  addXY(vel * velMult * RM::timePassed());
  updateCollisions();
  
  if (streaker == true && fader.getActive() == false)
  {
    RM::bounce_arcsine(velMult, velMultData, Point2(1.0, 4.0), 1.0 * RM::timePassed());
  }
  
  Point1 finalXScale = velMult;
  if (lifeState == RM::ACTIVE && fader.getActive()) finalXScale = fadeAlpha;
    
  setScale(finalXScale, fadeAlpha);
  
  updateGlowFrame();    
}

void BounceComet::redrawMe()
{
  drawWarning();
  
  setAlpha(std::min(1.0 - glowAlpha, fadeAlpha));
  setImage(imgsetCache[bounceCometSet][glowFrame]);
  drawMe();
  
  setAlpha(std::min(glowAlpha, fadeAlpha));
  setImage(imgsetCache[bounceCometSet][(glowFrame + 1) % 
      imgsetCache[bounceCometSet].count]);
  drawMe();
}

void BounceComet::updateWarning()
{
  RM::flatten_me(warningAlpha, shouldWarn() ? 1.0 : 0.0, 4.0 * RM::timePassed()); 
}

void BounceComet::drawWarning()
{
  if (warningAlpha > 0.01)
  {
    Box camBox = level->camBox();
    
    Image* arrowImg = imgCache[warningH];
    Point2 arrowXY = Point2(camBox.right(), getY());
    Logical hFlipWarning = false;
    
    if (vel.x > 0.1)
    {
      arrowXY.x = camBox.left();
      hFlipWarning = true;
    }
    
    Point2 flippedScale = RM::flip_scale(Point2(1.0, 1.0) / level->camera.zoom, hFlipWarning);
    arrowImg->draw_scale(arrowXY, flippedScale, 0.0, ColorP4(1.0, 1.0, 1.0, warningAlpha));
  }
}

void BounceComet::updateGlowFrame()
{
  glowAlpha += 4.0 * RM::timePassed();
  glowFrame += RM::wrap1_me(glowAlpha, 0.0, 1.0) == 0 ? 0 : 1;
  glowFrame %= imgsetCache[bounceCometSet].count;
}

void BounceComet::updateCollisions()
{
  if (getActive() == false) return;
  
  if (playerCollision() == true)
  {
    player->attacked(getXY(), this);
    
    startDissipate();
  }
  else if (getX() < -10000 || getX() > 10000)
  {
    startDissipate();
  }
}

void BounceComet::startDissipate()
{
  fader.reset(0.0, 0.25);
  vel.set(0.0, 0.0);
  setPassive();
}

Logical BounceComet::shouldWarn()
{
  Box camBox = level->camBox();

  Logical leftScreen = (vel.x > 0.0 && getX() < camBox.left()) || 
      (vel.x < 0.0 && getX() > camBox.right()) ||
      (vel.y > 0.0 && getY() < camBox.top());
  
  return warning == true && leftScreen == true;
}

void BounceComet::callback(ActionEvent* caller)
{
  if (caller == &fader && lifeState == RM::PASSIVE)
  {
    setRemove();
  }
}

// =============================== BounceEnergyBall ============================ //

BounceEnergyBall::BounceEnergyBall(Point2 origin, Point2 setVel) :
  BounceComet(origin, setVel),
  drawnLast(false)
{
  setImage(imgCache[bounceGiantComet]);
  setWH(96.0, 96.0);
}

void BounceEnergyBall::updateMe()
{
  BounceComet::updateMe();
  
  if (drawnLast == true)
  {
    vflip(!getVFlip());
    setColor(RM::randf(0.7, 1.0), RM::randf(0.7, 1.0), RM::randf(0.7, 1.0));
    drawnLast = false;
  }
}

void BounceEnergyBall::redrawMe()
{
  drawWarning();
  
  setAlpha(fadeAlpha);
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
  
  drawnLast = true;
}

void BounceEnergyBall::startDissipate()
{
  BombExplosion* explode = new BombExplosion(getXY());
  explode->setXScale(2.0);
  level->addAction(explode);
  BounceComet::startDissipate();
}

// =============================== WormholeEffect ============================ //

Wormhole::Wormhole(Point1 arrowWaitTime) :
  PizzaGO(),
  ActionListener(),
  waiter(arrowWaitTime - 1.0, this),
  fader(&color.w, 1.0, 1.0, this)
{
  setAlpha(0.0);
  setScale(0.0, 0.0);
  fader.setInactive();
  setImage(imgCache[bounceWormhole]);
}

void Wormhole::updateMe()
{
  waiter.update();
  fader.update();
  addRotation(TWO_PI * RM::timePassed());
  setScale(getAlpha(), getAlpha());
}

void Wormhole::callback(ActionEvent* caller)
{
  if (caller == &waiter)
  {
    fader.setActive();
  }
  else if (caller == &fader && lifeState == RM::ACTIVE)
  {
    fader.reset(0.0, 0.3);
    lifeState = RM::TRANS_OUT;
  }
  else
  {
    setRemove();
  }
}

// =============================== BounceShock ============================ //

BounceShock::BounceShock() :
  PizzaGO(),
  ActionListener(),
  animator(this, imgsetCache[bounceLightningSet], 0.1),
  script(this),
  warningNow(true),
  warningAlpha(0.0)
{
  animator.repeats();
  autoSize();
  script.wait(1.5);
}

void BounceShock::updateMe()
{
  script.update();
  animator.update();
  RM::flatten_me(warningAlpha, warningNow ? 1.0 : 0.0, 4.0 * RM::timePassed());  
  
  if (warningNow == false && hitThisAttack == false)
  {
    if (collisionBox().collision(player->collisionBox()) == true)
    {
      player->attacked(Point2(player->getX(), getY()), this);
      hitThisAttack = true;
    }
  }
}

void BounceShock::redrawMe()
{
  if (warningAlpha > 0.0)
  {
    Box platBox = level->camBox();
    
    imgCache[warningH]->draw_scale(Point2(platBox.left(), getY()),
                                   Point2(-1.0, 1.0) / level->camera.zoom, 0.0, 
                                   ColorP4(1.0, 1.0, 1.0, warningAlpha));
    
    imgCache[warningH]->draw_scale(Point2(platBox.right(), getY()),
                                   Point2(1.0, 1.0) / level->camera.zoom, 0.0, 
                                   ColorP4(1.0, 1.0, 1.0, warningAlpha));  
  }
  
  if (warningNow == false)
  {
    drawMeRepeating();
  }
}

void BounceShock::callback(ActionEvent* caller)
{
  if (caller == &script && warningNow == true)
  {
    warningNow = false;
    
    Box camBox = level->camBox();
    setX(camBox.left() - 384.0);
    setW(camBox.width() + 768.0);
    
    script.enqueueX(new LinearFn(&color.w, 1.0, 0.1));
    script.enqueueX(new FunctionCommand0(ResourceManager::playLightning));
    script.wait(0.35);
    script.enqueueX(new LinearFn(&color.w, 0.0, 0.1));
  }
  else if (caller == &script && warningNow == false)
  {
    setRemove();
  }
}

// =============================== PuppyCage ============================ //

PuppyCage::PuppyCage() :
  PizzaGO(),
  impactTimer(0.1),
  damage(0),
  puppy(NULL)
{
  setImage(imgsetCache[puppyCage][0]);
  autoSize();
  impactTimer.setActive(false);
}

void PuppyCage::load()
{
  init_box_TL(getXY() - getImage()->handle, getSize(), false);
  fixture->SetUserData(this);
  set_collisions(1 << PizzaGO::TYPE_DEBRIS, 0xffff);
}

void PuppyCage::updateMe()
{
  impactTimer.update();
  if (lifeState == RM::TRANS_OUT)
  {
    destroyPhysicalStd(Coord2(2, 3), getImage());
    puppy->cageBroken();
  }
}
              
void PuppyCage::redrawMe()
{
  drawMe();
}

void PuppyCage::tryDamageCage()
{
  if (lifeState != RM::ACTIVE) return;
  
  impactTimer.reset();
  damage++;
  
  if (damage < imgsetCache[puppyCage].count)
  {
    setImage(imgsetCache[puppyCage][damage]);
  }
  else
  {
    lifeState = RM::TRANS_OUT;
  }
}

void PuppyCage::explosionOnscreen(const Circle& explosion)
{
  if (collisionCircle().collision(explosion) == true) 
  {
    tryDamageCage();
  }
}

void PuppyCage::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (normalImpulse < 800.0) return;
  if (impactTimer.getActive() == true) return;

  tryDamageCage();
}

// =============================== Billy ============================ //

const Point1 Billy::HOP_GRAVITY = 500.0;

Billy::Billy() :
  PizzaGO(),
  ActionListener(),

  fullImgSet(NULL),
  jumpAnim(),
  walkAnim(),
  tackBehavior(this),

  turnTimer(1.5, this),
  barkOnTimer(0.15, this),
  barkOffTimer(2.0, this),
  wagOnTimer(0.15, this),
  wagOffTimer(2.0, this),

  aiState(WAIT_FOR_BREAK),
  stickTargetOffset(0.0, 0.0),
  currSpeed(0.0),
  xHopSpeed(0.0),
  timeForHop(1.0)
{
  type = TYPE_PUPPY;
  aiState = WAIT_FOR_BREAK;
}

void Billy::load()
{
  if (breedIndex == 0) fullImgSet = &imgsetCache[puppyAImgs];
  else if (breedIndex == 1) fullImgSet = &imgsetCache[puppyBImgs];
  else if (breedIndex == 2) fullImgSet = &imgsetCache[puppyCImgs];
  
  jumpAnim.init(this, DataList<Image*>(*fullImgSet, 7, 10), 0.15);
  jumpAnim.repeats();
  
  walkAnim.init(this, DataList<Image*>(*fullImgSet, 3, 6), 0.15);
  walkAnim.repeats();
  
  setImage(fullImgSet->get(0));
  wagOnTimer.setInactive();
  barkOnTimer.setInactive();
  
  autoSize();
}

void Billy::updateMe()
{
  updateAI();
}

void Billy::redrawMe()
{
  drawMe();
}

void Billy::updateAI()
{
  switch (aiState)
  {
    default:
    case WAIT_FOR_BREAK:
      turnTimer.update();
      barkOnTimer.update();
      barkOffTimer.update();
      break;
      
    case WAIT_FOR_PLAYER: 
      turnTimer.update();
      barkOnTimer.update();
      barkOffTimer.update();

      if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
      {
        stickTargetOffset = Point2(RM::randf(-64.0, 64.0), RM::randf(-64.0, 64.0));
        currSpeed = 400.0;
        ResourceManager::playBark();
        aiState = JUMP_FOR_PLAYER;
      }
      break;
      
    case JUMP_FOR_PLAYER:
    {
      jumpAnim.update();
      currSpeed += 200.0 * RM::timePassed();
      Point2 playerCenter = player->collisionCircle().xy;
      
      Logical hitTarget = RM::attract_me(xy, playerCenter + stickTargetOffset, currSpeed * RM::timePassed());
      
      if (hitTarget == true) 
      {
        tackBehavior.reset(player);
        setImage(fullImgSet->get(0));
        level->puppyFound(this);
        aiState = ON_PLAYER;
      }
      break;
    }
      
    case ON_PLAYER:
      wagOnTimer.update();
      wagOffTimer.update();
      tackBehavior.update();
      break;
      
    case JUMP_OFF_PLAYER:
    {
      jumpAnim.update();
      // setRotation(RM::chase_angle(getRotation(), 0.0, 2.0 * RM::timePassed()));
      
      hflip(getX() >= level->pranceStartPt().x);
      RM::flatten_me(xy.x, level->pranceStartPt().x, xHopSpeed * RM::timePassed());
      
      currSpeed += HOP_GRAVITY * RM::timePassed();
      addY(currSpeed * RM::timePassed());
      
      Logical landed = RM::clamp_me(xy.y, -1000.0, level->pranceStartPt().y) != 0;
      if (landed == true)
      {
        aiState = GO_TO_HOUSE;
      }
      
      break;      
    }
    case GO_TO_HOUSE:
    {
      walkAnim.update();
      hflip(getX() > level->pranceEndPt().x);
      Logical arrivedX = RM::flatten_me(xy.x, level->pranceEndPt().x, 200.0 * RM::timePassed());
      if (arrivedX == true)
      {
        aiState = IN_HOUSE;
        setXY(level->pranceEndPt());
        // setRotation(0.0);
        level->puppyReturned(this);
      }
      break;      
    }
      
    case IN_HOUSE:
      break;
  }
}

void Billy::cageBroken()
{
  aiState = WAIT_FOR_PLAYER;
}

void Billy::puppyJumpHome()
{
  aiState = JUMP_OFF_PLAYER;
  currSpeed = -100.0;
  
  Point1 varA = HOP_GRAVITY;
  Point1 varB = currSpeed;
  Point1 varC = getY() - level->pranceStartPt().y;
  timeForHop = (-varB + std::sqrt(varB * varB - 4 * varA * varC)) / 2 * varA;
  
  xHopSpeed = std::abs(getX() - level->pranceStartPt().x);
  
  ResourceManager::playBark();
  setRotation(0.0);
}

Circle Billy::collisionCircle()
{
  return Circle(getXY() - Point2(0.0, getHeight() * 0.5), getWidth() * 0.5);
}

void Billy::callback(ActionEvent* caller)
{
  if (caller == &turnTimer)
  {
    toggleHFlip();
    turnTimer.reset();
  }
  else if (caller == &barkOnTimer)
  {
    barkOffTimer.reset(RM::randf(1.0, 2.0));
    setImage(fullImgSet->get(0));
  }
  else if (caller == &barkOffTimer)
  {
    barkOnTimer.reset();
    setImage(fullImgSet->get(1));
    if (onScreen() == true) ResourceManager::playBark();
  }
  else if (caller == &wagOnTimer)
  {
    wagOffTimer.reset(RM::randf(1.0, 2.0));
    setImage(fullImgSet->get(0));    
  }
  else if (caller == &wagOffTimer)
  {
    wagOnTimer.reset();
    setImage(fullImgSet->get(2));    
  }
}

// =============================== SkullBomb ============================ //

SkullBomb::SkullBomb() :
  PizzaGO(),
  ActionListener(),
  animator(this, imgsetCache[bombSet], 0.03),
  explodeTimer(5.0, this)
{
  animator.repeats();
  setImage(imgsetCache[bombSet][0]);
  autoSize(0.8);
}

void SkullBomb::load()
{
  init_circle(getXY(), getWidth() * 0.5, true);
  fixture->SetUserData(this);
  set_restitution(0.35);
  set_collisions(1 << PizzaGO::TYPE_DEBRIS, 0xffff);
  set_ang_vel(RM::randf(-PI, PI));
}

void SkullBomb::updateMe()
{
  updateFromPhysical();
  explodeTimer.update();
  animator.update();
  
  Point1 otherColors = 1.0 - RM::clamp((explodeTimer.progress() - 0.5) / 0.5, 0.0, 1.0);
  setColor(1.0, otherColors, otherColors);
}

void SkullBomb::redrawMe()
{
  drawMe();
}

void SkullBomb::callback(ActionEvent* caller)
{
  if (caller == &explodeTimer)
  {
    if (playerCollision() == true)
    {
      player->attacked(getXY(), this);
    }
    
    if (onScreen() == true) ResourceManager::playBombExplode();

    level->reportExplode(collisionCircle());
    level->addAction(new BombExplosion(getXY()));
    level->addAction(FishShatter::createForBounce(*this, 3, 3));
    
    lifeState = RM::REMOVE;
  }
}

// =============================== BombExplosion ============================ //

BombExplosion::BombExplosion(Point2 center) :
  VisRectangular(),
  Effect(),
  ActionListener(),
  glowAlpha(0.0),
  glowFrame(0),
  fadeAlpha(1.0),
  scaler(&scale.x, 1.5, 1.5),   // angry sun, barrel, plane bullet change this manually
  beforeFadeTimer(1.0, this),  // plane bullet changes
  fader(&fadeAlpha, 0.0, 0.5, this)  // plane bullet changes
{
  setXY(center);
  setScale(0.25, 0.25);  // angry sun, barrel, plane bullet, giant comet change this manually
  setImage(imgsetCache[bombExplodeSet][0]);
  fader.setActive(false);
}

void BombExplosion::updateMe()
{
  beforeFadeTimer.update();
  fader.update();
  scaler.update();
  matchXScale();
  
  glowAlpha += RM::timePassed() * 4.0;
  glowFrame += RM::wrap1_me(glowAlpha, 0.0, 1.0) == 0 ? 0 : 1;
  glowFrame %= imgsetCache[bombExplodeSet].count;
}

void BombExplosion::redrawMe()
{
  setAlpha(std::min(1.0 - glowAlpha, fadeAlpha));
  setImage(imgsetCache[bombExplodeSet][glowFrame]);
  drawMe();
  
  setAlpha(std::min(glowAlpha, fadeAlpha));
  setImage(imgsetCache[bombExplodeSet][(glowFrame + 1) % imgsetCache[bombExplodeSet].count]);
  drawMe();
}

void BombExplosion::callback(ActionEvent* caller)
{
  if (caller == &beforeFadeTimer)
  {
    fader.setActive(true);
  }
  else if (caller == &fader)
  {
    done();
  }
}

// =============================== Mimic ============================ //

Mimic::Mimic() :
  PizzaGO(),
  ActionListener(),

  animator(),
  jumpScript(this),
  jumpTimer(1.0, this),

  oldXY(0.0, 0.0),
  startY(0.0),
  hasCoin(true)
{
  setImage(imgsetCache[mimicSet][0]);
  setWH(96.0, 96.0);
  
  DataList<Image*> fullBite;
  Image::create_pingpong(imgsetCache[mimicSet], fullBite);
  
  animator.init(this, fullBite, 0.12);
  animator.setInactive();
}

void Mimic::load()
{
  init_box(getXY(), getSize(), false);
  set_body_type(b2_kinematicBody);
  set_b2d_callback_data(this);
  set_collisions(1 << PizzaGO::TYPE_DEBRIS, 0xffff);
  
  oldXY = getXY();
  sortY = getY() + getHeight() * 0.5;
  startY = getY();
  facePlayer();
  
  callback(&jumpScript);
}

void Mimic::updateMe()
{
  updateFromWorld();
  
  Logical playerCloseX = player->collisionCircle().right() + 192.0 > getX() &&
      player->collisionCircle().left() - 192.0 < getX();

  oldXY = getXY();
  
  if (playerCloseX == true) jumpTimer.update();
  else jumpTimer.reset(0.5);
  
  jumpScript.update();
  animator.update();
  
  // always be at least eyes if the player is close
  if (playerCloseX == true && getImage() == imgsetCache[mimicSet][0])
  {
    setImage(imgsetCache[mimicSet][1]);
  }
  else if (playerCloseX == false && getImage() == imgsetCache[mimicSet][1])
  {
    setImage(imgsetCache[mimicSet][0]);    
  }
  
  facePlayer();  
  updateCollisions();
}

void Mimic::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    attackCircle().draw_outline(RED_SOLID);
  }
}

Logical Mimic::onGround()
{
  return getY() + getHeight() * 0.5 + 1.0 >= sortY;
}

void Mimic::updateCollisions()
{
  Logical hitNow = attackCircle().collision(player->collisionCircle());
  
  if (hitNow == true)
  {
    if (hitThisAttack == false) player->attacked(attackCircle().xy, this);
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
}

void Mimic::puppyTurn(Point1 objectiveDir)
{
  hasCoin = true;
}

void Mimic::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (normalImpulse > 4000 && hasCoin == true)
  {
    createMagnetCoin(DIME);
    level->gotScore(15);
    hasCoin = false;
  }
}

Circle Mimic::attackCircle()
{
  return Circle(getXY() + Point2(getWidth() * 0.5 * facingToDir(), 0.0), 16.0);
}

void Mimic::callback(ActionEvent* caller)
{
  if (caller == &jumpScript)
  {
    setImage(imgsetCache[mimicSet][0]);
    jumpTimer.reset(1.0);
  }
  else if (caller == &jumpTimer)
  {
    ResourceManager::playMimicJump();
    animator.reset();
    jumpScript.enqueueX(new DeAccelFn(&xy.y, startY - 128.0, 0.5));
    jumpScript.enqueueX(new AccelFn(&xy.y, startY, 0.5));  
  }
}

// =============================== JumpingFireball ============================ //

JumpingFireball::JumpingFireball() :
  PizzaGO(),
  animator(this, imgsetCache[puppyFireballSet], 0.15),
  upMover(&xy.y, 0.0, 1.0, this),
  downMover(&xy.y, 0.0, 1.0, this),
  waitTimer(1.0, this),
  baseWaitTime(0.0),
  startXY(0.0, 0.0)
{
  animator.repeats();
  setImage(imgsetCache[puppyFireballSet][0]);
  setWH(24.0, 24.0);
  setRotation(PI * 1.5);
}

void JumpingFireball::setMovement(Point1 upVal, Point1 upTime, Point1 waitTime)
{
  startXY = getXY();
  baseWaitTime = waitTime;
  
  upMover.reset(getY() - upVal, upTime);
  downMover.reset(getY(), upTime);
  waitTimer.reset(baseWaitTime);
  
  upMover.setActive(false);
  downMover.setActive(false);
}

void JumpingFireball::updateMe()
{
  Box lavaBox = Box(0.0, 0.0, 96.0, 96.0);
  lavaBox.center_on(Point2(getX(), downMover.endVal));
  
  if (level->camBox().collision(lavaBox) == false)
  {
    upMover.reset();
    downMover.reset();
    waitTimer.reset(1.75);
    
    setY(downMover.endVal);
    upMover.setInactive();
    downMover.setInactive();
    hflip(false);
  }
  
  animator.update();
  upMover.update();
  downMover.update();
  waitTimer.update();
  
  if (playerCollision() == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(getXY(), this);
      hitThisAttack = true;
    }
  }
  else
  {
    hitThisAttack = false;
  }
}

void JumpingFireball::redrawMe()
{
  RMGraphics->clip_world(Box(startXY + Point2(-64.0, -512.0), Point2(128.0, 512.0)));
  PizzaGO::redrawMe();
  RMGraphics->unclip();
}

void JumpingFireball::callback(ActionEvent* caller)
{
  if (caller == &waitTimer)
  {
    ResourceManager::playFireballJump();
    level->addAction(new FireSplash(getXY()));
    upMover.reset();
    hflip(false);
  }
  else if (caller == &upMover)
  {
    downMover.reset();
    hflip(true);
  }
  else if (caller == &downMover)
  {
    waitTimer.reset(baseWaitTime);
    hflip(false);
  }
}

// =============================== Mole ============================ //

Mole::Mole() :
  PizzaGO(),
  ActionListener(),

  upMover(&xy.y, 0.0, 1.0, this),
  downMover(&xy.y, 0.0, 1.0, this),
  
  waitThrowTimer(1.0, this),
  riseTimer(0.3, this),
  
  moleState(MOLE_DOWN),
  hasCoin(true),
  oldXY(0.0, 0.0),
  startXY(0.0, 0.0),

  upAnim(this, DataList<Image*>(imgsetCache[moleImgs], 0, 4), 0.15),
  downAnim(),
  throwAnim(this, DataList<Image*>(imgsetCache[moleImgs], 5, 8), 0.15, this),
  resetAnim(this, DataList<Image*>(imgsetCache[moleImgs], 9, 14), 0.15, this),
  downForwardAnim(this, DataList<Image*>(imgsetCache[moleImgs], 15, 19), 0.15),
  currAnim(NULL)
{
  DataList<Image*> moleDown(imgsetCache[moleImgs], 0, 4);
  moleDown.reverse();
  downAnim.init(this, moleDown, 0.15);
  
  setWH(32.0, 32.0);
  setImage(imgsetCache[moleImgs][0]);
  currAnim = &upAnim;
}

void Mole::load()
{
  init_circle(getXY(), getWidth(), false);
  set_body_type(b2_kinematicBody);
  set_collisions(1 << TYPE_TERRAIN, 1 << TYPE_PLAYER);
  set_restitution(0.5);
  set_b2d_callback_data(this);
  
  startXY = getXY();
  upMover.reset(getY() - 64.0, 0.5);
  upMover.setActive(false);
  downMover.reset(getY(), 0.5);
  downMover.setActive(false);
  waitThrowTimer.setInactive();
  
  moleState = MOLE_GOING_UP;
}

void Mole::updateMe()
{
  updateFromPhysical();
  
  oldXY = getXY();
  if (onScreen() == true)
  {    
    upMover.update();
    downMover.update();
    riseTimer.update();
    waitThrowTimer.update();
    currAnim->update();
  }
  else
  {
    moleState = MOLE_DOWN;
    
    setY(downMover.endVal);
    updateFromWorld();
    
    currAnim = &upAnim;
    currAnim->reset();
    currAnim->setInactive();
    upMover.reset();
    upMover.setInactive();
    downMover.reset();
    downMover.setInactive();
    riseTimer.reset(0.3);
  }
  
  velFromWorld(oldXY);  
  facePlayer();
}

void Mole::redrawMe()
{
  Point2 realXY = getXY();
  setXY(startXY + Point2(0.0, -32.0));
  PizzaGO::redrawMe();
  setXY(realXY);
}

void Mole::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (moleState == MOLE_GOING_DOWN || moleState == MOLE_DOWN) return;
  
  moleState = MOLE_GOING_DOWN;
  upMover.setInactive();
  waitThrowTimer.setInactive();
  
  Image* myImg = getImage();
  if (myImg == imgsetCache[moleImgs][7] || 
      myImg == imgsetCache[moleImgs][8] ||
      myImg == imgsetCache[moleImgs][9] ||
      myImg == imgsetCache[moleImgs][10] ||
      myImg == imgsetCache[moleImgs][11] ||
      myImg == imgsetCache[moleImgs][15] ||
      myImg == imgsetCache[moleImgs][16] ||
      myImg == imgsetCache[moleImgs][17] ||
      myImg == imgsetCache[moleImgs][18] ||
      myImg == imgsetCache[moleImgs][19])
  {
    currAnim = &downForwardAnim;
  }
  else
  {
    currAnim = &downAnim;
  }
  currAnim->reset();
  
  if (player->isSlamming == true) downMover.reset_time(0.25);
  else downMover.reset_time(0.5);
  
  if (hasCoin == true)
  {
    level->gotScore(10);
    createMagnetCoin(NICKEL);
    hasCoin = false;
  }
}

void Mole::puppyTurn(Point1 objectiveDir)
{
  hasCoin = true;
}

void Mole::callback(ActionEvent* caller)
{
  if (caller == &upMover)
  {
    moleState = MOLE_UP;
    waitThrowTimer.reset(0.5);
  }
  else if (caller == &downMover)
  {
    moleState = MOLE_DOWN;
    currAnim = &upAnim;
    currAnim->reset();
    currAnim->apply();
    currAnim->setInactive();
    riseTimer.reset(2.0);
  }
  else if (caller == &riseTimer)
  {
    moleState = MOLE_GOING_UP;
    currAnim = &upAnim;
    currAnim->reset();
    upMover.reset();
  }
  else if (caller == &throwAnim)
  {
    Caltrop* spike = new Caltrop();
    spike->setXY(getXY() + Point2(66.0 * facingToDir(), -2.0));
    level->addAlly(spike);
    spike->set_velocity(Point2(200.0 * facingToDir(), -150.0));
    spike->set_ang_vel(TWO_PI * facingToDir());
    
    ResourceManager::playMoleThrow();
    
    currAnim = &resetAnim;
    currAnim->reset();
  }
  else if (caller == &resetAnim)
  {
    waitThrowTimer.reset(1.0);
    currAnim->setInactive();
  }
  else if (caller == &waitThrowTimer)
  {
    currAnim = &throwAnim;
    currAnim->reset();    
  }
}

// =============================== Caltrop ============================ //

Caltrop::Caltrop() :
  PizzaGO(),
  lifeTimer(3.0, this)
{
  setImage(imgCache[caltropImg]);
  autoSize(0.5);
}

void Caltrop::load()
{
  init_circle(getXY(), getWidth(), true);
  set_b2d_callback_data(this);
  set_collisions(1 << TYPE_SKELETON, 0xffff);
}

void Caltrop::updateMe()
{
  updateFromPhysical();
  
  if (lifeState == RM::TRANS_OUT)
  {
    player->attacked(getXY(), this);
    destroyPhysicalStd(Coord2(1, 2), getImage());
  }
  
  lifeTimer.update();
  
  if (lifeState == RM::ACTIVE && onScreen() == false)
  {
    setRemove();
  }
}

void Caltrop::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (pgo == player) lifeState = RM::TRANS_OUT;
}

void Caltrop::callback(ActionEvent* caller)
{
  if (lifeState == RM::ACTIVE) destroyPhysicalStd(Coord2(1, 2), getImage());
}

// =============================== TrianglePivot ============================ //

TrianglePivot::TrianglePivot() :
  PizzaGO(),
  anchor()
{
  setImage(imgCache[puppyPivotTri]);
}

void TrianglePivot::load()
{
  body = create_body(getXY(), true);
  fixture = create_tri_fixture(body, Point2(0.0, 0.0), Point2(0.0, -160.0), Point2(-160.0, 0.0));
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  set_b2d_callback_data(this);
  
  anchor.init(body, getXY());
}

void TrianglePivot::updateMe()
{
  updateFromPhysical();
}

void TrianglePivot::puppyTurn(Point1 objectiveDir)
{
  setRotation(HALF_PI);
  updateFromWorld();
}

// =============================== TrianglePivot ============================ //

SquarePivot::SquarePivot() :
  PizzaGO(),
  anchor()
{
  setImage(imgCache[puppyPivotSquare]);
}

void SquarePivot::load()
{
  init_box(getXY(), getImage()->natural_size(), true);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  set_b2d_callback_data(this);
  
  anchor.init(body, getXY());
  
  setRotation(HALF_PI * 0.5);
  updateFromWorld();
}

void SquarePivot::updateMe()
{
  updateFromPhysical();
}

void SquarePivot::puppyTurn(Point1 objectiveDir)
{
  setRotation(HALF_PI * 0.5);
  set_velocity(Point2(0.0, 0.0));
  updateFromWorld();
}

// =============================== AngrySun ============================ //

AngrySun::AngrySun() :
  PizzaGO(),
  ActionListener(),

  warper(this, dwarpCache[sunWarp]),
  warpWeight(0.0),
  warpTarget(1),

  script(this),
  sunState(MOVING_OFFSCREEN),

  flameGen(0.0),
  flameList(),

  relativeCamXY(0.0, 0.0),
  targetY(0.0),
  swapsSides(false),
  harmless(false),

  wasLeftOfPlayer(false),
  hitThisPass(false),
  angerNextFrame(false),
  coinsLeft(75),

  bobbing(false),
  bobOffset(0.0),
  bobData(0.5, 1.0)
{
  callback(&script);
  setImage(imgCache[madsun]);
  setWH(36.0, 36.0);
  targetY = PLAY_H - getHeight();
}

void AngrySun::updateMe()
{
  generateFlames();
  
  Logical arrived = RM::flatten_me(warpWeight, 1.0, 2.0 * RM::timePassed());
  
  // this updates the idle warp
  if (warpTarget == 0 && arrived == true)
  {
    warpTarget = 1;
    warper.last_to_cont();
    warpWeight = 0.0;
  }
  else if (warpTarget == 1 && arrived == true)
  {
    warpTarget = 0;
    warper.last_to_cont();
    warpWeight = 0.0;
  }
  
  script.update();
  flameList.update();
  if (angerNextFrame == true) startAngry();
  
  if (sunState == COMING_ONSCREEN)
  {
    setXY(level->camBox().corner(facingToCorner()) + relativeCamXY);
  }
  else if (sunState == THREATENING)
  {
    setXY(level->camBox().corner(facingToCorner()) + relativeCamXY);
  }
  else if (sunState == ATTACKING)
  {
    updateCollisions();
    updateCoinToss();
  }
  else if (sunState == MOVING_OFFSCREEN)
  {
    updateCollisions();    
    addX(256.0 * facingToDir() * RM::timePassed());
    updateCoinToss();
    
    Box camBox = level->camBox();
    Point1 offEdge = (getHFlip() == true && getX() < player->getX()) ||
    (getHFlip() == false && getX() > player->getX());

    if (imageBox().collision(camBox) == false && offEdge == true)
    {
      callback(&script);
    }
  }
  
  if (bobbing == true) RM::bounce_arcsine(bobOffset, bobData, Point2(-4.0, 4.0), 4.0 * RM::timePassed());
  scale.y = scale.x;
  wasLeftOfPlayer = getX() < player->getX();
}

void AngrySun::redrawMe()
{
  flameList.redraw();
  
  addY(bobOffset);
  setImage(imgCache[madsun]);
  warper.draw_cont(warpTarget, warpWeight);
  
  setImage(imgCache[madsunGlow]);
  warper.draw_cont(warpTarget, warpWeight);
  addY(-bobOffset);
}

void AngrySun::updatePostCamera()
{
  if (sunState == COMING_ONSCREEN)
  {
    setXY(level->camBox().corner(facingToCorner()) + relativeCamXY);
  }
  else if (sunState == THREATENING)
  {
    setXY(level->camBox().corner(facingToCorner()) + relativeCamXY);
  }
}

void AngrySun::updateCollisions()
{
  if (harmless == true) return;
  
  if (playerCollision() == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(getXY(), this);
    }
    hitThisPass = true;
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
}

void AngrySun::updateCoinToss()
{
  if (hitThisPass == true || coinsLeft <= 0 || harmless == true) return;
  
  // cout << "oldX " << oldX << " player->getX() " << player->getX() << " " << getX() << endl;
  if ((wasLeftOfPlayer == true && getX() > player->getX()) || 
      (wasLeftOfPlayer == false && getX() < player->getX()))
  {
    createMagnetCoin();
    coinsLeft--;
  }
}

void AngrySun::generateFlames()
{
  if (sunState != WAITING_OFFSCREEN)
  {
    flameGen += RM::randf(0.0, 2.0) * RM::timePassed();
    
    while (flameGen > 1.0 / 60.0)
    {
      flameList.addX(new SunSpark(getXY() + RM::pol_to_cart(RM::randf(0.0, getWidth()), 
                                                            RM::randf(0.0, TWO_PI))));
      flameGen -= 1.0 / 60.0;
    }
  }
}

Coord1 AngrySun::facingToCorner()
{
  return getHFlip() == false ? 0 : 1;
}

void AngrySun::startAngry()
{
  warper.last_to_cont();
  warpWeight = 0.0;
  warpTarget = 2;

  ResourceManager::playSunAttack();

  angerNextFrame = false;
}

void AngrySun::puppyTurn(Point1 objectiveDir)
{
  sunState = MOVING_OFFSCREEN;
  harmless = true;
  hitThisPass = false;
  coinsLeft = 75;
  
  script.clear();
  script.enqueueX(new LinearFn(&color.w, 0.0, 0.25));
  script.enqueueX(new SetValueCommand<Byte4>(&flipFlags, 0x0));
  script.enqueueX(new SetValueCommand<Logical>(&harmless, false));
}

void AngrySun::puppyDone(Point2 sunEnd, Point1 duration)
{
  setAlpha(1.0);
  sunState = MOVING_OFFSCREEN;
  lifeState = RM::TRANS_OUT;
  harmless = true;
  script.clear();
  
  warper.last_to_cont();
  warpWeight = 0.0;
  warpTarget = 3;
  
  ActionList* movers = new ActionList();
  movers->addX(new LinearFn(&xy.x, sunEnd.x, duration * 0.5));
  movers->addX(new LinearFn(&xy.y, sunEnd.y, duration * 0.5), true);
  
  script.enqueueX(movers);
  script.enqueueX(new CosineFn(&scale.x, 2.0, duration * 0.5));
}

void AngrySun::bounceDone(Point2 playerOffset)
{
  
}

void AngrySun::callback(ActionEvent* caller)
{
  bobbing = false;  // set to true on particular states
  
  if (lifeState == RM::TRANS_OUT)
  {
    setImage(imgCache[madsun]);
    level->addAction(new FishShatter(*this, 4, 4));

    setImage(imgCache[madsunGlow]);
    level->addAction(new FishShatter(*this, 4, 4));

    BombExplosion* explode = new BombExplosion(getXY());
    explode->setScale(2.0, 2.0);
    explode->scaler.reset(3.5);
    level->addAction(explode);
    
    setRemove();
    return;
  }
  
  ++sunState;
  sunState %= NUM_SUNSTATES;
  
  switch (sunState)
  {
    case WAITING_OFFSCREEN:
      warper.last_to_cont();
      warpWeight = 0.0;
      warpTarget = 0;
      
      setAlpha(0.0);
      script.wait(1.0);
      break;
    case COMING_ONSCREEN:
      setAlpha(1.0);
      setRotation(0.0);
      bobbing = true;
      if (swapsSides == true) hflip(!getHFlip());
      relativeCamXY = getSize() * Point2(-facingToDir(), 1.0);
      script.enqueueX(new CosineFn(&relativeCamXY.x, getWidth() * facingToDir(), 1.0));
      break;      
    case THREATENING:
      bobbing = true;
      script.wait(2.5);
      script.enqueueX(new SetValueCommand<Logical>(&angerNextFrame, true));
      script.enqueueX(new SetValueCommand<Logical>(&bobbing, false));
      script.wait(0.5);
      break;
    case ATTACKING:
    {
      hitThisPass = false;
      Point2 attackTarget = Point2(player->getX(), targetY);
            
      ActionList* movers = new ActionList();
      movers->addX(new LinearFn(&xy.x, attackTarget.x, 2.0));
      movers->addX(new DeAccelFn(&xy.y, attackTarget.y, 2.0), true);
      
      script.enqueueX(movers);
      
      break;      
    }
    case MOVING_OFFSCREEN:
      // handled during the update
      break;
  }  
}

// =============================== SunSpark ============================ //

SunSpark::SunSpark(Point2 center) :
  VisRectangular(center, imgCache[madsunFlame]),
  Effect(),

  totalDur(RM::randf(0.5, 0.75)),
  angVel(RM::randf(-1.5, 1.5)),

  fader(&color.w, 1.0, totalDur * 0.5, this),
  scaler(&scale.x, RM::randf(1.5, 2.0), totalDur),

  fadeIn(true)
{
  Point1 startScale = RM::randf(0.5, 0.75);
  setRotation(RM::randf(0.0, TWO_PI));
}

void SunSpark::updateMe()
{
  fader.update();
  scaler.update();
  addRotation(angVel * RM::timePassed());
  addY(-75.0 * RM::timePassed());
}

void SunSpark::redrawMe()
{
  drawMe();
}

void SunSpark::callback(ActionEvent* caller)
{
  if (caller == &fader && fadeIn == true)
  {
    fadeIn = false;
    fader.reset(0.0);
  }
  else if (caller == &fader && fadeIn == false)
  {
    done();
  }
}

// =============================== StickyPatch ============================ //

StickyPatch::StickyPatch(Point2 topCenter, Point1 angle) :
  PizzaGO()
{
  flags = STICK_BIT | SLOW_BIT;
  
  // setImage(ResourceManager::sludge);
  // this is a hack so that it draws before everything else
  shadowImg = imgCache[sludge];
  setWH(96.0, 16.0);
  setXY(topCenter + getHeight() * 2.0);
  setRotation(angle);
}

void StickyPatch::load()
{
  init_box(getXY() + Point2(0.0, getHeight() * 0.0), getSize(), false);
  fixture->SetSensor(true);
  fixture->SetUserData(this);
  set_collisions(0x1 << TYPE_TERRAIN, 0x1 << TYPE_PLAYER);
  sortY = getY();
}

void StickyPatch::updateMe()
{
  if (touchingPGO(player) == true) 
  {
    level->reportSurface(flags);
  }
}

void StickyPatch::redrawMe()
{
  
}

// =============================== LavaPatch ============================ //

LavaPatch::LavaPatch(Point2 leftCenter, Logical small) :
  PizzaGO(),
  animator(this, small ? imgsetCache[puppyLavaSetSmall] : imgsetCache[puppyLavaSetLarge], 0.1)
{
  animator.repeats();
  setWH(getImage()->natural_size().x, 64.0);  // size is full size  
  setXY(leftCenter);
}

void LavaPatch::load()
{
  init_box_TL(getXY() - Point2(0.0, getHeight() * 0.15), getSize(), false);
  fixture->SetSensor(true);
  set_b2d_callback_data(this);
  set_collisions(0x1 << TYPE_TERRAIN, 0x1 << TYPE_PLAYER);
}

void LavaPatch::updateMe()
{
  animator.update();
  
  if (touchingPGO(player) == true) 
  {
    player->attacked(getXY() + 0.5 * getSize(), this);
  }
}

void LavaPatch::redrawMe()
{
  drawMe();
}

// =============================== TerrainQuad ============================ //

TerrainQuad::TerrainQuad() :
  PizzaGO(),
  ActionListener(),

  topRight(0.0, 0.0),

  puppyEdgeBits(0x0),

  isBouncy(false),
  startBounce(false),
  bouncer(&scale.x, 1.1, 0.1, this)
{
  basePoints = 5;
  type = TYPE_TERRAIN;
  bouncer.setInactive();
}

void TerrainQuad::load()
{
  // this is just a default
  if (level->spikedTerrain == true) flags |= SPIKED_BIT;
  if (crushesPhysical == true) level->totalDestructibles++;
}

void TerrainQuad::updateMe()
{
  if (body->GetType() != b2_staticBody) updateFromPhysical();

  checkSurface();  
  checkPhysicalDestruction();
  
  bouncer.update();
  setYScale(getXScale());
  
  // should only be called once
  if (destroyedPhysical == true)
  {
    blockDestroyed();
  }
    
  // removes boxes that fall into puppy pits
  if ((flags & DELETE_BELOWWORLD_BIT) != 0 && 
      getActive() == true &&
      getY() > PLAY_H + 128.0)
  {
    setRemove();
  }
}

void TerrainQuad::redrawMe()
{
  if (getImage() != NULL) drawMe();
  if (puppyEdgeBits != 0) drawPuppyEdges();
}

void TerrainQuad::setBreakaway()
{
  flags |= BREAKAWAY_BIT;
}

void TerrainQuad::checkSurface()
{
  if (flags == 0) return;
  
  Point2 location;
  if (touchingPGO(player, &location) == true) playerTouch(location);
  
  if ((flags & BALANCE_BOUNCE_BIT) != 0)
  {
    BalanceSkull* ball = level->getCurrBall();
    
    if (touchingPGO(ball) == true)
    {
      ball->set_velocity(Point2(ball->get_velocity().x, -128.0));
    }
  }
}

void TerrainQuad::playerTouch(Point2 location)
{
  if ((flags & SPIKED_BIT) != 0)
  {
    player->attacked(location, this);
  }

  if ((flags & SLOW_BIT) != 0)
  {
    level->reportSurface(flags);
  }

  if ((flags & STICK_BIT) != 0)
  {
    level->reportSurface(flags);
  }
  
  if ((flags & BREAKAWAY_BIT) != 0)
  {
    level->addAction(new TerrainShatter(*this, 8, 5));
    setRemove();
  }
}

void TerrainQuad::explosionOnscreen(const Circle& explosion)
{
  if (crushesPhysical == false) return;
  
  Circle blowUpCircle(getXY(), getWidth() * 1.0);
  if (blowUpCircle.collision(explosion) == true) 
  {
    BoneBreakEffect* breaker = new BoneBreakEffect(*this, 2, 2);
    for (Coord1 i = 0; i < breaker->particles.count; ++i)
    {
      breaker->particles[i]->apply_impulse(RM::ring_edge(explosion.xy, getXY(), 0.5), getXY());
    }
    
    level->addDebris(breaker);
    setRemove();
  }
}

void TerrainQuad::setPuppyEdges(Coord1 numLeft, Coord1 numRight)
{
  for (Coord1 i = 0; i < numLeft; ++i)
  {
    puppyEdgeBits |= 0x1 << i;
  }

  for (Coord1 i = 0; i < numRight; ++i)
  {
    puppyEdgeBits |= 0x1 << (i + 16);
  }
}

void TerrainQuad::setPuppyCorners(Logical leftCorner, Logical rightCorner)
{
  puppyEdgeBits |= ((leftCorner ? 1 : 0) << 8);
  puppyEdgeBits |= ((rightCorner ? 1 : 0) << 24);
  
  if (leftCorner == true)
  {
    // the destroy world will clean these up
    b2Body* body = create_body(getXY() + Point2(-32.0, 0.0), false);
    b2Fixture* fixture = create_quad_fixture(body, 
                                             Point2(0.0, 0.0), Point2(0.0, 128.0),
                                             Point2(32.0, 128.0), Point2(32.0, 0.0));
  }
  if (rightCorner == true)
  {
    // the destroy world will clean these up
    b2Body* body = create_body(topRight, false);
    b2Fixture* fixture = create_quad_fixture(body, 
                                             Point2(0.0, 0.0), Point2(0.0, 128.0),
                                             Point2(32.0, 128.0), Point2(32.0, 0.0));     
  }
}

void TerrainQuad::drawPuppyEdges()
{
  Point2 currPos = getXY();
  Coord1 i = 0;
  
  while ((puppyEdgeBits & (0x1 << i)) != 0)
  {
    Image* img = i == 0 ? imgCache[puppyEdgeTL] : imgCache[puppyEdgeL];
    img->draw(currPos);
    currPos += Point2(0.0, img->natural_size().y - img->handle.y);
    i++;
  }
  
  if ((puppyEdgeBits & (0x1 << 8)) != 0)
  {
    imgCache[puppyCornerL]->draw(getXY());
  }
  
  currPos = topRight;
  i = 16;
  while ((puppyEdgeBits & (0x1 << i)) != 0)
  {
    Image* img = i == 16 ? imgCache[puppyEdgeTR] : imgCache[puppyEdgeR];
    img->draw(currPos);
    currPos += Point2(0.0, img->natural_size().y - img->handle.y);
    i++;
  }

  if ((puppyEdgeBits & (0x1 << 24)) != 0)
  {
    imgCache[puppyCornerR]->draw(topRight);
  }
}

void TerrainQuad::blockDestroyed()
{
  Coord1 randCoin = RM::randi(0, 1);
  
  if (randCoin == 0) 
  { 
    createMagnetCoin(SlopeCoin::randValueAll());
  }
  
  ResourceManager::playCrunch();
  level->enemyDefeated(this);
  level->gotScore(basePoints);
}

void TerrainQuad::ignoreAngrySkulls()
{
  set_collisions(0x1 << PizzaGO::TYPE_TERRAIN, 
                 0xffff & (~(0x1 << PizzaGO::TYPE_SKELETON)));
}

void TerrainQuad::setBouncy(Point1 newRestitution)
{
  set_restitution(newRestitution);
  isBouncy = true;
}

void TerrainQuad::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{  
  if (isBouncy == false) 
  {
    // this will break destructibles
    PizzaGO::collidedPhysical(pgo, normalImpulse);
  }
  else
  {
    // this triggers the bounce
    ResourceManager::playBounceSound();
    startBounce = true;
    bouncer.reset(1.15); 
  }  
}

void TerrainQuad::callback(ActionEvent* caller)
{
  if (caller == &bouncer)
  {
    if (startBounce == true) 
    {
      bouncer.reset(1.0);
      startBounce = false;
    }
  }
}


TerrainQuad* TerrainQuad::create_static_quad(const Point2& pt0, const Point2& pt1, 
                                             const Point2& pt2, const Point2& pt3)
{
  TerrainQuad* staticQuad = new TerrainQuad();
  staticQuad->setXY(pt0);
  staticQuad->topRight = pt3;

  staticQuad->init_static_quad(pt0, pt1, pt2, pt3);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->set_b2d_callback_data(staticQuad);
  staticQuad->set_friction(0.5);
  return staticQuad;
}

TerrainQuad* TerrainQuad::create_block_quad(const Point2& pt0, const Point2& pt1, 
                                            const Point2& pt2, const Point2& pt3,
                                            Image* img)
{
  TerrainQuad* quadBlock = new TerrainQuad();
  quadBlock->setImage(img);
  quadBlock->crushesPhysical = true;
  quadBlock->crushValue = 650.0;
  
  quadBlock->init_quad(pt0, pt1, pt2, pt3, true);
  quadBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  quadBlock->set_b2d_callback_data(quadBlock);
  quadBlock->set_friction(0.5);
  quadBlock->updateFromPhysical();
  
  return quadBlock;  
}

TerrainQuad* TerrainQuad::create_block_circle(Point2 center, Point1 radius, Image* img)
{
  TerrainQuad* circleBlock = new TerrainQuad();
  circleBlock->setImage(img);
  circleBlock->crushesPhysical = true;
  circleBlock->crushValue = 650.0;
  
  circleBlock->init_circle(center, radius, true);
  circleBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  circleBlock->set_b2d_callback_data(circleBlock);
  circleBlock->set_friction(0.5);
  circleBlock->updateFromPhysical();
  
  return circleBlock;  
}

TerrainQuad* TerrainQuad::create_static_circle(Point2 center, Point1 radius, Image* img)
{
  TerrainQuad* circleBlock = new TerrainQuad();
  circleBlock->setImage(img);
  
  circleBlock->init_circle(center, radius, false);
  circleBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  circleBlock->set_b2d_callback_data(circleBlock);
  circleBlock->set_friction(0.5);
  circleBlock->updateFromPhysical();
  
  return circleBlock;
}

PlatformRiser* TerrainQuad::create_static_riser(const Point2& pt0, const Point2& pt1, 
                                                const Point2& pt2, const Point2& pt3)
{
  PlatformRiser* staticQuad = new PlatformRiser();
  staticQuad->setXY(pt0);
  staticQuad->topRight = pt3;
  
  staticQuad->init_static_quad(pt0, pt1, pt2, pt3);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->fixture->SetUserData(staticQuad);
  staticQuad->set_friction(0.5);
  return staticQuad;
}

TerrainQuad* TerrainQuad::create_block_TL(Point2 topLeft, Point2 widthHeight, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setBox(topLeft + widthHeight * 0.5, widthHeight);
  dynamicBox->setImage(setImg);
  dynamicBox->crushesPhysical = true;
  dynamicBox->crushValue = 650.0;
  
  dynamicBox->init_box_TL(topLeft, widthHeight, true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(0.5);
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_block_TL(Point2 topLeft, Image* setImg)
{
  return create_block_TL(topLeft, setImg->natural_size(), setImg);
}

TerrainQuad* TerrainQuad::create_block_center(Point2 center, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setXY(center);
  dynamicBox->setImage(setImg);
  dynamicBox->autoSize();
  dynamicBox->crushesPhysical = true;
  dynamicBox->crushValue = 650.0;
  
  dynamicBox->init_box(dynamicBox->getXY(), dynamicBox->getSize(), true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(0.5);
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_block_center(Point2 center, Point2 size, 
                                              Point1 rotation, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setXY(center);
  dynamicBox->setImage(setImg);
  dynamicBox->setWH(size);
  dynamicBox->crushesPhysical = true;
  dynamicBox->crushValue = 650.0;
  
  dynamicBox->init_box(dynamicBox->getXY(), dynamicBox->getSize(), rotation, true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(0.5);
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_block_tri(const Tri& worldTri, Image* setImg)
{
  TerrainQuad* triBlock = new TerrainQuad();
  triBlock->setImage(setImg);
  triBlock->crushesPhysical = true;
  triBlock->crushValue = 650.0;
  
  triBlock->init_tri_corner(worldTri, true);
  triBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  triBlock->fixture->SetUserData(triBlock);
  triBlock->set_friction(0.5);
  triBlock->updateFromPhysical();
  
  return triBlock;  
}

TerrainQuad* TerrainQuad::create_ground_TL(Point2 topLeft, Point2 widthHeight, Image* setImg)
{
  TerrainQuad* staticBox = new TerrainQuad();
  staticBox->setBox(topLeft + widthHeight * 0.5, widthHeight);
  staticBox->setImage(setImg);
  
  staticBox->init_box_TL(topLeft, widthHeight, false);
  staticBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticBox->fixture->SetUserData(staticBox);
  staticBox->set_friction(0.5);
  return staticBox;
}

// =============================== TerrainGround ============================ //

TerrainGround::TerrainGround(const ArrayList<Point2>& worldPts) :
  PizzaGO()
{
  setWH(worldPts.last().x - worldPts.first().x, 64.0);
  init_segmented(worldPts, false);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  fixture->SetUserData(this);
  set_friction(0.5);
}

// =============================== PlatformRiser ============================ //

PlatformRiser::PlatformRiser() :
  TerrainQuad(),
  targetY(0.0),
  riseSpeed(-256.0)
{
  
}

void PlatformRiser::load()
{
  TerrainQuad::load();
  set_body_type(b2_kinematicBody);
}

void PlatformRiser::updateMe()
{
  updateFromPhysical();
  checkSurface();
    
  if (level->getCurrBall() != NULL &&
      level->getCurrBall()->getX() > getX() && getY() > targetY)
  {
    set_velocity(Point2(0.0, -256.0));
  }
  else if (getY() < targetY) 
  {
    setY(targetY);
    set_velocity(Point2(0.0, 0.0));
  }
}

// =============================== TerrainCircle ============================ //

TerrainCircle::TerrainCircle(Image* setImg) :
  PizzaGO(),
  dynamic(false)
{
  setImage(setImg);
}

void TerrainCircle::load()
{
  // this is just a default
  if (level->spikedTerrain == true) flags |= SPIKED_BIT;

  init_circle(getXY(), getWidth(), dynamic);
  set_collisions(1 << TYPE_TERRAIN, 0xfff);
}

void TerrainCircle::updateMe()
{
  if ((flags & SPIKED_BIT) != 0 && 
      touchingPGO(player) == true) 
  {
    player->attacked(getXY(), this);
  }
  
  if (dynamic == true) updateFromPhysical();
}

void TerrainCircle::redrawMe()
{
  if (onScreenFixed() == true) drawMe();
}

// =============================== TerrainShatter ============================ //

TerrainShatter::TerrainShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  ParticleEffect()
{
  Coord2 shatterGrid(cols, rows);
  
  ProceduralBurst* burst = new ProceduralBurst(visRect, shatterGrid.x, shatterGrid.y, Point2(1.0, 2.0));
  
  burst->angleBounds.set(PI * 0.125, PI * 0.375);   // starting angle that it flies in
  burst->speedBounds.set(200.0, 300.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-TWO_PI * 0.05, TWO_PI * 0.05);
  
  burst->gravity.set(40.0, 70.0);  
  // burst->scaleBounds.set(0.75, 2.0);
  
  emitters.add(burst);
}

// =============================== SkullSwitch ============================ //

SkullSwitch::SkullSwitch() :
  PizzaGO(),
  panel(NULL),
  swapped(false),
  rotator(&rotation, PI * 0.33, 0.15)
{
  setImage(imgsetCache[skullSwitchSet][0]);
  autoSize(0.5);
  rotator.setInactive();
}

void SkullSwitch::updateMe()
{
  rotator.update();
  
  if (playerCollision() == true && swapped == false)
  {
    ResourceManager::playSlopeSwitch();
    setImage(imgsetCache[skullSwitchSet][1]);
    rotator.setActive();
    panel->flags &= ~SPIKED_BIT;
    panel->setImage(imgCache[slopeSpikesDown]);
    swapped = true;
  }
}

void SkullSwitch::redrawMe()
{
  drawMe();
}

// =============================== SpikePanel ============================ //

SpikePanel::SpikePanel() : 
  PizzaGO(),
  scaler(&scale.y, 1.0, 0.25)
{
  setYScale(0.0);
  scaler.setActive(false);
}

void SpikePanel::updateMe()
{
  scaler.update();
}

void SpikePanel::redrawMe()
{
  drawMe();
}

void SpikePanel::spikeTrigger()
{
  
}

// =============================== BounceSegment ============================ //

BounceSegment::BounceSegment(Point2 topCenter) :
  PizzaGO(),
  ActionListener(),

  bounceScript(this),
  squishes(false),
  squishedNow(false),
  naturalRestitution(1.0)
{
  type = TYPE_TERRAIN;
  
  setXY(topCenter);
  setWH(144.0, 80.0);
}

void BounceSegment::load()
{
  init_box_TL(Point2(getX() - 72.0, getY()), getSize(), false);
  set_b2d_callback_data(this);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  set_friction(0.1);
  set_restitution(naturalRestitution);   
}

void BounceSegment::updateMe()
{
  bounceScript.update();
  
  if (squishedNow == true) set_restitution(0.0);
  else set_restitution(naturalRestitution);
}

void BounceSegment::redrawMe()
{
  drawMe();
}

void BounceSegment::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (bounceScript.getActive() == true) return;
  
  ResourceManager::playBounceSound();
  
  ActionList* downList = new ActionList();
  downList->addX(new DeAccelFn(&scale.x, 1.25, 0.15), true);
  downList->addX(new DeAccelFn(&xy.y, getY() + 16.0, 0.15));

  ActionList* upList = new ActionList();
  upList->addX(new DeAccelFn(&scale.x, 1.0, 0.15), true);
  upList->addX(new DeAccelFn(&xy.y, getY(), 0.15));
  
  bounceScript.enqueueX(downList);
  if (squishes == true) 
  {
    bounceScript.enqueueX(new SetValueCommand<Logical>(&squishedNow, true));
    bounceScript.enqueueX(new SetValueCommand<Image*>(&image, imgsetCache[bounceSquishSet][1]));
    bounceScript.wait(2.5);
    bounceScript.enqueueX(new SetValueCommand<Image*>(&image, imgsetCache[bounceSquishSet][0]));
    bounceScript.enqueueX(new SetValueCommand<Logical>(&squishedNow, false));
  }
  bounceScript.enqueueX(upList);
}


void BounceSegment::callback(ActionEvent* caller)
{
  
}

// =============================== BounceSegment2 ============================ //

BounceSegment2::BounceSegment2(Point2 topCenter) :
  PizzaGO(),
  ActionListener(),

  bounceScript(this),
  squishes(false),
  squishedNow(false),
  naturalRestitution(1.0)
{
  type = TYPE_TERRAIN;
  
  setXY(topCenter);
  setWH(144.0, 80.0);
}

void BounceSegment2::load()
{
  init_box_TL(Point2(getX() - 72.0, getY()), getSize(), false);
  set_b2d_callback_data(this);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  set_friction(0.1);
  set_restitution(0.0);  
  
  if (squishes == true)
  {
    setImage(imgsetCache[bounceSquishSet][0]);
  }
}

void BounceSegment2::updateMe()
{
  bounceScript.update();
  // updateFromWorld();  ///////
  
  if (touchingPGO(player) == true)
  {
    Point1 bounceVelY = PLAYER_JUMP_VEL * (squishedNow == false ? 1.5 : 0.5);
    Point1 finalVelY = std::min(bounceVelY, player->get_velocity().y); // for nearby squishers
    player->set_velocity(Point2(player->get_velocity().x, finalVelY));
    player->nojumpTimer.reset();  // prevents "double" jump
  }
}

void BounceSegment2::redrawMe()
{
  drawMe();
}

void BounceSegment2::collidedPhysical(PizzaGO* pgo, Point1 normalImpulse)
{
  if (bounceScript.getActive() == true) return;
  
  ActionList* downList = new ActionList();
  downList->addX(new DeAccelFn(&scale.x, 1.25, 0.15), true);
  downList->addX(new DeAccelFn(&xy.y, getY() + 16.0, 0.15));
  
  ActionList* upList = new ActionList();
  upList->addX(new DeAccelFn(&scale.x, 1.0, 0.15), true);
  upList->addX(new DeAccelFn(&xy.y, getY(), 0.15));
  
  if (pgo == player) ResourceManager::playBounceSound();
  
  bounceScript.enqueueX(downList);
  
  if (squishes == true) 
  {
    bounceScript.enqueueX(new SetValueCommand<Logical>(&squishedNow, true));
    bounceScript.enqueueX(new SetValueCommand<Image*>(&image, imgsetCache[bounceSquishSet][1]));
    bounceScript.wait(2.5);
    bounceScript.enqueueX(new SetValueCommand<Logical>(&squishedNow, false));
  }
  
  bounceScript.enqueueX(upList);
  
  if (squishes == true)
  {
    bounceScript.enqueueX(new SetValueCommand<Image*>(&image, imgsetCache[bounceSquishSet][0]));    
  }
}


void BounceSegment2::callback(ActionEvent* caller)
{
  
}

// =============================== BoneBreakEffect ============================ //

BoneBreakEffect::BoneBreakEffect(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  PhysicalTriangleParticles(),
  ActionEvent(),
  ActionListener(),
  solidTimer(1.5, this),
  fadeTimer(0.5, this)
{
  collisionPad = Point2(0.35, 0.35);
  create(visRect, cols, rows);
  
  setCollisions(1 << PizzaGO::TYPE_DEBRIS, 0xffff);
  setRestitutions(0.0);
  fadeTimer.setActive(false);
}

void BoneBreakEffect::updateMe()
{
  solidTimer.update();
  fadeTimer.update();
}

void BoneBreakEffect::redrawMe()
{
  startColor.w = 1.0 - fadeTimer.progress();
  drawParticles();
}

void BoneBreakEffect::setCollisions(Coord1 groupBits, Coord1 maskBits)
{
  for (Coord1 i = 0; i < particles.count; ++i)
  {
    particles[i]->set_collisions(groupBits, maskBits);
  }
}

void BoneBreakEffect::setRestitutions(Point1 rest)
{
  for (Coord1 i = 0; i < particles.count; ++i)
  {
    particles[i]->set_restitution(rest);
  }  
}

void BoneBreakEffect::setVelFromPt(Point2 explodePt, Point1 speed)
{
  for (Coord1 i = 0; i < particles.count; ++i)
  {
    Point2 particlePos = b2dpt_to_world(particles[i]->body->GetPosition());
    Point2 vel = RM::pol_to_cart(speed, RM::angle(explodePt, particlePos));
    particles[i]->set_velocity(vel);
  }
}

void BoneBreakEffect::setVelFromPtPos(Point2 explodePt, Point1 speed)
{
  for (Coord1 i = 0; i < particles.count; ++i)
  {
    Point2 particlePos = b2dpt_to_world(particles[i]->body->GetPosition());
    Point2 vel = RM::pol_to_cart(speed, RM::angle(explodePt, particlePos));
    vel.y = std::abs(vel.y);
    particles[i]->set_velocity(vel);
  }
}

void BoneBreakEffect::callback(ActionEvent* caller)
{
  if (caller == &solidTimer)
  {
    fadeTimer.setActive(true);
  }
  else if (caller == &fadeTimer)
  {    
    done();
  }
}

// =============================== Decoration ============================ //

Decoration::Decoration(Point2 position, Image* setImg) :
  VisRectangular(position, Point2(64.0, 64.0), setImg),
  ActionEvent()
{
  
}

void Decoration::updateMe()
{
  
}

void Decoration::redrawMe()
{
  drawMe();
}

Box Decoration::imageBox()
{
  return Box(getXY() - getImage()->handle, getImage()->natural_size());
}


// =============================== LakeGO ============================ //

LakeGO::LakeGO() :
  VisRectangular(),
  BaseGOLogic(),
  type(LAKE_TYPE_FISH),
  sizeIndex(SIZE_16),
  level(NULL),
  player(NULL),
  mouthTimer(0.25),
  poison(false),
  poisonTimer(0.25, this)
{
  mouthTimer.setInactive();
  poisonTimer.setInactive();
}

void LakeGO::update()
{
  // trans out means swallow
  if (lifeState == RM::REMOVE || lifeState == RM::TRANS_OUT) return;
  
  updateMe();
}

void LakeGO::redrawMe()
{
  drawMe();
}

Circle LakeGO::mouthCircle()
{
  return Circle(getXY(), getWidth());
}

Circle LakeGO::collisionCircle()
{
  return Circle(getXY(), getWidth() * 0.5);
}

Logical LakeGO::playerNearEating()
{
  Point1 nearbyRadius = collisionCircle().radius + player->collisionCircle().radius * 1.5;
  
  if (Circle::collision(Circle(getXY(), nearbyRadius), player->collisionCircle()) == true)
  {
    if ((player->getHFlip() == false && getX() > player->getX()) ||
        (player->getHFlip() == true && getX() < player->getX())) 
    {
      return true;
    }    
  }
  
  return false;
}

void LakeGO::swallowedByPlayer()
{
  FishSwallow* swallowEffect = new FishSwallow(this, player->getXY());
  level->addAction(swallowEffect);
  
  Point2 target = RM::attract(player->mouthCircle().xy, this->getXY(), player->sizeIndex * 16.0);
  level->addAction(new CosineFn(&player->xy.x, target.x, 0.20));
  level->addAction(new CosineFn(&player->xy.y, target.y, 0.20));
  
  for (Coord1 i = 0; i < (sizeIndex + 1) * 3; ++i)
  {
    level->addAlly(new LakeBubble(collisionCircle().random_pt(), RM::randi(0, 1)));    
  }
  
  level->ateFish(this);
  lifeState = RM::TRANS_OUT;
}

Logical LakeGO::touchingPlayer()
{
  return collisionCircle().collision(player->collisionCircle());
}

Logical LakeGO::touchingPlayer(const Circle& circle)
{
  return circle.collision(player->collisionCircle());
}

Logical LakeGO::touchingPlayerMouth()
{
  return collisionCircle().collision(player->mouthCircle());
}

Logical LakeGO::touchingPlayerMouth(const Circle& circle)
{
  return circle.collision(player->mouthCircle());
}

Logical LakeGO::outsideWorld()
{
  return collisionCircle().AABB().collision(LAKE_WORLD_BOX) == false;
}

// =============================== LakePlayer ============================ //

LakePlayer::LakePlayer() :
  LakeGO(),
  ActionListener(),

  face(this),

  scaler(&scale.x, 2.0, 1.5, this),
  fadeInImage(NULL),
  fadeInAlpha(0.0),
  dmgTimer(2.0),

  vxy(0.0, 0.0),
  sizeMult(1.0),
  amountEaten(0.0),
  drawAngle(TWO_PI - 0.01),

  lostStandard(false)
{
  type = LAKE_TYPE_PLAYER;
  mouthTimer.setListener(this);
  scaler.setInactive();
  dmgTimer.setInactive();
  setRotation(TWO_PI - 0.01);
}

void LakePlayer::load()
{
  setFishSize(SIZE_32); // level should be set before calling this
}

void LakePlayer::updateMe()
{
  if (level->levelState == PizzaLevelInterface::LEVEL_PLAY)
  {
    updateStd();
  }
  else if (level->levelState == PizzaLevelInterface::LEVEL_WIN)
  {
    scaler.update();
    scale.y = scale.x;
  }
  else if (lostStandard == true)
  {
    face.actionTrigger(LakeFace::HURT); 
    setRotation(0.0);
    drawAngle = 0.0;
    addY(-64.0 * RM::timePassed());
    hflip(false);
    vflip();
  }

  face.updateFace();
}

void LakePlayer::redrawMe()
{
  Point1 realAlpha = getAlpha();
  Point1 realRotation = getRotation();
  Image* realImg = getImage();
  Point2 realScale = getScale();
  
  if (tempInvincible() == true) setAlpha(0.5);
  setRotation(drawAngle);

  face.drawFace();
  
  if (fadeInImage != NULL) 
  {
    setScale(getScale() * 0.5);
    setAlpha(std::min(getAlpha(), fadeInAlpha));
    setImage(fadeInImage);
    
    face.drawFace();    
  }

  setScale(realScale);
  setImage(realImg);
  setRotation(realRotation);
  setAlpha(realAlpha);
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 1.0, 0.0, 0.25));
    mouthCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 0.25));
  }
}

void LakePlayer::updateStd()
{
  scaler.update();
  scale.y = scale.x;
  fadeInAlpha = scaler.progress();
  
  dmgTimer.update();
  mouthTimer.update();
  
  if (mouthTimer.getActive() == true) face.actionTrigger(LakeFace::MOUTH_OPEN);
  drawAngle = RM::chase_angle(rotation, face.targetHFlip == false ? 
                              TWO_PI - 0.01 : PI + 0.01, 10.0 * RM::timePassed());
  setRotation(drawAngle);
  
  if (getRotation() < PI * 1.5) 
  {
    vflip();
  }
  else 
  {
    vflip(false);
  }
  
  updateMotion(); 
}

void LakePlayer::updateMotion()
{
  vxy.y += 200.0 * sizeMult * RM::timePassed();  // gravity  
  vxy.y = std::min(vxy.y, 100.0 * sizeMult);  // max y vel
  
  xy += vxy * RM::timePassed();
  
  // enforce world boundaries
  RM::clamp_me(xy.x, 0.0, LAKE_WORLD_SIZE.x);
  RM::clamp_me(xy.y, 0.0, LAKE_WORLD_SIZE.y);
  
  hflip(vxy.x < 0.0);
}

void LakePlayer::tryJump()
{
  if (getY() < 0.0) return;  // out of water
  
  ResourceManager::playSwim();
  vxy.y = -200.0 * sizeMult;
  if (face.faceState == LakeFace::IDLE) face.idleTrigger(LakeFace::SWIM_1);
}

void LakePlayer::tryMove(Point1 mag)
{
  vxy.x = 200.0 * sizeMult * mag;
}

void LakePlayer::smallerNearby()
{
  mouthTimer.reset();
}

void LakePlayer::ateFish(LakeGO* fish)
{  
  mouthTimer.setInactive();
  ResourceManager::playFishEat();
  
  if (fish->poison == true)
  {
    attacked(getXY());
    face.actionTrigger(LakeFace::HURT);
    return;
  }
  
  Point1 oldEaten = amountEaten;
  amountEaten += fish->sizeIndex + 1.0;
  level->gotScore((fish->sizeIndex + 1) * 20);
  
  if (oldEaten < sizeIndex * 2.0 && amountEaten >= sizeIndex * 2.0)
  {
    Coord1 newSize = sizeIndex + 1;
    amountEaten = 0.0;
    
    // this is just for debug
    if (newSize > SIZE_256) 
    {
      scaler.reset();
      level->winLevel();
      newSize = SIZE_256;
    }
    else
    {
      sizeUp();
    }
  }
  else
  {
    face.actionTrigger(LakeFace::SWALLOW);
  }
}

Logical LakePlayer::attacked(Point2 atkPoint, Logical instantLose)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return false;
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return false;
  if (tempInvincible() == true) return false;
  
  face.actionTrigger(LakeFace::HURT);
  level->playerDamaged(instantLose);
  level->addSauceEffect(*this, atkPoint);

  if (level->levelState == PizzaLevelInterface::LEVEL_PLAY) 
  {
    // take normal damage but didn't lose
    if (instantLose == false) dmgTimer.reset();
  }
  else if (level->levelState == PizzaLevelInterface::LEVEL_LOSE && instantLose == false) 
  {
    // take normal damage and did lose
    loseToDamage();
  }
  
  return true;
}

Logical LakePlayer::tempInvincible()
{
  return dmgTimer.getActive();
}

void LakePlayer::loseToDamage()
{
  lostStandard = true;
}

void LakePlayer::sizeUp()
{
  face.actionTrigger(LakeFace::TRANSFORM);
  scaler.reset();
  fadeInImage = getFishImg(nextSize(sizeIndex));
}

Coord1 LakePlayer::nextSize(Coord1 currSize)
{
  Coord1 newSize = (currSize + 1) % (SIZE_512 + 1);
  if (newSize < SIZE_32) newSize = SIZE_32;
  return newSize;
}

void LakePlayer::setFishSize(Coord1 newSize)
{
  sizeIndex = newSize;
  
  setImage(getFishImg(sizeIndex));  
  setScale(1.0, 1.0);

  Point1 nominalSize = 16.0 * std::pow(2.0, sizeIndex);
  setWH(nominalSize, nominalSize);
  
  sizeMult = 1.0 + 0.25 * (sizeIndex - SIZE_32);
  level->playerGrew(sizeIndex);
}

Image* LakePlayer::getFishImg(Coord1 size)
{
  switch (size)
  {
    default:
    case SIZE_32: return imgCache[lakePizza32]; break;
    case SIZE_64: return imgCache[lakePizza64]; break;
    case SIZE_128: return imgCache[lakePizza128]; break;
    case SIZE_256: return imgCache[lakePizza256]; break;
    case SIZE_512: return imgCache[lakePizza256]; break;
  }
}

void LakePlayer::setFacing(Coord1 newFacing)
{
  face.targetHFlip = (newFacing == -1);
}

Coord1 LakePlayer::facingToDir()
{
  return face.targetHFlip == false ? 1 : -1;
}

Circle LakePlayer::mouthCircle()
{
  Point2 offset(0.0, 0.0);
  Point1 mouthRadius = 16.0;
  
  if (sizeIndex != SIZE_32)
  {
    mouthRadius = getWidth() * 0.25;
    offset.set(getWidth() * 0.25, getWidth() / 16.0);
  }
  
  return Circle(getXY() + Point2(facingToDir() * offset.x, offset.y), mouthRadius);
}

void LakePlayer::callback(ActionEvent* caller)
{
  if (caller == &scaler)
  {    
    setFishSize(nextSize(sizeIndex));
    fadeInAlpha = 0.0;
    fadeInImage = NULL;
  }
  else if (caller == &mouthTimer && face.faceState == LakeFace::MOUTH_OPEN)
  {
    face.idleTrigger(LakeFace::SWIM_0);
  }
}

// =============================== LakeFace ============================ //

LakeFace::LakeFace(VisRectangular* setParent) :
  ActionListener(),

  parent(setParent),
  faceWarper(parent, dwarpCache[lakeFaceWarp]),
  faceScript(this),

  faceState(IDLE),
  idleState(SWIM_0),
  frameTarget(0),
  targetHFlip(false),
  currHFlip(false),

  currWeight(0.0),
  breatheData(0.0, 1.0),

  useOverride(false),
  overrideDuration(0.2)
{
  resetFace();
  idleTrigger(SWIM_0);
}

void LakeFace::resetFace()
{
  faceWarper.set(parent, dwarpCache[lakeFaceWarp]);
  faceWarper.lastWarpFrame = faceWarper.sequence->warpFrames[0];  
  faceWarper.contWarpFrame = faceWarper.sequence->warpFrames[0];
}

void LakeFace::updateFace()
{
  faceScript.update();
}

void LakeFace::drawFace()
{  
  Point1 myRotation = parent->getRotation();
  Logical parentHFlip = parent->getHFlip();

  parent->hflip(currHFlip);

  faceWarper.draw_cont(frameTarget, currWeight);

  parent->hflip(parentHFlip);
}

Point1 LakeFace::damagePercent()
{
  return std::min(Pizza::currGameVars[VAR_DAMAGE] / 8.0, 1.0);
}

Logical LakeFace::isHurt()
{
  return faceState == HURT;
}

void LakeFace::actionTrigger(Coord1 actionType)
{
  if (faceState >= actionType) return;
  
  faceScript.clear();
  faceWarper.last_to_cont();
  currWeight = 0.0;
  faceState = actionType;
  
  switch (faceState)
  {      
    case MOUTH_OPEN:
      frameTarget = 2;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(10.0);
      break;
    case SWALLOW:
      frameTarget = 3;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.15));
      break;
    case HURT:
      frameTarget = 4;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(0.4);
      break;
    case SWALLOW_TRANS:
      frameTarget = 3;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.15));
      break;
    case TRANSFORM:
      frameTarget = 5;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, 1.35));
      break;
  }
  
  useOverride = false;
}

void LakeFace::idleTrigger(Coord1 idleType)
{
  faceScript.clear();
  faceWarper.last_to_cont();
  currWeight = 0.0;
  
  idleState = idleType;
  faceState = IDLE;
  
  switch (idleState)
  {
    case SWIM_0:
      frameTarget = frameTarget == 0 ? 1 : 0;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.5));
      break;
    case SWIM_1:
      frameTarget = 1;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.05));
      break;
    case TURN:
      frameTarget = 6;
      overrideDuration = 0.2;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.1));
      faceScript.enqueueX(new SetValueCommand<Logical>(&currHFlip, targetHFlip));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&frameTarget, 0));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&idleState, SWIM_0));
      faceScript.enqueueX(new SetValueCommand<Logical>(&useOverride, true));
      break;
  }  
  
  useOverride = false;
}

void LakeFace::callback(ActionEvent* caller)
{
  if (caller == &faceScript)
  {
    if (faceState == SWALLOW_TRANS)
    {
      actionTrigger(TRANSFORM);
    }
    else
    {
      idleTrigger(SWIM_0);      
    }
    /*
    else if (targetHFlip == currHFlip)
    {
      idleTrigger(SWIM_0);
    }
    else
    {
      idleTrigger(TURN);
    }
     */
  }
}

// =============================== LakeFish ============================ //

LakeFish::LakeFish(Coord1 size) :
  LakeGO(),

  mover(this),
  bodyWarper(this, ResourceManager::fishBodyWarps[size]),
  headWarper(this, ResourceManager::fishHeadWarps[size]),

  bodyWeight(0.0),
  bodyWeightData(0.0, 1.0),

  headWeight(0.0),

  headImg(NULL),
  onscreen(true),

  swimDir(RM::randl() ? -1.0 : 1.0),
  currSpeed(0.0),
  currAngle(0.0),
  
  willChase(false),
  willRun(false)
{
  sizeIndex = size;
  
  setImage(imgsetCache[lakeFishBodySet][sizeIndex]);
  headImg = imgsetCache[lakeFishHeadSet][sizeIndex];

  Point1 nominalSize = 16.0 * std::pow(2.0, sizeIndex);
  setWH(nominalSize, nominalSize);
}

void LakeFish::load()
{
  mover.player = player;
  onscreen = Box::collision(imageBox(), level->camera.myBox());
  hflip(swimDir < 0.0);
  
  currSpeed = normalSpeed();
  currAngle = swimDir > 0.0 ? 0.0 : PI;
  
  if (poison == true) 
  {
    poisonTimer.setActive();
    poisonTimer.setListener(this);
    setColor(RM::color255(247, 255, 153));
  }
}

void LakeFish::updateMe()
{
  Point1 oscillateSpeed = 5.0 - 0.5 * sizeIndex;
  RM::bounce_linear(bodyWeight, bodyWeightData, Point2(0.0, 1.0), oscillateSpeed * RM::timePassed());
  
  poisonTimer.update();
  mouthTimer.update();
  mover.update();

  hflip(RM::standardize_rads(currAngle) < PI * 1.5 && 
        RM::standardize_rads(currAngle) > PI * 0.5);
  
  setRotation(getHFlip() ? currAngle - PI : currAngle);
  
  // if (mouthTimer.getActive() == true)// MOUTH OPEN HERE;
  RM::flatten_me(headWeight, mouthTimer.getActive() ? 1.0 : 0.0, 2.0 * RM::timePassed());
  
  updateCollisions();
  
  if (outsideWorld() == true) 
  {
    level->createNormalFish(*this);
    setRemove();
  }
}

void LakeFish::redrawMe()
{
  if (onscreen == false) return;
  
  bodyWarper.draw(bodyWeight);
  // bodyWarper.draw(bodyWeight);

  Image* bodyImg = getImage();
  setImage(headImg);
  
  headWarper.draw(headWeight);
  
  setImage(bodyImg);
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    mouthCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

Logical LakeFish::facingPlayer()
{
  return (mouthCircle().xy.x > player->getX()) == getHFlip();
}

void LakeFish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  // this roughly includes the head and rotation
  Box bigBox = Box::from_enclosed_circle(Circle(getXY(),
                                                imageBox().wh.x + mouthCircle().radius));
  onscreen = Box::collision(bigBox, level->camera.myBox());

  if (onscreen == false) return;
  
  // fish is smaller
  if (sizeIndex < player->sizeIndex)
  {
    if (playerNearEating() == true)
    {
      player->smallerNearby();
 
      if (touchingPlayerMouth() == true)
      {
        swallowedByPlayer();
        
        level->createNormalFish(*this);
      }
    }
  }
  // fish is bigger
  else if (sizeIndex > player->sizeIndex)
  {
    // the second clause makes sure big fish always open their mouth if you are close so
    // that if you are bitten it plays the whole animation
    if (facingPlayer() == true || 
        touchingPlayer(Circle(mouthCircle().xy, mouthCircle().radius * 2.0)))
    {
      mouthTimer.reset();
    }
    
    if (mouthCircle().collision(player->collisionCircle()) == true)
    {
      // fish is 2+ bigger
      if (sizeIndex > player->sizeIndex + 1)
      {
        Logical hit = player->attacked(getXY(), true);
        if (hit == true)
        {
          ResourceManager::playFishEat();
          swallowedPlayer();
        }
      }
      // first is 1 bigger
      else
      {
        player->attacked(getXY());
      }
    }
  }
}

void LakeFish::swallowedPlayer()
{
  FishSwallow* swallowEffect = new FishSwallow(player, mouthCircle().xy);
  level->addAction(swallowEffect);
  
  Point2 target = RM::attract(this->getXY(), player->mouthCircle().xy, sizeIndex * 16.0);
  level->addAction(new CosineFn(&xy.x, target.x, 0.20));
  level->addAction(new CosineFn(&xy.y, target.y, 0.20));
  
  for (Coord1 i = 0; i < (sizeIndex + 1) * 3; ++i)
  {
    level->addAlly(new LakeBubble(collisionCircle().random_pt(), RM::randi(0, 1)));    
  }
}

Point1 LakeFish::normalSpeed()
{
  return 50.0;
}

Point1 LakeFish::maxSpeed()
{
  return normalSpeed() * (1.0 + 0.5 * sizeIndex);
}

Circle LakeFish::mouthCircle()
{
  Point2 end = getXY() + Point2(getWidth() * 0.375 * swimDir, 0.0);
  return Circle(RM::rotate(getXY(), end, getHFlip() ? currAngle - PI : currAngle), getWidth() / 8.0);
}

void LakeFish::callback(ActionEvent* caller)
{
  if (caller == &poisonTimer)
  {
    Point2 poisonCoords = getXY() + getWidth() * 0.4 * RM::randf(-1.0, 1.0);
    level->addAction(new PoisonPuff(poisonCoords));
    poisonTimer.reset(RM::randf(0.25, 1.0));
  }
}

// =============================== FishMover ============================ //

FishMover::FishMover(LakeFish* setParent) :
  parent(setParent),
  speed(-64.0),
  yOffset(0.0),
  bobData(RM::randf(), 1.0),
  bobMult(RM::randf(0.8, 1.2))
{
  
}

void FishMover::update()
{
  if (parent->onscreen == false)
  {
    swimForward();
    return;
  }
    
  // fish smaller
  if (parent->willRun == true &&
      parent->sizeIndex < player->sizeIndex)
  {
    // swimForward();
    awayFromPlayer();
  }
  // fish bigger
  else if (parent->willChase == true &&
           parent->sizeIndex > player->sizeIndex)
  {
    if (parent->sizeIndex == player->sizeIndex + 1)
    {
      if (parent->facingPlayer()) fastTowardPlayer();
      else swimForward();
    }
    else
    {
      if (parent->facingPlayer()) lazyTowardPlayer();
      else swimForward();
    }
  }
  else
  {
    swimForward();
  }
}

void FishMover::swimForward()
{
  swim(parent->normalSpeed(), parent->swimDir > 0.0 ? 0.0 : PI, HALF_PI * 0.5);
  
  parent->addY(-yOffset);
  RM::bounce_arcsine(yOffset, bobData, Point2(-8.0, 8.0) * (parent->sizeIndex + 1.0), 
                  0.5 * bobMult * RM::timePassed());
  parent->addY(yOffset);
}

void FishMover::lazyTowardPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->normalSpeed(), RM::angle(mouthCenter, player->getXY()), HALF_PI);
}

void FishMover::fastTowardPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->maxSpeed(), RM::angle(mouthCenter, player->getXY()), PI);
}

void FishMover::awayFromPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->maxSpeed(), RM::angle(player->getXY(), mouthCenter), PI);  
}

void FishMover::swim(Point1 speedTarget, Point1 angleTarget, Point1 turnSpeed)
{
  RM::flatten_me(parent->currSpeed, speedTarget, speedTarget * RM::timePassed());
  parent->currAngle = RM::chase_angle(parent->currAngle, 
                                      angleTarget, 
                                      turnSpeed * RM::timePassed());
  
  parent->addXY(RM::pol_to_cart(parent->currSpeed, parent->currAngle) * RM::timePassed());

}

// =============================== LakePuffer ============================ //

LakePuffer::LakePuffer() :
  LakeGO(),
  ActionListener(),

  waiter(RM::randf(0.0, 2.0), this),
  transTimer(0.15, this),
  puffState(FULL_UNPUFF),

  deflatedWarper(this, dwarpCache[pufferDeflatedWarp]),
  inflatedWarper(this, dwarpCache[pufferInflatedWarp]),
  warpWeight(0.0),
  warpData(RM::randf(0.0, 1.0), 1.0),

  bobData(0.0, 1.0),
  startY(0.0)
{
  transTimer.setInactive();
  setImage(imgCache[lakePufferDeflated]);
  sizeIndex = SIZE_64;
  hflip(RM::randl());
}

void LakePuffer::load()
{
  startY = getY();
}

void LakePuffer::updateMe()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;

  transTimer.update();
  waiter.update();
  
  RM::bounce_arcsine(xy.y, bobData, Point2(-8.0, 8.0) + startY, RM::timePassed());
  RM::bounce_linear(warpWeight, warpData, Point2(0.0, 1.0), RM::timePassed());
  updateCollisions();
}

void LakePuffer::redrawMe()
{
  PizzaWarper* currWarp = &deflatedWarper;
  
  if (transTimer.getActive() == true) 
  {
    setImage(imgCache[lakePufferTrans]);
  }
  else if (puffState == FULL_UNPUFF) 
  {
    setImage(imgCache[lakePufferDeflated]);
  }
  else 
  {
    setImage(imgCache[lakePufferInflated]);
    currWarp = &inflatedWarper;
  }
  
  currWarp->draw(warpWeight);
}

void LakePuffer::updateCollisions()
{
  Logical sizeOverride = player->sizeIndex > sizeIndex;
  
  if (sizeIndex <= player->sizeIndex && 
      playerNearEating() == true &&
      ((puffState != FULL_PUFF) || (sizeOverride == true)))
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->createPuffer();
    }
  }
  else if (puffState == FULL_PUFF && touchingPlayer() == true)
  {
    if (touchingPlayer(Circle(getXY(), 32.0)) == true)
    {
      player->attacked(getXY());
    }
  }
}

void LakePuffer::callback(ActionEvent* caller)
{
  if (caller == &waiter)
  {
    if (puffState == FULL_UNPUFF) puffState = FULL_PUFF;
    else puffState = FULL_UNPUFF;
    
    transTimer.reset(0.06);    
    waiter.reset(RM::randf(1.75, 2.25));
  }
}

// =============================== LakeSwordfish ============================ //

LakeSwordfish::LakeSwordfish() : 
  LakeGO(),
  ActionListener(),
  xVel(0.0),
  startY(0.0),
  bobData(0.0, 1.0),
  speedTimer(2.0, this),
  fastSwim(false),
  warper(this, dwarpCache[swordfishWarp]),
  warpWeight(0.0),
  warpData(0.0, 1.0),
  oldXY(0.0, 0.0)
{
  setImage(imgCache[lakeSwordfishImg]);
  setWH(32.0, 32.0);
  sizeIndex = SIZE_64;
  hflip(RM::randl());
}

void LakeSwordfish::load()
{
  startY = getY();
  oldXY = getXY();
}

void LakeSwordfish::updateMe()
{
  speedTimer.update();
  
  Point1 warpSpeed = 2.0 * (speedTimer.getActive() ? 1.0 : 2.0);
  RM::bounce_linear(warpWeight, warpData, Point2(0.0, 1.0), warpSpeed * RM::timePassed());
  
  oldXY = getXY();
  RM::flatten_me(xVel, (fastSwim ? 128.0 : 64.0) * facingToDir(), 256.0 * RM::timePassed());
  addX(xVel * RM::timePassed());
  RM::bounce_arcsine(xy.y, bobData, Point2(-24.0, 24.0) + startY, 0.5 * RM::timePassed());
  
  if (getHFlip() == false) setRotation(RM::angle(oldXY, getXY()));
  else setRotation(RM::angle(oldXY, getXY()) - PI);
  
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->createSwordfish();
    setRemove();
  }
}

void LakeSwordfish::redrawMe()
{
  warper.draw(warpWeight);
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    mouthCircle().draw_outline(RED_SOLID);
  }
}

void LakeSwordfish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;

  if (sizeIndex < player->sizeIndex &&
      playerNearEating() == true &&
      ((player->facingToDir() == 1 && player->getX() < collisionCircle().xy.x) ||
      (player->facingToDir() == -1 && player->getX() > collisionCircle().xy.x)))
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->createSwordfish();
    }
  }
  else if (touchingPlayer(mouthCircle()) == true)
  {
    player->attacked(mouthCircle().xy);
  }
}

Circle LakeSwordfish::collisionCircle()
{
  return Circle(getXY() + Point2(facingToDir() * -16.0, 0.0), getWidth());
}

Circle LakeSwordfish::mouthCircle()
{
  Point2 rotationMod = RM::rotate(Point2(56.0, 0.0), getHFlip() ? PI + getRotation() : getRotation());
  return Circle(getXY() + rotationMod, 16.0);
}

void LakeSwordfish::callback(ActionEvent* caller)
{
  if (caller == &speedTimer)
  {
    fastSwim = !fastSwim;
    speedTimer.reset(fastSwim ? 1.0 : 2.0);
  }
}

// =============================== Starfish ============================ //

Starfish::Starfish() :
  LakeGO(),
  origin(0.0, 0.0),
  currAngle(0.0),
  circleRad(0.0),
  distance(0.0),
  distData(0.0, 1.0),
  eyesOpenTimer(2.0, this),
  eyesClosedTimer(0.4, this)
{
  circleRad = RM::randf(32.0, 96.0);
  currAngle = RM::randf(0.0, TWO_PI);
  setImage(imgsetCache[lakeStarfishSet][0]);
  setWH(92.0, 92.0);
  sizeIndex = SIZE_128;
  
  eyesClosedTimer.setInactive();
}

void Starfish::load()
{
  origin = getXY();
  setXY(origin);
}

void Starfish::updateMe()
{
  eyesOpenTimer.update();
  eyesClosedTimer.update();
  
  currAngle += PI * 0.33 * RM::timePassed();
  setRotation(currAngle);
  RM::bounce_arcsine(distance, distData, Point2(0.0, circleRad), 0.2 * RM::timePassed());
  setXY(origin + RM::pol_to_cart(distance, currAngle));
  
  updateCollisions();
}

void Starfish::redrawMe()
{
  drawMe();

  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void Starfish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;

  if (playerNearEating() && player->sizeIndex > sizeIndex)
  {
    player->smallerNearby();
  }

  if (player->sizeIndex > sizeIndex && touchingPlayerMouth() == true)
  {
    swallowedByPlayer();
    level->createStarfish();
  }
  else if (player->sizeIndex <= sizeIndex && touchingPlayer() == true)
  {
    player->attacked(getXY());
  }
}

void Starfish::callback(ActionEvent* caller)
{
  if (caller == &eyesOpenTimer)
  {
    eyesClosedTimer.reset();
    setImage(imgsetCache[lakeStarfishSet][1]);
  }
  else if (caller == &eyesClosedTimer)
  {
    eyesOpenTimer.reset();
    setImage(imgsetCache[lakeStarfishSet][0]);    
  }
}

// =============================== Harpoon ============================ //

Harpoon::Harpoon(Point2 setTarget) :
  LakeGO(),
  warningTimer(2.5, this),
  warningAlpha(0.0),
  targetPt(setTarget.x, setTarget.y + 128.0),
  hitPlayer(false)  
{
  setXY(targetPt.x, -64.0);
  setWH(32.0, 32.0);
  setImage(imgCache[lakeHaroon]);
  type = LAKE_TYPE_HARPOON;
  
  lifeState = RM::TRANS_IN;
}

void Harpoon::updateMe()
{
  warningTimer.update();
  RM::flatten_me(warningAlpha, warningTimer.getActive() ? 1.0 : 0.0, 4.0 * RM::timePassed());
  
  if (lifeState == RM::TRANS_IN)
  {
    Box camBox = level->camera.myBox();
    setY(camBox.top() - 64.0);
  }
  else if (lifeState == RM::ACTIVE)
  {
    Logical arrived = RM::attract_me(xy, targetPt, 512.0 * RM::timePassed());
    
    if (touchingPlayer() == true)
    {
      hitPlayer = hitPlayer || player->attacked(getXY(), true);
    }
    if (arrived == true) 
    {
      lifeState = RM::PASSIVE;
    }
  }
  else if (lifeState == RM::PASSIVE)
  {
    // Logical arrived = RM::flatten_me(color.w, 0.0, 2.0 * RM::timePassed());
    Logical arrived = RM::attract_me(xy, Point2(getX(), level->camera.myBox().top() - 64.0), 
                                     512.0 * RM::timePassed());
    if (arrived == true) 
    {
      setRemove();
    }    
  }
  
  if (hitPlayer == true) player->setXY(getXY());
}

void Harpoon::redrawMe()
{
  if (warningAlpha > 0.01)
  {
    Box camBox = level->camera.myBox();
    imgCache[warningV]->draw_scale(Point2(getX(), camBox.top()), 
                                   Point2(1.0, 1.0), 0.0, ColorP4(1.0, 1.0, 1.0, warningAlpha));
  }
  
  if (lifeState != RM::TRANS_IN)
  {
    imgCache[lakeHarpoonRope]->draw_repeating_tgt(Point2(getX(), -64.0), 
                                                  getXY() - Point2(0.0, 84.0), getColor());
    drawMe();
  }
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void Harpoon::callback(ActionEvent* caller)
{
  if (caller == &warningTimer)
  {
    ResourceManager::playHarpoonShoot();
    lifeState = RM::ACTIVE;
  }
}

// =============================== LakeBarrel ============================ //

LakeBarrel::LakeBarrel() :
  LakeGO(),
  ActionListener(),

  explodeTimer(1.5, this),
  fallSpeed(64.0),
  rotateSpeed(RM::randf(-TWO_PI, TWO_PI)),

  blinkTimer(0.2, this),
  blinkIndex(0)
{
  setWH(64.0, 64.0);
  setImage(imgsetCache[lakeBarrelSet][blinkIndex]);
  
  blinkTimer.setInactive();
  explodeTimer.setInactive();
}

void LakeBarrel::updateMe()
{
  addY(fallSpeed * RM::timePassed());
  addRotation(rotateSpeed * RM::timePassed());
  explodeTimer.update();
  blinkTimer.update();
  updateCollisions();
}

void LakeBarrel::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    nearbyCircle().draw_outline(WHITE_SOLID);
    explodeCircle().draw_outline(ORANGE_SOLID);
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void LakeBarrel::updateCollisions()
{
  if (touchingPlayer() == true)
  {
    explode();
  }
  else if (touchingPlayer(nearbyCircle()) == true)
  {
    explodeTimer.setActive();
    blinkTimer.setActive();
  }
  else if (getY() >= LAKE_SIZE.y)
  {
    explode();
  }
}

void LakeBarrel::explode()
{
  // this could happen if it's touched on the same frame as the timer goes off
  if (getActive() == false) return;
  
  if (touchingPlayer(explodeCircle()) == true)
  {
    player->attacked(getXY());
  }
  
  setImage(imgsetCache[lakeBarrelSet][1]);
  level->addAction(FishShatter::createForBounce(*this, 3, 3));
  
  BombExplosion* explode = new BombExplosion(getXY());
  explode->setScale(0.75, 0.75);
  explode->scaler.reset(1.5, 0.75);
  level->addAction(explode);
  
  if (imageBox().collision(level->camera.myBox()) == true)
  {
    ResourceManager::playBombExplode();
  }
  
  for (Coord1 i = 0; i < 10; ++i)
  {
    level->addAlly(new LakeBubble(explodeCircle().random_pt(), RM::randi(0, 1)));    
  }
  
  level->createBarrel();
  setRemove();
}

Circle LakeBarrel::nearbyCircle()
{
  return Circle(getXY(), getWidth() * 2.5);
}

Circle LakeBarrel::explodeCircle()
{
  return Circle(getXY(), getWidth() * 1.5);  
}

void LakeBarrel::callback(ActionEvent* caller)
{
  if (caller == &explodeTimer)
  {
    explode();
  }
  else if (caller == &blinkTimer)
  {
    blinkIndex++;
    blinkIndex %= 2;
    setImage(imgsetCache[lakeBarrelSet][blinkIndex]);
    
    Point1 resetTime = explodeTimer.progress() < 0.66 ? 0.1 : 0.05;
    blinkTimer.reset(resetTime);
  }
}

// =============================== LakeEel ============================ //

LakeEel::LakeEel() :
  LakeGO(),
  ActionListener(),

  warper(this, dwarpCache[eelWarp]),
  animator(this, imgsetCache[lakeEelSet], 0.08),
  warpVal(0.0),

  xMover(&xy.x, 0.0, 3.5),
  yMover(&xy.y, 0.0, 3.5, this),
  waitTimer(2.0, this),

  oldXY(0.0, 0.0),
  targetXY(0.0, 0.0),
  shockAlpha(0.0)
{
  setImage(imgsetCache[lakeEelSet][0]);
  setWH(32.0, 32.0);
  sizeIndex = SIZE_64;
  
  animator.repeats();
  xMover.setInactive();
  yMover.setInactive();
  waitTimer.setInactive();
}

void LakeEel::load()
{
  oldXY = getXY();
  callback(&yMover);
}

void LakeEel::updateMe()
{
  xMover.update();
  yMover.update();
  waitTimer.update();
  
  hflip(RM::standardize_rads(getRotation()) < PI * 1.5 &&
        RM::standardize_rads(getRotation()) > PI * 0.5);
  
  if (xMover.getActive() == true)
  {
    // darting
    animator.update();
    RM::flatten_me(shockAlpha, 1.0, 4.0 * RM::timePassed());
    warpVal += 2.0 * RM::timePassed();
  }
  else
  {
    // still
    setImage(imgsetCache[lakeEelSet][0]);
    Point1 targetAngle = RM::angle(getXY(), targetXY);
    setRotation(RM::chase_angle(getRotation(), targetAngle, PI * RM::timePassed()));
    RM::flatten_me(shockAlpha, 0.0, 4.0 * RM::timePassed());
    warpVal += 0.5 * RM::timePassed();
  }
  
  RM::wrap1_me(warpVal, 0.0, 1.0);
  
  if (outsideWorld() == true)
  {
    level->createEel();
    setRemove();
  }
  else
  {
    updateCollisions();
  }
}

void LakeEel::redrawMe()
{
  if (xMover.getActive() == true)
  {
    setColor(YELLOW_SOLID);
  }
  else
  {
    setColor(COLOR_FULL);
  }
  
  Point1 realRotion = getRotation();
  setRotation(getHFlip() ? getRotation() - PI : getRotation());
  warper.draw(warpVal);
  setRotation(realRotion);
  
  if (Pizza::DEBUG_COLLISION == true) collisionCircle().draw_outline(YELLOW_SOLID);
}

void LakeEel::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;

  // moving with shock
  if (xMover.getActive() == true)
  {
    if (touchingPlayer() == true)
    {
      player->attacked(getXY());
    }
  }
  // still, can be eaten
  else
  {
    if (playerNearEating() == true)
    {
      if (player->sizeIndex > sizeIndex) player->smallerNearby();
      
      if (player->sizeIndex > sizeIndex && touchingPlayerMouth() == true)
      {
        swallowedByPlayer();
        level->createEel();
      }
    }
  }
}

void LakeEel::callback(ActionEvent* caller)
{
  if (caller == &yMover)
  {
    targetXY = RM::pol_to_cart(400.0, RM::rand_angle()) + getXY();
    waitTimer.reset();
  }
  else if (caller == & waitTimer)
  {
    if (imageBox().collision(level->camera.myBox()) == true)
    {
      ResourceManager::playEelShock();
    }
    
    xMover.reset(targetXY.x);
    yMover.reset(targetXY.y);
  }
}

// =============================== LakePearl ============================ //

LakePearl::LakePearl() :
  LakeGO(),
  sparkleOnTimer(0.4, this),
  sparkleOffset(0.0, 0.0)
{
  type = LAKE_TYPE_PEARL;
  setXY(-100.0, LAKE_WORLD_BOX.bottom() + 64.0);
  setImage(imgCache[lakePearlImg]);
  setWH(16.0, 16.0);
}

void LakePearl::updateMe()
{
  sparkleOnTimer.update();
  addY(-32.0 * RM::timePassed());
  
  if (getY() < -64.0) 
  {
    level->createPearl();
    setRemove();
  }
  else
  {
    updateCollisions();
  }
}

void LakePearl::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;

  if (playerNearEating() == true)
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      sizeIndex = player->sizeIndex * 2;
      swallowedByPlayer();
      level->createPearl();
    }    
  }
}

void LakePearl::redrawMe()
{
  drawMe();
  Image* sparkleImg = AnimationBehavior::pickImage(imgsetCache[lakePearlSparkleSet], sparkleOnTimer.progress());
  sparkleImg->draw(getXY() + sparkleOffset);
}

void LakePearl::callback(ActionEvent* caller)
{
  if (caller == &sparkleOnTimer)
  {
    sparkleOffset.set(RM::randf(-12.0, 12.0), RM::randf(-12.0, 12.0));
    sparkleOnTimer.reset();
  }
}

// =============================== LakeBubble ============================ //

Point1 LakeBubble::SIZE_MULTS[] =
{
  0.4, 0.75, 1.0, 1.4, 2.0
};

LakeBubble::LakeBubble() :
  LakeGO(),

  value(0),
  ySpeed(-64.0),
  xOffset(0.0),

  bobData(RM::randf(-1.0, 1.0), RM::randl() ? 1.0 : -1.0),
  bobMult(0.0),
  
  baseScale(RM::randf(0.5, 1.0)),
  squishPercent(0.0),
  squishData(RM::randf(0.0, 1.0), RM::randl() ? -1.0 : 1.0)
{
  type = LAKE_TYPE_BUBBLE;
  setSize(RM::randi(0, imgsetCache[lakeBubbleSet].count - 1));
}

LakeBubble::LakeBubble(Point2 center, Coord1 size) :
  LakeGO(),

  value(0),
  ySpeed(-64.0),
  xOffset(0.0),

  bobData(RM::randf(-1.0, 1.0), RM::randl() ? 1.0 : -1.0),
  bobMult(0.0),

  baseScale(RM::randf(0.5, 1.0)),
  squishPercent(0.0),
  squishData(RM::randf(0.0, 1.0), RM::randl() ? -1.0 : 1.0)
{
  setXY(center);
  setSize(size);
}

void LakeBubble::setSize(Coord1 size)
{
  value = size;
  bobMult = RM::randf(0.8, 1.2) / SIZE_MULTS[value];
  setImage(imgsetCache[lakeBubbleSet][value]);
  autoSize();
}

void LakeBubble::updateMe()
{
  addY(ySpeed * RM::timePassed());
  
  addX(-xOffset);
  RM::bounce_arcsine(xOffset, bobData, Point2(-8.0, 8.0) * SIZE_MULTS[value], 2.13 * bobMult * RM::timePassed());
  addX(xOffset);

  RM::bounce_arcsine(squishPercent, squishData, Point2(0.0, 1.0), 2.0 / SIZE_MULTS[value] * RM::timePassed());
  Point1 squishAmount = 0.25;
  Point1 xScale = (1.0 - squishAmount * 0.5) + squishPercent * squishAmount;
  setXScale(baseScale * xScale);
  setYScale(baseScale / xScale);
  
  Box camBox = level->camera.myBox();
  if (getY() + 64.0 < camBox.top()) setRemove();
}

// =============================== LakeCoin ============================ //

LakeCoin::LakeCoin() :
  LakeGO(),
  animator(),
  value(PizzaGO::PENNY),

  speed(-64.0),
  xOffset(0.0),
  bobData(0.5, 1.0),
  bobMult(RM::randf(0.8, 1.2))
{
  
}

void LakeCoin::load()
{
  SlopeCoin::setAnimator(this, animator, value, level->getCoinFrame());
}

void LakeCoin::updateMe()
{
  animator.update();
  
  addY(speed * RM::timePassed());
  
  addX(-xOffset);
  RM::bounce_arcsine(xOffset, bobData, Point2(-4.0, 4.0), 2.13 * bobMult * RM::timePassed());
  addX(xOffset);
  
  if (touchingPlayer() == true)
  {
    level->gotCoin(getXY(), SlopeCoin::toCash(value));
    setRemove();
  }
  else if (getY() < -64.0)
  {
    setRemove();
  }
}

// =============================== LakeUnlockable ============================ //

LakeUnlockable::LakeUnlockable(Coord2 setToppingID) :
  LakeGO(),

  toppingID(setToppingID),

  glowRotation(0.0),
  bobOffset(0.0),
  bobData(0.0, 1.0),

  startedMagnet(false),
  startXY(0.0, 0.0),
  currDistPercent(0.0)
{
  setImage(imgCache[secretOutfit]);
  setWH(16.0, 16.0);
}

void LakeUnlockable::updateMe()
{
  glowRotation += TWO_PI * 0.1 * RM::timePassed();
  if (startedMagnet == false) updateFloater();
  else updateMagnet();
}

void LakeUnlockable::redrawMe()
{
  setImage(imgCache[secretOutfitGlow]);
  setRotation(glowRotation);
  drawMe();
  
  setImage(imgCache[secretOutfit]);
  setRotation(0.0);
  drawMe();
}

void LakeUnlockable::updateFloater()
{
  addY(-bobOffset);
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 
                  0.5 * RM::timePassed());
  addY(bobOffset);
  
  if (touchingPlayer() == true)
  {
    startMagnet();
  }
}

void LakeUnlockable::updateMagnet()
{
  Point1 speedMult = level->levelState == PizzaLevel::LEVEL_PLAY ? 1.0 : 2.0;
  Logical atPlayer = RM::flatten_me(currDistPercent, 1.0, speedMult * RM::timePassed());
  Point1 totalDist = RM::distance_to(startXY, player->getXY());
  
  setXY(RM::attract(startXY, player->getXY(), totalDist * currDistPercent));
  
  if (atPlayer == true)
  {
    Pizza::currIngredientFound = toppingID;
    ResourceManager::playOutfitFound();
    level->addAction(new FlagStars(getXY(), false));
    setRemove();
  }
}

void LakeUnlockable::startMagnet()
{
  startedMagnet = true;
  startXY = getXY();
}

// =============================== PoisonPuff ============================ //

PoisonPuff::PoisonPuff(Point2 center) :
  PoofEffect(center, RM::random(imgsetCache[lakePoisonSet])),
  vel(RM::randf(-64.0, 64.0), -128.0)
{
  scaleTime = 3.0;
  startScale = 0.5;
  endScale = 2.0;
  beforeFadeTime = 1.0;
  fadeTime = 2.0;
  init();
}

void PoisonPuff::updateMe()
{
  PoofEffect::updateMe();
  addRotation(PI * RM::timePassed());
  addXY(vel * RM::timePassed());
}

// =============================== FishShatter ============================ //

FishShatter::FishShatter(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  ParticleEffect(),
  burst(NULL)
{
  burst = new ProceduralBurst(visRect, rows, cols, Point2(1.0, 2.0));
  
  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(0.0, 150.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-TWO_PI, TWO_PI);
  
  burst->gravity.set(0.0, 40.0);  
  burst->scaleBounds.set(0.75, 2.0);
   
  emitters.add(burst);
}

FishShatter* FishShatter::createForBounce(const VisRectangular& visRect, Coord1 cols, Coord1 rows)
{
  FishShatter* particles = new FishShatter(visRect, cols, rows);
  particles->burst->speedBounds.set(100.0, 250.0);
  particles->burst->gravity.set(0.0, 100.0);
  particles->burst->durationBounds.set(0.5, 1.0);
  return particles;
}

FishShatter* FishShatter::createForPlane(const VisRectangular& visRect, Coord1 cols, Coord1 rows)
{
  FishShatter* particles = new FishShatter(visRect, cols, rows);
  particles->burst->speedBounds.set(100.0, 250.0);
  particles->burst->gravity.set(0.0, 100.0);
  particles->burst->durationBounds.set(0.5, 1.0);
  particles->burst->fullAlphaPercent = 0.5;
  return particles;
}

// =============================== FishSwallow ============================ //

FishSwallow::FishSwallow(LakeGO* setParent, Point2 setSwallowPt) :
  ActionEvent(),
  ActionListener(),
  parent(setParent),
  swallowPt(setSwallowPt),
  swallowTime(0.4),
  moveSpeed(RM::distance_to(parent->getXY(), swallowPt) / swallowTime),
  scaler(&parent->scale.x, 0.0, swallowTime, this)
{
  
}

void FishSwallow::updateMe()
{
  // RM::attract_me(parent->xy, swallowPt, moveSpeed * RM::timePassed());
  scaler.update();
  parent->setYScale(parent->getXScale());
}

void FishSwallow::callback(ActionEvent* caller)
{
  parent->setRemove();
  done();
}

// =============================== WreckGO ============================ //

WreckGO::WreckGO() :
  PhysRectangular(),
  BaseGOLogic(),
  level(NULL),
  player(NULL),
  
  type(UNSET),
  hitThisAttack(false),
  basePoints(0)
{
  
}

void WreckGO::redrawMe()
{
  if (getImage() != NULL) drawMe();
}

void WreckGO::removeHighStd()
{
  if (getBottomEdge() + 64.0 < level->camera.getY())
  {
    setRemove();
  }
}

void WreckGO::removeLowStd()
{
  if (getY() - 64.0 > level->camera.getY() + PLAY_H)
  {
    setRemove();
  }
}

Logical WreckGO::touchingPlayerPhys(Point2* location)
{
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    if (edge->contact->IsTouching() == true && edge->contact->IsEnabled() == true) 
    {
      WreckGO* pgo1 = static_cast<WreckGO*>(edge->contact->GetFixtureA()->GetUserData());
      WreckGO* pgo2 = static_cast<WreckGO*>(edge->contact->GetFixtureB()->GetUserData());
      
      if (pgo1 == player || pgo2 == player) 
      {
        if (location != NULL) (*location) = edge_to_wpt(edge);
        return true;
      }
    }
  }
  
  return false;
}

void WreckGO::destroyStd()
{
  // NOTE: throw skeleton calls this twice
  
  ResourceManager::playCrunch();
  
  BoneBreakEffect* breaker = new BoneBreakEffect(*this, 2, RM::randi(2, 3));
  // rules out invis walls and building particles
  breaker->setCollisions(1 << PizzaGO::TYPE_TERRAIN, 0xffff & 
                         (~(0x1 << PizzaGO::TYPE_DEBRIS) & 
                         (~(0x1 << PizzaGO::TYPE_TERRAIN))));
  breaker->setVelFromPt(getXY() + Point2(80.0 * -facingToDir(), 80.0), 200.0);
  level->addDebris(breaker);
  
  setRemove();
}

Logical WreckGO::touchingPlayerCircle()
{
  return collisionCircle().collision(player->collisionCircle());
}

Logical WreckGO::touchingPlayerCircle(const Circle& c)
{
  return c.collision(player->collisionCircle());
}

Logical WreckGO::onScreen()
{
  return imageBox().collision(level->camera.myBox());
}

Circle WreckGO::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== WreckChain ============================ //

WreckChain::WreckChain() :
  WreckGO(),
  links(11, true),
  joints(11, true),
  leftWall(),
  rightWall(),
  topWall()
{
  
}

WreckChain::~WreckChain()
{
  // it's count - 1 because the last joint will be destroyed when
  // the player's dtor executes
  for (Coord1 i = 0; i < joints.count - 1; ++i)
  {
    // null if cut from cut attack
    if (joints[i] != NULL) RMPhysics->DestroyJoint(joints[i]);
  }
}

void WreckChain::load()
{
  Point2 chainSize(8.0, 32.0);
  
  for (Coord1 i = 0; i < links.count; ++i)
  {
    Point2 center(240.0, -160.0 + chainSize.y * i);
    links[i].setBox(center, chainSize);
    links[i].setImage(imgCache[wreckChain]);
    
    links[i].init_box(center, chainSize, true);
    links[i].set_collisions(1, 0x0);
    links[i].fixture->SetUserData(this);
    links[i].updateFromPhysical();
  }
  
  links[0].body->SetType(b2_kinematicBody);

  for (Coord1 i = 0; i < joints.count - 1; ++i)
  {
    joints[i] = Physical::join_revolute(links[i].body, links[i + 1].body, 
                                        links[i].xy + Point2(0.0, chainSize.y * 0.5));
  }
  
  /*
  joints.last() = Physical::join_revolute(links.last().body, player->body, 
                                          links.last().xy + Point2(0.0, chainSize.y * 0.5));
  */
  joints.last() = Physical::join_revolute(links.last().body, player->body, links.last().xy);
   
  Point2 chainVel(0.0, level->getChainVel());
  Point2 wallSize(32.0, PLAY_H * 2.0);
  
  leftWall.init_box_TL(Point2(-wallSize.x, -wallSize.y * 0.5), wallSize, false);
  leftWall.set_body_type(b2_kinematicBody);
  leftWall.set_collisions(0x1 << PizzaGO::TYPE_TERRAIN, 0xffff & (~(0x1 << PizzaGO::TYPE_DEBRIS)));
    
  rightWall.init_box_TL(Point2(PLAY_W, -wallSize.y * 0.5), wallSize, false);
  rightWall.set_body_type(b2_kinematicBody);
  rightWall.set_collisions(0x1 << PizzaGO::TYPE_TERRAIN, 0xffff & (~(0x1 << PizzaGO::TYPE_DEBRIS)));
  
  topWall.init_box_TL(Point2(-16.0, -128.0), Point2(SCREEN_W, 64.0), false);
  topWall.set_body_type(b2_kinematicBody);
  topWall.set_collisions(0x1 << PizzaGO::TYPE_TERRAIN, 0xffff & (~(0x1 << PizzaGO::TYPE_DEBRIS)));
}

void WreckChain::updateMe()
{
  Point2 chainVel(0.0, level->getChainVel());
  if (level->levelState == WreckLevel::LEVEL_PLAY) links[0].set_velocity(chainVel);
  else links[0].set_velocity(Point2(0.0, 0.0));
  
  for (Coord1 i = 0; i < links.count; ++i)
  {
    links[i].updateFromPhysical();
  }

  leftWall.set_velocity(Point2(0.0, level->getChainVel()));
  rightWall.set_velocity(Point2(0.0, level->getChainVel()));
  topWall.set_velocity(Point2(0.0, level->getChainVel()));
}

void WreckChain::redrawMe()
{
  for (Coord1 i = 0; i < links.count; ++i)
  {
    links[i].drawMe();
  }

  if (Pizza::DEBUG_COLLISION == true)
  {
    for (Coord1 i = 0; i < joints.count; ++i)
    {
      if (joints[i] != NULL) jointCircle(i).draw_outline(BLUE_SOLID);
    }  
  }
}

Circle WreckChain::linkCircle(Coord1 linkID)
{
  return Circle(links[linkID].getXY(), links[linkID].getHeight() * 0.5);
}

Circle WreckChain::jointCircle(Coord1 jointID)
{
  return Circle(b2dpt_to_world(joints[jointID]->GetAnchorA()), links[0].getHeight() * 0.5);
}

// =============================== WreckPlayer ============================ //

WreckPlayer::WreckPlayer() :
  WreckGO(),
  face(this),
  powerAnim(this, imgsetCache[wreckShieldSet], 0.15),
  powerAlpha(0.0)
{
  setImage(imgCache[pizzaBase]);
  setBox(240.0, 180.0, 36.0, 36.0);
  setScale(0.22, 0.22);
  powerAnim.repeats();
}

void WreckPlayer::load()
{
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << PizzaGO::TYPE_PLAYER, 0xffff);
  fixture->SetUserData(this);
}

void WreckPlayer::updateMe()
{
  powerAnim.update();
  RM::flatten_me(powerAlpha, level->powerActive() ? 1.0 : 0.0, 4.0 * RM::timePassed());
  updateFromPhysical();
  face.updateFace();
}

void WreckPlayer::redrawMe()
{
  face.drawFace();

  if (powerAlpha > 0.01)
  {
    Point2 realScale = getScale();
    setScale(1.0, 1.0);
    setAlpha(powerAlpha);
    setImage(powerAnim.currImage());
    drawMe();
    
    setAlpha(1.0);
    setScale(realScale);
  }
}

void WreckPlayer::tryMove(Point1 mag)
{
  Point1 forceMag = 400.0 * mag;
  if (RM::sign(mag) != RM::sign(get_velocity().x))
  {
    forceMag *= 2.5;
  }
  
  face.targetHFlip = forceMag < 0.0;
  apply_force(Point2(forceMag, 0.0), getXY());
}

void WreckPlayer::tryJump(Point1 multiplier)
{
  if (get_velocity().y < -100.0) return;
  if (getY() < level->camera.getY()) return;
  
  ResourceManager::playJump();
  // apply_impulse(Point2(0.0, -200.0 * multiplier), getXY());
  set_velocity(Point2(get_velocity().x, -425.0 * multiplier));
}

void WreckPlayer::attacked(Point2 atkPoint, Point1 magnitude)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return;
  if (level->powerActive() == true) return;
  if (face.isHurt() == true) return;
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  level->playerDamaged();
  level->smallShake();
  
  apply_impulse(RM::ring_edge(atkPoint, getXY(), magnitude), atkPoint);
  face.actionTrigger(PizzaFace::HURT);
  
  level->addSauceEffect(*this, atkPoint);  
  
  ResourceManager::playDamaged();
  
  Pizza::currGameVars[VAR_DAMAGE]++;
}

Circle WreckPlayer::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

Box WreckPlayer::collisionBox()
{
  return Box::from_center(getXY(), getSize() * 2.0);
}

// =============================== WreckBuilding ============================ //

WreckBuilding::WreckBuilding(Logical leftSide, Point1 top) :
  WreckGO(),

  fireAnim(this, imgsetCache[wreckWindowFireImgs], 0.15),
  padImg(imgCache[wreckBuildingPad]),
  occupant(NULL),

  variation(RM::randi(0, 3)),
  hardWindow(false),
  spikedWindow(false)
{
  basePoints = 5;
  
  fireAnim.repeats();
  fireAnim.setInactive();
  setImage(imgsetCache[wreckWindowAll][variation]);

  type = BUILDING;
  setXY(leftSide ? 0.0 : PLAY_W, top);
  hflip(leftSide == false);
}

void WreckBuilding::load()
{
  setWH(level->buildingSize);
  Point1 boxX = getHFlip() == false ? 0.0 : PLAY_W - getWidth();
  init_box_TL(Point2(boxX, getY()), getSize(), false);
  fixture->SetUserData(this);
}

void WreckBuilding::updateMe()
{
  if (lifeState == RM::PASSIVE)
  {
    RMPhysics->DestroyBody(body);
    body = NULL;
    
    fireAnim.setInactive();
    player->face.actionTrigger(PizzaFace::BUMP);
    
    if (level->powerActive() == true) level->bigShake();
    else level->smallShake();
    
    BoneBreakEffect* breaker = new BoneBreakEffect(*this, 3, 4);
    breaker->setVelFromPt(getXY() + Point2(64.0 * facingToDir(), 64.0), 200.0);
    level->addDebris(breaker);
    level->buildingDefeated(this);
    level->gotScore(basePoints);
    
    level->addAction(new Smokeburst(getXY() + Point2(64.0 * facingToDir(), 64.0), false));
    
    lifeState = RM::TRANS_OUT;
    setImage(imgsetCache[wreckWindowAll][variation + 4]);
    
    if (occupant == NULL)
    {
      ResourceManager::playCrunch();
      
      if (RM::randi(0, 1) == 0)
      {        
        level->createCoin(getXY() + Point2(64.0 * facingToDir(), 64.0), 
                          SlopeCoin::randValueAll(PizzaGO::NICKEL));
      }
    }

    if (occupant != NULL) 
    {
      occupant->ballCollision();
      occupant = NULL;
    }
  }
  
  fireAnim.update();
  
  removeHighStd();
  if (lifeState == RM::REMOVE && occupant != NULL) occupant->setRemove();
}

void WreckBuilding::redrawMe()
{
  Image* myImage = getImage();
  drawMe();

  setImage(padImg);
  drawMe();

  setImage(myImage);
}

void WreckBuilding::setHardWindow()
{
  basePoints = 15;
  hardWindow = true;
  setImage(imgCache[wreckWindowStrong]);
}

void WreckBuilding::setSpikeWindow()
{
  basePoints = 20;
  spikedWindow = true;
  fireAnim.setActive();
}

Box WreckBuilding::collisionBox()
{
  return Box(getXY() + Point2(getHFlip() ? -96.0 : 0.0, 0.0), Point2(96.0, 128.0));
}

void WreckBuilding::collidedPhysical(WreckGO* wgo, Point1 impulse)
{  
  if (impulse > 100.0 && lifeState == RM::ACTIVE)
  {
    if (hardWindow == false || impulse > 150.0) 
    {
      lifeState = RM::PASSIVE;
    }
    if (spikedWindow == true && wgo == player) 
    {
      player->attacked(getXY());
    }
  }
}

// =============================== WreckSkele ============================ //

Point2 WreckSkele::ARM_OFFSETS[] = 
{
  Point2(60.0, 97.0),
  Point2(61.75, 97.0),
  Point2(63.5, 97.0),
  Point2(65.25, 97.0),
  Point2(67.0, 97.0),
  Point2(65.25, 97.0),
  Point2(63.5, 97.0),
  Point2(61.75, 97.0)  
};

WreckSkele::WreckSkele(Logical leftSide, Point1 top) :
  WreckGO(),
  throwAnim()
{
  basePoints = 25;
  hflip(leftSide == false);
  setXY(leftSide ? - 16.0 : PLAY_W + 16.0, top);
  
  ArrayList<Point1> throwTimes(ResourceManager::wreckThrowerFull.count, true);
  throwTimes[0] = 2.00;
  throwTimes[1] = 0.15;
  throwTimes[2] = 0.15;
  throwTimes[3] = 0.15;
  throwTimes[4] = 0.50;
  throwTimes[5] = 0.07;
  throwTimes[6] = 0.07;
  throwTimes[7] = 0.07;
  
  throwAnim.init(this, ResourceManager::wreckThrowerFull, throwTimes);
  throwAnim.repeats();
  autoSize();
}

void WreckSkele::updateMe()
{
  Box camBox = level->camera.myBox();
  
  if (getY() < camBox.bottom() && getY() + 64.0 > camBox.top())
  {
    Coord1 oldAnimIndex = throwAnim.currIndex();
    throwAnim.update();
    
    if (oldAnimIndex == 3 && throwAnim.currIndex() == 4)
    {
      ResourceManager::playWreckThrow();
      
      WreckBone* bone = new WreckBone(getXY() + Point2(160.0 * facingToDir(), 100.0));
      level->addEnemy(bone);
    }
  }
}

void WreckSkele::redrawMe()
{
  drawMe();
  
  Coord1 armIndex = throwAnim.currIndex();
  Image* armImg = ResourceManager::wreckArmFull[armIndex];
  Point2 armOffset = ARM_OFFSETS[armIndex] * Point2(facingToDir(), 1.0);
  Point1 armRot = RM::angle(getXY(), player->getXY()) + (getHFlip() ? PI : 0.0);
  armImg->draw_scale(getXY() + armOffset, RM::flip_scale(getHFlip()), armRot, COLOR_FULL);
}

void WreckSkele::ballCollision()
{
  if (getActive() == true) 
  {
    destroyStd();
    
    level->gotScore(basePoints);
    level->objectDefeated(this);
    level->createCoin(getXY() + Point2(80.0 * facingToDir(), 64.0), 
                      SlopeCoin::randValueAll(PizzaGO::NICKEL));
    
    Coord1 armIndex = throwAnim.currIndex();
    setImage(ResourceManager::wreckArmFull[armIndex]);
    setRotation(RM::angle(getXY(), player->getXY()) + (getHFlip() ? PI : 0.0));
    setXY(getXY() + ARM_OFFSETS[armIndex] * Point2(facingToDir(), 1.0));
    destroyStd();   
  }
}

// =============================== WreckSkeleEasy ============================ //

WreckSkeleEasy::WreckSkeleEasy(Logical leftSide, Point1 top) :
  WreckGO(),
  animator()
{
  basePoints = 5;
  hflip(leftSide == false);
  setXY(leftSide ? - 16.0 : PLAY_W + 16.0, top);
  
  animator.repeats();
}

void WreckSkeleEasy::load()
{
  Coord1 value = level->getEasySkeleIndex();
  if (value == 0)
  {
    animator.init(this, ResourceManager::wreckEasy1Full, 0.15);    
  }
  else if (value == 1)
  {
    animator.init(this, ResourceManager::wreckEasy2Full, 0.15);        
  }
  else if (value == 2)
  {
    ArrayList<Point1> animTimes;
    animTimes.add(0.30);
    animTimes.add(0.13);
    animTimes.add(0.13);
    animTimes.add(0.13);
    animTimes.add(0.50);
    animTimes.add(0.13);
    animTimes.add(0.13);
    animTimes.add(0.13);
    animator.init(this, ResourceManager::wreckEasy3Full, animTimes);        
  }

  animator.randomize();
  autoSize();
}

void WreckSkeleEasy::updateMe()
{
  animator.update();
}

void WreckSkeleEasy::ballCollision()
{
  if (getActive() == true) 
  {
    level->gotScore(basePoints);
    level->objectDefeated(this);
    level->createCoin(getXY() + Point2(80.0 * facingToDir(), 64.0), 
                      SlopeCoin::randValueAll(PizzaGO::NICKEL));
    destroyStd();
  }
}

// =============================== WreckBatter ============================ //

WreckBatter::WreckBatter(Logical leftSide, Point1 top) :
  WreckGO(),
  animator(this, ResourceManager::wreckBatterFull, 0.03, this),
  hitThisAttack(false),
  idleChooser(0.0)
{
  basePoints = 40;
  hflip(leftSide == false);
  setXY(leftSide ? - 16.0 : PLAY_W + 16.0, top);

  animator.changeFrameTime(4, 0.5);
  animator.setInactive();
  autoSize();
}

void WreckBatter::load()
{

}

void WreckBatter::updateMe()
{
  if (touchingPlayerCircle(attackCircle()) == true)
  {
    animator.setActive();
  }
  
  animator.update();
  
  updateCollisions();
  
  if (animator.getActive() == false)
  {
    idleChooser += RM::timePassed();
    RM::wrap1_me(idleChooser, 0.0, 2.0);
    setImage(idleChooser < 1.0 ? animator.images[0] : animator.images[1]);
  }
}

void WreckBatter::redrawMe()
{
  WreckGO::redrawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    attackCircle().draw_outline(RED_SOLID);
  }
}

void WreckBatter::updateCollisions()
{
  if (animator.imgIndex == 4 && touchingPlayerCircle(attackCircle()) == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(attackCircle().xy + Point2(48.0 * -facingToDir(), 0.0), 275.0);
      hitThisAttack = true;      
    }
  }
  else 
  {
    hitThisAttack = false;
  }
}

Circle WreckBatter::attackCircle()
{
  return Circle(getXY() + Point2(144.0 * facingToDir(), 66.0), 48.0);
}

void WreckBatter::ballCollision()
{
  if (getActive() == true) 
  {
    level->gotScore(basePoints);
    level->objectDefeated(this);
    level->createCoin(getXY() + Point2(80.0 * facingToDir(), 64.0), 
                      SlopeCoin::randValueAll(PizzaGO::DIME));
    destroyStd();
  }
}

void WreckBatter::callback(ActionEvent* caller)
{
  if (caller == &animator)
  {
    animator.reset();
    animator.setInactive();
    idleChooser = 0.0;
  }
}

// =============================== WreckGirl ============================ //

WreckGirl::WreckGirl(Logical leftSide, Point1 top) :
  WreckGO(),
  ActionListener(),
  animator(),
  slider(&xy.x, 0.0, 1.5, this),
  value(WRECK_BOY)
{
  hflip(leftSide == false);
  setY(top);
}

void WreckGirl::load()
{
  value = level->randGen.randi(0, WRECK_OLD);
  
  if (value == WRECK_BOY)
  {
    animator.init(this, ResourceManager::wreckBoyAnim, 0.25);
  }
  else if (value == WRECK_GIRL)
  {
    ArrayList<Point1> girlFrames(ResourceManager::wreckGirlAnim.count, true);
    girlFrames[0] = 0.25;
    girlFrames[1] = 0.10;
    girlFrames[2] = 0.25;
    girlFrames[3] = 0.10;
    
    animator.init(this, ResourceManager::wreckGirlAnim, girlFrames);
  }
  else
  {
    animator.init(this, ResourceManager::wreckOldAnim, 0.25);    
  }
  
  animator.repeats();
  animator.randomize();
  autoSize();
}

void WreckGirl::updateMe()
{
  if (lifeState == RM::ACTIVE)
  {
    animator.update();
  }
  else if (lifeState == RM::TRANS_OUT)
  {
    slider.update();
  }  
}

void WreckGirl::redrawMe()
{
  drawMe();
}

void WreckGirl::ballCollision()
{
  if (lifeState == RM::ACTIVE) 
  {
    if (value == WRECK_GIRL) setImage(imgsetCache[wreckGirlSrc].last());
    else if (value == WRECK_BOY) setImage(imgsetCache[wreckBoySrc].last());
    else if (value == WRECK_OLD) setImage(imgsetCache[wreckOldSrc].last());
    
    Pizza::currGameVars[VAR_CASH] = std::max(Pizza::currGameVars[VAR_CASH] - 25, 0);
    
    BonusText* textEffect = new BonusText("-$0.25", ResourceManager::font, 
                                          getXY() + Point2(64.0 * facingToDir(), 64.0), false);
    textEffect->setColor(1.0, 0.2, 0.2);
    textEffect->upwardVal = 0.0;
    textEffect->holdTime = 1.0;
    textEffect->setScale(0.70, 0.70);
    textEffect->init();
    level->addAction(textEffect);
    
    // player->attacked(getCenter());
    Point1 extraSlide = std::abs(Pizza::platformTL.x) + 32.0;
    slider.reset(getHFlip() ? getX() + getImage()->natural_size().x + extraSlide :
                 -getImage()->natural_size().x - extraSlide);
    
    ResourceManager::playHitCivilian();
    
    lifeState = RM::TRANS_OUT;
  }
}

void WreckGirl::callback(ActionEvent* caller)
{
  if (caller == &slider) 
  {
    setRemove();
  }
}

// =============================== WreckBone ============================ //

WreckBone::WreckBone(Point2 start) :
  WreckGO(),
  vel(0.0, 0.0)
{
  setImage(imgCache[wreckBone]);
  autoSize();
  setXY(start);
}

void WreckBone::load()
{
  Point1 speed = 200.0;
  vel = RM::pol_to_cart(speed, RM::angle(getXY(), player->getXY() + Point2(0.0, 100.0)));
}

void WreckBone::updateMe()
{
  addXY(vel * RM::timePassed());
  addRotation(PI * 2.5 * RM::timePassed());
  
  if (Box::collision(boxFromC(), level->camera.myBox()) == false)
  {
    lifeState = RM::REMOVE;
  }
  else
  {
    updateCollisions();
  }
}

void WreckBone::redrawMe()
{
  drawMe();
}

void WreckBone::updateCollisions()
{
  if (collisionCircle().collision(player->collisionCircle()) == true)
  {
    destroyStd();
    
    player->attacked(getXY());
  }
}

Circle WreckBone::collisionCircle()
{
  return Circle(getXY(), getWidth() * 0.25);
}

// =============================== WreckSkeleCatcher ============================ //

WreckSkeleCatcher::WreckSkeleCatcher(Logical leftSide, Point1 top) :
  WreckGO(),
  ActionListener(),
  myBall(NULL),
  throwAnim(this, ResourceManager::wreckCatcherThrow, 0.10, this),
  catchAnim(this, ResourceManager::wreckCatcherReverse, 0.10, this)
{
  basePoints = 20;
  type = SKELE_CATCHER;
  
  hflip(leftSide == false);
  setXY(leftSide ? -16.0 : PLAY_W + 16.0, top);
  autoSize();
  
  throwAnim.setInactive();
  catchAnim.setInactive();
}

void WreckSkeleCatcher::updateMe()
{
  throwAnim.update();
  catchAnim.update();
}

void WreckSkeleCatcher::redrawMe()
{
  drawMe();
  if (Pizza::DEBUG_COLLISION == true) collisionCircle().draw_outline(COLOR_FULL);
}

void WreckSkeleCatcher::catchBaseball()
{
  catchAnim.reset();
  catchAnim.setFrame(1);
}

void WreckSkeleCatcher::ballCollision()
{
  if (lifeState == RM::ACTIVE) 
  {
    level->gotScore(basePoints);
    level->createCoin(getXY() + Point2(80.0 * facingToDir(), 64.0), 
                      SlopeCoin::randValueAll(PizzaGO::DIME));
    level->objectDefeated(this);
    destroyStd();
  }
}

Circle WreckSkeleCatcher::collisionCircle()
{
  return Circle(getXY() + Point2(156.0 * facingToDir(), 108.0), 32.0);
}

void WreckSkeleCatcher::callback(ActionEvent* caller)
{
  if (caller == &catchAnim)
  {
    throwAnim.reset();
  }
  else if (caller == &throwAnim)
  {
    WreckBaseball* ball = new WreckBaseball();
    ball->setXY(getXY() + Point2(156.0 * facingToDir(), 108.0));
    ball->setTarget(getHFlip() == false ? PLAY_W - 156.0 + 16.0 : 156.0 - 16.0);
    level->addEnemy(ball);    

    if (getY() < level->camBox().bottom()) ResourceManager::playWreckThrow();

    setImage(imgsetCache[wreckBaseballSkele].last());
  }
}

// =============================== WreckBaseball ============================ //

WreckBaseball::WreckBaseball() :
  WreckGO(),
  ActionListener(),

  sideMover(&xy.x, 1.0, 1.5, this),
  upMover(&xy.y, 1.0, 0.75, this),
  downMover(&xy.y, 1.0, 0.75),

  falling(false),
  fallVel(0.0, 0.0)
{
  sideMover.setInactive();
  // upMover.setInactive();
  // downMover.setInactive();
  setImage(imgCache[wreckBaseball]);
  autoSize(0.5);
}

void WreckBaseball::updateMe()
{
  if (falling == false)
  {    
    sideMover.update();
    upMover.update();
    downMover.update();
  }
  else
  {
    addXY(fallVel * RM::timePassed());
    removeLowStd();
  }
  
  if (lifeState == RM::ACTIVE && touchingPlayerCircle() == true)
  {
    player->attacked(getXY());
    destroyStd();
  }
}

void WreckBaseball::setTarget(Point1 tgtX)
{
  sideMover.reset(tgtX);
  upMover.reset(getY() - 64.0);
  downMover.reset(getY());
  
  downMover.setInactive();
}

void WreckBaseball::callback(ActionEvent* caller)
{
  if (caller == &upMover)
  {
    downMover.setActive();
  }
  else if (caller == &sideMover)
  {
    WreckGO* catcher = level->objectAtPt(SKELE_CATCHER, getXY());
    
    if (catcher != NULL) 
    {
      setRemove();
      catcher->catchBaseball();
    }
    else
    {
      // fallVel = Point2(200.0 * (getX() > PLAY_CX ? 1.0 : -1.0), 128.0);
      // falling = true;
      // destroyStd();
      level->addAction(new FishShatter(*this, 2, 2));
      setRemove();
    }
  }
}

// =============================== WreckCutterSkele ============================ //

WreckCutterSkele::WreckCutterSkele(Logical leftSide, Point1 top) :
  WreckGO(),
  animator(this, DataList<Image*>(imgsetCache[wreckCutSkeleImgs], 0, 3), 0.15, this),
  thrown(false)
{
  basePoints = 60;
  autoSize(0.5);
  hflip(leftSide == false);
  setXY(leftSide ? -16.0 : PLAY_W + 16.0, top);
  animator.setInactive();
}

void WreckCutterSkele::updateMe()
{
  if (thrown == false &&
      player->collisionCircle().bottom() > getY() + getHeight())
  {
    animator.setActive();
    thrown = true;
  }
  
  animator.update();
}

void WreckCutterSkele::ballCollision()
{
  if (lifeState == RM::ACTIVE) 
  {
    level->gotScore(basePoints);
    level->createCoin(getXY() + Point2(80.0 * facingToDir(), 64.0), 
                      SlopeCoin::randValueAll());
    level->objectDefeated(this);
    destroyStd();
  }
}

void WreckCutterSkele::callback(ActionEvent* caller)
{
  if (caller == &animator)
  {
    setImage(imgsetCache[wreckCutSkeleImgs].last());
    
    ResourceManager::playWreckThrow();
    
    WreckCutterBlade* blade = new WreckCutterBlade();
    blade->setXY(getX() + 156.0 * facingToDir(), getY() + 108.0);
    level->addEnemy(blade);
  }
}

// =============================== WreckCutterBlade ============================ //

WreckCutterBlade::WreckCutterBlade() :
  WreckGO()
{
  setImage(imgCache[wreckSickle]);
  setWH(24.0, 24.0);
}

void WreckCutterBlade::load()
{
  hflip(getX() > PLAY_CX);
}

void WreckCutterBlade::updateMe()
{
  addX(100.0 * RM::timePassed() * (getHFlip() == false ? 1.0 : -1.0));
  Coord1 atkResult = 0;
  
  if (lifeState == RM::ACTIVE) atkResult = level->cutAttack(Circle(getXY(), getWidth()));
  
  if (atkResult == 1) destroyStd();
  else if (atkResult == 2) setPassive();
  
  addRotation(TWO_PI * RM::timePassed());
  
  removeHighStd();
}

// =============================== WreckShocker ============================ //

WreckShocker::WreckShocker(Point2 xy0, Point2 xy1) :
  WreckGO(),
  secondNode(xy1, Point2(16.0, 16.0), imgCache[wreckNode]),
  animWeight(RM::randf(0.0, 1.0))
{
  setBox(xy0, Point2(16.0, 16.0));  // size is radius
  setImage(imgCache[wreckNode]);
  
  setRotation(RM::angle(getXY(), secondNode.getXY()));
  secondNode.setRotation(getRotation() + PI);
}

void WreckShocker::load()
{
  init_box(RM::lerp(getXY(), secondNode.getXY(), 0.5), 
           Point2(RM::distance_to(getXY(), secondNode.getXY()), 16.0), false);
  fixture->SetSensor(true);
  rotationFromWorld();
}

void WreckShocker::updateMe()
{
  updateCollisions();
  animWeight = RM::wrap1(animWeight + 4.0 * RM::timePassed(), 0.0, 1.0);
}

void WreckShocker::redrawMe()
{
  Image* elecImg = AnimationBehavior::pickImage(imgsetCache[wreckElectricSet], animWeight);  
  elecImg->draw_repeating_tgt(getXY(), secondNode.getXY(), COLOR_FULL);

  drawMe();
  secondNode.drawMe();  
}

void WreckShocker::updateCollisions()
{
  if (touchingPlayerPhys() == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(RM::lerp(getXY(), secondNode.getXY(), 0.5));
    }
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
}

// =============================== WreckBird ============================ //

WreckBird::WreckBird() :
  WreckGO(),
  hoverAnim(this, ResourceManager::wreckWingHover, 0.15),
  attackAnim(this, ResourceManager::wreckWingAttack, 0.15),
  xMover(&xy.x, 1.0, 1.0),
  yMover(&xy.y, 1.0, 1.0, this),
  waiter(1.5, this)
{
  basePoints = 30;
  
  xMover.setInactive();
  yMover.setInactive();
  
  hoverAnim.repeats();
  attackAnim.repeats();
  attackAnim.setInactive();
  
  setImage(imgsetCache[wreckFlierSrc][0]);
  setWH(24.0, 24.0);
}

void WreckBird::updateMe()
{
  // cout << "levelcambox " << level->camera.myBox() << " plat " << level->camBox() << endl;
  
  if (level->camBox().collision(imageBox()) == true)
  {    
    xMover.update();
    yMover.update();
    waiter.update();
  }
  
  if (waiter.getActive() == true) hflip(getX() > player->getX());
  
  hoverAnim.update();
  attackAnim.update();
  
  updateCollisions();
  removeHighStd();
}

void WreckBird::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    if (level->powerActive() == true)
    {
      level->objectDefeated(this);
      level->gotScore(basePoints);
      destroyStd();      
    }
    else if (hitThisAttack == false && yMover.getActive() == true)
    {
      player->attacked(getXY());
    }
    else if (yMover.getActive() == false)
    {
      level->objectDefeated(this);
      level->gotScore(basePoints);
      level->addAction(new Smokeburst(getXY(), false));
      destroyStd();
    }
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
}

void WreckBird::callback(ActionEvent* caller)
{
  if (caller == &waiter)
  {
    // go 128 past the player
    Point2 newTarget = RM::lerp(getXY(), player->getXY(), 1.25);
    xMover.reset(newTarget.x, 1.0);
    yMover.reset(newTarget.y, 1.0);
    
    attackAnim.setFrame(hoverAnim.currIndex());
    attackAnim.setActive();
    hoverAnim.setInactive();
  }
  else if (caller == &yMover)
  {
    waiter.reset();
  
    hoverAnim.setFrame(attackAnim.currIndex());
    hoverAnim.setActive();
    attackAnim.setInactive();
  }
}

// =============================== WreckScreenShock ============================ //

WreckScreenShock::WreckScreenShock() :
  WreckGO(),

  UFO(Point2(0.0, 0.0), imgsetCache[wreckUFOSet][0]),
  blast(Point2(0.0, 0.0), imgCache[wreckUFOBlast]),
  script(this),

  UFOYOffset(-32.0),
  started(false)
{
  blast.autoSize();
  blast.setH(0.0);
  blast.setAlpha(0.0);
}

void WreckScreenShock::updateMe()
{
  if (started == false && player->getY() >= getY())
  {
    started = true;
    ResourceManager::playWreckUFOAppear();
    
    script.enqueueX(new CosineFn(&UFOYOffset, 64.0, 1.0));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][1]));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][0]));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][1]));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][0]));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][1]));
    script.wait(0.05);
    script.enqueueX(new FunctionCommand0(&ResourceManager::playWreckUFOShock));
    script.enqueueX(new LinearFn(&blast.color.w, 1.0, 0.1));
    script.enqueueX(new LinearFn(&blast.wh.y, SCREEN_H, 0.25));
    script.wait(0.5);
    script.enqueueX(new LinearFn(&blast.color.w, 0.0, 0.1));
    script.enqueueX(new SetValueCommand<Image*>(&UFO.image, imgsetCache[wreckUFOSet][0]));
    script.enqueueX(new SetValueCommand<Point1>(&blast.wh.y, 0.0));
    script.enqueueX(new CosineFn(&UFOYOffset, -32.0, 1.0));
  }
  else if (started == false)
  {
    return;
  }
  
  script.update();
  
  UFO.setXY(Point2(PLAY_CX, level->camera.myBox().top() + UFOYOffset));
  blast.setXY(UFO.getXY());
  blast.autoScale();
  
  updateCollisions();
}

void WreckScreenShock::redrawMe()
{
  if (started == false) return;
  
  UFO.drawMe();
  blast.drawMe();
  imgCache[wreckUFOBlastBot]->draw_scale(blast.xy + Point2(0.0, blast.getHeight()), 
                                         Point2(1.0, 1.0), 0.0, blast.getColor());
}

void WreckScreenShock::updateCollisions()
{
  if (started == false) return;
  if (blast.wh.y < 5.0) return;
  
  if (blast.boxFromTC().collision(player->collisionBox()) == true)
  {
    if (hitThisAttack == false)
    {
      player->attacked(player->getXY() + Point2(0.0, -64.0));
    }
    
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
}

void WreckScreenShock::callback(ActionEvent* caller)
{
  setRemove();
}

// =============================== WreckPowerup ============================ //

WreckPowerup::WreckPowerup() :
  WreckGO(),
  ActionListener(),
  animator(this, imgsetCache[wreckPowerupSet], 0.15),
  fader(&color.w, 0.0, 0.25, this)
{
  basePoints = 25;
  autoSize(0.5);
  animator.repeats();
  fader.setInactive();
}

void WreckPowerup::updateMe()
{
  animator.update();
  fader.update();
  updateCollisions();
  
  if (lifeState == RM::TRANS_OUT)
  {
    RM::attract_me(xy, player->getXY(), 64.0 * RM::timePassed());
  }
  
  removeHighStd();
}

void WreckPowerup::redrawMe()
{
  drawMe();
}

void WreckPowerup::updateCollisions()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (touchingPlayerCircle() == true)
  {
    lifeState = RM::TRANS_OUT;
    
    ResourceManager::playWreckPowerup();
    
    level->addAction(new WreckSpark(getXY()));
    level->addAction(new WreckSpark(getXY()));
    level->addAction(new WreckSpark(getXY()));
    level->addAction(new WreckSpark(getXY()));
    
    level->gotScore(basePoints);
    level->gotPowerup();
    fader.setActive();
  }
}

void WreckPowerup::callback(ActionEvent* caller)
{
  setRemove();
}

// =============================== WreckCreature ============================ //

WreckCreature::WreckCreature() :
  WreckGO(),
  attackAnim(this, ResourceManager::wreckClimberAttack, 0.15),
  climbAnim(this, ResourceManager::wreckClimberClimb, 0.15),
  moveState(WAITING),
  yVel(0.0)
{
  basePoints = 30;
  setWH(32.0, 32.0);
  attackAnim.repeats();
  climbAnim.repeats();
  climbAnim.setInactive();
}

void WreckCreature::updateMe()
{
  if (moveState == WAITING && getY() < player->getY() - 48.0)
  {
    moveState = MOVING_DOWN;
    yVel = level->getChainVel() * 1.85;
    
    climbAnim.reset();
    attackAnim.setInactive();
  }
  else if (moveState == MOVING_DOWN && getY() > 
           level->camera.getY() + PLAY_H * 0.85)
  {
    yVel = 0.0;
    moveState = WAITING;
    
    attackAnim.reset();
    climbAnim.setInactive();
  }
  
  attackAnim.update();
  climbAnim.update();
  
  addY(yVel * RM::timePassed());
  hflip(getX() < PLAY_CX);
  updateCollisions();
  
  if (lifeState == RM::ACTIVE)
  {
    if (level->buildingAtPt(getXY() + Point2(facingToDir() * 64.0, 0.0)) == NULL)
    {
      level->addAction(new Smokeburst(getXY(), false));
      destroyStd();
    }
  }
}

void WreckCreature::redrawMe()
{
  drawMe();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle().draw_outline(RED_SOLID);    
  }
}

Circle WreckCreature::attackCircle()
{
  return Circle(getXY() + Point2(-40.0 * facingToDir(), 0.0), 24.0);
}

void WreckCreature::updateCollisions()
{
  if (attackCircle().collision(player->collisionCircle()) == true)
  {
    if (moveState == WAITING && level->powerActive() == false)
    {
      player->attacked(getXY());
    }
    hitThisAttack = true;
  }
  else
  {
    hitThisAttack = false;
  }
  
  if (touchingPlayerCircle() == true)
  {
    level->gotScore(basePoints);
    level->objectDefeated(this);
    level->addAction(new Smokeburst(getXY(), false));
    destroyStd();
    /*
    if (level->powerActive() == true || 
        moveState == MOVING_DOWN)
    {
      level->addAction(new Smokeburst(getXY(), false));
      destroyStd();
    }
     */
  }
}

// =============================== WreckDebris ============================ //

WreckDebris::WreckDebris() : 
  WreckGO(),
  warningTimer(2.0, this),
  warningAlpha(0.0)
{
  setImage(imgsetCache[junkSet][0]);
  setWH(24.0, 24.0);
  // warningTimer.setInactive();
}

void WreckDebris::load()
{
  setY(level->camera.getY() - 64.0);
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << PizzaGO::TYPE_DEBRIS, 
                 (0x1 << PizzaGO::TYPE_PLAYER) | (0x1 << PizzaGO::TYPE_TERRAIN));
  change_density(3.0);
  set_b2d_callback_data(this);
  set_ang_vel(level->randGen.randf(-PI, PI));
}

void WreckDebris::updateMe()
{
  warningTimer.update();
  RM::flatten_me(warningAlpha, warningTimer.getActive() ? 1.0 : 0.0, 4.0 * RM::timePassed());
  
  if (warningTimer.getActive() == false)
  {
    updateFromPhysical();
    
    if ((flags & PizzaGO::SPIKED_BIT) != 0 &&
        touchingPlayerPhys() == true)
    {
      player->attacked(getXY());
    }
  }
  else
  {
    updateFromWorld();
  }
  
  removeLowStd();
}

void WreckDebris::redrawMe()
{
  drawMe();
  
  if (warningAlpha > 0.01)
  {
    imgCache[warningV]->draw_scale(Point2(getX(), level->camBox().top()), Point2(1.0, 1.0), 
                                   0.0, ColorP4(1.0, 1.0, 1.0, warningAlpha));    
  }
}

void WreckDebris::setSpiked()
{
  flags |= PizzaGO::SPIKED_BIT;
  setImage(imgsetCache[junkSet][1]);
  warningTimer.setActive();
}

void WreckDebris::callback(ActionEvent* caller)
{
  if (caller == &warningTimer)
  {
    setY(level->camera.getY() - 64.0);
    set_velocity(Point2(0.0, 0.0));
  }
}

// =============================== WreckFloatSquare ============================ //

WreckFloatSquare::WreckFloatSquare() :
  WreckGO()
{
  basePoints = 20;
  setWH(64.0, 64.0);
  setImage(imgCache[wreckObstacle]);
}

void WreckFloatSquare::load()
{
  init_box(getXY(), getSize(), false);
  set_collisions(0x1 << PizzaGO::TYPE_TERRAIN, 0x1 << PizzaGO::TYPE_PLAYER);
  set_restitution(0.5);
  set_b2d_callback_data(this);
  updateFromWorld();  // to set the rotation
}

void WreckFloatSquare::updateMe()
{
  if (lifeState == RM::PASSIVE) 
  {
    level->gotScore(basePoints);
    // level->objectDefeated(this);
    destroyStd();
  }  
}

void WreckFloatSquare::collidedPhysical(WreckGO* wgo, Point1 impulse)
{
  if (wgo == player && level->powerActive() == true && impulse > 100.0)
  {
    setPassive();
  }
}


// =============================== WreckCoin ============================ //

WreckCoin::WreckCoin() :
  WreckGO(),
  animator(),
  value(PizzaGO::PENNY)
{
  
}

void WreckCoin::load()
{
  SlopeCoin::setAnimator(this, animator, value, level->getCoinFrame());
}

void WreckCoin::updateMe()
{
  animator.update();
  
  if (touchingPlayerCircle() == true)
  {
    level->gotCoin(getXY(), SlopeCoin::toCash(value));
    setRemove();
  }
}

// =============================== WreckUnlockable ============================ //

WreckUnlockable::WreckUnlockable(Coord2 setToppingID) :
  WreckGO(),

  toppingID(setToppingID),

  glowRotation(0.0),
  bobOffset(0.0),
  bobData(0.0, 1.0),

  startedMagnet(false),
  startXY(0.0, 0.0),
  currDistPercent(0.0)
{
  setImage(imgCache[secretOutfit]);
  setWH(16.0, 16.0);
}

void WreckUnlockable::updateMe()
{
  glowRotation += TWO_PI * 0.1 * RM::timePassed();
  if (startedMagnet == false) updateFloater();
  else updateMagnet();
}

void WreckUnlockable::redrawMe()
{
  setImage(imgCache[secretOutfitGlow]);
  setRotation(glowRotation);
  drawMe();
  
  setImage(imgCache[secretOutfit]);
  setRotation(0.0);
  drawMe();
}

void WreckUnlockable::updateFloater()
{
  addY(-bobOffset);
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 
                  0.5 * RM::timePassed());
  addY(bobOffset);
  
  if (touchingPlayerCircle() == true)
  {
    startMagnet();
  }
}

void WreckUnlockable::updateMagnet()
{
  Point1 speedMult = level->levelState == PizzaLevel::LEVEL_PLAY ? 1.0 : 2.0;
  Logical atPlayer = RM::flatten_me(currDistPercent, 1.0, speedMult * RM::timePassed());
  Point1 totalDist = RM::distance_to(startXY, player->getXY());
  
  setXY(RM::attract(startXY, player->getXY(), totalDist * currDistPercent));
  
  if (atPlayer == true)
  {
    ResourceManager::playOutfitFound();
    Pizza::currIngredientFound = toppingID;
    level->addAction(new FlagStars(getXY(), false));
    setRemove();
  }
}

void WreckUnlockable::startMagnet()
{
  startedMagnet = true;
  startXY = getXY();
}

// =============================== WreckSpark ============================ //

const Point1 WreckSpark::SPEED = 150.0;

WreckSpark::WreckSpark(Point2 center) :
  VisRectangular(),
  Effect(),
  ActionListener(),

  waiter(1.0, this),
  fader(&color.w, 1.0, 0.1),
  speedMult(RM::randf(0.45, 1.25))
{
  setRotation(RM::rand_angle());
  setXY(center + RM::pol_to_cart(RM::randf(0.0, 64.0), getRotation()));
  setImage(RM::random(imgsetCache[wreckPowerupSparkSet]));
}

void WreckSpark::updateMe()
{
  addXY(RM::pol_to_cart(SPEED * speedMult * RM::timePassed(), getRotation()));
  waiter.update();
  fader.update();
}

void WreckSpark::redrawMe()
{
  drawMe();
}

void WreckSpark::callback(ActionEvent* caller)
{
  if (caller == &waiter)
  {
    fader.reset(0.0, 0.25);
    fader.setListener(this);
  }
  else if (caller == &fader)
  {
    done();
  }
}

// ==================================== PlaneGO ==================================//

PlaneGO::PlaneGO() :
  VisRectangular(),
  BaseGOLogic(),

  level(NULL),
  player(NULL),

  hitThisAttack(false),
  type(-1),
  basePoints(0),
  topCoinGiven(-1),

  hitPoints(0.9)
{
  flags = BREAKS_STD | SMOKES_STD;
  hflip(true);
}

void PlaneGO::redrawMe()
{
  drawMe();
  drawDebugStd();
}

void PlaneGO::updateDMGStd()
{
  explodeTouchingPlayer();
  updateDMGColor();
}

Logical PlaneGO::touchingPlayerCircle() 
{
  return collisionCircle().collision(player->collisionCircle());
}

Logical PlaneGO::touchingPlayerCircle(const Circle& c) 
{
  return c.collision(player->collisionCircle());
}

Logical PlaneGO::onScreen()
{
  return level->camBox().collision(imageBox());
}

Logical PlaneGO::attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg)
{
  if (getActive() == false) return false;
    
  hitPoints -= dmg;
  
  if (hitPoints <= 0.0) 
  {
    createCoin();
    destroyStd();
    level->enemyDefeated(this);
    damagedCallback(atkPoint, attacker);
  }
  else 
  {
    ResourceManager::playPlaneBulletHit();

    setBlue(0.0);
    setGreen(0.0);
  }
  
  return true;
}

void PlaneGO::destroyStd()
{
  setRemove();
  if ((flags & BREAKS_STD) != 0) breakEffectStd(3, 3);
  if ((flags & BREAKS_STD) != 0) ResourceManager::playCrunch();
  if ((flags & SMOKES_STD) != 0) smokePuffStd();
}

void PlaneGO::updateDMGColor()
{
  RM::flatten_me(color.y, 1.0, 4.0 * RM::timePassed());
  color.z = color.y;
}

Point2 PlaneGO::yRange()
{
  return Point2(getHeight(), PLAY_H - getHeight());
}

Point1 PlaneGO::randYRanged()
{
  return RM::randf(yRange());
}

void PlaneGO::drawDebugStd()
{
  debugCircle(collisionCircle(), WHITE_SOLID);
}

void PlaneGO::breakEffectStd(Coord1 breakCols, Coord1 breakRows)
{
  breakCols = std::max(breakCols + RM::randi(-1, 1), 1);
  breakRows = std::max(breakRows + RM::randi(-1, 1), 1);
  
  setColor(COLOR_FULL);
  FishShatter* shatter = FishShatter::createForPlane(*this, breakCols, breakRows);
  level->addDebris(shatter);
}

void PlaneGO::debugCircle(const Circle& circle, const ColorP4 outlineColor)
{
  if (Pizza::DEBUG_COLLISION == true) circle.draw_outline(outlineColor);
}

void PlaneGO::explodeTouchingPlayer()
{
  if (getActive() == true && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyStd();
  }
}

void PlaneGO::smokePuffStd()
{
  level->addAction(new Smokeburst(collisionCircle().xy, false));
}

void PlaneGO::createCoinStd()
{
  if (topCoinGiven == -1) return;
  
  level->addAlly(new PlaneCoin(getXY(), topCoinGiven));
}

Circle PlaneGO::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

Circle PlaneGO::touchingMyCircle(const Circle& c)
{
  Circle result(collisionCircle());
  
  // this means no collision
  if (collisionCircle().collision(c) == false) result.radius = -1.0;
  
  return result;
}

// =============================== PlanePlayer ============================ //

PlanePlayer::PlanePlayer() :
  PlaneGO(),
  ActionListener(),

  warper(this, dwarpCache[planeWarp]),
  warpWeight(0.0),
  warpData(0.0, 1.0),

  cooldownTimer(0.2),
  changePropIndex(false),
  propIndex(0),
  oldXY(0.0, 0.0),
  
  flashList()
{
  flags = 0;
  setWH(32.0, 32.0);  // radius
  hflip(false);  // override parent ctor
}

void PlanePlayer::updateMe()
{
  flashList.update();
  cooldownTimer.update();
  RM::bounce_arcsine(warpWeight, warpData, Point2(0.0, 1.0), 2.0 * RM::timePassed());
  updateTilting();
  updatePropeller();
}

void PlanePlayer::redrawMe()
{
  setImage(imgCache[planeFront]);
  warper.draw(1, 2, warpWeight);
  
  setImage(imgsetCache[planePropSet][propIndex]);
  drawMe();
  changePropIndex = true;
  
  flashList.redraw();
  
  debugCircle(collisionCircle(), WHITE_SOLID);
}

void PlanePlayer::updateTilting()
{
  Point1 deltaY = getY() - oldXY.y;
  Point1 targetDir = RM::approxEq(deltaY, 0.0) ? 0.0 : RM::sign(deltaY);
  
  Point1 rotateSpeed = PI * 0.25;
  
  if (targetDir != 0.0 && RM::sign(targetDir) == -RM::sign(getRotation()))
  {
    rotateSpeed *= 4.0;
  }
      
  RM::flatten_me(rotation, targetDir * (PI / 12.0) * RM::clamp(std::abs(deltaY) / 4.0, 0.0, 1.0),
                 rotateSpeed * RM::timePassed());
  oldXY = getXY();
}

void PlanePlayer::updatePropeller()
{
  if (changePropIndex == true)
  {
    propIndex++;
    propIndex %= 2;
    changePropIndex = false;
  }
}

void PlanePlayer::tryMoveToPoint(Point1 yVal)
{
  RM::flatten_me(xy.y, yVal, 320.0 * RM::timePassed());
}

void PlanePlayer::tryMoveDirection(Point1 tiltMag)
{
  addY(320.0 * tiltMag * RM::timePassed());
  RM::clamp_me(xy.y, SCREEN_BOX.top() + getHeight(), SCREEN_BOX.bottom() - getHeight());
}

void PlanePlayer::tryFire()
{
  if (cooldownTimer.getActive() == false)
  {
    PlanePlayerBullet* bullet = new PlanePlayerBullet();
    bullet->setXY(getXY());
    bullet->vel = RM::pol_to_cart(650.0, getRotation());
    bullet->setRotation(getRotation());
    level->addAlly(bullet);
    
    flashList.addX(new MuzzleFlash(this));
    ResourceManager::playPlaneFire();
    
    cooldownTimer.reset();
  }
}

Logical PlanePlayer::attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return false;
  // if (face.isHurt() == true) return false;
  if (level->levelState != PizzaLevel::LEVEL_PLAY) return false;
  
  // face.actionTrigger(PizzaFace::HURT);
  level->smallShake();
  level->addSauceEffect(*this, atkPoint);  

  ResourceManager::playDamaged();
  Pizza::currGameVars[VAR_DAMAGE]++;
  
  return true;
}

Box PlanePlayer::collisionBox()
{
  return Box::from_center(getXY(), getSize() * 1.75);
}

void PlanePlayer::callback(ActionEvent* caller)
{

}

// =============================== MuzzleFlash ============================ //

MuzzleFlash::MuzzleFlash(VisRectangular* setParent) :
  ImageEffect(RM::random(imgsetCache[planeMuzzleSet])),
  parent(setParent)
{
  setXY(parent->getXY());
  setAlpha(0.0);
  
  enqueueX(new LinearFn(&color.w, 1.0, 0.05));
  script.wait(0.1);
  enqueueX(new LinearFn(&color.w, 0.0, 0.025));
}

void MuzzleFlash::updateMe()
{
  setXY(parent->getXY());
  setRotation(parent->getRotation());
  ImageEffect::updateMe();
}

// =============================== PlanePlayerBullet ============================ //

PlanePlayerBullet::PlanePlayerBullet() :
  PlaneGO(),
  vel(0.0, 0.0)
{
  flags = 0x0;
  setWH(8.0, 8.0);
  hflip(false);  // override parent ctor
  setImage(imgCache[planeBullet]);
}

void PlanePlayerBullet::updateMe()
{
  addXY(vel * RM::timePassed());
  updateCollisions();
  
  if (onScreen() == false) setRemove();
}

void PlanePlayerBullet::redrawMe()
{
  drawMe();
  debugCircle(collisionCircle(), BLUE_SOLID);
}

void PlanePlayerBullet::createExplosion(const Point2& enemyCircleXY)
{
  Point2 explodePt = getXY() + RM::pol_to_cart(getWidth(), RM::angle(getXY(), enemyCircleXY));
  
  BombExplosion* explode = new BombExplosion(explodePt);
  explode->setScale(0.1, 0.1);
  explode->scaler.reset(0.25, 0.35);
  explode->beforeFadeTimer.reset(0.10);
  explode->fader.reset(0.0, 0.25);
  
  level->addAction(explode);
}

void PlanePlayerBullet::updateCollisions()
{
  for (LinkedListNode<PlaneGO*>* enemyNode = level->objman.enemyList.first; 
       enemyNode != NULL; 
       enemyNode = enemyNode->next)
  {
    PlaneGO* enemy = enemyNode->data;
    
    if (enemy->getActive() == false) continue;
    if ((enemy->flags & IGNORES_BULLETS) != 0) continue;
        
    Circle overlapCircle = enemy->touchingMyCircle(collisionCircle());
    
    if (overlapCircle.radius > 0.0)
    {
      Logical hit = enemy->attacked(getXY(), this);
      
      if (hit == true)
      {
        createExplosion(overlapCircle.xy);
        setRemove();
        break;        
      }
    }
  }
}

// =============================== PlaneFlameskull ============================ //

PlaneFlameskull::PlaneFlameskull() :
  PlaneGO(),

  glowAlpha(0.0),
  glowFrame(0)
{
  basePoints = 10;
  topCoinGiven = PizzaGO::PENNY;
  
  setWH(24.0, 24.0);
  setImage(imgCache[flamingSkull]);
}

void PlaneFlameskull::updateMe()
{
  Point1 motion = 144.0 * facingToDir() * RM::timePassed();
  addX(motion);
  addRotation(RM::linear_to_rads(motion, getWidth()));
  
  updateGlow();
  
  if (getX() < level->camBox().left() - 96.0) setRemove();
  
  updateDMGStd();
}

void PlaneFlameskull::redrawMe()
{
  Point1 rot = getRotation();
  
  setRotation(0.0);
  setAlpha(1.0 - glowAlpha);
  setImage(imgsetCache[flameskullFlameSet][glowFrame]);
  drawMe();
  
  setAlpha(glowAlpha);
  setImage(imgsetCache[flameskullFlameSet][(glowFrame + 1) % 
                                           imgsetCache[flameskullFlameSet].count]);
  drawMe();
  
  setRotation(rot);
  setAlpha(1.0);
  setImage(imgCache[flamingSkull]);
  drawMe();
  
  drawDebugStd();
}

void PlaneFlameskull::updateGlow()
{
  glowAlpha += 8.0 * RM::timePassed();
  glowFrame += RM::wrap1_me(glowAlpha, 0.0, 1.0) == 0 ? 0 : 1;
  glowFrame %= imgsetCache[flameskullFlameSet].count;
}

void PlaneFlameskull::poofOut()
{  
  PoofEffect* flamePoof = new PoofEffect(getXY(), imgsetCache[flameskullFlameSet][0]);
  flamePoof->hflip(getHFlip());
  flamePoof->beforeFadeTime = 0.0;
  flamePoof->fadeTime = 0.5;
  flamePoof->startScale = 1.0;
  flamePoof->endScale = 0.0;
  flamePoof->init();
  
  level->addAction(flamePoof);
}

void PlaneFlameskull::destroyStd()
{
  poofOut();
  PlaneGO::destroyStd();
}

// =============================== PlaneCupid ============================ //

PlaneCupid::PlaneCupid() :
  PlaneGO(),

  idleAnim(this, imgsetCache[cupidIdleSet], 0.06),
  wingAnim(this, ResourceManager::cupidWingSet, 0.06),
  fireAnim(this, imgsetCache[cupidShootSet], 0.15, this),
  bodyAnim(&idleAnim),

  mover(&xy.y, 0.0, 2.0, this),

  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0)
{
  hitPoints = 2.9;
  basePoints = 40;
  topCoinGiven = PizzaGO::NICKEL;
  
  setWH(24.0, 24.0);
  idleAnim.apply();
  wingAnim.repeats();
  idleAnim.repeats();
}

void PlaneCupid::load()
{
  mover.reset(randYRanged());
}

void PlaneCupid::updateMe()
{  
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  mover.update();
  
  Coord1 startIndex = bodyAnim->currIndex();
  bodyAnim->update();
  wingAnim.update();
  
  if (bodyAnim == &fireAnim &&
      startIndex == 4 &&
      bodyAnim->currIndex() == 5)
  {
    fire();
  }
  
  updateDMGColor();
}

void PlaneCupid::redrawMe()
{
  setImage(bodyAnim->currImage());
  drawMe(Point2(0.0, bobOffset));
  
  setImage(wingAnim.currImage());
  drawMe(Point2(0.0, bobOffset));
  
  drawDebugStd();
}

void PlaneCupid::fire()
{
  ResourceManager::playCupidShoot();
  
  Point2 firePoint = getXY() + Point2(22.0 * facingToDir(), -5.0 + bobOffset);
  level->addEnemy(new PlaneCupidArrow(firePoint, -200.0));
}

void PlaneCupid::destroyStd()
{
  setImage(bodyAnim->currImage());
  PlaneGO::destroyStd();
  
  setImage(wingAnim.currImage());
  breakEffectStd(2, 2);
}

void PlaneCupid::callback(ActionEvent* caller)
{
  if (caller == &mover)
  {
    bodyAnim = &fireAnim;
    bodyAnim->reset();
  }
  else if (caller == &fireAnim)
  {
    bodyAnim = &idleAnim;
    mover.reset(randYRanged());
  }
}

// =============================== PlaneCupidArrow ============================ //

PlaneCupidArrow::PlaneCupidArrow(Point2 origin, Point1 setVelX) :
  PlaneGO(),
  ActionListener(),

  stickBehavior(this),
  xVel(setVelX),
  rotationOffset(0.0),
  rotationData(0.0, 1.0)
{
  flags &= ~SMOKES_STD;
  stickBehavior.setListener(this);
  setImage(imgCache[cupidArrow]);
  setXY(origin);
}

void PlaneCupidArrow::updateMe()
{
  if (lifeState == RM::ACTIVE)
  {
    addX(xVel * RM::timePassed());
    if (onScreen() == false) lifeState = RM::REMOVE;
    updateCollisions();
    
    RM::bounce_arcsine(rotationOffset, rotationData, 
                    Point2(-PI * 0.02, PI * 0.02), RM::timePassed() * 4.0);    
    
    setRotation(rotationOffset);
  }
  else if (lifeState == RM::PASSIVE)
  {
    stickBehavior.update();
    setAlpha(1.0 - RM::clamp((stickBehavior.percentDone() - 0.5) / 0.5, 0.0, 1.0));
  }
  
}

void PlaneCupidArrow::updateCollisions()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    
    stickBehavior.reset(player, 1.5);
    lifeState = RM::PASSIVE;
  }
}

Circle PlaneCupidArrow::collisionCircle()
{
  return Circle(getXY(), 7.0);
}

void PlaneCupidArrow::callback(ActionEvent* caller)
{
  if (caller == &stickBehavior)
  {
    setRemove();
  }
}

// =============================== PlaneDemon ============================ //

PlaneDemon::PlaneDemon() :
  PlaneGO(),
  animation(this, ResourceManager::pumpkinFly, 0.1),
  xMover(&xy.x, 0.0, 1.5, this),
  yMover(&xy.y, 0.0, 1.5, this)
{
  basePoints = 20;
  topCoinGiven = PizzaGO::PENNY;

  setWH(17.0, 17.0);
  animation.repeats();
  animation.randomize();
}

void PlaneDemon::load()
{
  Point1 moveTime = RM::randf(2.5, 3.0);
  Point1 yTarget = RM::lerp(PLAY_BOX.center().y + 64.0, PLAY_BOX.bottom(), RM::randf());
  
  xMover.reset(level->camBox().left() - 96.0, moveTime);
  yMover.reset(yTarget, moveTime);
  
  ResourceManager::playDemonSwoop();
}

void PlaneDemon::updateMe()
{
  xMover.update();
  yMover.update();
  animation.update();
  
  if (animation.getActive() == true && xMover.progress() > 0.5)
  {
    animation.setInactive();
    setImage(imgsetCache[pumpkinImgs].last());
  }
  
  updateDMGStd();
}

void PlaneDemon::redrawMe()
{
  drawMe();
  debugCircle(collisionCircle(), RED_SOLID);
}

void PlaneDemon::callback(ActionEvent* caller)
{
  if (caller == &xMover) setRemove();
}

// =============================== PlaneBrain ============================ //

PlaneBrain::PlaneBrain() :
  PlaneGO(),
  animator(),
  orbitCircle(Point2(0.0, 0.0), RM::randf(24.0, 48.0)),
  orbitAngle(0.0),
  orbitSpeed(TWO_PI),
  xMover(&orbitCircle.xy.x, -(orbitCircle.radius + 128.0), 5.0, this),
  yMover(&orbitCircle.xy.y, 0.0, 4.0)
{
  hitPoints = 2.9;
  basePoints = 40;
  topCoinGiven = PizzaGO::NICKEL;
  
  setWH(24.0, 24.0);
  setScale(0.5, 0.5);
  
  DataList<Image*> pingpong;
  Image::create_pingpong(imgsetCache[brainFly], pingpong);
  animator.init(this, pingpong, 0.1);
  animator.repeats();
}

void PlaneBrain::load()
{
  orbitCircle.xy = getXY();
  setXY(orbitCircle.xy + RM::pol_to_cart(orbitCircle.radius, orbitAngle));

  Point1 yBuffer = orbitCircle.radius + getWidth() + 16.0;
  Point1 moveYTgt = RM::randf(yBuffer, PLAY_H - yBuffer);
  yMover.reset(moveYTgt);
}

void PlaneBrain::updateMe()
{
  animator.update();
  xMover.update();
  yMover.update();
  
  orbitAngle += orbitSpeed * RM::timePassed();
  setXY(orbitCircle.xy + RM::pol_to_cart(orbitCircle.radius, orbitAngle));
  
  updateDMGStd();
}

void PlaneBrain::callback(ActionEvent* caller)
{
  if (caller == &xMover) setRemove();
}

// =============================== PlaneWisp ============================ //

PlaneWisp::PlaneWisp() :
  PlaneGO(),
  ActionListener(),

  warper(this, dwarpCache[wispWarp]),
  warpWeight(0.0),
  warpData(0.0, 1.0),

  mover(this),
  stateTimer(2.0, this),

  currState(Wisp::INVISIBLE),
  blinkCount(0)
{
  basePoints = 70;
  topCoinGiven = PizzaGO::DIME;

  setImage(imgsetCache[wispSet][0]);
  setWH(16.0, 16.0);
}

void PlaneWisp::load()
{
  Point1 moveTime = RM::randf(1.5, 2.0);
  
  Box bounds = level->camBox();
  bounds.grow_right(-32.0);
  bounds.grow_down(-32.0);
  bounds.grow_left(-PLAY_W * 0.5);
  bounds.grow_up(-32.0);
  
  mover.init(bounds, Point2(moveTime, moveTime));
  
  setInvisible();
}

void PlaneWisp::updateMe()
{
  RM::bounce_linear(warpWeight, warpData, Point2(0.0, 1.0), RM::timePassed());
  
  stateTimer.update();
  mover.update();
}

void PlaneWisp::redrawMe()
{
  warper.draw(warpWeight);
  drawDebugStd();
}

void PlaneWisp::setVisible()
{
  flags &= ~IGNORES_BULLETS;
  currState = Wisp::VISIBLE;
  setImage(imgsetCache[wispSet][0]);
  stateTimer.reset(2.0);  
}

void PlaneWisp::setInvisible()
{
  flags |= IGNORES_BULLETS;
  currState = Wisp::INVISIBLE;
  setImage(imgsetCache[wispSet][1]);
  stateTimer.reset(1.0);
}

void PlaneWisp::setBlink()
{
  flags |= IGNORES_BULLETS;
  currState = Wisp::INVIS_BLINK;
  setImage(imgsetCache[wispSet][2]);
  stateTimer.reset(1.5);
}

void PlaneWisp::fire()
{
  ResourceManager::playWispShoot();
  
  PlaneWispFlame* flame = new PlaneWispFlame(getXY());
  level->addEnemy(flame);
}

void PlaneWisp::destroyStd()
{
  ResourceManager::playWispDie();
  level->addAction(new WispExplode(getXY()));
  setRemove();
}

void PlaneWisp::callback(ActionEvent* caller)
{
  if (caller == &stateTimer)
  {
    switch (currState)
    {
      default:
      case Wisp::VISIBLE:
        setInvisible();
        break;
      case Wisp::INVISIBLE:
        if (blinkCount >= 2) setVisible();
        else setBlink();
        blinkCount++;
        blinkCount %= 3;
        break;
      case Wisp::INVIS_BLINK:
        fire();
        setInvisible();
        break;
    }
  }
}

// =============================== PlaneWispFlame ============================ //

PlaneWispFlame::PlaneWispFlame(Point2 center) :
  PlaneGO(),
  animator(this, imgsetCache[wispFlameSet], 0.1),
  fader(&color.w, 1.0, 0.2, this),
  vel(0.0, 0.0)
{
  flags |= IGNORES_BULLETS;  
  animator.repeats();
  setAlpha(0.0);
  setBox(center.x, center.y, 8.0, 8.0);
}

void PlaneWispFlame::load()
{
  vel = RM::ring_edge(getXY(), player->getXY(), 200.0);
  setRotation(RM::angle(-vel));
}

void PlaneWispFlame::updateMe()
{
  animator.update();
  fader.update();
  addXY(vel * RM::timePassed());
  updateDMGStd();
  if (onScreen() == false) setRemove();
}

void PlaneWispFlame::destroyStd()
{
  setPassive();
  fader.reset(0.0);
}

void PlaneWispFlame::callback(ActionEvent* caller)
{
  if (lifeState == RM::PASSIVE)
  {
    setRemove();
  }
}

// =============================== PlaneRabbit ============================ //

PlaneRabbit::PlaneRabbit() :
  PlaneGO(),
  animation(this, imgsetCache[rabbitSet], 0.1),
  xMover(&xy.x, 0.0, 2.0, this),
  yMover(&xy.y, 0.0, 2.0, this)
{
  hitPoints = 1.9;
  basePoints = 40;
  topCoinGiven = PizzaGO::NICKEL;
  
  setWH(17.0, 17.0);
  setScale(0.75, 0.75);
  animation.setInactive();
}

void PlaneRabbit::load()
{
  Point1 moveTime = RM::randf(2.5, 3.5);
  Point1 yTarget = RM::lerp(PLAY_BOX.top(), PLAY_BOX.center().y - 64.0, RM::randf());
  
  xMover.reset(level->camBox().left() - 96.0, moveTime);
  yMover.reset(yTarget, moveTime);
}

void PlaneRabbit::updateMe()
{
  xMover.update();
  yMover.update();
  animation.update();
  
  if (animation.getActive() == false && 
      animation.currIndex() == 0 &&
      imageBox().bottom() < level->camBox().bottom())
  {
    ResourceManager::playRabbitJump();
    animation.setActive();
  }
  
  updateDMGStd();
}

void PlaneRabbit::callback(ActionEvent* caller)
{
  if (caller == &xMover) setRemove();
}

// =============================== PlaneNaga ============================ //

PlaneNaga::PlaneNaga() :
  PlaneGO(),
  ActionListener(),

  pieces(),
  tailLength(Naga::TAIL_LENGTH),

  sinePercent(0.0),
  currAmplitude(Naga::TAIL_AMPLITUDE_START),
  currNumPeriods(Naga::TAIL_PERIODS_START),

  onscreenMover(&xy.x, 0.0, 0.5, this),
  chargeWaiter(2.0, this),
  chargeMover(&xy.x, 0.0, 3.5, this),
  aiState(MOVE_ONSCREEN),
  currSpeed(0.0),

  flameGen(0.0),
  flameList()
{
  hitPoints = 4.9;
  basePoints = 85;
  topCoinGiven = PizzaGO::DIME;
  
  setImage(imgCache[nagaHeadOpen]);
  setWH(16.0, 16.0);
}

void PlaneNaga::load()
{
  Box screenBounds = level->camBox();
  
  onscreenMover.reset(screenBounds.right() - 48.0);
  chargeWaiter.setInactive();
  
  chargeMover.reset(screenBounds.left() - tailLength - 16.0);
  chargeMover.setInactive();
  
  pieces.ensure_capacity(24);
  Point1 currPiecePlace = tailLength;
  Point2 runningXY = getXY();
  
  while (currPiecePlace > 0.0)
  {
    Point1 pieceScale = RM::lerp(0.5, 1.0, currPiecePlace / tailLength);
    
    pieces.add(VisRectangular());
    pieces.last().setImage(imgCache[nagaBone]);    
    pieces.last().setWH(Naga::BASE_BONE_SIZE * pieceScale, Naga::BASE_BONE_SIZE * pieceScale);
    pieces.last().setScale(pieceScale, pieceScale);
    pieces.last().hflip(getHFlip());
    
    runningXY += Point2(pieces.last().getWidth(), 0.0);
    pieces.last().setXY(runningXY);
    
    // cout << currPiecePlace << " " << pieces.last().getWidth() << endl;
    currPiecePlace -= Naga::BASE_BONE_SIZE * pieceScale;
    
    if (currPiecePlace <= 0.0)
    {
      pieces.last().setImage(imgCache[nagaTail]);
    }
  }
  
  updatePieces(true);
}

void PlaneNaga::updateMe()
{
  sinePercent = RM::wrap1(sinePercent + RM::timePassed(), 0.0, 1.0);
  if (aiState == MOVE_ONSCREEN) generateFlames();
  flameList.update();
  
  onscreenMover.update();
  chargeWaiter.update();
  chargeMover.update();
  
  updateDMGColor();
  updatePieces(false);
  updateCollisions();
}

void PlaneNaga::redrawMe()
{
  setImage(aiState == CHARGING ? imgCache[nagaHeadClosed] : imgCache[nagaHeadOpen]);
  drawMe();
  drawPieces();
  flameList.redraw();
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    drawDebugStd();
    
    for (Coord1 i = 0; i < pieces.count; ++i)
    {
      if (i == 0 || i % 3 != 0) continue;
      
      Circle(pieces[i].getXY(), pieces[i].getWidth()).draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
    }
  }
}

void PlaneNaga::updatePieces(Logical firstUpdate)
{
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    VisRectangular& currPiece = pieces[i];
    VisRectangular& lastPiece = i == 0 ? *this : pieces[i-1];
    
    Point1 piecePercent = RM::lerp_reverse(currPiece.getXScale(), 0.5, 1.0);
    Point1 curveVal = std::fmod(piecePercent + sinePercent, 1.0) * TWO_PI * currNumPeriods;
    
    Point1 waveMult = std::sqrt(1.0 - piecePercent);
    Point1 maxDistance = (lastPiece.getWidth() * 0.5 + currPiece.getWidth() * 0.5);
    
    if (i == 0) 
    {
      waveMult = 0.1;  // this is just an approximation based on current values
      maxDistance = 16.0;
    }
    
    Point1 oldX = currPiece.getX();
    Point1 pieceX = RM::clamp(currPiece.getX(), lastPiece.getX() - maxDistance, 
                              lastPiece.getX() + maxDistance);
    if (firstUpdate == true) pieceX = lastPiece.getX() + maxDistance * -facingToDir();
    
    if (pieceX != oldX) currPiece.hflip(oldX > pieceX);
    if (firstUpdate == true) currPiece.hflip(getHFlip());
    
    currPiece.setX(pieceX);
    currPiece.setY(getY() + std::sin(curveVal) * currAmplitude * waveMult);
    currPiece.setRotation(std::cos(curveVal) * PI * 0.2 * waveMult);
    currPiece.setColor(getColor());
  }
  
  setRotation(pieces.first().getRotation());
}

void PlaneNaga::updateCollisions()
{
  if (hitThisAttack == true) 
  {
    return;
  }
    
  if (touchingPlayerCircle() == true)
  {
    destroyStd();
    player->attacked(getXY(), this);
    return;
  }
  
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    if (i == 0 || i % 3 != 0) continue;
    
    Circle pieceCircle(pieces[i].getXY(), pieces[i].getWidth());
    
    if (touchingPlayerCircle(pieceCircle) == true)
    {
      player->attacked(pieceCircle.xy, this);
      hitThisAttack = true;
      break;
    }
  }
    
}

void PlaneNaga::drawPieces()
{
  for (Coord1 i = pieces.count - 1; i >= 0; --i)
  {
    // i think it has to do the rotation compensation because the curve
    // equation changes in the other direction
    Point1 currRot = pieces[i].getRotation();
    pieces[i].setRotation(pieces[i].getHFlip() ? TWO_PI - currRot : currRot);
    pieces[i].drawMe();
    
    if (i % 4 == 3) 
    {
      Image* currImg = pieces[i].getImage();
      pieces[i].setImage(imgCache[nagaArm]);
      pieces[i].drawMe();
      pieces[i].setImage(currImg);
    }
    pieces[i].setRotation(currRot);
  }  
  drawMe();
}

void PlaneNaga::generateFlames()
{
  flameGen += RM::randf(0.0, 0.5) * RM::timePassed();
  
  while (flameGen > 1.0 / 60.0)
  {
    flameList.addX(new SunSpark(getXY() + Point2(16.0 * facingToDir(), 16.0)));
    flameGen -= 1.0 / 60.0;
  }
}

void PlaneNaga::destroyStd()
{
  ResourceManager::playCrunch();
  smokePuffStd();
  setColor(COLOR_FULL);
  breakEffectStd(2, 3);
  
  for (Coord1 i = 0; i < pieces.count; ++i)
  {
    pieces[i].setColor(COLOR_FULL);
    level->addDebris(FishShatter::createForPlane(pieces[i], 1, 1));
    
    if (i % 4 == 3)
    {
      pieces[i].setImage(imgCache[nagaArm]);
      level->addDebris(FishShatter::createForPlane(pieces[i], 1, 1));
    }
  }
  
  setRemove();
}

void PlaneNaga::callback(ActionEvent* caller)
{
  if (caller == &onscreenMover)
  {
    ResourceManager::playNagaBreath();
    level->addEnemy(new PlaneNagaFireball(getXY() + Point2(16.0 * facingToDir(), 16.0)));
    aiState = WAIT_TO_CHARGE;
    chargeWaiter.setActive();
  }
  else if (caller == &chargeWaiter)
  {
    aiState = CHARGING;
    chargeMover.setActive();
  }
  else if (caller == &chargeMover)
  {
    setRemove();
  }
}


// =============================== PlaneNagaFireball ============================ //


PlaneNagaFireball::PlaneNagaFireball(Point2 center) :
  PlaneGO(),
  flameGen(0.0),
  flameList(),
  xMover(&xy.x, 0.0, 1.0, this)
{
  flags |= IGNORES_BULLETS;
  setWH(18.0, 18.0);
  setXY(center);
}

void PlaneNagaFireball::load()
{
  xMover.reset(level->camBox().left() - 64.0);
}

void PlaneNagaFireball::updateMe()
{
  if (getActive() == true) generateFlames();
  flameList.update();
  xMover.update();
  
  if (getActive() == true)
  {
    if (touchingPlayerCircle() == true)
    {      
      player->attacked(getXY(), this);
      setPassive();
    }
  }
  else if (lifeState == RM::PASSIVE)
  {
    if (flameList.count == 0) setRemove();
  }
}

void PlaneNagaFireball::redrawMe()
{
  flameList.redraw();
  drawDebugStd();
}

void PlaneNagaFireball::generateFlames()
{
  flameGen += RM::randf(0.25, 0.5) * RM::timePassed();
  
  while (flameGen > 1.0 / 60.0)
  {
    flameList.addX(new SunSpark(getXY()));
    flameGen -= 1.0 / 60.0;
  }
}

void PlaneNagaFireball::callback(ActionEvent* caller)
{
  if (caller == &xMover)
  {
    setPassive();
  }
}

// =============================== PlaneNodes ============================ //

const Point1 PlaneNodes::MIN_BEAM_HEIGHT = 100.0;

PlaneNodes::PlaneNodes() :
  PlaneGO(),
  beamHeight(MIN_BEAM_HEIGHT),
  animWeight(RM::randf(0.0, 1.0))
{
  setImage(imgCache[wreckNode]);
  setWH(16.0, 16.0);
  hitPoints = 2.9;

  basePoints = 40;
}

void PlaneNodes::load()
{
  
}

void PlaneNodes::updateMe()
{
  addX(-200.0 * RM::timePassed());
  updateCollisions();
  animWeight = RM::wrap1(animWeight + 4.0 * RM::timePassed(), 0.0, 1.0);
  updateDMGColor();
  
  if (onScreen() == false && getX() < player->getX())
  {
    setRemove();
  }
}

void PlaneNodes::redrawMe()
{
  Image* elecImg = AnimationBehavior::pickImage(imgsetCache[wreckElectricSet], animWeight);
  elecImg->draw_repeating_tgt(getXY(), getBottomCircle().xy, COLOR_FULL);

  drawMe();
  drawMe(getBottomCircle().xy - getXY());
  
  if (Pizza::DEBUG_COLLISION == true)
  {
    collisionCircle().draw_solid(RED_SOLID);
    getBottomCircle().draw_solid(RED_SOLID);
    Box::from_norm(getXY(), Point2(16.0, beamHeight), HANDLE_TC).draw_outline(YELLOW_SOLID);
  }
}

void PlaneNodes::updateCollisions()
{
  Circle topCircle = collisionCircle();
  Circle botCircle = getBottomCircle();
  Box electricBox = Box::from_norm(getXY(), Point2(16.0, beamHeight), HANDLE_TC);
  Logical touchingTop = touchingPlayerCircle(topCircle);
  Logical touchingBot = touchingPlayerCircle(botCircle);
  
  if (touchingTop == true || touchingBot == true)
  {
    if (touchingTop == true) player->attacked(topCircle.xy, this);
    else player->attacked(botCircle.xy, this);
    
    destroyStd();
  }
  else if (electricBox.collision(player->collisionBox()) == true &&
           hitThisAttack == false)
  {
    player->attacked(electricBox.center(), this);
    hitThisAttack = true;
  }  
}

void PlaneNodes::destroyStd()
{
  PlaneGO::destroyStd();
  Point2 realXY = getXY();
  setXY(getBottomCircle().xy);
  breakEffectStd(3, 3);
  setXY(realXY);  // this is for the coin to be in the right place
}

Circle PlaneNodes::getBottomCircle()
{
  return Circle(getXY() + Point2(0.0, beamHeight), getWidth());
}

void PlaneNodes::damagedCallback(Point2 atkPoint, PlaneGO* attacker)
{
  if (lifeState != RM::REMOVE) return;

  Point2 coinLocation = collisionCircle().xy;
  
  if (RM::distance_to(atkPoint, getBottomCircle().xy) < RM::distance_to(atkPoint, collisionCircle().xy))
  {
    coinLocation = getBottomCircle().xy;
  }
  
  level->addAlly(new PlaneCoin(coinLocation, SlopeCoin::NICKEL));
}

Circle PlaneNodes::touchingMyCircle(const Circle& c)
{
  Circle result(collisionCircle());
  result.radius = -1.0;

  // this means no collision
  if (collisionCircle().collision(c) == true)
  {
    result = collisionCircle();
  }
  else if (getBottomCircle().collision(c) == true)
  {
    result = getBottomCircle();
  }
  
  return result;
}

// =============================== PlaneCoin ============================ //

PlaneCoin::PlaneCoin(Point2 center, Coord1 setValue) :
  PlaneGO(),
  animator(),
  value(setValue)
{
  setXY(center);
  hflip(false);
}

void PlaneCoin::load()
{
  SlopeCoin::setAnimator(this, animator, value, level->getCoinFrame());
  setScale(0.75, 0.75);
  wh *= scale;
}

void PlaneCoin::updateMe()
{
  animator.update();
  
  Point1 speed = -256.0 * (level->levelState == PizzaLevelInterface::LEVEL_PLAY ? 1.0 : 3.0);
  addX(speed * RM::timePassed());
  
  if (touchingPlayerCircle() == true)
  {
    gotCoin();
  }
  else if (getX() < level->camBox().left() - 64.0)
  {
    setRemove();
  }
}

void PlaneCoin::redrawMe()
{
  drawMe();
}

void PlaneCoin::gotCoin()
{
  level->gotCoin(getXY(), SlopeCoin::toCash(value));
  setRemove();
}

// =============================== PlaneUnlockable ============================ //

PlaneUnlockable::PlaneUnlockable(Coord2 setToppingID) :
  PlaneGO(),

  toppingID(setToppingID),

  glowRotation(0.0),
  bobOffset(0.0),
  bobData(0.0, 1.0),

  startedMagnet(false),
  startXY(0.0, 0.0),
  currDistPercent(0.0)
{
  hflip(false);
  setImage(imgCache[secretOutfit]);
  setWH(16.0, 16.0);
}

void PlaneUnlockable::updateMe()
{
  addX(-256.0 * RM::timePassed());
  if (getX() < level->camBox().left() - 64.0) setRemove();
  
  glowRotation += TWO_PI * 0.1 * RM::timePassed();
  
  if (startedMagnet == false) updateFloater();
  else updateMagnet();
}

void PlaneUnlockable::redrawMe()
{
  setImage(imgCache[secretOutfitGlow]);
  setRotation(glowRotation);
  drawMe();
  
  setImage(imgCache[secretOutfit]);
  setRotation(0.0);
  drawMe();
}

void PlaneUnlockable::updateFloater()
{
  addY(-bobOffset);
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 
                  0.5 * RM::timePassed());
  addY(bobOffset);
  
  if (touchingPlayerCircle() == true)
  {
    startMagnet();
  }
}

void PlaneUnlockable::updateMagnet()
{
  Point1 speedMult = level->levelState == PizzaLevel::LEVEL_PLAY ? 1.0 : 2.0;
  Logical atPlayer = RM::flatten_me(currDistPercent, 1.0, speedMult * RM::timePassed());
  Point1 totalDist = RM::distance_to(startXY, player->getXY());
  
  setXY(RM::attract(startXY, player->getXY(), totalDist * currDistPercent));
  
  if (atPlayer == true)
  {
    ResourceManager::playOutfitFound();
    Pizza::currIngredientFound = toppingID;
    level->addAction(new FlagStars(getXY(), false));
    setRemove();
  }
}

void PlaneUnlockable::startMagnet()
{
  startedMagnet = true;
  startXY = getXY();
}

// ==================================== TimingGO ================================== //

TimingGO::TimingGO() :
  VisRectangular(),
  BaseGOLogic(),

  level(NULL),
  player(NULL),
  type(TTYPE_UNKNOWN)
{
  
}

void TimingGO::updateMe()
{
  
}

void TimingGO::redrawMe()
{
  drawMe();
}

// ==================================== TimingPizza ================================== //

TimingPizza::TimingPizza() :
  TimingGO(),
  
  animState(ANIM_RUN),
  walkStartX(0.0),
  animWeight(0.0),
  jumper(&animWeight, 0.0, 1.0)
{
  type = TTYPE_PLAYER;
}

void TimingPizza::load()
{
  walkStartX = getX();
}

void TimingPizza::updateMe()
{
  switch (animState)
  {
    default:
    case ANIM_RUN:
    {
      Point1 oldWeight = animWeight;
      animWeight = std::fmod(getX() - walkStartX, 256.0) / 256.0;
      if (animWeight >= 0.5 && oldWeight < 0.5) ResourceManager::playTimingStep();
      if (animWeight < 0.2 && oldWeight > 0.8) ResourceManager::playTimingStep();
      break;
    }
    case ANIM_JUMP:
      jumper.update();
      break;
    case ANIM_FALL:
      animWeight = RM::wrap1(animWeight + 2.0 * RM::timePassed(), 0.0, 1.0);
      break;
    case ANIM_START_WIN:
      animWeight += 2.0 * RM::timePassed();
      if (RM::wrap1_me(animWeight, 0.0, 1.0) != 0) animState = ANIM_LOOP_WIN;
      break;
    case ANIM_LOOP_WIN:
      animWeight = RM::wrap1(animWeight + 1.0 * RM::timePassed(), 0.0, 1.0);
      break;
  }
}

void TimingPizza::redrawMe()
{
  Warp3AnimationID warpType = timingRunWarp;
  
  switch (animState)
  {
    default:
    case ANIM_RUN: warpType = timingRunWarp; break;
    case ANIM_JUMP: warpType = timingJumpWarp; break;
    case ANIM_FALL: warpType = timingFallWarp; break;
    case ANIM_START_WIN: warpType = timingWinStartWarp; break;
    case ANIM_LOOP_WIN: warpType = timingVictoryWarp; break;
  }
  
  warp3Cache[warpType]->draw(this, animWeight);
}

void TimingPizza::playJumpAnim(Point1 time, Logical goodJump)
{
  animWeight = 0.0;

  if (goodJump == true)
  {
    animState = ANIM_JUMP;
    jumper.reset(1.0, time);
  }
  else
  {
    animState = ANIM_FALL;
  }
}

void TimingPizza::playWinAnim()
{
  animWeight = 0.00;
  animState = ANIM_START_WIN;
}


// ==================================== TimingBounceObj ================================== //

TimingBounceObj::TimingBounceObj() :
  TimingGO(),
  ActionListener(),
  scaler(&scale.x, 1.1, 0.1, this)
{
  type = TTYPE_BOUNCER;
  scaler.setInactive();
}

void TimingBounceObj::updateMe()
{
  scaler.update();
  matchXScale();
}

void TimingBounceObj::pizzaTouched()
{
  scaler.setActive();
  ResourceManager::playBounceSound();
}

void TimingBounceObj::callback(ActionEvent* caller)
{
  scaler.reset(1.0);
  scaler.setListener(NULL);
}

// ==================================== TimingGenie ================================== //

TimingGenie::TimingGenie() :
  TimingGO(),

  animator(this, imgsetCache[timingGenieSet], 0.1),
  flameAlpha(1.0),
  flameWeight(0.0),
  touched(false)
{
  type = TTYPE_BOUNCER;
  animator.repeats();
}

void TimingGenie::updateMe()
{
  RM::flatten_me(flameAlpha, touched ? 0.0 : 1.0, 2.0 * RM::timePassed());
  flameWeight = RM::wrap1(flameWeight + RM::timePassed(), 0.0, 1.0);
  animator.update();
}

void TimingGenie::redrawMe()
{
  // the level can change this externally
  Point1 realAlpha = getAlpha();
  
  animator.apply();
  if (touched == false) drawMe();
  
  setAlpha(std::min(realAlpha, flameAlpha));
  warp3Cache[genieWarp]->draw(this, flameWeight);

  setAlpha(realAlpha);
}

void TimingGenie::pizzaTouched()
{
  ResourceManager::playCrunch();
  animator.apply();
  level->addAction(new FishShatter(*this, 3, 3));
  touched = true;
}

// ==================================== TimingBuilding ================================== //

TimingBuilding::TimingBuilding() :
  TimingGO(),
  numMiddles(1)
{
  type = TTYPE_BUILDING;
}

void TimingBuilding::load()
{
  setW(imgsetCache[timingBuildingSet][2]->natural_size().x * numMiddles);
}

void TimingBuilding::redrawMe()
{
  // left piece
  setImage(imgsetCache[timingBuildingSet][1]);
  drawMe();
  
  // middles
  setImage(imgsetCache[timingBuildingSet][2]);
  for (Coord1 i = 0; i < numMiddles; ++i)
  {
    drawMe(Point2(i * getImage()->natural_size().x, 0.0));
  }
  
  // right piece
  setImage(imgsetCache[timingBuildingSet][3]);
  drawMe(Point2(getWidth(), 0.0));
}

// ==================================== TimingCarpet ================================== //

TimingCarpet::TimingCarpet() :
  TimingGO(),
  
  script(this),
  actions(),

  carpetState(WAITING_FOR_JUMP),
  animWeight(0.0),

  startDipY(0.0),
  sparkleGen(0.0),
  carpetDipOffset(0.0)
{
  type = TTYPE_CARPET;
  setImage(imgCache[timingCarpet]);
}

void TimingCarpet::updateMe()
{
  actions.update();
  script.update();
  generateSparkles();
  
  if (carpetState == JUST_DANCE ||
      carpetState == WIN_FLYING)
  {
    matchXScale();
    player->setXY(getXY());
    player->setScale(getScale());
  }
  
  if (carpetState == JUST_DANCE)
  {
    setY(startDipY + carpetDipOffset);
  }
  
  animWeight = RM::wrap1(animWeight + RM::timePassed(), 0.0, 1.0);
}

void TimingCarpet::redrawMe()
{
  warp3Cache[carpetWarp]->draw(this, animWeight);
}

void TimingCarpet::generateSparkles()
{
  if (carpetState == WAITING_FOR_JUMP) return;
  
  sparkleGen += RM::randf(5.0, 10.0) * RM::timePassed();
  
  while (sparkleGen >= 1.0)
  {
    sparkleGen -= 1.0;
    Point2 sparkOffset = Point2(RM::randf(-1.0, 1.0) * 128.0 * getXScale(),
                                RM::randf(0.0, 16.0) * getXScale());
    level->addAction(new CarpetSparkle(this, sparkOffset));
  }
}

void TimingCarpet::catchPizza(Point2 targetXY, Point1 timeLeft)
{
  carpetState = CATCHING_PIZZA;
  setXY(player->getX() - PLAY_W, targetXY.y);
  
  soundCache[music]->stop();
  ResourceManager::carpetMusic->play();
  
  script.wait(0.66 * timeLeft);
  script.enqueueX(new DeAccelFn(&xy.x, targetXY.x, timeLeft * 0.33));
}

void TimingCarpet::callback(ActionEvent* caller)
{
  carpetState++;
  
  switch (carpetState)
  {
    default:
    case WAITING_FOR_JUMP:
      break;
    case CATCHING_PIZZA:
      break;
    case JUST_DANCE:
      // script.wait(1.0);
      startDipY = getY();
      
      script.enqueueX(new DeAccelFn(&carpetDipOffset, 16.0, 0.25));
      script.enqueueX(new AccelFn(&carpetDipOffset, 0.0, 0.25));
      script.wait(0.5);
      
      player->playWinAnim();
      
      ResourceManager::carpetMusic->stop();
      ResourceManager::playTimingWin();
      break;
    case WIN_FLYING:
    {
      Point1 totalFlyTime = 3.0;
      Point1 startLeft = getX() - PLAY_CX;
      level->startZoomOutCam();
      
      script.enqueueX(new DeAccelFn(&xy.x, startLeft + PLAY_W * 0.65, totalFlyTime * 0.3));
      script.enqueueX(new ArcsineFn(&xy.x, startLeft + PLAY_W * 0.2, totalFlyTime * 0.5));
      script.enqueueX(new AccelFn(&xy.x, startLeft + PLAY_W * 1.3, totalFlyTime * 0.2));

      ActionQueue* winEffectScript = new ActionQueue();
      winEffectScript->wait(0.7 * totalFlyTime);
      winEffectScript->enqueueX(new ChangeLevelStatusCMD(level, PizzaLevelInterface::LEVEL_WIN));

      actions.addX(winEffectScript);
      actions.addX(new ArcsineFn(&xy.y, 64.0, totalFlyTime));
      actions.addX(new ArcsineFn(&scale.x, 0.6, totalFlyTime));
      break;
    }
    case WIN_FINISH:
      break;
  }
}

// ==================================== CarpetSparkle ================================== //

CarpetSparkle::CarpetSparkle(TimingGO* setCarpet, Point2 startingOffset) :
  VisRectangular(setCarpet->getXY() + startingOffset),
  Effect(),
  ActionListener(),

  parent(setCarpet),

  scaler(&scale.x, 1.0, RM::randf(0.31, 0.7), this),
  scaleIn(true),

  offset(startingOffset),
  rotationMult(RM::randl() ? -1.0 : 1.0)
{
  setImage(imgCache[winSparkle]);
  setScale(0.0, 0.0);
}

void CarpetSparkle::updateMe()
{
  addRotation(TWO_PI * rotationMult * RM::timePassed());
  scaler.update();
  matchXScale();
  
  offset += Point2(0.0, 64.0 * parent->getXScale() * RM::timePassed());
  setXY(parent->getXY() + offset);
}

void CarpetSparkle::redrawMe()
{
  Point2 myScale = getScale();
  setScale(myScale * parent->getScale());
  
  drawMe();
  
  setScale(myScale);
}

void CarpetSparkle::callback(ActionEvent* caller)
{
  if (scaleIn == true)
  {
    scaler.reset(0.0);
    scaleIn = false;
  }
  else
  {
    done();
  }
}

// =============================== TimingCoin ============================ //

TimingCoin::TimingCoin() :
  TimingGO(),

  animator(),
  fader(&color.w, 1.0, 8.0 * RM::timePassed(), NULL),

  value(PizzaGO::PENNY),
  spins(0)
{
  setAlpha(0.0);
}

void TimingCoin::load()
{
  SlopeCoin::setAnimator(this, animator, value, level->getCoinFrame());
  animator.setTimes(0.03);
  animator.repeats(false);
  animator.setListener(this);
}

void TimingCoin::updateMe()
{
  animator.update();
  fader.update();
  setXY(level->camera.xy + PLAY_CENTER + Point2(0.0, -64.0));
}

void TimingCoin::callback(ActionEvent* caller)
{
  if (spins <= 2)
  {
    spins++;
    animator.reset();
  }
  else
  {
    level->gotCoin(getXY(), SlopeCoin::toCash(value));
    setRemove();
  }
}

// =============================== TimingUnlockable ============================ //

TimingUnlockable::TimingUnlockable(Coord2 setToppingID) :
  TimingGO(),

  toppingID(setToppingID),

  glowRotation(0.0),
  bobOffset(0.0),
  bobData(0.0, 1.0),

  startedMagnet(false),
  startXY(0.0, 0.0),
  currDistPercent(0.0)
{
  hflip(false);
  setImage(imgCache[secretOutfit]);
  setWH(16.0, 16.0);
}

void TimingUnlockable::updateMe()
{
  glowRotation += TWO_PI * 0.1 * RM::timePassed();
  
  updateFloater();
}

void TimingUnlockable::redrawMe()
{
  setImage(imgCache[secretOutfitGlow]);
  setRotation(glowRotation);
  drawMe();
  
  setImage(imgCache[secretOutfit]);
  setRotation(0.0);
  drawMe();
}

void TimingUnlockable::updateFloater()
{
  addY(-bobOffset);
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0),
                     0.5 * RM::timePassed());
  addY(bobOffset);
  
  if (Circle::collision(Circle(player->getXY() + Point2(0.0, -32.0), 16.0),
                        Circle(getXY(), 16.0)) == true)
  {
    ResourceManager::playOutfitFound();
    Pizza::currIngredientFound = toppingID;
    level->addAction(new FlagStars(getXY(), false));
    setRemove();
  }
}


// =============================== SauceParticles ============================ //

SauceParticles::SauceParticles(Point2 setXY, Point1 angle) :
  ParticleEffect(),
  xy(setXY)
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(4, 8), Point2(0.8, 1.0), xy);
  burst->images.append(imgsetCache[sauceDrops]);
  burst->color = RM::color255(226.0, 74.0, 66.0);
  
  burst->speedBounds.set(150.0, 250.0);
  burst->angleBounds.set(angle - PI * 0.15, angle + PI * 0.15);  
  burst->ballisticRotate = true;
  burst->scaleBounds.set(1.5, 2.0);
  
  emitters.add(burst);
}

// =============================== StarParticles ============================ //

StarParticles::StarParticles(Point2 setXY, Point1 angle) :
  ParticleEffect(),
  burst(NULL),
  xy(setXY)
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  burst = new ProceduralBurst(RM::randi(3, 5), Point2(0.8, 1.0), xy);
  burst->images.add(imgCache[slamStar]);
  
  burst->angleBounds.set(angle + PI * 0.15,  angle - PI * 0.15);
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(0.5, 1.0);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  
  emitters.add(burst);
}

StarParticles::StarParticles(Point2 setXY) :
  ParticleEffect(),
  xy(setXY)
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(25, 25), Point2(1.0, 2.5), xy);
  burst->images.add(imgCache[slamStar]);
  
  burst->speedBounds.set(25.0, 150.0);
  burst->scaleBounds.set(0.5, 1.0);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  
  emitters.add(burst);
}

// =============================== CoinParticles ============================ //

CoinParticles::CoinParticles(Point2 center) :
  ParticleEffect()
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(12, 18), Point2(0.8, 1.0), center);
  burst->images = imgsetCache[coinSparkleSet];
  
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(1.0, 2.0);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  burst->alphaOscillate = true;
  burst->alphaOscillateBounds.set(0.5, 1.0);
  
  emitters.add(burst);
  
//  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
//  ProceduralBurst* burst = new ProceduralBurst(1, Point2(0.8, 1.0), center);
//  // burst->images = imgsetCache[coinSparkleSet];
//  burst->images.add(imgCache[slamStar]);
//  // burst->color = BLINK_STAR_COLORS[RM::randi(0, RM::count(BLINK_STAR_COLORS) - 1)];
//  
//  burst->speedBounds.set(150.0, 250.0);
//  burst->scaleBounds.set(0.5, 1.0);
//  burst->constantRotate = true;
//  burst->rotateSpeedBounds.set(PI, PI * 3.0);
//  burst->alphaOscillate = true;
//  burst->alphaOscillateBounds.set(0.5, 1.0);
//  
//  emitters.add(burst);
}

// =============================== SingleCoinSparkle ============================ //

SingleCoinSparkle::SingleCoinSparkle(Point2 center) :
  VisRectangular(center + RM::pol_to_cart(RM::randf(0.0, 24.0), RM::rand_angle()), 
                RM::random(imgsetCache[coinSparkleSet])),
  ActionEvent(),
  ActionListener(),

  scaler(&scale.x, 2.0, 0.2, this),
  scaleIn(true),
  rotateSpeed(RM::randf(HALF_PI, TWO_PI) * (RM::randl() ? 1.0 : -1.0))
{
  setScale(0.0, 0.0);
}

void SingleCoinSparkle::updateMe()
{
  scaler.update();
  matchXScale();
  addRotation(rotateSpeed * RM::timePassed());
}

void SingleCoinSparkle::redrawMe()
{
  drawMe();
}

void SingleCoinSparkle::callback(ActionEvent* caller)
{
  if (scaleIn == true)
  {
    scaleIn = false;
    scaler.reset(0.0, 0.5);
  }
  else
  {
    done();
  }
}

// =============================== FlagStars ============================ //

FlagStars::FlagStars(Point2 center, Logical big) :
  ParticleEffect(),
  burst(NULL)
{
  Point1 bigMult = big ? 2.0 : 1.0;

  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  burst = new ProceduralBurst(RM::randi(6, 8) * bigMult, Point2(0.8, 1.0), center);
  burst->images.add(imgCache[blinkStar]);
    
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds = Point2(0.75, 1.25);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  
  emitters.add(burst);
}

void FlagStars::updateMe()
{
  burst->color = BLINK_STAR_COLORS[RM::randi(0, RM::count(BLINK_STAR_COLORS) - 1)];
  ParticleEffect::updateMe();
}

// =============================== SnowflakeBurst ============================ //

SnowflakeBurst::SnowflakeBurst(Point2 center, Logical big) :
  ParticleEffect(),
  burst(NULL)
{
  Point1 numMult = big ? 4.0 : 1.0;
  Point1 timeMult = big ? 4.0 : 1.0;
  
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  burst = new ProceduralBurst(RM::randi(10, 16) * numMult, Point2(0.8, 1.0) * timeMult, center);
  burst->images += imgsetCache[bgObjectSet];
  burst->images += imgsetCache[smokeSet];
  
  burst->speedBounds.set(50.0, 350.0);
  burst->scaleBounds = Point2(0.75, 1.5);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  
  emitters.add(burst);
}

// =============================== SnowflakeBurst ============================ //

Smokeburst::Smokeburst(Point2 center, Logical big) :
  ParticleEffect(),
  burst(NULL)
{
  Point1 numMult = big ? 2.0 : 1.0;
  Point1 timeMult = big ? 2.0 : 1.0;
  Point1 speedMult = big ? 2.0 : 1.0;
  
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  burst = new ProceduralBurst(RM::randi(10, 16) * numMult, Point2(0.8, 1.0) * timeMult, center);
  burst->images += imgsetCache[smokeSet];
  
  burst->speedBounds = Point2(25.0, 175.0) * speedMult;
  burst->scaleBounds = Point2(0.5, 1.0);
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(PI, PI * 3.0);
  
  emitters.add(burst);
}

// =============================== RingEffect ============================ //

RingEffect::RingEffect(Point2 center) :
  PoofEffect(center, imgCache[burstRing])
{

}

// =============================== FireSplash ============================ //

FireSplash::FireSplash(Point2 center) :
  ParticleEffect()
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(5, 10), Point2(0.8, 1.0), center);
  burst->images = imgsetCache[puppyFireSplashSet];
  
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(1.0, 1.0);
  burst->gravity.set(0.0, 200.0);
  burst->angleBounds.set(PI * 1.25, PI * 1.75);
  burst->constantRotate = false;
  burst->ballisticRotate = false;
  
  emitters.add(burst);
}

// =============================== GooSplash ============================ //

GooSplash::GooSplash(Point2 center) :
  ParticleEffect()
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(5, 10), Point2(0.8, 1.0), center);
  burst->images.add(imgCache[sludgeParticle]);
  
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(0.5, 1.0);
  burst->gravity.set(0.0, 300.0);
  burst->angleBounds.set(PI * 1.1, PI * 1.9);
  burst->constantRotate = false;
  burst->ballisticRotate = false;
  
  emitters.add(burst);
}

// =============================== RockSplash ============================ //

RockSplash::RockSplash(Point2 center) :
  ParticleEffect()
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(5, 10), Point2(1.0, 2.0), center);
  burst->images.add(imgCache[wormParticleImg]);
  
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(1.0, 1.5);
  burst->gravity.set(0.0, 250.0);
  burst->angleBounds.set(PI * 1.1, PI * 1.9);
  burst->constantRotate = false;
  burst->ballisticRotate = true;
  
  emitters.add(burst);
}

// =============================== GrassSplash ============================ //

GrassSplash::GrassSplash(Point2 center) :
  ParticleEffect()
{
  // ctor(ParticleManager*, numParticles, durationBounds, startingLocation)
  ProceduralBurst* burst = new ProceduralBurst(RM::randi(5, 10), Point2(0.8, 1.0), center);
  burst->images = imgsetCache[sumoGrassSet];
  
  burst->speedBounds.set(150.0, 250.0);
  burst->scaleBounds.set(0.5, 1.0);
  burst->gravity.set(0.0, 300.0);
  burst->angleBounds.set(PI * 1.1, PI * 1.9);
  burst->rotateSpeedBounds.set(HALF_PI, TWO_PI);
  burst->constantRotate = true;
  burst->ballisticRotate = false;
  
  emitters.add(burst);
}

// =============================== ScreenShatter ============================ //

ScreenShatter::ScreenShatter(const VisRectangular& visRect) :
  ParticleEffect()
{
  Coord2 shatterGrid(RM::randi(3, 5), RM::randi(2, 4));
  
  ProceduralBurst* burst = new ProceduralBurst(visRect, shatterGrid.x, shatterGrid.y, 
                                               Point2(0.25, Pizza::SCENE_WIPE_DUR));
  
  burst->angleBounds.set(0.0, TWO_PI);   // starting angle that it flies in
  burst->speedBounds.set(250.0, 450.0);    // starting speed to go with angle
  
  burst->constantRotate = true;
  burst->rotateSpeedBounds.set(-TWO_PI * 0.5, TWO_PI * 0.5);
  
  burst->gravity.set(0.0, 200.0);  
  // burst->scaleBounds.set(0.75, 2.0);
  
  emitters.add(burst);
}

// =============================== LoseRedEffect ============================ //

LoseRedEffect::LoseRedEffect(Point1 duration) :
  VisRectangular(Pizza::platScreenBox, imgCache[losePx]),
  Effect(),
  progress(0.0),
  progressMover(&progress, 1.0, duration)
{
  autoScale();
}

void LoseRedEffect::updateMe()
{
  progressMover.update();
}

void LoseRedEffect::redrawMe()
{
  ColorP4 destColor = RM::color255(156, 33, 0);
  setColor(RM::lerp(COLOR_FULL, destColor, progress));
  drawMe();
}





