#include "Strategies.h"
#include "world/world.h"
#include <array>
#include <cstdlib>

namespace Ecosystem
{
	Position RandomWalk::choose_next(int x, int y) {
		static const std::array<std::pair<int, int>, 4> directions{{
			{1, 0},
			{-1, 0},
			{0, 1},
			{0, -1}
		}};

		int index = std::rand() % 4;
		auto [dx, dy] = directions[index];
		return { x + dx, y + dy };
	}

	void HerbivoreFeeding::try_feed(class World& world, int x, int y) {
		auto& cell = world.get_cell(x, y);
		if (!cell) return;

		if (cell->plant && cell->animal) {
			cell -> plant.reset();
			cell -> animal->satiety_ref() = world.cfg().satiety_after_eat;
			cell -> anumal -> hungery_ref() = 0;
		}
	}

	void CarnivoreFeeding::try_feed(class World& world, int x, int y) {
		static const std::array<std::pair<int, int>, 4> directions{{
			{1 , 0},
			{-1, 0},
			{0, 1},
			{0, -1}
		}};
		for (auto [dx, dy] : directions) {
			int nx = x + dx;
			int ny = y + dy;

			auto* ncell = world.getCell(nx, ny);
			auto* c = world.getCell(x, y);

			if (ncell->animal && ncell->animal->kind() == AnimalKind::Herbivore) {
				ncell->animal.reset();
				c -> animal -> satietyRef() = world.cfg().satietyAfterEat;
				c -> animal -> hungerRef() = 0;
				break
			}
		}
	}
}