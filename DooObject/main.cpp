#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>
#include <SFML\Audio.hpp>
#include <Thor\Animation.hpp>
#include <Thor\Config.hpp>
#include <Thor\Graphics.hpp>
#include <Thor\Input.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <ctype.h>

#include "WindowManager.h"
#include "Math.h"
#include "json\json.h"
#include "FileSystem.h"
#include "String.h"

enum
{
	ID_NEW,
	ID_SAVE,
	ID_LOAD_IMAGE,
	ID_DRAW_CIRCLE,
	ID_DRAW_LINES
};

struct App
{
	void create()
	{
		music.openFromFile("../assets/music/music_objectWindow.wav");
		music.play();
		music.setLoop(true);
		windowManager = new WindowManager("Create object - Lingonstyle");
		open = true;
		draw_lines = true;
		drags = false;
		startedDraw = false;
		circle_radius = 20;
		circle_shape.setRadius(20);
		circle_shape.setOrigin(20, 20);
		circle_shape.setFillColor(sf::Color(0, 0, 0, 0));
		circle_shape.setOutlineThickness(1);
		circle_shape.setOutlineColor(sf::Color::Magenta);
		HMENU menuBar = CreateMenu();
		HMENU hFileDropDown = CreateMenu();

		AppendMenuA(hFileDropDown, MF_STRING, ID_NEW, "New object");
		AppendMenuA(hFileDropDown, MF_STRING, ID_SAVE, "Save object");
		AppendMenuA(hFileDropDown, MF_STRING, ID_LOAD_IMAGE, "Load image");
		AppendMenuA(hFileDropDown, MF_STRING, ID_DRAW_CIRCLE, "Use circles");
		AppendMenuA(hFileDropDown, MF_STRING | MF_CHECKED, ID_DRAW_LINES, "Use lines");
		AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hFileDropDown, "Controls");

		SetMenu(windowManager->getWindow()->getSystemHandle(), menuBar);

		actionMap["closed"] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
		actionMap["MenuSelected"] = thor::Action(sf::Event::MenuItemSelected);
		actionMap["Click"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
		actionMap["Done"] = thor::Action(sf::Mouse::Right, thor::Action::PressOnce);
		actionMap["Reset"] = thor::Action(sf::Keyboard::R, thor::Action::PressOnce);
		actionMap["WheelEvent"] = thor::Action(sf::Event::MouseWheelMoved);
		actionMap["Undo"] = thor::Action(sf::Keyboard::LControl, thor::Action::Hold) && thor::Action(sf::Keyboard::Z, thor::Action::PressOnce);
		system.connect("MenuSelected", std::bind(&App::handleMenu, this, std::placeholders::_1));
		system.connect("WheelEvent", std::bind(&App::handleScroll, this, std::placeholders::_1));
	}

	void handleScroll(thor::ActionContext<std::string> context)
	{
		sf::Event event = *context.event;
		float speed = 1;
		circle_radius += speed * event.mouseWheel.delta;
		circle_shape.setRadius(circle_radius);
		circle_shape.setOrigin(circle_radius, circle_radius);
	}

	void update()
	{
		while (open)
		{
			actionMap.update(*windowManager->getWindow());
			actionMap.invokeCallbacks(system, nullptr);
			if (actionMap.isActive("closed"))
			{
				open = false;
			}

			if (actionMap.isActive("Reset"))
			{
				startedDraw = false;
				points.clear();
			}

			if (actionMap.isActive("Done"))
			{
				startedDraw = false;
			}

			if (actionMap.isActive("Undo"))
			{
				if (points.size() > 0)
				{
					points.pop_back();
					if (points.size() != 0)
					{
						currentPoint = points.back();
					}
					else
					{
						startedDraw = false;
					}
				}
			}

			sf::Vector2i mouse_pos = sf::Mouse::getPosition(*windowManager->getWindow());
			sf::Vector2f real_mouse_pos = windowManager->getWindow()->mapPixelToCoords(mouse_pos);
			if (actionMap.isActive("Click"))
			{
				if (draw_lines)
				{

					// check if clicked on a point
					if (!drags && !startedDraw && points.size() > 1)
					{
						for (int i = 0; i < points.size(); i++)
						{
							if (Math::euclideanDistance(real_mouse_pos, points[i]) < 10)
							{
								drags = true;
								dragging_point = i;
								break;
							}
						}
					}
					else if (!startedDraw && drags)
					{
						drags = false;
					}
					else
					{
						currentPoint = real_mouse_pos;

						if (points.size() > 2)
						{
							if (Math::euclideanDistance(real_mouse_pos, points[0]) < 20)
							{
								currentPoint = points[0];
							}
						}
						points.push_back(currentPoint);
						startedDraw = true;
					}
				}
				else
				{
					circle_shape.setRadius(circle_radius);
					circle_shape.setPosition(real_mouse_pos);
					startedDraw = true;
				}
			}

			windowManager->getWindow()->clear();
			windowManager->getWindow()->draw(sprite);

			if (drags)
			{
				points[dragging_point] = real_mouse_pos;
			}
			if (draw_lines)
			{
				if (startedDraw)
				{
					if (draw_lines)
					{

						if (points.size() > 2)
						{
							if (Math::euclideanDistance(real_mouse_pos, points[0]) < 20)
							{
								real_mouse_pos = points[0];
							}
						}
						sf::Vertex line[] =
						{
							sf::Vertex(sf::Vector2f(currentPoint.x, currentPoint.y)),
							sf::Vertex(sf::Vector2f(real_mouse_pos.x, real_mouse_pos.y))
						};
						windowManager->getWindow()->draw(line, 2, sf::Lines);
					}
					else
					{
					
					}
				}

				if (points.size() >= 2)
				{
					for (int i = 0; i < points.size(); i++) {
						if (i < points.size() - 1)
						{
							sf::Vertex line[] =
							{
								sf::Vertex(sf::Vector2f(points[i].x, points[i].y)),
								sf::Vertex(sf::Vector2f(points[i + 1].x, points[i + 1].y))
							};

							windowManager->getWindow()->draw(line, 2, sf::Lines);
						}
					}
				}

				for (auto &point : points)
				{
					sf::CircleShape shape;
					shape.setRadius(6);
					shape.setFillColor(sf::Color(0, 0, 0, 0));
					shape.setOutlineThickness(1);
					shape.setOutlineColor(sf::Color::Cyan);
					shape.setOrigin(6, 6);
					shape.setPosition(point);
					windowManager->getWindow()->draw(shape);
				}

			}
			else
			{
				windowManager->getWindow()->draw(circle_shape);
			}

			windowManager->getWindow()->display();
		}
	}

