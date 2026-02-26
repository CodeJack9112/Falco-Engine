#pragma once

#include <string>

using namespace std;

struct UIEvent
{
public:
	UIEvent() {}
	~UIEvent() {}

	string sceneNode = "None";
	string functionName = "None";
};