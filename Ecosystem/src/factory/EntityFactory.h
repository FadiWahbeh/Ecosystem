#pragma once
#include <memory>
#include "core/Interfaces.h"

namespace Ecosystem {
    struct IPlant;
    struct IAnimal;

    struct EntityFactory {
        static std::unique_ptr<IPlant>  makePlant();
		
        // gentre aléatoire
		static std::unique_ptr<IAnimal> makeHerbivore();
		static std::unique_ptr<IAnimal> makeCarnivore();

		// genre spécifié
		static std::unique_ptr<IAnimal> makeHerbivore(Gender g);
		static std::unique_ptr<IAnimal> makeCarnivore(Gender g);

		//static Gender randomGender();
    };
}
