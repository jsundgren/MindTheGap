#include "sre/SDLRenderer.hpp"
#include "sre/SpriteAtlas.hpp"
#include <vector>
#include "Box2D/Dynamics/b2World.h"
#include "GameObject.hpp"
#include "SideScrollingCamera.hpp"
#include "BackgroundComponent.hpp"
#include "Box2DDebugDraw.hpp"

class PhysicsComponent;

enum class GameState{
    Ready,
    Running,
    GameOver
};

class BirdGame : public b2ContactListener {
public:

    BirdGame();

	#pragma region Physics Fields
		void BeginContact(b2Contact *contact) override;
		void EndContact(b2Contact *contact) override;

		b2World * world = nullptr;
	#pragma endregion

	#pragma region GameManager Fields
		static BirdGame* instance;
		static const glm::vec2 windowSize;

		void setGameState(GameState newState);
		std::shared_ptr<GameObject> createGameObject();
	#pragma endregion

private:

	#pragma region GameManager Fields
		GameState gameState = GameState::Ready;
		void init();
		void update(float time);

		std::vector<std::shared_ptr<GameObject>> sceneObjects;
		BackgroundComponent background1Component;
		BackgroundComponent background2Component;
	#pragma endregion

	#pragma region Physics Fields
		void initPhysics();
		void updatePhysics();
		const float physicsScale = 100;
		
		void handleContact(b2Contact *contact, bool begin);

		void registerPhysicsComponent(PhysicsComponent *r);
		void deregisterPhysicsComponent(PhysicsComponent *r);
		
		std::map<b2Fixture*,PhysicsComponent *> physicsComponentLookup;
		
		Box2DDebugDraw debugDraw;
		bool doDebugDraw = false;
	#pragma endregion

	#pragma region Input
		void onKey(SDL_Event &event);
	#pragma endregion

	#pragma region Rendering
		sre::SDLRenderer r;
		void render();

		std::shared_ptr<SideScrollingCamera> camera;
		std::shared_ptr<sre::SpriteAtlas> spriteAtlas;
	#pragma endregion

    friend class PhysicsComponent;
};



