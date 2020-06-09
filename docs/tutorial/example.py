import numpy as np
import time
import helios
import xnetwork
import igraph as ig

# networkSize = 10000;
network = xnetwork.xnet2igraph("/Users/filipi/Dropbox/Projects/CDT-Visualization/Networks/Content/WS_10000_10_001-major.xnet")
edges = np.ascontiguousarray(network.get_edgelist(),dtype=np.uint64);
networkSize = network.vcount();

positions = np.random.random((networkSize, 3));
# edges = np.random.randint(0,networkSize-1,(networkSize, 2));

positions = np.ascontiguousarray(positions,dtype=np.float32);
edges = np.ascontiguousarray(edges,dtype=np.uint64);
speeds = np.zeros(positions.shape,dtype=np.float32);
speeds = np.ascontiguousarray(speeds,dtype=np.float32);

threadID = helios.startAsyncLayout(edges,positions,speeds);
try:
  for i in range(1000):
    time.sleep(10)
    print(positions);
except KeyboardInterrupt:
  print("got keyboardInterrupt...")
  helios.stopAsyncLayout(threadID)
  threadID=0
except Exception as e:
  print("got exception...")
  helios.stopAsyncLayout(threadID)
  threadID=0;
  raise
  
helios.stopAsyncLayout(threadID)




