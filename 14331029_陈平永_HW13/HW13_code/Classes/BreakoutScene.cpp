#include "BreakoutScene.h"
#include <string>
using namespace std;
#include "audio\include\SimpleAudioEngine.h"

using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Breakout::createScene() {
    auto scene = Scene::createWithPhysics();
   // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = Breakout::create(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool Breakout::init(PhysicsWorld* world) {
    if (!Layer::init()) {
        return false;
    }

	this->setPhysicsWorld(world);
    visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

    preloadMusic();
    playBgm();

    addBackground();
    addEdge();
    addPlayer();

	addContactListener();
    addTouchListener();
    addKeyboardListener();
	setBlood();
	addTime();

    this->schedule(schedule_selector(Breakout::update), 1);

    return true;
}

Breakout *Breakout::create(PhysicsWorld * world) {
	Breakout* pRet = new(std::nothrow) Breakout();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void Breakout::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
}

void Breakout::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//设置血条
void Breakout::setBlood() {
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	blood = 100;
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(blood);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);
}

void Breakout::addTime() {
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	time = 30;
	CCString* ccTime = CCString::createWithFormat("Remaining Time:%d", time);
	leftTime = Label::createWithTTF(ttfConfig, ccTime->getCString());
	leftTime->setPosition(Vec2(origin.x + visibleSize.width / 3 * 2,
		origin.y + visibleSize.height - leftTime->getContentSize().height/2));
	this->addChild(leftTime);
	schedule(schedule_selector(Breakout::showTime), 1.0f, kRepeatForever, 0);
}

void Breakout::showTime(float f) {
	if (time <= 0) {
		win();
		return;
	} else {
		time--;
		auto t = String::createWithFormat("Remaining Time:%d", time);
		leftTime->setString(t->_string);
		if (time == 15) addBlood();
		if (time == 10) addRope();
	}
}

void Breakout::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void Breakout::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
    boundBody->setTag(0);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void Breakout::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(35));

	//set bit mask
	player->setTag(100);
	player->getPhysicsBody()->setDynamic(true);
	player->getPhysicsBody()->setCategoryBitmask(0x02);       //飞机掩码，类别为2
	player->getPhysicsBody()->setCollisionBitmask(0x01);      //会和类别为1的物体发生碰撞
	player->getPhysicsBody()->setContactTestBitmask(0x01);    //检测飞机与陨石的碰撞

	player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(1);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);
}

void Breakout::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener(){
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan,this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved,this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
	if (blood > 0 && time > 0) {
		Vec2 postion = touch->getLocation();
		if (postion.x > visibleSize.width) postion.x = visibleSize.width;
		if (postion.y > visibleSize.height) postion.y = visibleSize.height;
		player->runAction((MoveTo::create(1, postion)));
		return true;
	}
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
	if (blood > 0 && time > 0) {
		Vec2 postion = touch->getLocation();
		if (postion.x > visibleSize.width) postion.x = visibleSize.width;
		if (postion.y > visibleSize.height) postion.y = visibleSize.height;
		player->runAction((MoveTo::create(1, postion)));
	}
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
	if (blood > 0 && time > 0) {
		Vec2 postion = touch->getLocation();
		if (postion.x > visibleSize.width) postion.x = visibleSize.width;
		if (postion.y > visibleSize.height) postion.y = visibleSize.height;
		player->runAction((MoveTo::create(1, postion)));
	}
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {

	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	if (nodeA->getTag() == 1 || nodeB->getTag() == 1) {
		if(time > 0) dead();
	}
	else {
		//播放碰撞音效
		SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");

		//陨石爆炸并消失
		ParticleSystemQuad* fireWorks;
		if (nodeA->getTag() != 100) {
			nodeA->getPhysicsBody()->setName("isContact");
			if (nodeA->getTag() == 10) {
				blood -= 10;
				fireWorks = ParticleSystemQuad::create("fire1.plist");
			}
			else if (nodeA->getTag() / 10000 == 20) {
				blood -= 15;
				fireWorks = ParticleSystemQuad::create("fire2.plist");
			}
			else {
				blood -= 20;
				fireWorks = ParticleSystemQuad::create("fire3.plist");
			}
			fireWorks->setPosition(nodeA->getPosition());
			this->removeChild(nodeA);
		}
		else {
			nodeB->getPhysicsBody()->setName("isContact");
			if (nodeB->getTag() == 10) {
				blood -= 10;
				fireWorks = ParticleSystemQuad::create("fire1.plist");
			}
			else if (nodeB->getTag() == 20) {
				blood -= 15;
				fireWorks = ParticleSystemQuad::create("fire2.plist");
			}
			else {
				blood -= 20;
				fireWorks = ParticleSystemQuad::create("fire3.plist");
			}
			fireWorks->setPosition(nodeB->getPosition());
			this->removeChild(nodeB);
		}
		this->addChild(fireWorks, 1);
		for (int i = 0; i < enemys.size(); i++) {
			if (enemys.at(i)->getName() == "isContact") {
				enemys.erase(enemys.begin() + i);
			}
		}
		pT->setPercentage(blood);
		if (blood <= 0) dead();
	}
	return true;
}

//飞船触碰血瓶事件
void Breakout::contactBlood(Node* n) {
	blood += 50;
	pT->setPercentage(blood);
	this->removeChild(n);
}

//游戏失败
void Breakout::dead() {
	blood = 0;
	pT->setPercentage(blood);
	unschedule(schedule_selector(Breakout::update));
	unschedule(schedule_selector(Breakout::showTime));
	ParticleSystemQuad* fireWorks = ParticleSystemQuad::create("fire.plist");
	fireWorks->setPosition(player->getPosition());
	this->addChild(fireWorks);

	this->removeChild(player);
	LayerColor* gameOverLayer = LayerColor::create(Color4B(128, 128, 128, 128));
	Label* over = Label::createWithSystemFont("Game Over", "Arial", 80);
	over->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	gameOverLayer->addChild(over);
	this->addChild(gameOverLayer, 5);
}

//游戏成功
void Breakout::win() {
	unschedule(schedule_selector(Breakout::update));
	unschedule(schedule_selector(Breakout::showTime));
	LayerColor* gameOverLayer = LayerColor::create(Color4B(128, 128, 128, 128));
	Label* over = Label::createWithSystemFont("You Win", "Arial", 80);
	over->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	gameOverLayer->addChild(over);
	this->addChild(gameOverLayer, 5);
	for (int i = 0; i < enemys.size(); i++) {
		enemys.at(i)->setVelocity(Vec2(0, 0));
	}
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
}

void Breakout::update(float f) {
    newEnemys();
}

void Breakout::newEnemys() {
	if (enemys.size() > 50) return;
    int newNum = 2;
    while (newNum--) {
        int type = 0;
        if (CCRANDOM_0_1() > 0.85) { type = 2; }
        else if (CCRANDOM_0_1() > 0.6) { type = 1; }

        Point location = Vec2(0, 0);
        switch (rand() % 4)
        {
        case 0:
            location.y = visibleSize.height;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 1:
            location.x = visibleSize.width;
            location.y = rand() % (int)(visibleSize.height);
            break;
        case 2:
            location.y = 0;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 3:
            location.x = 0;
            location.y = rand() % (int)(visibleSize.height);
            break;
        default:
            break;
        }
        addEnemy(type, location);
    }
}

void Breakout::addEnemy(int type, Point p) {
    char path[100];
    int tag;
    switch (type)
    {
    case 0:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    case 1:
        sprintf(path, "stone2.png");
        tag = 20;
        break;
    case 2:
        sprintf(path, "stone3.png");
        tag = 30;
        break;
    default:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    if(tag != 20) re->setScale(0.5, 0.5);
    re->setPosition(p);

	// TODO set bitmask
	re->getPhysicsBody()->setDynamic(true);
	re->getPhysicsBody()->setCategoryBitmask(0x01);       //陨石掩码，类别为1
	re->getPhysicsBody()->setCollisionBitmask(0x02);      //会和类别为2的物体发生碰撞
	re->getPhysicsBody()->setContactTestBitmask(0x02);    //检测飞机与陨石的碰撞

    if (rand() % 100 < 50) {
        re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
    }
    else {
        re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
    }
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
	re->setTag(tag);
    enemys.pushBack(re->getPhysicsBody());
	addChild(re);
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	if (blood > 0 && time > 0) {
		switch (code)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_W:
			player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_S:
			player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			removeAllEnemys();
			break;
		default:
			break;
		}
	}
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	if (blood > 0 && time > 0) {
		switch (code)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_W:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_S:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
			break;
		default:
			break;
		}
	}
}

