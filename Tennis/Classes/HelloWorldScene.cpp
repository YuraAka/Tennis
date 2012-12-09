#include "HelloWorldScene.h"
#include <iostream>
#include <stdexcept>

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
}

HelloWorld::HelloWorld()
  : WindowSize(CCDirector::sharedDirector()->getWinSize())
{
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
    AddExitButton();
    AddCaption();
    AddPlayer(true);
    AddPlayer(false);
    AddBall();
    //AddBackground();
    return true;
  }
  catch (const std::exception& err)
  {
    CCLOGERROR("Init error: %s", err.what());
    return false;
  }
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
  CCSprite* player = CCSprite::create("Player.png");
  THROW_UNLESS(player);
  const int playerX = left ? player->getContentSize().width / 2 : WindowSize.width - player->getContentSize().width / 2;
  const int playerY = WindowSize.height / 2;
  player->setPosition(ccp(playerX, playerY));
  addChild(player, 1);
}

void HelloWorld::AddBall()
{
  CCSprite* ball = CCSprite::create("Ball.png");
  THROW_UNLESS(ball);
  ball->setPosition(ccp(WindowSize.width / 2, WindowSize.height / 2));
  addChild(ball, 1);
  CCAction* toLeft = CCMoveTo::create(10, ccp(ball->getContentSize().width / 2, WindowSize.height / 2));
  ball->runAction(toLeft);
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

