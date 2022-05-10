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

void EntityManager::initSpawnEntities(ECS& ecs)
{
	for (int i = 0; i < 2000; ++i)
		spawnNormal(ecs, true);
	for (int i = 0; i < 500; i++)
	{
		spawnHealer(ecs, true);
		spawnAttacker(ecs, true);
	}
}

void EntityManager::spawnNewEntity(ECS& ecs)
{
	// 80% spawn normal
	const auto random = rand() % 10;
	if (random < 8)
		spawnNormal(ecs, false);
	else if (random < 9)
		spawnAttacker(ecs, false);
	else
		spawnHealer(ecs, false);
}

float EntityManager::randRange(float min, float max)
{
	auto normalizedFloat = (float)(rand()) / (float)(RAND_MAX);	// Between 0.f and 1.f
	return min + ((max - min) * normalizedFloat);
}

void EntityManager::spawnNormal(ECS& ecs, bool init)
{
	const float width = window->getSize().x;
	const float height = window->getSize().y;
	constexpr float maxVel = 50.f;
	constexpr float maxAcc = 25.f;
	constexpr float minSize = 5.f;
	constexpr float maxSize = 10.f;
	constexpr float minHealth = 1;
	constexpr float maxHealth = 3;

#ifdef GROUPED

	// Create entity - assign comps
	EntityID id(-1);
	if (init)
		id = ecs.init_CreateEntity<c::Transform, c::Health>();
	else
		id = ecs.createEntity<c::Transform, c::Health>();

	// Randomise transform
	auto* transform = ecs.getEntitysComponent<c::Transform>(id);
	transform->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	transform->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	//transform->acceleration = sf::Vector2f(randRange(-maxAcc, maxAcc), randRange(-maxAcc, maxAcc));
	transform->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#else
	EntityID id(-1);
	if (init)
		id = ecs->init_CreateEntity<c::Position, c::Acceleration, c::Size, c::Velocity, c::Health>();
	else
		id = ecs->createEntity<c::Position, c::Acceleration, c::Size, c::Velocity, c::Health>();

	// Randomize data
	ecs->getEntitysComponent<c::Position>(id)->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	ecs->getEntitysComponent<c::Velocity>(id)->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	ecs->getEntitysComponent<c::Size>(id)->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#endif

	// Process more component data
	ecs.getEntitysComponent<c::Health>(id)->health = randRange(minHealth, maxHealth);
}

void EntityManager::spawnHealer(ECS& ecs, bool init)
{
	const float width = window->getSize().x;
	const float height = window->getSize().y;
	constexpr float maxVel = 500.f;
	constexpr float maxAcc = 25.f;
	constexpr float minSize = 5.f;
	constexpr float maxSize = 10.f;
	constexpr float minHealth = 1;
	constexpr float maxHealth = 3;
	constexpr uint8_t minHeal = 1;
	constexpr uint8_t maxHeal = 3;

#ifdef GROUPED

	// Create entity - assign comps
	EntityID id(-1);
	if (init)
		id = ecs.init_CreateEntity<c::Transform, c::Health, c::Healer>();
	else
		id = ecs.createEntity<c::Transform, c::Health, c::Healer>();

	// Randomise transform
	auto* transform = ecs.getEntitysComponent<c::Transform>(id);
	transform->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	transform->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	//transform->acceleration = sf::Vector2f(randRange(-maxAcc, maxAcc), randRange(-maxAcc, maxAcc));
	transform->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#else
	EntityID id(-1);
	if (init)
		id = ecs->init_CreateEntity<c::Position, c::Acceleration, c::Size, c::Velocity>();
	else
		id = ecs->createEntity<c::Position, c::Acceleration, c::Size, c::Velocity>();

	// Randomize data
	ecs->getEntitysComponent<c::Position>(id)->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	ecs->getEntitysComponent<c::Velocity>(id)->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	ecs->getEntitysComponent<c::Size>(id)->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#endif

	// Process more component data
	ecs.getEntitysComponent<c::Health>(id)->health = randRange(minHealth, maxHealth);
	ecs.getEntitysComponent<c::Healer>(id)->heal = randRange(minHeal, maxHeal);
}

void EntityManager::spawnAttacker(ECS& ecs, bool init)
{
	const float width = window->getSize().x;
	const float height = window->getSize().y;
	constexpr float maxVel = 500.f;
	constexpr float maxAcc = 25.f;
	constexpr float minSize = 5.f;
	constexpr float maxSize = 10.f;
	constexpr float minHealth = 1;
	constexpr float maxHealth = 3;
	constexpr uint8_t minAttack = 2;
	constexpr uint8_t maxAttack = 5;

#ifdef GROUPED

	// Create entity - assign comps
	EntityID id(-1);
	if (init)
		id = ecs.init_CreateEntity<c::Transform, c::Health, c::Attacker>();
	else
		id = ecs.createEntity<c::Transform, c::Health, c::Attacker>();

	// Randomise transform
	auto* transform = ecs.getEntitysComponent<c::Transform>(id);
	transform->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	transform->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	//transform->acceleration = sf::Vector2f(randRange(-maxAcc, maxAcc), randRange(-maxAcc, maxAcc));
	transform->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#else
	EntityID id(-1);
	if (init)
		id = ecs->init_CreateEntity<c::Position, c::Acceleration, c::Size, c::Velocity, c::Health, c::Attacker>();
	else
		id = ecs->createEntity<c::Position, c::Acceleration, c::Size, c::Velocity, c::Health, c::Attacker>();

	// Randomize data
	ecs->getEntitysComponent<c::Position>(id)->position = sf::Vector2f(randRange(0, width), randRange(0, height));
	ecs->getEntitysComponent<c::Velocity>(id)->velocity = sf::Vector2f(randRange(-maxVel, maxVel), randRange(-maxVel, maxVel));
	ecs->getEntitysComponent<c::Size>(id)->size = sf::Vector2f(randRange(minSize, maxSize), randRange(minSize, maxSize));
#endif

	// Process more component data
	ecs.getEntitysComponent<c::Health>(id)->health = randRange(minHealth, maxHealth);
	ecs.getEntitysComponent<c::Attacker>(id)->damage = randRange(minAttack, maxAttack);
}
