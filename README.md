# Helios

Helios is a python library implemented in C for layouting and visualizing complex networks.

## Layout

Helios implements a force layout algorithm based on the FR algorithm [1].

## Install

Requires python headers and a C11 compatible compiler, such as gcc or clang.

To install it, simply run:

```bash
pip install helios-python
```

or clone this repository and install it from master by running:

```bash
pip install git+git://github.com/heliosnet/helios-core.git
```
## Usage

Currently only the layout interface is implemented.
You can layout a graph by running

```python
import numpy as np
import helios

positions = np.array([
  [1,2,3],
  [4,5,6],
  [7,8,9],
  [10,11,12]
],dtype=np.float32);

edges = np.array([
  [0,1],
  [2,3]
],dtype=np.uint64);

positions = np.ascontiguousarray(positions,dtype=np.float32);
edges = np.ascontiguousarray(edges,dtype=np.uint64);
speeds = np.zeros(positions.shape,dtype=np.float32);
speeds = np.ascontiguousarray(speeds,dtype=np.float32);
for i in range(100):
  helios.layout(edges,positions,speeds);
  print(positions);
```


## References

[1] Fruchterman, T. M. J., & Reingold, E. M. (1991). Graph Drawing by Force-Directed Placement. Software: Practice and Experience, 21(11).
