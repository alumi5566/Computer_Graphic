#include "boolean.h"


// Determine if the ray intersects with the boolean of A and B.
//Object* Boolean::Intersection(const Ray& ray, std::vector<Hit>& hits) const
//{
//	Object* objOutput;
//	return objOutput;
//}
#if 1
bool Boolean::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
    // TODO
    if(type == type_intersection){
        //printf("AND\n");
        std::vector<Hit> HitTmpA;
        std::vector<Hit> HitTmpB;

		if(A->Intersection(ray,HitTmpA) && B->Intersection(ray,HitTmpB)){
			// enter
			if(HitTmpA[0].t < HitTmpB[0].t){
				Hit hitT1;
                hitT1.object = B;
                hitT1.t = HitTmpB[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);
			}else{
				Hit hitT1;
                hitT1.object = A;
                hitT1.t = HitTmpA[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);
			}
			//leaving
			if(HitTmpA[1].t < HitTmpB[1].t){
				Hit hitT1;
                hitT1.object = A;
                hitT1.t = HitTmpA[1].t;
                hitT1.ray_exiting = true;
                hits.push_back(hitT1);
			}else{
				Hit hitT1;
                hitT1.object = B;
                hitT1.t = HitTmpB[1].t;
                hitT1.ray_exiting = true;
                hits.push_back(hitT1);
			}
			return true;
		}else{
			return false;
		}
			
    }else if(type == type_difference){
        //printf("------\n");
        std::vector<Hit> HitTmpA;
        std::vector<Hit> HitTmpB;

		if(A->Intersection(ray,HitTmpA) && B->Intersection(ray,HitTmpB)){
			int i=HitTmpA.size()-1;
			int j=HitTmpB.size()-1;
			//printf("i=%d,j=%d\n",i,j);
			/*double AE = HitTmpA[0].t;
			double AL = HitTmpA[1].t;
			double BE = HitTmpB[0].t;
			double BL = HitTmpB[1].t;*/
			double AE = HitTmpA[0].t;
			double AL = HitTmpA[i].t;
			double BE = HitTmpB[0].t;
			double BL = HitTmpB[j].t;
#if 1
			/*
			if(AE<BE){
				// just enter A
				hits.push_back(HitTmpA[0]);
			}else if(AE==BE){
				if(AL>=BL) hits.push_back(HitTmpB[1]);
				else return false;
			}else{
				// AE is after BE, return BL?
				if(AE>=BL) hits.push_back(HitTmpA[0]); // 
				else if(AE<BL && AL>BL) hits.push_back(HitTmpB[1]);
				else return false;
			}
			
			if(AL>BL){
				hits.push_back(HitTmpA[1]);
			}else if(AL==BL){
				if(AE<=BE) hits.push_back(HitTmpB[0]);
				else return false;
			}else{
				if(AL<=BE) hits.push_back(HitTmpA[1]);
				else if(AL>BE && AE<BE) hits.push_back(HitTmpB[0]);
				else return false;
			}
			return true;*/
			if(AE<BE){
				// just enter A
				hits.push_back(HitTmpA[0]);
			}else if(AE==BE){
				if(AL>BL) hits.push_back(HitTmpB[j]);
				else return false;
			}else{
				// AE is after BE, return BL?
				if(AE>BL) hits.push_back(HitTmpA[0]); // 
				else if(AE<BL && AL>BL) hits.push_back(HitTmpB[j]);
				else return false;
			}
			
			if(AL>BL){
				hits.push_back(HitTmpA[i]);
			}else if(AL==BL){
				if(AE<BE) hits.push_back(HitTmpB[0]);
				else return false;
			}else{
				if(AL<BE) hits.push_back(HitTmpA[i]);
				else if(AL>BE && AE<BE) hits.push_back(HitTmpB[0]);
				else return false;
			}
			return true;
			
#endif
			#if 0
			if(AE<BE){
				if(AL<=BE){
					hits.push_back(HitTmpA[0]);
					hits.push_back(HitTmpA[1]);
					return true;
				}else if(AL>BE && AL<=BL){
					hits.push_back(HitTmpA[0]);
					hits.push_back(HitTmpB[0]);
					return true;
				}else if(AL>BE && AL>BL){
					hits.push_back(HitTmpA[0]);
					hits.push_back(HitTmpB[0]);
					hits.push_back(HitTmpB[1]);
					hits.push_back(HitTmpA[1]);
					return true;
				}else{
					printf("something wrong\n");
					return false;
				}
			}else if(AE==BE){
				if(AL>BE && AL<BL){
					Hit hitT1;
                	hitT1.object = A;
                	hitT1.t = HitTmpA[0].t;
                	hitT1.ray_exiting = false;
                	hits.push_back(hitT1);
                	hitT1.object = A;
                	hitT1.t = HitTmpA[0].t;
                	hitT1.ray_exiting = true;
					hits.push_back(hitT1);
					return true;
				}else if(AL>BL){
					hits.push_back(HitTmpB[1]);
					hits.push_back(HitTmpA[1]);
					return true;
				}else{
					return false;
				}
			}else if(AE>BE && AE<BL){
				if(AL>BE && AL<= BL){
					return false;
				}else if(AL>BE && AL>BL){
					hits.push_back(HitTmpB[1]);
					hits.push_back(HitTmpA[1]);
					return true;					
				}else{
					//printf("something wrong2\n");
					return false;				
				}
			}else if(AE==BL){
				if(AL>BL){
					hits.push_back(HitTmpB[1]);
					hits.push_back(HitTmpA[1]);
					return true;
				}else{
					return false;
				}
			}else if(AE>BL && AL!=AE){
					hits.push_back(HitTmpA[0]);
					hits.push_back(HitTmpA[1]);
					return true;			
			}else{
				printf("shouldnt be here\n");
				return false;
			}
			printf("shouldnt be here2\n");
			return false;
		#endif
#if 0
			std::vector<Hit> Tmp;
			//double AE = HitTmpA[0].t;
			//double AL = HitTmpA[1].t;
			//double BE = HitTmpB[0].t;
			//double BL = HitTmpB[1].t;
			if(AE<BE) Tmp.push_back(HitTmpB[0]); else Tmp.push_back(HitTmpA[0]);
			if(AL<BL) Tmp.push_back(HitTmpA[1]); else Tmp.push_back(HitTmpB[1]);
			std::vector<Hit> Output;
			for(int i=0;i<HitTmpA.size();i++) Output.push_back(HitTmpA[i]);
			if( Output[0].t < Tmp[0].t){
				hits.push_back(Output[0]);
				//return true;
			}else{
				if(Output[1].t > Tmp[1].t)
					hits.push_back(Tmp[1]);
				else
					return false;
			}
			if( Output[1].t > Tmp[1].t){
				hits.push_back(Tmp[1]);
				//return true;
			}else{
				if(Output[0].t < Tmp[1].t)
					hits.push_back(Tmp[1]);
				else 
					return false;
			}
			
			return true;
#endif			
		}else if(A->Intersection(ray,HitTmpA) && !B->Intersection(ray,HitTmpB)){
			Hit hitT1;
            hitT1.object = A;
        	hitT1.t = HitTmpA[0].t;
            hitT1.ray_exiting = false;
            hits.push_back(hitT1);
            
			Hit hitT2;
            hitT2.object = A;
        	hitT2.t = HitTmpA[1].t;
            hitT2.ray_exiting = true;
            hits.push_back(hitT2);
			return true;
		}else{
			return false;
		}

    }else if(type == type_union){
        //printf("OR\n");
        std::vector<Hit> HitTmpA;
        std::vector<Hit> HitTmpB;
        if(A->Intersection(ray,HitTmpA) && !B->Intersection(ray,HitTmpB)){
            Hit hitT1;
        	hitT1.object = A;
            hitT1.t = HitTmpA[0].t;
            hitT1.ray_exiting = false;
            hits.push_back(hitT1);
            Hit hitT2;
            hitT2.object = A;
            hitT2.t = HitTmpA[1].t;
            hitT2.ray_exiting = true;
            hits.push_back(hitT2);
            return true;
        }else if(!A->Intersection(ray,HitTmpA) && B->Intersection(ray,HitTmpB)){
            Hit hitT1;
            hitT1.object = B;
            hitT1.t = HitTmpB[0].t;
            hitT1.ray_exiting = false;
            hits.push_back(hitT1);
            Hit hitT2;
            hitT2.object = B;
            hitT2.t = HitTmpB[1].t;
            hitT2.ray_exiting = true;
            hits.push_back(hitT2);
            return true;
        }else if(A->Intersection(ray,HitTmpA) && B->Intersection(ray,HitTmpB)){
			double AE = HitTmpA[0].t;
			double AL = HitTmpA[1].t;
			double BE = HitTmpB[0].t;
			double BL = HitTmpB[1].t;
			if(AE<BE){
				Hit hitT1;
            	hitT1.object = A;
            	hitT1.t = HitTmpA[0].t;
            	hitT1.ray_exiting = false;
            	hits.push_back(hitT1);
			}else{
				Hit hitT1;
            	hitT1.object = B;
            	hitT1.t = HitTmpB[0].t;
            	hitT1.ray_exiting = false;
            	hits.push_back(hitT1);
			}
			
			if(AL>BL){
				Hit hitT1;
            	hitT1.object = A;
            	hitT1.t = HitTmpA[1].t;
            	hitT1.ray_exiting = true;
            	hits.push_back(hitT1);
			}else{
				Hit hitT1;
            	hitT1.object = B;
            	hitT1.t = HitTmpB[1].t;
            	hitT1.ray_exiting = true;
            	hits.push_back(hitT1);
			}
			return true;
        }else{
        	return false;
        }
#if 0
        if(A->Intersection(ray,HitTmpA) || B->Intersection(ray,HitTmpB)){
            if( A->Intersection(ray,HitTmpA) && !B->Intersection(ray,HitTmpB)){
                Hit hitT1;
                hitT1.object = A;
                hitT1.t = HitTmpA[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);
                return true;
            }else if(!A->Intersection(ray,HitTmpA) && B->Intersection(ray,HitTmpB)){
                Hit hitT1;
                hitT1.object = B;
                hitT1.t = HitTmpB[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);
                return true;
            }
            
            if( HitTmpA[0].t < HitTmpB[0].t ){
                Hit hitT1;
                hitT1.object = A;
                hitT1.t = HitTmpA[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);
                /*
                Hit hitT2;
                hitT1.object = B;
                hitT1.t = HitTmpB[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT2);
                */
            }else if (HitTmpA[0].t >= HitTmpB[0].t){
                Hit hitT1;
                hitT1.object = B;
                hitT1.t = HitTmpB[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT1);

                /*
                Hit hitT2;
                hitT1.object = A;
                hitT1.t = HitTmpA[0].t;
                hitT1.ray_exiting = false;
                hits.push_back(hitT2);
                */
            }else{
                printf("something goes wrong");
            }
            return true;
            
        }
#endif

    }else{
        printf("something goes wrong\n");
    }
    return false;
}
#endif
// This should never be called.
vec3 Boolean::Normal(const vec3& point) const
{
    assert(false);
    return vec3();
}
