#pragma once
#include <memory>
#include "core/Interfaces.h"

namespace Ecosystem {
    struct IPlant;
    struct IAnimal;

    struct EntityFactory {
        static std::unique_ptr<IPlant>  makePlant();

		static std::unique_ptr<IAnimal> makeHerbivore();
		static std::unique_ptr<IAnimal> makeCarnivore();

		static std::unique_ptr<IAnimal> makeHerbivore(Gender g);
		static std::unique_ptr<IAnimal> makeCarnivore(Gender g);
    };
}
