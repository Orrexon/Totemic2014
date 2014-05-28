#pragma once

#include <string>

namespace thor
{
	template <typename T> class ActionMap;
}

struct StateAsset;

class GameState
{
	friend class GameStateManager;
public:
	virtual ~GameState();

	virtual void entering() = 0;
	virtual void leaving() = 0;
	virtual void obscuring() = 0;
	virtual void releaving() = 0;
	virtual bool update(float dt) = 0;
	virtual void draw() = 0;
	virtual void setupActions() = 0;
	void setAssets(StateAsset* p_stateAsset);
	
	thor::ActionMap<std::string>* getActionMap();
	StateAsset* getAssets();
	bool isExclusive();

	StateAsset* m_stateAsset;
protected:
	bool m_exclusive;
	thor::ActionMap<std::string>* m_actionMap;
};

