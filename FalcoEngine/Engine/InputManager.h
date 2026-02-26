#pragma once

#include <functional>
#include <map>

class InputManager
{
public:
	enum MouseButton { MBE_LEFT, MBE_RIGHT, MBE_MIDDLE };

	typedef std::function<void(int x, int y)> MouseEvent;
	typedef std::function<void(MouseButton mb, int x, int y)> MouseButtonEvent;

protected:
	static std::pair<int, int> mouseDir;
	static std::pair<int, int> mouseWheel;
	static std::pair<int, int> mousePos;
	static std::pair<int, int> relativeMousePos;
	static std::map<int, bool> keyStates;
	static std::map<int, bool> keyDownStates;
	static std::map<int, bool> keyUpStates;
	static bool mouseStates[32];
	static bool mouseDownStates[32];
	static bool mouseUpStates[32];

private:
	std::vector<MouseEvent>mouseMoveEvents;
	std::vector<MouseButtonEvent>mouseDownEvents;
	std::vector<MouseButtonEvent>mouseUpEvents;
	std::vector<MouseEvent>mouseWheelEvents;

public:
	enum EventType { MouseMove, MouseDown, MouseUp, MouseScrollWheel, KeyDown, KeyUp, KeyPress };

	InputManager();
	~InputManager();

	size_t SubscribeMouseMoveEvent(MouseEvent callback) { mouseMoveEvents.push_back(callback); return mouseMoveEvents.size() - 1; }
	size_t SubscribeMouseDownEvent(MouseButtonEvent callback) { mouseDownEvents.push_back(callback); return mouseDownEvents.size() - 1; }
	size_t SubscribeMouseUpEvent(MouseButtonEvent callback)   { mouseUpEvents.push_back(callback); return mouseUpEvents.size() - 1; }
	size_t SubscribeMouseWheelEvent(MouseEvent callback)   { mouseWheelEvents.push_back(callback); return mouseWheelEvents.size() - 1; }

	void UnsubscribeMouseMoveEvent(size_t id) { mouseMoveEvents.at(id) = nullptr; }
	void UnsubscribeMouseDownEvent(size_t id) { mouseDownEvents.at(id) = nullptr; }
	void UnsubscribeMouseUpEvent(size_t id)   { mouseUpEvents.at(id) = nullptr; }
	void UnsubscribeMouseWheelEvent(size_t id)   { mouseWheelEvents.at(id) = nullptr; }

	void MouseMoveEvent(int x, int y);
	void MouseDownEvent(MouseButton mb, int x, int y);
	void MouseUpEvent(MouseButton mb, int x, int y);
	void MouseWheelEvent(int x, int y);

	static bool GetMouseButton(int button);
	static void SetMouseButton(int button, bool pressed);

	static bool GetMouseButtonDown(int button);
	static void SetMouseButtonDown(int button, bool state);

	static bool GetMouseButtonUp(int button);
	static void SetMouseButtonUp(int button, bool state);

	static std::pair<int, int> GetMouseWheel();
	static void SetMouseWheel(int x, int y);

	static bool GetKey(int key);
	static void SetKey(int key, bool pressed);

	static bool GetKeyDown(int key);
	static void SetKeyDown(int key, bool state);

	static bool GetKeyUp(int key);
	static void SetKeyUp(int key, bool state);

	static void ResetKeys();

	static std::pair<int, int> GetMouseDirection();
	static void SetMouseDirection(int x, int y);

	static std::pair<int, int> GetMousePosition();
	static void SetMousePosition(int x, int y);

	static std::pair<int, int> GetMouseRelativePosition();
	static void SetMouseRelativePosition(int x, int y);
};

