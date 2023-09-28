#include "utilities.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "material.h"

int main() {
	hittable_list world;

	#pragma region 世界设置

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2));

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	//world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	#pragma endregion


	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1920;
	cam.samples_per_pixel = 500;
	cam.max_depth = 20;


	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	cam.render(world);

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = end - start;

	long hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
	diff -= std::chrono::duration<double, std::milli>(hours * 3600 * 1000);
	long minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
	diff -= std::chrono::duration<double, std::milli>(minutes * 60 * 1000);
	long seconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
	diff -= std::chrono::duration<double, std::milli>(seconds * 1000);
	long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

	std::clog << "耗时: " << std::setw(2) << hours << "h:"
		<< std::setw(2) << minutes << "m:"
		<< std::setw(2) << seconds << "s:"
		<< std::setw(3) << milliseconds << "ms" << std::endl;

	std::cin.get();
	return 0;
}