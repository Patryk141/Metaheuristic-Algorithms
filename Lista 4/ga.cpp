#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <random>
#include <climits>

struct individual {
    std::vector<short> genotype;
    int phenotype; // wartosc funkcji celu

    individual(std::vector<short> genotype, int phenotype) {
        this->genotype = genotype;
        this->phenotype = phenotype;
    }
};

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

// Krzyzowanie
individual crossover(std::vector<std::vector<int>>& graph, const individual& parent1, const individual& parent2) {
    int size = parent1.genotype.size();

    std::vector<short> child;
    // std::vector<bool> visited(size, true);

    for (int i = 0; i < size/2; i++) {
        child.push_back(parent2.genotype[i]);
    }

    bool tmp;

    for (auto it: parent1.genotype) {
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

    // int size = parent1.genotype.size();
    // std::vector<int> v;

    // // Tworzenie wektora dziecka z rozmiarem i wartościami rodzica 1
    // std::vector<short> child(size, -1);

    // // Indeksy do oznaczania, które miasta zostały już dodane do dziecka
    // std::vector<bool> visited(size, false);

    // // Wybór cyklu od pierwszego rodzica
    // int current = parent1.genotype[0];
    // do {
    //     child[current] = parent1.genotype[current];
    //     visited[current] = true;
    //     current = parent2.genotype[current];
    // } while (current != parent1.genotype[0]);

    // // Uzupełnienie reszty dziecka z drugiego rodzica
    // bool tmp;
    // for (int i = 0; i < size; ++i) {
    //     tmp = false;
    //     if (!visited[i]) {
    //         // v.push_back(parent1[i]);
    //         for (int j = 0; j < size; j++) {
    //             if (child[j] == parent2.genotype[i]) {
    //                 tmp = true;
    //                 break;
    //             }
    //         }
    //         if (!tmp) {
    //             child[i] = parent2.genotype[i];
    //         } else {
    //             v.push_back(i);
    //         }
    //     }
    // }

    // for (int i = size - 1; i >= 0; i--) {
    //     tmp = false;
    //     for (int j = 0; j < size; j++) {
    //         if (child[j] == parent2.genotype[i]) {
    //             tmp = true;
    //             break;
    //         }
    //     }

    //     if (!tmp) {
    //         child[v.back()] = parent2.genotype[i];
    //         v.pop_back();
    //     }
    // }

    return individual(child, evaluateSolution(child, graph));
}

int tournamentSelection(std::vector<individual> population, int size) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, population.size()-1);
    std::vector<int> tournament;
    for (int i = 0; i < size; i++) {
        int randomIndex = dis(g);
        tournament.push_back(randomIndex);
    }

    return *std::min_element(tournament.begin(), tournament.end(), 
    [population](const int& a, const int& b) {
        return population[a].phenotype < population[b].phenotype;
    });
}

void evaluatePopulation(std::vector<individual>& population, std::vector<std::vector<int>>& graph) {
    for (auto& it: population) {
        it.phenotype = evaluateSolution(it.genotype, graph);
    }
}

std::vector<individual> initialPopulation(int n, int size) {
    std::vector<individual> population;

    for (int i = 0; i < size; i++) {
        population.push_back(individual(random_path(n), -1));
    }

    return population;
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

bool isDuplicate(const std::vector<individual>& population, int index) {
    for (int i = 0; i < index; ++i) {
        if (population[i].phenotype == population[index].phenotype) {
            return true;
        }
    }
    return false;
}

void mutation(individual& child, double mutationRate, std::vector<std::vector<int>>& graph) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_real_distribution<> dis;
    int currCost = evaluateSolution(child.genotype, graph);
    int min = INT_MAX;
    std::vector<std::pair<int,int>> inver;
    int n = child.genotype.size();
    std::vector<short> tmp;
    std::vector<short> better;

    double randNumb = dis(g);
    if (randNumb < mutationRate) {
        inver = randomInversions(n);
        min = INT_MAX;
        for (auto i: inver) {
            tmp = invert(child.genotype, i.first, i.second);

            if (evaluateSolution1(i.first, i.second, tmp, graph, currCost) < min) {
                better = tmp;
                min = evaluateSolution(tmp, graph);
            }
        }
        child.genotype = better;
        child.phenotype = min;
    }
}

