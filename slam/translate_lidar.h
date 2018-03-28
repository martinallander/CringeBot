#pragma once

#include "map.h"

#define FIRST 1
#define SECOND 0.9
#define THIRD 0.74
#define FOURTH 0.25

using namespace std;

void add_dot_first_lap(Map & map, int abs_x, int abs_y)
{
    //Fourth "ring" outside of the added dot
    for(int i = -3; i < 4; i++)
    {
        for(int j = -3; j < 4; j + 6)
        {
            if (map.get_value(abs_x + i, abs_y + j) < FOURTH)
            map.add_value(abs_x + i, abs_y + j, FOURTH);
        }
    }
    for(int i = -2; i < 3; i++)
    {
        for(int j = -3; j < 4; j + 6)
        {
            if (map.get_value(abs_x + j, abs_y + i) < FOURTH)
            map.add_value(abs_x + j, abs_y + i, FOURTH);
        }
    }

    //Third "ring" outside of the added dot
    for(int i = -2; i < 3; i++)
    {
        for(int j = -2; j < 3; j + 4)
        {
            if (map.get_value(abs_x + i, abs_y + j) < THIRD)
            map.add_value(abs_x + i, abs_y + j, THIRD);
        }
    }
    for(int i = -1; i < 2; i++)
    {
        for(int j = -2; j < 3; j + 4)
        {
            if (map.get_value(abs_x + j, abs_y + i) < THIRD)
            map.add_value(abs_x + j, abs_y + i, THIRD);
        }
    }

    //Second "ring" outside of the added dot
    for(int i = -1; i < 2; i++)
    {
        for(int j = -1; j < 2; j++)
        {
            if (map.get_value(abs_x + i, abs_y + j) < SECOND)
            map.add_value(abs_x + i, abs_y + j, SECOND);
        }
    }

    map.add_value(abs_x, abs_y, FIRST);
}

void add_dot(Map & map, int abs_x, int abs_y)
{
    //Fourth "ring" outside of the added dot
    for(int i = -3; i < 4; i++)
    {
        for(int j = -3; j < 4; j + 6)
        {
            if (map.get_value(abs_x + i, abs_y + j) < FOURTH)
            map.add_value(abs_x + i, abs_y + j, FOURTH);
        }
    }
    for(int i = -2; i < 3; i++)
    {
        for(int j = -3; j < 4; j + 6)
        {
            if (map.get_value(abs_x + j, abs_y + i) < FOURTH)
            map.add_value(abs_x + j, abs_y + i, FOURTH);
        }
    }

    //Third "ring" outside of the added dot
    for(int i = -2; i < 3; i++)
    {
        for(int j = -2; j < 3; j + 4)
        {
            if (map.get_value(abs_x + i, abs_y + j) < THIRD)
            map.add_value(abs_x + i, abs_y + j, THIRD);
        }
    }
    for(int i = -1; i < 2; i++)
    {
        for(int j = -2; j < 3; j + 4)
        {
            if (map.get_value(abs_x + j, abs_y + i) < THIRD)
            map.add_value(abs_x + j, abs_y + i, THIRD);
        }
    }

    //Second "ring" outside of the added dot
    for(int i = -1; i < 2; i++)
    {
        for(int j = -1; j < 2; j++)
        {
            if (map.get_value(abs_x + i, abs_y + j) < SECOND)
            map.add_value(abs_x + i, abs_y + j, SECOND);
        }
    }

    if (map.get_value(abs_x, abs_y) < SECOND)
    map.add_value(abs_x, abs_y, SECOND);
}

void add_dot_incremental(Map & map, int abs_x, int abs_y)
{
    for(int k = 0; k < 5; k++)
    {
        for(int i = -k; i < k + 1; i++)
        {
            for(int j = -k; j < k + 1; j++)
            {
                map.add_value(abs_x + i, abs_y + j, 0.01);
            }
        }
    }
}

void lower_weight(Map & map)
{
    for(int i = 0; i < map.MAP_ROW; i++)
    {
        for(int j = 0; j < map.MAP_COL; j++)
        {
            float value = map.get_value(i, j);
            if(value < 0.03)
            {
                map.set_value(i, j, 0);
            }
            else if(value < 0.75)
            {
                map.set_value(i, j, value - 0.03);
            }
        }
    }
}
