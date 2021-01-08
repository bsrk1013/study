#pragma once

namespace DBBD {
	template <typename T>
	class Singleton {
	public:
		static T* Instance() {
			if (!instance) {
				instance = new T;
			}
			return instance;
		}

	private:
		static T* instance = nullptr;
		Singleton(Singleton const&);
		Singleton& operator=(Singleton const&);
	};
}