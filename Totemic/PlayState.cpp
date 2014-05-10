﻿#include <SFML\Graphics.hpp>
#include <Thor\Math\Random.hpp>
#include <iostream>
#include "PlayState.h"
#include "MenuState.h"

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
#include "TotemTweenerListener.h"

PlayState::PlayState() : m_world(b2Vec2(0.f, 0.f))
{
	m_winGameTweener = nullptr;
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_exclusive = false;
	m_gameWon = false;

	m_winGameTweener = new CDBTweener();
	m_totemTweenerListener = new TotemTweenerListener();

	m_contactListener = new ContactListener();
	m_contactFilter = new ContactFilter();

	m_world.SetAllowSleeping(true);
	m_world.SetContactListener(m_contactListener);
	m_world.SetContactFilter(m_contactFilter);

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

	m_totemHead.setTexture(m_stateAsset->resourceHolder->getTexture("totempole.png"));
	m_totemHead.setOrigin(129, m_totemHead.getGlobalBounds().height);
	m_totemHead.setPosition(m_players.back()->getTotemSprite()->getPosition().x, m_players.back()->getTotemSprite()->getPosition().y + m_players.back()->getTotemSprite()->getGlobalBounds().height / 2.f);

	m_totemFoot.setTexture(m_stateAsset->resourceHolder->getTexture("totemfoot.png"));
	m_totemFoot.setOrigin(m_totemFoot.getGlobalBounds().width / 2.f, 0.f);
	m_totemFoot.setPosition(m_players.front()->getTotemSprite()->getPosition().x, m_players.front()->getTotemSprite()->getPosition().y + m_players.front()->getTotemSprite()->getGlobalBounds().height / 2.f - 4);

	m_walls.push_back(createWall(sf::Vector2f(FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH), sf::Vector2f(1920 - FRAME_BORDER_WIDTH, FRAME_BORDER_WIDTH))); // ⍐
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

	std::cout << "Entering play state" << std::endl;
}

void PlayState::leaving()
{
	if (m_winGameTweener != nullptr)
	{
		delete m_winGameTweener;
		m_winGameTweener = nullptr;
	}

	delete m_totemTweenerListener;
	m_totemTweenerListener = nullptr;

	auto it_players = m_players.begin();
	while (it_players != m_players.end())
	{
		delete *it_players;
		*it_players = nullptr;
		++it_players;
	}
	m_players.clear();

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_contactListener;
	m_contactListener = nullptr;

	delete m_contactFilter;
	m_contactFilter = nullptr;


	std::cout << "Leaving play state" << std::endl;
}

void PlayState::obscuring()
{
}

void PlayState::releaving()
{
}

bool PlayState::update(float dt)
{
	if (m_actionMap->isActive("Exit"))
	{
		return false;
	}

	if (m_gameWon)
	{
		if (m_actionMap->isActive("Restart"))
		{
			m_gameWon = false;
			loadNewLevel();
		}
		setupWinTweeners();
		return true;
	}

	for (auto &player : m_players)
	{
		if (player->hasWon())
		{
			m_gameWon = true;
		}
	}

#pragma region powerups_update
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
#pragma endregion

#pragma region traps_update
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
						if (Math::pointInCircle(m_players[k]->getGatherer()->getSprite()->getPosition(), traps[i]->getExplosionPosition(), traps[i]->getExplosionRadius()))
						{
							m_players[k]->setDead(true);
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
#pragma endregion

#pragma region coins_update
	std::vector<Coin*> coins = m_currentLevel->getCoins();
	if (coins.size() < MAX_COINS)
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
				Coin* coin = new Coin();
				coin->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("Coin.png"));
				coin->getSprite()->setPosition(coinSpawns[randomCoinSpawn].position);
				coin->getSprite()->setOrigin(32, 32);
				coin->getAnimator()->playAnimation("idle", true);
				coin->m_coinSpawnIndex = randomCoinSpawn;
				m_currentLevel->addCoin(coin);
			}
		}
	}
	coins = m_currentLevel->getCoins();

	for (std::size_t i = 0; i < coins.size(); i++)
	{
		coins[i]->getAnimator()->update(sf::seconds(dt));
		coins[i]->getAnimator()->animate(*coins[i]->getSprite());
	}
