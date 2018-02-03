#include "light.h"
#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& same_side_normal,int recursion_depth) const
{
    vec3 color;
    // TODO: determine the color
    //vec3 ambient_color, diffuse_color, specular_color;
    //ambient_color = world.ambient_color
    //printf("reflect shader\n");
    vec3 CO,CR; // original color and reflect color
    
    //for(int i = 0; i < world.lights.size(); i++){
        
        //vec3 L = (world.lights[i]->position - intersection_point).normalized();
        vec3 camera=(ray.endpoint - intersection_point).normalized();
        vec3 reflect = 2*dot(camera,same_side_normal)*same_side_normal-camera;
        Ray C2R(intersection_point, reflect.normalized());
        // use camera to calculate reflect
        CR = world.Cast_Ray(C2R,recursion_depth+1);
        CO = this->shader->Shade_Surface(ray, intersection_point, same_side_normal, recursion_depth);
        
        color += CO + reflectivity*(CR-CO);
        
    //}
    
    return color;
}
