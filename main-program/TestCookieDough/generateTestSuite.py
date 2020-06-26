import re
import os
import sys

def FindAllTestFunctions(dir):
    # Reads/grabs all tests from all files ending with Test.cpp
    testFunctions = []
    for fileName in os.listdir(dir):
        if fileName.endswith("Test.cpp"):
            with open(dir + fileName, "r") as file:
                text = file.read()
                regex = r"(?<=\nvoid\s)(.+)(?=\(\))"
                matches = re.findall(regex, text)

                for match in matches:
                    testFunctions.append(match)
    
    return testFunctions

def FindAllMockableFunctions(dir):
    # Reads/grabs all Fake functions and their parameters in folder "Fakes".
    #allMockableFiles = []
    # Mockable classes/structs    r"(class|struct)\s([^\{\(\)\}\s]+)\s?{([^\}]+)};"
    mockableFunctions = []
    neccessaryIncludes = []
    for fileName in os.listdir(dir):
        if fileName.endswith(".h"):
            #currentFile = { 'name': '', 'functions': [], 'classes': [] }

            with open(dir + fileName, "r") as file:
                text = file.read()

                neccessaryIncludes.append(fileName)

                regex = r"([^\(\)\;\n]+)\s([^\(\)\;\n]+)\(([^\)\;]*)\)(?=\;)"
                matches = re.findall(regex, text)

                for match in matches:
                    returnType = match[0]
                    nameOfFunction = match[1]

                    rawParameters = match[2].split(",")
                    parameters = []
                    for rawParameter in rawParameters:
                        if len(rawParameter) > 0:
                            parameterParts = rawParameter.strip().split()
                            parameterName = parameterParts.pop(len(parameterParts) - 1)
                            parameterType = " ".join(parameterParts)

                            newParameter = { 'name': parameterName, 'type': parameterType }
                            parameters.append(newParameter)

                    newFunction = { 'returnType': returnType, 'name': nameOfFunction, 'parameters': parameters}
                    mockableFunctions.append(newFunction)
    
    return (mockableFunctions, neccessaryIncludes)

def PrintMockedFunctions(mockedFunctions):
    showMockedFunctions = '\nMocking functions: '
    for function in mockedFunctions:
        showMockedFunctions += function['name'] + ". "
    print(showMockedFunctions)

def WriteCodeForIncludingFakes(neccessaryIncludes, file):
    # Write all includes needed for mocked functions.
    for include in neccessaryIncludes:
        file.write('#include \"Fakes/' + include + '\"\n')
    file.write('\n')

def WriteCodeForRunningTests(testFunctions, file):
    # Declare all test functions
    for methodName in testFunctions:
        file.write('void ' + methodName + '();\n')

    # Run all tests
    file.write('\nvoid RunTests() \n{\n')

    for methodName in testFunctions:
        file.write('\tRUN_TEST(' + methodName + ');\n')
    
    file.write('}\n\n')

def WriteCodeForMockedLibraries(mockedFunctions, file):
    # Write global variables for mocked functions.
    for mockableFunction in mockableFunctions:
        if mockableFunction['returnType'] != 'void':
            file.write(mockableFunction['returnType'] + ' ' + mockableFunction['name'] + '_' + 'return;\n')
        file.write('unsigned int' + ' ' + mockableFunction['name'] + '_' + 'invocations = 0;\n')

        for parameter in mockableFunction['parameters']:
            file.write(parameter['type'] + ' ' + mockableFunction['name'] + '_param_' + parameter['name'] + ';\n')

        # Write declaration of mocked function.
        file.write(mockableFunction['returnType'] + ' ' + mockableFunction['name'] + '(')
        for i in range(len(mockableFunction['parameters'])):
            currentParam = mockableFunction['parameters'][i]
            file.write(currentParam['type'] + ' ' + currentParam['name'])
            if i != len(mockableFunction['parameters']) - 1:
                file.write(", ")

        file.write(')\n{\n')

        # Fill function body with mocked functionality.
        for parameter in mockableFunction['parameters']:
            file.write('\t' + mockableFunction['name'] + '_param_' + parameter['name'] + ' = ' + parameter['name'] + ';\n') # TODO: Break out into function.
        file.write('\t' + mockableFunction['name'] + '_' + 'invocations++;\n')
        if mockableFunction['returnType'] != 'void':
            file.write('\treturn ' + mockableFunction['name'] + '_' + 'return;\n')
        
        file.write('}\n\n')

        
    # Write Reset global variables for mocked functions.
    file.write('\nvoid ResetMocks() \n{\n')

    for mockableFunction in mockableFunctions:
        for parameter in mockableFunction['parameters']:
            lastTypeParts = parameter['type'].strip().split()[-1]
            file.write('\t' + mockableFunction['name'] + '_param_' + parameter['name'] + ' = ' + lastTypeParts + '();\n') # TODO: Break out into function.
        file.write('\t' + mockableFunction['name'] + '_' + 'invocations = 0;\n')
        if mockableFunction['returnType'] != 'void':
            lastReturnTypeParts = mockableFunction['returnType'].strip().split()[-1]
            file.write('\t' + mockableFunction['name'] + '_' + 'return = ' + lastReturnTypeParts + '();\n')

    file.write('}\n\n')










if len(sys.argv) <= 1:
    raise NameError("ABORTED: Please provide a folder in which the testSuite.cpp will be generated.")
else:
    currentDir = sys.argv[1] + '/'
    testFunctions = FindAllTestFunctions(currentDir)

    fakesDir = sys.argv[1] + '/Fakes/'
    (mockableFunctions, neccessaryIncludes) = FindAllMockableFunctions(fakesDir)

    PrintMockedFunctions(mockableFunctions)

    # Write tests to RunTest function.
    with open(currentDir + "testSuite.cpp", "w") as file:
        file.write('// THIS FILE WAS AUTOGENERATED BY FILE generateTestSuite.py\n')
        file.write('#include "testSuite.h" \n#include "test.h"\n\n')

        WriteCodeForIncludingFakes(neccessaryIncludes, file)
        WriteCodeForRunningTests(testFunctions, file)
        WriteCodeForMockedLibraries(mockableFunctions, file)
        
    # SIMPLE TEST TO CHECK THE REFACTORING WORKED. DELETE THIS AFTER REFACTORING CODE.
    with open(currentDir + "testSuite_WORKING.txt", "r") as file1:
        with open(currentDir + "testSuite.cpp", "r") as file2:
            if(file1.read() != file2.read()):
                raise AssertionError("\n\nFAILED TEST. FILES NOT MATCHING!!!!!!!!!!!!! <-------------")