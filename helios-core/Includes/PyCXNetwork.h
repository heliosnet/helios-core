#ifndef __PYCXNETWORK_H__
#define __PYCXNETWORK_H__

#include <Python.h>
#include "../Includes/CVNetwork.h"




PyObject * PyCXNetworkLayout(PyObject*,PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStart(PyObject*,PyObject*,PyObject*);
PyObject * PyCXNetworkLayoutStop(PyObject*,PyObject*);

#endif
