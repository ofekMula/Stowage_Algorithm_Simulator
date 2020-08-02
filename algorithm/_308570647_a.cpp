//
// Created by Owner on 16-May-20.
//

#include "_308570647_a.h"

REGISTER_ALGORITHM(_308570647_a)

void _308570647_a::order_containers_for_load(
		vector<Container> &containers_to_load) const
{
	int ship_capacity = ship.get_capacity();
	if (ship_capacity < (int)containers_to_load.size())
	{
		// Sort containers by route so the rejected ones are furthest in route.
		std::sort(containers_to_load.begin(), containers_to_load.end(),
				  CompareByRoute(ship_route, curr_port_idx, false));

		int difference = (int)containers_to_load.size() - ship_capacity;
		/* Sort the non-rejected containers so the ones unloaded first will be
		 * at the top of the tower
		 */
		std::sort(containers_to_load.begin(), containers_to_load.end() - difference,
				  CompareByRoute(ship_route, curr_port_idx, true));
	} else
	{
		/* Sort the containers so the ones unloaded first will be
		 * at the top of the tower
		 */
		std::sort(containers_to_load.begin(), containers_to_load.end(),
				  CompareByRoute(ship_route, curr_port_idx, true));
	}
}
