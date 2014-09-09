#include <SFML\Graphics.hpp>
#include <Thor\Math.hpp>
#include <Thor\Particles.hpp>
#include <iostream>
#include "PlayState.h"
#include "MenuState.h"

#include "Deathcloud.h"
#include "GameStateAssets.h"
#include "Powerup.h"
#include "Config.h"
#include "Player.h"
#include "HotSpot.h"
#include "manymouse.hpp"
#include "LevelLoader.h"
#include "Level.h"
#include "FileSystem.h"
#include "Math.h"
#include "PhysicsHelper.h"
#include "Gatherer.h"
#include "Defender.h"
#include "Trap.h"
#include "Coin.h"
#include "Box2DWorldDraw.h"
#include "ContactListener.h"
#include "FloatingScoreText.h"
#include "ContactFilter.h"
#include "Coinbird.h"
#include "TotemTweenerListener.h"
#include "DeathcloudTweenListener.h"
#include "TotemParticle.h"
#include "TotemParticleListener.h"

PlayState::PlayState() : m_world(b2Vec2(0.f, 0.f))
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_leadingPlayer = nullptr;
	m_hasStartedToChangeWinBackgroundOpacity = false;
	m_exclusive = false;
	m_gameWon = false;
	m_hasStarted321GOTimer = false;
	m_setupGameWon = false;
	m_isShowingTimerText = false;
	m_totemIsBlockingPlayer = false;
	m_starting = true;
	m_321GO_timerExpired = false;

	m_totemTweenerListener = new TotemTweenerListener();
	m_deathcloudTweenListener = new DeathcloudTweenListener();

	m_contactListener = new ContactListener();
	m_contactFilter = new ContactFilter();

	m_world.SetAllowSleeping(true);
	m_world.SetContactListener(m_contactListener);
	m_world.SetContactFilter(m_contactFilter);

	m_mouseIndicies.resize(4);
	initManyMouse();
	initPlayers();

	m_levelLoader = new LevelLoader();
	m_levelLoader->setDirectory("../levels/");
	m_levelLoader->setResourceHolder(m_stateAsset->resourceHolder);
	m_currentLevel = nullptr;

	m_hotSpot = new HotSpot();

	loadNewLevel();

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0;

	b2Vec2 position = PhysicsHelper::gameToPhysicsUnits(m_players.front()->getTotemSprite()->getPosition());
	bodyDef.position = position;
	b2Body* body = m_world.CreateBody(&bodyDef);

	b2CircleShape circleShape;
	circleShape.m_radius = PhysicsHelper::gameToPhysicsUnits(64);

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.restitution = 0;
	fixtureDef.friction = 1;
	fixtureDef.shape = &circleShape;

	body->CreateFixture(&fixtureDef);

	m_totemHead.setTexture(m_stateAsset->resourceHolder->getTexture("totem_head.png"));
	m_totemHead.setOrigin(129, 157);
	m_totemHead.setPosition(m_players.back()->getTotemSprite()->getPosition().x, m_players.back()->getTotemSprite()->getPosition().y + m_players.back()->getTotemSprite()->getGlobalBounds().height / 2.f + 8);

	int width = 263;
	int height = 157;
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 0, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 1, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 2, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 3, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 4, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 5, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 6, height, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 7, height, 263, 157));

	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 0, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 1, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 2, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 3, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 4, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 5, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 6, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 7, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 8, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 9, 0, 263, 157));
	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(width * 10, 0, 263, 157));

	m_totemHeadAnimator.addAnimation("active", m_totemHeadActiveAnimation, sf::seconds(0.6));
	m_totemHeadAnimator.addAnimation("idle", m_totemHeadIdleAnimation, sf::seconds(0.6));
	m_totemHeadAnimator.playAnimation("idle", true);

	m_totemFoot.setTexture(m_stateAsset->resourceHolder->getTexture("totemfoot.png"));
	m_totemFoot.setOrigin(m_totemFoot.getGlobalBounds().width / 2.f, 0.f);
	m_totemFoot.setPosition(m_players.front()->getTotemSprite()->getPosition().x, m_players.front()->getTotemSprite()->getPosition().y + m_players.front()->getTotemSprite()->getGlobalBounds().height / 2.f - 4);

	m_walls.push_back(createWall(sf::Vector2f(FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH + 32), sf::Vector2f(1920 - FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH + 32))); // ⍐
	m_walls.push_back(createWall(sf::Vector2f(1920 - FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH), sf::Vector2f(1920 - FRAME_BORDER_WIDTH, 1080 - FRAME_BORDER_WIDTH))); // ⍈
	m_walls.push_back(createWall(sf::Vector2f(FRAME_BORDER_WIDTH, 1080 - FRAME_BORDER_WIDTH), sf::Vector2f(1920 - FRAME_BORDER_WIDTH, 1080 - FRAME_BORDER_WIDTH))); // ⍗
	m_walls.push_back(createWall(sf::Vector2f(FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH), sf::Vector2f(FRAME_BORDER_WIDTH, 1080 - FRAME_BORDER_WIDTH))); // ⍇

	m_timerBarBackground.setSize(sf::Vector2f(TIMER_WIDTH, TIMER_HEIGHT));
	m_timerBarBackground.setPosition(TIMER_POS_X, TIMER_POS_Y);
	m_timerBarBackground.setFillColor(TIMER_BG_COLOR);

	m_timerBar.setTexture(m_stateAsset->resourceHolder->getTexture("BarRed.png"));
	m_timerBar.setTextureRect(sf::IntRect(0, 0, 0, 0));
	m_timerBar.setPosition(TIMER_POS_X, TIMER_POS_Y);

	m_frame.setTexture(m_stateAsset->resourceHolder->getTexture("guiframe.png"));

	m_lightningEffect.setSize(sf::Vector2f(1920, 1080));
	m_lightningEffect.setFillColor(sf::Color(255, 255, 255, 0));
	m_lightningAlpha = 0.f;

	m_defenderEmitter = new thor::UniversalEmitter;
	m_defenderParticleSystem = new thor::ParticleSystem;
	m_defenderParticleTex.loadFromFile("../assets/textures/particles.png", sf::IntRect(0, 0, 47, 15));
	m_defenderParticleSystem->setTexture(m_defenderParticleTex);
	m_defenderParticleSystem->addTextureRect(sf::IntRect(1, 1, 13, 13));
	m_defenderParticleSystem->addTextureRect(sf::IntRect(16, 0, 14, 15));
	m_defenderParticleSystem->addTextureRect(sf::IntRect(33, 0, 13, 15));

	m_gathererDeathEmitter = new thor::UniversalEmitter;
	m_gathererDeathSystem = new thor::ParticleSystem;
	feather.loadFromFile("../assets/textures/death_particle_blue.png", sf::IntRect(0, 0, 16, 16));
	featherRed.loadFromFile("../assets/textures/death_particle_red.png", sf::IntRect(0, 0, 16, 16));
	featherYellow.loadFromFile("../assets/textures/death_particle_yellow.png", sf::IntRect(0, 0, 16, 16));
	featherPurple.loadFromFile("../assets/textures/death_particle_purple.png", sf::IntRect(0, 0, 16, 16));
	m_gathererDeathSystem->setTexture(feather);
	m_gathererDeathAffector = new thor::ForceAffector(sf::Vector2f(0.f, 0.f));
	m_gathererDeathAffector->setAcceleration(sf::Vector2f(0.f, 9.98f));
	m_gathererTorqueAffector = new thor::TorqueAffector(0.f);
	m_gathererTorqueAffector->setAngularAcceleration(90.f);

	m_scoreIndicatorEmitter = new thor::UniversalEmitter;
	m_scoreIndicatorSystem = new thor::ParticleSystem;
	m_scoreIndicatorAffector = new thor::ForceAffector(sf::Vector2f(0.f, 20.f));
	m_glow1.loadFromFile("../assets/textures/sparcle_particles_001.png", sf::IntRect(0, 0, 40, 8));
	scoreTextureIndex0 = m_scoreIndicatorSystem->addTextureRect(sf::IntRect(16, 0, 8, 8));
	scoreTextureIndex1 = m_scoreIndicatorSystem->addTextureRect(sf::IntRect(32, 0, 8, 8));
	scoreTextureIndex2 = m_scoreIndicatorSystem->addTextureRect(sf::IntRect(8, 0, 8, 8));
	scoreTextureIndex3 = m_scoreIndicatorSystem->addTextureRect(sf::IntRect(24, 0, 8, 8));
	m_scoreIndicatorSystem->setTexture(m_glow1);
	m_leaderPoint = { 0.f, 0.f };
	//m_glowUpdate.restart();
	updateGlow = false;

	m_123GO.setTexture(m_stateAsset->resourceHolder->getTexture("321go.png"));
	m_123GO.setOrigin(418, 186);
	m_123GO.setPosition(m_stateAsset->windowManager->getWindow()->getSize().x / 2.f, m_stateAsset->windowManager->getWindow()->getSize().y / 2.f);

	m_123GOAnimation.addFrame(1.f, sf::IntRect(1672, 0, 836, 372));
	m_123GOAnimation.addFrame(1.f, sf::IntRect(836, 0, 836, 372));
	m_123GOAnimation.addFrame(1.f, sf::IntRect(0, 0, 836, 372));
	m_123GOAnimation.addFrame(1.f, sf::IntRect(2508, 0, 836, 372));

	m_123GOAnimator.addAnimation("idle", m_123GOAnimation, sf::seconds(4));
	
	m_321GOTimer.reset(sf::seconds(PLAYSTATE_SECONDS_BEFORE_COUNTDOWN));
	m_321GOTimer.start();

	m_stateAsset->audioSystem->playMusic("Bamboozle", true);

	mWinBackground.setTexture(m_stateAsset->resourceHolder->getTexture("win.png"));
	m_winBackgroundAlpha = 0.f;

	m_hotSpot->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("totem_glitter_anim.png"));
	m_hotSpot->getSprite()->setOrigin(224, 224);
	m_hotSpot->getSprite()->setPosition(m_stateAsset->windowManager->getWindow()->getSize().x / 2.f, m_stateAsset->windowManager->getWindow()->getSize().y / 2.f);

	mToMenuTimerText.setFont(m_stateAsset->resourceHolder->getFont(DEFAULT_FONT));
	mToMenuTimerText.setCharacterSize(44);
	mToMenuTimerText.setPosition(m_stateAsset->windowManager->getWindow()->getSize().x / 2.f + 400, 1080 + 100);
	m_toMenuTimerTextY = mToMenuTimerText.getPosition().y;

	m_totemParticleListener = new TotemParticleListener();
	m_totemParticleTimer.restart(sf::seconds(TOTEM_PARTICLE_RATE));
}

