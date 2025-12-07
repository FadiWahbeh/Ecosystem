#include "Plant.h"

namespace Ecosystem {
	void Plant::age_one_trun() {
			m_age++;
		}

		int Plant::age() const {
			return m_age;
		}

		std::string Plant::name() const {
			return "Plant";
		}
}
