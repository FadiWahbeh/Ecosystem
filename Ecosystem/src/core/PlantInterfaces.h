#pragma once
#include "EntityBase.h"

namespace Ecosystem {
	struct IPlant : public IEntity {
		virtual void age_one_trun() = 0;
	};
}