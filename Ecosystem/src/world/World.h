#pragma once
#include <vector>
#include <string>
#include "../core/Config.h"
#include "Cell.h"

namespace Ecosystem {

    class World {
    public:
        explicit World(Config& cfg);

        void step();
        void print(int debugX = -1, int debugY = -1) const;
        std::string statsLine(int turn) const;
        std::string serialize(int turn) const;

        void debugPrintAnimalAt(int x, int y) const;


        Cell* getCell(int x, int y);
        const Cell* getCell(int x, int y) const;
        const Config& cfg() const { return cfg_; }

    private:
        Config& cfg_;
        std::vector<Cell> grid_;
        int turn_ = 0;

        int idx(int x, int y) const { return y * cfg_.width + x; }
        bool inBounds(int x, int y) const {
            return x >= 0 && x < cfg_.width && y >= 0 && y < cfg_.height;
        }

        void seedPlants(int n);
        void seedHerbivores(int n);
        void seedCarnivores(int n);

        void sysMove();
        void sysFeed();
        void sysReproduce();
        void sysPlantsSpread();
        void sysAgingAndStarvation();

        char charForCell(const Cell& c) const;
    };

}
