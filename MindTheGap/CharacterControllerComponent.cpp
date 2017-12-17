#include "Box2D\Box2D.h"

#include "CharacterControllerComponent.hpp"
#include "GameManager.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "DoublePhysicsComponent.hpp"
#include "GravityComponent.hpp"

#include <glm/gtx/vector_angle.hpp>

#include <time.h>
#include <SDL_events.h>
#include <iostream>

using namespace glm;

#pragma region Static Initialization
float CharacterControllerComponent::gravityFactor = 4.5f;
float CharacterControllerComponent::density = 0.5f;
float CharacterControllerComponent::damping = 2.5f;
float CharacterControllerComponent::acceleration = 3.0f;
float CharacterControllerComponent::airAcceleration = 1.0f;
float CharacterControllerComponent::maximumVelocity = 10.0f;
float CharacterControllerComponent::jumpForce = 1.5f;
float CharacterControllerComponent::jumpTimer = 0.3f;
float CharacterControllerComponent::jumpTimerWait = 0.05f;
float CharacterControllerComponent::jumpTimerForce = 0.03f;
#pragma endregion

CharacterControllerComponent::CharacterControllerComponent(GameObject *gameObject) : Component(gameObject) {
	
	//Character Controllers allways need a PhysicsComponent to be attached to the same object
	phys = gameObject->getComponent<PhysicsComponent>();

	//some general Physics initializations
	phys->body->SetFixedRotation(true);
	phys->body->SetLinearDamping(damping);
	phys->body->SetGravityScale(gravityFactor);
	phys->body->GetFixtureList()->SetDensity(density);
	phys->body->ResetMassData();
}

void CharacterControllerComponent::update(float deltaTime) {
	calcMovement(deltaTime);
	calcGravity(deltaTime);
	calcJump(deltaTime);
	setRotation();
}

void CharacterControllerComponent::calcMovement(float deltaTime) {

	//Movement
	vec2 movement = vec2(0);
	float currentAcceleration = onGround ? acceleration : airAcceleration;

	if (left) { movement.x--; }
	if (right) { movement.x++; }
	if (down) { movement.y--; }
	if (up) { movement.y++; }

	phys->addImpulse(movement * currentAcceleration * deltaTime);

	//Clamp Movement
	vec2 velocity = phys->getLinearVelocity();

	if (abs(length(velocity)) > maximumVelocity) {
		velocity = normalize(velocity) * maximumVelocity;
		phys->setLinearVelocity(velocity);
	}
}

void CharacterControllerComponent::calcGravity(float deltaTime) {
    float x = 0;
	float y = 0;
	int size = GameManager::instance->gravityComponents.size();
	if (size == 0) return; //<- do nothing, if there are no gravity Components

	//Get nearest GravityComponent for later
	GravityComponent* nearest = getNearestGravityComp();

	//Iterate through Gravity Components
	for each (GravityComponent* gravComp in GameManager::instance->gravityComponents)
	{

		//get the already dampened gravity vector of each one
		vec2 grav = gravComp->getDampenedGravityVector(gameObject->getPosition());
		float strength = length(grav);

		//Some manipulations to inverse Square Law
		float power = 2.0f;
		float xAxisTranslation = 20.0f;
		float yAxisScale = 1 / 3000.0f;

		float inverseSquare = 1 / (pow(strength + xAxisTranslation, power) * yAxisScale);

		//Clamp the factor above 1%, so that the player cannot not "fly" away
		if (inverseSquare <= 0.01) {
			inverseSquare = 0.03f;
		}

		//Apply Inverse Square Law
		grav *= inverseSquare;

		//Nearest gravity should pull with 100% force all the others with 5%
		if (gravComp != nearest) {
			grav *= 0.05f; 
		}

		//Add the components of the gravity vectors
		x += grav.x;
		y += grav.y;
	}

	//To average them later
	x /= size;
	y /= size;

	//Apply gravity to the specific world
	b2Vec2 newGrav = b2Vec2(x , y);
	if (player1) {
		GameManager::instance->world1->SetGravity(newGrav);
	} else {
		GameManager::instance->world2->SetGravity(newGrav);
	}
}

