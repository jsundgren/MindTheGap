#pragma once

#include "Component.hpp"
#include "PhysicsComponent.hpp"

using namespace std;

class GravityComponent;

enum GroundDirection4 {U, L, R, D};

class CharacterControllerComponent : public Component {
public:
    explicit CharacterControllerComponent(GameObject *gameObject);

	void update(float deltaTime) override;

    bool onKey(SDL_Event &event) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;

	bool onGround = false;
	bool player1 = true;
private:
	static float gravityFactor;
	static float density;
	static float damping;
	static float acceleration;
	static float airAcceleration;
	static float maximumVelocity;
	static float jumpForce;
	static float jumpTimer;
	static float jumpTimerWait;
	static float jumpTimerForce;

	void calcMovement(float deltaTime);
	void calcGravity(float deltaTime);
	void calcJump(float deltaTime);
	void setRotation();

	GroundDirection4 groundDirection = GroundDirection4::D;
	GravityComponent* getNearestGravityComp();
	shared_ptr<PhysicsComponent> phys;

	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool jump = false;
	
	float endTime;
	float timer = FLT_MAX;
};

