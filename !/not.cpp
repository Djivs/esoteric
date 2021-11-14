#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
using namespace std;

//name : value
map <string, string> stringsMap;
map <string, double> numbersMap;
map <string, bool> booleansMap;
int lineNumber;

const string dataTypes = "_$!";

void error(string message) {
    cout << "Error!" << endl;
    cout << message << endl;
    exit(1);
}

void codeError(string message, string code, int errorIndex) {
    if (code.length() > errorIndex + 11) {
        code.substr(0, errorIndex + 10) + "...";
    }
    string errorMessage = message + '\n' + to_string(lineNumber) + " | " + code + '\n';
    for (int i = 0; i < to_string(lineNumber).length() + 3 + errorIndex; ++i) {
        errorMessage += ' ';
    }
    errorMessage += '^';
    error(errorMessage);
}

void printString(string str) {
    for (int i = 0; i < str.length(); ++i) {
        if (i < str.length() - 1) {
            if (str.substr(i, 2) == "/n") {
                cout << endl;
                ++i;
            } else {
                cout << str[i];
            }
        } else {
            cout << str[i];
        }
    }
}



string getStringFromCommand(string command) {
    if (dataTypes.find(command[0]) == string::npos) {
        return command;
    }
    string res;
    bool plusModeEnabled = true;
    for (int i = 0; i < command.length(); ++i) {
        if (command[i] == '_') {
            if (i == command.length() - 1)
                continue;
            if (command[i+1] == ' ') {
                if ((i < command.length() - 3 && command[i+2] == ' ' && command[i+3] == '+') || (i == command.length() - 2)) {
                    if (plusModeEnabled) {
                        res += ' ';
                        i += 2;
                        plusModeEnabled = false;
                    } else {
                        codeError("+ missing", command, i);
                    }
                }
            } else {
                if (!plusModeEnabled) {
                    codeError("+ missing", command, i);
                }
                int j = i + 1;
                for (; j < command.length() && command[j] != ' '; ++j);
                string varName = command.substr(i+1, j - i - 1);
                bool isVarExists = false;
                for (auto &i : stringsMap) {
                    if (i.first == varName) {
                        res += i.second;
                        isVarExists = true;
                        break;
                    }
                }
                if (isVarExists) {
                    i = j;
                    plusModeEnabled = false;
                } else {
                    res += varName;
                }
            }
        } else if (command[i] == '+') {
            plusModeEnabled = true;
            if (((i != command.length() - 1) && (command[i+1] != ' ')) || (i == command.length() - 1)) {
                codeError("Missing space after +", command, i);
            }
            i++;
        } else if (command[i] == '$' && i != command.length() - 1 && command[i+1] != ' ') {
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            bool isVarExists = false;
            for (auto &i : numbersMap) {
                if (i.first == varName) {
                    res += to_string(i.second);
                    isVarExists = true;
                    break;
                }
            }
            if (!isVarExists) {
                try {
                    double test = stod(varName);
                } catch (invalid_argument) {
                    codeError("No such number variable: " + varName, command, i + 1);
                }
                res += varName;
            }
            i = j;
            plusModeEnabled = false;
            
        } else if (command[i] == '!' && i != command.length() - 1 && command[i+1] != ' ') {
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            if (varName == "!") {
                res += '0';
            } else if (varName == "!!") {
                res += '1';
            } else {
                bool isVarExists = false;
                for (auto &i : booleansMap) {
                    if (i.first == command) {
                        res += to_string((short)i.second);
                        isVarExists = true;
                        break;
                    }
                }
                if (isVarExists) {
                    i = j;
                    plusModeEnabled = false;
                } else {
                    codeError("No such boolean variable: " + varName, command, i + 1);
                }
            }
        }
    }
    return res;
}
double getNumberFromCommand(string command) {
    if (dataTypes.find(command[0]) == string::npos) {
        try {
            return stod(command);
        } catch (invalid_argument) {
            codeError("Invalid number", command, 0);
        }
    }
    double outputValue = 0;
    bool plusModeEnabled = true;
    for (int i = 0; i < command.length(); ++i) {
        if (command[i] == '$') {
            if ((i == command.length() - 1)) {
                codeError("Can't add number without a value", command, i);
            } else if (command[i+1] == ' ') {
                if (!i) {
                    ++i;
                    continue;
                } else {
                    codeError("Can't add number without a value", command, i);
                }
            }
            if (!plusModeEnabled) {
                codeError("+ missing", command, i);
            }
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            bool isVarExists = false;
            for (auto &i : numbersMap) {
                if (i.first == varName) {
                    outputValue += i.second;
                    isVarExists = true;
                    break;
                }
            }
            if (isVarExists) {
                i = j;
                plusModeEnabled = false;
            } else {
                try {
                outputValue += stod(varName);
                } catch (invalid_argument) {
                    codeError("No variable with name " + varName, command, i);
                }
            }
        } else if (command[i] == '+') {
            plusModeEnabled = true;
            if (((i != command.length() - 1) && (command[i+1] != ' ')) || (i == command.length() - 1)) {
                codeError("Missing space after +", command, i);
            }
            i++;
        } else if (command[i] == '_' && i != command.length() - 1 && command[i+1] != ' ') {
            codeError("Can't add number and a string", command, i);
        } else if (command[i] == '!' && i != command.length() - 1 && command[i+1] != ' ') {
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            if (varName == "!!") {
                outputValue++;
            } else  if (varName != "!") {
                bool isVarExists = false;
                for (auto &i : booleansMap) {
                    if (i.first == command) {
                        outputValue += i.second;
                        isVarExists = true;
                        break;
                    }
                }
                if (isVarExists) {
                    i = j;
                    plusModeEnabled = false;
                } else {
                    codeError("No such boolean variable: " + varName, command, i + 1);
                }
            }
        }
    }
    return outputValue;
}
bool getBooleanFromCommand(string command) {
    if (dataTypes.find(command[0]) == string::npos) {
        try {
            return (bool)stoi(command);
        } catch (invalid_argument) {
            codeError("Invalid boolean", command, 0);
        }
    }
    bool outputValue = 0;
    bool plusModeEnabled = true;
    for (int i = 0; i < command.length(); ++i) {
        if (command[i] == '!') {
            if ((i == command.length() - 1)) {
                codeError("Can't add boolean without a value", command, i);
            } else if (command[i+1] == ' ') {
                if (!i) {
                    ++i;
                    continue;
                } else {
                    codeError("Can't add boolean without a value", command, i);
                }
            }
            if (!plusModeEnabled) {
                codeError("+ missing", command, i);
            }
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            if (varName == "!!") {
                outputValue += true;
                i = j;
                plusModeEnabled = false;
            } else if (varName == "!") {
                i = j;
                plusModeEnabled = false;  
            } else {
                bool isVarExists = false;
                for (auto &i : booleansMap) {
                    if (i.first == varName) {
                        outputValue += i.second;
                        isVarExists = true;
                        break;
                    }
                }
                if (isVarExists) {
                    i = j;
                    plusModeEnabled = false;
                } else {
                    try {
                    outputValue += stoi(varName);
                    } catch (invalid_argument) {
                        codeError("No variable with name " + varName, command, i);
                    }
                }
            }
        } else if (command[i] == '+') {
            plusModeEnabled = true;
            if (((i != command.length() - 1) && (command[i+1] != ' ')) || (i == command.length() - 1)) {
                codeError("Missing space after +", command, i);
            }
            i++;
        } else if (command[i] == '_' && i != command.length() - 1 && command[i+1] != ' ') {
            codeError("Can't add string and a boolean", command, i);
        } else if (command[i] == '$' && i != command.length() - 1 && command[i+1] != ' ') {
            int j = i + 1;
            for (; j < command.length() && command[j] != ' '; ++j);
            string varName = command.substr(i+1, j - i - 1);
            bool isVarExists = false;
            for (auto &i : numbersMap) {
                if (i.first == command) {
                    outputValue += (bool)i.second;
                    isVarExists = true;
                    break;
                }
            }
            if (isVarExists) {
                i = j;
                plusModeEnabled = false;
            } else {
                try {
                outputValue += stoi(varName);
                } catch (invalid_argument) {
                    codeError("No variable with name " + varName, command, i);
                }
            }
        }
    }
    return outputValue;
}


