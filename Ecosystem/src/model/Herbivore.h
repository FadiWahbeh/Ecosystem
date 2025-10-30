#pragma once
#include "Animal.h"

namespace Ecosystem {
	class Herbivore : public Animal {
	public :
		using Animal::Animal;
		std::string name() const override {
			return "Herbivore";
		}
	};
}