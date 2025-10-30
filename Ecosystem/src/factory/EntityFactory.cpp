#include "EntityFactory.h"
#include "model/Plant.h"
#include "model/Herbivore.h"
#include "model/Carnivore.h"
#include "core/Strategies.h"

namespace Ecosystem {
	
	std::unique_ptr<IPlant> EntityFactory::makePlant() {
		return std::make_unique<Plant>();
	}

	std::unique_ptr<IAnimal> EntityFactory::makeHerbivore() {
		//strategy pattern
		return std::make_unique<Herbivore>(
			AnimalKind::Herbivore,
			std::make_unique<RandomWalk>(),
			std::make_unique<HerbivoreFeeding>()
		);
	}

	std::unique_ptr<IAnimal> EntityFactory::makeCarnivore() {
		return std::make_unique<Carnivore>(
			AnimalKind::Carnivore,
			std::make_unique<RandomWalk>(),
			std::make_unique<CarnivoreFeeding>()
		);
	}
}