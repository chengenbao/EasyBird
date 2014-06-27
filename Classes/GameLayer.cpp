#include "GameLayer.h"
#include "Resource.h"

GameLayer::GameLayer(){}

GameLayer::~GameLayer(){}

bool GameLayer::init(){
	if(Layer::init()) {
		//get the origin point of the X-Y axis, and the visiable size of the screen
		Size visiableSize = Director::getInstance()->getVisibleSize();
		Point origin = Director::getInstance()->getVisibleOrigin();

		this->gameStatus = GAME_STATUS_READY;
		this->score = 0;

		// Add the bird
		this->bird = BirdSprite::getInstance();
		this->bird->createBird();
		PhysicsBody *body = PhysicsBody::create();
		body->addShape(PhysicsShapeCircle::create(BIRD_RADIUS));
		body->setDynamic(true);
		body->setLinearDamping(0.0f);
		body->setGravityEnable(false);
		this->bird->setPhysicsBody(body);
		this->bird->setPosition(origin.x + visiableSize.width*1/3 - 5, origin.y + visiableSize.height/2 + 5);
		this->bird->idle();
		this->addChild(this->bird);

		// Add the ground
		this->groundNode = Node::create();
		float landHeight = BackgroundLayer::getLandHeight();
		auto groundBody = PhysicsBody::create();
		groundBody->addShape(PhysicsShapeBox::create(Size(288, landHeight)));
		groundBody->setDynamic(false);
		groundBody->setLinearDamping(0.0f);
		this->groundNode->setPhysicsBody(groundBody);
		this->groundNode->setPosition(144, landHeight/2);
		this->addChild(this->groundNode);

		// init land
		this->landSpite1 = Sprite::createWithSpriteFrame(AtlasLoader::getInstance()->getSpriteFrameByName("land"));
		this->landSpite1->setAnchorPoint(Point::ZERO);
		this->landSpite1->setPosition(Point::ZERO);
		this->addChild(this->landSpite1, 30);

		this->landSpite2 = Sprite::createWithSpriteFrame(AtlasLoader::getInstance()->getSpriteFrameByName("land"));
		this->landSpite2->setAnchorPoint(Point::ZERO);
		this->landSpite2->setPosition(this->landSpite1->getContentSize().width-2.0f,0);
		this->addChild(this->landSpite2, 30);

		shiftLand = schedule_selector(GameLayer::scrollLand);
		this->schedule(shiftLand, 0.01f);
		auto fly =  schedule_selector(GameLayer::autoFly);
		this->schedule(fly, 0.005f);

		this->scheduleUpdate();

		return true;
	}else {
		return false;
	}
}

bool GameLayer::onContactBegin(const PhysicsContact &contact) {
	this->gameOver();
	return true;
}

void GameLayer::scrollLand(float dt){
	if (this->gameStatus == GAME_STATUS_START) {
		this->landSpite1->setPositionX(this->landSpite1->getPositionX() - 2.0f);
		this->landSpite2->setPositionX(this->landSpite1->getPositionX() + this->landSpite1->getContentSize().width - 2.0f);
		if(this->landSpite2->getPositionX() == 0) {
			this->landSpite1->setPositionX(0);
		}

		// move the pips
		for (auto singlePip : this->pips) {
			singlePip->setPositionX(singlePip->getPositionX() - 2);
			if(singlePip->getPositionX() < -PIP_WIDTH) {
				singlePip->setTag(PIP_NEW);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				singlePip->setPositionX(visibleSize.width);
				singlePip->setPositionY(this->getRandomHeight());
			}
		}
	}
}


void GameLayer::onTouch() {
	if(this->gameStatus == GAME_STATUS_OVER) {
		return;
	}

	SimpleAudioEngine::getInstance()->playEffect(BIRD_WING_MUSIC);
	if(this->gameStatus == GAME_STATUS_READY) {
		this->delegator->onGameStart();
		this->bird->fly();
		this->gameStatus = GAME_STATUS_START;
		this->createPips();
	}else if(this->gameStatus == GAME_STATUS_START) {
		this->bird->getPhysicsBody()->setVelocity(Vect(0, 260));
	}
}

void GameLayer::rotateBird() {
	float verticalSpeed = this->bird->getPhysicsBody()->getVelocity().y;
	this->bird->setRotation(min(max(-90, (verticalSpeed*0.2 + 60)), 30));
}


void GameLayer::update(float delta) {
	if (this->gameStatus == GAME_STATUS_START) {
		this->rotateBird();
		this->checkHit();
	}
}

