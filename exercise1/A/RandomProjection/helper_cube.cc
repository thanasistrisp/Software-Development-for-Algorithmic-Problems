#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <map>
#include <random>
#include <tuple>
#include <set>

#include "lp_metric.h"
#include "hypercube.hpp"

using namespace std;

std::vector<std::vector<int>> hypercube::pack(const std::vector<int> &q_proj, int hamming_distance)
{
	if (hamming_distance == 0) {
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
	// get all permutations of q_proj with hamming distance
	set<vector<int>> permutation = get_permutation(q_proj, hamming_distance);
	// for each permutation, find the vertex in hash_table
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
	if (f_map[i].find(x) == f_map[i].end()) {
		f_map[i][x] = rand() % 2;
	}
	return f_map[i][x];
}

// get all permutations of q_proj based on hamming distance
set<vector<int>> hypercube::get_permutation(const vector<int> &q_proj, int hamming_distance) {
	set<vector<int>> result;
	for (int i = 0; i < (int) q_proj.size(); i++) {
		vector<int> permutation = q_proj;
		permutation[i] = 1 - permutation[i];
		// if not in used_vertices, insert
		if (hamming_distance != 1)
			result.insert(permutation);
		else if (used_vertices->find(permutation) == used_vertices->end()) {
			result.insert(permutation);
			used_vertices->insert(permutation);
		}
	}
	if (hamming_distance == 1)
		return result;
	set<vector<int>> temp;
	for (auto it = result.begin(); it != result.end(); it++) {
		set<vector<int>> temp2 = get_permutation(*it, hamming_distance - 1);
		temp.insert(temp2.begin(), temp2.end());
	}
	return temp;
}