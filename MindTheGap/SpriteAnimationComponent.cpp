//
// Created by Morten Nobel-Jørgensen on 10/10/2017.
//

#include "SpriteAnimationComponent.hpp"
#include "GameObject.hpp"
#include "sre\SpriteAtlas.hpp"
#include "sre\Sprite.hpp"
#include <iostream>
#include <memory>

using namespace std;
using namespace sre;

SpriteAnimationComponent::SpriteAnimationComponent(GameObject *gameObject) : Component(gameObject) {
}

void SpriteAnimationComponent::update(float deltaTime) {

	auto physComp = gameObject->getComponent<PhysicsComponent>();
	auto spriteComp = gameObject->getComponent<SpriteComponent>();
	auto contrlComp = gameObject->getComponent<BirdController>();

	assert(physComp != nullptr);
    assert(spriteComp != nullptr);
	assert(contrlComp != nullptr);
    time += deltaTime;

	vector<Sprite> spriteAnim;
	Sprite final;

	int index;
	if (time > animationTime) {
		time = fmod(time, animationTime);
		spriteIndex = (spriteIndex + 1) % 1000; //<- hereby setting the maximum Animation lengtht to 1000 frames, i just didn't want the number to get ridiculously high. 
	}

	if (contrlComp->onGround) {
		float horizVelocity = abs(physComp->getLinearVelocity().x);

		if (horizVelocity < FLT_EPSILON) {
			//Idle
			spriteAnim = idleSprites;
		} else if (horizVelocity < walkThreshold) {
			//Walk
			spriteAnim = walkSprites;
		} else {
			//Run
			spriteAnim = runSprites;
		}
	} else {
		float vertVelocity = physComp->getLinearVelocity().y;

		if (abs(vertVelocity) < FLT_EPSILON) {
			//MidAir
			spriteAnim = jumpMidSprites;
		} else if (vertVelocity > 0) {
			//JumpUp
			spriteAnim = jumpUpSprites;
		} else {
			//JumpFall
			spriteAnim = jumpFallSprites;
		}
	}

	final = spriteAnim[spriteIndex % spriteAnim.size()];

	float horizVelocity = physComp->getLinearVelocity().x;
	if (horizVelocity < 0) {
		//Mirror to Left
		final.setFlip({true, false});
	} else {
		//Mirror to Right
		final.setFlip({false, false});
	}

    spriteComp->setSprite(final);
}
int SpriteAnimationComponent::calcAnimatedSpriteIndex(float time, int size) {
	int res = 0;
	if (time > animationTime) {
		time = fmod(time, animationTime);
		res = (spriteIndex + 1) % size;
	}
	return res;
}

void SpriteAnimationComponent::setPlayerSprites(std::shared_ptr<sre::SpriteAtlas> spriteAtlas) {
	vector<sre::Sprite> res({
		spriteAtlas->get("OPP_Idle (1).png"),
		spriteAtlas->get("OPP_Idle (2).png"),
		spriteAtlas->get("OPP_Idle (3).png"),
		spriteAtlas->get("OPP_Idle (4).png"),
		spriteAtlas->get("OPP_Idle (5).png"),
		spriteAtlas->get("OPP_Idle (6).png"),
		spriteAtlas->get("OPP_Idle (7).png"),
		spriteAtlas->get("OPP_Idle (8).png"),
	});
	this->idleSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_Walk (1).png"),
		spriteAtlas->get("OPP_Walk (2).png"),
		spriteAtlas->get("OPP_Walk (3).png"),
		spriteAtlas->get("OPP_Walk (4).png"),
		spriteAtlas->get("OPP_Walk (5).png"),
		spriteAtlas->get("OPP_Walk (6).png"),
		spriteAtlas->get("OPP_Walk (7).png"),
		spriteAtlas->get("OPP_Walk (8).png"),
	});
	this->walkSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_Run (1).png"),
		spriteAtlas->get("OPP_Run (2).png"),
		spriteAtlas->get("OPP_Run (3).png"),
		spriteAtlas->get("OPP_Run (4).png"),
		spriteAtlas->get("OPP_Run (5).png"),
		spriteAtlas->get("OPP_Run (6).png"),
	});
	this->runSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_JumpUp (1).png"),
		spriteAtlas->get("OPP_JumpUp (2).png"),
	});
	this->jumpUpSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_JumpMid (1).png"),
		spriteAtlas->get("OPP_JumpMid (2).png"),
	});
	this->jumpMidSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_JumpFall (1).png"),
		spriteAtlas->get("OPP_JumpFall (2).png"),
	});
	this->jumpFallSprites = res;

	res = vector<sre::Sprite>({
		spriteAtlas->get("OPP_JumpLand (1).png"),
		spriteAtlas->get("OPP_JumpLand (2).png"),
	});
	this->jumpLandSprites = res;
}

float SpriteAnimationComponent::getAnimationTime() const {
    return animationTime;
}

void SpriteAnimationComponent::setAnimationTime(float animationTime) {
    SpriteAnimationComponent::animationTime = animationTime;
}