void Breakout::removeAllEnemys() {
	if (!flag) {
		for (int i = 0; i < enemys.size(); i++) {
			Vec2 p = enemys.at(i)->getPosition();
			ParticleSystemQuad* fireWorks = ParticleSystemQuad::create("fire3.plist");
			fireWorks->setPosition(p);
			this->addChild(fireWorks);
			this->removeChild(enemys.at(i)->getNode());
		}
		enemys.clear();
		flag = true;
	}
}

//设置加血功能
void Breakout::addBlood() {
	bloodBottle = Sprite::create("blood.jpg");
	bloodBottle->setScale(0.25, 0.3);

	Point p = Vec2(0, 0);
	p.x = rand() % (int)(visibleSize.width-200);
	p.y = rand() % (int)(visibleSize.height-200);
	bloodBottle->setPosition(p);

	this->addChild(bloodBottle);
	schedule(schedule_selector(Breakout::meetBlood), 0.2f, kRepeatForever, 0);
}

void Breakout::meetBlood(float f) {
	if (player != NULL) {
		if (player->getPosition().getDistance(bloodBottle->getPosition()) < 30) {
			blood += 40;
			pT->setPercentage(blood);
			this->removeChild(bloodBottle);	
			unschedule(schedule_selector(Breakout::meetBlood));
		}
	}
}

void Breakout::addRope() {
	//添加绳子
	rope = Sprite::create("rope.png");
	rope->setScaleX(2);
	rope->setAnchorPoint(Vec2(0, 0));
	rope->setPosition(Point(visibleSize.width/2 - 150, visibleSize.height/2));
	auto r = PhysicsBody::createBox(rope->getContentSize());
	rope->setPhysicsBody(r);
	rope->getPhysicsBody()->setGravityEnable(false);

	rope->setTag(1);
	rope->getPhysicsBody()->setCategoryBitmask(0x1);
	rope->getPhysicsBody()->setCollisionBitmask(0x2);
	rope->getPhysicsBody()->setContactTestBitmask(0x2);

	this->addChild(rope, 1);

	//给绳子设置旋转点
	auto ropebase = Sprite::create();
	auto rb = PhysicsBody::createCircle(1.0f);
	ropebase->setPhysicsBody(rb);
	ropebase->getPhysicsBody()->setDynamic(false);
	ropebase->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
	this->addChild(ropebase, 1);
	PhysicsJointPin* jointpoint = PhysicsJointPin::construct(ropebase->getPhysicsBody(), rope->getPhysicsBody(), ropebase->getPosition());
	m_world->addJoint(jointpoint);
}