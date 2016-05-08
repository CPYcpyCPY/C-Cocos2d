#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    

	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void stopAc();
	void updateTime(float dt);
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
	cocos2d::Label* time;
	int dtime;                                  //计时
	int blood;                                  //生命
	cocos2d::ProgressTimer* pT;
};

#endif // __HELLOWORLD_SCENE_H__
