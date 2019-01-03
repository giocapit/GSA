#include "GSA.hpp"

void GSAResults::collect( double f_v, double f_v1, double f_v2 ){

		D_x+=f_v*f_v1;
		D_y+=f_v*f_v2;
		f_0+=f_v;
		D+=f_v*f_v;
}


void GSAResults::complete(int m){

	f_0=f_0/m;
	D_x=D_x/m-pow(f_0,2);
	D_y=D_y/m-pow(f_0,2);
	D=D/m-pow(f_0,2);
	S_x=D_x/D;
	S_y=D_y/D;
}