void PlayState::leaving()
{
	delete m_totemTweenerListener;
	m_totemTweenerListener = nullptr;

	delete m_totemParticleListener;
	m_totemParticleListener = nullptr;

	auto it_players = m_players.begin();
	while (it_players != m_players.end())
	{
		if (*it_players != nullptr)
		{
			delete *it_players;
			*it_players = nullptr;
		}
		++it_players;
	}
	m_players.clear();

	auto it_deathclouds = m_deathClouds.begin();
	while (it_deathclouds != m_deathClouds.end())
	{
		if (*it_players != nullptr)
		{
			delete *it_deathclouds;
			*it_deathclouds = nullptr;
		}
		++it_deathclouds;
	}
	m_deathClouds.clear();

	auto it_totemParticle = m_totemParticles.begin();
	while (it_totemParticle != m_totemParticles.end())
	{
		if (*it_totemParticle != nullptr)
		{
			delete *it_totemParticle;
			*it_totemParticle = nullptr;
		}
		++it_totemParticle;
	}
	m_totemParticles.clear();

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_contactListener;
	m_contactListener = nullptr;

	delete m_contactFilter;
	m_contactFilter = nullptr;

	delete m_deathcloudTweenListener;
	m_deathcloudTweenListener = nullptr;

	delete m_gathererDeathEmitter;
	m_gathererDeathEmitter = nullptr;

	delete m_gathererTorqueAffector;
	m_gathererTorqueAffector = nullptr;

	delete m_gathererDeathAffector;
	m_gathererDeathAffector = nullptr;

	delete m_gathererDeathSystem;
	m_gathererDeathSystem = nullptr;

	delete m_defenderParticleSystem;
	m_defenderParticleSystem = nullptr;

	delete m_defenderEmitter;
	m_defenderEmitter = nullptr;

	delete m_levelLoader;
	m_levelLoader = nullptr;

	m_stateAsset->audioSystem->getMusic("Bamboozle")->stop();
}

void PlayState::obscuring()
{
}

void PlayState::releaving()
{
}

bool PlayState::update(float dt)
{
	updateGlow = false;
	/*for (auto& player : m_players)
	{
		player->getGatherer()->getBody()->SetAngularDamping(10.f);
	}*/
	m_deathcloudTweener.step(dt);

	// Update deathclouds
	auto deathCloudIterator = m_deathClouds.begin();
	while (deathCloudIterator != m_deathClouds.end())
	{
		(*deathCloudIterator)->sprite.rotate(DEATHCLOUD_ROTATION_SPEED);
		sf::Color oldColor = (*deathCloudIterator)->sprite.getColor();
		oldColor.a = (*deathCloudIterator)->alphaTween;
		sf::Color newColor = oldColor;
		(*deathCloudIterator)->sprite.setColor(newColor);

		if ((*deathCloudIterator)->borned)
		{
			if ((*deathCloudIterator)->timer.isExpired() && !(*deathCloudIterator)->isDying)
			{
				CDBTweener::CTween* tween = new CDBTweener::CTween();
				tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::EEasing::TWEA_OUT, DEATH_CLOUD_FADE_OUT_TIME);
				tween->addValue(&(*deathCloudIterator)->alphaTween, 0);
				tween->addListener(m_deathcloudTweenListener);
				tween->setUserData((*deathCloudIterator));
				m_deathcloudTweener.addTween(tween);
				(*deathCloudIterator)->isDying = true;
			}
		}

		if ((*deathCloudIterator)->dead)
		{
			delete *deathCloudIterator;
			*deathCloudIterator = nullptr;
			deathCloudIterator = m_deathClouds.erase(deathCloudIterator);
		}
		else
		{
			++deathCloudIterator;
		}
	}

	// Update totem particles
	auto itTotemParticle = m_totemParticles.begin();
	while (itTotemParticle != m_totemParticles.end())
	{
		TotemParticle* totemParticle = (*itTotemParticle);

		sf::Color oldColor = totemParticle->m_sprite->getColor();
		oldColor.a = totemParticle->fadeInAlpha;
		totemParticle->m_sprite->setColor(oldColor);

		if (totemParticle->borned)
		{
			if (totemParticle->deathTimer.isExpired())
			{
				delete totemParticle;
				totemParticle = nullptr;
				itTotemParticle = m_totemParticles.erase(itTotemParticle);
			}
			else
			{
				++itTotemParticle;
			}
		}
		else
		{
			++itTotemParticle;
		}
	}

	if (m_gameWon)
	{
		if (!m_hasStartedToChangeWinBackgroundOpacity)
		{
			m_hasStartedToChangeWinBackgroundOpacity = true;
			CDBTweener::CTween* changeWinBackgroundOpacityTween = new CDBTweener::CTween();
			changeWinBackgroundOpacityTween->setEquation(&CDBTweener::TWEQ_SINUSOIDAL, CDBTweener::TWEA_OUT, 0.5f);
			changeWinBackgroundOpacityTween->addValue(&m_winBackgroundAlpha, 255);
			m_winGameTweener.addTween(changeWinBackgroundOpacityTween);
		}

		sf::Color oldColor = mWinBackground.getColor();
		oldColor.a = static_cast<int>(m_winBackgroundAlpha);
		mWinBackground.setColor(oldColor);

		if (!m_setupGameWon)
			setupGameWon();

		m_winGameTweener.step(dt);

		for (auto &player : m_players)
		{
			player->getDefender()->getAnimatior()->update(sf::seconds(dt));
			player->getGatherer()->getAnimatior()->update(sf::seconds(dt));

			player->getDefender()->getAnimatior()->animate(*player->getDefender()->getSprite());
			player->getGatherer()->getAnimatior()->animate(*player->getGatherer()->getSprite());

			if (m_setupGameWon && player->m_scoreTextTimer.isExpired() && player->m_addedScoreTextTween == false)
			{
				player->m_addedScoreTextTween = true;
				player->m_tweeningScoreTextX = 1920 + 400;
				CDBTweener::CTween* winScoreTween = new CDBTweener::CTween();
				winScoreTween->setEquation(&CDBTweener::TWEQ_ELASTIC, CDBTweener::TWEA_OUT, 1.f);
				winScoreTween->addValue(&player->m_tweeningScoreTextX, player->m_tweeningScoreTextXTarget);
				m_winGameTweener.addTween(winScoreTween);
			}

			sf::Vector2f def_pos;
			sf::Vector2f gat_pos;
			def_pos.x = player->getDefender()->m_tweenX;
			def_pos.y = player->getDefender()->m_tweenY;
			gat_pos.x = player->getGatherer()->m_tweenX;
			gat_pos.y = player->getGatherer()->m_tweenY;
			player->getDefender()->getSprite()->setPosition(def_pos);
			player->getGatherer()->getSprite()->setPosition(gat_pos);

			player->mWinScoreText->setPosition(player->m_tweeningScoreTextX, player->mWinScoreText->getPosition().y);
			player->m_winNumberSprite->setPosition(player->m_winNumberSprite->getPosition().x, player->mWinNumberSpriteY);
		}

		if (m_toMenuTimer.isExpired())
		{
			m_stateAsset->gameStateManager->m_players.clear();
			m_stateAsset->gameStateManager->changeState(new MenuState());
			return true;
		}

		if (!m_isShowingTimerText && m_toMenuTimer.getRemainingTime().asSeconds() <= SECONDS_WHEN_TEXT_SHOWS)
		{
			m_isShowingTimerText = true;
			CDBTweener::CTween* tween = new CDBTweener::CTween();
			tween->setEquation(&CDBTweener::TWEQ_ELASTIC, CDBTweener::TWEA_OUT, 1.f);
			tween->addValue(&m_toMenuTimerTextY, m_stateAsset->windowManager->getWindow()->getSize().y / 2.f);
			m_winGameTweener.addTween(tween);
		}
		mToMenuTimerText.setPosition(mToMenuTimerText.getPosition().x, m_toMenuTimerTextY);

		if (!m_toMenuTimer.isExpired())
		{
			mToMenuTimerText.setString("Entering menu in " + std::to_string(m_toMenuTimer.getRemainingTime().asSeconds()).substr(0, 1));
		}
		return true;
	}

	m_hotSpot->getAnimator()->update(sf::seconds(dt));
	m_hotSpot->getAnimator()->animate(*m_hotSpot->getSprite());

	for (auto &player : m_players)
	{
		if (player->hasWon())
		{
			m_gameWon = true;
		}

		if (player->getGatherer()->m_shieldStunned && player->getGatherer()->m_shieldStunnedTimer.getElapsedTime().asSeconds() >= SHIELD_STUNNED_TIMER)
		{
			player->getGatherer()->m_shieldStunned = false;
			player->getGatherer()->m_shieldStunnedTimer.reset();
		}

		if (player->getDefender()->m_shieldStunned && player->getDefender()->m_shieldStunnedTimer.getElapsedTime().asSeconds() >= SHIELD_STUNNED_TIMER)
		{
			player->getDefender()->m_shieldStunned = false;
			player->getDefender()->m_shieldStunnedTimer.reset();
		}
	}

#pragma region powerups_update
	if (!m_starting)
	{
		if (m_currentLevel->getPowerups().size() < MAX_POWERUP && m_currentLevel->getPowerupTimer()->getElapsedTime().asSeconds() >= POWERUPS_SPAWN_RATE)
		{
			m_currentLevel->getPowerupTimer()->restart();
			createPowerup();
		}

		{
			std::vector<Powerup*> powerups = m_currentLevel->getPowerups();
			auto powerupIt = powerups.begin();
			while (powerupIt != powerups.end())
			{
				(*powerupIt)->getAnimator()->update(sf::seconds(dt));
				(*powerupIt)->getAnimator()->animate(*(*powerupIt)->getSprite());
				if (!(*powerupIt)->isActive())
				{
					sf::Vector2f oldScale = (*powerupIt)->getSprite()->getScale();
					oldScale.x -= 0.006f;
					oldScale.y -= 0.006f;
					(*powerupIt)->getSprite()->scale(oldScale);
					if ((*powerupIt)->getSprite()->getGlobalBounds().width <= 0.5f)
					{
						delete *powerupIt;
						*powerupIt = nullptr;
						powerupIt = powerups.erase(powerupIt);
					}
					else
					{
						++powerupIt;
					}
				}
				else
				{
					++powerupIt;
				}
			}
			m_currentLevel->setNewPowerups(powerups);
		}
	}
#pragma endregion

