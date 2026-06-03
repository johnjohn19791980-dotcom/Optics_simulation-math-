#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "math.h"
using namespace std;

class surface {
public:
    virtual ~surface(){}    
    virtual bool intersect(Ray& ray, HitRecord& rec) = 0;
    virtual bool interact(Ray& inray, HitRecord& rec, Ray& outray) = 0;
    virtual Vector3D normal(Vector3D& p) = 0;
    virtual string describe() = 0;
    virtual double focalpoint() { return 0; }

};

class flatMirror : public surface {
private:
    Vector3D point;
    Vector3D n;
    Material mater;
public:
    flatMirror(Vector3D p, Vector3D normal, string m)  {
        point = p;
        n = normal.normalized();
        mater =Material (m);
    }

    bool intersect(Ray& ray, HitRecord& rec) override {
        double denominator = ray.direction().dot(n);
        if (abs(denominator) < 0.0001)
            return false;

        double t = (point - ray.origin()).dot(n) / denominator;
        if (t < 0.001)
            return false;
        rec.material1=mater;
        rec.t = t;
        rec.point = ray.at(t);
        rec.setFaceNormal(ray, n);
        return true;
    }

    bool interact(Ray& inray, HitRecord& rec, Ray& outray) override {
        Vector3D reflected = OpticsMath::reflect(inray.direction(), rec.normal);
        outray = Ray(rec.point + rec.normal* 0.001, reflected);
        return true;
    }

    Vector3D normal(Vector3D& p) override {
        return n;
    }

    string describe() override {
        return "Flat mirror";
    }
    Vector3D getPoint()  const { return point; }
    Vector3D getNormal() const { return n; }
};
class concavemirror : public surface {

private:

    Vector3D centre;
    Vector3D axis;
    Vector3D vertex;
    double R;
    Material mat;

public:

    concavemirror(Vector3D Axis,Vector3D Vertex,double Radius, string Material_Type) {
        axis = Axis.normalized();
        vertex = Vertex;
        R = Radius;
        centre = vertex + axis * R;
        mat = Material(Material_Type);
    }


    Vector3D normal(Vector3D& p) override {
        return (centre - p).normalized();
    }

    bool intersect(Ray& ray, HitRecord& rec) override {
        Vector3D oc = ray.origin() - centre;
        double a = ray.direction().dot(ray.direction());
        double b = 2.0 * oc.dot(ray.direction());
        double c = oc.dot(oc) - R * R;
        double disc = b*b - 4*a*c;
        if (disc < 0.0)
            return false;
        double s = sqrt(disc);
        double t1 = (-b - s) / (2.0 * a);
        double t2 = (-b + s) / (2.0 * a);
        std::vector<double> candidates;
        if (t1 > 0.001)
            candidates.push_back(t1);
        if (t2 > 0.001)
            candidates.push_back(t2);
        double t = -1;
        bool found = false;
        for (double candidate : candidates) {
            Vector3D hitPoint = ray.at(candidate);
            Vector3D toHit =
                (hitPoint - centre).normalized();
            if (toHit.dot(axis) <= 0) {
                t = candidate;
                found = true;
                break;
            }
        }
        if (!found){
            return false;
        }
        rec.t = t;
        rec.point = ray.at(t);
        rec.material1 = mat;
        rec.setFaceNormal(ray, normal(rec.point));
        return true;
    }
    bool interact(Ray& inray, HitRecord& rec, Ray& outray) override {
        Vector3D reflected =
            OpticsMath::reflect(inray.direction(), rec.normal);
        outray =Ray( rec.point + reflected * 0.001,reflected);
        return true;
    }
    double focalpoint() override {
        return R / 2.0;
    }

    string describe() override {
        return "Concave mirror";
    }

    Vector3D getVertex() const { return vertex; }
    Vector3D getAxis() const { return axis; }
    double getRadius() const { return R; }
};

