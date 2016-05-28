#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
	initSpriteFrame();
}
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

//ÒÆ³ý¹ÖÎï
void Factory::removeMonster(Sprite* sp) {
	Animation* animation = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	Animate* animate = Animate::create(animation);
	Sequence* seq = Sequence::create(animate,
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	monster.eraseObject(sp);
}

//¹ÖÎïÒÆ¶¯
void Factory::moveMonster(Vec2 playerPos,float time){
	for (int i = 0; i < monster.size(); i++) {
		Vec2 monsterPos = monster.at(i)->getPosition();
		Vec2 direction = playerPos - monsterPos;
		direction.normalize();
		monster.at(i)->runAction(MoveBy::create(time, direction * 30));
	}
}

//Åö×²¼ì²â
Sprite* Factory::collider(Rect rect) {
	Rect colliderRect = Rect(rect.getMinX() + 5, rect.getMinY(),
		(rect.getMaxX() - rect.getMinX())*0.9, (rect.getMaxY() - rect.getMinY())*0.8);
	for (int i = 0; i < monster.size(); i++) {
		if (monster.at(i)->getBoundingBox().intersectsRect(colliderRect)) {
			return monster.at(i);
		}
	}
	return NULL;
}

Vector<Sprite*> Factory::getMonster() {
	return monster;
}