#pragma region traps_update
	if (!m_starting)
	{
		std::vector<Trap*> &traps = m_currentLevel->getTraps();
		thor::StopWatch* trapTimer = m_currentLevel->getTrapTimer();
		if (traps.size() > 0 && trapTimer->getElapsedTime().asSeconds() >= TRAP_SECONDS_INTERVAL)
		{
			trapTimer->restart();
			unsigned int randomTrapIndex = thor::random(0U, traps.size() - 1);
			traps[randomTrapIndex]->getAnimator().playAnimation("crackle");
			traps[randomTrapIndex]->setActive(true);
		}

		for (std::size_t i = 0; i < traps.size(); i++)
		{
			if (traps[i]->isActive())
			{
				if (!traps[i]->getAnimator().isPlayingAnimation())
				{
					if (traps[i]->isExploding())
					{
						traps[i]->setActive(false);
						traps[i]->setExploding(false);
					}
					else
					{
						for (std::size_t k = 0; k < m_players.size(); k++)
						{
							if (m_players[k] == nullptr || m_players[k]->isDead() || m_players[k]->isDying()) continue;
							if (Math::pointInCircle(m_players[k]->getGatherer()->getSprite()->getPosition(), traps[i]->getExplosionPosition(), traps[i]->getExplosionRadius()))
							{
								m_players[k]->setDying(true);
							}
							else if (Math::pointInCircle(m_players[k]->getDefender()->getSprite()->getPosition(), traps[i]->getExplosionPosition(), traps[i]->getExplosionRadius()))
							{
								sf::Vector2f v1 = m_players[k]->getDefender()->getSprite()->getPosition();
								sf::Vector2f v2 = traps[i]->getExplosionPosition();
								float xDiff = v1.x - v2.x;
								float yDiff = v1.y - v2.y;
								float dist = std::sqrtf(xDiff * xDiff + yDiff * yDiff);
								sf::Vector2f unitDirection(xDiff / dist, yDiff / dist);
								float impulse = (Math::clamp(traps[i]->getExplosionRadius() - dist, traps[i]->getExplosionRadius(), traps[i]->getExplosionRadius() * 0.4f)) / traps[i]->getExplosionRadius() * 300.f;
								m_players[k]->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(unitDirection.x * impulse, unitDirection.y * impulse), m_players[k]->getDefender()->getBody()->GetWorldCenter(), true);
							}
						}
						traps[i]->setExploding(true);
					}
					traps[i]->getAnimator().playAnimation("explosion");
				}
			}
			traps[i]->getAnimator().update(sf::seconds(dt));
			traps[i]->getAnimator().animate(traps[i]->getSprite());
		}
	}
#pragma endregion

#pragma region coins_update
	if (!m_starting)
	{
		{
			std::vector<Coin*> coins = m_currentLevel->getCoins();
			int amount = 0;
			for (int i = 0; i < coins.size(); i++)
			{
				if (coins[i]->isState(CoinState::IDLE) || coins[i]->isState(CoinState::SENDED)) amount++;
			}
			if (amount < MAX_COINS)
			{
				if (!m_currentLevel->getCoinTimer()->isRunning())
				{
					m_currentLevel->getCoinTimer()->start();
				}
				else
				{
					if (m_currentLevel->getCoinTimer()->getElapsedTime().asSeconds() > COINS_TIMER_SECONDS)
					{
						m_currentLevel->getCoinTimer()->reset();

						// add coin
						std::vector<CoinObject> coinSpawns = m_currentLevel->getCoinSpawns();
						unsigned int randomCoinSpawn = thor::random(0U, coinSpawns.size() - 1);
						while (coinSpawns[randomCoinSpawn].occupied == true)
						{
							randomCoinSpawn = thor::random(0U, coinSpawns.size() - 1);
						}
						m_currentLevel->setCoinSpawnOccupied(randomCoinSpawn, true);


						// Get random spawn position for the songbird
						//x
						int margin = 300;
						int x, y = 0;
						int side = thor::random(0, 3); // top, left, right, bottom
						switch (side)
						{
						case 0:
							x = thor::random(-margin, 1920 + margin);
							y = -margin;
							break;
						case 1:
							x = -margin;
							y = thor::random(-margin, 1080 + margin);
							break;
						case 2:
							x = 1920 + margin;
							y = thor::random(-margin, 1080 + margin);
							break;
						case 3:
							x = thor::random(-margin, 1920 + margin);
							y = 1080 + margin;
							break;
						}

						Coinbird* coinbird = new Coinbird();
						coinbird->m_stateAsset = m_stateAsset;
						coinbird->m_sprite->setPosition(sf::Vector2f(x, y));
						coinbird->m_sprite->setTexture(m_stateAsset->resourceHolder->getTexture("coinbird.png"));
						coinbird->m_sprite->setOrigin(64, 64);
						coinbird->m_speed = COINBIRD_SPEED;
						coinbird->m_dropTarget = coinSpawns[randomCoinSpawn].position;
						coinbird->m_animation.addFrame(1.f, sf::IntRect(0, 0, 128, 128));
						coinbird->m_animation.addFrame(1.f, sf::IntRect(128, 0, 128, 128));
						coinbird->m_animation.addFrame(1.f, sf::IntRect(256, 0, 128, 128));
						coinbird->m_animation.addFrame(1.f, sf::IntRect(384, 0, 128, 128));
						coinbird->m_animator->addAnimation("flying", coinbird->m_animation, sf::seconds(COINBIRD_ANIM_SPEED));
						coinbird->m_animator->playAnimation("flying", true);
						coinbird->m_sprite->setRotation(Math::RAD2DEG(Math::angleBetween(coinbird->m_sprite->getPosition(), coinbird->m_dropTarget)));
						sf::Vector2f oldScale = coinbird->m_sprite->getScale();
						if (coinbird->m_sprite->getRotation() > 90 && coinbird->m_sprite->getRotation() <= 270)
						{
							coinbird->m_sprite->setScale(oldScale.x, oldScale.y * -1);
						}
						sf::Vector2f direction = Math::direction(coinbird->m_sprite->getPosition(), coinbird->m_dropTarget);
						sf::Vector2f velocity;
						velocity.x = direction.x * COINBIRD_SPEED;
						velocity.y = direction.y * COINBIRD_SPEED;
						coinbird->m_velocity = velocity;

						Coin* coin = new Coin();
						coin->setState(CoinState::SENDED);
						coin->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("coin.png"));
						coin->getSprite()->setPosition(coinbird->m_sprite->getPosition());
						coin->getSprite()->setOrigin(32, 32);
						coin->getAnimator()->playAnimation("idle", true);
						coin->m_coinSpawnIndex = randomCoinSpawn;

						coinbird->m_coin = coin;
						m_currentLevel->addCoinbird(coinbird);
						m_currentLevel->addCoin(coin);
					}
				}
			}
			coins = m_currentLevel->getCoins();

			auto it = coins.begin();
			while (it != coins.end())
			{
				bool kill = false;
				if ((*it)->isState(CoinState::GATHERED))
				{
					if (Math::euclideanDistance(
						(*it)->getSprite()->getPosition(),
						(*it)->getPlayerGathered()->getTotemSprite()->getPosition()
						) < COIN_GATHERED_SPEED)
					{
						(*it)->setState(CoinState::SLURPING);
					}
					else
					{
						sf::Vector2f direction = Math::direction(
							(*it)->getSprite()->getPosition(),
							(*it)->getPlayerGathered()->getTotemSprite()->getPosition());

						sf::Vector2f oldPosition = (*it)->getSprite()->getPosition();
						sf::Vector2f newPosition = oldPosition + sf::Vector2f(direction.x * COIN_GATHERED_SPEED, direction.y * COIN_GATHERED_SPEED);

						(*it)->getSprite()->setPosition(newPosition);
					}
				}
				else if ((*it)->isState(CoinState::SLURPING))
				{
					sf::Vector2f oldScale = (*it)->getSprite()->getScale();
					oldScale.x -= 0.006f;
					oldScale.y -= 0.006f;
					(*it)->getSprite()->scale(oldScale);

					if ((*it)->getSprite()->getGlobalBounds().width <= 0.5f)
					{
						kill = true;
						(*it)->getPlayerGathered()->addToBounty(1);
					}
				}
				(*it)->getAnimator()->update(sf::seconds(dt));
				(*it)->getAnimator()->animate(*(*it)->getSprite());

				if (kill)
				{
					delete *it;
					*it = nullptr;
					it = coins.erase(it);
				}
				else
				{
					++it;
				}
			}
			m_currentLevel->setNewCoins(coins);
		}
	}
#pragma endregion

#pragma region ManyMouseDefender
	// Fetch raw mouse events
	ManyMouseEvent event;
	while (ManyMouse_PollEvent(&event))
	{
		if (m_players[event.device] == nullptr || m_players[event.device]->isDead() || m_players[event.device]->isStunned() || m_players[event.device]->getDefender()->m_shieldStunned || m_starting) continue;
		Player* player = m_players[event.device];

		if (event.type == MANYMOUSE_EVENT_RELMOTION)
		{
			if (event.item == 0) // x
			{
				player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(5.f * PhysicsHelper::gameToPhysicsUnits(static_cast<float>(event.value)), 0.f), player->getDefender()->getBody()->GetWorldCenter(), true);
			}
			if (event.item == 1) // y
			{
				player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(0.f, 5.f * PhysicsHelper::gameToPhysicsUnits(static_cast<float>(event.value))), player->getDefender()->getBody()->GetWorldCenter(), true);
			}
		}
	}
	
	int indexJoystick = 0;
	for (auto &player : m_players)
	{
		sf::Vector2f moveSpeed(
			sf::Joystick::getAxisPosition(indexJoystick, sf::Joystick::Axis::X),
			sf::Joystick::getAxisPosition(indexJoystick, sf::Joystick::Axis::Y));

		if (std::fabs(moveSpeed.x) < 14) moveSpeed.x = 0.f;
		if (std::fabs(moveSpeed.y) < 14) moveSpeed.y = 0.f;
		
		player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(PhysicsHelper::gameToPhysicsUnits(moveSpeed.x), 0.f), player->getDefender()->getBody()->GetWorldCenter(), true);
		player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(0.f, PhysicsHelper::gameToPhysicsUnits(moveSpeed.y)), player->getDefender()->getBody()->GetWorldCenter(), true);

		indexJoystick++;
	}
#pragma endregion

#pragma region FloatingTextUpdate
	for (std::size_t i = 0; i < m_floatingScoreTexts.size(); i++)
	{
		sf::Color oldColorAlpha = m_floatingScoreTexts[i]->getText()->getColor();
		oldColorAlpha.a -= 5;
		m_floatingScoreTexts[i]->getText()->setColor(oldColorAlpha);
		if (oldColorAlpha.a <= 0)
		{
			m_floatingScoreTexts[i]->m_dead = true;
		}
		sf::Vector2f oldPosition = m_floatingScoreTexts[i]->getText()->getPosition();
		m_floatingScoreTexts[i]->getText()->setPosition(oldPosition + sf::Vector2f(0.f, -1.f));
	}

	auto it = m_floatingScoreTexts.begin();
	while (it != m_floatingScoreTexts.end())
	{
		if ((*it)->m_dead)
		{
			delete *it;
			*it = nullptr;
			it = m_floatingScoreTexts.erase(it);
		}
		else
		{
			++it;
		}
	}

#pragma endregion

