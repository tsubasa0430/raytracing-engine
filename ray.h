#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    vec3 orig; //原点 A
    vec3 dir;  //方向 B

    //コンストラクタ
    ray() {}
    ray(const vec3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    //値を取得する関数
    vec3 origin() const  { return orig; }
    vec3 direction() const { return dir; }

    //光線の方程式 P(t) = A + tB に基づいて、t の値に対応する点を計算する関数
    vec3 at(double t) const {
        return orig + dir * t;
    }
};

#endif