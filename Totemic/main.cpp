#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <windows.h>

#include "Engine.h"

int main(int argc, char *argv[])
{
	Engine engine;
	engine.init("Submarine Fever");
	engine.loop();

	return 0;
}

#if defined(_WIN32)

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	return main(__argc, __argv);
}

#endif