#include <iostream>
#include <fstream>
#include <string.h>

int main(int argc, char const *argv[]) {
    std::ifstream file(argv[1]);
    std::string text;
    int n = 0;
    int MST = 0;
    int avg = 0;
    int current = 0;
    int step = 0;
    int min = 1000000;
    int start, end;

    while (std::getline(file, text)) {
        start = text.find(" ") + 1;
        MST += stoi(text.substr(0, start));
        end = text.find(" ", start);
        current = stoi(text.substr(start, end));
        start = end + 1;
        end = text.find("\n", start);
        step += stoi(text.substr(start, end));
        n++;
        if (min > current) {
            min = current;
        }
        avg += current;
    }

    std::cout << "MST\t AVG\t STEP\t MIN" << std::endl;
    std::cout << MST/n << "\t " << avg/n << "\t " << step/n << "\t " << min << std::endl;

    return 0;
}
