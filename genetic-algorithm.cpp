#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <random>
#include <utility>
#include <vector>
using namespace std;

using NQueenState = int;
const ssize_t NOGENLIMIT = -1;

int nqueen_fitthreshold(size_t table_size) {
  return table_size * ((table_size - 1) / 2.0);
}

template <typename S>
int nqueen_fitness(vector<S> individual) {
  int collisions = 0;
  for (int i = 0; i < individual.size() - 1; i++) {
    for (int j = 1; j < individual.size(); j++) {
      int target = individual[i + j];
      int forward_move = individual[i];
      int forward_ascending_move = individual[i] + j;
      int forward_descending_move = individual[i] - j;
      bool at_bounds_upwards = forward_ascending_move <= individual.size();
      bool at_bounds_downwards = forward_descending_move > 0;

      if ((forward_move == target) ||
          (at_bounds_upwards && forward_ascending_move == target) ||
          (at_bounds_downwards && forward_descending_move == target))
        collisions++;
    }
  }
  return nqueen_fitthreshold(individual.size()) - collisions;
}

template <typename S>
vector<S> mutate(vector<S> individual, vector<S> genes) {
  int n = rand() % individual.size();
  int m = rand() % genes.size();
  if (individual[n] == m)
    m = (m + 1) % genes.size();

  individual[n] = genes[m];
  return individual;
}

template <typename S>
vector<S> reproduce(vector<S> parentx, vector<S> parenty) {
  int n = rand() % parentx.size();
  vector<S> child;

  for (int i = 0; i < n; i++)
    child.push_back(parentx[i]);
  for (int i = n; i < parenty.size(); i++)
    child.push_back(parenty[i]);

  return child;
}

// https://stackoverflow.com/questions/56723551
template <typename S>
pair<vector<S>, vector<S>> selection(vector<vector<S>> population, vector<int> weights) {
  int total = 0;
  for (int weight : weights)
    total += weight;

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> distribution(0, total);
  auto rnd = distribution(gen);

  vector<vector<S>> parents;
  int accumulated = 0;
  for (int i = 0; i < population.size() && parents.size() < 2; i++) {
    accumulated += weights[i];
    if (rnd <= accumulated)
      parents.push_back(population[i]);
  }

  for (int i = population.size() - 1; parents.size() < 2; i--)
    if (parents.size() == 0 || parents[0] != population[i])
      parents.push_back(population[i]);

  return {parents[0], parents[1]};
}

template <typename S>
vector<int> weight_by(vector<vector<S>> population, function<int(vector<S>)> fitness) {
  vector<int> weights(population.size());
  for (int i = 0; i < population.size(); i++)
    weights[i] = fitness(population[i]);
  return weights;
}

template <typename S>
vector<S> genetic_algorithm(vector<vector<S>> population, function<int(vector<S>)> fitness, vector<S> genes, int fitthreshold = 0, ssize_t genlimit = 10000, double mutation_probability = 0.1, shared_ptr<size_t> gens = NULL) {
  while (genlimit > 0 || genlimit == NOGENLIMIT) {
    auto weights = weight_by(population, fitness);

    vector<vector<S>> generation(population.size());
    for (int i = 0; i < population.size(); i++) {
      auto [parentx, parenty] = selection(population, weights);
      auto child = reproduce(parentx, parenty);
      if (rand() / static_cast<double>(RAND_MAX) < mutation_probability)
        child = mutate(child, genes);

      if (fitthreshold != 0)
        for (int i = 0; i < population.size(); i++)
          if (weights[i] >= fitthreshold)
            return population[i];

      generation[i] = child;
    }
    population = generation;

    if (gens != NULL)
      (*gens)++;

    if (genlimit != -1)
      genlimit--;
  }

  return population[0];
}

void print_table_with_result(vector<NQueenState> solution, size_t gens, size_t table_size) {
  auto get_room_char = [solution](int c, int k) { return solution[c] == k + 1 ? "♛" : " "; };
  cout << "Generations: " << gens << endl;
  cout << "Array representation: [ ";
  for (auto gene : solution) {
    cout << gene << " ";
  }
  cout << "]" << endl;
  cout << "┏━";
  for (int c = table_size - 1; c > 0; c--)
    cout << "━━┳━";
  cout << "━━┓" << endl;
  for (int c = table_size - 1; c > 0; c--) {
    cout << "┃ ";
    for (int k = 0; k < table_size - 1; k++)
      cout << get_room_char(c, k) << " ┃ ";
    cout << get_room_char(c, (int)table_size - 1) << " ┃" << endl;
    cout << "┣━";
    for (int k = table_size - 1; k > 0; k--)
      cout << "━━╋━";
    cout << "━━┫" << endl;
  }
  cout << "┃ ";
  for (int k = 0; k < table_size - 1; k++)
    cout << get_room_char(0, k) << " ┃ ";
  cout << get_room_char(0, (int)table_size - 1) << " ┃" << endl;
  cout << "┗━";
  for (int c = table_size - 1; c > 0; c--)
    cout << "━━┻━";
  cout << "━━┛" << endl;
}

int main(void) {
  srand(time(NULL));
  size_t table_size = 8;
  vector<NQueenState> genes = {1, 2, 3, 4, 5, 6, 7, 8};
  size_t population_size = 100;
  vector<vector<NQueenState>> population;
  shared_ptr<size_t> gens = make_shared<size_t>(0);

  for (int i = 0; i < population_size; i++) {
    population.push_back({});
    for (int j = 0; j < table_size; j++)
      population[i].push_back(rand() % table_size + 1);
  }

  auto fittest = genetic_algorithm<NQueenState>(population, nqueen_fitness<NQueenState>, genes, nqueen_fitthreshold(table_size), NOGENLIMIT, 0.1, gens);
  print_table_with_result(fittest, *gens, table_size);
}
