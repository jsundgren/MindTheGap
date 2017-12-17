#include "Box2D/Dynamics/Contacts/b2Contact.h"

#include "sre/RenderPass.hpp"
#include "sre\Profiler.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include "GameManager.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimationComponent.hpp"
#include "BackgroundComponent.hpp"
#include "PhysicsComponent.hpp"
#include "DoublePhysicsComponent.hpp"
#include "CharacterControllerComponent.hpp"
#include "GravityComponent.hpp"

#include <iostream>

using namespace std;
using namespace sre;
using namespace glm;
using namespace rapidjson;

#pragma region Static Initialization
const glm::vec2 GameManager::windowSize(1280, 720);
GameManager* GameManager::instance = nullptr;
float GameManager::roundDuration = 30.0f;
#pragma endregion

GameManager::GameManager() :debugDraw(physicsScale)
{
    instance = this;
    r.setWindowSize(windowSize);
    r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL );

	mapLoader = new MapLoader();
	initMainScreen();

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

void GameManager::initMainScreen()
{
	//Background
	background = std::shared_ptr<BackgroundComponent>(new BackgroundComponent());
	background->init("Map_1_Background.png");

	//Foreground
	/*foreground = std::shared_ptr<BackgroundComponent>(new BackgroundComponent());
	foreground->initForeground("vignette.png");*/

	//SpriteAtlasses
	guiSpriteAtlas = SpriteAtlas::create("GUI_SpriteSheet.json", "GUI_SpriteSheet.png");
	playerSpriteAtlas = SpriteAtlas::create("OPP_Sheet.json", "OPP_Sheet.png");

	//Camera
	cameraObj = createGameObject("Camera", Tag::Camera, vec2(0));
	camera = cameraObj->addComponent<FighterCamera>();
}

void GameManager::initNewGame()
{
	//Score Initialization
	player1HasToCatch = true;
	player1Score = 0;
	player2Score = 0;
	roundCount = 1;

	cout << "----------------- NEW GAME ------------------" << endl;
}

void GameManager::initNewRound() {

	//Memory deallocations
    sceneObjects.clear();
	sceneObjects.push_back(cameraObj);
    physicsComponentLookup.clear();
	player2Dummy.reset();
	player1.reset();
	player2.reset();
	
	gravityComponents.clear();
	gravityComponents = vector<GravityComponent*>();
    initPhysics();
	roundTime = 0.0f;

	//+++++++++++++++++++++ SCENE - INIT ++++++++++++++++++++++++//

	cout << "-------------- NEW ROUND ---------------" << endl;

	//+++++++++++++++++++++ SCENE - BUILDUP ++++++++++++++++++++++++//

	//Map
	map = generateMap(roundCount);
	roundCount = (roundCount + 1) % 3;

	//Player1
    player1 = createGameObject("Player1", Tag::Player, mapLoader->spawnPos.first);
	
		//Player1 --- Sprite & Animation
	auto playerSpriteComp = player1->addComponent<SpriteComponent>();
	playerSpriteComp->setColor(vec4(1.f, 0.3f, 0.3f, 1.f));
    auto sprite = playerSpriteAtlas->get("OPP_Idle (1).png");
    playerSpriteComp->setSprite(sprite);

    auto anim = player1->addComponent<SpriteAnimationComponent>();
	anim->setPlayerSprites(playerSpriteAtlas);

		//Player1 --- Physics
    auto phys = player1->addComponent<PhysicsComponent>();
	phys->initBox(b2_dynamicBody, vec2(20, 30) / physicsScale, { player1->getPosition().x / physicsScale,player1->getPosition().y / physicsScale }, 1);
	
		//Player1 --- Controller
	auto charaController = player1->addComponent<CharacterControllerComponent>();

	//Player2
	player2 = createGameObject("Player2", Tag::Player, mapLoader->spawnPos.second);

		//Player2 --- Sprite & Animation
	playerSpriteComp = player2->addComponent<SpriteComponent>();
	playerSpriteComp->setColor(vec4(0.3f,0.3f,1.0f,1));
	sprite = playerSpriteAtlas->get("OPP_Idle (1).png");
	playerSpriteComp->setSprite(sprite);

	anim = player2->addComponent<SpriteAnimationComponent>();
	anim->setPlayerSprites(playerSpriteAtlas);

		//Player2 --- Physics
	phys = player2->addComponent<PhysicsComponent>();
	phys->world = world2;
	phys->initBox(b2_dynamicBody, vec2(20, 30) / physicsScale, { player2->getPosition().x / physicsScale,player2->getPosition().y / physicsScale }, 1);
	
		//Player2 --- Controller
	charaController = player2->addComponent<CharacterControllerComponent>();
	charaController->player1 = false;

}

