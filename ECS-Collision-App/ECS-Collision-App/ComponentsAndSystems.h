#pragma once

#include "SFML/Graphics.hpp"
#include "EntityManager.h"

// Macro to decide whether to run with a grouped Transform component,
// or, if undefined, to run as seperate components
// This is application specific, not ECS library
#define GROUPED
//#undef GROUPED

// c for components
namespace c
{
#ifdef GROUPED
	// The transform component
	struct Transform
	{
		Transform() = default;

		sf::Vector2f position = sf::Vector2f(0.f, 0.f);
		sf::Vector2f size = sf::Vector2f(80.f, 80.f);
		sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);
		sf::Vector2f acceleration = sf::Vector2f(0.f, 0.f);
	};
#else
	struct Position
	{
		Position() = default;
		sf::Vector2f position = sf::Vector2f(0.f, 0.f);
	};
	struct Size
	{
		Size() = default;
		sf::Vector2f size = sf::Vector2f(80.f, 80.f);
	};
	struct Velocity
	{
		Velocity() = default;
		sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);
	};
	struct Acceleration
	{
		Acceleration() = default;
		sf::Vector2f acceleration = sf::Vector2f(0.f, 0.f);
	};
#endif

	struct Health
	{
		Health() = default;
		int8_t health = 0;
	};

	struct Healer
	{
		Healer() = default;
		uint8_t heal = 0;
	};

	struct Attacker
	{
		Attacker() = default;
		uint8_t damage = 0;
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
			auto processEntity = [&](EntityID entityID)
			{
#ifdef GROUPED
				// Get this entity's components
				auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);

				// Process this component
				transform->velocity += transform->acceleration * DeltaTime;
				transform->position += transform->velocity * DeltaTime;
#else
				// Get components
				auto& position = ecs.getEntitysComponent<c::Position>(entityID)->position;
				auto& velocity = ecs.getEntitysComponent<c::Velocity>(entityID)->velocity;
				auto& acceleration = ecs.getEntitysComponent<c::Acceleration>(entityID)->acceleration;

				// Process components
				velocity += acceleration * DeltaTime;
				position += velocity * DeltaTime;
#endif
			};

#if IMPL != 3

#ifdef GROUPED
			auto compMask = ecs.getCompMask<c::Transform>();
#else
			auto compMask = ecs.getCompMask<c::Acceleration, c::Velocity, c::Position>();
#endif

			// Loop through entities
			for (EntityID entityID = 0; entityID < ecs.getNoOfEntities(); entityID++)
			{
				// Test if correct type of entity
				if (!ecs.entityHasComponents(entityID, compMask))
					continue;

				processEntity(entityID);
			}
		}

#elif IMPL == 3

			for (auto group : ecs.getEntityGroups())
			{
				for (int i = group->startIndex; i <= group->getEndIndex(); i++)
				{
					processEntity(i);
				}
			}

#endif
		}

	};
};

