#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Map
{
public:
	int MAP_ROW = 2000;
    int MAP_COL = 2000;
	float get_value(int x, int y)
	{
		return map.at(x).at(y);
	}
	void set_value(int x, int y, float value)
	{
		map.at(x).at(y) = value;
	}
	//Absolute coodinates
	void add_value(int abs_x, int abs_y, float value)
	{
		set_value(abs_x, abs_y, value);
	}
private:
	vector<vector<float>> map{ MAP_ROW, vector<float>(MAP_COL, 0.0) };
};
