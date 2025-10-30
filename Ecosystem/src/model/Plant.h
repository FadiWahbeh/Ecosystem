#pragma once
#include "core/Interfaces.h"

namespace Ecosystem {
	class Plant : public IPlant {
	public:
		void age_one_trun() override {
			m_age++;
		}

		int age() const {
			return m_age;
		}

		std::string name() const override {
			return "Plant";
		}

	private:
		int m_age = 0;
	};
}