// // Function to perform crossover between two parent chromosomes
// individual crossover(std::vector<std::vector<int>>& graph, const individual& parent1, const individual& parent2) {
//     int n = parent1.genotype.size();
//     std::vector<short> child(n, -1);

//     int startPos = rand() % n;
//     int endPos = rand() % n;

//     if (startPos > endPos) {
//         std::swap(startPos, endPos);
//     }

//     for (int i = startPos; i <= endPos; ++i) {
//         child[i] = parent1.genotype[i];
//     }

//     int currentIndex = 0;
//     for (int i = 0; i < n; ++i) {
//         if (child[i] == -1) {
//             while (find(child.begin(), child.end(), parent2.genotype[currentIndex]) != child.end()) {
//                 currentIndex = (currentIndex + 1) % n;
//             }
//             child[i] = parent2.genotype[currentIndex];
//             currentIndex = (currentIndex + 1) % n;
//         }
//     }

//     return individual(child, evaluateSolution(child, graph));
// }


individual pmxCrossover(std::vector<std::vector<int>>& graph, std::vector<short>& parent1, std::vector<short>& parent2, int start, int end) {
    int n = parent1.size();
    
    std::vector<short> child(n, -1);
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

    for (int i = 0; i < n; ++i) {
        if (child[i] == -1) {
            child[i] = parent2[i];
        }
    }

    return individual(child, evaluateSolution(child, graph));
}

void ga(std::vector<short> initialSolution, std::vector<std::vector<int>>& graph) {
    int n = initialSolution.size();
    int populationSize = 50;
    int generations = 10000;
    int tournamentSize = 5;
    double mutationRate = 0.1;
    int parent1;
    int parent2;
    std::vector<individual> population = initialPopulation(n, populationSize);
    evaluatePopulation(population, graph);

    for (int generation = 0; generation < generations; generation++) {
        std::vector<individual> newPopulation;

        sort(population.begin(), population.end(), 
        [](const individual& a, const individual& b) {
            return a.phenotype < b.phenotype;
        });

        for (int i = 0; i < populationSize / 5; ++i) {
            newPopulation.push_back(population[i]);
        }

        for (int i = populationSize / 5; i < populationSize; i += 2) {
            parent1 = tournamentSelection(population, tournamentSize);
            do {
                parent2 = tournamentSelection(population, tournamentSize);
            } while (parent1 == parent2);
            

            int crossoverPoint1 = rand()%n;
            int crossoverPoint2 = rand()%n;
            
            if (crossoverPoint1 > crossoverPoint2) {
                std::swap(crossoverPoint1, crossoverPoint2);
            }
                
            individual child1 = crossover(graph, population[parent1], population[parent2]);
            individual child2 = crossover(graph, population[parent2], population[parent1]);
            // individual child1 = pmxCrossover(graph, population[parent1].genotype, population[parent2].genotype, crossoverPoint1, crossoverPoint2);
            // individual child2 = pmxCrossover(graph, population[parent2].genotype, population[parent1].genotype, crossoverPoint1, crossoverPoint2);

            mutation(child1, mutationRate, graph);
            mutation(child2, mutationRate, graph);

            if (isDuplicate(newPopulation, newPopulation.size()-1)) {
                mutation(child1, 1.0, graph);
            }
            newPopulation.push_back(child1);
            if (isDuplicate(newPopulation, newPopulation.size()-1)) {
                mutation(child2, 1.0, graph);
            }
            newPopulation.push_back(child2);
        }

        population = newPopulation;
        // std::cout << "3" << std::endl;
        // auto bestInGeneration = *std::min_element(population.begin(), population.end(), 
        // [](const individual& a, individual& b) {
        //     return a.phenotype < b.phenotype;
        // });

        // std::cout << "Generation " << generation << " best " << bestInGeneration.phenotype << std::endl;
            // std::cout << "Generation " << generation << std::endl;
    }
    auto bestInGeneration = *std::min_element(population.begin(), population.end(), 
    [](const individual& a, individual& b) {
        return a.phenotype < b.phenotype;
    });
    std::cout << 1 << " " << bestInGeneration.phenotype << " " << 1 << std::endl;
    // std::cout << "best " << bestInGeneration.phenotype << std::endl;
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

    std::vector<short> previous = random_path(graph.size()); // Krok 1
    ga(previous, graph);

    return 0;
}