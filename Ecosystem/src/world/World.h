#pragma once
#include <string>
#include <vector>
#include "core/Config.h"
#include "Cell.h"

namespace Ecosystem {

	class World {
	public :
		explicit World(Config& config);

		void step();
		void print() const;
		std::string serialize(int turn) const;
		
		Cell* getCell(int x, int y);
		const Config& cfg() const {
			return config;
		}

	private :
		Config& config;
		std::vector<Cell> grid;
		int turn = 0;

		int index (int x, int y) const {
			return y * config.width + x;
		}

		bool inBounds(int x, int y) const {
			return x >= 0 && x < config.width && y >= 0 && y < config.height;
		}
	};

}
