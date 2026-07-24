#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits>
#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

const double infinity = std::numeric_limits<double>::infinity();

//色を計算する関数
vec3 ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;

    if(depth <= 0){
        return vec3(0, 0, 0);
    }

    //0.001はノイズ防止用　無限遠まで判定
    if(world.hit(r, 0.001, infinity, rec)){
        ray scattered;
        vec3 attenuation;
        
        //衝突した材質のscatter関数を呼び出して、散乱光と色吸収を計算
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            vec3 color_from_scatter = ray_color(scattered, world, depth - 1);
            return vec3(attenuation.x() * color_from_scatter.x(), 
                        attenuation.y() * color_from_scatter.y(), 
                        attenuation.z() * color_from_scatter.z());
        }
        return vec3(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);


    // --- 描画品質と処理速度の設定 ---
    //動画撮影時などのテスト出力では、samples_per_pixelを減らし、待機時間を短くして撮影した。
    const int samples_per_pixel = 100; //1ピクセルあたりの光線の数（画質・ノイズの少なさ）
    const int max_depth = 50;          //光の最大反射回数

    // --- 世界の構築 ---
    hittable_list world;

    //材質の設定
    //vec3(R, G, B) の各値は 0.0(暗) ～ 1.0(明) で色を指定
    //値を変更することで、出力される画像の色味を自由に変更
    auto material_ground = std::make_shared<lambertian>(vec3(0.8, 0.8, 0.0)); // マットな材質（黄緑色）
    auto material_center = std::make_shared<lambertian>(vec3(0.1, 0.2, 0.5)); // マットな材質（青色）
    
    // 屈折率の設定（1.5はガラス相当）。値をいじると背景の歪み方（スネルの法則）が変わる。
    auto material_left   = std::make_shared<dielectric>(1.5);                 

    // 第2引数(0.0)は金属表面の粗さ　　大きくすると曇りガラスのような金属になる
    auto material_right  = std::make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.0); 

    // --- 物体（球体）の配置 ---
    // sphere の引数は ( 中心座標vec3(X, Y, Z), 半径, 材質 )
    world.add(std::make_shared<sphere>(vec3( 0.0, -100.5, -1.0), 100.0, material_ground)); // 巨大な球で地面を表現
    world.add(std::make_shared<sphere>(vec3( 0.0,    0.0, -1.0),   0.5, material_center)); // 中央の青い球
    world.add(std::make_shared<sphere>(vec3(-1.0,    0.0, -1.0),   0.5, material_left));   // 左のガラス球
    world.add(std::make_shared<sphere>(vec3( 1.0,    0.0, -1.0),   0.5, material_right));  // 右の金属球


    //視点の設定
    double viewport_height = 2.0;
    double viewport_width = aspect_ratio * viewport_height;
    double focal_length = 1.0;

    vec3 origin = vec3(0, 0, 0);
    vec3 horizontal = vec3(viewport_width, 0, 0);
    vec3 vertical = vec3(0, viewport_height, 0);
    vec3 lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - vec3(0, 0, focal_length);

    // --- レンダリングループ ---
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 pixel_color(0, 0, 0);
            //1ピクセル内でランダムに位置をずらして複数回サンプリングし、平均化することで
            //物体の境界線のジャギーをなくす（アンチエイリアシング処理）
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + random_double()) / (image_width - 1);
                double v = (j + random_double()) / (image_height - 1);
                vec3 direction = lower_left_corner + horizontal * u + vertical * v - origin;
                ray r(origin, direction);
                
                pixel_color = pixel_color + ray_color(r, world, max_depth); 
            }

            //サンプル数で割って平均色を算出
            double scale = 1.0 / samples_per_pixel;
            //画像ビューア等で正しく表示されるように、ガンマ2.0の補正（1/2乗 = 平方根）をかける
            double r = std::sqrt(pixel_color.x() * scale);
            double g = std::sqrt(pixel_color.y() * scale);
            double b = std::sqrt(pixel_color.z() * scale);

            int ir = static_cast<int>(256 * std::min(r, 0.999));
            int ig = static_cast<int>(256 * std::min(g, 0.999));
            int ib = static_cast<int>(256 * std::min(b, 0.999));
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::cerr << "\nDone.\n";
}