#include <SFML\Graphics.hpp>
#include <Thor\Math.hpp>
#include <Thor\Particles.hpp>
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
#include "Coinbird.h"
#include "TotemTweenerListener.h"

PlayState::PlayState() : m_world(b2Vec2(0.f, 0.f))
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_hasStartedToChangeWinBackgroundOpacity = false;
	m_exclusive = false;
	m_gameWon = false;
	m_setupGameWon = false;
	m_totemIsBlockingPlayer = false;
	m_starting = false;

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

	m_totemHead.setTexture(m_stateAsset->resourceHolder->getTexture("totemhead.png"));
	m_totemHead.setOrigin(129, 157);
	m_totemHead.setPosition(m_players.back()->getTotemSprite()->getPosition().x, m_players.back()->getTotemSprite()->getPosition().y + m_players.back()->getTotemSprite()->getGlobalBounds().height / 2.f);

	int width = 263;
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 0, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 1, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 2, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 3, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 4, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 5, 0, 263, 157));
	m_totemHeadActiveAnimation.addFrame(1.f, sf::IntRect(width * 6, 0, 263, 157));


	m_totemHeadIdleAnimation.addFrame(1.f, sf::IntRect(0, 0, 263, 157));

	m_totemHeadAnimator.addAnimation("active", m_totemHeadActiveAnimation, sf::seconds(0.6));
	m_totemHeadAnimator.addAnimation("idle", m_totemHeadIdleAnimation, sf::seconds(64));
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
	tex.loadFromFile("../assets/textures/defender_particle.png", sf::IntRect(0, 0, 8, 8));
	m_defenderParticleSystem->setTexture(tex);

	m_gathererDeathEmitter = new thor::UniversalEmitter;
	m_gathererDeathSystem = new thor::ParticleSystem;
	feather.loadFromFile("../assets/textures/death_particle_blue.png", sf::IntRect(0, 0, 16, 16));
	feather2.loadFromFile("../assets/textures/death_particle_red.png", sf::IntRect(0, 0, 16, 16));
	feather3.loadFromFile("../assets/textures/death_particle_yellow.png", sf::IntRect(0, 0, 16, 16));
	feather4.loadFromFile("../assets/textures/death_particle_purple.png", sf::IntRect(0, 0, 16, 16));
	m_gathererDeathSystem->setTexture(feather);
	m_gathererDeathAffector = new thor::ForceAffector(sf::Vector2f(0.f, 0.f));
	m_gathererDeathAffector->setAcceleration(sf::Vector2f(0.f, 9.98f));
	m_gathererTorqueAffector = new thor::TorqueAffector(0.f);
	m_gathererTorqueAffector->setAngularAcceleration(90.f);

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

	m_hotSpot->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("totem_glitter.png"));
	m_hotSpot->getSprite()->setOrigin(224, 244);
	m_hotSpot->getSprite()->setPosition(m_stateAsset->windowManager->getWindow()->getSize().x / 2.f, m_stateAsset->windowManager->getWindow()->getSize().y / 2.f);
}

void PlayState::leaving()
{
	delete m_totemTweenerListener;
	m_totemTweenerListener = nullptr;

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

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_contactListener;
	m_contactListener = nullptr;

	delete m_contactFilter;
	m_contactFilter = nullptr;

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
	if (m_actionMap->isActive("Exit"))
	{
		return false;
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
		return true;
	}

	m_hotSpot->getAnimator()->update(sf::seconds(dt));
	m_hotSpot->getAnimator()->animate(*m_hotSpot->getSprite());

	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
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
							if (m_players[k] == nullptr) continue;
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
						int margin = -300;
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
			player->getGatherer()->getAnimatior()->playAnimation("walk", true);
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
	m_currentLevel->update(dt);
	m_defenderParticleSystem->update(sf::seconds(dt));
	m_gathererDeathSystem->update(sf::seconds(dt));
	m_world.Step(1.f / 60.f, 8, 3);

#pragma region Gatherer_Movement
	b2Vec2 up_impulse(0.f, -15.f);
	b2Vec2 down_impulse(0.f, 15.f);
	b2Vec2 left_impulse(-15.f, 0.f);
	b2Vec2 right_impulse(15.f, 0.f);

	if (!m_starting && m_players[0] != nullptr && !m_players[0]->getGatherer()->m_shieldStunned && m_players[0]->getGatherer()->getBody()->IsActive() && !m_players[0]->isStunned())
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
	/*
	This loop does:
	- Gatherer velocity capping
	- Moves the defender and gatherer sprites from the Box2D bodies
	- Responsible for playing animations
	- Checks if gatherer picks up coins and powerups
	- Handle powerup
	- Changes the point indicator in the HUD
	- Processes eventual death
	- Make sure that the totem is transparent when blocking any player
	*/

	bool hasChangedTotemBlockedState = false;
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

		player->processEventualDeath(m_currentLevel);
		player->getDeathTimer()->update();
		player->getDefender()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getDefender()->getBody()->GetPosition()) - sf::Vector2f(0, 64));
		player->getGatherer()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetPosition()) - sf::Vector2f(0, 15));
		player->getGatherer()->m_shieldOverlay->setPosition(player->getGatherer()->getSprite()->getPosition());


		if (!player->getDefender()->getAnimatior()->isPlayingAnimation())
		{
			player->getDefender()->getAnimatior()->playAnimation("walk", true);
		}
		if (!player->getGatherer()->getAnimatior()->isPlayingAnimation())
		{
			player->getGatherer()->getAnimatior()->playAnimation("walk", true);
		}

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
		player->getDefender()->getAnimatior()->update(sf::seconds(dt));
		player->getGatherer()->getAnimatior()->update(sf::seconds(dt));
		player->getGatherer()->m_shieldOverlayAnimatior->update(sf::seconds(dt));

		player->getDefender()->getAnimatior()->animate(*player->getDefender()->getSprite());
		player->getGatherer()->getAnimatior()->animate(*player->getGatherer()->getSprite());
		player->getGatherer()->m_shieldOverlayAnimatior->animate(*player->getGatherer()->m_shieldOverlay);

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
										  m_players[i]->getGatherer()->getAnimatior()->playAnimation("stun", true);
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
		if (m_totemHeadAnimator.getPlayingAnimation() != "active")
			m_totemHeadAnimator.playAnimation("active", true);
		if (!activePlayers.back()->m_holdingTotem)
		{
			activePlayers.back()->m_holdingTotem = true;
			onEnterTotem(activePlayers.back());
		}
		activePlayers.back()->addPoints(POINTS_PER_SECOND * dt, activePlayers.back()->getGatherer()->getSprite()->getPosition(), SCORE_HOTSPOT);
		if (activePlayers.back()->getBounty() > 0)
		{
			float score = activePlayers.back()->getBounty() * SCORE_PER_COIN;
			activePlayers.back()->addPoints(score, activePlayers.back()->getTotemSprite()->getPosition(), PlayerScoreTypes::SCORE_COIN);
			activePlayers.back()->resetBounty();
		}
		updateHoldingTotem(activePlayers.back());
	}
	else
	{
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

	bool expired = m_321GOTimer.isExpired();
	if (m_starting && expired && !m_stateAsset->audioSystem->getSound("321GO")->isPlaying())
	{
		m_stateAsset->audioSystem->playSound("321GO");
		m_123GOAnimator.playAnimation("idle");
	}

	// Update 321GO animation
	if (m_starting && expired)
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
	m_currentLevel->update(dt);
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
	}

	m_stateAsset->windowManager->getWindow()->draw(m_totemFoot);
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
	//m_world.DrawDebugData();

	m_stateAsset->windowManager->getWindow()->draw(m_timerBarBackground);
	m_stateAsset->windowManager->getWindow()->draw(m_timerBar);
	m_stateAsset->windowManager->getWindow()->draw(m_frame);
	m_stateAsset->windowManager->getWindow()->draw(*m_defenderParticleSystem);
	m_stateAsset->windowManager->getWindow()->draw(*m_gathererDeathSystem);
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
	if (m_starting && m_321GOTimer.isExpired())
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
	}
}

