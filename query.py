import os
import sys


if len(sys.argv) != 2:
    print("ERROR: usage: ./query outputfilename\n")
    print("Exiting now...")
    sys.exit();

words = input("Enter word(s) to query: \n")
#print(word)

words = words.split(" ")
for word in words:
    print("w: "+word+"\n")
    found  = 0
    fin = open(str(sys.argv[1]), "r")
    for num, line in enumerate(fin):
        if word in line:
            if found == 0:
                print(word+" appears in the following documents and on the following lines:\n")
            line = line.split("\t")
            print(line[1]+"\t"+line[2]+"\n")
            found = 1
    if found == 0:
        print("I'm sorry, '"+word+"' does not appear in any of the files.\n") 
