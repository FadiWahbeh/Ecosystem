#pragma once
#include "Position.h"

namespace Ecosystem {
	class World;
	
	struct IMovementStrategy {
		virtual ~IMovementStrategy() = default;
		virtual Position choose_next(const World& world, int x, int y) = 0;
	};

	struct IFeedingStrategy {
		virtual ~IFeedingStrategy() = default;
		virtual void try_feed(class World& world, int x, int y) = 0;
	};


}