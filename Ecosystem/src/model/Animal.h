#pragma once
#include "core/Interfaces.h"
#include <memory>

namespace Ecosystem {

	class IMovementStrategy;
	class IFeedingStrategy;

	class Animal : public IAnimal {
	public :
		Animal(AnimalKind k, std::unique_ptr<IMovementStrategy> m, std::unique_ptr<IFeedingStrategy> f);

		AnimalKind kind() const override {
			return m_kind;
		}

		void on_step_begin() override;

		bool is_Hungry() const override {
			return m_hunger;
		}

		int& hungery_ref() override {
			return m_hunger;
		}

		int& satiety_ref() override {
			return m_satiety;
		}

		int& repro_cooldown_ref() override {
			return m_repro_cooldown;
		}

		IMovementStrategy& movement() {
			return *m_movement_strategy;
		}

		IFeedingStrategy& feeding() {
			return *m_feeding_strategy;
		}

	protected :
		AnimalKind m_kind;
		int m_hunger = 0;
		int m_satiety = 0;
		int m_repro_cooldown = 0;
		std::unique_ptr<IMovementStrategy> m_movement_strategy;
		std::unique_ptr<IFeedingStrategy> m_feeding_strategy;
	};
}