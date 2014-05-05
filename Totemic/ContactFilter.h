#pragma once

#include <Box2D\Dynamics\b2WorldCallbacks.h>
#include <Box2D\Dynamics\b2Fixture.h>

enum FilterCategories
{
	CAT_DEFENDER = 0x0001,
	CAT_GATHERER = 0x0002
};

class ContactFilter: public b2ContactFilter
{
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

