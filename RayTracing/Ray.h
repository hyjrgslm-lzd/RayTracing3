#pragma once
#include "Vec3.h"

/// <summary>
/// 光线基类
/// </summary>
class ray {
private:
	/// <summary>
	/// 光线的起点坐标
	/// </summary>
	point3 orig;

	/// <summary>
	/// 光线方向向量
	/// </summary>
	vec3 dir;

public:
	ray() {}
	ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {
		dir.normalize();
	}

	/// <summary>
	/// 返回光线沿方向传播t后的位置
	/// </summary>
	/// <param name="t">光线传播时长</param>
	/// <returns>光线传播t时长后的位置</returns>
	point3 at(double t) const { return orig + dir * t; }

	const point3& origin() const { return orig; }

	const vec3& direction() const { return dir; }

	void set_origin(const point3& p) { orig = p; }
	void set_direction(const vec3& dire) { dir = dire.normalized(); }
};