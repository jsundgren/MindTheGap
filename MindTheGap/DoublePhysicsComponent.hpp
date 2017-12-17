#pragma once

#include "Box2D/Common/b2Math.h"
#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Box2D/Collision/Shapes/b2CircleShape.h"
#include "Box2D/Dynamics/b2Body.h"

#include "PhysicsComponent.hpp"
#include "Component.hpp"

class DoublePhysicsComponent : public PhysicsComponent {
public:
    explicit DoublePhysicsComponent(GameObject *gameObject);

    void initCircle(b2BodyType type, float radius,glm::vec2 center,float density) override;
    void initBox(b2BodyType type, glm::vec2 size,glm::vec2 center,float density) override;

	b2World* world1 = nullptr;
	b2World* world2 = nullptr;
	b2Body * body1 = nullptr;
	b2Body * body2 = nullptr;
private:

};

