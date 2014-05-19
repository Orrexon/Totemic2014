#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <Thor\Animation.hpp>
#include <iostream>
#include "GameStateAssets.h"

#include "GameState.h"
#include "LoadingState.h"
#include "PlayState.h"

LoadingState::LoadingState()
{
}

LoadingState::~LoadingState()
{

}

void LoadingState::entering()
{
	m_exclusive = false;
	
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
	m_stateAsset->gameStateManager->changeState(new PlayState());
	return true;
}

void LoadingState::draw()
{
	
}

void LoadingState::setupActions()
{
	
}