std::shared_ptr<GameObject> GameManager::generateMap(int id) {

	//Read in the file
	string name = "Map_" + to_string(id + 1);
	mapLoader->readFile( name + ".json");
	vector<pair<vec2, vec2>> sp = mapLoader->getSizePos();

	//Create Map object
	auto mapObj = createGameObject("Map", Tag::Ground, mapLoader->getSpritePos());
		
		//Map Sprite Components
	auto mapSpriteComp = mapObj->addComponent<SpriteComponent>();
	mapSpriteAtlas = SpriteAtlas::create(name + "_SpriteSheet.json", name + "_SpriteSheet.png");
	auto mapSprite = mapSpriteAtlas->get(name + ".png");
	mapSpriteComp->setSprite(mapSprite);

	//Iterate through platform positions and sizes
	for (int i = 0; i < sp.size(); i++) {
		vec2 size = sp[i].first;
		vec2 pos = sp[i].second;

		//Create Platform Objects
		auto platFormObj = createGameObject("Platform" + std::to_string(i), Tag::Ground, pos);

			//Platform Physics Components
		auto physComp = platFormObj->addComponent<DoublePhysicsComponent>();
		auto gravityComp = platFormObj->addComponent<GravityComponent>();
		physComp->initBox(b2_staticBody, size / physicsScale, pos / physicsScale, 1.0f);
	}

	//Delete size and position vectors
	mapLoader->clear();
	return mapObj;
}

void GameManager::update(float time) {
    if (gameState == GameState::Running){

		//Update Physics & Objects
		update2WorldPhysics();
		updatePhysics();
		for (int i=0;i<sceneObjects.size();i++){
			sceneObjects[i]->update(time);
		}

		//Check for roundTime
		roundTime += time;
		if (roundTime >= roundDuration) {
			player1HasToCatch = !player1HasToCatch;
			if (player1HasToCatch) {
				cout << "------------ Player 1 ran out of Time -------------" << endl;
			} else {
				cout << "------------ Player 2 ran out of Time -------------" << endl;
			}
			setGameState(GameState::TimeUp);
		}
    }
}

void GameManager::setGameState(GameState newState) {
	//Also reset Cameraposition
	camera->getCamera().setOrthographicProjection(GameManager::windowSize.y / 2, -1, 1);
    this->gameState = newState;
}

std::shared_ptr<GameObject> GameManager::createGameObject(string name, Tag tag, vec2 position) {
	cout << " * Creating: " << name << " at x: " << position.x << ", y: " << position.y << endl;
   
	auto obj = shared_ptr<GameObject>(new GameObject());
    sceneObjects.push_back(obj);
	obj->ID = sceneObjects.size() - 1;
	obj->name = name;
	obj->tag = tag;
	obj->setPosition(position);

    return obj;
}

void GameManager::playerTouchedPlayer()
{
	if (player1HasToCatch) {
		player1Score++;
		cout << "----------- Player 1 Score: " << player1Score << " ------------" << endl;
		cout << "----------- Player 2 Score: " << player2Score << " ------------" << endl;
		setGameState(GameState::Player2HasToCatch);
	} else {
		player2Score++;
		cout << "----------- Player 1 Score: " << player1Score << " ------------" << endl;
		cout << "----------- Player 2 Score: " << player2Score << " ------------" << endl;
		setGameState(GameState::Player1HasToCatch);
	}

	player1HasToCatch = !player1HasToCatch;

	if (player1Score >= 3) {
		setGameState(GameState::Player1Won);
		cout << "----------- Player 1 Won! ------------" << endl;
		return;
	}

	if (player2Score >= 3) {
		setGameState(GameState::Player2Won);
		cout << "----------- Player 2 Won! ------------" << endl;
		return;
	}

}

