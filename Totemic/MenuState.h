#pragma once

#include "GameState.h"
#include "dbtweener.h"
#include <SFML\Graphics\RectangleShape.hpp>
#include <Thor\Time.hpp>

class MenuState : public GameState
{
public:
	MenuState();
	~MenuState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();

private:
	bool m_timerActive;
	thor::Timer m_timer;
	thor::Animator<sf::Sprite, std::string> m_animatorHead;
	thor::Animator<sf::Sprite, std::string> m_animatorPressStartText;
	thor::Animator<sf::Sprite, std::string> m_defenderAnimator;
	thor::Animator<sf::Sprite, std::string> m_gathererAnimator;
	sf::Sprite m_pressStartText;
	sf::Sprite m_background;
	sf::Sprite m_foreground;
	sf::Sprite m_logo;
	sf::Sprite m_head;
	sf::Sprite m_maskFrame;
	sf::Sprite m_defender;
	sf::Sprite m_gatherer;
	thor::FrameAnimation m_headAnimation;
	thor::FrameAnimation m_playerIdleAnimation;
	thor::FrameAnimation m_playerReadyAnimation;
	thor::FrameAnimation m_fireAnimation;
	thor::FrameAnimation m_pressStartAnimation;
	thor::FrameAnimation m_defenderAnimation;
	thor::FrameAnimation m_gathererAnimation;

	sf::Text m_timerText;
};

