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


def FindAllMockableFiles(dir):
    mockableFiles = []
    for fileName in os.listdir(dir):
        if fileName.endswith(".h"):
            with open(dir + fileName, "r") as file:
                currentFile = {'name': fileName,
                               'functions': [], 'classes': []}

                text = file.read()
                (classes, remainingText) = ExtractClassesFromText(text, True)
                functions = ExtractFunctionsFromText(remainingText)

                currentFile['functions'] += functions
                currentFile['classes'] += classes

                mockableFiles.append(currentFile)

    return mockableFiles


def ExtractFunctionsFromText(text):
    functions = []
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

                newParameter = {'name': parameterName, 'type': parameterType}
                parameters.append(newParameter)

        newFunction = {'returnType': returnType,
                       'name': nameOfFunction, 'parameters': parameters}
        functions.append(newFunction)

    return functions


def ExtractClassesFromText(text, removeMatchedText=False):
    classes = []
    regex = r"(class|struct)\s([^\{\(\)\}\s]+)\s?{([^\}]+)};"
    matches = re.findall(regex, text)

    for match in matches:
        name = match[1]
        functions = ExtractFunctionsFromText(match[2])

        newClass = {'name': name, 'functions': functions}
        classes.append(newClass)

    newText = re.sub(regex, '', text) if removeMatchedText else text
    return (classes, newText)


def PrintMockedFiles(mockedFiles):
    showMockedFiles = '\nMocking: \n'
    for mockedFile in mockedFiles:
        showMockedFiles += '  ' + mockedFile['name'] + ' ('
        shouldEndWithComma = len(mockedFile['classes']) > 0
        showMockedFiles += FunctionsAsText(
            mockedFile['functions'], shouldEndWithComma)

        for mockedClass in mockedFile['classes']:
            showMockedFiles += mockedClass['name'] + ' {'
            showMockedFiles += FunctionsAsText(mockedClass['functions'])
            showMockedFiles += '}'
            if(mockedClass != mockedFile['classes'][-1]):
                showMockedFiles += ", "
        showMockedFiles += ')\n'
    print(showMockedFiles)


def FunctionsAsText(functions, endWithComma=False):
    text = ''
    for function in functions:
        text += function['name']
        if(function != functions[-1] or endWithComma):
            text += ", "

    return text


def WriteCodeForIncludingMockedLibraries(mockableFiles, file):
    # Write all includes needed for mocked functions.
    for mockableFile in mockableFiles:
        file.write('#include \"Fakes/' + mockableFile['name'] + '\"\n')
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


def WriteCodeForMockedLibraries(mockableFiles, file):
    # Write global variables for mocked functions.
    for mockableFile in mockableFiles: # <------------- CONTINUE HERE. USE mockableFile['classes'] TO MOCK THE CLASSES.
        for mockableFunction in mockableFile['functions']:
            if mockableFunction['returnType'] != 'void':
                file.write(mockableFunction['returnType'] + ' ' +
                           mockableFunction['name'] + '_' + 'return;\n')
            file.write('unsigned int' + ' ' +
                       mockableFunction['name'] + '_' + 'invocations = 0;\n')

            for parameter in mockableFunction['parameters']:
                file.write(parameter['type'] + ' ' + mockableFunction['name'] +
                           '_param_' + parameter['name'] + ';\n')

            # Write declaration of mocked function.
            file.write(mockableFunction['returnType'] +
                       ' ' + mockableFunction['name'] + '(')
            for i in range(len(mockableFunction['parameters'])):
                currentParam = mockableFunction['parameters'][i]
                file.write(currentParam['type'] + ' ' + currentParam['name'])
                if i != len(mockableFunction['parameters']) - 1:
                    file.write(", ")

            file.write(')\n{\n')

            # Fill function body with mocked functionality.
            for parameter in mockableFunction['parameters']:
                # TODO: Break out into function.
                file.write('\t' + mockableFunction['name'] + '_param_' +
                           parameter['name'] + ' = ' + parameter['name'] + ';\n')
            file.write(
                '\t' + mockableFunction['name'] + '_' + 'invocations++;\n')
            if mockableFunction['returnType'] != 'void':
                file.write('\treturn ' +
                           mockableFunction['name'] + '_' + 'return;\n')

            file.write('}\n\n')

    # Write Reset global variables for mocked functions.
    file.write('\nvoid ResetMocks() \n{\n')
    for mockableFile in mockableFiles:
        for mockableFunction in mockableFile['functions']:
            for parameter in mockableFunction['parameters']:
                lastTypeParts = parameter['type'].strip().split()[-1]
                # TODO: Break out into function.
                file.write('\t' + mockableFunction['name'] + '_param_' +
                           parameter['name'] + ' = ' + lastTypeParts + '();\n')
            file.write(
                '\t' + mockableFunction['name'] + '_' + 'invocations = 0;\n')
            if mockableFunction['returnType'] != 'void':
                lastReturnTypeParts = mockableFunction['returnType'].strip(
                ).split()[-1]
                file.write(
                    '\t' + mockableFunction['name'] + '_' + 'return = ' + lastReturnTypeParts + '();\n')

    file.write('}\n\n')


if len(sys.argv) <= 1:
    raise NameError(
        "ABORTED: Please provide a folder in which the testSuite.cpp will be generated.")
else:
    currentDir = sys.argv[1] + '/'
    testFunctions = FindAllTestFunctions(currentDir)

    fakesDir = sys.argv[1] + '/Fakes/'
    mockableFiles = FindAllMockableFiles(fakesDir)

    PrintMockedFiles(mockableFiles)

    # Write tests to RunTest function.
    with open(currentDir + "testSuite.cpp", "w") as file:
        file.write(
            '// THIS FILE WAS AUTOGENERATED BY FILE generateTestSuite.py\n')
        file.write('#include "testSuite.h" \n#include "test.h"\n\n')

        WriteCodeForIncludingMockedLibraries(mockableFiles, file)
        WriteCodeForRunningTests(testFunctions, file)
        WriteCodeForMockedLibraries(mockableFiles, file)

    # SIMPLE TEST TO CHECK THAT THE REFACTORING WORKED. DELETE THIS AFTER REFACTORING CODE.
    with open(currentDir + "testSuite_WORKING.txt", "r") as file1:
        with open(currentDir + "testSuite.cpp", "r") as file2:
            if(file1.read() != file2.read()):
                raise AssertionError(
                    "\n\nFAILED TEST. FILES NOT MATCHING!!!!!!!!!!!!! <-------------")
