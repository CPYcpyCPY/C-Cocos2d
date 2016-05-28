#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "sqlite3.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
   
	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void moveByKeyBord(EventKeyboard::KeyCode, Event*);
	void stopAc();
	void showKillNum(int killNum);
	void createMonster(float dt);
	void hitByMonster(float dt);
	bool attackMonster();
	void gameOver();
	void monsterMove(float dt);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);


	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;       //攻击动作
	cocos2d::Vector<SpriteFrame*> dead;		    //死亡动作
	cocos2d::Vector<SpriteFrame*> run;          //移动
	cocos2d::Vector<SpriteFrame*> idle;         //停止动作
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* num;
	sqlite3* pdb;
	int killNum;
	int dtime;                                  //计时
	int blood;                                  //生命
	char lastCid;                               //上一次的方向
	cocos2d::ProgressTimer* pT;
};

#endif // __HELLOWORLD_SCENE_H__
