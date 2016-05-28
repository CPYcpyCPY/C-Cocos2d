#include "HelloWorldScene.h"
#include "Monster.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	killNum = 0;

	//链接数据库
	pdb = NULL;
	std::string path = FileUtils::getInstance()->getWritablePath() + "hw11.db";
	int result = sqlite3_open(path.c_str(), &pdb);
	std::string sql = "create table if not exists killNum(ID int, number int);";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);

	//查询数据库，如果有数据，则赋给killNum, 否则初始化0
	char** re;
	int row, col;
	sqlite3_get_table(pdb, "select * from killNum", &re, &row, &col, NULL);
	if (row == 0) {
		sql = sqlite3_mprintf("insert into killNum values(1, %d)", killNum);
		result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	} else {
		log("%s", re[3]);
		killNum = std::atoi(re[3]);
	}
	sqlite3_free_table(re);


    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	lastCid = 'D';

	//用瓦片地图设置背景
	TMXTiledMap* tmx = TMXTiledMap::create("bg.tmx");
	tmx->setPosition(visibleSize.width/2, visibleSize.height/2);
	tmx->setScaleX(1.2);
	tmx->setScaleY(1.2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(tmx, 0);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	
	//倒计时的数字
	dtime = 180;

	//杀怪计数
	CCString* ccNum = CCString::createWithFormat("Monsters Killed:%d", killNum);
	num = Label::createWithTTF(ttfConfig, ccNum->getCString());

	//每三秒产生一只怪物，且所有怪物朝人物移动
	schedule(schedule_selector(HelloWorld::createMonster), 3.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::monsterMove), 4.0f, kRepeatForever, 0);

	//每0.1秒检测碰撞
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f, kRepeatForever, 0);
	
	num->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - num->getContentSize().height));
	addChild(num);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	blood = 100;
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(blood);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	
	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	dead.pushBack(frame0);
	
	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");

	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this,'D'));
	auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'X'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item6->setPosition(Vec2(origin.x + visibleSize.width*5/6, origin.y + 50));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	EventListenerKeyboard* listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::moveByKeyBord, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void HelloWorld::moveByKeyBord(EventKeyboard::KeyCode keyCode, Event* event) {
	if (blood > 0) {
		auto animation = Animation::createWithSpriteFrames(run, 0.05f);
		auto animate = Animate::create(animation);
		float playerX = player->getPosition().x;
		float playerY = player->getPosition().y;
		if (keyCode == EventKeyboard::KeyCode::KEY_A) {
			if (lastCid != 'A') {
				player->setFlipX(true);
			}
			lastCid = 'A';
			playerX -= 40;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
			playerX += 40;
			if (lastCid != 'D') {
				player->setFlipX(false);
			}
			lastCid = 'D';
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_W) playerY += 40;
		else if (keyCode == EventKeyboard::KeyCode::KEY_S) playerY -= 40;
		else if (keyCode == EventKeyboard::KeyCode::KEY_Y) actionEvent(this, 'Y');
		if (player->numberOfRunningActions() == 0) {
			if (playerX > 0 && playerY > 0 && playerX < origin.x + visibleSize.width && playerY < origin.y + visibleSize.height)
				player->runAction(MoveTo::create(0.3f, Vec2(playerX, playerY)));
			player->runAction(Repeat::create(animate->clone(), 1));
		}
	}
}

void HelloWorld::moveEvent(Ref*,char cid) {
	if (blood > 0) {
		auto animation = Animation::createWithSpriteFrames(run, 0.05f);
		auto animate = Animate::create(animation);
		float playerX = player->getPosition().x;
		float playerY = player->getPosition().y;
		switch (cid) {
		case 'W':       //向上移动
			playerY += 40;
			break;
		case 'A':       //向左移动
			if (lastCid != 'A') {
				player->setFlipX(true);
			}
			lastCid = 'A';
			playerX -= 40;
			break;
		case 'S':       //向下移动
			playerY -= 40;
			break;
		case 'D':       //向右移动
			if (lastCid != 'D') {
				player->setFlipX(false);
			}
			lastCid = 'D';
			playerX += 40;
			break;
		}
		if (player->numberOfRunningActions() == 0) {
			if (playerX > 0 && playerY > 0 && playerX < origin.x + visibleSize.width && playerY < origin.y + visibleSize.height)
				player->runAction(MoveTo::create(0.3f, Vec2(playerX, playerY)));
			player->runAction(Repeat::create(animate->clone(), 1));
		}
	}
}

void HelloWorld::actionEvent(Ref*, char cid) {
	if (blood > 0) {
		Animation* animation;
		Vector<SpriteFrame*> style;
		switch (cid) {
		case 'Y':             //攻击并加血
			style = attack;
			if (attackMonster()) {
				if (blood < 100) blood += 20;
				killNum++;
				showKillNum(killNum);
				std::string sql = sqlite3_mprintf("update killNum set number='%d' where ID=1;", killNum);
				int result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
				pT->setPercentage(blood);
			}
			break;
		case 'X':             //死亡并扣血
			style = dead;
			if (blood > 0) blood -= 20;
			pT->setPercentage(blood);
			if (blood <= 0) gameOver();
			break;
		}
		if (player->numberOfRunningActions() == 0) {
			animation = Animation::createWithSpriteFrames(style, 0.06f);
			auto animate = Animate::create(animation);
			player->runAction(Repeat::create(animate->clone(), 1));
		}
	}
}

void HelloWorld::stopAc() {
	auto animation = Animation::createWithSpriteFrames(idle, 0.01f);
	auto animate = Animate::create(animation);
	player->runAction(RepeatForever::create(animate));
}

//产生怪物
void HelloWorld::createMonster(float dt) {
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(Vec2(x, y));
	addChild(m, 3);
}
   
//与怪物发生碰撞
void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* monster = fac->collider(player->getBoundingBox());
	if (monster != NULL) {
		fac->removeMonster(monster);
		player->stopAllActions();
		actionEvent(this, 'X');
	}
}

//判断是否击中怪物
bool HelloWorld::attackMonster() {
	auto fac = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 35, playerRect.getMinY(),
		playerRect.getMaxX() - playerRect.getMinX() + 70, playerRect.getMaxY() - playerRect.getMinY());
	for (int i = 0; i < fac->getMonster().size(); i++) {
		if (fac->getMonster().at(i)->getBoundingBox().intersectsRect(attackRect)) {
			fac->removeMonster(fac->getMonster().at(i));
			return true;
		}
	}
	return false;
}

void HelloWorld::monsterMove(float) {
	auto fac = Factory::getInstance();
	fac->moveMonster(player->getPosition(), 0.6);
}

void HelloWorld::showKillNum(int killNum) {
	auto num_ = String::createWithFormat("Monsters Killed:%d", killNum);
	num->setString(num_->_string);
}

//游戏结束，给出提示，停止调度器
void HelloWorld::gameOver() {
	LayerColor* gameOverLayer = LayerColor::create(Color4B(128, 128, 128, 128));
	Label* over = Label::createWithSystemFont("Game Over", "Arial", 80);
	over->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	gameOverLayer->addChild(over);
	this->addChild(gameOverLayer, 5);
	unschedule(schedule_selector(HelloWorld::createMonster));
	unschedule(schedule_selector(HelloWorld::monsterMove));
}