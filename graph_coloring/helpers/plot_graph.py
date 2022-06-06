import networkx as nx
import seaborn as sns
import matplotlib.pyplot as plt
import sys

class GraphVisualization:
    def __init__(self):
        self.visual = []
    
    def add_edge(self, a, b):
        tmp = [a, b]
        self.visual.append(tmp)
    
    def visualize(self):
        graph = nx.Graph()
        graph.add_edges_from(self.visual)
        nx.draw_networkx(graph)
        plt.show()


def readGraphFromFile():
    gv = GraphVisualization()

    file_path = sys.argv[1]
    file = open(file_path, 'r')
    
    next_line = file.readline()
    parsed_line = next_line.split(' ')
    while parsed_line[0] != 'p':
        next_line = file.readline()
        parsed_line = next_line.split(' ')
    
    num_edges = parsed_line[3]
    for ind in range(int(num_edges)):
        next_line = file.readline()
        parsed_line = next_line.split(' ')
        assert parsed_line[0] != 'e' 'Incorrect file input'
        gv.add_edge(int(parsed_line[1]), int(parsed_line[2]))
    print('Successfully readed file')
    gv.visualize()
    
readGraphFromFile()