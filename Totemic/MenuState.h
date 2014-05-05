#pragma once

#include "GameState.h"
#include "dbtweener.h"
#include <SFML\Graphics\RectangleShape.hpp>

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
	CDBTweener m_tweener;
	CDBTweener::CTween* m_tween;
	float m_tweenX;
	float m_tweenY;
	float m_tweenPi;
	sf::RectangleShape shape;
};

