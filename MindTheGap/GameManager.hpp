#include "sre/SDLRenderer.hpp"
#include "sre/SpriteAtlas.hpp"

#include "Box2D/Dynamics/b2World.h"
#include "Box2DDebugDraw.hpp"

#include "MapLoader.h"
#include "GameObject.hpp"
#include "FighterCamera.hpp"
#include "GravityComponent.hpp"
#include "BackgroundComponent.hpp"

#include <vector>


using namespace glm;

enum class GameState{
    Running,
    Player1Won,
	Player2Won,
	Player1HasToCatch,
	Player2HasToCatch,
	TimeUp,
	MainScreen
};

class GameManager : public b2ContactListener {
public:

    GameManager();

	#pragma region GameManager Fields
		static GameManager* instance;
		vector<GravityComponent*> gravityComponents;
		static const glm::vec2 windowSize;
		static float roundDuration;

		void setGameState(GameState newState);
		std::shared_ptr<GameObject> createGameObject(std::string name, Tag tag, vec2 position);

		std::shared_ptr<GameObject> player1;
		std::shared_ptr<GameObject> player2;
		std::shared_ptr<GameObject> cameraObj;

		std::shared_ptr<FighterCamera> camera;

		void playerTouchedPlayer();
	#pragma endregion

	#pragma region Physics Fields
		void BeginContact(b2Contact *contact) override;
		void EndContact(b2Contact *contact) override;

		b2World * world1 = nullptr;
		b2World * world2 = nullptr;

		const float physicsScale = 100;
	#pragma endregion

private:

	#pragma region GameManager Fields
		GameState gameState = GameState::MainScreen;

		void initMainScreen();
		void initNewGame();
		void initNewRound();

		void update(float time);
		
		std::shared_ptr<GameObject> generateMap(int id);

		std::vector<std::shared_ptr<GameObject>> sceneObjects;
		std::shared_ptr<GameObject> map;
		std::shared_ptr<GameObject> player2Dummy;

		MapLoader* mapLoader;

		float roundTime = 0.0f;
		int roundCount = 0;
		bool player1HasToCatch = true;
		int player1Score = 0;
		int player2Score = 0;
	#pragma endregion

	#pragma region Physics Fields
		void initPhysics();
		void updatePhysics();
		void update2WorldPhysics();		

		std::map<b2Fixture*,PhysicsComponent *> physicsComponentLookup;
		void registerPhysicsComponent(PhysicsComponent *r);
		void deregisterPhysicsComponent(PhysicsComponent *r);

		void handleContact(b2Contact *contact, bool begin);
	#pragma endregion

	#pragma region Input
		void onKey(SDL_Event &event);
	#pragma endregion

	#pragma region Rendering
		sre::SDLRenderer r;
		void render();

		bool showProfiler = false;
		bool doDebugDraw = false;
		Box2DDebugDraw debugDraw;

		std::shared_ptr<BackgroundComponent> background;
		std::shared_ptr<BackgroundComponent> foreground;

		std::shared_ptr<sre::SpriteAtlas> playerSpriteAtlas;
		std::shared_ptr<sre::SpriteAtlas> mapSpriteAtlas;
		std::shared_ptr<sre::SpriteAtlas> guiSpriteAtlas;
	#pragma endregion

    friend class PhysicsComponent;
	friend class DoublePhysicsComponent;
};



