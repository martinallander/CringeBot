// SLAM.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

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
    vector <pair<string, string>> words {18000};
    string line;
    pair<string, string> data;
    
    ifstream myfile ("lmao.txt");
    if (myfile.is_open())
    
    while (getline(myfile,line))
    {
    	//cout << line << "\n";
        if (line[0] == 'S')
        {
    	     cout << "bonus" << "\n";
    	     data = split(line, ' ');
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
    
    myfile.close();
    return 0;
}

