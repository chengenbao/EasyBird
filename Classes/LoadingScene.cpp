#include "LoadingScene.h"
#include "Resource.h"

LoadingScene::LoadingScene(){}

LoadingScene::~LoadingScene(){}

bool LoadingScene::init() {
	if(Scene::init()){
		return true;
	} else {
		return false;
	}
}

void LoadingScene::onEnter(){
	// add background to current scene
	Sprite *background = Sprite::create("splash.png");
	Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
	background->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
	this->addChild(background);

	// start ansyc method load the atlas.png
	Director::getInstance()->getTextureCache()->addImageAsync("atlas.png", CC_CALLBACK_1(LoadingScene::loadingCallBack, this));
}

void LoadingScene::loadingCallBack(Texture2D *texture){
	AtlasLoader::getInstance()->loadAtlas("atlas.txt", texture);

	// After loading the texture , preload all the sound
	SimpleAudioEngine::getInstance()->preloadEffect(BIRD_DIE_MUSIC);
	SimpleAudioEngine::getInstance()->preloadEffect(BIRD_HIT_MUSIC);
	SimpleAudioEngine::getInstance()->preloadEffect(BIRD_POINT_MUSIC);
	SimpleAudioEngine::getInstance()->preloadEffect(BIRD_SWOOSHING_MUSIC);
	SimpleAudioEngine::getInstance()->preloadEffect(BIRD_WING_MUSIC);

	// After load all the things, change the scene to new one
	//auto scene = HelloWorld::createScene();
	auto scene = WelcomeScene::create();
	TransitionScene *transition = TransitionFade::create(1, scene);
	Director::getInstance()->replaceScene(transition);
}