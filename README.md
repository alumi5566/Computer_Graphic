# Computer_Graphic
(1) Graphic_Ray_traying:
package: under OSX (apt-get under other Linux base environment)
    brew install libpng
    brew install gsl
    brew install scons

usage: compile by scons ( scons -c to clean )
    ./ray_tracer -i [txt file] to generate outpu.pngby
    ./ray_tracer -i [txt file] -s [cmp pic file] to compute diff with answer
    python grading-script.py . to run the grading script (00.txt - 37.txt)

ray tracing process is included in the report:
    graphic_chek_point_chenyang_yu_0125.pdf

(2) Graphic_miniGL:
usage: compile by scons ( scons -c to clean )
    ./minigl -gi [txt file] to compute diff with answer
    python grading-script.py . to run the grading script

(3) Final_project:
package: under OSX (apt-get under other Linux base environment)

usage: gcc demo.c solver.c -framework GLUT -framework OpenGL -Wno-deprecated
    ./a.out
