inFile = open("RequestData.py", 'r')
outFile = open("RequestData.txt", 'w')

for line in inFile:
    newLine = "\""
    for word in line[:-1]:
        if(word == "\""):
            newLine += "\\"
        newLine += word
    print >> outFile, newLine, "\\n\""

inFile.close()
outFile.close()
