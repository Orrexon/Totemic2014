#pragma once

#include <SFML\Window\Event.hpp>
#include <Thor\Resources.hpp>
#include <Thor\Animation.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\Text.hpp>
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

struct PlayerMenu
{
	std::string m_key_ready;
	bool m_ready;
	sf::Text m_readyText;
	sf::Sprite m_sprite;
	sf::Sprite m_fireSprite;
	thor::Animator<sf::Sprite, std::string> m_fireAnimator;
	thor::Animator<sf::Sprite, std::string> m_animator;
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

public: // Public variables
	std::vector<PlayerMenu> m_players;
	bool m_useJoysticks;
};