	void handleMenu(thor::ActionContext<std::string> context)
	{
		sf::Event event = *context.event;

		switch (event.menuAction.identifier)
		{
		case ID_SAVE:
			saveObject();
			break;
		case ID_LOAD_IMAGE:
			loadImage();
			break;
		case ID_DRAW_CIRCLE:
			draw_lines = false;
			break;
		case ID_DRAW_LINES:
			draw_lines = true;
			break;
		}
	}

	void newObject()
	{

	}

	void saveObject()
	{
		std::string current = FileSystem::getCurrentDirectory();
		OPENFILEINFO ofi;
		ofi.caption = "Save level";
		ofi.filters = "Object files\0*.objx";
		ofi.flags = OFN_EXTENSIONDIFFERENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		std::string savepath = windowManager->saveFile(ofi);
		if (!savepath.empty())
		{
			if (draw_lines && points.size() < 2)
			{
				MessageBoxA(windowManager->getWindow()->getSystemHandle(), "You have to define at least two points in order to save this object.", "Fail", 0);
				return;
			}
			if (texture_path.empty())
			{
				MessageBoxA(windowManager->getWindow()->getSystemHandle(), "Image path is missing. Have you selected an image? If not, do so.", "Fail", 0);
				return;
			}
			Json::Value rootNode;
			Json::Value pointsNode;
			Json::Value imageNode;
			Json::Value circleNode;

			for (int i = 0; i < points.size(); i++)
			{
				Json::Value pointNode;
				pointNode["x"] = static_cast<int>(points[i].x - sprite.getPosition().x);
				pointNode["y"] = static_cast<int>(points[i].y - sprite.getPosition().y);
				pointsNode.append(pointNode);
			}

			Json::Value circlePosition;
			circlePosition["x"] = static_cast<int>(circle_shape.getPosition().x - sprite.getPosition().x);
			circlePosition["y"] = static_cast<int>(circle_shape.getPosition().y - sprite.getPosition().y);
			circleNode["position"] = circlePosition;
			circleNode["radius"] = static_cast<int>(circle_radius);

			imageNode["path"] = texture_path;

			rootNode["points"] = pointsNode;
			rootNode["image"] = imageNode;
			rootNode["use_circle"] = !draw_lines;
			rootNode["circle"] = circleNode;

			Json::StyledWriter writer;
			std::string output = writer.write(rootNode);
			std::ofstream ofs;
			ofs.open(savepath);
			if (ofs.is_open())
			{
				ofs << output;
				ofs.close();
				std::string message = "Object was saved in " + savepath;
				MessageBoxA(windowManager->getWindow()->getSystemHandle(), message.c_str(), "Success", 0);
			}
		}
		SetCurrentDirectoryA(current.c_str());
	}

	void loadImage()
	{
		OPENFILEINFO ofi;
		ofi.caption = "Choose level file";
		ofi.filters = "Images\0*.PNG";
		ofi.flags = OFN_EXTENSIONDIFFERENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		std::string openpath = windowManager->browseFile(ofi);
		std::transform(openpath.begin(), openpath.end(), openpath.begin(), tolower);
		if (!openpath.empty())
		{
			if (texture.loadFromFile(openpath))
			{
				texture_path = FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), openpath.c_str());
				texture_path = String::replace(texture_path, "\\\\", "/");
				sf::Vector2f middle(windowManager->getWindow()->getSize().x / 2.f - texture.getSize().x / 2.f,
									windowManager->getWindow()->getSize().y / 2.f - texture.getSize().y / 2.f);
				sprite.setPosition(middle);
				sprite.setTexture(texture, true);
			}
		}
	}

	WindowManager* windowManager;
	bool open;
	bool draw_lines;
	bool startedDraw;
	bool drags;
	int dragging_point;
	float circle_radius;
	thor::ActionMap<std::string>::CallbackSystem system;
	thor::ActionMap<std::string> actionMap;

	sf::CircleShape circle_shape;
	std::string texture_path;
	sf::Texture texture;
	sf::Sprite sprite;
	std::vector<sf::Vector2f> points;
	sf::Vector2f currentPoint;
	sf::Music music;
	std::string root;
};

int main()
{
	App app;
	app.root = FileSystem::getCurrentDirectory();
	app.create();
	app.update();
	app.windowManager->getWindow()->close();
	return 0;
}