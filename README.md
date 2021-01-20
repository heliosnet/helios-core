![Build Test and Publish](https://github.com/heliosnet/helios-core/workflows/Build%20Test%20and%20Publish/badge.svg?event=push)

# Helios

Helios is a python library implemented in C for layouting and visualizing complex networks.

## Layout

Helios implements a force layout algorithm based on the FR algorithm [1].

## Install

Requires python headers and a C11 compatible compiler, such as gcc or clang.

To install it, simply run:

```bash
pip install helios
```

or clone this repository and install it from master by running:

```bash
pip install git+git://github.com/heliosnet/helios-core.git
```
## Usage

Currently only the FR layout interface is implemented.

Example initialization with a small network.  

```python
import numpy as np
import helios

positions = np.random.random((4, 3))

edges = np.array([
  [0,1],
  [2,3]
],dtype=np.uint64)

# positions is required to be an contiguous float32 numpy array
positions = np.ascontiguousarray(positions,dtype=np.float32)

# edges is required to be an contiguous uint64 numpy array
edges = np.ascontiguousarray(edges,dtype=np.uint64)

# speeds is required to be an contiguous uint64 numpy array
speeds = np.zeros(positions.shape,dtype=np.float32)

layout = helios.FRLayout(edges,positions,speeds)
```

Two APIS are available to iterate the layouts, synchronized and aynchronous.

Example of the synchronized API:

```python 
print("Initial positions:")
print(layout.positions)
layout.iterate(iterations=100)
print("Final positions:")
print(layout.positions)
```

Example using the aynchronous API:

```python
print("Initial positions:")
print(layout.positions)
if(layout.running()): #False
  print("It is running...")
else:
  print("Not running...")
layout.start()
time.sleep(1)
print("Current positions:")
print(layout.positions)
time.sleep(1)
print("Current positions:")
print(layout.positions)
if(layout.running()): #True
  print("It is running...")
else:
  print("Not running...")
layout.stop()
print("Final positions:")
print(layout.positions)
```
You can restart the layout once it stopped. Subsequent calls to the `start` method have no effect if the layout is running.


## Compiling on windows:

You need m2w64 and libpython to compile it.
```bash
conda install -c anaconda libpython
conda install -c msys2 m2w64-toolchain
```

You can now use pip install:

```bash
pip install ./
```
or compile distribs: 

```bash
python setup.py sdist bdist_wheel
```

Creating a build environment using:
```
conda create -n buildpy36 -c default -c anaconda -c msys2 python=3.6 numpy libpython m2w64-toolchain 
```

change `python=3.6` and `buildpy36` to the version of python you would like to use.



## References

[1] Fruchterman, T. M. J., & Reingold, E. M. (1991). Graph Drawing by Force-Directed Placement. Software: Practice and Experience, 21(11).
