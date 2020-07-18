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
    regex = r"([^\(\)\;\n\:]+\s[*&]?)([^\(\)\;\n]+)\(([^\)\;]*)\)(?=\;)"
    matches = re.findall(regex, text)

    for match in matches:
        if match[0][:2] == '//':
            continue

        returnType = match[0].strip()
        nameOfFunction = match[1].strip()

        rawParameters = match[2].strip().split(",")
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
    # Define global variables for mocked function information.
    functionBlueprints = []
    for mockableFile in mockableFiles:
        # Mock functions
        functionBlueprints += BlueprintsForMockedFunctions(mockableFile['functions'])

        # Mock class functions
        for currentClass in mockableFile['classes']:
            functionBlueprints += BlueprintsForMockedFunctions(currentClass['functions'], currentClass['name'])
            
    # Generate code for mocked functions
    definitions = GenerateCodeFromBlueprints(functionBlueprints)
    file.write(definitions)

    # Write Reset global variables for mocked functions.
    file.write('\nvoid ResetMocks() \n{\n')
    
    resetCode = GenerateResetCodeFromBlueprints(functionBlueprints)
    file.write(resetCode)

    file.write('}\n\n')


def BlueprintsForMockedFunctions(functions, className = ''):
    allBlueprints = []

    # Count functions. Used to add overload suffix.
    functionInfo = {}
    for function in functions:
        functionInfo[function['name']] = { 'count': 0, 'completed': 0 }
    for function in functions:
        functionInfo[function['name']]['count'] += 1

    # Create and fill blueprint for each function.
    for function in functions:
        overloadCount = functionInfo[function['name']]['count']
        currentOverloadNumber = functionInfo[function['name']]['completed'] + 1
        blueprint = { 
            'name': function['name'],
            'class': className,
            'returnType': function['returnType'],
            'overloadSuffix': 'o' + str(currentOverloadNumber) if overloadCount > 1 else '',
            'returnVariable': {'name': '', 'type': '' },
            'invocationsVariable': {'name': '', 'type': '' },
            'parameterVariables': [],
        }

        if function['returnType'] != 'void':
            blueprint['returnVariable']['type'] = function['returnType']
            blueprint['returnVariable']['name'] = VariableNameReturn(function)

        blueprint['invocationsVariable']['type'] = 'unsigned int'
        blueprint['invocationsVariable']['name'] = VariableNameInvocations(function)

        for parameter in function['parameters']:
            parameterVariable = { 
                'name': VariableNameParameter(function, parameter),
                'type': parameter['type'],
                'parameter': parameter,
            }
            blueprint['parameterVariables'].append(parameterVariable)

        functionInfo[function['name']]['completed'] += 1
        allBlueprints.append(blueprint)
       
    return allBlueprints

def GenerateCodeFromBlueprints(functionBlueprints):
    code = ''

    for blueprint in functionBlueprints:
        prefix = '' if blueprint['class'] == '' else blueprint['class'] + '_'
        suffix = '' if blueprint['overloadSuffix'] == '' else '_' + blueprint['overloadSuffix']

        # Declare variables.
        if blueprint['returnType'] != 'void':
            code += CleanupType(blueprint['returnVariable']['type']) + ' '
            code += prefix + blueprint['returnVariable']['name'] + suffix
            code += ';\n'

        code += CleanupType(blueprint['invocationsVariable']['type']) + ' '
        code += prefix + blueprint['invocationsVariable']['name'] + suffix
        code += ' = 0;\n'

        for variable in blueprint['parameterVariables']:
            code += CleanupType(variable['type']) + ' '
            code += prefix + variable['name'] + suffix
            code += ';\n'


        # Write declaration of mocked function.
        code += blueprint['returnType'] + ' '
        if blueprint['class'] != '':
            code += blueprint['class'] + '::'
        code += blueprint['name'] + '('

        functionParameters = list(map(lambda x: x['parameter'], blueprint['parameterVariables']))
        for i in range(len(functionParameters)):
            currentParam = functionParameters[i]

            code += currentParam['type'] + ' ' + currentParam['name']
            if i != len(functionParameters) - 1:
                code += ", "

        code += ')\n'
        code += '{\n'

        # Fill blueprint body with mocked functionality.
        for variable in blueprint['parameterVariables']:
            code += '\t'
            code += prefix + variable['name'] + suffix
            code += ' = '
            code += '(' + IgnoreConst(variable['type']) + ')' 
            code += variable['parameter']['name']
            code += ';\n'

        code += '\t'
        code += prefix + blueprint['invocationsVariable']['name'] + suffix
        code += '++;\n'
        if blueprint['returnType'] != 'void':
            code += '\treturn '
            code += prefix + blueprint['returnVariable']['name'] + suffix
            code += ';\n'

        code += '}\n\n'

    return code

def CleanupType(typeName):
    result = typeName
    result = IgnoreAmpersand(result)
    result = IgnoreConst(result)
    return result

def IgnoreAmpersand(typeName):
    return typeName[:-1] if typeName[-1] == '&' else typeName 
    
def IgnoreConst(typeName):
    return ' '.join(typeName.split()[1:]) if typeName.split()[0] == 'const' else typeName 

def VariableNameReturn(function):
    return function['name'] + '_' + 'return'


def VariableNameInvocations(function):
    return function['name'] + '_' + 'invocations'


def VariableNameParameter(function, parameter):
    return function['name'] + '_param_' + parameter['name']


def GenerateResetCodeFromBlueprints(functionBlueprints):
    code = ''

    for blueprint in functionBlueprints:
        prefix = '' if blueprint['class'] == '' else blueprint['class'] + '_'
        suffix = '' if blueprint['overloadSuffix'] == '' else '_' + blueprint['overloadSuffix']
        
        for parameter in blueprint['parameterVariables']:
            lastPartOfType = GetLastPartOfType(parameter['type'])
            code += '\t'
            code += prefix + parameter['name'] + suffix

            lastCharOfType = parameter['type'].strip()[-1]
            if lastCharOfType != '*':
                code += ' = ' + lastPartOfType + '();\n'
            else:
                code += ' = nullptr;\n'

        code += '\t'
        code += prefix + blueprint['invocationsVariable']['name'] + suffix
        code += ' = 0;\n'

        if blueprint['returnType'] != 'void':
            lastPartOfReturnType = GetLastPartOfType(blueprint['returnVariable']['type'])
            code += '\t'
            code += prefix + blueprint['returnVariable']['name'] + suffix
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



    # # SIMPLE TEST TO CHECK THAT THE REFACTORING WORKED. DELETE THIS AFTER REFACTORING CODE.
    # with open(currentDir + "testSuite_WORKING.txt", "r") as file1: # TODO: Uncomment and change into test for mock framework.
    #     with open(currentDir + "testSuite.cpp", "r") as file2:
    #         if(file1.read() != file2.read()):
    #             raise AssertionError(
    #                 "\n\nFAILED TEST. FILES NOT MATCHING!!!!!!!!!!!!! <-------------")