void CharacterControllerComponent::calcJump(float deltaTime) {

	vec2 jumpVector = vec2(0);

	//Depending on the position regarding the platform, we want to jump in different directions
	switch (groundDirection) {
		case D: { jumpVector.y = 1; break; }
		case L: { jumpVector.x = 1; break; }
		case R: { jumpVector.x = -1; break; }
		case U: { jumpVector.y = -1; break; }
	}

	timer += deltaTime;

	//Not only add the impuls for the jump, but also start the timer
	if (jump && onGround) {
		timer = 0;
		phys->addImpulse(jumpVector * jumpForce);
		onGround = false;
	}

	//Long Jump (holding the jump button, slightly inceases jump height)
	if (jump && timer > jumpTimerWait && timer < jumpTimer) {
		phys->addImpulse(jumpVector * jumpTimerForce);
	}
}

void CharacterControllerComponent::setRotation() {

	//Depending on the position to the nearest gravity component, we rotate the player
	GravityComponent* nearest = getNearestGravityComp();
	GroundDirection8 posToAABB = nearest->getPositionToAABB(gameObject->getPosition());

	switch (posToAABB) {
		case UM: {
			gameObject->setRotation(0);
			break;
		}
		case UL: {
			gameObject->setRotation(45);
			break;
		}
		case ML: {
			gameObject->setRotation(90);
			break;
		}
		case LL: {
			gameObject->setRotation(135);
			break;
		}
		case LM: {
			gameObject->setRotation(180);
			break;
		}
		case LR: {
			gameObject->setRotation(225);
			break;
		}
		case MR: {
			gameObject->setRotation(270);
			break;
		}
		case UR: {
			gameObject->setRotation(315);
			break;
		}
		default: {
		}
	}
}


GravityComponent* CharacterControllerComponent::getNearestGravityComp() {
	float min = FLT_MAX;
	GravityComponent* nearest = nullptr;

	for each (GravityComponent* gravComp in GameManager::instance->gravityComponents)
	{
		float dist = distance(gravComp->getGameObject()->getPosition(), this->gameObject->getPosition());
		if (dist < min) {
			nearest = gravComp;
			min = dist;
		}
	}

	return nearest;
}

bool CharacterControllerComponent::onKey(SDL_Event &event) {
	if (player1) {
		switch (event.key.keysym.sym) {
				//PLAYER 1
			case SDLK_RALT:
			{
				jump = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_j:
			{
				left = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_l:
			{
				right = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_i:
			{
				up = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_k:
			{
				down = event.type == SDL_KEYDOWN;
				break;
			}
		}
	} else {
		switch (event.key.keysym.sym) {
				//PLYER 2
			case SDLK_LALT:
			{
				jump = event.type == SDL_KEYDOWN;;
				break;
			}
			case SDLK_a:
			{
				left = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_d:
			{
				right = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_w:
			{
				up = event.type == SDL_KEYDOWN;
				break;
			}
			case SDLK_s:
			{
				down = event.type == SDL_KEYDOWN;
				break;
			}
		}
	}

	return false;
}

void CharacterControllerComponent::onCollisionStart(PhysicsComponent *comp) {
	if (comp->getGameObject()->tag == Tag::Ground) {

		//Get physics Comp
		auto phys = (DoublePhysicsComponent*)comp;
		auto body = phys->body1;

		//To get the aabb
		b2AABB aabb;
		b2Transform trans = body->GetTransform();
		body->GetFixtureList()->GetShape()->ComputeAABB(&aabb, trans, 0);
		aabb.lowerBound *= GameManager::instance->physicsScale;
		aabb.upperBound *= GameManager::instance->physicsScale;

		//To get the players position regarding this ground
		GroundDirection8 dir8 = GravityComponent::calcPositionToAABB(aabb, gameObject->getPosition());
		GroundDirection4 dir4 = GravityComponent::convertDirection8To4(dir8);
		
		//To be able to jump in the right direction
		groundDirection = dir4;
		onGround = true;
	}

	if (comp->getGameObject()->tag == Tag::Dummy) {
			cout << "----------- Player touched Player ------------" << endl;
			GameManager::instance->playerTouchedPlayer();
	}
}

void CharacterControllerComponent::onCollisionEnd(PhysicsComponent *comp) {
	if (comp->getGameObject()->tag == Tag::Ground) {
		onGround = false;
	}
}






