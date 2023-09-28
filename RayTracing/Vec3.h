#pragma once
#include <cmath>
#include <iostream>
#include "utilities.h"

/// <summary>
/// 向量类
/// </summary>
class vec3 {
public:
	double e[3];

#pragma region 构造函数
	vec3() : e{ 0, 0, 0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 }  { }

	//vec3(vec3& cmp) { e[0] = cmp.e[0]; e[1] = cmp.e[1]; e[2] = cmp.e[2]; }
#pragma endregion

#pragma region GETTER
	inline double x() const { return e[0]; }
	inline double y() const { return e[1]; }
	inline double z() const { return e[2]; }

	inline double r() const { return e[0]; }
	inline double g() const { return e[1]; }
	inline double b() const { return e[2]; }
#pragma endregion

#pragma region 运算符重载
	inline double operator[](int i) const { return e[i]; }
	inline double& operator[](int i) { return e[i]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

	inline vec3 operator+(const vec3& rhu) const { return vec3(e[0] + rhu.e[0], e[1] + rhu.e[1], e[2] + rhu.e[2]); }
	inline vec3 operator-(const vec3& rhu) const { return *this + (-rhu); }
	inline vec3 operator*(const vec3& rhu) const { return vec3(e[0] * rhu.e[0], e[1] * rhu.e[1], e[2] * rhu.e[2]); }
	inline vec3 operator*(double t) const { return vec3(e[0] * t, e[1] * t, e[2] * t); }
	inline vec3 operator/(const vec3& rhu) const { return vec3(e[0] / rhu.e[0], e[1] / rhu.e[1], e[2] / rhu.e[2]); }
	inline vec3 operator/(double t) const { return *this * (1 / t); }

	inline vec3& operator+=(const vec3& v) { e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2]; return *this; }
	inline vec3& operator-=(const vec3& v) { e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2]; return *this; }
	inline vec3& operator*=(double t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; }
	inline vec3& operator/=(double t) { e[0] /= t; e[1] /= t; e[2] /= t; return *this; }
#pragma endregion

