#pragma once
#include <string>

namespace Ecosystem {
	struct IEntity {
		virtual ~IEntity() = default;
		virtual std::string name() const = 0;
	};
}