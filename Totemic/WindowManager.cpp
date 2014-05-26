#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include "System.hpp"
#include "WindowManager.h"
#include "FileSystem.h"

WindowManager::WindowManager(std::string p_title)
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	m_inFocus = true;
	m_postFocus = false;
	m_window = new sf::RenderWindow(sf::VideoMode(1920, 1080), p_title, sf::Style::None, settings);
	m_window->setVerticalSyncEnabled(true);
	m_window->setKeyRepeatEnabled(false);
	m_window->setActive(true);
	m_window->setMouseCursorVisible(false);
}

WindowManager::~WindowManager()
{
}

void WindowManager::setWindowIcon(std::string p_filepath)
{
	sf::Image image;
	if (image.loadFromFile(p_filepath))
	{
		m_window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
	}
}

sf::RenderWindow* WindowManager::getWindow()
{
	return m_window;
}

std::string WindowManager::browseFile(OPENFILEINFO &ofi)
{
	std::string currentDir = FileSystem::getCurrentDirectory();
	auto filename = CreateZeroed<CHAR, 65536>();
	auto open_filename = CreateZeroed<OPENFILENAMEA>();

	open_filename.lStructSize = sizeof(OPENFILENAMEA);
	//open_filename.lpstrFilter = "All files(*.*)\0*.*\0";
	open_filename.lpstrFilter = ofi.filters;
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = ofi.caption.c_str();
	open_filename.hwndOwner = m_window->getSystemHandle();
	open_filename.Flags = ofi.flags;

	auto result = GetOpenFileNameA(&open_filename);

	if (!result)
	{
		return "";
	}
	SetCurrentDirectoryA(currentDir.c_str());
	return std::string(filename.data());
}

std::string WindowManager::saveFile(OPENFILEINFO &ofi)
{
	std::string currentDir = FileSystem::getCurrentDirectory();
	auto filename = CreateZeroed<CHAR, 255>();
	auto open_filename = CreateZeroed<OPENFILENAMEA>();

	open_filename.lStructSize = sizeof(OPENFILENAMEA);
	open_filename.lpstrFilter = "All files(*.*)\0*.*\0";
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = ofi.caption.c_str();
	open_filename.hwndOwner = m_window->getSystemHandle();
	open_filename.Flags = ofi.flags;

	auto result = GetSaveFileNameA(&open_filename);

	if (!result)
	{
		return "";
	}
	SetCurrentDirectoryA(currentDir.c_str());
	return std::string(filename.data());
}

void WindowManager::setMenu(HMENU menu)
{
	SetMenu(m_window->getSystemHandle(), menu);
}

void WindowManager::setFocus(bool p_value)
{
	m_inFocus = p_value;
}

bool WindowManager::isInFocus()
{
	return m_inFocus;
}

bool WindowManager::postFocus()
{
	return m_postFocus;
}

void WindowManager::setPostFocus(bool p_value)
{
	m_postFocus = p_value;
}