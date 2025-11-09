#pragma once
#include <string>

namespace Ecosystem {

	class World;

    struct Position {
        int x{};
        int y{};
    };

    struct IEntity {
        virtual ~IEntity() = default;
        virtual std::string name() const = 0;
    };

    struct IPlant : public IEntity {
        virtual void age_one_trun() = 0;
    };

    enum class AnimalKind {
        Herbivore,
        Carnivore
    };

    // Genre des animaux
    enum class Gender {
        Male,
        Female
    };

    struct IAnimal : public IEntity {
        virtual AnimalKind kind() const = 0;
        virtual Gender gender() const = 0;
        virtual void on_step_begin() = 0;
        virtual bool is_Hungry() const = 0;
        virtual int& hungery_ref() = 0;
        virtual int& satiety_ref() = 0;
        virtual int& repro_cooldown_ref() = 0;

        // nombre de tours où l’animal est 
        virtual int& baby_turns_ref() = 0;

        virtual class IMovementStrategy& movement() = 0;
        virtual class IFeedingStrategy& feeding() = 0;
    };

    struct IMovementStrategy {
        virtual ~IMovementStrategy() = default;
        //virtual Position choose_next(int x, int y) = 0;
        virtual Position choose_next(const World& world, int x, int y) = 0;
    };
    struct IFeedingStrategy {
        virtual ~IFeedingStrategy() = default;
        virtual void try_feed(class World& world, int x, int y) = 0;
    };
}