#pragma endregion

#pragma region Physics Fields
void GameManager::initPhysics() {
	float gravity = -9.8; // 9.8 m/s2
    delete world1;
	delete world2;
    world1 = new b2World(b2Vec2(0, gravity));
	world2 = new b2World(b2Vec2(0, gravity));
	world1->SetContactListener(this);
    world2->SetContactListener(this);

    if (doDebugDraw){
        world1->SetDebugDraw(&debugDraw);
		world2->SetDebugDraw(&debugDraw);
    }
}

void GameManager::updatePhysics() {
    const float32 timeStep = 1.0f / 60.0f;
    const int positionIterations = 2;
    const int velocityIterations = 6;
    world1->Step(timeStep, velocityIterations, positionIterations);
	world2->Step(timeStep, velocityIterations, positionIterations);

    for (auto phys : physicsComponentLookup){
        if (phys.second->rbType == b2_staticBody) continue;
        auto position = phys.second->body->GetPosition();
        float angle = phys.second->body->GetAngle();
        auto gameObject = phys.second->getGameObject();
        gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
        gameObject->setRotation(angle);
    }
}

void GameManager::update2WorldPhysics() {

	// If the player2 Dummy is null, make a new one, if not, update its position to the one of player2, but in world1
	if (player2Dummy == nullptr) {
		
		player2Dummy = createGameObject("player2Dummy", Tag::Dummy, player2->getPosition());
		
		auto origPhys = player2->getComponent<PhysicsComponent>();
		auto phys = player2Dummy->addComponent<PhysicsComponent>();
		vec2 size = origPhys->size * 0.7f; // <- make it a bit smaller than the original, since it lags behind
		phys->initBox(b2_staticBody, size, { player2->getPosition().x / physicsScale,player2->getPosition().y / physicsScale }, 1.0f);
	} else {
		player2Dummy->setRotation(player2->getRotation());
		player2Dummy->setPosition(player2->getPosition());
	}
}

void GameManager::BeginContact(b2Contact *contact) {
    b2ContactListener::BeginContact(contact);
    handleContact(contact, true);
}
void GameManager::EndContact(b2Contact *contact) {
    b2ContactListener::EndContact(contact);
    handleContact(contact, false);
}
void GameManager::handleContact(b2Contact *contact, bool begin) {
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

void GameManager::registerPhysicsComponent(PhysicsComponent *r) {
    physicsComponentLookup[r->fixture] = r;
}
void GameManager::deregisterPhysicsComponent(PhysicsComponent *r) {
    auto iter = physicsComponentLookup.find(r->fixture);
    if (iter != physicsComponentLookup.end()){
        physicsComponentLookup.erase(iter);
    } else {
        assert(false); // cannot find physics object
    }
}
#pragma endregion

#pragma region Input
void GameManager::onKey(SDL_Event &event) {
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
            case SDLK_t:
                // press 't' for physics and profiler debug
                doDebugDraw = !doDebugDraw;
				if (doDebugDraw) {
					if (world1 != nullptr && world2 != nullptr){
						world1->SetDebugDraw(&debugDraw);
						world2->SetDebugDraw(&debugDraw);
					}
					showProfiler = true;
                } else {
					if (world1 != nullptr && world2 != nullptr) {
						world1->SetDebugDraw(nullptr);
						world2->SetDebugDraw(nullptr);
					}
					showProfiler = false;
                }
                break;
			case SDLK_RETURN:
				if (gameState == GameState::Player1Won || gameState == GameState::Player2Won) {
					gameState = GameState::MainScreen;
					initMainScreen();
				} else if (gameState == GameState::Player1HasToCatch || gameState == GameState::Player2HasToCatch){
                    setGameState(GameState::Running);
					initNewRound();
				} else if (gameState == GameState::TimeUp) {
					if (player1HasToCatch) {
						setGameState(GameState::Player2HasToCatch);
					} else {
						setGameState(GameState::Player1HasToCatch);
					}
				} else if (gameState == GameState::MainScreen) {
					setGameState(GameState::Player1HasToCatch);
					initNewGame();
				}
                break;
        }
    }
}
#pragma endregion

