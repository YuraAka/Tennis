#include "GameScene.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <Box2D/Common/b2Settings.h>

#pragma warning (disable : 4355) // this in ctor
#define THROW_UNLESS(expr) ThrowUnless(!!expr, #expr)

namespace
{
  using namespace cocos2d;

  void ThrowUnless(bool cond, const std::string& msg)
  {
    if (!cond)
    {
      throw std::logic_error(msg);
    }
  }

  bool IsContain(const CCSize& srcRect, const CCPoint& srcCenter, const CCPoint& targetPoint)
  {
    return 
      (targetPoint.x > srcCenter.x - srcRect.width / 2)
      && (targetPoint.x < srcCenter.x + srcRect.width / 2)
      && (targetPoint.y > srcCenter.y - srcRect.height / 2)
      && (targetPoint.y < srcCenter.y + srcRect.height / 2);
  }

  bool IsContain(const CCSize& rect, const CCPoint& point)
  {
    return IsContain(rect, ccp(rect.width / 2, rect.height / 2), point);
  }

  bool IsContain(const CCSize& srcRect, const CCPoint& destCenter, const CCSize& destRect)
  {
    return 
      IsContain(srcRect, ccp(destCenter.x + destRect.width / 2, destCenter.y + destRect.height / 2))
      || IsContain(srcRect, ccp(destCenter.x - destRect.width / 2, destCenter.y + destRect.height / 2))
      || IsContain(srcRect, ccp(destCenter.x + destRect.width / 2, destCenter.y - destRect.height / 2))
      || IsContain(srcRect, ccp(destCenter.x - destRect.width / 2, destCenter.y - destRect.height / 2));
  }

  CCSprite& CreateSprite(const std::string& picturePath)
  {
    CCSprite* picture = CCSprite::create(picturePath.c_str());
    THROW_UNLESS(picture);
    //picture->setVisible(false);
    return *picture;
  }

  void AddEarth(CCPoint pos, const GameContext& context)
  {
    const Coordinator& coords = *context.Coords;
    b2BodyDef def;
    def.type = b2_staticBody;
    def.allowSleep = false;
    def.position = coords.ToBox2d(pos);
    b2Body* earth = context.World->CreateBody(&def);
    b2PolygonShape shape;
    shape.SetAsBox(coords.ToBox2d(pos.x), coords.ToBox2d(5));
    b2FixtureDef fixDef;
    fixDef.density = 0;
    fixDef.restitution = 1;
    fixDef.shape = &shape;
    fixDef.friction = 0;
    earth->CreateFixture(&fixDef);
  }
  
  void CreateEarth(const GameContext& context)
  {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    const Coordinator& coords = *context.Coords;
    AddEarth(ccp(winSize.width / 2, -5), context);
    AddEarth(ccp(winSize.width / 2, winSize.height + 5), context);
  }
}

b2FixtureDef CreateStandartFixtureDefinition(Coordinator::Ptr coords)
{
  b2FixtureDef def;
  def.restitution = 1;
  def.friction = 0;
  return def;
}

GameContext::GameContext(CCLayer& myLayer, Coordinator::Ptr myCoords, b2WorldPtr myWorld)
  : Layer(myLayer)
  , Coords(myCoords)
  , World(myWorld)
  , WindowSize(CCDirector::sharedDirector()->getWinSize())
  , BodyFixture(CreateStandartFixtureDefinition(myCoords))
{
}

Coordinator::Coordinator()
  : Koeff(CCDirector::sharedDirector()->getWinSize().width / 5)
{
  THROW_UNLESS(int(Koeff));
}

float Coordinator::ToBox2d(float cocos2dPos) const
{
  return cocos2dPos / Koeff;
}

float Coordinator::ToCocos2d(float box2dPos) const
{
  return box2dPos * Koeff;
}

b2Vec2 Coordinator::ToBox2d(const CCPoint& cocos2dPoint) const
{
  return b2Vec2(ToBox2d(cocos2dPoint.x), ToBox2d(cocos2dPoint.y));
}

b2Vec2 Coordinator::ToBox2d(const CCSize& size) const
{
  return b2Vec2(ToBox2d(size.width), ToBox2d(size.height));
}

float Coordinator::GetTransformKoeff() const
{
  return Koeff;
}

CCPoint Coordinator::ToCocos2d(const b2Vec2& pos) const
{
  return CCPoint(pos.x * Koeff, pos.y * Koeff);
}

