/*
 *  rgo.cpp
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
#include "rmspine.h"
#include "rmeffect.h"

using std::cout;
using std::endl;

// =============================== PizzaSpineAnimatorStd ============================ //

PizzaSpineAnimatorStd::PizzaSpineAnimatorStd() :
  mySpineAnim(),
  currSpineTimeMult(1.0)
{
  
}

void PizzaSpineAnimatorStd::initSpineAnim(Coord1 spineResIndex, const String1& startTrackName, Logical looping)
{
  initSpineAnim(mySpineAnim, spineResIndex, startTrackName, looping);
}

void PizzaSpineAnimatorStd::initSpineAnim(SpineAnimator& anim, Coord1 spineResIndex,
                               const String1& startTrackName, Logical looping)
{
  anim.init(getSpineSkeleton(spineResIndex), getSpineAnim(spineResIndex));
  spSkeleton_setToSetupPose(anim.spineSkeleton);
  spSkeleton_updateWorldTransform(anim.spineSkeleton);
  
  spAnimationState_setAnimationByName(anim.spineAnimState,
                                      0, startTrackName.as_cstr(), looping ? 1 : 0);
  
  anim.spineAnimState->data->defaultMix = 0.4;
}

void PizzaSpineAnimatorStd::setSpineAnim(Coord1 trackIndex, const Char* animName, Logical looping)
{
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, trackIndex, animName,
                                      looping ? 1 : 0);
}

void PizzaSpineAnimatorStd::setSpineAnim(SpineAnimator& anim, Coord1 trackIndex, const Char* animName, Logical looping)
{
  spAnimationState_setAnimationByName(anim.spineAnimState, 0, animName,
                                      looping ? 1 : 0);
}

void PizzaSpineAnimatorStd::updateSpineAnim(SpineAnimator& anim, Point1 timeMult)
{
  anim.updateSpineSkeleton(RM::timePassed() * timeMult);
}

void PizzaSpineAnimatorStd::updateSpineAnim(SpineAnimator& anim)
{
  anim.updateSpineSkeleton();
}

void PizzaSpineAnimatorStd::updateSpineAnim()
{
  mySpineAnim.updateSpineSkeleton();
  
  // this is to update AABB even if it's not drawing to cull
  mySpineAnim.syncFromVisRect(*get_visrect(), false);
}

void PizzaSpineAnimatorStd::updateSpineAnim(Point1 timeMult)
{
  mySpineAnim.updateSpineSkeleton(RM::timePassed() * timeMult);

  // this is to update AABB even if it's not drawing to cull
  mySpineAnim.syncFromVisRect(*get_visrect(), false);
}


void PizzaSpineAnimatorStd::renderSpineAnim(SpineAnimator& anim)
{
  anim.syncFromVisRect(*get_visrect(), false);
  anim.renderSpineSkeleton();
}


void PizzaSpineAnimatorStd::renderSpineAnim()
{
  renderSpineAnim(mySpineAnim);
}

Logical PizzaSpineAnimatorStd::usingSpineAnim()
{
  return mySpineAnim.getActive();
}

// =============================== DragonFireballInf ============================ //

/*
DragonFireballInf::DragonFireballInf() :
  vel(0.0, 0.0)
{
  
}

void DragonFireballInf::updateFireball()
{
  // addXY(vel * RM::timePassed());
}

void DragonFireballInf::redrawFireball()
{
  // myCircle.draw_solid(RM::lerp(SOLID_ORA))
}
 */

// =============================== ShooterSeasoning ============================ //

ShooterSeasoning::ShooterSeasoning(ShooterSeasCallback* setParent) :
  parent(setParent),

  fireballTimer(1000.0, this),
  lightningTimer(1000.0, this)
{
  Coord1 fireMag = Pizza::getEquippedSeasoningLevel(SEAS_DRAGON_0);
  
  if (fireMag <= 0)
  {
    fireballTimer.setInactive();
  }
  else
  {
    fireballTimer.reset(12.0 - 2.0 * fireMag);
  }
  
  Coord1 lightningMag = Pizza::getEquippedSeasoningLevel(SEAS_EMPEROR_0);
  
  if (lightningMag <= 0)
  {
    lightningTimer.setInactive();
  }
  else
  {
    lightningTimer.reset(18.0 - 3.0 * lightningMag);
  }
}

void ShooterSeasoning::update()
{
  fireballTimer.update();
  lightningTimer.update();
}

void ShooterSeasoning::redraw()
{
  
}

void ShooterSeasoning::callback(ActionEvent* caller)
{
  if (caller == &fireballTimer)
  {
    parent->shootFire();
    fireballTimer.reset();
  }
  else if (caller == &lightningTimer)
  {
    parent->shootLightning();
    lightningTimer.reset();
  }
}

// =============================== PizzaGO ============================ //

PizzaGOStd::PizzaGOStd() :
  PhysRectangular(),
  BaseGOLogic(),
  RenderableParticleBreakerInf(),
  SortDrawer(),
  ActionListener(),

  level(NULL),
  player(NULL),
  shadowImg(NULL),

  dmgOverlayImg(NULL),

  script(this),
  actions(),

  phase(PHASE_ENEMY_STD),
  type(TYPE_ENEMY),
  value(VALUE_UNSET),
  basePoints(0),

  coinToDropType(RANDOM_COIN),
  numCoinDrops(1),
  coinToDropIsPhysical(false),

  sortY(0.0),
  crushValue(999.0),
  dmgTimer(0.0),
  maxHP(1.0),
  currHP(maxHP),
  saberStrikePower(1.0),
  saberDOTPower(1.0),

  myPlatformLine(Point2(0.0, 0.0), Point2(0.0, 0.0)),
  platformOffset(0.0, 0.0),

  hitThisAttack(false),
  flies(false),
  crushesPhysical(false),
  destroyedPhysical(false),

  lightsaberFixtures(4),

  hidingObjects(),

  pathMover(this)
{
  setActive();
}

PizzaGOStd::~PizzaGOStd()
{
  
}

void PizzaGOStd::loadMe()
{

}


void PizzaGOStd::updateMe()
{
  actions.update();
  script.update();
}

void PizzaGOStd::redrawMe()
{
  if (onRMScreen() == false)
  {
    return;
  }
  
  if (usingSpineAnim() == true)
  {
    renderSpineAnim();
  }
  else if (getImage() != NULL)
  {
    drawMe();
  }
}

void PizzaGOStd::drawDStruct()
{
  if (usingSpineAnim() == true)
  {
    renderSpineAnim();
  }
  else if (getImage() != NULL)
  {
    drawMe();
  }
}

void PizzaGOStd::drawSorted()
{
  redrawMe();
}

void PizzaGOStd::touchedFireball(Point2 location, PizzaGOStd* attacker)
{
  // default implementation for gladiator enemies
  if (type == TYPE_ENEMY)
  {
    level->objectCrushedStd(attacker);
  }
}

void PizzaGOStd::strikeWithLightning()
{
  destroyPhysical();
}

void PizzaGOStd::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (objectShouldCrush(contact, pgo, normalImpulse) == true)
  {
    level->objectCrushedStd(this);
  }
  else
  {
    objectBumped(contact, pgo, normalImpulse);
  }
}

Logical PizzaGOStd::objectShouldCrush(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  return crushesPhysical == true &&
      normalImpulse >= crushValue &&
      level->levelState == PhysicsLevelStd::LEVEL_PLAY;
}

Logical PizzaGOStd::standingPhysical(Point2* contactPt)
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
    // NOTE: for the moment i am adding the boss's lightsaber as standable because it
    //   avoids some weird collision situations where you can't move, especially on fungus boss
    Coord1 standableBits = (1 << TYPE_TERRAIN) | (1 << TYPE_DEBRIS)  | (1 << TYPE_SUMO_BOSS)
        | (1 << TYPE_BOSS_LIGHTSABER);

    Logical leftIsTerrain =
        (fixtureA->GetFilterData().categoryBits & standableBits) != 0;
    Logical rightIsTerrain =
        (fixtureB->GetFilterData().categoryBits & standableBits) != 0;
    Logical eitherTerrain = leftIsTerrain || rightIsTerrain;
    Logical standingAngle = (contactAngle > PI * 0.1 && contactAngle < PI * 0.9);
    
    // this is so you can jump out of the lava
    standingAngle = standingAngle || fixtureA->IsSensor() == true || fixtureB->IsSensor() == true;

    if (eitherTerrain == true &&
        edge->contact->IsTouching() == true &&
        edge->contact->IsEnabled() == true &&
        standingAngle == true)
    {
      if (contactPt != NULL)
      {
        *contactPt = pt0;
      }
      
      return true;
    }
  }

  return false;
}

LightsaberTouchResult PizzaGOStd::touchingLightsaber()
{
  LightsaberTouchResult result;
  
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    Point2 pt0 = edge_to_wpt(edge);
    Point1 contactAngle = RM::angle(getXY(), pt0);
    
    b2Fixture* fixtureA = edge->contact->GetFixtureA();
    b2Fixture* fixtureB = edge->contact->GetFixtureB();
    b2Filter filterA = fixtureA->GetFilterData();
    b2Filter filterB = fixtureB->GetFilterData();

    result.touchType = SABER_TOUCHING_NOTHING;
    
    if (type == TYPE_PLAYER &&
        b2FiltersMatchCategories(filterA, filterB, TYPE_PLAYER, TYPE_BOSS_LIGHTSABER) == true)
    {
      result.touchType = SABER_TOUCHING_ME;
      result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_PLAYER) ?
                                                     fixtureB->GetUserData() : fixtureA->GetUserData());
    }
    else if (type == TYPE_SUMO_BOSS &&
             b2FiltersMatchCategories(filterA, filterB, TYPE_SUMO_BOSS, TYPE_PLAYER_LIGHTSABER) == true)
    {
      result.touchType = SABER_TOUCHING_ME;
      result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_SUMO_BOSS) ?
                                            fixtureB->GetUserData() : fixtureA->GetUserData());
    }
    else if (b2FiltersMatchCategories(filterA, filterB, TYPE_PLAYER_LIGHTSABER, TYPE_BOSS_LIGHTSABER) == true)
    {
      // sabers are touching
      result.touchType = SABER_TOUCHING_MY_SABER;
      
      if (type == TYPE_PLAYER)
      {
        // NOT TESTED YET
        result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_PLAYER_LIGHTSABER) ?
            fixtureB->GetUserData() : fixtureA->GetUserData());
      }
      else
      {
        // NOT TESTED YET
        result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_BOSS_LIGHTSABER) ?
            fixtureB->GetUserData() : fixtureA->GetUserData());
      }
    }

    if (result.touchType != SABER_TOUCHING_NOTHING &&
        edge->contact->IsTouching() == true &&
        edge->contact->IsEnabled() == true)
    {
      if (fixtureA->IsSensor() == false &&
          fixtureB->IsSensor() == false)
      {
        result.touchPt = pt0;
      }
      
      return result;
    }
  }
  
  return result;
}

LightsaberTouchResult PizzaGOStd::touchingPlayerLightsaber()
{
  LightsaberTouchResult result;

  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    Point2 pt0 = edge_to_wpt(edge);
    Point1 contactAngle = RM::angle(getXY(), pt0);
    
    b2Fixture* fixtureA = edge->contact->GetFixtureA();
    b2Fixture* fixtureB = edge->contact->GetFixtureB();
    b2Filter filterA = fixtureA->GetFilterData();
    b2Filter filterB = fixtureB->GetFilterData();
    
    result.touchType = SABER_TOUCHING_NOTHING;
    
    if (b2FiltersMatchCategories(filterA, filterB, type, TYPE_PLAYER_LIGHTSABER) == true)
    {
      result.touchType = SABER_TOUCHING_ME;
      result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_PLAYER) ?
                                                     fixtureB->GetUserData() : fixtureA->GetUserData());
    }
    
    if (result.touchType != SABER_TOUCHING_NOTHING &&
        edge->contact->IsTouching() == true &&
        edge->contact->IsEnabled() == true)
    {
      if (fixtureA->IsSensor() == false &&
          fixtureB->IsSensor() == false)
      {
        result.touchPt = pt0;
      }
      
      return result;
    }
  }
  
  return result;
}

void PizzaGOStd::hideInBreakable(PizzaGOStd* breakable, Coord1 collisionMaskBits)
{
  breakable->hidingObjects.add(this);
  
  lifeState = RM::TRANS_IN;
  setAlpha(0.0);
  
  if (body != NULL)
  {
    body->SetActive(false);
  }
}

void PizzaGOStd::parentBroke(PizzaGOStd* breakable, Coord1 collisionMaskBits)
{
  setActive();
  setAlpha(1.0);
  
  if (body != NULL)
  {
    set_collisions(0x1 << type, collisionMaskBits);
    body->SetActive(true);
  }
}

void PizzaGOStd::updateLightsaberDmg()
{
  if (lightsaberFixtures.count == 0)
  {
    return;
  }
  
  LightsaberTouchResult saberTouch = touchingLightsaber();
  
  if (saberTouch.touchType == SABER_TOUCHING_MY_SABER)
  {
    PizzaLevelInterface::createSparksEffect(saberTouch.touchPt, 0.25, RM::rand_angle(), &level->objman.actions);
  }
  else if (saberTouch.touchType == SABER_TOUCHING_ME)
  {
    Logical generateSparks = true;
    
    if (RM::approxEq(saberTouch.touchPt.x, 0.0) == true && RM::approxEq(saberTouch.touchPt.y, 0.0) == true)
    {
      // probably generated from a sensor, like the cupcake candle, which does not make contact points
      generateSparks = false;
    }
    
    if (dmgTimer <= 0.01)
    {
      addDamage(saberTouch.attackingPgo->saberStrikePower);
      
      if (generateSparks == true)
      {
        PizzaLevelInterface::createSparksEffect(saberTouch.touchPt, 1.0, RM::angle(saberTouch.touchPt, getXY()),
                                                &level->objman.actions);
      }
    }
    else
    {
      addDamage(saberTouch.attackingPgo->saberDOTPower * RM::timePassed());
      
      if (generateSparks == true)
      {
        PizzaLevelInterface::createSparksEffect(saberTouch.touchPt, 0.25, RM::angle(saberTouch.touchPt, getXY()),
                                                &level->objman.actions);
      }
    }
    
    dmgTimer = 0.25;
  }
  
  RM::flatten_me(dmgTimer, 0.0, 1.0 * RM::timePassed());
}

void PizzaGOStd::setMaxHP(Point1 newMax)
{
  maxHP = newMax;
  currHP = maxHP;
}

void PizzaGOStd::addDamage(Point1 dmgVal)
{
  if (this == player)
  {
    activeGameplayData->addHP(-dmgVal);
  }
  else
  {
    currHP -= dmgVal;
  }
}

Box PizzaGOStd::getLocalDrawAABB()
{
  // cout << "getLocalDrawAABB " << endl;
 
  if (usingSpineAnim() == true)
  {
    Box spineBox = mySpineAnim.calcSpineAllBoxesAABB();
    spineBox.xy -= getXY();
    // cout << "spine box " << spineBox << endl;
    return spineBox;
  }
  else if (getImage() == NULL)
  {
    Box startBox = collisionBox();
    startBox.xy -= getXY();
    cout << "img box " << startBox << endl;
    return startBox;
  }
  
  // cout << "natural image size " << getImage()->natural_size() << endl;
  Box result = getImage()->calc_quad_coords(getXY(), getScale(), getRotation(), Quad::unit_square()).to_AABB();
  result.xy -= getXY();
  // cout << "local box " << result << endl;

  return result;
}

Logical PizzaGOStd::turnAtWalls()
{
  Logical turned = shouldTurn();
  
  if (turned == true)
  {
    setRotation(RM::standardize_rads(rotation + PI));
    toggleHFlip();
  }
  
  return turned;
}

Logical PizzaGOStd::touchingPlayerCircle()
{
  return collisionCircle().collision(player->collisionCircle());
}

Logical PizzaGOStd::touchingPlayerCircle(const Circle& circle)
{
  return circle.collision(player->collisionCircle());
}

void PizzaGOStd::magicTriggerOn()
{
  setAlpha(1.0);
  set_collisions(0x1 << TYPE_TERRAIN, 0xffff);
}

void PizzaGOStd::magicTriggerOff()
{
  setAlpha(0.0);
  set_collisions(0x1 << TYPE_TERRAIN, 0x0);
}

void PizzaGOStd::emergeFromBlock()
{
  setScale(0.0, 0.0);
  lifeState = RM::TRANS_IN;
  
  actions.addX(new CosineFn(&scale.x, 1.0, 0.5));
  
  script.enqueueX(new CosineFn(&xy.y, getY() - 128.0, 0.5));
  script.enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::ACTIVE));
}

void PizzaGOStd::walkAlongPlatform(Point1 speed, Logical withRotation, Logical withAutoHFlip, Logical withTurning)
{
  if (withTurning == true)
  {
    turnAtWalls();
  }
  else
  {
    clampAtWalls();
  }
  
  Point1 walkAngle = myPlatformLine.angle() + ((facingToDir() == 1) ? 0.0 : PI);
  Point2 walkVector = RM::pol_to_cart(speed * RM::timePassed(), walkAngle);
  
  if (withRotation == true)
  {
    setRotation(walkAngle);
  }
  
  if (withAutoHFlip == true)
  {
    hFlipForRotation();
  }
  
  addXY(walkVector);
  setY(myPlatformLine.get_y_at_x(getX()) + platformOffset.y);  // this keeps y distance constant
}

Logical PizzaGOStd::shouldTurn()
{
  if (collisionCircle().left() < myPlatformLine.left() && getHFlip() == true) return true;
  if (collisionCircle().right() > myPlatformLine.right() && getHFlip() == false) return true;
  return false;
}

Logical PizzaGOStd::clampAtWalls()
{
  if (collisionCircle().left() < myPlatformLine.left())
  {
    addX(myPlatformLine.left() - collisionCircle().left());
    return true;
  }

  if (collisionCircle().right() > myPlatformLine.right())
  {
    addX(myPlatformLine.right() - collisionCircle().right());
    return true;
  }
  
  return false;
}

Logical PizzaGOStd::playerOverMyPlatform()
{
  return player->collisionCircle().right() >= myPlatformLine.left() &&
      player->collisionCircle().left() <= myPlatformLine.right();
}


Point1 PizzaGOStd::sortVal() const
{
  // return sortY;
  return getY();
}

void PizzaGOStd::destroyPhysical()
{
  if (getActive() == false)
  {
    return;
  }
  
  destructionStarted();
  
  if (body != NULL)
  {
    // destroying the body instead can cause a crash because this function can get called
    // while the b2d world is locked
    set_collisions(0x0, 0x0);
  }
  
  if (usingSpineAnim() == true)
  {
    mySpineAnim.hideSpecialBlendSlots = true;
  }
  
  lifeState = RM::TRANS_OUT;
  
  CreateRenderedParticlesAction* deathParticles = new CreateRenderedParticlesAction(this);

  // cout << "breaking" << endl;
  dropCoin();
  
  for (Coord1 i = 0; i < hidingObjects.count; ++i)
  {
    hidingObjects[i]->parentBroke(this, 0xffff);
  }
  
  ActionQueue* dieScript = new ActionQueue();
  dieScript->enqueueX(deathParticles);
  dieScript->enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::REMOVE));
  level->addAction(dieScript);
  // cout << "script length " << script.length() << " " << &script << endl;
}

void PizzaGOStd::dropCoin()
{
  if (coinToDropType == NO_COIN)
  {
    return;
  }
  
  Coord1 finalDropType = coinToDropType;
  
  if (coinToDropType == RANDOM_COIN)
  {
    if (level->standardEnemyDrop() == RANDOM_GEM)
    {
      finalDropType = RANDOM_GEM;
    }
    else
    {
      finalDropType = RM::randi(COIN_PENNY, COIN_QUARTER);
    }
  }
  
  if (finalDropType == RANDOM_GEM)
  {
    finalDropType = RM::randi(FIRST_GEM_TYPE, LAST_GEM_TYPE);
  }

  for (Coord1 i = 0; i < numCoinDrops; ++i)
  {
    if (coinToDropIsPhysical == true)
    {
      PizzaCoinStd* coin = new PizzaCoinStd(getXY(), finalDropType);
      coin->usePhysics = true;
      level->addPGO(coin);
    }
    else
    {
      MagnetCoin* magCoin = new MagnetCoin(getXY(), finalDropType);
      level->addPGO(magCoin);
    }
  }
  
}

void PizzaGOStd::facePlayer()
{
  hflip(getX() > player->getX());
}

void PizzaGOStd::faceDir(Point1 dir)
{
  hflip(dir < 0.0);
}

Circle PizzaGOStd::collisionCircle()
{
  if (usingSpineAnim() == true)
  {
    return mySpineAnim.calcAttachmentCircle("body_collision", "body_collision");
  }
  else
  {
    return Circle(getXY(), getWidth());
  }
}

Logical PizzaGOStd::onRMScreen()
{
  return level->rmboxCamera.myBox().collision(visibleAABB());
}

Box PizzaGOStd::visibleAABB()
{
  if (usingSpineAnim() == true)
  {
    return mySpineAnim.calcSpineAllBoxesAABB();
  }
  else if (getImage() != NULL)
  {
    return imageBox();
  }
  else
  {
    return collisionBox();
  }
}

void PizzaGOStd::checkPhysicalDestruction()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (destroyedPhysical == true)
  {
    destroyPhysical();
  }
}

Logical PizzaGOStd::touchingPGO(PizzaGOStd* targetPGO, Point2* location)
{
  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    if (edge->contact->IsTouching() == true && edge->contact->IsEnabled() == true)
    {
      PizzaGOStd* pgo1 = static_cast<PizzaGOStd*>(edge->contact->GetFixtureA()->GetUserData());
      PizzaGOStd* pgo2 = static_cast<PizzaGOStd*>(edge->contact->GetFixtureB()->GetUserData());
      
      if (pgo1 == targetPGO || pgo2 == targetPGO)
      {
        if (location != NULL) (*location) = edge_to_wpt(edge);
        return true;
      }
    }
  }
  
  return false;
}

VisRectangular* PizzaGOStd::get_visrect()
{
  return this;
}

Box PizzaGOStd::get_local_draw_AABB()
{
  return getLocalDrawAABB();
}

void PizzaGOStd::render_for_particles()
{
  drawDStruct();
}

void PizzaGOStd::particles_ready(BoneBreakEffect* breakEffect)
{
  if (backDebris() == true)
  {
    level->addDebris(breakEffect);
  }
  else
  {
    level->addFrontDebris(breakEffect);
  }
}

void PizzaGOStd::predraw_call()
{
  if (usingSpineAnim() == true)
  {
    mySpineAnim.syncFromVisRect(*this, true);  // sets the rotation to 0 before getting the AABB
  }
}

void PizzaGOStd::postdraw_call()
{
  // dozer stays active to destroy chunks
  if (lifeState == RM::TRANS_OUT)
  {
    setAlpha(0.0);
  }
  
  // this puts the "spine" back where it was in case that position was important
  if (usingSpineAnim() == true)
  {
    mySpineAnim.syncFromVisRect(*this, true);
  }
}

Logical operator<(const SortDrawer& pgo1, const SortDrawer& pgo2)
{
  return pgo1.sortVal() < pgo2.sortVal();
}

Logical operator>(const SortDrawer& pgo1, const SortDrawer& pgo2)
{
  return pgo1.sortVal() > pgo2.sortVal();
}

// =============================== PizzaSpoke ============================ //

PizzaSpoke::PizzaSpoke() :
  localAngle(0.0),
  length(0.0),
  worldEndpoint(0.0, 0.0),
  touchingTerrain(false)
{
  
}

// =============================== PizzaFace ============================ //

PizzaFace::PizzaFace(VisRectangular* setParent) :
  ActionListener(),

  parent(setParent),
  saucePercent(0.0),
  glowAlpha(0.0),

  eyesWarper(),
  mouthWarper(),
  noseWarper(),
  accWarper(),

  faceScript(this),

  toppingPlacementData(TOPPINGS_PER_SELECTION * NUM_SELECTED_TOPPINGS),

  faceState(IDLE),
  idleState(BREATHE),
  frameTarget(0),
  targetHFlip(false),
  currHFlip(false),

  currWeight(0.0),
  breatheData(0.0, 1.0),

  clothesWarper(),
  clothesWeight(0.0),
  clothesScript(),

  useOverride(false),
  overrideDuration(0.2)
{
  resetFace();
  idleTrigger(BREATHE);  // blink makes it blink AND breathe
}

void PizzaFace::resetFace()
{
  initMySpine(eyesWarper, pizzaEyesSpine);
  initMySpine(mouthWarper, pizzaMouthSpine);
  initMySpine(noseWarper, pizzaNoseSpine);
  initMySpine(accWarper, pizzaAccSpine);

  placeToppings();
}

void PizzaFace::initMySpine(SpineAnimator& anim, Coord1 spineIndex)
{
  anim.init(getSpineSkeleton(spineIndex), getSpineAnim(spineIndex));
  spSkeleton_setToSetupPose(anim.spineSkeleton);
  spSkeleton_updateWorldTransform(anim.spineSkeleton);
  spAnimationState_addAnimationByName(anim.spineAnimState, 0, "idle", false, 0.0);
  anim.spineAnimState->data->defaultMix = 0.1;
}

void PizzaFace::placeToppings()
{
  toppingPlacementData.clear();
  
  for (Coord1 i = 0; i < toppingPlacementData.capacity; ++i)
  {
    Point1 randWeight = 1.0 - (RM::randf() * RM::randf());  // distribute further out
    Point1 randMag = RM::lerp(32.0, 180.0, randWeight);
    Point2 toppingPos = RM::pol_to_cart(randMag, RM::rand_angle());
    Point1 toppingLocalAngle = RM::rand_angle();
    
    toppingPlacementData.add(Point3(toppingPos.x, toppingPos.y, toppingLocalAngle));
  }
}

void PizzaFace::updateFace()
{
  // RM::push_time_mult((faceState == IDLE && idleState == TURN) ? 8.0 : 2.0);
  faceScript.update();
  
  eyesWarper.updateSpineSkeleton();
  mouthWarper.updateSpineSkeleton();
  noseWarper.updateSpineSkeleton();
  accWarper.updateSpineSkeleton();

  clothesScript.update();
  saucePercent = RM::flatten(saucePercent, damagePercent(), 0.25 * RM::timePassed());
  RM::pop_time_mult();
}

void PizzaFace::drawFace()
{
  Logical parentHFlip = parent->getHFlip();

  // draw toppiongs
  for (Coord1 i = 0; i < toppingPlacementData.count; ++i)
  {
    Coord1 imgIndex = pizzaToppings0Img + i / TOPPINGS_PER_SELECTION;
    Point2 localPosition = Point2(toppingPlacementData[i].x, toppingPlacementData[i].y);
    Point1 localAngle = toppingPlacementData[i].z;
    Point2 finalPosition = parent->getXY() + RM::rotate(localPosition * parent->getXScale(),
                                                        parent->getRotation());
    Point1 finalAngle = parent->getRotation() + localAngle;
    
    getImg(imgIndex)->draw_scale(finalPosition, parent->getScale(), finalAngle, parent->getColor());
  }
  
  // prep unrotated
  Point1 myRotation = parent->getRotation();
  parent->setRotation(0.0);
  parent->hflip(currHFlip);
  
  eyesWarper.syncFromVisRect(*parent);
  eyesWarper.renderSpineSkeleton();

  mouthWarper.syncFromVisRect(*parent);
  mouthWarper.renderSpineSkeleton();

//  noseWarper.syncFromVisRect(*parent);
//  noseWarper.renderSpineSkeleton();

//  accWarper.syncFromVisRect(*parent);
//  accWarper.renderSpineSkeleton();

  parent->setRotation(myRotation);
  parent->hflip(parentHFlip);
}

Point1 PizzaFace::damagePercent()
{
  // return std::min(Pizza::currGameVars[VAR_DAMAGE] / 8.0, 1.0);
  return 0.0;
}

Logical PizzaFace::isHurt()
{
  return faceState == HURT;
}

void PizzaFace::actionTrigger(Coord1 actionType, Logical ignorePriority)
{
  // cout << "incoming action " << actionType << endl;
  
  if (faceState >= actionType && ignorePriority == false) return;
  
  faceScript.clear();
  // faceWarper.last_to_cont();
  // mouthWarper.last_to_cont();
  
  currWeight = 0.0;
  faceState = actionType;
  
  switch (faceState)
  {
    case SLAM:
      /*
      frameTarget = 5;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(10.0);
       */
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "slam", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "slam", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "slam", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "slam", 0);
      
      faceScript.wait(eyesWarper.getAnimationDuration("slam"));
      break;
    case BUMP:
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "bump", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "bump", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "bump", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "bump", 0);
      
      faceScript.wait(eyesWarper.getAnimationDuration("bump"));
      // cout << "bump" << endl;
      break;
    case HURT:
      /*
      frameTarget = 6;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.2));
      faceScript.wait(0.4);
       */
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "hurt", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "hurt", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "hurt", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "hurt", 0);

      faceScript.wait(eyesWarper.getAnimationDuration("hurt"));
      // cout << "hurt, duration " << eyesWarper.getAnimationDuration("hurt") << endl;
      break;
    case ROAR_1:
      /*
      frameTarget = 7;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, 2.0));
      break;
       */
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "victory", 0);

      faceScript.wait(eyesWarper.getAnimationDuration("victory"));
      // cout << "hurt" << endl;
    case ROAR_2:
      /*
      frameTarget = 8;
      faceScript.enqueueX(new SineFn(&currWeight, 1.0, 2.0));
       */
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "victory", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "victory", 0);

      faceScript.wait(eyesWarper.getAnimationDuration("victory"));
      break;
  }
  
  useOverride = false;
}

void PizzaFace::idleTrigger(Coord1 idleType)
{
  // cout << "incoming idle " << idleType << endl;

  faceScript.clear();
  // faceWarper.last_to_cont();
  // mouthWarper.last_to_cont();
  currWeight = 0.0;
  
  if (idleType == BLINK && RM::randi(0, 5) != 0) idleType = BREATHE;
  idleState = idleType;
  faceState = IDLE;
  
  switch (idleState)
  {
    case BREATHE:
      /*
      frameTarget = frameTarget != 1 ? 1 : 2;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.5));
       */
      // eyesWarper.startAnimation("idle", 0, true);
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "idle", 1);

      faceScript.wait(eyesWarper.getAnimationDuration("idle"));
      // cout << "breathe" << endl;
      break;
    case BLINK:
      /*
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "blink", 0);

      faceScript.wait(eyesWarper.getAnimationDuration("blink"));
       */
      
      // blink does the same thing as breathe right now
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "idle", 1);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "idle", 1);
      faceScript.wait(eyesWarper.getAnimationDuration("idle"));

      // cout << "blink" << endl;
      break;
    case TURN:
      /*
      frameTarget = 9;
      overrideDuration = 0.2;
      faceScript.enqueueX(new LinearFn(&currWeight, 1.0, useOverride ? overrideDuration : 0.1));
      faceScript.enqueueX(new SetValueCommand<Logical>(&currHFlip, targetHFlip));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&frameTarget, 0));
      faceScript.enqueueX(new SetValueCommand<Coord1>(&idleState, BLINK));
      faceScript.enqueueX(new SetValueCommand<Logical>(&useOverride, true));
      
      clothesScript.enqueueX(new LinearFn(&clothesWeight, 1.0, 0.1));
      clothesScript.enqueueX(new LinearFn(&clothesWeight, 0.0, 0.1));
       */
      spAnimationState_setAnimationByName(eyesWarper.spineAnimState, 0, "turn", 0);
      spAnimationState_setAnimationByName(mouthWarper.spineAnimState, 0, "turn", 0);
      spAnimationState_setAnimationByName(noseWarper.spineAnimState, 0, "turn", 0);
      spAnimationState_setAnimationByName(accWarper.spineAnimState, 0, "turn", 0);

      Point1 turnDur = eyesWarper.getAnimationDuration("turn");
      faceScript.wait(turnDur * 0.5);
      faceScript.enqueueX(new SetValueCommand<Logical>(&currHFlip, targetHFlip));
      // faceScript.wait(turnDur * 0.5);
      faceScript.enqueueX(new SetValueCommand<Coord1>(&idleState, BREATHE));
      faceScript.enqueueX(new SetValueCommand<Logical>(&useOverride, true));
      // cout << "turn" << endl;
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
      idleTrigger(BREATHE);
    }
    else
    {
      // cout << "turn" << endl;
      idleTrigger(TURN);
    }
  }
}

// =============================== PizzaRenderer ============================ //

/*
PizzaRenderer::PizzaRenderer(VisRectangular* setParent) :
  parent(setParent),
  face(setParent)
{
  
}

void PizzaRenderer::update()
{
  face.updateFace();
}

void PizzaRenderer::redraw()
{
  
}
 */

// =============================== PizzaPlayer ============================ //

PizzaPlayer::PizzaPlayer() :
  PizzaGOStd(),

  face(this),

  standingTimer(0.2),
  nojumpTimer(0.22),  // must be at least 1/60 greater than standing timer
  isStanding(false),
  isSlamming(false),
  usedDoubleJump(false),
  waterJustReported(false),

  oldXY(0.0, 0.0),
  facing(1),

  slamAlpha(0.0),
  slamRotation(0.0),
  slamYVel(0.0),

  starTimer(5.0),
  starMaker(0.5, true, this),

  gourdoFlyTimer(10.0),

  flasher(),
  invincibilityTimer(0.01),

  spokeList(NUM_PIZZA_SPOKES),
  fullSpokeLength(DEFAULT_SPOKE_LENGTH),

  balloonList(8),

  rollForce(PLAYER_ROLL_FORCE),
  rollTorque(PLAYER_ROLL_TORQUE),

  shooters(this),

  lastRaycastFixture(NULL),
  lastRaycastPt(0.0, 0.0),
  lastRaycastNormal(0.0, 0.0),
  lastRaycastFraction(0.0)
{
  phase = PHASE_PLAYER;
  type = TYPE_PLAYER;
  
  setXY(Point2(PIZZA_INNER_RADIUS, PIZZA_INNER_RADIUS));
  setWH(Point2(PIZZA_INNER_RADIUS, PIZZA_INNER_RADIUS)); // width = radius
  oldXY = getXY();
  
  invincibilityTimer.reset(1.0 * Pizza::getEquippedSeasoningLevel(SEAS_INVINCIBILITY_0));
  invincibilityTimer.setInactive();
  
  gourdoFlyTimer.setInactive();
}

void PizzaPlayer::load()
{
  // this is all done here so that the size can be set manually before this gets called
  init_circle(getXY(), getWidth(), true);
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(1.0);
  fixture->SetUserData(this);
  
  for (Coord1 i = 0; i < NUM_PIZZA_SPOKES; ++i)
  {
    spokeList.add(PizzaSpoke());
    spokeList.last().localAngle = (TWO_PI / NUM_PIZZA_SPOKES) * i;
    spokeList.last().length = fullSpokeLength;
    spokeList.last().worldEndpoint = RM::pol_to_cart(spokeList.last().length, spokeList.last().localAngle) + getXY();
  }
}

void PizzaPlayer::updateMe()
{
  if (lifeState == RM::PASSIVE) return;

  oldXY = getXY();
  Coord1 oldAngBossDir = 1;

  updateFromPhysical();
  updateStanding();
  updateFlying();
  shooters.update();

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

  if (waterJustReported == true)
  {
    body->SetLinearDamping(2.0);
    body->SetAngularDamping(2.0);
  }
  else
  {
    body->SetLinearDamping(0.0);
    body->SetAngularDamping(0.0);
  }
  
  Coord1 floatyLevel = Pizza::getEquippedSeasoningLevel(SEAS_FLOATY_0);
  
  // apply "floaty" seasoning
  if (floatyLevel >= 1 &&
      isSlamming == false &&
      isStanding == false &&
      waterJustReported == false &&
      get_velocity().y >= 0.0)
  {
    Point1 newYVel = get_velocity().y;
    Point1 addition = RM::approach_atan(0.0, 500.0, ((Point1) floatyLevel) * 0.333);
    newYVel += -addition * RM::timePassed();
    set_velocity(Point2(get_velocity().x, newYVel));
  }

  waterJustReported = false;
  
  updateSpokes();
  updateFireblockDetector();
  updateLightsaberDmg();
  face.updateFace();
  flasher.update();
  invincibilityTimer.update();
}

void PizzaPlayer::redrawMe()
{
  if (invincibilityTimer.getActive() == false ||
      flasher.flashIsOn() == false)
  {
    drawSquishyBase();
    face.drawFace();
  }

  flasher.redraw();
  shooters.redraw();
  
  if (Pizza::DRAW_DEBUG)
  {
    Circle(getXY(), getWidth()).draw_outline(GREEN_SOLID);
    Circle(getXY(), fullSpokeLength).draw_outline(BLUE_SOLID);

    if (gourdoFlyTimer.getActive() == true)
    {
      Circle(getXY(), 16.0).draw_solid(GREEN_SOLID);
    }
  }
  
  if (level->shieldActive == true)
  {
    Circle(getXY(), getWidth() + 2.0).draw_outline(CYAN_SOLID);
  }
}

void PizzaPlayer::updateFlying()
{
  gourdoFlyTimer.update();
}

void PizzaPlayer::drawSquishyBase()
{
  // draw pizza
  for (Coord1 i = 0; i < NUM_PIZZA_SPOKES; ++i)
  {
    /*
     Point2 posPt0 = getXY();
     Point2 posPt1 = spokeList[i].worldEndpoint;
     Point2 posPt2 = spokeList[(i + 1) % NUM_PIZZA_SPOKES].worldEndpoint;
     */
    Point2 posPt0 = getXY();
    Point2 posPt1 = RM::ring_edge(getXY(), spokeList[i].worldEndpoint, spokeList[i].length) + getXY();
    Point2 posPt2 = RM::ring_edge(getXY(), spokeList[(i + 1) % NUM_PIZZA_SPOKES].worldEndpoint,
                                  spokeList[(i + 1) % NUM_PIZZA_SPOKES].length) + getXY();
    
    Point1 visualRadius = 0.46;
    Point2 uv0 = Point2(0.5, 0.5);
    Point2 uv1 = Point2(0.5, 0.5) + RM::pol_to_cart(visualRadius, spokeList[i].localAngle);
    Point2 uv2 = Point2(0.5, 0.5) + RM::pol_to_cart(visualRadius, spokeList[(i + 1) % NUM_PIZZA_SPOKES].localAngle);
    
    getImg(pizzaBaseImg)->draw(Tri(posPt0, posPt1, posPt2), Tri(uv0, uv1, uv2), getColor());
    
    // Line(getXY(), posPt1).draw(GREEN_SOLID);
  }
}

void PizzaPlayer::createLightsaber()
{
  Point2 saberSize = Point2(280.0, 16.0);
  Point2 saberCenter = Point2(getWidth() + saberSize.x * 0.45, -saberSize.y * 0.5);
  lightsaberFixtures.add(create_box_fixture_center(body, saberCenter, saberSize, 0.0));
  lightsaberFixtures.first()->SetUserData(this);
  set_collisions(lightsaberFixtures.first(), 1 << TYPE_PLAYER_LIGHTSABER, 0xffff & (~(0x1 << TYPE_TERRAIN)));
}

void PizzaPlayer::drawLightsaber()
{
  if (lightsaberFixtures.count == 0)
  {
    return;
  }
  
  Point2 saberSize = Point2(256.0, 16.0);
  Point2 saberCenter = getXY() + Point2(getWidth() + saberSize.x * 0.5, -saberSize.y * 0.5);

  Box saberBox = Box::from_center(saberCenter, saberSize);
  Quad saberQuad(saberBox);
  
  for (Coord1 i = 0; i < saberQuad.count(); ++i)
  {
    saberQuad[i] = RM::rotate(getXY(), saberQuad[i], getRotation());
  }
  
  saberQuad.draw_solid(BLUE_SOLID);
}


void PizzaPlayer::tryJump()
{
  if (canJump() == true)
  {
    /*
    ResourceManager::playJump();
    if ((level->playerMotionFlags & STICK_BIT) != 0)
    {
      level->addAction(new GooSplash(Point2(getX(), collisionCircle().bottom())));
    }
     */

    if (face.faceState == PizzaFace::SLAM)
    {
      face.actionTrigger(PizzaFace::BUMP);  // override slam
    }

    // takes sticky into account
    bounceUp(level->bounceMult() + 0.125 * Pizza::getEquippedSeasoningLevel(SEAS_JUMP_HEIGHT_0));
    usedDoubleJump = false;
  }
  else if (canDoubleJump() == true)
  {
    bounceUp(0.25 + 0.25 * Pizza::getEquippedSeasoningLevel(SEAS_DOUBLE_JUMP_0));
    usedDoubleJump = true;
  }
  else if (canSlam() == true)
  {
    // ResourceManager::playSlamStart();
    slamDown();
  }
}

Logical PizzaPlayer::canJump()
{
  Logical kindaStanding = isStanding == true ||
      standingTimer.getActive() == true ||
      gourdoFlyTimer.getActive() == true;
  
  Logical result =
    level->jumpEnabled() == true &&
    kindaStanding == true &&
    nojumpTimer.getActive() == false &&
    getActive() == true;

  return result;
}

void PizzaPlayer::gotGourdo()
{
  gourdoFlyTimer.reset();
}

Logical PizzaPlayer::canSlam()
{
  Logical result =
      isSlamming == false &&
      isStanding == false &&
      level->slamDisabled() == false;

  return result;
}

Logical PizzaPlayer::canDoubleJump()
{
  Logical result =
      Pizza::getEquippedSeasoningLevel(SEAS_DOUBLE_JUMP_0) >= 1 &&
      level->jumpEnabled() == true &&
      usedDoubleJump == false &&
      nojumpTimer.getActive() == false &&
      get_velocity().y >= -300.0 &&
      isSlamming == false &&
      isStanding == false;
  
  return result;
}

void PizzaPlayer::bounceUp(Point1 percentOfJump)
{
  Point2 currVel = get_velocity();
  set_velocity(Point2(currVel.x, PLAYER_JUMP_VEL * percentOfJump));

  isStanding = false;
  standingTimer.setActive(false);
  nojumpTimer.reset();
}

void PizzaPlayer::reportInWater()
{
  waterJustReported = true;
}

void PizzaPlayer::slamDown()
{
  Point2 currVel = get_velocity();
  slamYVel = std::max(-PLAYER_JUMP_VEL * 1.35 * level->bounceMult(), currVel.y * 1.5);
  slamYVel *= 1.0 + 0.125 * Pizza::getEquippedSeasoningLevel(SEAS_STOMPER_0);

  set_velocity(Point2(currVel.x, slamYVel));
  slamRotation = RM::angle(get_velocity());
  isSlamming = true;
  
  // swing lightsaber
  /*
  if (lightsaberFixture != NULL)
  {
    Logical bossBelow = collisionBox().grow_down(1008.0).collision(level->boss->collisionBox());
    Logical saberDown = RM::standardize_rads(getRotation()) >= PI * 0.3 && RM::standardize_rads(getRotation()) <= PI * 0.7;
    
    if (bossBelow == true && saberDown == true)
    {
      // don't swing
      set_ang_vel(0.0);
    }
    else
    {
      // swing
      Point1 angleToBoss = RM::angle(getXY(), level->boss->getXY());
      Point1 angDir = RM::standardize_rads(angleToBoss - getRotation()) < PI ? 1.0 : -1.0;
      set_ang_vel(angDir * TWO_PI);
    }
  }
   */

  nojumpTimer.setInactive();

  face.actionTrigger(PizzaFace::SLAM);
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

  if (oldStanding == false && canJump() == true)
  {
    // just landed
    if (isSlamming == true)
    {
      /*
      StarParticles* leftStars = new StarParticles(standingXY, RM::angle(getXY(), standingXY) - HALF_PI);
      level->addAction(leftStars);

      StarParticles* rightStars = new StarParticles(standingXY, RM::angle(getXY(), standingXY) + HALF_PI);
      level->addAction(rightStars);

      ResourceManager::playSlamLand();
       */
    }

    level->reportPlayerLanded(isSlamming);
    isSlamming = false;
  }
}

void PizzaPlayer::tryMove(Point1 normMagnitude)
{
  Point2 centerOfMass = b2dpt_to_world(body->GetWorldCenter());

  Point1 armLength = getWidth() * 0.9 * RM::sign(normMagnitude);
  Point1 torqueMult = 1.0;
  
  if (isStanding == false)
  {
    torqueMult = 0.0;
  }

  // this is to turn faster
  Point1 turnMult = 1.0;
  
  if ((normMagnitude < 0.0 && get_velocity().x > 0.0) ||
      (normMagnitude > 0.0 && get_velocity().x < 0.0))
  {
    turnMult *= 2.5;
  }

  Point1 accelSeasoningMult = 1.0 + 0.3 * Pizza::getEquippedSeasoningLevel(SEAS_ACCEL_0);
  Point1 finalRollForce = rollForce * normMagnitude * turnMult * accelSeasoningMult;
  Point1 finalRollTorque = rollTorque * torqueMult * std::abs(normMagnitude) * accelSeasoningMult;

  apply_force(Point2(finalRollForce, finalRollTorque),
              centerOfMass + Point2(armLength * torqueMult, 0.0));
}

void PizzaPlayer::updateFireblockDetector()
{
  if (balloonList.count <= 0)
  {
    return;
  }
  
  TerrainQuad* burningObj = NULL;
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = level->objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->getActive() == false ||
        enemy->type != TYPE_TERRAIN ||
        enemy->value != VALUE_BURNING_BLOCK)
    {
      continue;
    }
    
    Box testBox = collisionBox().grow_down(getHeight() * 4.0);
    TerrainQuad* testObj = static_cast<TerrainQuad*>(enemy);

    if (testObj->balloonStartedSeekingMe == false &&
        testBox.collision(enemy->collisionBox()))
    {
      burningObj = testObj;
      break;
    }
  }
  
  if (burningObj == NULL)
  {
    return;
  }
  
  burningObj->balloonStartedSeekingMe = true;
  balloonList.first()->seekObject(burningObj);
  balloonList.remove_first();
}

void PizzaPlayer::setFacing(Coord1 newFacing)
{
  facing = newFacing;
  
  if (facing == -1) face.targetHFlip = true;
   else face.targetHFlip = false;
}

void PizzaPlayer::updateSpokes()
{
  for (Coord1 i = 0; i < NUM_PIZZA_SPOKES; ++i)
  {
    Point1 worldAngle = RM::standardize_rads(getRotation() + spokeList[i].localAngle);
    spokeList[i].worldEndpoint = RM::pol_to_cart(fullSpokeLength, worldAngle) + getXY();
    Point1 targetSpokeLength = fullSpokeLength;
    spokeList[i].touchingTerrain = false;

    lastRaycastFixture = NULL;
    b2Vec2 b2dSstartPoint = wpt_to_b2d(getXY());
    b2Vec2 b2dEndPoint = wpt_to_b2d(spokeList[i].worldEndpoint);
    
    RMPhysics->RayCast(this, b2dSstartPoint, b2dEndPoint);
    
    // I'm not sure if box2d returns the obstacles in order of closest to farthest
    if (lastRaycastFixture != NULL &&  // this happens if there is no collision
        ((lastRaycastFixture->GetFilterData().maskBits & fixture->GetFilterData().maskBits) != 0) &&  // make sure the player collides with this
        (lightsaberFixtures.count == 0 || lastRaycastFixture != lightsaberFixtures.first()) &&  // don't squish with your own lightsaber
        lastRaycastFixture->IsSensor() == false)  // don't squish to sensors since they are not physical objects
    {
      targetSpokeLength = RM::distance_to(getXY(), lastRaycastPt);

      spokeList[i].worldEndpoint = lastRaycastPt;
      spokeList[i].touchingTerrain = true;
    }

    RM::flatten_me(spokeList[i].length, targetSpokeLength, 96.0 * RM::timePassed());
  }
}

void PizzaPlayer::attacked(const Point2& atkPoint, PizzaGOStd* attacker)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return;
  if (level->levelState != PhysicsLevelStd::LEVEL_PLAY) return;
  // if (starActive() == true) return;
  if (face.isHurt() == true) return;
  if (invincibilityTimer.getActive() == true) return;
  
  if (level->tryHitShield() == true)
  {
    // do some sort of particle effect here
    return;
  }

  // level->pizzaDamaged();
  addDamage(1.0);
  face.actionTrigger(PizzaFace::HURT);
  isSlamming = false;  // cancel the slam
  
  if (Pizza::getEquippedSeasoningLevel(SEAS_INVINCIBILITY_0) >= 1)
  {    
    invincibilityTimer.reset();
  }
  
  // level->addSauceEffect(*this, atkPoint);
  damaged(atkPoint, attacker);
}

void PizzaPlayer::damaged(const Point2& atkPoint, PizzaGOStd* attacker)
{
  Point2 glocalEdgePoint = getXY() + RM::pol_to_cart(getWidth(), RM::angle(getXY(), atkPoint));
  Point2 bounceVector = RM::ring_edge(glocalEdgePoint, getXY(), 700.0);
  bounceVector.x = RM::clamp(bounceVector.x, -450.0, 450.0);
  set_velocity(bounceVector);
  // cout << "bounce " << get_velocity().y << endl;
  
  // ResourceManager::playDamaged();
  // Pizza::currGameVars[VAR_DAMAGE]++;
  
  for (Coord1 i = 0; i < 2; ++i)
  {
    if (balloonList.count == 0)
    {
      break;
    }
    
    balloonList.first()->flyAway();
    balloonList.remove_first();
  }
}

void PizzaPlayer::touchedFire(Point2 location, PizzaGOStd* attacker)
{
  attacked(location, attacker);
}

void PizzaPlayer::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  // cout << "normal " << normalImpulse << endl;

  if (normalImpulse >= 1000.0)
  {
    // ResourceManager::playLanding();
    face.actionTrigger(PizzaFace::BUMP);
  }
  else if (normalImpulse > 250.0 && face.faceState == PizzaFace::SLAM)
  {
    // this cancels a slam face on a glancing blow
    face.actionTrigger(PizzaFace::BUMP);
  }
  
  // level->reportPlayerBump(normalImpulse);
}

void PizzaPlayer::shootFire()
{
  if (getActive() == false)
  {
    return;
  }
  
  DragonFireballStd* fireball = new DragonFireballStd();
  fireball->setXY(getXY());
  fireball->vel = Point2(facingToDir() * 500.0, 0.0);
  level->addPGO(fireball);
}

void PizzaPlayer::shootLightning()
{
  if (getActive() == false)
  {
    return;
  }
  
  PizzaGOStd* target = NULL;
  
  // will target an enemy that's both onscreen and in the same direction that the player
  // is facing
  for (LinkedListNode<PizzaGOStd*>* enemyNode = level->objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;
    
    if (enemy->type != TYPE_ENEMY ||
        enemy->getActive() == false ||
        enemy->onRMScreen() == false)
    {
      continue;
    }
    
    PlayerLightningStd* lightningEffect = new PlayerLightningStd();
    lightningEffect->setXY(getXY());
    lightningEffect->target = enemy->collisionCircle().xy;
    level->addPGO(lightningEffect);
    
    enemy->strikeWithLightning();
  }
  
  if (target != NULL)
  {
    // lightning effect
  }
}

// from b2RayCastCallback
float32 PizzaPlayer::ReportFixture(b2Fixture* hitFixture, const b2Vec2& point,
                                   const b2Vec2& normal, float32 fraction)
{
  if (hitFixture == fixture)
  {
    return -1;
  }
  
  lastRaycastFixture = hitFixture;
  lastRaycastPt = b2dpt_to_world(point);
  lastRaycastNormal = b2dpt_to_world(normal);
  lastRaycastFraction = fraction;
  
  return lastRaycastFraction;
}

Box PizzaPlayer::collisionBox()
{
  return Box::from_center(getXY(), getSize() * 2.0);
}

// =============================== "fuse" ============================ //

UnarmedSkeleton::UnarmedSkeleton() :
  PizzaGOStd(),

  walkSpeed(RM::randf(40.0, 60.0)),

  directionChangeTimer(0.5),
  willBackAway(false),
  movingForward(true)
{
  type = TYPE_ENEMY;
  basePoints = 3;
}

void UnarmedSkeleton::load()
{
  setWH(40.0, 44.0);
  
  facePlayer();
  sortY = getY();
  useFishRotation(true);
  
  switch (RM::randi(0, 3))
  {
    default:
    case 0:
      initSpineAnim(unarmedSkeletonSpine, "walk0");
      mySpineAnim.setSkin("var0");
      break;
    case 1:
      initSpineAnim(unarmedSkeletonSpine, "walk1");
      mySpineAnim.setSkin("var1");
      break;
    case 2:
      initSpineAnim(unarmedSkeletonSpine, "walk2");
      mySpineAnim.setSkin("var2");
      break;
    case 3:
      initSpineAnim(unarmedSkeletonSpine, "walk3");
      mySpineAnim.setSkin("var3");
      break;
  }  
}

void UnarmedSkeleton::updateMe()
{
  walkAlongPlatform(walkSpeed, true, true, true);
  
  updateSpineAnim(0.5);
  
  updateCollisions();
}

void UnarmedSkeleton::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  // visibleAABB().draw_solid(BLACK_SOLID); /////////////

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    
//    Box drawBox = getLocalDrawAABB();
//    drawBox.xy += getXY();
//    drawBox.draw_solid(ColorP4(0.0, 1.0, 0.0, 0.5));
  }
}

void UnarmedSkeleton::drawSorted()
{
  // drawMe();
}

void UnarmedSkeleton::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    /*
     createMagnetCoin();
     destroyPhysicalStd(Coord2(3, 4), getImage());
     smokePuffStd();
     */
    level->enemyDefeated(this);
  }
}

// =============================== SkeletonSpearman ============================ //

SkeletonSpearman::SkeletonSpearman(Coord1 setValue) :
  PizzaGOStd(),

  walkSpeed(RM::randf(40.0, 60.0)),
  attackCircle(Point2(0.0, 0.0), 16.0)
{
  value = setValue;
  basePoints = 10;
}

void SkeletonSpearman::load()
{
  setWH(value == VAL_SIDEWAYS ? 40.0 : 32.0, 48.0);

  facePlayer();
  useFishRotation(true);
  sortY = getY();
  
  if (value == VAL_SIDEWAYS)
  {
    initSpineAnim(horiSpearmanSpine, "walk");
  }
  else if (value == VAL_VERTICAL)
  {
    initSpineAnim(vertSpearmanSpine, "walk");
  }
}

void SkeletonSpearman::updateMe()
{
  walkAlongPlatform(walkSpeed, true, true, true);
 
  updateSpineAnim(0.4 * (walkSpeed / 40.0));
  
  updateCollisions();
}

void SkeletonSpearman::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle.draw_outline(RED_SOLID);
  }
}

void SkeletonSpearman::drawSorted()
{
  // drawMe();
}

void SkeletonSpearman::updateCollisions()
{
  attackCircle = mySpineAnim.calcAttachmentCircle("attack_collision", "attack_collision");

  Logical attackHitPlayer = Circle::collision(attackCircle, player->collisionCircle());
  
  if (value == VAL_VERTICAL && player->standingTimer.getActive() == true)
  {
    attackHitPlayer = false;
  }
  else if (value == VAL_SIDEWAYS && player->collisionCircle().bottom() < level->getGroundY() - 32.0)
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
  else if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    /*
    createMagnetCoin();
    destroyPhysicalStd(Coord2(3, 4), getImage());
    smokePuffStd();
     */
    level->enemyDefeated(this);
  }
}


// =============================== Spiny ============================ //

Spiny::Spiny() :
  PizzaGOStd(),

  flipped(false),
  airState(ON_GROUND),

  yVel(0.0),
  rotFlipper(&rotation, -PI, 0.5),
  flipTimer(4.5, this),

  shouldFollowPlayer(true)
{
  basePoints = 20;
  setWH(48.0, 48.0);  // this is radius and extends under ground
  
  // shadowImg = imgCache[shadowSmall];
  
  rotFlipper.setActive(false);
  flipTimer.setActive(false);
}
  
void Spiny::load()
{
  facePlayer();
  sortY = getY();
  platformOffset = Point2(0.0, -getHeight());
  
  setY(myPlatformLine.get_y_at_x(getX()) + platformOffset.y);
  useFishRotation(true);

  initSpineAnim(spineySpine, "walk");
}

void Spiny::updateMe()
{
  if (flipped == false &&
      airState == ON_GROUND)
  {
    // this AI is used in gladiator
    if (shouldFollowPlayer == true)
    {
      walkAlongPlatform(100.0, true, true, false);
      
      if (getHFlip() == true && getX() < player->getX() - 256.0) hflip(false);
      if (getHFlip() == false && getX() > player->getX() + 256.0) hflip(true);
    }
    // this ai is used in the chunks
    else
    {
      walkAlongPlatform(100.0, true, true, true);
    }
    
  }
  else
  {
    updatePop();
  }
  
  updateSpineAnim(0.8);
  updateCollisions();
}

void Spiny::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    Circle(getXY(), 4.0).draw_solid(WHITE_SOLID);
  }
}

void Spiny::updatePop()
{
  rotFlipper.update();
  flipTimer.update();
  
  if (airState != ON_GROUND)
  {
    yVel += 1100.0 * RM::timePassed();
    addY(yVel * RM::timePassed());
  }
  
  // NOTE: If the spiney image is handled bottom center, it needs to land higher when it flips.
  //   If the spine image is handled center, it needs a draw offset that centers the rendering.
  
  // landed on ground on back
  if (airState == TO_BACK &&
      yVel > 0.0 && getY() >= groundTouchY())
  {
    airState = ON_GROUND;
    setY(groundTouchY());
  }
  // landed on ground on front
  else if (airState == TO_FRONT &&
           yVel > 0.0 && getY() >= groundTouchY())
  {
    airState = ON_GROUND;
    setY(groundTouchY());
  }
}

Point1 Spiny::groundTouchY()
{
  return myPlatformLine.get_y_at_x(getX()) - getHeight();
}

void Spiny::pizzaSlammed()
{
  if (getActive() == false) return;
  if (std::abs(player->getX() - getX()) > RM_WH.x * 0.75) return;
  
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
  yVel = -400.0;
  
  Point1 rotTarget = PI;
  if (getHFlip() == true) rotTarget = -PI;
  
  flipTimer.reset();
  rotFlipper.reset(getRotation() + rotTarget);
}

void Spiny::flipToFront(Logical fromSlam)
{
  flipped = false;
  airState = TO_FRONT;
  yVel = -400.0;
  
  Point1 rotTarget = PI;
  if (getHFlip() == true) rotTarget = -PI;
  
  if (fromSlam == true) rotTarget *= -1.0;
  
  rotFlipper.reset(getRotation() + rotTarget);
}

void Spiny::updateCollisions()
{
  if (getActive() == false) return;

  if (touchingPlayerCircle() == true)
  {
    if (flipped == true || (flipped == false && airState == TO_FRONT))
    {
      /*
      smokePuffStd();
      createMagnetCoin(NICKEL);
      destroyPhysicalStd(Coord2(3, 4), getImage());
       */
      destroyPhysical();
      level->enemyDefeated(this);
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

// =============================== DonutHoleCannonball ============================ //

DonutHoleCannonball::DonutHoleCannonball() :
  PizzaGOStd(),

  breakTimer(2.5, this)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_ENEMY;
  
  setImage(getImg(donutHoleImg));
  autoSize(0.5);  // radius
  
  coinToDropType = NO_COIN;
}

void DonutHoleCannonball::load()
{
  sortY = myPlatformLine.get_y_at_x(getX());
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger
  
  change_density(3.0);
  set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);
  set_ang_vel(PI * facingToDir());
  set_velocity(Point2(500.0 * facingToDir(), 0.0));
  set_restitution(0.7);
  fixture->SetUserData(this);
}

void DonutHoleCannonball::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  if (lifeState == RM::ACTIVE && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysical();
    
    // level->addAction(new BombExplosion(getXY()));
  }
}


void DonutHoleCannonball::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void DonutHoleCannonball::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  destroyPhysical();
}

// =============================== DonutCanon ============================ //

const Point1 DonutCannon::WALK_SPEED = 256.0;

DonutCannon::DonutCannon() :
  PizzaGOStd(),

  faceAnim(),
  currSpeed(256.0),
  bodyRotation(0.0)
{
  basePoints = 10;
}

void DonutCannon::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation(true);  // its actual "rotation" is for its face
  
  initSpineAnim(donutSpine, "walk_body", true);
  initSpineAnim(faceAnim, donutSpine, "walk_face", true);
  
  setWH(collisionCircle().radius, collisionCircle().radius);  // radius
  
  platformOffset.y = -collisionCircle().radius;
  addY(platformOffset.y);

  callback(&script);
}

void DonutCannon::updateMe()
{
  script.update();
  
  /*
  Point1 walkDistance = currSpeed * facingToDir() * RM::timePassed();
  addX(walkDistance);
  turnAtWalls();
   */
  Point1 walkDistance = currSpeed * facingToDir() * RM::timePassed();
  walkAlongPlatform(currSpeed, true, true, true);
  
  // its actual "rotation" is for its face
  Point1 circumference = PI * getWidth() * 2.0;
  bodyRotation += (TWO_PI * walkDistance) / circumference;
  
  updateSpineAnim();
  updateSpineAnim(faceAnim);

  updateCollisions();
}

void DonutCannon::redrawMe()
{
  // render body, which uses a special rotation, and ignores hflip
  Logical realHFlip = getHFlip();
  Point1 realRotation = getRotation();
  hflip(false);
  setRotation(bodyRotation);
  PizzaGOStd::redrawMe();
  hflip(realHFlip);
  setRotation(realRotation);

  // render face, which uses the "regular" values
  renderSpineAnim(faceAnim);
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void DonutCannon::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

void DonutCannon::midCallback(Coord1 value)
{
  switch (value)
  {
    default:
    case 0:
    {
      // don't shoot if you're not onscreend
      if (onRMScreen() == false)
      {
        return;
      }

      Point1 ballAngle = getX() > player->getX() ? PI : 0.0;
      
      DonutHoleCannonball* ball = new DonutHoleCannonball();
      ball->setXY(getXY());
      level->addPGO(ball);
      // ball->set_velocity(Point2(500.0 * facingToDir(), -128.0));
      Point2 ballVel = RM::pol_to_cart(500.0, ballAngle);
      ballVel.y -= 128.0; // technically this should be a change to its angle but this is okay
      ball->set_velocity(ballVel);
      break;
    }
  }
}

void DonutCannon::callback(ActionEvent* caller)
{
  mySpineAnim.startAnimation("walk_body", 0, true);
  faceAnim.startAnimation("walk_face", 0, true);

  script.wait(1.0);
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.5));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_body", 0));
  script.enqueueX(new StartSpineCommand(&faceAnim, "shoot_face", 0));
  script.wait(0.5);
  script.enqueueMidCallback(this, 1);
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, -WALK_SPEED));
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.5));
  script.enqueueX(new SineFn(&currSpeed, WALK_SPEED, 0.25));
}

// =============================== EntRoot ============================ //

EntRoot::EntRoot() :
  PizzaGOStd(),

  visibleBox(),
  dangerous(false)
{
  phase = PHASE_BG_TERRAIN;
  type = TYPE_ENEMY;
}

void EntRoot::load()
{
  sortY = getY();
  
  initSpineAnim(entRootSpine, "attack", false);
  updateSpineAnim(0.001);  // this just keeps it from flashing onscreen
  
  setWH(visibleAABB().wh);  // the actual collision is only the tip of the root
  visibleBox = Box::from_norm(getXY(), getSize(), HANDLE_BC);
  
  script.wait(0.25);
  script.enqueueX(new SetValueCommand<Logical>(&dangerous, true));
  script.enqueueX(new SineFn(&xy.y, getY() - getHeight(), 0.25));
  script.wait(0.1);
  script.enqueueX(new SineFn(&xy.y, getY(), 0.25));
  script.enqueueX(new SetValueCommand<Logical>(&dangerous, false));
}

void EntRoot::updateMe()
{
  script.update();
  
  updateSpineAnim();
  
  updateCollisions();
}

void EntRoot::redrawMe()
{
  RMGraphics->clip_world(visibleBox);
  PizzaGOStd::redrawMe();
  RMGraphics->unclip();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}


Circle EntRoot::collisionCircle()
{
  return Circle(getXY() + Point2(0.0, 8.0), 4.0);
}

void EntRoot::updateCollisions()
{
  if (hitThisAttack == false &&
      touchingPlayerCircle() == true)
  {
    hitThisAttack = true;
    player->attacked(getXY(), this);
  }
}

void EntRoot::callback(ActionEvent* caller)
{
  setRemove();
}

// =============================== Ent ============================ //

const Point1
Ent::WALK_SPEED = 96.0,
Ent::WALL_PAD = 192.0;

Ent::Ent() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED),

  animState(WALKING)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  basePoints = 10;
}

void Ent::load()
{
  facePlayer();
  sortY = getY();
  
  initSpineAnim(entBodySpine, "walk");

  setWH(collisionCircle().radius, collisionCircle().radius);
  
  DataList<Point2> currPtList;
  currPtList.add(Point2(48.0, -598.0));
  currPtList.add(Point2(48.0, -63.0));
  currPtList.add(Point2(91.0, 0.0));
  currPtList.add(Point2(-91.0, 0.0));
  currPtList.add(Point2(-48.0, -63.0));
  currPtList.add(Point2(-48.0, -598.0));
  
  for (Coord1 i = 0; i < currPtList.count; ++i)
  {
    // this is because the coords were done at 2x
    currPtList[i] *= 0.5;
  }
  
  init_shape(getXY(), currPtList, true);
  
  set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
  set_b2d_callback_data(this);

  crushesPhysical = false;
  crushValue = 650.0;

  fixture->SetDensity(125.0);
  body->ResetMassData();

  callback(&script);
}

void Ent::updateMe()
{
  switch (animState)
  {
    default:
    case WALKING:
    {
      Point2 currVel(0.0, 0.0);
      
      // if stable, walk
      Point1 walkForceMult = currSpeed / WALK_SPEED;
      
      body->ApplyForce(wpt_to_b2d(Point2(80000.0 * walkForceMult * facingToDir(), 0.0)),
                       body->GetWorldCenter(),
                       true);
      
      currVel = b2dpt_to_world(body->GetLinearVelocity());
      
      // enforce max walking speed
      if (std::abs(currVel.x) > WALK_SPEED)
      {
        // cout << "old vel " << body->GetLinearVelocity().x << " " << body->GetLinearVelocity().y << endl;
        
        b2Vec2 newVel = wpt_to_b2d(Point2(WALK_SPEED * facingToDir(), currVel.y));
        body->SetLinearVelocity(newVel);
        
        // cout << " new vel " << newVel.x << " " << newVel.y << endl;
      }

      turnAtWalls();
      currSpineTimeMult = 0.4 * (std::abs(currVel.x) / WALK_SPEED);

      if (isStable() == false)
      {
        startUnstable();
      }
      else
      {
        script.update();
      }
      
      break;
    }
      
    case ATTACKING:
    {
      if (isStable() == false)
      {
        startUnstable();
      }
      else
      {
        script.update();
      }
      break;
    }
      
    case GROUNDED:
    {
      // this is if it restabilized
      if (isStable() == true)
      {
        animState = WALKING;

        script.clear();
        callback(&script);
        
        spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "walk", 1);
      }

      break;
    }
  }
  
  if (fullyGrounded() == true)
  {
    crushesPhysical = true;
  }
  else
  {
    crushesPhysical = false;
  }
//  setRotation(HALF_PI * 0.5);
//  crushesPhysical = true;
  
  checkPhysicalDestruction();
  
  updateFromPhysical();
  updateCollisions();
  
  updateSpineAnim(currSpineTimeMult);

}

void Ent::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    Box visAABB = visibleAABB();
    visAABB.draw_outline(GREEN_SOLID);
  }
}

Logical Ent::isStable()
{
  return RM::approxEq(getRotation(), 0.0, 0.1) == true &&
      body->GetAngularVelocity() < 0.05;
}

void Ent::startUnstable()
{
  script.clear();
  animState = GROUNDED;
  currSpineTimeMult = 0.5;
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "struggle", 1);
}

Logical Ent::fullyGrounded()
{
  return (RM::standardize_rads(getRotation()) >= HALF_PI) &&
      (RM::standardize_rads(getRotation()) <= HALF_PI * 3.0);
}


void Ent::updateCollisions()
{

}

void Ent::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (pgo != player)
  {
    return;
  }
  
  // the level play check is for objects responding to forces right when the
  // level starts
  if (crushesPhysical == true &&
      normalImpulse >= crushValue &&
      level->levelState == PhysicsLevelStd::LEVEL_PLAY)
  {
    level->objectCrushedStd(this);
  }
}

Logical Ent::shouldTurn()
{
  if (collisionCircle().left() < level->worldBox.left() + WALL_PAD && getHFlip() == true) return true;
  if (collisionCircle().right() > level->worldBox.right() - WALL_PAD && getHFlip() == false) return true;
  return false;
}

void Ent::midCallback(Coord1 value)
{
  switch (value)
  {
    default:
    case 0:
    {
      currSpineTimeMult = 1.0;
      spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "attack", 0);
      animState = ATTACKING;
      break;
    }
      
    case 1:
      currSpineTimeMult = 0.2;
      spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "walk", 1);
      animState = WALKING;
      break;
    
    case 10:
    case 11:
    case 12:
    {
      EntRoot* root = new EntRoot();
      root->setXY(getXY());
      RM::flatten_me(root->xy.x, player->getX(), 256.0 + 128.0 * (value - 10));
      level->addPGO(root);
      break;
    }
  }
}

void Ent::callback(ActionEvent* caller)
{
  facePlayer();
  
  script.wait(3.0);
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.5));
  script.enqueueMidCallback(this, 0);  // starting slowing to a stop
  script.wait(0.75);
  script.enqueueMidCallback(this, 10);  // start root
  script.wait(0.25);
  script.enqueueMidCallback(this, 11);  // start root
  script.wait(0.25);
  script.enqueueMidCallback(this, 12);  // start root
  script.wait(0.5);
  script.enqueueMidCallback(this, 1);
  script.enqueueX(new SineFn(&currSpeed, WALK_SPEED, 0.5));
}

// =============================== GrimReaper ============================ //

GrimReaper::GrimReaper() :
  PizzaGOStd(),

  walkSpeed(RM::randf(40.0, 60.0)),
  attackCircle()
{
  basePoints = 10;
}

void GrimReaper::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(reaperSpine, "walk", true);
  mySpineAnim.animSpeedMult = 0.2;
  
  setWH(visibleAABB().wh);
  callback(&script);
}

void GrimReaper::updateMe()
{
  script.update();
  walkAlongPlatform(walkSpeed, true, true, true);
  
  updateSpineAnim();
  
  updateCollisions();
}

void GrimReaper::redrawMe()
{
  // addSortDraw();
  PizzaGOStd::redrawMe();
//  visibleAABB().draw_solid(BLUE_SOLID);
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle.draw_outline(RED_SOLID);
  }
}

void GrimReaper::updateCollisions()
{
  attackCircle = mySpineAnim.calcAttachmentCircle("attack_collision", "attack_collision");
  attackCircle.radius *= 0.5;
  
  Logical attackHitPlayer = Circle::collision(attackCircle, player->collisionCircle());
  
  
  if (hitThisAttack == true && attackHitPlayer == false)
  {
    hitThisAttack = false;
  }
  else if (hitThisAttack == false && attackHitPlayer == true)
  {
    hitThisAttack = true;
    player->attacked(attackCircle.xy, this);
  }
  else if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    /*
     createMagnetCoin();
     destroyPhysicalStd(Coord2(3, 4), getImage());
     smokePuffStd();
     */
    level->enemyDefeated(this);
  }
}

Circle GrimReaper::collisionCircle()
{
  Circle adjustedCircle = PizzaGOStd::collisionCircle();
  adjustedCircle.xy.x += facingToDir();
  adjustedCircle.radius *= 0.4;
  return adjustedCircle;
}



void GrimReaper::midCallback(Coord1 value)
{
  if (std::abs(getX() - player->getX()) < player->getWidth() * 3.0)
  {
    facePlayer();
  }
}

void GrimReaper::callback(ActionEvent* caller)
{
  if (std::abs(getX() - player->getX()) < player->getWidth() * 3.0)
  {
    // this is so it doesn't keep hitting the end of its platform in other levels
    facePlayer();
  }
  
  walkSpeed = RM::randf(60.0, 90.0);
  mySpineAnim.animSpeedMult = 0.3;
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "walk", 1);

  script.wait(1.0);
  script.enqueueMidCallback(this, 0);
  script.enqueueX(new SpineAnimationAction(&mySpineAnim, "attack", 0));
}

// =============================== SkeleCupid ============================ //

SkeleCupid::SkeleCupid() :
  PizzaGOStd(),

  fireScript(this),
  turnTimer(1.0, this),
  velX(0.0),

  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  basePoints = 40;
  flies = true;
}

void SkeleCupid::load()
{
  facePlayer();
  useFishRotation();
  
  platformOffset = Point2(0.0, -player->getHeight());
  setY(myPlatformLine.get_y_at_x(getX()) + platformOffset.y);

  initSpineAnim(cupidSpine, "idle", true);
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 1, "wingflap", 1);

  setWH(visibleAABB().wh);
  currSpineTimeMult = 0.1;
  
  callback(&fireScript);
}

void SkeleCupid::updateMe()
{
  turnTimer.update();
  Point1 oldX = getX();
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  
  if (onRMScreen() == true)
  {
    fireScript.update();
    actions.update();
    
    Point1 playerMax = level->playerTXVel();
    Point1 finalVelX = player->get_velocity().x * 1.2;
    
    if (RM::sign(player->get_velocity().x) == facingToDir())
    {
      finalVelX = 0.0;
    }
    
    // this accounts for how the walking function assumes forward is positive and backward is negative
    finalVelX += 0.35 * playerMax * facingToDir();
    Point1 dirMult = RM::sign(finalVelX) == facingToDir() ? 1.0 : -1.0;
    finalVelX = std::abs(finalVelX) * dirMult;

    walkAlongPlatform(finalVelX, true, true, false);
  }
  else if (onRMScreen() == false && playerOverMyPlatform() == true)
  {
    walkAlongPlatform(250.0, true, true, false);
  }
  
  // clamp inside walls
  Point1 width = collisionCircle().radius;
  xy.x = RM::clamp(xy.x, myPlatformLine.left() + width, myPlatformLine.right() - width);
  
  velX = getX() - oldX;
  Point1 wingMult = RM::clamp(std::abs((velX / RM::timePassed()) / 250.0), 1.0, 3.0);
  mySpineAnim.getTrack(1)->timeScale = wingMult;
  
  updateSpineAnim();
  
  updateCollisions();
}

void SkeleCupid::redrawMe()
{
  addY(bobOffset);
  PizzaGOStd::redrawMe();
  addY(-bobOffset);

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void SkeleCupid::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

Circle SkeleCupid::collisionCircle()
{
  return Circle(getXY(), 24.0);
}

void SkeleCupid::midCallback(Coord1 value)
{
  Point2 firePoint = getXY() + Point2(87.0 * facingToDir(), -25.0);
  Point2 arrowVel = RM::pol_to_cart(800.0 + velX, getRotation());
  
  CupidArrow* arrow = new CupidArrow(firePoint, arrowVel);
  level->addPGO(arrow);  // ally so it doesn't affect facing or spawning
}

void SkeleCupid::callback(ActionEvent* caller)
{
  if (caller == &fireScript)
  {
    spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "idle", 1);

    fireScript.wait(2.0);
    fireScript.enqueueAddX(new SpineAnimationAction(&mySpineAnim, "fire", 0, 1.0), &actions);
    fireScript.wait(0.5);
    fireScript.enqueueMidCallback(this, 0);
    fireScript.wait(0.5);  // fire animation over after this
  }
  else if (caller == &turnTimer)
  {
    facePlayer();
    turnTimer.reset();
  }
}

// =============================== CupidArrow ============================ //

CupidArrow::CupidArrow(Point2 origin, Point2 setVel) :
  PizzaGOStd(),

  stickBehavior(this),
  vel(setVel)
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;

  stickBehavior.setListener(this);
  setImage(getImg(cupidArrowImg));
  setXY(origin);
  setRotation(RM::angle(setVel));
  useFishRotation();
  hFlipForRotation();
}

void CupidArrow::updateMe()
{
  if (lifeState == RM::ACTIVE)
  {
    addXY(vel * RM::timePassed());

    if (onRMScreen()== false)
    {
      lifeState = RM::REMOVE;
    }
    
    updateCollisions();
  }
  else if (lifeState == RM::PASSIVE)
  {
    stickBehavior.update();
    setAlpha(1.0 - RM::clamp((stickBehavior.percentDone() - 0.5) / 0.5, 0.0, 1.0));
  }
  
}

void CupidArrow::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void CupidArrow::updateCollisions()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (touchingPlayerCircle() == true)
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

// =============================== Giant ============================ //

const Point1 Giant::WALK_SPEED = 64.0;

Giant::Giant() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED),

  headThrown(false)
{
  basePoints = 10;
}

void Giant::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(giantSpine, "walk");
  
  setWH(collisionCircle().radius, collisionCircle().radius);  // radius

  Point1 throwDuration = mySpineAnim.getAnimationDuration("throw");
  
  script.wait(2.0);
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.25));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "throw", 0, 1.0));
  script.wait(0.485);
  script.enqueueMidCallback(this, 0);  // head throw
  script.wait(0.515);  // go back to walking
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "walk_headless", 0, true));
  script.enqueueX(new SineFn(&currSpeed, WALK_SPEED, 0.25));
}

void Giant::updateMe()
{
//  RM::push_time_mult(0.1);
  if (onRMScreen() == true)
  {
    script.update();

    walkAlongPlatform(currSpeed, true, true, true);
  }
  
  updateSpineAnim();
//  RM::pop_time_mult();

  updateCollisions();
}

void Giant::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    
    if (headThrown == false)
    {
      Circle(skullPositionWalk(), 24.0).draw_outline(RED_SOLID);
    }
  }
}

void Giant::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  if (headThrown == false &&
      Circle(skullPositionWalk(), 24.0).collision(player->collisionCircle()) == true)
  {
    headThrown = true;
    script.clear();
    currSpeed = WALK_SPEED;

    player->attacked(skullPositionWalk(), this);

    spTrackEntry* trackEntry = spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0,
        "walk_headless", 1);
  }
  else if (touchingPlayerCircle() == true)
  {
    if (headThrown == false)
    {
      // head falls
      GiantSkull* skull = new GiantSkull();
      skull->setXY(skullPositionWalk());
      skull->myPlatformLine = myPlatformLine;
      skull->hflip(getHFlip());
      level->addPGO(skull);
      
      Point2 ballVel = RM::pol_to_cart(500.0, getRotation());
      ballVel.y -= 128.0; // technically this should be a change to its angle but this is okay
      skull->set_velocity(ballVel);
    }
    
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

Point2 Giant::skullPositionWalk()
{
  return Point2(getX() + 0.0 * facingToDir(), getY() + -218.0);
}

Point2 Giant::skullPositionThrow()
{
  return Point2(getX() + 100.0 * facingToDir(), getY() + -200.0);
}

void Giant::midCallback(Coord1 value)
{
  switch (value)
  {
    case 0:
    {
      // head is already thrown if player hit it
      if (headThrown == false)
      {
        headThrown = true;
        
        GiantSkull* skull = new GiantSkull();
        skull->setXY(skullPositionThrow());
        skull->myPlatformLine = myPlatformLine;
        skull->hflip(getHFlip());
        level->addPGO(skull);
        
        Point2 ballVel = RM::pol_to_cart(500.0, getRotation());
        ballVel.y -= 128.0; // technically this should be a change to its angle but this is okay
        skull->set_velocity(ballVel);
      }
      
      break;
    }
  }
}

// =============================== GiantSkull ============================ //

GiantSkull::GiantSkull() :
  PizzaGOStd(),

  breakTimer(2.5, this)
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;
  setWH(22.0, 22.0);
  
  setImage(getImg(giantHeadImg));
}

void GiantSkull::load()
{
  sortY = myPlatformLine.get_y_at_x(getX());
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger
  
  change_density(3.0);
  set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);
  set_ang_vel(PI * facingToDir());
  set_restitution(0.7);
  fixture->SetUserData(this);
}

void GiantSkull::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  if (lifeState == RM::ACTIVE && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysical();
    
    // level->addAction(new BombExplosion(getXY()));
  }
}

void GiantSkull::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void GiantSkull::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  destroyPhysical();
}

// =============================== Wisp ============================ //

Wisp::Wisp() :
  PizzaGOStd(),

  mover(this),
  stateTimer(2.0, this),

  currState(INVISIBLE),
  blinkCount(0),
  seenPlayer(false)
{
  basePoints = 70;
  setWH(12.0, 12.0);   // half size
  flies = true;
}

void Wisp::load()
{
  Point1 moveTime = RM::randf(1.5, 2.0);
  
  addY(-RM_WH.y * 0.5);
  
  /*
  Box bounds(getX() - RM_WH.x * 0.4, deviceSafeBox().top() + getHeight(),
             RM_WH.x * 0.5, level->getGroundY() - (deviceSafeBox().top() + getHeight()) - player->getHeight() * 0.4);

   RM::clamp_me(bounds.xy.x, level->worldBox.left() + 48.0, level->worldBox.right() - bounds.width());
   */
  Point1 leftBound = getX() - RM_WH.x * 0.25;
  leftBound = std::max(leftBound, myPlatformLine.left() + getWidth() * 2.0);
  
  Point1 rightBound = getX() + RM_WH.x * 0.25;
  rightBound = std::min(rightBound, myPlatformLine.right() - getWidth() * 2.0);
  
  Point1 topLeftBound = myPlatformLine.get_y_at_x(leftBound) - player->getHeight() * 1.5;
  Point1 bottomLeftBound = myPlatformLine.get_y_at_x(leftBound) - player->getHeight() * 0.4;
  Point1 topRightBound = myPlatformLine.get_y_at_x(rightBound) - player->getHeight() * 1.5;
  Point1 bottomRightBound = myPlatformLine.get_y_at_x(rightBound) - player->getHeight() * 0.4;

  Quad bounds(Point2(leftBound, topLeftBound), Point2(rightBound, topRightBound),
              Point2(rightBound, bottomRightBound), Point2(leftBound, bottomLeftBound));
  
  mover.init(bounds, Point2(moveTime, moveTime));
  
  initSpineAnim(wispSpine, "idle_invisible", true);
  
  callback(&script);
}

void Wisp::updateMe()
{
  // don't do anything until you see the player for the first time
  if (seenPlayer == false)
  {
    if (onRMScreen() == true)
    {
      seenPlayer = true;
    }
    else
    {
      return;
    }
  }
  
  // stateTimer.update();
  mover.update();
  facePlayer();
  
  script.update();
  updateSpineAnim();
  
  updateCollisions();
}

void Wisp::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    ColorP4 debugColor = currState == VISIBLE ? WHITE_SOLID : BLUE_SOLID;
    collisionCircle().draw_outline(debugColor);
    
    mover.bounds.draw_outline(BLUE_SOLID);
  }
}

void Wisp::updateCollisions()
{
  if (currState == VISIBLE &&
      touchingPlayerCircle() == true)
  {
    // level->addAction(new WispExplode(getXY()));
    level->enemyDefeated(this);
    setRemove();
  }
}

void Wisp::midCallback(Coord1 value)
{
  Point1 targetX = RM::clamp(player->getX(), getX() - 300.0, getX() + 300.0);
  Point1 targetY = std::max(myPlatformLine.get_y_at_x(targetX) - 150.0, getY() + 150.0);
  
  // ResourceManager::playWispShoot();
  
  WispFlame* flame = new WispFlame(getXY(), Point2(targetX, targetY));
  flame->myPlatformLine = myPlatformLine;
  level->addPGO(flame);
}

void Wisp::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    // invis, eyes open
    currState = INVISIBLE;
    setSpineAnim(0, "idle_invisible", true);
    script.wait(2.0);
    
    // invis, eyes closed
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.0, 0.48), 2.0));
    script.wait(2.0);
    
    // invis, open eyes to fire
    script.enqueueMidCallback(this, 0);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.48, -1.0), 0.25));
    script.wait(0.25);

    // invis, eyes open
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_invisible", 0, true));
    script.wait(2.0);
    
    // invis, eyes closed
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.0, 0.48), 2.0));
    script.wait(2.0);
    
    // invis, open eyes to fire 0.25 second
    script.enqueueMidCallback(this, 0);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.48, -1.0), 0.25));
    script.wait(0.25);
    
    // invis, eyes open
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_invisible", 0, true));
    script.wait(2.0);
    
    // visible, eyes open 2 seconds
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_visible", 0, true));
    script.enqueueX(new SetValueCommand<Coord1>(&currState, VISIBLE));
    script.wait(2.0);
    
  }
}

// =============================== WispFlame ============================ //

WispFlame::WispFlame(Point2 center, Point2 target) :
  PizzaGOStd(),

  lifeTimer(5.0, this),
  vel(RM::ring_edge(center, target, 175.0))
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;
  setBox(center.x, center.y, 6.0, 6.0);
  setRotation(RM::angle(vel));
  
  initSpineAnim(wispFireSpine, "animation", true);
}

void WispFlame::updateMe()
{
  updateSpineAnim();

  lifeTimer.update();
  if (lifeState == RM::PASSIVE) setAlpha(1.0 - lifeTimer.progress());
  
  addXY(vel * RM::timePassed());
  
  if (lifeState == RM::ACTIVE)
  {
    if (getY() >= myPlatformLine.get_y_at_x(getX()))
    {
      dissipate();
    }
    else if (touchingPlayerCircle() == true)
    {
      player->attacked(getXY(), this);
      dissipate();
    }
  }
}

void WispFlame::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void WispFlame::dissipate()
{
  if (lifeState != RM::ACTIVE) return;
  
  setPassive();
  vel.set(0.0, 0.0);
  lifeTimer.reset(0.2);
}

Circle WispFlame::collisionCircle()
{
  return Circle(getXY(), getWidth());
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

// =============================== Utensil ============================ //

const Point1 Utensil::WALK_SPEED = 64.0;

Utensil::Utensil() :
  PizzaGOStd(),

  turnTimer(2.0, this)
{
  basePoints = 10;
}

void Utensil::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(utensilSpine, "walk");
  mySpineAnim.spineAnimState->data->defaultMix = 0.0;

  setWH(collisionCircle().radius, collisionCircle().radius);  // radius
  
  mySpineAnim.startAnimation("back_arm", 1, Point2(0.0, 1.0 / 60.0), 1.0, false);
  mySpineAnim.startAnimation("front_arm", 2, Point2(0.0, 0.01 / 60.0), 100.0, false);
  
  script.wait(1.0);
}

void Utensil::updateMe()
{
  turnTimer.update();
  script.update();
  
  walkAlongPlatform(WALK_SPEED, true, true, true);
  
  updateSpineAnim();
  
  updateCollisions();
}

void Utensil::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    backArmCircle().draw_outline(RED_SOLID);
    frontArmCircle().draw_outline(RED_SOLID);
  }
}

Circle Utensil::backArmCircle()
{
  Circle backArm = mySpineAnim.calcAttachmentCircle("back_attack_collision", "back_attack_collision");
  backArm.radius *= 0.25;
  return backArm;
}

Circle Utensil::frontArmCircle()
{
  Circle frontArm = mySpineAnim.calcAttachmentCircle("front_attack_collision", "front_attack_collision");
  frontArm.radius *= 0.25;
  return frontArm;
}

void Utensil::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  Logical backArmHitPlayer = Circle::collision(backArmCircle(), player->collisionCircle());
  Logical frontArmHitPlayer = Circle::collision(frontArmCircle(), player->collisionCircle());
  Logical attackHitPlayer = backArmHitPlayer | frontArmHitPlayer;

  if (hitThisAttack == true && attackHitPlayer == false)
  {
    hitThisAttack = false;
  }
  else if (hitThisAttack == false && attackHitPlayer == true)
  {
    hitThisAttack = true;
    player->attacked(backArmHitPlayer ? backArmCircle().xy : frontArmCircle().xy, this);
  }
  else if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

void Utensil::callback(ActionEvent* caller)
{
  Point1 spineFPS = 60.0;
  
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "back_arm", 1, Point2(-1.0, 2.0 / spineFPS), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "front_arm", 2, Point2(0.0, 1.0 / spineFPS), 1.0));
  script.wait(3.0);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "back_arm", 1, Point2(-1.0, 3.0 / spineFPS), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "front_arm", 2, Point2(-1.0, 2.0 / spineFPS), 1.0));
  script.wait(3.0);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "back_arm", 1, Point2(-1.0, 4.0 / spineFPS), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "front_arm", 2, Point2(-1.0, 3.0 / spineFPS), 1.0));
  script.wait(3.0);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "back_arm", 1, Point2(0.0, 1.0 / spineFPS), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "front_arm", 2, Point2(-1.0, 4.0 / spineFPS), 1.0));
  script.wait(3.0);
}

// =============================== Chip ============================ //

Chip::Chip() :
  PizzaGOStd(),

  stickBehavior(this),
  attackTimer(1.0, true, this),

  attachmentState(FLYING),
  flyDir(1),

  maxDangerousAngle(0.25)  // times PI
{
  basePoints = 10;
}

void Chip::load()
{
  flyDir = -RM::sign(getX() - player->getX());
  
  sortY = getY();
  platformOffset.y = -player->getWidth();
  addXY(platformOffset);
  // useFishRotation();
  
  initSpineAnim(chipSpine, "idle");
  
  setWH(16.0, 16.0);  // radius
}

void Chip::updateMe()
{
  script.update();
  
  if (attachmentState == FLYING)
  {
    walkAlongPlatform(192.0, false, false, true);
    // Point1 walkDistance = 192.0 * flyDir * RM::timePassed();
    // addX(walkDistance);
    // turnAtWalls();
  }
  else
  {
    stickBehavior.update();
    attackTimer.update();
  }
  
  updateSpineAnim();
  
  updateCollisions();
}

void Chip::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

Logical Chip::shouldTurn()
{
  if (collisionCircle().left() < myPlatformLine.left() && flyDir == -1) return true;
  if (collisionCircle().right() > myPlatformLine.right() && flyDir == 1) return true;
  return false;
}

Logical Chip::turnAtWalls()
{
  Logical turned = shouldTurn();
  
  if (turned == true)
  {
    flyDir *= -1;
  }
  
  return turned;
}

void Chip::walkAlongPlatform(Point1 speed, Logical withRotation, Logical withAutoHFlip, Logical withTurning)
{
  if (withTurning == true)
  {
    turnAtWalls();
  }
  else
  {
    clampAtWalls();
  }
  
  Point1 walkAngle = myPlatformLine.angle() + ((flyDir == 1) ? 0.0 : PI);
  Point2 walkVector = RM::pol_to_cart(speed * RM::timePassed(), walkAngle);

  addXY(walkVector);
  setY(myPlatformLine.get_y_at_x(getX()) + platformOffset.y);  // this keeps y distance constant
}

Circle Chip::collisionCircle()
{
  // return mySpineAnim.calcAttachmentCircle("top_collision", "top_collision");
  return Circle(getXY(), getWidth());
}

void Chip::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  if (attachmentState == FLYING)
  {
    Point1 angleToPlayer = RM::angle(getXY(), player->getXY());
    
    Point1 turnSpeed = TWO_PI * 2.0 * RM::timePassed();
    
    // getting close to player, turn toward it
    if (std::abs(getX() - player->getX()) < player->getWidth() * 2.0)
    {
      Point1 clampAngle = angleToPlayer;
      
      if (clampAngle >= PI && clampAngle <= PI * (1.0 + maxDangerousAngle))
      {
        clampAngle = PI;
      }
      else if (clampAngle >= PI * (1.0 + maxDangerousAngle) && clampAngle <= PI * (2.0 - maxDangerousAngle))
      {
        clampAngle = HALF_PI;
      }
      else if (clampAngle >= PI * (2.0 - maxDangerousAngle))
      {
        clampAngle = 0.0;
      }

      RM::chase_me_angle(rotation, clampAngle, turnSpeed);
      RM::clamp_me(rotation, 0.0, PI);
    }
    // not close to player, turn downward
    else
    {
      RM::chase_me_angle(rotation, HALF_PI, turnSpeed);
    }
    
    if (touchingPlayerCircle() == true)
    {
      // player crushed from above
      if (angleToPlayer >= PI * (1.0 + maxDangerousAngle) && angleToPlayer <= PI * (2.0 - maxDangerousAngle))
      {
        destroyPhysical();
        level->enemyDefeated(this);
      }
      else
      {
        attachmentState = ATTACHED;
        setRotation(angleToPlayer);
        stickBehavior.reset(player);
        setSpineAnim(0, "attack", true);
      }
    }
  }
}

void Chip::pizzaSlammed()
{
  if (getActive() == true && attachmentState == ATTACHED)
  {
    Point1 angleToPlayer = RM::angle(getXY(), player->getXY());
    Point1 tolerance = PI * 0.1;
    
    if (std::abs(PI * 1.5 - angleToPlayer) <= tolerance)
    {
      // this should later just use a triangle shatter, but we'd need a static image for it
      destroyPhysical();
      level->enemyDefeated(this);
    }
  }
}

void Chip::callback(ActionEvent* caller)
{
  if (caller == &attackTimer)
  {
    player->attacked(getXY(), this);
  }
}

// =============================== JumpingSpider ============================ //

const Point1 JumpingSpider::WALK_SPEED = 64.0;

JumpingSpider::JumpingSpider() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED),

  startWalkName("walk_teethup"),
  startJumpName("jump_teethup"),
  startFlipRange(0.0, 32.0 / 60.0),

  oppWalkName("walk_teethdown"),
  oppJumpName("jump_teethdown"),
  oppFlipRange(32.0 / 60.0, -1.0)
{
  basePoints = 10;
}

void JumpingSpider::load()
{
  facePlayer();
  sortY = getY();
  
  if (RM::randl() == true)
  {
    startWalkName = "walk_teethdown";
    startJumpName = "jump_teethdown";
    startFlipRange.set(32.0 / 60.0, -1.0);
    
    oppWalkName = "walk_teethup";
    oppJumpName = "jump_teethup";
    oppFlipRange.set(0.0, 32.0 / 60.0);
  }
  
  initSpineAnim(jumpingSpiderSpine, startWalkName.as_cstr());
  mySpineAnim.setDefaultMixDuration(0.1);

  spTrackEntry* track = mySpineAnim.startAnimation(startWalkName.as_cstr(), 0, true);
  track->timeScale = 0.75;

  setWH(64.0, 32.0);  // radius

  script.wait(3.0);
}

void JumpingSpider::updateMe()
{
  script.update();
  
  Point1 walkDistance = currSpeed * facingToDir() * RM::timePassed();
  addX(walkDistance);
  turnAtWalls();
  setRotation(0.0);  // this cancels out how turnAtWalls does a rotation now
  
  updateSpineAnim();
  
  updateCollisions();
}

void JumpingSpider::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle().draw_outline(RED_SOLID);
  }
}

void JumpingSpider::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  Logical attackHitPlayer = Circle::collision(attackCircle(), player->collisionCircle());

  if (attackHitPlayer == true)
  {
    player->attacked(getXY(), this);  // note: the collisionBox is not yet accurate
  }
  else if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

Circle JumpingSpider::collisionCircle()
{
  return mySpineAnim.calcAttachmentCircle("head_collision", "head_collision");
}

Circle JumpingSpider::attackCircle()
{
  return mySpineAnim.calcAttachmentCircle("teeth_collision", "teeth_collision");
}

void JumpingSpider::callback(ActionEvent* caller)
{
  Point1 D_FPS = 60.0;
  
  // walk, jump, walk
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startJumpName.as_cstr(), 0, Point2(0.0, 16.0 / D_FPS), 0.1));
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.1));
  script.wait(0.1);
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, WALK_SPEED));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startJumpName.as_cstr(), 0, Point2(-1.0, 32.0 / D_FPS), 0.1));
  script.enqueueX(new DeAccelFn(&xy.y, getY() - player->getWidth() * 1.5, 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startJumpName.as_cstr(), 0, Point2(-1.0, 48.0 / D_FPS), 0.1));
  script.enqueueX(new AccelFn(&xy.y, getY(), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startJumpName.as_cstr(), 0, Point2(-1.0, 1.0), 0.1));
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, 0.0));
  script.wait(0.1);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startWalkName.as_cstr(), 0, true));
  script.enqueueX(new SineFn(&currSpeed, WALK_SPEED, 0.1));
  script.wait(2.0);
  
  // flip
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "flip", 0, startFlipRange, 0.5));
  script.wait(0.5);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppWalkName.as_cstr(), 0, true));
  script.wait(2.0);
  
  // walk, jump, walk
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppJumpName.as_cstr(), 0, Point2(0.0, 16.0 / D_FPS), 0.1));
  script.enqueueX(new SineFn(&currSpeed, 0.0, 0.1));
  script.wait(0.1);
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, WALK_SPEED));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppJumpName.as_cstr(), 0, Point2(-1.0, 32.0 / D_FPS), 0.1));
  script.enqueueX(new DeAccelFn(&xy.y, getY() - player->getWidth() * 1.5, 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppJumpName.as_cstr(), 0, Point2(-1.0, 48.0 / D_FPS), 0.1));
  script.enqueueX(new AccelFn(&xy.y, getY(), 1.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppJumpName.as_cstr(), 0, Point2(-1.0, 1.0), 0.1));
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, 0.0));
  script.wait(0.1);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, oppWalkName.as_cstr(), 0, true));
  script.enqueueX(new SineFn(&currSpeed, WALK_SPEED, 0.1));
  script.wait(2.0);
  
  // flip
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "flip", 0, oppFlipRange, 0.5));
  script.wait(0.5);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, startWalkName.as_cstr(), 0, true));
  script.wait(2.0);
}

// =============================== Scientist ============================ //

const Point1 Scientist::WALK_SPEED = 64.0;

Scientist::Scientist() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED)
{
  basePoints = 10;
}

void Scientist::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(scientistSpine, "walk");
  
  setWH(collisionCircle().radius, collisionCircle().radius);  // radius
 
  script.wait(0.3);
}

void Scientist::updateMe()
{
  if (onRMScreen() == true)
  {
    script.update();
    updateSpineAnim();
  }
  
  walkAlongPlatform(currSpeed, true, true, true);
  
  updateCollisions();
}

void Scientist::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void Scientist::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  if (touchingPlayerCircle() == true)
  {
    lifeState = RM::TRANS_OUT;
    
    currSpeed = std::abs(player->get_velocity().x * 0.25);
    
    if (RM::sign(facingToDir()) != RM::sign(player->get_velocity().x))
    {
      currSpeed *= -4.0;
    }
    
    script.clear();
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, "die", 0));
    script.enqueueX(new LinearFn(&color.w, 0.0, 0.5));
    script.enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::REMOVE));
    
    level->enemyDefeated(this);
  }
}

void Scientist::midCallback(Coord1 value)
{
  TestTube* vial = new TestTube();
  vial->setXY(Point2(getX() + 62.0 * facingToDir(), getY() + -167.0));
  vial->hflip(getHFlip());
  level->addPGO(vial);

  vial->set_velocity(Point2(512.0 * facingToDir(), -50.0));
  vial->set_ang_vel(PI * vial->facingToDir());
}

void Scientist::callback(ActionEvent* caller)
{
  // not sure why the tube needs to be thrown at 0.75 second instead of 1 second
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "throw", 0, Point2(0.0, 32.0 / 60.0), 0.7));
  script.wait(0.7);
  script.enqueueMidCallback(this, 0);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "throw", 0, Point2(-1.0, -1.0), 0.5));
  // script.wait(0.5);
  
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, 0.0));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "throw", 0, Point2(-1.0, -1.0), 0.25));
  script.wait(0.0);
  script.enqueueX(new SetValueCommand<Point1>(&currSpeed, WALK_SPEED));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "walk", 0, true));
  script.wait(2.0);
}

// =============================== TestTube ============================ //

TestTube::TestTube() :
  PizzaGOStd(),

  breakTimer(3.0, this),
  breakAngle(0.0),
  createAcid(true)
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;
  setWH(22.0, 22.0);
  
  setImage(getImg(scientistVialImg));
}

void TestTube::load()
{
  sortY = level->getGroundY();
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger

  change_density(10.0); // this is so it generates enough of an impact for a collision callback
  set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);
  set_ang_vel(PI * facingToDir());
  fixture->SetUserData(this);
}

void TestTube::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  checkPhysicalDestruction();
  
  if (getActive() == true && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysical();
  }
}

void TestTube::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (getActive() == false || destroyedPhysical == true)
  {
    return;
  }
  
  destroyedPhysical = true;
  
  updateFromPhysical();
  breakAngle = RM::standardize_rads(RM::angle(getXY(), contact_to_wpt(contact)) - HALF_PI);
}

void TestTube::destroyPhysical()
{
  if (createAcid == true)
  {
    AcidPatch* acid = new AcidPatch(getXY() + Point2(0.0, 16.0));
    acid->setRotation(breakAngle);
    level->addPGO(acid);
  }
  
  PizzaGOStd::destroyPhysical();
}


void TestTube::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void TestTube::callback(ActionEvent* caller)
{
  if (getActive() == false) return;
  
  createAcid = false;
  destroyPhysical();
}


// =============================== AcidPatch ============================ //

AcidPatch::AcidPatch(Point2 center) :
  PizzaGOStd()
{
  setWH(168.0, 38.0);
  setXY(center);
  
  setScale(0.0, 0.0);
  
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;
}

void AcidPatch::load()
{
  initSpineAnim(scientistAcidSpine, "idle");
  
  init_box(getXY(), getSize(), false);
  fixture->SetSensor(true);
  
  set_b2d_callback_data(this);
  set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_SUMO_BOSS));
  updateFromWorld();
  
  script.enqueueX(new DeAccelFn(&scale.x, 1.0, 0.2));
  script.wait(5.0);
  script.enqueueX(new LinearFn(&color.w, 0.0, 0.2));
}

void AcidPatch::updateMe()
{
  updateSpineAnim();
  
  script.update();
  matchXScale();
  
  Point2 touchLocation;
  
  if (touchingPGO(player, &touchLocation) == true)
  {
    player->touchedFire(player->collisionBox().norm_pos(HANDLE_BC), this);
  }
  
  if (touchingPGO(level->boss, &touchLocation) == true)
  {
    level->boss->touchedFire(level->boss->collisionBox().norm_pos(HANDLE_BC), this);
  }
}

void AcidPatch::redrawMe()
{
  PizzaGOStd::redrawMe();
}

void AcidPatch::callback(ActionEvent* caller)
{
  setRemove();
}

// =============================== Dozer ============================ //

Dozer::Dozer() :
  PizzaGOStd(),

  flippedFixture(NULL),
  crushTimer(0.25)
{
  basePoints = 10;
  
  setMaxHP(2.9);
}

void Dozer::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(dozerSpine, "move_base", true);
  initSpineAnim(shell1Animator, dozerSpine, "move_shell1", true);
  initSpineAnim(shell2Animator, dozerSpine, "move_shell2", true);
  initSpineAnim(wheelAnimator, dozerSpine, "move_wheels", true);

  // mySpineAnim.startAnimation("move_combined", 1, Point2(0.0, 0.01), 0.1, false);
  
  // points when facing forward
  DataList<Point2> currPtList;
  currPtList.add(Point2(-856.0, -704.0));
  currPtList.add(Point2(-864.0, 16.0));
  currPtList.add(Point2(1448.0, 16.0));
  currPtList.add(Point2(1040.0, -464.0));
  currPtList.add(Point2(656.0, -464.0));
  currPtList.add(Point2(152.0, -968.0));
  currPtList.add(Point2(-224.0, -984.0));
  currPtList.add(Point2(-232.0, -752.0));
  
  for (Coord1 i = 0; i < currPtList.count; ++i)
  {
    // this is because the coords were done at 4x
    currPtList[i] *= 0.25;
  }
  
  init_shape(getXY(), currPtList, false);
  body->SetType(b2_kinematicBody);
  set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
  set_b2d_callback_data(this);

  // points when facing backward (hflip == true)
  for (Coord1 i = 0; i < currPtList.count; ++i)
  {
    // this is because the coords were done at 4x
    currPtList[i].x *= -1.0;
  }
  
  flippedFixture = create_shape_fixture(body, currPtList);
  set_collisions(flippedFixture, 0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
  set_b2d_callback_data(flippedFixture, this);
  
  crushesPhysical = true;
  crushValue = 2500.0;

  setWH(visibleAABB().wh);
  
  syncHFlipCollision();
  
  crushTimer.setInactive();
}

void Dozer::updateMe()
{
  Logical oldHFlip = getHFlip();
  walkAlongPlatform(256.0, true, true, true);
  
  if (getHFlip() != oldHFlip)
  {
    syncHFlipCollision();
  }
  
  Point1 realRotation = getRotation();
  setRotation(getHFlip() ? getRotation() + PI : getRotation());
  updateFromWorld();
  setRotation(realRotation);

  updateSpineAnim();
  updateSpineAnim(shell1Animator);
  updateSpineAnim(shell2Animator);
  updateSpineAnim(wheelAnimator);

  updateCollisions();
  crushTimer.update();
  checkPhysicalDestruction();
}

void Dozer::redrawMe()
{
  // addSortDraw();
  PizzaGOStd::redrawMe();

  if (currHP >= 2.0)
  {
    renderSpineAnim(shell1Animator);
  }
  
  if (currHP >= 1.0)
  {
    renderSpineAnim(shell2Animator);
  }

  renderSpineAnim(wheelAnimator);

  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    attackCircle().draw_outline(RED_SOLID);
  }
}

void Dozer::syncHFlipCollision()
{
  if (getHFlip() == false)
  {
    set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
    set_collisions(flippedFixture, 0x1 << TYPE_TERRAIN, 0x0);
  }
  else
  {
    set_collisions(0x1 << TYPE_TERRAIN, 0x0);
    set_collisions(flippedFixture, 0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
  }
}

void Dozer::updateCollisions()
{
  
  if (touchingPlayerCircle(attackCircle()) == true)
  {
    player->attacked(collisionCircle().xy, this);
  }
  else if (touchingPlayerCircle() == true)
  {
    // do nothing
  }
}

void Dozer::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (pgo != player)
  {
    return;
  }
  
  // the level play check is for objects responding to forces right when the
  // level starts
  if (getActive() == true &&
      crushesPhysical == true &&
      normalImpulse >= crushValue &&
      crushTimer.getActive() == false &&
      level->levelState == PhysicsLevelStd::LEVEL_PLAY)
  {
    addDamage(1.0);
    
    if (currHP <= 0.0)
    {
      level->objectCrushedStd(this);
    }
    else
    {
      // partially crushed
      crushTimer.reset();

      CreateRenderedParticlesAction* crushParticles = new CreateRenderedParticlesAction(this);
      level->addAction(crushParticles);
    }
  }
}

void Dozer::drawDStruct()
{
  if (currHP <= 1.0)
  {
    renderSpineAnim(shell1Animator);
  }
  else if (currHP <= 2.0)
  {
    renderSpineAnim(shell2Animator);
  }
  else
  {
    renderSpineAnim();
    renderSpineAnim(wheelAnimator);
  }
}


Circle Dozer::attackCircle()
{
  Circle spikes = mySpineAnim.calcAttachmentCircle("spikes_collision", "spikes_collision");
  spikes.radius *= 0.75;
  return spikes;
}

// =============================== Caveman ============================ //

Caveman::Caveman() :
  PizzaGOStd(),

  turnTimer(1.0, this),
  velX(0.0),

  attackOn(false),

  secondsPerRoll(40.0 / 30.0),
  wheelRadius(76.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  basePoints = 40;
  flies = true;
}

void Caveman::load()
{
  facePlayer();
  useFishRotation();
  
  initSpineAnim(cavemanSpine, "idle", true);
  mySpineAnim.startAnimation("roll_forward", 1, true);
  
  setWH(collisionCircle().radius, collisionCircle().radius);  // radius
  
  callback(&script);
}

void Caveman::updateMe()
{
  turnTimer.update();
  Point1 oldX = getX();
  
  if (onRMScreen() == true)
  {
    script.update();
    actions.update();
    
    Point1 playerMax = level->playerTXVel();
    Point1 finalVelX = player->get_velocity().x * 1.2;
    
    if (RM::sign(player->get_velocity().x) == facingToDir())
    {
      finalVelX = 0.0;
    }
    
    // addX(finalVelX * RM::timePassed());
    // this accounts for how the walking function assumes forward is positive and backward is negative
    finalVelX += 0.35 * playerMax * facingToDir();
    Point1 dirMult = RM::sign(finalVelX) == facingToDir() ? 1.0 : -1.0;
    finalVelX = std::abs(finalVelX) * dirMult;
    walkAlongPlatform(finalVelX, true, true, false);

  }
  else
  {
    // addX(250.0 * facingToDir() * RM::timePassed());
    walkAlongPlatform(250.0, true, true, false);
  }
  
  // clamp inside walls
  Point1 width = collisionCircle().radius;
  xy.x = RM::clamp(xy.x, myPlatformLine.left() + width, myPlatformLine.right() - width);

  velX = getX() - oldX;
  
  //  Coord1 startIndex = bodyAnim->currIndex();
  
  syncRolling();
  updateSpineAnim();
  //  Point1 wingMult = RM::clamp(std::abs((velX / RM::timePassed()) / 250.0), 1.0, 3.0);
  //  wingAnim.setSpeedMult(wingMult);
  //  wingAnim.update();
  
  updateCollisions();
}

void Caveman::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    
    if (attackOn == true)
    {
      attackCircle().draw_outline(RED_SOLID);
    }
  }
}

void Caveman::syncRolling()
{
  Point1 wheelCircumference = TWO_PI * wheelRadius;
  Point1 pixelsPerSecond = (1.0 / secondsPerRoll) * wheelCircumference;
  // cout << "px per seocnd " << pixelsPerSecond << " vx " << std::abs(velX) << endl;

  spTrackEntry* rollTrack = spAnimationState_getCurrent(mySpineAnim.spineAnimState, 1);
  Point1 currTrackTime = rollTrack->trackTime;

  Logical isRollingFoward = strcmp(rollTrack->animation->name, "roll_forward") == 0;
  Logical shouldRollForward = (getHFlip() == false && velX >= 0.0) || (getHFlip() == true && velX <= 0.0);
  // cout << "isRollingFoward " << isRollingFoward << " shouldRollForward " << shouldRollForward << endl;

  if (isRollingFoward == true && shouldRollForward == false)
  {
    // start rolling backward
    rollTrack = mySpineAnim.startAnimation("roll_backward", 1, true);
    rollTrack->mixDuration = 0.1;
  }
  else if (isRollingFoward == false && shouldRollForward == true)
  {
    // start rolling forward
    rollTrack = mySpineAnim.startAnimation("roll_forward", 1, true);
    rollTrack->mixDuration = 0.1;
  }
  
  rollTrack->timeScale = std::abs(velX / RM::timePassed()) / pixelsPerSecond;
  rollTrack->trackTime = currTrackTime;

}

void Caveman::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
  else if (touchingPlayerCircle(attackCircle()) == true)
  {
    player->attacked(attackCircle().xy, this);
  }
}

Circle Caveman::collisionCircle()
{
  return Circle(visibleAABB().center() + Point2(0.0, -96.0), 48.0);
}

Circle Caveman::attackCircle()
{
  return mySpineAnim.calcAttachmentCircle("attack_collision", "attack_collision");
}

void Caveman::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    mySpineAnim.startAnimation("idle", 0, true);
    
    script.wait(0.1);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "attack", 0, 1.0));
    script.wait(0.5);
    script.enqueueX(new SetValueCommand<Logical>(&attackOn, true));
    script.wait(0.2);
    script.enqueueX(new SetValueCommand<Logical>(&attackOn, false));
    script.wait(0.3);
  }
  else if (caller == &turnTimer)
  {
    facePlayer();
    turnTimer.reset();
  }
}

// =============================== Alien ============================ //

const Point1 Alien::WALK_SPEED = 64.0;

Alien::Alien() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED),

  teleportTimer(3.0, this),
  nextTeleportSide(1.0)
{
  basePoints = 10;
}

void Alien::load()
{
  facePlayer();
  sortY = getY();
  useFishRotation();
  
  initSpineAnim(alienSpine, "walk");
  mySpineAnim.startAnimation("idle", 1, true);
  
  setWH(24.0, 24.0);  // radius
  
  teleportTimer.setInactive();
  callback(&script);
}

void Alien::updateMe()
{
  if (onRMScreen() == true)
  {
    script.update();
    updateSpineAnim();
  }
  
  if (lifeState == RM::ACTIVE)
  {
    facePlayer();
  }
  
  walkAlongPlatform(currSpeed, true, true, false);

  teleportTimer.update();
  
  updateCollisions();
}

void Alien::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void Alien::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  // teleport
  if (RM::distance_to(getXY(), player->getXY()) < player->getWidth() + 256.0 &&
      teleportTimer.getActive() == false)
  {
    script.clear();
    
    nextTeleportSide = getX() > player->getX() ? -1.0 : 1.0;
    setW(-1.0);
    currSpeed = 0.0;
    mySpineAnim.startAnimation("teleport", 1, false);
    
    // teleport script
    script.wait(32.0 / 60.0);
    script.enqueueMidCallback(this, 1);
    
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "fire", 1, Point2(0.0, -1.0), 65.0 / 60.0));
    script.enqueueX(new SetValueCommand<Point1>(&currSpeed, WALK_SPEED));
    script.wait(15.0 / 60.0);
    script.enqueueMidCallback(this, 0);
    script.wait(17.0 / 60.0);
    script.enqueueX(new SetValueCommand<Point1>(&wh.x, 24.0));
    script.wait(2.0);

    teleportTimer.reset();
  }
  
  if (touchingPlayerCircle() == true)
  {
    // destroyPhysical();
    // level->enemyDefeated(this);
    lifeState = RM::TRANS_OUT;
    
    currSpeed = std::abs(player->get_velocity().x * 0.25);
    const Char* dieStr = "diefoward";
    
    if (RM::sign(facingToDir()) != RM::sign(player->get_velocity().x))
    {
      currSpeed *= -4.0;
      dieStr = "diebackwards";
    }
    
    script.clear();
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, dieStr, 0));
    script.enqueueX(new LinearFn(&color.w, 0.0, 0.5));
    script.enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::REMOVE));
    
    level->enemyDefeated(this);
  }
}

Circle Alien::collisionCircle()
{
  return Circle(visibleAABB().center() + Point2(0.0, -48.0), getWidth());
}

void Alien::midCallback(Coord1 value)
{
  if (value == 0)
  {
    Point2 startPos = Point2(getX() + 76.0 * facingToDir(), getY() - 150.0);
    Point2 laserVel = RM::pol_to_cart(1024.0, getRotation());
    AlienLaser* laser = new AlienLaser(startPos, laserVel);
    
    level->addPGO(laser);
  }
  else if (value == 1)
  {
    setX(player->getX() + (player->getWidth() + 256.0) * nextTeleportSide);
    Box currBox = Box::from_norm(getXY(), visibleAABB().wh, HANDLE_BC);
    
    if (currBox.left() <= myPlatformLine.left() ||
        currBox.right() >= myPlatformLine.right())
    {
      setX(player->getX() + (player->getWidth() + 256.0) * -nextTeleportSide);
    }
    
    setY(myPlatformLine.get_y_at_x(getX()));
  }
}

void Alien::callback(ActionEvent* caller)
{
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "fire", 1, Point2(0.0, -1.0), 65.0 / 60.0));
  script.wait(15.0 / 60.0);
  script.enqueueMidCallback(this, 0);
  script.wait(50.0 / 60.0);
  script.wait(2.0);

}

// =============================== AlienLaser ============================ //

AlienLaser::AlienLaser(Point2 startXY, Point2 setVel) :
  PizzaGOStd(),

  origin(startXY),
  endpoints(startXY, startXY),
  vel(setVel)
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;
  
  setImage(getImgSet(alienLaserSet)[0]);
  setXY(origin);
}

void AlienLaser::updateMe()
{
  if (lifeState == RM::ACTIVE)
  {
    addXY(vel * RM::timePassed());
    endpoints.y = getXY();
    
    if (onRMScreen() == false)
    {
      lifeState = RM::TRANS_OUT;
    }
    
    updateCollisions();
  }
  else if (lifeState == RM::TRANS_OUT)
  {
    Logical effectDone = RM::attract_me(endpoints.x, endpoints.y, RM::magnitude(vel * 2.0) * RM::timePassed());
    
    if (effectDone == true)
    {
      setRemove();
    }
  }
}

void AlienLaser::redrawMe()
{
  // drawMe();
  ColorP4 ringColors[] =
  {
    RED_SOLID,
    ORANGE_SOLID,
    GREEN_SOLID,
    CYAN_SOLID,
    BLUE_SOLID,
    ColorP4(1.0, 0.0, 0.5, 1.0),
    ColorP4(1.0, 0.0, 1.0, 1.0)
  };
  
  Point1 distanceBetweenRings = 32.0;
  Point1 currDistance = 0.0;
  Point1 totalDistance = RM::distance_to(endpoints.x, endpoints.y);
  Coord1 ringIndex = 0;
  
  while (currDistance <= totalDistance)
  {
    Point2 currPos = RM::attract(endpoints.x, endpoints.y, currDistance);
    Point1 xScale = std::min(0.1 + 0.1 * (ringIndex - 1), 1.25);
    Point2 ringScale(xScale, xScale);

    getImgSet(alienLaserSet)[0]->draw_scale(currPos, ringScale, 0.0, ringColors[ringIndex % 7]);
    getImgSet(alienLaserSet)[1]->draw_scale(currPos, ringScale, 0.0, WHITE_SOLID);

    ringIndex++;
    currDistance += distanceBetweenRings;
  }
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
}

void AlienLaser::updateCollisions()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    
    lifeState = RM::TRANS_OUT;
  }
}

Circle AlienLaser::collisionCircle()
{
  return Circle(getXY(), 7.0);
}

void AlienLaser::callback(ActionEvent* caller)
{
  
}

// =============================== BombBat ============================ //

BombBat::BombBat() :
  PizzaGOStd(),

  wingAnimator(),
  wingPt(0.0, 0.0),

  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0),

  currSpeed(0.0),
  flying(true)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  setWH(16.0, 16.0);

  basePoints = 40;
  flies = true;
}

void BombBat::load()
{
  facePlayer();
  useFishRotation();
  
  platformOffset = Point2(0.0, -player->getHeight() * RM::randf(0.5, 1.5));
  setY(myPlatformLine.get_y_at_x(getX()) + platformOffset.y);

  initSpineAnim(batSpine, "idle", true);
  initSpineAnim(wingAnimator, batSpine, "flap", true);

  // this keeps the wings especially from flashing at (0.0, 0.0)
  mySpineAnim.syncFromVisRect(*this, false);
  wingAnimator.syncFromVisRect(*this, false);
  syncWingPt();
  
  callback(&script);
}

void BombBat::updateMe()
{
  if (flying == true)
  {
    updateFlying();
  }
  else
  {
    updateAsBomb();
  }
  
  updateSpineAnim();
  updateSpineAnim(wingAnimator);
  
  //  Point1 wingMult = RM::clamp(std::abs((velX / RM::timePassed()) / 250.0), 1.0, 3.0);
  //  wingAnim.setSpeedMult(wingMult);
  //  wingAnim.update();
  
  updateCollisions();
}

void BombBat::updateFlying()
{
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  
  script.update();
  walkAlongPlatform(currSpeed, true, true, false);
  
  syncWingPt();
}

void BombBat::syncWingPt()
{
  wingPt = mySpineAnim.calcAttachmentCircle("body_collision", "body_collision").xy + Point2(0.0, 16.0);
}

void BombBat::updateAsBomb()
{
  updateFromPhysical();
  
  if (standingPhysical() == true)
  {
    BombExplosion* explosion = new BombExplosion();
    explosion->setXY(getXY());
    level->addPGO(explosion);
    
    dropCoin();
    
    setRemove();
  }
}

void BombBat::redrawMe()
{
  if (flying == true)
  {
    addY(bobOffset);
  }
  
  Point2 realXY = getXY();
  Point1 realRotation = getRotation();
  Logical realFish = fishRotationActive();
  setXY(wingPt);
  useFishRotation(false);
  setRotation(0.0);
  
  if (onRMScreen() == true)
  {
    renderSpineAnim(wingAnimator);
  }

  setXY(realXY);
  useFishRotation(realFish);
  setRotation(realRotation);
  PizzaGOStd::redrawMe();

  if (flying == true)
  {
    addY(-bobOffset);
  }
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    Circle(wingPt, 4.0).draw_solid(GREEN_SOLID);
  }
}

void BombBat::updateCollisions()
{
  if (touchingPlayerCircle() == true && flying == true)
  {
    flying = false;

    useFishRotation(false);
    setRotation(0.0);
    
    mySpineAnim.startAnimation("fuseburn", 0, 1.0);
    wingAnimator.startAnimation("lose_wings", 0, 0.25);

    init_circle(getXY(), getWidth(), true);
    set_collisions(0x1 << TYPE_ENEMY, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_TERRAIN));
    set_b2d_callback_data(this);
    change_density(40.0);
    crushesPhysical = false;
  }
}

Circle BombBat::collisionCircle()
{
  return Circle(getXY(), 24.0);
}

void BombBat::midCallback(Coord1 value)
{

}

void BombBat::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    facePlayer();
    
    if (playerOverMyPlatform() == true)
    {
      // script.enqueueX(new ArcsineFn(&xy.x, RM::flatten(getX(), player->getX(), RM::randf(128.0, 256.0)), RM::randf(1.0, 2.0)));
      Point1 duration = RM::randf(1.0, 2.0);
      script.enqueueX(new ArcsineFn(&currSpeed, RM::randf(200.0, 300.0), duration * 0.5));
      script.enqueueX(new ArcsineFn(&currSpeed, 0.0, duration * 0.5));
    }
    else
    {
      script.wait(0.5);
    }
  }
}


// =============================== HammerBro ============================ //

const Point1 HammerBro::WALK_SPEED = 64.0;

HammerBro::HammerBro(Coord1 setValue) :
  PizzaGOStd(),

  startXY(0.0, 0.0),

  walkXData(0.0, 0.5, 1.0),

  isWalking(true),

  throwScript(this)
{
  basePoints = 10;
  
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  value = setValue;
}

void HammerBro::load()
{
  startXY = getXY();
  sortY = getY();
  // useFishRotation();
  
  initSpineAnim(hammerBrosSpine, "walk_bottom");
  setSpineAnim(1, "idle_top", true);
  
  switch (value)
  {
    default:
    case GL_ENEMY_HAMMER_BROS: mySpineAnim.setSkin("hammer"); break;
    case GL_ENEMY_TRIANGLE_BROS: mySpineAnim.setSkin("engineer"); break;
    case GL_ENEMY_DYNAMITE_BROS: mySpineAnim.setSkin("dynamite"); break;
  }
  
  setWH(collisionCircle().radius, collisionCircle().radius);  // radius
  
  script.wait(0.3);
  throwScript.wait(0.5);

  facePlayer();
  updateSpineAnim();
}

void HammerBro::updateMe()
{
  if (isWalking)
  {
    RM::bounce_accelcurve(walkXData, Point2(-1.0, 1.0), 1.0 * RM::timePassed());
  }
  
  setX(startXY.x + walkXData.x * 64.0);
  
  script.update();
  throwScript.update();
  
  actions.update();
  updateSpineAnim();

  updateCollisions();
}

void HammerBro::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void HammerBro::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  if (touchingPlayerCircle() == true)
  {
    destroyPhysical();
    level->enemyDefeated(this);
  }
}

void HammerBro::midCallback(Coord1 value)
{
  Point2 projectileXY = getXY() + Point2(0.0 * facingToDir(), -204.0);
  
  switch (this->value)
  {
    default:
    case GL_ENEMY_HAMMER_BROS:
    {
      Point1 projAngle = getHFlip() == false ? HALF_PI * 3.5 : HALF_PI * 2.5;
      
      Hammer* projectile = new Hammer();
      projectile->setXY(projectileXY);
      projectile->hflip(getHFlip());
      projectile->setRotation(facingToDir() == 1 ? HALF_PI * 3.5 : HALF_PI * 2.5);
      level->addPGO(projectile);
      
      Point2 projVel = RM::pol_to_cart(500.0, projAngle);
      projVel.y -= 128.0; // technically this should be a change to its angle but this is okay
      projectile->set_velocity(projVel);
      break;
    }
      
    case GL_ENEMY_TRIANGLE_BROS:
    {
      Boomerang* projectile = new Boomerang();
      projectile->setXY(projectileXY);
      projectile->hflip(getHFlip());
      projectile->setRotation(facingToDir() == 1 ? HALF_PI * 3.5 : HALF_PI * 2.5);
      level->addPGO(projectile);
      break;
    }
      
    case GL_ENEMY_DYNAMITE_BROS:
    {
      Point1 projAngle = getHFlip() == false ? HALF_PI * 3.5 : HALF_PI * 2.5;
      
      Dynamite* projectile = new Dynamite();
      projectile->setXY(projectileXY);
      projectile->hflip(getHFlip());
      projectile->setRotation(facingToDir() == 1 ? HALF_PI * 3.5 : HALF_PI * 2.5);
      level->addPGO(projectile);
      
      Point2 projVel = RM::pol_to_cart(500.0, projAngle);
      projVel.y -= 128.0; // technically this should be a change to its angle but this is okay
      projectile->set_velocity(projVel);
      break;
    }
  }
}

void HammerBro::enqueueShuffle(Point1 duration)
{
  script.wait(duration);
}

void HammerBro::enqueueThrow(Coord1 numShots)
{
  // windup
  throwScript.enqueueX(new StartSpineCommand(&mySpineAnim, "attack_top", 1, Point2(0.0, 10.0 / 60.0), 0.3));
  throwScript.wait(0.5);

  // throws
  throwScript.enqueueX(new StartSpineCommand(&mySpineAnim, "attack_top", 1, Point2(-1.0, 14.0 / 60.0), 0.1));

  for (Coord1 i = 0; i < numShots; ++i)
  {
    throwScript.wait(0.1);
    throwScript.enqueueMidCallback(this, 0);
  }
  
  // follow through
  throwScript.enqueueX(new StartSpineCommand(&mySpineAnim, "attack_top", 1, Point2(-1.0, -1.0), 0.5));
  throwScript.wait(0.5);
  
  throwScript.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_top", 1, true));
}

void HammerBro::enqueueJump()
{
  isWalking = false;
  
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "jump_bottom", 0, Point2(0.0, 15.0 / 60.0), 0.25));
  script.enqueueX(new DeAccelFn(&xy.y, startXY.y - 196.0, 0.6));
  script.enqueueX(new AccelFn(&xy.y, startXY.y, 0.6));
  // script.enqueueX(new StartSpineCommand(&mySpineAnim, "jump_bottom", 0, Point2(-1.0, -1.0), 0.1));
  // script.wait(0.1);

  script.enqueueX(new StartSpineCommand(&mySpineAnim, "walk_bottom", 0, true));
  script.enqueueX(new SetValueCommand<Logical>(&isWalking, true));
}

void HammerBro::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    if (RM::randl() == true)
    {
      enqueueJump();
      enqueueShuffle(RM::randf(1.5, 4.0));
    }
    else
    {
      enqueueShuffle(RM::randf(1.5, 4.0));
    }
  }
  else if (caller == &throwScript)
  {
    throwScript.wait(RM::randf(0.5, 1.7) * RM::randf(0.5, 1.7));
    enqueueThrow(RM::randi(1, 3));
  }
}


// =============================== Hammer ============================ //

Hammer::Hammer() :
  PizzaGOStd(),

  breakTimer(2.5, this)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_ENEMY;
  
  setImage(getImg(hammerImg));
  autoSize(0.5);  // radius
  
  coinToDropType = NO_COIN;
}

void Hammer::load()
{
  sortY = myPlatformLine.get_y_at_x(getX());
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger
  
  change_density(3.0);
  set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);
  set_ang_vel(PI * facingToDir());
  set_velocity(Point2(500.0 * facingToDir(), 0.0));
  set_restitution(0.7);
  fixture->SetUserData(this);
}

void Hammer::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  if (lifeState == RM::ACTIVE && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysical();
    
    // level->addAction(new BombExplosion(getXY()));
  }
}


void Hammer::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void Hammer::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  destroyPhysical();
}

// =============================== Boomerang ============================ //

Boomerang::Boomerang() :
  PizzaGOStd(),

  flyDur(1.5)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_ENEMY;
  
  setImage(getImg(speedSquareImg));
  autoSize(0.5);  // radius
  
  coinToDropType = NO_COIN;
}

void Boomerang::load()
{
  actions.addX(new LinearFn(&xy.y, getY() + 64.0, flyDur));
  script.enqueueX(new SineFn(&xy.x, getX() + facingToDir() * 400.0, flyDur * 0.5));
  script.enqueueAddX(new CosineFn(&xy.x, getX() - facingToDir() * 400.0, flyDur * 1.0), &actions);
  script.wait(flyDur * 0.7);
  script.enqueueX(new LinearFn(&color.w, 0.0, flyDur * 0.03));
}

void Boomerang::updateMe()
{
  actions.update();
  script.update();
  
  addRotation(2.0 * facingToDir() * TWO_PI * RM::timePassed());
  
  if (lifeState == RM::ACTIVE && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    destroyPhysical();
  }
}


void Boomerang::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void Boomerang::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  setRemove();
}

// =============================== Dynamite ============================ //

Dynamite::Dynamite() :
  PizzaGOStd(),

  breakTimer(2.5, this)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_ENEMY;
  
  setImage(getImg(dynamiteImg));
  autoSize(0.5);  // radius
  
  coinToDropType = NO_COIN;
}

void Dynamite::load()
{
  sortY = myPlatformLine.get_y_at_x(getX());
  init_circle(getXY(), getWidth(), true);
  addWH(4.0, 4.0);  // make true collision a hair larger
  
  change_density(10.0);   // this is so it generates enough of an impact for a collision callback
  set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);
  set_ang_vel(PI * facingToDir());
  set_velocity(Point2(500.0 * facingToDir(), 0.0));
  set_restitution(0.7);
  fixture->SetUserData(this);
}

void Dynamite::updateMe()
{
  updateFromPhysical();
  breakTimer.update();
  
  if (getActive() == true &&
      touchingPlayerCircle() == true)
  {
    destroyPhysical();
  }
  
  checkPhysicalDestruction();
}

void Dynamite::destroyPhysical()
{
  BombExplosion* explode = new BombExplosion();
  explode->setXY(getXY());
  level->addPGO(explode);
  
  PizzaGOStd::destroyPhysical();
}

void Dynamite::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (getActive() == false || destroyedPhysical == true)
  {
    return;
  }
  
  destroyedPhysical = true;
}


void Dynamite::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void Dynamite::callback(ActionEvent* caller)
{
  if (getActive() == false)
  {
    return;
  }
  
  destroyPhysical();
}

// =============================== EnemyPathMover ============================ //

EnemyPathMover::EnemyPathMover(PizzaGOStd* myParent) :
  parent(myParent),

  pathPoints(),
  targetPathIndex(1),
  pathMoveDir(1),
  speed(128.0),

  endType(PATHMOVE_LOOP)
{
  
}

void EnemyPathMover::init(const DataList<Point2>& path, Coord1 setMoveDir)
{
  pathPoints = path;
  pathMoveDir = setMoveDir;
  
  // find the nearest point
  Coord1 closestPathIndex = 0;
  Point1 closestDist = RM::distance_to(parent->getXY(), pathPoints[0]);
  
  for (Coord1 i = 1; i < pathPoints.count; ++i)
  {
    Point1 currDistance = RM::distance_to(parent->getXY(), pathPoints[i]);
    
    if (currDistance < closestDist)
    {
      closestPathIndex = i;
      closestDist = currDistance;
    }
  }
  
  targetPathIndex = closestPathIndex;
  parent->setXY(pathPoints[targetPathIndex]);
}

void EnemyPathMover::update()
{
  if (targetPathIndex == -1 || pathPoints.count <= 1)
  {
    return;
  }
  
  Logical arrived = RM::attract_me(parent->xy, pathPoints[targetPathIndex], speed * RM::timePassed());
  
  if (arrived == true)
  {
    Coord2 pathResult = nextPtIndex(targetPathIndex, pathMoveDir);
    targetPathIndex = pathResult.x;
    pathMoveDir = pathResult.y;
  }
}

Coord2 EnemyPathMover::nextPtIndex(Coord1 startIndex, Coord1 dir)
{
  Coord2 result(startIndex + dir, dir);
  
  if (endType == PATHMOVE_LOOP)
  {
    if (result.x > pathPoints.last_i())
    {
      return Coord2(0, dir);
    }
    else if (result.x < 0)
    {
      return Coord2(pathPoints.last_i(), dir);
    }
  }
  else
  {
    if (result.x > pathPoints.last_i())
    {
      return Coord2(pathPoints.last_i() - 1, -1);
    }
    else if (result.x < 0)
    {
      return Coord2(1, 1);
    }
  }
  
  return result;
}

// =============================== Chicken ============================ //

const Point1 Chicken::WALK_SPEED = 128.0;

Chicken::Chicken() :
  PizzaGOStd(),

  currSpeed(WALK_SPEED)
{
  
}

void Chicken::load()
{
  setWH(64.0, 64.0);  // radius
  initSpineAnim(chickenSpine, "idle");
}

void Chicken::updateMe()
{
  facePlayer();
  pathMover.update();
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  
  updateSpineAnim();
  
  updateCollisions();
}

void Chicken::redrawMe()
{
  addY(bobOffset);
  PizzaGOStd::redrawMe();
  addY(-bobOffset);
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
  
}

void Chicken::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    Point1 angleToPlayer = RM::angle(getXY(), player->getXY());
    
    if (angleToPlayer >= PI * 1.25 && angleToPlayer <= PI * 1.75)
    {
      destroyPhysical();
      level->enemyDefeated(this);
    }
    else
    {
      player->attacked(getXY(), this);
    }
  }
}

Circle Chicken::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== Flameskull ============================ //

Flameskull::Flameskull() :
  PizzaGOStd()
{
  
}

void Flameskull::load()
{
  setWH(64.0, 64.0);  // radius
  initSpineAnim(flameskullSpine, "idle");
}

void Flameskull::updateMe()
{
  Point2 oldXY = getXY();
  pathMover.update();
  
  faceDir(RM::float_to_int(getX() - oldXY.x));
  
  updateSpineAnim();
  
  updateCollisions();
}

void Flameskull::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
  
}

void Flameskull::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    Point1 angleToPlayer = RM::angle(getXY(), player->getXY());

    player->attacked(getXY(), this);
  }
}

Circle Flameskull::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== Mohawk ============================ //

Mohawk::Mohawk() :
  PizzaGOStd()
{
  
}

void Mohawk::load()
{
  setWH(64.0, 64.0);  // radius
  initSpineAnim(mohawkSpine, "idle");
}

void Mohawk::updateMe()
{
  Point2 oldXY = getXY();
  pathMover.update();

  addRotation(HALF_PI * RM::timePassed());
  
  updateSpineAnim();
  
  updateCollisions();
}

void Mohawk::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
  
}

void Mohawk::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    Point1 localHitAngle = RM::standardize_rads(RM::angle(getXY(), player->getXY()) - getRotation());

    if (localHitAngle >= PI && localHitAngle <= PI * 1.75)
    {
      player->attacked(getXY(), this);
    }
    else
    {
      destroyPhysical();
      level->enemyDefeated(this);
    }
  }
}

Circle Mohawk::collisionCircle()
{
  return Circle(getXY(), getWidth());
}


// =============================== BombExplosion ============================ //

BombExplosion::BombExplosion() :
  PizzaGOStd()
{
  phase = PHASE_ENEMY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;

  setScale(0.0, 0.0);
  setImage(getImg(batExplosionImg));
}

void BombExplosion::load()
{
  script.enqueueX(new DeAccelFn(&scale.x, 1.0, 0.1));
  script.wait(1.0);
  script.enqueueAddX(new DeAccelFn(&scale.x, 2.0, 0.1), &actions);
  script.enqueueX(new DeAccelFn(&color.w, 0.0, 0.1));
}

void BombExplosion::updateMe()
{
  matchXScale();
  
  script.update();
  actions.update();
  
  updateCollisions();
}

void BombExplosion::updateCollisions()
{
  if (touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
  }
}


void BombExplosion::redrawMe()
{
  drawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void BombExplosion::callback(ActionEvent *caller)
{
  setRemove();
}

// =============================== ParticleTestSkull ============================ //

ParticleTestSkull::ParticleTestSkull() :
  PizzaGOStd(),

  animator(NULL)
{
  phase = PHASE_ENEMY_STD;
}

void ParticleTestSkull::load()
{
  animator = new SpineAnimator(getSpineSkeleton(gilaSpine), getSpineAnim(gilaSpine));
  animator->spineSkeleton->x = getX();
  animator->spineSkeleton->y = getY();
  
  spSkeleton_setToSetupPose(animator->spineSkeleton);
  spSkeleton_updateWorldTransform(animator->spineSkeleton);
  
  int track = 0.0;
  int loop = 1;
  float delay = 0.0;
  spAnimationState_addAnimationByName(animator->spineAnimState, track, "walk", loop, delay);
  
  // animator->spineAnimState->listener = &animEventListener;
}

void ParticleTestSkull::updateMe()
{
  // cout << "updationg " << script.length() << " " << &script  << endl;
  animator->updateSpineSkeleton();
}

void ParticleTestSkull::redrawMe()
{
  /*
  // Box(getX(), getY(), 104.0, 56.0).draw_solid(GREEN_SOLID);
  Point2 realXY = getXY();

  setImage(getImg(skullTestImg));
  setScale(1.0, 1.0);
  drawMe();

  setImage(getImg(skullTestImg));
  setScale(1.0, 1.0);
  setXY(realXY);
   */

  Box drawAABB = getLocalDrawAABB();
  drawAABB.xy += getXY();
  drawAABB.draw_outline(GREEN_SOLID);

  Circle(getXY(), 4.0).draw_outline(RED_SOLID);
  
  animator->spineSkeleton->x = getX();
  animator->spineSkeleton->y = getY();
  spSkeleton_updateWorldTransform(animator->spineSkeleton);
  animator->renderSpineSkeleton();
  // animator->drawBoundingBoxes();
  // cout << "redraw " << script.length() << " " << &script  << endl;
}

Box ParticleTestSkull::getLocalDrawAABB()
{
  if (usingSpineAnim() == true)
  {
    Box spineBox = mySpineAnim.calcSpineAllBoxesAABB();
    spineBox.xy -= getXY();
    return spineBox;
  }
  else
  {
    return Box::from_center(Point2(24.0, 0.0), Point2(280.0, 100.0));
  }
}

// =============================== TerrainQuad ============================ //

TerrainQuad::TerrainQuad() :
  PizzaGOStd(),

  handledCenter(true),
  updatesFromPhysical(true),

  topRight(0.0, 0.0),

  puppyEdgeBits(0x0),

  isBouncy(false),
  startBounce(false),
  bounceScaleOffset(1.0),
  bouncer(&bounceScaleOffset, 1.1, 0.1, this),

  respondsToCupcakeFlame(false),
  continuousBurn(false),
  igniteTimer(0.0),
  burnoutTimer(0.0),
  balloonStartedSeekingMe(false),

  fireVR(),
  fireAnim(),
  firemanSetIndex(firemanTeddySet),

  disintigrateState(DOESNT_DISINTIGRATE),
  disintigrateScript(this),

  targetPathIndex(-1),
  pathMoveDir(0),
  pathPoints(),
  pathMoveSpeed(256.0),

  isSpinner(false),
  spinningNow(false),
  spinScript(this),

  isMagicTrigger(false),
  magicTimer(5.0, this),
  magicObjectList(),

  usesSpikeSides(false),
  spikeSides(0x0),

  usesBreakSides(false),
  breakSides(0x0),

  surpriseRestockTimer(5.0, this),
  surpriseItemType(NO_COIN),

  animScript(this),

  algoBlockDrawer(),
  algoDrawType(ALGOBLOCK_UNSET),
  algoQuadPts(),

  anchor()
{
  basePoints = 5;
  
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;

  bouncer.setInactive();
  magicTimer.setInactive();
  surpriseRestockTimer.setInactive();
}

void TerrainQuad::load()
{
  if (level->spikedTerrain == true) flags |= SPIKED_BIT;
  if (crushesPhysical == true) level->totalDestructibles++;
}

void TerrainQuad::updateMe()
{
  if (body != NULL)
  {
    if (body->GetType() == b2_dynamicBody)
    {
      updateFromPhysical();
    }
    else
    {
      updatePathMotion();  // does nothing unless enabled
      updateMagicTrigger();  // does nothing unless enabled
      
      if (handledCenter == true)
      {
        updateFromWorld();
      }
      
      body->SetLinearVelocity(wpt_to_b2d((getXY() - oldXY) / RM::timePassed()));
    }
    
    checkSurface();
    checkPhysicalDestruction();
  }
  
  updateCupcakeBurn();
 
  updateDisintigrate();
  
  animScript.update();
  animQueue.update();
  
  bouncer.update();
  matchXScale();
  
  if (usingSpineAnim() == true)
  {
    updateSpineAnim();
  }
  
  surpriseRestockTimer.update();

  updateSpinning();

  // should only be called once
  if (destroyedPhysical == true)
  {
    blockDestroyed();
  }
  
  // removes boxes that fall into puppy pits
  if ((flags & DELETE_BELOWWORLD_BIT) != 0 &&
      getActive() == true &&
      getY() > RM_WH.y + 128.0)
  {
    setRemove();
  }
  
  oldXY = getXY();
}

void TerrainQuad::redrawMe()
{
   if (drawsAlgo() == true && onRMScreen() == true)
  {
    // cout << visibleAABB() << endl;
    drawAlgo();
  }
  else if (usingSpineAnim() == true && onRMScreen() == true)
  {
    // Circle(getXY(), 4.0).draw_solid(GREEN_SOLID);
    
    PizzaGOStd::redrawMe();
  }
  else if (getImage() != NULL && onRMScreen() == true)
  {
    // this is for bouncy things
    Point2 realScale = getScale();
    setScale(realScale * bounceScaleOffset);
    
    drawMe();
    
    if (dmgOverlayImg != NULL)
    {
      Point2 quadXY = getImage()->calc_quad_xy(getXY(), getDrawScale(), getRotation(), Quad::unit_square());
      // Quad posQuad = getImage()->calc_quad_coords(getXY(), getDrawScale(), getRotation(), Quad::unit_square());
      Point2 sizeCorrection =  getImage()->natural_size() / dmgOverlayImg->natural_size();
      
      // cout << "xy " << quadXY << " posquad " << posQuad << endl;
      Quad dmgQuad = Quad::unit_square();
      getImage()->draw_dmg_overlay_quad(quadXY, getDrawScale(), getRotation(),
                                        Quad::unit_square(), dmgOverlayImg, dmgQuad, 1.0);
    }
    
    setScale(realScale);
    
    // spinners draw the nut
    if (getImage() == getImg(spinnerImg))
    {
      Point1 realRotation = getRotation();
      setRotation(0.0);
      setImage(getImg(spinnerBoltImg));
      drawMe();

      setImage(getImg(spinnerImg));
      setRotation(realRotation);
    }
  }
  
  if (isBurning() == true)
  {
    // Circle(collisionBox().center(), 16.0).draw_solid(ColorP4(1.0, 0.15, 0.0, 0.5));
    // collisionBox().draw_outline(WHITE_SOLID);
    
    if (onRMScreen() == true)
    {
      fireVR.drawMe();
    }
  }
}

void TerrainQuad::drawDStruct()
{
  if (drawsAlgo() == true)
  {
    drawAlgo();
  }
  else if (getImage() != NULL)
  {
    Point2 realScale = getScale();
    setScale(realScale * bounceScaleOffset);
    
    drawMe();

    setScale(realScale);
  }
}

void TerrainQuad::updateSpinning()
{
  spinScript.update();
  
  if (spinningNow == true)
  {
    set_ang_vel(TWO_PI * 1.25);
  }
  else
  {
    set_ang_vel(0.0);
  }
}

Box TerrainQuad::collisionBox()
{
  if (handledCenter == true)
  {
    return boxFromC();
  }
  else
  {
    return boxFromTL();
  }
}

LightsaberTouchResult TerrainQuad::touchingBossCandle()
{
  LightsaberTouchResult result;

  for (b2ContactEdge* edge = body->GetContactList(); edge != NULL; edge = edge->next)
  {
    Point2 pt0 = edge_to_wpt(edge);
    Point1 contactAngle = RM::angle(getXY(), pt0);
    
    b2Fixture* fixtureA = edge->contact->GetFixtureA();
    b2Fixture* fixtureB = edge->contact->GetFixtureB();
    b2Filter filterA = fixtureA->GetFilterData();
    b2Filter filterB = fixtureB->GetFilterData();
    
    result.touchType = SABER_TOUCHING_NOTHING;
    
    if (b2FiltersMatchCategories(filterA, filterB, type, TYPE_BOSS_LIGHTSABER) == true)
    {
      result.touchType = SABER_TOUCHING_ME;
      
      // NOT TESTED YET
      result.attackingPgo = static_cast<PizzaGOStd*>(b2FilterMatchesCategory(filterA, TYPE_BOSS_LIGHTSABER) ?
          fixtureB->GetUserData() : fixtureA->GetUserData());
    }
    
    if (result.touchType != SABER_TOUCHING_NOTHING &&
        edge->contact->IsTouching() == true &&
        edge->contact->IsEnabled() == true)
    {
      result.touchPt = pt0;
     
      return result;
    }
  }
  
  return result;
}

void TerrainQuad::updateCupcakeBurn()
{
  Point1 timeToLight = 0.25;
  Point1 timeToBurn = 3.0;
  
  if (respondsToCupcakeFlame == true)
  {
    Logical touchingCandle = touchingBossCandle().touchType == SABER_TOUCHING_ME;

    // touched flame long enough, light it up
    if (touchingCandle == true && burnoutTimer < 0.01)
    {
      burnoutTimer = timeToBurn;
    }
    
    RM::flatten_me(igniteTimer, touchingCandle ? 0.0 : timeToLight, RM::timePassed());
    RM::flatten_me(burnoutTimer, touchingCandle ? timeToBurn : 0.0, RM::timePassed());
  }
  
  if (continuousBurn == true)
  {
    burnoutTimer = timeToBurn;
  }
  
  // burn player
  if (isBurning() == true && touchingPGO(player) == true)
  {
    player->touchedFire(player->collisionBox().norm_pos(HANDLE_BC), this);
  }
  
  if (isBurning() == true)
  {
    // need to init the flame animation
    if (fireAnim.parent == NULL)
    {
      fireAnim.init(&fireVR, getImgSet(firemanFlameSet), 0.02);
      fireAnim.repeats();
    }

    fireVR.setXY(collisionBox().norm_pos(HANDLE_BC));
    fireAnim.update();
  }
}

Logical TerrainQuad::isBurning()
{
  return burnoutTimer >= 0.01;
}

void TerrainQuad::quenchBurning()
{
  continuousBurn = false;
  burnoutTimer = 0.0;
  activeGameplayData->objectsExtinguished++;
  
  setImage(getImgSet(firemanSetIndex)[1]);
}

void TerrainQuad::setDisintigrate()
{
  disintigrateState = FULLY_FORMED;
  
  initSpineAnim(disintigrateBlockSpine, "idle");
}

void TerrainQuad::updateDisintigrate()
{
  if (disintigrateState == DOESNT_DISINTIGRATE)
  {
    return;
  }
  
  disintigrateScript.update();
  
  switch (disintigrateState)
  {
    case DOESNT_DISINTIGRATE:
    default:
      return;
      break;
      
    case FULLY_FORMED:
      if (touchingPGO(player) == true)
      {
        disintigrateState = FADING_OUT;
        disintigrateScript.wait(1.0);
        mySpineAnim.startAnimation("shake", 0, true);
      }
      break;
      
    case FADING_OUT:
    {
      if (disintigrateScript.getActive() == false)
      {
        mySpineAnim.startAnimation("break", 0, false);
        set_collisions(0x1 << TYPE_TERRAIN, 0x0);
        disintigrateState = FULLY_OUT;
        disintigrateScript.wait(5.0);
      }
    }
      
    case FULLY_OUT:
      if (disintigrateScript.getActive() == false)
      {
        disintigrateState = FADING_IN;
        disintigrateScript.wait(1.0);
        mySpineAnim.startAnimation("regen", 0, false);
      }
      break;
      
    case FADING_IN:
      if (disintigrateScript.getActive() == false)
      {
        disintigrateState = FULLY_FORMED;
        set_collisions(0x1 << TYPE_TERRAIN, 0xffff);
        mySpineAnim.startAnimation("idle", 0, true);
      }
      break;
  }
}

void TerrainQuad::enablePathMove()
{
  targetPathIndex = 0;
}

void TerrainQuad::updatePathMotion()
{
  if (targetPathIndex == -1 || pathPoints.count <= 1)
  {
    return;
  }
  
  Coord1 startMoveDir = 0;
  
  // if player is to the right of the entire path, move right
  if (pathMoveDir != 1 &&
      targetPathIndex < pathPoints.last_i() &&
      player->collisionCircle().left() > pathPoints.last().x)
  {
    startMoveDir = 1;
  }
  // if player is to the left of the entire path, move left
  else if (pathMoveDir != -1 &&
           targetPathIndex > 0 &&
           player->collisionCircle().right() < pathPoints.first().x)
  {
    startMoveDir = -1;
  }
  // if stopped and touching player, move forward
  else if (pathMoveDir == 0)
  {
    if (touchingPGO(player) == true)
    {
      if (targetPathIndex == 0)
      {
        startMoveDir = 1;
      }
      else
      {
        startMoveDir = -1;
      }
    }
  }
  
  if (startMoveDir != 0)
  {
    pathMoveDir = startMoveDir;
    targetPathIndex += startMoveDir;
  }
  
  if (pathMoveDir == 0)
  {
    return;
  }
  
  Point1 speed = pathMoveSpeed * RM::timePassed();
  Logical arrived = RM::attract_me(xy, pathPoints[targetPathIndex], speed);

  if (arrived == true)
  {
    targetPathIndex += pathMoveDir;
    RM::clamp_me(targetPathIndex, 0, pathPoints.last_i());
  }
  
  if (arrived == true &&
      (targetPathIndex == 0 || targetPathIndex == pathPoints.last_i()))
  {
    pathMoveDir = 0;
  }
}

void TerrainQuad::enableMagicTrigger(Point1 onTime)
{
  isMagicTrigger = true;
  magicTimer.reset(onTime);
  magicTimer.setInactive();
  
  setImage(getImgSet(shopButtonImgs)[0]);
}

void TerrainQuad::updateMagicTrigger()
{
  if (isMagicTrigger == false)
  {
    return;
  }
  
  magicTimer.update();
  
  if (magicTimer.getActive() == true)
  {
    return;
  }
  
  if (touchingPGO(player) == true)
  {
    set_collisions(0x1 << TYPE_TERRAIN, 0x0);  // make me disappear
    setImage(getImgSet(shopButtonImgs)[1]);

    for (Coord1 i = 0; i < magicObjectList.count; ++i)
    {
      magicObjectList[i]->magicTriggerOn();  // make these appear
    }
    
    magicTimer.reset();
  }
}

XFormer TerrainQuad::drawAlgo(Logical suppressDraw)
{
  LightingEnvironment lighting;
  lighting.lights.add(LightSource());
  lighting.lights.last().color = WHITE_SOLID;
  
  XFormer result;
  
  switch (algoDrawType)
  {
    case ALGOBLOCK_QUAD:
    {
      // cout << "quad pts " << algoQuadPts << endl;
      result = algoBlockDrawer.draw(algoQuadPts, getXY(), getScale(), getRotation(), getColor(), lighting, suppressDraw);
      break;
    }
      
    case ALGOBLOCK_TRI:
    {
      Tri algoTriPts(algoQuadPts.x, algoQuadPts.y, algoQuadPts.z);
      
      result = algoBlockDrawer.draw(algoTriPts, getXY(), getScale(), getRotation(), getColor(), lighting, suppressDraw);
      break;
    }
      
    case ALGOBLOCK_DISK:
    {
      result = algoBlockDrawer.draw_disc(getWidth(), getXY(), getScale(), getRotation(), getColor(), lighting, 1, suppressDraw);
      break;
    }
  }
  
  // cout << "params xy " << getXY() << " scale " << getScale() << " rotation " << getRotation() << endl;
  // cout << "postAABB " << result.postAABB << endl;
  
  // Circle(collisionBox().xy, 4.0).draw_outline(WHITE_SOLID);
  return result;
}

void TerrainQuad::setBreakaway()
{
  flags |= BREAKAWAY_BIT;
}

void TerrainQuad::setSpikey()
{
  flags |= SPIKED_BIT;
}

void TerrainQuad::setSpikeySide(Coord1 dir4Side)
{
  usesSpikeSides = true;
  spikeSides |= (0x1 << dir4Side);
}

void TerrainQuad::setBreakSide(Coord1 dir4Side)
{
  usesBreakSides = true;
  breakSides |= 0x1 << dir4Side;
}

void TerrainQuad::setSpinning()
{
  isSpinner = true;
  callback(&spinScript);
}

void TerrainQuad::checkSurface()
{
  if (flags == 0 && usesSpikeSides == false) return;
  
  Point2 location;
  if (touchingPGO(player, &location) == true) playerTouch(location);
  
  /*
  if ((flags & BALANCE_BOUNCE_BIT) != 0)
  {
    BalanceSkull* ball = level->getCurrBall();
   
    if (touchingPGO(ball) == true)
    {
      ball->set_velocity(Point2(ball->get_velocity().x, -128.0));
    }
  }
   */
}

void TerrainQuad::playerTouch(Point2 location)
{
  if ((flags & SPIKED_BIT) != 0)
  {
    player->attacked(location, this);
  }
  else if (usesSpikeSides == true)
  {
    // this assumes that the spike block is a square, handled top left
    Coord1 hitSide = 0;

    Point1 angleToPizza = RM::angle(getXY(), location);
    
    if (angleToPizza >= HALF_PI * 0.5 && angleToPizza < HALF_PI * 1.5)
    {
      hitSide = 1;
    }
    else if (angleToPizza >= HALF_PI * 1.5 && angleToPizza <= HALF_PI * 2.5)
    {
      hitSide = 2;
    }
    else if (angleToPizza >= HALF_PI * 2.5 && angleToPizza <= HALF_PI * 3.5)
    {
      hitSide = 3;
    }
    
    if (((0x1 << hitSide) & spikeSides) != 0)
    {
      player->attacked(location, this);
    }
  }
  
  /*
  if ((flags & SLOW_BIT) != 0)
  {
    level->reportSurface(flags);
  }
  
  if ((flags & STICK_BIT) != 0)
  {
    level->reportSurface(flags);
  }
   */

  if ((flags & BREAKAWAY_BIT) != 0)
  {
    // level->addAction(new TerrainShatter(*this, 8, 5));
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

void TerrainQuad::objectBumped(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (surpriseRestockTimer.getActive() == false &&
      surpriseItemType != NO_COIN &&
      pgo == player &&
      normalImpulse >= 1000.0)
  {
    PizzaGOStd* item = NULL;
    
    switch (surpriseItemType)
    {
      case GOURDO_ITEM:
      {
        item = new GourdoLantern();
        surpriseRestockTimer.reset();
        break;
      }
      case RANDOM_COIN:
      {
        item = new PizzaCoinStd();
        surpriseItemType = NO_COIN;
        break;
      }
      case RANDOM_GEM:
      {
        item = new PizzaCoinStd(getXY(), RM::randi(FIRST_GEM_TYPE, LAST_GEM_TYPE));
        surpriseItemType = NO_COIN;
        break;
      }
    }

    item->setXY(getXY());
    item->emergeFromBlock();
    level->addPGO(item);
    
    animScript.clear();
    animScript.enqueueX(new SpineAnimationAction(&mySpineAnim, "open", 0));
    animScript.enqueueX(new StartSpineCommand(&mySpineAnim, "open_idle", 0, true));

    Point2 target = getXY() + RM::pol_to_cart(16.0, RM::angle(player->getXY(), getXY()));
    Point1 duration = 0.3;
    
    ActionQueue* bumpScript = new ActionQueue();
    bumpScript->enqueueAddX(new CosineFn(&xy.y, target.y, duration * 0.5), &level->objman.actions);
    bumpScript->enqueueX(new CosineFn(&xy.x, target.x, duration * 0.5));
    bumpScript->enqueueAddX(new CosineFn(&xy.y, getY(), duration * 0.5), &level->objman.actions);
    bumpScript->enqueueX(new CosineFn(&xy.x, getX(), duration * 0.5));
    level->addAction(bumpScript);
  }
}

TerrainQuad* TerrainQuad::createFiremanBlock(Point2 topLeft, Point2 bottomRight)
{
  Box currBox = Box::from_corners(topLeft, bottomRight);
  Quad currQuad = Quad(currBox);
  
  TerrainQuad* burningBlock = TerrainQuad::create_static_quad(currQuad.x, currQuad.y,
                                                              currQuad.z, currQuad.w);
  burningBlock->continuousBurn = true;
  burningBlock->handledCenter = false;
  burningBlock->setBox(currBox);
  burningBlock->phase = PHASE_ENEMY_STD;
  burningBlock->type = TYPE_TERRAIN;
  burningBlock->value = VALUE_BURNING_BLOCK;
  
  pickFiremanBlock(burningBlock);

  activeGameplayData->objectsIgnited++;
  
  return burningBlock;
}

void TerrainQuad::pickFiremanBlock(TerrainQuad* burningBlock)
{
  Point2 size = burningBlock->getSize();
  Coord1 setIndex = -1;
  Coord1 sizeSet = -1;  // this is just debug
  
  DataList<Point2> imgSizes;
  imgSizes.add(Point2(96.0, 96.0));  // teddy
  imgSizes.add(Point2(96.0, 160.0));  // porta/david
  imgSizes.add(Point2(64.0, 64.0));  // books/crown
  imgSizes.add(Point2(120.0, 80.0));  // horn/nuclear
  imgSizes.add(Point2(256.0, 256.0));  // van/waffle
  imgSizes.add(Point2(160.0, 160.0));  // monalisa/computer
  imgSizes.add(Point2(192.0, 192.0));  // stonehenge/motorcycle
  imgSizes.add(Point2(128.0, 128.0));  // burger/synth
  imgSizes.add(Point2(192.0, 128.0));  // cart/wheelbarrow

  Coord1 leastScaleIndex = 0;
  Point1 closestScaleDist = 1.0;
  Point1 uniScale = -1.0;
  
  for (Coord1 i = 0; i < imgSizes.count; ++i)
  {
    Point1 xAbsScale = std::abs(1.0 - size.x / imgSizes[i].x);
    Point1 yAbsScale = std::abs(1.0 - size.y / imgSizes[i].y);
    Point1 scaleDist = std::max(xAbsScale, yAbsScale);
    
    if (scaleDist < closestScaleDist)
    {
      leastScaleIndex = i;
      closestScaleDist = scaleDist;
      uniScale = std::max(size.x / imgSizes[i].x, size.y / imgSizes[i].y);
    }
  }
  
  if (leastScaleIndex == 0)
  {
    setIndex = firemanTeddySet;
    sizeSet = 0;
  }
  else
  {
    setIndex = firemanPortaSet + (leastScaleIndex - 1) * 2 + (RM::randl() ? 1 : 0);
    sizeSet = leastScaleIndex;
  }
    
  burningBlock->firemanSetIndex = setIndex;
  burningBlock->setImage(getImgSet(burningBlock->firemanSetIndex)[0]);
  burningBlock->setScale(uniScale, uniScale);

  // cout << "my size " << size << "target size " << imgSizes[sizeSet]
  //  << " sizeSet " << sizeSet << " scale " << burningBlock->getScale() << endl;
}


TerrainQuad* TerrainQuad::createSpinner(Point2 topLeft, Point2 bottomRight)
{
  Point2 spinnerSize = bottomRight - topLeft;
  
  TerrainQuad* spinner = new TerrainQuad();
  spinner->setBox(topLeft + spinnerSize * 0.5, spinnerSize);
  spinner->setImage(getImg(spinnerImg));
  // spinner->autoScale();
  
  spinner->init_box(topLeft + spinnerSize * 0.5, spinnerSize, false);
  spinner->set_body_type(b2_kinematicBody);
  spinner->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  spinner->set_b2d_callback_data(spinner);
  spinner->set_friction(0.5);
  spinner->set_restitution(0.0);

  spinner->setSpinning();
  
  return spinner;
}

TerrainQuad* TerrainQuad::createSurpriseBlock(Point2 topLeft, Coord1 itemType)
{
  Point2 size(128.0, 128.0);
  TerrainQuad* block = TerrainQuad::create_static_TL(topLeft, size, NULL);
  block->setXY(topLeft + 0.5 * size);
  // block->setImage(getImgSet(fblockSet)[0]);
  block->initSpineAnim(fblockSpine, "idle_normal", true);
  block->surpriseItemType = itemType;
  block->crushesPhysical = false;
  return block;
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
  /*
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
   */
}

void TerrainQuad::blockDestroyed()
{
  /*
  Coord1 randCoin = RM::randi(0, 1);
  
  if (randCoin == 0)
  {
    createMagnetCoin(SlopeCoin::randValueAll());
  }
  
  ResourceManager::playCrunch();
  level->enemyDefeated(this);
  level->gotScore(basePoints);
   */
}

void TerrainQuad::ignoreAngrySkulls()
{
  set_collisions(0x1 << TYPE_TERRAIN,
                 0xffff & (~(0x1 << TYPE_ENEMY)));
}

void TerrainQuad::setBouncy(Point1 newRestitution)
{
  set_restitution(newRestitution);
  isBouncy = true;
}

Logical TerrainQuad::drawsAlgo()
{
  return algoBlockDrawer.centerImg != NULL && algoDrawType != ALGOBLOCK_UNSET;
}

void TerrainQuad::setRedrockImgs()
{
  algoBlockDrawer.centerImg = getImg(redrockCenterImg);
  algoBlockDrawer.borderImg = getImg(redrockBorderImg);
}

Box TerrainQuad::getLocalDrawAABB()
{
  if (drawsAlgo() == true)
  {
    // cout << "natural image size " << getImage()->natural_size() << endl;
    /*
    Box colBox = collisionBox();
    colBox.grow_from_center(Point2(8.0, 8.0));  // the image draws a little bigger
    
    Quad collisionQuad(colBox);
    collisionQuad.rotate_me(collisionQuad.center(), getRotation());
    
    Box result = Box::from_quad(collisionQuad);
    
    // cout << "absolute box " << result << endl;
    result.xy -= getXY();
    return result;
     */
    
    /*
    Box colBox = collisionBox();
    colBox.grow_from_center(Point2(8.0, 8.0));  // the image draws a little bigger
    colBox.xy -= getXY();
    return colBox;
     */
    
    XFormer xform = drawAlgo(true);
    // cout << "terrain quad aabb " << xform.preAABB << endl;
    return xform.preAABB;
  }
  else
  {
    return PizzaGOStd::getLocalDrawAABB();
  }
}

Box TerrainQuad::visibleAABB()
{
  if (drawsAlgo() == true)
  {
    XFormer xform = drawAlgo(true);
    // cout << "terrain quad aabb " << xform.preAABB << endl;
    return xform.postAABB;
  }
  else
  {
    return PizzaGOStd::visibleAABB();
  }
}

void TerrainQuad::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (isBouncy == false)
  {
    // this will break destructibles
    PizzaGOStd::collidedPhysical(contact, pgo, normalImpulse);
  }
  else
  {
    // this triggers the bounce
    // ResourceManager::playBounceSound();
    startBounce = true;
    bouncer.reset(1.15);
  }
}

Logical TerrainQuad::objectShouldCrush(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  Logical onCorrectSide = true;
  
  if (usesBreakSides == true)
  {
    Point2 touchPt = contact_to_wpt(contact);
    
    // this assumes that the spike block is a square, handled top left
    Coord1 hitSide = 0;
    
    Point1 angleToCollider = RM::angle(getXY(), touchPt);
    
    if (angleToCollider >= HALF_PI * 0.5 && angleToCollider < HALF_PI * 1.5)
    {
      hitSide = 1;
    }
    else if (angleToCollider >= HALF_PI * 1.5 && angleToCollider <= HALF_PI * 2.5)
    {
      hitSide = 2;
    }
    else if (angleToCollider >= HALF_PI * 2.5 && angleToCollider <= HALF_PI * 3.5)
    {
      hitSide = 3;
    }
    
    if (((0x1 << hitSide) & breakSides) != 0)
    {
      onCorrectSide = true;
    }
    else
    {
      onCorrectSide = false;
    }
  }
  
  return crushesPhysical == true &&
      onCorrectSide == true &&
      normalImpulse >= crushValue &&
      level->levelState == PhysicsLevelStd::LEVEL_PLAY;
}

TerrainQuad* TerrainQuad::setRedrockTL(Point2 topLeft, Point2 widthHeight)
{
  algoBlockDrawer.borderImg = getImg(breakableA9TopImg);
  algoBlockDrawer.centerImg = getImg(breakableACenterImg);
  algoBlockDrawer.edgeOffset = 2.0;
  algoBlockDrawer.edgeOffset = 0.5;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 0.5;
  algoQuadPts = Quad(Box(topLeft - widthHeight * 0.5, widthHeight));
  algoQuadPts -= topLeft;
  algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  
  return this;
}

void TerrainQuad::setGenericBlockTL(Point2 topLeft, Point2 widthHeight)
{
  algoQuadPts = Quad(Box(topLeft - widthHeight * 0.5, widthHeight));
  algoQuadPts -= topLeft;
  algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
}

void TerrainQuad::setDrawerRandomizedBreakable()
{
  DataList<Point1> probs;
  probs.add(1.0);  // wood
  probs.add(1.0);  // sandstone small cracked

  Coord1 randIndex = RM::random_index_weighted(probs);
  
  if (getImg(blockWorldFillImg) != NULL)
  {
    randIndex = 2;
  }

  switch (randIndex)
  {
    default:
    case 0:
      setDrawerWood();
      break;
      
    case 1:
      if (algoQuadPts.to_AABB().width() < PIZZA_INNER_RADIUS * 0.5 ||
          algoQuadPts.to_AABB().height() < PIZZA_INNER_RADIUS * 0.5)
      {
        setDrawerSandstoneSmallCracked();
      }
      else
      {
        setDrawerSandstoneLargeCracked();
      }
      break;

    case 2:
      switch (level->backgroundType)
      {
        default:
        case GRAVEYARD_BG: setDrawerGravestone(); break;
        case JURASSIC_BG: setDrawerLavarock(); break;
        case CAKE_BG: setDrawerCookie(); break;
        case DUMP_BG: setDrawerTrash(); break;
        case SILICON_BG: setDrawerWindow(); break;
        case BODY_BG: setDrawerBacteria(); break;
        case CATACOMBS_BG: setDrawerSkulls(); break;
      }
      break;
  }
}

void TerrainQuad::setDrawerWood()
{
  algoBlockDrawer.borderImg = getImg(blockWoodBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWoodFillImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1;
}

void TerrainQuad::setDrawerSandstoneLarge()
{
  algoBlockDrawer.borderImg = getImg(blockSandstoneBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSandstoneFillImg);
  algoBlockDrawer.triXImg = getImg(blockSandstoneTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerSandstoneLargeCracked()
{
  algoBlockDrawer.borderImg = getImg(blockSandstoneCrackedBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSandstoneCrackedFillImg);
  algoBlockDrawer.triXImg = getImg(blockSandstoneCrackedTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerSandstoneSmall()
{
  algoBlockDrawer.borderImg = getImg(blockSandstoneSmallBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSandstoneFillImg);
  algoBlockDrawer.triXImg = getImg(blockSandstoneSmallTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerSandstoneSmallCracked()
{
  algoBlockDrawer.borderImg = getImg(blockSandstoneSmallCrackedBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSandstoneCrackedFillImg);
  algoBlockDrawer.triXImg = getImg(blockSandstoneSmallCrackedTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerSteel()
{
  algoBlockDrawer.borderImg = getImg(blockSteelBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSteelFillImg);
  algoBlockDrawer.triXImg = getImg(blockSteelTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerSpikes()
{
  algoBlockDrawer.borderImg = getImg(blockSpikesBorderImg);
  algoBlockDrawer.centerImg = getImg(blockSteelFillImg);
  //algoSpikes.triXImg = getImg(blockSteelTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 0;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -40;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerCookie()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.triXImg = getImg(blockWorldTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 4;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -12;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerGravestone()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.triXImg = getImg(blockWorldTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 4;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -12;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerLavarock()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.triXImg = getImg(blockWorldTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerTrash()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.triXImg = getImg(blockWorldTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 2;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 1.0;
  
}

void TerrainQuad::setDrawerWindow()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 0;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -1;
  algoBlockDrawer.fillScale = 1.0;
}

void TerrainQuad::setDrawerBacteria()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 4;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 0.75;
}

void TerrainQuad::setDrawerSkulls()
{
  algoBlockDrawer.borderImg = getImg(blockWorldBorderImg);
  algoBlockDrawer.centerImg = getImg(blockWorldFillImg);
  algoBlockDrawer.triXImg = getImg(blockWorldTrixImg);
  algoBlockDrawer.debugDraw = false;
  algoBlockDrawer.edgeOffset = 4;
  algoBlockDrawer.edgeScale = 1;
  algoBlockDrawer.fillOffset = -16;
  algoBlockDrawer.fillScale = 1.0;
}

TerrainQuad* TerrainQuad::setSteelTL(Point2 topLeft, Point2 widthHeight)
{
  crushesPhysical = false;
  fixture->SetRestitution(0.0);
  change_density(10.0);
  
  algoBlockDrawer.borderImg = getImg(steel9TopImg);
  algoBlockDrawer.centerImg = getImg(steelCenterImg);
  algoBlockDrawer.edgeOffset = 2.0;
  algoBlockDrawer.edgeOffset = 0.5;
  algoBlockDrawer.fillOffset = -8;
  algoBlockDrawer.fillScale = 0.5;
  algoQuadPts = Quad(Box(topLeft - widthHeight * 0.5, widthHeight));
  algoQuadPts -= topLeft;
  algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  
  return this;
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
  else if (caller == &magicTimer)
  {
    set_collisions(0x1 << TYPE_TERRAIN, 0xffff);  // turn me on
    setImage(getImgSet(shopButtonImgs)[0]);

    for (Coord1 i = 0; i < magicObjectList.count; ++i)
    {
      magicObjectList[i]->magicTriggerOff();  // turn these off
    }
  }
  else if (caller == &spinScript)
  {
    Point1 angSpeed = TWO_PI * 8.0;
    spinningNow = false;
    
    spinScript.wait(2.0);
    spinScript.enqueueX(new SetValueCommand<Logical>(&spinningNow, true));
    spinScript.enqueueX(new LinearFn(&rotation, getRotation() + angSpeed, 2.0));
  }
  else if (caller == &surpriseRestockTimer)
  {
    // setImage(getImgSet(fblockSet)[0]);
    animScript.clear();
    animScript.enqueueX(new SpineAnimationAction(&mySpineAnim, "close", 0));
    animScript.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_normal", 0, true));
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
  
  staticQuad->algoQuadPts = Quad(pt0, pt1, pt2, pt3);
  staticQuad->algoQuadPts -= pt0;
  staticQuad->algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;

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
  circleBlock->setWH(radius * 2.0, radius * 2.0);
  
  circleBlock->init_circle(center, radius, false);
  circleBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  circleBlock->set_b2d_callback_data(circleBlock);
  circleBlock->set_friction(0.5);
  circleBlock->updateFromPhysical();
  
  return circleBlock;
}

TerrainQuad* TerrainQuad::create_pachinko_peg(Point2 center, Point1 radius)
{
  TerrainQuad* circleBlock = new TerrainQuad();
  
  circleBlock->init_circle(center, radius, false);
  circleBlock->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  circleBlock->set_b2d_callback_data(circleBlock);
  circleBlock->set_friction(0.1);
  circleBlock->set_restitution(0.25);
  circleBlock->updateFromPhysical();
  
  return circleBlock;
}

TerrainQuad* TerrainQuad::create_bouncer(Point2 center, Point1 radius, Point1 restitution)
{
  TerrainQuad* bouncer = TerrainQuad::create_static_circle(center, radius, NULL);
  bouncer->setBouncy(restitution);
  bouncer->setImage(getImg(bouncerImg));
  bouncer->autoScale();
  return bouncer;
}


TerrainQuad* TerrainQuad::create_pachinko_cross(Point2 center, Point2 horiSize)
{
  TerrainQuad* cross = new TerrainQuad();
  
  cross->init_box(center, horiSize, true);
  cross->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  cross->set_b2d_callback_data(cross);
  cross->set_friction(0.1);
  cross->set_restitution(0.25);
  cross->updateFromPhysical();
  
  b2Fixture* vertFixture = Physical::create_box_fixture(cross->body, Point2(horiSize.y, horiSize.x));
  
  cross->anchor.init(cross->body, center);
  
  return cross;
}

/*
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
 */

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

TerrainQuad* TerrainQuad::create_static_TL(Point2 topLeft, Point2 widthHeight, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setBox(topLeft + widthHeight * 0.5, widthHeight);
  dynamicBox->setImage(setImg);
  dynamicBox->crushesPhysical = true;
  dynamicBox->crushValue = 650.0;
  
  dynamicBox->init_box_TL(topLeft, widthHeight, false);
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
  staticBox->set_b2d_callback_data(staticBox);
  staticBox->set_friction(0.5);
  return staticBox;
}

TerrainQuad* TerrainQuad::create_redrock_TL(Point2 topLeft, Point2 widthHeight, Image* setImg)
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
  
  dynamicBox->algoBlockDrawer.borderImg = getImg(brick9TopImg);
  dynamicBox->algoBlockDrawer.centerImg = getImg(brickCenterImg);
  dynamicBox->algoBlockDrawer.edgeOffset = 2.0;
  dynamicBox->algoBlockDrawer.edgeOffset = 0.5;
  dynamicBox->algoBlockDrawer.fillOffset = -8;
  dynamicBox->algoBlockDrawer.fillScale = 0.5;
  dynamicBox->algoQuadPts = Quad(Box(topLeft, widthHeight));
  dynamicBox->algoQuadPts -= dynamicBox->algoQuadPts.center();
  dynamicBox->algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  // cout << "quad pts " << dynamicBox->algoQuadPts << endl;
  
  // cout << "box " << Box(topLeft, widthHeight) << " local quad " << dynamicBox->algoBlockDrawer.points << endl;

  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_redrock_static_quad(const Point2& pt0, const Point2& pt1,
                                                     const Point2& pt2, const Point2& pt3)
{
  TerrainQuad* staticQuad = new TerrainQuad();
  staticQuad->setXY(pt0);
  staticQuad->topRight = pt3;
  
  staticQuad->init_static_quad(pt0, pt1, pt2, pt3);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->set_b2d_callback_data(staticQuad);
  staticQuad->set_friction(0.5);
  
  staticQuad->algoBlockDrawer.borderImg = getImg(brick9TopImg);
  staticQuad->algoBlockDrawer.centerImg = getImg(brickCenterImg);
  staticQuad->algoBlockDrawer.edgeOffset = 2.0;
  staticQuad->algoBlockDrawer.edgeOffset = 0.5;
  staticQuad->algoBlockDrawer.fillOffset = -8;
  staticQuad->algoBlockDrawer.fillScale = 0.5;
  staticQuad->algoQuadPts = Quad(pt0, pt3, pt2, pt1);
  staticQuad->algoQuadPts -= pt0;
  staticQuad->algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  // cout << "quad pts " << dynamicBox->algoQuadPts << endl;
  
  // cout << "box " << Box(topLeft, widthHeight) << " local quad " << dynamicBox->algoBlockDrawer.points << endl;
  
  staticQuad->setWH(pt2 - pt0);
  
  return staticQuad;
}

TerrainQuad* TerrainQuad::create_redrock_static_quad(const Point2& topLeft, const Point2& botRight)
{
  return create_redrock_static_quad(topLeft, Point2(botRight.x, topLeft.y),
                                    botRight, Point2(topLeft.x, botRight.y));
}

TerrainQuad* TerrainQuad::create_redrock_block(const Point2& pt0, const Point2& pt1,
                                               const Point2& pt2, const Point2& pt3)
{
  TerrainQuad* dynamicQuad = new TerrainQuad();
  dynamicQuad->setXY(pt0);
  dynamicQuad->topRight = pt3;
  
  dynamicQuad->init_quad(pt0, pt1, pt2, pt3, true);
  dynamicQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicQuad->set_b2d_callback_data(dynamicQuad);
  dynamicQuad->set_friction(0.5);
  dynamicQuad->crushesPhysical = true;
  dynamicQuad->crushValue = 650.0;
  
  dynamicQuad->algoBlockDrawer.borderImg = getImg(brick9TopImg);
  dynamicQuad->algoBlockDrawer.centerImg = getImg(brickCenterImg);
  dynamicQuad->algoBlockDrawer.edgeOffset = 2.0;
  dynamicQuad->algoBlockDrawer.edgeOffset = 0.5;
  dynamicQuad->algoBlockDrawer.fillOffset = -8;
  dynamicQuad->algoBlockDrawer.fillScale = 0.5;
  dynamicQuad->algoQuadPts = Quad(pt0, pt3, pt2, pt1);
  dynamicQuad->algoQuadPts -= pt0;
  dynamicQuad->algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  // cout << "quad pts " << dynamicBox->algoQuadPts << endl;
  
  // cout << "box " << Box(topLeft, widthHeight) << " local quad " << dynamicBox->algoBlockDrawer.points << endl;
  
  return dynamicQuad;
}

TerrainQuad* TerrainQuad::create_fading_redrock(Point2 topLeft, Point2 widthHeight)
{
  TerrainQuad* staticQuad = new TerrainQuad();
  staticQuad->setBox(topLeft, widthHeight);
  staticQuad->topRight = topLeft + Point2(widthHeight.x, 0.0);
  staticQuad->setScale(widthHeight.x / 192.0, widthHeight.y / 192.0);
  staticQuad->handledCenter = false;
  staticQuad->setDisintigrate();
  
  staticQuad->init_box_TL(topLeft, widthHeight, false);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->set_b2d_callback_data(staticQuad);
  staticQuad->set_friction(0.5);
  
  return staticQuad;
}


TerrainQuad* TerrainQuad::create_static_quad_themed(const Quad& quad, Coord1 bgIndex)
{
  TerrainQuad* staticQuad = new TerrainQuad();
  staticQuad->setXY(quad.x);
  staticQuad->topRight = quad.w;
  
  staticQuad->init_static_quad(quad.x, quad.y, quad.z, quad.w);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->set_b2d_callback_data(staticQuad);
  staticQuad->set_friction(0.5);
  
  staticQuad->algoBlockDrawer.borderImg = getImg(brick9TopImg);
  staticQuad->algoBlockDrawer.centerImg = getImg(brickCenterImg);
  staticQuad->algoBlockDrawer.edgeOffset = 2.0;
  staticQuad->algoBlockDrawer.edgeOffset = 0.5;
  staticQuad->algoBlockDrawer.fillOffset = -8;
  staticQuad->algoBlockDrawer.fillScale = 0.5;
  staticQuad->algoQuadPts = quad;
  staticQuad->algoQuadPts -= quad.x;
  staticQuad->algoDrawType = TerrainQuad::ALGOBLOCK_QUAD;
  // cout << "quad pts " << dynamicBox->algoQuadPts << endl;
  
  // cout << "box " << Box(topLeft, widthHeight) << " local quad " << dynamicBox->algoBlockDrawer.points << endl;
  
  staticQuad->setWH(quad.z - quad.x);
  
  return staticQuad;
}

TerrainQuad* TerrainQuad::create_halfspike_TL(Point2 topLeft, Point2 widthHeight, Coord1 spikeDir)
{
  TerrainQuad* staticQuad = new TerrainQuad();
  staticQuad->setBox(topLeft + widthHeight * 0.5, widthHeight);
  staticQuad->topRight = topLeft + Point2(widthHeight.x, 0.0);
  staticQuad->setImage(getImg(halfSpikeRight + spikeDir));
  // staticQuad->setScale(4.0, 4.0);
  // staticQuad->setRotation(spikeDir * HALF_PI);
  staticQuad->setSpikeySide(spikeDir);
  staticQuad->setBreakSide(((spikeDir + 2) % 4));

  staticQuad->init_box(topLeft + widthHeight * 0.5, widthHeight, false);
  staticQuad->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticQuad->set_b2d_callback_data(staticQuad);
  staticQuad->set_friction(0.5);
  
  staticQuad->crushesPhysical = true;
  staticQuad->crushValue = 650.0;
  
  return staticQuad;
}

TerrainQuad* TerrainQuad::create_gravestone_BC(Point2 bottomCenter, Image* setImg)
{
  // the graves are actually handled center since the b2d position assumes this
  Point2 sizeMult = Point2(0.85, 0.9);
  Point2 adjustedSize = setImg->natural_size() * sizeMult;
  Box myBox = Box::from_norm(bottomCenter, adjustedSize, HANDLE_BC);
  
  TerrainQuad* staticBox = new TerrainQuad();
  staticBox->setImage(setImg);
  
  staticBox->setWH(std::floor(adjustedSize.x), adjustedSize.y);
  staticBox->setXY(myBox.xy + 0.5 * adjustedSize);
  
  staticBox->init_box_TL(myBox.xy, myBox.wh, false);
  staticBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticBox->crushesPhysical = true;
  staticBox->crushValue = 3250.0;
  staticBox->fixture->SetUserData(staticBox);
  staticBox->set_friction(1.0);
  
  return staticBox;
}

TerrainQuad* TerrainQuad::create_trike_block_TL(Point2 topLeft, Image* setImg)
{
  TerrainQuad* staticBox = new TerrainQuad();
  staticBox->setImage(setImg);
  
  // if the width is fractional then Physical->physTypeID gets set to garbage
  // when cast during the box2d collision. no idea why, could be a compiler bug
  staticBox->setWH(128.0, 128.0);
  staticBox->setXY(topLeft + staticBox->getSize() * 0.5);
  
  staticBox->init_box(staticBox->getXY(), staticBox->getSize(), false);
  staticBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticBox->crushesPhysical = true;
  staticBox->crushValue = 3250.0;
  staticBox->fixture->SetUserData(staticBox);
  staticBox->set_friction(1.0);
  
  return staticBox;
}

TerrainQuad* TerrainQuad::create_donut_C(Point2 center, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setXY(center);
  dynamicBox->setImage(setImg);
  dynamicBox->setWH(192.0, 192.0);  // entire size
  dynamicBox->crushesPhysical = false;
  
  dynamicBox->init_circle(dynamicBox->getXY(), dynamicBox->getWidth() * 0.5, true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(1.0);
  
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_loaf_C(Point2 center, Image* setImg)
{
  DataList<Point2> localLoafPts;
  localLoafPts.add(Point2(111.0, -49.0));
  localLoafPts.add(Point2(157.0, -2.0));
  localLoafPts.add(Point2(157.0, 46.0));
  localLoafPts.add(Point2(-160.0, 45.0));
  localLoafPts.add(Point2(-160.0, -1.0));
  localLoafPts.add(Point2(-113.0, -49.0));
  
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setXY(center);
  dynamicBox->setImage(setImg);
  dynamicBox->setWH(319.0, 96.0);  // entire size
  dynamicBox->crushesPhysical = false;
  
  dynamicBox->init_shape(center, localLoafPts, true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(1.0);
  
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_roll_C(Point2 center, Image* setImg)
{
  TerrainQuad* dynamicBox = new TerrainQuad();
  dynamicBox->setXY(center);
  dynamicBox->setImage(setImg);
  dynamicBox->setWH(96.0, 96.0);  // entire size
  dynamicBox->crushesPhysical = false;
  
  dynamicBox->init_poly(center, 6, 54.0, true);
  dynamicBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  dynamicBox->fixture->SetUserData(dynamicBox);
  dynamicBox->set_friction(1.0);
  
  return dynamicBox;
}

TerrainQuad* TerrainQuad::create_mushroom(Point2 topLeft, Coord1 shroomIndex)
{
  DataList<Point2> localMushroomPts;

  switch (shroomIndex)
  {
    case 0:
      localMushroomPts.add(Point2(128.0, 0.0));
      localMushroomPts.add(Point2(256.0, 0.0));
      localMushroomPts.add(Point2(384.0, 64.0));
      localMushroomPts.add(Point2(384.0, 192.0));
      localMushroomPts.add(Point2(0.0, 192.0));
      localMushroomPts.add(Point2(0.0, 64.0));
      break;
      
    case 1:
      localMushroomPts.add(Point2(128.0, 0.0));
      localMushroomPts.add(Point2(320.0, 0.0));
      localMushroomPts.add(Point2(448.0, 128.0));
      localMushroomPts.add(Point2(448.0, 256.0));
      localMushroomPts.add(Point2(0.0, 256.0));
      localMushroomPts.add(Point2(0.0, 128.0));
      break;
      
    case 2:
      localMushroomPts.add(Point2(64.0, 0.0));
      localMushroomPts.add(Point2(128.0, 0.0));
      localMushroomPts.add(Point2(192.0, 64.0));
      localMushroomPts.add(Point2(192.0, 192.0));
      localMushroomPts.add(Point2(0.0, 192.0));
      localMushroomPts.add(Point2(0.0, 64.0));
      break;
  }
  
  TerrainQuad* mushroom = new TerrainQuad();
  mushroom->setXY(topLeft);
  mushroom->setImage(getImg(breakableObjA + shroomIndex));
  mushroom->setWH(localMushroomPts[3]);  // entire size
  
  mushroom->init_shape(topLeft, localMushroomPts, false);
  mushroom->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  mushroom->fixture->SetUserData(mushroom);
  mushroom->set_friction(1.0);
  
  return mushroom;
}

TerrainQuad* TerrainQuad::create_invis_wall(Point2 topLeft, Point2 botRight)
{
  TerrainQuad* staticBox = new TerrainQuad();
  staticBox->setBox(Box::from_corners(topLeft, botRight));
  
  staticBox->init_box_TL(topLeft, staticBox->getSize(), false);
  staticBox->set_collisions(1 << TYPE_TERRAIN, 0xffff);
  staticBox->fixture->SetUserData(staticBox);
  staticBox->set_friction(1.0);
  return staticBox;
}


// =============================== TerrainGround ============================ //

TerrainGround::TerrainGround(const DataList<Point2>& worldPts) :
  PizzaGOStd(),

  terrainDrawer(),

  isBouncy(false),
  startBounce(false),
  bounceScaleOffset(1.0),
  bouncer(&bounceScaleOffset, 1.1, 0.1, this),
  
  drawOffset(0.0, 0.0),

  worldAABB()
{
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;

  setXY(worldPts.first());
  setWH(worldPts.last().x - worldPts.first().x, 64.0);
  
  init_segmented(worldPts, false);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  fixture->SetUserData(this);
  set_friction(1.0);
  
  Point2 topLeft(10000.0, 10000.0);
  Point2 botRight(-10000.0, -10000.0);
  
  for (Coord1 i = 0; i < worldPts.count; ++i)
  {
    topLeft.x = std::min(worldPts[i].x, topLeft.x);
    topLeft.y = std::min(worldPts[i].y, topLeft.y);
    botRight.x = std::max(worldPts[i].x, botRight.x);
    botRight.y = std::max(worldPts[i].y, botRight.y);
  }
  
  // the 64 is a little extra padding since the image may bleed off the collision
  worldAABB = Box::from_corners(topLeft - 64.0, botRight + 64.0);
}

void TerrainGround::updateMe()
{
  actions.update();
  script.update();
  
  terrainDrawer.color = getColor();
//  updateFromWorld();
  
  checkSurface();
}

void TerrainGround::redrawMe()
{
  // use algorithmic terrain drawer
  // note that if the terrainground DStructs, we'll need to do an override and remove
  //   the check to see if it's onscreen
  if (onRMScreen() == true &&
      terrainDrawer.points.count >= 1 &&
      getAlpha() >= 0.01)
  {
    LightingEnvironment lighting;
    lighting.lights.add(LightSource());
    lighting.lights.last().color = WHITE_SOLID;
    
    terrainDrawer.draw(Point2(0.0, 0.0), getScale(), getRotation(), lighting, false);
    
    // worldAABB.draw_outline(WHITE_SOLID);
  }
  else if (getImage() != NULL)
  {
    drawMe(drawOffset);
    
    if (Pizza::DRAW_DEBUG == true)
    {
      Circle(getXY(), 2.0).draw_solid(BLUE_SOLID);
    }
  }
}

Box TerrainGround::visibleAABB()
{
  return worldAABB;
}

void TerrainGround::setSpikey()
{
  flags |= SPIKED_BIT;
}

void TerrainGround::setLava()
{
  flags |= SPIKED_BIT;
}

void TerrainGround::setBouncy(Point1 newRestitution)
{
  set_restitution(newRestitution);
  isBouncy = true;
}

void TerrainGround::checkSurface()
{
  if (flags == 0) return;
  
  Point2 location;
  
  if (touchingPGO(player, &location) == true)
  {
    playerTouch(location);
  }
  
  /*
   if ((flags & BALANCE_BOUNCE_BIT) != 0)
   {
   BalanceSkull* ball = level->getCurrBall();
   
   if (touchingPGO(ball) == true)
   {
   ball->set_velocity(Point2(ball->get_velocity().x, -128.0));
   }
   }
   */
}

void TerrainGround::playerTouch(Point2 location)
{
  if ((flags & SPIKED_BIT) != 0)
  {
    player->attacked(location, this);
  }
  
  /*
   if ((flags & SLOW_BIT) != 0)
   {
   level->reportSurface(flags);
   }
   
   if ((flags & STICK_BIT) != 0)
   {
   level->reportSurface(flags);
   }
   */
  
  if ((flags & BREAKAWAY_BIT) != 0)
  {
    // level->addAction(new TerrainShatter(*this, 8, 5));
    setRemove();
  }
}

void TerrainGround::setGraveyardGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  //graveyard
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 320/4;
  surfaceUp.leftOffset = 128/4;
  surfaceUp.rightOffset = 128/4;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 7;
  surfaceUp.tiles = Box(0,0,3584/4,448/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 16;
  surfaceRight.leftOffset = 8;
  surfaceRight.rightOffset = 8;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 7;
  surfaceRight.tiles = Box(0,448/4,1792/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 16;
  surfaceDown.leftOffset = 8;
  surfaceDown.rightOffset = 8;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 7;
  surfaceDown.tiles = Box(0,448/4,1792/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 16;
  surfaceLeft.leftOffset = 8;
  surfaceLeft.rightOffset = 8;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 7;
  surfaceLeft.tiles = Box(0,448/4,1792/4,128/4);
  
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = 0;
  terrainDrawer.texScale = 1.0;
  //terrainA.fillResolution = 1024;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts, surfaces);
}
  
void TerrainGround::setJurassicGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 160/4;
  surfaceUp.leftOffset = 16;
  surfaceUp.rightOffset = 16;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 7;
  surfaceUp.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 160/4;
  surfaceRight.leftOffset = 16;
  surfaceRight.rightOffset = 16;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 7;
  surfaceRight.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 160/4;
  surfaceDown.leftOffset = 16;
  surfaceDown.rightOffset = 16;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 7;
  surfaceDown.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 160/4;
  surfaceLeft.leftOffset = 16;
  surfaceLeft.rightOffset = 16;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 7;
  surfaceLeft.tiles = Box(0,0,3584/4,320/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = -8;
  terrainDrawer.texScale = 1.0;
  //terrainA.fillResolution = 1024;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setCakeGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  //graveyard
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 100/4;
  surfaceUp.leftOffset = 64/4;
  surfaceUp.rightOffset = 64/4;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 4;
  surfaceUp.tiles = Box(0,0,1024/4,192/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 16;
  surfaceRight.leftOffset = 4;
  surfaceRight.rightOffset = 4;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 4;
  surfaceRight.tiles = Box(0,192/4,1024/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 16;
  surfaceDown.leftOffset = 4;
  surfaceDown.rightOffset = 4;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 4;
  surfaceDown.tiles = Box(0,192/4,1024/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 16;
  surfaceLeft.leftOffset = 4;
  surfaceLeft.rightOffset = 4;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 4;
  surfaceLeft.tiles = Box(0,192/4,1024/4,128/4);
  
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = -4;
  terrainDrawer.texScale = 1.0;
  //terrainA.fillResolution = 1024;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setDumpGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 128/4;
  surfaceUp.leftOffset = 64/4;
  surfaceUp.rightOffset = 64/4;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 7;
  surfaceUp.tiles = Box(0,0,1792/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 128/4;
  surfaceRight.leftOffset = 64/4;
  surfaceRight.rightOffset = 64/4;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 7;
  surfaceRight.tiles = Box(0,256/4,1792/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 128/4;
  surfaceDown.leftOffset = 64/4;
  surfaceDown.rightOffset = 64/4;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 7;
  surfaceDown.tiles = Box(0,512/4,1792/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 128/4;
  surfaceLeft.leftOffset = 64/4;
  surfaceLeft.rightOffset = 64/4;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 7;
  surfaceLeft.tiles = Box(0,768/4,1792/4,256/4);
  
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = 0;
  terrainDrawer.texScale = 1.0;
  //terrainA.fillResolution = 1024;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setSiliconGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 2;
  surfaceUp.topOffset = 16;
  surfaceUp.leftOffset = 4;
  surfaceUp.rightOffset = 4;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 4;
  surfaceUp.tiles = Box(0,0,2048/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 16;
  surfaceRight.leftOffset = 4;
  surfaceRight.rightOffset = 4;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 4;
  surfaceRight.tiles = Box(0,0,2048/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 16;
  surfaceDown.leftOffset = 4;
  surfaceDown.rightOffset = 4;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 4;
  surfaceDown.tiles = Box(0,0,2048/4,128/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 16;
  surfaceLeft.leftOffset = 4;
  surfaceLeft.rightOffset = 4;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 4;
  surfaceLeft.tiles = Box(0,0,2048/4,128/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = 0;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setBodyGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 96/4;
  surfaceUp.leftOffset = 16;
  surfaceUp.rightOffset = 16;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 4;
  surfaceUp.tiles = Box(0,0,2048/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 16;
  surfaceRight.leftOffset = 8;
  surfaceRight.rightOffset = 8;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 4;
  surfaceRight.tiles = Box(0,320/4,1024/4,192/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 16;
  surfaceDown.leftOffset = 8;
  surfaceDown.rightOffset = 8;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 4;
  surfaceDown.tiles = Box(0,320/4,1024/4,192/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 16;
  surfaceLeft.leftOffset = 8;
  surfaceLeft.rightOffset = 8;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 4;
  surfaceLeft.tiles = Box(0,320/4,1024/4,192/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = 0;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setCatacombsGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 3;
  surfaceUp.topOffset = 8;
  surfaceUp.leftOffset = 8;
  surfaceUp.rightOffset = 8;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 4;
  surfaceUp.tiles = Box(0,0,2048/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 4;
  surfaceRight.leftOffset = 0;
  surfaceRight.rightOffset = 0;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 4;
  surfaceRight.tiles = Box(0,320/4,1024/4,192/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 8;
  surfaceDown.leftOffset = 8;
  surfaceDown.rightOffset = 8;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 4;
  surfaceDown.tiles = Box(0,704/4,1024/4,192/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 4;
  surfaceLeft.leftOffset = 4;
  surfaceLeft.rightOffset = 4;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 4;
  surfaceLeft.tiles = Box(0,512/4,1024/4,192/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = -8;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}


void TerrainGround::setFiremanGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 160/4;
  surfaceUp.leftOffset = 16;
  surfaceUp.rightOffset = 16;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 7;
  surfaceUp.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 160/4;
  surfaceRight.leftOffset = 16;
  surfaceRight.rightOffset = 16;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 7;
  surfaceRight.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 160/4;
  surfaceDown.leftOffset = 16;
  surfaceDown.rightOffset = 16;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 7;
  surfaceDown.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 160/4;
  surfaceLeft.leftOffset = 16;
  surfaceLeft.rightOffset = 16;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 7;
  surfaceLeft.tiles = Box(0,0,3584/4,320/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = -8;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setPuppyGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef surfaceUp(TERRAIN_UP);
  surfaceUp.smooth = 5;
  surfaceUp.topOffset = 128/4;
  surfaceUp.leftOffset = 12;
  surfaceUp.rightOffset = 12;
  surfaceUp.hasCaps = true;
  surfaceUp.img = getImg(terrainBordersImg);
  surfaceUp.numTiles = 7;
  surfaceUp.tiles = Box(0,0,3584/4,320/4);
  
  AlgoTerrainSurfaceDef surfaceRight(TERRAIN_RIGHT);
  surfaceRight.smooth = 1;
  surfaceRight.topOffset = 24;
  surfaceRight.leftOffset = 12;
  surfaceRight.rightOffset = 12;
  surfaceRight.hasCaps = true;
  surfaceRight.img = getImg(terrainBordersImg);
  surfaceRight.numTiles = 7;
  surfaceRight.tiles = Box(0,320/4,1792/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceDown(TERRAIN_DOWN);
  surfaceDown.smooth = 1;
  surfaceDown.topOffset = 24;
  surfaceDown.leftOffset = 12;
  surfaceDown.rightOffset = 12;
  surfaceDown.hasCaps = true;
  surfaceDown.img = getImg(terrainBordersImg);
  surfaceDown.numTiles = 7;
  surfaceDown.tiles = Box(0,320/4,1792/4,256/4);
  
  AlgoTerrainSurfaceDef surfaceLeft(TERRAIN_LEFT);
  surfaceLeft.smooth = 1;
  surfaceLeft.topOffset = 24;
  surfaceLeft.leftOffset = 12;
  surfaceLeft.rightOffset = 12;
  surfaceLeft.hasCaps = true;
  surfaceLeft.img = getImg(terrainBordersImg);
  surfaceLeft.numTiles = 7;
  surfaceLeft.tiles = Box(0,320/4,1792/4,256/4);
  
  surfaces.add(surfaceRight);
  surfaces.add(surfaceLeft);
  surfaces.add(surfaceDown);
  surfaces.add(surfaceUp);
  
  //AlgoTerrain terrainA;
  terrainDrawer.fillOffset = 0;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(terrainFillImg);
  terrainDrawer.init(worldPts,surfaces);
}

void TerrainGround::setSpikeyGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  
  AlgoTerrainSurfaceDef spikeUni(Point2(1,0));
  spikeUni.smooth = 5;
  spikeUni.topOffset = 0;
  spikeUni.leftOffset = 0;
  spikeUni.rightOffset = 0;
  spikeUni.hasCaps = true;
  spikeUni.img = getImg(algoSpikesImg);
  spikeUni.numTiles = 4;
  spikeUni.tiles = Box(0,0,1024/4,512/4);
  surfaces.add(spikeUni);
  
  terrainDrawer.isFilled = false;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.init(worldPts, surfaces);
}


void TerrainGround::setRedrockGround(const DataList<Point2>& worldPts)
{
  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  AlgoTerrainSurfaceDef up(TERRAIN_UP);
  AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
  AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
  AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
  
  //AlgoTerrainSurfaceDef up(TERRAIN_UP);
  up.smooth = 0;
  up.topOffset = 10;
  up.leftOffset = 15;
  up.rightOffset = 15;
  up.hasCaps = true;
  up.img = getImg(redrockTerrainBordersImg);
  up.numTiles = 6;
  up.tiles = Box(0,0,768,48);
  
  //AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
  right.smooth = 0;
  right.rightOffset = 0;
  right.leftOffset = 0;
  right.topOffset = 10;
  right.hasCaps = false;
  right.img = getImg(redrockTerrainBordersImg);
  right.numTiles = 1;
  right.tiles = Box(144,80,48,32);
  
  //AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
  down.smooth = 0;
  down.topOffset = 5;
  down.leftOffset = 15;
  down.rightOffset = 15;
  down.hasCaps = true;
  down.img = getImg(redrockTerrainBordersImg);
  down.numTiles = 4;
  down.tiles = Box(0,48,512,32);
  
  //AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
  left.smooth = 0;
  left.rightOffset = 0;
  left.leftOffset = 0;
  left.topOffset = 10;
  left.hasCaps = false;
  left.img = getImg(redrockTerrainBordersImg);
  left.numTiles = 1;
  left.tiles = Box(8,80,48,40);
  
  ArrayList<AlgoTerrainSurfaceDef> cliffSurfaces;
  cliffSurfaces.add(right);
  cliffSurfaces.add(left);
  cliffSurfaces.add(down);
  cliffSurfaces.add(up);
  
  terrainDrawer.fillOffset = -10;
  terrainDrawer.texScale = 1.0;
  terrainDrawer.fillImg = getImg(redrockCenterImg);
  terrainDrawer.fillResolution = 512.0;
  terrainDrawer.init(worldPts, cliffSurfaces);
}

TerrainGround* TerrainGround::createNoThemeGround(const DataList<Point2>& worldPtsCW, Coord1 bgIndex, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPtsCW;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);

  return ground;
}

TerrainGround* TerrainGround::createThemedGround(const DataList<Point2>& worldPtsCW, Coord1 bgIndex, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPtsCW;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);
  
  switch (bgIndex)
  {
    default:
    case GRAVEYARD_BG: ground->setGraveyardGround(worldPtsCW); break;
    case JURASSIC_BG: ground->setJurassicGround(worldPtsCW); break;
    case CAKE_BG: ground->setCakeGround(worldPtsCW); break;
    case DUMP_BG: ground->setDumpGround(worldPtsCW); break;
    case SILICON_BG: ground->setSiliconGround(worldPtsCW); break;
    case BODY_BG: ground->setBodyGround(worldPtsCW); break;
    case CATACOMBS_BG: ground->setCatacombsGround(worldPtsCW); break;

    case REDROCK_BG: ground->setRedrockGround(worldPtsCW); break;
    case PIZZERIA_BG: ground->setRedrockGround(worldPtsCW); break;
    case PUPPY_BG: ground->setPuppyGround(worldPtsCW); break;
    case FIREMAN_BG: ground->setFiremanGround(worldPtsCW); break;
  }
  
  return ground;
}

TerrainGround* TerrainGround::createThemedGround(const DataList<Point2>& worldPtsCW,
                                                 Coord2 ptIndexRange, Coord1 bgIndex)
{
  DataList<Point2> tempList(worldPtsCW, ptIndexRange.x, ptIndexRange.y);
  return createThemedGround(tempList, bgIndex, true);
}

/*
TerrainGround* TerrainGround::createGraveyardGround(const DataList<Point2>& worldPts, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPts;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);
  
  AlgoTerrainSurfaceDef graveyardUp(TERRAIN_UP);
  graveyardUp.smooth = 5;
  graveyardUp.topOffset = 40;
  graveyardUp.leftOffset = 20;
  graveyardUp.rightOffset = 20;
  graveyardUp.hasCaps = true;
  graveyardUp.img = getImg(terrainBordersImg);
  graveyardUp.numTiles = 7;
  graveyardUp.tiles = Box(0,0,448,152);
  
  AlgoTerrainSurfaceDef graveyardRight(TERRAIN_RIGHT);
  graveyardRight.smooth = 1;
  graveyardRight.rightOffset = 0;
  graveyardRight.leftOffset = 0;
  graveyardRight.topOffset = 16;
  graveyardRight.hasCaps = true;
  graveyardRight.img = getImg(terrainBordersImg);
  graveyardRight.numTiles = 4;
  graveyardRight.tiles = Box(256,152,128,32);
  
  AlgoTerrainSurfaceDef graveyardDown(TERRAIN_DOWN);
  graveyardDown.smooth = 5;
  graveyardDown.topOffset = 24;
  graveyardDown.leftOffset = 16;
  graveyardDown.rightOffset = 16;
  graveyardDown.hasCaps = true;
  graveyardDown.img = getImg(terrainBordersImg);
  graveyardDown.numTiles = 4;
  graveyardDown.tiles = Box(0,152,256,64);
  
  AlgoTerrainSurfaceDef graveyardLeft(TERRAIN_LEFT);
  graveyardLeft.smooth = 1;
  graveyardLeft.rightOffset = 0;
  graveyardLeft.leftOffset = 0;
  graveyardLeft.topOffset = 16;
  graveyardLeft.hasCaps = true;
  graveyardLeft.img = getImg(terrainBordersImg);
  graveyardLeft.numTiles = 4;
  graveyardLeft.tiles = Box(256,152,128,32);
  
  ArrayList<AlgoTerrainSurfaceDef> graveyardSurfaces;
  graveyardSurfaces.add(graveyardRight);
  graveyardSurfaces.add(graveyardLeft);
  graveyardSurfaces.add(graveyardDown);
  graveyardSurfaces.add(graveyardUp);
  
  //AlgoTerrain terrainA;
  ground->terrainDrawer.fillOffset = -10;
  ground->terrainDrawer.texScale = 1.0;
  //ground->terrainDrawer = 1024;
  ground->terrainDrawer.fillImg = getImg(terrainFillImg);
  ground->terrainDrawer.init(worldPts, graveyardSurfaces);

  return ground;
}


TerrainGround* TerrainGround::createStonehengeGround(const DataList<Point2>& worldPts, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPts;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);

  ArrayList<AlgoTerrainSurfaceDef> surfaces;
  AlgoTerrainSurfaceDef up(TERRAIN_UP);
  AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
  AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
  AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
  
  //AlgoTerrainSurfaceDef up(TERRAIN_UP);
  up.smooth = 0;
  up.topOffset = 10;
  up.leftOffset = 15;
  up.rightOffset = 15;
  up.hasCaps = true;
  up.img = getImg(redrockTerrainBordersImg);
  up.numTiles = 6;
  up.tiles = Box(0,0,768,48);
  
  //AlgoTerrainSurfaceDef right(TERRAIN_RIGHT);
  right.smooth = 0;
  right.rightOffset = 0;
  right.leftOffset = 0;
  right.topOffset = 10;
  right.hasCaps = false;
  right.img = getImg(redrockTerrainBordersImg);
  right.numTiles = 1;
  right.tiles = Box(144,80,48,32);
  
  //AlgoTerrainSurfaceDef down(TERRAIN_DOWN);
  down.smooth = 0;
  down.topOffset = 5;
  down.leftOffset = 15;
  down.rightOffset = 15;
  down.hasCaps = true;
  down.img = getImg(redrockTerrainBordersImg);
  down.numTiles = 4;
  down.tiles = Box(0,48,512,32);
  
  //AlgoTerrainSurfaceDef left(TERRAIN_LEFT);
  left.smooth = 0;
  left.rightOffset = 0;
  left.leftOffset = 0;
  left.topOffset = 10;
  left.hasCaps = false;
  left.img = getImg(redrockTerrainBordersImg);
  left.numTiles = 1;
  left.tiles = Box(8,80,48,40);
  
  ArrayList<AlgoTerrainSurfaceDef> cliffSurfaces;
  cliffSurfaces.add(right);
  cliffSurfaces.add(left);
  cliffSurfaces.add(down);
  cliffSurfaces.add(up);
  
  ground->terrainDrawer.fillOffset = -10;
  ground->terrainDrawer.texScale = 1.0;
  ground->terrainDrawer.fillImg = getImg(redrockCenterImg);
  ground->terrainDrawer.fillResolution = 512.0;
  ground->terrainDrawer.init(worldPts, cliffSurfaces);
  
  return ground;
};

TerrainGround* TerrainGround::createTrikeGround(const DataList<Point2>& worldPts, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPts;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);
  
  return ground;
}
 */

TerrainGround* TerrainGround::createSpikeyGround(const DataList<Point2>& worldPts, Logical ensureClosedLoop)
{
  DataList<Point2> groundPts = worldPts;
  
  if (ensureClosedLoop == true &&
      (RM::approxEq(groundPts.first().x, groundPts.last().x, 0.1) == false ||
       RM::approxEq(groundPts.first().y, groundPts.last().y, 0.1) == false))
  {
    groundPts.add(groundPts.first());
  }
  
  TerrainGround* ground = new TerrainGround(groundPts);
  ground->setSpikeyGround(worldPts);
  
  return ground;
};

TerrainGround* TerrainGround::createSpikeyCircle(const Circle& circle)
{
  DataList<Point2> circlePts;
  Point1 subdivisions = 12.0;
  
  for (Coord1 i = 0; i < 12; ++i)
  {
    circlePts.add(circle.xy + RM::pol_to_cart(circle.radius, i * (TWO_PI / subdivisions)));
  }
  
  circlePts.add(circlePts.first());
  
  TerrainGround* spikeCircle = new TerrainGround(circlePts);
  spikeCircle->setWH(circle.radius * 2.0, circle.radius * 2.0);
  spikeCircle->drawOffset.x = -circle.radius;
  spikeCircle->setImage(getImg(circle.radius <= 96 ? spikeball64Img : spikeball128Img));
  spikeCircle->autoScale();
  
  return spikeCircle;
}

// =============================== PizzeriaBuyButton ============================ //

PizzeriaBuyButton::PizzeriaBuyButton() :
  PizzaGOStd(),

  buttonIndex(0),
  pressState(UP_STATE),

  shouldAnimate(false),

  baseFixture(NULL)
{
  phase = PHASE_BG_TERRAIN;
  
  setWH(48.0, 48.0);  // radius
  
  // handle this bottom center
}

void PizzeriaBuyButton::load()
{
  init_circle(getXY(), getWidth(), false);
  set_b2d_callback_data(this);
  
  baseFixture = create_box_fixture(body, Point2(getWidth() * 2.0, 24.0));
  set_collisions(baseFixture, 0x1 << TYPE_TERRAIN, 0x1 << TYPE_PLAYER);
}

void PizzeriaBuyButton::updateMe()
{
  if (shouldAnimate == true)
  {
    updateSpineAnim();
  }
}

void PizzeriaBuyButton::redrawMe()
{
  renderSpineAnim();
  // drawMe();
}

void PizzeriaBuyButton::pizzaSlammed()
{
  if (pressState != UP_STATE)
  {
    return;
  }
  
  Circle detectionCircle = Circle(getXY(), getWidth());
  detectionCircle.radius += 4.0;
  
  if (player->collisionCircle().collision(detectionCircle) == true)
  {
    level->shopButtonPressed(buttonIndex);
  }
}

void PizzeriaBuyButton::setState(Coord1 newState)
{
  pressState = newState;
  
  if (pressState == UP_STATE)
  {
    initSpineAnim(shopButtonSpine, "up", false);
    // setImage(getImgSet(shopButtonImgs)[0]);
    set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER));
    shouldAnimate = false;
  }
  else if (pressState == DISABLE_STATE)
  {
    initSpineAnim(shopButtonSpine, "disable", false);
    // setImage(getImgSet(shopButtonImgs)[2]);
    set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER));
    shouldAnimate = true;
  }
  else if (pressState == DOWN_STATE)
  {
    initSpineAnim(shopButtonSpine, "up", false);
    // setImage(getImgSet(shopButtonImgs)[1]);
    set_collisions(0x1 << TYPE_TERRAIN, 0x0);
    shouldAnimate = true;
  }
}

// =============================== MotionBomb ============================ //

MotionBomb::MotionBomb() :
  PizzaGOStd(),

  stableTimer(1.0),
  fuseTimer(1.0, this)
{
  setWH(36.0, 36.0);  // radius
  
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  fuseTimer.setInactive();
}

void MotionBomb::load()
{
  initSpineAnim(bombSpine, "fuse", false);
  
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << type, 0xffff);
  
  updateSpineAnim(0.0);  // get the AABB in place without animating
  updateFromPhysical();
  updateSpineAnim(0.0);  // not sure why i have to call this in both places, but i do
}

void MotionBomb::updateMe()
{
  stableTimer.update();
  
  // the reason it's a velocity is because structures that fit together can be slightly unstable
  // and will "settle" apart, causing a slight angular/linear velocity
  if (stableTimer.getActive() == false &&
      RM::magnitude(get_velocity()) >= 15.0 &&
      fuseTimer.getActive() == false)
  {
    fuseTimer.reset();
  }
  
  if (fuseTimer.getActive() == true)
  {
    fuseTimer.update();
    updateSpineAnim();
  }
  
  PizzaGOStd::updateMe();
  updateFromPhysical();
}

void MotionBomb::redrawMe()
{
  PizzaGOStd::redrawMe();
}

void MotionBomb::callback(ActionEvent* caller)
{
  if (caller == &fuseTimer)
  {
    setRemove();
    
    BombExplosion* explode = new BombExplosion();
    explode->setXY(getXY());
    level->addPGO(explode);
  }
}

// =============================== WaterArea ============================ //

WaterArea::WaterArea(const DataList<Point2>& setWorldPts) :
  PizzaGOStd(),

  worldPts(setWorldPts),
  myAABB(),

  simBox()
{
  phase = PHASE_FG_WATER;
  type = TYPE_TERRAIN;

  setXY(worldPts.first());
  myAABB = Box(worldPts.first(), Point2(1.0, 1.0));
  
  DataList<Point2> localPts(worldPts.count);
  
  for (Coord1 i = 0; i < worldPts.count; ++i)
  {
    localPts.add(worldPts[i] - getXY());
    
    myAABB.set_right(std::max(worldPts[i].x, myAABB.right()));
    myAABB.set_bottom(std::max(worldPts[i].y, myAABB.bottom()));
    myAABB.set_left(std::min(worldPts[i].x, myAABB.left()));
    myAABB.set_top(std::min(worldPts[i].y, myAABB.top()));
  }
  
  init_shape(getXY(), localPts, false);
  set_collisions(1 << TYPE_TERRAIN, 0x1 << TYPE_PLAYER);
  fixture->SetUserData(this);
  fixture->SetSensor(true);
}

void WaterArea::load()
{
  simBox.setBox(myAABB);
  simBox.setImage(getImg(waterSimBodyImg));
  simBox.highlightImg = getImg(waterSimHighlightImg);
  simBox.init(simBox.getWidth() / 32.0);
}

void WaterArea::updateMe()
{
  if (touchingPGO(player) == true)
  {
    // cout << "touch player" << endl;
    player->reportInWater();
    
    checkDisturb(player->collisionBox());
  }
  else
  {
    // cout << "NOPE" << endl;
  }
  
  simBox.update();
}

void WaterArea::checkDisturb(Box playerBox)
{
  Box surfaceBox = myAABB;
  surfaceBox.set_bottom(surfaceBox.bottom() + 1.0);
  
  if (playerBox.collision(surfaceBox) == false)
  {
    return;
  }
  
  simBox.disturb(player->getX() - surfaceBox.left(), 4.0,
                 (player->get_velocity().y / PLAYER_JUMP_VEL) * 6.0);
}

void WaterArea::redrawMe()
{
  if (onRMScreen() == true)
  {
    simBox.redraw();
  }
  
  if (Pizza::DRAW_DEBUG)
  {
    RM::draw_convex_poly(worldPts, ColorP4(0.0, 0.0, 1.0, 0.25), true);
    myAABB.draw_outline(CYAN_SOLID);
  }
}

// =============================== Decoration ============================ //

Decoration::Decoration() :
  PizzaGOStd()
{
  
}

Decoration::Decoration(Point2 position, Image* setImg) :
  PizzaGOStd()
{
  setImage(setImg);
  setXY(position);
}

void Decoration::redrawMe()
{
  if (onRMScreen() == true)
  {
    drawMe();
  }
}

// =============================== LavaPatch ============================ //

LavaPatch::LavaPatch(Point2 topLeft, Point2 setSize) :
  PizzaGOStd()
{
  setWH(setSize);  // size is full size
  setXY(topLeft);
  
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;
}

LavaPatch::LavaPatch(const Box& box) :
  PizzaGOStd()
{
  setBox(box);
  
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;
}

void LavaPatch::load()
{
  init_box_TL(getXY(), getSize(), false);
  fixture->SetSensor(true);
  
  set_b2d_callback_data(this);
  set_collisions(0x1 << TYPE_TERRAIN, (0x1 << TYPE_PLAYER) | (0x1 << TYPE_SUMO_BOSS));
}

void LavaPatch::updateMe()
{
  Point2 touchLocation;
  
  if (touchingPGO(player, &touchLocation) == true)
  {
    player->touchedFire(player->collisionBox().norm_pos(HANDLE_BC), this);
  }
  
  if (touchingPGO(level->boss, &touchLocation) == true)
  {
    level->boss->touchedFire(level->boss->collisionBox().norm_pos(HANDLE_BC), this);
  }
}

void LavaPatch::redrawMe()
{
  collisionBox().draw_solid(ColorP4(1.0, 0.0, 0.0, 0.5));
}

// =============================== BoneBreakEffect ============================ //

Coord1 BoneBreakEffect::CURR_BREAK_IMG_INDEX = 2;

BoneBreakEffect::BoneBreakEffect(const VisRectangular& visRect, Coord1 cols, Coord1 rows) :
  PhysicalPolyParticles(),
  ActionEvent(),
  ActionListener(),

  solidTimer(1.5, this),
  fadeTimer(0.5, this)
{
  triDmgImgs.add(getImgSet(dmgMultiTexSet)[CURR_BREAK_IMG_INDEX]);
  quadDmgImgs.add(getImgSet(dmgMultiTexSet)[CURR_BREAK_IMG_INDEX + 1]);

//  cout << "INDEX " << CURR_BREAK_IMG_INDEX << endl;
//  CURR_BREAK_IMG_INDEX += 2;
//  CURR_BREAK_IMG_INDEX %= 4;

  collisionPad = Point2(0.4, 0.4);
  create(visRect, cols, rows);

  setCollisions(1 << TYPE_DEBRIS, 0xffff);
  setRestitutions(0.0);
  fadeTimer.setActive(false);
}

void BoneBreakEffect::updateMe()
{
  updateParticles();
  
  solidTimer.update();
  fadeTimer.update();
}

void BoneBreakEffect::redrawMe()
{
  startingVR.setAlpha(1.0 - fadeTimer.progress());
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

// =============================== JengaPuffball ============================ //

JengaPuffball::JengaPuffball() :
  PizzaGOStd(),

  startXY(0.0, 0.0)
{
  setWH(36.0, 36.0);  // radius
  
  phase = PHASE_ALLY_STD;
  type = TYPE_PUFFBALL;
}

void JengaPuffball::load()
{
  startXY = getXY();
  
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << type, (0x1 << TYPE_TERRAIN) | (0x1 << TYPE_DEBRIS));
}

void JengaPuffball::updateMe()
{
  // the reason it's a velocity is because structures that fit together can be slightly unstable
  // and will "settle" apart, causing a slight angular/linear velocitydd
  if (level->levelState == LEVEL_PLAY &&
      RM::magnitude(get_velocity()) >= 5.0)
  {
    level->loseLevel();
    cout << "lose" << endl;
  }
  else if (level->levelState == LEVEL_PLAY &&
           collisionCircle().collision(player->collisionCircle()) == true)
  {
    level->winLevel();
    cout << "win" << endl;
  }
  
  PizzaGOStd::updateMe();
  updateFromPhysical();
}

void JengaPuffball::redrawMe()
{
  PizzaGOStd::redrawMe();
}

// =============================== JengaBomb ============================ //

JengaBomb::JengaBomb() :
  PizzaGOStd()
{
  setWH(36.0, 36.0);  // radius
  
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
}

void JengaBomb::load()
{
  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << type, 0xffff);
  fixture->SetUserData(this);
}

void JengaBomb::updateMe()
{
  updateFromPhysical();
}

void JengaBomb::redrawMe()
{
  PizzaGOStd::redrawMe();
}

void JengaBomb::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (level->levelState == LEVEL_PLAY &&
      pgo == player)
  {
    cout << "lose" << endl;
    level->loseLevel();
  }
}

// =============================== AngrySkull ============================ //

AngrySkull::AngrySkull() :
  PizzaGOStd(),

  switchAnimCheckTimer(-0.1)
{
  setWH(ANGRY_SKULL_RAD, ANGRY_SKULL_RAD);  // radius
  
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  value = AN_ENEMY_SKULL;
}

void AngrySkull::load()
{
  level->skullsCreated++;

  init_circle(getXY(), getWidth(), true);
  set_collisions(0x1 << type, 0xffff);
  set_friction(0.3);  // any lower and you can't stack skulls
  fixture->SetUserData(this);
  
  Coord1 spineIndex = RM::randi(angrySkullA, angrySkullC);
  
  if (RM::randi(0, 7) == 0)
  {
    spineIndex = RM::randi(angrySkullFunA, angrySkullFunC);
  }
  
  initSpineAnim(spineIndex, "idle");
  
  crushesPhysical = true;
  crushValue = 500.0;
}

void AngrySkull::updateMe()
{
  checkScared();
  
  updateSpineAnim();
  updateFromPhysical();
  checkPhysicalDestruction();
  
  if (getActive() == true &&
      getY() > level->getCameraGroundY(Point2(getX() - 1.0, getX() + 1.0)))
  {
    destroyPhysical();
  }
}

void AngrySkull::redrawMe()
{
  PizzaGOStd::redrawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_solid(getColor());
  }
}

void AngrySkull::checkScared()
{
  switchAnimCheckTimer -= RM::timePassed();

  if (switchAnimCheckTimer >= 0.0)
  {
    return;
  }
  
  Box underPlayerBox = player->collisionBox();
  underPlayerBox.grow_down(512.0);
  
  Logical shouldBeScared = underPlayerBox.collision(collisionBox());
  
  if (scared != shouldBeScared)
  {
    scared = shouldBeScared;
    switchAnimCheckTimer = 1.0;
    
    mySpineAnim.startAnimation(scared ? "scared" : "idle", 0, true);
  }
}

void AngrySkull::destructionStarted()
{
  level->skullsDestroyed++;
}

Box AngrySkull::getLocalDrawAABB()
{
  Box startBox = collisionBox();
  startBox.xy -= getXY();
  return startBox;
}

//void AngrySkull::collidedPhysical(PizzaGOStd* pgo, Point1 normalImpulse)
//{
//
//}



// =============================== PuppyCage ============================ //

PuppyCage::PuppyCage() :
  PizzaGOStd(),

  impactTimer(0.1),
  damage(0),
  puppy(NULL)
{
  setImage(getImgSet(pupCageSet)[0]);
  // autoSize();
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  value = PU_ENEMY_CAGE;
  
  setWH(112.0, 120.0);  // entire size
  impactTimer.setActive(false);
}

void PuppyCage::load()
{
  addXY(getWidth() * -0.5, getHeight() * -1.0);
  
  init_box_TL(getXY(), getSize(), false);
  fixture->SetUserData(this);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
}

void PuppyCage::updateMe()
{
  impactTimer.update();
  
  if (getActive() == true && damage >= 3)
  {
    destroyPhysical();
    puppy->cageBroken();
  }
}

void PuppyCage::redrawMe()
{
  Coord1 baseImgIndex = std::min(damage, 2) * 2;

  setImage(getImgSet(pupCageSet)[baseImgIndex + 1]);
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionBox().draw_outline(WHITE_SOLID);
  }
}

void PuppyCage::drawBack()
{
  Coord1 baseImgIndex = std::min(damage, 2) * 2;

  setImage(getImgSet(pupCageSet)[baseImgIndex]);
  PizzaGOStd::redrawMe();
}

void PuppyCage::tryDamageCage()
{
  if (lifeState != RM::ACTIVE) return;
  
  impactTimer.reset();
  damage++;
}

void PuppyCage::drawDStruct()
{
  Coord1 baseImgIndex = std::min(damage, 2) * 2;
  
  setImage(getImgSet(pupCageSet)[baseImgIndex]);
  drawMe();
  
  setImage(getImgSet(pupCageSet)[baseImgIndex + 1]);
  drawMe();
}

void PuppyCage::strikeWithLightning()
{
  damage += 5;
}

void PuppyCage::explosionOnscreen(const Circle& explosion)
{
  if (collisionCircle().collision(explosion) == true)
  {
    tryDamageCage();
  }
}

void PuppyCage::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (normalImpulse < 800.0) return;
  if (impactTimer.getActive() == true) return;
  
  tryDamageCage();
}

// =============================== Billy ============================ //

const Point1 Billy::HOP_GRAVITY = 500.0;

Billy::Billy() :
  PizzaGOStd(),

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
  phase = PHASE_PUPPY;
  type = TYPE_PUPPY;
  aiState = WAIT_FOR_BREAK;
}

void Billy::load()
{
  level->puppyList.add(this);
  level->puppiesToFind++;
  level->puppiesToReturn++;
  level->puppiesTotal++;

  setWH(72.0, 72.0);  // radius

  initSpineAnim(puppiesSpine, "caged");

  Coord1 currPupp = (level->puppiesToFind - 1) % 3;

  if (currPupp == 0)
  {
    mySpineAnim.setSkin("Nass");
  }
  else if (currPupp == 1)
  {
    mySpineAnim.setSkin("Darcy");
  }
  else
  {
    mySpineAnim.setSkin("Geordi");
  }

  wagOnTimer.setInactive();
  barkOnTimer.setInactive();
}

void Billy::updateMe()
{
  updateAI();
}

void Billy::redrawMe()
{
  PizzaGOStd::redrawMe();

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(GREEN_SOLID);
  }
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
      
      facePlayer();
      break;
      
    case WAIT_FOR_PLAYER:
      turnTimer.update();
      barkOnTimer.update();
      barkOffTimer.update();
      
      facePlayer();

      if (Circle::collision(collisionCircle(), player->collisionCircle()) == true)
      {
        stickTargetOffset = Point2(RM::randf(-64.0, 64.0), RM::randf(-64.0, 64.0));
        currSpeed = 400.0;
        // ResourceManager::playBark();
        aiState = JUMP_FOR_PLAYER;
        mySpineAnim.startAnimation("jump", 0, true);
      }
      break;
      
    case JUMP_FOR_PLAYER:
    {
      // jumpAnim.update();
      currSpeed += 1000.0 * RM::timePassed();
      Point2 playerCenter = player->collisionCircle().xy;
      
      Logical hitTarget = RM::attract_me(xy, playerCenter + stickTargetOffset, currSpeed * RM::timePassed());
      
      if (hitTarget == true)
      {
        tackBehavior.reset(player);
        mySpineAnim.startAnimation("riding", 0, true);
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
      // setRotation(RM::chase_angle(getRotation(), 0.0, 2.0 * RM::timePassed()));
      
      hflip(getX() >= level->pranceStartPt().x);
      RM::flatten_me(xy.x, level->pranceStartPt().x, xHopSpeed * RM::timePassed());
      
      currSpeed += HOP_GRAVITY * RM::timePassed();
      addY(currSpeed * RM::timePassed());
      
      Logical landed = RM::clamp_me(xy.y, -1000.0, level->pranceStartPt().y) != 0;
      
      if (landed == true)
      {
        aiState = GO_TO_HOUSE;
        mySpineAnim.startAnimation("run", 0, true);
      }
      
      break;
    }
      
    case GO_TO_HOUSE:
    {
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
  
  updateSpineAnim();
}

void Billy::cageBroken()
{
  aiState = WAIT_FOR_PLAYER;
}

void Billy::puppyJumpHome()
{
  aiState = JUMP_OFF_PLAYER;
  currSpeed = -100.0;
  
  mySpineAnim.startAnimation("jump", 0, true);

  Point1 varA = HOP_GRAVITY;
  Point1 varB = currSpeed;
  Point1 varC = getY() - level->pranceStartPt().y;
  timeForHop = (-varB + std::sqrt(varB * varB - 4 * varA * varC)) / 2 * varA;
  
  xHopSpeed = std::abs(getX() - level->pranceStartPt().x);
  
  // ResourceManager::playBark();
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
    // toggleHFlip();
    // turnTimer.reset();
  }
  else if (caller == &barkOnTimer)
  {
    // barkOffTimer.reset(RM::randf(1.0, 2.0));
    // setImage(fullImgSet->get(0));
  }
  else if (caller == &barkOffTimer)
  {
    // barkOnTimer.reset();
    // setImage(fullImgSet->get(1));
    // if (onScreen() == true) ResourceManager::playBark();
  }
  else if (caller == &wagOnTimer)
  {
    // wagOffTimer.reset(RM::randf(1.0, 2.0));
    // setImage(fullImgSet->get(0));
  }
  else if (caller == &wagOffTimer)
  {
    // wagOnTimer.reset();
    // setImage(fullImgSet->get(2));
  }
}

// =============================== SprintFlag ============================ //

SprintFlag::SprintFlag() :
  PizzaGOStd(),

  touched(false)
{
  // it's an "enemy" for the camera
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  setWH(16.0, 16.0);
  initSpineAnim(sprintFlagSpine, "idle");
}

void SprintFlag::updateMe()
{
  if (touched == false &&
      player->collisionBox().right() >= getX())
  {
    touched = true;
    mySpineAnim.startAnimation("wave", 0, true);
  }
  
  updateSpineAnim();
}

void SprintFlag::redrawMe()
{
  PizzaGOStd::redrawMe();
}

Circle SprintFlag::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

Box SprintFlag::visibleAABB()
{
  return Box::from_norm(getXY(), Point2(256.0, 512.0), HANDLE_BC);
}

// =============================== ShopChef ============================ //

ShopChef::ShopChef() :
  PizzaGOStd()
{
  // setImage(imgsetCache[puppyCage][0]);
  // autoSize();
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;
  
  setWH(28.0, 84.0);  // entire size
}

void ShopChef::load()
{
  initSpineAnim(bgObject1, "idle");

  addXY(getWidth() * -0.5, getHeight() * -0.85);
  init_box_TL(getXY(), getSize(), false);
  fixture->SetUserData(this);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  addXY(getWidth() * 0.5, getHeight() * 0.85);
}

void ShopChef::updateMe()
{
  script.update();
  
  updateSpineAnim();
}

void ShopChef::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG)
  {
    collisionBox().draw_outline(WHITE_SOLID);
  }
}

void ShopChef::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (pgo == player)
  {
    startClap();
  }
}

void ShopChef::startClap()
{
  script.clear();
  
  mySpineAnim.startAnimation("clap", 0);
  script.wait(1.0);
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle", 0, true));
}


// =============================== PirateShip ============================ //

PirateShip::PirateShip() :
  PizzaGOStd(),

  frontAnim(),

  startXY(0.0, 0.0)
  // bobData(0.0, 0.0, 1.0)
{
  phase = PHASE_BG_TERRAIN;
  type = TYPE_TERRAIN;
}

void PirateShip::load()
{
  DataList<Point2> localPts;
  localPts.add(Point2(0.0, -24.0));
  localPts.add(Point2(-352.0, -24.0));
  localPts.add(Point2(-304.0, 64.0));
  localPts.add(Point2(272.0, 72.0));
  localPts.add(Point2(272.0, -24.0));

  init_shape(getXY(), localPts, true);
  // set_body_type(b2_kinematicBody);
  fixture->SetUserData(this);
  body->SetFixedRotation(true);
  change_density(10.0);
  body->SetLinearDamping(0.5);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  
  initSpineAnim(pirateShipSpine, "idle_back");
  initSpineAnim(frontAnim, pirateShipSpine, "idle_front", true);

  startXY = getXY();
}

void PirateShip::updateMe()
{
  // RM::bounce_arcsine(bobData, Point2(-8.0, 8.0), 0.5 * RM::timePassed());

  updateSpineAnim();
  updateSpineAnim(frontAnim);
  
  // setY(startXY.y + bobData.x);
  
  updateFromPhysical();
  setX(startXY.x);
  
  updateFromWorld();
  
  Point1 belowEq = getY() - startXY.y;
  
  if (belowEq > 0.0)
  {
    apply_force(Point2(0.0, -10000.0 * belowEq / 20.0));
  }
}

void PirateShip::redrawMe()
{
  PizzaGOStd::redrawMe();
}

Box PirateShip::visibleAABB()
{
  return Box::from_center(getXY(), RM_WH);
}

// =============================== PirateChest ============================ //

PirateChest::PirateChest() :
  PizzaGOStd(),

  ship(NULL),
  offsetXY(0.0, 0.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  setWH(128.0, 128.0);
}

void PirateChest::load()
{
  init_box(getXY(), getSize(), false);
  // set_body_type(b2_kinematicBody);
  fixture->SetUserData(this);
  // body->SetFixedRotation(true);
  set_collisions(1 << TYPE_PICKUP, 0x1 << TYPE_PLAYER);
  
  initSpineAnim(pirateChestSpine, "idle_closed");
  mySpineAnim.setDefaultMixDuration(0.0);

  offsetXY = getXY() - ship->getXY();
}

void PirateChest::updateMe()
{
  setXY(ship->getXY() + offsetXY);
  updateFromWorld();
  updateSpineAnim();
}

void PirateChest::redrawMe()
{
  addY(getHeight() * 0.5);
  PizzaGOStd::redrawMe();
  addY(-getHeight() * 0.5);
}

Box PirateChest::visibleAABB()
{
  return boxFromC();
}

// =============================== BalloonBasket ============================ //

BalloonBasket::BalloonBasket() :
  PizzaGOStd(),

  impactTimer(1.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  value = FI_BALLOON_BASKET;

  initSpineAnim(balloonBasketSpine, "idle");

  setWH(162.0, 134.0);  // entire size
}

void BalloonBasket::load()
{
  init_box_TL(getXY(), getSize(), false);
  fixture->SetUserData(this);
  set_collisions(1 << TYPE_TERRAIN, 0xffff);
  
}

void BalloonBasket::updateMe()
{
  impactTimer.update();
  
  updateSpineAnim();
}

void BalloonBasket::redrawMe()
{
  PizzaGOStd::redrawMe();
}


void BalloonBasket::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (impactTimer.getActive() == true) return;
  
  if (pgo == player)
  {
    impactTimer.reset();
    
    Coord1 currNumPlayerBalloons = player->balloonList.count;
    
    for (Coord1 i = currNumPlayerBalloons; i < level->maxPlayerBalloons(); ++i)
    {
      FiremanBalloon* balloon = new FiremanBalloon();
      balloon->setXY(collisionBox().center());
      balloon->addXY(RM::randf(-64.0, 64.0), RM::randf(-64.0, 64.0));
      level->addPGO(balloon);

      player->balloonList.add(balloon);
    }
  }
}

Box BalloonBasket::visibleAABB()
{
  return boxFromTL();
}

// =============================== FiremanBalloon ============================ //

FiremanBalloon::FiremanBalloon() :
  PizzaGOStd(),

  tackBehavior(this),

  aiState(FLOAT_TO_PLAYER),
  stickTargetOffset(0.0, 0.0),

  xTowardPlayer(&xy.x, 0.0, 1.0),
  yTowardPlayer(&xy.y, 0.0, 1.0),

  rotateSpeed(0.0),

  yTowardObject(),
  xTowardObject(&xy.x, 0.0, 1.0)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_BALLOON;
  
  setWH(48.0, 48.0);  // radius
  
  stickTargetOffset = Point2(RM::randf(-128.0, 128.0), RM::randf(-128.0, 128.0));

  initSpineAnim(balloonSpine, "idle");
  
  switch (RM::randi(0, 4))
  {
    default:
    case 0: mySpineAnim.setSkin("blue"); break;
    case 1: mySpineAnim.setSkin("green"); break;
    case 2: mySpineAnim.setSkin("orange"); break;
    case 3: mySpineAnim.setSkin("pink"); break;
    case 4: mySpineAnim.setSkin("red"); break;
  }
  
  
  Coord1 skipAhead = RM::randi(0, 55);
  
  for (Coord1 i = 0; i < skipAhead; ++i)
  {
    updateSpineAnim();
  }
}

void FiremanBalloon::load()
{
  Point2 target = player->getXY() + stickTargetOffset;
  Point1 duration = RM::randf(0.75, 1.5);
  
  xTowardPlayer.reset(target.x, duration);
  yTowardPlayer.reset(target.y, duration);
  
  rotateSpeed = TWO_PI * RM::randf(1.0, 4.0) * RM::rand_sign();
}

void FiremanBalloon::updateMe()
{
  updateSpineAnim();
  updateAI();
}

void FiremanBalloon::redrawMe()
{
  PizzaGOStd::redrawMe();
}


Box FiremanBalloon::visibleAABB()
{
  return boxFromC();
}

void FiremanBalloon::updateAI()
{
  switch (aiState)
  {
    default:
    case FLOAT_TO_PLAYER:
    {
      Point2 target = player->getXY() + stickTargetOffset;

      xTowardPlayer.endVal = target.x;
      yTowardPlayer.endVal = target.y;

      addRotation(rotateSpeed * RM::timePassed());
      
      xTowardPlayer.update();
      yTowardPlayer.update();

      Logical hitTarget = yTowardPlayer.getActive() == false;
      
      if (hitTarget == true)
      {
        xTowardPlayer.setInactive();
        yTowardPlayer.setInactive();
        
        tackBehavior.reset(player);
        // setImage(fullImgSet->get(0));
        // level->puppyFound(this);
        aiState = ON_PLAYER;
      }
      break;
    }
      
    case ON_PLAYER:
      // wait for player to tell me to fly off
      tackBehavior.update();
      // setRotation(0.0);
      break;
      
    case FLY_AWAY:
    {
      
      break;
    }
      
    case JUMP_TO_OBJECT:
    {
      xTowardObject.update();
      yTowardObject.update();
      
      addRotation(rotateSpeed * RM::timePassed());
      
      Logical arrived = yTowardObject.getActive() == false;
      
      if (arrived == true)
      {
        ImageEffect* effect = new ImageEffect(getImg(splashImg));
        effect->matchScales = true;
        effect->setXY(getXY());
        effect->setScale(0.1, 0.1);
        effect->addX(new LinearFn(&effect->scale.x, 1.0, 0.25));
        effect->addX(new LinearFn(&effect->color.w, 0.0, 0.25));
        level->addAction(effect);
        
        myBurningObj->quenchBurning();
        
        aiState = EXPLODED;
        setRemove();
      }
      
      break;
    }
      
    case EXPLODED:
      break;
  }
}

void FiremanBalloon::seekObject(TerrainQuad* obj)
{
  aiState = JUMP_TO_OBJECT;
  obj->lifeState = RM::PASSIVE;  // this is for the camera
  myBurningObj = obj;
  
  Point2 target = myBurningObj->collisionBox().center();
  
  Point1 duration = 0.75;
  xTowardObject.reset(target.x, duration);
  yTowardObject.enqueueX(new DeAccelFn(&xy.y, getY() - 200.0, duration * 0.4));
  yTowardObject.enqueueX(new AccelFn(&xy.y, target.y, duration * 0.6));
}

void FiremanBalloon::flyAway()
{
  ImageEffect* effect = new ImageEffect(getImg(splashImg));
  effect->matchScales = true;
  effect->setXY(getXY());
  effect->setScale(0.1, 0.1);
  effect->addX(new LinearFn(&effect->scale.x, 1.0, 0.25));
  effect->addX(new LinearFn(&effect->color.w, 0.0, 0.25));
  level->addAction(effect);
  
  aiState = EXPLODED;
  setRemove();
}

Circle FiremanBalloon::collisionCircle()
{
  return Circle(getXY() - Point2(0.0, getHeight() * 0.5), getWidth() * 0.5);
}

void FiremanBalloon::callback(ActionEvent* caller)
{

}

// =============================== BossShroom ============================ //

BossAttachment::BossAttachment() :
  parent(NULL),
  fixture(NULL),
  img(NULL),
  bossOffset(0.0, 0.0),
  touched(false)
{
  
}

void BossAttachment::redraw()
{
  if (touched == true)
  {
    return;
  }
  
  if (img != NULL)
  {
    img->draw_scale(calcWorldXY(), Point2(1.0, 1.0), calcWorldRotation(), parent->getColor());
  }
}

Point2 BossAttachment::calcWorldXY()
{
  return parent->getXY() + RM::rotate(bossOffset, parent->getRotation());
}

Point1 BossAttachment::calcWorldRotation()
{
  return parent->getRotation() + RM::angle(bossOffset);
}

// =============================== BossSpore ============================ //

BossSpore::BossSpore() :
  PizzaGOStd(),

  vel(0.0, 0.0),
  accel(0.0, 0.0)
{
  setWH(32.0, 32.0);  // radius
  setImage(getImg(breakableObjD));
  
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
}

void BossSpore::load()
{
  vel = Point2(RM::randf(-128.0, 128.0), -256.0);
  accel = Point2(0.0, 128.0);
}

void BossSpore::updateMe()
{
  if (getActive() == false)
  {
    return;
  }
  
  // can't do this on spawn because it happens during a collision while the b2d world is locked
  if (body == NULL)
  {
    init_circle(getXY(), 8.0, true);  // has to be dynamic or it won't collide with static terrain
    set_collisions(0x1 << TYPE_ENEMY, 0x1 << TYPE_TERRAIN);  // only collide with mushrooms
    fixture->SetSensor(true);
    fixture->SetUserData(this);
  }
  
  vel += accel * RM::timePassed();
  xy += vel * RM::timePassed();
  updateFromWorld();
  
  updateCollisions();
  
  if (getY() > level->worldBox.bottom() + 128.0)
  {
    cout << "remove out of bounds " << endl;
    setRemove();
  }
}

void BossSpore::redrawMe()
{
  drawMe();
}

void BossSpore::updateCollisions()
{
  Coord1 shroomIndex = 0;
  
  for (LinkedListNode<PizzaGOStd*>* terrainNode = level->objman.phaseList[PHASE_BG_TERRAIN].first;
       terrainNode != NULL;
       terrainNode = terrainNode->next)
  {
    PizzaGOStd* mushroom = terrainNode->data;

    if (touchingPGO(mushroom) == true)
    {
      Point2 saberBasePt = getXY() + Point2(0.0, 8.0);
      Point2 saberSize(192.0, 8.0);
      Point1 saberAngle = PI * 1.25 + RM::randf(0.0, HALF_PI);
      Point2 saberCenter = saberBasePt + RM::pol_to_cart(saberSize.x * 0.5, saberAngle);
      
      mushroom->lightsaberFixtures.add(mushroom->create_box_fixture_center(mushroom->body, saberCenter - mushroom->getXY(),
          saberSize, saberAngle));
      mushroom->lightsaberFixtures.first()->SetUserData(mushroom);
      mushroom->set_collisions(mushroom->lightsaberFixtures.first(), 1 << TYPE_BOSS_LIGHTSABER,
                               (0x1 << TYPE_PLAYER) | (0x1 << TYPE_PLAYER_LIGHTSABER));
      
      Decoration* saberDecoration = new Decoration(saberBasePt, getImg(breakableObjE));
      saberDecoration->setRotation(saberAngle);
      level->addAction(new PointLockBehavior(saberDecoration, mushroom));
      level->addPGO(saberDecoration);

      setRemove();
      return;
    }
  }
}

// =============================== SporeSaber ============================ //

SporeSaber::SporeSaber() :
  PizzaGOStd()
{
  
}

void SporeSaber::load()
{
  
}

void SporeSaber::updateMe()
{
  
}

void SporeSaber::redrawMe()
{
  
}

void SporeSaber::updateCollisions()
{
  
}

// =============================== SumoBoss ============================ //

SumoBoss::SumoBoss() :
  PizzaGOStd(),

  ai(NULL),
  geometry(CIRCLE),

  coinsLeft(10),
  coinStyle(DROP_STD),
  justSlammed(false),
  canBeSlammedTimer(0.5),

  strengthMult(1.0),
  dmgStrMult(1.0),
  isStanding(true),

  touchPlayerTime(0.0),
  sparkTimer(0.1),

  shootTimer(5.0, true, this),
  shockTimer(6.0, true, this),
  hurtTimer(2.0, this),
  ringTimer(0.15, true, this),

  lavaTimer(1.0),
  bouncesOnLava(false),

  dmgAngleRange(0.0, 0.0),
  blower(false),

  hasWeakRange(true),
  weakAngleRange(0.0, TWO_PI),

  usesGem(false),
  gemGlowAlpha(0.0),
  gemGlowSpeedMult(1.0),
  gemGlowData(0.0, 1.0),

  attachments(8),

  warningAlpha(0.0),

  // warper(this, dwarpCache[sumoWarp]),
  warpWeight(0.0),
  tgtFrame(SumoAI::MAD_FRAME),

  losing(false),
  loseDraw(false),
  losingTimer(3.0, this),
  losingTimer2(3.0, this),
  loseFlameTimer(0.2, this),
  loseSinkSpeed(32.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_SUMO_BOSS;
  
  // setImage(imgCache[sumoBoss]);
  setWH(224.0, 224.0);  // radius
  
  maxHP = 8.0;
  currHP = 8.0;
  
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
  else if (geometry == DIAMOND) init_poly(getXY(), 4, getWidth(), true);
  else if (geometry == HEXAGON) init_poly(getXY(), 6, getWidth(), true);
  else init_poly(getXY(), 8, getWidth(), true);
  
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(1.0);
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
  /*
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
   */
  
  if (getImage() != NULL)
  {
    drawMe();
    drawAttachments();
  }
  else
  {
    collisionCircle().draw_solid(YELLOW_SOLID);
  }
}

void SumoBoss::updateStd()
{
  shootTimer.update();
  shockTimer.update();
  hurtTimer.update();
  canBeSlammedTimer.update();
  lavaTimer.update();
  
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
  
  checkDamage();
}

void SumoBoss::checkDamage()
{
  if (currHP <= 0.0 && getActive() == true)
  {
    destroyedPhysical = true;
  }
  
  checkPhysicalDestruction();
}

void SumoBoss::drawAttachments()
{
  for (Coord1 i = 0; i < attachments.count; ++i)
  {
    attachments[i].redraw();
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
  /*
  VisRectangular::drawImg(this, imgCache[sumoGem]);
  
  Point1 realAlpha = getAlpha();
  setAlpha(gemGlowAlpha);
  VisRectangular::drawImg(this, imgCache[sumoGemGlow]);
  setAlpha(realAlpha);
   */
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
  /*
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
   */
}

void SumoBoss::sparkAt(Point2 center, Coord1 biggestIndex)
{
  /*
  Point1 baseAngle = RM::randf(0.0, TWO_PI);
  Coord1 sparkIndex = RM::randi(0, biggestIndex);
  
  SparkEffect* spark = new SparkEffect(center, baseAngle, (sparkIndex + 1) * 8.0,
                                       imgsetCache[sparkSet][sparkIndex]);
  level->addAction(spark);
   */
}

void SumoBoss::slamResponder()
{
  if (justSlammed == true && canBeSlammedTimer.getActive() == false)
  {
    if (hasWeakRange == true)
    {
      // has weakpoint
      Point1 localHitAngle = RM::standardize_rads(RM::angle(getXY(), player->getXY()) - getRotation());
      
      if (localHitAngle >= weakAngleRange.x && localHitAngle <= weakAngleRange.y)
      {
        createCoins();
        
        addDamage(1.0);
        level->createSparksEffect(getXY() + RM::ring_edge(getXY(), player->getXY(), getWidth()), 1.0,
                                  RM::angle(player->getXY(), getXY()), &level->objman.actions);
        // level->sumoWeakpointHit();
      }
    }
    else
    {
      // no weakpoint
      createCoins();
      
      dmgStrMult -= 0.01;
      dmgStrMult = std::max(0.9, dmgStrMult);
      
    }

    canBeSlammedTimer.reset();
  }
  
  justSlammed = false;
}

void SumoBoss::createCoins()
{
  /*
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
   */
}

void SumoBoss::updateCollisions()
{
  updateLightsaberDmg();
  
  if (dmgAngleRange.x < 0.01 && dmgAngleRange.y < 0.01) return;
  
  Point2 location(0.0, 0.0);
  
  if (touchingPGO(player, &location) == true)
  {
    Point1 angle = RM::standardize_rads(RM::angle(getXY(), location) - getRotation());
    
    if (angle > dmgAngleRange.x && angle < dmgAngleRange.y)
    {
      player->attacked(location, this);
    }
  }
}

void SumoBoss::touchedFire(Point2 location, PizzaGOStd* attacker)
{
  if (bouncesOnLava == true)
  {
    if (lavaTimer.getActive() == false)
    {
      addDamage(1.0);
      lavaTimer.reset();
    }

    Point2 bounceVector = RM::ring_edge(location, getXY(), 700.0);
    bounceVector.x = RM::clamp(bounceVector.x, -450.0, 450.0);
    set_velocity(bounceVector);
  }
  else
  {
    addDamage(1.0 * RM::timePassed());
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

void SumoBoss::collidedPhysical(b2Contact* contact, PizzaGOStd* pgo, Point1 normalImpulse)
{
  if (b2FiltersMatchCategories(contact->GetFixtureA()->GetFilterData(), contact->GetFixtureB()->GetFilterData(),
                               TYPE_PLAYER, TYPE_BOSS_LIGHTSABER))
  {
    // ignore player colliding with lightsaber here. The player handles that collision on their own
  }
  else if (b2FiltersMatchCategories(contact->GetFixtureA()->GetFilterData(), contact->GetFixtureB()->GetFilterData(),
                               TYPE_PLAYER_LIGHTSABER, TYPE_BOSS_LIGHTSABER))
  {
    // ignore player's lightsaber collision with lightsaber here
  }
  else if (b2FiltersMatchCategories(contact->GetFixtureA()->GetFilterData(), contact->GetFixtureB()->GetFilterData(),
                                    TYPE_PLAYER, TYPE_BOSS_ATTACHMENT))
  {
    b2Fixture* attachmentFixture = b2FilterMatchesCategory(contact->GetFixtureA()->GetFilterData(), TYPE_BOSS_ATTACHMENT) ?
        contact->GetFixtureA() : contact->GetFixtureB();
    
    // player collided with boss shroom
    for (Coord1 i = 0; i < attachments.count; ++i)
    {
      if (attachmentFixture == attachments[i].fixture)
      {
        attachmentHit(attachmentFixture, &attachments[i]);
        break;
      }
    }
  }
  else if (player->isSlamming == true &&
      pgo == player &&
      player->getY() < getY())
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
  /*
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
   */
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

  saberSpinTimer(5.0, this),

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
  if (parent->level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  strengthTimer.update();
  actionTimer.update();
  powerTimer.update();
  powerWait.update();
  warpMover.update();
  smallWarper.update();
  saberSpinTimer.update();
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
                                   parent->level->groundCameraPts.first().x,
                                   parent->level->groundCameraPts.last().x);
  return result;
}

Point1 SumoAI::playerArenaPercent()
{
  Point1 result = RM::lerp_reverse(player->getX(),
                                   parent->level->groundCameraPts.first().x,
                                   parent->level->groundCameraPts.last().x);
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
  
  // parent->warper.last_to_cont();
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
    // ResourceManager::playSumoAngry();
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
  else if (caller == &saberSpinTimer)
  {
    // swing lightsaber
    /*
    if (parent->lightsaberFixture != NULL)
    {
      Point1 angleToPlayer = RM::angle(parent->getXY(), player->getXY());
      Point1 angDir = RM::standardize_rads(angleToPlayer - parent->getRotation()) < PI ? 1.0 : -1.0;
      parent->set_ang_vel(angDir * TWO_PI);
    }
     */
    
    saberSpinTimer.reset();
  }
  else if (caller == &gustTimer)
  {
    /*
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
     */
  }
}

// =============================== SumoRayBoss ============================ //

const Point1 SumoRayBoss::RAY_MAX_SPEED = 256.0;

SumoRayBoss::SumoRayBoss() :
  SumoBoss(),

  nextMovePt(0),
  currSpeed(256.0),

  movePts(128.0)
{
  setMovePts();
}

void SumoRayBoss::load()
{
  init_poly(getXY(), 4, getWidth(), false);
  
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(1.0);
  fixture->SetUserData(this);

  callback(&script);
}

void SumoRayBoss::setMovePts()
{
  movePts.add(Point2(1528.0, 2176.0));
  movePts.add(Point2(1984.0, 2040.0));
  movePts.add(Point2(2176.0, 1368.0));
  movePts.add(Point2(1632.0, 1016.0));
  movePts.add(Point2(1096.0, 1336.0));

}

void SumoRayBoss::updateStd()
{
  hurtTimer.update();
  canBeSlammedTimer.update();
  
  updateSwimming();
  syncTail();
  updateFromWorld();
  
  slamResponder();
  updateCollisions();
  updateSparks();
  
  oldXY = getXY();
}

void SumoRayBoss::updateSwimming()
{
  script.update();
  
  rotation = RM::chase_angle(rotation, RM::angle(getXY(), getCurrMovePt()), TWO_PI * 0.1 * RM::timePassed());
  addXY(RM::pol_to_cart(currSpeed * RM::timePassed(), getRotation()));
  
  if (RM::distance_to(getXY(), getCurrMovePt()) <= 72.0)
  {
    nextMovePt++;
    nextMovePt %= movePts.count;
  }
}

Point2 SumoRayBoss::getCurrMovePt()
{
  return movePts[nextMovePt % movePts.count];
}

void SumoRayBoss::syncTail()
{
  
}

void SumoRayBoss::callback(ActionEvent* caller)
{

}

// =============================== SumoJunkBoss ============================ //

SumoJunkBoss::SumoJunkBoss() :
  SumoBoss(),

  waitRange(1.5, 3.0),
  jumpVel(400.0, PLAYER_JUMP_VEL),
  spinVal(0.0)
{
  
}

void SumoJunkBoss::load()
{
  init_box(getXY(), Point2(getWidth() * 2.0, getWidth() * 2.0), true);
  
  set_collisions(1 << type, 0xffff);
  fixture->SetFriction(1.0);
  fixture->SetUserData(this);
  
  script.wait(RM::randf(2.0, 4.0));
}

void SumoJunkBoss::updateStd()
{
  hurtTimer.update();
  canBeSlammedTimer.update();
  
  script.update();
  matchXScale();
  
  updateFromPhysical();
  
  slamResponder();
  updateCollisions();
  updateSparks();
  
  checkDamage();
}

void SumoJunkBoss::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    Point1 dirToPlayer = getX() < player->getX() ? 1.0 : -1.0;
    set_velocity(Point2(dirToPlayer, 1.0) * jumpVel);
    
    if (spinVal > 0.01)
    {
      set_ang_vel(dirToPlayer * spinVal);
    }
    
    script.enqueueX(new SineFn(&scale.x, 1.0, 0.1));
    script.wait(RM::randf(waitRange));
    script.enqueueX(new SineFn(&scale.x, 0.8, 1.0));
    script.wait(0.1);
  }
}

// =============================== SumoFungusBoss ============================ //

void SumoFungusBoss::attachmentHit(b2Fixture* attachmentFixture, BossAttachment* attachment)
{
  attachment->touched = true;
  Physical::set_collisions(attachmentFixture, 0x1 << TYPE_BOSS_ATTACHMENT, 0x0);
  attachmentFixture->SetSensor(true);  // this keeps the pizza's crust from responding to it
  
  BossSpore* spore = new BossSpore();
  spore->setXY(attachment->calcWorldXY());
  level->addPGO(spore);
}

// =============================== SumoEdisonBoss ============================ //

void SumoEdisonBoss::attachmentHit(b2Fixture* attachmentFixture, BossAttachment* attachment)
{
  attachment->touched = true;
  Physical::set_collisions(attachmentFixture, 0x1 << TYPE_BOSS_ATTACHMENT, 0x0);
  attachmentFixture->SetSensor(true);  // this keeps the pizza's crust from responding to it

  VisRectangular shatterHelper;
  shatterHelper.setImage(attachment->img);
  shatterHelper.setXY(attachment->calcWorldXY());
  shatterHelper.setRotation(attachment->calcWorldRotation());
  level->createTriangleShatter(&shatterHelper, Coord2(5, 5), Point2(2.0, 3.0), &level->objman.actions);
  
  turnOffSaber(saberSide);
  
  saberSide += 1;
  saberSide %= 2;
  
  turnOnSaber(saberSide);
}

void SumoEdisonBoss::turnOffSaber(Coord1 side)
{
  Physical::set_collisions(lightsaberFixtures[side], 0x1 << TYPE_BOSS_LIGHTSABER, 0x0);
  lightsaberFixtures[side]->SetSensor(true);  // this keeps the pizza's crust from responding to it
}

void SumoEdisonBoss::turnOnSaber(Coord1 side)
{
  Physical::set_collisions(lightsaberFixtures[side], 0x1 << TYPE_BOSS_LIGHTSABER,
                           (0x1 << TYPE_PLAYER));
  lightsaberFixtures[side]->SetSensor(false);

}

void SumoEdisonBoss::drawLightsaber()
{
  Image* saberImg = saberSide == 0 ? getImg(sumoBoss3Img) : getImg(sumoBoss4Img);
  Point2 saberXY = RM::pol_to_cart(getWidth(), getRotation() + (saberSide == 0 ? PI : 0.0)) + getXY();
  saberImg->draw_scale(saberXY, Point2(1.0, 1.0), getRotation(), getColor());
}

// =============================== Batclops ============================ //

Batclops::Batclops() :
  PizzaGOStd(),

  startY(),
  activeXRange(0.0, 0.0),

  yOffsetData(0.0, RM::randf(), 1.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  setWH(32.0, 32.0);  // radius
  setImage(getImg(breakableObjC));
}

void Batclops::load()
{
  startY = getY();
  
  // sensor for player lightsaber
  init_circle(getXY(), getWidth(), false);
  fixture->SetSensor(true);
  set_b2d_callback_data(this);
  set_collisions(0x1 << type, (0x1 << TYPE_PLAYER_LIGHTSABER));
}

void Batclops::updateMe()
{
  if (getActive() == false)
  {
    return;
  }
  
  RM::bounce_arcsine(yOffsetData, RM::timePassed());
  setY(startY + yOffsetData.x * 64.0);
  
  addX(-256.0 * RM::timePassed());
  
  if (onRMScreen() == false && getX() < activeXRange.x)
  {
    // spawn another one on right side of screen
    spawnNext();
    setRemove();
  }
  
  updateCollisions();
}

void Batclops::redrawMe()
{
  drawMe();
}

void Batclops::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  updateFromWorld();
  
  if (touchingPlayerLightsaber().touchType == SABER_TOUCHING_ME)
  {
    PizzaLevelInterface::createSparksEffect(getXY(), 1.0, RM::angle(player->getXY(), getXY()), &level->objman.actions);
    destroyPhysical();
    spawnNext();
  }
  else if (collisionCircle().collision(player->collisionCircle()) == true)
  {
    Point1 playerAngleToMe = RM::angle(player->getXY(), getXY());
    
    if (playerAngleToMe >= HALF_PI * 0.1 && playerAngleToMe <= PI - HALF_PI * 0.1)
    {
      // player crushed me
      destroyPhysical();
      spawnNext();
    }
    else
    {
      // i hit player from above
      player->attacked(getXY(), this);
    }
  }
}

void Batclops::spawnNext()
{
  Batclops* copyBat = new Batclops();
  copyBat->setXY(activeXRange.y, startY);
  copyBat->activeXRange = activeXRange;
  copyBat->level = level;
  
  // the player can see where this is spawning, move it to the right a lot
  if (copyBat->onRMScreen() == true)
  {
    copyBat->setX(level->rmboxCamera.myBox().right() + level->rmboxCamera.myBox().width());
  }
  
  level->addPGO(copyBat);
}

// =============================== DragonFireballStd ============================ //

DragonFireballStd::DragonFireballStd() :
  script(this),
  colorWeight(0.0),
  vel()
{
  type = TYPE_PROJECTILE;
  phase = PHASE_ALLY_STD;
  
  setWH(96.0, 96.0);
  setScale(0.0, 0.0);
  
  script.enqueueX(new LinearFn(&scale.x, 1.0, 0.25));
  script.wait(2.5);
  script.enqueueX(new LinearFn(&color.w, 0.0, 0.25));
}

void DragonFireballStd::updateMe()
{
  colorWeight += 4.0 * RM::timePassed();
  RM::wrap1_me(colorWeight);
  
  addXY(vel * RM::timePassed());
  script.update();
  
  updateCollisions();
  matchXScale();
}

void DragonFireballStd::redrawMe()
{
  ColorP4 glowColor = RM::lerp(RED_SOLID, YELLOW_SOLID, colorWeight);
  setColor(glowColor.x, glowColor.y, glowColor.z, getAlpha());
  
  Circle fireballCircle = collisionCircle();
  fireballCircle.radius *= getXScale();
  fireballCircle.draw_solid(getColor());
}

void DragonFireballStd::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  for (LinkedListNode<PizzaGOStd*>* enemyNode = level->objman.phaseList[PHASE_ENEMY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* enemy = enemyNode->data;

    if (enemy->type != TYPE_ENEMY ||
        enemy->getActive() == false)
    {
      continue;
    }

    Circle myCircle = collisionCircle();
    Circle enemyCircle = enemy->collisionCircle();
    
    if (enemyCircle.collision(myCircle) == true)
    {
      enemy->destroyPhysical();
      
      script.clear();
      setRemove();
    }
  }
}

void DragonFireballStd::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    setRemove();
  }
}

// =============================== PlayerLightningStd ============================ //

PlayerLightningStd::PlayerLightningStd()
{
  type = TYPE_PROJECTILE;
  phase = PHASE_ALLY_STD;
  
  script.enqueueX(new LinearFn(&scale.x, 1.0, 0.1));
  script.wait(0.1);
  script.enqueueX(new LinearFn(&color.w, 0.0, 0.1));
}

void PlayerLightningStd::updateMe()
{
  script.update();

}

void PlayerLightningStd::redrawMe()
{
  setRed(RM::randf());
  setBlue(RM::randf());
  setGreen(RM::randf());
  getImg(tempLightningImg)->draw_repeating_tgt(getXY(), target, getColor());
}

void PlayerLightningStd::callback(ActionEvent* caller)
{
  setRemove();
}

// =============================== ForceBone ============================ //

ForceBone::ForceBone() :
  PizzaGOStd(),

  parent(NULL),

  sideOfParent(1),
  parentOffset(0.0, 0.0),
  vel(0.0, 0.0),

  hoverTimer(2.0),
  flyTimer(5.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  setWH(54.0, 54.0);  // radius
  
  setScale(0.0, 0.0);
  
  flyTimer.setInactive();
  setImage(getImg(breakableObjA));
}

void ForceBone::load()
{
  sideOfParent = player->getX() > parent->getX() ? 1 : -1;
  Point1 angleOffet = HALF_PI * 0.1;
  parentOffset = RM::pol_to_cart(parent->getWidth() + 128.0, sideOfParent == 1 ? TWO_PI - angleOffet : PI + angleOffet);
  
  // sensor for player lightsaber
  init_circle(getXY(), getWidth(), false);
  fixture->SetSensor(true);
  set_b2d_callback_data(this);
  set_collisions(0x1 << type, (0x1 << TYPE_PLAYER_LIGHTSABER));
}

void ForceBone::updateMe()
{
  if (lifeState == RM::TRANS_OUT)
  {
    return;
  }
  
  addRotation(sideOfParent * TWO_PI * 4.0 * RM::timePassed());
  Logical wasHovering = hoverTimer.getActive();

  if (wasHovering == true)
  {
    hoverTimer.update();
    setXY(parent->getXY() + parentOffset);
    RM::flatten_me(scale.x, 1.0, 1.0 * RM::timePassed());
    matchXScale();
  }
  
  if (wasHovering == true && hoverTimer.getActive() == true)
  {
    return;
  }
  else if (wasHovering == true && hoverTimer.getActive() == false)
  {
    throwAtPlayer();
  }

  flyTimer.update();
  addXY(vel * RM::timePassed());

  // thrown at player, waiting for hit or timeout
  if (flyTimer.getActive() == true)
  {
    updateCollisions();
  }
  // timed out without hitting anything
  else if (flyTimer.getActive() == false)
  {
    if (onRMScreen() == false)
    {
      setRemove();
    }
  }
}

void ForceBone::redrawMe()
{
  drawMe();
}

void ForceBone::throwAtPlayer()
{
  flyTimer.setActive();
  vel = RM::pol_to_cart(1024.0, RM::angle(getXY(), player->getXY()));
}

void ForceBone::updateCollisions()
{
  updateFromWorld();

  if (touchingPlayerLightsaber().touchType == SABER_TOUCHING_ME)
  {
    PizzaLevelInterface::createSparksEffect(getXY(), 1.0, RM::angle(player->getXY(), getXY()), &level->objman.actions);
    destroyPhysical();
  }
  else if (collisionCircle().collision(player->collisionCircle()) == true)
  {
    // i hit player from above
    player->attacked(getXY(), this);
    
    destroyPhysical();
  }
}


// =============================== PizzaCoinStd ============================ //

PizzaCoinStd::PizzaCoinStd() :
  PizzaGOStd(),

  flipbookAnimator(),
  animPauseTimer(1.0, this),

  usePhysics(false),
  
  gemScaleData(0.0, 0.0, 1.0)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_PICKUP;
  value = COIN_PENNY;
  
  setWH(24.0, 24.0);  // radius
}

PizzaCoinStd::PizzaCoinStd(Point2 center, Coord1 setValue) :
  PizzaGOStd(),

  flipbookAnimator(),
  animPauseTimer(1.0, this),

  usePhysics(false),

  gemScaleData(0.0, 0.0, 1.0)
{
  setXY(center);
  
  phase = PHASE_ALLY_STD;
  type = TYPE_PICKUP;
  value = setValue;
  
  setWH(24.0, 24.0);  // radius
}

void PizzaCoinStd::load()
{
  DataList<Image*> animationSet;

  if (isGem() == true)
  {
    animationSet = getImgSet(gemBluePickupSet + value - FIRST_GEM_TYPE);
  }
  else
  {
    for (Coord1 i = 0; i < 16; ++i)
    {
      animationSet.add(getImgSet(coinPickupSet)[(value - FIRST_COIN_TYPE) * 16 + i]);
    }
  }
  
  setImage(animationSet.first());
  
  flipbookAnimator.init(this, animationSet, 0.06);
  flipbookAnimator.setListener(this);
  
  animPauseTimer.setInactive();
  
  if (usePhysics == true)
  {
    init_circle(getXY(), getWidth(), true);
    set_collisions(1 << TYPE_PICKUP, (0x1 << TYPE_TERRAIN));
    set_restitution(0.90);
    crushesPhysical = false;
  }
}

void PizzaCoinStd::updateMe()
{
  // this is for when it emerges from a block
  script.update();
  actions.update();
  
  RM::bounce_linear(gemScaleData, RM::timePassed());

  if (isGem() == true)
  {
    setXScale(1.0 + gemScaleData.x * 0.2);
  }
  
  flipbookAnimator.update();
  animPauseTimer.update();
  
  
  matchXScale();
  
  if (usePhysics == true)
  {
    updateFromPhysical();
    setRotation(0.0);
  }
  
  if (getActive() == true &&
      collisionCircle().collision(player->collisionCircle()) == true)
  {
    gotCoin();
    setRemove();
  }
}

void PizzaCoinStd::redrawMe()
{
  if (onRMScreen() == false)
  {
    return;
  }
  
  if (getImage() != NULL)
  {
    drawMe();
  }
  else
  {
    collisionCircle().draw_solid(ColorP4(1.0, 1.0, 0.0, getAlpha()));
  }
}

Logical PizzaCoinStd::isCoin()
{
  return value >= FIRST_COIN_TYPE && value <= LAST_COIN_TYPE;
}

Logical PizzaCoinStd::isGem()
{
  return value >= FIRST_GEM_TYPE && value <= LAST_GEM_TYPE;
}


void PizzaCoinStd::gotCoin()
{
  activeGameplayData->collectedCoins[value]++;
  
  setRemove();
}

void PizzaCoinStd::callback(ActionEvent* caller)
{
  if (caller == &flipbookAnimator)
  {
    if (isCoin() == true)
    {
      flipbookAnimator.reset();
    }
    else if (isGem() == true)
    {
      flipbookAnimator.setInactive();
      animPauseTimer.reset(1.0);
    }
  }
  else if (caller == &animPauseTimer)
  {
    flipbookAnimator.reset();
  }
}

// =============================== GourdoLantern ============================ //

GourdoLantern::GourdoLantern() :
  PizzaGOStd()
{
  phase = PHASE_ALLY_STD;
  type = TYPE_PICKUP;
  value = GOURDO_ITEM;
  
  setWH(64.0, 64.0);  // radius
  
  // setImage(getImg(gourdoLanternImg));
  initSpineAnim(gourdoLanternSpine, "idle");
}

void GourdoLantern::updateMe()
{
  updateSpineAnim();
  
  actions.update();
  script.update();
  
  matchXScale();
  
  if (getActive() == true &&
      collisionCircle().collision(player->collisionCircle()) == true)
  {
    player->gotGourdo();
    setRemove();
  }
}

void GourdoLantern::redrawMe()
{
  addY(64.0);
  PizzaGOStd::redrawMe();
  addY(-64.0);

  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 1.0, 0.0, getAlpha()));
  }
}

Circle GourdoLantern::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== MagnetCoin ============================ //

MagnetCoin::MagnetCoin(Point2 center, Coord1 setValue) :
  PizzaCoinStd(center, setValue),

  progressTimer(RM::randf(0.75, 1.25), this),

  startXY(0.0, 0.0),
  startDist(0.0),
  tgtDist(RM::randf(16.0, 180.0)),

  angleToPlayer(0.0),
  rotSpeed(RM::randf(PI * 0.4, PI * 0.6))
{
  rotSpeed *= RM::randl() ? 1.0 : -1.0;
}

void MagnetCoin::load()
{
  PizzaCoinStd::load();
  
  startXY = getXY();
  
  startDist = RM::distance_to(startXY, player->getXY());
  angleToPlayer = RM::angle(player->getXY(), getXY());
}

void MagnetCoin::updateMe()
{
  // animator.update();
  
  // double update if player is about to win
  if (level->levelState == LEVEL_WIN) updateMotions();
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

// =============================== PachinkoBall ============================ //

PachinkoBall::PachinkoBall()
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_PACHINKO_BALL;
  
  setWH(32.0, 32.0);
}

void PachinkoBall::load()
{
  init_circle(getXY(), getWidth(), true);
}

void PachinkoBall::updateMe()
{
  if (getActive() == true)
  {
    updateFromPhysical();
    
    updateCollisions();
  }
}

void PachinkoBall::redrawMe()
{
  
}

void PachinkoBall::updateCollisions()
{
  for (LinkedListNode<PizzaGOStd*>* enemyNode = level->objman.phaseList[PHASE_ALLY_STD].first;
       enemyNode != NULL;
       enemyNode = enemyNode->next)
  {
    PizzaGOStd* pgo = enemyNode->data;
    
    if (pgo->type == TYPE_PACHINKO_HOLE &&
        pgo->getActive() == true &&
        pgo->collisionCircle().collision(collisionCircle()) == true)
    {
      setRemove();
    }
  }
  
  if (collisionCircle().top() >= level->worldBox.bottom())
  {
    setRemove();
  }
}

// =============================== PachinkoHole ============================ //

PachinkoHole::PachinkoHole() :
  PizzaGOStd(),

  numStars(0)
{
  phase = PHASE_ALLY_STD;
  type = TYPE_PACHINKO_HOLE;
  
  setWH(32.0, 32.0);
}

void PachinkoHole::updateMe()
{
  if (collisionCircle().collision(player->collisionCircle()) == true)
  {
    if (numStars >= 1)
    {
      level->winLevel();
    }
    else
    {
      level->loseLevel();
    }
  }
}

void PachinkoHole::redrawMe()
{
  collisionCircle().draw_solid(ColorP4(0.0, 0.0, numStars * 0.333, 1.0));
}


// =============================== LakeGO ============================ //

LakeGO::LakeGO() :
  VisRectangular(),
  BaseGOLogic(),
  PizzaSpineAnimatorStd(),
  ActionListener(),

  phase(PHASE_ENEMY_STD),
  type(LAKE_TYPE_FISH),
  sizeIndex(SIZE_16),

  level(NULL),
  player(NULL),

  mouthTimer(0.25),
  poison(false),

  script(this),
  actions(),

  poisonTimer(0.25, (ActionListener*) this)
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
  cout << "super redraw" << endl;
  
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
    // level->addAlly(new LakeBubble(collisionCircle().random_pt(), RM::randi(0, 1)));
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

VisRectangular* LakeGO::get_visrect()
{
  return this;
}

// =============================== LakePlayer ============================ //

LakePlayer::LakePlayer() :
  LakeGO(),

  face(this),

  scaler(&scale.x, 2.0, 1.5, this),
  fadeInImage(NULL),
  fadeInAlpha(0.0),

  flasher(),
  invincibilityTimer(0.01),

  vxy(0.0, 0.0),
  sizeMult(1.0),
  amountEaten(0.0),
  drawAngle(TWO_PI - 0.01),

  lostStandard(false)
{
  phase = PHASE_PLAYER;
  type = LAKE_TYPE_PLAYER;
  
  mouthTimer.setListener(this);
  scaler.setInactive();
  setRotation(TWO_PI - 0.01);
  
  invincibilityTimer.reset(1.0 + 1.0 * Pizza::getEquippedSeasoningLevel(SEAS_INVINCIBILITY_0));
  invincibilityTimer.setInactive();
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
    addY(-128.0 * RM::timePassed());
    hflip(false);
    vflip();
  }

  face.updateFace();
}

void LakePlayer::redrawMe()
{
  collisionCircle().draw_outline(WHITE_SOLID);

  if (invincibilityTimer.getActive() == false ||
      flasher.flashIsOn() == false)
  {
//    collisionCircle().draw_outline(ColorP4(1.0, 1.0, 0.0, 1.0));
//    mouthCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
  
  // cout << "invince " << invincibilityTimer.getActive() << " flasher " << flasher.flashIsOn() << endl;
  
  
  flasher.redraw();
}

void LakePlayer::updateStd()
{
  scaler.update();
  scale.y = scale.x;
  fadeInAlpha = scaler.progress();
  
  invincibilityTimer.update();
  flasher.update();
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
  vxy.y += 400.0 * sizeMult * RM::timePassed();  // gravity
  vxy.y = std::min(vxy.y, 200.0 * sizeMult);  // max y vel
  
  xy += vxy * RM::timePassed();
  
  // enforce world boundaries
  RM::clamp_me(xy.x, 0.0, LAKE_WORLD_SIZE.x);
  RM::clamp_me(xy.y, 0.0, LAKE_WORLD_SIZE.y);
  
  hflip(vxy.x < 0.0);
}

void LakePlayer::tryJump()
{
  if (getY() < 0.0) return;  // out of water
  
  // ResourceManager::playSwim();
  vxy.y = -400.0 * sizeMult;
  if (face.faceState == LakeFace::IDLE) face.idleTrigger(LakeFace::SWIM_1);
}

void LakePlayer::tryMove(Point1 mag)
{
  vxy.x = 400.0 * sizeMult * mag;
}

void LakePlayer::smallerNearby()
{
  mouthTimer.reset();
}

void LakePlayer::ateFish(LakeGO* fish)
{
  mouthTimer.setInactive();
  // ResourceManager::playFishEat();
  
  if (fish->poison == true)
  {
    attacked(getXY());
    face.actionTrigger(LakeFace::HURT);
    return;
  }
  
  Point1 oldEaten = amountEaten;
  amountEaten += fish->sizeIndex + 1.0;
  // level->gotScore((fish->sizeIndex + 1) * 20);
  
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

Logical LakePlayer::attacked(Point2 atkPoint, Point1 dmg)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return false;
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return false;
  if (tempInvincible() == true) return false;
  
  face.actionTrigger(LakeFace::HURT);
  level->playerDamaged(dmg);
  // level->addSauceEffect(*this, atkPoint);
  
  if (level->levelState == PizzaLevelInterface::LEVEL_PLAY)
  {
    // take normal damage but didn't lose
    invincibilityTimer.reset();
  }
  else if (level->levelState == PizzaLevelInterface::LEVEL_LOSE)
  {
    // take normal damage and did lose
    loseToDamage();
  }
  
  return true;
}

Logical LakePlayer::tempInvincible()
{
  return invincibilityTimer.getActive();
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
  
  // setImage(getFishImg(sizeIndex));
  setScale(1.0, 1.0);

  Point1 nominalSize = 16.0 * std::pow(2.0, sizeIndex);
  setWH(nominalSize, nominalSize);
  
  sizeMult = 1.0 + 0.25 * (sizeIndex - SIZE_32);
  level->playerGrew(sizeIndex);
}

Image* LakePlayer::getFishImg(Coord1 size)
{
  /*
  switch (size)
  {
    default:
    case SIZE_32: return imgCache[lakePizza32]; break;
    case SIZE_64: return imgCache[lakePizza64]; break;
    case SIZE_128: return imgCache[lakePizza128]; break;
    case SIZE_256: return imgCache[lakePizza256]; break;
    case SIZE_512: return imgCache[lakePizza256]; break;
  }
   */
  return NULL;
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
  // faceWarper(parent, dwarpCache[lakeFaceWarp]),
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
  /*
  faceWarper.set(parent, dwarpCache[lakeFaceWarp]);
  faceWarper.lastWarpFrame = faceWarper.sequence->warpFrames[0];
  faceWarper.contWarpFrame = faceWarper.sequence->warpFrames[0];
  */
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

  // faceWarper.draw_cont(frameTarget, currWeight);

  parent->hflip(parentHFlip);
}

Point1 LakeFace::damagePercent()
{
  // return std::min(Pizza::currGameVars[VAR_DAMAGE] / 8.0, 1.0);
  return 0.0;
}

Logical LakeFace::isHurt()
{
  return faceState == HURT;
}

void LakeFace::actionTrigger(Coord1 actionType)
{
  if (faceState >= actionType) return;
  
  faceScript.clear();
  // faceWarper.last_to_cont();
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
  // faceWarper.last_to_cont();
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

  spineIndex(fish16Spine),

  normalSwimStr("swim_normal"),
  chaseSwimStr("swim_attack"),
  scaredSwimStr("swim_scared"),
  eatStr("eat"),

  bodyWeight(0.0),
  bodyWeightData(0.0, 1.0),

  headWeight(0.0),

  headImg(NULL),
  onscreen(true),

  swimDir(RM::randl() ? -1.0 : 1.0),
  currSpeed(0.0),
  currAngle(0.0),

  willChase(false),
  willRun(false),

  swimState(SWIM_NORMAL)
{
  phase = PHASE_ENEMY_STD;
  
  sizeIndex = size;
  spineIndex = fish16Spine + sizeIndex;

  Point1 nominalSize = 16.0 * std::pow(2.0, sizeIndex);
  setWH(nominalSize, nominalSize);
}

void LakeFish::load()
{
  mover.player = player;
  onscreen = Box::collision(collisionBox(), level->rmboxCamera.myBox());
  hflip(swimDir < 0.0);
  
  currSpeed = normalSpeed();
  currAngle = swimDir > 0.0 ? 0.0 : PI;
  
  if (poison == true)
  {
    poisonTimer.setActive();
    poisonTimer.setListener(this);
    setColor(RM::color255(247, 255, 153));
  }
  
  initSpineAnim(spineIndex, normalSwimStr.as_cstr(), true);
}

void LakeFish::updateMe()
{
  Point1 oscillateSpeed = 5.0 - 0.5 * sizeIndex;
  RM::bounce_linear(bodyWeight, bodyWeightData, Point2(0.0, 1.0), oscillateSpeed * RM::timePassed());
  
  poisonTimer.update();
  mouthTimer.update();
  mover.update();

  if (usingSpineAnim() == true)
  {
    updateSpineAnim();
  }
  
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
  if (usingSpineAnim() == true)
  {
    renderSpineAnim();
  }
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(COLOR_FULL);
    mouthCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
  }
  
  // cout << "drawing " << this << " at " << getXY() << endl;
}

Logical LakeFish::facingPlayer()
{
  return (mouthCircle().xy.x > player->getX()) == getHFlip();
}

void LakeFish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  // this is really approximate becasue of hflip
  onscreen = Box::collision(collisionBox(), level->rmboxCamera.myBox());

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
        Logical hit = player->attacked(getXY(), 100.0);
        
        if (hit == true)
        {
          // ResourceManager::playFishEat();
          swallowedPlayer();
        }
      }
      // first is 1 bigger
      else
      {
        player->attacked(getXY(), 1.0);
      }
    }
  }
}

void LakeFish::swallowedPlayer()
{
  FishSwallow* swallowEffect = new FishSwallow(player, collisionCircle().xy);
  level->addAction(swallowEffect);
  
  reportMovementType(LakeFish::SWIM_BITE);

  Point2 target = RM::attract(this->getXY(), player->mouthCircle().xy, sizeIndex * 16.0);
  level->addAction(new CosineFn(&xy.x, target.x, 0.20));
  level->addAction(new CosineFn(&xy.y, target.y, 0.20));
  
  for (Coord1 i = 0; i < (sizeIndex + 1) * 3; ++i)
  {
    // level->addAlly(new LakeBubble(collisionCircle().random_pt(), RM::randi(0, 1)));
  }
}

void LakeFish::reportMovementType(Coord1 animType)
{
  // same state reported
  if (usingSpineAnim() == false ||
      swimState == animType ||
      swimState == SWIM_BITE)
  {
    return;
  }
  
  swimState = animType;
  
  switch (swimState)
  {
    default:
    case SWIM_NORMAL:
      mySpineAnim.startAnimation(normalSwimStr.as_cstr(), 0, true);
      break;
    case SWIM_CHASE:
      mySpineAnim.startAnimation(chaseSwimStr.as_cstr(), 0, true);
      break;
    case SWIM_RUN:
      mySpineAnim.startAnimation(scaredSwimStr.as_cstr(), 0, true);
      break;
    case SWIM_BITE:
      mySpineAnim.startAnimation(eatStr.as_cstr(), 0, 0.4);
      break;
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
  Point2 end = getXY() + Point2(getWidth() * 0.5 * swimDir, getWidth() / 8.0);
  return Circle(RM::rotate(getXY(), end, getHFlip() ? currAngle - PI : currAngle), getWidth() / 8.0);
}

void LakeFish::callback(ActionEvent* caller)
{
  /*
  if (caller == &poisonTimer)
  {
    Point2 poisonCoords = getXY() + getWidth() * 0.4 * RM::randf(-1.0, 1.0);
    level->addAction(new PoisonPuff(poisonCoords));
    poisonTimer.reset(RM::randf(0.25, 1.0));
  }
   */
}

// =============================== LakeShark ============================ //

LakeShark::LakeShark(Coord1 size) :
  LakeFish(size)
{
  isShark = true;
  willChase = true;
  willRun = true;
  
  spineIndex = size == SIZE_256 ? shark256Spine : shark128Spine;
  
  normalSwimStr = "swim";
  chaseSwimStr = "swim_attack";
  scaredSwimStr = "swim_scared";
  eatStr = "eat";
}

Point1 LakeShark::maxSpeed()
{
  return normalSpeed() * (1.0 + 0.5 * sizeIndex) * 0.75;
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
  
  parent->reportMovementType(LakeFish::SWIM_NORMAL);
}

void FishMover::lazyTowardPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->normalSpeed(), RM::angle(mouthCenter, player->getXY()), HALF_PI);

  parent->reportMovementType(LakeFish::SWIM_CHASE);
}

void FishMover::fastTowardPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->maxSpeed(), RM::angle(mouthCenter, player->getXY()), PI);

  parent->reportMovementType(LakeFish::SWIM_CHASE);
}

void FishMover::awayFromPlayer()
{
  Point2 mouthCenter = parent->mouthCircle().xy;
  swim(parent->maxSpeed(), RM::angle(player->getXY(), mouthCenter), PI);

  parent->reportMovementType(LakeFish::SWIM_RUN);
}

void FishMover::swim(Point1 speedTarget, Point1 angleTarget, Point1 turnSpeed)
{
  RM::flatten_me(parent->currSpeed, speedTarget, speedTarget * RM::timePassed());
  parent->currAngle = RM::chase_angle(parent->currAngle,
                                      angleTarget,
                                      turnSpeed * RM::timePassed());
  
  parent->addXY(RM::pol_to_cart(parent->currSpeed, parent->currAngle) * RM::timePassed());

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
  scaler.update();
  parent->setYScale(parent->getXScale());
}

void FishSwallow::callback(ActionEvent* caller)
{
  parent->setRemove();
  done();
}

// =============================== LakeSwordfish ============================ //

LakeSwordfish::LakeSwordfish() :
  LakeGO(),

  xVel(0.0),
  startY(0.0),
  bobData(0.0, 1.0),

  speedTimer(4.0, this),
  fastSwim(false),

  oldXY(0.0, 0.0)
{
  setWH(32.0, 32.0);
  sizeIndex = SIZE_64;
  hflip(RM::randl());
  
  initSpineAnim(mySpineAnim, seahorseSpine, "swim", true);
}

void LakeSwordfish::load()
{
  startY = getY();
  oldXY = getXY();
}

void LakeSwordfish::updateMe()
{
  speedTimer.update();
  
  updateSpineAnim(fastSwim ? 1.0 : 0.25);
  
  oldXY = getXY();
  RM::flatten_me(xVel, (fastSwim ? 256.0 : 64.0) * facingToDir(), 256.0 * RM::timePassed());
  addX(xVel * RM::timePassed());
  RM::bounce_arcsine(xy.y, bobData, Point2(-24.0, 24.0) + startY, 0.5 * RM::timePassed());
  
  if (getHFlip() == false) setRotation(RM::angle(oldXY, getXY()));
  else setRotation(RM::angle(oldXY, getXY()) - PI);
  
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeSwordfish());
    setRemove();
  }
}

void LakeSwordfish::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    mouthCircle().draw_outline(RED_SOLID);
  }
}

void LakeSwordfish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  if (touchingPlayer(mouthCircle()) == true)
  {
    player->attacked(mouthCircle().xy, 2.0);
  }
  else if (sizeIndex < player->sizeIndex &&
      playerNearEating() == true)
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->placeAndAddLGO(new LakeSwordfish());
    }
  }
}

Circle LakeSwordfish::collisionCircle()
{
  return Circle(getXY() + Point2(facingToDir() * -16.0, 0.0), getWidth());
}

Circle LakeSwordfish::mouthCircle()
{
  Point2 rotationMod = RM::rotate(Point2(26.0, 0.0), getHFlip() ? PI + getRotation() : getRotation());
  return Circle(getXY() + rotationMod, 16.0);
}

void LakeSwordfish::callback(ActionEvent* caller)
{
  if (caller == &speedTimer)
  {
    fastSwim = !fastSwim;
    speedTimer.reset(fastSwim ? 3.0 : 3.0);
  }
}


// =============================== LakeUrchin ============================ //

LakeUrchin::LakeUrchin() :
  LakeGO(),

  jitter(&xy, Point2(32.0, 96.0), Point2(3.0, 8.0))
{
  // setImage(imgCache[lakeSwordfishImg]);
  setWH(64.0, 64.0);
  sizeIndex = SIZE_128;
  hflip(RM::randl());
  
  initSpineAnim(mySpineAnim, urchinSpine, "swim", true);
}

void LakeUrchin::load()
{

}

void LakeUrchin::updateMe()
{
  jitter.update();
  
  updateSpineAnim();
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeUrchin());
    setRemove();
  }
}

void LakeUrchin::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}

void LakeUrchin::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  if (sizeIndex < player->sizeIndex &&
      playerNearEating() == true)
  {
    player->smallerNearby();

    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->placeAndAddLGO(new LakeUrchin());
    }
  }
  else if (touchingPlayer(mouthCircle()) == true)
  {
    player->attacked(mouthCircle().xy, 2.0);
  }
}

// =============================== LakeLobster ============================ //

LakeLobster::LakeLobster() :
  LakeGO(),

  swimData(0.0, 0.0, 1.0),
  speedMult(RM::randf(0.65, 2.0)),
  startX(0.0)
{
  // setImage(imgCache[lakeSwordfishImg]);
  setWH(32.0, 32.0);
  sizeIndex = SIZE_64;
  hflip(RM::randl());
  
  initSpineAnim(mySpineAnim, lobsterSpine, "swim", true);
  mySpineAnim.setDefaultMixDuration(0.1);
}

void LakeLobster::load()
{
  startX = getX();
}

void LakeLobster::updateMe()
{
  Point1 oldZ = swimData.z;
  RM::bounce_arcsine(swimData, Point2(-128.0, 128.0), 0.333 * speedMult * RM::timePassed());
  setX(startX + swimData.x);
  
  Coord1 newZ = swimData.z;
  
  // hflip(swimData.z <= 0.0);
  
  if (oldZ >= 0.0 && newZ < 0.0)
  {
    // flip from left to right
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, "turn_start", 0, 0.25));
    script.enqueueX(new SetValueCommand<Byte4>(&flipFlags, (Byte4) HFLIP_BIT));
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, "turn_end", 0, 0.25));
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "swim", 0, true));
  }
  else if (oldZ < 0.0 && newZ >= 0.0)
  {
    // flip from right to left
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, "turn_start", 0, 0.25));
    script.enqueueX(new SetValueCommand<Byte4>(&flipFlags, (Byte4) 0x0));
    script.enqueueX(new SpineAnimationAction(&mySpineAnim, "turn_end", 0, 0.25));
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "swim", 0, true));
  }

  script.update();
  updateSpineAnim();
  
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeLobster());
    setRemove();
  }
}

void LakeLobster::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    mouthCircle().draw_outline(RED_SOLID);
  }
}

void LakeLobster::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  if (sizeIndex < player->sizeIndex && playerNearEating() == true)
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->placeAndAddLGO(new LakeLobster());
    }
  }
  else if (touchingPlayer(mouthCircle()) == true)
  {
    player->attacked(mouthCircle().xy, 2.0);
  }
}

Circle LakeLobster::mouthCircle()
{
  return Circle(getXY() + Point2(facingToDir() * 48.0, 0.0), 24.0);
}

// =============================== LakeJellyfish ============================ //

LakeJellyfish::LakeJellyfish() :
  LakeGO(),

  electricOn(false),

  centerPt(0.0, 0.0),
  oldXY(0.0, 0.0),
  startAngle(0.0),
  deathAngle(HALF_PI),

  motionWeight(0.125),
  humpRadius(24.0),
  speedMult(RM::randf(0.8, 1.2)),
  moveAnimTgt(0.0)
{
  // setImage(imgCache[lakeSwordfishImg]);
  sizeIndex = RM::randi(SIZE_16, SIZE_64);
  
  speedMult *= RM::randl() ? 1.0 : -1.0;
}

void LakeJellyfish::load()
{
  setWH(std::pow(2, 4 + sizeIndex), std::pow(2, 4 + sizeIndex));
  
  humpRadius = 16.0 + 4.0 * sizeIndex;
  
  oldXY = getXY();
  centerPt = RM::pol_to_cart(32.0 + 16.0 * sizeIndex, startAngle) + getXY();
  
  Point1 moveTime = 0.5;
  
  Coord1 spineIndex = jellySmallSpine;
  
  if (sizeIndex == SIZE_32)
  {
    spineIndex = jellyMedSpine;
  }
  else if (sizeIndex == SIZE_64)
  {
    spineIndex = jellyLargeSpine;
  }
  
  initSpineAnim(mySpineAnim, spineIndex, "swim", true);
  
  lifeState = RM::TRANS_IN;
  
  actions.addX(new SineFn(&xy.x, centerPt.x, moveTime));
  script.enqueueX(new SineFn(&xy.y, centerPt.y, moveTime));
  script.enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::ACTIVE));
  script.wait(RM::randf(0.0, 2.0));
}

void LakeJellyfish::updateMe()
{
  updateSpineAnim();
  
  actions.update();
  script.update();
  
  centerPt.y -= (4.0 + sizeIndex * 2.0) * RM::timePassed();
  motionWeight = RM::wrap1(motionWeight + 0.15 * speedMult * RM::timePassed());
  
  oldXY = getXY();
  setXY(centerPt + localMotionCoords(humpRadius, motionWeight));

  Point2 vel = (getXY() - oldXY) / RM::timePassed();
  // rotation = RM::chase_angle(rotation, RM::angle(vel) + HALF_PI, TWO_PI * RM::timePassed());

  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeJellyfish());
    setRemove();
  }
}

void LakeJellyfish::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    ColorP4 debugColor = electricOn ? RED_SOLID : WHITE_SOLID;
    collisionCircle().draw_outline(debugColor);
  }
}

void LakeJellyfish::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  if (getActive() == false) return;
  
  if (electricOn == true)
  {
    if (touchingPlayer() == true)
    {
      player->attacked(mouthCircle().xy, 2.0);
    }
  }
  else if (sizeIndex < player->sizeIndex &&
           playerNearEating() == true)
  {
    player->smallerNearby();

    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      
      if (sizeIndex > SIZE_16)
      {
        // split
        LakeJellyfish* leftJelly = new LakeJellyfish();
        leftJelly->sizeIndex = sizeIndex - 1;
        leftJelly->setXY(getXY());
        leftJelly->startAngle = deathAngle - HALF_PI;
        level->addPGO(leftJelly);
        
        LakeJellyfish* rightJelly = new LakeJellyfish();
        rightJelly->sizeIndex = sizeIndex - 1;
        rightJelly->setXY(getXY());
        rightJelly->startAngle = deathAngle + HALF_PI;
        level->addPGO(rightJelly);
      }
      else
      {
        // make a new random one
        LakeJellyfish* randomJelly = new LakeJellyfish();
        randomJelly->sizeIndex = RM::randi(SIZE_16, SIZE_64);
        level->placeAndAddLGO(randomJelly);
      }
    }
  }
}

Point2 LakeJellyfish::localMotionCoords(Point1 humpRadius, Point1 functionWeight)
{
  // function has 4 parts of approximately equal distance
  // 0: top left corner to bottom right corner
  // 1: bot right corner to top right corner
  // 2: top right corner to bot left corner
  // 3: bot left corner to top left corner
  Coord1 numSegments = 4;
  Point1 diagonalLength = humpRadius * PI;
  Point1 singleSegmentWeight = 1.0 / numSegments;
  
  Coord1 currSegment = RM::clamp(RM::float_to_int(functionWeight / singleSegmentWeight), 0, 3);
  Point1 currSegmentWeight = RM::double_lerp(functionWeight,
                                             Point2(currSegment * singleSegmentWeight,
                                                    (currSegment + 1) * singleSegmentWeight),
                                             Point2(0.0, 1.0));
  Point1 diagXLength = std::sqrt(diagonalLength * diagonalLength - humpRadius * humpRadius);
  Point2 topLeftPt = Point2(-diagXLength * 0.5, -humpRadius);
  Point2 topRightPt = Point2(diagXLength * 0.5, -humpRadius);
  Point2 botRightPt = -topLeftPt;
  Point2 botLeftPt = -topRightPt;
  Point2 leftHumpCenter = Point2(-diagXLength * 0.5, 0.0);
  Point2 rightHumpCenter = Point2(diagXLength * 0.5, 0.0);
  
  Point2 result(0.0, 0.0);
  
  switch (currSegment)
  {
      // 0: top left corner to bottom right corner
    default:
    case 0:
      result.x = RM::lerp(topLeftPt.x, botRightPt.x, currSegmentWeight);
      result.y = RM::arcsine_fn(Point2(topLeftPt.y, botRightPt.y), currSegmentWeight);
      moveAnimTgt = 1.0;
      break;
      
      // 1: bot right corner to top right corner
    case 1:
      result = rightHumpCenter + RM::pol_to_cart(humpRadius, HALF_PI - PI * currSegmentWeight);
      moveAnimTgt = 0.0;
      break;
      
      // 2: top right corner to bot left corner
    case 2:
      result.x = RM::lerp(topRightPt.x, botLeftPt.x, currSegmentWeight);
      result.y = RM::arcsine_fn(Point2(topRightPt.y, botLeftPt.y), currSegmentWeight);
      moveAnimTgt = 1.0;
      break;
      
      // 3: bot left corner to top left corner
    case 3:
      result = leftHumpCenter + RM::pol_to_cart(humpRadius, HALF_PI + PI * currSegmentWeight);
      moveAnimTgt = 0.0;
      break;
  }
  
  return result;
}

void LakeJellyfish::callback(ActionEvent* caller)
{
  script.wait(2.0);
  script.enqueueX(new SetValueCommand<Logical>(&electricOn, true));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "shock", 0, true));

  script.wait(2.0);
  script.enqueueX(new SetValueCommand<Logical>(&electricOn, false));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "swim", 0, true));
}

// =============================== LakeStingray ============================ //

LakeStingray::LakeStingray() :
  LakeGO(),

  oldXY(0.0, 0.0),
  target(0.0, 0.0),

  speedMult(RM::randf(0.5, 2.0)),

  targetTimeout(5.0)
{
  // setImage(imgCache[lakeSwordfishImg]);
  setWH(128.0, 128.0);
  sizeIndex = SIZE_128;
  
  initSpineAnim(mySpineAnim, stingraySpine, "swim", true);
  mySpineAnim.setDefaultMixDuration(0.05);
}

void LakeStingray::load()
{
  oldXY = getXY();
  
  pickTarget();
}

void LakeStingray::updateMe()
{
  oldXY = getXY();

  Point1 totalSpeed = 128.0 * speedMult;
  addXY(RM::pol_to_cart(totalSpeed, getRotation()) * RM::timePassed());
  RM::chase_me_angle(rotation, RM::angle(getXY(), target), 0.5 * HALF_PI * RM::timePassed());
  
  targetTimeout.update();
  
  if (RM::distance_to(getXY(), target) < 64.0 ||
      targetTimeout.getActive() == false)
  {
    pickTarget();
  }
  
  script.update();
  updateSpineAnim();
  
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeStingray());
    setRemove();
  }
}

void LakeStingray::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
    // mouthCircle().draw_outline(RED_SOLID);
    mouthCircle2().draw_outline(RED_SOLID);
  }
}

void LakeStingray::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  if (sizeIndex < player->sizeIndex &&
      playerNearEating() == true)
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      swallowedByPlayer();
      level->placeAndAddLGO(new LakeSwordfish());
    }
  }
  else if (touchingPlayer(mouthCircle2()) == true)
  {
    player->attacked(mouthCircle2().xy, 2.0);
  }
  else if (sizeIndex > player->sizeIndex)
  {
    // the second clause makes sure big fish always open their mouth if you are close so
    // that if you are bitten it plays the whole animation
    if (collisionCircle().collision(player->collisionCircle()) == true)
    {
      // ray is 2+ bigger
      if (sizeIndex > player->sizeIndex + 1)
      {
        Logical hit = player->attacked(getXY(), 100.0);
        
        if (hit == true)
        {
          // ResourceManager::playFishEat();
          FishSwallow* swallowEffect = new FishSwallow(player, collisionCircle().xy);
          level->addAction(swallowEffect);
          
          Point2 target = RM::attract(this->getXY(), player->mouthCircle().xy, sizeIndex * 16.0);
          level->addAction(new CosineFn(&xy.x, target.x, 0.20));
          level->addAction(new CosineFn(&xy.y, target.y, 0.20));
          
          script.enqueueX(new SpineAnimationAction(&mySpineAnim, "eat", 0, 0.2));
          script.enqueueX(new SpineAnimationAction(&mySpineAnim, "swim", 0, true));
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

Circle LakeStingray::mouthCircle()
{
  return Circle(getXY() + RM::pol_to_cart(96.0, getRotation() + PI), 16.0);
}

Circle LakeStingray::mouthCircle2()
{
  // return Circle(getXY() + RM::pol_to_cart(140.0, getRotation() + PI), 24.0);
  return mySpineAnim.calcAttachmentCircle("tail", "tail");
}

void LakeStingray::pickTarget()
{
  Point2 newTarget = RM::pol_to_cart(RM::randf(RM_WH.x * 0.2, RM_WH.x * 1.0), RM::rand_angle());
  
  Box smallerWorldBox = LAKE_WORLD_BOX;
  smallerWorldBox.grow_from_center(Point2(-256.0, -256.0));
  
  RM::bounce1_me(newTarget.x, smallerWorldBox.left(), smallerWorldBox.right());
  RM::bounce1_me(newTarget.y, smallerWorldBox.top(), smallerWorldBox.bottom());

  target = newTarget;
}

void LakeStingray::callback(ActionEvent* caller)
{
  
}

// =============================== LakeSpineclam ============================ //

LakeSpineclam::LakeSpineclam() :
  LakeGO(),

  jitter(&xy, Point2(32.0, 96.0), Point2(3.0, 8.0)),
  isClosed(false)
{
  setWH(64.0, 64.0);
  sizeIndex = SIZE_64;
  hflip(RM::randl());
  
  initSpineAnim(mySpineAnim, clamSpine, "swim_open", true);
  mySpineAnim.setDefaultMixDuration(0.1);
}

void LakeSpineclam::load()
{
  script.wait(RM::randf(0.1, 2.0));
}

void LakeSpineclam::updateMe()
{
  jitter.update();
  
  script.update();
  updateSpineAnim();
  
  updateCollisions();
  
  if (outsideWorld() == true)
  {
    level->placeAndAddLGO(new LakeSpineclam());
    setRemove();
  }
}

void LakeSpineclam::redrawMe()
{
  // warper.draw(warpWeight);
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    ColorP4 debugColor = isClosed ? RED_SOLID : WHITE_SOLID;
    collisionCircle().draw_outline(debugColor);
    
    if (isClosed == false)
    {
      topCircle().draw_outline(WHITE_SOLID);
    }
  }
}

void LakeSpineclam::updateCollisions()
{
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return;
  
  if (isClosed == true)
  {
    if (touchingPlayer() == true)
    {
      player->attacked(mouthCircle().xy, 2.0);
    }
  }
  else if (sizeIndex < player->sizeIndex &&
           playerNearEating() == true)
  {
    player->smallerNearby();

    if (touchingPlayerMouth() == true ||
        touchingPlayer(topCircle()) == true)
    {
      swallowedByPlayer();
      level->placeAndAddLGO(new LakeSpineclam());
    }
  }
}

Circle LakeSpineclam::topCircle()
{
  return Circle(getXY() + Point2(0.0, -getWidth()), getWidth() * 0.5);
}

void LakeSpineclam::callback(ActionEvent* caller)
{
  script.wait(2.0);
  script.enqueueX(new SpineAnimationAction(&mySpineAnim,
      "closed_opened_closed", 0, Point2(0.1333, 0.2666), 0.1333));
  script.enqueueX(new SetValueCommand<Logical>(&isClosed, true));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "swim_closed", 0, true));
  script.wait(2.0);
  script.enqueueX(new SpineAnimationAction(&mySpineAnim,
                                           "closed_opened_closed", 0, Point2(0.0, 0.1333), 0.1333));
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "swim_open", 0, true));
  script.enqueueX(new SetValueCommand<Logical>(&isClosed, false));
}

// ==================================== LakePearl ==================================//

LakePearl::LakePearl() :
  LakeGO(),
  jitter(this, Point2(8.0, 10.0), Point2(1.0, 1.2))
{
  setXY(-100.0, LAKE_WORLD_BOX.bottom() + 64.0);
  setWH(16.0, 16.0);
  
  initSpineAnim(mySpineAnim, planktonSpine, "swim", true);
  
  setRotation(PI * 1.5);
}

void LakePearl::updateMe()
{
  jitter.update();
  addY(-64.0 * RM::timePassed());
  
  updateSpineAnim();
  
  if (getY() < -128.0)
  {
    level->placeAndAddLGO(new LakePearl());
    setRemove();
  }
  else
  {
    updateCollisions();
  }
}

void LakePearl::updateCollisions()
{
  if (level->levelState != LEVEL_PLAY) return;
  
  if (playerNearEating() == true)
  {
    player->smallerNearby();
    
    if (touchingPlayerMouth() == true)
    {
      sizeIndex = player->sizeIndex * 2;
      swallowedByPlayer();
      level->placeAndAddLGO(new LakePearl());
    }
  }
}

void LakePearl::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_solid(GREEN_SOLID);
  }
}

void LakePearl::callback(ActionEvent* caller)
{
  
}

// =============================== LakeBarrel ============================ //

LakeBarrel::LakeBarrel() :
  LakeGO(),

  explodeTimer(1.5, this),
  fallSpeed(64.0),
  rotateSpeed(RM::randf(-TWO_PI, TWO_PI)),

  blinkTimer(0.2, this),
  blinkIndex(0)
{
  setWH(64.0, 64.0);
  
  blinkTimer.setInactive();
  explodeTimer.setInactive();
  
  initSpineAnim(mySpineAnim, barrelSpine, "swim", true);
  mySpineAnim.setDefaultMixDuration(0.1);
}

void LakeBarrel::updateMe()
{
  updateSpineAnim();
  addY(fallSpeed * RM::timePassed());
//  addRotation(rotateSpeed * RM::timePassed());
  explodeTimer.update();
  blinkTimer.update();
  updateCollisions();
}

void LakeBarrel::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    nearbyCircle().draw_outline(WHITE_SOLID);
    explodeCircle().draw_outline(ORANGE_SOLID);
    collisionCircle().draw_outline(RED_SOLID);
  }
}

void LakeBarrel::updateCollisions()
{
  if (getActive() == false)
  {
    return;
  }
  
  if (touchingPlayer() == true)
  {
    explode();
  }
  else if (explodeTimer.getActive() == false &&
           touchingPlayer(nearbyCircle()) == true)
  {
    explodeTimer.setActive();
    blinkTimer.setActive();

    mySpineAnim.startAnimation("expode", 0, 1.5);
  }
  else if (getY() >= LAKE_WORLD_BOX.bottom())
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
  
  level->placeAndAddLGO(new LakeBarrel());
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
    // setImage(imgsetCache[lakeBarrelSet][blinkIndex]);
    
    Point1 resetTime = explodeTimer.progress() < 0.66 ? 0.1 : 0.05;
    blinkTimer.reset(resetTime);
  }
}


// ==================================== PlaneGO ==================================//

PlaneGO::PlaneGO() :
  PhysRectangular(),
  BaseGOLogic(),
  PizzaSpineAnimatorStd(),
  RenderableParticleBreakerInf(),
  ActionListener(),

  level(NULL),
  player(NULL),

  actions(),
  script(this),

  hitThisAttack(false),
  destroyedPhysical(false),

  phase(PHASE_ENEMY_STD),
  type(-1),
  basePoints(0),
  topCoinGiven(-1),

  maxHP(0.9),
  currHP(maxHP),

  lastHitPt(0.0, 0.0),

  bobOffset(0.0),
  bobData(RM::randf(0.0, 1.0), 1.0)
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
  return level->rmboxCamera.myBox().collision(collisionBox());
}

Logical PlaneGO::attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg)
{
  if (getActive() == false) return false;
  
  currHP -= dmg;
  
  lastHitPt = getXY() + RM::pol_to_cart(collisionCircle().radius,
                                        RM::angle(getXY(), atkPoint));
  
  if (currHP <= 0.0)
  {
    createCoin();
    destroyMe();
    
    level->enemyDefeated(this);
    damagedCallback(atkPoint, attacker);
  }
  else
  {
    // esourceManager::playPlaneBulletHit();

    setBlue(0.0);
    setGreen(0.0);
  }
  
  return true;
}

void PlaneGO::destroyStd()
{
  destroyedPhysical = true;
}

void PlaneGO::updateDMGColor()
{
  RM::flatten_me(color.y, 1.0, 4.0 * RM::timePassed());
  color.z = color.y;
}

Point2 PlaneGO::yRange()
{
  return Point2(deviceSafeBox().top() + getHeight(),
                deviceSafeBox().bottom() - getHeight());
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
//  FishShatter* shatter = FishShatter::createForPlane(*this, breakCols, breakRows);
//  level->addDebris(shatter);
}

void PlaneGO::debugCircle(const Circle& circle, const ColorP4 outlineColor)
{
  if (Pizza::DRAW_DEBUG == true) circle.draw_outline(outlineColor);
}

Logical PlaneGO::explodeTouchingPlayer()
{
  if (getActive() == true && touchingPlayerCircle() == true)
  {
    player->attacked(getXY(), this);
    attacked(player->getXY(), player, 10.0);
    return true;
  }
  
  return false;
}

void PlaneGO::smokePuffStd()
{
  // level->addAction(new Smokeburst(collisionCircle().xy, false));
}

void PlaneGO::createCoinStd()
{
  if (topCoinGiven == -1) return;
  
  // level->addAlly(new PlaneCoin(getXY(), topCoinGiven));
}

Circle PlaneGO::collisionCircle()
{
  if (usingSpineAnim() == true)
  {
    return mySpineAnim.calcAttachmentCircle("body_collision", "body_collision");
  }
  else
  {
    return Circle(getXY(), getWidth());
  }
  
}

Circle PlaneGO::touchingMyCircle(const Circle& c)
{
  Circle result(collisionCircle());
  
  // this means no collision
  if (collisionCircle().collision(c) == false) result.radius = -1.0;
  
  return result;
}

VisRectangular* PlaneGO::get_visrect()
{
  return this;
}

Box PlaneGO::get_local_draw_AABB()
{
  if (usingSpineAnim() == true)
  {
    Box spineBox = mySpineAnim.calcSpineAllBoxesAABB();
    spineBox.xy -= getXY();
    return spineBox;
  }
  else if (getImage() == NULL)
  {
    Box startBox = collisionBox();
    startBox.xy -= getXY();
    return startBox;
  }
  
  // cout << "natural image size " << getImage()->natural_size() << endl;
  Box result = getImage()->calc_quad_coords(getXY(), getScale(), getRotation(), Quad::unit_square()).to_AABB();
  // cout << "absolute box " << result << endl;
  result.xy -= getXY();
  
  return result;
}

void PlaneGO::render_for_particles()
{
  redrawMe();
}

Logical PlaneGO::visualAABBFullyOnscreen()
{
  Box aabb = get_local_draw_AABB();
  aabb.xy += getXY();
  return Box::encloses(deviceSafeBox(), aabb);
}

void PlaneGO::particles_ready(BoneBreakEffect* breakEffect)
{
  if (lastHitPt.x <= 0.0 && lastHitPt.y <= 0.0)
  {
    lastHitPt = getXY();
  }
  else
  {
    // cout << "last hit set to " << lastHitPt << endl;
  }
  
  breakEffect->setVelFromPt(lastHitPt, RM::randf(150.0, 300.0));
  level->addAction(breakEffect);
}

void PlaneGO::predraw_call()
{
  if (usingSpineAnim() == true)
  {
    mySpineAnim.syncFromVisRect(*this, true);  // sets the rotation to 0 before getting the AABB
  }
}

void PlaneGO::postdraw_call()
{
  // this puts the "spine" back where it was in case that position was important
  if (usingSpineAnim() == true)
  {
    mySpineAnim.syncFromVisRect(*this, true);
  }
}

void PlaneGO::checkPhysicalDestruction()
{
  if (lifeState != RM::ACTIVE) return;
  
  if (destroyedPhysical == true)
  {
    destroyPhysical();
  }
}

void PlaneGO::destroyPhysical()
{
  if (getActive() == false)
  {
    return;
  }
  
  // destructionStarted();
  
  /*
  if (body != NULL)
  {
    // destroying the body instead can cause a crash because this function can get called
    // while the b2d world is locked
    set_collisions(0x0, 0x0);
  }
   */
  
  if (usingSpineAnim() == true)
  {
    mySpineAnim.hideSpecialBlendSlots = true;
  }
  
  lifeState = RM::TRANS_OUT;
  
  CreateRenderedParticlesAction* deathParticles = new CreateRenderedParticlesAction(this);
  
  // cout << "breaking" << endl;
  // dropCoin();
  
  ActionQueue* dieScript = new ActionQueue();
  dieScript->enqueueX(deathParticles);
  dieScript->enqueueX(new SetValueCommand<Coord1>(&lifeState, RM::REMOVE));
  level->addAction(dieScript);
  // cout << "script length " << script.length() << " " << &script << endl;
}


// =============================== PlanePlayer ============================ //

PlanePlayer::PlanePlayer() :
  PlaneGO(),

  face(NULL),

//  warper(this, dwarpCache[planeWarp]),
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
  
  face = new PizzaFace(this);

  setImage(AnimationBehavior::pickImage(getImgSet(planePlayerSet), 0.5));
}

PlanePlayer::~PlanePlayer()
{
  delete face;
}

void PlanePlayer::load()
{
  initSpineAnim(planePlayerSpine, "fly_up", true);
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 1, "thruster", 1);
}

void PlanePlayer::updateMe()
{
  flashList.update();
  cooldownTimer.update();
  RM::bounce_arcsine(warpWeight, warpData, Point2(0.0, 1.0), 2.0 * RM::timePassed());
  
  updateTilting();
  updateSpineAnim(mySpineAnim, 1.0);

  face->updateFace();

  // updatePropeller();
}

void PlanePlayer::redrawMe()
{
  /*
  setImage(imgCache[planeFront]);
  warper.draw(1, 2, warpWeight);
  
  setImage(imgsetCache[planePropSet][propIndex]);
  drawMe();
  changePropIndex = true;
  
  flashList.redraw();
   */
  renderSpineAnim(mySpineAnim);
  
  setScale(0.06, 0.06);
  Point2 offset = RM::rotate(Point2(29.0, -9.0), getRotation());
  addXY(offset);
  VisRectangular::drawImg(this, getImg(pizzaBaseImg));
  face->drawFace();
  addXY(-offset);
  setScale(1.0, 1.0);

  drawMe();

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
  
  Point1 tiltWeight = RM::double_lerp(getRotation() + TWO_PI,
                                      Point2(TWO_PI - PI / 12.0, TWO_PI + PI / 12.0),
                                      Point2(0.001, 0.999));
  setImage(AnimationBehavior::pickImage(getImgSet(planePlayerSet), tiltWeight));
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
  RM::clamp_me(xy.y, deviceScreenBox().top() + getHeight(),
               deviceScreenBox().bottom() - getHeight());
}

void PlanePlayer::tryFire()
{
  if (cooldownTimer.getActive() == false)
  {
    PlanePlayerBullet* bullet = new PlanePlayerBullet();
    bullet->setXY(getXY() + RM::pol_to_cart(96.0, getRotation()));
    bullet->vel = RM::pol_to_cart(650.0, getRotation());
    bullet->setRotation(getRotation());
    level->addPGO(bullet);
    
    flashList.addX(new MuzzleFlash(this));
    // ResourceManager::playPlaneFire();
    
    cooldownTimer.reset();
  }
}

Logical PlanePlayer::attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg)
{
  if (Pizza::DEBUG_INVINCIBLE == true) return false;
  // if (face.isHurt() == true) return false;
  if (level->levelState != PizzaLevelInterface::LEVEL_PLAY) return false;
  
  // face.actionTrigger(PizzaFace::HURT);
  level->smallShake();
//  level->addSauceEffect(*this, atkPoint);

//  ResourceManager::playDamaged();
  activeGameplayData->addHP(-dmg);

  return true;
}

Box PlanePlayer::collisionBox()
{
  return Box::from_center(getXY(), getSize() * 1.75);
}

Circle PlanePlayer::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

void PlanePlayer::callback(ActionEvent* caller)
{

}

// =============================== MuzzleFlash ============================ //

MuzzleFlash::MuzzleFlash(VisRectangular* setParent) :
//  ImageEffect(RM::random(imgsetCache[planeMuzzleSet])),
  ImageEffect(),
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

void MuzzleFlash::redrawMe()
{
//  ImageEffect::redrawMe();
}

// =============================== PlanePlayerBullet ============================ //

PlanePlayerBullet::PlanePlayerBullet() :
  PlaneGO(),
  vel(0.0, 0.0)
{
  phase = PHASE_ALLY_STD;
  flags = 0x0;
  setWH(8.0, 8.0);
  hflip(false);  // override parent ctor
  setImage(getImg(planeBulletImg));
}

void PlanePlayerBullet::load()
{
  init_circle(getXY(), getWidth(), false);
  body->SetType(b2_kinematicBody);
  set_collisions(0x1 << TYPE_PLAYER, 0xffff);
  set_b2d_callback_data(this);
}

void PlanePlayerBullet::updateMe()
{
  addXY(vel * RM::timePassed());
  
  body->SetLinearVelocity(wpt_to_b2d(vel));
  updateFromWorld();
  
  updateCollisions();
  
  if (onScreen() == false)
  {
    setRemove();
  }
}

void PlanePlayerBullet::redrawMe()
{
  drawMe();
  debugCircle(collisionCircle(), BLUE_SOLID);
}

void PlanePlayerBullet::createExplosion(const Point2& enemyCircleXY)
{
  Point2 explodePt = getXY() + RM::pol_to_cart(getWidth(), RM::angle(getXY(), enemyCircleXY));
  
  /*
  BombExplosion* explode = new BombExplosion(explodePt);
  explode->setScale(0.1, 0.1);
  explode->scaler.reset(0.25, 0.35);
  explode->beforeFadeTimer.reset(0.10);
  explode->fader.reset(0.0, 0.25);
  
  level->addAction(explode);
   */
}

void PlanePlayerBullet::updateCollisions()
{
  for (LinkedListNode<PlaneGO*>* enemyNode = level->objman.phaseList[PHASE_ENEMY_STD].first;
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

// =============================== PlaneMohawk ============================ //

PlaneMohawk::PlaneMohawk() :
  PlaneGO()
{
  basePoints = 10;
  
  setWH(24.0, 24.0);
  setScale(0.5, 0.5);
  initSpineAnim(mohawkSpine, "idle");
}

void PlaneMohawk::updateMe()
{
  Point1 motion = 225.0 * facingToDir() * RM::timePassed();
  addX(motion);
  addRotation(0.5 * RM::linear_to_rads(motion, getWidth()));
  
  if (getX() < level->rmboxCamera.myBox().left() - 96.0)
  {
    setRemove();
  }
  
  updateDMGStd();
  
  updateSpineAnim();
  checkPhysicalDestruction();
}

void PlaneMohawk::redrawMe()
{
  renderSpineAnim();
  
  debugCircle(collisionCircle(), RED_SOLID);
}

Circle PlaneMohawk::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== PlaneCupid ============================ //

PlaneCupid::PlaneCupid() :
  PlaneGO()
{
  currHP = 5.9;
  
  setWH(48.0, 48.0);
}

void PlaneCupid::load()
{
  initSpineAnim(cupidSpine, "idle", true);
  spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 1, "wingflap", 1);
  
  script.enqueueX(new ArcsineFn(&xy.x, deviceSafeBox().right() - getSize().x -
                                RM::randf(32.0, 64.0), 1.0));
}

void PlaneCupid::updateMe()
{
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  
  script.update();
  actions.update();
  
  updateSpineAnim(mySpineAnim, 1.0);
  updateDMGColor();
  
  checkPhysicalDestruction();
}

void PlaneCupid::redrawMe()
{
  addY(bobOffset);
  renderSpineAnim(mySpineAnim);
  addY(-bobOffset);
  
  drawDebugStd();
}

void PlaneCupid::midCallback(Coord1 value)
{
  Point2 firePoint = getXY() + Point2(87.0 * facingToDir(), -25.0);
  Point2 arrowVel = RM::pol_to_cart(-1000.0, getRotation());
  
  PlaneCupidArrow* arrow = new PlaneCupidArrow(firePoint, arrowVel);
  level->addPGO(arrow);
}

void PlaneCupid::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    spAnimationState_setAnimationByName(mySpineAnim.spineAnimState, 0, "idle", 1);
    
    script.enqueueX(new ArcsineFn(&xy.y, randYRanged(), 1.0));
    script.enqueueAddX(new SpineAnimationAction(&mySpineAnim, "fire", 0, 1.0), &actions);
    script.wait(0.5);
    script.enqueueMidCallback(this, 0);
    script.wait(0.5);  // fire animation over after script
  }
}

// =============================== PlaneCupidArrow ============================ //

PlaneCupidArrow::PlaneCupidArrow(Point2 origin, Point2 setVel) :
  PlaneGO(),

  stickBehavior(this),
  vel(setVel)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  currHP = 1.9;
  
  stickBehavior.setListener(this);
  setImage(getImg(cupidArrowImg));
  setXY(origin);
}

void PlaneCupidArrow::updateMe()
{
  checkPhysicalDestruction();
  
  if (lifeState == RM::ACTIVE)
  {
    addXY(vel * RM::timePassed());
    
    if (onScreen() == false)
    {
      setRemove();
    }
    
    updateCollisions();
  }
  else if (lifeState == RM::PASSIVE)
  {
    stickBehavior.update();
    setAlpha(1.0 - RM::clamp((stickBehavior.percentDone() - 0.5) / 0.5, 0.0, 1.0));
  }
  
  updateDMGStd();
}

void PlaneCupidArrow::redrawMe()
{
  drawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(ColorP4(1.0, 0.0, 0.0, 1.0));
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

// =============================== PlaneBombbat ============================ //

PlaneBombbat::PlaneBombbat() :
  PlaneGO(),

  speed(128.0),

  wingAnimator(),
  wingPt(0.0, 0.0)
{
  currHP = 1.9;
  
  setWH(48.0, 48.0);
}

void PlaneBombbat::load()
{
  useFishRotation();
  setRotation(RM::angle(getXY(), player->getXY()));
  
  initSpineAnim(batSpine, "idle", true);
  initSpineAnim(wingAnimator, batSpine, "flap", true);
  
  // this keeps the wings especially from flashing at (0.0, 0.0)
  mySpineAnim.syncFromVisRect(*this);
  wingAnimator.syncFromVisRect(*this);
  syncWingPt();
}

void PlaneBombbat::updateMe()
{
  checkPushOnscreenY();
  
  RM::attract_me(xy, player->getXY(), speed * RM::timePassed());
  setRotation(RM::angle(getXY(), player->getXY()));

  Logical explodedFromPlayer = explodeTouchingPlayer();
  
  if (explodedFromPlayer == true)
  {
    createExplodeEffect();
  }
  
  RM::bounce_arcsine(bobOffset, bobData, Point2(-8.0, 8.0), 2.0 * RM::timePassed());
  
  script.update();
  actions.update();
  
  updateDMGColor();
  updateSpineAnim();
  updateSpineAnim(wingAnimator);
  
  checkPhysicalDestruction();

  syncWingPt();
}

void PlaneBombbat::redrawMe()
{
  addY(bobOffset);

  Point2 realXY = getXY();
  Point1 realRotation = getRotation();
  Logical realFish = fishRotationActive();
//  setXY(wingPt);
  useFishRotation(false);
  setRotation(0.0);
  renderSpineAnim(wingAnimator);
  
  setXY(realXY);
  useFishRotation(realFish);
  setRotation(realRotation);
  renderSpineAnim();
  
  addY(-bobOffset);

  drawDebugStd();
}

void PlaneBombbat::createExplodeEffect()
{
  ImageEffect* bombExplode = new ImageEffect(getImg(batExplosionImg));
  bombExplode->setXY(getXY());
  bombExplode->enqueueX(new LinearFn(&bombExplode->scale.x, 1.0, 0.05));
  bombExplode->script.wait(0.5);
  bombExplode->enqueueX(new LinearFn(&bombExplode->color.w, 0.0, 0.15));
  level->addAction(bombExplode);
}

void PlaneBombbat::destroyPhysical()
{
  PlaneGO::destroyPhysical();
  
  createExplodeEffect();
}

void PlaneBombbat::checkPushOnscreenY()
{
  Logical fullyOnscreen = visualAABBFullyOnscreen();

  if (fullyOnscreen == false)
  {
    RM::flatten_me(xy.y, deviceSafeBox().center().y, 256.0 * RM::timePassed());
  }
}

void PlaneBombbat::syncWingPt()
{
  wingPt = mySpineAnim.calcAttachmentCircle("body_collision", "body_collision").xy + Point2(0.0, 16.0);
}

Circle PlaneBombbat::collisionCircle()
{
  return Circle(getXY(), 24.0);
}

Box PlaneBombbat::get_local_draw_AABB()
{
  Box spineBox = mySpineAnim.calcSpineAllBoxesAABB();
  // spineBox.grow_from_center(Point2(256.0, 96.0));
  spineBox.xy -= getXY();
  return spineBox;
}

// =============================== PlaneChip ============================ //

PlaneChip::PlaneChip() :
  PlaneGO(),

  xVel(0.0),
  yMover(&xy.y, 0.0, 1.0)
{
  currHP = 1.9;
  
  setWH(32.0, 32.0);
}

void PlaneChip::load()
{
  Point1 travelSeconds = 2.5;
  xVel = -1.0 * (getX() - deviceScreenBox().left()) / travelSeconds;
  yMover.reset(player->getY(), travelSeconds);
  
  initSpineAnim(chipSpine, "idle");
}

void PlaneChip::updateMe()
{
  addX(xVel * RM::timePassed());
  yMover.update();
  
  script.update();
  actions.update();
  
  updateDMGStd();
  updateSpineAnim();
  
  if (getActive() == true &&
      collisionCircle().right() + 64.0 <= deviceScreenBox().left())
  {
    setRemove();
  }
  
  checkPhysicalDestruction();
}

void PlaneChip::redrawMe()
{
  renderSpineAnim();
  
  drawDebugStd();
}

// =============================== PlaneDonut ============================ //

PlaneDonut::PlaneDonut() :
  PlaneGO(),

  xVel(0.0)
{
  currHP = 3.9;
}

void PlaneDonut::load()
{
  setWH(32.0, 32.0);  // radius
  setScale(0.5, 0.5);
  
  initSpineAnim(donutSpine, "walk_combined", true);

  setWH(collisionCircle().radius, collisionCircle().radius);  // radius

  callback(&script);

}

void PlaneDonut::updateMe()
{
  addX(xVel * RM::timePassed());
  RM::flatten_me(xVel, 0.0, 50.0 * RM::timePassed());
  addY(64.0 * RM::timePassed());
  
  updateSpineAnim();

  script.update();
  actions.update();
  
  updateDMGColor();
  updateSpineAnim();
  
  checkPhysicalDestruction();
  
  if (getActive() == true &&
      collisionCircle().top() >= deviceScreenBox().bottom())
  {
    setRemove();
  }
}

void PlaneDonut::redrawMe()
{
  renderSpineAnim();

  drawDebugStd();
}

void PlaneDonut::midCallback(Coord1 value)
{
  switch (value)
  {
    default:
    case 0:
    {
      xVel = 50.0;
      
      PlaneDonutHoleCannonball* ball = new PlaneDonutHoleCannonball();
      ball->setXY(getXY());
      level->addPGO(ball);
      break;
    }
  }
}


void PlaneDonut::callback(ActionEvent* caller)
{
  mySpineAnim.startAnimation("walk_combined", 0, true);

  script.wait(1.0);
  // i don't know why i have to set it to take 2 seconds, it should only be 1
  script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_combined", 0, 2.0));
  script.wait(0.5);
  script.enqueueMidCallback(this, 1);
  script.wait(0.5);
}

// =============================== PlaneDonutHoleCannonball ============================ //

PlaneDonutHoleCannonball::PlaneDonutHoleCannonball() :
  PlaneGO(),

  vel(-500.0, -100.0)
{
  phase = PHASE_ENEMY_STD;
  type = TYPE_ENEMY;
  
  setImage(getImg(donutHoleImg));
  
  autoSize(0.25);  // radius
  setScale(0.5, 0.5);
}

void PlaneDonutHoleCannonball::load()
{

}

void PlaneDonutHoleCannonball::updateMe()
{
  vel.y += 200.0 * RM::timePassed();
  addXY(vel * RM::timePassed());
  
  checkPhysicalDestruction();
  
  if (getActive() == true &&
      onScreen() == false)
  {
    setRemove();
  }
  
  updateDMGStd();
}


void PlaneDonutHoleCannonball::redrawMe()
{
  drawMe();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    collisionCircle().draw_outline(WHITE_SOLID);
  }
}
void PlaneDonutHoleCannonball::callback(ActionEvent* caller)
{
  if (lifeState != RM::ACTIVE) return;
  
  destroyPhysical();
}


// =============================== PlaneChicken ============================ //

PlaneChicken::PlaneChicken() :
  PlaneGO(),

  vel(0.0, 0.0)
{
  currHP = 2.9;
}

void PlaneChicken::load()
{
  setWH(64.0, 64.0);  // radius
  initSpineAnim(chickenSpine, "idle");
}

void PlaneChicken::updateMe()
{
  addXY(vel * RM::timePassed());
  vel.x += -125.0 * RM::timePassed();
  
  script.update();
  actions.update();
  
  updateDMGStd();
  updateSpineAnim();
  
  if (getActive() &&
      collisionCircle().right() + 64.0 <= deviceScreenBox().left())
  {
    setRemove();
  }
  
  checkPhysicalDestruction();
}

void PlaneChicken::redrawMe()
{
  renderSpineAnim();
  
  drawDebugStd();
}

Circle PlaneChicken::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

// =============================== PlaneFlameskull ============================ //

PlaneFlameskull::PlaneFlameskull() :
  PlaneGO()
{
  basePoints = 10;
  // topCoinGiven = PizzaGO::PENNY;

  setWH(24.0, 24.0);
  setScale(0.5, 0.5);
  initSpineAnim(flameskullSpine, "idle");

  currHP = 9.9;
}

void PlaneFlameskull::updateMe()
{
  Point1 motion = 300.0 * facingToDir() * RM::timePassed();
  addX(motion);
  
  if (getX() < level->rmboxCamera.myBox().left() - 96.0)
  {
    setRemove();
  }
  
  updateSpineAnim();

  updateDMGStd();
  
  checkPhysicalDestruction();
}

void PlaneFlameskull::redrawMe()
{
  renderSpineAnim();

  debugCircle(collisionCircle(), RED_SOLID);
}

// =============================== PlaneSpider ============================ //

PlaneSpider::PlaneSpider() :
  PlaneGO(),

  startsBottom(true)
{
  basePoints = 10;
  // topCoinGiven = PizzaGO::PENNY;
  
  setWH(28.0, 28.0);
  setScale(0.5, 0.5);
  
  initSpineAnim(jumpingSpiderSpine, "walk_teethdown");
  mySpineAnim.setDefaultMixDuration(0.1);

  currHP = 1.9;
}

void PlaneSpider::load()
{
  startsBottom = getY() > deviceSafeBox().center().y;
  
  if (startsBottom == false)
  {
    setRotation(PI);
  }
}

void PlaneSpider::updateMe()
{
  Point2 oldXY = getXY();
  RM::flatten_me(xy.x, player->getX(), 150.0 * RM::timePassed());

  updateAI(oldXY);
  updateSpineAnim();

  script.update();
  actions.update();
  
  updateDMGStd();
  
  checkPhysicalDestruction();
}

void PlaneSpider::redrawMe()
{
  renderSpineAnim();
  
  debugCircle(collisionCircle(), RED_SOLID);
}

void PlaneSpider::updateAI(Point2 oldXY)
{
  // turn when halfway to player
  Point1 halfwayPt = RM::lerp(player->getX(), deviceSafeBox().right(), 0.5);
  Logical wasPassedHalfway = oldXY.x <= halfwayPt;
  Logical isPassedHalfway = getX() <= halfwayPt;
  
  if (getActive() == true &&
      wasPassedHalfway == false &&
      isPassedHalfway == true)
  {
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "flip", 0, Point2(32.0 / 60.0, -1.0), 0.5));
    script.wait(0.5);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "walk_teethup", 0, true));
    script.wait(2.0);
  }
  
  // jump when reaching player
  Logical wasToPlayer = RM::approxEq(oldXY.x, player->getX());
  Logical isToPlayer = RM::approxEq(getX(), player->getX());
  
  if (getActive() == true &&
      wasToPlayer == false &&
      isToPlayer == true)
  {
    Point1 D_FPS = 60.0;
    
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "jump_teethup", 0, Point2(0.0, 16.0 / D_FPS), 0.1));
    script.wait(0.1);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "jump_teethup", 0, Point2(-1.0, 32.0 / D_FPS), 0.1));
    script.enqueueAddX(new LinearPr(&xy.y, 350.0 * (startsBottom ? -1.0 : 1.0)), &actions);
  }
  
}

Circle PlaneSpider::collisionCircle()
{
  return Circle(getXY() + Point2(0.0, 24.0 * (startsBottom ? -1.0 : 1.0)), getWidth());
}

void PlaneSpider::callback(ActionEvent* caller)
{
  
}

// =============================== PlaneWisp ============================ //

PlaneWisp::PlaneWisp() :
  PlaneGO(),

  mover(this),
  stateTimer(2.0, this),

  currState(INVISIBLE),
  blinkCount(0)
{
  basePoints = 70;
  setWH(12.0, 12.0);   // half size
}

void PlaneWisp::load()
{
  Point1 moveTime = RM::randf(1.5, 2.0);
  
  Point1 leftBound = RM::lerp(deviceSafeBox().left(), deviceSafeBox().right(), 0.7);
  Point1 topBound = deviceSafeBox().top() + 48.0;
  Point1 rightBound = RM::lerp(deviceSafeBox().left(), deviceSafeBox().right(), 0.95);
  Point1 botBound = deviceSafeBox().bottom() - 48.0;

  
  Quad bounds(Point2(leftBound, topBound), Point2(rightBound, topBound),
              Point2(rightBound, botBound), Point2(leftBound, botBound));
  
  mover.init(bounds, Point2(moveTime, moveTime));
  
  initSpineAnim(wispSpine, "idle_invisible", true);
  
  callback(&script);
}

void PlaneWisp::updateMe()
{
  // stateTimer.update();
  mover.update();
  
  actions.update();
  script.update();
  
  updateDMGStd();
  checkPhysicalDestruction();

  updateSpineAnim();
}

void PlaneWisp::redrawMe()
{
  renderSpineAnim();
  
  if (Pizza::DRAW_DEBUG == true)
  {
    ColorP4 debugColor = currState == VISIBLE ? WHITE_SOLID : BLUE_SOLID;
    collisionCircle().draw_outline(debugColor);
    
    mover.bounds.draw_outline(BLUE_SOLID);
  }
}

Logical PlaneWisp::attacked(Point2 atkPoint, PlaneGO* attacker, Point1 dmg)
{
  if (currState == INVISIBLE)
  {
    return false;
  }
  
  return PlaneGO::attacked(atkPoint, attacker, dmg);
}

void PlaneWisp::midCallback(Coord1 value)
{
  PlaneWispFlame* flame = new PlaneWispFlame(getXY(), player->getXY());
  level->addPGO(flame);
}

void PlaneWisp::callback(ActionEvent* caller)
{
  if (caller == &script)
  {
    Point1 waitTime = 1.0;
    
    // invis, eyes open
    currState = INVISIBLE;
    setSpineAnim(0, "idle_invisible", true);
    script.wait(waitTime);
    
    // invis, eyes closed
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.0, 0.48), 2.0));
    script.wait(waitTime);
    
    // invis, open eyes to fire
    script.enqueueMidCallback(this, 0);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.48, -1.0), 0.25));
    script.wait(0.25);

    // invis, eyes open
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_invisible", 0, true));
    script.wait(waitTime);
    
    // invis, eyes closed
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.0, 0.48), 2.0));
    script.wait(waitTime);
    
    // invis, open eyes to fire 0.25 second
    script.enqueueMidCallback(this, 0);
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "shoot_invisible", 0, Point2(0.48, -1.0), 0.25));
    script.wait(0.25);
    
    // invis, eyes open
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_invisible", 0, true));
    script.wait(waitTime);
    
    // visible, eyes open 2 seconds
    script.enqueueX(new StartSpineCommand(&mySpineAnim, "idle_visible", 0, true));
    script.enqueueX(new SetValueCommand<Coord1>(&currState, VISIBLE));
    script.wait(waitTime * 2.0);
    
  }
}

// =============================== PlaneWispFlame ============================ //

PlaneWispFlame::PlaneWispFlame(Point2 center, Point2 target) :
  PlaneGO(),

  lifeTimer(5.0, this),
  vel(RM::ring_edge(center, target, 275.0))
{
  phase = PHASE_ALLY_STD;  // ally so that it doesn't affect camera or spawning
  type = TYPE_ENEMY;
  setBox(center.x, center.y, 6.0, 6.0);
  setRotation(RM::angle(vel));
  
  initSpineAnim(wispFireSpine, "animation", true);
}

void PlaneWispFlame::updateMe()
{
  updateSpineAnim();

  lifeTimer.update();
  if (lifeState == RM::PASSIVE) setAlpha(1.0 - lifeTimer.progress());
  
  addXY(vel * RM::timePassed());
  
  if (getActive() == true)
  {
    if (onScreen() == false)
    {
      dissipate();
    }
    else if (touchingPlayerCircle() == true)
    {
      player->attacked(getXY(), this);
      dissipate();
    }
  }
}

void PlaneWispFlame::redrawMe()
{
  renderSpineAnim();
  
  debugCircle(collisionCircle(), RED_SOLID);
}

void PlaneWispFlame::dissipate()
{
  if (getActive() == false) return;
  
  setPassive();
  vel.set(0.0, 0.0);
  lifeTimer.reset(0.2);
}

Circle PlaneWispFlame::collisionCircle()
{
  return Circle(getXY(), getWidth());
}

void PlaneWispFlame::callback(ActionEvent* caller)
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
