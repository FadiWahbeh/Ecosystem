#pragma once
#include "Animal.h"

namespace Ecosystem {
	class Carnivore : public Animal {
	public:
		using Animal::Animal;
		std::string name() const override {
			return "Carnivore";
		}
	};
}
