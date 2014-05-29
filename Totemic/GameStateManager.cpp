#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "GameStateManager.h"
#include "WindowManager.h"
#include "GameState.h"

GameStateManager::GameStateManager()
{
	m_stateAsset = new StateAsset();
	m_stateAsset->gameStateManager = this;
	m_useJoysticks = false;
}


GameStateManager::~GameStateManager()
{
	delete m_stateAsset;
	m_stateAsset = nullptr;
}

void GameStateManager::changeState(GameState* gameState)
{
	if (!m_activeStates.empty())
	{
		popState();
	}
	pushState(gameState);
}

void GameStateManager::popState()
{
	if (!m_activeStates.empty())
	{
		auto it = --m_activeStates.end();
		(*it)->leaving();
		delete *it;
		m_activeStates.pop_back();
	}

	notifyRevealedStates();
}

void GameStateManager::pushState(GameState* gameState)
{
	gameState->setAssets(m_stateAsset);
	gameState->m_actionMap = new thor::ActionMap<std::string>();
	gameState->setupActions();
	m_activeStates.push_back(gameState);

	notifyObscuredStates();

	gameState->entering();
	gameState->update(0.01666667f);
}

void GameStateManager::notifyRevealedStates()
{
	if (m_activeStates.empty())
	{
		return;
	}

	std::size_t index = m_activeStates.size() - 1;
	while (index > 0)
	{
		if (m_activeStates[index]->isExclusive())
		{
			break;
		}
		index--;
	}

	while (index < m_activeStates.size())
	{
		m_activeStates[index]->releaving();
		index++;
	}
}

void GameStateManager::notifyObscuredStates()
{
	if (m_activeStates.size() < 2)
	{
		return;
	}

	std::size_t index = m_activeStates.size() - 2;
	while (index > 0)
	{
		if (m_activeStates[index]->isExclusive())
		{
			break;
		}
		index--;
	}

	while (index < m_activeStates.size() - 1)
	{
		m_activeStates[index]->obscuring();
		index++;
	}
}

bool GameStateManager::updateActiveStates(float dt)
{
	bool returnValue = true;
	for (unsigned int i = 0; i < m_activeStates.size(); i++)
	{
		if (m_stateAsset->windowManager->isInFocus())
		{
			if (!m_activeStates[i]->update(dt))
			{
				returnValue = false;
			}
		}
	}
	return returnValue;
}

void GameStateManager::draw()
{
	for (auto &state : m_activeStates)
	{
		state->draw();
	}
}

void GameStateManager::pushEvents(sf::Event event)
{
	for (auto &state : m_activeStates)
	{
		sf::Event localEvent = event;
		if (localEvent.type == sf::Event::MenuItemSelected)
		{
			std::cout << "Menu selected event triggered" << std::endl;
		}
		state->getActionMap()->pushEvent(localEvent);
	}
}

void GameStateManager::clearEvents()
{
	for (auto &state : m_activeStates)
	{
		state->getActionMap()->clearEvents();
	}
}

StateAsset* GameStateManager::getStateAsset()
{
	return m_stateAsset;
}