#include "HelloWorldScene.h"
#include <random>

#define CARD_NUM 13
#define CARD_TYPE_NUM 4

#define CARD_1_POS_X 200
#define CARD_1_POS_Y 320

#define TIMER_LABEL_POS_X 550
#define TIMER_LABEL_POS_Y 120

#define CARD_DISTANCE_X 140
#define CARD_DISTANCE_Y 200

#define BUTTON_POS_X 340
#define BUTTON_POS_Y 120

#define ZORDER_SHOW_CARD 1
#define ZORDER_MOVING_CARD 2

#define TAG_TRASH_CARD 11
#define TAG_BACK_CARD 12
#define TAG_TIMER_LABEL 13

#define MOVING_TIME 0.3

USING_NS_CC;
USING_NS_CC_EXT;

bool CardSprite::init(){
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

void CardSprite::onEnter(){
    Sprite::onEnter();
    
    setTexture("card_back.png");

    setTag(_posIndex.x + _posIndex.y * 5 +1 );
}

std::string CardSprite::getFileName(CardType cardType) {
    std::string filename;
    switch (cardType) {
        case Clubs:     filename = "card_clubs.png";break;
        case Diamonds:  filename = "card_diamonds.png";break;
        case Hearts:    filename = "card_hearts.png";break;
        default:        filename = "card_spades.png";break;
    }
    return filename;
}

void CardSprite::showNumber(){
    std::string numberString;
    switch (_card.number) {
        case 1: numberString = "A";break;
        case 11:numberString = "J";break;
        case 12:numberString = "Q";break;
        case 13:numberString = "K";break;
        default:numberString = StringUtils::format("%d",_card.number);break;
    }
    
    Color4B textColor;
    switch (_card.type) {
        case Clubs:
        case Spades:
            textColor = Color4B::BLACK;
            break;
            
        default:
            textColor = Color4B::RED;
            break;
    }
    
    auto number = Label::createWithSystemFont(numberString, "Arial", 96);
    number->setPosition(Point(getContentSize()/2));
    number->setTextColor(textColor);
    addChild(number);
}

void CardSprite::moveBackToInitPos(){
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * _posIndex.x;
    float posY = CARD_1_POS_Y + CARD_DISTANCE_Y * _posIndex.y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX,posY));
    
    runAction(move);
}

void CardSprite::moveToTrash(){
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * 4;
    float posY = CARD_1_POS_Y - CARD_DISTANCE_Y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX,posY));
    
    auto func = CallFunc::create([&](){
        this->setTag(TAG_TRASH_CARD);
    });
    
    auto seq = Sequence::create(move,func,nullptr);
    
    runAction(seq);

}

void CardSprite::moveToInitPos(){
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * _posIndex.x;
    float posY = CARD_1_POS_Y + CARD_DISTANCE_Y * _posIndex.y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX,posY));

    auto scale1 = ScaleTo::create(MOVING_TIME / 2,0,1);
    auto func1 = CallFunc::create([&]{
        setTexture(getFileName(_card.type));
        
        showNumber();
    });
    auto scale2 = ScaleTo::create(MOVING_TIME / 2,1,1);
    auto seq1 = Sequence::create(scale1,func1,scale2, nullptr);
    
    auto spawn = Spawn::create(move,seq1, nullptr);
    
    runAction(spawn);
}

Scene* HelloWorld::createScene(){
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    
    scene->addChild(layer);
    return scene;
}


bool HelloWorld::init(){
    if (!Layer::init()) {
        return false;
    }
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(_swallowsTouches);
    
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    initGame();
    
    return true;
}

void HelloWorld::initCards(){
    _cards.clear();
    
    for (int type = 0; type < CARD_TYPE_NUM; type++) {
        for (int number = 1; number <= CARD_NUM; number++) {
            Card card;
            card.number = number;
            card.type = (CardType)type;
            
            _cards.push_back(card);
        }
    }
}

Card HelloWorld::getCard(){
    std::random_device rd;
    std::mt19937 rand = std::mt19937(rd());
    
    int index = std::uniform_int_distribution<int>(0,(int)_cards.size()-1)(rand);
    
    auto card = _cards[index];
    
    _cards.erase(_cards.begin() + index);
    
    return card;
}

void HelloWorld::createCard(PosIndex posIndex){
    float posX = CARD_1_POS_X;
    float posY = CARD_1_POS_Y - CARD_DISTANCE_Y;
    
    auto card = CardSprite::create();
    card->setCard(getCard());
    card->setPosition(posX,posY);
    card->setPosIndex(posIndex);
    card->moveToInitPos();
    addChild(card,ZORDER_SHOW_CARD);
}

