#pragma once
#include "Vec3.h"
#include <iostream>

using color = vec3;

inline double linear_2_gamma(double linear_component) {
	return sqrt(linear_component);
}

void write_color(std::ostream& out, color pixel_color) {
	out << static_cast<int>(255.999 * pixel_color.r()) << ' '
		<< static_cast<int>(255.999 * pixel_color.g()) << ' '
		<< static_cast<int>(255.999 * pixel_color.b()) << std::endl;
}

void write_color(int* Dst, long index, color& pixel_color) {
	static const interval intensity(0.0, 0.999);
	Dst[index * 3] = static_cast<int>(256 * intensity.clamp(linear_2_gamma(pixel_color.r())));
	Dst[index * 3 + 1] = static_cast<int>(256 * intensity.clamp(linear_2_gamma(pixel_color.g())));
	Dst[index * 3 + 2] = static_cast<int>(256 * intensity.clamp(linear_2_gamma(pixel_color.b())));
}

