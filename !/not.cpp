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
const short ERROR_MESSAGE_CODE_LINE_LENGTH = 10;

const string dataTypes = "_$!";

void error(string message) {
    cout << "Error!" << endl;
    cout << message << endl;
    exit(1);
}

void codeError(string message, string code, int errorIndex) {
    code = code.substr(errorIndex, ERROR_MESSAGE_CODE_LINE_LENGTH);
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

template <typename T> bool findVar(map <string, T> varMap, string varName) {
    for (auto &i : varMap) {
        if (i.first == varName) {
            return true;
        }
    }
    return false;
}

double stringToDouble(string str) {
    try {
        return stod(str);
    } catch (invalid_argument) {
        error ("Can\'t covert to number: " + str);
    }
    return 0;
}

bool stringToBoolean(string str) {
    if (str == "!") {
        return false;
    } else if (str == "!!") {
        return true;
    } else {
        try {
            return stoi(str);
        } catch (invalid_argument) {
            error ("Can\'t covert to boolean: " + str);
        }   
    }
    return false;
}
vector <pair<char, string>> getParsedCommand(string command) {
    vector <pair<char, string>> parsedCommands;
    bool plusModeEnabled = true;
    for (int i = 0; i < command.length(); ++i) {
        if (dataTypes.find(command[i]) != string::npos) {
            if (i == command.length() - 1) {
                if (plusModeEnabled) {
                    if (command[i] == '!') {
                        parsedCommands.push_back(make_pair('!', "0"));
                        break;
                    }
                } else {
                    codeError("Type identifier without value", command, i);
                }
            } else if (command[i] == '!' && command[i+1] == '!' && i == command.length() - 2) {
                parsedCommands.push_back(make_pair('!', "1"));
                break;
            } else if (i < command.length() - 2 && command[i] == '!' && command[i+1] == '!' && command[i+2] == ' ') {
                parsedCommands.push_back(make_pair('!', "1"));
                i += 2;
                continue;
            } else if (command[i] == '!' && command[i+1] == ' ') {
                parsedCommands.push_back(make_pair('!', "0"));
                ++i;
                continue;
            }


            if (command[i+1] == ' ') {
                if (!i) {
                    ++i;
                    continue;
                }
                if (command[i] == '_') {
                    if ((i < command.length() - 3 && command[i+2] == ' ' && command[i+3] == '+') || (i == command.length() - 2)) {
                        if (plusModeEnabled) {
                            parsedCommands.push_back({'_', " "});
                            i += 2;
                            plusModeEnabled = false;
                        } else {
                            codeError("+ missing", command, i);
                        }
                    } else {
                        codeError("Type identifier without value", command, i);
                    }
                } else {
                    codeError("+ missing", command, i);
                }
            } else {
                if (!plusModeEnabled) {
                    codeError("+ missing", command, i);
                }
                int j = i + 1;
                for (; j < command.length() && command[j] != ' '; ++j);
                string varName = command.substr(i+1, j - i - 1);
                int firstSize = parsedCommands.size();
                switch (command[i]) {
                    case '_':
                        if(findVar(stringsMap, varName)) {
                            parsedCommands.push_back({command[i], stringsMap[varName]});
                        }
                        break;
                    case '$':
                        if(findVar(numbersMap, varName)) {
                            parsedCommands.push_back({command[i], to_string(numbersMap[varName])});
                        }
                        break;
                    case '!':
                        if(findVar(booleansMap, varName)) {
                            parsedCommands.push_back({command[i], to_string(booleansMap[varName])});
                        }
                        break;
                }
                if (firstSize == parsedCommands.size()) {
                    for (; (j < command.length()) && (!((j < command.length() - 1) && (command[j] == ' ') && (command[j + 1] == '+'))); ++j);
                    parsedCommands.push_back({command[i], command.substr(i + 1, j -i - 1)});
                }
                i = j;
                plusModeEnabled = false;
            }
        } else if (command[i] == '+') {
            plusModeEnabled = true;
            if (((i != command.length() - 1) && (command[i+1] != ' ')) || (i == command.length() - 1)) {
                codeError("Missing space after +", command, i);
            }
            ++i;
        }
    }
    return parsedCommands;
}

string getStringFromCommand(string command) {
    vector <pair<char, string>> parsedCommand = getParsedCommand(command);
    string res;
    for (auto &i : parsedCommand) {
        res += i.second;
    }
    return res;
}
double getNumberFromCommand(string command) {
    vector <pair<char, string>> parsedCommand = getParsedCommand(command);
    double res;
    for (auto &i : parsedCommand) {
        switch (i.first) {
            case '_':
                error ("Can't add string " + i.second + "to a number");
            case '$':
                res += stringToDouble(i.second);
                break;
            case '!':
                res += stringToBoolean(i.second);
                break;
        }
    }
    return res;
}
bool getBooleanFromCommand(string command) {
    vector <pair<char, string>> parsedCommand = getParsedCommand(command);
    bool res;
    for (auto &i : parsedCommand) {
        switch (i.first) {
            case '_':
                error ("Can't add string " + i.second + "to a number");
            case '$':
                res += (bool)stringToDouble(i.second);
                break;
            case '!':
                res += stringToBoolean(i.second);
                break;
        }
    }
    return res;
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
            if (varValue[0] != '_') {
                varValue = '_' + varValue;
            }
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
            if (varValue[0] != '$') {
                varValue = '$' + varValue;
            }
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
            if (varValue[0] != '!') {
                varValue = '!' + varValue;
            }
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
    file.close();
}