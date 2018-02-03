#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"


Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find the closest object of intersection and return the object that was
// intersected.  Record the Hit structure in hit.  If no intersection occurred,
// return NULL.  Note that in the case of a Boolean, the object returned will be
// the Boolean, but the object stored in hit will be the underlying primitive.
// Any intersection with t<=small_t should be ignored.
Object* Render_World::Closest_Intersection(const Ray& ray, Hit& hit)
{
    // TODO
    // 230: check parse.cpp, all object should store in world.objects
    double distance = std::numeric_limits<double>::max();
    Object* objOutput;
    bool flag = false;
    //printf("obj size = %lu\n",objects.size());
    vec3 tmp = vec3(0.066576,-0.262063,-0.962752);
    //tmp = tmp.normalized();
    vec3 tmp2 = ray.direction;
    tmp2 /= 0.000001;
    //if(tmp2[0] == tmp[0] && tmp2[1] == tmp[1] && tmp2[2] == tmp[2])
    if(tmp2[0] == 66576 && tmp2[1] == -262063 && tmp2[2] == -962752)
        printf("should be on blue\n");
    
    for(int i=0;i<objects.size();i++){
    // for every obj in the world, check if intersect
        Object* objTmp = objects[i];
        std::vector<Hit> HitTmp;
        if(objTmp->Intersection(ray,HitTmp)){
            // the shortest distance is in HitTmp[0], no matter 1 or 2 intersect
            for(int i=0;i<HitTmp.size();i++){
              if(HitTmp[i].t < distance && HitTmp[i].t>small_t ){ //&&!HitTmp[0].ray_exiting
                // beware that distance might = 0
                if(HitTmp[i].t==0)
                	printf("we v a zero\n");
                objOutput = objTmp;
                distance = HitTmp[i].t;
                
                hit = HitTmp[i];
                //if(HitTmp[0].t != HitTmp[1].t)
                //	hit = HitTmp[0];
                //else
                //	hit = HitTmp[1];
                flag = 1;
                //printf("Hit: obj[%d], distance:%f,Bool:%d\n",i,HitTmp[0].t,HitTmp[0].ray_exiting);
              }
            }
        }

    }
    
    if(flag)
        return objOutput;
    else
        return NULL;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    //Ray ray; // TODO: set up the initial view ray here
    const vec3 CP = camera.position;
    //care with zero vector?
    const vec3 DIR = (camera.World_Position(pixel_index)-camera.position);
    Ray ray(camera.position, DIR.normalized());
    if(pixel_index[0] == 350 && pixel_index[1] == 230){
    	//230
    	//printf("ray = %f %f %f\n", ray.endpoint[0],ray.endpoint[1],ray.endpoint[2]);
    	//printf("dir = %f %f %f\n", ray.direction[0],ray.direction[1],ray.direction[2]);
    }
    // 230 after we have the pixel position in the world
    // we hv the ray in the world (p_pos - camera_pos)
    
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{
    // TODO
    vec3 color;
    // not hit: BG
    // hit: color
    Hit hit;
    Object* objInter = Closest_Intersection(ray,hit);
    vec3 dummy;
    if(recursion_depth > recursion_depth_limit)
        return color=background_shader->Shade_Surface(ray,dummy,dummy,recursion_depth);
    
    if( objInter){
        //if(!objInter->type)
        //    vec3 norm = objInter->Normal(ray.Point(hit.t));
        //else
        //!!vec3 norm = objInter->Normal(ray.Point(hit.t));
        //printf("HERE\n");
        vec3 norm = hit.object->Normal(ray.Point(hit.t)).normalized();
        //printf("HERE2\n");
        if(hit.ray_exiting)
            norm *= (-1.0);
        color=objInter->material_shader->Shade_Surface(ray,ray.Point(hit.t),norm,recursion_depth);
        //color+=objInter->
    }else{
        vec3 dummy;
        color=background_shader->Shade_Surface(ray,dummy,dummy,recursion_depth);
    }
    
    // determine the color here

    return color;
}
