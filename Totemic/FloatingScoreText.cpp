#include "FloatingScoreText.h"
#include "Config.h"
#include "Math.h"
#include <SFML\Graphics\Text.hpp>

#include <iostream>

FloatingScoreText::FloatingScoreText()
{
	m_dead = false;
	m_text = new sf::Text();
	m_text->setString("Doodlemeat 2014");
	m_text->setColor(sf::Color::White);
}


FloatingScoreText::~FloatingScoreText()
{
	delete m_text;
	m_text = nullptr;
}
void FloatingScoreText::setScore(float score)
{
	float font_size = FLOATING_SCORE_TEXT_POINTS_PER_FONTSIZE * score;
	font_size = Math::clamp(font_size, FLOATING_SCORE_TEXT_MAX_SIZE, FLOATING_SCORE_TEXT_MIN_SIZE);
	m_text->setCharacterSize(static_cast<unsigned int>(font_size));
}

sf::Text* FloatingScoreText::getText()
{
	return m_text;
}