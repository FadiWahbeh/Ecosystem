#pragma once
#include <cstdint>

namespace Ecosystem {
	class Config {
	
		public:
			static Config& I() {
				static Config inst;
				return inst;
			}

			int width = 40;
			int height = 20;

			int initial_plants = 250;
			int initial_herbivores = 40;
			int initial_carnivores = 25;

			int plant_spread_period = 3;
			int repro_cool_down = 4;
			int satiety_after_eat = 3;
			int starvation_limit = 8;

			unsigned seed = 1337;
	private:
		Config() = default;
	};

}