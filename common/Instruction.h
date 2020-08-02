//
// Created by Owner on 17-May-20.
//

#ifndef EX2_ADV_CPP_INSTRUCTION_H
#define EX2_ADV_CPP_INSTRUCTION_H

#include <string>
#include <iostream>

using std::to_string;
using std::string;

class Instruction
{
public:
	enum Type{load='L', unload='U', move = 'M', reject='R'};
private:
	Type type;
	int row_idx;
	int col_idx;

	int floor_height;
	/* Convention: when referring to floors in their actual height in the ship
	 * call them "floor height". When referring to indices in the vector of
	 * Containers in the shop call them "floor index"
	 * See translation functions in class Ship.
	 */

	int move_dest_row_idx;
	int move_dest_col_idx;
	int move_dest_floor_height;

	string container_id;

public:
	Instruction() = default;
	Instruction(Instruction::Type p_type, int p_row_idx, int p_col_idx,
				int p_floor_height, string p_container_id);

	Instruction(Instruction::Type p_type,
				int p_row_idx, int p_col_idx, int p_floor_height,
				int move_dest_row, int move_dest_col, int p_move_dest_floor_height,
				string p_container_id);
	string string_rep() const;

	// Getters and setters for all fields. This is stupid.
	inline Type get_type() const {return type;}
	inline void set_type(Type p_type) {Instruction::type = p_type;}
	inline int get_row_idx() const {return row_idx;}
	inline void set_row_idx(int rowIdx) { row_idx = rowIdx;}
	inline int get_col_idx() const {return col_idx;}
	inline void set_col_idx(int p_col_idx) { col_idx = p_col_idx;}
	inline int get_floor_height() const {return floor_height;}
	inline void set_floor_height(int p_floor_height) { floor_height = p_floor_height;}
	inline int get_move_dest_col_idx() const {return move_dest_col_idx;}
	inline void set_move_dest_col_idx(int p_move_dest_col_idx) {move_dest_col_idx = p_move_dest_col_idx;}
	inline int get_move_dest_row_idx() const {return move_dest_row_idx;}
	inline void set_move_dest_row_idx(int p_move_dest_row_idx) {move_dest_row_idx = p_move_dest_row_idx;}
	inline int get_move_dest_floor_height() const {return move_dest_floor_height;}
	inline void set_move_dest_floor_height(int p_move_dest_floor_height) { move_dest_floor_height = p_move_dest_floor_height;}
	inline const string &get_container_id() const {return container_id;}
	inline void set_container_id(const string &p_container_id) {container_id = p_container_id;}

#ifdef DEBUG
	void print() const;
#endif
};



#endif //EX2_ADV_CPP_INSTRUCTION_H
