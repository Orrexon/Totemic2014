#pragma once

#include "GameState.h"
#include "Box2D\Box2D.h"
#include "dbtweener.h"
#include <Thor\Particles.hpp>

class Player;
class LevelLoader;
class HotSpot;
class Level;
class ContactListener;
class ContactFilter;
class TotemTweenerListener;
class Trap;
class FloatingScoreText;
class Powerup;

namespace thor
{
	class UniversalEmitter;
	class StopWatch;
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
	void createPowerup();
	void setupWinTweeners();
	void onEnterTotem(Player* player);
	void updateHoldingTotem(Player* player); // Sets m_holdingTotem = false except player

	b2Body* createWall(sf::Vector2f v1, sf::Vector2f v2);

public:
	bool m_gameWon;
	bool m_totemIsBlockingPlayer;

	b2World m_world;
	ContactListener* m_contactListener;
	ContactFilter* m_contactFilter;

	std::vector<unsigned int> m_mouseIndicies;
	std::vector<Player*> m_players;
	std::vector<b2Body*> m_walls;
	std::vector<FloatingScoreText*> m_floatingScoreTexts;
	HotSpot* m_hotSpot;
	Level* m_currentLevel;
	LevelLoader* m_levelLoader;

	// HUD
	sf::RectangleShape m_timerBarBackground;
	
	sf::RectangleShape m_lightningEffect;
	float m_lightningAlpha;

	sf::Sprite m_timerBar;
	sf::Sprite m_frame;
	sf::Sprite m_totemHead;
	sf::Sprite m_totemFoot;

	CDBTweener *m_winGameTweener;
	CDBTweener m_totemTweener;
	TotemTweenerListener* m_totemTweenerListener;

	thor::ParticleSystem* m_defenderParticleSystem;
	thor::UniversalEmitter* m_defenderEmitter;
	thor::ParticleSystem* m_timerParticleSystem;
	thor::UniversalEmitter* m_timerEmitter;
	thor::Connection m_timerEmitterConnection;
};
