#ifndef UTILS_H
#define UTILS_H

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string&, char);
std::vector<std::vector<int>> generate_adj_matrix(const std::vector<int>&);
void visualize_adj_matrix(std::vector<std::vector<int>>);

#endif // UTILS_H
