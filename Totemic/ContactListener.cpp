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
		}
	}
	else if (charB->getData()->isType(DEFENDER) && charA->getData()->isType(GATHERER))
	{
		if (!charB->getData()->isSamePlayer(charA->getData()->getPlayer()))
		{
			charA->getData()->getPlayer()->setDying(true);
			sf::Vector2f oldScale = charB->getData()->getPlayer()->getDefender()->getSprite()->getScale();
			charA->getData()->getPlayer()->getGatherer()->getDeathSprite()->setScale(sf::Vector2f(oldScale.x * -1, oldScale.y));
		}
	}
	else if (charA->getData()->isType(DEFENDER) && charB->getData()->isType(DEFENDER))
	{
	}
}

void ContactListener::playerContactEnd(b2Contact* p_contact)
{

}