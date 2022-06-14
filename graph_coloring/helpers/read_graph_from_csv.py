import pandas as ps

import json

import sys
import os

from yaml import parse

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
    return data

def readFromTxt():
    path_to_file = sys.argv[1]
    file1 = open(path_to_file)
    line = file1.readline()
    new_file_path = 'email.clq'
    file = open(new_file_path, 'w')
    while len(line) != 0:
        parsedLine = line.split('\t')
        file.write('e ' + str(int(parsedLine[0]) + 1) + ' ' + str(int(parsedLine[1]) + 1) + '\n')
        line = file1.readline()
    file.close()
    file1.close()


def write2GraphFile(data):
    new_file_path = 'facebook_large.clq'
    file = open(new_file_path, 'w')

    for index, row in data.iterrows():
        file.write('e ' + str(row['node_1'] + 1) + ' ' + str(row['node_2'] + 1) + '\n')
    file.close()

def write2GraphFromTxt(data):
    new_file = 'email.clq'
    file = open(new_file, 'w')

    for edge in data:
        file.write('e' + str(edge[0]) + ' ' + str(edge[1]) + '\n')
        


# write2GraphFile(readFromCsv())
# write2GraphFromTxt(readFromTxt())
readFromTxt()

# readFromJson()
