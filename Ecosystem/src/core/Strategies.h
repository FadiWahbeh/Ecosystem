#pragma once
#include "Interfaces.h"

class World;

namespace Ecosystem {
	class RandomWalk : public IMovementStrategy {
	public :
		explicit RandomWalk() = default;

		Position choose_next(const World& world, int x, int y) override;
	};

	class SmartHerbivoreMove : public IMovementStrategy {
		public :
		Position choose_next(const World& world, int x, int y) override;
	private :
		Position flee_from_carnivore(const World& world, int x, int y);
		bool find_nearest_plant(const World& world, int x, int y, int radius, Position& out);
		bool find_nearest_mate(const World& world, int x, int y, int radius, Position& out);
		Position step_towards(int x, int y, const Position& target);
		Position random_step(int x, int y);
	};

	class SmartCarnivoreMove : public IMovementStrategy {
	public:
		Position choose_next(const World& world, int x, int y) override;
	private:
		bool find_nearest_prey(const World& world, int x, int y, int radius, Position& out);
		bool find_nearest_mate(const World& world, int x, int y, int radius, Position& out);
		Position step_towards(int x, int y, const Position& target);
		Position random_step(int x, int y);
	};

	class HerbivoreFeeding : public IFeedingStrategy {
	public :
		void try_feed(class World& world, int x, int y) override;
	};

	class CarnivoreFeeding : public IFeedingStrategy {
	public :
		void try_feed(class World& world, int x, int y) override;
	};
}
