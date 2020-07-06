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
    regex = r"([^\(\)\;\n\:]+)\s([^\(\)\;\n]+)\(([^\)\;]*)\)(?=\;)"
    matches = re.findall(regex, text)

    for match in matches:
        if match[0][:2] == '//':
            continue

        returnType = match[0].lstrip()
        nameOfFunction = match[1]

        rawParameters = match[2].split(",")
        parameters = []
        for rawParameter in rawParameters:
            if len(rawParameter) > 0:
                parameterParts = rawParameter.strip().split()
                parameterName = parameterParts.pop(len(parameterParts) - 1)
                parameterType = parameterParts

                if parameterName[0] == '&' or parameterName[0] == '*':
                    # The name contains the reference/pointer operator, move it to the type instead.
                    parameterType.append(parameterName[0])
                    parameterName = parameterName[1:]

                parameterType = " ".join(parameterType)

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
        typeName = match[0]
        name = match[1]
        functions = ExtractFunctionsFromText(match[2])

        newClass = {'name': name, 'type': typeName, 'functions': functions}
        classes.append(newClass)

    newText = re.sub(regex, '', text) if removeMatchedText else text
    # classes = [] # DEBUG: REMOVE THIS LINE
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

    functionNames = list(map(lambda x : x['name'], functions))
    uniqueFunctionNames = Unique(functionNames)

    for function in uniqueFunctionNames:
        text += function
        if(function != uniqueFunctionNames[-1] or endWithComma):
            text += ", "

    return text

def Unique(listOfItems):
    uniqueList = []

    for item in listOfItems:
        if item not in uniqueList:
            uniqueList.append(item)

    return uniqueList
        

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
    for mockableFile in mockableFiles:
        definitions = GenerateCodeForFunctions(mockableFile['functions'])
        definitions += GenerateCodeForClasses(mockableFile['classes'])
        file.write(definitions)

    # Write Reset global variables for mocked functions.
    file.write('\nvoid ResetMocks() \n{\n')
    for mockableFile in mockableFiles:
        resetCode = GenerateCodeForResettingMocks(
            mockableFile['functions'], mockableFile['classes'])
        file.write(resetCode)

    file.write('}\n\n')


def GenerateCodeForFunctions(functions, className = ''): # TODO: CONTINUE HERE. Reuse variables that have already been defined to prevent redefinitions!
    code = ''
    prefix = ''
    isInsideClass = className != ''
    definedVariables = []

    if isInsideClass:
        prefix = className + '_'

    for function in functions:
        if function['returnType'] != 'void':
            returnVar = prefix + ReturnVariableName(function)
            code += function['returnType'] + ' '
            code += returnVar + ';\n'

        invocationsVar = prefix + InvocationsVariableName(function)
        code += 'unsigned int '
        code += invocationsVar + ' = 0;\n'

        for parameter in function['parameters']:
            parameterVar = prefix + ParameterVariableName(function, parameter)
            code += parameter['type'] + ' '
            code += parameterVar + ';\n'

        # Write declaration of mocked function.
        code += function['returnType'] + ' '
        if isInsideClass:
            code += className + '::'
        code += function['name'] + '('
        for i in range(len(function['parameters'])):
            currentParam = function['parameters'][i]

            code += currentParam['type'] + ' ' + currentParam['name']
            if i != len(function['parameters']) - 1:
                code += ", "

        code += ')\n'
        code += '{\n'

        # Fill function body with mocked functionality.
        for parameter in function['parameters']:
            code += '\t'
            code += prefix + ParameterVariableName(function, parameter)
            code += ' = ' + parameter['name'] + ';\n'

        code += '\t' + prefix + InvocationsVariableName(function) + '++;\n'
        if function['returnType'] != 'void':
            code += '\treturn ' + prefix + ReturnVariableName(function) +';\n'

        code += '}\n\n'

    return code


def ReturnVariableName(function):
    return function['name'] + '_' + 'return'


def InvocationsVariableName(function):
    return function['name'] + '_' + 'invocations'


def ParameterVariableName(function, parameter):
    return function['name'] + '_param_' + parameter['name']


def GenerateCodeForClasses(classes):
    code = ''
    for currentClass in classes:
        code += GenerateCodeForFunctions(
            currentClass['functions'], currentClass['name'])

    return code


def GenerateCodeForResettingMocks(functions, classes):
    code = ''
    code += GenerateCodeForResettingFunctions(functions)

    for currentClass in classes:
        code += GenerateCodeForResettingFunctions(
            currentClass['functions'], currentClass['name'])

    return code


def GenerateCodeForResettingFunctions(functions, className=''): # TODO: Refactor to use VariableName functions.
    code = ''
    prefix = ''
    isInsideClass = className != ''

    if isInsideClass:
        prefix = className + '_'

    for function in functions:
        for parameter in function['parameters']:
            lastPartOfType = GetLastPartOfType(parameter['type'])
            code += '\t' + prefix + \
                function['name'] + '_param_' + parameter['name']

            lastCharOfType = parameter['type'].strip()[-1] 
            if lastCharOfType != '*':
                code += ' = ' + lastPartOfType + '();\n'
            else:
                code += ' = nullptr;\n'

        code += '\t' + prefix + function['name'] + '_' + 'invocations = 0;\n'

        if function['returnType'] != 'void':
            lastPartOfReturnType = function['returnType'].strip().split()[-1]
            code += '\t' + prefix + function['name'] + '_' + 'return'
            code += ' = ' + lastPartOfReturnType + '();\n'

    return code


def GetLastPartOfType(fullType):
    lastPartOfType = fullType.strip().split()[-1]
    if lastPartOfType == '&' or lastPartOfType == '*':
        lastPartOfType = fullType.strip().split()[-2]

    return lastPartOfType


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
    with open(currentDir + "testSuite_WORKING.txt", "r") as file1: # TODO: Uncomment and change into test for mock framework.
        with open(currentDir + "testSuite.cpp", "r") as file2:
            if(file1.read() != file2.read()):
                raise AssertionError(
                    "\n\nFAILED TEST. FILES NOT MATCHING!!!!!!!!!!!!! <-------------")
