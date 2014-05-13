#include <Thor\Time\StopWatch.hpp>
#include <Thor\Animation\FrameAnimation.hpp>
#include <Thor\Math\Random.hpp>
#include <iostream>
#include <Box2D\Dynamics\b2World.h>

#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include "ResourceHolder.h"
#include "Box2D\Dynamics\b2Body.h"
#include "PhysicsHelper.h"
#include "Config.h"
#include "Math.h"
#include "FloatingScoreText.h"
#include "Level.h"

Player::Player()
{
	m_bounty = 0;
	m_hotspotScoreSum = 0.f;
	m_multiplier = 1.f;
	m_points = 0.f;
	m_shield = false;
	m_holdingTotem = false;
	m_won = false;
	m_dead = false;
	m_stunned = false;
	m_stunnedTimer.reset();
	m_shieldTimer.reset();
	m_postCheckDead = false;
	m_changingOrder = false;
	m_defender = nullptr;
	m_gatherer = nullptr;
	m_tweeningValue = 0.f;
	m_timer = new thor::StopWatch();
	m_deathTimer = new thor::CallbackTimer();
	m_deathTimer->connect(std::bind(&Player::onRespawn, this, std::placeholders::_1));
	m_totemSprite = new sf::Sprite();
	m_totemBountyIcon = new sf::Sprite();
	m_totemBountyIconAnimator = new thor::Animator<sf::Sprite, std::string>;
	m_totemBountyAmount = new sf::Text();
}
Player::~Player()
{
	delete m_gatherer;
	m_gatherer = nullptr;

	delete m_defender;
	m_defender = nullptr;

	delete m_deathTimer;
	m_deathTimer = nullptr;

	delete m_totemSprite;
	m_totemSprite = nullptr;

	delete m_barPointsIndicator;
	m_barPointsIndicator = nullptr;

	delete m_timer;
	m_timer = nullptr;

	delete m_totemBountyAmount;
	m_totemBountyAmount = nullptr;

	delete m_totemBountyIcon;
	m_totemBountyIcon = nullptr;

	delete m_totemBountyIconAnimator;
	m_totemBountyIconAnimator = nullptr;

	delete m_totemBountyAnimation;
	m_totemBountyAnimation = nullptr;
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!m_dead)
	{
		target.draw(*m_gatherer->getSprite());
		if (m_shield)
		target.draw(*m_gatherer->m_shieldOverlay);
		target.draw(*m_defender->getSprite());
	}
}
void Player::setDevice(unsigned int p_deviceNo)
{
	m_deviceNo = p_deviceNo;
}
void Player::clear(b2World& world)
{
	if (m_defender != nullptr)
	{
		world.DestroyBody(m_defender->getBody());
		delete m_defender;
		m_defender = nullptr;
	}

	if (m_gatherer != nullptr)
	{
		world.DestroyBody(m_gatherer->getBody());
		delete m_gatherer;
		m_gatherer = nullptr;
	}

	m_dead = false;
	m_won = false;
	m_points = 0.f;
}
void Player::setDefender(Defender* p_defender)
{
	m_defender = p_defender;
	m_defender->setPlayer(this);
	m_defender->setType(DEFENDER);
	m_defender->getSprite()->setPosition(m_defender->getSpawnPosition());
	m_defender->getSprite()->setOrigin(128.f, 128.f);
	
	thor::FrameAnimation* walk_animation = new thor::FrameAnimation();
	walk_animation->addFrame(1.f, sf::IntRect(0,   0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(257, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(514, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(771, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(1028, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(1285, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(1542, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(1799, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(2056, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(2313, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(2570, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(2827, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(3084, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(3341, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(3598, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(3855, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(4112, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(4369, 0, 256, 256));
	walk_animation->addFrame(1.f, sf::IntRect(4626, 0, 256, 256));
	
	m_defender->addAnimation("Walk_Animation", walk_animation);

	m_defender->getAnimatior()->addAnimation("walk", *walk_animation, sf::seconds(0.5f));
}
void Player::setGatherer(Gatherer* p_gatherer)
{
	m_gatherer = p_gatherer;

	m_gatherer->setPlayer(this);
	m_gatherer->setType(GATHERER);
	m_gatherer->getSprite()->setPosition(m_gatherer->getSpawnPosition());
	m_gatherer->getSprite()->setOrigin(64.f, 64.f);

	m_gatherer->m_shieldOverlay->setPosition(m_gatherer->getSpawnPosition());
	m_gatherer->m_shieldOverlay->setOrigin(73, 69);
	
	thor::FrameAnimation* walk_animation = new thor::FrameAnimation();
	walk_animation->addFrame(1.f, sf::IntRect(0, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(128, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(256, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(385, 0, 128, 128));
	walk_animation->addFrame(1.f, sf::IntRect(512, 0, 128, 128));

	thor::FrameAnimation* death_animation = new thor::FrameAnimation();
	death_animation->addFrame(1.f, sf::IntRect(0, 0, 128, 128));

	m_gatherer->addAnimation("Death_Animation", death_animation);
	m_gatherer->addAnimation("Walk_Animation", walk_animation);

	m_gatherer->getAnimatior()->addAnimation("walk", *walk_animation, sf::seconds(0.5f));
}
void Player::setDead(bool value)
{
	m_dead = value;
	m_postCheckDead = true;
	m_multiplier = 1.f;
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
void Player::addPoints(float value, sf::Vector2f position, PlayerScoreTypes type)
{
	m_points += value;
	if (m_points >= POINTS_TO_WIN)
	{
		m_won = true;
		m_points = POINTS_TO_WIN;
	}

	if (type == SCORE_HOTSPOT)
	{
		if (m_hotspotScoreSum >= FLOATING_SCORE_HOTSPOT_SUM)
		{
			m_hotspotScoreSum = FLOATING_SCORE_HOTSPOT_SUM - m_hotspotScoreSum;
			FloatingScoreText* FST = new FloatingScoreText();
			FST->getText()->setPosition(position);
			FST->getText()->setFont(m_resourceHolder->getFont("arial.ttf"));

			FST->getText()->setString("+" + std::to_string(static_cast<int>(FLOATING_SCORE_HOTSPOT_SUM)));
			FST->setScore(FLOATING_SCORE_HOTSPOT_SUM);
			m_floatingScoreTexts->push_back(FST);
		}
		else
		{
			m_hotspotScoreSum += value;
		}
	}
	else
	{
		FloatingScoreText* FST = new FloatingScoreText();
		FST->getText()->setPosition(position);
		FST->getText()->setFont(m_resourceHolder->getFont("arial.ttf"));

		FST->getText()->setString("+" + std::to_string(static_cast<int>(value)));
		FST->setScore(value);
		m_floatingScoreTexts->push_back(FST);
	}
}
void Player::setPointsBarImage(std::string value)
{
	m_pointsBarImage = value;
}
void Player::setPointsIndicator(sf::Sprite* sprite)
{
	m_barPointsIndicator = sprite;
}
void Player::setFSTRef(std::vector<FloatingScoreText*>& FSTRef)
{
	m_floatingScoreTexts = &FSTRef;
}
void Player::setResourceHolder(ResourceHolder* resourceHolder)
{
	m_resourceHolder = resourceHolder;
}
void Player::processEventualDeath(Level* level)
{
	if (!m_postCheckDead) return;
	if (m_dead)
	{
		std::vector<PlayerSpawn*> playerSpawns = level->getPlayerSpawns();
		int randomSpawnIndex = thor::random(0U, playerSpawns.size() - 1);
		while (playerSpawns[randomSpawnIndex]->occupied == true)
		{
			randomSpawnIndex = thor::random(0U, playerSpawns.size() - 1);
		}
		level->setPlayerSpawnOccupied(randomSpawnIndex, true);

		m_gatherer->getBody()->SetActive(false);
		m_gatherer->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(playerSpawns[randomSpawnIndex]->gat_spawn), m_gatherer->getBody()->GetAngle());
		m_gatherer->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
		m_gatherer->getBody()->SetAngularVelocity(0.f);

		m_defender->getBody()->SetActive(false);
		m_defender->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(playerSpawns[randomSpawnIndex]->def_spawn), m_defender->getBody()->GetAngle());
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
sf::Sprite* Player::getTotemSprite()
{
	return m_totemSprite;
}
sf::Sprite* Player::getPointsIndicator()
{
	return m_barPointsIndicator;
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
float Player::getPoints()
{
	return m_points;
}
std::string Player::getPointsBarImage()
{
	return m_pointsBarImage;
}
bool Player::isDead()
{
	return m_dead;
}
bool Player::isChangingOrder()
{
	return m_changingOrder;
}
bool Player::hasWon()
{
	return m_won;
}
void Player::onRespawn(thor::CallbackTimer& trigger)
{
	setDead(false);
}
void Player::setStunned(bool value)
{
	m_stunned = value;
	if (m_stunned) m_stunnedTimer.start();
	if (!m_stunned) m_stunnedTimer.reset();
}
bool Player::isStunned()
{
	return m_stunned;
}
void Player::setShield(bool value)
{
	m_shield = value;
	m_shieldTimer.restart();
}
void Player::addToBounty(int value)
{
	m_bounty += value;
	m_totemBountyAmount->setString("x" + std::to_string(m_bounty));
	m_totemBountyAmount->setOrigin(m_totemBountyAmount->getGlobalBounds().width / 2.f, 0);
}
void Player::resetBounty()
{
	m_bounty = 0;
}
bool Player::hasShield()
{
	return m_shield;
}
int Player::getBounty()
{
	return m_bounty;
}