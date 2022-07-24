#pragma once
#include "SFML-2.5.1/include/SFML/Graphics.hpp"

class Util {
public:
	/// <summary>
	/// Checks collision between two sprites.
	/// </summary>
	/// <param name="sprite1">The first sprite to check.</param>
	/// <param name="sprite2">The second sprite to check.</param>
	/// <returns>True if the sprites have collided.</returns>
	static bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);

private:
	// Disallow creating an instance of this object
	Util() {}
};