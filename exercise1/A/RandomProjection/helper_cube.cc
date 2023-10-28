#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <map>
#include <random>
#include <tuple>
#include <set>

#include "lp_metric.hpp"
#include "hypercube.hpp"

using namespace std;

std::vector<std::vector<int>> hypercube::pack(const std::vector<int> &q_proj, int hamming_distance)
{
	if (hamming_distance == 0) {
		// If q_proj exists in hash_table, return the corresponding bucket else return empty vector.
		binary_string bs(q_proj);
		auto it = hash_table->find(bs);
		if (it != hash_table->end()) {
			used_vertices->insert(bs);
			return vector<vector<int>>(1, it->second);
		}
		else {
			return vector<vector<int>>();
		}
	}
	// Get all permutations of q_proj with specific hamming distance.
	set<vector<int>> permutation = get_permutation(q_proj, hamming_distance);
	// For each permutation, find the vertex in hash_table if exists.
	vector<vector<int>> result;
	int sz = permutation.size();
	for (int i = 0; i < sz; i++) {
		vector<int> temp = *permutation.begin();
		permutation.erase(permutation.begin());
		binary_string bs(temp);
		auto it = hash_table->find(bs);
		if (it != hash_table->end()) {
			result.push_back(it->second);
		}
	}
	return result;
}

int hypercube::f(int x, int i) {
	// If f_map[i] does not contain x, calculate f_i(x) and store it in f_map[i] else return the stored value.
	if (f_map[i].find(x) == f_map[i].end()) {
		f_map[i][x] = rand() % 2;
	}
	return f_map[i][x];
}

set<vector<int>> hypercube::get_permutation(const vector<int> &q_proj, int hamming_distance) {
	set<vector<int>> result;
	for (int i = 0; i < (int) q_proj.size(); i++) {
		vector<int> permutation = q_proj;
		permutation[i] = 1 - permutation[i];
		// Edge case: hamming distance = 1.
		if (hamming_distance != 1)
			result.insert(permutation);
		// If permutation is already used, do not insert it to result.
		else if (used_vertices->find(permutation) == used_vertices->end()) {
			result.insert(permutation);
			used_vertices->insert(permutation); // Update.
		}
	}
	if (hamming_distance == 1) // Base case.
		return result;
	set<vector<int>> temp;
	// For each permutation, get all permutations with hamming distance - 1.
	for (auto it = result.begin(); it != result.end(); it++) {
		set<vector<int>> temp2 = get_permutation(*it, hamming_distance - 1);
		temp.insert(temp2.begin(), temp2.end());
	}
	return temp;
}