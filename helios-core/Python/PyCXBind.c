
#define PY_SSIZE_T_CLEAN
#include "PyCXVersion.h"
#include <Python.h>
#include "structmember.h"
#include <pthread.h>
#include <sys/types.h>
#include "PyCXNetwork.h"
#include <CVNetwork.h>
#include <CVNetworkLayout.h>
// #define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL helios_ARRAY_API
#include <numpy/arrayobject.h>

#if CV_USE_OPENMP
#include <omp.h>
#endif //_OPENMP



static PyArrayObject *pyvector(PyObject *objin)  {
	return (PyArrayObject *) PyArray_ContiguousFromObject(objin,NPY_FLOAT, 1,1);
}
/* ==== Create 1D Carray from PyArray ======================
    Assumes PyArray is contiguous in memory.             */
static void *pyvector_to_Carrayptrs(PyArrayObject *arrayin)  {
	int i,n;
	
	n=arrayin->dimensions[0];
	return arrayin->data;  /* pointer to arrayin data as double */
}

/* ==== Check that PyArrayObject is a double (Float) type and a vector ==============
    return 1 if an error and raise exception */ 
static int  not_floatvector(PyArrayObject *vec)  {
	if (vec->descr->type_num != NPY_FLOAT)  {
		PyErr_SetString(PyExc_ValueError,
			"In not_floatvector: array must be of type Float and 1 dimensional (n).");
		return 1;  }
	return 0;
}


/* ==== Check that PyArrayObject is a double (Float) type and a vector ==============
    return 1 if an error and raise exception */ 
// FIXME: make it work for 32bits
static int  not_intvector(PyArrayObject *vec)  {
	if (vec->descr->type_num != NPY_UINT64)  {
		PyErr_SetString(PyExc_ValueError,
			"In not_intvector: array must be of type Long and 1 dimensional (n).");
		return 1;  }
	return 0;
}


char layoutfunc_docs[] = "Layout network.";
char asyncLayoutStart_docs[] = "Async Layout network start.";
char asyncLayoutStop_docs[] = "Async Layout network stop.";

static PyMethodDef helios_funcs[] = {
	{
		"layout",
		(PyCFunction)PyCXNetworkLayout,
		METH_VARARGS,
		layoutfunc_docs
	},
	{
		"startAsyncLayout",
		(PyCFunction)PyCXNetworkLayoutStart,
		METH_VARARGS,
		asyncLayoutStart_docs
	},
	{
		"stopAsyncLayout",
		(PyCFunction)PyCXNetworkLayoutStop,
		METH_VARARGS,
		asyncLayoutStop_docs
	},
	{NULL}
};


typedef struct {
		PyObject_HEAD
		PyArrayObject *edgesArray;
		PyArrayObject *positionsArray;  /* last name */
		PyArrayObject *velocitiesArray;  /* last name */
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
		CVSize _threadIterations;
		CVBool _shallStop;
		CVBool _isRunning;
		pthread_t _thread;
} PyFRLayout;

int PyFRLayout_traverse(PyFRLayout *self, visitproc visit, void *arg){
		Py_VISIT(self->edgesArray);
		Py_VISIT(self->positionsArray);
		Py_VISIT(self->velocitiesArray);
		return 0;
}

int PyFRLayout_clear(PyFRLayout *self){
		Py_CLEAR(self->edgesArray);
		Py_CLEAR(self->positionsArray);
		Py_CLEAR(self->velocitiesArray);
		return 0;
}

void PyFRLayout_dealloc(PyFRLayout *self){
		PyObject_GC_UnTrack(self);
		PyFRLayout_clear(self);
		Py_TYPE(self)->tp_free((PyObject *) self);
}

PyObject * PyFRLayout_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
		PyFRLayout *self;
		self = (PyFRLayout *) type->tp_alloc(type, 0);
		self->edgesArray = NULL;
		self->positionsArray = NULL;
		self->velocitiesArray = NULL;
		self->edges = NULL;
		self->R = NULL;
		self->dR = NULL;
		self->edgesCount = 0;
		self->verticesCount = 0;
		self->iterations = 20000;
		self->internalIterations = 1;
		self->attractiveConstant = -1;
		self->repulsiveConstant = -1;
		self->viscosityConstant = -1;
		self->_threadIterations = 1;
		self->_shallStop = CVFalse;
		self->_isRunning = CVFalse;
		return (PyObject *) self;
}

