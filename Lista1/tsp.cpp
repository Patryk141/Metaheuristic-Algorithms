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

        for (int v = 0; v < n; v++) {
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

int tsp(std::vector<int> previous, std::vector<std::vector<int>>& graph) {
    int n = previous.size();
    std::vector<std::list<int>> list(n);

    for (int i = 1; i < n; i++) {
        list[previous[i]].push_back(i);
    }

    std::vector<bool> isVisited(n, false);
    isVisited[0] = true;
    std::stack<int> stack;
    stack.push(0);
    int current = 0;
    int prev = 0;
    int totalWeight = 0;

    while (!stack.empty()) {
        prev = current;
        current = stack.top();
        stack.pop();
        totalWeight += graph[prev][current];
        // std::cout << current + 1 << std::endl;
        for (std::list<int>::iterator i = list[current].begin(); i != list[current].end(); i++) {
            if (!isVisited[*i]) {
                isVisited[*i] = true;
                stack.push(*i);
            }
        }
    }
    return totalWeight;
}

int random_path(std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    int totalWeigh = 0;
    std::vector<int> permutation(n);

    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(permutation.begin(), permutation.end(), g);

    for (int i = 0; i < n; i++) {
        totalWeigh += graph[i][permutation[i]];
    }

    return totalWeigh;
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

    int min_10 = INT_MAX;
    int min_50 = INT_MAX;
    int min_1000 = INT_MAX;

    int avg_10 = 0;
    int avg_50 = 0;

    for (int i = 1; i <= 1000; i++) {
        int value = random_path(graph);
        
        if (i%10 == 0) {
            avg_10 += min_10;
            min_10 = INT_MAX;
        }
        if (i%50 == 0) {
            avg_50 += min_50;
            min_50 = INT_MAX;
        }
        if (value < min_10) {
            min_10 = value;
        }
        if (value < min_50) {
            min_50 = value;
        }
        if (value < min_1000) {
            min_1000 = value;
        }
    } 
    
    std::cout << "TSP: " << tsp(prim(graph, MST_value), graph) << std::endl;
    std::cout << "MSP: " << MST_value << std::endl;
    // tsp(prim(graph), graph);
    std::cout << "Random: " << avg_10/100 << " " << avg_50/20 << " " << min_1000 << std::endl;

    return 0;
}
