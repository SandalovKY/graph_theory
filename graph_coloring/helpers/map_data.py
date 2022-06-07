import sys

from pexpect import EOF


path_to_file = sys.argv[1]
file = open(path_to_file)

next_line = file.readline()

retMap = {}
currInd = 1

retList = []

while len(next_line) != 0:
    parsed_line = next_line.split(' ')
    retList.append([int(parsed_line[0]), int(parsed_line[1])])
    if retMap.get(int(parsed_line[0])) is None:
        retMap[int(parsed_line[0])] = currInd
        currInd += 1
    if retMap.get(int(parsed_line[1])) is None:
        retMap[int(parsed_line[1])] = currInd
        currInd += 1
    next_line = file.readline()

file.close()

# mapped_file = open("mapped_file", 'w')
# for vert in retMap:
#     mapped_file.write(str(retMap[vert]) + ' ' + str(vert) + '\n')
# mapped_file.close()

updated_file = open(path_to_file + '_reordered', 'w')
for edge in retList:
    updated_file.write('e ' + str(retMap[edge[0]]) + ' ' + str(retMap[edge[1]]) + '\n')
updated_file.close()

