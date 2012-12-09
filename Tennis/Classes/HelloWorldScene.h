#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include <SimpleAudioEngine.h>

class HelloWorld : public cocos2d::CCLayer
{
public:
  HelloWorld();

//-- cocos2d::CCLayer
public:
  virtual bool init();  

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

private:
  const cocos2d::CCSize WindowSize;
};

#endif  // __HELLOWORLD_SCENE_H__