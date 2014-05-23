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
#include "Audiosystem.h"

Engine::Engine()
{
	m_running = true;
	m_windowManager = nullptr;
	m_actionMap = nullptr;
	m_resourceHolder = nullptr;
}

Engine::~Engine()
{
}

bool Engine::init(std::string p_title)
{
	m_gameStateManager = new GameStateManager();

	m_windowManager = new WindowManager(p_title);
	m_windowManager->setWindowIcon("../resources/icon.png");

	m_actionMap = new thor::ActionMap<std::string>();
	m_actionMap->operator[]("quit") = thor::Action(sf::Event::Closed);
	m_actionMap->operator[]("lost_focus") = thor::Action(sf::Event::LostFocus);
	m_actionMap->operator[]("gained_focus") = thor::Action(sf::Event::GainedFocus);

	m_resourceHolder = new ResourceHolder();

	m_audioSystem = new Audiosystem();
	m_audioSystem->createSound("Lightning", "../assets/sounds/lightning_sfx.ogg");
	m_audioSystem->createSound("Coin_Pickup", "../assets/sounds/crystal.wav");
	m_audioSystem->createSound("Player_Ready", "../assets/sounds/player_ready.wav");
	m_audioSystem->createSound("Coinbird", "../assets/sounds/coinbird.wav");
	m_audioSystem->createSound("321GO", "../assets/sounds/321go.wav");
	m_audioSystem->createSound("NewLeader", "../assets/sounds/new_leader2.ogg");
	m_audioSystem->createMusic("Bamboozle", "../assets/music/bamboozle.wav");
	m_audioSystem->getMusic("Bamboozle")->setVolume(10);

	m_gameStateManager->getStateAsset()->windowManager = m_windowManager;
	m_gameStateManager->getStateAsset()->actionMap = m_actionMap;
	m_gameStateManager->getStateAsset()->resourceHolder = m_resourceHolder;
	m_gameStateManager->getStateAsset()->audioSystem = m_audioSystem;

	m_gameStateManager->pushState(new MenuState());
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

		if (!m_gameStateManager->updateActiveStates(0.01666667f))
		{
			m_running = false;
			break;
		}

		m_windowManager->getWindow()->clear();
		m_gameStateManager->draw();
		m_windowManager->getWindow()->display();
		m_audioSystem->update();
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

	delete m_audioSystem;
	m_audioSystem = nullptr;
}

WindowManager* Engine::getWindowManager()
{
	return m_windowManager;
}