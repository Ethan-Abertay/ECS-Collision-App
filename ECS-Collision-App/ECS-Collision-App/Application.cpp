#include "Application.h"
#include "ComponentsAndSystems.h"
#include "EntityManager.h"

Application::Application()
{
	// Create window
	window = new sf::RenderWindow(sf::VideoMode(2560, 1440), "Collision App", sf::Style::Fullscreen);

	// srand
	//srand(time(0));
	srand(1);

	// Get Font and text
	bool result = arialFont.loadFromFile("arial.ttf");
	fps_text.setFont(arialFont);
	fps_text.setCharacterSize(24); // in pixels, not points!
	fps_text.setFillColor(sf::Color::Red);
	fps_text.setStyle(sf::Text::Bold);

	// Initialise ECS
	ecs = new ECS();

	// Create the entity manager 
	entityManager = new EntityManager(window);

	// Initialise components
	ecs->initComponents<c::Health, c::Healer, c::Attacker>();

#ifdef GROUPED
	ecs->initComponents<c::Transform>();
#else
	ecs->initComponents<c::Position, c::Acceleration, c::Size, c::Velocity>();
#endif

	// Iniitalise entities
	entityManager->initSpawnEntities(*ecs);

#if IMPL == 3

	ecs->performFullRefactor();

#endif

	// Create rectangle asset
	rectangle.setOutlineColor(sf::Color::Red);
	rectangle.setFillColor(sf::Color::White);
	rectangle.setOutlineThickness(0);
}

Application::~Application()
{
	if (window)
		delete window;
	window = 0;

	if (ecs)
		delete ecs;
	ecs = 0;

	if (entityManager)
		delete entityManager;
	entityManager = 0;
}

void Application::run()
{
	// Length of time to run test in seconds
	float totalTime = 0.f;

	while (window->isOpen())
	{
		// Record start time
		auto start = Clock::now();

		// Run Game Loop
		gameLoop();

		// Record end time
		auto end = Clock::now();

		// Calculate Delta Time - Number of microseconds converted to float then divided by 1,000 twice to get delta time in seconds
		DeltaTime = (float)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000.f / 1000.f;

		// Calculate fps
		FPS = 1.f / DeltaTime;
		
		// Add to total time
		totalTime += DeltaTime;

		// Add delta time to record
		deltaTimes.push_back(std::pair(DeltaTime, totalTime));

		// Test if done (15 seconds)
		if (totalTime >= 15.f)
		{
			// Record output
			std::ofstream file;
			file.open("Output.csv");
			for (auto& f : deltaTimes)
				file << f.first << ',' << f.second << std::endl;
			file.close();

			// Close window (hence application)
			window->close();
		}

		//std::cout << "DeltaTime " << DeltaTime << " FPS " << FPS << std::endl;
	}
}

void Application::gameLoop()
{
	updateInputs();
	update();
	render();
}

void Application::updateInputs()
{
	while (window->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			window->close();
	}
}

void Application::update()
{
	// Handle standard systems
	ecs->processSystems<s::Translation>(DeltaTime);

	// Handle extra systems
	eps::EntityCollision(*ecs, DeltaTime, entityManager);
	eps::checkBoundaryCollision(*ecs, window);

	// Handle entity manager
	entityManager->process(*ecs, DeltaTime);
}

void Application::render()
{
	window->clear();

	eps::renderRectangle(*ecs, DeltaTime, window, rectangle);

	std::string s = "FPS " + std::to_string((int)FPS);
	fps_text.setString(s);
	window->draw(fps_text);

	window->display();
}
