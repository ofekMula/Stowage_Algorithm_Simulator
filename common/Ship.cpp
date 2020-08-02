//
// Created by Owner on 12-May-20.
//

#include "Ship.h"




Ship::Ship() = default;

Ship::Ship(ShipPlan p_ship_plan) : plan(p_ship_plan)
{
	this->containers = vector<vector<vector<Container>>>(plan.length);
	for (int i = 0; i < p_ship_plan.length; i++)
	{
		containers[i] = vector<vector<Container>>(plan.width);
		for (int j = 0; j < p_ship_plan.width; j++)
		{
			containers[i][j] = vector<Container>();
		}
	}
}

Container& Ship::get_container_at_pos(int row, int col, int floor_height)
{
	return containers[row][col][floor_height_to_idx(row, col, floor_height)];
}


int Ship::get_container_tower_height(int row, int col) const {
	return containers[row][col].size() + this->plan.floor_start[row][col];
}

void Ship::perform_instruction(const Instruction& instruction,
							   vector<Container>& containers_at_port)
{
	if (instruction.get_type() == Instruction::reject)
	{
		return;
	}

	int row_idx = instruction.get_row_idx();
	int col_idx = instruction.get_col_idx();
	int dest_row_idx = instruction.get_move_dest_row_idx();
	int dest_col_idx = instruction.get_move_dest_col_idx();
	int floor_height = instruction.get_floor_height();
	vector<Container>& container_tower = containers[row_idx][col_idx];
	int container_idx = 0;

	switch(instruction.get_type())
	{
		case Instruction::unload:
			containers_at_port.push_back(get_container_at_pos(row_idx, col_idx, floor_height));
			container_tower.pop_back();
			break;
		case Instruction::load:
			container_idx = Port::get_container_idx_by_id(containers_at_port,
															instruction.get_container_id());
			this->containers[row_idx][col_idx].push_back(containers_at_port[container_idx]);
			containers_at_port.erase(containers_at_port.begin() + container_idx);
			break;
		case Instruction::move:
			containers[dest_row_idx][dest_col_idx].push_back(container_tower.back());
			container_tower.pop_back();
			break;
		case Instruction::reject:
			break;

	}
}

int Ship::floor_height_to_idx(int row_idx, int col_idx, int floor_height) {
	return floor_height - plan.floor_start[row_idx][col_idx];
}

int Ship::floor_idx_to_height(int row_idx, int col_idx, int floor_idx) {
	return floor_idx + plan.floor_start[row_idx][col_idx];
}


void Ship::perform_instructions(const vector<Instruction>& instructions,
								vector<Container> &containers_at_port)
{
	for (Instruction instruction: instructions)
	{
		perform_instruction(instruction, containers_at_port);
	}
}

int Ship::get_floor_start (int row, int col) const
{
	return plan.get_floor_start(row, col);
}

int Ship::get_width() const{
	return plan.get_width();
}

int Ship::get_height() const {
	return plan.get_height();
}

int Ship::get_length() const {
	return plan.get_length();
}

const Container &Ship::get_container_at_idx(int row, int col, int floor_idx) const
{
	return containers[row][col][floor_idx];
}

const vector<Container> &Ship::get_container_tower(int row, int col) const
{
	return containers[row][col];
}

void Ship::get_container_idx_by_id(const string &container_id, int &row, int &col,
									int &floor_idx) const
{
	row = NOT_FOUND; col = NOT_FOUND; floor_idx = NOT_FOUND;
	for (int curr_row = 0; curr_row < plan.get_length(); curr_row++)
	{
		for (int curr_col = 0; curr_col < plan.get_width(); curr_col++)
		{
			for (int curr_floor_idx = 0;
				 curr_floor_idx < (int) containers[curr_row][curr_col].size();
				 curr_floor_idx++)
			{
				if (containers[curr_row][curr_col][curr_floor_idx].get_id() == container_id)
				{
					row = curr_row;
					col = curr_col;
					floor_idx = curr_floor_idx;
					return;
				}
			}
		}
	}
}

bool Ship::is_container_on_ship(const string &container_id) const {
	int row, col, floor_idx;
	get_container_idx_by_id(container_id, row, col, floor_idx);
	return !(row == NOT_FOUND || col == NOT_FOUND || floor_idx == NOT_FOUND);
}

int Ship::get_capacity() const
{
	return get_capacity_exclude_tower(-1, -1);
}

int Ship::get_capacity_exclude_tower(int exclude_tower_row, int exclude_tower_col) const
{
	int capacity = 0;
	for (int row = 0; row < get_length(); row++)
	{
		for (int col = 0; col < get_width(); col++)
		{
			if (!(col == exclude_tower_col && row == exclude_tower_row))
			{
				capacity += get_height() - get_container_tower_height(row, col);
			}
		}
	}

	return capacity;
}

bool Ship::is_tower_full(int row, int col)
{
	return get_container_tower_height(row, col) >= get_height();
}

// DEBUG FUNCTIONS
#ifdef DEBUG

void Ship::print_containers_loaded()
{
	for (int i = 0; i < plan.length; i++)
	{
		for (int j = 0; j < plan.width; j++)
		{
			std::cout << "Containers at " << i << ", " << j << std::endl;
			for (int floor_height = 0; floor_height < (int)get_container_tower_height(i, j); floor_height++)
			{
				if (floor_height < plan.floor_start[i][j])
				{
					std::cout << "||||||||||| ";
				} else
				{
					std::cout << get_container_at_pos(i, j, floor_height).get_id() << " ";
				}
			}
			std::cout << std::endl;
		}
	}
}

void Ship::print_container_destinations() {
	for (int i = 0; i < plan.length; i++)
	{
		for (int j = 0; j < plan.width; j++)
		{
			std::cout << "Container destinations at " << i << ", " << j << std::endl;
			for (int floor_height = 0; floor_height < (int)get_container_tower_height(i, j); floor_height++)
			{
				if (floor_height < plan.floor_start[i][j])
				{
					std::cout << "||||| ";
				} else
				{
					std::cout << get_container_at_pos(i, j, floor_height).get_dest_port() << " ";
				}
			}
			std::cout << std::endl;
		}
	}
}
#endif
