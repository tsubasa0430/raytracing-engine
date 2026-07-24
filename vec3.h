#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <cstdlib> //randを使用

class vec3 {
public:
    //3つの成分を格納する配列
    double e[3];

    //コンストラクタ
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    //成分へのアクセスをわかりやすくする関数
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    /*別のベクトルとの足し算を実装、自分で構造体を定義しているので、
    演算子オーバーロードを使って、ベクトル同士の足し算を実装した。*/
    vec3 operator+(const vec3& v) const{
        return vec3(e[0] + v.x(), e[1] + v.y(), e[2] + v.z());
    }

    //引き算の定義
    vec3 operator-(const vec3& v) const{
        return vec3(e[0] - v.x(), e[1] - v.y(), e[2] - v.z());
    }

    //実数倍の定義
    vec3 operator*(double t) const{
        return vec3(e[0] * t, e[1] * t, e[2] * t);
    }
    //実数倍の定義（実数 * ベクトルの順番も定義）
    friend vec3 operator*(double t, const vec3& v){
        return v * t; //先に定義した実数倍の関数を呼び出す
    }

    //実数で割る定義
    vec3 operator/(double t) const{
        return *this * (1.0 / t);
    }

    //ベクトルの長さを計算する関数
    double length() const{
        return std::sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }
};

//ユーティリティ関数

//内積を計算する関数
inline double dot(const vec3& u, const vec3& v){
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

//外積を計算する関数
inline vec3 cross(const vec3& u, const vec3& v){
    return vec3(u.y() * v.z() - u.z() * v.y(), u.z() * v.x() - u.x() * v.z(), u.x() * v.y() - u.y() * v.x());
}

//ベクトルの正規化を行う関数、vec3の割り算は実装していないので、逆数にして定数倍
inline vec3 unit_vector(vec3 v){
    return v * (1.0 / v.length());
}

//0.0 以上 1.0 未満のランダムな実数を返す
inline double random_double(){
    return rand() / (RAND_MAX + 1.0);
}

//最小値 min と最大値 max の間のランダムな実数を返す
inline double random_double(double min, double max){
    return min + (max - min) * random_double();
}

//半径1の球体内のランダムな点（ベクトル）を生成する関数
inline vec3 random_in_unit_sphere(){
    while(true){
        //x, y, z それぞれ -1.0 〜 1.0 のランダムなベクトルを作る
        vec3 p = vec3(random_double(-1,1), random_double(-1,1), random_double(-1,1));
        //長さの2乗が1より小さければ（球の内部なら）採用
        if(p.length() * p.length() >= 1) continue;
        return p;
    }
}

//反射ベクトルを計算する関数
//R = v - 2(v・n)n
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

//屈折ベクトルを計算する関数（スネルの法則）
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    double cos_theta = std::fmin(dot(uv * -1.0, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + n * cos_theta);
    vec3 r_out_parallel = n * -std::sqrt(std::fabs(1.0 - dot(r_out_perp, r_out_perp)));
    return r_out_perp + r_out_parallel;
}

//ベクトルがゼロベクトルに極めて近いかを判定する関数
inline bool near_zero(const vec3& v) {
    const double s = 1e-8;
    return (std::fabs(v.x()) < s) && (std::fabs(v.y()) < s) && (std::fabs(v.z()) < s);
}


#endif