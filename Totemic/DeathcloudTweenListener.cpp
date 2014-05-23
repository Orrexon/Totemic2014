#include "DeathcloudTweenListener.h"
#include "Deathcloud.h"
#include "Config.h"
#include <iostream>
void DeathcloudTweenListener::onTweenFinished(CDBTweener::CTween *pTween)
{
	if (pTween->getUserData() != nullptr)
	{
		DeathCloud* dc = static_cast<DeathCloud*>(pTween->getUserData());
		if (!dc->borned)
		{
			dc->timer.restart(sf::seconds(DEATHCLOUD_SECONDS_ALIVE));
			dc->borned = true;
		}
		else
		{
			dc->dead = true;
		}
	}
}
