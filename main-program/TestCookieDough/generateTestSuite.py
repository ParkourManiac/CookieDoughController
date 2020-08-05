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
    regex = r"([^\(\)\;\n\:*&]+[^\S\n*&][*&]?)([^\(\)\;\n]+)\(([^\)\;]*)\)([^\S\n]+)?;([^\S\n]+)?(\/\/[^\S\n]*cookieOption:([^\n]+))?"
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

        specialOptions = match[6].split(",")
        specialOptions = list(map(lambda x: x.strip(), specialOptions))

        newFunction = {'returnType': returnType,
                       'name': nameOfFunction, 'parameters': parameters, 'options': specialOptions}
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
            'returnVariable': {'name': '', 'type': function['returnType'] },
            'invocationsVariable': {'name': VariableNameInvocations(function), 'type': 'unsigned int' },
            'parameterVariables': [],
            'options': function['options'],
        }

        if function['returnType'] != 'void':
            blueprint['returnVariable']['name'] = VariableNameReturn(function)

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
        code += DefineReturnVariable(blueprint['returnVariable'], prefix, suffix, blueprint['options'])
        code += DefineInvocationsVariable(blueprint['invocationsVariable'], prefix, suffix)

        for variable in blueprint['parameterVariables']:
            code += DefineParameterVariable(variable, prefix, suffix, blueprint['options'])

        for variable in blueprint['parameterVariables']:
            code += DefineReferenceVariable(variable, prefix, suffix, blueprint['options'])

        # Write declaration of mocked function.
        code += DeclareFunction(blueprint)
        code += DefineFunctionBody(blueprint, prefix, suffix)

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


def DefineReturnVariable(variable, prefix, suffix, options):
    code = ''
    if variable['type'] != 'void':
        returnVariableType = CleanupType(variable['type'])
        code += returnVariableType + ' '
        code += prefix + variable['name'] + suffix
        code += ';\n'

        if 'useVector' in options:
            returnVectorType = 'std::vector<' + returnVariableType + '>'
            code += returnVectorType + ' '
            code += prefix + variable['name'] + suffix + '_v'
            code += ';\n'

    return code

def DefineInvocationsVariable(variable, prefix, suffix):
    code = ''

    code += CleanupType(variable['type']) + ' '
    code += prefix + variable['name'] + suffix
    code += ' = 0;\n'

    return code

def DefineParameterVariable(variable, prefix, suffix, options):
    code = ''

    parameterVariableType = CleanupType(variable['type']) 
    parameterVariableName = prefix + variable['name'] + suffix
    code += parameterVariableType + ' '
    code += parameterVariableName
    code += ';\n'
    if 'useVector' in options:
        parameterVectorType = 'std::vector<' + parameterVariableType + '>'
        code += parameterVectorType + ' '
        code += parameterVariableName + '_v'
        code += ';\n'

    return code

def DefineReferenceVariable(variable, prefix, suffix, options):
    code = ''

    if 'overwriteReference' in options:
        if variable['type'][-1] == '&':
            referenceVariableType = CleanupType(variable['type']) 
            parameterVariableName = prefix + variable['name'] + suffix
            code += referenceVariableType + ' '
            code += parameterVariableName + '_r'
            code += ';\n'
            if 'useVector' in options:
                referenceVectorType = 'std::vector<' + referenceVariableType + '>'
                code += referenceVectorType + ' '
                code += parameterVariableName + '_v' + 'r'
                code += ';\n'

    return code

def DeclareFunction(blueprint):
    code = ''
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

    return code

def DefineFunctionBody(blueprint, prefix, suffix):
    code = '{\n'

    invocationsVariableName = prefix + blueprint['invocationsVariable']['name'] + suffix
    code += DefineInvocationsVariableBehaviour(invocationsVariableName)

    # Fill blueprint body with mocked functionality.
    for variable in blueprint['parameterVariables']:
        code += DefineParameterVariableBehaviour(variable, prefix, suffix, blueprint['options'])

    code += '\n'
    for variable in blueprint['parameterVariables']:
        code += DefineReferenceVariableBehaviour(variable, invocationsVariableName, prefix, suffix, blueprint['options'])

    code += DefineReturnVariableBehaviour(blueprint, invocationsVariableName, prefix, suffix)

    code += '}\n\n'

    return code

def DefineParameterVariableBehaviour(variable, prefix, suffix, options):
    code = ''

    parameterVariableName = prefix + variable['name'] + suffix
    code += '\t'
    code += parameterVariableName
    code += ' = '
    code += '(' + IgnoreConst(variable['type']) + ')' 
    code += variable['parameter']['name']
    code += ';\n'
    if 'useVector' in options:
        parameterVectorName = parameterVariableName + '_v'
        code += '\t'
        code += parameterVectorName
        code += '.push_back('
        code += '(' + IgnoreConst(variable['type']) + ')' 
        code += variable['parameter']['name']
        code += ');\n'

    return code

def DefineReferenceVariableBehaviour(variable, invocationsVariableName, prefix, suffix, options):
    code = ''

    if 'overwriteReference' in options:
        if variable['type'][-1] == '&':
            parameterVariable = prefix + variable['name'] + suffix
            referenceVariableName = parameterVariable + '_r'

            if 'useVector' in options: 
                referenceVectorName = parameterVariable + '_vr'
                # Check if vector has an element at the current position
                code += '\tif('
                code += referenceVectorName + '.size() < ' + invocationsVariableName
                code += ') '
                # code += '\n'
                
                # Handle overwriting reference with correct value
                # code += '\t\t'
                code += variable['parameter']['name']
                code += ' = '
                code += '(' + IgnoreConst(variable['type']) + ')'
                code += referenceVariableName
                code += ';\n'

                code += '\telse '
                # code += '\n'
                # code += '\t\t'
                code += variable['parameter']['name']
                code += ' = '
                code += '(' + IgnoreConst(variable['type']) + ')'
                code += referenceVectorName
                code += '.at(' + invocationsVariableName + '-1);\n\n'
            else:
                code += '\t'
                code += variable['parameter']['name']
                code += ' = '
                code += '(' + IgnoreConst(variable['type']) + ')'
                code += referenceVariableName
                code += ';\n'

    return code

