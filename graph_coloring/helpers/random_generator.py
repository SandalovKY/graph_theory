import networkx
import matplotlib.pyplot as plt
import sys

from numpy import double

if len(sys.argv) < 3: sys.exit()
numEdges = int(sys.argv[1])
density = int(sys.argv[2])

g = networkx.generators.barabasi_albert_graph(numEdges, density)

networkx.write_edgelist(g, 'barabasi_rand_' + str(numEdges) + '_' + str(density))
plt.show()