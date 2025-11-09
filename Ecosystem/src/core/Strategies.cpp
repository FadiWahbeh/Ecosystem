#include "Strategies.h"
#include "world/World.h"
#include "Interfaces.h"
#include "world/Cell.h"
#include <array>
#include <cstdlib>
#include <limits>
#include <cmath>

namespace Ecosystem {

    // RandomWalk

    Position RandomWalk::choose_next(const World& world, int x, int y) {
        static const std::array<std::pair<int, int>, 4> directions{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        int index = std::rand() % 4;
        auto [dx, dy] = directions[index];
        return { x + dx, y + dy };
    }

    static int manhattan(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }


    Position SmartHerbivoreMove::flee_from_carnivore(const World & world, int x, int y) {
        const int dangerRadius = 3;
        const Cell* predatorCell = nullptr;
        int predatorX = 0, predatorY = 0;
        int bestDist = std::numeric_limits<int>::max();

        for (int dy = -dangerRadius; dy <= dangerRadius; ++dy) {
            for (int dx = -dangerRadius; dx <= dangerRadius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                const Cell* c = world.getCell(nx, ny);
                if (!c || !c->animal) continue;
                if (c->animal->kind() == AnimalKind::Carnivore) {
                    int d = manhattan(x, y, nx, ny);
                    if (d < bestDist) {
                        bestDist = d;
                        predatorCell = c;
                        predatorX = nx;
                        predatorY = ny;
                    }
                }
            }
        }

        if (!predatorCell) {
            return { x, y };
        }

        static const std::array<std::pair<int, int>, 4> dirs{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        Position bestPos{ x, y };
        int bestAwayDist = bestDist;

        for (auto [dx, dy] : dirs) {
            int nx = x + dx;
            int ny = y + dy;
            const Cell* dst = world.getCell(nx, ny);
            if (!dst) continue;

            if (dst->animal) continue;

            int d = manhattan(nx, ny, predatorX, predatorY);
            if (d > bestAwayDist) {
                bestAwayDist = d;
                bestPos = { nx, ny };
            }
        }

        return bestPos;
    }

    bool SmartHerbivoreMove::find_nearest_plant(const World& world, int x, int y, int radius, Position& out) {
        int bestDist = std::numeric_limits<int>::max();
        bool found = false;

        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                const Cell* c = world.getCell(nx, ny);
                if (!c || !c->plant) continue;

                int d = manhattan(x, y, nx, ny);
                if (d < bestDist) {
                    bestDist = d;
                    out = { nx, ny };
                    found = true;
                }
            }
        }
        return found;
    }

