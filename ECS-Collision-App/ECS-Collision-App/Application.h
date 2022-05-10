#pragma once

#include <iostream>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <fstream>
#include <vector>

#include "SFML/Graphics.hpp"
#include "../../../ECS-Library/ECS/ECS/ECS.h"

typedef std::chrono::high_resolution_clock Clock;

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	// Functions
	void gameLoop();
	void updateInputs();
	void update();
	void render();

	// Classes
	sf::RenderWindow* window = 0;
	sf::Event e;
	ECS *ecs = 0;
	class EntityManager* entityManager = 0;
	sf::RectangleShape rectangle;
	sf::Text fps_text;
	sf::Font arialFont;

	// Timer variables
	std::vector<std::pair<float, float>> deltaTimes;	// first element is delta time, second element is the total time expired when this frame was called

	// Variables
	float DeltaTime = 0.f;
	float FPS = 0.f;
};

