#pragma once

#include "dbtweener.h"

class DeathcloudTweenListener : public CDBTweener::IListener
{
public:
	void onTweenFinished(CDBTweener::CTween *pTween);
};
