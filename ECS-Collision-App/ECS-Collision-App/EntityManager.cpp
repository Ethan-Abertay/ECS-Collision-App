#include "EntityManager.h"
#include "ComponentsAndSystems.h"
#include "SFML/Graphics.hpp"

EntityManager::EntityManager(sf::RenderWindow* window_)
{
	window = window_;
}

void EntityManager::process(ECS& ecs, float DeltaTime)
{
	for (auto it = timers.begin(); it != timers.end();)
	{
		*it -= DeltaTime;

		// If timer is expired
		if (*it <= 0.f)
		{
			spawnNewEntity(ecs);		// Spawn new entity 
			it = timers.erase(it);	// Erase timer
		}
		else
			it++;	// Continue loop
	}
}

void EntityManager::addTimer()
{
	constexpr float minTime = 2.f;
	constexpr float maxTime = 5.f;

	timers.push_back(randRange(minTime, maxTime));
}

void EntityManager::spawnNewEntity(ECS& ecs)
{
	const float width = window->getSize().x;
	const float height = window->getSize().y;
	constexpr float maxVel = 500.f;
	constexpr float maxAcc = 25.f;
	constexpr float minSize = 5.f;
	constexpr float maxSize = 10.f;
	constexpr float minHealth = 1;
	constexpr float maxHealth = 1;

#ifdef GROUPED

	// Create entity - assign comps
	auto id = ecs.createEntity<c::Transform>();
	//ecs->assignComps<c::Transform, c::RenderData>(id);

	// Randomise transform
	auto* transform = ecs.getEntitysComponent<c::Transform>(id);
	transform->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	transform->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	//transform->acceleration = sf::Vector2f(randRange(-maxAcc, maxAcc), randRange(-maxAcc, maxAcc));
	transform->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#else
	auto id = ecs->createEntity<c::Position, c::Acceleration, c::Size, c::Velocity>();

	// Randomize data
	ecs->getEntitysComponent<c::Position>(id)->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	ecs->getEntitysComponent<c::Velocity>(id)->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	ecs->getEntitysComponent<c::Size>(id)->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#endif

	// Process more component data
	ecs.getEntitysComponent<c::Health>(id)->health = randRange(minHealth, maxHealth);
}

float EntityManager::randRange(float min, float max)
{
	auto normalizedFloat = (float)(rand()) / (float)(RAND_MAX);	// Between 0.f and 1.f
	return min + ((max - min) * normalizedFloat);
}
