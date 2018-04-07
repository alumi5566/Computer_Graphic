/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 *
 * You may include minigl.h and any of the standard C++ libraries.
 * No other includes are permitted.  Other preprocessing directives
 * are also not permitted.  These requirements are strictly
 * enforced.  Be sure to run a test grading to make sure your file
 * passes the sanity tests.
 *
 * The behavior of the routines your are implenting is documented here:
 * https://www.opengl.org/sdk/docs/man2/
 * Note that you will only be implementing a subset of this.  In particular,
 * you only need to implement enough to pass the tests in the suite.
 */

#include "minigl.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>
#include <stack>
#include <cstdio>

using namespace std;

/**
 * Useful data types
 */
typedef mat<MGLfloat,4,4> mat4; //data structure storing a 4x4 matrix, see mat.h
typedef mat<MGLfloat,3,3> mat3; //data structure storing a 3x3 matrix, see mat.h
typedef vec<MGLfloat,4> vec4;   //data structure storing a 4 dimensional vector, see vec.h
typedef vec<MGLfloat,3> vec3;   //data structure storing a 3 dimensional vector, see vec.h
typedef vec<MGLfloat,2> vec2;   //data structure storing a 2 dimensional vector, see vec.h
#define PI 3.14159265


/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
    printf("%s\n", description);
    exit(1);
}
// RGB to store color
vec3 RGB;
// store 4d posotion and color
struct Vertex{
	vec4 position;
	vec3 color;
	// constructor
	Vertex(MGLfloat x,MGLfloat y,MGLfloat z,MGLfloat w,vec3 RGB):
		position(x,y,z,w),color(RGB){}
};
vector<Vertex> list_vec3;

// type will be assign in mglBegin()
MGLpoly_mode type;
// type will be assign in mglMatrixMode()
MGLmatrix_mode matrix_type;

// store for triangle
struct Triangle{
	Vertex A,B,C;
	// constructor
	Triangle(Vertex A, Vertex B, Vertex C):
		A(A),B(B),C(C){}
};
vector<Triangle> list_Triangle;

mat4 Tmp = {{1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1}};
mat4 curMatrix = {{1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1}};
stack<mat4>projectStack;			
stack<mat4>modelStack;

vector<mat4>projectVector;
	//projectVector[0] = Tmp;
vector<mat4>modelVector;
	//modelVector[0] = Tmp;

void outputMat(mat4 input){
	for(int i=0;i<16;i++){
		printf("%.0f ",input.values[i]);
		if(i%4==3)
			printf("\t");
	}
	printf("\n");
}

void outputVec3(Vertex input){
	for(int i=0;i<3;i++){
		printf("%.3f ",input.position[i]);
	}
	printf("\n");
}

int WIDTH = 0;
int HEIGHT = 0;
MGLpixel *mFrameBuffer;
MGLfloat *mZBuffer;

