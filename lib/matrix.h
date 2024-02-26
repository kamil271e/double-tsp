#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Matrix{
public:
    Matrix();
    void load_from_path(std::string);
    std::vector<int> x_coord;
    std::vector<int> y_coord;
};