#pragma endregion

#pragma region ManyMouseDefender
	// Fetch raw mouse events
	ManyMouseEvent event;
	while (ManyMouse_PollEvent(&event))
	{
		if (m_players[event.device] == nullptr || m_players[event.device]->isStunned() || m_players[event.device]->isDeflected()) continue;
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
			player->getGatherer()->getSprite()->setScale(1.f, 1.f);
		}
		else
		{
			player->getGatherer()->getSprite()->setScale(-1.f, 1.f);
		}

		if (player->isChangingOrder())
		{
			player->getTotemSprite()->setPosition(player->getTotemSprite()->getPosition().x, player->m_tweeningValue);
		}

		if (player->isStunned() && player->m_stunnedTimer.getElapsedTime().asSeconds() >= POWERUP_STUN_TIME)
		{
			player->setStunned(false);
		}
		if (player->hasShield() && player->m_shieldTimer.getElapsedTime().asSeconds() >= POWERUP_SHIELD_TIME)
		{
			player->setShield(false);

		}
		if (player->isDeflected() && player->m_deflectionTimer.getElapsedTime().asSeconds() >= DEFLECTIONTIME);
		{
			player->setDeflected(false);
		}
	}
#pragma endregion

	m_world.Step(1.f / 60.f, 8, 3);
#pragma region Power_Ups_update
	for (auto player : m_players)
	{
		if (player->hasShield())
		{
			player->setPlaceholderShieldPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetWorldCenter()));

		}
	}
	for (auto player : m_players)
	{
		if (player->isDeflected())
		{
			//player->getDefender()->getBody()->ApplyLinearImpulse(300 * player->NormDir, player->getDefender()->getBody()->GetWorldCenter(), true);
			//player->getDefender()->getBody()->ApplyForce(300 * player->NormDir, player->getDefender()->getBody()->GetWorldCenter(), true);
			player->getDefender()->getBody()->SetLinearVelocity(30 * player->NormDir);
		}
	}
#pragma endregion

#pragma region Gatherer_Movement
	b2Vec2 up_impulse(0.f, -15.f);
	b2Vec2 down_impulse(0.f, 15.f);
	b2Vec2 left_impulse(-15.f, 0.f);
	b2Vec2 right_impulse(15.f, 0.f);

	if (m_players[0] != nullptr && m_players[0]->getGatherer()->getBody()->IsActive() && !m_players[0]->isStunned())
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

	if (m_players[1] != nullptr && m_players[1]->getGatherer()->getBody()->IsActive() && !m_players[1]->isStunned())
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
#pragma endregion

	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
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

		player->processEventualDeath();
		player->getDeathTimer()->update();
		player->getDefender()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getDefender()->getBody()->GetPosition()));
		player->getGatherer()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetPosition()));

		if (!player->getDefender()->getAnimatior()->isPlayingAnimation())
		{
			player->getDefender()->getAnimatior()->playAnimation("walk");
		}
		if (!player->getGatherer()->getAnimatior()->isPlayingAnimation())
		{
			player->getGatherer()->getAnimatior()->playAnimation("walk");
		}
		player->getDefender()->getAnimatior()->update(sf::seconds(dt));
		player->getGatherer()->getAnimatior()->update(sf::seconds(dt));

		player->getDefender()->getAnimatior()->animate(*player->getDefender()->getSprite());
		player->getGatherer()->getAnimatior()->animate(*player->getGatherer()->getSprite());

		std::vector<Coin*> coins = m_currentLevel->getCoins();
		auto it = coins.begin();
		while (it != coins.end())
		{
			// If player stands on a coin, pick it up
			if (player->getGatherer()->getSprite()->getGlobalBounds().intersects((*it)->getSprite()->getGlobalBounds()))
			{
				player->addPoints(POINTS_PER_COIN * player->m_multiplier, (*it)->getSprite()->getPosition(), SCORE_COIN);
				player->m_multiplier *= COIN_MULTIPLIER;

				// Set the spawn area as not occupied
				m_currentLevel->setCoinSpawnOccupied((*it)->m_coinSpawnIndex, false);

				delete *it;
				*it = nullptr;
				it = coins.erase(it);


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
	}

	std::vector<Player*> activePlayers = m_hotSpot->getActivePlayers(m_players);
	if (activePlayers.size() == 1)
	{
		activePlayers.back()->addPoints(POINTS_PER_SECOND * dt, activePlayers.back()->getGatherer()->getSprite()->getPosition(), SCORE_HOTSPOT);
	}

	sortTotem();
	m_totemTweener.step(dt);

	// Update lightning effect
	sf::Color oldColor = m_lightningEffect.getFillColor();
	oldColor.a = static_cast<int>(m_lightningAlpha);
	m_lightningEffect.setFillColor(oldColor);
	return true;
}

