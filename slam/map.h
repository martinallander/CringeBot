#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Map
{
	private:
	vector<vector<float>> map_{1000, vector<float>(1000, 0.0) };
	vector<pair<int, int> > changed_squares;
public:
	Map() = default;
	int MAP_ROW = 1000;
    int MAP_COL = 1000;
	float get_value(int x, int y)
	{
		return map_.at(x).at(y);
	}
	void set_value(int x, int y, float value)
	{
		map_.at(x).at(y) = value;
	}
	//Absolute coodinates
	void add_value(int abs_x, int abs_y, float value)
	{
		pair<int, int> square = {abs_x/25, abs_y/25};
		if (find(changed_squares.begin(), 
				 changed_squares.end(),
				 square) 						== changed_squares.end())
		{
			changed_squares.push_back(square);
		}
		set_value(abs_x, abs_y, value);
	}
	void print_map()
	{
		for_each(begin(changed_squares), end(changed_squares), [this] (pair<int, int> p)
		{
			for(int k = 0; k < 25; k++)
			{
				for(int l = 0; l < 25; l++)
				{
					cout << this->get_value(25 * p.first + l, 25 * p.second + k); 
					cout << " ";
				}
				cout << "\n";
			}
			cout << "\n";
		});
	}
};
/*
struct point
{
	int x;
	int y;
	float value;
};
*/
