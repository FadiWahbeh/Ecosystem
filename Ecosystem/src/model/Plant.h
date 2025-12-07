#pragma once
#include "core/Interfaces.h"

namespace Ecosystem {
	class Plant : public IPlant {
	public:
		void age_one_trun() override;

		int age() const;

		std::string name() const override;

	private:
		int m_age = 0;
	};
}
