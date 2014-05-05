#pragma comment(lib, "Shlwapi.lib")

#include <Thor\Input.hpp>
#include <Thor\Math.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <json\json.h>
#include <fstream>
#include <Shlwapi.h>
#include <fstream>
#include <regex>

#include "GameStateAssets.h"
#include "LevelEditorState.h"
#include "resource.h"
#include "Math.h"
#include "String.h"
#include "FileSystem.h"

enum
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_DEFAULT,
	ID_FILE_EXIT,
	ID_CREATE_OBJECT,
	ID_LOAD_OBJECT,
	ID_BACKGROUND_CHANGE,
	ID_PLAYER_1,
	ID_PLAYER_2,
	ID_PLAYER_3,
	ID_PLAYER_4,
	ID_PLAYER_1_GATHERER,
	ID_PLAYER_1_DEFENDER,
	ID_PLAYER_2_GATHERER,
	ID_PLAYER_2_DEFENDER,
	ID_PLAYER_3_GATHERER,
	ID_PLAYER_3_DEFENDER,
	ID_PLAYER_4_GATHERER,
	ID_PLAYER_4_DEFENDER,
	ID_HOTSPOT_IMAGE,
	ID_OBJECT_LOAD,
	ID_NEW_POWERUP,
	ID_NEW_TRAP,
	ID_NEW_COIN,
	ID_NEW_IMAGEOBJECT
};

enum
{
	PLAYER_DEFENDER,
	PLAYER_GATHERER
};

enum
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4
};

const std::string DEFAULT_BACKGROUND = "../assets/textures/backgrounds/default.png";
const std::string DEFAULT_HOTSPOT = "../assets/textures/hotspot.psd";
const int DEFENDER_RADIUS = 30;
const int GATHERER_RADIUS = 15;
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam);

LevelEditorState::LevelEditorState()
{

}

LevelEditorState::~LevelEditorState()
{

}

