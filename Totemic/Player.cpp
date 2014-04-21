#include <Thor/Particles.hpp>
#include <Thor\Time\StopWatch.hpp>
#include <Thor\Animation\FrameAnimation.hpp>

#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include "ResourceHolder.h"
#include "Box2D\Dynamics\b2Body.h"
#include "PhysicsHelper.h"

#include <iostream>

Player::Player()
{
	m_won = false;
	m_dead = false;
	m_postCheckDead = false;
	m_changingOrder = false;
	m_defender = nullptr;
	m_gatherer = nullptr;
	m_particleSystem = new thor::ParticleSystem();
	m_tweeningValue = 0.f;
	m_timer = new thor::StopWatch();
	m_deathTimer = new thor::CallbackTimer();
	m_deathTimer->connect(std::bind(&Player::onRespawn, this, std::placeholders::_1));
	m_totemSprite = new sf::Sprite();
}

Player::~Player()
{
	delete m_gatherer;
	m_gatherer = nullptr;

	delete m_defender;
	m_defender = nullptr;

	delete m_particleSystem;
	m_particleSystem = nullptr;

	delete m_deathTimer;
	m_deathTimer = nullptr;

	delete m_totemSprite;
	m_totemSprite = nullptr;

	delete m_timer;
	m_timer = nullptr;
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!m_dead)
	{
		target.draw(*m_gatherer->getSprite());
		target.draw(*m_defender->getSprite());
	}
}
void Player::setDevice(unsigned int p_deviceNo)
{
	m_deviceNo = p_deviceNo;
}
void Player::clear()
{
	if (m_defender != nullptr)
	{
		delete m_defender;
		m_defender = nullptr;
	}

	if (m_gatherer != nullptr)
	{
		delete m_gatherer;
		m_gatherer = nullptr;
	}

	m_dead = false;
}
void Player::setDefender(Defender* p_defender)
{
	m_defender = p_defender;
	m_defender->setPlayer(this);
	m_defender->setType(DEFENDER);
	m_defender->getSprite()->setPosition(m_defender->getSpawnPosition());
	m_defender->getSprite()->setOrigin(128.f / 2.f, 128.f / 2.f);
	
	thor::FrameAnimation* walk_animation = new thor::FrameAnimation();
	walk_animation->addFrame(1.f, sf::IntRect(0,   0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(129, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(258, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(387, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(516, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(645, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(774, 0, 128, 128));
	
	m_defender->addAnimation("Walk_Animation", walk_animation);

	m_defender->getAnimatior()->addAnimation("walk", *walk_animation, sf::seconds(0.5f));
}
void Player::setGatherer(Gatherer* p_gatherer)
{
	m_gatherer = p_gatherer;
	m_gatherer->setPlayer(this);
	m_gatherer->setType(GATHERER);
	m_gatherer->getSprite()->setPosition(m_gatherer->getSpawnPosition());
	m_gatherer->getSprite()->setOrigin(m_gatherer->getSprite()->getLocalBounds().width / 2, m_gatherer->getSprite()->getLocalBounds().height / 2);
}
void Player::initializeParticleSystem(ResourceHolder* resourceHolder)
{
	m_particleSystem->setTexture(resourceHolder->getTexture("particle_round.png"));
}
void Player::setDead(bool value)
{
	m_dead = value;
	m_postCheckDead = true;
}
void Player::setColor(sf::Color color)
{
	m_color = color;
}
void Player::setOrder(unsigned int index)
{
	m_order = index;
}
void Player::setChangingOrder(bool value)
{
	m_changingOrder = value;
}
void Player::processEventualDeath()
{
	if (!m_postCheckDead) return;
	if (m_dead)
	{
		m_gatherer->getBody()->SetActive(false);
		m_gatherer->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(m_gatherer->getSpawnPosition()), m_gatherer->getBody()->GetAngle());
		m_gatherer->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
		m_gatherer->getBody()->SetAngularVelocity(0.f);

		m_defender->getBody()->SetActive(false);
		m_defender->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(m_defender->getSpawnPosition()), m_defender->getBody()->GetAngle());
		m_defender->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
		m_defender->getBody()->SetAngularVelocity(0.f);

		m_deathTimer->restart(sf::seconds(3.f));
	}
	else
	{
		m_defender->getBody()->SetActive(true);
		m_gatherer->getBody()->SetActive(true);
	}
	m_postCheckDead = false;
}
thor::CallbackTimer* Player::getDeathTimer()
{
	return m_deathTimer;
}
thor::ParticleSystem* Player::getParticleSystem()
{
	return m_particleSystem;
}
sf::Sprite* Player::getTotemSprite()
{
	return m_totemSprite;
}
Gatherer* Player::getGatherer()
{
	return m_gatherer;
}
Defender* Player::getDefender()
{
	return m_defender;
}
unsigned int Player::getDevice()
{
	return m_deviceNo;
}
thor::StopWatch* Player::getTimer()
{
	return m_timer;
}
sf::Color Player::getColor()
{
	return m_color;
}
unsigned int Player::getOrder()
{
	return m_order;
}
bool Player::isDead()
{
	return m_dead;
}
bool Player::isChangingOrder()
{
	return m_changingOrder;
}
void Player::onRespawn(thor::CallbackTimer& trigger)
{
	setDead(false);
}
