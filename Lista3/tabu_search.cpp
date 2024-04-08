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


// Funkcja oceny
uint32_t evaluate_solution(const std::vector<uint16_t> sequence, const std::vector<std::vector<uint16_t>>& graph) noexcept {
    const size_t n = sequence.size();
    uint32_t sum = 0;

    for (size_t i = 1; i <= n; ++i)
        sum += graph[sequence[i-1]][sequence[i%n]];

    return sum;
}

// Funkcja oceny
uint32_t evaluate_solution1(const uint16_t a, const uint16_t b, const std::vector<uint16_t> sequence, const std::vector<std::vector<uint16_t>>& graph, const uint32_t curr_cost) noexcept {
    const size_t n = sequence.size();
    uint32_t sum = curr_cost;

    if (a != 0 || b != n-1) {
        sum += graph[sequence[(a-1+n)%n]][sequence[a%n]];
        sum -= graph[sequence[(a-1+n)%n]][sequence[b%n]];
        sum += graph[sequence[b%n]][sequence[(b+1)%n]];
        sum -= graph[sequence[a%n]][sequence[(b+1)%n]];
    }

    return sum;
}

std::vector<uint16_t> invert(std::vector<uint16_t> current_solution, size_t a, size_t b) noexcept {
    while(a < b) {
        std::swap(current_solution[a], current_solution[b]);
        a++;
        b--;
    }

    return current_solution;
}

// Funkcja generująca wszystkie inwersje w danej permutacji
// std::vector<std::vector<uint16_t>> generate_all_inversions(const std::vector<uint16_t>& permutation) noexcept {
//     std::vector<std::vector<uint16_t>> all_inversions;
//     const size_t n = permutation.size();

//     for (size_t i = 0; i < n; ++i)
//         for (size_t j = i + 1; j < n; ++j)
//             all_inversions.push_back(invert(permutation, i, j));

//     return all_inversions;
// }
std::vector<std::pair<size_t, size_t>> generate_all_inversions(const size_t n) noexcept {
    std::vector<std::pair<size_t, size_t>> all_inversion;

    for (size_t i = 0; i < n; ++i)
        for (size_t j = i + 1; j < n; ++j)
            all_inversion.push_back(std::make_pair(i, j));

    return all_inversion;
}



std::vector<std::pair<size_t, size_t>> random_inversions(const size_t n) noexcept {
    std::vector<std::pair<size_t, size_t>>inversion;
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, n-1);
    uint16_t num1, num2;

    for (size_t i = 0; i < n; ++i) {
        num1 = dis(g);
        num2 = dis(g);

        while (num1 == num2)
            num2  = dis(g);

        if (num1 < num2)
            inversion.push_back(std::make_pair(num1, num2));
        else
            inversion.push_back(std::make_pair(num2, num1));
    }

    return inversion;
}


std::vector<uint16_t> tabu_search(const std::vector<uint16_t> initial_solution, const std::vector<std::vector<uint16_t>>& graph, int& counter) noexcept {
    const size_t n = initial_solution.size();
    const size_t m = n;

    std::vector<uint16_t> current_solution = initial_solution;
    std::vector<uint16_t> tmp;
    std::vector<uint16_t> better;
    std::vector<uint16_t> best_solution = initial_solution;

    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> tabu_list;
    std::vector<std::pair<size_t, size_t>> inver;

    bool is_equal = false;
    // bool greater_precision = false;
    
    uint32_t min = UINT16_MAX;
    uint32_t curr_cost = evaluate_solution(current_solution, graph);
    uint32_t tmp_cost;
    uint32_t best_cost = curr_cost;

    uint32_t no_improvment_counter = 0;

    while (no_improvment_counter < 25000) {
        // if (greater_precision)
            // inver = generate_all_inversions(n);
        // else
            inver = random_inversions(n);
        
        min = UINT32_MAX;

        for (const auto& i: inver) {
            tmp = invert(current_solution, i.first, i.second);
            tmp_cost = evaluate_solution1(i.first, i.second, tmp, graph, curr_cost);
            // tmp_cost = evaluate_solution(tmp, graph);
            if (tmp_cost < min) {
                is_equal = false;
                for (const auto& j: tabu_list) {
                    if (tmp_cost == j.first) {
                        if (std::equal(j.second.begin(), j.second.end(), tmp.begin())) {
                            std::cout << "1" << std::endl;
                            is_equal = true;
                            break;
                        }
                    }
                }
                if (!is_equal) {
                    better = tmp;
                    min = evaluate_solution(tmp, graph);
                }
            }
        }
/*
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                tmp = invert(current_solution, i, j);
                tmp_cost = evaluate_solution1(i, j, tmp, graph, curr_cost);

                if (tmp_cost < min) {
                    is_equal = false;
                    for (const auto& j: tabu_list) {
                        if (tmp_cost == j.first) {
                            if (std::equal(j.second.begin(), j.second.end(), tmp.begin(), tmp.end())) {
                                std::cout << "1" << std::endl;
                                is_equal = true;
                                break;
                            }
                        }
                    }
                    if (!is_equal) {
                        better = tmp;
                        min = evaluate_solution(tmp, graph);
                    }
                }
            }
        }
*/
        curr_cost = min;

        // for (const auto& i: better)
        //     std::cout << i << " ";

        // std::cout << std::endl;

        current_solution = better;
        tabu_list.push_back(std::make_pair(min, better));
        if (tabu_list.size() > m)
            tabu_list.erase(tabu_list.begin());

        if (min < best_cost) {
            best_cost = min;
            best_solution = better;
            no_improvment_counter = 0;
        } else {
            // greater_precision = true;
            ++no_improvment_counter;
        }
        std::cout << best_cost << " " << min << std::endl;
        ++counter;

    }
    

    return best_solution;
}

std::vector<uint16_t> random_path(const size_t n) noexcept {
    std::vector<uint16_t> permutation(n);

    for (size_t i = 0; i < n; i++)
        permutation[i] = i;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(permutation.begin(), permutation.end(), g);

    return permutation;
}

int main(int argc, const char *argv[]) {
    std::ifstream file(argv[1]);
    std::string text;

    size_t n = 0;
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

    std::vector<std::vector<uint16_t>> graph(n, std::vector<uint16_t>(n));
    std::vector<std::pair<uint16_t,uint16_t>> V;

    // Wczytywanie danych z pliku
    int a, b;
    for (size_t i = 0; i < n; i++) {
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
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i != j) {
                graph[i][j] = std::round(sqrt(std::pow(V[i].first - V[j].first, 2) + std::pow(V[i].second - V[j].second, 2)));
            }  else {
                graph[i][j] = 0;
            }
        }
    }

    int counter = 0;
    
    std::vector<uint16_t> previous = random_path(graph.size());
    std::vector<uint16_t> sequence = tabu_search(previous, graph, counter);
    std::cout << evaluate_solution(previous, graph) << " " << evaluate_solution(sequence, graph) << " " << counter << std::endl;

    return 0;
}
