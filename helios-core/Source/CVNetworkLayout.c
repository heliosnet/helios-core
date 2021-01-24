
#include "CVNetwork.h"

// static inline void __recenterNetworkPositions(float* Rx,float* Ry,float* Rz,CVSize count){
// 	double aRMx=0;
// 	double aRMy=0;
// 	double aRMz=0;
// 	int i=0;
// 	double invertexf = 1/(double)count;
// 	for(i=0;i<count;i++){
// 		aRMx+=Rx[i]*invertexf;
// 		aRMy+=Ry[i]*invertexf;
// 		aRMz+=Rz[i]*invertexf;
// 	}
	
// 	for(i=0;i<count;i++){
// 		Rx[i]-=aRMx;
// 		Ry[i]-=aRMy;
// 		Rz[i]-=aRMz;
// 	}
// }//////


void CVNetworkRadiusRecenter(CVFloat* R, CVSize vcount){
	double aRMx=0;
	double aRMy=0;
	double aRMz=0;
	
	double invertexf = 1/(double)vcount;
	for(CVIndex i=0;i<vcount;i++){
		CVFloat* Ri = R+i*3;
		aRMx+=Ri[0];
		aRMy+=Ri[1];
		aRMz+=Ri[2];
	}

	aRMx *= invertexf;
	aRMy *= invertexf;
	aRMz *= invertexf;

	for(CVIndex i=0;i<vcount;i++){
		CVFloat* Ri = R+i*3;
		Ri[0]-=aRMx;
		Ri[1]-=aRMy;
		Ri[2]-=aRMz;
	}
}


#define	kVertexMaxSpeed 25.0f
#define	kVertexMaxForce 5.0f
#define kGravitationConstant 1.0e-10




