import os
env = Environment(ENV = os.environ)

env.Append(LIBS=["png"])
env.Append(CXXFLAGS=["-std=c++11","-g","-Wall","-O3","-I/usr/include/libpng12"])
env.Append(LINKFLAGS=["-L/usr/local/lib"])

env.Program("ray_tracer",["boolean.cpp","camera.cpp",
             "dump_png.cpp","flat_shader.cpp","main.cpp","parse.cpp",
             "phong_shader.cpp","plane.cpp","reflective_shader.cpp",
             "render_world.cpp","sphere.cpp"])