// eps for extra parameter systems
namespace eps
{
	static void EntityCollision(ECS& ecs, float DeltaTime, EntityManager* entityManager)
	{
		// Lambdas
		auto healthModification = [&](EntityID id, int16_t amount)
		{
			auto& health = ecs.getEntitysComponent<c::Health>(id)->health;
			health += amount;
			if (health <= 0)
			{
				ecs.destroyEntity(id);		// Destroy entity now
				entityManager->addTimer();	// Add respawn timer
			}
		};
		auto processHealth = [&](EntityID id1, EntityID id2)
		{
			// If 1 is attacker
			if (ecs.entityHasComponents(id1, ecs.getCompMask<c::Attacker>()))
				healthModification(id2, ecs.getEntitysComponent<c::Attacker>(id1)->damage);
			else if (ecs.entityHasComponents(id1, ecs.getCompMask<c::Healer>()))
				healthModification(id2, ecs.getEntitysComponent<c::Healer>(id1)->heal);
			else
				healthModification(id2, -1);
		};
		auto postCollision = [&](float& vel1, float& acc1, float& vel2, float& acc2, EntityID id1, EntityID id2)
		{
			// Handle vel and acc
			if (vel1 * vel2 < 0)	// If travelling in opposite directions
			{
				vel1 *= -1.f;
				acc1 *= -1.f;
				vel2 *= -1.f;
				acc2 *= -1.f;
			}
			else	// Travelling in the same direction
			{
				// Invert the greater velocity
				if (abs(vel1) > abs(vel2))
				{
					vel1 *= -1.f;
					acc1 *= -1.f;
				}
				else
				{
					vel2 *= -1.f;
					acc2 *= -1.f;
				}
			}

			// Process health 
			processHealth(id1, id2);
			processHealth(id2, id1);
		};
		auto handleCollision = [&](sf::Vector2f& position1, sf::Vector2f& position2, sf::Vector2f& velocity1, sf::Vector2f& velocity2, sf::Vector2f& acceleration1, sf::Vector2f& acceleration2, const sf::Vector2f& size1, const sf::Vector2f& size2, EntityID id1, EntityID id2)
		{
			auto left1 = position1.x;
			auto right1 = left1 + size1.x;
			auto top1 = position1.y;
			auto bottom1 = top1 + size1.y;
			auto left2 = position2.x;
			auto right2 = left2 + size2.x;
			auto top2 = position2.y;
			auto bottom2 = top2 + size2.y;

			bool overlapX = (right1 > left2 && right1 < right2) || (left1 > left2 && left1 < right2) || (right2 > left1 && right2 < right1) || (left2 > left1 && left2 < right1);
			bool overlapY = (bottom1 > top2 && bottom1 < bottom2) || (top1 > top2 && top1 < bottom2) || (bottom2 > top1 && bottom2 < bottom1) || (top2 > top1 && top2 < bottom1);

			if (overlapX &&		// Collision X
				overlapY)		// Collision Y
			{
				// These entities are colliding

				// Displace 
				auto offsetRight = abs(right1 - left2);
				auto offsetLeft = abs(left1 - right2);
				auto offsetTop = abs(top1 - bottom2);
				auto offsetBottom = abs(bottom1 - top2);

				// Determine which direction to bounce
				bool collideRight = offsetRight < offsetLeft&& offsetRight < offsetTop&& offsetRight < offsetBottom;
				bool collideLeft = offsetLeft < offsetRight&& offsetLeft < offsetTop&& offsetLeft < offsetBottom;
				bool collideTop = offsetTop < offsetRight&& offsetTop < offsetLeft&& offsetTop < offsetBottom;
				bool collideBottom = offsetBottom < offsetRight&& offsetBottom < offsetLeft&& offsetBottom < offsetTop;

				//cout << "right " << collideRight << " left " << collideLeft << " top " << collideTop << " bottom " << collideBottom << endl;

				if (collideRight)
				{
					// Move
					position1.x -= offsetRight / 2.f;
					position2.x += offsetRight / 2.f;

					postCollision(velocity1.x, acceleration1.x, velocity2.x, acceleration2.x, id1, id2);
				}
				else if (collideLeft)
				{
					// Move
					position1.x -= offsetLeft / 2.f;
					position2.x += offsetLeft / 2.f;

					postCollision(velocity1.x, acceleration1.x, velocity2.x, acceleration2.x, id1, id2);
				}
				else if (collideTop)
				{
					// Move
					position1.y -= offsetTop / 2.f;
					position2.y += offsetTop / 2.f;

					postCollision(velocity1.y, acceleration1.y, velocity2.y, acceleration2.y, id1, id2);
				}
				else if (collideBottom)
				{
					// Move
					position1.y -= offsetBottom / 2.f;
					position2.y += offsetBottom / 2.f;

					postCollision(velocity1.y, acceleration1.y, velocity2.y, acceleration2.y, id1, id2);
				}
			}
		};
		auto processTwoEntities = [&](EntityID i, EntityID j)
		{
#ifdef GROUPED
			// Get components
			auto* transform1 = ecs.getEntitysComponent<c::Transform>(i);
			auto* transform2 = ecs.getEntitysComponent<c::Transform>(j);

			handleCollision(transform1->position, transform2->position, transform1->velocity, transform2->velocity, transform1->acceleration, transform2->acceleration, transform1->size, transform2->size, i, j);
#else
			// Get components
			auto* position1 = ecs.getEntitysComponent<c::Position>(entitiesWithComponents->at(i));
			auto* position2 = ecs.getEntitysComponent<c::Position>(entitiesWithComponents->at(j));
			auto* velocity1 = ecs.getEntitysComponent<c::Velocity>(entitiesWithComponents->at(i));
			auto* velocity2 = ecs.getEntitysComponent<c::Velocity>(entitiesWithComponents->at(j));
			auto* acceleration1 = ecs.getEntitysComponent<c::Acceleration>(entitiesWithComponents->at(i));
			auto* acceleration2 = ecs.getEntitysComponent<c::Acceleration>(entitiesWithComponents->at(j));
			auto* size1 = ecs.getEntitysComponent<c::Size>(entitiesWithComponents->at(i));
			auto* size2 = ecs.getEntitysComponent<c::Size>(entitiesWithComponents->at(j));

			handleCollision(position1->position, position2->position, velocity1->velocity, velocity2->velocity, acceleration1->acceleration, acceleration2->acceleration, size1->size, size2->size, i, j);
#endif
		};

		// Get relevent entities
#if IMPL != 3

#ifdef GROUPED
		auto compMask = ecs.getCompMask<c::Transform>();
#else
		auto compMask = ecs.getCompMask<c::Position, c::Size, c::Acceleration, c::Velocity>();
#endif
		// Loop through entities
		for (int i = 0; i < ecs.getNoOfEntities(); ++i)
		{
			// Test if correct type of entity
			if (!ecs.entityHasComponents(i, compMask))
				continue;

			for (int j = i + 1; j < ecs.getNoOfEntities(); ++j)
			{
				// Ensure entity i hasn't died
				if (ecs.entityIsDead(i))
					break;

				// Test if correct type of entity
				if (!ecs.entityHasComponents(j, compMask))
					continue;

				processTwoEntities(i, j);
			}
		}

#elif IMPL == 3

		const auto groups = ecs.getEntityGroups();
		for (int i = 0; i < groups.size(); i++)
		{
			// This is to loop through each entity 
			for (int j = groups[i]->startIndex; j <= groups[i]->getEndIndex(); j++)
			{
				// Loop through each group so this entity can interact with each other entity
				for (int k = i; k < groups.size(); k++)
				{
					int w = 0;
					if (k == i)
						w = j + 1;
					else
						w = groups[k]->startIndex;

					// These are all entities that haven't yet been interacted with
					while (w <= groups[k]->getEndIndex())
					{
						processTwoEntities(j, w);
						w++;
					}
				}
			}
		}
#endif
	}

