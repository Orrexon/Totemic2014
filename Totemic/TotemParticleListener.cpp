#include "TotemParticleListener.h"
#include "TotemParticle.h"
#include "Config.h"
#include <iostream>

void TotemParticleListener::onTweenFinished(CDBTweener::CTween *pTween)
{
	if (pTween->getUserData() != nullptr)
	{
		TotemParticle* tp = static_cast<TotemParticle*>(pTween->getUserData());
		tp->deathTimer.restart(sf::seconds(TOTEM_PARTICLE_SECONDS_ALIVE));
		tp->borned = true;
	}
}