static void CVNetworkIteratePositions_implementation(CVFloat attractiveForce, CVFloat repulsiveForce,CVFloat viscosityForce, CVFloat softening,
												CVFloat* R, CVFloat* dR,
												CVIndex* edges,
												CVSize vcount, CVSize ecount){
	CVFloat dt=1;
	
	CVSize ci;
	CVSize verticesCount = vcount;
	const CVSize dimensions = 3;
	
	double* totalF = calloc(verticesCount*dimensions, sizeof(double));
	CVSize unrolledLoops = kCVDefaultParallelBlocks;
	CVSize unrolledSize = 1 + ((verticesCount - 1) / unrolledLoops);
	
	CVParallelForStart(repulstiveIteraction, blockIndex, unrolledLoops){
		double* localF = calloc(verticesCount*dimensions, sizeof(double));
		
		CVSize maxIndex = CVMIN((blockIndex+1)*unrolledSize, verticesCount);
		CVIndex i;
		for(i=blockIndex*unrolledSize;i<maxIndex;i++){
			const CVFloat* Ri=R+i*dimensions;
			CVIndex j;
			for(j=0;j<i;j++){
				const CVFloat* Rj = R+j*dimensions;
				
				const CVFloat DX=(Ri[0]-Rj[0]);
				const CVFloat DY=(Ri[1]-Rj[1]);
				const CVFloat DZ=(Ri[2]-Rj[2]);
				
				const CVFloat D2=DX*DX+DY*DY+DZ*DZ + softening;
				
				const CVFloat invsqrtD2=1.0f/sqrtf(D2);
				const CVFloat dx=-DX*invsqrtD2;
				const CVFloat dy=-DY*invsqrtD2;
				const CVFloat dz=-DZ*invsqrtD2;
				
				const CVFloat Ftotal=-repulsiveForce/D2;
				
				const CVFloat Ftotalx = Ftotal*dx;
				const CVFloat Ftotaly = Ftotal*dy;
				const CVFloat Ftotalz = Ftotal*dz;
				
				localF[i*3+0]+=Ftotalx;
				localF[i*3+1]+=Ftotaly;
				localF[i*3+2]+=Ftotalz;
				
				localF[j*3+0]-=Ftotalx;
				localF[j*3+1]-=Ftotaly;
				localF[j*3+2]-=Ftotalz;
			}
		}
		CVParallelLoopCriticalRegionStart(repulstiveIteraction){
			for (i=0; i<verticesCount*3; i++) {
				totalF[i] += localF[i];
			}
		}
		CVParallelLoopCriticalRegionEnd(repulstiveIteraction);
		free(localF);
	}
	CVParallelForEnd(repulstiveIteraction);
	
	
	unrolledLoops = kCVDefaultParallelBlocks;
	unrolledSize = 1 + ((ecount - 1) / unrolledLoops);
	CVParallelForStart(attractiveIteraction, blockIndex, unrolledLoops){
		double* localF = calloc(verticesCount*3, sizeof(double));
		
		size_t maxIndex = CVMIN((blockIndex+1)*unrolledSize, ecount);
		size_t i;
		for(i=blockIndex*unrolledSize;i<maxIndex;i++){
			CVIndex fromVertex=edges[i*2+0];
			CVIndex toVertex=edges[i*2+1];
			if(fromVertex!=toVertex){
				const CVFloat Rxi=R[fromVertex*3+0];
				const CVFloat Ryi=R[fromVertex*3+1];
				const CVFloat Rzi=R[fromVertex*3+2];
				
				CVFloat DX=(Rxi-R[toVertex*3+0]);
				CVFloat DY=(Ryi-R[toVertex*3+1]);
				CVFloat DZ=(Rzi-R[toVertex*3+2]);
				CVFloat D2=DX*DX+DY*DY+DZ*DZ+softening;
				
				CVFloat invsqrtD2=1.0f/sqrtf(D2);
				CVFloat dx=-DX*invsqrtD2;
				CVFloat dy=-DY*invsqrtD2;
				CVFloat dz=-DZ*invsqrtD2;
				
				CVFloat Ftotal=attractiveForce*D2;
				
				localF[fromVertex*3+0]+=Ftotal*dx;
				localF[fromVertex*3+1]+=Ftotal*dy;
				localF[fromVertex*3+2]+=Ftotal*dz;
				
				localF[toVertex*3+0]-=Ftotal*dx;
				localF[toVertex*3+1]-=Ftotal*dy;
				localF[toVertex*3+2]-=Ftotal*dz;
			}
		}
		
		CVParallelLoopCriticalRegionStart(attractiveIteraction){
			for (i=0; i<verticesCount*3; i++) {
				totalF[i] += localF[i];
			}
		}
		CVParallelLoopCriticalRegionEnd(attractiveIteraction);
		free(localF);
	}
	CVParallelForEnd(attractiveIteraction);
	
	
	for(ci=0;ci<vcount;ci++){
		CVFloat* dRci = dR + dimensions*ci;
		CVFloat Fx = totalF[ci*3+0]-viscosityForce*dRci[0];
		CVFloat Fy = totalF[ci*3+1]-viscosityForce*dRci[1];
		CVFloat Fz = totalF[ci*3+2]-viscosityForce*dRci[2];
		
		CVFloat DDF = Fx*Fx +Fy*Fy + Fz*Fz;
		if(DDF > kVertexMaxForce*kVertexMaxForce){
			Fx *= 0.75f*kVertexMaxForce/sqrtf(DDF);
			Fy *= 0.75f*kVertexMaxForce/sqrtf(DDF);
			Fz *= 0.75f*kVertexMaxForce/sqrtf(DDF);
		}
		
		if(isnan(DDF)){
			Fx = 0.0f;
			Fy = 0.0f;
			Fz = 0.0f;
		}
		
		dRci[0]+=Fx*dt;
		dRci[1]+=Fy*dt;
		dRci[2]+=Fz*dt;
		CVFloat DDR = dRci[0]*dRci[0]+dRci[1]*dRci[1]+dRci[2]*dRci[2];
		if(DDR > kVertexMaxSpeed*kVertexMaxSpeed){
			dRci[0] *= 0.75f*kVertexMaxSpeed/sqrtf(DDR);
			dRci[1] *= 0.75f*kVertexMaxSpeed/sqrtf(DDR);
			dRci[2] *= 0.75f*kVertexMaxSpeed/sqrtf(DDR);
		}
		
		if(isnan(DDR)){
			dRci[0] = 0.0f;
			dRci[1] = 0.0f;
			dRci[2] = 0.0f;
		}
				
		CVFloat* Rci = R+ci*dimensions;
		Rci[0]+=dRci[0]*dt;
		Rci[1]+=dRci[1]*dt;
		Rci[2]+=dRci[2]*dt;
		
		
	}
	
	double aRMx=0;
	double aRMy=0;
	double aRMz=0;
	
	int i=0;
	double invertexf = 1/(double)vcount;
	for(i=0;i<vcount;i++){
		CVFloat* Ri = R+i*dimensions;
		aRMx+=Ri[0]*invertexf;
		aRMy+=Ri[1]*invertexf;
		aRMz+=Ri[2]*invertexf;
	}
	
	for(i=0;i<vcount;i++){
		CVFloat* Ri = R+i*dimensions;
		Ri[0]-=aRMx;
		Ri[1]-=aRMy;
		Ri[2]-=aRMz;
	}
	free(totalF);
}


#define k_CVDefaultAttractionConstant 0.0001f
#define k_CVDefaultRepulsiveConstant 0.1f
#define k_CVDefaultViscosityConstant 0.1f
#define k_CVDefaultSofteningConstant 2.0f

void CVNetworkIteratePositions(CVIndex* edges, float* R, float* dR,
	CVSize edgesCount, CVSize verticesCount, CVSize iterations,
	CVFloat attractiveConstant,CVFloat repulsiveConstant,CVFloat viscosityConstant){
	if (attractiveConstant < 0.0f) attractiveConstant = k_CVDefaultAttractionConstant;
	if (repulsiveConstant < 0.0f) repulsiveConstant = k_CVDefaultRepulsiveConstant;
	if (viscosityConstant < 0.0f) viscosityConstant = k_CVDefaultViscosityConstant;
	CVFloat softeningConstant = k_CVDefaultSofteningConstant;
	for(CVIndex i=0;i<iterations;i++){
		CVNetworkIteratePositions_implementation(attractiveConstant, repulsiveConstant, viscosityConstant, softeningConstant,
												 R, dR,
												 edges, verticesCount, edgesCount);
		
		CVNetworkRadiusRecenter(R,verticesCount);
	}
}