	static void checkBoundaryCollision(ECS& ecs, sf::RenderWindow* window)
	{
		// Lambda to handle boundaries
		auto process = [](float& pos, float& vel, float& acc, const float width, const float size)
		{
			if (pos < 0)
			{
				pos = 0;
				vel *= -1.f;
				acc *= -1.f;
			}
			else if (pos + width >= size)
			{
				pos = size - width;
				vel *= -1.f;
				acc *= -1.f;
			}
		};
		auto processEntity = [&](EntityID entityID)
		{
#ifdef GROUPED
			// Get components
			auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);

			// Process variables
			process(transform->position.x, transform->velocity.x, transform->acceleration.x, transform->size.x, window->getSize().x);
			process(transform->position.y, transform->velocity.y, transform->acceleration.y, transform->size.y, window->getSize().y);
#else
			// Get components
			auto& position = ecs.getEntitysComponent<c::Position>(entityID)->position;
			auto& size = ecs.getEntitysComponent<c::Size>(entityID)->size;
			auto& velocity = ecs.getEntitysComponent<c::Velocity>(entityID)->velocity;
			auto& acceleration = ecs.getEntitysComponent<c::Acceleration>(entityID)->acceleration;

			// Process variables
			process(position.x, velocity.x, acceleration.x, size.x, window->getSize().x);
			process(position.y, velocity.y, acceleration.y, size.y, window->getSize().y);
#endif
		};

		// Get relevent entities

#ifdef GROUPED
		auto compMask = ecs.getCompMask<c::Transform>();
#else
		auto compMask = ecs.getCompMask<c::Position, c::Size, c::Acceleration, c::Velocity>();
#endif

#if IMPL != 3

		// Loop through entities
		for (EntityID entityID = 0; entityID < ecs.getNoOfEntities(); entityID++)
		{
			// Test if correct type of entity
			if (!ecs.entityHasComponents(entityID, compMask))
				continue;

			processEntity(entityID);
		}

#elif IMPL == 3

		for (auto group : ecs.getEntityGroups())
		{
			for (int i = group->startIndex; i <= group->getEndIndex(); i++)
			{
				processEntity(i);
			}
		}

#endif
	}

	static void renderRectangle(ECS& ecs, float DeltaTime, sf::RenderWindow* window, sf::RectangleShape& rectangle)
	{
		//auto entitiesWithComponents = ecs.getEntitiesWithComponents<c::RenderData>();
#ifdef GROUPED
		const auto compMask = ecs.getCompMask<c::Transform>();
#else
		const auto compMask = ecs.getCompMask<c::Position, c::Size>();
#endif

		const auto attackerMask = ecs.getCompMask<c::Attacker>();
		const auto healerMask = ecs.getCompMask<c::Healer>();

		auto processEntity = [&](EntityID entityID)
		{
#ifdef GROUPED
			// Get components
			auto* transform = ecs.getEntitysComponent<c::Transform>(entityID);

			// Process information
			rectangle.setPosition(transform->position);
			rectangle.setSize(transform->size);
#else
			// Get components
			auto& position = ecs.getEntitysComponent<c::Position>(entityID)->position;
			auto& size = ecs.getEntitysComponent<c::Size>(entityID)->size;

			// Process information
			rectangle.setPosition(position);
			rectangle.setSize(size);
#endif

			// Process colour 
			if (ecs.entityHasComponents(entityID, attackerMask))
				rectangle.setFillColor(sf::Color::Red);
			else if (ecs.entityHasComponents(entityID, healerMask))
				rectangle.setFillColor(sf::Color::Green);
			else
				rectangle.setFillColor(sf::Color::White);

			window->draw(rectangle);
		};

#if IMPL != 3

		for (EntityID entityID = 0; entityID < ecs.getNoOfEntities(); entityID++)
		{
			// Test if correct type of entity
			if (!ecs.entityHasComponents(entityID, compMask))
				continue;

			processEntity(entityID);
		}

#elif IMPL == 3

		for (auto group : ecs.getEntityGroups())
		{
			for (int i = group->startIndex; i <= group->getEndIndex(); i++)
			{
				processEntity(i);
			}
		}

#endif
	}
};
