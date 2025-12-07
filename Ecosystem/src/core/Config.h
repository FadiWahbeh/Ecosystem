#pragma once
#include <cstdint>
#include <random>

namespace Ecosystem {
    class Config {

    public:
        static Config& I() {
            static Config inst;
            return inst;
        }

        int width = 20;
        int height = 15;

        int initial_plants = 80;
        int initial_herbivores = 30;
        int initial_carnivores = 30;

        int plant_spread_period = 3;
        int repro_cool_down = 4;
        int satiety_after_eat = 3;
        int starvation_limit = 8;
        int plant_spread_chance_percent = 30;
        int max_plant_percent = 40;
		int max_carnivore_percent = 30;
		int max_herbivore_percent = 30;

        int baby_stay_turns = 3;

        unsigned seed;

    private:
        Config() {
            seed = std::random_device{}();
        }
    };

}
