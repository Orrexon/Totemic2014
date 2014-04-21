#pragma once

#include "GameState.h"
#include "Box2D\Box2D.h"
#include "dbtweener.h"

class Player;
class LevelLoader;
class HotSpot;
class Level;
class ContactListener;
class ContactFilter;
class TotemTweenerListener;

namespace thor
{
	class UniversalEmitter;
}

namespace sf
{
	class RectangleShape;
}


class PlayState : public GameState
{
public:
	PlayState();
	~PlayState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();

	void initManyMouse();
	void initPlayers();
	void loadNewLevel();
	void createPlayerBodies();
	void sortTotem();

	b2Body* createWall(sf::Vector2f v1, sf::Vector2f v2);
private:
	b2World m_world;
	ContactListener* m_contactListener;
	ContactFilter* m_contactFilter;

	std::vector<unsigned int> m_mouseIndicies;
	std::vector<Player*> m_players;
	std::vector<b2Body*> m_walls;
	HotSpot* m_hotSpot;
	Level* m_currentLevel;
	LevelLoader* m_levelLoader;

	// HUD
	sf::RectangleShape* m_timerBarBackground;
	sf::RectangleShape* m_timerBar;

	CDBTweener m_totemTweener;
	TotemTweenerListener* m_totemTweenerListener;
	std::vector<int> m_totemPlayerIndices;

	thor::UniversalEmitter* m_playerParticleEmitter;
};
