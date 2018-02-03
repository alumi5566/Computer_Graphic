#include "plane.h"
#include "ray.h"
#include <cfloat>


// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
bool Plane::
Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
    // TODO
    vec3 u = ray.endpoint;
    vec3 w = ray.direction;
    //double t = (-1)*dot(u,normal.normalized())/dot(w.normalized(),normal.normalized());
    double t = (dot(x1,normal)-dot(u,normal) )/ dot(w,normal);
    if(dot(w,normal.normalized())==0)
        return false;
    if(t>0){
        Object* obj=new Plane(x1,normal.normalized());
        Hit hitT;
        hitT.object = obj;
        hitT.t = t;
        hitT.ray_exiting = false;
        
        if(dot(w,normal) < 0){
            //Object* obj=new Plane(x1,normal.normalized());
            //Hit hitT;
            //hitT.object = obj;
            //hitT.t = t;
            //hitT.ray_exiting = false;
            hits.push_back(hitT);
            return true;
        }else{
            //Object* obj=new Plane(x1,normal.normalized());
            Hit hitT0;
            hitT0.object = obj;
            hitT0.t = 0;
            hitT0.ray_exiting = false;
            
            //Hit hitT;
            //hitT.object = obj;
            //hitT.t = t;
            hitT.ray_exiting = true;
            
            hits.push_back(hitT0);
            hits.push_back(hitT);
            return true;
        }
    }else{
        return false;
    }
    // should not reach here
    return false;
}

vec3 Plane::
Normal(const vec3& point) const
{
    return normal;
}
