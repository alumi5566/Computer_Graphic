#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}

void add_source ( int N, float * x, float * s, float dt )
{
    int i, size=(N+2)*(N+2);
    for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
}

void set_bnd ( int N, int b, float * x )
{
    int i;
    
    for ( i=1 ; i<=N ; i++ ) {
        x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
        x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
        x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
        x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
    }
    x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
    x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
    x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
    x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
}

void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
{
    int i, j, k;
    
    for ( k=0 ; k<20 ; k++ ) {
        FOR_EACH_CELL
        x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
        END_FOR
        set_bnd ( N, b, x );
    }
}

void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
{
    float a=dt*diff*N*N;
    lin_solve ( N, b, x, x0, a, 1+4*a );
}

void advect_cool ( int N, int b, float * d, float * d0, float * u, float * v, float dt , float * T, float * T0)
{
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    float cooling = 5.0;
    float c0 = 1.0f - cooling*dt;
    dt0 = dt*N;
    FOR_EACH_CELL
    x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
    if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
    if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
    s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
    d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
    s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
    // add temperture
    T[IX(i,j)] = s0*(t0*T0[IX(i0,j0)]+t1*T0[IX(i0,j1)])+
    s1*(t0*T0[IX(i1,j0)]+t1*T0[IX(i1,j1)]);
    T[IX(i,j)] *= c0;
    //
    END_FOR
    set_bnd ( N, b, d );
}

void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
{
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    
    dt0 = dt*N;
    FOR_EACH_CELL
    x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
    if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
    if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
    s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
    d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
    s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
    END_FOR
    set_bnd ( N, b, d );
}

void project ( int N, float * u, float * v, float * p, float * div )
{
    int i, j;
    
    FOR_EACH_CELL
    div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/N;
    p[IX(i,j)] = 0;
    END_FOR
    set_bnd ( N, 0, div ); set_bnd ( N, 0, p );
    
    lin_solve ( N, 0, p, div, 1, 4 );
    
    FOR_EACH_CELL
    u[IX(i,j)] -= 0.5f*N*(p[IX(i+1,j)]-p[IX(i-1,j)]);
    v[IX(i,j)] -= 0.5f*N*(p[IX(i,j+1)]-p[IX(i,j-1)]);
    END_FOR
    set_bnd ( N, 1, u ); set_bnd ( N, 2, v );
}

//dens_step ( N, dens, dens_prev, u, v, diff, dt );
void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
{
    add_source ( N, x, x0, dt );
    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
    
}
void add_buoyancy(int N,float *u,float *heat, float dt){
    double buoyancy=5.2;
    int i, size=(N+2)*(N+2);
    for ( i=0 ; i<size ; i++ ){
        u[i] += dt*buoyancy*(heat[i]-0.0);
    }
}

void add_confinement(int N,float *u0,float *v0,float dt,float *u,float *v){
    int size = (N+2)*(N+2);
    float *temp=(float *) malloc ( size*sizeof(float) );
    
    for(int i=0;i<size;i++) temp[i]=0.0;
    
    for(int i=1;i<=N;i++){
        for(int j=1;j<=N;j++){
            float x =(v0[IX(i,j+1)] -v0[IX(i,j-1)] )*15.34;
            float y =(u0[IX(i+1,j)] -u0[IX(i-1,j)] )*15.34;
            temp[IX(i,j)]= sqrtf(x*x+y*y);
        }
    }
    
    float Nx=0.0;
    float Ny=0.0;
    for (int i=1;i<=N;i++){
        for(int j=1;j<=N;j++){
            Nx = (temp[IX(i+1,j)]-temp[IX(i-1,j)] ) ;
            Ny = (temp[IX(i,j+1)]-temp[IX(i,j-1)] );
            float normal = 1.0/(0.0001+sqrtf(Nx*Nx+ Ny*Ny));
            Nx*=normal;
            Ny*=normal;
            u[IX(i,j)]+= -(Ny*temp[IX(i,j)] -Nx*temp[IX(i,j)] )*dt;
            v[IX(i,j)]+= -(Ny*temp[IX(i,j)] -Nx*temp[IX(i,j)] )*dt;
        }
    }

}

//vel_step ( N, u, v, u_prev, v_prev, visc, dt );
void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
{
    add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
   add_buoyancy(N,u,u0, dt);add_buoyancy(N,v,v0, dt);
    add_confinement(N,u0,v0,dt,u,v);
    // 加浮力
    SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
    SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
    project ( N, u, v, u0, v0 );
    SWAP ( u0, u ); SWAP ( v0, v );
    advect ( N, 1, u, u0, u0, v0, dt ); advect ( N, 2, v, v0, u0, v0, dt );
    project ( N, u, v, u0, v0 );
    
    // disapear if density is lower
    /*int i, j;
     FOR_EACH_CELL
     if(u[IX(i,j)]<100){
     u[IX(i,j)]=0;
     u[IX(i,j-1)]=0;
     u[IX(i-1,j)]=0;
     u[IX(i-1,j-1)]=0;
     u0[IX(i,j)]=0;
     }
     END_FOR
     */
}

void heat_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
{
    add_source ( N, x, x0, dt );
    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
    int i, j;
    FOR_EACH_CELL
    if(0){
        //if(x[IX(i,j)]>50){
        x[IX(i,j)]=0;
        x[IX(i,j-1)]=0;
        x[IX(i-1,j)]=0;
        x[IX(i-1,j-1)]=0;
        //u0[IX(i,j)]=0;
    }
    END_FOR
}

//void add_source ( int N, float * x, float * s, float dt )
// x+=s*dt
//dens_step ( N, dens, dens_prev, u, v, diff, dt );
void dens_temp_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt ,float * T, float * T0)
{
    add_source ( N, x, x0, dt );
    add_source ( N, T, T0, dt );
    
    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
    
    SWAP ( T0, T );
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    SWAP ( T0, T ); advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    advect_cool ( N, 0, x, x0, u, v, dt , T, T0);
    
}
