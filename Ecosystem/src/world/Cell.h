#pragma once
#include <memory>
#include "core/Config.h"
#include "core/Interfaces.h"

namespace Ecosystem {
	struct Cell {
		std::unique_ptr<IPlant> plant;
		std::unique_ptr<IAnimal> animal;
	};
}