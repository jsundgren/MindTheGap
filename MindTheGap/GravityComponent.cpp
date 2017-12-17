#include "Box2D\Box2D.h"

#include "GravityComponent.hpp"
#include "GameObject.hpp"
#include "GameManager.hpp"
#include "DoublePhysicsComponent.hpp"

#include <iostream>
#include <cmath>

#pragma region Static Initialization
float GravityComponent::gravDampFactor = 0.025f;
#pragma endregion

GravityComponent::GravityComponent(GameObject *gameObject) : Component(gameObject) {
	GameManager::instance->gravityComponents.push_back(this);
}

vec2 GravityComponent::getDampenedGravityVector(vec2 position) {

	//Bacause the GravityComponente will only be added to static platforms, body from world1 is enough
	auto phys = gameObject->getComponent<DoublePhysicsComponent>();
	auto body = phys->body1;

	//Get the colliders AABB
	b2AABB aabb;
	b2Transform trans = body->GetTransform();
	body->GetFixtureList()->GetShape()->ComputeAABB(&aabb, trans, 0);
	aabb.lowerBound *= GameManager::instance->physicsScale;
	aabb.upperBound *= GameManager::instance->physicsScale;

	//return the dampened Gravity vector according to aabb and the players position
	return calcGravityDampenVec(aabb, position);
}

GroundDirection8 GravityComponent::getPositionToAABB(vec2 position) {
	//Get the position to platform via aabb and player position
	auto phys = gameObject->getComponent<DoublePhysicsComponent>();
	assert(phys != nullptr);

	auto body = phys->body1;

	b2AABB aabb;
	b2Transform trans = body->GetTransform();
	body->GetFixtureList()->GetShape()->ComputeAABB(&aabb, trans, 0);
	aabb.lowerBound *= GameManager::instance->physicsScale;
	aabb.upperBound *= GameManager::instance->physicsScale;

	return calcPositionToAABB(aabb, position);
}

vec2 GravityComponent::calcGravityDampenVec(b2AABB aabb, vec2 position){

	//Get Position of Character relative to Collider
	GroundDirection8 posToAABB = calcPositionToAABB(aabb, position);

	//Calc some values for use later
	float width = abs(aabb.upperBound.x - aabb.lowerBound.x);
	float height = abs(aabb.upperBound.y - aabb.lowerBound.y);
	float horizMiddle = (aabb.upperBound.x + aabb.lowerBound.x) / 2;
	float vertMiddle = (aabb.upperBound.y + aabb.lowerBound.y) / 2;

	//Calc the gravity Vector
	vec2 grav = gameObject->getPosition() - position; 
	//and shorten it, so that it points to the surface of the Collider, not its center
	//that had to be done, because the inverse square law had a to big fall off for bigger
	//platforms compared to smaller ones
	switch (posToAABB) {//<- just an approximation, raycast would be more precise. Doesn't affect game feel, so we stuck to it
		case UL: {
			grav.x -= width / 2;
			grav.y += height / 2;
			break;
		}
		case UM: {
			grav.y += height/2 ;
			break;
		}
		case UR: {
			grav.x += width / 2;
			grav.y += height / 2;
			break;
		}
		case ML: {
			grav.x -= width / 2;
			break;
		}
		case MR: {
			grav.x += width / 2;
			break;
		}
		case LL: {
			grav.x -= width / 2;
			grav.y -= height / 2;
			break;
		}
		case LM: {
			grav.y -= height / 2;
			break;
		}
		case LR: {
			grav.x += width / 2;
			grav.y -= height/ 2;
			break;
		}
		default: {
		}
	}

	//Calc dampening factors
	float factorX = 1;
	float factorY = 1;

	//Get the actual dampening factors
	switch (posToAABB) {
		case UM: {
			factorX = dampenCalculation(horizMiddle, position.x, width);
			break;
			}
		case ML: {
			factorY = dampenCalculation(vertMiddle, position.y, height);
			break;
		}
		case MR: {
			factorY = dampenCalculation(vertMiddle, position.y, height);
			break;
		}
		case LM: {
			factorX = dampenCalculation(horizMiddle, position.x, width);
			break;
		}
		default: {
			factorY = 0.4;
			factorX = 0.4;
		}
	}

	//return grav after dampening
	return vec2(grav.x * factorX, grav.y * factorY);
}

float GravityComponent::dampenCalculation(float middle, float position, float width) {

	//How fr is the player standing towards a corner?
	float dist = abs(middle - position);
	dist /= (width / 2);

	//let the dampen factor grow towards the corner, to a max of gravDampFactor, because dist is between [0,1]
	float gravityFactor = sqrt(dist) * gravDampFactor;
	return gravityFactor;
}

GroundDirection8 GravityComponent::calcPositionToAABB(b2AABB aabb, vec2 position) {
	//Get Position of Character relative to Collider
	GroundDirection8 res;

	if (position.y >= aabb.upperBound.y) {
		//Above
		if (position.x >= aabb.upperBound.x) {
			//Upper Right
			res = UR;
		}
		else if (position.x <= aabb.lowerBound.x) {
			//Upper Left
			res = UL;
		}
		else {
			//Upper Middle
			res = UM;
		}
	}
	else if (position.y <= aabb.lowerBound.y) {
		//Below
		if (position.x >= aabb.upperBound.x) {
			//Lower Right
			res = LR;
		}
		else if (position.x <= aabb.lowerBound.x) {
			//Lower Left
			res = LL;
		}
		else {
			//Lower Middle
			res = LM;
		}
	}
	else {
		//Middle
		if (position.x >= aabb.upperBound.x) {
			//Middle Right
			res = MR;
		}
		else if (position.x <= aabb.lowerBound.x) {
			//Middle Left
			res = ML;
		}
		else {
			//Middle Middle
			res = MM;
		}
	}

	return res;
}

GroundDirection4 GravityComponent::convertDirection8To4(GroundDirection8 direction)
{
	if (direction == GroundDirection8::UL || direction == GroundDirection8::UM || direction == GroundDirection8::UR) {
		return GroundDirection4::D;
	}
	if (direction == GroundDirection8::ML) {
		return GroundDirection4::R;
	}
	if (direction == GroundDirection8::MR) {
		return GroundDirection4::L;
	}
	if (direction == GroundDirection8::LL || direction == GroundDirection8::LM || direction == GroundDirection8::LR) {
		return GroundDirection4::U;
	}
}
