#pragma once
#include "Box2D\Box2D.h"

#include "Component.hpp"
#include "CharacterControllerComponent.hpp"

using namespace sre;
using namespace glm;

enum GroundDirection8 {UM, UR, UL, MM, MR, ML, LM, LR, LL};

class GravityComponent : public Component {
public:
    explicit GravityComponent(GameObject *gameObject);

	vec2 getDampenedGravityVector(vec2 position);

	GroundDirection8 getPositionToAABB(vec2 position);
	static GroundDirection8 calcPositionToAABB(b2AABB aabb, vec2 position);
	static GroundDirection4 convertDirection8To4(GroundDirection8 direction);
private:
	vec2 calcGravityDampenVec(b2AABB aabb, vec2 position);
	float dampenCalculation(float middle, float position, float width);

	static float gravDampFactor;
};