def DefineInvocationsVariableBehaviour(variableName):
    code = '\t'
    code += variableName
    code += '++;\n'

    return code

def DefineReturnVariableBehaviour(blueprint, invocationsVariableName, prefix, suffix):
    code = ''
    if blueprint['returnType'] != 'void':
        returnVariableName = prefix + blueprint['returnVariable']['name'] + suffix
        if 'useVector' in blueprint['options']:
            returnVectorName = returnVariableName + '_v'
            # Prepare error message
            code += '\tif('
            code += returnVectorName + '.size() < ' + invocationsVariableName
            code += ')\n\t{\n'
            # code += '\t\tprintf("\033[01;31m""Please populate the vector \\\"'
            # code += returnVectorName
            # code += '\\\" with one value for each invocation of the function '
            # code += '\\\"'
            # code += prefix + blueprint['name']
            # code += '\\\". '
            # code += 'Do this inside your test before invoking the mocked function. '
            # code += 'Example: \\\"'
            # code += returnVectorName
            # code += '.push_back(myValueToBeReturned);\\\". '
            # code += '\\n\\nNOTE: The values in the vector will be iterated and returned (first element to last) for each invocation of the mocked function inside your test. '
            # code += 'The first element of the vector will be returned at the first invocation, '
            # code += 'for the next invocation it will move on to the next element in the list and continue to do so for each invocation of the mocked function. '
            # code += 'This error arises when the vector runs out of items to return.'
            # code += '\\n""\033[0m");\n'
            
            # Handle returning correct value
            code += '\t\treturn '
            code += returnVariableName
            code += ';\n'
            code += '\t}\n'

            code += '\telse\n\t{\n'
            code += '\t\treturn '
            code += returnVectorName
            code += '.at(' + invocationsVariableName + '-1);\n'
            code += '\t}\n'
        else: 
            returnVariableName = prefix + blueprint['returnVariable']['name'] + suffix
            code += '\treturn '
            code += returnVariableName
            code += ';\n'

    return code

def GenerateResetCodeFromBlueprints(functionBlueprints):
    code = ''

    for blueprint in functionBlueprints:
        prefix = '' if blueprint['class'] == '' else blueprint['class'] + '_'
        suffix = '' if blueprint['overloadSuffix'] == '' else '_' + blueprint['overloadSuffix']
        
        for variable in blueprint['parameterVariables']:
            code += DefineParameterVariableResetBehaviour(variable, prefix, suffix, blueprint['options'])

        for variable in blueprint['parameterVariables']:
            code += DefineReferenceVariableResetBehaviour(variable, prefix, suffix, blueprint['options'])

        code += DefineInvocationsVariableResetBehaviour(blueprint, prefix, suffix)
        code += DefineReturnVariableResetBehaviour(blueprint, prefix, suffix)

    return code


def GetLastPartOfType(fullType):
    lastPartOfType = fullType.strip().split()[-1]
    if lastPartOfType == '&' or lastPartOfType == '*':
        lastPartOfType = fullType.strip().split()[-2]

    return lastPartOfType

def DefineParameterVariableResetBehaviour(variable, prefix, suffix, options):
    code = ''
    
    parameterVariableName = prefix + variable['name'] + suffix
    lastPartOfType = GetLastPartOfType(variable['type'])
    code += '\t'
    code += parameterVariableName

    lastCharOfType = variable['type'].strip()[-1]
    if lastCharOfType != '*':
        code += ' = ' + lastPartOfType + '();\n'
    else:
        code += ' = nullptr;\n'

    if 'useVector' in options:
        parameterVectorName = parameterVariableName + '_v'
        code += '\t'
        code += parameterVectorName
        code += '.clear();\n'

    return code 

def DefineReferenceVariableResetBehaviour(variable, prefix, suffix, options):
    code = ''

    if 'overwriteReference' in options:
        if variable['type'][-1] == '&':
            parameterVariableName = prefix + variable['name'] + suffix
            referenceVariableName = parameterVariableName + '_r'
            lastPartOfType = GetLastPartOfType(variable['type'])

            code += '\t'
            code += referenceVariableName

            lastCharOfType = variable['type'].strip()[-1]
            if lastCharOfType != '*':
                code += ' = ' + lastPartOfType + '();\n'
            else:
                code += ' = nullptr;\n'

            if 'useVector' in options:
                referenceVectorName = parameterVariableName + '_vr'
                code += '\t'
                code += referenceVectorName
                code += '.clear();\n'

    return code 

def DefineInvocationsVariableResetBehaviour(blueprint, prefix, suffix):
    code = '\t'
    code += prefix + blueprint['invocationsVariable']['name'] + suffix
    code += ' = 0;\n'

    return code

def DefineReturnVariableResetBehaviour(blueprint, prefix, suffix):
    code = ''

    if blueprint['returnType'] != 'void':
        returnVariableName = prefix + blueprint['returnVariable']['name'] + suffix
        lastPartOfReturnType = GetLastPartOfType(blueprint['returnVariable']['type'])
        code += '\t'
        code += returnVariableName
        code += ' = ' + lastPartOfReturnType + '();\n'
        if 'useVector' in blueprint['options']:
            returnVectorName = returnVariableName + '_v'
            code += '\t'
            code += returnVectorName
            code += '.clear();\n'

    return code

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