#pragma region Pre_Player_Update
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		b2Vec2 velo(player->getGatherer()->getBody()->GetLinearVelocity().x *0.89f,
			player->getGatherer()->getBody()->GetLinearVelocity().y *0.89f);
		player->getGatherer()->getBody()->SetLinearVelocity(velo);

		if (player->getDefender()->getBody()->GetLinearVelocity().x < 0)
		{
			player->getDefender()->getSprite()->setScale(1.f, 1.f);
		}
		else
		{
			player->getDefender()->getSprite()->setScale(-1.f, 1.f);
		}

		if (player->getGatherer()->getBody()->GetLinearVelocity().x > 0)
		{
			player->getGatherer()->getSprite()->setScale(-1.f, 1.f);
		}
		else
		{
			player->getGatherer()->getSprite()->setScale(1.f, 1.f);
		}

		if (player->isChangingOrder())
		{
			player->getTotemSprite()->setPosition(player->getTotemSprite()->getPosition().x, player->m_tweeningValue);
		}

		if (player->isStunned() && player->m_stunnedTimer.getElapsedTime().asSeconds() >= POWERUP_STUN_TIME)
		{
			player->setStunned(false);
		}

		if (player->hasShield())
		{
			if (player->m_shieldTimer.getElapsedTime().asSeconds() >= SHIELD_TIMER)
			{
				player->setShield(false);
			}
			else
			{
				for (std::size_t i = 0; i < m_players.size(); i++)
				{
					if (m_players[i] == nullptr) continue;
					if (player != m_players[i])
					{
						// Gatherer knockback
						if (Math::euclideanDistance(
							player->getGatherer()->getSprite()->getPosition(),
							m_players[i]->getGatherer()->getSprite()->getPosition()) <= SHIELD_RADIUS)
						{
							m_players[i]->getGatherer()->m_shieldStunned = true;
							m_players[i]->getGatherer()->m_shieldStunnedTimer.restart();
							sf::Vector2f v1 = m_players[i]->getGatherer()->getSprite()->getPosition();
							sf::Vector2f v2 = player->getGatherer()->getSprite()->getPosition();
							sf::Vector2f direction = Math::direction(v2, v1);
							float length = Math::euclideanDistance(v1, v2);
							float force =
								(Math::clamp(SHIELD_RADIUS - length, SHIELD_RADIUS, SHIELD_FORCE_RADIUS_MIN)) /
								SHIELD_RADIUS * SHIELD_MAX_FORCE;
							m_players[i]->getGatherer()->getBody()->ApplyLinearImpulse(
								PhysicsHelper::gameToPhysicsUnits(sf::Vector2f(
								direction.x * force,
								direction.y * force)),
								m_players[i]->getGatherer()->getBody()->GetWorldCenter(), true);
						}

						// Defender knockback
						if (Math::euclideanDistance(
							player->getGatherer()->getSprite()->getPosition(),
							m_players[i]->getDefender()->getSprite()->getPosition()) <= SHIELD_RADIUS)
						{
							m_players[i]->getDefender()->m_shieldStunned = true;
							m_players[i]->getDefender()->m_shieldStunnedTimer.restart();
							sf::Vector2f v1 = m_players[i]->getDefender()->getSprite()->getPosition();
							sf::Vector2f v2 = player->getGatherer()->getSprite()->getPosition();
							sf::Vector2f direction = Math::direction(v2, v1);
							float length = Math::euclideanDistance(v1, v2);
							float force =
								(Math::clamp(SHIELD_RADIUS - length, SHIELD_RADIUS, SHIELD_FORCE_RADIUS_MIN)) /
								SHIELD_RADIUS * SHIELD_MAX_FORCE;
							m_players[i]->getDefender()->getBody()->ApplyLinearImpulse(
								PhysicsHelper::gameToPhysicsUnits(sf::Vector2f(
								direction.x * force,
								direction.y * force)),
								m_players[i]->getDefender()->getBody()->GetWorldCenter(), true);
						}
					}
				}
			}

			if (player->hasShield() && player->m_shieldTimer.getElapsedTime().asSeconds() >= SHIELD_TIMER)
			{
				player->setShield(false);
			}
		}
		if (player->getRespawnProtectionTimer().getElapsedTime().asSeconds() >= PLAYER_SPAWNPROTECTION_TIME)
		{
			player->setRespawnProtection(false);
		}
	}
#pragma endregion


	m_defenderParticleSystem->update(sf::seconds(dt));
	m_gathererDeathSystem->update(sf::seconds(dt));

	m_currentLevel->update(dt);
	m_world.Step(1.f / 60.f, 8, 3);


#pragma region GLOW_PARTICLES
	sf::Sprite temp[4];
	sf::Vector2f tempV = m_leaderPoint;
	int index = 0;
	for (auto& it : m_players)
	{
		temp[index] = *it->getPointsIndicator();
		index++;
	}
	for (int j = 0; j < m_players.size(); j++)
	{
		if (temp[j].getPosition().x >= m_leaderPoint.x)
		{
			m_leaderPoint = temp[j].getPosition();
		}
	}
	m_scoreIndicatorEmitter->setEmissionRate(4.f);
	m_scoreIndicatorEmitter->setParticleLifetime(thor::Distributions::uniform(sf::seconds(.1f), sf::seconds(.4f)));
	m_scoreIndicatorEmitter->setParticleVelocity(thor::Distributions::deflect(sf::Vector2f(0.f, -100.f), 180.f));
	m_scoreIndicatorEmitter->setParticlePosition(m_leaderPoint + sf::Vector2f(5.f, 0.f));

	m_scoreIndicatorSystem->addAffector(*m_scoreIndicatorAffector, sf::seconds(1.f));
	m_scoreIndicatorSystem->addEmitter(*m_scoreIndicatorEmitter, sf::milliseconds(1000));
	m_scoreIndicatorSystem->update(sf::seconds(dt + 0.025));