#pragma region Rendering
void GameManager::render() {
    auto rp = RenderPass::create()
            .withCamera(camera->getCamera())
            .build();


	//Game-GUI
	if (gameState != GameState::MainScreen) {
		ImGuiCond cond = 0;
		ImGui::SetWindowPos(ImVec2(windowSize.x / 2 - (ImGui::GetWindowWidth() / 2.f), 0), cond);
		ImGui::GetStyle().Alpha = 0.5f;
		string text;
		if (player1HasToCatch) { text = "Blue Catch Red!"; }
		else { text = "Red Catch Blue!"; }

		ImGui::LabelText("Time", to_string(roundTime).c_str());
		ImGui::LabelText("Objective", text.c_str());
		ImGui::LabelText("Blue Score", to_string(player1Score).c_str());
		ImGui::LabelText("Red Score", to_string(player2Score).c_str());
	}

	//SRE-Profiler
	if (showProfiler) {
		static Profiler profiler;
		profiler.update();
		profiler.gui(true);
	}

	//Render Background first
    auto pos = camera->getGameObject()->getPosition();
    background->renderBackground(rp, +pos.x*0.7f);


	auto spriteBatchBuilder = SpriteBatch::create(); //<-- since sprite.setOrderInBatch(priority) and the order in which we add sprites to batches didn't work as axpected, we have one batch for each "layer" and draw them after each other
	shared_ptr<SpriteBatch> sb = nullptr;

	//Gamestate dependent Renders
	if (gameState == GameState::MainScreen) {
		auto sprite = guiSpriteAtlas->get("mindthegap.png");
		sprite.setScale(vec2(0.7f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
		sb = spriteBatchBuilder.build();
		rp.draw(sb);
		return;
	}

	if (gameState == GameState::Running) {
		spriteBatchBuilder = SpriteBatch::create();
		player1->renderSprite(spriteBatchBuilder);
		player2->renderSprite(spriteBatchBuilder);
		sb = spriteBatchBuilder.build();
		rp.draw(sb);

		spriteBatchBuilder = SpriteBatch::create();
		map->renderSprite(spriteBatchBuilder);
		sb = spriteBatchBuilder.build();
		rp.draw(sb);
	}

	if (gameState == GameState::Player1HasToCatch){
		auto sprite = guiSpriteAtlas->get("bpcrp.png");
		sprite.setScale(vec2(0.8f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
    } 

	if (gameState == GameState::Player2HasToCatch) {
		auto sprite = guiSpriteAtlas->get("rpcbp.png");
		sprite.setScale(vec2(0.8f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
	}

	if (gameState == GameState::Player1Won){
		auto sprite = guiSpriteAtlas->get("bpwins.png");
		sprite.setScale(vec2(0.7f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
    }

	if (gameState == GameState::Player2Won) {
		auto sprite = guiSpriteAtlas->get("rpwins.png");
		sprite.setScale(vec2(0.7f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
	}

	if (gameState == GameState::TimeUp) {
		auto sprite = guiSpriteAtlas->get("tu.png");
		sprite.setScale(vec2(0.8f));
		sprite.setPosition(pos);
		spriteBatchBuilder.addSprite(sprite);
	}

    sb = spriteBatchBuilder.build();
    rp.draw(sb);

	//foreground->renderForeground(rp, camera->orthographicSize , vec2{pos.x - windowSize.x/2, pos.y - windowSize.y / 2});

    if (doDebugDraw){
		if (world1 != nullptr && world2 != nullptr) {
			world1->DrawDebugData();
			world2->DrawDebugData();
			rp.drawLines(debugDraw.getLines());
			debugDraw.clear();
		}
    }
}
#pragma endregion





