#include <iostream>
#include <vector>
#include <algorithm>

// Funkcja do krzyżowania cyklicznego
std::vector<int> cyclicCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2) {
    int size = parent1.size();
    std::vector<int> v;

    // Tworzenie wektora dziecka z rozmiarem i wartościami rodzica 1
    std::vector<int> child(size, -1);

    // Indeksy do oznaczania, które miasta zostały już dodane do dziecka
    std::vector<bool> visited(size, false);

    // Wybór cyklu od pierwszego rodzica
    int current = parent1[0];
    do {
        child[current] = parent1[current];
        visited[current] = true;
        current = parent2[current];
    } while (current != parent1[0]);

    // Uzupełnienie reszty dziecka z drugiego rodzica
    bool tmp;
    for (int i = 0; i < size; ++i) {
        tmp = false;
        if (!visited[i]) {
            // v.push_back(parent1[i]);
            for (int j = 0; j < size; j++) {
                if (child[j] == parent2[i]) {
                    tmp = true;
                    break;
                }
            }
            if (!tmp) {
                child[i] = parent2[i];
            } else {
                v.push_back(i);
            }
        }
    }

    for (int i = size - 1; i >= 0; i--) {
        tmp = false;
        for (int j = 0; j < size; j++) {
            if (child[j] == parent2[i]) {
                tmp = true;
                break;
            }
        }

        if (!tmp) {
            child[v.back()] = parent2[i];
            v.pop_back();
        }
    }

    return child;
}

std::vector<int> crossover(const std::vector<int>& parent1, const std::vector<int>& parent2) {
    int size = parent1.size();

    std::vector<int> child;
    std::vector<bool> visited(size, true);

    for (int i = 0; i < size/2; i++) {
        child.push_back(parent2[i]);
    }

    bool tmp;

    for (auto it: parent1) {
        tmp = false;
        for (int i = 0; i < size/2; i++) {
            if (it == child[i]) {
                tmp = true;
                break;
            }
        }
        if (!tmp) {
            child.push_back(it);
        }
    }

    return child;
}

// Implementacja krzyżowania PMX
std::vector<int> pmxCrossover(const std::vector<int>& parent1, const std::vector<int>& parent2, int start, int end) {
    std::vector<int> child(parent1.size(), -1);
    for (int i = start; i <= end; ++i) {
        child[i] = parent1[i];
    }

    for (int i = start; i <= end; ++i) {
        if (std::find(child.begin(), child.end(), parent2[i]) == child.end()) {
            int index = i;
            while (child[index] != -1) {
                index = find(parent2.begin(), parent2.end(), parent1[index]) - parent2.begin();
            }
            child[index] = parent2[i];
        }
    }

    for (int i = 0; i < child.size(); ++i) {
        if (child[i] == -1) {
            child[i] = parent2[i];
        }
    }

    return child;
}

int main() {
    // Przykładowe rodzice (permutacje miast)
    std::vector<int> parent1 = {7, 8, 4, 5, 6, 3, 1, 2};
    std::vector<int> parent2 = {4, 1, 7, 2, 3, 6, 5, 8};

    // Wywołanie krzyżowania cyklicznego
    std::vector<int> child = pmxCrossover(parent1, parent2, 3, 7);

    // Wyświetlenie wyniku
    std::cout << "Parent 1: ";
    for (int city : parent1) {
        std::cout << city << " ";
    }
    std::cout << std::endl;

    std::cout << "Parent 2: ";
    for (int city : parent2) {
        std::cout << city << " ";
    }
    std::cout << std::endl;

    std::cout << "Child   : ";
    for (int city : child) {
        std::cout << city << " ";
    }
    std::cout << std::endl;

    return 0;
}