#pragma endregion
#pragma region Gatherer_Movement
	b2Vec2 up_impulse(0.f, -15.f);
	b2Vec2 down_impulse(0.f, 15.f);
	b2Vec2 left_impulse(-15.f, 0.f);
	b2Vec2 right_impulse(15.f, 0.f);

	int indexJoystickGat = 0;
	for (auto &player : m_players)
	{
		sf::Vector2f moveSpeed(
			sf::Joystick::getAxisPosition(indexJoystickGat, sf::Joystick::Axis::U),
			sf::Joystick::getAxisPosition(indexJoystickGat, sf::Joystick::Axis::R));

		if (std::fabs(moveSpeed.x) < 14) moveSpeed.x = 0.f;
		if (std::fabs(moveSpeed.y) < 14) moveSpeed.y = 0.f;

		player->getGatherer()->getBody()->ApplyLinearImpulse(b2Vec2(PhysicsHelper::gameToPhysicsUnits(moveSpeed.x), 0.f), player->getGatherer()->getBody()->GetWorldCenter(), true);
		player->getGatherer()->getBody()->ApplyLinearImpulse(b2Vec2(0.f, PhysicsHelper::gameToPhysicsUnits(moveSpeed.y)), player->getGatherer()->getBody()->GetWorldCenter(), true);

		indexJoystickGat++;
	}
	
	if (!m_starting && m_players[0] != nullptr && 
		!m_players[0]->getGatherer()->m_shieldStunned && 
		m_players[0]->getGatherer()->getBody()->IsActive() && 
		!m_players[0]->isStunned())
	{
		b2Vec2 body_point = m_players[0]->getGatherer()->getBody()->GetWorldCenter();
		if (m_actionMap->isActive("p1_up"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(up_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p1_down"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(down_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p1_left"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(left_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p1_right"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(right_impulse, body_point, true);
		}
	}

	if (!m_starting && m_players[1] != nullptr && !m_players[1]->getGatherer()->m_shieldStunned && m_players[1]->getGatherer()->getBody()->IsActive() && !m_players[1]->isStunned())
	{
		b2Vec2 body_point = m_players[1]->getGatherer()->getBody()->GetWorldCenter();
		if (m_actionMap->isActive("p2_up"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(up_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p2_down"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(down_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p2_left"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(left_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p2_right"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(right_impulse, body_point, true);
		}
	}

	if (!m_starting && m_players[2] != nullptr && !m_players[2]->getGatherer()->m_shieldStunned && m_players[2]->getGatherer()->getBody()->IsActive() && !m_players[2]->isStunned())
	{
		b2Vec2 body_point = m_players[2]->getGatherer()->getBody()->GetWorldCenter();
		if (m_actionMap->isActive("p3_up"))
		{
			m_players[2]->getGatherer()->getBody()->ApplyLinearImpulse(up_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p3_down"))
		{
			m_players[2]->getGatherer()->getBody()->ApplyLinearImpulse(down_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p3_left"))
		{
			m_players[2]->getGatherer()->getBody()->ApplyLinearImpulse(left_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p3_right"))
		{
			m_players[2]->getGatherer()->getBody()->ApplyLinearImpulse(right_impulse, body_point, true);
		}
	}


	if (!m_starting && m_players[3] != nullptr && !m_players[3]->getGatherer()->m_shieldStunned && m_players[3]->getGatherer()->getBody()->IsActive() && !m_players[3]->isStunned())
	{
		b2Vec2 body_point = m_players[1]->getGatherer()->getBody()->GetWorldCenter();
		if (m_actionMap->isActive("p4_up"))
		{
			m_players[3]->getGatherer()->getBody()->ApplyLinearImpulse(up_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p4_down"))
		{
			m_players[3]->getGatherer()->getBody()->ApplyLinearImpulse(down_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p4_left"))
		{
			m_players[3]->getGatherer()->getBody()->ApplyLinearImpulse(left_impulse, body_point, true);
		}
		if (m_actionMap->isActive("p4_right"))
		{
			m_players[3]->getGatherer()->getBody()->ApplyLinearImpulse(right_impulse, body_point, true);
		}
	}
#pragma endregion

#pragma region PLAYER_POST_UPDATE

	bool hasChangedTotemBlockedState = false;
	for (auto &player : m_players)
	{
		b2Vec2 MAX_VELOCITY_GATHERER(8.f, 8.f);
		if (player->getGatherer()->getBody()->GetLinearVelocity().x >= MAX_VELOCITY_GATHERER.x)
		{
			player->getGatherer()->getBody()->SetLinearVelocity(b2Vec2(MAX_VELOCITY_GATHERER.x, player->getGatherer()->getBody()->GetLinearVelocity().y));
		}
		if (player->getGatherer()->getBody()->GetLinearVelocity().y >= MAX_VELOCITY_GATHERER.y)
		{
			player->getGatherer()->getBody()->SetLinearVelocity(b2Vec2(player->getGatherer()->getBody()->GetLinearVelocity().x, MAX_VELOCITY_GATHERER.y));
		}
		if (player->getGatherer()->getBody()->GetLinearVelocity().x <= -MAX_VELOCITY_GATHERER.x)
		{
			player->getGatherer()->getBody()->SetLinearVelocity(b2Vec2(-MAX_VELOCITY_GATHERER.x, player->getGatherer()->getBody()->GetLinearVelocity().y));
		}
		if (player->getGatherer()->getBody()->GetLinearVelocity().y <= -MAX_VELOCITY_GATHERER.y)
		{
			player->getGatherer()->getBody()->SetLinearVelocity(b2Vec2(player->getGatherer()->getBody()->GetLinearVelocity().x, -MAX_VELOCITY_GATHERER.y));
		}

		player->processEventualDeath(m_currentLevel);
		player->getDeathTimer()->update();
		player->getDefender()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getDefender()->getBody()->GetPosition()) - sf::Vector2f(0, 64));
		player->getDefender()->m_stunBirds->setPosition(player->getDefender()->getSprite()->getPosition());
		player->getGatherer()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetPosition()) - sf::Vector2f(0, 15));
		player->getGatherer()->m_shieldOverlay->setPosition(player->getGatherer()->getSprite()->getPosition());


		if (player->m_holdingTotem)
		{
			if (player->isStunned())
			{
				if (player->getGatherer()->getAnimatior()->isPlayingAnimation() &&
					player->getGatherer()->getAnimatior()->getPlayingAnimation() != "stun_glow")
				{
					player->getGatherer()->getAnimatior()->playAnimation("stun_glow", true);
				}
			}
			else if (
				player->getGatherer()->getAnimatior()->isPlayingAnimation() &&
				player->getGatherer()->getAnimatior()->getPlayingAnimation() != "walk_glow")
			{
				player->getGatherer()->getAnimatior()->playAnimation("walk_glow", true);
			}
		}
		else
		{
			if (player->isStunned())
			{
				if (player->getGatherer()->getAnimatior()->isPlayingAnimation() &&
					player->getGatherer()->getAnimatior()->getPlayingAnimation() != "stun")
				{
					player->getGatherer()->getAnimatior()->playAnimation("stun", true);
				}
			}
			else if (
				player->getGatherer()->getAnimatior()->isPlayingAnimation() &&
				player->getGatherer()->getAnimatior()->getPlayingAnimation() != "walk")
			{
				player->getGatherer()->getAnimatior()->playAnimation("walk", true);
			}
		}

		if (!player->getDefender()->getAnimatior()->isPlayingAnimation())
		{
			player->getDefender()->getAnimatior()->playAnimation("walk", true);
		}

		player->getDefender()->getAnimatior()->update(sf::seconds(dt));
		player->getDefender()->m_stunBirdsAnimator->update(sf::seconds(dt));
		player->getGatherer()->getAnimatior()->update(sf::seconds(dt));
		player->getGatherer()->m_shieldOverlayAnimatior->update(sf::seconds(dt));

		player->getDefender()->getAnimatior()->animate(*player->getDefender()->getSprite());
		player->getDefender()->m_stunBirdsAnimator->animate(*player->getDefender()->m_stunBirds);
		player->getGatherer()->getAnimatior()->animate(*player->getGatherer()->getSprite());
		player->getGatherer()->m_shieldOverlayAnimatior->animate(*player->getGatherer()->m_shieldOverlay);


		if (player->isDying())
		{
			player->getDefender()->getDeathAnimator()->update(sf::seconds(dt));
			player->getGatherer()->getDeathAnimator()->update(sf::seconds(dt));
			player->getDefender()->getDeathAnimator()->animate(*player->getDefender()->getDeathSprite());
			player->getGatherer()->getDeathAnimator()->animate(*player->getGatherer()->getDeathSprite());

			if (!player->getGatherer()->getDeathAnimator()->isPlayingAnimation() &&
				!player->getDefender()->getDeathAnimator()->isPlayingAnimation())
			{
				player->setDying(false);
				player->setDead(true);
			}
		}
		/*
		*********************
		COIN PICKUP
		*********************/
		std::vector<Coin*> coins = m_currentLevel->getCoins();
		auto it = coins.begin();
		while (it != coins.end())
		{
			// If player stands on a coin, pick it up
			if ((*it)->isState(CoinState::IDLE) && player->getGatherer()->getSprite()->getGlobalBounds().intersects((*it)->getSprite()->getGlobalBounds()))
			{
				// Set the spawn area as not occupied
				m_currentLevel->setCoinSpawnOccupied((*it)->m_coinSpawnIndex, false);
				(*it)->setGathered(player);
				(*it)->setState(CoinState::GATHERED);
				m_stateAsset->audioSystem->playSound("Coin_Pickup");
				m_currentLevel->getCoinTimer()->restart();
			}
			else
			{
				++it;
			}
		}
		m_currentLevel->setNewCoins(coins);


		std::vector<Powerup*> powerups = m_currentLevel->getPowerups();
		auto powerupIt = powerups.begin();
		while (powerupIt != powerups.end())
		{
			if ((*powerupIt)->isActive() && player->getGatherer()->getSprite()->getGlobalBounds().intersects((*powerupIt)->getSprite()->getGlobalBounds()))
			{
				// Activate powerup!!!
				switch ((*powerupIt)->getType())
				{
				case LIGHTNING:
				{
								  for (std::size_t i = 0; i < m_players.size(); i++)
								  {
									  if (m_players[i] == nullptr) continue;
									  if (m_players[i] != player)
									  {
										  m_players[i]->setStunned(true);
									  }
								  }

								  m_lightningAlpha = 255.f;

								  CDBTweener::CTween* tween = new CDBTweener::CTween();
								  tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 1.f);
								  tween->addValue(&m_lightningAlpha, 0.f);
								  m_totemTweener.addTween(tween);

								  m_stateAsset->audioSystem->playSound("Lightning");

								  break;
				}
				case SHIELD:
					player->setShield(true);
					player->m_shieldTimer.restart();
					break;
				}
				m_currentLevel->setPowerupSpawnOccupied((*powerupIt)->m_coinSpawnIndex, false);
				(*powerupIt)->setActive(false);
				m_currentLevel->getPowerupTimer()->restart();

			}
			else
			{
				++powerupIt;
			}
		}
		m_currentLevel->setNewPowerups(powerups);

		float percent = player->getPoints() / POINTS_TO_WIN;
		float indicatorExtensionX = TIMER_POS_X + TIMER_WIDTH * percent;
		player->getPointsIndicator()->setPosition(indicatorExtensionX, player->getPointsIndicator()->getPosition().y);

		// Make sure the totem is transparent if totem is blocking player
		if (!hasChangedTotemBlockedState)
		{
			if (m_totemHead.getGlobalBounds().intersects(player->getDefender()->getSprite()->getGlobalBounds()) ||
				m_totemHead.getGlobalBounds().intersects(player->getGatherer()->getSprite()->getGlobalBounds()) ||
				m_totemFoot.getGlobalBounds().intersects(player->getDefender()->getSprite()->getGlobalBounds()) ||
				m_totemFoot.getGlobalBounds().intersects(player->getGatherer()->getSprite()->getGlobalBounds())
				)
			{
				hasChangedTotemBlockedState = true;
				m_totemIsBlockingPlayer = true;
			}
			else
			{
				for (std::size_t i = 0; i < m_players.size(); i++)
				{
					if (m_players[i] == nullptr) continue;
					if (m_players[i]->getTotemSprite()->getGlobalBounds().intersects(player->getDefender()->getSprite()->getGlobalBounds()) ||
						m_players[i]->getTotemSprite()->getGlobalBounds().intersects(player->getGatherer()->getSprite()->getGlobalBounds())
						)
					{
						hasChangedTotemBlockedState = true;
						m_totemIsBlockingPlayer = true;
						break;
					}
				}
			}
		}

		player->m_totemBountyIconAnimator->update(sf::seconds(dt));
		player->m_totemBountyIconAnimator->animate(*player->m_totemBountyIcon);
	}
	if (!hasChangedTotemBlockedState)
	{
		m_totemIsBlockingPlayer = false;
	}
#pragma endregion

	if (m_totemIsBlockingPlayer)
	{
		{
			sf::Color oldColor = m_totemHead.getColor();
			oldColor.a = TOTEM_ALPHA_WHEN_BLOCK;
			m_totemHead.setColor(oldColor);
		}
		for (auto &player : m_players)
		{
			if (player == nullptr) continue;
			sf::Color oldColor = player->getTotemSprite()->getColor();
			oldColor.a = TOTEM_ALPHA_WHEN_BLOCK;
			player->getTotemSprite()->setColor(oldColor);
		}
	}
	else
	{
		sf::Color oldColor = m_totemHead.getColor();
		oldColor.a = 255;
		m_totemHead.setColor(oldColor);

		for (auto &player : m_players)
		{
			if (player == nullptr) continue;
			sf::Color oldColor = player->getTotemSprite()->getColor();
			oldColor.a = 255;
			player->getTotemSprite()->setColor(oldColor);
		}
	}

	std::vector<Player*> activePlayers = m_hotSpot->getActivePlayers(m_players);
	if (activePlayers.size() == 1)
	{
		if (m_totemParticleTimer.isExpired())
		{
			m_totemParticleTimer.restart(sf::seconds(TOTEM_PARTICLE_RATE));
			addTotemParticle(activePlayers.back()->m_totemParticleTextureRect);
		}
		if (m_totemHeadAnimator.getPlayingAnimation() != "active")
			m_totemHeadAnimator.playAnimation("active", true);
		if (!activePlayers.back()->m_holdingTotem)
		{
			activePlayers.back()->m_holdingTotem = true;
			onEnterTotem(activePlayers.back());
		}
		activePlayers.back()->addPoints(POINTS_PER_SECOND * dt, activePlayers.back()->getGatherer()->getSprite()->getPosition(), SCORE_HOTSPOT);
		//update bar particles
		updateGlow = true;
		if (activePlayers.back()->getColor() == sf::Color(99, 152, 211, 94)) // "blue"
		{
			m_scoreIndicatorEmitter->setParticleTextureIndex(scoreTextureIndex0);
		}
		else if (activePlayers.back()->getColor() == sf::Color(244, 86, 86, 94))//"red
		{
			m_scoreIndicatorEmitter->setParticleTextureIndex(scoreTextureIndex1);
		}
		else if (activePlayers.back()->getColor() == sf::Color(248, 248, 83, 94))//"yellow"
		{
			m_scoreIndicatorEmitter->setParticleTextureIndex(scoreTextureIndex2);
		}
		else if (activePlayers.back()->getColor() == sf::Color(209, 105, 225, 94))//purple
		{
			m_scoreIndicatorEmitter->setParticleTextureIndex(scoreTextureIndex3);
		}
		if (activePlayers.back()->getBounty() > 0)
		{
			float score = activePlayers.back()->getBounty() * SCORE_PER_COIN;
			activePlayers.back()->addPoints(score, activePlayers.back()->getTotemSprite()->getPosition(), PlayerScoreTypes::SCORE_COIN);
			activePlayers.back()->resetBounty();
		}
		updateHoldingTotem(activePlayers.back());

		if (m_leadingPlayer == nullptr)
		{
			m_stateAsset->audioSystem->playSound("NewLeader");
			m_leadingPlayer = activePlayers.back();
		}
		else
		{
			Player* maxScorePlayer = nullptr;
			for (auto &player : m_players)
			{
				if (player != m_leadingPlayer)
				{
					if (player->getPoints() > m_leadingPlayer->getPoints())
					{
						maxScorePlayer = player;
					}
				}
			}
			if (maxScorePlayer != nullptr)
			{
				m_leadingPlayer = maxScorePlayer;
				m_stateAsset->audioSystem->playSound("NewLeader");
			}
		}
	}
	else
	{
		auto it_totemParticle = m_totemParticles.begin();
		while (it_totemParticle != m_totemParticles.end())
		{
			(*it_totemParticle)->m_sprite->setTextureRect(sf::IntRect(0, 0, 16, 16));
			++it_totemParticle;
		}

		if (m_totemHeadAnimator.getPlayingAnimation() != "idle")
			m_totemHeadAnimator.playAnimation("idle", true);

		updateHoldingTotem(nullptr); // Set all to false
	}


	sortTotem();
	m_totemTweener.step(dt);

	// Update totem text position accordingly to the totem sprite
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		player->m_totemBountyAmount->setPosition(player->getTotemSprite()->getPosition());
		player->m_totemBountyIcon->setPosition(player->getTotemSprite()->getPosition());
	}

	// Update lightning effect
	sf::Color oldColor = m_lightningEffect.getFillColor();
	oldColor.a = static_cast<int>(m_lightningAlpha);
	m_lightningEffect.setFillColor(oldColor);

	// Update totemhead animations
	m_totemHeadAnimator.update(sf::seconds(dt));
	m_totemHeadAnimator.animate(m_totemHead);

	m_321GO_timerExpired = m_321GOTimer.isExpired();

	if (!m_hasStarted321GOTimer && m_starting && m_321GO_timerExpired && !m_stateAsset->audioSystem->getSound("321GO")->isPlaying())
	{
		m_hasStarted321GOTimer = true;
		m_stateAsset->audioSystem->playSound("321GO");
		m_123GOAnimator.playAnimation("idle");
	}

	// Update 321GO animation
	if (m_starting && m_321GO_timerExpired)
	{
		if (!m_123GOAnimator.isPlayingAnimation())
		{
			m_starting = false;
		}
		else
		{
			m_123GOAnimator.update(sf::seconds(dt));
			m_123GOAnimator.animate(m_123GO);
		}
	}
	//m_currentLevel->update(dt);
	return true;
}


void PlayState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(*m_currentLevel);
	m_stateAsset->windowManager->getWindow()->draw(*m_hotSpot->getShape());
	m_stateAsset->windowManager->getWindow()->draw(*m_hotSpot->getSprite());
	if (!m_gameWon)
	{
		for (auto &player : m_players)
		{
			if (player->m_online)
				m_stateAsset->windowManager->getWindow()->draw(*player);
		}

		for (auto &deathCloud : m_deathClouds)
		{
			m_stateAsset->windowManager->getWindow()->draw(deathCloud->sprite);
		}
	}

	m_stateAsset->windowManager->getWindow()->draw(m_totemFoot);

	for (auto &totemParticle : m_totemParticles)
	{
		m_stateAsset->windowManager->getWindow()->draw(*totemParticle->m_sprite);
	}

	m_stateAsset->windowManager->getWindow()->draw(m_totemHead);
	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->getTotemSprite());
	}

	if (!m_gameWon)
		m_currentLevel->drawFlyingCoins(m_stateAsset->windowManager->getWindow());

	Box2DWorldDraw debugDraw(m_stateAsset->windowManager->getWindow());
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	m_world.SetDebugDraw(&debugDraw);
	//2m_world.DrawDebugData();

	m_stateAsset->windowManager->getWindow()->draw(m_timerBarBackground);
	m_stateAsset->windowManager->getWindow()->draw(m_timerBar);
	m_stateAsset->windowManager->getWindow()->draw(m_frame);
	m_stateAsset->windowManager->getWindow()->draw(*m_defenderParticleSystem);
	m_stateAsset->windowManager->getWindow()->draw(*m_gathererDeathSystem);
	if (updateGlow)
	{
		m_stateAsset->windowManager->getWindow()->draw(*m_scoreIndicatorSystem);
	}
	
	for (auto &player : m_players)
	{
		if (player->m_online)
			m_stateAsset->windowManager->getWindow()->draw(*player->getPointsIndicator());


		if (player->m_bounty > 0 && player->m_online)
		{
			m_stateAsset->windowManager->getWindow()->draw(*player->m_totemBountyAmount);
			m_stateAsset->windowManager->getWindow()->draw(*player->m_totemBountyIcon);
		}
	}

	if (!m_gameWon)
	{
		for (auto &FST : m_floatingScoreTexts)
		{
			m_stateAsset->windowManager->getWindow()->draw(*FST->getText());
		}
		m_stateAsset->windowManager->getWindow()->draw(m_lightningEffect);
	}
	if (m_starting && m_321GO_timerExpired)
	{
		m_stateAsset->windowManager->getWindow()->draw(m_123GO);
	}

	if (m_gameWon)
	{
		m_stateAsset->windowManager->getWindow()->draw(mWinBackground);
		for (auto &player : m_players)
		{
			if (player->m_online)
			{
				m_stateAsset->windowManager->getWindow()->draw(*player);
				m_stateAsset->windowManager->getWindow()->draw(*player->m_winNumberSprite);
			}
		}

		m_stateAsset->windowManager->getWindow()->draw(mToMenuTimerText);
	}
}

void PlayState::initManyMouse()
{
	std::vector<std::string> acceptableMouseIDS;
	acceptableMouseIDS.push_back("dcd89f3"); // Blue
	acceptableMouseIDS.push_back("36aea35"); // Red
	acceptableMouseIDS.push_back("3340eb3"); // Yellow
	acceptableMouseIDS.push_back("20873a9"); // Purple

	int numDevices = ManyMouse_Init();
	int index = 0;
	for (int i = 0; i < numDevices; i++)
	{
		std::string name = ManyMouse_DeviceName(i);
		std::string buffer = ManyMouse_ID(i);
		std::string id = buffer.substr(28, 7);
		std::transform(id.begin(), id.end(), id.begin(), ::tolower);

		auto pos = std::find(acceptableMouseIDS.begin(), acceptableMouseIDS.end(), id);
		if (pos != acceptableMouseIDS.end())
		{
			m_mouseIndicies[index] = id;
			index++;
		}
	}
}

void PlayState::initPlayers()
{
	m_players.clear();

	std::vector<std::string> playerTotemImages;
	playerTotemImages.push_back("blueblock.png");
	playerTotemImages.push_back("redblock.png");
	playerTotemImages.push_back("yellowblock.png");
	playerTotemImages.push_back("purpleblock.png");

	std::vector<std::string> playerBarImages;
	playerBarImages.push_back("BarBlue.png");
	playerBarImages.push_back("BarRed.png");
	playerBarImages.push_back("BarYellow.png");
	playerBarImages.push_back("BarPurple.png");

	std::vector<std::string> playerPointIndicatorImages;
	playerPointIndicatorImages.push_back("arrowblue.png");
	playerPointIndicatorImages.push_back("arrowred.png");
	playerPointIndicatorImages.push_back("arrowyellow.png");
	playerPointIndicatorImages.push_back("arrowpurple.png");

	std::vector<sf::Color> playerColors;
	playerColors.push_back(sf::Color(99, 152, 211, 94));
	playerColors.push_back(sf::Color(244, 86, 86, 94));
	playerColors.push_back(sf::Color(248, 248, 83, 94));
	playerColors.push_back(sf::Color(209, 105, 225, 94));

	std::string glitterAnims[4];
	glitterAnims[0] = "blue";
	glitterAnims[1] = "red";
	glitterAnims[2] = "yellow";
	glitterAnims[3] = "purple";

	sf::IntRect deathCloudTextureRects[4];
	deathCloudTextureRects[0] = sf::IntRect(0, 0, 56, 56);
	deathCloudTextureRects[1] = sf::IntRect(112, 0, 56, 56);
	deathCloudTextureRects[2] = sf::IntRect(168, 0, 56, 56);
	deathCloudTextureRects[3] = sf::IntRect(56, 0, 56, 56);

	sf::IntRect totemParticleTextureRects[4];
	totemParticleTextureRects[0] = sf::IntRect(32, 0, 16, 16);
	totemParticleTextureRects[1] = sf::IntRect(64, 0, 16, 16);
	totemParticleTextureRects[2] = sf::IntRect(16, 0, 16, 16);
	totemParticleTextureRects[3] = sf::IntRect(48, 0, 16, 16);

	std::vector<std::string> acceptableMouseIDS;
	acceptableMouseIDS.push_back("dcd89f3"); // Blue
	acceptableMouseIDS.push_back("36aea35"); // Red
	acceptableMouseIDS.push_back("3340eb3"); // Yellow
	acceptableMouseIDS.push_back("20873a9"); // Purple
	

	// Här skapar jag spelarna, i manuell ordning
	for (std::size_t i = 0; i < 4; i++)
	{
		m_players.push_back(new Player());
		m_players.back()->game = this;
		m_players.back()->m_mouseID = acceptableMouseIDS[i]; // här ger jag varje player in rätta mouseID
		m_players.back()->m_deathCloudTextureRect = deathCloudTextureRects[i];
		m_players.back()->m_totemParticleTextureRect = totemParticleTextureRects[i];
		m_players.back()->setResourceHolder(m_stateAsset->resourceHolder);
		m_players.back()->setFSTRef(m_floatingScoreTexts);
		m_players.back()->setColor(playerColors[i]);
		m_players.back()->getTotemSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(playerTotemImages[i]));
		m_players.back()->setOrder(i);
		m_players.back()->setPointsBarImage(playerBarImages[i]);
		m_players.back()->mHotSpotGlitterAnimation = glitterAnims[i];

		sf::Sprite* sprite = new sf::Sprite();
		sprite->setTexture(m_stateAsset->resourceHolder->getTexture(playerPointIndicatorImages[i]));
		sprite->setOrigin(sprite->getGlobalBounds().width / 2.f, sprite->getGlobalBounds().height);
		sprite->setPosition(TIMER_POS_X, TIMER_POS_Y);
		m_players.back()->setPointsIndicator(sprite);

		m_players.back()->m_totemBountyAmount->setFont(m_stateAsset->resourceHolder->getFont("lithospro.otf"));
		m_players.back()->m_totemBountyAmount->setCharacterSize(20);
		m_players.back()->m_totemBountyAmount->setColor(sf::Color::Black);
		m_players.back()->m_totemBountyAmount->setString("x0");

		m_players.back()->m_totemBountyAmount->setOrigin(m_players.back()->m_totemBountyAmount->getGlobalBounds().width / 2.f, m_players.back()->m_totemBountyAmount->getGlobalBounds().height / 2.f);

		m_players.back()->m_totemBountyIcon->setTexture(m_stateAsset->resourceHolder->getTexture("coin.png"));

		m_players.back()->m_totemBountyAnimation = new thor::FrameAnimation();
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(64, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(128, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(192, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(256, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(320, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(384, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(448, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(512, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(576, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(640, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(704, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(768, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(832, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(896, 0, 64, 64));
		m_players.back()->m_totemBountyAnimation->addFrame(1.f, sf::IntRect(960, 0, 64, 64));
		m_players.back()->m_totemBountyIconAnimator->addAnimation("idle", *m_players.back()->m_totemBountyAnimation, sf::seconds(1.f));

		m_players.back()->m_totemBountyIcon->setOrigin(32, 64);
		m_players.back()->m_totemBountyIcon->setScale(0.3, 0.3);
		m_players.back()->m_totemBountyIconAnimator->playAnimation("idle", true);

		m_players.back()->mWinScoreText->setFont(m_stateAsset->resourceHolder->getFont(DEFAULT_FONT));
		m_players.back()->mWinScoreText->setString("");
		m_players.back()->mWinScoreText->setCharacterSize(60);

		m_players.back()->m_winNumberSprite->setTexture(m_stateAsset->resourceHolder->getTexture("numbers.png"));
		m_players.back()->m_winNumberSprite->setOrigin(126, 186);
		m_players.back()->m_winNumberSprite->setScale(sf::Vector2f(0.4, 0.4));

		if (!m_stateAsset->gameStateManager->m_players[i].m_ready)
		{
			m_players.back()->m_online = false;
			m_players.back()->setPoints(-1);
		}
	}
	/*
	for (auto &string : m_mouseIndicies)
	{
		std::cout << string << std::endl;
	}

	std::cout << "Before" << std::endl;
	for (auto &player : m_players)
	{
		if (player->m_online)
		{
			std::cout << player->m_mouseID << std::endl;
		}
		else
		{
			std::cout << "Not online " << player->m_mouseID << std::endl;
		}
	}

	// Nu vill jag sortera m_players emot m_mouseIndicies
	std::vector<Player*> newPlayerVector(4, nullptr);
	std::vector<int> usedIndices;
	for (int i = 0; i < 4; i++)
	{
		int index = -1;
		for (int j = 0; j < 4; j++)
		{
			if (m_players[i]->m_mouseID == m_mouseIndicies[j])
			{
				index = j;
			}
		}
		if (index != -1)
		{
			usedIndices.push_back(index);
			newPlayerVector[index] = m_players[i];
		}
		else
		{
			// Find an index that has not been used
			bool found = false;
			int randomIndex = thor::random(0, 3);

			while (!found)
			{
				if (std::find(usedIndices.begin(), usedIndices.end(), randomIndex) != usedIndices.end()) 
				{
					randomIndex = thor::random(0, 3);
				}
				else 
				{
					found = true;
				}
			}
			newPlayerVector[randomIndex] = m_players[i];
			usedIndices.push_back(randomIndex);
		}
	}
	m_players.clear();
	m_players = newPlayerVector;

	std::cout << "After:" << std::endl;
	for (auto &player : m_players)
	{
		if (player->m_online)
		{
			std::cout << player->m_mouseID << std::endl;
		}
		else
		{
			std::cout << "Not online " << player->m_mouseID << std::endl;
		}
	}
	*/
}

void PlayState::setupActions()
{
	m_actionMap->operator[]("p3_up") = thor::Action(sf::Keyboard::Y, thor::Action::Hold);
	m_actionMap->operator[]("p3_down") = thor::Action(sf::Keyboard::H, thor::Action::Hold);
	m_actionMap->operator[]("p3_left") = thor::Action(sf::Keyboard::G, thor::Action::Hold);
	m_actionMap->operator[]("p3_right") = thor::Action(sf::Keyboard::J, thor::Action::Hold);

	m_actionMap->operator[]("p4_up") = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
	m_actionMap->operator[]("p4_down") = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
	m_actionMap->operator[]("p4_left") = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
	m_actionMap->operator[]("p4_right") = thor::Action(sf::Keyboard::Right, thor::Action::Hold);

	m_actionMap->operator[]("p1_up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("p1_down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("p1_left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("p1_right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);

	m_actionMap->operator[]("p2_up") = thor::Action(sf::Keyboard::Numpad8, thor::Action::Hold);
	m_actionMap->operator[]("p2_down") = thor::Action(sf::Keyboard::Numpad5, thor::Action::Hold);
	m_actionMap->operator[]("p2_left") = thor::Action(sf::Keyboard::Numpad4, thor::Action::Hold);
	m_actionMap->operator[]("p2_right") = thor::Action(sf::Keyboard::Numpad6, thor::Action::Hold);
}

void PlayState::loadNewLevel()
{
	std::vector<std::string> levels = FileSystem::getFilesFromDirectory("../levels/", "level");
	int randomLevelIndex = thor::random(0, static_cast<int>((levels.size() - 1)));

	if (m_currentLevel == nullptr)
	{
		delete m_currentLevel;
		m_currentLevel = nullptr;
	}

	m_currentLevel = m_levelLoader->parseLevel(levels[randomLevelIndex], m_world);
	m_currentLevel->game = this;
	m_currentLevel->getBackground()->setTexture(m_stateAsset->resourceHolder->getTexture(m_currentLevel->getBackgroundPath(), false));

	m_hotSpot->setRadius(200);
	m_hotSpot->setPosition(sf::Vector2f(m_stateAsset->windowManager->getWindow()->getSize().x / 2.f, m_stateAsset->windowManager->getWindow()->getSize().y / 2.f));

	float start_y_position = m_hotSpot->getPosition().y - m_players[0]->getTotemSprite()->getGlobalBounds().height / 2.f;
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		player->m_addedScoreTextTween = false;
		player->getTotemSprite()->setOrigin(player->getTotemSprite()->getGlobalBounds().width / 2.f, player->getTotemSprite()->getGlobalBounds().height / 2.f);
		player->getTotemSprite()->setPosition(m_hotSpot->getPosition().x, start_y_position);
		player->m_totemBountyAmount->setPosition(player->getTotemSprite()->getPosition());
		start_y_position -= player->getTotemSprite()->getGlobalBounds().height - 2;
	}
	m_players.back()->getTotemSprite()->setPosition(m_players.back()->getTotemSprite()->getPosition().x, m_players.back()->getTotemSprite()->getPosition().y - 8);

	// Create defenders and gatherers
	std::vector<std::string> defender_textures;
	defender_textures.push_back("def_blue.png");
	defender_textures.push_back("def_red.png");
	defender_textures.push_back("def_yellow.png");
	defender_textures.push_back("def_purple.png");

	std::vector<std::string> gatherer_textures;
	gatherer_textures.push_back("g_blue.png");
	gatherer_textures.push_back("g_red.png");
	gatherer_textures.push_back("g_yellow.png");
	gatherer_textures.push_back("g_purple.png");

	std::string playerGathererDeathTextures[4];
	playerGathererDeathTextures[0] = "death_blue.png";
	playerGathererDeathTextures[1] = "death_red.png";
	playerGathererDeathTextures[2] = "death_yellow.png";
	playerGathererDeathTextures[3] = "death_purple.png";

	std::string playerDefenderDeathTextures[4];
	playerDefenderDeathTextures[0] = "blue_d_death.png";
	playerDefenderDeathTextures[1] = "red_d_death.png";
	playerDefenderDeathTextures[2] = "yellow_d_death.png";
	playerDefenderDeathTextures[3] = "purple_d_death.png";

	for (std::size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i] == nullptr) continue;
		m_players[i]->clear(m_world);
		Defender* defender = new Defender();
		Gatherer* gatherer = new Gatherer();

		defender->getDeathSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(playerDefenderDeathTextures[i]));
		gatherer->getDeathSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(playerGathererDeathTextures[i]));

		m_stateAsset->resourceHolder->getTexture(defender_textures[i]).setSmooth(true);
		m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]).setSmooth(true);

		defender->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(defender_textures[i]));
		gatherer->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]));

		defender->setSpawnPosition(m_currentLevel->getDefenderSpawn(i));
		gatherer->setSpawnPosition(m_currentLevel->getGathererSpawn(i));

		m_players[i]->setDefender(defender);
		m_players[i]->setGatherer(gatherer);
		defender->m_stunBirds->setTexture(m_stateAsset->resourceHolder->getTexture("bird_spin.png"));
		gatherer->m_shieldOverlay->setTexture(m_stateAsset->resourceHolder->getTexture("shield_animation.png"));
	}

	createPlayerBodies();
	m_gameWon = false;
}

void PlayState::createPlayerBodies()
{
	for (auto &player : m_players)
	{
		{ // Defender
			b2BodyDef bodyDef;
			b2Vec2 physicsPosition = PhysicsHelper::gameToPhysicsUnits(player->getDefender()->getSprite()->getPosition());
			bodyDef.position.Set(physicsPosition.x, physicsPosition.y);
			bodyDef.type = b2_dynamicBody;
			bodyDef.angle = 0;
			bodyDef.linearDamping = 0.6f;
			bodyDef.angularDamping = 1.f;
			b2Body* body = m_world.CreateBody(&bodyDef);

			b2CircleShape shape;
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(32);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &shape;
			fixtureDef.restitution = 0.6f;
			body->CreateFixture(&fixtureDef);
			player->getDefender()->setBody(body);
			if (!player->m_online) body->SetActive(false);
		}

		{ // Gatherer
		b2BodyDef bodyDef;
		b2Vec2 physicsPosition = PhysicsHelper::gameToPhysicsUnits(player->getGatherer()->getSprite()->getPosition());
		bodyDef.position.Set(physicsPosition.x, physicsPosition.y);
		bodyDef.type = b2_dynamicBody;
		bodyDef.angle = 0;
		bodyDef.linearDamping = 0.3f;
		bodyDef.angularDamping = 1.f;
		b2Body* body = m_world.CreateBody(&bodyDef);

		b2CircleShape shape;
		shape.m_radius = PhysicsHelper::gameToPhysicsUnits(24);

		b2FixtureDef fixtureDef;
		fixtureDef.density = 1;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape = &shape;
		fixtureDef.restitution = 0.6f;
		body->CreateFixture(&fixtureDef);
		player->getGatherer()->setBody(body);
		if (!player->m_online) body->SetActive(false);
	}
	}
}

bool sortTotemAlgorithm(Player* a, Player* b)
{
	return a->getPoints() < b->getPoints();
}
bool sortWinningPlayers(Player* a, Player* b)
{
	return a->getPoints() > b->getPoints();
}

void PlayState::sortTotem()
{
	std::vector<Player*> sortedPlayerVector = m_players;
	std::sort(sortedPlayerVector.begin(), sortedPlayerVector.end(), sortTotemAlgorithm);

	m_timerBar.setTexture(m_stateAsset->resourceHolder->getTexture(sortedPlayerVector.back()->getPointsBarImage()));

	float percent = sortedPlayerVector.back()->getPoints() / POINTS_TO_WIN;
	sf::IntRect textureRect;
	textureRect.left = 0;
	textureRect.top = 0;
	textureRect.height = TIMER_POS_Y;
	textureRect.width = TIMER_WIDTH * percent;
	m_timerBar.setTextureRect(textureRect);

	// Find out difference of all totem heads
	float start_y_position = m_hotSpot->getPosition().y - m_players[0]->getTotemSprite()->getGlobalBounds().height / 2.f;
	for (int i = 0; i < sortedPlayerVector.size(); i++)
	{
		if (sortedPlayerVector[i] == nullptr) continue;
		float newPositionY = start_y_position;
		if (i == sortedPlayerVector.size() - 1) newPositionY -= 8;

		float oldPositionY = sortedPlayerVector[i]->getTotemSprite()->getPosition().y;

		if (static_cast<int>(newPositionY) != static_cast<int>(oldPositionY))
		{
			sortedPlayerVector[i]->m_tweeningValue = oldPositionY;

			if (!sortedPlayerVector[i]->m_isTweeningTotem || sortedPlayerVector[i]->m_currentTotemTween->getValues()[0]->m_fTarget != newPositionY)
			{
				if (sortedPlayerVector[i]->m_currentTotemTween != nullptr)
				{
					m_totemTweener.removeTween(sortedPlayerVector[i]->m_currentTotemTween);
				}
				CDBTweener::CTween* tween = new CDBTweener::CTween();
				tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 0.3f);
				tween->addValue(&sortedPlayerVector[i]->m_tweeningValue, newPositionY);
				tween->setUserData(sortedPlayerVector[i]);
				tween->addListener(m_totemTweenerListener);
				sortedPlayerVector[i]->m_isTweeningTotem = true;
				sortedPlayerVector[i]->m_currentTotemTween = tween;
				m_totemTweener.addTween(tween);
			}
			sortedPlayerVector[i]->setChangingOrder(true);
		}

		start_y_position -= sortedPlayerVector[i]->getTotemSprite()->getGlobalBounds().height - 2;
	}
}

void PlayState::createPowerup()
{
	// Get random enum value
	int randomIndex = thor::random(0, POWERUP_COUNT - 1);

	std::vector<PowerupObject> powerupSpawns = m_currentLevel->getPowerupsSpawns();
	unsigned int randomSpawn = thor::random(0U, powerupSpawns.size() - 1);
	while (powerupSpawns[randomSpawn].occupied == true)
	{
		randomSpawn = thor::random(0U, powerupSpawns.size() - 1);
	}
	m_currentLevel->setPowerupSpawnOccupied(randomSpawn, true);

	Powerup* powerup = new Powerup();

	switch (randomIndex)
	{
	case PowerupEnum::SHIELD:
		powerup->setType(PowerupEnum::SHIELD);
		powerup->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("shield.png"));
		powerup->getSprite()->setOrigin(34.f, 35.f);
		powerup->getAnimation()->addFrame(1.f, sf::IntRect(0.f, 0.f, 68.f, 70.f));
		break;
	case PowerupEnum::LIGHTNING:
		powerup->setType(PowerupEnum::LIGHTNING);
		powerup->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("lightning.png"));
		powerup->getSprite()->setOrigin(32.f, 32.f);
		powerup->getAnimation()->addFrame(1.f, sf::IntRect(0.f, 0.f, 68, 68.f));
		break;
	}

	powerup->m_coinSpawnIndex = randomSpawn;
	powerup->getSprite()->setPosition(powerupSpawns[randomSpawn].position);
	powerup->getAnimator()->addAnimation("Idle", *powerup->getAnimation(), sf::seconds(1.f));
	powerup->getAnimator()->playAnimation("Idle", true);
	m_currentLevel->addPowerup(powerup);
}

void PlayState::setupGameWon()
{
	m_toMenuTimer.restart(sf::seconds(SECONDS_IN_WINNING_SCREEN));
	m_setupGameWon = true;
	float middle_y = static_cast<float>(m_stateAsset->windowManager->getWindow()->getSize().y / 2);
	float middle_x = static_cast<float>(m_stateAsset->windowManager->getWindow()->getSize().x / 2);
	std::vector<Player*> players = m_players;
	for (auto &player : players)
	{
		if (!player->m_online)
		{
			player->setPoints(-1);
		}
	}
	std::sort(players.begin(), players.end(), sortWinningPlayers);

	float startY = middle_y - 390.f;
	float startXDef = middle_x;
	float startXGat = middle_x;

	float startSeconds = 0.5;
	std::vector<float> dist_secs = { 0, 0, 0, 0 };
	int a = 0;
	for (auto &p : players)
	{
		dist_secs[a] = startSeconds;
		a++;
		startSeconds += 0.8f;
	}
	a--;

	int width = 252;
	for (auto &p : players)
	{
		if (p->isStunned())
		{
			p->getGatherer()->getAnimatior()->playAnimation("stun", true);
		}
		else
		{
			p->getGatherer()->getAnimatior()->playAnimation("walk", true);
		}
		p->setDead(false);
		p->setShield(false);
		p->processEventualDeath(m_currentLevel);
		p->getDefender()->getAnimatior()->playAnimation("walk", true);
		sf::Vector2f def_pos = p->getDefender()->getSprite()->getPosition();
		sf::Vector2f gat_pos = p->getGatherer()->getSprite()->getPosition();
		p->getDefender()->m_tweenX = def_pos.x;
		p->getDefender()->m_tweenY = def_pos.y;
		p->getGatherer()->m_tweenX = gat_pos.x;
		p->getGatherer()->m_tweenY = gat_pos.y;

		/*CDBTweener::CTween* tween = new CDBTweener::CTween();
		tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 1.f);*/

		p->mWinNumberSpriteY = 1080 + 300;
		p->m_winNumberSprite->setTextureRect(sf::IntRect(width, 0, 252, 372));
		p->m_winNumberSprite->setPosition(startXDef - 140, p->mWinNumberSpriteY);

		CDBTweener::CTween* tweenNumberSprite = new CDBTweener::CTween();
		tweenNumberSprite->setEquation(&CDBTweener::TWEQ_ELASTIC, CDBTweener::TWEA_OUT, 1.f);
		tweenNumberSprite->addValue(&p->mWinNumberSpriteY, startY + 40);
		m_winGameTweener.addTween(tweenNumberSprite);

		CDBTweener::CTween* tween = new CDBTweener::CTween();
		tween->setEquation(&CDBTweener::TWEQ_ELASTIC, CDBTweener::TWEA_OUT, 2.f);

		tween->addValue(&p->getDefender()->m_tweenX, startXDef);
		tween->addValue(&p->getDefender()->m_tweenY, startY);
		tween->addValue(&p->getGatherer()->m_tweenX, startXGat);
		tween->addValue(&p->getGatherer()->m_tweenY, startY + 80);
		m_winGameTweener.addTween(tween);

		p->mWinScoreText->setString(std::to_string(static_cast<int>(p->getPoints())));
		p->mWinScoreText->setOrigin(p->mWinScoreText->getGlobalBounds().width / 2.f, p->mWinScoreText->getGlobalBounds().height / 2.f);
		//p->mWinScoreText->setPosition(1920 + 400, startY);

		/*	CDBTweener::CTween* tween = new CDBTweener::CTween();
			tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 1.f);
			tween->addValue(&p->getDefender()->m_tweenX, startXDef);
			tween->addValue(&p->getDefender()->m_tweenY, startY);
			tween->addValue(&p->getGatherer()->m_tweenX, startXGat);
			tween->addValue(&p->getGatherer()->m_tweenY, startY + 80);
			tween->setUserData(p);
			m_winGameTweener.addTween(tween);
			*/
		//startY += 128;
		p->mWinScoreText->setPosition(1920 + 400, startY + 40);

		p->m_tweeningScoreTextX = 1920 + 400;
		p->m_tweeningScoreTextXTarget = startXGat + 170;
		p->m_scoreTextTimer.restart(sf::seconds(dist_secs[a]));

		startY += 250;
		width += 252;
		a--;
	}
}

b2Body* PlayState::createWall(sf::Vector2f v1, sf::Vector2f v2)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0;

	// Center of line segment
	sf::Vector2f lineCenter((v1.x + v2.x) / 2.f, (v1.y + v2.y) / 2.f);

	// Length of segment
	float length = Math::euclideanDistance(v1, v2);

	// Convert the position
	b2Vec2 position = PhysicsHelper::gameToPhysicsUnits(lineCenter);
	bodyDef.position.Set(position.x, position.y);
	bodyDef.userData = this;
	b2Body* body = m_world.CreateBody(&bodyDef);

	// Create shape and fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.restitution = 0;
	fixtureDef.friction = 1;

	b2EdgeShape edgeShape;
	float physicsLength = PhysicsHelper::gameToPhysicsUnits(length);
	b2Vec2 v1S(-physicsLength / 2.f, 0);
	b2Vec2 v2S(physicsLength / 2.f, 0);
	edgeShape.Set(v1S, v2S);

	fixtureDef.shape = &edgeShape;
	body->CreateFixture(&fixtureDef);
	body->SetTransform(position, Math::angleBetween(v1, v2));

	return body;
}

void PlayState::onEnterTotem(Player* player)
{
	m_hotSpot->getAnimator()->playAnimation(player->mHotSpotGlitterAnimation, true);
	m_hotSpot->getShape()->setFillColor(player->getColor());
	//m_timerEmitter->setEmissionRate(40);
	//m_timerEmitter->setParticlePosition(player->getPointsIndicator()->getPosition());
	//m_timerEmitter->setParticleVelocity(thor::Distribution::deflect());
	//m_timerEmitterConnection = m_timerParticleSystem->addEmitter()
}

void PlayState::updateHoldingTotem(Player* player)
{
	if (player == nullptr)
	{
		if (m_hotSpot->getAnimator()->getPlayingAnimation() != "idle")
		{
			m_hotSpot->getAnimator()->playAnimation("idle", true);
		}
		m_hotSpot->getShape()->setFillColor(m_hotSpot->mIdleColor);
	}
	for (auto &p : m_players)
	{
		if (p != player)
		{
			p->m_holdingTotem = false;
		}
	}
}


void PlayState::addDeathcloud(sf::Vector2f position, sf::IntRect textureRect)
{
	DeathCloud* dc = new DeathCloud();
	dc->alphaTween = 0;
	dc->sprite.setTexture(m_stateAsset->resourceHolder->getTexture("deathcloud.png"));
	dc->sprite.setTextureRect(textureRect);
	dc->sprite.setPosition(position);
	dc->sprite.setOrigin(sf::Vector2f(28, 28));

	CDBTweener::CTween* tween = new CDBTweener::CTween();
	tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::EEasing::TWEA_OUT, DEATH_CLOUD_FADE_IN_TIME);
	tween->addValue(&dc->alphaTween, 255);
	tween->addListener(m_deathcloudTweenListener);
	tween->setUserData(dc);
	m_deathcloudTweener.addTween(tween);

	m_deathClouds.push_back(dc);
}

void PlayState::addTotemParticle(sf::IntRect textureRect)
{
	// get random position
	sf::Vector2f random_pos = m_hotSpot->getPosition();

	float q = thor::random(0.f, 1.f) * (b2_pi * 2);
	float r = std::sqrtf(thor::random(0.f, 1.f));
	float x = (m_hotSpot->getRadius() * r) * std::cosf(q);
	float y = (m_hotSpot->getRadius() * r) * std::sinf(q);
	random_pos.x += x;
	random_pos.y += y;

	TotemParticle* particle = new TotemParticle();
	particle->m_sprite->setTextureRect(textureRect);
	particle->m_sprite->setTexture(m_stateAsset->resourceHolder->getTexture("sparcle_particles.png"));
	particle->m_sprite->setOrigin(8, 8);
	particle->m_sprite->setPosition(random_pos);
	particle->m_sprite->setColor(sf::Color(255, 255, 255, 0));

	CDBTweener::CTween *tween = new CDBTweener::CTween();
	tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::EEasing::TWEA_OUT, TOTEM_PARTICLE_FADEIN_TIME);
	tween->addValue(&particle->fadeInAlpha, 255.f);
	tween->setUserData(particle);
	tween->addListener(m_totemParticleListener);
	m_totemTweener.addTween(tween);

	m_totemParticles.push_back(particle);
}
