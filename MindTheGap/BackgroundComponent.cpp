//
// Created by Morten Nobel-Jørgensen on 10/10/2017.
//

#include <sre/SpriteAtlas.hpp>
#include <sre/Texture.hpp>
#include <glm/gtx/transform.hpp>
#include "BackgroundComponent.hpp"
#include "GameManager.hpp"
#include <iostream>

using namespace sre;
using namespace glm;

BackgroundComponent::BackgroundComponent()
{
}

void BackgroundComponent::renderBackground(sre::RenderPass &renderPass, float offset) {
    
	renderPass.draw(batch, glm::translate(vec3(offset,0,0)));
}

void BackgroundComponent::renderForeground(sre::RenderPass & renderPass, float scale, vec2 pos)
{
	float scaleX = (GameManager::windowSize.x / tex->getWidth()) * scale * -1.f * 0.00000001f;
	float scaleY = (GameManager::windowSize.y / tex->getHeight()) * scale * -1.f * 0.00000001f;
	std::cout << scale << std::endl;
	spr.setScale({ scaleX, scaleY });
	auto batchBuilder = SpriteBatch::create();
	batchBuilder.addSprite(spr);
	batch = batchBuilder.build();
	renderPass.draw(batch, glm::translate(vec3(pos.x, pos.y, 0)));
}

void BackgroundComponent::init(std::string filename) {
    tex = Texture::create().withFile(filename)
            .withFilterSampling(false)
            .build();

    auto atlas = SpriteAtlas::createSingleSprite(tex,"background", vec2(0,0));
    spr = atlas->get("background");
    float scale = (GameManager::windowSize.y / tex->getHeight()) * 3.0f;
    spr.setScale({scale,scale});
    auto batchBuilder = SpriteBatch::create();
    for (int i=-5;i<5;i++){
		for (int l = -5; l < 5; l++) {
			spr.setPosition(vec2(tex->getWidth() * (i - 1) * scale, tex->getHeight() * (l - 1) * scale));
			batchBuilder.addSprite(spr);
		}
    }
    batch = batchBuilder.build();
}

void BackgroundComponent::initForeground(std::string filename)
{
	tex = Texture::create().withFile(filename)
		.withFilterSampling(false)
		.build();

	auto atlas = SpriteAtlas::createSingleSprite(tex, "foreground", vec2(0, 0));
	spr = atlas->get("foreground");
}