void HelloWorld::showInitCards(){
    for (int tag = 1; tag <= 10 ; tag++) {
        auto card = getChildByTag(tag);
        if (card) {
            card->removeFromParent();
        }
    }
    
    for (int x = 0; x<5; x++) {
        for (int y=0; y<2; y++) {
            PosIndex posIndex;
            posIndex.x = x;
            posIndex.y = y;
            
            createCard(posIndex);
        }
    }
}

void HelloWorld::initGame(){
    showBackCards();
    showTimerLabel();
    showButton();
}


CardSprite* HelloWorld::getTouchCard(Touch *touch){
    for (int tag = 1; tag <= 10; tag++) {
        auto card = (CardSprite*)getChildByTag(tag);
        if (card && card != _firstCard && card->getBoundingBox().containsPoint(touch->getLocation())) {
            return card;
        }
    }
            return nullptr;
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event) {
    _firstCard = getTouchCard(touch);
    if (_firstCard) {
        _firstCard->setLocalZOrder(ZORDER_MOVING_CARD);
        return true;
    }
    return false;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *unused_event) {
    _firstCard->setPosition(_firstCard->getPosition() + touch->getDelta());
}

void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event) {
    bool success = false;
    
    auto _secondSprite = getTouchCard(touch);
    if (_secondSprite) {
        if (_firstCard->getCard().number + _secondSprite->getCard().number ==13) {
            success = true;
        }
    }else{
        if (_firstCard->getCard().number == 13) {
            success = true;
        }
    }
    
    if (success) {
        if ((int)_cards.size() > 0) {
            createCard(_firstCard->getPosIndex());
        }
        
        _firstCard->moveToTrash();
        
        if (_secondSprite) {
            if ((int)_cards.size() > 0) {
                createCard(_secondSprite->getPosIndex());
            }
            
            _secondSprite->moveToTrash();
        }
        if ((int)_cards.size() <= 0) {
            removeChildByTag(TAG_BACK_CARD);
        }
    }else{
        _firstCard->moveBackToInitPos();
        _firstCard->setLocalZOrder(ZORDER_SHOW_CARD);
    }
    _firstCard = nullptr;
}

void HelloWorld::onTouchCancelled(Touch *touch, Event *unused_event) {
    onTouchEnded(touch, unused_event);
}

void HelloWorld::showButton(){
    auto button = ControlButton::create(Scale9Sprite::create("button.png"));
    
    button->setAdjustBackgroundImage(false);
    
    button->setPosition(BUTTON_POS_X,BUTTON_POS_Y);
    
    button->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::onTapButton), Control::EventType::TOUCH_UP_INSIDE);
    
    button->setTitleForState("Start", Control::State::NORMAL);
    
    addChild(button);
}

void HelloWorld::initTrash(){
    while (true) {
        auto card = getChildByTag(TAG_TRASH_CARD);
        if (card) {
            card->removeFromParent();
        }else{
            break;
        }
    }
}

void HelloWorld::showBackCards(){
    auto backCards = getChildByTag(TAG_BACK_CARD);
    if (!backCards) {
        float posX = CARD_1_POS_X;
        float posY = CARD_1_POS_Y - CARD_DISTANCE_Y;
        
        backCards = Sprite::create("card_back.png");
        backCards->setPosition(posX,posY);
        backCards->setTag(TAG_BACK_CARD);
        addChild(backCards);
    }
}

void HelloWorld::onTapButton(Ref* sender, Control::EventType controlEvent) {
    unscheduleUpdate();
    initCards();
    showInitCards();
    showBackCards();
    initTrash();
    showTimerLabel();
    scheduleUpdate();
}

void HelloWorld::showTimerLabel(){
    _timer = 0;
    
    auto timerLabel = (Label*)getChildByTag(TAG_TIMER_LABEL);
    if (!timerLabel) {
        timerLabel = Label::createWithSystemFont("", "Arial", 48);
        timerLabel->setPosition(TIMER_LABEL_POS_X,TIMER_LABEL_POS_Y);
        timerLabel->setTag(TAG_TIMER_LABEL);
        addChild(timerLabel);
    }
    
    timerLabel->setString(StringUtils::format("%0.2fs",_timer));
}

void HelloWorld::update(float dt){
    _timer += dt;
    auto timerLabel = (Label*)getChildByTag(TAG_TIMER_LABEL);
    if (timerLabel) {
        timerLabel->setString(StringUtils::format("%0.2fs",_timer));
    }
    
    bool finish = true;
    for (int tag = 1; tag<=10; tag++) {
        auto node = getChildByTag(tag);
        if (node) {
            finish = false;
            break;
        }
    }
    
    if (finish) {
        unscheduleUpdate();
    }
}
