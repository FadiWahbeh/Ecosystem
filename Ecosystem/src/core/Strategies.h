#pragma once
#include "Interfaces.h"

class World;

namespace Ecosystem {
	class RandomWalk : public IMouvementStrategy {
	public :
		explicit RandomWalk() = default;

		Position choose_next(int x, int y) override;
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
