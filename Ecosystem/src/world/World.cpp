#include "World.h"
#include "../factory/EntityFactory.h"
#include "../core/Strategies.h" 
#include <iostream>
#include <array>
#include <cstdlib> 

namespace Ecosystem {

    World::World(Config& cfg) : cfg_(cfg) {
        std::srand(cfg_.seed);
        grid_.resize(cfg_.width * cfg_.height);
        seedPlants(cfg_.initial_plants);
        seedHerbivores(cfg_.initial_herbivores);
        seedCarnivores(cfg_.initial_carnivores);
    }

    Cell* World::getCell(int x, int y) {
        if (!inBounds(x, y)) return nullptr;
        return &grid_[idx(x, y)];
    }

    void World::seedPlants(int n) {
        for (int i = 0; i < n; i++) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.plant) c.plant = EntityFactory::makePlant();
        }
    }

    void World::seedHerbivores(int n) {
        int placed = 0, guard = n * 20 + 1000;
        while (placed < n && guard--) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.animal) { c.animal = EntityFactory::makeHerbivore(); placed++; }
        }
    }

    void World::seedCarnivores(int n) {
        int placed = 0, guard = n * 20 + 1000;
        while (placed < n && guard--) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.animal) { c.animal = EntityFactory::makeCarnivore(); placed++; }
        }
    }

    void World::sysMove() {
        struct Move { int x, y, nx, ny; };
        std::vector<Move> moves;
        moves.reserve(cfg_.width * cfg_.height / 2);

        for (int y = 0; y < cfg_.height; y++) {
            for (int x = 0; x < cfg_.width; x++) {

                auto& cell = grid_[idx(x, y)];
                if (!cell.animal) continue;

                auto& a = *cell.animal;

                //ici on initialise bien next avant de l'utiliser
                auto next = a.movement().choose_next(x, y);

                // inBounds prend bien 2 arguments
                if (!inBounds(next.x, next.y)) continue;

                auto& dst = grid_[idx(next.x, next.y)];

                if (!dst.animal) {
                    moves.push_back({ x, y, next.x, next.y });
                }
            }
        }

        for (auto& m : moves) {
            // on vérifie que la case source contient toujours l’animal
            if (!grid_[idx(m.x, m.y)].animal) continue;
            if (grid_[idx(m.nx, m.ny)].animal) continue;

            //  transfert du pointeur unique
            grid_[idx(m.nx, m.ny)].animal = std::move(grid_[idx(m.x, m.y)].animal);
        }
    }

    void World::sysFeed() {
        for (int y = 0; y < cfg_.height; y++)
            for (int x = 0; x < cfg_.width; x++)
                if (auto& a = grid_[idx(x, y)].animal)
                    a->feeding().try_feed(*this, x, y);
    }

    void World::sysReproduce() {
        static const std::array<std::pair<int, int>, 4> dirs{ {{1,0},{-1,0},{0,1},{0,-1}} };
        for (int y = 0; y < cfg_.height; y++) {
            for (int x = 0; x < cfg_.width; x++) {
                auto& cell = grid_[idx(x, y)];
                if (!cell.animal) continue;
                auto& a = *cell.animal;
                if (a.repro_cooldown_ref() > 0) continue;

                for (auto [dx, dy] : dirs) {
                    int nx = x + dx, ny = y + dy;
                    if (!inBounds(nx, ny)) continue;
                    auto& ncell = grid_[idx(nx, ny)];
                    if (ncell.animal && ncell.animal->kind() == a.kind()
                        && ncell.animal->repro_cooldown_ref() == 0) {

                        for (auto [ex, ey] : dirs) {
                            int bx = x + ex, by = y + ey;
                            if (!inBounds(bx, by)) continue;
                            auto& bcell = grid_[idx(bx, by)];
                            if (!bcell.animal) {
                                if (a.kind() == AnimalKind::Herbivore)
                                    bcell.animal = EntityFactory::makeHerbivore();
                                else
                                    bcell.animal = EntityFactory::makeCarnivore();
                                a.repro_cooldown_ref() = cfg_.repro_cool_down;
                                ncell.animal->repro_cooldown_ref() = cfg_.repro_cool_down;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    void World::sysPlantsSpread() {
        if (turn_ % cfg_.plant_spread_period != 0) return;
        std::vector<std::pair<int, int>> pos;
        for (int y = 0; y < cfg_.height; y++)
            for (int x = 0; x < cfg_.width; x++)
                if (grid_[idx(x, y)].plant)
                    pos.emplace_back(x, y);

        static const std::array<std::pair<int, int>, 4> dirs{ {{1,0},{-1,0},{0,1},{0,-1}} };
        for (auto [x, y] : pos) {
            for (auto [dx, dy] : dirs) {
                int nx = x + dx, ny = y + dy;
                if (!inBounds(nx, ny)) continue;
                auto& c = grid_[idx(nx, ny)];
                if (!c.plant) c.plant = EntityFactory::makePlant();
            }
        }
    }

    void World::sysAgingAndStarvation() {
        for (int y = 0; y < cfg_.height; y++) {
            for (int x = 0; x < cfg_.width; x++) {
                auto& c = grid_[idx(x, y)];
                if (c.plant) c.plant->age_one_trun();
                if (c.animal) {
                    auto& a = *c.animal;
                    a.on_step_begin();
                    if (a.hungery_ref() >= cfg_.starvation_limit)
                        c.animal.reset();
                }
            }
        }
    }

    void World::step() {
        sysMove();
        sysFeed();
        sysReproduce();
        sysPlantsSpread();
        sysAgingAndStarvation();
        turn_++;
    }

    std::string World::serialize(int turn) const {
        return statsLine(turn);   // ou ton équivalent
    }


    void World::print() const {
        for (int y = 0; y < cfg_.height; ++y) {
            for (int x = 0; x < cfg_.width; ++x) {
                auto const& c = grid_[idx(x, y)];

                // ICI tu remplaces ton ancien code par le nouveau :
                char ch = '.';
                bool hasPlant = (c.plant != nullptr);
                bool hasAnimal = (c.animal != nullptr);

                if (!hasPlant && !hasAnimal) {
                    ch = '.';
                }
                else if (hasPlant && !hasAnimal) {
                    ch = '*';              // plante seule
                }
                else if (!hasPlant && hasAnimal) {
                    ch = (c.animal->kind() == AnimalKind::Herbivore ? 'h' : 'C'); // animal seul
                }
                else { // hasPlant && hasAnimal
                    ch = (c.animal->kind() == AnimalKind::Herbivore ? 'H' : 'K');
                }

                std::cout << ch;
            }
            std::cout << '\n';
        }
    }

    std::string World::statsLine(int turn) const {
        int plants = 0, herb = 0, carn = 0;
        for (auto const& c : grid_) {
            if (c.plant) plants++;
            if (c.animal) (c.animal->kind() == AnimalKind::Herbivore ? herb++ : carn++);
        }
        return "Turn " + std::to_string(turn) +
            " | Plants=" + std::to_string(plants) +
            " Herb=" + std::to_string(herb) +
            " Carn=" + std::to_string(carn);
    }

} // namespace Ecosystem
