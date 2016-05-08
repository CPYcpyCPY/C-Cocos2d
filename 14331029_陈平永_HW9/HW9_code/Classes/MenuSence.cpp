#include "MenuSence.h"
#include "GameSence.h"
#include <iostream>
using namespace std;
USING_NS_CC;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(MenuSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);
	
	auto start_gold = Sprite::create("menu-start-gold.png");
	start_gold->setPosition(Vec2(750 + origin.x, visibleSize.height/2 + origin.y - 200));
	this->addChild(start_gold, 1);

	myStart = Sprite::create("start-0.png");
	myStart->setPosition(Vec2(750 + origin.x, visibleSize.height / 2 + origin.y - 150));
	this->addChild(myStart, 1);

	auto gold_minter_text = Sprite::create("gold-miner-text.png");
	gold_minter_text->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/5*4));
	this->addChild(gold_minter_text, 1);

    return true;
}

bool MenuSence::onTouchBegan(CCTouch *touch, CCEvent *unused_event) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto location = touch->getLocation();
	auto box = myStart->boundingBox();
	bool isTouch = box.containsPoint(location);

	//如果点击的区域在Start区域内，则进入另一个场景
	if (isTouch) {
		this->removeChild(myStart);
		myStart = Sprite::create("start-1.png");
		myStart->setPosition(Vec2(750 + origin.x, visibleSize.height / 2 + origin.y - 150));
		this->addChild(myStart, 1);
		Director::getInstance()->replaceScene(TransitionFade::create(1, GameSence::createScene()));
	}
	return true;
}


