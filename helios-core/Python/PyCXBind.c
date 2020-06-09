#include "PyCXNetwork.h"

#define PY_ARRAY_UNIQUE_SYMBOL helios_ARRAY_API
#include <numpy/arrayobject.h>

char layoutfunc_docs[] = "Layout network.";
char asyncLayoutStart_docs[] = "Async Layout network start.";
char asyncLayoutStop_docs[] = "Async Layout network stop.";
char randomSeedfunc_docs[] = "Rewire network.";
char randomSeedDevfunc_docs[] = "Rewire network.";

PyMethodDef helios_funcs[] = {
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
	{
		"setRandomSeed",
		(PyCFunction)PyCXRandomSeed,
		METH_VARARGS,
		randomSeedfunc_docs
	},
	{
		"randomSeedDev",
		(PyCFunction)PyCXRandomSeedDev,
		METH_VARARGS,
		randomSeedDevfunc_docs
	},
	{
		NULL
	}
};

char heliosmod_docs[] = "This is CXNetwork module.";

PyModuleDef helios_mod = {
		PyModuleDef_HEAD_INIT,
		"helios",
		heliosmod_docs,
		-1,
		helios_funcs,
		NULL,
		NULL,
		NULL,
		NULL};

PyMODINIT_FUNC PyInit_helios(void)
{
	import_array();
	return PyModule_Create(&helios_mod);
}
