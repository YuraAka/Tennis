#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define NOMINMAX
#include <platform/CCPlatformConfig.h>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  #include <tr1/memory>
#else
  #include <memory>
#endif

#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <Box2D\Common\b2Math.h>
#include <SimpleAudioEngine.h>
#include <GLES-Render.h>

class Coordinator
{
public:
  typedef tr1::shared_ptr<Coordinator> Ptr;

  Coordinator();

  float ToBox2d(float cocos2dPos) const;
  b2Vec2 ToBox2d(const cocos2d::CCPoint& cocos2dPoint) const;
  b2Vec2 ToBox2d(const cocos2d::CCSize& size) const;
  float ToCocos2d(float box2dPos) const;
  cocos2d::CCPoint ToCocos2d(const b2Vec2& pos) const;
  float GetTransformKoeff() const;

private:
  const float Koeff;
};

typedef tr1::shared_ptr<b2World> b2WorldPtr;

struct GameContext
{
  typedef tr1::shared_ptr<GameContext> Ptr;

  cocos2d::CCLayer& Layer;
  Coordinator::Ptr Coords;
  b2WorldPtr World;
  const cocos2d::CCSize WindowSize;
  const b2FixtureDef BodyFixture;

  GameContext(cocos2d::CCLayer& myLayer, Coordinator::Ptr myCoords, b2WorldPtr myWorld);
};

class TennisBall
{
public:
  TennisBall(GameContext::Ptr svc);
  void Update();

private:
  cocos2d::CCSprite& Sprite;
  b2Body* Body;
  GameContext::Ptr Ctx;
};

class TennisPlayer
{
public:
  TennisPlayer(bool isLeft, GameContext::Ptr svc);
  void Move(const float dy);
  bool IsContain(const cocos2d::CCPoint& point) const;

private:
  mutable cocos2d::CCSprite& Sprite;
  const float UpperBound;
  const float LowerBound;
  b2Body* Body;
  GameContext::Ptr Ctx;
};

class TennisGame : public cocos2d::CCLayer
{
public:
  TennisGame();

//-- cocos2d::CCLayer
public:
  virtual bool init();
  virtual void draw();

  virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

public:
  static cocos2d::CCScene* scene();
  void menuCloseCallback(CCObject* pSender);
  CREATE_FUNC(TennisGame);

private:
  void AddExitButton();
  void AddCaption();
  void AddBackground();

  std::auto_ptr<TennisPlayer> CreatePlayer(bool left) const;
  std::auto_ptr<TennisBall> CreateBall() const;

  void Tick(float delta);

  GameContext::Ptr CreateContext();
private:
  const GameContext::Ptr Ctx;
  GLESDebugDraw DbgDraw;

  std::auto_ptr<TennisBall> Ball;
  std::auto_ptr<TennisPlayer> PlayerLeft;
  std::auto_ptr<TennisPlayer> PlayerRight;
};

#endif  // __HELLOWORLD_SCENE_H__