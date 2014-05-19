#include "ContactListener.h"
#include "Player.h"
#include "UserData.h"
#include "PlayState.h"
#include "Math.h"
#include "Defender.h"
#include "Gatherer.h"
#include "PhysicsHelper.h"
#include <Thor\Particles.hpp>
#include <Thor\Math.hpp>
#include <Thor\Vectors.hpp>
#include "Math.h"

void ContactListener::BeginContact(b2Contact* p_contact)
{
	b2Fixture* fixtureA = p_contact->GetFixtureA();
	b2Fixture* fixtureB = p_contact->GetFixtureB();

	void* bodyUserDataA = fixtureA->GetBody()->GetUserData();
	void* bodyUserDataB = fixtureB->GetBody()->GetUserData();

	if (bodyUserDataA != nullptr && bodyUserDataB != nullptr)
	{
		UserData* userDataA = static_cast<UserData*>(bodyUserDataA);
		UserData* userDataB = static_cast<UserData*>(bodyUserDataB);

		if (userDataA->isType(UserDataType::PLAYER) && userDataB->isType(UserDataType::PLAYER))
		{
			playerContactBegin(userDataA, userDataB);
		}
	}
}

void ContactListener::EndContact(b2Contact* p_contact)
{
	playerContactEnd(p_contact);
}

