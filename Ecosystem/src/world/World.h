#pragma once
#include <vector>
#include <string>
#include "../core/Config.h"
#include "Cell.h"

namespace Ecosystem {

    class World {
    public:
        explicit World(Config& cfg);

        // API utilisée par main()
        void step();
        void print() const;
        std::string statsLine(int turn) const;

        // Si ton main appelle encore serialize(), on expose un alias
        std::string serialize(int turn) const;

        // Accès cellule
        Cell* getCell(int x, int y);

        // Accès config
        const Config& cfg() const { return cfg_; }


    private:

        
        Config& cfg_;
        std::vector<Cell> grid_;
        int turn_ = 0;

        // utilitaires
        int idx(int x, int y) const { return y * cfg_.width + x; }
        bool inBounds(int x, int y) const {
            return x >= 0 && x < cfg_.width && y >= 0 && y < cfg_.height;
        }

        // initialisation
        void seedPlants(int n);
        void seedHerbivores(int n);
        void seedCarnivores(int n);

        // sous-systèmes
        void sysMove();
        void sysFeed();
        void sysReproduce();
        void sysPlantsSpread();
        void sysAgingAndStarvation();
    };

} // namespace Ecosystem
