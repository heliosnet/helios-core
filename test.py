import numpy as np

import helios

networkSize = 1000;

positions = np.random.random((networkSize, 3));
edges = np.random.randint(0,networkSize-1,(networkSize, 2));

positions = np.ascontiguousarray(positions,dtype=np.float32);
edges = np.ascontiguousarray(edges,dtype=np.uint64);
speeds = np.zeros(positions.shape,dtype=np.float32);
speeds = np.ascontiguousarray(speeds,dtype=np.float32);
for i in range(100):
  helios.layout(edges,positions,speeds);
  print(positions);
