#include <iostream>
#include <tuple>

#include "kmeans_eval.hpp"
#include "brute_force.hpp"

using namespace std;

KMeansEval::KMeansEval(const std::vector<std::vector<double>>& dataset) : KMeans(dataset) {}

vector<variant<double, vector<double>>> KMeansEval::silhouette(vector<vector<double>> &initial_dataset, vector<vector<double>> &decoded_centroids) {
	vector<double> si(clusters.size(), 0);
	vector<double> sil(clusters.size(), 0);
    vector<vector<int>> clusters = get_clusters();
    double stotal = 0;
    for (int i = 0; i < (int) clusters.size(); i++) {
        for (int j = 0; j < (int) clusters[i].size(); j++) {
            si[i] += silhouette(clusters[i][j], initial_dataset, decoded_centroids);
        }   
        stotal += si[i];
        si[i] /= clusters[i].size();
        sil[i] = si[i];
    }
    stotal /= initial_dataset.size();

    // Return stotal, sil.
    return {stotal, sil};
}

double KMeansEval::silhouette(int i, vector<vector<double>> &initial_dataset, vector<vector<double>> &decoded_centroids)
{
    int cluster = point_to_cluster[i];
    double a = 0, b = 0;
    for(int j : clusters[cluster]){
        a += distance(initial_dataset[i], initial_dataset[j]);
    }
    a /= clusters[cluster].size();

    // Find second closest cluster.
    int c1 = cluster, c2 = -1;
    for(int j = 0; j < (int) centroids.size(); j++){
        if(j != c1){
            double dist = distance(initial_dataset[i], decoded_centroids[j]);
            if(c2 == -1 || dist < distance(initial_dataset[i], decoded_centroids[c2])){
                c2 = j;
            }
        }
    }
    for(int j : clusters[c2]){
        b += distance(initial_dataset[i], initial_dataset[j]);
    }
    b /= clusters[c2].size();
    
    return (b - a) / max(a, b);
}