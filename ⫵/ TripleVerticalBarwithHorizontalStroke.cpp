#include <fstream>
#include <iostream>
using namespace std;
int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "\nUsage: TripleVerticalBarwithHorizontalStroke filename\n";
        return 0;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "Could not open " << argv[1] << endl;
        return 0;
    }

    string line;
    char command = '⫵';
    unsigned char count = 0;
    
    while (getline(file, line)) {
        for (auto &i : line) {
            count += (i == command);
        }
    }

    unsigned char acc = 0;

    while (true) {
        acc += static_cast<int>(count);
        if ((!(acc & 1)) && (acc < 10)) {
            cout << acc;
        } else if ((acc & 1) && (acc < 10)) {
            acc = 0;
        } else if (acc == 255) {
            cout << acc;
            break;
        } else if (acc & 1) {
            cout << acc;
            acc = 0;
        }
    }


    return 0;
}