void LevelEditorState::entering()
{
	sf::Vector2u windowSize = m_stateAsset->windowManager->getWindow()->getSize();
	m_dialogEditHotspot = nullptr;

	m_placingHotspot = false;
	m_placingPlayer = false;
	m_placingObject = false;
	m_placingPowerup = false;
	m_placingTrap = false;
	m_placingCoin = false;
	m_placingImageObject = false;
	m_exit = false;
	m_scrollingScreen = false;

	m_placingPlayerIndex = 0;
	m_placingPlayerChartype = 0;
	
	std::vector<sf::Color> colors;
	colors.push_back(sf::Color::Red);
	colors.push_back(sf::Color::Green);
	colors.push_back(sf::Color::Yellow);
	colors.push_back(sf::Color::Blue);

	std::vector<sf::Vector2f> defender_positions;
	defender_positions.push_back(sf::Vector2f(100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, windowSize.y - 100));
	defender_positions.push_back(sf::Vector2f(100, windowSize.y - 100));

	std::vector<sf::Vector2f> gatherer_positions;
	gatherer_positions.push_back(sf::Vector2f(50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, windowSize.y - 50));
	gatherer_positions.push_back(sf::Vector2f(50, windowSize.y - 50));

	for (int i = 0; i < 4; i++)
	{
		PlayerStruct* player = new PlayerStruct();
		player->defender = new sf::CircleShape();
		player->defender->setFillColor(colors[i]);
		player->defender->setPosition(defender_positions[i]);
		player->defender->setOrigin(DEFENDER_RADIUS, DEFENDER_RADIUS);
		player->defender->setRadius(DEFENDER_RADIUS);

		player->gatherer = new sf::CircleShape();
		player->gatherer->setFillColor(colors[i]);
		player->gatherer->setPosition(gatherer_positions[i]);
		player->gatherer->setOrigin(GATHERER_RADIUS, GATHERER_RADIUS);
		player->gatherer->setRadius(GATHERER_RADIUS);

		m_players.push_back(player);
	}

	m_backgroundPath = DEFAULT_BACKGROUND;
	m_backgroundSprite = new sf::Sprite();
	m_backgroundTexture = new sf::Texture();
	m_backgroundTexture->loadFromFile(DEFAULT_BACKGROUND);
	m_backgroundSprite->setTexture(*m_backgroundTexture);

	m_hotspotPath = DEFAULT_HOTSPOT;
	m_hotspotSprite = new sf::Sprite();
	m_hotspotSprite->setTexture(m_stateAsset->resourceHolder->getTexture(m_hotspotPath, false));
	m_hotspotSprite->setOrigin(m_hotspotSprite->getGlobalBounds().width / 2.f, m_hotspotSprite->getGlobalBounds().height / 2.f);
	m_hotspotSprite->setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);

	m_hotSpotShape.setRadius(200);
	m_hotSpotShape.setOrigin(m_hotSpotShape.getRadius(), m_hotSpotShape.getRadius());
	m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
	m_hotSpotShape.setFillColor(sf::Color(100, 160, 100, 255));

	m_levelView = new sf::View();
	m_levelView->setSize(sf::Vector2f(windowSize.x, windowSize.y));
	m_levelView->setCenter(windowSize.x / 2, windowSize.y / 2);
	
	m_jsonRoot = new Json::Value();
	m_exclusive = false;

	std::cout << "Entering level editor state" << std::endl;

	HMENU menuBar = CreateMenu();
	HMENU hFileDropDown = CreateMenu();
	HMENU hObjectDropDown = CreateMenu();
	HMENU hBackgroundDropDown = CreateMenu();
	HMENU hPlayerDropDown = CreatePopupMenu();
	HMENU hHotSpotDropDown = CreateMenu();
	HMENU hMiscDropDown = CreateMenu();
	HMENU hPlayer1Sub = CreatePopupMenu();
	HMENU hPlayer2Sub = CreatePopupMenu();
	HMENU hPlayer3Sub = CreatePopupMenu();
	HMENU hPlayer4Sub = CreatePopupMenu();


	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hFileDropDown, "File");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hObjectDropDown, "Objects");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hBackgroundDropDown, "Background");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hPlayerDropDown, "Player");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hHotSpotDropDown, "HotSpot");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hMiscDropDown, "Misc");

	AppendMenuA(hMiscDropDown, MF_STRING | MF_POPUP, ID_NEW_POWERUP, "New powerup");
	AppendMenuA(hMiscDropDown, MF_STRING | MF_POPUP, ID_NEW_TRAP, "New trap");
	AppendMenuA(hMiscDropDown, MF_STRING | MF_POPUP, ID_NEW_COIN, "New coin");
	AppendMenuA(hMiscDropDown, MF_STRING | MF_POPUP, ID_NEW_IMAGEOBJECT, "New image object");

	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer1Sub, "Player 1");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer2Sub, "Player 2");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer3Sub, "Player 3");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer4Sub, "Player 4");

	AppendMenuA(hPlayer1Sub, MF_STRING | MF_POPUP, ID_PLAYER_1_GATHERER, "Gatherer");
	AppendMenuA(hPlayer1Sub, MF_STRING | MF_POPUP, ID_PLAYER_1_DEFENDER, "Defender");
	AppendMenuA(hPlayer2Sub, MF_STRING | MF_POPUP, ID_PLAYER_2_GATHERER, "Gatherer");
	AppendMenuA(hPlayer2Sub, MF_STRING | MF_POPUP, ID_PLAYER_2_DEFENDER, "Defender");
	AppendMenuA(hPlayer3Sub, MF_STRING | MF_POPUP, ID_PLAYER_3_GATHERER, "Gatherer");
	AppendMenuA(hPlayer3Sub, MF_STRING | MF_POPUP, ID_PLAYER_3_DEFENDER, "Defender");
	AppendMenuA(hPlayer4Sub, MF_STRING | MF_POPUP, ID_PLAYER_4_GATHERER, "Gatherer");
	AppendMenuA(hPlayer4Sub, MF_STRING | MF_POPUP, ID_PLAYER_4_DEFENDER, "Defender");

	AppendMenuA(hHotSpotDropDown, MF_STRING, ID_HOTSPOT_IMAGE, "Choose image");
	
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_NEW, "New level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_OPEN, "Open level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_SAVE, "Save level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_DEFAULT, "Set default\tCTRL+D");
	AppendMenuA(hFileDropDown, MF_SEPARATOR | MF_BYPOSITION, 0, nullptr);
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_EXIT, "Exit");

	AppendMenuA(hObjectDropDown, MF_STRING, ID_LOAD_OBJECT, "Load object");

	AppendMenuA(hBackgroundDropDown, MF_STRING, ID_BACKGROUND_CHANGE, "Change background");

	HBITMAP p1_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP1));
	HBITMAP p2_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP2));
	HBITMAP p3_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP3));
	HBITMAP p4_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP4));
	
	DWORD dLastError = GetLastError();
	LPCTSTR strErrorMessage = NULL;

	SetMenuItemBitmaps(hPlayerDropDown, 0, MF_BYPOSITION, p1_icon, p1_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 1, MF_BYPOSITION, p2_icon, p2_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 2, MF_BYPOSITION, p3_icon, p3_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 3, MF_BYPOSITION, p4_icon, p4_icon);

	RECT rect;
	GetClientRect(m_stateAsset->windowManager->getWindow()->getSystemHandle(), &rect);
	
	m_levelView->setSize(sf::Vector2f(1920, 1055));
	
	m_stateAsset->windowManager->setMenu(menuBar);

	m_system = new thor::ActionMap<std::string>::CallbackSystem;
	m_system->connect("MenuSelected", std::bind(&LevelEditorState::handleMenuSelected, this, std::placeholders::_1));
}

void LevelEditorState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_system;
	m_system = nullptr;

	delete m_backgroundSprite;
	m_backgroundSprite = nullptr;

	delete m_backgroundTexture;
	m_backgroundTexture = nullptr;

	delete m_levelView;
	m_levelView = nullptr;

	std::cout << "Leaving level editor state" << std::endl;
}

void LevelEditorState::obscuring()
{
}

