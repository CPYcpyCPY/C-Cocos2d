#include "Thunder.h"
#include "GameScene.h"
#include "Global.h"
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;
auto audio = SimpleAudioEngine::getInstance();

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }

	move = score = 0;

    visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
    // bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, \
    //     visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

    addEnemy(5);

    preloadMusic();
    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

	//每0.1s更新一次
	schedule(schedule_selector(Thunder::update), 0.01f, kRepeatForever, 0);
    
    return true;
}

void Thunder::preloadMusic() {
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music / bgm.mp3");
}

void Thunder::playBgm() {
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * (i + 1));
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::addTouchListener(){
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Thunder::addKeyboardListener() {
	EventListenerKeyboard* listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Thunder::update(float f) {
	win();
	float x = player->getPositionX() + move;
	if (x > origin.x + visibleSize.width) x = origin.x + visibleSize.width - 1;
	if (x < origin.x) x = origin.x + 1;
	player->setPosition(x, player->getPositionY());
	static double count = 0;
	static int dir = 1;
	count += f;
	if (count > 1) {
		time++; count = 0.0; dir = -dir;
		if(time % 2 == 0) moveEnemys();
		death();
	}

	for (unsigned i = 0; i < bullets.size(); i++) {
		if (bullets[i]!= NULL) {
			bullets[i]->setPosition(bullets[i]->getPositionX(), bullets[i]->getPositionY() + 5);
			if (bullets[i]->getPositionY() > visibleSize.height - 10) {
				bullets[i]->removeFromParentAndCleanup(true);
				bullets[i] = NULL;
			}
		}
	}
	
	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			for (unsigned j = 0; j < bullets.size(); j++) {
				if (bullets[j] != NULL && bullets[j]->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
					EventCustom e("meet");
					e.setUserData(&i);
					_eventDispatcher->dispatchEvent(&e);
					removeBullet(j);
					break;
				}
			}
		}
	}
	
}

//陨石向前移动
void Thunder::moveEnemys() {
	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(0, -40));
		}
	}
}

void Thunder::removeBullet(int j) {
	this->removeChild(bullets[j]);
	bullets.erase(bullets.begin() + j);
}

//判断死亡
void Thunder::death() {
	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			if (player->getPosition().y + 30 > enemys[i]->getPosition().y ) {
				lost();
				this->removeChild(player);
				break;
			}
		}
	}
}

//游戏失败
void Thunder::lost() {
	unschedule(schedule_selector(Thunder::update));
	LayerColor* gameOverLayer = LayerColor::create(Color4B(128, 128, 128, 128));
	Label* over = Label::createWithSystemFont("Game Over", "Arial", 80);
	over->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	gameOverLayer->addChild(over);
	this->addChild(gameOverLayer, 5);

	Button* submit_button = Button::create();
	submit_button->setTitleText("Submit");
	submit_button->setColor(Color3B(128, 0, 0));
	submit_button->setTitleFontSize(50);
	submit_button->setPosition(Size(visibleSize.width / 2, visibleSize.height / 4));
	submit_button->addClickEventListener(CC_CALLBACK_1(Thunder::submit, this));
	this->addChild(submit_button, 2);
}

//游戏成功
void Thunder::win() {
	if (enemys.size() == 0) {
		unschedule(schedule_selector(Thunder::update));
		LayerColor* gameOverLayer = LayerColor::create(Color4B(128, 128, 128, 128));
		Label* over = Label::createWithSystemFont("You Win", "Arial", 80);
		over->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
		gameOverLayer->addChild(over);
		this->addChild(gameOverLayer, 5);

		Button* submit_button = Button::create();
		submit_button->setTitleText("Submit");
		submit_button->setColor(Color3B(0, 0, 255));
		submit_button->setTitleFontSize(50);
		submit_button->setPosition(Size(visibleSize.width / 2, visibleSize.height / 4));
		submit_button->addClickEventListener(CC_CALLBACK_1(Thunder::submit, this));
		this->addChild(submit_button, 2);
	}
}

bool Thunder::submit(Ref *ref) {
	if (score > Global::score) Global::score = score;

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl("http://localhost:8080/submit");
	request->setResponseCallback(CC_CALLBACK_2(Thunder::onHttpSubmitRequestCompleted, this));

	CCString* ns = CCString::createWithFormat("%d", Global::score);
	string s = ns->getCString();
	string score = "score=" + s;
	const char* postData = score.c_str();
	request->setRequestData(postData, strlen(postData));
	request->setTag("POST score");

	vector<string> headers;
	headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	Director::getInstance()->replaceScene(TransitionFade::create(1, GameScene::createScene()));
	return true;
}

void Thunder::onHttpSubmitRequestCompleted(HttpClient *sender, HttpResponse* response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}
	string head = "";
	std::vector<char> *header = response->getResponseHeader();
	for (unsigned int i = 0; i < header->size(); i++) head += (*header)[i];
	std::vector<char> *buffer = response->getResponseData();
	string info = Global::toString(buffer);
	log("%s", info);
}

void Thunder::fire() {
    Sprite* bullet = Sprite::create("bullet.png");
	if (player != NULL) {
		bullet->setPosition(player->getPosition());
		addChild(bullet);
		bullets.push_back(bullet);
		auto audio1 = SimpleAudioEngine::getInstance();
		audio1->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
	}
}

void Thunder::meet(EventCustom * event) {
	int i = *((int*)event->getUserData());
	this->removeChild(enemys[i]);
	enemys.erase(enemys.begin() + i);
	auto audio2 = SimpleAudioEngine::getInstance();
	audio2->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
	score++;
}

void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	if (position.x > origin.x + visibleSize.width) position.x = origin.x + visibleSize.width;
	else if (position.x < origin.x) position.x = origin.x;
	player->setPosition(Vec2(position.x, player->getPositionY()));
	return true;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	if (position.x > origin.x + visibleSize.width) position.x = origin.x + visibleSize.width;
	else if (position.x < origin.x) position.x = origin.x;
	player->setPosition(Vec2(position.x, player->getPositionY()));
}

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	if (position.x > origin.x + visibleSize.width) position.x = origin.x + visibleSize.width;
	else if (position.x < origin.x) position.x = origin.x;
	player->setPosition(Vec2(position.x, player->getPositionY()));
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPositionX() - 1, player->getPositionY());
			move -= 5;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPositionX() + 1, player->getPositionY());
			move += 5;
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			fire();
			break;
		default:
			break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPositionX() - 1, player->getPositionY());
			move += 5;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPositionX() + 1, player->getPositionY());
			move -= 5;
		default:
			break;
	}
}