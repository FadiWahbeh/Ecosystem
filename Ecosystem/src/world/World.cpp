#include "World.h"
#include "./factory/EntityFactory.h"
#include "./core/Strategies.h"
#include "core/ConsoleColor.h"
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

        int totalCells = cfg_.width * cfg_.height;
        int maxPlants = (totalCells * cfg_.max_plant_percent) / 100;
        n = std::min(n, maxPlants);

        int planted = 0;
        int guard = n * 20 + 1000;

        while (planted < n && guard--) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.plant) {
                c.plant = EntityFactory::makePlant();
                planted++;
            }
        }
    }


    void World::seedHerbivores(int n) {
        int maxAllowed = cfg_.width * cfg_.height * cfg_.max_herbivore_percent;
        if (n > maxAllowed) n = maxAllowed;

        int placed = 0, guard = n * 20 + 1000;
        while (placed < n && guard--) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.animal) {
                c.animal = EntityFactory::makeHerbivore();
                placed++;
            }
        }
    }

    void World::seedCarnivores(int n) {

        int maxAllowed = cfg_.width * cfg_.height * cfg_.max_carnivore_percent;
        if (n > maxAllowed) n = maxAllowed;

        int placed = 0, guard = n * 20 + 1000;
        while (placed < n && guard--) {
            int x = std::rand() % cfg_.width;
            int y = std::rand() % cfg_.height;
            auto& c = grid_[idx(x, y)];
            if (!c.animal) { c.animal = EntityFactory::makeCarnivore(); placed++; }
        }
    }

    // D�placements

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

    const char* World::color_for_char(char ch) const {
        switch (ch) {
        case '*': return GREEN;
        case 'h': return CYAN;
        case 'H': return BLUE;
        case 'm': return LightRed;
        case 'f': return BRIGHT_BLACK;
        case 'c': return RED;
        case 'C': return MAGENTA;
        case 'M': return LightRed;
        case 'F': return WHITE;
        default: return RESET;
        }
    }

    char World::charForCell(const Cell& c) const {

        if (!c.plant && !c.animal) {
            return '.';
        }

        if (c.animal) {
            auto& a = *c.animal;
            bool isBaby = (a.baby_turns_ref() > 0);
            bool isMale = (a.gender() == Gender::Male);

            if (a.kind() == AnimalKind::Herbivore) {
                if (isBaby) {
                    return isMale ? 'm' : 'f';
                }
                return isMale ? 'h' : 'H';
            }
            else {
                if (isBaby) {
                    return isMale ? 'M' : 'F';
                }
                return isMale ? 'c' : 'C';
            }
        }

        if (c.plant) {
            return '*';
        }

        return '.';
    }

    void World::print(int debugX, int debugY) const {
        for (int y = 0; y < cfg_.height; ++y) {
            for (int x = 0; x < cfg_.width; ++x) {
                const Cell& c = grid_[idx(x, y)];
                char ch = charForCell(c);
                const char* col = color_for_char(ch);

                bool isDebug = (x == debugX && y == debugY);

                if (isDebug) {
                    std::cout << '[' << col << ch << RESET << ']';
                }
                else {
                    std::cout << col << ch << RESET;
                }
            }
            std::cout << '\n';
        }

        std::cout << "\nLegende :\n"
            << "  " << color_for_char('*') << "*" << RESET << " = plante\n"
            << "  " << color_for_char('h') << "h" << RESET << " = herbivore adulte male\n"
            << "  " << color_for_char('H') << "H" << RESET << " = herbivore adulte femelle\n"
            << "  " << color_for_char('m') << "m" << RESET << " = baby herbivore male\n"
            << "  " << color_for_char('f') << "f" << RESET << " = baby herbivore femelle\n"
            << "  " << color_for_char('c') << "c" << RESET << " = carnivore adulte male\n"
            << "  " << color_for_char('C') << "C" << RESET << " = carnivore adulte femelle\n"
            << "  " << color_for_char('M') << "M" << RESET << " = baby carnivore male\n"
            << "  " << color_for_char('F') << "F" << RESET << " = baby carnivore femelle\n"
            << "  " << color_for_char('.') << "." << RESET << " = vide\n\n";


    }

    std::string World::statsLine(int turn) const {
        int plants = 0;
        int herbTotal = 0, carnTotal = 0;

        int babyHerbMale = 0;
        int babyHerbFemale = 0;
        int babyCarnMale = 0;
        int babyCarnFemale = 0;

        for (auto const& c : grid_) {
            if (c.plant) {
                plants++;
            }
            if (c.animal) {
                auto& a = *c.animal;
                bool isBaby = (a.baby_turns_ref() > 0);
                bool isMale = (a.gender() == Gender::Male);

                if (a.kind() == AnimalKind::Herbivore) {
                    herbTotal++;
                    if (isBaby) {
                        if (isMale) babyHerbMale++;
                        else        babyHerbFemale++;
                    }
                }
                else {
                    carnTotal++;
                    if (isBaby) {
                        if (isMale) babyCarnMale++;
                        else        babyCarnFemale++;
                    }
                }
            }
        }

        std::string s = "Turn " + std::to_string(turn) +
            " | Plants=" + std::to_string(plants) +
            " Herb=" + std::to_string(herbTotal) +
            " (baby hm=" + std::to_string(babyHerbMale) +
            ", hf=" + std::to_string(babyHerbFemale) + ")" +
            " Carn=" + std::to_string(carnTotal) +
            " (baby cm=" + std::to_string(babyCarnMale) +
            ", cf=" + std::to_string(babyCarnFemale) + ")";

        return s;
    }

    void World::debugPrintCell(int x, int y) const {
        if (!inBounds(x, y)) {
            std::cout << "(" << x << "," << y << ") est hors de la grille\n";
            std::cout << "------------------------------------------------------------\n\n";
            return;
        }

        auto const& cell = grid_[idx(x, y)];

        std::cout << "Cellule (" << x << "," << y << ") :\n";

        if (!cell.plant && !cell.animal) {
            std::cout << " vide\n";
            std::cout << "------------------------------------------------------------\n\n";
            return;
        }

        if (cell.plant) {
            std::cout << "  Plante presente" << "\n";
        }

        if (cell.animal) {
            auto& a = *cell.animal;

            std::string kindStr =
                (a.kind() == AnimalKind::Herbivore ? "Herbivore" : "Carnivore");
            std::string genderStr =
                (a.gender() == Gender::Male ? "Male" : "Female");

            bool isBaby = (a.baby_turns_ref() > 0);

            std::cout << "  Animal : " << kindStr
                << " | sexe=" << genderStr
                << " | baby=" << (isBaby ? "oui" : "non")
                << " | hunger=" << a.hungery_ref()
                << " | satiety=" << a.satiety_ref()
                << " | repro_cd=" << a.repro_cooldown_ref()
                << " | baby_turns=" << a.baby_turns_ref()
                << "\n";
        }

        std::cout << "------------------------------------------------------------\n\n";
    }


}