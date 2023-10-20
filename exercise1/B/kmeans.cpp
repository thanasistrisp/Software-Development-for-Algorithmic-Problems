#include <vector>
#include <set>
#include <tuple>

using namespace std;

#include "kmeans.h"

KMeans::KMeans(std::vector<std::vector<double>> &dataset) : dataset(dataset)
{
    point_to_cluster.resize(dataset.size());
}

KMeans::~KMeans()
{

}

tuple<int,int> KMeans::assign_lloyds(int index, int k)
{
    int old_cluster = point_to_cluster[index];
    int new_cluster = 0;
    double min_dist = distance(dataset[index], centroids[0]);
    for(int i = 1; i < k; i++){
        double dist = distance(dataset[index], centroids[i]);
        if(dist < min_dist){
            min_dist = dist;
            new_cluster = i;
        }
    }
    point_to_cluster[index] = new_cluster; // update point's cluster
    clusters[old_cluster].erase(clusters[old_cluster].begin() + index); // remove point from old cluster
    clusters[new_cluster].push_back(index); // add point to new cluster
    return make_tuple(old_cluster, new_cluster);
}

tuple<int,int> KMeans::assign_lsh(int index, int k)
{
    return make_tuple(-1,-1);
}

tuple<int,int> KMeans::assign_hypercube(int index, int k)
{
    return make_tuple(-1,-1);
}

void KMeans::update(int k) // MacQueen's update rule
{
    for(int i = 0; i < k; i++){ // For each cluster
        for(int j = 0; j < (int) centroids[i].size(); j++){
            centroids[i][j] = 0; // Reset centroid
        }
        for(int j = 0; j < (int) clusters[i].size(); j++){ // For each point in cluster
            for(int l = 0; l < (int) dataset[clusters[i][j]].size(); l++){
                centroids[i][l] += dataset[clusters[i][j]][l]; // Add point's coordinates to centroid
            }
        }
        for(int j = 0; j < (int) centroids[i].size(); j++){
            centroids[i][j] /= clusters[i].size(); // Divide by number of points in cluster (mean)
        }
    }
}

void KMeans::update(int k1, int k2, int k) { // recalculate centroids only for the two affected clusters
    set<int> s;
    s.insert(k1);
    s.insert(k2);
    for (int i : s) {
        for (int j = 0; j < (int) centroids[i].size(); j++) {
            centroids[i][j] = 0;
        }
        for (int j = 0; j < (int) clusters[i].size(); j++) {
            for (int l = 0; l < (int) dataset[clusters[i][j]].size(); l++) {
                centroids[i][l] += dataset[clusters[i][j]][l];
            }
        }
        for (int j = 0; j < (int) centroids[i].size(); j++) {
            centroids[i][j] /= clusters[i].size();
        }
    }

}

void KMeans::compute_clusters(int k, update_method method, std::vector<int> method_args, const std::vector<double>&)
{
    tuple<int,int> (KMeans::*assign)(int, int);
    if(method == CLASSIC){
        assign = &KMeans::assign_lloyds;
    }
    else if(method == LSH){
        assign = &KMeans::assign_lsh;
    }
    else {
        assign = &KMeans::assign_hypercube;
    }

    // Initialize centroids using KMeans++ algorithm.
    kmeanspp(k);
    while(true){
        for(int i = 0; i < (int) dataset.size(); i++){
            // Assign point to cluster and apply MacQueen's update rule.
            int old_cluster, new_cluster;
            tie(old_cluster, new_cluster) = (this->*assign)(i, k);
            if(old_cluster != new_cluster){
                update(old_cluster, new_cluster, k);
            }
        }
        update(k);
    }
}

/* todo: add check for unchanged cluster centroids => end of algorithm */

std::vector<std::vector<double>> KMeans::get_centroids() const
{
    return centroids;
}

std::vector<std::vector<int>> KMeans::get_clusters() const
{
    return clusters;
}