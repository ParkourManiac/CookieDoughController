import re
import os
import sys

if len(sys.argv) <= 1:
    raise NameError("ABORTED: Please provide a folder in which the testSuite.cpp will be generated.")
else:
    # Reads/grabs all tests from all files ending with Test.cpp
    currentDir = sys.argv[1] + '/'
    allMethodNames = []
    for fileName in os.listdir(currentDir):
        if fileName.endswith("Test.cpp"):
            with open(currentDir + fileName, "r") as file:
                text = file.read()
                regex = r"(?<=\nvoid\s)(.+)(?=\(\))"
                matches = re.findall(regex, text)

                for match in matches:
                    allMethodNames.append(match)

    # Reads/grabs all Fake functions and their parameters in folder "Fakes".
    currentDir = sys.argv[1] + '/Fakes'
    allMockableFunctions = []
    for fileName in os.listdir(currentDir):
        if fileName.endswith(".h"):
            with open(currentDir + fileName, "r") as file:
                text = file.read()
                regex = r"(?<=\n)([^\(\n]+)\s([^\(\n]+)\(([^\)]*)\)"
                matches = re.findall(regex, text)

                for match in matches:
                    returnType = match.group(1)
                    nameOfFunction = match.group(2)

                    rawParameters = match.group(3).split(",")
                    parameters = []
                    for rawParameter in rawParameters:
                        parameterParts = rawParameter.strip().split()
                        parameterName = parameterParts.pop(len(parameterParts) - 1)
                        parameterType = " ".join(parameterParts)
                        # TODO: CONTINUE EXTRACTING INFO FROM PARAMETER AND STORE PARAMETER AS DICTIONARY.

                    allMockableFunctions.append(match)

    with open(currentDir + "testSuite.cpp", "w") as file:
        # Write tests to RunTest function.
        file.write('// THIS FILE WAS AUTOGENERATED BY FILE generateTestSuite.py\n')
        file.write('#include "testSuite.h" \n#include "test.h"\n\n')

        for methodName in allMethodNames:
            file.write('void ' + methodName + '();\n')

        file.write('\nvoid RunTests() \n{\n')

        for methodName in allMethodNames:
            file.write('\tRUN_TEST(' + methodName + ');\n')
        
        file.write('}\n\n')

        # Write tests to RunTest function.
        file.write('\nvoid ResetMocks() \n{\n')

        #for methodName in allMethodNames:
        #    file.write('\tRUN_TEST(' + methodName + ');\n')
        
        file.write('}\n\n')
        

    
