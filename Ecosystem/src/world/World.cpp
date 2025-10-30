#include "World.h"
#include "factory/EntityFactory.h"
#include <iostream>
#include <array>
#include <cstdlib>

namespace Ecosystem {

	World::World(Config& config) : config(config) {
		std::srand(config.seed);
		grid_.resize(config.width * config.height);
		seedPlants(config.initial_plants);
		seedHerbivores(config.initial_herbivores);
		seedCarnivores(config.initial_carnivores);
	}

	Cell* World::getCell(int x, int y) {
		if (!inBounds(x, y)) {
			return nullptr;
		}
		return &grid_[index(x, y)];
	}

	void World::seedPlants(int n) {
		for (int i = 0; i < n; i++) {
			int x = std::rand() % config.width;
			int y = std::rand() % config.height;
			auto& cell = grid_[index(x, y)];
			if (!cell.plant) {
				cell.plant = EntityFactory::makePlant();
			}
		}
	}

	void World::seedAnimalsGeneric(int n, std::unique_ptr<IAnimal>(*make)()) {
		int placed = 0;
		int guard = n * 20 + 1000;
		while (placed < n && guard--) {
			int x = std::rand() % config.width;
			int y = std::rand() % config.height;
			auto& cell = grid_[index(x, y)];
			if (!cell.animal) {
				cell.animal = make();
				placed++;
			}
		}
	}
	void World::seedHerbivores(int n) {
		seedAnimalsGeneric(n, &EntityFactory::makeHerbivore);
	}

	void World::seedCarnivores(int n) {
		seedAnimalsGeneric(n, &EntityFactory::makeCarnivore);
	}


}