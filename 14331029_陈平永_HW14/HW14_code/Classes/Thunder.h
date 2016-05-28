#ifndef __THUNDER_H__
#define __THUNDER_H__

#include "cocos2d.h"
#include "audio\include\SimpleAudioEngine.h"
#include <vector>
#include "network/HttpClient.h"
using namespace cocos2d::network;

USING_NS_CC;
	

class Thunder : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(Thunder);
private:
    void preloadMusic();
    void playBgm();
    
    void addTouchListener();
    void addKeyboardListener();

    void addEnemy(int n);
    void update(float f);
    void fire();
    void meet(EventCustom* event);
    void addCustomListener();
	void removeBullet(int i);
	void moveEnemys();
	void death();
	void lost();
	void win();
	bool submit(Ref *ref);
	void onHttpSubmitRequestCompleted(HttpClient *sender, HttpResponse* response);


    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	int move, time, score;
    Size visibleSize;
	Vec2 origin;
	Sprite* player;
	std::vector<Sprite*> bullets;
    std::vector<Sprite*> enemys;
};

#endif // __THUNDER_H__
