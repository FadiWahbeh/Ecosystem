#include "Animal.h"
#include "core/Strategies.h"

namespace Ecosystem {

    Animal::Animal(AnimalKind k,
        Gender g,
        std::unique_ptr<IMovementStrategy> m,
        std::unique_ptr<IFeedingStrategy> f)
        : m_kind(k),
        m_gender(g),
        m_movement_strategy(std::move(m)),
        m_feeding_strategy(std::move(f)) {
    }

    AnimalKind Animal::kind() const  {
        return m_kind;
    }

    Gender Animal::gender() const  {
        return m_gender;
    }

    bool Animal::is_Hungry() const  {
        return m_hunger;
    }

    int& Animal::hungery_ref()  {
        return m_hunger;
    }

    int& Animal::satiety_ref()  {
        return m_satiety;
    }

    int& Animal::repro_cooldown_ref()  {
        return m_repro_cooldown;
    }

    int& Animal::baby_turns_ref()  {
        return m_baby_turns;
    }

    IMovementStrategy& Animal::movement()  {
        return *m_movement_strategy;
    }

    IFeedingStrategy& Animal::feeding()  {
        return *m_feeding_strategy;
    }

    void Animal::on_step_begin() {
        if (m_satiety > 0) {
            m_satiety--;
        }
        else {
            m_hunger++;
        }
        if (m_repro_cooldown > 0) {
            m_repro_cooldown--;
        }
    }

}
