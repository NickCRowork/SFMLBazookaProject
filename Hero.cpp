#include "Hero.h"

Hero::Hero()
{

}

Hero::~Hero()
{

}

void Hero::init(std::string textureName, int frameCount, float animDuration, sf::Vector2f position, float mass)
{
	m_position = position;
	m_mass = mass;

	m_grounded = false;
	m_frameCount = frameCount;
	m_animDuration = animDuration;

	//Load texture
	m_texture.loadFromFile(textureName.c_str());
	// Create Sprite and Attach a Texture
	m_spriteSize = sf::Vector2i(92, 126);
	m_sprite.setTexture(m_texture);
	m_sprite.setTextureRect(sf::IntRect(0, 0, m_spriteSize.x,
		m_spriteSize.y));
	m_sprite.setPosition(m_position);
	m_sprite.setOrigin(m_spriteSize.x / 2, m_spriteSize.y / 2);
	// Create the charge gauge rectangles.
	emptyCharge.setSize(sf::Vector2f(m_spriteSize.x / 2, m_spriteSize.y));
	emptyCharge.setFillColor(sf::Color::Black);
	emptyCharge.setPosition(sf::Vector2f(m_position.x - m_spriteSize.x - 5, m_position.y));
	emptyCharge.setOrigin(sf::Vector2f(emptyCharge.getSize().x / 2, emptyCharge.getSize().y / 2));
	chargeFill.setSize(sf::Vector2f(m_spriteSize.x / 2, (-(float)(m_spriteSize.y))));
	chargeFill.setFillColor(sf::Color::Blue);
	chargeFill.setPosition(sf::Vector2f(m_position.x - m_spriteSize.x - 5, m_position.y));
	chargeFill.setOrigin(sf::Vector2f(chargeFill.getSize().x / 2, chargeFill.getSize().y / 2));
}

void Hero::update(float dt) {
	// Animate Sprite
	m_elapsedTime += dt;
	int animFrame = static_cast<int> ((m_elapsedTime /
		m_animDuration) * m_frameCount) % m_frameCount;
	m_sprite.setTextureRect(sf::IntRect(animFrame * m_spriteSize.x,
		0, m_spriteSize.x, m_spriteSize.y));
	//Prevent elapsed time overflow
	if (animFrame == 0 && m_elapsedTime > m_animDuration * m_frameCount)
	{
		m_elapsedTime = 0;
	}
	//Update position
	m_velocity -= m_mass * m_gravity * dt;
	m_position.y -= m_velocity * dt;
	m_sprite.setPosition(m_position);
	emptyCharge.setPosition(sf::Vector2f(m_position.x - m_spriteSize.x - 5, m_position.y));
	chargeFill.setPosition(sf::Vector2f(m_position.x - m_spriteSize.x - 5, m_position.y));
	if (!canShoot())
	{
		recharging += dt;
		chargeFill.setSize(sf::Vector2f(chargeFill.getSize().x, m_spriteSize.y * (-(recharging/maxRecharge))));
	}
	if (m_position.y >= 768 * 0.75f)
	{
		m_position.y = 768 * 0.75f;
		m_velocity = 0;
		m_grounded = true;
		jumpCount = 0;
	}
}

void Hero::jump(float velocity) 
{
	if (jumpCount < 2) {
		jumpCount++;
		m_velocity = velocity;
		m_grounded = false;
	}
}

void Hero::recharge()
{
	recharging = 0.0f;
}

bool Hero::canShoot()
{
	return recharging >= maxRecharge;
}

sf::Sprite Hero::getSprite() {
	return m_sprite;
}

sf::RectangleShape Hero::getChargeEmpty()
{
	return emptyCharge;
}

sf::RectangleShape Hero::getChargeFill()
{
	return chargeFill;
}