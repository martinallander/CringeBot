#include <math.h>

using namespace std;

class Robot
{
private:
    float pos_x;
    float pos_y;
    float angle;

    Robot()
    :x {MAP_ROW / 2}, y {MAP_COL / 2}, angle{0}
    {
    }

public:
    int get_data_x_coord(float dist, float in_angle)
    {
        return pos_x + dist * sin((angle + in_angle) * PI / 180);
    }

    int get_data_y_coord(float dist, float in_angle)
    {
        return pos_y + dist * cos((angle + in_angle) * PI / 180);
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
        pos_x += dist * sin((angle + delta / 2) * PI / 180);
        pos_y += dist * cos((angle + delta / 2) * PI / 180);
        angle += delta;
    }


};
