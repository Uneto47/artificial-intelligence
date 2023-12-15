#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
using namespace std;

using NQueenState = int;

int nqueen_fitthreshold(size_t table_size) {
  return table_size * (table_size - 1 / 2);
}

NQueenState nqueen_fitness(vector<NQueenState> individual) {
  NQueenState collisions = 0;
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
          (at_bounds_downwards && forward_descending_move == target)) {
        collisions++;
      }
    }
  }
  return nqueen_fitthreshold(individual.size()) - collisions;
}

template <typename S>
vector<S> mutate(vector<S> individual, vector<S> genes) {
  int n = rand() % individual.size();
  int m = rand() % genes.size();
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

template <typename S>
pair<vector<S>, vector<S>> selection(vector<vector<S>> population, vector<int> weights) {
  int total = 0;
  for (int weight : weights)
    total += weight;
  int rnd = rand() % total;

  vector<int> parents;
  int accumulated = 0;
  for (int i = 0; i < population.size() && parents.size() < 2; i++) {
    accumulated += weights[i];
    if (rnd <= accumulated) {
      parents.push_back(population[i]);
    }
  }

  for (int i = population.size() - 1; parents.size() < 2; i++)
    if (parents.size() == 0 || parents[0] != population[i])
      parents.push_back(population[i]);

  return {parents[0], parents[1]};
}

template <typename S>
vector<S> genetic_algorithm(vector<vector<S>> &population, function<vector<int>(vector<S>)> fitness, vector<S> genes, int fitthreshold = 0, int genlimit = 1000, double mutation_probability = 0.05) {
  while (genlimit > 0) {
    vector<int> weights;
    for (auto individual : population)
      weights.push_back(fitness(individual));

    if (fitthreshold != 0) {
      vector<S> fittest;
      for (int i = 0; i < population.size(); i++)
        if (weights[i] >= fitthreshold)
          return population[i];
    }

    vector<S> generation;
    for (int i = 0; i < population.size(); i++) {
      auto [parentx, parenty] = selection(population, weights);
      auto child = reproduce(parentx, parenty);
      if (rand() / static_cast<double>(RAND_MAX) < mutation_probability)
        child = mutate(child, genes);

      generation.push_back(child);
    }
    population = generation;
    genlimit--;
  }

  return population[0];
}

int main(void) { cout << "Hello, World" << endl; }
