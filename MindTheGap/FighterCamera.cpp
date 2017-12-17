#include "FighterCamera.hpp"
#include "GameManager.hpp"
#include "SpriteComponent.hpp"

#include <iostream>
#include <algorithm>

using namespace glm;

FighterCamera::FighterCamera(GameObject *gameObject)
        : Component(gameObject)
{
    camera.setOrthographicProjection(GameManager::windowSize.y/2,-1,1);
}

sre::Camera &FighterCamera::getCamera() {
    return camera;
}

void FighterCamera::update(float deltaTime) {
	updateMultiplayerCamera();
}

void FighterCamera::updateMultiplayerCamera() {
	if (GameManager::instance->player1 != nullptr && GameManager::instance->player2 != nullptr) {
		
		//Some Inits
		float minDist = 450;
		vector<float> xPos;
		vector<float> yPos;

		//push the x and y values of the players postion onto a vector, to later find min and max values
		xPos.push_back(GameManager::instance->player1->getPosition().x);
		xPos.push_back(GameManager::instance->player2->getPosition().x);
		yPos.push_back(GameManager::instance->player1->getPosition().y);
		yPos.push_back(GameManager::instance->player2->getPosition().y);

		float xMax = *std::max_element(xPos.begin(), xPos.begin() + xPos.size());
		float xMin = *std::min_element(xPos.begin(), xPos.begin() + xPos.size());
		float yMax = *std::max_element(yPos.begin(), yPos.begin() + yPos.size());
		float yMin = *std::min_element(yPos.begin(), yPos.begin() + yPos.size());

		//Get the averaged middle
		float xMiddle = (xMin + xMax) / 2;
		float yMiddle = (yMin + yMax) / 2;

		//Get the size, by choosing the size, that still contains all x and y values
		float orthographicSize = std::max(xMax - xMin, yMax - yMin);

		//Clamp orthographic size
		if (orthographicSize < minDist) {
			orthographicSize = minDist;
		}

		//Smooth out the Camera movement over time
		float smoothSpeed = 0.125f;
		vec2 position = vec2(xMiddle, yMiddle);
		glm::vec2 desPosition = position + offset;
		glm::vec2 smoothPos = glm::mix(gameObject->getPosition(), desPosition, smoothSpeed);
		position = smoothPos;

		gameObject->setPosition(position);
		vec3 eye(position, 0);
		vec3 at(position, -1);
		vec3 up(0, 1, 0);

		camera.lookAt(eye, at, up);
		camera.setOrthographicProjection(orthographicSize * 0.7f, -1, 1);
	}
}
