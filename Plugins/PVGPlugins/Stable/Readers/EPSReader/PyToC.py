#!/usr/bin/python
import os, sys

inFile = sys.stdin
output = sys.stdout

inFile = open("EPSRequestInfo.py")
output = open("EPSRequestDataCleaned.txt", 'w')

output.write("requestInfo = \"")
for s in inFile:
    output.write(s.replace("\"", "\\\""))

inFile.close()
output.close()

inFile = open("EPSRequestDataCleaned.txt")
output = open("EPSRequestDataCleanedAgain.txt", 'w')

for s in inFile:
    #newLine = "initscript += \" "
    newLine = s[:-1].rstrip()
    #no empty lines or comment lines
    if (len(newLine) > 1 and not (newLine.lstrip()[0]  == "#") ):        
        #newLine += "\""
        newLine += "\\n\\\n"
        output.write(newLine)

output.write("\"")
inFile.close()
output.close()
