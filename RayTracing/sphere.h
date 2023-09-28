#pragma once
#include "hittable.h"

/// <summary>
/// 球体对象
/// </summary>
class sphere : public hittable {
private:
	point3 center;
	double radius;
	shared_ptr<material> material_surface;

public:
	sphere(point3 _center, double _radius, shared_ptr<material> _material) : center(_center), radius(_radius), material_surface(_material) {}
	

	/// 原理：
	/// 设球心为C(Cx, Cy, Cz)，一个点为P，那么点在球上就等价于(P-C)^2 = r^2
	/// P点在直线上，所以P = A + tB，其中t为光线的时间，A是光线的Origin，B是光线的方向
	/// 代入展开，作为t的一元二次方程求解，得到：
	/// a = B*B
	/// b = 2*B*(A-C)
	/// c = (A-C)^2 - r^2
	/// 
	/// 则得到t的两个可能取值
	/// 最终返回数值较小的正t值
	/// <summary>
	/// 求解光线r = orig + t * dir与球体交点的函数
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_tmin"></param>
	/// <param name="ray_tmax"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		vec3 oc = r.origin() - center;
		auto a = r.direction().length_squared();
		auto half_b = vec3::dot(oc, r.direction());
		auto c = oc.length_squared() - radius * radius;
		auto discriminant = half_b * half_b - a * c;

		if (discriminant < 0) return false;

		auto sqrtd = sqrt(discriminant);
		auto root = (-half_b - sqrtd) / a;

		if (!ray_t.surrounds(root)) {
			root = (-half_b + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return false;
		}

		rec.t = root;
		rec.p = r.at(root);
		auto outward_normal = (rec.p - center).normalized();
		rec.set_face_normal(r, outward_normal);
		rec.material_of_hit = material_surface;

		return true;
	}
};