#pragma once

#include "Component.hpp"
#include "sre/Sprite.hpp"


using namespace glm;

class SpriteComponent : public Component {
public:
    explicit SpriteComponent(GameObject *gameObject);

    void renderSprite(sre::SpriteBatch::SpriteBatchBuilder &spriteBatchBuilder) override;

	void setColor(vec4 color);
	void setRenderOder(int priority);

    void setSprite(const sre::Sprite& sprite);
    sre::Sprite getSprite();
private:
    sre::Sprite sprite;

	vec4 color = vec4(1);
	int priority = 0;
};

