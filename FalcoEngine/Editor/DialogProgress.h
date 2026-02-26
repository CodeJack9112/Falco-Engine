#pragma once

#include <string>
#include <functional>

class DialogProgress
{
public:
	void update();
	void show();
	void hide();

	void setStatusText(std::string text) { statusText = text; }
	void setProgress(float p);
	void setTitle(std::string text) { title = text; }

private:
	std::string statusText;
	std::string title = "Progress";
	float progress = 0;

	bool visible = false;
};

