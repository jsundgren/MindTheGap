//
// Created by Morten Nobel-Jørgensen on 19/10/2017.
//

#include <SDL_events.h>
#include <iostream>
#include "BirdController.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "BirdGame.hpp"
#include "SpriteComponent.hpp"

BirdController::BirdController(GameObject *gameObject) : Component(gameObject) {
    // initiate bird physics
	gameObject->getComponent<PhysicsComponent>()->setLinearVelocity(glm::vec2(1,0));
}



bool BirdController::onKey(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
        std::cout << "Space key pressed" << std::endl;
		gameObject->getComponent<PhysicsComponent>()->addImpulse(glm::vec2(0,0.15));
    } else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE){
        std::cout << "Space key released" << std::endl;
    }
    return false;
}

void BirdController::onCollisionStart(PhysicsComponent *comp) {
    std::cout << "bird collided with something" << std::endl;
	if (comp->getGameObject()->name == "Coin") {
		std::cout << "hit coin" << std::endl;	
		comp->getGameObject()->removeComponent(comp->getGameObject()->getComponent<SpriteComponent>());
	} else {
		BirdGame::instance->setGameState(GameState::GameOver);
	}
}

void BirdController::onCollisionEnd(PhysicsComponent *comp) {

}