void LevelEditorState::releaving()
{
}

bool LevelEditorState::update(float dt)
{
	sf::Vector2f current_mouse_pos = m_stateAsset->windowManager->getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_stateAsset->windowManager->getWindow()));
	sf::Vector2i local_mouse_pos = sf::Mouse::getPosition(*m_stateAsset->windowManager->getWindow());
	getActionMap()->invokeCallbacks(*m_system, nullptr);
	
	if (m_exit)
	{
		return false;
	}

	bool isSnapping = false;
	sf::Vector2f snapCord;
	if (m_actionMap->isActive("Snap"))
	{
		isSnapping = true;
		snapCord.x = Math::roundByMultiple(current_mouse_pos.x, 64.f);
		snapCord.y = Math::roundByMultiple(current_mouse_pos.y, 32.f);
	}

	if (getActionMap()->isActive("Drag"))
	{
		if (!m_scrollingScreen)
		{
			m_scrollHandlePosition = local_mouse_pos;
			m_scrollingScreen = !m_scrollingScreen;
			m_defaultCenter = m_levelView->getCenter();
		}
	}
	else
	{
		m_scrollingScreen = false;
	}

	if (m_scrollingScreen)
	{
		// Get direction of draggin
		float radians = std::atan2f(local_mouse_pos.y - m_scrollHandlePosition.y, local_mouse_pos.x - m_scrollHandlePosition.x);
		float dir_x_r = std::cosf(radians);
		float dir_y_r = std::sinf(radians);


		// Get draggin distance
		float distance = std::sqrtf(
			(local_mouse_pos.x - m_scrollHandlePosition.x) * (local_mouse_pos.x - m_scrollHandlePosition.x) +
			(local_mouse_pos.y - m_scrollHandlePosition.y) * (local_mouse_pos.y - m_scrollHandlePosition.y));
		
		m_levelView->setCenter(m_defaultCenter + sf::Vector2f(distance * -dir_x_r, distance * -dir_y_r));

		// Cap the view to not cross borders of the background
		sf::Vector2f max(m_backgroundSprite->getGlobalBounds().width, m_backgroundSprite->getGlobalBounds().height);
		sf::Vector2f center = m_levelView->getCenter();
		sf::Vector2f size = m_levelView->getSize();
		sf::Vector2f newCenter = center;
		if (center.x - size.x / 2 <= 0)
		{
			newCenter.x = size.x / 2;
		}
		else if (center.x + size.x / 2 >= max.x)
		{
			newCenter.x = max.x - size.x / 2;
		}
		if (center.y - size.y / 2 <= 0)
		{
			newCenter.y = size.y / 2;
		}
		else if (center.y + size.y / 2 >= max.y)
		{
			newCenter.y = max.y - size.y / 2;
		}
		m_levelView->setCenter(newCenter);
	}

	if (m_placingImageObject)
	{
		sf::Vector2f newPosition = current_mouse_pos;
		if (isSnapping)
		{
			newPosition = snapCord;
		}
		m_imageObjects[m_placingImageObjectIndex].position = newPosition;
		m_imageObjects[m_placingImageObjectIndex].sprite.setPosition(newPosition);

		if (getActionMap()->isActive("Place"))
		{
			m_placingImageObject = false;
		}
		if (getActionMap()->isActive("Delete"))
		{
			m_imageObjects.erase(m_imageObjects.begin() + m_placingImageObjectIndex);
			m_placingImageObject = false;
		}
	}
	else if (m_placingCoin)
	{
		sf::Vector2f newPosition = current_mouse_pos;
		if (isSnapping)
		{
			newPosition = snapCord;
		}
		m_coins[m_placingCoinIndex].position = newPosition;

		if (getActionMap()->isActive("Place"))
		{
			m_placingCoin = false;
		}
		if (getActionMap()->isActive("Delete"))
		{
			m_coins.erase(m_coins.begin() + m_placingCoinIndex);
			m_placingCoin = false;
		}
	}
	else if (m_placingTrap)
	{
		sf::Vector2f newPosition = current_mouse_pos;
		if (isSnapping)
		{
			newPosition = snapCord;
		}
		m_traps[m_placingTrapIndex].position = newPosition;

		if (getActionMap()->isActive("Place"))
		{
			m_placingTrap = false;
		}
		if (getActionMap()->isActive("Delete"))
		{
			m_traps.erase(m_traps.begin() + m_placingTrapIndex);
			m_placingTrap = false;
		}
	}
	else if (m_placingPowerup)
	{
		sf::Vector2f newPosition = current_mouse_pos;
		if (isSnapping)
		{
			newPosition = snapCord;
		}
		m_powerups[m_placingPowerupIndex].position = newPosition;

		if (getActionMap()->isActive("Place"))
		{
			m_placingPowerup = false;
		}
		if (getActionMap()->isActive("Delete"))
		{
			m_powerups.erase(m_powerups.begin() + m_placingPowerupIndex);
			m_powerups.shrink_to_fit();
			m_placingPowerup = false;
		}
	}
	else if (m_placingObject)
	{
		sf::Vector2f newPosition;
		newPosition.x = current_mouse_pos.x - objects[m_placingObjectIndex].sprite->getGlobalBounds().width / 2.f;
		newPosition.y = current_mouse_pos.y - objects[m_placingObjectIndex].sprite->getGlobalBounds().height / 2.f;

		if (isSnapping)
		{
			newPosition.x = snapCord.x - objects[m_placingObjectIndex].sprite->getGlobalBounds().width / 2.f;
			newPosition.y = snapCord.y - objects[m_placingObjectIndex].sprite->getGlobalBounds().height / 2.f;
		}

		objects[m_placingObjectIndex].sprite->setPosition(newPosition);
		if (getActionMap()->isActive("Place"))
		{
			m_placingObject = false;
		}
		else if (getActionMap()->isActive("Delete"))
		{
			m_placingObject = false;
			objects.erase(objects.begin() + m_placingObjectIndex);
			objects.shrink_to_fit();
		}
	}
	// Changing hotspot position
	else if (m_placingHotspot)
	{
		if (isSnapping)
		{
			m_hotSpotShape.setPosition(snapCord);
		}
		else if (getActionMap()->isActive("Center"))
		{
			m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
		}
		else
		{
			m_hotSpotShape.setPosition(current_mouse_pos);
		}

		sf::Color color = m_hotSpotShape.getFillColor();
		color.a = 150;
		m_hotSpotShape.setFillColor(color);
		

		if (getActionMap()->isActive("Place"))
		{
			m_placingHotspot = false;
			sf::Color color = m_hotSpotShape.getFillColor();
			color.a = 255;
			m_hotSpotShape.setFillColor(color);
		}
	}
	// Changing player position
	else if (m_placingPlayer)
	{
		sf::Vector2f position = current_mouse_pos;
		if (isSnapping)
		{
			position = snapCord;
		}
		if (m_placingPlayerChartype == PLAYER_DEFENDER)
		{
			m_players[m_placingPlayerIndex]->defender->setPosition(position);
		}
		else if (m_placingPlayerChartype == PLAYER_GATHERER)
		{
			m_players[m_placingPlayerIndex]->gatherer->setPosition(position);
		}

		if (getActionMap()->isActive("Place"))
		{
			m_placingPlayer = false;
		}
	}
	else
	{
		// Click on an entity
		if (getActionMap()->isActive("Place"))
		{
			bool clicked = false;
			for (unsigned int i = 0; i < m_players.size(); i++)
			{
				if (Math::pointInCircle(current_mouse_pos, m_players[i]->defender->getPosition(), m_players[i]->defender->getRadius()))
				{
					m_placingPlayer = true;
					m_placingPlayerIndex = i;
					m_placingPlayerChartype = PLAYER_DEFENDER;
					clicked = true;
				}
				else if (Math::pointInCircle(current_mouse_pos, m_players[i]->gatherer->getPosition(), m_players[i]->gatherer->getRadius()))
				{
					m_placingPlayer = true;
					m_placingPlayerIndex = i;
					m_placingPlayerChartype = PLAYER_GATHERER;
					clicked = true;
				}
			}

			// Clicking on hotspot
			if (!clicked && Math::pointInCircle(current_mouse_pos, m_hotSpotShape.getPosition(), m_hotSpotShape.getRadius()))
			{
				clicked = true;
				m_placingHotspot = true;
			}

			// Clicking on an object
			if (!clicked)
			{
				sf::Vector2f point = current_mouse_pos;
				for (int k = 0; k < objects.size(); k++) {
					int nvert = objects[k].points.size() - 1;
					bool inside = false;
					for (unsigned int i = 0, j = nvert - 1; i < nvert; j = i++)
					{
						const sf::Vector2f &vert1 = objects[k].sprite->getPosition() + objects[k].points[i];
						const sf::Vector2f &vert2 = objects[k].sprite->getPosition() + objects[k].points[j];

						if (
							((vert1.y > point.y) != (vert2.y > point.y)) &&
							(point.x < ((vert2.x - vert1.x) * (point.y - vert1.y) / (vert2.y - vert1.y) + vert1.x)))
						{
							inside = !inside;
						}
					}
					if (inside)
					{
						m_placingObject = true;
						m_placingObjectIndex = k;
						break;
					}
				}
			}

			if (!clicked)
			{
				for (std::size_t i = 0; i < m_powerups.size(); i++)
				{
					if (Math::pointInCircle(current_mouse_pos, m_powerups[i].position, 10))
					{
						m_placingPowerup = true;
						m_placingPowerupIndex = i;
						break;
					}
				}
			}

			if (!clicked)
			{
				for (std::size_t i = 0; i < m_traps.size(); i++)
				{
					if (Math::pointInCircle(current_mouse_pos, m_traps[i].position, 10))
					{
						m_placingTrap = true;
						m_placingTrapIndex = i;
						break;
					}
				}
			}
			
			if (!clicked)
			{
				for (std::size_t i = 0; i < m_coins.size(); i++)
				{
					if (Math::pointInCircle(current_mouse_pos, m_coins[i].position, 10))
					{
						m_placingCoin = true;
						m_placingCoinIndex = i;
						break;
					}
				}
			}

			if (!clicked)
			{
				for (std::size_t i = 0; i < m_imageObjects.size(); i++)
				{

					if (m_imageObjects[i].sprite.getGlobalBounds().contains(current_mouse_pos))
					{
						m_placingImageObject = true;
						m_placingImageObjectIndex = i;
						break;
					}
				}
			}
		}
	}

	if (getActionMap()->isActive("Set_Default"))
	{
		setDefault();
	}

	if (getActionMap()->isActive("Exit"))
	{
		return false;
	}
	return true;
}

