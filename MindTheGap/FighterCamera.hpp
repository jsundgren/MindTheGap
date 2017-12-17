#pragma once

#include "sre/Camera.hpp"
#include "glm/glm.hpp"

#include "Component.hpp"

class FighterCamera : public Component {
public:
    explicit FighterCamera(GameObject *gameObject);

	void updateMultiplayerCamera();

    void update(float deltaTime) override;

    sre::Camera& getCamera();

	float orthographicSize;

private:
    sre::Camera camera;
    std::shared_ptr<GameObject> followObject;
    glm::vec2 offset;
};