int PyFRLayout_init(PyFRLayout *self, PyObject *args, PyObject *kwds){
		static char *kwlist[] = {
			"edges",
			"positions",
			"velocities",
			"attractiveConstant",
			"repulsiveConstant",
			"viscosityConstant",
			NULL
		};
		PyArrayObject *edgesArray = NULL;
		PyArrayObject *positionsArray = NULL;
		PyArrayObject *velocitiesArray = NULL; 
		PyArrayObject *tmp = NULL; 

		if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!|fff", kwlist,
														&PyArray_Type, &edgesArray,
														&PyArray_Type, &positionsArray,
														&PyArray_Type, &velocitiesArray,
														&self->attractiveConstant,
														&self->repulsiveConstant,
														&self->viscosityConstant)){
				return -1;
		}

		if (edgesArray) {
				tmp = self->edgesArray;
				Py_INCREF(edgesArray);
				self->edgesArray = edgesArray;
				Py_XDECREF(tmp);
		}else{
			return -1;
		}

		if (positionsArray) {
				tmp = self->positionsArray;
				Py_INCREF(positionsArray);
				self->positionsArray = positionsArray;
				Py_XDECREF(tmp);
		}else{
			return -1;
		}

		if (velocitiesArray) {
				tmp = self->velocitiesArray;
				Py_INCREF(velocitiesArray);
				self->velocitiesArray = velocitiesArray;
				Py_XDECREF(tmp);
		}else{
			return -1;
		}
		
		if (not_intvector(self->edgesArray)){
        PyErr_SetString(PyExc_TypeError,"The edges attribute must be a int numpy array.");
			return -1;
		}
		if (not_floatvector(self->positionsArray)){
        PyErr_SetString(PyExc_TypeError,"The positions attribute must be a float numpy array.");
			return -1;
		}
		if (not_floatvector(self->velocitiesArray)){
        PyErr_SetString(PyExc_TypeError,"The velocities attribute must be a float numpy array.");
			return -1;
		}

		self->verticesCount =self->positionsArray->dimensions[0];
		self->edgesCount=self->edgesArray->dimensions[0];
		
		self->edges=pyvector_to_Carrayptrs(self->edgesArray);
		self->R=pyvector_to_Carrayptrs(self->positionsArray);
		self->dR=pyvector_to_Carrayptrs(self->velocitiesArray);


		
		self->_shallStop = CVFalse;
		return 0;
}

PyMemberDef PyFRLayout_members[] = {
		{"attractiveConstant", T_FLOAT, offsetof(PyFRLayout, attractiveConstant), 0,"Attractive constant"},
		{"repulsiveConstant", T_FLOAT, offsetof(PyFRLayout, repulsiveConstant), 0,"Repulsive constant"},
		{"viscosityConstant", T_FLOAT, offsetof(PyFRLayout, viscosityConstant), 0,"Viscosity constant"},
		{NULL}  /* Sentinel */
};

PyObject * PyFRLayout_getEdges(PyFRLayout *self, void *closure){
	Py_INCREF(self->edgesArray);
	return (PyObject*)self->edgesArray;
}
PyObject * PyFRLayout_getPositions(PyFRLayout *self, void *closure){
	Py_INCREF(self->positionsArray);
	return (PyObject*)self->positionsArray;
}
PyObject * PyFRLayout_getVelocities(PyFRLayout *self, void *closure){
	Py_INCREF(self->velocitiesArray);
	return (PyObject*)self->velocitiesArray;
}

static PyGetSetDef PyFRLayout_getsetters[] = {
		{"edges", (getter) PyFRLayout_getEdges,  NULL,"Edges array", NULL},
		{"positions", (getter) PyFRLayout_getPositions,  NULL,"Positions array", NULL},
		{"velocities", (getter) PyFRLayout_getVelocities,  NULL,"Velocities array", NULL},
		{NULL}  /* Sentinel */
};

static void _iterate(PyFRLayout* layout){
	CVNetworkIteratePositions(layout->edges, layout->R, layout->dR,
		layout->edgesCount, layout->verticesCount, layout->internalIterations,
		layout->attractiveConstant,layout->repulsiveConstant,layout->viscosityConstant);
		
}

