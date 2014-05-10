#pragma once

#include <Thor\Animation\FrameAnimation.hpp>
#include <Thor\Animation\Animator.hpp>

namespace sf
{
	class Sprite;
}
class Player;

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

public:
	int m_coinSpawnIndex;

private:
	bool m_gathered;
	Player* m_playerGathered;
	sf::Sprite* m_sprite;
	thor::Animator<sf::Sprite, std::string>* m_animator;
	thor::FrameAnimation m_animation;
};

