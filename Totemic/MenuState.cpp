#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <Thor\Animation.hpp>
#include <Thor\Math.hpp>
#include <iostream>

#include "MenuState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"
#include "PlayState.h"
#include "LevelEditorState.h"
#include "GameStateAssets.h"
#include "Config.h"
#include "LoadingState.h"

MenuState::MenuState()
{
}

MenuState::~MenuState()
{
	
}

void MenuState::entering()
{
	if (sf::Joystick::isConnected(0))
	{
		m_stateAsset->gameStateManager->m_useJoysticks = true;
	}

	m_exclusive = false;
	m_timerActive = false;

	int middle_y = m_stateAsset->windowManager->getWindow()->getSize().y / 2.f;
	int middle_x = m_stateAsset->windowManager->getWindow()->getSize().x / 2.f;

	m_playerIdleAnimation.addFrame(1.f, sf::IntRect(0, 0, 200, 256));

	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(200, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(400, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(600, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(800, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(1000, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(1200, 0, 200, 256));
	m_playerReadyAnimation.addFrame(1.f, sf::IntRect(1400, 0, 200, 256));

	m_fireAnimation.addFrame(1.f, sf::IntRect(0, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(192, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(384, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(576, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(768, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(960, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(1152, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(1344, 0, 192, 256));
	m_fireAnimation.addFrame(1.f, sf::IntRect(1536, 0, 192, 256));

	int width = 944;
	int height = 88;
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(0, 0, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 1, 0, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 2, 0, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 3, 0, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 0, 88, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 1, 88, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 2, 88, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 3, 88, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 0, 176, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 1, 176, width, height));
	m_pressStartAnimation.addFrame(1.f, sf::IntRect(width * 2, 176, width, height));
	m_pressStartAnimation.addFrame(8.f, sf::IntRect(width * 3, 176, width, height));

	m_animatorPressStartText.addAnimation("idle", m_pressStartAnimation, sf::seconds(2.f));
	m_animatorPressStartText.playAnimation("idle", true);

	std::string textures[4];
	textures[0] = "menu_blue_anim.png";
	textures[1] = "menu_red_anim.png";
	textures[2] = "menu_yellow_anim.png";
	textures[3] = "menu_purple_anim.png";

	std::string key_ready[4];
	key_ready[0] = "up_p1";
	key_ready[1] = "up_p2";
	key_ready[2] = "up_p3";
	key_ready[3] = "up_p4";

	int spaceBetween = 210;
	int start_x = middle_x - spaceBetween * 2.f + spaceBetween / 2.f;
	for (int i = 0; i < 4; i++)
	{
		PlayerMenu p;
		p.m_ready = false;
		p.m_animator.addAnimation("idle", m_playerIdleAnimation, sf::seconds(1.f));
		p.m_animator.addAnimation("ready", m_playerReadyAnimation, sf::seconds(1.f));
		p.m_fireAnimator.addAnimation("idle", m_fireAnimation, sf::seconds(1.f));
		p.m_animator.playAnimation("idle");
		p.m_fireAnimator.playAnimation("idle", true);
		p.m_sprite.setTexture(m_stateAsset->resourceHolder->getTexture(textures[i]));
		p.m_sprite.setOrigin(100, 128);
		p.m_sprite.setPosition(start_x, middle_y + 140);
		p.m_fireSprite.setTexture(m_stateAsset->resourceHolder->getTexture("menu_fire.png"));
		p.m_fireSprite.setOrigin(96, 128);
		p.m_fireSprite.setPosition(p.m_sprite.getPosition());
		p.m_key_ready = key_ready[i];
		p.m_readyText.setCharacterSize(24);
		p.m_readyText.setFont(m_stateAsset->resourceHolder->getFont("lithospro.otf"));
		p.m_readyText.setString("READY");
		p.m_readyText.setOrigin(p.m_readyText.getGlobalBounds().width / 2.f, p.m_readyText.getGlobalBounds().height / 2.f);
		p.m_readyText.setPosition(start_x, middle_y + 310);
		m_stateAsset->gameStateManager->m_players.push_back(p);
		start_x += spaceBetween;
	}

	m_headAnimation.addFrame(1.f, sf::IntRect(0, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(96, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(192, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(288, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(384, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(480, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(576, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(672, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(768, 0, 96, 96));
	m_headAnimation.addFrame(1.f, sf::IntRect(864, 0, 96, 96));

	m_animatorHead.addAnimation("head", m_headAnimation, sf::seconds(1.2f));
	m_animatorHead.playAnimation("head", true);
	m_head.setTexture(m_stateAsset->resourceHolder->getTexture("menu_head_anim.png"));
	m_head.setPosition(middle_x - 76, m_logo.getPosition().y + 54);

	m_background.setTexture(m_stateAsset->resourceHolder->getTexture("menu_background.png"));
	m_foreground.setTexture(m_stateAsset->resourceHolder->getTexture("menu_foreground.png"));
	
	m_logo.setTexture(m_stateAsset->resourceHolder->getTexture("menu_logo.png"));
	m_logo.setOrigin(m_logo.getGlobalBounds().width / 2.f, 0);
	m_logo.setPosition(middle_x, 95);

	m_maskFrame.setTexture(m_stateAsset->resourceHolder->getTexture("menu_frame.png"));
	m_maskFrame.setOrigin(m_maskFrame.getGlobalBounds().width / 2.f, 0);
	m_maskFrame.setPosition(middle_x, m_logo.getPosition().y + m_logo.getGlobalBounds().height + 64);

	m_timerText.setFont(m_stateAsset->resourceHolder->getFont("lithospro.otf"));
	m_timerText.setCharacterSize(40);
	m_timerText.setColor(sf::Color::White);
	m_timerText.setPosition(middle_x, middle_y + 300);

	m_pressStartText.setPosition(middle_x, middle_y + 450);
	m_pressStartText.setTexture(m_stateAsset->resourceHolder->getTexture("menu_text.png"));
	m_pressStartText.setOrigin(472, 88);

	std::string defender_textures[4];
	defender_textures[0] = "menu_defender_blue.png";
	defender_textures[1] = "menu_defender_red.png";
	defender_textures[2] = "menu_defender_yellow.png";
	defender_textures[3] = "menu_defender_purple.png";

	std::string gatherer_textures[4];
	gatherer_textures[0] = "menu_gatherer_blue.png";
	gatherer_textures[1] = "menu_gatherer_red.png";
	gatherer_textures[2] = "menu_gatherer_yellow.png";
	gatherer_textures[3] = "menu_gatherer_purple.png";

	int randomIndex = thor::random(0, 3);
	int size = 256;
	m_defenderAnimation.addFrame(1.f, sf::IntRect(0, 0, size, size));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(257, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(514, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(771, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(1028, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(1285, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(1542, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(1799, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(2056, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(2313, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(2570, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(2827, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(3084, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(3341, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(3598, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(3855, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(4112, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(4369, 0, 256, 256));
	m_defenderAnimation.addFrame(1.f, sf::IntRect(4626, 0, 256, 256));
	
	size = 128;
	m_gathererAnimation.addFrame(1.f, sf::IntRect(0, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 1, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 2, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 3, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 4, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 5, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 6, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 7, 0, size, size));
	m_gathererAnimation.addFrame(1.f, sf::IntRect(size * 8, 0, size, size));

	m_defenderAnimator.addAnimation("idle", m_defenderAnimation, sf::seconds(1.f));
	m_gathererAnimator.addAnimation("idle", m_gathererAnimation, sf::seconds(0.8f));

	m_defenderAnimator.playAnimation("idle", true);
	m_gathererAnimator.playAnimation("idle", true);

	m_defender.setTexture(m_stateAsset->resourceHolder->getTexture(defender_textures[randomIndex]));
	m_gatherer.setTexture(m_stateAsset->resourceHolder->getTexture(gatherer_textures[randomIndex]));
	m_defender.setOrigin(128, 128);
	m_gatherer.setOrigin(64, 64);
	m_defender.setPosition(100, 945);
	m_gatherer.setPosition(1820, 970);

	m_defender.setScale(-1, 1.f);
}

void MenuState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;
	std::cout << "Leaving menu state" << std::endl;
}

void MenuState::obscuring()
{
}

void MenuState::releaving()
{
}

bool MenuState::update(float dt)
{
	if (getActionMap()->isActive("level_editor"))
	{
		m_stateAsset->gameStateManager->changeState(new LevelEditorState());
		return true;
	}

	m_defenderAnimator.update(sf::seconds(dt));
	m_defenderAnimator.animate(m_defender);

	m_gathererAnimator.update(sf::seconds(dt));
	m_gathererAnimator.animate(m_gatherer);

	m_animatorHead.update(sf::seconds(dt));
	m_animatorHead.animate(m_head);
	m_animatorPressStartText.update(sf::seconds(dt));
	m_animatorPressStartText.animate(m_pressStartText);

	int playerReadyAmount = 0;
	for (auto &player : m_stateAsset->gameStateManager->m_players)
	{
		if (player.m_ready)
		{
			playerReadyAmount++;
		}

		player.m_animator.update(sf::seconds(dt));
		player.m_fireAnimator.update(sf::seconds(dt));
		player.m_animator.animate(player.m_sprite);
		player.m_fireAnimator.animate(player.m_fireSprite);
	}

	if ((playerReadyAmount == 4 || m_timer.isExpired()) && m_timerActive)
	{
		m_stateAsset->gameStateManager->changeState(new LoadingState());
		return true;
	}

	if (playerReadyAmount >= 2 && !m_timer.isRunning())
	{
		m_timerActive = true;
		m_timer.restart(sf::seconds(MENU_TIME_READY));
	}

	if (!m_timer.isExpired())
	{
		m_timerText.setString(std::to_string(static_cast<int>(m_timer.getRemainingTime().asSeconds())));
	}

	m_timerText.setOrigin(m_timerText.getGlobalBounds().width / 2.f, m_timerText.getGlobalBounds().height / 2.f);

	for (auto &player : m_stateAsset->gameStateManager->m_players)
	{
		if (getActionMap()->isActive(player.m_key_ready) && !player.m_ready)
		{
			player.m_animator.playAnimation("ready", true);
			m_stateAsset->audioSystem->playSound("Player_Ready");
			player.m_ready = true;
		}
	}

	return true;
}

void MenuState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(m_background);
	m_stateAsset->windowManager->getWindow()->draw(m_foreground);
	m_stateAsset->windowManager->getWindow()->draw(m_logo);
	m_stateAsset->windowManager->getWindow()->draw(m_head);
	m_stateAsset->windowManager->getWindow()->draw(m_maskFrame);

	for (auto &player : m_stateAsset->gameStateManager->m_players)
	{
		if (player.m_ready)
		{
			m_stateAsset->windowManager->getWindow()->draw(player.m_readyText);
			m_stateAsset->windowManager->getWindow()->draw(player.m_fireSprite);
		}
		m_stateAsset->windowManager->getWindow()->draw(player.m_sprite);
	}

	m_stateAsset->windowManager->getWindow()->draw(m_pressStartText);
	m_stateAsset->windowManager->getWindow()->draw(m_timerText);

	m_stateAsset->windowManager->getWindow()->draw(m_defender);
	m_stateAsset->windowManager->getWindow()->draw(m_gatherer);
}

void MenuState::setupActions()
{
	m_actionMap->operator[]("up_p2") = thor::Action(sf::Keyboard::W, thor::Action::Hold) || thor::Action(thor::JoystickButton::JoystickButton(1, 0));
	m_actionMap->operator[]("up_p4") = thor::Action(sf::Keyboard::Up, thor::Action::Hold) || thor::Action(thor::JoystickButton::JoystickButton(3, 0));
	m_actionMap->operator[]("up_p1") = thor::Action(sf::Keyboard::Y, thor::Action::Hold) || thor::Action(thor::JoystickButton::JoystickButton(0, 0));
	m_actionMap->operator[]("up_p3") = thor::Action(sf::Keyboard::Numpad8, thor::Action::Hold) || thor::Action(thor::JoystickButton::JoystickButton(2, 0));
	m_actionMap->operator[]("level_editor") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
}
