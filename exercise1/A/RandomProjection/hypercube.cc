#include <iostream>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>

#include "metrics.hpp"
#include "hypercube.hpp"
#include "defines.hpp"

using namespace std;

hypercube::hypercube(std::vector<std::vector<double>> p, int k, int M, int probes, 
					 int N, double R, double (*distance)(const std::vector<double> &, const std::vector<double> &))
{
	this->p = p;
	this->k = k;
	this->M = M;
	this->probes = probes;
	this->N = N;
	this->R = R;
	this->distance = distance;
	
	clock_t start = clock();
	cout << "Preprocessing..." << endl;
	
	// Initialize h_i functions, i = 1, ..., k.
    HashFunction *h;
    for(int i = 0; i < k; i++){
        h = new HashFunction(p[0].size(), w);
        hash_functions.push_back(h);
    }

	// Initialize f_map
	f_map = new unordered_map<int, int>[k];

	p_proj = preprocess(p, k);

	clock_t end = clock();
	double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
	cout << "Preprocessing time: " << elapsed_secs << endl;
}

hypercube::~hypercube() {
	for (int i = 0; i < (int) hash_functions.size(); i++) {
		delete hash_functions[i];
	}	
	delete[] f_map;
}

tuple<vector<int>, vector<double>> hypercube::query_n_nearest_neighbors(const vector<double> &q, const vector<int> &q_proj) {
	int num_points = 0;
	int num_vertices = 0;
	
	multimap<double, int> candidates; // allows duplicate keys (distances) sorted by ascending order
	int hamming_distance = 0;
	while (true) {
		vector<vector<int>> vertices = pack(p_proj, (int) p_proj.size(), q_proj, hamming_distance);
		for (int i = 0; i < (int) vertices.size(); i++) {
			for (int j = 0; j < (int)vertices[i].size(); j++)
			{
				double dist = distance(p[vertices[i][j]], q);
				candidates.insert(pair<double, int>(dist, vertices[i][j]));
				num_points++;
				if (num_points >= M)
					goto check;
			}
			num_vertices++;
			if (num_vertices >= probes)
				goto check;
		}
		hamming_distance++;
	}

	check:
		vector<int> n_nearest_neighbors;
		vector<double> dist;
		for (auto it = candidates.begin(); it != candidates.end(); it++) {
			n_nearest_neighbors.push_back(it->second);
			dist.push_back(it->first);
		}
		return make_tuple(n_nearest_neighbors, dist);
}

vector<int> hypercube::query_range(const vector<double> &q, const vector<int> &q_proj) {
	int num_points = 0;
	int num_vertices = 0;

	multimap<double, int> candidates;
	int hamming_distance = 0;
	while (true) {
		vector<vector<int>> vertices = pack(p_proj, (int) p_proj.size(), q_proj, hamming_distance);
		for (int i = 0; i < (int) vertices.size(); i++) {
			for (int j = 0; j < (int)vertices[i].size(); j++)
			{
				if (distance(p[vertices[i][j]], q) < R)
				{
					candidates.insert(pair<double, int>(distance(p[vertices[i][j]], q), vertices[i][j]));
				}
				num_points++;
				if (num_points >= M)
					goto check;
			}
			num_vertices++;
			if (num_vertices >= probes)
				goto check;
		}
		hamming_distance++;
	}

	check:
		vector<int> range;
		for (auto it = candidates.begin(); it != candidates.end(); it++) {
			range.push_back(it->second);
		}
		return range;
}

vector<int> hypercube::calculate_q_proj(const vector<double> &q) {
	vector<int> q_proj;
	for (int i = 0; i < k; i++) {
		q_proj.push_back(f(hash_functions[i]->hash(q), i));
	}
	return q_proj;
}

// preprocess: store points p -> [f-i(h_i(p))] for i = 1, ..., d'=k
vector<vector<int>> hypercube::preprocess(const vector<vector<double>> &p, int k) {
	vector<vector<int>> result(p.size(), vector<int>(k));
	for (int i = 0; i < (int) p.size(); i++) {
		for (int j = 0; j < k; j++) {
			int h_j = hash_functions[j]->hash(p[i]);
			result[i][j] = f(h_j, j);
		}
	}
	return result;
}