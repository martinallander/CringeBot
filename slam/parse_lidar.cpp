// SLAM.cpp : Defines the entry point for the console application.
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "translate_lidar.h"
#include "robot.h"

using namespace std;

pair<string, string> split(const string &s, char delim)
{
    pair<string, string> data;
    bool add_theta {false};
    bool add_dist {false};
    bool add_q {false};
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {	
    	if (add_theta)
    	{
    	     data.first = item;
    	     add_theta = false;
    	}
    	if (add_dist)
    	{
    	     data.second = item;
    	     add_dist = false;
    	}
    	if (add_q)
    	{
    	    if (item == "0")
    	    {
    	         add_q = false;
    	         data.first = "no_data";
    	         data.second = "no_data";
    	         return data;
    	    }
    	}
    	if (item == "theta:")
    	     add_theta = true;
    	if (item == "Dist:")
    	     add_dist = true;
    	if (item == "Q:")
    	     add_q = true;
    }
    return data;
}



int main()
{
	Map map;
	Robot rob;
    vector <pair<string, string>> words;
    string line;
    pair<string, string> data;
    
    ifstream myfile ("lmao.txt");
    if (myfile.is_open())
    {
		while (getline(myfile,line))
		{
			//cout << line << "\n";
			if (line[0] == 'S')
			{
    	     //cout << "bonus" << "\n";
			 //data = split(line, ' ');
				 continue;
			if (!(data.first == "no_data"))
				words.push_back(data);
			}
			else
			{
			data = split(line, ' ');
			/*
			cout << data.first;
			cout << "  ";
			cout << data.second;
			cout << "\n";
			*/
			
			if (!(data.first == "no_data"))
				words.push_back(data);
			}
		}
	}
    for_each(begin(words), end(words), [&rob, &map](pair<string, string> p)
    {	
		/*
		cout << stof(p.first);
		cout << " ,";
		cout << stof(p.second);
		cout << "\n";
		 */
		add_dot(map, rob.get_data_x_coord(stof(p.second), stof(p.first)), 
		rob.get_data_y_coord(stof(p.second), stof(p.first)));
	});
	map.print_map();
    
    myfile.close();
    return 0;
}

