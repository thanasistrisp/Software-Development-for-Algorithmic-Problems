#pragma once

#include <vector>
#include <string>
#include <tuple>

#include "hypercube.hpp"

std::vector<std::vector<double>> read_mnist_data(const std::string &filename); // does not check if file exists
std::tuple<int, int, int, int, int, int> read_config_file(const std::string &filename); // does not check if file exists
void export_image(const std::vector<double> &image, std::string filename);
void handle_ouput(hypercube &cube, std::ofstream &output, const std::vector<std::vector<double>> &queries, double R, int N);
bool file_exists(const std::string &filename);