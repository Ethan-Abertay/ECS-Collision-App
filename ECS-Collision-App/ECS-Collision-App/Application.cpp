#include "Application.h"
#include "ComponentsAndSystems.h"

Application::Application()
{
	// Create window
	window = new sf::RenderWindow(sf::VideoMode(800, 800), "Collision App");

	// Initialise ECS
	ecs = new ECS();

	// Initialise components 
	ecs->initComponents<c::Transform, c::RenderData>();

	// Iniitalise entities
	auto id = ecs->createEntity();
	ecs->assignComps<c::Transform, c::RenderData>(id);

	// Create rectangle asset
	rectangle.setSize(sf::Vector2f(80, 80));
	rectangle.setOutlineColor(sf::Color::Black);
	rectangle.setFillColor(sf::Color::White);
	rectangle.setOutlineThickness(2);
	rectangle.setPosition((sf::Vector2f)window->getSize() / 2.f);
}

Application::~Application()
{
	if (window)
		delete window;
	window = 0;
}

void Application::run()
{
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
	eps::checkBoundaryCollision(*ecs, window);
}

void Application::render()
{
	window->clear();

	//window->draw(rectangle);

	eps::renderRectangle(*ecs, DeltaTime, window, rectangle);

	window->display();
}
