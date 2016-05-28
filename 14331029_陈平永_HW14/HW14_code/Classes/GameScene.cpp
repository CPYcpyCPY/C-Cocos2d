#include "GameScene.h"
#include "LoginScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 2, visibleHeight / 4));
	rank_button->addClickEventListener(CC_CALLBACK_1(GameScene::rank, this));
    this->addChild(rank_button, 2);

    return true;
}

bool GameScene::rank(Ref* ref) {
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setUrl("http://localhost:8080/rank?top=10");
	request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRankRequestCompleted, this));

	vector<string> headers;
	headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	return true;
}

void GameScene::onHttpRankRequestCompleted(HttpClient *sender, HttpResponse* response) {
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
	string jsonStr = Global::toString(buffer);
	log("%s", jsonStr);
	showRank(jsonStr);
}

void GameScene::showRank(string jsonStr) {
	rapidjson::Document d;
	d.Parse<0>(jsonStr.c_str());
	string info = "";
	if (d.IsObject() && d.HasMember("info")) info = d["info"].GetString();
	vector<string> names;
	vector<string> scores;
	info += '|';
	int flag = 1;
	string name = "";
	string score = "";
	for (int i = 1; i < info.size(); i++) {
		if (info[i] == '|') {
			scores.push_back(score);
			score = "";
			flag = 1;
		}
		if (info[i] == ':') {
			names.push_back(name);
			name = "";
			flag = 0;
		}
		if (flag == 1 && info[i] != '|' && info[i] != ':') name += info[i];
		if (flag == 0 && info[i] != '|' && info[i] != ':') score += info[i];
	}
	string rankStr = "";
	for (int i = 0; i < names.size(); i++) {
		rankStr += names[i];
		rankStr += ":";
		rankStr += scores[i];
		rankStr += '\n';
	}
	rank_field->setString(rankStr);
}