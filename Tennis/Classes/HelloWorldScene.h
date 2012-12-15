#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define NOMINMAX
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <Box2D\Common\b2Math.h>
#include <SimpleAudioEngine.h>
#include <GLES-Render.h>

class Coordinator
{
public:
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

struct GameContext
{
  cocos2d::CCLayer* Layer;
  const Coordinator* Coords;
  b2World* World;

  GameContext(cocos2d::CCLayer* myLayer, const Coordinator* myCoords, b2World* myWorld)
    : Layer(myLayer)
    , Coords(myCoords)
    , World(myWorld)
  {
  }
};

class TennisBall
{
public:
  TennisBall(cocos2d::CCPoint pos, const GameContext& svc);
  //void EnableSyncWithBox2d(bool enable);
  void Update();

private:
  cocos2d::CCSprite& Sprite;
  b2Body* Body;
  const GameContext Context;
};

class HelloWorld : public cocos2d::CCLayer
{
public:
  HelloWorld();

//-- cocos2d::CCLayer
public:
  virtual bool init();  
  virtual void draw();

  virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
  virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);

public:
  static cocos2d::CCScene* scene();
  void menuCloseCallback(CCObject* pSender);
  CREATE_FUNC(HelloWorld);

private:
  void AddExitButton();
  void AddCaption();
  void AddBackground();
  void AddPlayer(bool left);
  void AddBall();

  void Tick(float delta);

private:
  const Coordinator Coords;
  const cocos2d::CCSize WindowSize;
  GLESDebugDraw DbgDraw;
  b2World World;
  const GameContext Context;

  std::auto_ptr<TennisBall> Ball;
  cocos2d::CCSprite* PlayerLeft;
};

#endif  // __HELLOWORLD_SCENE_H__