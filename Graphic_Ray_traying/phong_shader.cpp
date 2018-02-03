#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define swap(x,y) { x = x + y; y = x - y; x = x - y; }

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& same_side_normal,int recursion_depth) const
{
    vec3 color;
    //printf("phong shader\n");
    // TODO: determine the color
    // from constructor Phong_Shader(world, c0.vector, c1.vector, c2, vector, spec power)
    // world.ambient_color will not be assigned until 14.txt
    // world.ambient_intensity=f0;
    //printf("phong_shader");
    color = world.ambient_color * world.ambient_intensity * color_ambient;
    
for(int i=0;i< world.lights.size();i++){

    if(world.enable_shadows){
            vec3 P2S_vector = world.lights[i]->position - intersection_point;
            Ray P2S(intersection_point, P2S_vector.normalized());
            Hit hit;
            Object* objOutput = world.Closest_Intersection(P2S, hit);
            if(objOutput && hit.t<sqrt(dot(P2S_vector,P2S_vector)) ){
                //color += world.ambient_color * world.ambient_intensity * color_ambient;
                continue;
            }
    }
    
        vec3 L = world.lights[i]->position - intersection_point;
        //L = L.normalized();
        //std::max(0.0, dot( same_side_normal, L ) );
        //world.lights[0]->Emitted_Light(ray)/ dot(L,L);
        color += std::max(0.0, dot( same_side_normal, L.normalized() ) ) * (world.lights[i]->Emitted_Light(ray)/ dot(L,L) ) * color_diffuse;

    
        L = world.lights[i]->position - intersection_point;
        //L = L.normalized();
        vec3 reflect = 2*dot(L,same_side_normal)*same_side_normal-L;
        vec3 camera = ray.direction.normalized()*(-1);
        color += pow(std::max(0.0, dot(camera, reflect.normalized())), specular_power) * (world.lights[i]->Emitted_Light(ray)/ dot(L,L) ) * color_specular;
}
    return color;
}
