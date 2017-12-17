#include <Box2D/Box2D.h>

#include "DoublePhysicsComponent.hpp"
#include "GameManager.hpp"

#include "iostream"

DoublePhysicsComponent::DoublePhysicsComponent(GameObject *gameObject)
        : PhysicsComponent(gameObject)
{
    world1 = GameManager::instance->world1;
	world2 = GameManager::instance->world2;
}

void DoublePhysicsComponent::initCircle(b2BodyType type, float radius, glm::vec2 center, float density) {
    assert(body1 == nullptr);
    
    shapeType = b2Shape::Type::e_circle;
    b2BodyDef bd;
    bd.type = type;
    rbType = type;
    bd.position = b2Vec2(center.x, center.y);
    body1 = world1->CreateBody(&bd);
    circle = new b2CircleShape();
    circle->m_radius = radius;
    b2FixtureDef fxD;
    fxD.shape = circle;
    fxD.density = density;
    fixture = body1->CreateFixture(&fxD);

    GameManager::instance->registerPhysicsComponent(this);

	//Also create this in the second world, not sure about references, so just copying the code

	assert(body2 == nullptr);
	
	shapeType = b2Shape::Type::e_circle;
	bd.type = type;
	rbType = type;
	bd.position = b2Vec2(center.x, center.y);
	body2 = world2->CreateBody(&bd);
	circle = new b2CircleShape();
	circle->m_radius = radius;
	fxD.shape = circle;
	fxD.density = density;
	fixture = body2->CreateFixture(&fxD);

	GameManager::instance->registerPhysicsComponent(this);
}

void DoublePhysicsComponent::initBox(b2BodyType type, glm::vec2 size, glm::vec2 center, float density) {
    
	assert(body1 == nullptr);
    
	shapeType = b2Shape::Type::e_polygon;
    b2BodyDef bd;
    bd.type = type;
    rbType = type;
    bd.position = b2Vec2(center.x, center.y);
    body1 = world1->CreateBody(&bd);
    polygon = new b2PolygonShape();
    polygon->SetAsBox(size.x, size.y, {0,0}, 0);
    b2FixtureDef fxD;
    fxD.shape = polygon;
    fxD.density = density;
    fixture = body1->CreateFixture(&fxD);

    GameManager::instance->registerPhysicsComponent(this);

	//Also create this in the second world, not sure about references, so just copying the code

	assert(body2 == nullptr);
	
	shapeType = b2Shape::Type::e_polygon;
	bd.type = type;
	rbType = type;
	bd.position = b2Vec2(center.x, center.y);
	body2 = world2->CreateBody(&bd);
	polygon = new b2PolygonShape();
	polygon->SetAsBox(size.x, size.y, { 0,0 }, 0);
	fxD.shape = polygon;
	fxD.density = density;
	fixture = body2->CreateFixture(&fxD);

	GameManager::instance->registerPhysicsComponent(this);
}