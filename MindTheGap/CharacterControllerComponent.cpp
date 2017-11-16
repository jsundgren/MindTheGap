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
#include "Box2D\Box2D.h"
#include <time.h>

using namespace glm;

#pragma region Static Initialization
float BirdController::gravityFactor = 4.0f;
float BirdController::density = 0.5f;
float BirdController::damping = 1.0f;
float BirdController::acceleration = 3.0f;
float BirdController::airAcceleration = 3.0f;
float BirdController::maximumVelocity = 7.0f;
float BirdController::jumpForce = 1.5f;
float BirdController::jumpTimer = 0.25f;
float BirdController::jumpTimerWait = 0.05f;
float BirdController::jumpTimerForce = 0.05f;
#pragma endregion

BirdController::BirdController(GameObject *gameObject) : Component(gameObject) {
	phys = gameObject->getComponent<PhysicsComponent>();
	phys->body->SetFixedRotation(true);
	phys->body->SetLinearDamping(damping);
	phys->body->SetGravityScale(gravityFactor);
	phys->body->GetFixtureList()->SetDensity(density);
	phys->body->ResetMassData();
	cout << "Density: " << phys->body->GetFixtureList()->GetDensity() << " results in mass: " << phys->body->GetMass() << endl;

}

void BirdController::update(float deltaTime) {
	movement(deltaTime);
	jump(deltaTime);
	setRotation();
}

bool BirdController::onKey(SDL_Event &event) {

	switch (event.key.keysym.sym) {
	case SDLK_SPACE:
	{
		space = event.type == SDL_KEYDOWN;;
	}
	break;
	case SDLK_LEFT:
	{
		left = event.type == SDL_KEYDOWN;
	}
	break;
	case SDLK_RIGHT:
	{
		right = event.type == SDL_KEYDOWN;
	}
	break;
	}

	return false;
}

void BirdController::movement(float deltaTime) {

	//Movement
	vec2 movement = vec2(0);
	float currentAcceleration = onGround ? acceleration : airAcceleration;

	if (left) { movement.x--; }
	if (right) { movement.x++; }
	phys->addImpulse(movement * currentAcceleration * deltaTime);

	//Clamp Movement
	vec2 velocity = phys->getLinearVelocity();
	if (abs(velocity.x) > maximumVelocity) {
		velocity.x = sign(velocity.x) * maximumVelocity;
		phys->setLinearVelocity(velocity);
	}


	//std::cout << "x: " << gameObject->getPosition().x << "y: " << gameObject->getPosition().y << std::endl;
	//std::cout << "left: " << left << "right: " << right << std::endl;
}


void BirdController::jump(float deltaTime) {
	//cout << time(0) << endl;
	timer += deltaTime;

	if (space && onGround) {
		timer = 0;
		phys->addImpulse(vec2(0, jumpForce));
		onGround = false;
	}

	if (space && timer > jumpTimerWait && timer < jumpTimer) {
		phys->addImpulse(vec2(0, jumpTimerForce));
		cout << "tick";
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



