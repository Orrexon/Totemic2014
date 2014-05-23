#include "TotemTweenerListener.h"
#include "Player.h"

void TotemTweenerListener::onTweenFinished(CDBTweener::CTween *pTween)
{
	Player* player = static_cast<Player*>(pTween->getUserData());
	player->setChangingOrder(false);
	sf::Vector2f oldPosition = player->getTotemSprite()->getPosition();
	player->getTotemSprite()->setPosition(oldPosition.x, player->m_tweeningValue);
	player->m_currentTotemTween = nullptr;
	player->m_isTweeningTotem = false;
}
