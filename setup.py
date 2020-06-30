#!/usr/bin/env python3

from setuptools import setup, Extension, Command
import os.path;
import platform;
import numpy as np;
enableParallelism = True;

extraOptions = []
extraLinkOptions=[]
if(platform.system()=="Darwin"):
	extraOptions = ["-D OSX"];
	if(enableParallelism):
		extraOptions += ["-DCV_USE_LIBDISPATCH=1"];
elif(platform.system()=="Windows"):
	extraOptions = ["-D WIN32"];
	if(enableParallelism):
		extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"];
		extraLinkOptions+=["-lgomp"];
elif(platform.system()=="Linux"):
	extraOptions = ["-D Linux","-D_GNU_SOURCE=1"];
	if(enableParallelism):
		extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"];
		extraLinkOptions+=["-lgomp"];
else:
	if(enableParallelism):
		extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"];
		extraLinkOptions+=["-lgomp"];

import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

building_on_windows = platform.system() == "Windows"

setup(
	name="helios",
	version="0.1.8",
	author="Filipi N. Silva",
	author_email="filsilva@iu.edu",
	compiler = "mingw32" if building_on_windows else None,
	description="Experimental library to visualize complex networks",
	long_description=long_description,
	long_description_content_type="text/markdown",
	url="https://github.com/heliosnet/helios-core",
	packages=setuptools.find_packages(),
	classifiers=[
			"Programming Language :: Python :: 3",
			"License :: OSI Approved :: MIT License",
			"Operating System :: MacOS :: MacOS X",
			"Operating System :: Microsoft :: Windows",
			"Operating System :: POSIX :: Linux",
			"Development Status :: 3 - Alpha",
			"Programming Language :: C",
			"Topic :: Scientific/Engineering :: Visualization",
			"Intended Audience :: Science/Research"
	],
	python_requires='>=3.0',
	ext_modules = [
		Extension(
			"helios",
			sources=[
				os.path.join("helios-core","Source", "CVSimpleQueue.c"),
				os.path.join("helios-core","Source", "CVSet.c"),
				os.path.join("helios-core","Source", "CVNetwork.c"),
				os.path.join("helios-core","Source", "CVDictionary.c"),
				os.path.join("helios-core","Source", "CVNetworkLayout.c"),
				os.path.join("helios-core","Python", "PyCXNetwork.c"),
				os.path.join("helios-core","Python", "PyCXBind.c"),
			],
			include_dirs=[
				os.path.join("helios-core","Source"),
				os.path.join("helios-core","Python"),
				np.get_include()
			],
			extra_compile_args=[
				# "-g",
				"-std=c11",
				#"-m64",
				"-Wall",
				"-Wno-unused-function",
				"-Wno-deprecated-declarations",
				"-Wno-sign-compare",
				"-Wno-strict-prototypes",
				"-Wno-unused-variable",
				"-O3",
				# "-fvisibility=hidden",
				"-funroll-loops",
				"-fstrict-aliasing"
			]+extraOptions,
			extra_link_args=extraLinkOptions,
		),
	]
);
