#pragma once

#include "dbtweener.h"

class TotemTweenerListener : public CDBTweener::IListener
{
public:
	void onTweenFinished(CDBTweener::CTween *pTween);
};
