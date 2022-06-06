import pandas as ps

import json

import sys
import os

def readFromCsv():
    path_to_file = sys.argv[1]
    data = ps.read_csv(path_to_file)
    print(data)
    return data

def readFromJson():
    path_to_file = sys.argv[1]
    file = open(path_to_file)
    data = json.load(file)
    file.close()

def write2GraphFile(data):
    new_file_path = 'git.clq'
    file = open(new_file_path, 'w')

    for index, row in data.iterrows():
        file.write('e ' + str(row['id_1'] + 1) + ' ' + str(row['id_2'] + 1) + '\n')
        


write2GraphFile(readFromCsv())

# readFromJson()