void LevelEditorState::draw()
{
	m_stateAsset->windowManager->getWindow()->setView(*m_levelView);
	m_stateAsset->windowManager->getWindow()->draw(*m_backgroundSprite);
	m_stateAsset->windowManager->getWindow()->draw(*m_hotspotSprite);
	m_stateAsset->windowManager->getWindow()->draw(m_hotSpotShape);
	
	for (auto &player: m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->gatherer);
		m_stateAsset->windowManager->getWindow()->draw(*player->defender);
	}

	for (auto &obj : objects)
	{
		m_stateAsset->windowManager->getWindow()->draw(*obj.sprite);
	}

	for (auto &powerup : m_powerups)
	{
		sf::CircleShape shape;
		shape.setRadius(10);
		shape.setOrigin(10, 10);
		shape.setPosition(powerup.position);
		shape.setFillColor(sf::Color::Red);
		m_stateAsset->windowManager->getWindow()->draw(shape);
	}

	for (auto &trap : m_traps)
	{
		sf::CircleShape shape;
		shape.setRadius(10);
		shape.setOrigin(10, 10);
		shape.setPosition(trap.position);
		shape.setFillColor(sf::Color::Green);
		m_stateAsset->windowManager->getWindow()->draw(shape);
	}

	for (auto &coin : m_coins)
	{
		sf::CircleShape shape;
		shape.setRadius(10);
		shape.setOrigin(10, 10);
		shape.setPosition(coin.position);
		shape.setFillColor(sf::Color::Yellow);
		m_stateAsset->windowManager->getWindow()->draw(shape);
	}

	for (auto &imageObject : m_imageObjects)
	{
		m_stateAsset->windowManager->getWindow()->draw(imageObject.sprite);
	}

	sf::Sprite sprite;
	sprite.setTexture(m_stateAsset->resourceHolder->getTexture("grid.png"));
	m_stateAsset->windowManager->getWindow()->draw(sprite);
}

