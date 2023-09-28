#pragma once

#include "utilities.h"

/// <summary>
/// 用于管理具有最小值和最大值的间隔类
/// </summary>
class interval {
public:
	double min, max;

	interval() : min(infinity), max(-infinity) {}
	interval(double _min, double _max) : min(_min), max(_max) {}

	/// <summary>
	/// 返回x∈[min, max]
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	bool contains(double x) { return min <= x && x <= max; }

	/// <summary>
	/// 返回x∈(min, max)
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	bool surrounds(double x) { return min < x && x < max; }

	/// <summary>
	/// 将x限定在[min, max]之间
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	inline double clamp(double x) const {
		return std::clamp(x, min, max);
	}

	static const interval empty, universe;
};

const static interval empty(infinity, -infinity);
const static interval universe(-infinity, infinity);