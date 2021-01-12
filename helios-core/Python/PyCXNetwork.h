#ifndef __PYCXNETWORK_H__
#define __PYCXNETWORK_H__

#include <Python.h>
#include <CVNetwork.h>


typedef struct PyCXNetwork{
	CVIndex* edges;
	float* R;
	float* dR;
	CVSize edgesCount;
	CVSize verticesCount;
	CVSize iterations;
	CVSize internalIterations;
	CVFloat attractiveConstant;
	CVFloat repulsiveConstant;
	CVFloat viscosityConstant;
	CVSize threadIterations;
	pthread_t thread;
	CVBool shallStop;
} PyCXLayoutParameters;



PyObject * PyCXNetworkLayout(PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStart(PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStop(PyObject*,PyObject*);

#endif