void LevelEditorState::setupActions()
{
	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	m_actionMap->operator[]("MenuSelected") = thor::Action(sf::Event::MenuItemSelected);
	m_actionMap->operator[]("Drag") = thor::Action(sf::Keyboard::Space, thor::Action::Hold);
	m_actionMap->operator[]("Place") = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
	m_actionMap->operator[]("Center") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold);
	m_actionMap->operator[]("Set_Default") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold) && thor::Action(sf::Keyboard::D, thor::Action::PressOnce);
	m_actionMap->operator[]("Create_Object") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold) && thor::Action(sf::Keyboard::B, thor::Action::PressOnce);
	m_actionMap->operator[]("Delete") = thor::Action(sf::Keyboard::Delete, thor::Action::PressOnce) || thor::Action(sf::Keyboard::BackSpace, thor::Action::PressOnce);
	m_actionMap->operator[]("Snap") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
}

void LevelEditorState::openFile()
{
	OPENFILEINFO ofi;
	ofi.filters = "Level files\0*.level";
	ofi.caption = "Choose level file";
	std::string openpath = m_stateAsset->windowManager->browseFile(ofi);

	if (!openpath.empty())
	{
		std::ifstream istream;
		istream.open(openpath, std::ifstream::binary);
		if (!istream.is_open())
		{
			std::cout << "Failed to load level: Can't open file" << std::endl;
			return;
		}

		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(istream, root, false))
		{
			std::cout << "Failed to parse level " << openpath << " with message: " << reader.getFormatedErrorMessages() << std::endl;
			return;
		}

		m_backgroundPath = root["background"]["path"].asString();
		if (m_backgroundTexture->loadFromFile(m_backgroundPath))
		{
			m_backgroundSprite->setTexture(*m_backgroundTexture, true);
		}

		for (auto it = root["players"].begin(); it != root["players"].end(); ++it)
		{
			m_players[it.index()]->defender->setPosition(sf::Vector2f((*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble()));
			m_players[it.index()]->gatherer->setPosition(sf::Vector2f((*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble()));
		}

		objects.clear();
		for (auto it = root["objects"].begin(); it != root["objects"].end(); ++it)
		{
			loadObjectRaw((*it)["path"].asString(), sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
		}

		m_powerups.clear();
		for (auto it = root["powerups"].begin(); it != root["powerups"].end(); ++it)
		{
			PowerupStruct powerup;
			powerup.position = sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble());
			m_powerups.push_back(powerup);
		}

		m_traps.clear();
		for (auto it = root["traps"].begin(); it != root["traps"].end(); ++it)
		{
			TrapStruct trap;
			trap.position = sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble());
			m_traps.push_back(trap);
		}

		m_coins.clear();
		for (auto it = root["coins"].begin(); it != root["coins"].end(); ++it)
		{
			CoinStruct coin;
			coin.position = sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble());
			m_coins.push_back(coin);
		}

		m_imageObjects.clear();
		for (auto it = root["image_objects"].begin(); it != root["image_objects"].end(); ++it)
		{
			ImageObject imageObject;
			imageObject.position = sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble());
			imageObject.image_path = (*it)["image_path"].asString();
			imageObject.sprite.setTexture(m_stateAsset->resourceHolder->getTexture(imageObject.image_path, false));
			imageObject.sprite.setOrigin(imageObject.sprite.getGlobalBounds().width / 2.f, imageObject.sprite.getGlobalBounds().height / 2.f);
			imageObject.sprite.setPosition(imageObject.position);
			m_imageObjects.push_back(imageObject);
		}
	}
}