void GameLayer::createPips() {
	// Create the pips
	for (int i = 0; i < PIP_COUNT; i++) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Sprite *pipUp = Sprite::createWithSpriteFrame(AtlasLoader::getInstance()->getSpriteFrameByName("pipe_up"));
		Sprite *pipDown = Sprite::createWithSpriteFrame(AtlasLoader::getInstance()->getSpriteFrameByName("pipe_down"));
		Node *singlePip = Node::create();

		// bind to pair
		pipDown->setPosition(0, PIP_HEIGHT + PIP_DISTANCE);
		singlePip->addChild(pipDown, 0, DOWN_PIP);
		singlePip->addChild(pipUp, 0, UP_PIP);
		singlePip->setPosition(visibleSize.width + i*PIP_INTERVAL + WAIT_DISTANCE, this->getRandomHeight());
		auto body = PhysicsBody::create();
		auto shapeBoxDown = PhysicsShapeBox::create(pipDown->getContentSize(),PHYSICSSHAPE_MATERIAL_DEFAULT, Point(0, PIP_HEIGHT + PIP_DISTANCE));
		body->addShape(shapeBoxDown);
		body->addShape(PhysicsShapeBox::create(pipUp->getContentSize()));
		body->setDynamic(false);
		singlePip->setPhysicsBody(body);
		singlePip->setTag(PIP_NEW);

		this->addChild(singlePip);
		this->pips.push_back(singlePip);
	}
}

int GameLayer::getRandomHeight() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return rand()%(int)(2*PIP_HEIGHT + PIP_DISTANCE - visibleSize.height);
}

void GameLayer::checkHit() {
	int birdX = this->bird->getPositionX();
	int birdY = this->bird->getPositionY();
	int birdStartX = birdX - BIRD_RADIUS;
	int birdEndX = birdX + BIRD_RADIUS;
	int birdStartY = birdY - BIRD_RADIUS;
	int birdEndY = birdY + BIRD_RADIUS;

	for(auto pip : this->pips) {
		if (pip->getTag() == PIP_NEW) {
			int pipX = pip->getPositionX();
			int pipY = pip->getPositionY();
			int pipStartX = pipX - PIP_WIDTH / 2;
			int pipEndX = pipX + PIP_WIDTH / 2;
			int pipSafeStartY = pipY + PIP_HEIGHT / 2;
			int pipSafeEndY = pipSafeStartY + PIP_DISTANCE;

			// Enter the unsafe area, attention!
			if ( birdEndX >= pipStartX && birdStartX <=  pipEndX)
			{
				// hit the pip
				if (birdStartY  <= pipSafeStartY || birdEndY >= pipSafeEndY) {
					this->gameOver();
				}
			}

			// hit the groud
			if (birdStartY <= BackgroundLayer::getLandHeight()) {
				this->gameOver();
			}

			if (birdStartX > pipEndX) {
				SimpleAudioEngine::getInstance()->playEffect(BIRD_POINT_MUSIC);
				this->score++;
				this->delegator->onGamePlaying(this->score);
				pip->setTag(PIP_PASS);
			}
		}
	}
}

void GameLayer::gameOver() {
	if(this->gameStatus == GAME_STATUS_OVER) {
		return;
	}
	SimpleAudioEngine::getInstance()->playEffect(BIRD_HIT_MUSIC);
	//get the best score
	int bestScore = UserRecord::getInstance()->readIntegerFromUserDefault("best_score");
	//update the best score
	if(this->score > bestScore){
		UserRecord::getInstance()->saveIntegerToUserDefault("best_score",this->score);
	}
	this->delegator->onGameEnd(this->score, bestScore);
	this->unschedule(shiftLand);
	SimpleAudioEngine::getInstance()->playEffect(BIRD_DIE_MUSIC);
	this->bird->die();
	this->bird->setRotation(-90);
	this->birdSpriteFadeOut();
	this->gameStatus = GAME_STATUS_OVER;
}

void GameLayer::birdSpriteFadeOut(){
	FadeOut* animation = FadeOut::create(1.5);
	CallFunc* animationDone = CallFunc::create(bind(&GameLayer::birdSpriteRemove,this));
	Sequence* sequence = Sequence::createWithTwoActions(animation,animationDone);
	this->bird->stopAllActions();
	this->bird->runAction(sequence);
}

void GameLayer::birdSpriteRemove(){
	this->bird->setRotation(0);
	this->removeChild(this->bird);
}

void GameLayer::autoFly(float p) {

	if (this->gameStatus == GAME_STATUS_READY) { // 游戏开始
		this->onTouch();
		return;
	}

	// 获取离小鸟最近的管子
	int px = Director::getInstance()->getVisibleSize().width;
	Node *nearPip = NULL; // 最近的管子

	for(auto pip : this->pips) {
		if (pip->getPositionX() < px && 
			pip->getPositionX() + PIP_WIDTH / 2 > this->bird->getPositionX() - BIRD_RADIUS) {
				px = pip->getPositionX();
				nearPip = pip;
		}
	}

	int lifeLine = 0; // 生命线
	const int happyOffset = 20; // 开心偏离距离

	if ( nearPip == NULL ) { // 起始阶段， 贴近屏幕中间飞
		lifeLine = Director::getInstance()->getVisibleSize().height / 2;
	} else {
		lifeLine = nearPip->getPositionY() + PIP_HEIGHT / 2 + PIP_DISTANCE / 2;
	}

	if (this->bird->getPositionY() > lifeLine + happyOffset) { // 偏离生命线安全距离以上，等待降落
		do {
			Sleep(1);
		} while(0);
	} else if (this->bird->getPositionY() < lifeLine - happyOffset) { // 偏离生命线安全距离以下，奋力飞回
		do {
			this->onTouch();
		} while(0);
	}
}