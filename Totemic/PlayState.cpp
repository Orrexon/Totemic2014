#include <Thor\Input.hpp>
#include <Thor\Particles.hpp>
#include <SFML\Graphics.hpp>
#include <Thor\Math\Random.hpp>
#include <iostream>
#include "PlayState.h"
#include "MenuState.h"
#include "GameState.h"

#include "GameStateManager.h"
#include "WindowManager.h"
#include "ResourceHolder.h"

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
#include "Box2DWorldDraw.h"
#include "ContactListener.h"
#include "ContactFilter.h"
#include "TotemTweenerListener.h"

const float SECONDS_WIN = 15.f;

PlayState::PlayState() : m_world(b2Vec2(0.f, 0.f))
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_exclusive = false;

	m_totemTweenerListener = new TotemTweenerListener();

	m_contactListener = new ContactListener();
	m_contactFilter = new ContactFilter();

	m_world.SetAllowSleeping(true);
	m_world.SetContactListener(m_contactListener);
	m_world.SetContactFilter(m_contactFilter);

	m_playerParticleEmitter = new thor::UniversalEmitter();
	m_playerParticleEmitter->setEmissionRate(30);
	m_playerParticleEmitter->setParticleLifetime(sf::seconds(5));
	m_playerParticleEmitter->setParticlePosition(thor::Distributions::circle(sf::Vector2f(300, 300), 100));
	m_playerParticleEmitter->setParticleVelocity(thor::Distributions::deflect(sf::Vector2f(1.f, 1.f), 15.f));

	initManyMouse();
	initPlayers();

	m_levelLoader = new LevelLoader();
	m_levelLoader->setDirectory("../levels/");
	m_currentLevel = nullptr;

	m_hotSpot = new HotSpot();

	loadNewLevel();
	createPlayerBodies();
	m_walls.push_back(createWall(sf::Vector2f(0, 0), sf::Vector2f(1920, 0))); // ⍐
	m_walls.push_back(createWall(sf::Vector2f(1920, 0), sf::Vector2f(1920, 1080))); // ⍈
	m_walls.push_back(createWall(sf::Vector2f(0, 1080), sf::Vector2f(1920, 1080))); // ⍗
	m_walls.push_back(createWall(sf::Vector2f(0, 0), sf::Vector2f(0, 1080))); // ⍇

	m_timerBarBackground = new sf::RectangleShape();
	m_timerBarBackground->setSize(sf::Vector2f(500.f, 20.f));
	m_timerBarBackground->setPosition(1920.f / 2.f - m_timerBarBackground->getSize().x / 2.f, 50);
	m_timerBarBackground->setFillColor(sf::Color(100, 100, 100));

	m_timerBar = new sf::RectangleShape();
	m_timerBar->setSize(sf::Vector2f(0, 20.f));
	m_timerBar->setPosition(1920.f / 2.f - m_timerBarBackground->getSize().x / 2.f, 50);


	std::cout << "Entering play state" << std::endl;
}