class convexmirror: public surface{
    private: 
    Vector3D centre;
    Vector3D axis;
    Vector3D vertex; 
    double R;
    Material mat;
    public:
    convexmirror( Vector3D Axis, Vector3D Vertex, double Radius,string Material_Type){
        axis = Axis.normalized();
        vertex = Vertex;
        R = Radius;
        centre =Vertex - axis*R;
        mat = Material(Material_Type);
    }
    Vector3D normal(Vector3D& p){
        Vector3D n = (p-centre).normalized();
        return n ;
    }
    bool intersect (Ray & ray, HitRecord&rec)override {
        Vector3D oc = ray.origin() - centre;
        double a = ray.direction().dot(ray.direction());
        double b = 2.0 * oc.dot(ray.direction());
        double c = oc.dot(oc) - R * R;
        double disc = b * b - 4 * a * c;

        if (disc < 0) return false;

        double s = sqrt(disc);
        double t1 = (-b - s) / (2.0 * a);
        double t2 = (-b + s) / (2.0 * a);
        double t;

        if (t1 > 0.001)
            t = t1;
        else if (t2 > 0.001)
            t = t2;
        else
            return false;
        rec.t = t;
        rec.point = ray.at(t);
        Vector3D hitDir = (rec.point - vertex).normalized();
        if (hitDir.dot(axis) > 0)
            return false;
        rec . material1=mat;
        rec.setFaceNormal(ray, normal(rec.point));
        return true;
    }
     bool interact(Ray& inray, HitRecord& rec, Ray& outray) override {
        Vector3D reflected = OpticsMath::reflect(inray.direction(), rec.normal);
        outray = Ray(rec.point + rec.normal * 0.001, reflected);
        return true;
    }
    double focalpoint() override { 
        return -R / 2.0;
    }   
    string describe() override {
        return "Convex mirror  R=" + to_string(R) + "  f=" + to_string(focalpoint());
    }
    Vector3D getVertex() const { return vertex; }
    Vector3D getAxis()   const { return axis; }
    double   getRadius() const { return R; }

};
class flatRefractor : public surface {
private:
    Vector3D point;
    Vector3D norm;
    Material mat1;
    Material mat2;
public:
    flatRefractor(Vector3D p, Vector3D n, string material1, string material2)  {
        point = p;
        norm = n.normalized();
        mat1 = Material (material1);
        mat2 = Material(material2);
    }

    bool intersect(Ray& ray, HitRecord& rec) override {
        double denominator = ray.direction().dot(norm);
        if (abs(denominator) < 0.0001)
            return false;

        double t = (point - ray.origin()).dot(norm) / denominator;
        if (t < 0.001)
            return false;

        rec.t = t;
        rec.point = ray.at(t);
        rec.setFaceNormal(ray, norm);
        rec.material1=mat1;
        rec.material2=mat2;
        return true;
    }

    bool interact(Ray& inRay, HitRecord& rec, Ray& outRay) override {
        double fromIndex;
        double toIndex;

        if (rec.frontFace) {
            fromIndex =rec.material1.refractiveindex;
            toIndex = rec.material2.refractiveindex;
        } else {
            fromIndex = rec.material2.refractiveindex;
            toIndex =rec.material1.refractiveindex;
        }

        Vector3D refractedDir = OpticsMath::refract(inRay.direction(), rec.normal, fromIndex, toIndex);
        if (refractedDir.length() < 0.0001){
            Vector3D reflected = OpticsMath::reflect(inRay.direction(), rec.normal);
            outRay = Ray(rec.point + rec.normal * 0.001, reflected);
            return true;
        }
        outRay = Ray(rec.point + refractedDir * 0.01, refractedDir);
        return true;
    }

    Vector3D normal(Vector3D& p) override {
        return norm;
    }

    string describe() override {
        return "Flat refractor n1=" + to_string(mat1.refractiveindex) + " n2=" + to_string(mat2.refractiveindex);
    }
    Vector3D getPoint()  const { return point; }
    Vector3D getNormal() const { return norm; }
};

class sphericallens : public surface {
private:
    Vector3D center;
    Vector3D axis;
    double R;
    Material mat1;
    Material mat2;
public:
    sphericallens(Vector3D c, Vector3D a, double r, string material1, string material2) {
        center = c;
        axis = a.normalized();
        R = r;
        mat1 = Material (material1);
        mat2 = Material(material2);
    }

