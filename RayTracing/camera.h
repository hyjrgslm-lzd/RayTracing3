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
/// 相机类
/// </summary>
class camera {
private:
    #pragma region 基本属性

    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;

    int base_thread_num = 10;
    /// <summary>
    /// 防止光线交点微小偏移到球体内部的情形
    /// </summary>
    double min_distance = 0.001;

    /// <summary>
    /// 相机本身的三个坐标轴在世界空间的坐标
    /// u向右，v向上，w向观察方向中轴反方向
    /// </summary>
    vec3 u, v, w;


    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    #pragma endregion

    #pragma region 初始化

    void init() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = image_height < 1 ? 1 : image_height;
        
        center = lookfrom;

        #pragma region 计算视口

        //auto focal_length = (lookat - lookfrom).length();
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        #pragma region 计算相机坐标系
        w = (lookfrom - lookat).normalized();
        u = vec3::cross(vup, w).normalized();
        v = vec3::cross(w, u).normalized();
        #pragma endregion

        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * (-v);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        #pragma endregion

    }

    #pragma endregion

    #pragma region 功能函数

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
    /// 多线程支持的ray_color着色函数
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
        clog << "\r开始写入" << flush;
        ostringstream oss;
        for (int i = 0; i < num; ++i) {
            oss << res[3 * i] << ' ' << res[3 * i + 1] << ' ' << res[3 * i + 2] << '\n';
        }

        cout << oss.str() << endl;
    }

    /// <summary>
    /// 获得打向像素块(i, j)中心叠加像素内偏移的光线，不支持多线程
    /// </summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    ray get_ray(int i, int j) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    /// <summary>
    /// 获得打向像素块(i, j)中心叠加像素内偏移的光线，支持多线程
    /// </summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <param name="generator"></param>
    /// <returns></returns>
    ray get_ray(int i, int j, random_double_generator& generator) const {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(generator);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample(generator);
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    /// <summary>
    /// 返回基于像素中点的偏移，不支持多线程
    /// </summary>
    /// <returns></returns>
    vec3 pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    /// <summary>
    /// 返回基于像素中点的偏移，支持多线程
    /// </summary>
    /// <param name="generator"></param>
    /// <returns></returns>
    vec3 pixel_sample_square(random_double_generator& generator) const {
        auto px = -0.5 + generator.get_random_double();
        auto py = -0.5 + generator.get_random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    /// <summary>
    /// 返回焦散盘上随机一点，不支持多线程
    /// </summary>
    /// <returns></returns>
    point3 defocus_disk_sample() const {
        auto p = vec3::random_in_unit_circle();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    /// <summary>
    /// 返回焦散盘上随机一点，支持多线程
    /// </summary>
    /// <param name="generator"></param>
    /// <returns></returns>
    point3 defocus_disk_sample(random_double_generator& generator) const {
        auto p = vec3::random_in_unit_circle(generator);
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    #pragma endregion

public:

    #pragma region 基本属性

    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 100;
    int max_depth = 10;
    /// <summary>
    /// 纵向视口角度
    /// </summary>
    double vfov = 90;

    /// <summary>
    /// 相机的朝向由几个变量决定：相机的坐标lookfrom，相机看向的坐标lookat，以及相机的上方向的朝向vup
    /// 此时，我们可以确定一个坐标系(u, v, w)，其中u指向相机右侧，v指向相机上方，w指向相机观察方向的反方向（这里采用的是右手系观测）
    /// </summary>
    
    point3 lookfrom = point3(0, 0, -1);
    point3 lookat = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);

    /// <summary>
    /// 相机景深设置。
    /// 景深的实现原理是光线在一个一定大小的圆盘上随机发出打向观察点，观察点所在的垂直w轴的平面
    /// 是相机的焦平面，所有光线都会打到预期物体上，不会失真
    /// 观察点离焦平面越远，打在同一焦平面像素的光线分散越广，从而越模糊
    /// </summary>

    /// <summary>
    /// 焦平面中心点（lookat）看焦散盘（光线发出的盘）的角度，数值越大，焦散盘越大；等价于光圈大小，数值越大，相同距离的景深效果越大
    /// </summary>
    double defocus_angle;
    /// <summary>
    /// 焦平面到相机的距离
    /// </summary>
    double focus_dist;

    #pragma endregion

    #pragma region 方法

    void render(const hittable& world) {

        this->init();

        cout << "P3\n" << image_width << ' ' << image_height << "\n256\n";

        #pragma region 多线程

        int* ColorResult = new int[image_width * image_height * 3] {0};
        std::atomic<long> count = 0;

        int numThreads = std::thread::hardware_concurrency();
        numThreads = numThreads > 0 ? numThreads : base_thread_num;
        //numThreads = 1;

        int numRowsPerThread = image_height / numThreads;

        std::vector<std::thread> threads;


        ///注意：std::thread的构造函数使用了可变参数模板，将所有参数放在std::tuple中。但是std::atomic<>不能被拷贝，只能被移动。
        ///而std::thread对传递的参数进行复制，因此会导致std::atomic<long>对象无法生成对应的std::tuple
        ///解决方案是使用std::ref，将std::atomic<>对象封装为std::reference_wrapper<>对象，从而将拷贝行为替换为移动行为，从而生成对应的std::tuple

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

        clog << "\r完成!                                                    \n";
    }

    /// <summary>
    /// 每个渲染线程的渲染工作
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