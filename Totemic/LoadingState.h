#pragma once

#include "GameState.h"

class LoadingState : public GameState
{
public:
	LoadingState();
	~LoadingState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();
	
private:
};