void PlayState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(*m_currentLevel);
	m_stateAsset->windowManager->getWindow()->draw(*m_hotSpot->getShape());
	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player);
	}

	m_stateAsset->windowManager->getWindow()->draw(m_totemFoot);
	m_stateAsset->windowManager->getWindow()->draw(m_totemHead);
	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->getTotemSprite());
	}

	Box2DWorldDraw debugDraw(m_stateAsset->windowManager->getWindow());
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	m_world.SetDebugDraw(&debugDraw);
	m_world.DrawDebugData();

	m_stateAsset->windowManager->getWindow()->draw(m_timerBarBackground);
	m_stateAsset->windowManager->getWindow()->draw(m_timerBar);
	m_stateAsset->windowManager->getWindow()->draw(m_frame);

	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->getPointsIndicator());
	}

	for (auto &FST : m_floatingScoreTexts)
	{
		m_stateAsset->windowManager->getWindow()->draw(*FST->getText());
	}
	for (auto player : m_players)
	{
		if (player->hasShield())
		{
			m_stateAsset->windowManager->getWindow()->draw(player->getPlaceholderShield());
		}
	}
	m_stateAsset->windowManager->getWindow()->draw(m_lightningEffect);
}

void PlayState::initManyMouse()
{
	int numDevices = ManyMouse_Init();
	for (int i = 0; i < numDevices; i++)
	{
		std::string name = ManyMouse_DeviceName(i);
		if (name.find("Pad") != std::string::npos)
		{
			//m_mouseIndicies.push_back(-1);
			m_mouseIndicies.push_back(i);
			continue;
		}
		else
		{
			m_mouseIndicies.push_back(i);
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
	playerColors.push_back(sf::Color::Blue);
	playerColors.push_back(sf::Color::Red);
	playerColors.push_back(sf::Color::Yellow);
	playerColors.push_back(sf::Color(204, 0, 204));

	for (std::size_t i = 0; i < 4; i++)
	{
		m_players.push_back(new Player());
		m_players.back()->setResourceHolder(m_stateAsset->resourceHolder);
		m_players.back()->setFSTRef(m_floatingScoreTexts);
		m_players.back()->setColor(playerColors[i]);
		m_players.back()->getTotemSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(playerTotemImages[i]));
		m_players.back()->setOrder(i);
		m_players.back()->setPointsBarImage(playerBarImages[i]);

		sf::Sprite* sprite = new sf::Sprite();
		sprite->setTexture(m_stateAsset->resourceHolder->getTexture(playerPointIndicatorImages[i]));
		sprite->setOrigin(sprite->getGlobalBounds().width / 2.f, sprite->getGlobalBounds().height);
		sprite->setPosition(TIMER_POS_X, TIMER_POS_Y);
		m_players.back()->setPointsIndicator(sprite);
	}
}

void PlayState::setupActions()
{
	m_actionMap->operator[]("p1_up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("p1_down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("p1_left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("p1_right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);

	m_actionMap->operator[]("p2_up") = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
	m_actionMap->operator[]("p2_down") = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
	m_actionMap->operator[]("p2_left") = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
	m_actionMap->operator[]("p2_right") = thor::Action(sf::Keyboard::Right, thor::Action::Hold);

	m_actionMap->operator[]("p3_up") = thor::Action(sf::Keyboard::Y, thor::Action::Hold);
	m_actionMap->operator[]("p3_down") = thor::Action(sf::Keyboard::H, thor::Action::Hold);
	m_actionMap->operator[]("p3_left") = thor::Action(sf::Keyboard::G, thor::Action::Hold);
	m_actionMap->operator[]("p3_right") = thor::Action(sf::Keyboard::J, thor::Action::Hold);

	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	m_actionMap->operator[]("Restart") = thor::Action(sf::Keyboard::R, thor::Action::PressOnce);
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
	m_currentLevel->getBackground()->setTexture(m_stateAsset->resourceHolder->getTexture(m_currentLevel->getBackgroundPath(), false));

	m_hotSpot->setRadius(m_currentLevel->getHotspotRadius());
	m_hotSpot->setPosition(m_currentLevel->getHotspotPosition());

	float start_y_position = m_hotSpot->getPosition().y - m_players[0]->getTotemSprite()->getGlobalBounds().height / 2.f;
	for (auto &player : m_players)
	{
		player->getTotemSprite()->setOrigin(player->getTotemSprite()->getGlobalBounds().width / 2.f, player->getTotemSprite()->getGlobalBounds().height / 2.f);
		player->getTotemSprite()->setPosition(m_hotSpot->getPosition().x, start_y_position);
		start_y_position -= player->getTotemSprite()->getGlobalBounds().height - 2;
	}

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

	for (std::size_t i = 0; i < m_players.size(); i++)
	{
		m_players[i]->clear(m_world);
		Defender* defender = new Defender();
		Gatherer* gatherer = new Gatherer();

		m_stateAsset->resourceHolder->getTexture(defender_textures[i]).setSmooth(true);
		m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]).setSmooth(true);

		defender->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(defender_textures[i]));
		gatherer->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]));

		defender->setSpawnPosition(m_currentLevel->getDefenderSpawn(i));
		gatherer->setSpawnPosition(m_currentLevel->getGathererSpawn(i));

		m_players[i]->setDefender(defender);
		m_players[i]->setGatherer(gatherer);
	}

	createPlayerBodies();
}