// TennisBall
TennisBall::TennisBall(GameContext::Ptr ctx)
  : Sprite(CreateSprite("Ball.png"))
  , Body(NULL)
  , Ctx(ctx)
  , StartPosition(ccp(Ctx->WindowSize.width / 2, Ctx->WindowSize.height / 2))
{
  Sprite.setPosition(StartPosition);
  Ctx->Layer.addChild(&Sprite, 1);

  b2BodyDef bodyDef;
  bodyDef.position = Ctx->Coords->ToBox2d(StartPosition);
  bodyDef.type = b2_dynamicBody;
  bodyDef.fixedRotation = true;
  Body = Ctx->World->CreateBody(&bodyDef);

  b2CircleShape shape;
  shape.m_radius = Ctx->Coords->ToBox2d(Sprite.getContentSize().width) / 2;
  b2FixtureDef fixDef(Ctx->BodyFixture);
  fixDef.shape = &shape;
  fixDef.density = Ctx->Coords->ToBox2d(100);
  Body->CreateFixture(&fixDef);
}

void TennisBall::SetOutOfScreenCallback(TennisBallCallback callback)
{
  OnOutOfScreenCallback = callback;
}

void TennisBall::Update()
{
  if (!IsContain(Ctx->WindowSize, Sprite.getPosition(), Sprite.getContentSize()))
  {
    return;
  }

  CCPoint pos = Ctx->Coords->ToCocos2d(Body->GetPosition());
  Sprite.setPosition(pos);
  if (OnOutOfScreenCallback && !IsContain(Ctx->WindowSize, pos, Sprite.getContentSize()))
  {
    OnOutOfScreenCallback(pos);
  }
}

void TennisBall::Run()
{
  Sprite.setPosition(StartPosition);
  Body->SetTransform(Ctx->Coords->ToBox2d(StartPosition), 0);
  Body->SetLinearVelocity(b2Vec2(0, 0));
  Body->ApplyLinearImpulse(Ctx->Coords->ToBox2d(ccp(-2, -1)), Ctx->Coords->ToBox2d(StartPosition));
}

// TennisPlayer
TennisPlayer::TennisPlayer(bool isLeft, GameContext::Ptr ctx)
  : Sprite(CreateSprite("Player.png"))
  , UpperBound(ctx->WindowSize.height - Sprite.getContentSize().height / 2)
  , LowerBound(Sprite.getContentSize().height / 2)
  , Ctx(ctx)
{
  CCPoint pos(
    isLeft ? Sprite.getContentSize().width / 2 : (Ctx->WindowSize.width - Sprite.getContentSize().width / 2)
    , Ctx->WindowSize.height / 2
  );

  Sprite.setPosition(pos);
  Sprite.setFlipX(!isLeft);
  Ctx->Layer.addChild(&Sprite, 1);

  b2BodyDef bodyDef;
  bodyDef.allowSleep = true;
  bodyDef.type = b2_staticBody;
  bodyDef.position = Ctx->Coords->ToBox2d(pos);
  Body = Ctx->World->CreateBody(&bodyDef);
  b2PolygonShape shape;
  b2Vec2 size = Ctx->Coords->ToBox2d(Sprite.getContentSize());
  std::vector<b2Vec2> shapePoints;
  float reflectKoeff = isLeft ? 1 : -1;
  shapePoints.push_back(b2Vec2(-size.x / 2, -size.y / 2));
  shapePoints.push_back(b2Vec2(size.x / 2, -size.y / 4));
  shapePoints.push_back(b2Vec2(size.x / 2, size.y / 4));
  shapePoints.push_back(b2Vec2(-size.x / 2, size.y / 2));
  if (!isLeft)
  {
    std::reverse(shapePoints.begin(), shapePoints.end());
    for (std::size_t i = 0; i < shapePoints.size(); ++i)
    {
      shapePoints[i].x = reflectKoeff * shapePoints[i].x;
    }
  }

  shape.Set(&shapePoints[0], shapePoints.size());
  b2FixtureDef fixtureDef(Ctx->BodyFixture);
  fixtureDef.shape = &shape;
  fixtureDef.restitution = 1.2f;
  Body->CreateFixture(&fixtureDef);
}

void TennisPlayer::Move(const float dy)
{
  float playerNewY = Sprite.getPositionY() + dy;
  playerNewY = std::max(playerNewY, LowerBound);
  playerNewY = std::min(playerNewY, UpperBound);
  Sprite.setPositionY(playerNewY);

  b2Vec2 bodyPos = Body->GetPosition();
  float bodyAngle = Body->GetAngle();
  bodyPos.y = Ctx->Coords->ToBox2d(playerNewY);
  Body->SetTransform(bodyPos, bodyAngle);
}

bool TennisPlayer::IsContain(const cocos2d::CCPoint& point) const
{
  return ::IsContain(Sprite.getContentSize(), Sprite.getPosition(), point);
}

