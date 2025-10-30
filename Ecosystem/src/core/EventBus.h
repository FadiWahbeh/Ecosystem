#pragma once
#include <functional>
#include <vector>

namespace Ecosystem
{
	class EventBus {
	public :
		using Callback = std::function<void()>;
		
		void subscribe(Callback cb) {
			subscribers.push_back(std::move(cb));
		}

		void publish() {
			for (auto& cb : subscribers) {
				cb();
			}
		}

	private:
		std::vector<Callback> subscribers;

	};
}