#include "GameSence.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//添加第二个场景的背景
	auto level_background = Sprite::create("level-background-0.jpg");
	level_background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(level_background, 0);

	//添加石头
	auto stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(origin.x + 560, origin.y + 480));
	this->addChild(stone, 1);

	//添加Shoot
	auto shoot = MenuItemLabel::create(LabelTTF::create("Shoot", "Arial", 50), [&](Ref* sender) {

		auto fadeOut = FadeOut::create(2);
		auto delay = DelayTime::create(1);
		auto seq = Sequence::create(delay, fadeOut, NULL);

		//获取老鼠的位置，将石头射击过去
		//auto mousePosition = mouse->getPosition();
		auto newStone = Sprite::create("stone.png");
		newStone->setPosition(Vec2(origin.x + 560, origin.y + 480));
		this->addChild(newStone, 1);
		newStone->runAction(seq);
		newStone->runAction(MoveTo::create(0.5, mouse->getPosition()));
		
		//老鼠留下钻石并离开到左上角
		auto diamond = Sprite::create("diamond.png");
		diamond->setPosition(mouse->getPosition());
		this->addChild(diamond);
		mouse->runAction(MoveTo::create(1, Vec2(visibleSize.width/2 + 100, visibleSize.height/2 + 350)));
		
	});
	auto menu = Menu::create(shoot, NULL);
	menu->setPosition(Vec2(visibleSize.width * 0.8, visibleSize.height * 0.75));
	addChild(menu);

	//添加老鼠和钻石
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width/2, visibleSize.height/2);
	this->addChild(mouse, 1);


	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//获取点击的位置
	auto location = touch->getLocation();

	//创建奶酪
	auto fadeOut = FadeOut::create(2);
	auto delay = DelayTime::create(2);
	auto seq = Sequence::create(delay, fadeOut, NULL);

	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(Vec2(location.x, location.y));
	cheese->runAction(seq);
	this->addChild(cheese, 0);



	//老鼠旋转靠近奶酪
	mouse->runAction(MoveTo::create(1, Vec2(location.x, location.y)));
	float x = location.x - mouse->getPosition().x;
	float y = location.y - mouse->getPosition().y;
	float at = (float)CC_RADIANS_TO_DEGREES(atanf(x / y));
	if (x < 0) {
		if (y < 0)
			at = 180 + fabs(at);
		else
			at = 180 - fabs(at);
	}
	mouse->runAction(RotateTo::create(0.8, at));

	return true;
}