void PlayState::leaving()
{
	delete m_totemTweenerListener;
	m_totemTweenerListener = nullptr;

	auto it = m_players.begin();
	while (it != m_players.end())
	{
		delete *it;
		*it = nullptr;
		++it;
	}
	m_players.clear();

	delete m_playerParticleEmitter;
	m_playerParticleEmitter = nullptr;

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_contactListener;
	m_contactListener = nullptr;

	delete m_contactFilter;
	m_contactFilter = nullptr;

	delete m_timerBarBackground;
	m_timerBarBackground = nullptr;

	delete m_timerBar;
	m_timerBar = nullptr;

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

	// Fetch raw mouse events
	ManyMouseEvent event;
	while (ManyMouse_PollEvent(&event))
	{
		if (m_players[event.device] == nullptr) continue;
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

	sf::Vector2f max_velocity(100.f, 100.f);
	sf::Vector2f up_impulse(0.f, -9.f);
	sf::Vector2f down_impulse(0.f, 9.f);
	sf::Vector2f left_impulse(-9.f, 0.f);
	sf::Vector2f right_impulse(9.f, 0.f);

	if (m_players[0] != nullptr && m_players[0]->getGatherer()->getBody()->IsActive())
	{
		b2Vec2 body_point = m_players[0]->getGatherer()->getBody()->GetPosition();
		if (m_actionMap->isActive("p1_up"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(up_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p1_down"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(down_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p1_left"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(left_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p1_right"))
		{
			m_players[0]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(right_impulse), body_point, true);
		}
	}

	if (m_players[1] != nullptr && m_players[1]->getGatherer()->getBody()->IsActive())
	{
		b2Vec2 body_point = m_players[1]->getGatherer()->getBody()->GetPosition();
		if (m_actionMap->isActive("p2_up"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(up_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p2_down"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(down_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p2_left"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(left_impulse), body_point, true);
		}
		if (m_actionMap->isActive("p2_right"))
		{
			m_players[1]->getGatherer()->getBody()->ApplyLinearImpulse(PhysicsHelper::gameToPhysicsUnits(right_impulse), body_point, true);
		}
	}

	// Cap to max velocity
	for (auto &player : m_players)
	{
		sf::Vector2f current_velocity = PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetLinearVelocity());
		if (current_velocity.x > max_velocity.x) current_velocity.x = max_velocity.x;
		if (current_velocity.x < -max_velocity.x) current_velocity.x = -max_velocity.x;
		if (current_velocity.y > max_velocity.y) current_velocity.y = max_velocity.y;
		if (current_velocity.y < -max_velocity.y) current_velocity.y = -max_velocity.y;
		player->getGatherer()->getBody()->SetLinearVelocity(PhysicsHelper::gameToPhysicsUnits(current_velocity));

		if (player->getDefender()->getBody()->GetLinearVelocity().x < 0)
		{
			player->getDefender()->getSprite()->setScale(1.f, 1.f);
		}
		else
		{
			player->getDefender()->getSprite()->setScale(-1.f, 1.f);
		}
		
		if (player->isChangingOrder())
		{
			player->getTotemSprite()->setPosition(player->getTotemSprite()->getPosition().x, player->m_tweeningValue);
		}
	}

	m_world.Step(1.f / 60.f, 8, 3);

	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		player->processEventualDeath();
		player->getDeathTimer()->update();
		player->getParticleSystem()->update(sf::seconds(dt));
		player->getDefender()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getDefender()->getBody()->GetPosition()));
		player->getGatherer()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetPosition()));
		
		if (!player->getDefender()->getAnimatior()->isPlayingAnimation())
		{
			player->getDefender()->getAnimatior()->playAnimation("walk");
		}
		player->getDefender()->getAnimatior()->update(sf::seconds(dt));
		//player->getGatherer()->getAnimatior().update(sf::seconds(dt));
		
		player->getDefender()->getAnimatior()->animate(*player->getDefender()->getSprite());
		//player->getGatherer()->getAnimatior().animate(*player->getGatherer()->getSprite());
	}

	std::vector<Player*> activePlayers = m_hotSpot->getActivePlayers(m_players);
	if (activePlayers.size() == 1)
	{
		// A fix for cases where two players walk into/out of the hotspot at the same frame
		for (auto &player : m_players)
		{
			if (player != activePlayers.back())
			{
				player->getTimer()->stop();
			}
		}

		if (!activePlayers.back()->getTimer()->isRunning())
		{
			activePlayers.back()->getTimer()->start();
			m_timerBar->setFillColor(activePlayers.back()->getColor());
		}
		
		float seconds = activePlayers.back()->getTimer()->getElapsedTime().asSeconds();
		float percent = seconds / SECONDS_WIN;
		m_timerBar->setSize(sf::Vector2f(m_timerBarBackground->getSize().x * percent, m_timerBar->getSize().y));
	}
	else
	{
		m_timerBar->setSize(sf::Vector2f(0.f, m_timerBar->getSize().y));
		for (auto &player : m_players)
		{
			if (player->getTimer()->isRunning())
			{
				player->getTimer()->stop();
			}
		}
	}

	sortTotem();
	m_totemTweener.step(dt);
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

	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->getParticleSystem());
	}

	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->getTotemSprite());
	}

	Box2DWorldDraw debugDraw(m_stateAsset->windowManager->getWindow());
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	m_world.SetDebugDraw(&debugDraw);
	m_world.DrawDebugData();

	m_stateAsset->windowManager->getWindow()->draw(*m_timerBarBackground);
	m_stateAsset->windowManager->getWindow()->draw(*m_timerBar);
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
	m_totemPlayerIndices.clear();

	std::vector<std::string> playerTotemImages;
	playerTotemImages.push_back("totem_blue.png");
	playerTotemImages.push_back("totem_red.png");
	playerTotemImages.push_back("totem_yellow.png");
	playerTotemImages.push_back("totem_purple.png");

	std::vector<sf::Color> playerColors;
	playerColors.push_back(sf::Color::Blue);
	playerColors.push_back(sf::Color::Red);
	playerColors.push_back(sf::Color::Yellow);
	playerColors.push_back(sf::Color(204, 0, 204));

	//for (std::size_t i = 0; i < m_mouseIndicies.size(); i++)
	for (std::size_t i = 0; i < 4; i++)
	{
		//if (m_mouseIndicies[i] != -1)
		//{
			m_players.push_back(new Player());
			m_players.back()->setColor(playerColors[i]);
			m_players.back()->getTotemSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(playerTotemImages[i]));
			m_players.back()->setOrder(i);
			continue;
		//}
		//m_players.push_back(nullptr);
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

	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
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

	m_currentLevel = m_levelLoader->parseLevel(levels[randomLevelIndex]);
	m_currentLevel->constructObjects(&m_world, m_stateAsset->resourceHolder);
	m_currentLevel->getBackground()->setTexture(m_stateAsset->resourceHolder->getTexture(m_currentLevel->getBackgroundPath(), false));

	m_hotSpot->setRadius(m_currentLevel->getHotspotRadius());
	m_hotSpot->setPosition(m_currentLevel->getHotspotPosition());

	float start_y_position = m_hotSpot->getPosition().y;
	for (auto &player : m_players)
	{
		player->getTotemSprite()->setOrigin(player->getTotemSprite()->getGlobalBounds().width / 2.f, player->getTotemSprite()->getGlobalBounds().height / 2.f);
		player->getTotemSprite()->setPosition(m_hotSpot->getPosition().x, start_y_position);
		start_y_position -= player->getTotemSprite()->getGlobalBounds().height;
	}

	// Create defenders and gatherers
	std::vector<std::string> defender_textures;
	defender_textures.push_back("def_blue.png");
	defender_textures.push_back("def_red.png");
	defender_textures.push_back("def_yellow.png");
	defender_textures.push_back("def_purple.png");

	std::vector<std::string> gatherer_textures;
	gatherer_textures.push_back("blue_g.png");
	gatherer_textures.push_back("red_g.png");
	gatherer_textures.push_back("yellow_g.png");
	gatherer_textures.push_back("purple_g.png");

	for (std::size_t i = 0; i < m_players.size(); i++)
	{
		m_players[i]->clear();
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

		m_players[i]->initializeParticleSystem(m_stateAsset->resourceHolder);
		
		// Add particle emitters to players particlesystems
		m_players[i]->getParticleSystem()->addEmitter(*m_playerParticleEmitter, sf::seconds(10));
	}
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
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(64);

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
			bodyDef.linearDamping = 1.f;
			b2Body* body = m_world.CreateBody(&bodyDef);

			b2CircleShape shape;
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(player->getGatherer()->getSprite()->getGlobalBounds().width / 2);

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
	return a->getTimer()->getElapsedTime().asSeconds() < b->getTimer()->getElapsedTime().asSeconds();
}

void PlayState::sortTotem()
{
	std::vector<Player*> sortedPlayerVector = m_players;
	std::sort(sortedPlayerVector.begin(), sortedPlayerVector.end(), sortTotemAlgorithm);
	
	// Find out difference of all totem heads
	float start_y_position = m_hotSpot->getPosition().y;
	for (int i = 0; i < sortedPlayerVector.size(); i++)
	{
		float newPositionY = start_y_position;
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
			
			//std::cout << "Order has changed from y-position " << oldPositionY << " to " <<newPositionY << std::endl;
			sortedPlayerVector[i]->setChangingOrder(true);
			
			//sortedPlayerVector[i]->getTotemSprite()->setPosition(m_hotSpot->getPosition().x, newPositionY);
		}
		start_y_position -= sortedPlayerVector[i]->getTotemSprite()->getGlobalBounds().height;
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