# a quick and dirty script to turn those shader files into a C header with strings in it

header = open("shaders.h", "w")

header.write("#pragma once\n")

def writeFile(fileName, stringName):
    with open(fileName, "r") as f:
        shader = f.readlines()
        header.write(f'const char *{stringName} = "\\\n')
        header.writelines([l.rstrip('\n') +'\\n\\\n' for l in shader])
        header.write('";\n')

writeFile("shader.vert", "vertexShader")
writeFile("shader.frag", "fragmentShader")