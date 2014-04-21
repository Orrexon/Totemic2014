#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "MenuState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"
#include "PlayState.h"
#include "LevelEditorState.h"

MenuState::MenuState()
{
	
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
	return true;
}

void MenuState::draw()
{

}

void MenuState::setupActions()
{
	m_actionMap->operator[]("Move_Up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("Move_Down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("Move_Left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("Move_Right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);
}
