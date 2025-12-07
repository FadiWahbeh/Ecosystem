#pragma once
#include "core/Interfaces.h"
#include <memory>

namespace Ecosystem {

    class IMovementStrategy;
    class IFeedingStrategy;

    class Animal : public IAnimal {
    public:
        Animal(AnimalKind k,
            Gender g,
            std::unique_ptr<IMovementStrategy> m,
            std::unique_ptr<IFeedingStrategy> f);

		~Animal() override = default;

        AnimalKind kind() const override;

        Gender gender() const override;

        void on_step_begin() override;

        bool is_Hungry() const override;

        int& hungery_ref() override;

        int& satiety_ref() override;

        int& repro_cooldown_ref() override;

        int& baby_turns_ref() override;

        IMovementStrategy& movement() override;

        IFeedingStrategy& feeding() override;

    protected:
        AnimalKind m_kind;
        Gender     m_gender;

        int m_hunger = 0;
        int m_satiety = 0;
        int m_repro_cooldown = 0;
        int m_baby_turns = 0;

        std::unique_ptr<IMovementStrategy> m_movement_strategy;
        std::unique_ptr<IFeedingStrategy>  m_feeding_strategy;
    };
}
