#pragma once

namespace thor
{
	template <typename T> class ActionMap;
}

namespace sf
{
	class RenderWindow;
}

class GameStateManager;
class WindowManager;
class ResourceHolder;
class Audiosystem;

class Engine
{
public:
	Engine();
	~Engine();

	bool init(std::string p_title);
	void exit();
	void loop();
	void updateDeltaTime();
	void updateEvents();

	WindowManager* getWindowManager();

private:
	Audiosystem* m_audioSystem;
	WindowManager* m_windowManager;
	GameStateManager* m_gameStateManager;
	ResourceHolder* m_resourceHolder;
	thor::ActionMap<std::string>* m_actionMap;

	bool m_running;
};