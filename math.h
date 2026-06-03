#pragma once
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

class Vector3D {
public:
    double x, y, z;

    Vector3D(double x = 0.0, double y = 0.0, double z = 0.0)
        : x(x), y(y), z(z) {}

    Vector3D operator+(const Vector3D& v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
    Vector3D operator-(const Vector3D& v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
    Vector3D operator*(double s)          const { return Vector3D(x * s, y * s, z * s); }
    Vector3D operator/(double s)          const { return Vector3D(x / s, y / s, z / s); }
    Vector3D operator-()                  const { return Vector3D(-x, -y, -z); }

    double dot(const Vector3D& v) const { return x * v.x + y * v.y + z * v.z; }
    double length()               const { return sqrt(x * x + y * y + z * z); }

    Vector3D normalized() const {
        double len = length();
        if (len < 1e-8) return Vector3D(0, 0, 0);
        return (*this) / len;
    }

    Vector3D cross(const Vector3D& v) const {
        return Vector3D(y * v.z - z * v.y,
                        z * v.x - x * v.z,
                        x * v.y - y * v.x);
    }
};

inline Vector3D operator*(double s, const Vector3D& v) { return v * s; }

inline ostream& operator<<(ostream& out, const Vector3D& v) {
    out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return out;
}

class Ray {
public:
    Vector3D orig;
    Vector3D dir;

    Ray() : orig(), dir(Vector3D(1, 0, 0)) {}   
    Ray(const Vector3D& origin, const Vector3D& direction)
        : orig(origin), dir(direction.normalized()) {}

    Vector3D origin()    const { return orig; }
    Vector3D direction() const { return dir; }
    Vector3D at(double t) const { return orig + dir * t; }
};

namespace OpticsMath {

    Vector3D reflect(const Vector3D& incident, const Vector3D& normal) {
        return incident - 2.0 * incident.dot(normal) * normal;
    }

    Vector3D refract(const Vector3D& incident, const Vector3D& normal, double n1, double n2) {
        double eta = n1 / n2;
        Vector3D uv = incident.normalized();
        Vector3D n  = normal.normalized();

        double cosTheta   = fmin(-uv.dot(n), 1.0);
        Vector3D rOutPerp = eta * (uv + cosTheta * n);
        double discriminant = 1.0 - rOutPerp.dot(rOutPerp);

        if (discriminant < 0.0) return Vector3D(0, 0, 0);

        Vector3D rOutParallel = -sqrt(discriminant) * n;
        return rOutPerp + rOutParallel;
    }

    double angleBetween(const Vector3D& a, const Vector3D& b) {
        double lenA = a.length(), lenB = b.length();
        if (lenA < 1e-8 || lenB < 1e-8) return 0.0;
        double cosAngle = a.dot(b) / (lenA * lenB);
        cosAngle = fmax(-1.0, fmin(1.0, cosAngle));
        return acos(cosAngle);
    }

    double degToRad(double degrees) { return degrees * M_PI / 180.0; }
    double radToDeg(double radians) { return radians * 180.0 / M_PI; }
}

class Material {
public:
    string mat;
    double refractiveindex;
    Vector3D color;

    class MatEnt{
        public:
        string name;
        double n;
    };
    double index_of_refraction(string materialname) {
    vector<MatEnt> table = {
        {"Vacuum",          1.000},
        {"Air",             1.0003},
        {"Ice",             1.31},
        {"Water",           1.33},
        {"Ethyl Alcohol",   1.36},
        {"Fused Quartz",    1.46},
        {"Glycerine",       1.47},
        {"Acrylic",         1.49},
        {"Crown Glass",     1.52},
        {"Flint Glass",     1.66},
        {"Diamond",         2.42},
        {"Plate Glass",     1.52},
        {"Salt",            1.544},
        {"Silicon",         3.480},
        {"Germanium",       4.050},
        {"Cornea",          1.373},
        {"Blood",           1.354},
        {"Vitreous Humour", 1.336}
    };

    for (int i = 0; i < (int)table.size(); i++) {
        if (materialname == table[i].name)
            return table[i].n;
    }
    cout << "Unknown material: " << materialname << endl;
    return 1.0;
}
Material(){
    mat = "Air";
    refractiveindex = index_of_refraction(mat);
    color =Vector3D(1,1,1);
}
Material(string material, const Vector3D& col=Vector3D (1,1,1)){
    mat= material;
    refractiveindex= index_of_refraction(mat);
    color=col;
}
};
struct HitRecord {
    double t;
    Vector3D point;
    Vector3D normal;
    bool frontFace;
    Material material1;
    Material material2;

    HitRecord(){
        t=0;
        frontFace=true;
        material1 =Material ("Vacuum");
        material2 =Material ("Vacuum");
    }

    void setFaceNormal(const Ray& ray, const Vector3D& outwardNormal) {
        frontFace = ray.direction().dot(outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};