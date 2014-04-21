#include "ContactFilter.h"
#include "UserData.h"
#include "Player.h"

bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	const b2Body& bodyA = *fixtureA->GetBody();
	const b2Body& bodyB = *fixtureB->GetBody();
	
	if (bodyA.GetUserData() != nullptr && bodyB.GetUserData() != nullptr)
	{
		UserData* userDataA = static_cast<UserData*>(bodyA.GetUserData());
		UserData* userDataB = static_cast<UserData*>(bodyB.GetUserData());

		if (userDataA->isType(UserDataType::PLAYER) && userDataB->isType(UserDataType::PLAYER))
		{
			PlayerUD* playerDataA = static_cast<PlayerUD*>(userDataA);
			PlayerUD* playerDataB = static_cast<PlayerUD*>(userDataB);

			// Gatherers should clip through each other
			if (playerDataA->getData()->isType(GATHERER) && playerDataB->getData()->isType(GATHERER))
			{
				return false;
			}
		}
	}
	return true;
}