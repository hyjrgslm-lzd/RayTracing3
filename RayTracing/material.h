#pragma once
#include "utilities.h"
#include "Vec3.h"
#include "Ray.h"
#include "color.h"
#include "hittable.h"

/// <summary>
/// 材质抽象基类
/// </summary>
class material {
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, random_double_generator& generator) const = 0;
};


/// <summary>
/// 漫反射（lambert反射）材质基类
/// </summary>
class lambertian : public material {
private:
	color albedo;

public:
	lambertian(const color& a) : albedo(a) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, random_double_generator& generator) const override {
		auto scatter_direction = (rec.normal + vec3::random_unit_vector(generator));

		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered.set_origin(rec.p); scattered.set_direction(scatter_direction);

		attenuation = albedo;
		return true;
	}
};


/// <summary>
/// 金属材质（纯镜面反射）材质基类
/// </summary>
class metal : public material {
private:
	color albedo;

public:
	metal(const color& a) : albedo(a) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, random_double_generator& generator) const override {
		auto scatter_direction = reflect(r_in.direction(), rec.normal);
		scattered.set_origin(rec.p); scattered.set_direction(scatter_direction);
		attenuation = albedo;
		return true;
	}
};


/// <summary>
/// 有一定磨砂质感的金属表面
/// </summary>
class fuzz_metal : public material {
private:
	color albedo;
	double fuzz;

public:
	fuzz_metal(const color& a, double f) : albedo(a), fuzz(std::clamp(f, 0.0, 1.0)) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, random_double_generator& generator) const override {
		auto scatter_direction = reflect(r_in.direction(), rec.normal).normalized() + fuzz * vec3::random_unit_vector(generator);
		scatter_direction = scatter_direction.normalized();
		//auto scatter_direction_temp = scatter_direction + fuzz * vec3::random_unit_vector(generator);

		//if (scatter_direction_temp.near_zero())
		//	scatter_direction_temp = scatter_direction;

		scattered.set_origin(rec.p); scattered.set_direction(scatter_direction);
		attenuation = albedo;
		//return vec3::dot(rec.normal, scattered.direction()) > 0;
		return true;


		//auto scatter_direction = reflect(r_in.direction(), rec.normal);
		//scattered = ray(rec.p, scatter_direction + fuzz * vec3::random_unit_vector(generator));
		//attenuation = albedo;

		//return vec3::dot(scattered.direction(), rec.normal) > 0;
	}
};


/// <summary>
/// 存在折射的材质基类
/// </summary>
class dielectric : public material {
private:
	/// <summary>
	/// 材质的折射率
	/// </summary>
	double ir;

	/// <summary>
	/// 计算菲涅尔项中反射光线占总光线能量的大小
	/// </summary>
	/// <param name="cosine"></param>
	/// <param name="ref_idx"></param>
	/// <returns></returns>
	static double reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}

public:
	dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, random_double_generator& generator) const override {
		attenuation = color(1.0, 1.0, 1.0);

		double refraction_ratio = rec.front_face ? 1.0 / ir : ir;

		double cos_theta = fmin(vec3::dot(-r_in.direction(), rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;

		if (cannot_refract || generator.get_random_double() < reflectance(cos_theta, refraction_ratio))
			direction = reflect(r_in.direction(), rec.normal);
		else
			direction = refract(r_in.direction(), rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction);
		return true;
	}
};