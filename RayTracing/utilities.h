#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <algorithm>
#include <thread>
#include <atomic>
#include <cstdlib>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

/// <summary>
/// �Ƕ�תΪ����
/// </summary>
/// <param name="degress"></param>
/// <returns></returns>
inline double degrees_to_radians(double degress) {
	return degress * pi / 180.0;
}


/// <summary>
/// �����������������[0, 1]
/// ��Ҫע�⣺������������ڲ�������ͬ���������Զ��̲߳�������ʱ�ᵼ��Ч�ʵ���
/// </summary>
/// <returns></returns>
inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937_64 generator;
	return distribution(generator);
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

class random_double_generator {
private:
	std::uniform_real_distribution<double> distribution;
	std::mt19937_64 generator;

public:
	random_double_generator() : distribution(0.0, 1.0), generator() {}

	double get_random_double() { return distribution(generator); }

	double get_random_double(double min, double max) { return min + (max - min) * this->get_random_double(); }
};

#include "interval.h"