    bool SmartHerbivoreMove::find_nearest_mate(const World& world, int x, int y, int radius, Position& out) {
        const Cell* current = world.getCell(x, y);
        if (!current || !current->animal) return false;

        IAnimal& self = *current->animal;
        if (self.kind() != AnimalKind::Herbivore) return false;
        if (self.repro_cooldown_ref() > 0) return false;

        int bestDist = std::numeric_limits<int>::max();
        bool found = false;

        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                const Cell* c = world.getCell(nx, ny);
                if (!c || !c->animal) continue;

                IAnimal& other = *c->animal;
                if (other.kind() != AnimalKind::Herbivore) continue;
                if (other.gender() == self.gender()) continue; 
                if (other.repro_cooldown_ref() > 0) continue;

                int d = manhattan(x, y, nx, ny);
                if (d < bestDist) {
                    bestDist = d;
                    out = { nx, ny };
                    found = true;
                }
            }
        }
        return found;
    }

    Position SmartHerbivoreMove::step_towards(int x, int y, const Position& target) {
        int dx = 0, dy = 0;
        if (target.x > x) dx = 1;
        else if (target.x < x) dx = -1;

        if (target.y > y) dy = 1;
        else if (target.y < y) dy = -1;

        if (dx != 0) return { x + dx, y };
        if (dy != 0) return { x, y + dy };
        return { x, y };
    }

    Position SmartHerbivoreMove::random_step(int x, int y) {
        static const std::array<std::pair<int, int>, 4> directions{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        int index = std::rand() % 4;
        auto [dx, dy] = directions[index];
        return { x + dx, y + dy };
    }

    Position SmartHerbivoreMove::choose_next(const World& world, int x, int y) {
        Position fleePos = flee_from_carnivore(world, x, y);
        if (fleePos.x != x || fleePos.y != y) {
            return fleePos;
        }

        Position plantPos;
        if (find_nearest_plant(world, x, y, 4, plantPos)) {
            Position step = step_towards(x, y, plantPos);
            return step;
        }

        Position matePos;
        if (find_nearest_mate(world, x, y, 5, matePos)) {
            Position step = step_towards(x, y, matePos);
            return step;
        }

        return random_step(x, y);
    }

    // HerbivoreFeeding

    void HerbivoreFeeding::try_feed(World& world, int x, int y) {
        Cell* cell = world.getCell(x, y);
        if (!cell || !cell->animal) return;

        if (cell->plant && cell->animal) {
            cell->plant.reset();
            cell->animal->satiety_ref() = world.cfg().satiety_after_eat;
            cell->animal->hungery_ref() = 0;
        }
    }

    // CarnivoreFeeding

    void CarnivoreFeeding::try_feed(World& world, int x, int y) {
        Cell* current = world.getCell(x, y);
        if (!current || !current->animal) return;

        static const std::array<std::pair<int, int>, 4> directions{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        for (auto [dx, dy] : directions) {
            int nx = x + dx;
            int ny = y + dy;

            Cell* ncell = world.getCell(nx, ny);
            if (!ncell) continue;

            if (ncell->animal && ncell->animal->kind() == AnimalKind::Herbivore) {
                ncell->animal.reset();
                current->animal->satiety_ref() = world.cfg().satiety_after_eat;
                current->animal->hungery_ref() = 0;
                break;
            }
        }
    }


    bool SmartCarnivoreMove::find_nearest_prey(const World& world, int x, int y, int radius, Position& out) {
        int bestDist = std::numeric_limits<int>::max();
        bool found = false;

        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                const Cell* c = world.getCell(nx, ny);
                if (!c || !c->animal) continue;

                if (c->animal->kind() != AnimalKind::Herbivore) continue;

                int d = manhattan(x, y, nx, ny);
                if (d < bestDist) {
                    bestDist = d;
                    out = { nx, ny };
                    found = true;
                }
            }
        }
        return found;
    }

    bool SmartCarnivoreMove::find_nearest_mate(const World& world, int x, int y, int radius, Position& out) {
        const Cell* current = world.getCell(x, y);
        if (!current || !current->animal) return false;

        IAnimal& self = *current->animal;
        if (self.kind() != AnimalKind::Carnivore) return false;
        if (self.repro_cooldown_ref() > 0) return false;

        int bestDist = std::numeric_limits<int>::max();
        bool found = false;

        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                const Cell* c = world.getCell(nx, ny);
                if (!c || !c->animal) continue;

                IAnimal& other = *c->animal;
                if (other.kind() != AnimalKind::Carnivore) continue;
                if (other.gender() == self.gender()) continue;
                if (other.repro_cooldown_ref() > 0) continue;

                int d = manhattan(x, y, nx, ny);
                if (d < bestDist) {
                    bestDist = d;
                    out = { nx, ny };
                    found = true;
                }
            }
        }
        return found;
    }

    Position SmartCarnivoreMove::step_towards(int x, int y, const Position& target) {
        int dx = 0, dy = 0;
        if (target.x > x) dx = 1;
        else if (target.x < x) dx = -1;

        if (target.y > y) dy = 1;
        else if (target.y < y) dy = -1;

        if (dx != 0) return { x + dx, y };
        if (dy != 0) return { x, y + dy };
        return { x, y };
    }

    Position SmartCarnivoreMove::random_step(int x, int y) {
        static const std::array<std::pair<int, int>, 4> directions{ {
            {  1,  0},
            { -1,  0},
            {  0,  1},
            {  0, -1}
        } };

        int index = std::rand() % 4;
        auto [dx, dy] = directions[index];
        return { x + dx, y + dy };
    }

    Position SmartCarnivoreMove::choose_next(const World& world, int x, int y) {
        Position preyPos;
        if (find_nearest_prey(world, x, y, 6, preyPos)) {
            Position step = step_towards(x, y, preyPos);
            return step;
        }

        Position matePos;
        if (find_nearest_mate(world, x, y, 5, matePos)) {
            Position step = step_towards(x, y, matePos);
            return step;
        }

        return random_step(x, y);
    }

}
