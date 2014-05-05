#pragma once

#include <SFML\Window\Event.hpp>
#include <Thor\Resources.hpp>
#include <vector>
#include <memory>

class GameState;
class WindowManager;
class GameStateManager;
class ResourceHolder;
class Audiosystem;

namespace thor
{
	template <typename T> class ActionMap;
}

struct StateAsset
{
	WindowManager* windowManager;
	GameStateManager* gameStateManager;
	thor::ActionMap<std::string>* actionMap;
	ResourceHolder* resourceHolder;
	Audiosystem* audioSystem;
};

class GameStateManager
{
public:
	GameStateManager();
	~GameStateManager();

	void changeState(GameState* gameState);
	void popState();
	void pushState(GameState* gameState);
	void notifyRevealedStates();
	void notifyObscuredStates();
	void draw();
	void pushEvents(sf::Event event);
	void clearEvents();

	bool updateActiveStates(float dt);
	StateAsset* getStateAsset();
private:
	std::vector<GameState*> m_activeStates;
	int m_currentStateIdentifier;
	StateAsset* m_stateAsset;
};

