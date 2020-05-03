#ifndef CVNetwork_CVNetworkLayout_h
#define CVNetwork_CVNetworkLayout_h

#include "CVCommons.h"
void CVNetworkIteratePositions(CVIndex* edges, float* R, float* dR,
	CVSize edgesCount, CVSize verticesCount, CVSize iterations,
	CVFloat attractiveConstant,CVFloat repulsiveConstant,CVFloat viscosityConstant);
  
#endif