void execPrintCommand(string line) {
    string command = line.substr(1, line.length() - 1);
    switch(line[1]) {
        case '_':
            printString(getStringFromCommand(command));
            break;
        case '$':
            cout << getNumberFromCommand(command);
            break;
        case '!':
            cout << getBooleanFromCommand(command);
            break;
    }
}

void initVar(string command) {
    int j = 1;
    for (; j < command.length() && command[j] != ' '; ++j);

    if (j > command.length() - 4) {
        codeError("No variable initialization", command, j);
    }

    string varName = command.substr(1, j - 1);

    if (command[j + 1] != '?') {
        codeError("Need \'?\' in variable declaration", command, j + 1);
    }

    string varValue = command.substr(j + 3, command.length() - j - 3);

    string stringValue;
    double numberValue;
    bool booleanValue;


    switch (command[0]) {
        case '_':
            stringValue = getStringFromCommand(varValue);
            for (auto &i : stringsMap) {
                if (i.first == varName) {
                    stringsMap[varName] = stringValue;
                    return;
                }
            }
            stringsMap.insert({varName, stringValue});
            break;
        case '$':
            numberValue = getNumberFromCommand(varValue);
            for (auto &i : numbersMap) {
                if (i.first == varName) {
                    numbersMap[varName] = numberValue;
                    return;
                }
            }
            numbersMap.insert({varName, numberValue});
            break;
        case '!':
            booleanValue = getBooleanFromCommand(varValue);
            for (auto &i : booleansMap) {
                if (i.first == varName) {
                    booleansMap[varName] = booleanValue;
                    return;
                }
            }
            booleansMap.insert({varName, booleanValue});
            break;
    }
      
}
int main(int argc, char *argv[]) {
    lineNumber = 0;
    ifstream file;
    string code;

    if (argc != 2) {
        error("Invalid arguments.\n"
        "You chould use this program like this:\n"
        "$ not *your program file name*");
    }
    else {
        file.open(argv[1]);
        if (!file) {
            error("Can't open file");
            return 1;
        }
    }

    while (!file.eof()) {
        string line; getline(file, line);
        code += line+ (char)NULL;
    }

    int startingIndex = code.find("!-"), endingIndex = code.find("-!");
    if (startingIndex == string::npos) {
        error("You must initialize program with \'!-\'");
    } else if (endingIndex == string::npos) {
        error("You must end the program with \'-!\'" );
    } else if (abs(startingIndex - endingIndex)  < 2) {
        error("Invalid initialization of a program\n"
        "Can't run !-!");
    }

    for (int i = startingIndex + 2; i < endingIndex; ++i) {
        int lineEndIndex = i;
        for (; lineEndIndex < endingIndex && code[lineEndIndex] != (char)NULL; ++lineEndIndex);
        if (lineEndIndex - i < 3) {
            i = lineEndIndex;
            ++lineNumber;
            continue;
        }
        const string command =  code.substr(i, lineEndIndex - i);
        switch(command[0]) {
            case '#':
                execPrintCommand(command);
                break;
            default:
                if (dataTypes.find(command[0]) != string::npos) {
                    initVar(command);
                }
                break;
        
        }
        i = lineEndIndex;
        ++lineNumber;
    }

    // for (auto &i : stringsMap) {
    //     cout << i.first << " " << i.second << endl;
    // }


    file.close();
}