void LevelEditorState::saveFile()
{
	OPENFILEINFO ofi;
	ofi.caption = "Save level";
	std::string savepath = m_stateAsset->windowManager->saveFile(ofi);
	if (!savepath.empty())
	{
		Json::Value rootNode;
		Json::Value backgroundNode;
		Json::Value playersNode;
		Json::Value objectsNode;
		Json::Value hotspotNode;
		Json::Value powerupsNode;
		Json::Value trapsNode;
		Json::Value coinsNode;
		Json::Value imageObjectsNode;

		for (int i = 0; i < 4; i++)
		{
			Json::Value playerNode;
			playerNode["defender_position_x"] = m_players[i]->defender->getPosition().x;
			playerNode["defender_position_y"] = m_players[i]->defender->getPosition().y;
			playerNode["gatherer_position_x"] = m_players[i]->gatherer->getPosition().x;
			playerNode["gatherer_position_y"] = m_players[i]->gatherer->getPosition().y;
			playersNode.append(playerNode);
		}

		for (int i = 0; i < objects.size(); i++)
		{
			Json::Value objectNode;
			objectNode["path"] = objects[i].object_path;
			objectNode["x"] = static_cast<int>(objects[i].sprite->getPosition().x);
			objectNode["y"] = static_cast<int>(objects[i].sprite->getPosition().y);
			objectsNode.append(objectNode);
		}

		for (std::size_t i = 0; i < m_powerups.size(); i++)
		{
			Json::Value powerup;
			powerup["x"] = static_cast<int>(m_powerups[i].position.x);
			powerup["y"] = static_cast<int>(m_powerups[i].position.y);
			powerupsNode.append(powerup);
		}

		for (std::size_t i = 0; i < m_traps.size(); i++)
		{
			Json::Value trap;
			trap["x"] = static_cast<int>(m_traps[i].position.x);
			trap["y"] = static_cast<int>(m_traps[i].position.y);
			trapsNode.append(trap);
		}

		for (std::size_t i = 0; i < m_coins.size(); i++)
		{
			Json::Value coin;
			coin["x"] = static_cast<int>(m_coins[i].position.x);
			coin["y"] = static_cast<int>(m_coins[i].position.y);
			coinsNode.append(coin);
		}

		for (std::size_t i = 0; i < m_imageObjects.size(); i++)
		{
			Json::Value imageObject;
			imageObject["image_path"] = m_imageObjects[i].image_path;
			imageObject["x"] = static_cast<int>(m_imageObjects[i].position.x);
			imageObject["y"] = static_cast<int>(m_imageObjects[i].position.y);
			imageObjectsNode.append(imageObject);
		}

		hotspotNode["radius"] = m_hotSpotShape.getRadius();
		Json::Value hotSpotPositionNode;
		hotSpotPositionNode["x"] = m_hotSpotShape.getPosition().x;
		hotSpotPositionNode["y"] = m_hotSpotShape.getPosition().y;
		hotspotNode["position"] = hotSpotPositionNode;
		hotspotNode["image"] = m_hotspotPath;

		backgroundNode["path"] = m_backgroundPath;

		rootNode["objects"] = objectsNode;
		rootNode["background"] = backgroundNode;
		rootNode["players"] = playersNode;
		rootNode["hotspot"] = hotspotNode;
		rootNode["powerups"] = powerupsNode;
		rootNode["traps"] = trapsNode;
		rootNode["coins"] = coinsNode;
		rootNode["image_objects"] = imageObjectsNode;

		Json::StyledWriter writer;
		std::string output = writer.write(rootNode);
		std::ofstream ofs;
		ofs.open(savepath);
		if (ofs.is_open())
		{
			ofs << output;
			ofs.close();
			std::string message = "Level was saved in " + savepath;
			MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), "Success", message.c_str(), 0);
		}
	}
}

void LevelEditorState::newFile()
{

}

void LevelEditorState::exitFile()
{
	m_exit = true;
}

