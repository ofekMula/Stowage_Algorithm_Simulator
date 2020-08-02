//
// Created by Owner on 28-Mar-20.
//


#include <climits>
#include <algorithm>
#include "StowageAlgorithm.h"


StowageAlgorithm::StowageAlgorithm()
	:ship_initialized(false), route_initialized(false){}

StowageAlgorithm::~StowageAlgorithm() {}


void StowageAlgorithm::next_port()
{
	curr_port_idx++;
	if (curr_port_idx < (int)ship_route.size())
	{
		curr_port = ship_route[curr_port_idx];
	}
}

int StowageAlgorithm::readShipPlan(const string &full_path_and_file_name)
{
	int errors = SUCCESS;
	ShipPlan shipPlan;
	errors |= FileParser::read_ship_plan_file(full_path_and_file_name, shipPlan);
	if (!AlgErrorCodes::is_cannot_run_error(errors))
	{
		this->ship = Ship(shipPlan);
		ship_initialized = true;
	}
	else
	{
		ship_initialized = false;
	}
	return errors;
}

int StowageAlgorithm::readShipRoute(const string &full_path_and_file_name)
{
	int errors = SUCCESS;
	ship_route = vector<string>();
    errors = FileParser::read_route_file(full_path_and_file_name, ship_route);
	if (!AlgErrorCodes::is_cannot_run_error(errors))
	{
		this->curr_port = ship_route[0];
		this->curr_port_idx = 0;
		route_initialized = true;
	}
	else
	{
		route_initialized = false;
	}

	return errors;
}

int StowageAlgorithm::setWeightBalanceCalculator(
		WeightBalanceCalculator &calculator)
{
	balance_calculator = calculator;
	return SUCCESS;
}


int StowageAlgorithm::getInstructionsForCargo(
										vector<Container> &containers_to_load,
										vector<Instruction> &res_instructions)
{
	// This function should have a single return statement!
	// This is because of the call to next_port()
	int alg_errors = SUCCESS;

	// Copy ship to verify containers wasn't on ship before unload
	Ship ship_before_unload = ship;

	// Unload containers for port
	for (int row = 0; row < this->ship.get_length(); row++)
	{
		for (int col = 0; col < this->ship.get_width(); col++)
		{
			alg_errors |= add_instructions_to_unload_containers(row, col, res_instructions);
		}
	}
	alg_errors |= add_instructions_to_load_containers(containers_to_load,
													 res_instructions,
													 ship_before_unload);

#ifdef DEBUG
	std::cout << "Instructions performed returned by algorithm at port "
			  << curr_port << std::endl;
	for (const Instruction& instruction: res_instructions)
	{
		instruction.print();
	}
//	std::cout << "Algorithm ship after instructions performed at port " << curr_port <<  std::endl;
//	ship.print_containers_loaded();
#endif
	next_port();
	return alg_errors;
}

int StowageAlgorithm::add_instructions_to_load_containers(
										vector<Container> &containers_to_load,
										vector<Instruction> &res_instructions,
										const Ship &ship_before_unload)
{
	int alg_errors = SUCCESS;

	// To not change containers during iteration
	vector<Container> containers_copy = containers_to_load;

	if(is_last_port() && !containers_to_load.empty())
	{
		alg_errors |= AlgErrorCodes::CONTAINERS_LAST_PORT_HAS_CONTAINERS;
	}
	else
	{
		order_containers_for_load(containers_to_load);
		vector<Container> loaded_containers; // To check duplicates
		for (const Container& container : containers_to_load)
		{
			Instruction instruction;
			if (container.get_dest_port() == curr_port)
			{
				// Reject container with same source and dest
				instruction = Instruction(Instruction::reject, 0, 0, 0, container.get_id());
				alg_errors |= AlgErrorCodes::CONTAINER_ID;
			}
			else if (Port::get_container_idx_by_id(loaded_containers, container.get_id()) != NOT_FOUND)
			{
				// Reject containers already loaded
				instruction = Instruction(Instruction::reject, 0, 0, 0, container.get_id());
				alg_errors |= AlgErrorCodes::CONTAINER_DUPLICATE_ID_ON_PORT;
			}
			else
			{
				alg_errors |= get_next_load_instruction(container,
														instruction,
														ship_before_unload);
			}

			res_instructions.push_back(instruction);
			ship.perform_instruction(instruction, containers_copy);

			if (instruction.get_type() != Instruction::reject)
			{
				loaded_containers.push_back(container);
			}
		}
	}
	return alg_errors;
}

