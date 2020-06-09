import numpy as np
import time
import helios
import igraph as ig

networkSize = 100;
positions = np.random.random((networkSize, 3))
edges = np.random.randint(0,networkSize-1,(networkSize, 2))

positions = np.ascontiguousarray(positions,dtype=np.float32)
edges = np.ascontiguousarray(edges,dtype=np.uint64)
speeds = np.zeros(positions.shape,dtype=np.float32)
speeds = np.ascontiguousarray(speeds,dtype=np.float32)

threadID = helios.startAsyncLayout(edges,positions,speeds)
try:
  for i in range(4):
    time.sleep(1)
    print(positions);
except KeyboardInterrupt:
  print("got keyboardInterrupt...")
  helios.stopAsyncLayout(threadID)
  threadID=0
except Exception as e:
  print("got an exception...")
  helios.stopAsyncLayout(threadID)
  threadID=0;
  raise
helios.stopAsyncLayout(threadID)




