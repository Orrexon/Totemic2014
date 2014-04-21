#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>
#include <Windows.h>
#include <Thor\Input.hpp>
#include <iostream>
#include <tchar.h>

#include "Engine.h"
#include "GameStateManager.h"
#include "MenuState.h"
#include "ResourceHolder.h"
#include "WindowManager.h"
#include "FileSystem.h"
#include "Config.h"

Engine::Engine()
{
	m_running = true;
	m_windowManager = nullptr;
	m_actionMap = nullptr;
	m_resourceHolder = nullptr;
	m_config = nullptr;
}

Engine::~Engine()
{
}

bool Engine::init(std::string p_title)
{
	m_config = new Config();
	m_config->setRoot(FileSystem::getCurrentDirectory());

	m_gameStateManager = new GameStateManager();
	m_gameStateManager->pushState(new MenuState());

	m_windowManager = new WindowManager(p_title);
	m_windowManager->setWindowIcon("../resources/icon.png");

	m_actionMap = new thor::ActionMap<std::string>();
	m_actionMap->operator[]("quit") = thor::Action(sf::Event::Closed);
	m_actionMap->operator[]("lost_focus") = thor::Action(sf::Event::LostFocus);
	m_actionMap->operator[]("gained_focus") = thor::Action(sf::Event::GainedFocus);

	m_resourceHolder = new ResourceHolder();

	m_gameStateManager->getStateAsset()->windowManager = m_windowManager;
	m_gameStateManager->getStateAsset()->actionMap = m_actionMap;
	m_gameStateManager->getStateAsset()->resourceHolder = m_resourceHolder;
	m_gameStateManager->getStateAsset()->config = m_config;

	return true;
}

void Engine::loop()
{
	while (m_running)
	{
		updateDeltaTime();
		updateEvents();
		
		if (m_actionMap->isActive("lost_focus") && !m_windowManager->postFocus())
		{
			m_windowManager->setFocus(false);
		}
		else if (m_actionMap->isActive("gained_focus"))
		{
			m_windowManager->setPostFocus(false);
			m_windowManager->setFocus(true);
		}

		if (!m_gameStateManager->updateActiveStates(0.016f))
		{
			m_running = false;
			break;
		}

		m_windowManager->getWindow()->clear();
		m_gameStateManager->draw();
		m_windowManager->getWindow()->display();
	}
	exit();
}

void Engine::updateDeltaTime()
{

}

void Engine::updateEvents()
{
	m_actionMap->clearEvents();
	m_gameStateManager->clearEvents();

	sf::Event event;
	while (m_windowManager->getWindow()->pollEvent(event))
	{
		sf::Event localEvent = event;
		m_actionMap->pushEvent(event);
		m_gameStateManager->pushEvents(localEvent);
	}
}

void Engine::exit()
{
	delete m_gameStateManager;
	m_gameStateManager = nullptr;

	delete m_windowManager;
	m_windowManager = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_resourceHolder;
	m_resourceHolder = nullptr;
}

WindowManager* Engine::getWindowManager()
{
	return m_windowManager;
}