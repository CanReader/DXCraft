#pragma once
#include <vector>
#include <functional>

#define add_event(eventname, func, obj) \
    EventManager::getManager()->eventname.push_back(std::bind(func, obj));

#define add_event_1(eventname, func, obj) \
    EventManager::getManager()->eventname.push_back(std::bind(func, obj, std::placeholders::_1));

#define add_event_2(eventname, func, obj) \
    EventManager::getManager()->eventname.push_back(std::bind(func, obj, std::placeholders::_1,std::placeholders::_2));

class EventManager {
public:
	EventManager(EventManager&) = delete;
	EventManager(EventManager&&) = delete;

	std::vector<std::function<void(int width, int height)>> onWindowSizeChanged;
	std::vector<std::function<void(bool isFocusLost)>> onWindowFocusChanged;
	std::vector<std::function<void(int x, int y)>> onMouseMove;
	std::vector<std::function<void()>> onWindowPaint;
	std::vector<std::function<void()>> onMouseDownL;
	std::vector<std::function<void()>> onMouseUpL;
	std::vector<std::function<void()>> onMouseDownR;
	std::vector<std::function<void()>> onMouseUpR;
	std::vector<std::function<void(int)>> onKeyPressed;

	
	static EventManager* getManager() {
		if (instance == nullptr)
			instance = new EventManager();
		return instance;
	}

private:
	EventManager();

	static EventManager* instance;
};