void ContactListener::playerContactBegin(UserData* userDataA, UserData* userDataB)
{
	PlayerUD* charA = static_cast<PlayerUD*>(userDataA);
	PlayerUD* charB = static_cast<PlayerUD*>(userDataB);

	if (charA->getData()->isType(DEFENDER) && charB->getData()->isType(GATHERER))
	{
		if (!charA->getData()->isSamePlayer(charB->getData()->getPlayer()))
		{
			charB->getData()->getPlayer()->setDying(true);
			sf::Vector2f oldScale = charA->getData()->getPlayer()->getDefender()->getSprite()->getScale();
			charB->getData()->getPlayer()->getGatherer()->getDeathSprite()->setScale(sf::Vector2f(oldScale.x * -1, oldScale.y));

			if (charB->getData()->getPlayer()->getColor() == sf::Color::Blue)
			{
				charB->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charB->getData()->getPlayer()->game->feather);
			}
			else if (charB->getData()->getPlayer()->getColor() == sf::Color::Red)
			{
				charB->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charB->getData()->getPlayer()->game->feather2);
			}
			else if (charB->getData()->getPlayer()->getColor() == sf::Color::Yellow)
			{
				charB->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charB->getData()->getPlayer()->game->feather3);
			}
			else if (charB->getData()->getPlayer()->getColor() == sf::Color(204, 0, 204))
			{
				charB->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charB->getData()->getPlayer()->game->feather4);
			}

			charB->getData()->getPlayer()->game->m_gathererDeathEmitter->setEmissionRate(100);
			charB->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticlePosition(PhysicsHelper::physicsToGameUnits(charB->getData()->getBody()->GetPosition()));
			charB->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticleVelocity(thor::Distributions::deflect(30.f*sf::Vector2f(sqrtf(2.f),sqrtf(2.f)), 180.f));
			charB->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));			
			charB->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticleLifetime(sf::seconds(1.f));
			charB->getData()->getPlayer()->game->m_gathererDeathSystem->addEmitter(*charB->getData()->getPlayer()->game->m_gathererDeathEmitter, sf::seconds(0.1f));
			charB->getData()->getPlayer()->game->m_gathererDeathSystem->addAffector(*charB->getData()->getPlayer()->game->m_gathererDeathAffector);
			charB->getData()->getPlayer()->game->m_gathererDeathSystem->addAffector(*charB->getData()->getPlayer()->game->m_gathererTorqueAffector);
		}
	}
	else if (charB->getData()->isType(DEFENDER) && charA->getData()->isType(GATHERER))
	{
		if (!charB->getData()->isSamePlayer(charA->getData()->getPlayer()))
		{
			charA->getData()->getPlayer()->setDying(true);
			sf::Vector2f oldScale = charB->getData()->getPlayer()->getDefender()->getSprite()->getScale();
			charA->getData()->getPlayer()->getGatherer()->getDeathSprite()->setScale(sf::Vector2f(oldScale.x * -1, oldScale.y));

			if (charA->getData()->getPlayer()->getColor() == sf::Color::Blue)
			{
				charA->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charA->getData()->getPlayer()->game->feather);
			}
			else if (charA->getData()->getPlayer()->getColor() == sf::Color::Red)
			{
				charA->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charA->getData()->getPlayer()->game->feather2);
			}
			else if (charA->getData()->getPlayer()->getColor() == sf::Color::Yellow)
			{
				charA->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charA->getData()->getPlayer()->game->feather3);
			}
			else if (charA->getData()->getPlayer()->getColor() == sf::Color(204, 0, 204))
			{
				charA->getData()->getPlayer()->game->m_gathererDeathSystem->setTexture(charA->getData()->getPlayer()->game->feather4);
			}

			charA->getData()->getPlayer()->game->m_gathererDeathEmitter->setEmissionRate(100);
			charA->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticlePosition(PhysicsHelper::physicsToGameUnits(charA->getData()->getBody()->GetPosition()));
			charA->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticleVelocity(thor::Distributions::deflect(30.f*sf::Vector2f(sqrtf(2.f), sqrtf(2.f)), 180.f));
			charA->getData()->getPlayer()->game->m_gathererDeathEmitter->setParticleLifetime(sf::seconds(1.f));
			charA->getData()->getPlayer()->game->m_gathererDeathSystem->addEmitter(*charA->getData()->getPlayer()->game->m_gathererDeathEmitter,sf::seconds(0.1f));
			charA->getData()->getPlayer()->game->m_gathererDeathSystem->addAffector(*charA->getData()->getPlayer()->game->m_gathererDeathAffector);
			charA->getData()->getPlayer()->game->m_gathererDeathSystem->addAffector(*charA->getData()->getPlayer()->game->m_gathererTorqueAffector);

		}
	}
	else if (charA->getData()->isType(DEFENDER) && charB->getData()->isType(DEFENDER))
	{
		// Get direction between the bodies
		sf::Vector2f direction = Math::direction(
			charA->getData()->getPlayer()->getDefender()->getSprite()->getPosition(),
			charB->getData()->getPlayer()->getDefender()->getSprite()->getPosition());
		sf::Vector2f collision_position = charA->getData()->getPlayer()->getDefender()->getSprite()->getPosition();
		collision_position = collision_position + direction * PhysicsHelper::physicsToGameUnits(charA->getData()->getPlayer()->getDefender()->getBody()->GetFixtureList()[0].GetShape()->m_radius);
		
		//Get the magnitude of the colliding defenders' velocities
		//to get a relative emmission rate
		float emission = Math::sumMagnitude(charA->getData()->getBody()->GetLinearVelocity(), charB->getData()->getBody()->GetLinearVelocity());

		charA->getData()->getPlayer()->game->m_defenderEmitter->setEmissionRate(emission * 3);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticlePosition(collision_position);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleVelocity(thor::Distributions::deflect(300.f*direction, 45.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleRotationSpeed(thor::Distributions::uniform(10.f, 20.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleLifetime(sf::seconds(3.f));
		charA->getData()->getPlayer()->game->m_defenderParticleSystem->addEmitter(*charA->getData()->getPlayer()->game->m_defenderEmitter, sf::seconds(0.1));

		//particle spreads the opposite way as well
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticlePosition(collision_position);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleVelocity(thor::Distributions::deflect(-300.f*direction, 45.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));
		charA->getData()->getPlayer()->game->m_defenderParticleSystem->addEmitter(*charA->getData()->getPlayer()->game->m_defenderEmitter, sf::seconds(0.1));
		
	}
}

void ContactListener::playerContactEnd(b2Contact* p_contact)
{

}