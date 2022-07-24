#pragma once
#include ".\SFML-2.5.1\include\SFML\Graphics.hpp"
class Hero
{
public:
	Hero();
	~Hero();

	void init(std::string textureName, int frameCount, float animDuration, sf::Vector2f position, float mass);
	void update(float dt);
	void jump(float velocity);
	sf::Sprite getSprite();
	void recharge();
	bool canShoot();
	sf::RectangleShape getChargeEmpty();
	sf::RectangleShape getChargeFill();
	

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Vector2f m_position;
	sf::RectangleShape emptyCharge;
	sf::RectangleShape chargeFill;

	int m_frameCount;
	float m_animDuration;
	float m_elapsedTime;
	sf::Vector2i m_spriteSize;

	int jumpCount = 0;
	float m_mass;
	float m_velocity;
	const float m_gravity = 9.81f;
	bool m_grounded;
	float maxRecharge = 0.8f;
	float recharging = maxRecharge;
};

