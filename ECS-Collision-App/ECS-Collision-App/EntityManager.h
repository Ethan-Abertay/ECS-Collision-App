#pragma once
#include "../../../ECS-Library/ECS/ECS/ECS.h"

#include <vector>

namespace sf
{
	class RenderWindow;
}

class EntityManager
{
public:
	EntityManager(sf::RenderWindow* window_);
	void process(ECS& ecs, float DeltaTime);
	void addTimer();
	void spawnNewEntity(ECS& ecs);
	void initSpawnEntities(ECS& ecs);

protected:
	float randRange(float min, float max);
	void spawnNormal(ECS& ecs, bool init);
	void spawnHealer(ECS& ecs, bool init);
	void spawnAttacker(ECS& ecs, bool init);

	sf::RenderWindow* window = 0;

	std::vector<float> timers;	// A vector of timers to respawn a new entity
};

