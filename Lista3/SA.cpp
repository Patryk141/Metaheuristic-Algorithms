#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <fstream>
#include <cmath>
#include <stack>
#include <list>
#include <random>
#include <algorithm>

int counter = 0;

// Funkcja oceny
int evaluateSolution(std::vector<short> sequence, std::vector<std::vector<int>>& graph) {
    int sum = 0;
    int n = sequence.size();
    for (int i = 1; i <= n; ++i) {
        sum += graph[sequence[i-1]][sequence[i%n]];
    }
    return sum;
}

// Funkcja oceny
int evaluateSolution1(int a, int b, std::vector<short> sequence, std::vector<std::vector<int>>& graph, int currCost) {
    int n = sequence.size();
    int sum = currCost;
    if (a != 0 || b != n-1) {
        sum += graph[sequence[(a-1+n)%n]][sequence[a%n]];
        sum -= graph[sequence[(a-1+n)%n]][sequence[b%n]];
        sum += graph[sequence[b%n]][sequence[(b+1)%n]];
        sum -= graph[sequence[a%n]][sequence[(b+1)%n]];
    }
    return sum;
}

std::vector<short> invert(std::vector<short> currentSolution, int a, int b) {
    while(a < b) {
        std::swap(currentSolution[a], currentSolution[b]);
        ++a;
        --b;
    }
    return currentSolution;
}
std::vector<std::pair<int, int>> randomInversions(int n) {
    std::vector<std::pair<int, int>>inversion;
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, n-1);
    int num1, num2;

    for (int i = 0; i < n; ++i) {
        num1 = dis(g);
        num2 = dis(g);
        while (num1 == num2) {
            num2  = dis(g);
        }
        if (num1 < num2) {
            inversion.push_back(std::make_pair(num1, num2));
        } else {
            inversion.push_back(std::make_pair(num2, num1));
        }
    }

    return inversion;
}

std::pair<int, int> randomInversion(int n) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, n-1);
    int num1, num2;

    num1 = dis(g);
    num2 = dis(g);
    while (num1 == num2) {
        num2  = dis(g);
    }
    if (num1 < num2) {
        return std::make_pair(num1, num2);
    } else {
        return std::make_pair(num2, num1);
    }
}

std::vector<short> simulatedAnnealing(std::vector<short> initialSolution, std::vector<std::vector<int>>& graph) {
    int n = initialSolution.size();
    
    std::vector<short> currentSolution = initialSolution;
    int min = evaluateSolution(currentSolution, graph);
    int currCost = min;

    std::vector<short> bestSolution = initialSolution;
    int bestCost = min;

    std::vector<short> tmp;
    double T = 1000.0;

    std::vector<short> better;
    std::vector<std::pair<int,int>> inver;

    int noImprovmentCounter = 0;

    while (noImprovmentCounter < 20000) {
        if (counter%(n/2) == 0)
            T *= 0.8;

        // Wybieram najlepsze rozwiazanie z losowego sasiedztwa
        inver = randomInversions(n);
        min = INT_MAX;
        for (auto i: inver) {
            tmp = invert(currentSolution, i.first, i.second);

            if (evaluateSolution1(i.first, i.second, tmp, graph, currCost) < min) {
                better = tmp;
                min = evaluateSolution(tmp, graph);
            }
        }

        if (min < currCost) {
            // Jezeli jest lepsze od aktualnego rozwiazania to akceptuje
            currentSolution = better;
            currCost = min;
            if (min < bestCost) {
                noImprovmentCounter = 0;
                bestSolution = better;
                bestCost = min;
            }
        } else {
            // Jezeli jest gorsze
            ++noImprovmentCounter;
            std::random_device rd;
            std::mt19937 g(rd());
            std::uniform_real_distribution<> dis;
            double randNumb = dis(g);
            if (randNumb < exp(currCost - min)/T) {
                currentSolution = better;
                currCost = min;
            }
        }
        // std::cout << counter << " " << T << std::endl;
        ++counter;
    }
    
    return bestSolution;
}

std::vector<short> random_path(int n) {
    std::vector<short> permutation(n);

    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(permutation.begin(), permutation.end(), g);

    return permutation;
}

int main(int argc, char const *argv[]) {

    std::ifstream file(argv[1]);
    std::string text;

    int n = 0;
    int start, end;
    while (1) {
        getline(file, text, '\n');
        if (text.find("DIMENSION") != std::string::npos) {
            start = text.find(" ") + 1;
            end = text.find(" ", start);
            start = end + 1;
            n = stoi(text.substr(start, end - start));
            while (1) {
                getline(file, text, '\n');
                if (text.find("NODE_COORD_SECTION")) {
                    getline(file, text, '\n');
                    break;
                }
            }
            break;
        }    
    }

    std::vector<std::vector<int>> graph(n, std::vector<int>(n));
    std::vector<std::pair<int,int>> V;

// Wczytywanie danych z pliku
    int a, b;
    for (int i = 0; i < n; ++i) {
        getline(file, text, '\n');
        start = text.find(" ") + 1;
        end = text.find(" ", start);
        a = stoi(text.substr(start, end - start));
        start = end + 1;
        end = text.find(" ", start);
        b = stoi(text.substr(start, end - start));
        V.push_back(std::make_pair(a, b));
    }

// Obliczanie odlegosci miedzy punktami
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                graph[i][j] = std::round(sqrt(std::pow(V[i].first - V[j].first, 2) + std::pow(V[i].second - V[j].second, 2)));
            }  else {
                graph[i][j] = 0;
            }
        }
    }

    std::vector<short> previous = random_path(graph.size());
    std::vector<short> sequence = simulatedAnnealing(previous, graph);

    std::cout << evaluateSolution(previous, graph) << " " << evaluateSolution(sequence, graph) << " " << counter << std::endl;

    return 0;
}
