#ifndef __PYCXNETWORK_H__
#define __PYCXNETWORK_H__

#include <Python.h>


PyObject *PyCXRandomSeedDev(PyObject*, PyObject*);
PyObject *PyCXRandomSeed(PyObject*, PyObject*);

PyObject * PyCXNetworkLayout(PyObject*,PyObject*);

#endif
