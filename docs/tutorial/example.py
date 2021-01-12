import numpy as np
import time
import helios

networkSize = 10;
positions = np.random.random((networkSize, 3))
edges = np.random.randint(0,networkSize-1,(networkSize*2, 2))
edges = np.ascontiguousarray(edges,dtype=np.uint64)

positions = np.ascontiguousarray(positions,dtype=np.float32)
speeds = np.zeros(positions.shape,dtype=np.float32)

layout = helios.FRLayout(edges,positions,speeds)
print("Initial positions:")
print(layout.positions)
layout.iterate(iterations=100)
print("Final positions:")
print(layout.positions)

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
