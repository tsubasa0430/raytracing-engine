#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include <memory> //std::shared_ptr

class material;

//衝突した際の情報をまとめる構造体
struct hit_record {
    vec3 p;      //衝突した座標
    vec3 normal; //衝突した場所の法線ベクトル
    double t;    //光線の方程式 P(t) = A + tB の t の値
    bool front_face; //光線が物体の外側から当たったか、内側から当たったか
    std::shared_ptr<material> mat_ptr; //衝突した物体の材質へのポインタ

    //法線の向きを常に「光線と逆向き」に補正する関数
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : outward_normal * -1.0;
    }
};

//衝突可能なすべての物体の基底クラス（球体や平面など）
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif