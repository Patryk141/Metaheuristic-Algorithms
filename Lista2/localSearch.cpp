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

// Funkcja do znajdywania MSP
std::vector<int> prim(std::vector<std::vector<int>>& graph, int& MST_value) {
    int n = graph.size();
    std::vector<int> key(n, INT_MAX);
    std::vector<int> previous(n, -1);
    std::vector<bool> isVisited(n, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> queue;

    int startNode = 0;

    queue.push(std::make_pair(0, startNode));
    key[startNode] = 0;

    while (!queue.empty()) {
        int u = queue.top().second;
        queue.pop();

        isVisited[u] = true;

        for (int v = 0; v < n; ++v) {
            if (graph[u][v] && !isVisited[v] && graph[u][v] < key[v]) {
                key[v] = graph[u][v];
                previous[v] = u;
                queue.push(std::make_pair(key[v], v));
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        MST_value += key[i];
    }

    return previous;
}


std::vector<short> tsp(std::vector<int> previous, int startNode) {
    int n = previous.size();
    std::vector<std::list<int>> list(n);
    std::vector<short> sequence(n);
    int iter = 0;

    for (int i = 1; i < n; i++) {
        list[previous[i]].push_back(i);
        list[i].push_back(previous[i]);
    }

    std::vector<bool> isVisited(n, false);
    isVisited[startNode] = true;
    std::stack<int> stack;
    stack.push(startNode);
    int current = startNode;
    
    while (!stack.empty()) {
        current = stack.top();
        stack.pop();
        sequence[iter] = current;
        iter++;
        for (std::list<int>::iterator i = list[current].begin(); i != list[current].end(); i++) {
            if (!isVisited[*i]) {
                isVisited[*i] = true;
                stack.push(*i);
            }
        }
    }
    return sequence;
}

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
        a++;
        b--;
    }
    return currentSolution;
}

// Funkcja generująca wszystkie inwersje w danej permutacji
std::vector<std::vector<short>> generateAllInversions(const std::vector<short>& permutation) {
    std::vector<std::vector<short>> allInversions;
    int n = permutation.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            allInversions.push_back(invert(permutation, i, j));
        }
    }

    return allInversions;
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


std::vector<short> localSearch(std::vector<short> initialSolution, std::vector<std::vector<int>>& graph) {
    std::vector<short> currentSolution = initialSolution;
    int n = initialSolution.size();

    bool isBetter = true;
    int min = evaluateSolution(currentSolution, graph);
    int currCost = min;

    std::vector<short>tmp;
    std::vector<short>better;

    // std::vector<std::pair<int,int>>inver;

    while (isBetter) {
        isBetter = false;

        // inver = randomInversions(n);

        // for (auto i: inver) {
        //     tmp = invert(currentSolution, i.first, i.second);

        //     if (evaluateSolution1(i.first, i.second, tmp, graph, currCost) < min) {
        //         isBetter = true;
        //         better = tmp;
        //         min = evaluateSolution(tmp, graph);
        //     }
        // }

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                tmp = invert(currentSolution, i, j);

                if (evaluateSolution1(i, j, tmp, graph, currCost) < min) {
                    isBetter = true;
                    better = tmp;
                    min = evaluateSolution(tmp, graph);
                }
            }
        }

        if (isBetter) {
            counter++;
            currentSolution = better;
            currCost = min;
        }
    }
    

    return currentSolution;
}

std::vector<short> optimize(std::vector<int> previous, std::vector<std::vector<int>>& graph) {
    int n = previous.size();
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> distribution(0, n-1);
    int randomNumber = distribution(g);
    std::vector<short> sequence = tsp(previous, randomNumber);
    sequence = localSearch(sequence, graph);
    return sequence;
}

std::vector<short> random_path(int n) {
    std::vector<short> permutation(n);

    for (int i = 0; i < n; i++) {
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
    for (int i = 0; i < n; i++) {
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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                graph[i][j] = std::round(sqrt(std::pow(V[i].first - V[j].first, 2) + std::pow(V[i].second - V[j].second, 2)));
            }  else {
                graph[i][j] = 0;
            }
        }
    }

    int MST_value = 0;
    
    // std::vector<int> previous = prim(graph, MST_value);
    // std::vector<short> previous = random_path(graph.size());
    // std::cout << evaluateSolution(previous, graph) << std::endl;
    // std::cout << MST_value << std::endl;

    std::vector<short> sequence = optimize(prim(graph, MST_value), graph);
    std::cout << MST_value << std::endl;
    // std::vector<short> sequence = localSearch(previous, graph);
    std::cout << evaluateSolution(sequence, graph) << std::endl;
    std::cout << counter << std::endl;

    // for (int i = 0; i < n; ++i) {
    //     std::cout << sequence[i] + 1 << std::endl;
    // }

    return 0;
}
