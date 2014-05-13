#include "ContactListener.h"
#include "Player.h"
#include "UserData.h"
#include "PlayState.h"
#include "Math.h"
#include "Defender.h"
#include "PhysicsHelper.h"
#include <Thor\Particles.hpp>
#include <Thor\Math.hpp>
#include <Thor\Vectors.hpp>

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
			charB->getData()->getPlayer()->setDead(true);
		}
	}
	else if (charB->getData()->isType(DEFENDER) && charA->getData()->isType(GATHERER))
	{
		if (!charB->getData()->isSamePlayer(charA->getData()->getPlayer()))
		{
			charA->getData()->getPlayer()->setDead(true);
		}
	}
	else if (charA->getData()->isType(DEFENDER) && charB->getData()->isType(DEFENDER))
	{
		// Get direction between the bodies
		sf::Vector2f direction = Math::direction(
			charA->getData()->getPlayer()->getDefender()->getSprite()->getPosition(),
			charB->getData()->getPlayer()->getDefender()->getSprite()->getPosition());
		sf::Vector2f collision_position = charA->getData()->getPlayer()->getDefender()->getSprite()->getPosition();
		collision_position += direction * PhysicsHelper::physicsToGameUnits(charA->getData()->getPlayer()->getDefender()->getBody()->GetFixtureList()[0].GetShape()->m_radius);
		
		sf::Vector2f velocity(thor::random(-1, 1) * 200, thor::random(-1, 1) * 200);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setEmissionRate(90);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticlePosition(collision_position);
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleVelocity(thor::Distributions::deflect(velocity, 180.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleRotationSpeed(thor::Distributions::uniform(10.f, 20.f));
		charA->getData()->getPlayer()->game->m_defenderEmitter->setParticleLifetime(sf::seconds(3.f));
		charA->getData()->getPlayer()->game->m_defenderParticleSystem->addEmitter(*charA->getData()->getPlayer()->game->m_defenderEmitter, sf::seconds(0.1));
	}
}

void ContactListener::playerContactEnd(b2Contact* p_contact)
{

}