#pragma once

namespace sf
{
	class Text;
}

class FloatingScoreText
{
public:
	FloatingScoreText();
	~FloatingScoreText();

	void setScore(float score);
	sf::Text* getText();

public:
	bool m_dead;

private:
	sf::Text *m_text;
	float m_alpha;
};

