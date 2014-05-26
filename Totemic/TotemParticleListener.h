#pragma once

#include "dbtweener.h"

class TotemParticleListener : public CDBTweener::IListener
{
public:
	void onTweenFinished(CDBTweener::CTween *pTween);
};
