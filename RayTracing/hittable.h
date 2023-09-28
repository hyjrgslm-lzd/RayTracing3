#pragma once
#include "Ray.h"

class material;


/// <summary>
/// 光线命中物体的记录，包括命中点坐标、命中点的法线与对应的t。front_face表示光线是否从物体外命中物体表面
/// </summary>
class hit_record {
public:
	point3 p;
	vec3 normal;
	shared_ptr<material> material_of_hit;
	double t;
	bool front_face;

	/// <summary>
	/// 设置光线是否从物体外命中物体表面，并设置normal变量统一向物体外
	/// </summary>
	/// <param name="r"></param>
	/// <param name="outward_normal">当前</param>
	void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = vec3::dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};


/// <summary>
/// 光线可命中对象的抽象类
/// </summary>
class hittable {
public:
	virtual ~hittable() = default;

	/// <summary>
	/// 光线与物体本身求交的虚函数
	/// </summary>
	/// <param name="r">光线</param>
	/// <param name="ray_tmin">最小命中的t</param>
	/// <param name="ray_tmax">最大命中的t</param>
	/// <param name="rec">命中结果</param>
	/// <returns>光线与其是否相交</returns>
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};