void LevelEditorState::changeBackground()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose background image";
	ofi.filters = "PNG files\0*.png";
	std::string backgroundFile = m_stateAsset->windowManager->browseFile(ofi);
	if (!backgroundFile.empty())
	{
		if (m_backgroundTexture->loadFromFile(backgroundFile))
		{
			m_backgroundPath = FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str() , backgroundFile.c_str());
			m_backgroundSprite->setTexture(*m_backgroundTexture, true);
		}
		else
		{
			std::cout << "Image could not be loaded: " << backgroundFile << std::endl;
		}
	}
}

void LevelEditorState::handlePlayer(int player_id, int char_id)
{
	m_placingPlayer = true;
	m_placingPlayerChartype = char_id;
	m_placingPlayerIndex = player_id;
}

void LevelEditorState::setDefault()
{
	sf::Vector2u windowSize = m_stateAsset->windowManager->getWindow()->getSize();

	std::vector<sf::Color> colors;
	colors.push_back(sf::Color::Red);
	colors.push_back(sf::Color::Green);
	colors.push_back(sf::Color::Yellow);
	colors.push_back(sf::Color::Blue);

	std::vector<sf::Vector2f> defender_positions;
	defender_positions.push_back(sf::Vector2f(100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, windowSize.y - 100));
	defender_positions.push_back(sf::Vector2f(100, windowSize.y - 100));

	std::vector<sf::Vector2f> gatherer_positions;
	gatherer_positions.push_back(sf::Vector2f(50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, windowSize.y - 50));
	gatherer_positions.push_back(sf::Vector2f(50, windowSize.y - 50));

	
	for (auto &player : m_players)
	{
		delete player->defender;
		delete player->gatherer;
		player->defender = nullptr;
		player->gatherer = nullptr;
	}
	m_players.clear();

	for (int i = 0; i < 4; i++)
	{
		PlayerStruct* player = new PlayerStruct();
		player->defender = new sf::CircleShape();
		player->defender->setFillColor(colors[i]);
		player->defender->setPosition(defender_positions[i]);
		player->defender->setOrigin(DEFENDER_RADIUS, DEFENDER_RADIUS);
		player->defender->setRadius(DEFENDER_RADIUS);

		player->gatherer = new sf::CircleShape();
		player->gatherer->setFillColor(colors[i]);
		player->gatherer->setPosition(gatherer_positions[i]);
		player->gatherer->setOrigin(GATHERER_RADIUS, GATHERER_RADIUS);
		player->gatherer->setRadius(GATHERER_RADIUS);

		m_players.push_back(player);
	}
	
	if (m_backgroundSprite != nullptr)
	{
		delete m_backgroundSprite;
		m_backgroundSprite = nullptr;
	}

	if (m_backgroundTexture != nullptr)
	{
		delete m_backgroundTexture;
		m_backgroundTexture = nullptr;
	}


	m_backgroundPath = DEFAULT_BACKGROUND;
	m_backgroundSprite = new sf::Sprite();
	m_backgroundTexture = new sf::Texture();
	m_backgroundTexture->loadFromFile(DEFAULT_BACKGROUND);
	m_backgroundSprite->setTexture(*m_backgroundTexture);

	m_hotSpotShape.setRadius(100);
	m_hotSpotShape.setOrigin(m_hotSpotShape.getRadius(), m_hotSpotShape.getRadius());
	m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
	m_hotSpotShape.setFillColor(sf::Color(100, 160, 100, 255));
}

void LevelEditorState::loadObject()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose background image";
	ofi.filters = "Object files\0*.objx";
	std::string objectFile = m_stateAsset->windowManager->browseFile(ofi);
	if (!objectFile.empty())
	{
		std::cout << "ObjectFile: " << objectFile << std::endl;
		std::cout << "GetCurrentPath: " << FileSystem::getCurrentDirectory() << std::endl;
		std::string path = FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), objectFile.c_str());
		loadObjectRaw(String::replace(path, "\\\\", "/"), sf::Vector2f(400, 400));
	}
	else
	{
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), "Failed to load object file. A file was not selected.", "Failed to load object", 0);
	}
}

void LevelEditorState::changeHotspotImage()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose hotspot image";
	ofi.filters = "PNG files\0*.png";
	std::string hotspotFile = m_stateAsset->windowManager->browseFile(ofi);
	if (!hotspotFile.empty())
	{
		m_hotspotPath = String::replace(FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), hotspotFile.c_str()), "\\\\", "/");
		m_hotspotSprite->setTexture(m_stateAsset->resourceHolder->getTexture(false));
	}
}

void LevelEditorState::newPowerup()
{
	PowerupStruct powerup;
	m_powerups.push_back(powerup);
	m_placingPowerup = true;
	m_placingPowerupIndex = m_powerups.size() - 1;
}

void LevelEditorState::newTrap()
{
	TrapStruct trap;
	m_traps.push_back(trap);
	m_placingTrap = true;
	m_placingTrapIndex = m_traps.size() - 1;
}

