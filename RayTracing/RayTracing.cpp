#include "utilities.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "material.h"

int main() {
	hittable_list world;

	#pragma region 世界设置


	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.6) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.9) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<fuzz_metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					world.add(make_shared<sphere>(center, -0.15, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5));
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	#pragma endregion


	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 3840;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;


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