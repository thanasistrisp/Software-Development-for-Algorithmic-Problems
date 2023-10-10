#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include "metrics.hpp"
#include "h.hpp"
#include "hypercube.hpp"

using namespace std;

hypercube::hypercube(std::vector<std::vector<double>> p, int k, int M, int probes, 
					 int N, double R, double (*distance)(std::vector<double>, std::vector<double> distance))
{
	this->p = p;
	this->k = k;
	this->M = M;
	this->probes = probes;
	this->N = N;
	this->R = R;
	this->distance = distance;
	cout << "Preprocessing..." << endl;
	p_proj = preprocess(p, k);
}

vector<vector<double>> hypercube::query_n_nearest_neighbors(vector<double> q) {
	cout << "Querying n nearest neighbors..." << endl;

	vector<int> q_proj = calculate_q_proj(q);

	int num_points = 0;
	int num_vertices = 0;

	vector<vector<double>> k_candidates(N, vector<double>(p[0].size()));
	vector<double> k_distances(N, numeric_limits<double>::max());

	int hamming_distance = 0;
	while (true) {
		vector<vector<int>> vertices = pack(p_proj, (int) p_proj.size(), q_proj, hamming_distance);
		for (int i = 0; i < (int) vertices.size(); i++) {
			for (int j = 0; j < (int)vertices[i].size(); j++)
			{
				if (num_points >= M)
					return k_candidates;
				if (distance(p[vertices[i][j]], q) < k_distances[N - 1])
				{
					k_candidates[N - 1] = p[vertices[i][j]];
					k_distances[N - 1] = distance(p[vertices[i][j]], q);
					for (int k = N - 1; k > 0; k--)
					{
						if (k_distances[k] < k_distances[k - 1])
						{
							swap(k_distances[k], k_distances[k - 1]);
							swap(k_candidates[k], k_candidates[k - 1]);
						}
					}
				}
				num_points++;
			}
			num_vertices++;
			if (num_vertices >= probes)
				return k_candidates;
		}
		hamming_distance++;
	}

	return k_candidates;
}

vector<vector<double>> hypercube::query_range(vector<double> q) {
	cout << "Querying range..." << endl;

	vector<int> q_proj = calculate_q_proj(q);

	int num_points = 0;
	int num_vertices = 0;

	map <double, vector<double>> candidates;
	int hamming_distance = 0;
	while (true) {
		vector<vector<int>> vertices = pack(p_proj, (int) p_proj.size(), q_proj, hamming_distance);
		for (int i = 0; i < (int) vertices.size(); i++) {
			for (int j = 0; j < (int)vertices[i].size(); j++)
			{
				double dist = distance(p[vertices[i][j]], q);
				if (dist < R)
				{
					candidates[dist] = p[vertices[i][j]];
				}
				num_points++;
			}
			num_vertices++;
			if (num_vertices >= probes)
				break;
		}
		if (num_vertices >= probes)
			break;
		hamming_distance++;
	}

	vector<vector<double>> result;
	for (auto it = candidates.begin(); it != candidates.end(); it++) {
		result.push_back(it->second);
	}

	return result;
}

vector<int> hypercube::calculate_q_proj(vector<double> q) {
	vector<int> q_proj;
	for (int i = 0; i < k; i++) {
		q_proj.push_back(f(h(q)));
	}
	return q_proj;
}