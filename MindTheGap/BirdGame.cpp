#include "BirdGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimationComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "CharacterControllerComponent.hpp"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include <iostream>

using namespace std;
using namespace sre;
using namespace glm;
using namespace rapidjson;

#pragma region Static Initialization
const glm::vec2 BirdGame::windowSize(1280, 720);
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

	//Player
    auto playerObj = createGameObject("Player", Tag::Player, vec2(0, 300));

	auto playerSpriteComp = playerObj->addComponent<SpriteComponent>();
    playerSpriteAtlas = SpriteAtlas::create("OPP_Sheet.json","OPP_Sheet.png");
    auto sprite = playerSpriteAtlas->get("OPP_Idle (1).png");
    //sprite.setScale({2,2});
    playerSpriteComp->setSprite(sprite);
    auto anim = playerObj->addComponent<SpriteAnimationComponent>();
	anim->setPlayerSprites(playerSpriteAtlas);

    auto phys = playerObj->addComponent<PhysicsComponent>();
	phys->initBox(b2_dynamicBody, vec2(20, 30) / physicsScale, { playerObj->getPosition().x / physicsScale,playerObj->getPosition().y / physicsScale }, 1);
	auto birdC = playerObj->addComponent<BirdController>();

	//CAMERA
    auto camObj = createGameObject("Camera", Tag::Camera, vec2(0));
    camera = camObj->addComponent<SideScrollingCamera>();
    camera->setFollowObject(playerObj, vec2(0));


	//+++++++++++++++++++++ SCENE - BUILDUP ++++++++++++++++++++++++//

	//TEST MAP SQUARED SECTION

	vec2 pos = vec2(0);
	vec2 size = vec2(160, 160);

	auto platformObj = createGameObject("Platform", Tag::Ground, vec2(0, 0));

	auto platformSpriteComp = platformObj->addComponent<SpriteComponent>();
	platformSpriteAtlas = SpriteAtlas::create("test_map_squared_spritesheet.json", "test_map_squared_spritesheet.png");
	platformSpriteComp->setSprite(platformSpriteAtlas->get("test_map_squared.png"));

	auto physComp = platformObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);

	auto triggerObj = createGameObject("Trigger", Tag::Trigger, vec2(0, 0));
	pos = vec2(0, 320);
	size = vec2(160, 160);
	physComp = triggerObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);
	physComp->fixture->SetSensor(true);

	//generateSquarePlatform(pos, size);

	//TEST FLOOR SECTION
	/*auto spriteBottom = spriteAtlas->get("column_bottom.png");
	spriteBottom.setScale({ 2,2 });

	vec2 pos = vec2(0);
	vec2 size = vec2( 500,  50);
	auto obj = createGameObject("Bottom Floor", Tag::Ground, pos);

	auto physComp = obj->addComponent<PhysicsComponent>();
	physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);
	*/

	/*// BOTTOM
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

	

    background2Component.init("background2.png");
    background1Component.init("background.png");*/
	
}

void BirdGame::generateSquarePlatform(vec2 pos, vec2 size) {
	auto platformObj = createGameObject("Platform", Tag::Ground, vec2(0, 0));

	auto platformSpriteComp = platformObj->addComponent<SpriteComponent>();
	platformSpriteAtlas = SpriteAtlas::create("test_map_squared_spritesheet.json", "test_map_squared_spritesheet.png");
	platformSpriteComp->setSprite(platformSpriteAtlas->get("test_map_squared.png"));

	auto physComp = platformObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);

	for (int i = 0; i < 8; i++) {

	}

	auto triggerObj = createGameObject("Trigger", Tag::Trigger, vec2(0, 0));
	pos = vec2(0, 320);
	size = vec2(160, 160);
	physComp = triggerObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);
	physComp->fixture->SetSensor(true);


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
std::shared_ptr<GameObject> BirdGame::createGameObject(string name, Tag tag, vec2 position) {
    auto obj = shared_ptr<GameObject>(new GameObject());
    sceneObjects.push_back(obj);
	obj->name = name;
	obj->tag = tag;
	obj->setPosition(position);
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
    } else if (gameState == GameState::GameOver){
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





