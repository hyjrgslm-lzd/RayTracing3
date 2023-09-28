#pragma once
#include "Vec3.h"

/// <summary>
/// ���߻���
/// </summary>
class ray {
private:
	/// <summary>
	/// ���ߵ��������
	/// </summary>
	point3 orig;

	/// <summary>
	/// ���߷�������
	/// </summary>
	vec3 dir;

public:
	ray() {}
	ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {
		dir.normalize();
	}

	/// <summary>
	/// ���ع����ط��򴫲�t���λ��
	/// </summary>
	/// <param name="t">���ߴ���ʱ��</param>
	/// <returns>���ߴ���tʱ�����λ��</returns>
	point3 at(double t) const { return orig + dir * t; }

	const point3& origin() const { return orig; }

	const vec3& direction() const { return dir; }

	void set_origin(const point3& p) { orig = p; }
	void set_direction(const vec3& dire) { dir = dire.normalized(); }
};