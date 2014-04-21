#include "TotemTweenerListener.h"
#include "Player.h"

void TotemTweenerListener::onTweenFinished(CDBTweener::CTween *pTween)
{
	Player* player = static_cast<Player*>(pTween->getUserData());
	player->setChangingOrder(false);
	player->getTotemSprite()->setPosition(player->getTotemSprite()->getPosition().x, player->m_tweeningValue);
}
