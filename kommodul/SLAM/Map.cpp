#include <iostream>
#include <vector>

using namespace std;

class Map
{
private:
	int row = 100;
	int col = 100;
	vector<vector<float>> map{ row, vector<float>(col, 0.0) };


public:
	float get(int x, int y)
	{
		return map.at(x).at(y);
	}
	void set(int x, int y, float value)
	{
		map.at(x).at(y) = value;
	}
	//Absolute coodinates
	void add_value(int abs_x, int abs_y, float value)
	{
		set(abs_x, abs_y, value);
	}
};