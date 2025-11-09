#include "EntityFactory.h"
#include "model/Plant.h"
#include "model/Herbivore.h"
#include "model/Carnivore.h"
#include "core/Strategies.h"
#include <cstdlib>

namespace Ecosystem {

    std::unique_ptr<IPlant> EntityFactory::makePlant() {
        return std::make_unique<Plant>();
    }

    static Gender randomGender() {
        return (std::rand() % 2 == 0 ? Gender::Male : Gender::Female);
    }

    std::unique_ptr<IAnimal> EntityFactory::makeHerbivore(Gender g) {
        return std::make_unique<Herbivore>(
            AnimalKind::Herbivore,
            g,
            std::make_unique<SmartHerbivoreMove>(),
            std::make_unique<HerbivoreFeeding>()
        );
    }

    std::unique_ptr<IAnimal> EntityFactory::makeHerbivore() {
        return makeHerbivore(randomGender());
    }

    std::unique_ptr<IAnimal> EntityFactory::makeCarnivore(Gender g) {
        return std::make_unique<Carnivore>(
            AnimalKind::Carnivore,
            g,
            std::make_unique<SmartCarnivoreMove>(),
            std::make_unique<CarnivoreFeeding>()
        );
    }

    std::unique_ptr<IAnimal> EntityFactory::makeCarnivore() {
        return makeCarnivore(randomGender());
    }
}
