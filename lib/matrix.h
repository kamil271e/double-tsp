#ifndef MATRIX_H
#define MATRIX_H

#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Matrix {
  public:
	Matrix();
	void load_from_path(std::string);
	void generate_dist_matrix();
	int calc_distance(int, int, int, int);
	std::vector<int> x_coord;
	std::vector<int> y_coord;
	std::vector<std::vector<int>> dist_matrix;
};

#endif // MATRIX_H
