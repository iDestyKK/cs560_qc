import os
import sys


if len(sys.argv) != 2:
    print("ERROR: usage: ./query outputfilename\n")
    print("Exiting now...")
    sys.exit();

words = input("Enter word(s) to query: \n")

words = words.split(" ")
for word in words:
    found = 0
    fin = open(str(sys.argv[1]), "r")
    for num, line in enumerate(fin):
        line = line.split("\t")
        if word == line[0]:
            if found == 0:
                print("'"+word+"' appears in the following documents and on the following lines:\n")
            print(line[1]+"\t"+line[2])
            found = 1
    if found == 0:
        print("I'm sorry, '"+word+"' does not appear in any of the files.\n") 
