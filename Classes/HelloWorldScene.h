#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

enum CardType
{
    Clubs = 0,
    Diamonds,
    Hearts,
    Spades
};

struct Card
{
    int number;
    CardType type;
};

struct PosIndex
{
    int x;
    int y;
};


class CardSprite : public cocos2d::Sprite
{
protected:
    std::string getFileName(CardType cardType);
    void showNumber();
public:
    virtual bool init();
    void onEnter() override;
    CREATE_FUNC(CardSprite);
    
    CC_SYNTHESIZE(Card, _card, Card);
    CC_SYNTHESIZE(PosIndex, _posIndex, PosIndex);
    
    void moveBackToInitPos();
    void moveToTrash();
    void moveToInitPos();
};


class HelloWorld : public cocos2d::Layer
{
protected:
    std::vector<Card> _cards;//カード情報
    CardSprite* _firstCard;
    float _timer;
    
    void initCards();//カードを初期化する
    Card getCard();//カードを取得する
    void createCard(PosIndex posIndex);//カードを作成する
    void showInitCards();//ゲーム開始時にカードを10枚表示する
    void showTimerLabel();
    void initGame();//ゲームを初期化する
    CardSprite* getTouchCard(cocos2d::Touch *touch);
    void showButton();
    void initTrash();
    void onTapButton(cocos2d::Ref* sender,cocos2d::extension::Control::EventType controlEvent);
    void showBackCards();
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    virtual bool onTouchBegan(cocos2d::Touch *touch,cocos2d::Event *unused_event);
    virtual void onTouchMoved(cocos2d::Touch *touch,cocos2d::Event *unused_event);
    virtual void onTouchEnded(cocos2d::Touch *touch,cocos2d::Event *unused_event);
    virtual void onTouchCancelled(cocos2d::Touch *touch,cocos2d::Event *unused_event);
    void update(float dt) override;
};

#endif // __HELLOWORLD_SCENE_H__