    bool intersect(Ray& ray, HitRecord& rec) override {
        Vector3D orig = ray.origin() - center;
        double a = ray.direction().dot(ray.direction());
        double b = 2 * ray.direction().dot(orig);
        double c = orig.dot(orig) - R * R;
        double disc = b * b - 4.0 * a * c;

        if (disc < 0) return false;

        double sqrtDisc = sqrt(disc);
        double t1 = (-b - sqrtDisc) / (2.0 * a);
        double t2 = (-b + sqrtDisc) / (2.0 * a);
        double t;

    if (t1 > 0.001)
        t = t1;
    else if (t2 > 0.001)
        t = t2;
    else
        return false;

        rec.t = t;
        rec.point = ray.at(t);
        rec.setFaceNormal(ray, normal(rec.point));
        rec.material1=mat1;
        rec.material2=mat2;
        return true;
    }
        
    bool interact(Ray& inray, HitRecord& rec, Ray& outray) override {
        
        double incoming;
        double outcoming;
        if (rec.frontFace) {
            incoming = rec.material1.refractiveindex;
            outcoming = rec.material2.refractiveindex;
        } else {
            incoming = rec.material2.refractiveindex;
            outcoming = rec.material1.refractiveindex;
        }

        Vector3D refracted = OpticsMath::refract(inray.direction(), rec.normal, incoming, outcoming);
        if (refracted.length() < 0.001) {
            Vector3D reflected = OpticsMath::reflect(inray.direction(), rec.normal);
            outray = Ray(rec.point + rec.normal * 0.001, reflected);
            return true;
        }

        outray = Ray(rec.point + refracted * 0.01, refracted);
        return true;
    }

    Vector3D normal(Vector3D& p) override {
        return ((p - center) / R).normalized();
    }

    string describe() override {
        return "Spherical Lens R=" + to_string(R) + " n1=" + to_string(mat1.refractiveindex) + " n2=" + to_string(mat2.refractiveindex);
    }

    double focalpoint() override {
        return R * mat2.refractiveindex / (mat2.refractiveindex - mat1.refractiveindex);
    }
    Vector3D getCenter() const { return center; }
    double   getRadius() const { return R; }
};

class Scene {
private:
    vector<surface*> surfaces;
public:
 ~Scene() {
        for (int i = 0; i < (int)surfaces.size(); i++) {
            delete surfaces[i];
        }
    }
    void add(surface* s) {
        surfaces.push_back(s);
    }

    bool hitClosest( Ray& ray, HitRecord& rec) {
        bool hit = false;
        double closest = 1e18;

        for (int i = 0; i < (int)surfaces.size(); i++) {
            HitRecord temp;
            if (surfaces[i]->intersect(ray, temp)) {
                if (temp.t > 0.001 && temp.t < closest) {
                    closest = temp.t;
                    rec = temp;
                    hit = true;
                }
            }
        }
        return hit;
    }

    const vector<surface*>& getSurfaces() const {
        return surfaces;
    }

    void trace( Ray& ray, int& maxSteps) {
        cout << "Ray starts at: " << ray.origin() << "\n";

        for (int step = 0; step < maxSteps; step++) {
            HitRecord rec;

            if (!hitClosest(ray, rec)) {
                cout << "Step " << step + 1 << ": ray escaped scene\n";
                return;
            }

            cout << "Step " << step + 1 << ": hit at " << rec.point << "\n";

            Ray outRay;
            bool success = false;

            for (int i = 0; i < (int)surfaces.size(); i++) {
                HitRecord temp;
                if (surfaces[i]->intersect(ray, temp)) {
                    if (abs(temp.t - rec.t) < 0.0001) {
                        success = surfaces[i]->interact(ray, rec, outRay);
                        break;
                    }
                }
            }

            if (!success) {
                cout << "Step " << step + 1 << ": ray stopped\n";
                return;
            }

            cout << "Step " << step + 1 << ": out direction " << outRay.direction() << "\n";
            ray = outRay;
        }
    }

    void describe() {
        cout << "Scene has " << surfaces.size() << " surfaces:\n";
        for (int i = 0; i < (int)surfaces.size(); i++) {
            cout << "[" << i << "] " << surfaces[i]->describe() << "\n";
        }
    }
};