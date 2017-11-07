#include "BirdGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimationComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "BirdController.hpp"

#include <iostream>

using namespace std;
using namespace sre;

#pragma region Static Initialization
const glm::vec2 BirdGame::windowSize(800,1000);
BirdGame* BirdGame::instance = nullptr;
#pragma endregion

BirdGame::BirdGame() :debugDraw(physicsScale)
{
    instance = this;
    r.setWindowSize(windowSize);
    r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL );

    init();

    // setup callback functions
    r.keyEvent = [&](SDL_Event& e){
        onKey(e);
    };
    r.frameUpdate = [&](float deltaTime){
        update(deltaTime);
    };
    r.frameRender = [&](){
        render();
    };
    // start game loop
    r.startEventLoop();
}

#pragma region GameManager Fields
void BirdGame::init() {
    sceneObjects.clear();
    physicsComponentLookup.clear();
    initPhysics();
    auto camObj = createGameObject();
    camObj->name = "Camera";
    camera = camObj->addComponent<SideScrollingCamera>();
    camObj->setPosition(windowSize*0.5f);

    spriteAtlas = SpriteAtlas::create("bird.json","bird.png");

    auto birdObj = createGameObject();
    birdObj->name = "Bird";
    camera->setFollowObject(birdObj, birdObj->getPosition());
    auto so = birdObj->addComponent<SpriteComponent>();
    auto sprite = spriteAtlas->get("bird1.png");
    sprite.setScale({2,2});

    birdObj->setPosition({-100,300});
    so->setSprite(sprite);
    auto anim = birdObj->addComponent<SpriteAnimationComponent>();
    auto phys = birdObj->addComponent<PhysicsComponent>();
    phys->initCircle(b2_dynamicBody, 10/physicsScale, {birdObj->getPosition().x/physicsScale,birdObj->getPosition().y/physicsScale}, 1);
    auto birdC = birdObj->addComponent<BirdController>();

    vector<Sprite> spriteAnim({spriteAtlas->get("bird1.png"),spriteAtlas->get("bird2.png"),spriteAtlas->get("bird3.png"),spriteAtlas->get("bird2.png")});
    for(auto & s : spriteAnim){
        s.setScale({2,2});
    }
    anim-> setSprites(spriteAnim);


	// BOTTOM
    auto spriteBottom = spriteAtlas->get("column_bottom.png");
    spriteBottom.setScale({2,2});
    float curve = 250;
    int length = 50;
    float heighVariation = 80;
    float xVariation = 50;

    for (int i=0;i<length;i++) {
        auto obj = createGameObject();
        obj->name = "Wall bottom";
        auto so = obj->addComponent<SpriteComponent>();

        float xOffset = xVariation * cos(i*curve*0.2f);
        glm::vec2 pos{i*300+xOffset,spriteBottom.getSpriteSize().y/2 + sin(i*curve)*heighVariation};
        obj->setPosition(pos);
        so->setSprite(spriteBottom);

        glm::vec2 s { spriteBottom.getSpriteSize().x * spriteBottom.getScale().x/2, spriteBottom.getSpriteSize().y * spriteBottom.getScale().y/2};

		auto phys = obj->addComponent<PhysicsComponent>();
		phys->initBox(b2_staticBody, s / physicsScale, pos / physicsScale, 1.0f);

    }

	//TOP
    auto spriteTop = spriteAtlas->get("column_top.png");
    spriteTop.setScale({2,2});
    for (int i=0;i<length;i++){
        auto obj = createGameObject();
        obj->name = "Wall top";
        auto so = obj->addComponent<SpriteComponent>();

        float xOffset = xVariation * cos(i*curve*0.2f);
        glm::vec2 pos{ i*300+xOffset, windowSize.y-spriteTop.getSpriteSize().y/2 + sin(i*curve)*heighVariation};
        obj->setPosition(pos);
        glm::vec2 s { spriteTop.getSpriteSize().x * spriteTop.getScale().x/2, spriteTop.getSpriteSize().y * spriteTop.getScale().y/2};
        

		auto phys = obj->addComponent<PhysicsComponent>();
		phys->initBox(b2_staticBody, s / physicsScale, pos / physicsScale, 1.0f);
		
		so->setSprite(spriteTop);
    }

	//COINS
	auto spriteCoin = spriteAtlas->get("coin.png");
	spriteCoin.setScale({ 1,1 });
	for (int i = 0; i<length; i++) {
		auto obj = createGameObject();
		obj->name = "Coin";
		auto so = obj->addComponent<SpriteComponent>();

		float xOffset = xVariation * cos(i*curve*0.2f);
		glm::vec2 pos{ i * 300 + xOffset + 150, spriteCoin.getSpriteSize().y / 2 + sin(i*curve)*heighVariation + 300 };
		obj->setPosition(pos);

		auto phys = obj->addComponent<PhysicsComponent>();
		phys->initCircle(b2_staticBody, 10 / physicsScale, pos / physicsScale, 1.0f);
		phys->body->GetFixtureList()->SetSensor(true);

		so->setSprite(spriteCoin);

		cout << "spawned coint at: " << pos.x << ", " << pos.y << endl;
	}



    background1Component.init("background.png");
    background2Component.init("background2.png");
}
void BirdGame::update(float time) {
    if (gameState == GameState::Running){
        updatePhysics();
    }
    for (int i=0;i<sceneObjects.size();i++){
        sceneObjects[i]->update(time);
    }
}

