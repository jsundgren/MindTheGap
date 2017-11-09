//
// Created by Morten Nobel-Jørgensen on 19/10/2017.
//

#include <SDL_events.h>
#include <iostream>
#include "CharacterControllerComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "BirdGame.hpp"
#include "SpriteComponent.hpp"
#include <time.h>

using namespace glm;

#pragma region Static Initialization
float BirdController::gravityFactor = 6.0f;
float BirdController::damping = 3.0f;
float BirdController::movementSpeed = 50.0f;
float BirdController::airMovementFactor = 0.8f;
float BirdController::jumpForce = 0.2f;
float BirdController::jumpTimer = 0.5f;
#pragma endregion

BirdController::BirdController(GameObject *gameObject) : Component(gameObject) {
	phys = gameObject->getComponent<PhysicsComponent>();
	phys->body->SetFixedRotation(true);
	phys->body->SetLinearDamping(damping);
	phys->body->SetGravityScale(gravityFactor);


}

void BirdController::update(float deltaTime) {
	movement(deltaTime);
	jump(deltaTime);
	setRotation();
}

bool BirdController::onKey(SDL_Event &event) {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) {
		left = true;
	}
	if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_LEFT) {
		left = false;
	}

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) {
		right = true;
	}
	if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_RIGHT) {
		right = false;
	}

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
		space = true;
	}
	if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE) {
		space = false;
	}

	return false;
}

void BirdController::movement(float deltaTime) {

	vec2 velocity = vec2(0);//phys->getLinearVelocity();

	vec2 leftVec = vec2(0);
	vec2 rightVec = vec2(0);

	float speed = movementSpeed;

	if (!onGround) {
		speed *= airMovementFactor;
	}

	if (left) {
		leftVec = vec2(-speed, 0);
		leftVec *= deltaTime;
	}
	if (right) {
		rightVec = vec2(speed, 0);
		rightVec *= deltaTime;
	}

	velocity = velocity + leftVec + rightVec;

	phys->addForce(velocity);

	//phys->setLinearVelocity(velocity);

	//std::cout << "x: " << gameObject->getPosition().x << "y: " << gameObject->getPosition().y << std::endl;
	//std::cout << "left: " << left << "right: " << right << std::endl;
}


void BirdController::jump(float deltaTime) {
	//cout << time(0) << endl;
	timer += deltaTime;

	if (space && onGround) {
		timer = 0;
	}

	if (space && timer < jumpTimer) {
		phys->addImpulse(vec2(0, jumpForce));
	}
}

void BirdController::setRotation() {
}

void BirdController::onCollisionStart(PhysicsComponent *comp) {
	if (comp->getGameObject()->tag == Tag::Ground) {
		//std::cout << "Touching Ground" << std::endl;
		onGround = true;
	}
}

void BirdController::onCollisionEnd(PhysicsComponent *comp) {
	if (comp->getGameObject()->tag == Tag::Ground) {
		//std::cout << "UnTouching Ground" << std::endl;
		onGround = false;
	}
}



