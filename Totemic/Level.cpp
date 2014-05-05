#include "Level.h"
#include <iostream>
#include <Box2D\Box2D.h>
#include "PhysicsHelper.h"
#include "ResourceHolder.h"
#include "Trap.h"
#include "Coin.h"
#include "Powerup.h"

Level::Level()
{
	m_background = new sf::Sprite();
	m_defenderSpawn.resize(4);
	m_gathererSpawn.resize(4);

	m_coinTimer = new thor::StopWatch();
	m_trapTimer = new thor::StopWatch();
	m_powerupTimer = new thor::StopWatch();

	m_trapTimer->start();
	m_powerupTimer->start();
}

Level::~Level()
{
	delete m_background;
	m_background = nullptr;

	delete m_coinTimer;
	delete m_trapTimer;
	delete m_powerupTimer;

	m_coinTimer = nullptr;
	m_trapTimer = nullptr;
	m_powerupTimer = nullptr;

	auto coinIt = m_coins.begin();
	while (coinIt != m_coins.end())
	{
		delete *coinIt;
		*coinIt = nullptr;
		++coinIt;
	}

	auto trapIt = m_traps.begin();
	while (trapIt != m_traps.end())
	{
		delete *trapIt;
		*trapIt = nullptr;
		++trapIt;
	}

	auto powerupIt = m_powerups.begin();
	while (powerupIt != m_powerups.end())
	{
		delete *powerupIt;
		*powerupIt = nullptr;
		++powerupIt;
	}
}
void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_background);
	for (auto &obj : m_objects)
	{
		target.draw(obj->sprite);
	}

	for (auto &trap : m_traps)
	{
		if (trap->isActive())
		{
			target.draw(trap->getSprite());
		}
	}
	
	for (auto &coin : m_coins)
	{
		target.draw(*coin->getSprite());
	}

	for (auto &powerup : m_powerups)
	{
		target.draw(*powerup->getSprite());
	}

	for (auto &image : m_images)
	{
		target.draw(image->sprite);
	}
}
void Level::addObject(LevelObject* obj)
{
	m_objects.push_back(obj);
}
void Level::addCoin(Coin* obj)
{
	m_coins.push_back(obj);
}
void Level::addTrap(Trap* trap)
{
	m_traps.push_back(trap);
}
void Level::addPowerup(Powerup* powerup)
{
	m_powerups.push_back(powerup);
}
void Level::addImage(Image* image)
{
	m_images.push_back(image);
}
void Level::setBackgroundPath(std::string p_filepath)
{
	m_backgroundPath = p_filepath;
}
void Level::setHotspotPosition(float x, float y)
{
	m_hotspotPosition.x = x;
	m_hotspotPosition.y = y;
}
void Level::setHotspotRadius(float radius)
{
	m_hotspotRadius = radius;
}
void Level::addCoinSpawn(sf::Vector2f position)
{
	CoinObject obj;
	obj.position = position;
	obj.occupied = false;
	m_coinsSpawns.push_back(obj);
}
void Level::addPowerupSpawn(sf::Vector2f position)
{
	PowerupObject obj;
	obj.position = position;
	obj.occupied = false;
	m_powerupsSpawn.push_back(obj);
}
void Level::setDefenderSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_defenderSpawn[player_index] = spawn;
}
void Level::setGathererSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_gathererSpawn[player_index] = spawn;
}
void Level::setCoinSpawnOccupied(int index, bool value)
{
	m_coinsSpawns[index].occupied = value;
}
void Level::setPowerupSpawnOccupied(int index, bool value)
{
	m_powerupsSpawn[index].occupied = value;
}
void Level::setNewCoins(std::vector<Coin*> coinsCont)
{
	m_coins = coinsCont;
}
void Level::setNewPowerups(std::vector<Powerup*> powerupsCont)
{
	m_powerups = powerupsCont;
}

std::vector<Trap*> Level::getTraps()
{
	return m_traps;
}
std::vector<Coin*> Level::getCoins()
{
	return m_coins;
}
std::vector<Powerup*> Level::getPowerups()
{
	return m_powerups;
}
std::vector<CoinObject> &Level::getCoinSpawns()
{
	return m_coinsSpawns;
}
std::vector<PowerupObject> &Level::getPowerupsSpawns()
{
	return m_powerupsSpawn;
}
sf::Vector2f Level::getDefenderSpawn(int player_index)
{
	return m_defenderSpawn[player_index];
}
sf::Vector2f Level::getGathererSpawn(int player_index)
{
	return m_gathererSpawn[player_index];
}
sf::Sprite* Level::getBackground()
{
	return m_background;
}
std::string Level::getBackgroundPath()
{
	return m_backgroundPath;
}
sf::Vector2f Level::getHotspotPosition()
{
	return m_hotspotPosition;
}
float Level::getHotspotRadius()
{
	return m_hotspotRadius;
}

thor::StopWatch *Level::getTrapTimer()
{
	return m_trapTimer;
}
thor::StopWatch *Level::getCoinTimer()
{
	return m_coinTimer;
}
thor::StopWatch *Level::getPowerupTimer()
{
	return m_powerupTimer;
}