#pragma once

#include "utilities.h"

/// <summary>
/// ���ڹ��������Сֵ�����ֵ�ļ����
/// </summary>
class interval {
public:
	double min, max;

	interval() : min(infinity), max(-infinity) {}
	interval(double _min, double _max) : min(_min), max(_max) {}

	/// <summary>
	/// ����x��[min, max]
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	bool contains(double x) { return min <= x && x <= max; }

	/// <summary>
	/// ����x��(min, max)
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	bool surrounds(double x) { return min < x && x < max; }

	/// <summary>
	/// ��x�޶���[min, max]֮��
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