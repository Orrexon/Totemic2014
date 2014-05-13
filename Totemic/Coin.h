#pragma once

#include <Thor\Animation\FrameAnimation.hpp>
#include <Thor\Animation\Animator.hpp>

namespace sf
{
	class Sprite;
}
class Player;

enum CoinState
{
	IDLE, // The coin lies on the ground
	SLURPING, // The coin is in the totem, down scaled per frame
	SENDED, // The coin is sent by the coinbird
	GATHERED // The coin is gathered, flying to the totem
};

class Coin
{
public:
	Coin();
	~Coin();

	sf::Sprite *getSprite();
	thor::Animator<sf::Sprite, std::string> *getAnimator();
	thor::FrameAnimation &getAnimation();
	void setGathered(Player* playerGathered);
	bool isGathered();
	Player* getPlayerGathered();
	void setState(CoinState state);
	bool isState(CoinState state);

public:
	int m_coinSpawnIndex;

private:
	CoinState m_state;
	bool m_gathered;
	Player* m_playerGathered;
	sf::Sprite* m_sprite;
	thor::Animator<sf::Sprite, std::string>* m_animator;
	thor::FrameAnimation m_animation;
};

