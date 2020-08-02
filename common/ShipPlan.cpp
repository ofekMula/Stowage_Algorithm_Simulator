//
// Created by Owner on 18-May-20.
//

#include "ShipPlan.h"

ShipPlan::ShipPlan() : ShipPlan(0, 0, 0){}

ShipPlan::ShipPlan(int p_width, int p_length, int p_height)
		: width(p_width), length(p_length), height(p_height)
{
	floor_start = vector<vector<int>>(length, vector<int>(width));
}

ShipPlan::ShipPlan(int p_width, int p_length,
				   int p_height,vector<vector<int>> floor_start)
		: width(p_width), length(p_length),
		  height(p_height), floor_start(std::move(floor_start)){}

int ShipPlan::get_floor_start(int row, int col) const
{
	return floor_start[row][col];
}
