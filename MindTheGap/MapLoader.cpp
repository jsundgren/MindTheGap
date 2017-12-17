#include "MapLoader.h"
#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

using namespace std;
using namespace glm;
using namespace rapidjson;

// Reading the file and collecting the necessary data from the JSON.
void MapLoader::readFile(string filename) {
	ifstream fis(filename);
	IStreamWrapper isw(fis);
	Document d;
	d.ParseStream(isw);
	height = d["layers"][0]["height"].GetFloat();
	width = d["layers"][0]["width"].GetFloat();
	pair<glm::vec2, glm::vec2> pos;
	for (int i = 0; i < d["layers"].Size(); i++) {
		string name = d["layers"][i]["name"].GetString();
		if (name.find("FX") != std::string::npos) {
			//std::cout << "Skipping FX Layer: " << name << endl;
			continue;
		}
		if (name.find("Spawn") != std::string::npos) {
			//std::cout << "Found Spawn Layer: " << name << endl;
			Value& points = d["layers"][i]["data"];
			vector<int> values = readPoints(points, values);
			spawnPos = calcPosition(values);
			// Converting to coordinates in the game world
			spawnPos.first *= 32.0f;
			spawnPos.second *= 32.0f;
			//cout << "First: " << "(" << spawnPos.first.x << "," << spawnPos.first.y << ")" << endl;
			//cout << "Last: " << "(" << spawnPos.second.x << "," << spawnPos.second.y << ")" << endl;
			continue;
		}
		Value& points = d["layers"][i]["data"];
		vector<int> values = readPoints(points, values);
		pos = calcPosition(values);
		posVec.push_back(pos);
		//cout << "Layer: " << name << endl;
		//cout << "First: " << "(" << pos.first.x << "," << pos.first.y << ")" << endl;
		//cout << "Last: " << "(" << pos.second.x << "," << pos.second.y << ")" << endl;
	}
	sizePosVec = calcSizeandPos();
}
// Calculating the position of the sprite so its rendered in the center
vec2 MapLoader::getSpritePos() {
	float x = (width / 2) * 32;
	float y = (height / 2) * 32;
	return vec2(x, y);
}

void MapLoader::clear()
{
	width = 0;
	height = 0;
	posVec.clear();
	sizePosVec.clear();
}

vector<pair<vec2, vec2>> MapLoader::getSizePos() {
	return sizePosVec;
}

vector<int> MapLoader::readPoints(Value& point, std::vector<int> value) {
	for (SizeType i = 0; i < point.Size(); i++) {
		//point[i].GetUint();
		value.push_back(point[i].GetUint());
	}
	return value;
}

// Calculating the size and the positioning of the colliders and returning 
// them in a vector of pairs (vec2).
vector<pair<vec2, vec2>> MapLoader::calcSizeandPos() {
	vector<pair<vec2, vec2>> sizePos;
	for (int i = 0; i < posVec.size(); i++) {
		float w = abs(posVec[i].second.x - posVec[i].first.x);
		float h = abs(posVec[i].second.y - posVec[i].first.y);
		vec2 s = vec2(w+1, h+1);
		vec2 p = vec2(posVec[i].first.x + (s.x / 2), posVec[i].first.y + (s.y / 2));
		pair<vec2, vec2> sp;
		sp.first = s*16.0f;
		sp.second = p*32.0f;
		sp.second.y = (height * 32) - sp.second.y;
		sizePos.push_back(sp);
	}
	return sizePos;
}
// Reading the position of the top left and bottom right corner of the colliders. 
// Using this in the function CalcSizeandPos() above.
pair<vec2, vec2> MapLoader::calcPosition(vector<int> value) {
	vec2 *first = nullptr;
	vec2 *last = nullptr;
	int counter = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (value[j + counter] != 0) {
				if (first == nullptr) {
					first = new glm::vec2(j, i);
				}
				last = new glm::vec2(j, i);
			}
		}
		counter += width;
	}
	pair<vec2, vec2> pos;
	pos.first = *first;
	pos.second = *last;
	return pos;
}





