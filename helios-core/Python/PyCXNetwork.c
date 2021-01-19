#include <stdio.h>
#include "PyCXNetwork.h"
#include <CVNetworkLayout.h>
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL helios_ARRAY_API
#include <numpy/arrayobject.h>
#include <pthread.h>
#include <sys/types.h>

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


#if CV_USE_OPENMP
#include <omp.h>
#endif //_OPENMP
// CVNetwork* PyCXNewNetwork(PyObject* edgesList, CVBool directed){

// 	PyObject *edgesList;
// 	CVInteger edgesCount = PyObject_Length(edgesList);

// 	if (edgesCount <= 0){
// 		PyErr_SetString(PyExc_TypeError, "edges list is empty.");
// 		return NULL;
// 	}
// 	CVSize nodesCount = 0;
// 	CVIndex * fromIndices = calloc(edgesCount,sizeof(CVIndex));
// 	CVIndex * toIndices = calloc(edgesCount,sizeof(CVIndex));

// 	PyObject* iterator = PyObject_GetIter(edgesList);
// 	PyObject* item;

// 	if (iterator == NULL) {
// 		PyErr_SetString(PyExc_TypeError, "Edges list should be a iterable collection of 2-tuples.");
// 		return NULL;
// 	}

// 	while ((item = PyIter_Next(iterator))) {
// 		if(Tup)
// 		Py_DECREF(item);
// 	}

// 	Py_DECREF(iterator);

// 	if (PyErr_Occurred()){
// 			PyErr_SetString(PyExc_TypeError, "an error occurred.");
// 			return NULL;
// 	}else {
// 			/* continue doing useful work */
// 	}


// 	CVNetwork* network = CVNewNetwork(count,CVFalse,directed);
// 	CVNetworkAddNewEdges(network,fromIndices,toIndices,NULL,0);

// 	free(fromIndices);
// 	free(toIndices);
// 	return network;
// }

// PyObject * PyCXNewEdgesListFromNetwork(CVNetwork* network){
// 	PyObject * list = PyList_New(network->edgesCount);
// 	for (CVIndex edgeIndex = 0; edgeIndex < network->edgesCount; edgeIndex++){
// 		unsigned int fromIndex = (unsigned int)network->edgeFromList[edgeIndex];
// 		unsigned int toIndex = (unsigned int)network->edgeToList[edgeIndex];
// 		PyObject* fromToTuple = Py_BuildValue("(II)",fromIndex,toIndex);
// 		PyList_SetItem(list,edgeIndex,fromToTuple);
// 	}
// 	return list;
// }



// CVNetwork* PyCXNewNetwork(PyObject* edgesList,CVSize verticesCount, CVBool directed){
// 	PyObject* edgesSequence = PySequence_Fast(edgesList, "argument must be iterable");
// 	if (!edgesSequence){
// 		return NULL;
// 	}

// 	CVSize edgesCount = (CVSize)PySequence_Fast_GET_SIZE(edgesSequence);

// 	if (edgesCount <= 0){
// 		PyErr_SetString(PyExc_TypeError, "edges list is empty");
// 		return NULL;
// 	}

// 	CVIndex* fromIndices = calloc(edgesCount,sizeof(CVIndex));
// 	CVIndex* toIndices = calloc(edgesCount,sizeof(CVIndex));
	
// 	if (!fromIndices||!toIndices){
// 		free(fromIndices);
// 		free(toIndices);
// 		Py_XDECREF(edgesSequence);
// 		PyErr_SetString(PyExc_MemoryError, "out of memory");
// 		return NULL;
// 	}

// 	for (CVIndex edgeIndex = 0; edgeIndex < edgesCount; edgeIndex++){
// 		PyObject* edgeItem = PySequence_Fast_GET_ITEM(edgesSequence, edgeIndex);
// 		unsigned int fromIndex;
// 		unsigned int toIndex;

// 		if (
// 				!edgeItem || 
// 				!PyArg_ParseTuple(edgeItem,"II",&fromIndex,&toIndex) ||
// 				fromIndex>=verticesCount ||
// 				toIndex>=verticesCount
// 			){
// 			free(fromIndices);
// 			free(toIndices);
// 			Py_XDECREF(edgesSequence);
// 			PyErr_SetString(PyExc_TypeError, "a problem happened while converting edges list");
// 			return NULL;
// 		}
// 		fromIndices[edgeIndex] = (CVIndex)fromIndex;
// 		toIndices[edgeIndex] = (CVIndex)toIndex;
// 	}

// 	Py_XDECREF(edgesSequence);

// 	CVNetwork* network = CVNewNetwork(verticesCount,CVFalse,directed);

// 	if(!CVNetworkAddNewEdges(network,fromIndices,toIndices,NULL,edgesCount)){
// 		CVNetworkDestroy(network);
// 		free(fromIndices);
// 		free(toIndices);
// 		return NULL;
// 	}