#if 0
void Rasterize_Triangle(const Triangle& tri, int width, int height, MGLpixel *data){

	// Triangle is composed of Vector, which is composed of position and color
		double ax=((tri.A.position[0]/tri.A.position[3]+1)*width)/2-0.5;
		double ay=((tri.A.position[1]/tri.A.position[3]+1)*height)/2-0.5;
		double bx=((tri.B.position[0]/tri.B.position[3]+1)*width)/2-0.5;
		double by=((tri.B.position[1]/tri.B.position[3]+1)*height)/2-0.5;
		double cx=((tri.C.position[0]/tri.C.position[3]+1)*width)/2-0.5;
		double cy=((tri.C.position[1]/tri.C.position[3]+1)*height)/2-0.5;
		
		int minX = (int) floor(min(ax, min(bx,cx)) );
		int minY = (int) floor(min(ay, min(by,cy)) );
		int maxX = (int) ceil(max(ax, max(bx,cx)) );
		int maxY = (int) ceil(max(ay, max(by,cy)) );
		minX = max(0,minX);
		minY = max(0,minY);
		maxX = min(maxX,width);
		maxY = min(maxY,height);
		
		double abc = (0.5)*(ax*by-ay*bx+bx*cy-by*cx+cx*ay-cy*ax);
		//for(int i=0;i<width;i++){
		//	for(int j=0;j<height;j++){
		for(int i=minX;i<maxX;i++){
			for(int j=minY;j<maxY;j++){
				//data[i+j*width]=Make_Pixel(Tri.);
				double pab = (0.5)*(ax*by-ay*bx+bx*j-by*i+i*ay-j*ax);
				double pbc = (0.5)*(i*by-j*bx+bx*cy-by*cx+cx*j-cy*i);
				double pac = (0.5)*(ax*j-ay*i+i*cy-j*cx+cx*ay-cy*ax);
				double alpha = pbc/abc;
				double beta = pac/abc;
				double gamma = pab/abc;
				if(alpha>=0 && alpha<=1){
					if(beta>=0 && beta<=1){
						if(gamma>=0 && gamma<=1){
							//cout<<"inside Tri"<<endl;
							//printf("R%f G%f B%f\n",tri.A.color[0],tri.A.color[1],tri.A.color[2]);
							//data[i+j*width]=Make_Pixel(255,255,255);
							data[i+j*width]=Make_Pixel(tri.B.color[0]*255,tri.B.color[1]*255,tri.B.color[2]*255);
						}// if gamma
					}//if beta
				}// if alpha
			}
		}
}
#endif

void set_pixel(vec3 color, int x, int y, float depth)
{

    if((x < 0) || (x > WIDTH) || (y < 0) || (y > HEIGHT)){
		return ;
	}

	if(depth < mZBuffer[(WIDTH*y) + x]){
		mZBuffer[(WIDTH*y) + x] = depth;
		mFrameBuffer[(WIDTH*y) + x]=Make_Pixel(color[0]*255,color[1]*255,color[2]*255);
		return;	
	}
}

void Rasterize_Triangle(const Triangle& tri, int width, int height){

	// Triangle is composed of Vector, which is composed of position and color
		double ax=((tri.A.position[0]/tri.A.position[3]+1)*width)/2-0.5;
		double ay=((tri.A.position[1]/tri.A.position[3]+1)*height)/2-0.5;
		double bx=((tri.B.position[0]/tri.B.position[3]+1)*width)/2-0.5;
		double by=((tri.B.position[1]/tri.B.position[3]+1)*height)/2-0.5;
		double cx=((tri.C.position[0]/tri.C.position[3]+1)*width)/2-0.5;
		double cy=((tri.C.position[1]/tri.C.position[3]+1)*height)/2-0.5;
		
		int minX = (int) floor(min(ax, min(bx,cx)) );
		int minY = (int) floor(min(ay, min(by,cy)) );
		int maxX = (int) ceil(max(ax, max(bx,cx)) );
		int maxY = (int) ceil(max(ay, max(by,cy)) );
		minX = max(0,minX);
		minY = max(0,minY);
		maxX = min(maxX,width);
		maxY = min(maxY,height);
		
		//double az = ((tri.A.position[3]/tri.A.position[3]+1))/2-0.5;
		//double bz = ((tri.B.position[3]/tri.B.position[3]+1))/2-0.5;
		//double cz = ((tri.C.position[3]/tri.C.position[3]+1))/2-0.5;
		
		double abc = (0.5)*(ax*by-ay*bx+bx*cy-by*cx+cx*ay-cy*ax);
		//for(int i=0;i<width;i++){
		//	for(int j=0;j<height;j++){
		for(int i=minX;i<maxX;i++){
			for(int j=minY;j<maxY;j++){
				//data[i+j*width]=Make_Pixel(Tri.);
				double pab = (0.5)*(ax*by-ay*bx+bx*j-by*i+i*ay-j*ax);
				double pbc = (0.5)*(i*by-j*bx+bx*cy-by*cx+cx*j-cy*i);
				double pac = (0.5)*(ax*j-ay*i+i*cy-j*cx+cx*ay-cy*ax);
				double alpha = pbc/abc;
				double beta = pac/abc;
				double gamma = pab/abc;
							
				if(alpha>=0 && alpha<=1){
					if(beta>=0 && beta<=1){
						if(gamma>=0 && gamma<=1){
							
							// this is for perspective correct interpolation
							// fix the alpha/ beta/ gamma
							double wa=tri.A.position[3];
							double wb=tri.B.position[3];
							double wc=tri.C.position[3];
							double alpha2 = (alpha/wa);
							double beta2 = (beta/wb);
							double gamma2 = (gamma/wc);
							alpha = alpha2/(alpha2+beta2+gamma2);
							beta = beta2/(alpha2+beta2+gamma2);
							gamma = gamma2/(alpha2+beta2+gamma2);
							// for perspective correct interpolation
							
							//cout<<"inside Tri"<<endl;
							//printf("R%f G%f B%f\n",tri.A.color[0],tri.A.color[1],tri.A.color[2]);
							//data[i+j*width]=Make_Pixel(255,255,255);
							double depth = alpha*tri.A.position[2] + beta*tri.B.position[2] + gamma*tri.C.position[2];
							//float depth = alpha*az + beta*bz + gamma*cz;
							//set pixel (i,j) as color and depth
							vec3 colorMix;
							colorMix[0] = (alpha*tri.A.color[0])+(beta*tri.B.color[0])+ (gamma*tri.C.color[0]);
							colorMix[1] = (alpha*tri.A.color[1])+(beta*tri.B.color[1])+ (gamma*tri.C.color[1]);
							colorMix[2] = (alpha*tri.A.color[2])+(beta*tri.B.color[2])+ (gamma*tri.C.color[2]);
							
							
							set_pixel(colorMix, i, j, depth);
							//data[i+j*width]=Make_Pixel(tri.B.color[0]*255,tri.B.color[1]*255,tri.B.color[2]*255);
						}// if gamma
					}//if beta
				}// if alpha
			}
		}
}



