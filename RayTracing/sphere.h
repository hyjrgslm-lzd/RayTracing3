#pragma once
#include "hittable.h"

/// <summary>
/// �������
/// </summary>
class sphere : public hittable {
private:
	point3 center;
	double radius;
	shared_ptr<material> material_surface;

public:
	sphere(point3 _center, double _radius, shared_ptr<material> _material) : center(_center), radius(_radius), material_surface(_material) {}
	

	/// ԭ��
	/// ������ΪC(Cx, Cy, Cz)��һ����ΪP����ô�������Ͼ͵ȼ���(P-C)^2 = r^2
	/// P����ֱ���ϣ�����P = A + tB������tΪ���ߵ�ʱ�䣬A�ǹ��ߵ�Origin��B�ǹ��ߵķ���
	/// ����չ������Ϊt��һԪ���η�����⣬�õ���
	/// a = B*B
	/// b = 2*B*(A-C)
	/// c = (A-C)^2 - r^2
	/// 
	/// ��õ�t����������ȡֵ
	/// ���շ�����ֵ��С����tֵ
	/// <summary>
	/// ������r = orig + t * dir�����彻��ĺ���
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