int StowageAlgorithm::getInstructionsForCargo(
		const string &input_full_path_and_file_name,
		const string &output_full_path_and_file_name)
{
	int errors = SUCCESS;
	if (!route_initialized)
	{
		errors |= AlgErrorCodes::ROUTE_READ_FAIL_OR_FILE_EMPTY;
	}
	if (!ship_initialized)
	{
		errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
	}
	if (errors != SUCCESS)
	{
		return errors;
	}

	vector<Instruction> res_instructions;
	Port port;
	errors |= FileParser::read_Port_file(input_full_path_and_file_name, port);
	if (AlgErrorCodes::is_cannot_run_error(errors))
	{
		return errors;
	}

	errors = errors | getInstructionsForCargo(port.get_cargo(), res_instructions);

	FileParser::create_file(output_full_path_and_file_name);
	for (const Instruction& instruction : res_instructions)
	{
		FileParser::write_crane_instruction_to_file(instruction,
													output_full_path_and_file_name);

	}

	return errors;
}

int
StowageAlgorithm::check_container_errors_before_load(const Container &container,
													 const Ship &ship_before_unload) const
{
	int errors = SUCCESS;

	if (ship_before_unload.is_container_on_ship(container.get_id()))
	{
		errors |= AlgErrorCodes::CONTAINER_ID_ALREADY_ON_SHIP;
	}
	if (!container.verify_id())
	{
		errors |= AlgErrorCodes::CONTAINER_ID_ISO;
	}
	if (container.get_weight() == BAD_WEIGHT)
	{
		errors |= AlgErrorCodes::CONTAINER_WEIGHT;
	}
	if(!Port::verify_seaport_code(container.get_dest_port()))
	{
		errors |= AlgErrorCodes::CONTAINER_PORT_DEST;
	}

	return errors;
}


void
StowageAlgorithm::add_instructions_return_containers_not_for_this_port(int row,
																	   int col,
																	   vector<Instruction> &instructions,
																	   const vector<Container> &removed_containers)
{
	vector<Container> removed_containers_copy = removed_containers;
	for (const Container& container : removed_containers)
	{
		if (container.get_dest_port() != curr_port)
		{
			Instruction instruction = Instruction(Instruction::load, row, col,
												  ship.get_container_tower_height(row, col),
												  container.get_id());

			if  (balance_calculator.tryOperation
					(
							(char)instruction.get_type(), container.get_weight(),
							instruction.get_col_idx(), instruction.get_row_idx()
					)
				 == WeightBalanceCalculator::APPROVED
			 	)
			{
				instructions.push_back(instruction);
				ship.perform_instruction(instruction, removed_containers_copy);
			}
		}
	}
}

int StowageAlgorithm::num_ports_in_tower_other_than(int row, int col,
													const string& than_seaport_code)
{
	std::unordered_set<string> container_dest_port_set;
	for (const Container& container: ship.get_container_tower(row, col))
	{
		if (container.get_dest_port() != than_seaport_code)
		{
			container_dest_port_set.insert(container.get_dest_port());
		}
	}

	return container_dest_port_set.size();
}

void
StowageAlgorithm::get_idx_of_best_tower_for_load(int &best_row, int &best_col,
												 const Container &container)
{
	get_idx_of_best_tower_for_move(best_row, best_col, NO_SOURCE, NO_SOURCE, container);
}

void
StowageAlgorithm::get_idx_of_best_tower_for_move(int &best_row, int &best_col,
												 int source_row, int source_col,
												 const Container &container)
{
	best_row = NOT_FOUND;
	best_col = NOT_FOUND;
	int best_tower_num_ports = INT_MAX;

	for (int curr_row = 0; curr_row < ship.get_length(); curr_row++)
	{
		for (int curr_col = 0; curr_col < ship.get_width(); curr_col++)
		{
			if (!ship.is_tower_full(curr_row, curr_col)
				&& !(curr_row == source_row
					 && curr_col == source_col))
			{
				int curr_tower_num_ports = num_ports_in_tower_other_than(curr_row,
																		 curr_col,
																		 container.get_dest_port());
				if (curr_tower_num_ports < best_tower_num_ports)
				{
					best_row = curr_row;
					best_col = curr_col;
					best_tower_num_ports = curr_tower_num_ports;
				}
			}
		}
	}
}

bool
StowageAlgorithm::container_dest_in_next_route_ports(const Container &container)
{
	for (int i = curr_port_idx + 1; i < (int) ship_route.size(); i++)
	{
		if (container.get_dest_port() == ship_route[i])
			return true;
	}

	return false;
}

