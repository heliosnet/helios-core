#ifndef __PYCXNETWORK_H__
#define __PYCXNETWORK_H__

#include <Python.h>
#include <CVNetwork.h>




PyObject * PyCXNetworkLayout(PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStart(PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStop(PyObject*,PyObject*);

#endif
