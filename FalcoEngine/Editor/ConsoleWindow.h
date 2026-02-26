#pragma once

#include <OgreVector4.h>
#include <vector>
#include <string>

enum LogMessageType { LMT_INFO, LMT_WARNING, LMT_SUCCESS, LMT_ERROR, LMT_AUTODETECT };

class ConsoleWindow
{
public:
	ConsoleWindow();
	~ConsoleWindow();

	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void log(std::string value, LogMessageType type = LMT_AUTODETECT);
	void clear();
	void setClearOnPlay(bool value) { clearOnPlay = value; }
	bool getClearOnPlay() { return clearOnPlay; }

	void setScrollToBottom(bool val) { scrollTo = val; }

private:
	struct LogMessage
	{
	public:
		std::string text = "";
		Ogre::Vector4 color = Ogre::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		LogMessageType type = LogMessageType::LMT_AUTODETECT;

		LogMessage() = default;
		LogMessage(std::string msg, Ogre::Vector4 col = Ogre::Vector4(1.0f, 1.0f, 1.0f, 1.0f), LogMessageType msgType = LogMessageType::LMT_AUTODETECT)
		{
			text = msg;
			color = col;
			type = msgType;
		}
	};

	bool clearOnPlay = true;
	bool opened = true;
	int current_item = 0;
	std::vector<LogMessage> logData;

	bool scrollTo = false;
};