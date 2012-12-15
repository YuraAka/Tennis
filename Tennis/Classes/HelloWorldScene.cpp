#include "HelloWorldScene.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

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
  
  class GLDrawFlagsGuard
  {
  public:
    GLDrawFlagsGuard()
    {
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//      glEnableClientState(GL_VERTEX_ARRAY);
    }

    ~GLDrawFlagsGuard()
    {
//       glDisableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glEnable(GL_TEXTURE_2D);
    }
  };

  CCSprite& CreateSprite(const std::string& picturePath)
  {
    CCSprite* picture = CCSprite::create("Ball.png");
    THROW_UNLESS(picture);
    return *picture;
  }

  void AddEarth(CCPoint pos, const GameContext& context)
  {
    const Coordinator& coords = *context.Coords;
    b2BodyDef def;
    def.type = b2_staticBody;
    def.allowSleep = true;
    def.position = coords.ToBox2d(pos);
    b2Body* earth = context.World->CreateBody(&def);
    b2PolygonShape bottomEarth;
    bottomEarth.SetAsBox(coords.ToBox2d(pos.x), coords.ToBox2d(5));
    b2FixtureDef fixDef;
    fixDef.density = 0;
    fixDef.restitution = 1;
    fixDef.shape = &bottomEarth;
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

Coordinator::Coordinator()
  : Koeff(CCDirector::sharedDirector()->getWinSize().width / 10)
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

TennisBall::TennisBall(CCPoint pos, const GameContext& context)
  : Sprite(CreateSprite("Ball.png"))
  , Body(NULL)
  , Context(context)
{
  Sprite.setPosition(pos);
  b2BodyDef bodyDef;
  bodyDef.position = Context.Coords->ToBox2d(pos);
  bodyDef.type = b2_dynamicBody;
  bodyDef.fixedRotation = true;
  Body = Context.World->CreateBody(&bodyDef);
  b2PolygonShape shape;
  b2Vec2 shapeSize = Context.Coords->ToBox2d(Sprite.getContentSize());
  shape.SetAsBox(shapeSize.x / 2, shapeSize.y / 2);
  b2FixtureDef fixDef;
  fixDef.shape = &shape;
  fixDef.restitution = 1;
  fixDef.density = 10;
  fixDef.friction = 0;
  Body->CreateFixture(&fixDef);
  Context.Layer->addChild(&Sprite, 1);
  Body->ApplyLinearImpulse(Context.Coords->ToBox2d(ccp(0, -100)), Context.Coords->ToBox2d(pos));
}

void TennisBall::Update()
{
  Sprite.setPosition(Context.Coords->ToCocos2d(Body->GetPosition()));
}

HelloWorld::HelloWorld()
  : WindowSize(CCDirector::sharedDirector()->getWinSize())
  , DbgDraw(Coords.GetTransformKoeff())
  , World(b2Vec2(0, 0))
  , Context(this, &Coords, &World)
{
  DbgDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);
  World.SetDebugDraw(&DbgDraw);
}

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
  try
  {
    THROW_UNLESS(CCLayer::init());

    setTouchEnabled(true);

    AddExitButton();
    //AddCaption();
    AddPlayer(true);
    //AddPlayer(false);
    AddBall();
    CreateEarth(Context);
    //AddBackground();
    schedule(schedule_selector(HelloWorld::Tick));
    return true;
  }
  catch (const std::exception& err)
  {
    CCLOGERROR("Init error: %s", err.what());
    return false;
  }
}

void HelloWorld::draw()
{
  CCLayer::draw();
  //GLDrawFlagsGuard flags;
  World.DrawDebugData();
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
  int i = 0;
}

void HelloWorld::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
  for (CCSetIterator it = pTouches->begin(); it != pTouches->end(); ++it)
  {
    CCTouch* touch = static_cast<CCTouch*>(*it);
    if (!touch)
    {
      continue;
    }

    if (!IsContain(PlayerLeft->getContentSize(), PlayerLeft->getPosition(), touch->getPreviousLocation()))
    {
      continue;
    }

    float deltaY = touch->getDelta().y;
    float playerNewY = PlayerLeft->getPositionY() + deltaY;
    playerNewY = std::max(playerNewY, PlayerLeft->getContentSize().height / 2);
    playerNewY = std::min(playerNewY, WindowSize.height - PlayerLeft->getContentSize().height / 2);
    PlayerLeft->setPositionY(playerNewY);
  }
}

void HelloWorld::Tick(float delta)
{
  World.Step(delta, 10, 10);
  World.ClearForces();
  Ball->Update();
}

void HelloWorld::AddExitButton()
{
  CCMenuItemImage *pCloseItem 
    = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(HelloWorld::menuCloseCallback));

  THROW_UNLESS(pCloseItem);
  pCloseItem->setPosition(ccp(WindowSize.width - 20, 20));
  CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
  pMenu->setPosition(CCPointZero);
  THROW_UNLESS(pMenu);
  this->addChild(pMenu, 1);
}

void HelloWorld::AddCaption()
{
  CCLabelTTF* pLabel = CCLabelTTF::create("Welcome to Yura's first tennis", "Consolas", 24);
  THROW_UNLESS(pLabel);
  pLabel->setPosition(ccp(WindowSize.width / 2, WindowSize.height - 10));
  this->addChild(pLabel, 1);
}

void HelloWorld::AddBackground()
{
  CCSprite* pSprite = CCSprite::create("HelloWorld.png");
  THROW_UNLESS(pSprite);
  pSprite->setPosition(ccp(WindowSize.width/2, WindowSize.height/2));
  this->addChild(pSprite, 0);
}

void HelloWorld::AddPlayer(bool left)
{
  PlayerLeft = CCSprite::create("Player.png");
  THROW_UNLESS(PlayerLeft);
  const int playerX = left ? PlayerLeft->getContentSize().width / 2 : WindowSize.width - PlayerLeft->getContentSize().width / 2;
  const int playerY = WindowSize.height / 2;
  PlayerLeft->setPosition(ccp(playerX, playerY));
  addChild(PlayerLeft, 1);
}

void HelloWorld::AddBall()
{
  Ball.reset(new TennisBall(ccp(WindowSize.width / 2, WindowSize.height / 2), Context));
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
  // "close" menu item clicked
  CCDirector::sharedDirector()->end();
}
