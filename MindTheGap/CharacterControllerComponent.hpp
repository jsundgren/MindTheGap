#pragma once

#include "Component.hpp"

using namespace std;

class BirdController : public Component {
public:
    explicit BirdController(GameObject *gameObject);

    bool onKey(SDL_Event &event) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;

	void update(float deltaTime) override;

private:
	static float gravityFactor;
	static float damping;
	static float movementSpeed;
	static float airMovementFactor;
	static float jumpForce;
	static float jumpTimer;

	bool left = false;
	bool right = false;
	bool space = false;
	bool onGround = false;

	shared_ptr<PhysicsComponent> phys;
	
	float endTime;
	float timer = 100000000000000000.0f;

	void movement(float deltaTime);
	void jump(float deltaTime);
	void setRotation();

};
