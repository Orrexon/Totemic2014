#include "TotemParticle.h"


TotemParticle::TotemParticle()
{
	fadeInAlpha = 0.f;
	borned = false;
	m_sprite = new sf::Sprite();
}


TotemParticle::~TotemParticle()
{
	delete m_sprite;
	m_sprite = nullptr;
}