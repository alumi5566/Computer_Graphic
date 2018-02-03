#if 1
#include "sphere.h"
#include "ray.h"


// Determine if the ray intersects with the sphere
bool Sphere::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
    // TODO
    // basic formula: |ray-center|^2 = r
    // the intersect point is u+tw
    vec3 v = ray.endpoint - center;
    vec3 w = ray.direction;
    double D = dot(w,v)*dot(w,v) - dot(w,w)*(dot(v,v)-radius*radius);
    if(dot(w,w)==0)
    	return false;
    
    if(D<0){
        return false;
    }else if(D==0){
        // set cutting as not intersect
        
        /*	double t1 = ( dot(w,v)*(-1) ) / dot(w,w);
            Object* obj=new Sphere(center,radius);
            Hit hitT1;
            hitT1.object = obj;
            hitT1.t = t1;
            hitT1.ray_exiting = false;
            hits.push_back(hitT1);
            
            hitT1.object = obj;
            hitT1.t = t1;
            hitT1.ray_exiting = true;
            hits.push_back(hitT1);
            
        return true;
        */
        return false;
    }else{
        double t1 = ( dot(w,v)*(-1)-sqrt(D) ) / dot(w,w);
        double t2 = ( dot(w,v)*(-1)+sqrt(D) ) / dot(w,w);
        // t1<t2
        if(t2>0){
          if(t1>0){
            //t1 and t2 both >0, we hv two intersection
            Object* obj=new Sphere(center,radius);
            Hit hitT1;
            hitT1.object = obj;
            hitT1.t = t1;
            hitT1.ray_exiting = false;
            
            Hit hitT2;
            hitT2.object = obj;
            hitT2.t = t2;
            hitT2.ray_exiting = true;
            
            hits.push_back(hitT1);
            hits.push_back(hitT2);
            return true;
          }else if(t1<=0){
            //intersection: t2
            Object* obj=new Sphere(center,radius);
            Hit hitT1;
            hitT1.object = obj;
            hitT1.t = 0;
            hitT1.ray_exiting = false;
            hits.push_back(hitT1);
            
            Hit hitT2;
            hitT2.object = obj;
            hitT2.t = t2;
            hitT2.ray_exiting = true;
            hits.push_back(hitT2);
            return true;
          }
        }else{
        	return false;
        }
        // if t2 == 0, endpoint is on sphere, t1 is outside
        // if t2 < 0, endpoint is outside, both not intersect
    }
    // should not run here
    return false;
}

vec3 Sphere::Normal(const vec3& point) const
{
    vec3 normal;
    // TODO: set the normal
    normal = point - center;
    return normal.normalized();
}

#endif

