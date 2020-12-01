#pragma once
#include <random>

namespace DBBD {
	class Random
	{
	public:
		static Random& instance();
		~Random();

	public:
		int next(int max);
		int next(int min, int max);
		float next(float max);
		float next(float min, float max);

	private:
		Random();
		std::mt19937* mersenne;
	};
}