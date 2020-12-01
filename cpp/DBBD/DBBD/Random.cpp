#include "Random.h"
#include <random>

namespace DBBD {
	Random::Random() {
		std::random_device rd;
		mersenne = new std::mt19937(rd());
	}

	Random::~Random() {
		delete mersenne;
	}

	Random& Random::instance() {
		static Random* instance = new Random();
		return *instance;
	}

	int Random::next(int max) {
		return next(0, max);
	}

	int Random::next(int min, int max) {
		std::uniform_int_distribution<> die(min, max);
		return die(*mersenne);
	}

	float Random::next(float max) {
		return next(0.0f, max);
	}

	float Random::next(float min, float max) {
		std::uniform_real_distribution<> die(min, max);
		return die(*mersenne);
	}
}