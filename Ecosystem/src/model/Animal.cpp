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