void BirdGame::setGameState(GameState newState) {
    this->gameState = newState;
}
std::shared_ptr<GameObject> BirdGame::createGameObject() {
    auto obj = shared_ptr<GameObject>(new GameObject());
    sceneObjects.push_back(obj);
    return obj;
}
#pragma endregion

#pragma region Physics Fields
void BirdGame::initPhysics() {
    float gravity = -9.8; // 9.8 m/s2
    delete world;
    world = new b2World(b2Vec2(0,gravity));
    world->SetContactListener(this);

    if (doDebugDraw){
        world->SetDebugDraw(&debugDraw);
    }
}
void BirdGame::updatePhysics() {
    const float32 timeStep = 1.0f / 60.0f;
    const int positionIterations = 2;
    const int velocityIterations = 6;
    world->Step(timeStep, velocityIterations, positionIterations);

    for (auto phys : physicsComponentLookup){
        if (phys.second->rbType == b2_staticBody) continue;
        auto position = phys.second->body->GetPosition();
        float angle = phys.second->body->GetAngle();
        auto gameObject = phys.second->getGameObject();
        gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
        gameObject->setRotation(angle);
    }
}

void BirdGame::BeginContact(b2Contact *contact) {
    b2ContactListener::BeginContact(contact);
    handleContact(contact, true);
}
void BirdGame::EndContact(b2Contact *contact) {
    b2ContactListener::EndContact(contact);
    handleContact(contact, false);
}
void BirdGame::handleContact(b2Contact *contact, bool begin) {
    auto fixA = contact->GetFixtureA();
    auto fixB = contact->GetFixtureB();
    PhysicsComponent* physA = physicsComponentLookup[fixA];
    PhysicsComponent* physB = physicsComponentLookup[fixB];
    auto & aComponents = physA->getGameObject()->getComponents();
    auto & bComponents = physB->getGameObject()->getComponents();
    for (auto & c : aComponents){
        if (begin){
            c->onCollisionStart(physB);
        } else {
            c->onCollisionEnd(physB);
        }
    }
    for (auto & c : bComponents){
        if (begin){
            c->onCollisionStart(physA);
        } else {
            c->onCollisionEnd(physA);
        }
    }
}

void BirdGame::registerPhysicsComponent(PhysicsComponent *r) {
    physicsComponentLookup[r->fixture] = r;
}
void BirdGame::deregisterPhysicsComponent(PhysicsComponent *r) {
    auto iter = physicsComponentLookup.find(r->fixture);
    if (iter != physicsComponentLookup.end()){
        physicsComponentLookup.erase(iter);
    } else {
        assert(false); // cannot find physics object
    }
}
#pragma endregion

#pragma region Input
void BirdGame::onKey(SDL_Event &event) {
    for (auto & gameObject: sceneObjects) {
        for (auto & c : gameObject->getComponents()){
            bool consumed = c->onKey(event);
            if (consumed){
                return;
            }
        }
    }

    if (event.type == SDL_KEYDOWN){
        switch (event.key.keysym.sym){
            case SDLK_d:
                // press 'd' for physics debug
                doDebugDraw = !doDebugDraw;
                if (doDebugDraw){
                    world->SetDebugDraw(&debugDraw);
                } else {
                    world->SetDebugDraw(nullptr);
                }
                break;
            case SDLK_r:
                init();
                break;
            case SDLK_SPACE:
                if (gameState == GameState::GameOver){
                    init();
                    gameState = GameState::Ready;
                } else if (gameState == GameState::Ready){
                    gameState = GameState::Running;
                }
                break;

        }
    }
}
#pragma endregion

#pragma region Rendering
void BirdGame::render() {
    auto rp = RenderPass::create()
            .withCamera(camera->getCamera())
            .build();

    auto pos = camera->getGameObject()->getPosition();
    background1Component.renderBackground(rp, +pos.x*0.8f);

    auto spriteBatchBuilder = SpriteBatch::create();
    for (auto & go : sceneObjects){
        go->renderSprite(spriteBatchBuilder);
    }

    if (gameState == GameState::Ready){
        auto sprite = spriteAtlas->get("get-ready.png");
        sprite.setPosition(pos);
        spriteBatchBuilder.addSprite(sprite);
    } else if (gameState == GameState::GameOver){
        auto sprite = spriteAtlas->get("game-over.png");
        sprite.setPosition(pos);
        spriteBatchBuilder.addSprite(sprite);
    }

    auto sb = spriteBatchBuilder.build();
    rp.draw(sb);

    background2Component.renderBackground(rp, -pos.x*0.2f);

    if (doDebugDraw){
        world->DrawDebugData();
        rp.drawLines(debugDraw.getLines());
        debugDraw.clear();
    }
}
#pragma endregion





