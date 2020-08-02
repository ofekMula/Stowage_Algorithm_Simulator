//
// Created by Owner on 18-May-20.
//

#ifndef EX2_ADV_CPP_SHIPPLAN_H
#define EX2_ADV_CPP_SHIPPLAN_H

#include <vector>

using std::vector;

class ShipPlan
{
	friend class Ship;
	// It makes sense that Ship is a friend of ShipPlan because the two classes
	// are inter-connected.

	int width;
	int length;
	int height;
	vector<vector<int>> floor_start;
	/* a 2D vector of sizes length*width signifying the lowest floor
	 * of each position.
	 */

public:
	int get_width() const {return width;}
	int get_length() const {return length;}
	int get_height() const {return height;}

	int get_floor_start(int row, int col) const;

	ShipPlan();
	ShipPlan(int p_width, int p_length, int p_height);
	ShipPlan(int p_width, int p_length, int p_height,vector<vector<int>>);
};


#endif //EX2_ADV_CPP_SHIPPLAN_H
