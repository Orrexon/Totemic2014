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

	// Preload assets
	m_resourceHolder = new ResourceHolder();
	m_resourceHolder->getTexture("totem_head.png");
	m_resourceHolder->getTexture("321go.png");
	m_resourceHolder->getTexture("arrowblue.png");
	m_resourceHolder->getTexture("arrowpurple.png");
	m_resourceHolder->getTexture("arrowred.png");
	m_resourceHolder->getTexture("arrowyellow.png");
	m_resourceHolder->getTexture("BarBlue.png");
	m_resourceHolder->getTexture("BarPurple.png");
	m_resourceHolder->getTexture("BarRed.png");
	m_resourceHolder->getTexture("BarYellow.png");
	m_resourceHolder->getTexture("bird_spin.png");
	m_resourceHolder->getTexture("blue_d_death.png");
	m_resourceHolder->getTexture("blueblock.png");
	m_resourceHolder->getTexture("box.png");
	m_resourceHolder->getTexture("box1.png");
	m_resourceHolder->getTexture("box2.png");
	m_resourceHolder->getTexture("box3.png");
	m_resourceHolder->getTexture("broken_pillar_br.png");
	m_resourceHolder->getTexture("broken_pillar_tr.png");
	m_resourceHolder->getTexture("coin.png");
	m_resourceHolder->getTexture("coinbird.png");
	m_resourceHolder->getTexture("death_blue.png");
	m_resourceHolder->getTexture("death_particle_blue.png");
	m_resourceHolder->getTexture("death_particle_purple.png");
	m_resourceHolder->getTexture("death_particle_red.png");
	m_resourceHolder->getTexture("death_particle_yellow.png");
	m_resourceHolder->getTexture("death_purple.png");
	m_resourceHolder->getTexture("death_red.png");
	m_resourceHolder->getTexture("death_yellow.png");
	m_resourceHolder->getTexture("deathcloud.png");
	m_resourceHolder->getTexture("def_blue.png");
	m_resourceHolder->getTexture("def_purple.png");
	m_resourceHolder->getTexture("def_red.png");
	m_resourceHolder->getTexture("def_yellow.png");
	m_resourceHolder->getTexture("defender_collision.png");
	m_resourceHolder->getTexture("fin_box_1.png");
	m_resourceHolder->getTexture("fin_box_2.png");
	m_resourceHolder->getTexture("fin_box_3.png");
	m_resourceHolder->getTexture("fin_pillar_1.png");
	m_resourceHolder->getTexture("fin_ruin_bl_1.png");
	m_resourceHolder->getTexture("fin_ruin_br_3.png");
	m_resourceHolder->getTexture("fin_ruin_tl_2.png");
	m_resourceHolder->getTexture("fin_ruin_tr_4.png");
	m_resourceHolder->getTexture("fin_stone_1.png");
	m_resourceHolder->getTexture("flower.png");
	m_resourceHolder->getTexture("g_blue.png");
	m_resourceHolder->getTexture("g_purple.png");
	m_resourceHolder->getTexture("g_red.png");
	m_resourceHolder->getTexture("g_yellow.png");
	m_resourceHolder->getTexture("grid.png");
	m_resourceHolder->getTexture("guiframe.png");
	m_resourceHolder->getTexture("hotspot.PSD");
	m_resourceHolder->getTexture("lightning.png");
	m_resourceHolder->getTexture("loadingscreen.png");
	m_resourceHolder->getTexture("menu_background.png");
	m_resourceHolder->getTexture("menu_blue_anim.png");
	m_resourceHolder->getTexture("menu_defender_blue.png");
	m_resourceHolder->getTexture("menu_defender_purple.png");
	m_resourceHolder->getTexture("menu_defender_red.png");
	m_resourceHolder->getTexture("menu_defender_yellow.png");
	m_resourceHolder->getTexture("menu_fire.png");
	m_resourceHolder->getTexture("menu_foreground.png");
	m_resourceHolder->getTexture("menu_frame.png");
	m_resourceHolder->getTexture("menu_gatherer_blue.png");
	m_resourceHolder->getTexture("menu_gatherer_purple.png");
	m_resourceHolder->getTexture("menu_gatherer_red.png");
	m_resourceHolder->getTexture("menu_gatherer_yellow.png");
	m_resourceHolder->getTexture("menu_head_anim.png");
	m_resourceHolder->getTexture("menu_logo.png");
	m_resourceHolder->getTexture("menu_purple_anim.png");
	m_resourceHolder->getTexture("menu_red_anim.png");
	m_resourceHolder->getTexture("menu_text.png");
	m_resourceHolder->getTexture("menu_yellow_anim.png");
	m_resourceHolder->getTexture("numbers.png");
	m_resourceHolder->getTexture("particle2.png");
	m_resourceHolder->getTexture("particle3.png");
	m_resourceHolder->getTexture("particles.png");
	m_resourceHolder->getTexture("pillar.png");
	m_resourceHolder->getTexture("pillar1.png");
	m_resourceHolder->getTexture("pillar2.png");
	m_resourceHolder->getTexture("pillar3.png");
	m_resourceHolder->getTexture("plant.png");
	m_resourceHolder->getTexture("plant_1.png");
	m_resourceHolder->getTexture("plant_2.png");
	m_resourceHolder->getTexture("plant_3.png");
	m_resourceHolder->getTexture("pressStartText.png");
	m_resourceHolder->getTexture("purple_d_death.png");
	m_resourceHolder->getTexture("purpleblock.png");
	m_resourceHolder->getTexture("red_d_death.png");
	m_resourceHolder->getTexture("redblock.png");
	m_resourceHolder->getTexture("rock.png");
	m_resourceHolder->getTexture("ruin_bottomleft.png");
	m_resourceHolder->getTexture("ruin_bottomright.png");
	m_resourceHolder->getTexture("ruin_topleft.png");
	m_resourceHolder->getTexture("ruin_topright.png");
	m_resourceHolder->getTexture("shield.png");
	m_resourceHolder->getTexture("shield_animation.png");
	m_resourceHolder->getTexture("sparcle_particles.png");
	m_resourceHolder->getTexture("timer_particle.png");
	m_resourceHolder->getTexture("totem_glitter.png");
	m_resourceHolder->getTexture("totem_glitter_anim.png");
	m_resourceHolder->getTexture("totem_head.png");
	m_resourceHolder->getTexture("totemfoot.png");
	m_resourceHolder->getTexture("totemhead.png");
	m_resourceHolder->getTexture("totempole.png");
	m_resourceHolder->getTexture("trap.png");
	m_resourceHolder->getTexture("tuva.png");
	m_resourceHolder->getTexture("tuva_1.png");
	m_resourceHolder->getTexture("tuva_2.png");
	m_resourceHolder->getTexture("tuva_3.png");
	m_resourceHolder->getTexture("win.png");
	m_resourceHolder->getTexture("yellow_d.png");
	m_resourceHolder->getTexture("yellow_d_death.png");
	m_resourceHolder->getTexture("yellow_g.png");
	m_resourceHolder->getTexture("yellowblock.png");

	m_audioSystem = new Audiosystem();
	m_audioSystem->createSound("Lightning", "../assets/sounds/lightning_sfx.ogg");
	m_audioSystem->createSound("Coin_Pickup", "../assets/sounds/crystal.wav");
	m_audioSystem->createSound("Player_Ready", "../assets/sounds/player_ready.wav");
	m_audioSystem->createSound("Coinbird", "../assets/sounds/coinbird.wav");
	m_audioSystem->createSound("321GO", "../assets/sounds/321go.wav");
	m_audioSystem->createSound("NewLeader", "../assets/sounds/new_leader2.ogg");
	m_audioSystem->createSound("Defender_Hit", "../assets/sounds/hit.ogg");
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