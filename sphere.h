#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    vec3 center; //中心座標
    double radius; //半径
    std::shared_ptr<material> mat_ptr; //球の材質

    sphere() {}
    sphere(vec3 cen, double r, std::shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - radius * radius;

    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0) return false;
    double sqrtd = std::sqrt(discriminant);

    // 突点 t が描画範囲 (t_min 〜 t_max) に収まっているか確認
    double root = (-b - sqrtd) / (2.0 * a);
    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / (2.0 * a);
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif