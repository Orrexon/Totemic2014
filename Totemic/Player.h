#pragma once

#include <SFML\Graphics.hpp>
#include <Thor\Time\CallbackTimer.hpp>
#include <Thor\Animation\Animator.hpp>
#include <Box2D\Dynamics\b2Body.h>

#include "UserData.h"

namespace thor
{
	class ParticleSystem;
	class StopWatch;
	class FrameAnimation;
}

namespace sf
{
	class Sprite;
}

class ResourceHolder;
class Gatherer;
class Defender;

enum {
	DEFENDER,
	GATHERER
};

class Player;

struct PlayerCharBase
{
	PlayerCharBase()
	{
		m_sprite = new sf::Sprite();
		m_animatior = new thor::Animator<sf::Sprite, std::string>;
	}
	~PlayerCharBase()
	{
		delete m_sprite;
		m_sprite = nullptr;

		delete m_userData;
		m_userData = nullptr;
		
		for (auto &animation : m_animations)
		{
			delete animation.second;
			animation.second = nullptr;
		}
		m_animations.clear();
	}
	void setType(int type)
	{
		m_type = type;
	}

	bool isType(int type)
	{
		return type == m_type;
	}

	int getType()
	{
		return m_type;
	}

	void setPlayer(Player* player)
	{
		m_player = player;
	}

	bool isSamePlayer(Player* player)
	{
		return player == m_player;
	}

	Player* getPlayer()
	{
		return m_player;
	}

	void setSprite(sf::Sprite* sprite)
	{
		m_sprite = sprite;
	}

	sf::Sprite* getSprite()
	{
		return m_sprite;
	}

	void setSpawnPosition(sf::Vector2f spawn_position)
	{
		m_spawnPosition = spawn_position;
	}

	sf::Vector2f getSpawnPosition()
	{
		return m_spawnPosition;
	}

	void setBody(b2Body* body)
	{
		m_userData = new PlayerUD();
		m_userData->setType(UserDataType::PLAYER);
		m_userData->setData(this);

		m_body = body;
		m_body->SetUserData(m_userData);
	}

	b2Body* getBody()
	{
		return m_body;
	}

	void addAnimation(std::string name, thor::FrameAnimation* animation)
	{
		if (m_animations.find(name) == m_animations.end())
		{
			m_animations.insert(std::make_pair(name, animation));
		}
	}

	thor::FrameAnimation* getAnimation(std::string name)
	{
		auto it = m_animations.find(name);
		if (it != m_animations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	thor::Animator<sf::Sprite, std::string>* getAnimatior()
	{
		return m_animatior;
	}

	int m_type;
	Player* m_player;
	sf::Sprite* m_sprite;
	std::map<std::string, thor::FrameAnimation*> m_animations;
	sf::Vector2f m_spawnPosition;
	b2Body* m_body;
	PlayerUD* m_userData;
	thor::Animator<sf::Sprite, std::string>* m_animatior;
};

class Player: public sf::Drawable
{
public:
	Player();
	~Player();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setDevice(unsigned int p_deviceNo);
	void clear();
	void setDefender(Defender* p_defender);
	void setGatherer(Gatherer* p_gatherer);
	void initializeParticleSystem(ResourceHolder* resourceHolder);
	void processEventualDeath();
	void setDead(bool value);
	void setColor(sf::Color color);
	void setOrder(unsigned int index);
	void setChangingOrder(bool value);
	thor::CallbackTimer* getDeathTimer();

	Gatherer* getGatherer();
	Defender* getDefender();
	thor::ParticleSystem* getParticleSystem();
	thor::StopWatch* getTimer();
	sf::Sprite* getTotemSprite();
	sf::Color getColor();
	unsigned int getDevice();
	unsigned int getOrder();

	bool isDead();
	bool isChangingOrder();
	void onRespawn(thor::CallbackTimer& trigger);

public:
	float m_tweeningValue;

private:
	bool m_won;
	bool m_dead;
	bool m_postCheckDead;
	bool m_changingOrder;
	unsigned int m_order;
	unsigned int m_deviceNo;
	Gatherer* m_gatherer;
	Defender* m_defender;
	sf::Sprite* m_totemSprite;
	sf::Color m_color;
	thor::StopWatch* m_timer;
	thor::ParticleSystem* m_particleSystem;
	thor::CallbackTimer* m_deathTimer;
};

