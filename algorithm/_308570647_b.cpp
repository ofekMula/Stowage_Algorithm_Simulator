//
// Created by Owner on 16-May-20.
//

#include "_308570647_b.h"

REGISTER_ALGORITHM(_308570647_b)


void _308570647_b::order_containers_for_load(
		vector<Container> &containers_to_load) const
{
	// Sort containers by route so the rejected ones are furthest in route.
	std::sort(containers_to_load.begin(), containers_to_load.end(),
			  CompareByRoute(ship_route, curr_port_idx, false));
}