void PlayState::createPlayerBodies()
{
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;

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
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(48);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &shape;
			fixtureDef.restitution = 0.6f;
			body->CreateFixture(&fixtureDef);
			player->getDefender()->setBody(body);
		}

		{ // Gatherer
			b2BodyDef bodyDef;
			b2Vec2 physicsPosition = PhysicsHelper::gameToPhysicsUnits(player->getGatherer()->getSprite()->getPosition());
			bodyDef.position.Set(physicsPosition.x, physicsPosition.y);
			bodyDef.type = b2_dynamicBody;
			bodyDef.angle = 0;
			bodyDef.linearDamping = 0.3f;
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
		}
	}
}

bool sortTotemAlgorithm(Player* a, Player* b)
{
	//return a->getTimer()->getElapsedTime().asSeconds() < b->getTimer()->getElapsedTime().asSeconds();
	return a->getPoints() < b->getPoints();
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
		float newPositionY = start_y_position;
		if (i == sortedPlayerVector.size() - 1) newPositionY -= 8;

		float oldPositionY = sortedPlayerVector[i]->getTotemSprite()->getPosition().y;

		if (newPositionY != oldPositionY)
		{
			sortedPlayerVector[i]->m_tweeningValue = oldPositionY;

			CDBTweener::CTween* tween = new CDBTweener::CTween();
			tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 0.5f);
			tween->addValue(&sortedPlayerVector[i]->m_tweeningValue, newPositionY);
			tween->setUserData(sortedPlayerVector[i]);
			tween->addListener(m_totemTweenerListener);
			m_totemTweener.addTween(tween);
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

void PlayState::setupWinTweeners()
{

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