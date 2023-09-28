#pragma once

#include "utilities.h"
#include "color.h"
#include "hittable.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include "material.h"

using std::cout;
using std::endl;
using std::clog;
using std::flush;
using std::ostringstream;

/// <summary>
/// �����
/// </summary>
class camera {
private:
    #pragma region ��������

    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;

    int base_thread_num = 10;
    /// <summary>
    /// ��ֹ���߽���΢Сƫ�Ƶ������ڲ�������
    /// </summary>
    double min_distance = 0.001;

    #pragma endregion

    #pragma region ��ʼ��

    void init() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = image_height < 1 ? 1 : image_height;
        
        center = point3(0, 0, 0);

        #pragma region �����ӿ�

        auto focal_length = 1;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        #pragma endregion

    }

    #pragma endregion

    #pragma region ���ܺ���

    color ray_color(const ray& r, int depth, const hittable& world) const {

        if (depth >= max_depth)
            return color(0, 0, 0);

        hit_record rec;
        if (world.hit(r, interval(min_distance, infinity), rec)) {
            //return 0.5 * (rec.normal + color(1, 1, 1));
            //vec3 direction = vec3::random_on_hemisphere(rec.normal);
            vec3 direction = rec.normal + vec3::random_unit_vector();
            return 0.5 * ray_color(ray(rec.p, direction), depth + 1, world);
        }

        vec3 unit_direction = r.direction();
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    /// <summary>
    /// ���߳�֧�ֵ�ray_color��ɫ����
    /// </summary>
    /// <param name="r"></param>
    /// <param name="world"></param>
    /// <param name="generator"></param>
    /// <returns></returns>
    color ray_color(const ray& r, int depth, const hittable& world, random_double_generator& generator) const {

        if (depth >= max_depth)
            return color(0, 0, 0);

        hit_record rec;
        if (world.hit(r, interval(min_distance, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.material_of_hit->scatter(r, rec, attenuation, scattered, generator))
                return attenuation * ray_color(scattered, depth + 1, world, generator);
        }

        vec3 unit_direction = r.direction();
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    void WritePicture(int* res, long num) {
        clog << "\r��ʼд��" << flush;
        ostringstream oss;
        for (int i = 0; i < num; ++i) {
            oss << res[3 * i] << ' ' << res[3 * i + 1] << ' ' << res[3 * i + 2] << '\n';
        }

        cout << oss.str() << endl;
    }

    /// <summary>
    /// ��ô������ؿ�(i, j)���ĵ���������ƫ�ƵĹ���
    /// </summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    ray get_ray(int i, int j) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    ray get_ray(int i, int j, random_double_generator& generator) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(generator);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    /// <summary>
    /// ���ػ��������е��ƫ��
    /// </summary>
    /// <returns></returns>
    vec3 pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    vec3 pixel_sample_square(random_double_generator& generator) const {
        auto px = -0.5 + generator.get_random_double();
        auto py = -0.5 + generator.get_random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    #pragma endregion

public:

    #pragma region ��������

    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 100;
    int max_depth = 10;

    #pragma endregion

    #pragma region ����

    void render(const hittable& world) {

        this->init();

        cout << "P3\n" << image_width << ' ' << image_height << "\n256\n";

        #pragma region ���߳�

        int* ColorResult = new int[image_width * image_height * 3] {0};
        std::atomic<long> count = 0;

        int numThreads = std::thread::hardware_concurrency();
        numThreads = numThreads > 0 ? numThreads : base_thread_num;
        //numThreads = 1;

        int numRowsPerThread = image_height / numThreads;

        std::vector<std::thread> threads;


        ///ע�⣺std::thread�Ĺ��캯��ʹ���˿ɱ����ģ�壬�����в�������std::tuple�С�����std::atomic<>���ܱ�������ֻ�ܱ��ƶ���
        ///��std::thread�Դ��ݵĲ������и��ƣ���˻ᵼ��std::atomic<long>�����޷����ɶ�Ӧ��std::tuple
        ///���������ʹ��std::ref����std::atomic<>�����װΪstd::reference_wrapper<>���󣬴Ӷ���������Ϊ�滻Ϊ�ƶ���Ϊ���Ӷ����ɶ�Ӧ��std::tuple

        for (int t = 0; t < numThreads - 1; ++t) {
            int startRow = t * numRowsPerThread;
            int endRow = startRow + numRowsPerThread;
            threads.emplace_back(std::thread(&camera::render_thread, this, std::ref(world), startRow, endRow, std::ref(count), ColorResult));
        }

        threads.emplace_back(std::thread(&camera::render_thread, this, std::ref(world), (numThreads - 1) * numRowsPerThread, image_height, std::ref(count), ColorResult));

        for (auto& t : threads)
            t.join();

        #pragma endregion

        WritePicture(ColorResult, image_width * image_height);

        clog << "\r���!                                                    \n";
    }

    /// <summary>
    /// ÿ����Ⱦ�̵߳���Ⱦ����
    /// </summary>
    /// <param name="world"></param>
    /// <param name="startRow"></param>
    /// <param name="endRow"></param>
    /// <param name="count"></param>
    void render_thread(const hittable& world, int startRow, int endRow, std::atomic<long>& count, int* ColorResult) {
        random_double_generator generator;

        for (int j = startRow; j < endRow; ++j) {
            clog << "\rScanlines over: " << static_cast<double>(count) / (image_height * image_width) * 100 << "%  " << flush;
            
            for (int i = 0; i < image_width; ++i) {

                color pixel_color(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j, generator);
                    pixel_color += ray_color(r, 0, world, generator);
                }

                pixel_color /= samples_per_pixel;

                write_color(ColorResult, j * image_width + i, pixel_color);
                
                ++count;
            }
            //count += image_width;
        }
    }

    #pragma endregion
};