#pragma region 方法
	inline static double dot(const vec3& u, const vec3& v) {
		return u.e[0]*v.e[0] + u.e[1]*v.e[1] + u.e[2]*v.e[2];
	}

	inline static vec3 cross(const vec3& u, const vec3& v) {
		return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
					u.e[2] * v.e[0] - u.e[0] * v.e[2],
					u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	inline double length_squared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	inline double length() const { return sqrt(length_squared()); }

	inline vec3 normalized() const { return *this / this->length(); }
	inline void normalize() { *this = this->normalized(); }

	#pragma region 随机向量

	/// <summary>
	/// 返回三维随机在[0, 1]的随机向量，不支持多线程
	/// </summary>
	/// <returns></returns>
	static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	/// <summary>
	/// 返回三维随机在[0, 1]的随机向量，支持多线程
	/// </summary>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random(random_double_generator& generator) {
		return vec3(generator.get_random_double(), generator.get_random_double(), generator.get_random_double());
	}

	/// <summary>
	/// 返回三维随机在[min, max]的随机向量，不支持多线程
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	/// <summary>
	/// 返回三维随机在[min, max]的随机向量，支持多线程
	/// </summary>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random(double min, double max, random_double_generator& generator) {
		return vec3(generator.get_random_double(min, max), generator.get_random_double(min, max), generator.get_random_double(min, max));
	}

	/// <summary>
	/// 返回三维都在[-1, 1]的向量，不支持多线程
	/// </summary>
	/// <returns></returns>
	static vec3 random_in_unit_sphere() {
		return random(-1, 1);
	}

	/// <summary>
	/// 返回随机方向的单位向量，不支持多线程
	/// </summary>
	/// <returns></returns>
	static vec3 random_unit_vector() {
		return random_in_unit_sphere().normalized();
	}

	/// <summary>
	/// 返回三维都在[-1, 1]的向量，支持多线程
	/// </summary>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random_in_unit_sphere(random_double_generator& generator) {
		return random(-1, 1, generator);
	}

	/// <summary>
	/// 返回随机方向的单位向量，支持多线程
	/// </summary>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random_unit_vector(random_double_generator& generator) {
		return random_in_unit_sphere(generator);
	}

	/// <summary>
	/// 返回与normal在同一侧的半球内随机单位向量，不支持多线程
	/// </summary>
	/// <param name="normal"></param>
	/// <returns></returns>
	static vec3 random_on_hemisphere(const vec3& normal) {
		vec3 on_unit_sphere = random_unit_vector();
		return (dot(on_unit_sphere, normal) > 0 ? on_unit_sphere : -on_unit_sphere);
	}

	/// <summary>
	/// 返回与normal在同一侧的半球内随机单位向量，支持多线程
	/// </summary>
	/// <param name="normal"></param>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random_on_hemisphere(const vec3& normal, random_double_generator& generator) {
		vec3 on_unit_sphere = random_unit_vector(generator);
		return dot(on_unit_sphere, normal) > 0 ? on_unit_sphere : -on_unit_sphere;
	}

	/// <summary>
	/// 返回单位圆上的随机向量，不支持多线程
	/// </summary>
	/// <returns></returns>
	static vec3 random_in_unit_circle() {
		auto distance = random_double();
		auto degree = random_double(0, 2 * pi);
		//auto vec = vec3(cos(degree), sin(degree), 0);
		return vec3(cos(degree), sin(degree), 0) * distance;
	}

	/// <summary>
	/// 返回单位圆上的随机向量，支持多线程
	/// </summary>
	/// <param name="generator"></param>
	/// <returns></returns>
	static vec3 random_in_unit_circle(random_double_generator& generator) {
		auto distance = generator.get_random_double();
		auto degree = generator.get_random_double(0, 2*pi);
		//auto vec = vec3(cos(degree), sin(degree), 0);
		return vec3(cos(degree), sin(degree), 0) * distance;
	}

	#pragma endregion

	inline bool near_zero() const {
		auto s = 1e-8;
		return fabs(e[0] < s) && fabs(e[1] < s) && fabs(e[2] < s);
	}

#pragma endregion

};

using point3 = vec3;

//inline vec3 operator*(const vec3& u, double t) { return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t); }
inline vec3 operator*(double t, const vec3& u) { return u * t; }
inline vec3 operator*(double t, vec3& u) { return u * t; }

//inline vec3 operator/(const vec3& u, double t) { return vec3(u.e[0] / t, u.e[1] / t, u.e[2] / t); }
inline vec3 operator/(double t, const vec3& u) { return u / t; }
inline vec3 operator/(double t, vec3& u) { return u / t; }


/// <summary>
/// 计算镜面反射光线方向的函数
/// </summary>
/// <param name="v"></param>
/// <param name="normal"></param>
/// <returns></returns>
inline vec3 reflect(const vec3& v, const vec3& normal) {
	return v - 2 * vec3::dot(v, normal) * normal;
}


/// <summary>
/// 计算折射光线方向的函数
/// </summary>
/// <param name="uv">入射光线</param>
/// <param name="n">法线</param>
/// <param name="etai_over_etat">入射物体折射率/折射物体折射率</param>
/// <returns></returns>
/// 
/// 原理：
/// 入射的折射率是η，折射的折射率是η'，入射角是θ，反射角是θ'，则根据斯涅尔定律有sinθ' * η' = sinθ * η
/// 入射光线为R，折射光线为R'，可以将R'分解为平行与法线n'与垂直于n'的两部分
/// 其中R'垂直 = η / n' * (R + cosθ * n)
///    R‘平行 = -sqrt(1 - |R'垂直|^2) * n
/// 
/// 计算cosθ，可以通过两个单位向量的点积完成
/// 因此，最后得到R'垂直 = η/η’ * (R +(-R·n)n)
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = fmin(vec3::dot(-uv, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}


inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}