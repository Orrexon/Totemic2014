#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "MenuState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"
#include "PlayState.h"
#include "LevelEditorState.h"
#include "GameStateAssets.h"

MenuState::MenuState()
{
	/*m_tweenX = 1920.f / 2.f;
	m_tweenY = 500;
	m_tweenPi = 0.f;

	shape.setFillColor(sf::Color::Red);
	shape.setSize(sf::Vector2f(128, 128));
	shape.setOrigin(64, 64);
	shape.setPosition(1920.f / 2.f, m_tweenY);

	m_tween = new CDBTweener::CTween();
	m_tween->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 3.f);
	m_tween->addValue(&m_tweenY, m_tweenY - 300);
	m_tweener.addTween(m_tween);

	CDBTweener::CTween* tweenX = new CDBTweener::CTween();
	tweenX->setEquation(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 3.f);
	tweenX->addValue(&m_tweenPi, 3.14159265359);
	m_tweener.addTween(tweenX);*/
}

MenuState::~MenuState()
{
	
}

void MenuState::entering()
{
	m_exclusive = false;
	std::cout << "Entering menu state" << std::endl;
	std::cout << "W - Play Game" << std::endl;
	std::cout << "S - Level editor" << std::endl;

	Sound* snd = m_stateAsset->audioSystem->getSound("Lightning");
	if (snd != nullptr)
	{
		int count = snd->getBuffer().getSampleCount();
		const sf::Int16* samples = snd->getBuffer().getSamples();
		//for (std::size_t i = 0; i < count; i++)
		//{
		//	std::cout << samples[i] << std::endl;
		//}
	}
}

void MenuState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;
	std::cout << "Leaving menu state" << std::endl;
}

void MenuState::obscuring()
{
}

void MenuState::releaving()
{
}

bool MenuState::update(float dt)
{
	if (getActionMap()->isActive("Move_Up"))
	{
		m_stateAsset->gameStateManager->changeState(new PlayState());
	}
	else if (getActionMap()->isActive("Move_Down"))
	{
		m_stateAsset->gameStateManager->changeState(new LevelEditorState());
	}
	else if (getActionMap()->isActive("Move_Left"))
	{
		return false;
	}
	/*std::cout << m_tweenPi << std::endl;
	shape.setPosition((1920.f / 2.f) + std::sin(m_tweenPi) * 300, m_tweenY);
	m_tweener.step(dt);
	*/
	return true;
}

void MenuState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(shape);
}

void MenuState::setupActions()
{
	m_actionMap->operator[]("Move_Up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("Move_Down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("Move_Left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("Move_Right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);
}