// 	free(fromIndices);
// 	free(toIndices);
// 	return network;
// }


// PyObject *PyCXNetworkRewire(PyObject* self, PyObject* args){
// 	PyObject* pyObject;
// 	Py_ssize_t verticesCount;
// 	float rewireProbability;
// 	if (!PyArg_ParseTuple(args, "Onf", &pyObject, &verticesCount, &rewireProbability)){
// 		PyErr_SetString(PyExc_AttributeError, "three parameters should be provided");
// 		return NULL;
// 	}

// 	CVNetwork* network = PyCXNewNetwork(pyObject,verticesCount,CVFalse);
// 	if(!network){
// 		PyErr_SetString(PyExc_AttributeError, "a problem happened while converting network");
// 		return NULL;
// 	}

// 	CVNetwork* rewiredNetwork = CVNewNetworkFromRandomRewiring(network,rewireProbability);
// 	if(!rewiredNetwork){
// 		PyErr_SetString(PyExc_AttributeError, "a problem happened while converting network");
// 		return NULL;
// 	}

// 	// FILE* networkFile = fopen("oioi.xnet","w");
// 	// CVNetworkWriteToFile(rewiredNetwork,networkFile);
// 	// fclose(networkFile);

// 	PyObject* newEdgesList = PyCXNewEdgesListFromNetwork(rewiredNetwork);
	
// 	CVNetworkDestroy(network);
// 	CVNetworkDestroy(rewiredNetwork);
// 	return newEdgesList;
// }


PyArrayObject *pyvector(PyObject *objin)  {
	return (PyArrayObject *) PyArray_ContiguousFromObject(objin,NPY_FLOAT, 1,1);
}
/* ==== Create 1D Carray from PyArray ======================
    Assumes PyArray is contiguous in memory.             */
void *pyvector_to_Carrayptrs(PyArrayObject *arrayin)  {
	int i,n;
	
	n=arrayin->dimensions[0];
	return arrayin->data;  /* pointer to arrayin data as double */
}

/* ==== Check that PyArrayObject is a double (Float) type and a vector ==============
    return 1 if an error and raise exception */ 
int  not_floatvector(PyArrayObject *vec)  {
	if (vec->descr->type_num != NPY_FLOAT)  {
		PyErr_SetString(PyExc_ValueError,
			"In not_floatvector: array must be of type Float and 1 dimensional (n).");
		return 1;  }
	return 0;
}


/* ==== Check that PyArrayObject is a double (Float) type and a vector ==============
    return 1 if an error and raise exception */ 
// FIXME: make it work for 32bits
int  not_intvector(PyArrayObject *vec)  {
	if (vec->descr->type_num != NPY_UINT64)  {
		PyErr_SetString(PyExc_ValueError,
			"In not_intvector: array must be of type Long and 1 dimensional (n).");
		return 1;  }
	return 0;
}



void _iterate(PyCXLayoutParameters* par){
	while(1){
		CVNetworkIteratePositions(par->edges, par->R, par->dR,
		par->edgesCount, par->verticesCount, par->internalIterations,
		par->attractiveConstant,par->repulsiveConstant,par->viscosityConstant);
		if(par->shallStop){
			break;
		}
	}
	pthread_exit(0);
}


PyObject* PyCXNetworkLayout(PyObject *self, PyObject *args){
	PyArrayObject *positions;
	PyArrayObject *speeds;
	PyArrayObject *edges; 
	float *positionsArray;
	float *speedsArray;
	CVIndex *edgesArray;
	float attractiveConstant = -1;
	float repulsiveConstant = -1;
	float viscosityConstant = -1;
	CVIndex i,j,n;
	
	/* Parse tuples separately since args will differ between C fcns */
	if (!PyArg_ParseTuple(args, "O!O!O!|fff",
			&PyArray_Type, &edges,
			&PyArray_Type, &positions,
			&PyArray_Type, &speeds,
			&attractiveConstant,
			&repulsiveConstant,
			&viscosityConstant
		)){
			return NULL;
		}
	if (edges == NULL){
		return NULL;
	}
	if (positions == NULL){
		return NULL;
	}
	if (speeds == NULL){
		return NULL;
	}
	
	/* Check that objects are 'double' type and vectors
	     Not needed if python wrapper function checks before call to this routine */

	if (not_intvector(edges)){
		return NULL;
	}
	if (not_floatvector(positions)){
		return NULL;
	}
	if (not_floatvector(speeds)){
		return NULL;
	}
	
	/* Change contiguous arrays into C * arrays   */
	edgesArray=pyvector_to_Carrayptrs(edges);
	positionsArray=pyvector_to_Carrayptrs(positions);
	speedsArray=pyvector_to_Carrayptrs(speeds);
	
	/* Get vector dimension. */
	CVSize vertexCount=positions->dimensions[0];
	CVSize edgesCount=edges->dimensions[0];
	//Check dimensions here

	/* Operate on the vectors  */
	// for ( i=0; i<n; i++)  {
	// 	positionsArray[i]=10*positionsArray[i];
	// }
	PyCXLayoutParameters par;

	par.edges = edgesArray;
	par.R = positionsArray;
	par.dR = speedsArray;
	par.edgesCount = edgesCount;
	par.verticesCount = vertexCount;
	par.iterations = 50000;
	par.attractiveConstant = attractiveConstant;
	par.repulsiveConstant = repulsiveConstant;
	par.viscosityConstant = viscosityConstant;
	
	// void CVNetworkIteratePositions(edgesArray,positionsArray,
	// 	speedsArray, edgesCount, vertexCount, 2,
	// 	attractiveConstant,
	// 	repulsiveConstant,
	// 	viscosityConstant);
	
	// thrd_t thread;
	// int result;

	// thrd_create(&thread, run, NULL);

	CVNetworkIteratePositions(edgesArray, positionsArray, speedsArray,
	edgesCount, vertexCount, 2,
	attractiveConstant,repulsiveConstant,viscosityConstant);
	
	// #if CV_USE_OPENMP
	//   omp_set_num_threads(8);
	// #endif //_OPENMP

	// thrd_t tid;
	// thrd_create(&tid, _iterate, par);
	return Py_BuildValue("i", 1);
}




