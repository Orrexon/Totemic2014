#include "GameState.h"
#include "GameStateManager.h"

GameState::~GameState()
{

}

void GameState::setAssets(StateAsset* p_stateAsset)
{
	m_stateAsset = p_stateAsset;
}

bool GameState::isExclusive()
{
	return m_exclusive;
}

thor::ActionMap<std::string>* GameState::getActionMap()
{
	return m_actionMap;
}