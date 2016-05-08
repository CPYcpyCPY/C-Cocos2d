#ifndef __MENU_SEBCE_H__
#define __MENU_SEBCE_H__

#include "cocos2d.h"
#include <string>

class MenuSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    //void startMenuCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MenuSence);

	virtual bool onTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *unused_event);

private:
	cocos2d::Sprite* myStart;
};

#endif // __MENU_SEBCE_H__
