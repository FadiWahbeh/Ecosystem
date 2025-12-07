#pragma once
#include "EntityBase.h"

namespace Ecosystem {

	class World;
	class IMovementStrategy;
	class IFeedingStrategy;

	enum class AnimalKind {
		Herbivore,
		Carnivore
	};

	enum class Gender {
		Male,
		Female
	};

	struct IAnimal : public IEntity {
		virtual AnimalKind kind() const = 0;
		virtual Gender gender() const = 0;
		virtual void on_step_begin() = 0;
		virtual int& baby_turns_ref() = 0;

		virtual bool is_Hungry() const = 0;
		virtual int& hungery_ref() = 0;

		virtual int& satiety_ref() = 0;
		virtual int& repro_cooldown_ref() = 0;

		
		virtual class IMovementStrategy& movement() = 0;
		virtual class IFeedingStrategy& feeding() = 0;
	};
}