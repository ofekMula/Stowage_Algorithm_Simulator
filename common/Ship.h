//
// Created by Owner on 12-May-20.
//

#ifndef EX2_ADV_CPP_SHIP_H
#define EX2_ADV_CPP_SHIP_H

#include <vector>
#include "common.h"
#include "ShipPlan.h"

using std::vector;


class Ship
{
	ShipPlan plan;
	vector<vector<vector<Container>>> containers;

public:
	explicit Ship(ShipPlan p_ship_plan);
	Ship();

	int get_length() const;
	int get_width() const;
	int get_height() const;

	int get_floor_start(int row, int col) const;

	const Container& get_container_at_idx(int row, int col, int floor_idx) const;
	const vector<Container>& get_container_tower(int row, int col) const;

	/**
	 * Performs an instruction.
	 * Updates ship and containers_at_port with the result
	 * @param instruction
	 * @param containers_at_port
	 */
	void perform_instruction(const Instruction& instruction,
							 vector<Container>& containers_at_port);

	/**
	 * Performs the vector of instructions.
	 * Updates ship and containers_at_port with the result
	 * @param instructions
	 * @param containers_at_port
	 */
	void perform_instructions(const vector<Instruction>& instructions,
							  vector<Container> &containers_at_port);

	/**
	 * Performs height to index translation
	 * @param row_idx
	 * @param col_idx
	 * @param floor_height
	 * @return The index of the container in the tower at row_idx, col_idx
	 * 			and floor_height in the container vector
	 */
	int floor_height_to_idx(int row_idx, int col_idx, int floor_height);

	/**
	 * Performs index to height translation
	 * @param row_idx
	 * @param col_idx
	 * @param floor_idx
	 * @return The height of the floor of container at row_idx, col_idx
	 * 			at index floor_idx in the container vector
	 */
	int floor_idx_to_height(int row_idx, int col_idx, int floor_idx);

	/**
	 * @param row
	 * @param col
	 * @param floor_height
	 * @return Container at position at the specified row, column and floor_height
	 */
	Container& get_container_at_pos(int row, int col, int floor_height);

	/**
	 * Adds a container to the top of the container tower at position specified
	 * by row and col
	 * @param row
	 * @param col
	 */
	void add_container_to_tower(int row, int col);

	/**
	 * @param row
	 * @param col
	 * @return The height (not index) of the container tower at row, col
	 * 			This is the height at which a new container can be placed.
	 */
	int get_container_tower_height(int row, int col) const;

	/**
	 * Updates row and col and floor_idx to indices of a container
	 * with id container_id.
	 * If no such container is on the ship, they are updated to -1
	 * @param container_id
	 * @param row
	 * @param col
	 */
	void get_container_idx_by_id(const string& container_id, int& row, int& col,
									int& floor_idx) const;

	/**
	 *
	 * @param container_id
	 * @return true if container with container_id is on the ship
	 */
	bool is_container_on_ship(const string& container_id) const;

	/**
	 * @return The number of containers that can be loaded to the ship
	 */
	int get_capacity() const;

	bool is_tower_full(int row, int col);

	// DEBUG FUNCTIONS
#ifdef DEBUG
	void print_containers_loaded();
	void print_container_destinations();
#endif // DEBUG FUNCTIONS

	int get_capacity_exclude_tower(int exclude_tower_row,
								   int exclude_tower_col) const;
};


#endif //EX2_ADV_CPP_SHIP_H