PyObject* PyCXNetworkLayoutStart(PyObject *self, PyObject *args){
	PyArrayObject *positions;
	PyArrayObject *speeds;
	PyArrayObject *edges; 
	float *positionsArray;
	float *speedsArray;
	CVIndex *edgesArray;
	float attractiveConstant = -1;
	float repulsiveConstant = -1;
	float viscosityConstant = -1;
	CVIndex i,j,n;

	/* Parse tuples separately since args will differ between C fcns */
	if (!PyArg_ParseTuple(args, "O!O!O!|fff",
			&PyArray_Type, &edges,
			&PyArray_Type, &positions,
			&PyArray_Type, &speeds,
			&attractiveConstant,
			&repulsiveConstant,
			&viscosityConstant
		)){
			return NULL;
		}
	if (edges == NULL){
		return NULL;
	}
	if (positions == NULL){
		return NULL;
	}
	if (speeds == NULL){
		return NULL;
	}
	

	/* Check that objects are 'double' type and vectors
	     Not needed if python wrapper function checks before call to this routine */

	if (not_intvector(edges)){
		return NULL;
	}
	if (not_floatvector(positions)){
		return NULL;
	}
	if (not_floatvector(speeds)){
		return NULL;
	}
	
	/* Change contiguous arrays into C * arrays   */
	edgesArray=pyvector_to_Carrayptrs(edges);
	positionsArray=pyvector_to_Carrayptrs(positions);
	speedsArray=pyvector_to_Carrayptrs(speeds);
	

	/* Get vector dimension. */
	CVSize vertexCount=positions->dimensions[0];
	CVSize edgesCount=edges->dimensions[0];
	//Check dimensions here

	/* Operate on the vectors  */
	// for ( i=0; i<n; i++)  {
	// 	positionsArray[i]=10*positionsArray[i];
	// }
	PyCXLayoutParameters* par = calloc(1,sizeof(PyCXLayoutParameters));

	par->edges = edgesArray;
	par->R = positionsArray;
	par->dR = speedsArray;
	par->edgesCount = edgesCount;
	par->verticesCount = vertexCount;
	par->internalIterations = 1;
	par->iterations = 20000;
	par->attractiveConstant = attractiveConstant;
	par->repulsiveConstant = repulsiveConstant;
	par->viscosityConstant = viscosityConstant;
	
	// void CVNetworkIteratePositions(edgesArray,positionsArray,
	// 	speedsArray, edgesCount, vertexCount, 2,
	// 	attractiveConstant,
	// 	repulsiveConstant,
	// 	viscosityConstant);
	
	// CVNetworkIteratePositions(edgesArray, positionsArray, speedsArray,
	// edgesCount, vertexCount, 2,
	// attractiveConstant,repulsiveConstant,viscosityConstant);
	
	// // #if CV_USE_OPENMP
	// //   omp_set_num_threads(8);
	// // #endif //_OPENMP
	
	pthread_create(&(par->thread), NULL,(void *)_iterate, par);

	// return Py_BuildValue("i", 1);

	return Py_BuildValue("L", (long long)par);
}


PyObject* PyCXNetworkLayoutStop(PyObject *self, PyObject *args){
	long long parPointerID = 0;
	/* Parse tuples separately since args will differ between C fcns */
	if (!PyArg_ParseTuple(args, "L",&parPointerID)){
			return NULL;
	}

	if (parPointerID == 0){
		Py_RETURN_NONE;
	}

	PyCXLayoutParameters* par = (void *)parPointerID;
	par->shallStop=CVTrue;
	pthread_join(par->thread, NULL);
	free(par);
	Py_RETURN_NONE;
}