/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width,
                   MGLsize height,
                   MGLpixel *data)
{
	// rasterize triangles onto pixels
	// set color pixel by pixel
	//for(int i=0;i<height*width;i++){
	//	data[i]=Make_Pixel(0,0,0);
	//}
	WIDTH = width;
	HEIGHT = height;
	mFrameBuffer = new  MGLpixel[(width*height)];
	mZBuffer = new  MGLfloat[(width*height)];
	// initialize the two buffers 
	for(int i = 0; i < width*height; i++){
		mZBuffer[i] =  INFINITY; // very far
		mFrameBuffer[i] = 0; // all black
	}

	//rasterize all triangle
	
	for(int k=0;k<list_Triangle.size();k++){
		//printf("#%d Tri:\n",k);
		//outputVec3(list_Triangle[k].A);
		//outputVec3(list_Triangle[k].B);
		//outputVec3(list_Triangle[k].C);
		Rasterize_Triangle(list_Triangle[k],width,height);
	}
	for(int i = 0; i < width*height; i++){
		data[i] = mFrameBuffer[i];
	}	
	// clean triangle list in case next begin goes wrong
	while(!list_Triangle.empty())
		list_Triangle.pop_back();
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
	type = mode;
}

vector<Vertex> list_tmp;
#if 0
// check 6 plane of cube (-1,-1,-1):(1,1,1)
void InCube(vec3 A, vec3 B){
	vec3 n1(0,0,1);
	vec3 n2(0,1,0);
	vec3 n3(1,0,0);
	vec3 n4(0,0,-1);
	vec3 n5(0,-1,0);
	vec3 n6(-1,0,0);
	vec3 p1(1,-1,1);
	vec3 p2(1,1,-1);
	vec3 p3(1,1,1);
	vec3 p4(-1,1,-1);
	vec3 p5(-1,-1,1);
	vec3 p6(-1,1,1);
	vec3 N[6]={n1,n2,n3,n4,n5,n6};
	vec3 P[6]={p1,p2,p3,p4,p5,p6};
	for(int i=0;i<6;i++)
		double s = dot(n1,(p1-A))/dot(n1,B-A);
}
#endif
/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
	int n = list_vec3.size();
	if(type == MGL_TRIANGLES){
		//cout<<"TRI:"<<endl;
		if(n%3!=0)
			printf("TRI:wrong num\n");
		while(!list_vec3.empty()){
			Vertex A = list_vec3.back();
			list_vec3.pop_back();
			Vertex B = list_vec3.back();
			list_vec3.pop_back();
			Vertex C = list_vec3.back();
			list_vec3.pop_back();
			Triangle Tri(A,B,C);
			list_Triangle.push_back(Tri);
		}
		
	}else if(type == MGL_QUADS){
		if(n%4!=0)
			printf("QUAD:wrong num\n");
		while(!list_vec3.empty()){
			Vertex A = list_vec3.back();
			list_vec3.pop_back();
			Vertex B = list_vec3.back();
			list_vec3.pop_back();
			Vertex C = list_vec3.back();
			list_vec3.pop_back();
			Vertex D = list_vec3.back();
			list_vec3.pop_back();
			Triangle Tri1(A,B,C);
			Triangle Tri2(A,C,D);
			list_Triangle.push_back(Tri1);
			list_Triangle.push_back(Tri2);
		}
						
	}else{
		printf("unexpected type\n");
	}
	// clean list in case next begin goes wrong
	//if(!list_vec3.empty())
	//	printf("end of mglEnd, something left\n");
	
	// check the triangle here, clip if necessary
