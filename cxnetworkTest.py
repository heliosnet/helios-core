#!/usr/bin/env python3

import cxnetwork
import tqdm;
# print(helloworld.hello());
# print(helloworld.heyman(5, "StarNight"));
for i in tqdm.trange(0,100000000):
	cxnetwork.randomSeedDev();
	edges =  [(0,1),(1,2),(2,3),(3,4),(4,5),(5,6),(6,7),(7,8),(8,9),(9,10),(10,0)]
	# edges += [(0,2),(1,3),(2,4),(3,5),(4,6),(5,7),(6,8),(7,9),(8,10),(9,0),(10,1)]
	cxnetwork.rewire(edges,11,1.0)