TennisGame::TennisGame()
  : Ctx(CreateContext())
  , DbgDraw(Ctx->Coords->GetTransformKoeff())
{
  DbgDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);
  Ctx->World->SetDebugDraw(&DbgDraw);
}

GameContext::Ptr TennisGame::CreateContext()
{
  b2Vec2 gravity(0, 0);
  b2WorldPtr world(new b2World(gravity));
  Coordinator::Ptr coords(new Coordinator());
  return GameContext::Ptr(new GameContext(*this, coords, world));
}

CCScene* TennisGame::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        TennisGame *layer = TennisGame::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

bool TennisGame::init()
{
  try
  {
    THROW_UNLESS(CCLayer::init());
    setTouchEnabled(true);
    AddExitButton();
    AddCaption();
    PlayerLeft = CreatePlayer(true);
    PlayerRight = CreatePlayer(false);
    Ball = CreateBall();
    Ball->SetOutOfScreenCallback(tr1::bind(&TennisGame::ballOutOfScreenCallback, this, tr1::placeholders::_1));
    CreateEarth(*Ctx);
    schedule(schedule_selector(TennisGame::Tick));
    Ball->Run();
    return true;
  }
  catch (const std::logic_error& err)
  {
    std::string desc = err.what();
    CCLOGERROR("Init error: %s", desc);
    return false;
  }
}

void TennisGame::draw()
{
  CCLayer::draw();
  //Ctx->World->DrawDebugData();
}

void TennisGame::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
  CCLayer::ccTouchesMoved(pTouches, pEvent);

  for (CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it)
  {
    CCTouch* touch = static_cast<CCTouch*>(*it);
    if (!touch)
    {
      continue;
    }

    if (PlayerLeft->IsContain(touch->getPreviousLocation()))
    {
      PlayerLeft->Move(touch->getDelta().y);
    }
    else if (PlayerRight->IsContain(touch->getPreviousLocation()))
    {
      PlayerRight->Move(touch->getDelta().y);
    }
  }
}

void TennisGame::Tick(float delta)
{
  Ctx->World->Step(delta, 10, 10);
  Ctx->World->ClearForces();
  Ball->Update();
}

void TennisGame::AddExitButton()
{
  CCMenuItemImage *pCloseItem 
    = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(TennisGame::menuCloseCallback));

  THROW_UNLESS(pCloseItem);
  pCloseItem->setPosition(ccp(Ctx->WindowSize.width - 20, 20));
  CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
  pMenu->setPosition(CCPointZero);
  THROW_UNLESS(pMenu);
  this->addChild(pMenu, 1);
}

void TennisGame::AddCaption()
{
   PlayerLeftCount = CCLabelTTF::create("0", "Consolas", 24);
   PlayerRightCount = CCLabelTTF::create("0", "Consolas", 24);
   CountColon = CCLabelTTF::create(":", "Consolas", 24);
   PlayerLeftCount->setPosition(ccp(Ctx->WindowSize.width / 2 - 20, Ctx->WindowSize.height - 10));
   PlayerRightCount->setPosition(ccp(Ctx->WindowSize.width / 2 + 20, Ctx->WindowSize.height - 10));
   CountColon->setPosition(ccp(Ctx->WindowSize.width / 2, Ctx->WindowSize.height - 10));
   this->addChild(PlayerLeftCount, 1);
   this->addChild(PlayerRightCount, 1);
   this->addChild(CountColon, 1);
}

void TennisGame::AddBackground()
{
//   CCSprite* pSprite = CCSprite::create("HelloWorld.png");
//   THROW_UNLESS(pSprite);
//   pSprite->setPosition(ccp(WindowSize.width/2, WindowSize.height/2));
//   this->addChild(pSprite, 0);
}

std::auto_ptr<TennisPlayer> TennisGame::CreatePlayer(bool left) const
{
  return std::auto_ptr<TennisPlayer>(new TennisPlayer(left, Ctx));
}

std::auto_ptr<TennisBall> TennisGame::CreateBall() const
{
  return std::auto_ptr<TennisBall>(new TennisBall(Ctx));
}

void TennisGame::menuCloseCallback(CCObject* pSender)
{
  // "close" menu item clicked
  CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}

void TennisGame::ballOutOfScreenCallback(const cocos2d::CCPoint& ballPos)
{
  bool isLeftWin = (ballPos.x > 0);
  CCLabelTTF& count = isLeftWin ? *PlayerLeftCount : *PlayerRightCount;
  std::string countBuf(10, 0);
  count.setString(itoa(atoi(count.getString()) + 1, &countBuf[0], 10));
  Ball->Run();
}
