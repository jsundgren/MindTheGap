#pragma once

#include "Component.hpp"
#include "sre/SpriteBatch.hpp"
#include "sre/Texture.hpp"
#include "sre/RenderPass.hpp"
#include "sre\Sprite.hpp"

class BackgroundComponent {
public:
    BackgroundComponent();
	void init(std::string filename);
	void initForeground(std::string filename);
    void renderBackground(sre::RenderPass& renderPass, float offset);
	void renderForeground(sre::RenderPass& renderPass, float scale, glm::vec2 pos);
private:
    std::shared_ptr<sre::Texture> tex;
    std::shared_ptr<sre::SpriteBatch> batch;
	sre::Sprite spr;
};
