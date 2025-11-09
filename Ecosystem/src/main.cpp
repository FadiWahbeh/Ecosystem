#include "core/Config.h"
#include "world/World.h"
#include <iostream>
#include <thread>
#include <chrono>


#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

std::ofstream createLogFile() {
    namespace fs = std::filesystem;

    fs::path resultsDir = fs::current_path() / "results";
    if (!fs::exists(resultsDir)) {
        fs::create_directory(resultsDir);
    }

    auto t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << "run_"
        << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S")
        << ".txt";

    fs::path logPath = resultsDir / oss.str();
    std::ofstream logFile(logPath);

    if (!logFile.is_open()) {
        std::cerr << "  Impossible d’ouvrir le fichier de log : " << logPath << "\n";
    }
    else {
        std::cout << " Résultats enregistrés dans : " << logPath << "\n";
    }

    return logFile;
}

/*int main() {
    using namespace Ecosystem;
    auto& cfg = Config::I();
    World world(cfg);

    const int maxTurns = 4;*/
    /*for (int t = 0; t < maxTurns; ++t) {
        std::cout << world.statsLine(t) << "\n";
        world.print();
        world.step();
    }

    for (int t = 0; t < maxTurns; ++t) {
        std::cout << world.statsLine(t) << "\n";
        world.print();

        // choisis une case à observer, par ex (10, 5)
        world.debugPrintAnimalAt(5, 5);
        world.debugPrintAnimalAt(10, 10);
        world.debugPrintAnimalAt(10, 15);
        world.debugPrintAnimalAt(15, 15);
        world.debugPrintAnimalAt(20, 10);

        world.step();
    }*/
/*
    int dbgX = 10;
    int dbgY = 10;

    for (int t = 0; t < maxTurns; ++t) {
        std::cout << world.statsLine(t) << "\n";
        world.print(dbgX, dbgY);           // met un X sur la case (10,10)
        world.debugPrintAnimalAt(dbgX, dbgY);

        world.step();
    }
}*/

int main() {
    using namespace Ecosystem;

    auto& cfg = Config::I();
    World world(cfg);

    std::ofstream log = createLogFile();
    if (!log.is_open()) return 1;

    const int maxTurns = 30;
    int dbgX = 10;
    int dbgY = 10;

    for (int t = 0; t < maxTurns; ++t) {
        std::ostringstream frame;
        frame << world.statsLine(t) << "\n";
        std::cout << frame.str();
        log << frame.str();

        std::ostringstream grid;
        {
            std::stringstream buf;
            std::streambuf* oldCout = std::cout.rdbuf(buf.rdbuf());
            world.print(dbgX, dbgY);
            std::cout.rdbuf(oldCout);
            grid << buf.str();
        }

        std::cout << grid.str();
        log << grid.str();

        std::ostringstream dbg;
        {
            std::stringstream buf;
            std::streambuf* oldCout = std::cout.rdbuf(buf.rdbuf());
            world.debugPrintAnimalAt(dbgX, dbgY);
            std::cout.rdbuf(oldCout);
            dbg << buf.str();
        }

        std::cout << dbg.str();
        log << dbg.str();

        world.step();
    }

    std::cout << "\n Simulation terminée. Résultats enregistrés.\n";
    return 0;
}
