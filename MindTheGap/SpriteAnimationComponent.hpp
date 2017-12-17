#pragma once

#include "sre/Sprite.hpp"

#include "Component.hpp"
#include "SpriteComponent.hpp"
#include "CharacterControllerComponent.hpp"
#include "PhysicsComponent.hpp"

using namespace std;
using namespace sre;

class SpriteAnimationComponent: public Component {
public:
    SpriteAnimationComponent(GameObject *gameObject);
    void update(float deltaTime) override;

    void setPlayerSprites(std::shared_ptr<sre::SpriteAtlas> spriteAtlas);

    float getAnimationTime() const;
    void setAnimationTime(float animationTime);
private:
	vector<Sprite> idleSprites;
    vector<Sprite> walkSprites;
	vector<Sprite> runSprites;
	vector<Sprite> jumpUpSprites;
	vector<Sprite> jumpMidSprites;
	vector<Sprite> jumpFallSprites;
	vector<Sprite> jumpLandSprites;

    float animationTime = 0.133f;
	float walkThreshold = 3.5f;
    float time = 0;
    int spriteIndex = 0;
};


