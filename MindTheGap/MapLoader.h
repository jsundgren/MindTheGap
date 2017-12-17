
#include <vector>
#include "glm/glm.hpp"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <string>

#pragma once
using namespace rapidjson;
using namespace std;
using namespace glm;
class MapLoader
{
public:
	void readFile(string filename);
	vector<int> readPoints(Value& point, vector<int> value);
	pair<vec2, vec2> calcPosition(vector<int> value);
	vector<pair<vec2,vec2>> calcSizeandPos();
	vector<pair<vec2, vec2>> getSizePos();
	pair<vec2, vec2> spawnPos;
	vec2 getSpritePos();

	void clear();
private:
	float width = 0;
	float height = 0;
	vector<pair<vec2, vec2>> posVec;
	vector<pair<vec2, vec2>> sizePosVec;
};

