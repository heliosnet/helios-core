#ifndef __PYCXNETWORK_H__
#define __PYCXNETWORK_H__

#include "../Includes/CVNetwork.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>




PyObject *PyCXNetworkLayout(PyObject*,PyObject*,PyObject*);
PyObject *PyCXNetworkLayoutStart(PyObject*,PyObject*,PyObject*);
PyObject *PyCXNetworkLayoutStop(PyObject*,PyObject*);

#endif
