#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

//すべての材質の基底クラス
class material {
public:
    //光線が当たった際、どのように散乱するかと、どれくらい色が吸収されるかを計算する
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const = 0;
};

//拡散反射
class lambertian : public material {
public:
    vec3 albedo; //反射率（物体の色）

    lambertian(const vec3& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const override {
        vec3 scatter_direction = rec.normal + random_in_unit_sphere();
        
        if (near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

//鏡面反射
class metal : public material {
public:
    vec3 albedo; //金属の色
    double fuzz; //表面の粗さ（0なら完全な鏡、大きいと曇りガラスのような金属になる）

    metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const override {
        //反射ベクトル R = v - 2(v・n)n を計算
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        
        //反射方向に少しランダム性を加えて粗さを表現
        scattered = ray(rec.p, reflected + random_in_unit_sphere() * fuzz);
        attenuation = albedo;
        
        //表面の裏側に散乱しようとした場合は無効にする
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

//屈折
class dielectric : public material {
public:
    double ir; // 屈折率、水は1.33、ガラスは1.5

    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const override {
        attenuation = vec3(1.0, 1.0, 1.0);
        
        //光が外から中へ入るか、中から外へ出るかで屈折率の比率を変える
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        
        //全反射するか、フレネル反射が起きるかを判定
        double cos_theta = std::fmin(dot(unit_direction * -1.0, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        //全反射する場合、またはフレネル反射が起きた場合
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            //それ以外はスネルの法則に従って屈折する
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction);
        return true;
    }

private:
    //フレネル反射を近似計算するシュリックの近似式
    static double reflectance(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif