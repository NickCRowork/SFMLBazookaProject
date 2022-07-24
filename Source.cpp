#include "SFML-2.5.1/include/SFML/Graphics.hpp"
#include "SFML-2.5.1\include\SFML\Audio.hpp"
#include <vector>

#include "Hero.h"
#include "Enemy.h"
#include "Rocket.h"
#include "Util.h"

/// <summary>
/// Code made following the book "C++ Game Development By Example_ Learn to build games and graphics with SFML,
/// OpenGL, and Vulkan using C++ programming" chapters 3 - 5 by Siddharth Shekar.
/// My changes include: refatoring init to have multiple smaller init functions, refactoring update to
/// handle score changes more cleanly, changing update to loop through enemies and rockets backwards to
/// prevent skipping arrays elements, changing the music to loop, refactoring the collision code into a separte
/// file so that it can be reused in other projects, adding a cooldown for rocket fire with an included animation,
/// fixing some counters that continually incremented using dt so that they wouldn't eventually overflow.
/// </summary>

sf::Vector2f viewSize(1024, 768);//The view of the screen
sf::VideoMode vm(viewSize.x, viewSize.y);//Sets view to the screen
sf::RenderWindow window(vm, "Hello SFMLGame !!!", sf::Style::Default);//Renders to the screen

//Prototypes
void spawnEnemy();
void shoot();
void reset();
void updateScore();
void initText();
void initTextures();
void initSound();

//Sprites
sf::Texture skyTexture;
sf::Sprite skySprite;
sf::Texture bgTexture;
sf::Sprite bgSprite;
// Text
sf::Font headingFont;
sf::Text headingText;
sf::Font scoreFont;
sf::Text scoreText;
sf::Text tutorialText;
//Sound
sf::Music bgMusic;
sf::SoundBuffer fireBuffer;
sf::SoundBuffer hitBuffer;
sf::Sound fireSound(fireBuffer);
sf::Sound hitSound(hitBuffer);

Hero hero;

std::vector<Enemy*> enemies;
std::vector<Rocket*> rockets;

float currentTime = 0.0f;

int score = 0;
bool gameover = true;

void init()
{
	//Move code into sub-inits.
	initTextures();
	initText();
	initSound();
	
	//Create objects
	hero.init("Assets/graphics/heroAnim.png", 4, 1.0f,
		sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.5f), 200);
	srand((int)time(0));//Initialize random numbers
}

void initTextures()
{
	//Load textures
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	bgTexture.loadFromFile("Assets/graphics/bg.png");
	//Attach textures to sprites
	skySprite.setTexture(skyTexture);
	bgSprite.setTexture(bgTexture);
}

void initText()
{
	// Load font
	headingFont.loadFromFile("Assets/fonts/SnackerComic.ttf");
	// Set Heading Text
	headingText.setFont(headingFont);
	headingText.setString("Tiny Bazooka");
	headingText.setCharacterSize(84);
	headingText.setFillColor(sf::Color::Red);
	sf::FloatRect headingbounds = headingText.getLocalBounds();
	headingText.setOrigin(headingbounds.width / 2,
		headingbounds.height / 2);
	headingText.setPosition(sf::Vector2f(viewSize.x * 0.5f,
		viewSize.y * 0.10f));

	// Set Score Text
	scoreFont.loadFromFile("Assets/fonts/arial.ttf");
	scoreText.setFont(scoreFont);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(45);
	scoreText.setFillColor(sf::Color::Red);
	scoreText.setOutlineThickness(1.2);
	sf::FloatRect scorebounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scorebounds.width / 2, scorebounds.height /
		2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y
		* 0.10f));

	// Tutorial Text
	tutorialText.setFont(scoreFont);
	tutorialText.setString("Press Down Arrow to Fire and Start Game, Up Arrow to Jump");
	tutorialText.setCharacterSize(35);
	tutorialText.setFillColor(sf::Color::Red);
	sf::FloatRect tutorialbounds = tutorialText.getLocalBounds();
	tutorialText.setOrigin(tutorialbounds.width / 2,
		tutorialbounds.height / 2);
	tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f,
		viewSize.y * 0.20f));
}

void initSound()
{
	// Audio
	bgMusic.openFromFile("Assets/audio/bgMusic.ogg");
	bgMusic.setLoop(true);
	bgMusic.play();
	hitBuffer.loadFromFile("Assets/audio/hit.ogg");
	fireBuffer.loadFromFile("Assets/audio/fire.ogg");
}

