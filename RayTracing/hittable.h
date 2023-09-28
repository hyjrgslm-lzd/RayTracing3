#pragma once
#include "Ray.h"

class material;


/// <summary>
/// ������������ļ�¼���������е����ꡢ���е�ķ������Ӧ��t��front_face��ʾ�����Ƿ�������������������
/// </summary>
class hit_record {
public:
	point3 p;
	vec3 normal;
	shared_ptr<material> material_of_hit;
	double t;
	bool front_face;

	/// <summary>
	/// ���ù����Ƿ������������������棬������normal����ͳһ��������
	/// </summary>
	/// <param name="r"></param>
	/// <param name="outward_normal">��ǰ</param>
	void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = vec3::dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};


/// <summary>
/// ���߿����ж���ĳ�����
/// </summary>
class hittable {
public:
	virtual ~hittable() = default;

	/// <summary>
	/// ���������屾���󽻵��麯��
	/// </summary>
	/// <param name="r">����</param>
	/// <param name="ray_tmin">��С���е�t</param>
	/// <param name="ray_tmax">������е�t</param>
	/// <param name="rec">���н��</param>
	/// <returns>���������Ƿ��ཻ</returns>
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};