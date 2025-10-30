#include "core/Config.h"
#include "world/World.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    using namespace Ecosystem;

    auto& cfg = Config::I();
    World world(cfg);

    const int maxTurns = 200;
    for (int t = 0; t < maxTurns; ++t) {
        std::cout << "\x1B[2J\x1B[H"; // clear console ANSI
        std::cout << world.serialize(t) << "\n";
        world.print();
        world.step();
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    return 0;
}