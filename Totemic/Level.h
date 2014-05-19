#pragma once

#include <SFML\System\Vector2.hpp>
#include <Box2D\Dynamics\b2Body.h>
#include <Thor\Math.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>
#include <Thor\Time\Timer.hpp>

namespace sf
{
	class Sprite;
}

struct LevelObject
{
	bool use_circle;
	float circle_radius;
	std::string image_path;
	sf::Sprite sprite;
	b2Body* body;
	std::vector<sf::Vector2f> vertices;
	std::vector<thor::Triangle<sf::Vector2f>> triangles;
};

struct CoinObject
{
	sf::Vector2f position;
	bool occupied;
};

struct PowerupObject
{
	sf::Vector2f position;
	bool occupied;
};

struct PlayerSpawn
{
	sf::Vector2f def_spawn;
	sf::Vector2f gat_spawn;
	bool occupied;
};

struct Image
{
	sf::Sprite sprite;
};

class ResourceHolder;
class Trap;
class Coin;
class Powerup;
class PlayState;
class Coinbird;

class Level: public sf::Drawable
{
public:
	Level();
	~Level();

	void update(float dt);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void addObject(LevelObject* obj);
	void addTrap(Trap* trap);
	void addCoin(Coin* obj);
	void addCoinbird(Coinbird* coinbird);
	void addPowerup(Powerup* powerup);
	void addImage(Image* image);
	void setBackgroundPath(std::string p_filepath);
	void setDefenderSpawn(int player_index, float x, float y);
	void setGathererSpawn(int player_index, float x, float y);
	void setPlayerSpawn(int player_index, sf::Vector2f def_spawn, sf::Vector2f gat_spawn);
	void setHotspotPosition(float x, float y);
	void setHotspotRadius(float radius);
	void addCoinSpawn(sf::Vector2f position);
	void addPowerupSpawn(sf::Vector2f position);
	void setCoinSpawnOccupied(int index, bool value);
	void setPowerupSpawnOccupied(int index, bool value);
	void setPlayerSpawnOccupied(int index, bool value);
	void setNewCoins(std::vector<Coin*> coinsCont);
	void setNewPowerups(std::vector<Powerup*> powerupsCont);
	void drawFlyingCoins(sf::RenderWindow* window);

	std::vector<Trap*> getTraps();
	std::vector<Coin*> getCoins();
	std::vector<Powerup*> getPowerups();
	std::vector<CoinObject> &getCoinSpawns();
	std::vector<PowerupObject> &getPowerupsSpawns();
	std::vector<PlayerSpawn*> getPlayerSpawns();
	sf::Vector2f getDefenderSpawn(int player_index);
	sf::Vector2f getGathererSpawn(int player_index);
	sf::Sprite* getBackground();
	std::string getBackgroundPath();
	sf::Vector2f getHotspotPosition();
	float getHotspotRadius();
	thor::StopWatch *getTrapTimer();
	thor::StopWatch *getCoinTimer();
	thor::StopWatch *getPowerupTimer();

public:
	PlayState* game;

private:
	sf::Sprite* m_background;
	std::string m_backgroundPath;
	std::vector<CoinObject> m_coinsSpawns;
	std::vector<PowerupObject> m_powerupsSpawn;
	std::vector<PlayerSpawn*> m_playerSpawns;
	std::vector<sf::Vector2f> m_defenderSpawn;
	std::vector<sf::Vector2f> m_gathererSpawn;
	std::vector<LevelObject*> m_objects;
	std::vector<Trap*> m_traps;
	std::vector<Coin*> m_coins;
	std::vector<Coinbird*> m_coinBirds;
	std::vector<Powerup*> m_powerups;
	std::vector<Image*> m_images;
	sf::Vector2f m_hotspotPosition;
	thor::StopWatch* m_trapTimer;
	thor::StopWatch* m_coinTimer;
	thor::StopWatch* m_powerupTimer;
	float m_hotspotRadius;
};