static void _threadIterate(PyFRLayout* layout){
	while(1){
		_iterate(layout);
		if(layout->_shallStop){
			break;
		}
	}
	pthread_exit(0);
	
}

PyObject* PyFRLayout_iterate(PyFRLayout *self, PyObject *args, PyObject *kwds){
	static char *kwlist[] = {
		"iterations",
		NULL
	};
	long int iterationsCount = 1;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|l", kwlist,&iterationsCount)){
			return NULL;
	}

	if(self->_isRunning){
		printf("Is Running\n");
		PyErr_SetString(PyExc_BrokenPipeError,"The layout is running. Stop it before using iterate.");
		return NULL;
	}

	for(CVIndex iteration=0;iteration<iterationsCount;iteration++){
		_iterate(self);
	}
	Py_RETURN_NONE;
}

PyObject* PyFRLayout_isRunning(PyFRLayout *self, PyObject *Py_UNUSED(ignored)){
	return Py_BuildValue("O", self->_isRunning ? Py_True: Py_False);
}


PyObject* PyFRLayout_startLayout(PyFRLayout *self, PyObject *Py_UNUSED(ignored)){
	if(!self->_isRunning){
		self->_isRunning = CVTrue;
		pthread_create(&(self->_thread), NULL, (void*)_threadIterate, self);
	}
	Py_RETURN_NONE;
}

PyObject* PyFRLayout_stopLayout(PyFRLayout *self, PyObject *Py_UNUSED(ignored)){

	if(self->_isRunning){
		self->_shallStop=CVTrue;
		pthread_join(self->_thread, NULL);
		self->_isRunning=CVFalse;
		self->_shallStop=CVFalse;
	}
	Py_RETURN_NONE;
}

static PyMethodDef PyFRLayout_methods[] = {
		{"start", (PyCFunction) PyFRLayout_startLayout, METH_NOARGS,
		 "Starts the layout algorithm"
		},
		{"stop", (PyCFunction) PyFRLayout_stopLayout, METH_NOARGS,
		 "Stops the layout algorithm"
		},
		{"running", (PyCFunction) PyFRLayout_isRunning, METH_NOARGS,
		 "Returns True if it is running"
		},
		{"iterate", (PyCFunction) PyFRLayout_iterate, METH_VARARGS | METH_KEYWORDS,
		 "Iterate the layout by the provided number of iterations."
		},
		{NULL}  /* Sentinel */
};

static PyTypeObject PyFRLayoutType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		.tp_name = "helios.FRLayout",
		.tp_doc = "PyFRLayout objects",
		.tp_basicsize = sizeof(PyFRLayout),
		.tp_itemsize = 0,
		.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
		.tp_new = PyFRLayout_new,
		.tp_init = (initproc) PyFRLayout_init,
		.tp_dealloc = (destructor) PyFRLayout_dealloc,
		.tp_traverse = (traverseproc) PyFRLayout_traverse,
		.tp_clear = (inquiry) PyFRLayout_clear,
		.tp_members = PyFRLayout_members,
		.tp_methods = PyFRLayout_methods,
		.tp_getset = PyFRLayout_getsetters,
};


char heliosmod_docs[] = "This is CXNetwork module.";

static PyModuleDef helios_mod = {
	PyModuleDef_HEAD_INIT,
	.m_name = "helios",
	.m_doc = heliosmod_docs,
	.m_size = -1,
	.m_methods = helios_funcs,
	.m_slots = NULL,
	.m_traverse = NULL,
	.m_clear = NULL,
	.m_free = NULL
};

PyMODINIT_FUNC PyInit_helios(void){
	import_array();

	PyObject *m;
	if (PyType_Ready(&PyFRLayoutType) < 0){
			return NULL;
	}
	m = PyModule_Create(&helios_mod);
	if (m == NULL){
			return NULL;
	}
	Py_INCREF(&PyFRLayoutType);
	if (PyModule_AddObject(m, "FRLayout", (PyObject *) &PyFRLayoutType) < 0) {
			Py_DECREF(&PyFRLayoutType);
			Py_DECREF(m);
			return NULL;
	}
	
	if (PyModule_AddStringConstant(m,"__version__",CVTOKENTOSTRING(k_PYCXVersion))) {
			Py_DECREF(m);
			return NULL;
	}
	

	return m;
}
