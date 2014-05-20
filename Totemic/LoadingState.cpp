#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <Thor\Animation.hpp>
#include <iostream>
#include "GameStateAssets.h"

#include "GameState.h"
#include "LoadingState.h"
#include "PlayState.h"
#include "Config.h"

LoadingState::LoadingState()
{
}

LoadingState::~LoadingState()
{

}

void LoadingState::entering()
{
	m_exclusive = false;
	
	mBackground.setTexture(m_stateAsset->resourceHolder->getTexture("loadingscreen.png"));
	mTimer.reset(sf::seconds(LOADINGSCREEN_SECONDS));
	mTimer.start();
}

void LoadingState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;
}

void LoadingState::obscuring()
{
}

void LoadingState::releaving()
{
}

bool LoadingState::update(float dt)
{
	if (m_actionMap->isActive("start"))
	{
		m_stateAsset->gameStateManager->changeState(new PlayState());
		return true;
	}
	if (mTimer.isExpired())
	{
		m_stateAsset->gameStateManager->changeState(new PlayState());
	}
	return true;
}

void LoadingState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(mBackground);
}

void LoadingState::setupActions()
{
	m_actionMap->operator[]("start") = thor::Action(sf::Keyboard::R);
}