#if 0
	if(type == MGL_TRIANGLES){
		for(int i=0;i<list_Triangle.size();i++){
			printf("check the #%d Tri\n",i);
			//list_Triangle.push_back(Tri);
			/*pop_Tri()
			InCube(list_Triangle[i].A,list_Triangle[i].B);
			InCube(list_Triangle[i].B,list_Triangle[i].C);
			InCube(list_Triangle[i].C,list_Triangle[i].A);
			now list_tmp hold the right Vectex
			*/
		}
		
	}
#endif
	while(!list_vec3.empty())
		list_vec3.pop_back();
	//clear list_tmp also
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y)
{
	mglVertex3(x,y,0);
}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
	//printf("R%f G%f B%f\n",RGB[0],RGB[1],RGB[2]);
	// save the input as a matrix
	mat<MGLfloat,4,1> matV3{{x,y,z,1}};
	// Tmp is store the R, default is identity matrix
	//matV3 = Tmp*matV3;
	//printf("tmp0=%f\n",Tmp.values[0]);
	if(matrix_type == MGL_MODELVIEW){
		//modelStack.push(curMatrix);
		/*if(!projectStack.empty()){
			printf("HERE\n");
			matV3 = projectStack.top()*curMatrix*matV3;
		}else{
			printf("HERE2\n");*/
			//matV3 = curMatrix*matV3;}
	}else if(matrix_type == MGL_PROJECTION){
		/*if(!modelStack.empty())
			matV3 = modelStack.top()*curMatrix*matV3;
		else
			matV3 = curMatrix*matV3;*/
		
	}
	//printf("Vec3:Model Count=%lu\n",modelVector.size());
	//outputMat(modelVector[0]);
	//printf("Vec3:Project Count=%lu\n",projectVector.size());
	//outputMat(projectVector[0]);
	matV3 = projectVector.back()*modelVector.back()*matV3;
	//matV3 = curMatrix*matV3;
	//printf("curMatrix0=%f\n",Tmp.values[0]);
	
	//Vertex V(x,y,z,RGB);
	Vertex V(matV3.values[0],matV3.values[1],matV3.values[2],matV3.values[3],RGB);
	list_vec3.push_back(V);
}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
	/*if(matrix_type == MGL_MODELVIEW){
		//projectStack.push(curMatrix);
	}else if(matrix_type == MGL_PROJECTION){
		//modelStack.push(curMatrix);
	}*/
	if(mode == MGL_PROJECTION){
		//printf("Mode:Count=%lu,%lu\n",modelVector.size(),projectVector.size());
		//printf("Mode2:Count=%lu,%lu\n",list_Triangle.size(),list_vec3.size());
		/*while(!projectVector.empty())
			projectVector.pop_back();
		while(!modelVector.empty())
			modelVector.pop_back();
		*/
		vector<mat4>::iterator iter=modelVector.begin();
		for( ;iter!=modelVector.end();){
 		   iter=modelVector.erase(iter);
		}
		iter=projectVector.begin();
		for( ;iter!=projectVector.end();){
 		   iter=projectVector.erase(iter);
		}
		//projectVector.clear();
		//modelVector.clear();
	}
	
	if(modelVector.empty())
		modelVector.push_back(Tmp);
	if(projectVector.empty())
		projectVector.push_back(Tmp);
	//printf("Mode:Vector Count=%d,%d\n",modelVector.size(),projectVector.size());
	matrix_type = mode;
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{
	if(matrix_type == MGL_MODELVIEW){
		if(!modelVector.empty())
			modelVector.push_back(modelVector.back());
		else
			modelVector.push_back(Tmp);
		//modelStack.push(curMatrix);
	}else if(matrix_type == MGL_PROJECTION){
		if(!projectVector.empty())
			projectVector.push_back(projectVector.back());
		else
			projectVector.push_back(Tmp);
		//projectStack.push(curMatrix);
	}
}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{	
	if(matrix_type == MGL_MODELVIEW && !modelVector.empty()){
		//curMatrix = modelStack.top();
		//modelStack.pop();
		modelVector.pop_back();
	}else if(matrix_type == MGL_PROJECTION && !projectVector.empty()){
		//curMatrix = projectStack.top();
		//projectStack.pop();
		projectVector.pop_back();
	}
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
	for(int i=0;i<16;i++)
		curMatrix.values[i]=0;
	curMatrix.values[0]=1;
	curMatrix.values[5]=1;
	curMatrix.values[10]=1;
	curMatrix.values[15]=1;
	
}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix)
{
	mat4 A;
	for(int i=0;i<16;i++){
		A.values[i] = matrix[i];
	}
	if(matrix_type == MGL_MODELVIEW){
		//mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		//mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(A);
		//projectVector.back() = projectVector.back() * A;
	}
}

