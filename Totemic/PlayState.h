#pragma once

#include "GameState.h"
#include "Box2D\Box2D.h"
#include "dbtweener.h"
#include <Thor\Particles.hpp>
#include <Thor\Animation.hpp>
#include <Thor\Time\Timer.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include "Math.h"

class Player;
class LevelLoader;
class HotSpot;
class Level;
class ContactListener;
class ContactFilter;
class TotemTweenerListener;
class DeathcloudTweenListener;
class Trap;
class FloatingScoreText;
class Powerup;
class VCollection;
class DeathCloud;

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
	void setupGameWon();
	void onEnterTotem(Player* player);
	void addDeathcloud(sf::Vector2f position, sf::IntRect textureRect);
	void updateHoldingTotem(Player* player); // Sets m_holdingTotem = false except player
	void addTotemParticle(sf::IntRect textureRect);
	b2Body* createWall(sf::Vector2f v1, sf::Vector2f v2);

public:
	bool m_setupGameWon;
	bool m_gameWon;
	bool m_starting;
	bool m_totemIsBlockingPlayer;
	bool m_hasStartedToChangeWinBackgroundOpacity;
	bool m_321GO_timerExpired;

	b2World m_world;
	ContactListener* m_contactListener;
	ContactFilter* m_contactFilter;
	
	std::vector<unsigned int> m_mouseIndicies;
	std::vector<Player*> m_players;
	Player* m_leadingPlayer;
	std::vector<b2Body*> m_walls;
	std::vector<DeathCloud*> m_deathClouds;
	std::vector<FloatingScoreText*> m_floatingScoreTexts;
	HotSpot* m_hotSpot;
	Level* m_currentLevel;
	LevelLoader* m_levelLoader;

	// HUD
	sf::RectangleShape m_timerBarBackground;
	sf::RectangleShape m_lightningEffect;
	float m_lightningAlpha;
	float m_winBackgroundAlpha;

	sf::Sprite m_timerBar;
	sf::Sprite m_frame;
	sf::Sprite m_totemHead;
	sf::Sprite m_123GO;
	sf::Sprite m_totemFoot;
	sf::Sprite mWinBackground;

	thor::Animator<sf::Sprite, std::string> m_123GOAnimator;
	thor::Animator<sf::Sprite, std::string> m_totemHeadAnimator;
	thor::FrameAnimation m_totemHeadActiveAnimation;
	thor::FrameAnimation m_totemHeadIdleAnimation;
	thor::FrameAnimation m_123GOAnimation;
	thor::Timer m_321GOTimer;
	thor::Timer m_toMenuTimer;
	
	CDBTweener m_winGameTweener;
	CDBTweener m_totemTweener;
	CDBTweener m_deathcloudTweener;
	TotemTweenerListener* m_totemTweenerListener;
	DeathcloudTweenListener* m_deathcloudTweenListener;

	thor::UniversalEmitter* m_defenderEmitter;
	thor::ParticleSystem* m_defenderParticleSystem;
	sf::Texture m_defenderParticleTex;

	thor::UniversalEmitter* m_gathererDeathEmitter;
	thor::ParticleSystem* m_gathererDeathSystem;
	thor::ForceAffector* m_gathererDeathAffector;
	thor::TorqueAffector* m_gathererTorqueAffector;
	sf::Texture feather;
	sf::Texture featherRed;
	sf::Texture featherYellow;
	sf::Texture featherPurple;

	//the "arrowparticles"

	thor::UniversalEmitter* m_scoreIndicatorEmitter;
	thor::ParticleSystem* m_scoreIndicatorSystem;
	thor::ForceAffector* m_scoreIndicatorAffector;
	thor::StopWatch m_glowUpdate;
	sf::Texture m_glow1;

	sf::Vector2f m_leaderPoint;

	sf::Text mToMenuTimerText;

};
