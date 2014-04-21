#pragma once

#include <string>
#include <Thor\Resources.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Audio\SoundBuffer.hpp>

class ResourceHolder
{
public:
	ResourceHolder();

	sf::Texture& getTexture(const std::string& p_filename, bool use_defined_path = true);
	sf::Font& getFont(const std::string& p_filename, bool use_defined_path = true);
	sf::SoundBuffer& getSound(const std::string& p_filename, bool use_defined_path = true);

private:
	std::string m_texturePath;
	std::string m_fontPath;
	std::string m_soundPath;

	thor::MultiResourceCache m_cache;
};

