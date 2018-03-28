#pragma once
#include <math.h>

using namespace std;

class Robot
{
private:
    float pos_x;
    float pos_y;
    float angle;

public:
	int MAP_ROW = 1000;
    int MAP_COL = 1000;
	Robot() :pos_x {500.0}, pos_y {500.0}, angle{0}
    {
    }
	
    
    int get_data_x_coord(float dist, float in_angle)
    {
        float ret = pos_x + (dist / 20) * sin((angle + in_angle) * M_PI / 180);
       // cout << pos_x << " " << sin((angle + in_angle) * M_PI / 180) << " \n";
        return ret;
    }

    int get_data_y_coord(float dist, float in_angle)
    {
        float ret = pos_y + (dist / 20) * cos((angle + in_angle) * M_PI / 180);
        return ret;
    }

    float get_robot_x()
    {
        return pos_x;
    }

    float get_robot_y()
    {
        return pos_y;
    }

    float get_robot_angle()
    {
        return angle;
    }

    void move(float dist, float delta) //delta-angle
    {
        pos_x += dist * sin((angle + delta / 2) * M_PI / 180);
        pos_y += dist * cos((angle + delta / 2) * M_PI / 180);
        angle += delta;
    }
};
