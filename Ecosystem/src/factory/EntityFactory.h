#pragma once
#include <memory>

namespace Ecosystem {
	struct IPlant;
	struct IAnimal;

	struct EntityFactory {
		static std::unique_ptr<IPlant> makePlant();
		static std::unique_ptr<IAnimal> makeHerbivore();
		static std::unique_ptr<IAnimal> makeCarnivore();
	};
}