void draw()
{
	window.draw(skySprite);
	window.draw(bgSprite);
	window.draw(hero.getSprite());
	window.draw(hero.getChargeEmpty());
	window.draw(hero.getChargeFill());
	//Draw enemies
	for (Enemy* enemy : enemies)
	{
		window.draw(enemy->getSprite());
	}
	//Draw rockets
	for (Rocket* rocket : rockets) 
	{
		window.draw(rocket->getSprite());
	}
	if (gameover) {
		window.draw(headingText);
		window.draw(tutorialText);
	}
	else
	{
		window.draw(scoreText);
	}
}

void updateInput()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Up) 
			{
				hero.jump(750.0f);
			}
			if (event.key.code == sf::Keyboard::Down) 
			{
				if (gameover) 
				{
					gameover = false;
					reset();
				}
				else 
				{
					if (hero.canShoot())
					{
						shoot();
					}
				}
			}
			if (event.key.code == sf::Keyboard::Escape)
			{
				window.close();
			}
		}
		if (event.type == sf::Event::KeyReleased)
		{
			
		}
		if(event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void update(float dt)
{
	hero.update(dt);
	currentTime += dt;
	// Spawn Enemies
	if (currentTime >= 1.125f)
	{
		spawnEnemy();
		currentTime = 0.0f;
	}
	// Update Enemies
	//Fixed to prevent skipping elements from deletion
	for (int i = enemies.size() - 1; i >= 0; i--)
	{
		Enemy* enemy = enemies[i];
		enemy->update(dt);
		if (enemy->getSprite().getPosition().x < 0) 
		{
			enemies.erase(enemies.begin() + i);
			delete(enemy);
			gameover = true;
		}
	}
	// Update rockets
	//Fixed to prevent skipping elements from deletion
	for (int i = rockets.size() - 1; i >= 0 ; i--)
	{
		Rocket* rocket = rockets[i];
		rocket->update(dt);
		if (rocket->getSprite().getPosition().x > viewSize.x) 
		{
			rockets.erase(rockets.begin() + i);
			delete(rocket);
		}
	}
	// Check collision between Rocket and Enemies
	for (int i = rockets.size() - 1; i >= 0; i--) {
		for (int j = enemies.size() - 1; j >= 0; j--) {
			Rocket* rocket = rockets[i];
			Enemy* enemy = enemies[j];
			if (Util::checkCollision(rocket->getSprite(), enemy->getSprite())) {
				updateScore();
				hitSound.play();

				rockets.erase(rockets.begin() + i);
				enemies.erase(enemies.begin() + j);
				delete(rocket);
				delete(enemy);
				//printf(" rocket intersects enemy \n");
			}
		}
	}
}

int main()
{
	//Initialize Game Objects
	init();
	sf::Clock clock;
	//Main game loop
	while (window.isOpen())
	{
		//Handle Keyboard Events
		updateInput();

		//Update game
		sf::Time dt = clock.restart();
		if (!gameover)
		{
			update(dt.asSeconds());
		}
		//Update Game Objects
		window.clear(sf::Color::Red);
		//Render Game Objects
		draw();
		window.display();
	}
	return 0;
}

void updateScore()
{
	score++;
	std::string finalScore = "Score: " + std::to_string(score);
	scoreText.setString(finalScore);
	sf::FloatRect scorebounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scorebounds.width / 2, scorebounds.height /
		2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y
		* 0.10f));
}

void spawnEnemy() {
	int randLoc = rand() % 3;
	sf::Vector2f enemyPos;
	float speed;
	switch (randLoc) {
	case 0: 
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
		speed = -400;
		break;
	case 1:
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
		speed = -550; 
		break;
	case 2: 
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
		speed = -650; 
		break;
	default: 
		printf("incorrect y value \n"); 
		return;
	}
	Enemy* enemy = new Enemy();
	enemy->init("Assets/graphics/enemy.png", enemyPos, speed);
	enemies.push_back(enemy);
}

void shoot() 
{
	Rocket* rocket = new Rocket();
	rocket->init("Assets/graphics/rocket.png",
		hero.getSprite().getPosition(),
		400.0f);
	rockets.push_back(rocket);
	fireSound.play();
	hero.recharge();
}

void reset() {
	score = 0;
	currentTime = 0.0f;
	for (Enemy* enemy : enemies) {
		delete(enemy);
	}
	for (Rocket* rocket : rockets) {
		delete(rocket);
	}
	enemies.clear();
	rockets.clear();
}
