#include "GameObject.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>
#include "Component.hpp"
#include "PhysicsComponent.hpp"
#include "DoublePhysicsComponent.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>
#include "GameManager.hpp"


GameObject::~GameObject(){
   // remove reference to this in components
   for (auto& c : components){
        c->gameObject = nullptr;
    }
	cout << " ~ Deleted: " << name << endl;
}

bool GameObject::removeComponent(std::shared_ptr<Component> component) {
    auto comp = std::find(components.begin(), components.end(),component);
    if (comp != components.end()){
        components.erase(comp);
    }
    return false;
}

const glm::vec2 &GameObject::getPosition() const {
    return position;
}

void GameObject::setPosition(const glm::vec2 &position) {

	//Also set the colliders position
	auto phys = this->getComponent<PhysicsComponent>();
	if (phys != nullptr) {
		b2Vec2 newPos = b2Vec2(position.x / GameManager::instance->physicsScale, position.y / GameManager::instance->physicsScale);
		phys->body->SetTransform(newPos, radians(rotation));
	}

    GameObject::position = position;
}

float GameObject::getRotation() const {
    return rotation;
}

void GameObject::setRotation(float rotation) {

	//Also set the colliders rotation
	auto phys = this->getComponent<PhysicsComponent>();
	if (phys != nullptr) {
		b2Body* body = phys->body;
		body->SetTransform(body->GetPosition(), radians(rotation));
	}
    GameObject::rotation = rotation;
}

void GameObject::renderSprite(sre::SpriteBatch::SpriteBatchBuilder &spriteBatchBuilder) {
    for (auto& comp : components){
        comp->renderSprite(spriteBatchBuilder);
    }
}

void GameObject::update(float deltaTime) {
    for (auto& comp : components){
        comp->update(deltaTime);
    }
}

const std::vector<std::shared_ptr<Component>> &GameObject::getComponents() {
    return components;
}
