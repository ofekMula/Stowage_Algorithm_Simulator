//
// Created by Meir Gavish on 28-Mar-20.
//

#ifndef EX1_ADV_CPP_STOWAGEALGORITHM_H
#define EX1_ADV_CPP_STOWAGEALGORITHM_H

#include <vector>
#include <iostream>
#include <unordered_set>
#include "../common/common.h"
#include "../common/Ship.h"
#include "../common/FileParser.h"
#include "../interfaces/WeightBalanceCalculator.h"
#include "../interfaces/AlgorithmRegistration.h"

#define NO_SOURCE -1 // Used in get_idx_of_best_tower_for_move

using std::vector;

class Instruction;

class StowageAlgorithm : public AbstractAlgorithm {
	/**
	 * Base class for algorithms - contains all common implementations
	 * of the algorithms in order to have both algorithms inherit
	 * the same base class and not have them inherit one from the other.
	 */
protected:
	Ship ship;
	string curr_port;
	int curr_port_idx;
	WeightBalanceCalculator balance_calculator;
	vector<string> ship_route;

	bool ship_initialized;
	bool route_initialized;

	class CompareByRoute
	{
	/**
	 * 	Function object to sort containers by route
	 * 	Used in load containers to load them in order of route so
	 * 	rejected containers are furthest in route
	 */

		const vector<string>& ship_route;
		int alg_curr_port_idx;

		/* Whether a container with a farther destination is considered smaller
		 * in the comparison
		 */
		bool far_first;
	public:

		inline CompareByRoute(const vector<string> &p_ship_route,
							  int p_curr_port_idx,
							  bool p_far_first)
			:
			ship_route(p_ship_route),
			alg_curr_port_idx(p_curr_port_idx),
			far_first(p_far_first) {}
		/**
		 * @param container_a
		 * @param container_b
		 * @return true if @param container_a is greater than @param container_b
		 */
		bool operator()(const Container& container_a, const Container& container_b);
	};


	 /**
	 *
	 * @param row
	 * @param col
	 * @param floor_height
	 * Adds to instructions the instructions to remove all containers positioned
	 * above and on floor at floor_height.
	 * Afterwards all removed containers are placed in the removed_containers vector
	 */
	virtual void add_instructions_remove_all_above_containers(int row, int col,
															  int floor_height,
															  vector<Instruction> &instructions,
															  vector<Container> &removed_containers);

	/**
	 * Adds instructions to unload containers at row and col
	 * to vector instructions. Updates containers at port
	 * @param row
	 * @param col
	 * @param instructions
	 */
	virtual int add_instructions_to_unload_containers(int row, int col,
													  vector<Instruction> &instructions);
	/**
	 * Adds load instructions for all containers in removed_containers
	 * whose dest_port_code is not curr_port
	 * @param row
	 * @param col
	 * @param instructions
	 * @param removed_containers
	 */
	void add_instructions_return_containers_not_for_this_port(int row, int col,
															  vector<Instruction> &instructions,
															  const vector<Container> &removed_containers);

	/**
	 * Returns an instruction to load the container.
	 * The instruction indices will be valid and in the available height.
	 * The instruction will be a reject instruction if any of the following
	 * 		The ship is full
	 * 		container.dest_port_code is not in any of the next ports in route
	 */
	virtual int get_next_load_instruction(const Container &container,
										  Instruction &instruction,
										  const Ship &ship_before_unload);

	/**
	 *
	 * @param container
	 * @return true if container.dest_port_code is in the next ports of ship route
	 * false otherwise.
	 */
	bool container_dest_in_next_route_ports(const Container &container);

public:
	virtual ~StowageAlgorithm();
	StowageAlgorithm();
	/**
	 * Performs the algorithm for a file and outputs the instructions to a file
	 * @param input_full_path_and_file_name - A path to a file in port containers format
	 * @param output_full_path_and_file_name - A path to a file in instructions format
	 * @return error flags defined in AlgErrorCodes
	 */
	virtual int getInstructionsForCargo(
						const std::string& input_full_path_and_file_name,
						const std::string& output_full_path_and_file_name) override;

	/**
	 * This function overloads the file one, implementing the inner logic
	 * using the parsed data structures.
	 * @param containers_to_load - a vector of the containers to load
	 * @param res_instructions - a vector of instructions updated with
	 * 							 the instructions to load the containers in
	 * 							 @param containers_to_load
	 * 							 and unload the containers in the current port
	 * @return error flags defined in AlgErrorCodes
	 */
	virtual int
	getInstructionsForCargo(vector<Container> &containers_to_load,
							vector<Instruction> &res_instructions);

	virtual void next_port();

	int readShipPlan(const string &full_path_and_file_name) override;

	int readShipRoute(const string &full_path_and_file_name) override;

	int
	setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override;

	bool is_last_port();

	/**
	 * Assumes container.get_id() is a valid ID string
	 * that may have ISO problems
	 * Checks everything
	 * @param container
	 * @param ship_before_unload
	 * @return error flags defined in AlgErrorCodes
	 */
	int check_container_errors_before_load(const Container &container,
										   const Ship &ship_before_unload) const;

	int
	add_instructions_to_load_containers(vector<Container> &containers_to_load,
										vector<Instruction> &res_instructions,
										const Ship &ship_before_unload);

	/**
	 * This function is used to find an optimal tower to place a new container
	 * We want minimize the number of containers headed to different
	 * ports in the same tower.
	 * @param row
	 * @param col
	 * @param than_seaport_code
	 * @return Number of different destination ports of containers in tower
	 * 			in row, col that are not equal to than_seaport_code
	 *
	 */
	int num_ports_in_tower_other_than(int row, int col,
									  const string &than_seaport_code);
	/**
	 * Updates @param best_row and @param best_col with the indices of the tower of
	 * the best load for @param container
	 *
	 * Currently finds the non-full tower with the least number of containers
	 * with dest ports other than @param container
	 * @param best_row
	 * @param best_col
	 * @param container
	 */
	void get_idx_of_best_tower_for_load(int &best_row, int &best_col,
										const Container &container);

	/**
	 * Same as @function get_idx_of_best_tower_for_load
	 * only for move operation. It receives source indices so it knows not
	 * to return the same indices. Use NO_SOURCE to have the parameter ignored.
	 * @param best_row
	 * @param best_col
	 * @param source_row
	 * @param source_col
	 * @param container
	 */
	void
	get_idx_of_best_tower_for_move(int &best_row, int &best_col,
								   int source_row, int source_col,
								   const Container &container);

	/**
	 * Sorts @param containers_to_load in the best way to load the containers
	 * to the ship.
	 */
	virtual void order_containers_for_load(vector<Container> &containers_to_load) const = 0;


};


#endif //EX1_ADV_CPP_STOWAGEALGORITHM_H
