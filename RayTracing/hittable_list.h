#pragma once
#include "hittable.h"
#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::vector;

/// <summary>
/// hittable对象的列表，作为层级结构
/// </summary>
class hittable_list : public hittable {
public:
	vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.emplace_back(object); }

	/// <summary>
	/// 遍历存储的所有hittable对象，并选择光线第一个命中的目标
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_tmin"></param>
	/// <param name="ray_tmax"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_t;

		for (const auto& object : objects) {
			if (object->hit(r, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far.max = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}
};