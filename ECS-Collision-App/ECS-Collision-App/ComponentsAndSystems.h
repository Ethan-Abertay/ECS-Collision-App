#pragma once

// c for components
namespace c
{
	// The transform component
	struct Transform
	{
		Transform() = default;

		sf::Vector2f position = sf::Vector2f(0.f, 0.f);
		sf::Vector2f size = sf::Vector2f(80.f, 80.f);
		sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);
		sf::Vector2f acceleration = sf::Vector2f(50.f, 30.f);
	};

	struct RenderData
	{
		RenderData() = default;

	};
}

// s for systems
namespace s
{
	struct Translation
	{
		static void process(ECS& ecs, float DeltaTime)
		{
			// Get relevent entities
			auto entitiesWithComponents = ecs.getEntitiesWithComponents<c::Transform>();

			// Loop through entities
			for (auto& entityID : *entitiesWithComponents)
			{
				// Get this entity's components
				auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);

				// Process this component
				transform->velocity += transform->acceleration * DeltaTime;
				transform->position += transform->velocity * DeltaTime;
				//cout << position->position.x << endl;
			}
		}
	};
};

// es for extra parameter systems
namespace eps
{
	static void checkBoundaryCollision(ECS& ecs, sf::RenderWindow* window)
	{
		// Lambda to handle boundaries
		auto process = [](float* pos, float* vel, float* acc, const float width, const float size)
		{
			if (*pos < 0)
			{
				*pos = 0;
				*vel *= -1.f;
				*acc *= -1.f;
			}
			else if (*pos + width >= size)
			{
				*pos = size - width;
				*vel *= -1.f;
				*acc *= -1.f;
			}
		};

		// Get relevent entities
		auto entitiesWithComponents = ecs.getEntitiesWithComponents<c::Transform, c::RenderData>();

		// Loop through entities
		for (auto& entityID : *entitiesWithComponents)
		{
			// Get components
			auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);

			// Get variables
			auto* position = &transform->position;
			auto* size = &transform->size;

			// Process variables
			process(&position->x, &transform->velocity.x, &transform->acceleration.x, size->x, window->getSize().x);
			process(&position->y, &transform->velocity.y, &transform->acceleration.y, size->y, window->getSize().y);
		}
	}

	static void renderRectangle(ECS& ecs, float DeltaTime, sf::RenderWindow* window, sf::RectangleShape& rectangle)
	{
		auto entitiesWithComponents = ecs.getEntitiesWithComponents<c::RenderData>();

		for (auto& entityID : *entitiesWithComponents)
		{
			// Get components
			auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);
			auto* renderData = ecs.getEntitysComponent<c::RenderData>(entityID);

			// Process information
			rectangle.setPosition(transform->position);
			rectangle.setSize(transform->size);

			window->draw(rectangle);
		}
	}
};