void LevelEditorState::newCoin()
{
	CoinStruct coin;
	m_coins.push_back(coin);
	m_placingCoin = true;
	m_placingCoinIndex = m_coins.size() - 1;
}

void LevelEditorState::newImageObject()
{
	ImageObject imageObject;
	OPENFILEINFO ofi;
	ofi.caption = "Choose image";
	ofi.filters = "PNG files\0*.png";
	std::string imagePath = m_stateAsset->windowManager->browseFile(ofi);
	if (!imagePath.empty())
	{
		std::string path = String::replace(FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), imagePath.c_str()), "\\\\", "/");
		imageObject.image_path = path;
		imageObject.sprite.setTexture(m_stateAsset->resourceHolder->getTexture(imageObject.image_path, false));
		imageObject.sprite.setOrigin(imageObject.sprite.getGlobalBounds().width / 2.f, imageObject.sprite.getGlobalBounds().height / 2.f);
		m_imageObjects.push_back(imageObject);
		m_placingImageObject = true;
		m_placingImageObjectIndex = m_imageObjects.size() - 1;
	}
}

void LevelEditorState::handleMenuSelected(thor::ActionContext<std::string> context)
{
	m_stateAsset->windowManager->setPostFocus(true);
	sf::Event event = *context.event;
	switch (event.menuAction.identifier)
	{
	case ID_FILE_SAVE:
		saveFile();
		break;
	case ID_FILE_OPEN:
		openFile();
		break;
	case ID_FILE_NEW:
		newFile();
		break;
	case ID_FILE_EXIT:
		exitFile();
		break;
	case ID_BACKGROUND_CHANGE:
		changeBackground();
		break;

	case ID_PLAYER_1_DEFENDER: handlePlayer(PLAYER_1, PLAYER_DEFENDER); break;
	case ID_PLAYER_1_GATHERER: handlePlayer(PLAYER_1, PLAYER_GATHERER); break;
	case ID_PLAYER_2_DEFENDER: handlePlayer(PLAYER_2, PLAYER_DEFENDER); break;
	case ID_PLAYER_2_GATHERER: handlePlayer(PLAYER_2, PLAYER_GATHERER); break;
	case ID_PLAYER_3_DEFENDER: handlePlayer(PLAYER_3, PLAYER_DEFENDER); break;
	case ID_PLAYER_3_GATHERER: handlePlayer(PLAYER_3, PLAYER_GATHERER); break;
	case ID_PLAYER_4_DEFENDER: handlePlayer(PLAYER_4, PLAYER_DEFENDER); break;
	case ID_PLAYER_4_GATHERER: handlePlayer(PLAYER_4, PLAYER_GATHERER); break;

	case ID_DEFAULT:
		setDefault();
		break;
	case ID_LOAD_OBJECT:
		loadObject();
		break;
	case ID_HOTSPOT_IMAGE:
		changeHotspotImage();
		break;
	case ID_NEW_POWERUP:
		newPowerup();
		break;
	case ID_NEW_TRAP:
		newTrap();
		break;
	case ID_NEW_COIN:
		newCoin();
		break;
	case ID_NEW_IMAGEOBJECT:
		newImageObject();
		break;
	default:
		break;
	}
}

void LevelEditorState::loadObjectRaw(std::string objectFile, sf::Vector2f position)
{
	std::ifstream istream;
	istream.open(objectFile, std::ifstream::binary);
	if (!istream.is_open())
	{
		std::string content = "Failed to load object file: " + objectFile;
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), content.c_str(), "Failed to load object", 0);
		return;
	}

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(istream, root, false))
	{
		std::string content = "Failed to parse object file: " + objectFile;
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), content.c_str(), "Failed to parse object", 0);
		return;
	}

	bool parseError = false;

	// object data
	bool use_circle = false;
	sf::Vector2f circle_position(0, 0);
	float circle_radius = 0;
	std::string image_path;
	std::vector<sf::Vector2f> points;

	if (root["use_circle"].asBool())
	{
		use_circle = true;
		circle_position.x = root["circle"]["position"]["x"].asInt();
		circle_position.y = root["circle"]["position"]["y"].asInt();
		circle_radius = root["circle"]["radius"].asInt();

	}
			
	if (!root["points"].isNull() && root["points"].size() > 1)
	{
		for (auto it = root["points"].begin(); it != root["points"].end(); ++it)
		{
			sf::Vector2f point;
			point.x = (*it)["x"].asInt();
			point.y = (*it)["y"].asInt();
			points.push_back(point);
		}
	}

	image_path = root["image"]["path"].asString();

	Object obj;
	obj.circle_position = circle_position;
	obj.circle_radius = circle_radius;
	obj.image_path = image_path;
	obj.object_path = objectFile;
	obj.use_circle_collision = use_circle;
	obj.points = points;
	obj.sprite = new sf::Sprite();
	obj.sprite->setTexture(m_stateAsset->resourceHolder->getTexture(obj.image_path, false));
	obj.sprite->setPosition(position);
	objects.push_back(obj);
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		ShowWindow(hwnd, SW_SHOW);
		break;
	}
	return false;
}