/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix)
{
	mat4 A;
	for(int i=0;i<16;i++){
		A.values[i] = matrix[i];
	}
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(tmp*A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(tmp*A);
		//projectVector.back() = projectVector.back() * A;
	}
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{
	mat4 A={{1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1}};
			//Tmp = A;
			curMatrix = A*curMatrix;
			
	//printf("mglOrtho:Count=%lu,%lu\n",modelVector.size(),projectVector.size());
	//printf("mglOrtho2:Count=%lu,%lu\n",list_Triangle.size(),list_vec3.size());
	//printf("mglOrtho:Model Count=%lu\n",modelVector.size());
	//outputMat(modelVector[0]);
	//printf("mglOrtho:Project Count=%lu\n",projectVector.size());
	//outputMat(projectVector[0]);
		
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(A*tmp);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(A*tmp);
		//projectVector.back() = projectVector.back() * A;
	}
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z)
{
	float c = cos(angle*PI/180);
	float s = sin(angle*PI/180);
	float len = sqrt(x*x+y*y+z*z);
	float x1 = x/len;
	float y1 = y/len;
	float z1 = z/len;
	
	mat4 A={{x1*x1*(1-c)+c,y1*x1*(1-c)+z1*s,x1*z1*(1-c)-y1*s,0,
			x1*y1*(1-c)-z1*s,y1*y1*(1-c)+c,y1*z1*(1-c)+x1*s,0,
			x1*y1*(1-c)-y1*s,y1*z1*(1-c)-x1*s,z1*z1*(1-c)+c,0,
			0,0,0,1}};
			//Tmp = A;
			curMatrix = A*curMatrix;
			
	//printf("mglOrtho:Count=%lu,%lu\n",modelVector.size(),projectVector.size());
	//printf("mglOrtho2:Count=%lu,%lu\n",list_Triangle.size(),list_vec3.size());
	//printf("mglOrtho:Model Count=%lu\n",modelVector.size());
	//outputMat(modelVector[0]);
	//printf("mglOrtho:Project Count=%lu\n",projectVector.size());
	//outputMat(projectVector[0]);
		
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(tmp*A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(tmp*A);
		//projectVector.back() = projectVector.back() * A;
	}
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
	mat4 A = {{x,0,0,0,
			0,y,0,0,
			0,0,z,0,
			0,0,0,1}};
			//Tmp = A;
	curMatrix = A*curMatrix;
	/*if(matrix_type == MGL_MODELVIEW){
		modelVector.back() = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		projectVector.back() = projectVector.back() * A;
	}*/
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(tmp*A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(tmp*A);
		//projectVector.back() = projectVector.back() * A;
	}
	
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far)
{
	//printf("mglFrustum");
	/*mat4 A={{2*near/(right-left),0,0,0,
			0,2*near/(top-bottom),0,0,
			(right+left)/(right-left),(top+bottom)/(top-bottom),(-1)*(far+near)/(far-near),-1,
			0,0,(-2)*(far*near)/(far-near),0}};
			Tmp = A;*/
	mat4 A={{2*near/(right-left),0,0,0,
			0,2*near/(top-bottom),0,0,
			(right+left)/(right-left),(top+bottom)/(top-bottom),(-1)*(far+near)/(far-near),-1,
			0,0,(-2)*(far*near)/(far-near),0}};
			//Tmp = A;
			curMatrix = A*curMatrix;
	/*if(matrix_type == MGL_MODELVIEW){
		modelVector.back() = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		projectVector.back() = projectVector.back() * A;
	}*/
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(tmp*A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(tmp*A);
		//projectVector.back() = projectVector.back() * A;
	}	
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far)
{
	//printf("mglOrtho");
	//mat4 A={{a0,a1,a2,a3,
	//		a4,a5,a6,a7,
	//		a8,a9,a10,a11,
	//		a12,a13,a14,a15}};
	/*mat4 A={{2/(right-left),0,0,(-1)*(right+left)/(right-left),
			0,2/(top-bottom),0,(-1)*(top+bottom)/(top-bottom),
			0,0,(-2)/(far-near),(-1)*(far+near)/(far-near),
			0,0,0,1}};*/
	mat4 A={{2/(right-left),0,0,0,
			0,2/(top-bottom),0,0,
			0,0,(-2)/(far-near),0,
			(-1)*(right+left)/(right-left),(-1)*(top+bottom)/(top-bottom),(-1)*(far+near)/(far-near),1}};
			//Tmp = A;
			curMatrix = A*curMatrix;
			
	//printf("mglOrtho:Count=%lu,%lu\n",modelVector.size(),projectVector.size());
	//printf("mglOrtho2:Count=%lu,%lu\n",list_Triangle.size(),list_vec3.size());
	//printf("mglOrtho:Model Count=%lu\n",modelVector.size());
	//outputMat(modelVector[0]);
	//printf("mglOrtho:Project Count=%lu\n",projectVector.size());
	//outputMat(projectVector[0]);
		
	if(matrix_type == MGL_MODELVIEW){
		mat4 tmp = modelVector.back();
		modelVector.pop_back();
		modelVector.push_back(tmp*A);// = modelVector.back() * A;
	}else if(matrix_type == MGL_PROJECTION){
		mat4 tmp = projectVector.back();
		projectVector.pop_back();
		projectVector.push_back(tmp*A);
		//projectVector.back() = projectVector.back() * A;
	}	
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLfloat red,
              MGLfloat green,
              MGLfloat blue)
{
	RGB[0] = red;
	RGB[1] = green;
	RGB[2] = blue;
}