void PlayState::initManyMouse()
{
	int numDevices = ManyMouse_Init();
	for (int i = 0; i < numDevices; i++)
	{
		std::string name = ManyMouse_DeviceName(i);
		std::string driver = ManyMouse_DriverName();
		std::cout << name << std::endl;
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
		m_players.back()->game = this;
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
		}
	}
}

void PlayState::setupActions()
{
	m_actionMap->operator[]("p3_up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("p3_down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("p3_left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("p3_right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);

	m_actionMap->operator[]("p1_up") = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
	m_actionMap->operator[]("p1_down") = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
	m_actionMap->operator[]("p1_left") = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
	m_actionMap->operator[]("p1_right") = thor::Action(sf::Keyboard::Right, thor::Action::Hold);

	m_actionMap->operator[]("p2_up") = thor::Action(sf::Keyboard::Y, thor::Action::Hold);
	m_actionMap->operator[]("p2_down") = thor::Action(sf::Keyboard::H, thor::Action::Hold);
	m_actionMap->operator[]("p2_left") = thor::Action(sf::Keyboard::G, thor::Action::Hold);
	m_actionMap->operator[]("p2_right") = thor::Action(sf::Keyboard::J, thor::Action::Hold);

	m_actionMap->operator[]("p4_up") = thor::Action(sf::Keyboard::Numpad8, thor::Action::Hold);
	m_actionMap->operator[]("p4_down") = thor::Action(sf::Keyboard::Numpad5, thor::Action::Hold);
	m_actionMap->operator[]("p4_left") = thor::Action(sf::Keyboard::Numpad4, thor::Action::Hold);
	m_actionMap->operator[]("p4_right") = thor::Action(sf::Keyboard::Numpad6, thor::Action::Hold);

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

void PlayState::setupGameWon()
{
	m_setupGameWon = true;
	float middle_y = static_cast<float>(m_stateAsset->windowManager->getWindow()->getSize().y / 2);
	float middle_x = static_cast<float>(m_stateAsset->windowManager->getWindow()->getSize().x / 2);
	std::vector<Player*> players = m_players;
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
		p->setDead(false);
		p->setShield(false);
		sf::Vector2f def_pos = p->getDefender()->getSprite()->getPosition();
		sf::Vector2f gat_pos = p->getGatherer()->getSprite()->getPosition();
		p->getDefender()->m_tweenX = def_pos.x;
		p->getDefender()->m_tweenY = def_pos.y;
		p->getGatherer()->m_tweenX = gat_pos.x;
		p->getGatherer()->m_tweenY = gat_pos.y;

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
		p->mWinScoreText->setPosition(1920 + 400, startY + 40);

		p->m_tweeningScoreTextX = 1920 + 400;
		p->m_tweeningScoreTextXTarget = startXGat + 170;
		std::cout << dist_secs[a] << std::endl;
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
	//m_timerEmitter->setEmissionRate(40);
	//m_timerEmitter->setParticlePosition(player->getPointsIndicator()->getPosition());
	//m_timerEmitter->setParticleVelocity(thor::Distribution::deflect());
	//m_timerEmitterConnection = m_timerParticleSystem->addEmitter()
}

void PlayState::updateHoldingTotem(Player* player)
{
	for (auto &p : m_players)
	{
		if (p == nullptr) continue;
		if (p != player)
		{
			p->m_holdingTotem = false;
		}
	}
}