bool StowageAlgorithm::is_last_port() {
	return (int)ship_route.size() - 1 == curr_port_idx;
}

int StowageAlgorithm::add_instructions_to_unload_containers(int row, int col,
															vector<Instruction> &instructions) {
	int errors = SUCCESS;
	for (int floor_height = ship.get_floor_start(row, col);
		 floor_height < ship.get_container_tower_height(row, col); floor_height++)
	{
		if(ship.get_container_at_pos(row, col, floor_height).get_dest_port() == this->curr_port)
		{
			vector<Container> removed_containers;
			add_instructions_remove_all_above_containers(row, col, floor_height,
														 instructions,
														 removed_containers);
			add_instructions_return_containers_not_for_this_port(row, col,
																 instructions,
																 removed_containers);
			break;
		}
	}

	return errors;
}

void
StowageAlgorithm::add_instructions_remove_all_above_containers(int row, int col,
															   int floor_height,
															   vector<Instruction> &instructions,
															   vector<Container> &removed_containers)
{
	int other_tower_capacity = ship.get_capacity_exclude_tower(row, col);
	int top_height = this->ship.get_container_tower_height(row, col) - 1;

	for (int curr_height = top_height;
		 curr_height >= floor_height; curr_height--)
	{
		auto container = this->ship.get_container_at_pos(row, col, curr_height);
		const auto& container_id = container.get_id();

		Instruction instruction;
		int moved_containers = top_height - curr_height;
		if (moved_containers < other_tower_capacity
			&& container.get_dest_port() != curr_port)
		{
			int move_dest_row;
			int move_dest_col;
			get_idx_of_best_tower_for_move(move_dest_row, move_dest_col,
										   row, col, container);
			int move_dest_height = ship.get_container_tower_height(move_dest_row, move_dest_col);
			instruction = Instruction(Instruction::move,
									  row, col, curr_height,
									  move_dest_row, move_dest_col,
									  move_dest_height,
									  container_id);
		}
		else
		{
			instruction = Instruction(Instruction::unload,
									  row, col, curr_height,
									  container_id);
		}


		if ( balance_calculator.tryOperation
				(
						(char)instruction.get_type(), container.get_weight(),
						instruction.get_col_idx(), instruction.get_row_idx()
				)
			 == WeightBalanceCalculator::APPROVED)
		{
			instructions.push_back(instruction);
			ship.perform_instruction(instruction, removed_containers);
		}
	}
}

int StowageAlgorithm::get_next_load_instruction(const Container &container,
												Instruction &instruction,
												const Ship &ship_before_unload)
{
	int errors = SUCCESS;

	int row_idx = 0;
	int col_idx = 0;
	int floor_height = this->ship.get_container_tower_height(row_idx, col_idx);

	errors = check_container_errors_before_load(container, ship_before_unload);
	if (errors != SUCCESS || !container_dest_in_next_route_ports(container))
	{						 // There's no error code for container dest not in next route ports
		instruction = instruction = Instruction(Instruction::reject, row_idx, col_idx, floor_height, container.get_id());
		return errors;
	}

	get_idx_of_best_tower_for_load(row_idx, col_idx, container);

	if (row_idx == NOT_FOUND || col_idx == NOT_FOUND)
	{
		// Call to get_idx_of_best_tower_for_load returns NOT_FOUND only if
		// ship is full
		instruction = Instruction(Instruction::reject, 0, 0, 0, container.get_id());
		errors |= AlgErrorCodes::CONTAINERS_EXCEEDS_CAPACITY;
		return errors;
	}
	floor_height = this->ship.get_container_tower_height(row_idx, col_idx);
	instruction = Instruction(Instruction::load, row_idx, col_idx, floor_height, container.get_id());

	return errors;
}

bool StowageAlgorithm::CompareByRoute::operator()(const Container &container_a,
												  const Container &container_b)
{
	if (container_a.get_dest_port() == container_b.get_dest_port())
	{
		// To implement strict weak ordering, compare(A, A) = false
		return false;
	}
	for (int i = alg_curr_port_idx + 1; i < (int)ship_route.size(); i++)
	{
		if (ship_route[i] == container_b.get_dest_port())
		{
			return far_first;
		}
		if (ship_route[i] == container_a.get_dest_port())
		{
			return !far_first;
		}
	}

	return false; // Shouldn't get here, a container that's not in route shouldn't be loaded
}
