import numpy as np

import helios


positions = np.array([[1,2,3],[4,5,6],[7,8,9],[10,11,12]],dtype=np.float32);
edges = np.array([[0,1],[2,3]],dtype=np.uint64);

positions = np.ascontiguousarray(positions,dtype=np.float32);
edges = np.ascontiguousarray(edges,dtype=np.uint64);
speeds = np.zeros(positions.shape,dtype=np.float32);
speeds = np.ascontiguousarray(speeds,dtype=np.float32);
for i in range(100):
  helios.layout(edges,positions,speeds);
  print(positions);
