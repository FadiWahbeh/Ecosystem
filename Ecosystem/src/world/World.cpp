#include "World.h"
#include "./factory/EntityFactory.h"
#include "./core/Strategies.h"
#include <iostream>
#include <array>
#include <cstdlib>
#include <random>

namespace Ecosystem {

    World::World(Config& cfg) : cfg_(cfg) {
        std::srand(cfg_.seed);
        grid_.resize(cfg_.width * cfg_.height);

        std::mt19937 rng(cfg_.seed);
        auto randInRange = [&](int base) {
            if (base <= 0) return 0;
            int min = std::max(1, base / 2);
            int max = std::max(min, base * 2);
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng);
            };

        int nPlants = randInRange(cfg_.initial_plants);
        int nHerbs = randInRange(cfg_.initial_herbivores);
        int nCarns = randInRange(cfg_.initial_carnivores);

        seedPlants(nPlants);
        seedHerbivores(nHerbs);
        seedCarnivores(nCarns);
    }

    Cell* World::getCell(int x, int y) {
        if (!inBounds(x, y)) return nullptr;
        return &grid_[idx(x, y)];
    }
    const Cell* World::getCell(int x, int y) const {
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

    // Déplacements

    void World::sysMove() {
        struct Move { int x, y, nx, ny; };
        std::vector<Move> moves;
        moves.reserve(cfg_.width * cfg_.height / 2);

        for (int y = 0; y < cfg_.height; y++) {
            for (int x = 0; x < cfg_.width; x++) {

                auto& cell = grid_[idx(x, y)];
                if (!cell.animal) continue;

                auto& a = *cell.animal;

                if (a.baby_turns_ref() > 0) {
                    a.baby_turns_ref()--;
                    continue;
                }

                auto next = a.movement().choose_next(*this, x, y);

                if (!inBounds(next.x, next.y)) continue;

                auto& dst = grid_[idx(next.x, next.y)];

                if (!dst.animal) {
                    moves.push_back({ x, y, next.x, next.y });
                }
            }
        }

        for (auto& m : moves) {
            if (!grid_[idx(m.x, m.y)].animal) continue;
            if (grid_[idx(m.nx, m.ny)].animal) continue;

            grid_[idx(m.nx, m.ny)].animal = std::move(grid_[idx(m.x, m.y)].animal);
        }
    }

    // Nourrissage

    void World::sysFeed() {
        for (int y = 0; y < cfg_.height; y++)
            for (int x = 0; x < cfg_.width; x++)
                if (auto& a = grid_[idx(x, y)].animal)
                    a->feeding().try_feed(*this, x, y);
    }

    // Reproduction

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
                    if (!ncell.animal) continue;

                    auto& b = *ncell.animal;

                    if (b.kind() == a.kind() &&
                        b.repro_cooldown_ref() == 0 &&
                        b.gender() != a.gender()) {

                        bool spawned = false;
                        for (auto [ex, ey] : dirs) {
                            int bx = x + ex, by = y + ey;
                            if (!inBounds(bx, by)) continue;
                            auto& bcell = grid_[idx(bx, by)];
                            if (!bcell.animal) {
                                if (a.kind() == AnimalKind::Herbivore)
                                    bcell.animal = EntityFactory::makeHerbivore();
                                else
                                    bcell.animal = EntityFactory::makeCarnivore();

                                bcell.animal->baby_turns_ref() = cfg_.baby_stay_turns;

                                a.repro_cooldown_ref() = cfg_.repro_cool_down;
                                b.repro_cooldown_ref() = cfg_.repro_cool_down;

                                spawned = true;
                                break;
                            }
                        }
                        if (spawned) break;
                    }
                }
            }
        }
    }

    // Propagation des plantes

    void World::sysPlantsSpread() {

        if (turn_ == 0) return;

        if (turn_ % cfg_.plant_spread_period != 0) return;

        int totalCells = cfg_.width * cfg_.height;
        int plantCount = 0;
        for (auto const& c : grid_) {
            if (c.plant) plantCount++;
        }

        if (plantCount * 100 >= cfg_.max_plant_percent * totalCells) {
            return;
        }

        std::vector<std::pair<int, int>> pos;
        pos.reserve(plantCount);
        for (int y = 0; y < cfg_.height; ++y) {
            for (int x = 0; x < cfg_.width; ++x) {
                if (grid_[idx(x, y)].plant) {
                    pos.emplace_back(x, y);
                }
            }
        }

        static const std::array<std::pair<int, int>, 4> dirs{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        for (auto [x, y] : pos) {

            auto [dx, dy] = dirs[std::rand() % 4];
            int nx = x + dx;
            int ny = y + dy;

            if (!inBounds(nx, ny)) continue;

            auto& c = grid_[idx(nx, ny)];

            if (!c.plant && !c.animal) {

                int r = std::rand() % 100;
                if (r < cfg_.plant_spread_chance_percent) {
                    c.plant = EntityFactory::makePlant();
                    plantCount++;

                    if (plantCount * 100 >= cfg_.max_plant_percent * totalCells) {
                        break;
                    }
                }
            }
        }
    }


    // Vieillissement & faim

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
        return statsLine(turn);
    }

    void World::print(int debugX, int debugY) const {
        for (int y = 0; y < cfg_.height; ++y) {
            for (int x = 0; x < cfg_.width; ++x) {
                auto const& c = grid_[idx(x, y)];

                char plantCh = c.plant ? '*' : '.';

                char animalCh = '.';
                if (c.animal) {
                    auto& a = *c.animal;
                    bool isBaby = (a.baby_turns_ref() > 0);
                    bool isMale = (a.gender() == Gender::Male);

                    if (a.kind() == AnimalKind::Herbivore) {
                        if (isBaby) {
                            animalCh = isMale ? 'm' : 'f';
                        }
                        else {
                            animalCh = isMale ? 'h' : 'H';
                        }
                    }
                    else {
                        if (isBaby) {
                            animalCh = isMale ? 'M' : 'F';
                        }
                        else {
                            animalCh = isMale ? 'c' : 'C';
                        }
                    }
                }

                if (x == debugX && y == debugY) {
                    char mark1 = 'X';
                    char mark2 = '.';

                    if (c.animal) {
                        mark2 = animalCh;
                    }
                    else if (c.plant) {
                        mark2 = '*';
                    }

                    std::cout << mark1 << mark2;
                }
                else {
                    std::cout << plantCh << animalCh;
                }
            }
            std::cout << '\n';
        }

        std::cout << "\n";
    }

    /*void World::print() const {
        for (int y = 0; y < cfg_.height; ++y) {
            for (int x = 0; x < cfg_.width; ++x) {
                auto const& c = grid_[idx(x, y)];

                // 1er caractère : plante ou pas
                char plantCh = c.plant ? '*' : '.';

                // 2e caractère : animal / genre / bébé
                char animalCh = '.';
                if (c.animal) {
                    auto& a = *c.animal;
                    bool isBaby = (a.baby_turns_ref() > 0);

                    if (a.kind() == AnimalKind::Herbivore) {
                        if (isBaby) {
                            animalCh = 'b';           // bébé herbivore
                        }
                        else {
                            animalCh = (a.gender() == Gender::Male ? 'h' : 'H');
                        }
                    }
                    else { // Carnivore
                        if (isBaby) {
                            animalCh = 'B';           // bébé carnivore
                        }
                        else {
                            animalCh = (a.gender() == Gender::Male ? 'c' : 'C');
                        }
                    }
                }

                std::cout << plantCh << animalCh;
            }
            std::cout << '\n';
        }

        std::cout << "\n";
    }*/

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

    void World::debugPrintAnimalAt(int x, int y) const {
        if (!inBounds(x, y)) {
            std::cout << "Debug: (" << x << "," << y << ") est hors de la grille\n";
            return;
        }

        auto const& cell = grid_[idx(x, y)];
        if (!cell.animal) {
            std::cout << "Debug: aucun animal à (" << x << "," << y << ")\n";
            return;
        }

        auto& a = *cell.animal;

        std::string kindStr =
            (a.kind() == AnimalKind::Herbivore ? "Herbivore" : "Carnivore");
        std::string genderStr =
            (a.gender() == Gender::Male ? "Male" : "Female");

        std::cout << "Debug animal @(" << x << "," << y << ") : "
            << kindStr << " " << genderStr
            << " | hunger=" << a.hungery_ref()
            << " | satiety=" << a.satiety_ref()
            << " | baby_turns=" << a.baby_turns_ref()
            << " | repro_cd=" << a.repro_cooldown_ref()
            << "\n";
    }

} // namespace Ecosystem
