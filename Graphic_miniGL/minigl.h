/**
 * minigl.h
 * -------------------------------
 * This file defines the API to be implemented for MiniGL.
 */

#ifndef __MINIGL_H__
#define __MINIGL_H__

#include <cmath>
#include <iostream>

typedef int           MGLint;
typedef unsigned char MGLbyte;
typedef unsigned int  MGLsize;
typedef unsigned int  MGLpixel;
typedef float         MGLfloat;
typedef bool          MGLbool;

typedef enum {
    MGL_MODELVIEW,
    MGL_PROJECTION
} MGLmatrix_mode;

typedef enum {
    MGL_TRIANGLES,
    MGL_QUADS
} MGLpoly_mode;

inline MGLpixel Make_Pixel(int r, int g, int b)
{
    return (r<<24)|(g<<16)|(b<<8)|0xff;
}

inline void From_Pixel(MGLpixel p, int& r, int& g, int& b)
{
    r = (p>>24)&0xff;
    g = (p>>16)&0xff;
    b = (p>>8)&0xff;
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
                   MGLpixel *data);

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode);

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd();

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y);

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z);

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode);

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix();

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix();

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity();

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
void mglLoadMatrix(const MGLfloat *matrix);

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
void mglMultMatrix(const MGLfloat *matrix);

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z);

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z);

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z);

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far);

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far);

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLfloat red,
              MGLfloat green,
              MGLfloat blue);


//mat.h
template<class T, int m, int n> struct mat;
template<class T, int m, int n> T dot(const mat<T,m,n>& u,const mat<T,m,n>& v);

//Class mat: Aggregate data structure, initial values may not be 0;
//column major ordered matrix. e.g. A 3x4 matrix, M, will be stored in values as {M00,M10,M20,M01,M11,M21,M01,M12,M22,M02,M12,M22}

template<class T, int m, int n>
struct mat
{
    T values[m*n];

    int rows()
    {return m;}
    
    int cols()
    {return n;}

    void make_zero()
    {for(int i = 0; i < m*n; i++) values[i]=0;}

    mat& operator += (const mat& v)
    {for(int i = 0; i < m*n; i++) values[i] += v.values[i]; return *this;}

    mat& operator -= (const mat& v)
    {for(int i = 0; i < m*n; i++) values[i] -= v.values[i]; return *this;}

    mat& operator *= (const T& c)
    {for(int i = 0; i < m*n; i++) values[i] *= c; return *this;}

    mat& operator /= (const T& c)
    {for(int i = 0; i < m*n; i++) values[i] /= c; return *this;}

    mat operator + () const
    {return *this;}

    mat operator - () const
    {mat r; for(int i = 0; i < m*n; i++) r.values[i] = -values[i]; return r;}

    mat operator + (const mat& v) const
    {mat r; for(int i = 0; i < m*n; i++) r.values[i] = values[i] + v.values[i]; return r;}

    mat operator - (const mat& v) const
    {mat r; for(int i = 0; i < m*n; i++) r.values[i] = values[i] - v.values[i]; return r;}

    mat operator * (const T& c) const
    {mat r; for(int i = 0; i < m*n; i++) r.values[i] = values[i] * c; return r;}

    mat operator / (const T& c) const
    {mat r; for(int i = 0; i < m*n; i++) r.values[i] = values[i] / c; return r;}

    template<int p>
    mat<T,m,p> operator * (const mat<T,n,p>& M) const
    {
        mat<T,m,p> r; r.make_zero();
        for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++)
                for(int k = 0; k < p; k++)
                    r(i,k) += this->operator()(i,j) * M(j,k);
        return r;
    }

    inline const T& operator() (int i, int j) const
    {return values[i+j*m];}

    inline T& operator() (int i, int j)
    {return values[i+j*m];}
};

template <class T, int m, int n>
mat<T,m,n> operator * (const T& c, const mat<T,m,n>& v)
{return v*c;}

template <class T, int m, int n>
std::ostream& operator << (std::ostream& out, const mat<T,m,n> & M)
{
    for(int i = 0; i < m; i++)
    {
        if(i) out << " ; ";
        for(int j = 0; j < n; j++)
        {
            if(j) out << ' ';
            out << M(i,j);
        }
    }
    return out;
}

//vec.h
template<class T, int n> struct vec;
template<class T, int n> T dot(const vec<T,n>& u,const vec<T,n>& v);

template<class T, int n>
struct vec: mat<T,n,1>
{
    typedef mat<T,n,1> BASE;
    using BASE::make_zero; using BASE::values;

    enum{dim=n};
    vec()
    {make_zero();}

    vec(const T& a)
    {static_assert(n == 1,"vector size does not match this constructor");values[0]=a;}

    vec(const T& a, const T& b)
    {static_assert(n == 2,"vector size does not match this constructor");values[0]=a;values[1]=b;}

    vec(const T& a, const T& b, const T& c)
    {static_assert(n == 3,"vector size does not match this constructor");values[0]=a;values[1]=b;values[2]=c;}
    
    vec(const T& a, const T& b, const T& c, const T& d)
    {static_assert(n == 4,"vector size does not match this constructor");values[0]=a;values[1]=b;values[2]=c;values[3]=d;}

    vec(const mat<T,n,1>& M)
    {for(int i=0; i<n; i++) values[i]=M.values[i];}

    int size()
    {return n;}

    const T& operator[] (int i) const
    {return values[i];}

    T& operator[] (int i)
    {return values[i];}

    T magnitude_squared() const
    {return dot(*this, *this);}

    T magnitude() const
    {return sqrt(magnitude_squared());}

    // Be careful to handle the zero vector gracefully
    vec normalized() const
    {T mag = magnitude(); if(mag) return *this / mag; vec r; r[0] = 1; return r;};

    vec<T,n-1> remove_dim(int d) const
    {vec<T,n-1> v; for(int i=0; i<d; i++) v[i]=values[i];
        for(int i=d; i<n; i++) v[i]=values[i+1];
        return v;}

    vec<T,n-1> remove_last_dim() const
    {vec<T,n-1> v; for(int i=0; i<n-1; i++) v[i]=values[i]; return v;}

};

template <class T, int n>
T dot(const vec<T,n> & u, const vec<T,n> & v)
{
    T r  =  0;
    for(int i = 0; i < n; i++) r += u.values[i] * v.values[i];
    return r;
}

template <class T >
vec<T,3> cross(const vec<T,3> & u, const vec<T,3> & v)
{
    return vec<T,3> (
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]);
}

template <class T, int n>
std::ostream& operator << (std::ostream& out, const vec<T,n> & u)
{
    for(int i = 0; i < n; i++)
    {
        if(i) out << ' ';
        out << u[i];
    }
    return out;
}

typedef vec<float,2> vec2;
typedef vec<float,3> vec3;
typedef vec<int,2> ivec2;


#endif
