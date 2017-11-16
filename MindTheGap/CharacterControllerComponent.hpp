#pragma once

#include "Component.hpp"
#include "PhysicsComponent.hpp"

using namespace std;

class BirdController : public Component {
public:
    explicit BirdController(GameObject *gameObject);

    bool onKey(SDL_Event &event) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;

	void update(float deltaTime) override;

	bool onGround = false;

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

	bool left = false;
	bool right = false;
	bool space = false;

	shared_ptr<PhysicsComponent> phys;
	
	float endTime;
	float timer = 100000000000000000.0f;

	void movement(float deltaTime);
	void jump(float deltaTime);
	void setRotation();

};
