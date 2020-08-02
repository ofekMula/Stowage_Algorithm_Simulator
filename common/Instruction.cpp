//
// Created by Owner on 17-May-20.
//

#include "Instruction.h"



Instruction::Instruction(Type p_type, int p_row_idx, int p_col_idx,
						 int p_floor_height, string p_container_id)
		: type(p_type), row_idx(p_row_idx), col_idx(p_col_idx),
		  floor_height(p_floor_height), container_id(p_container_id){}

Instruction::Instruction(Instruction::Type p_type,
			int p_row_idx, int p_col_idx, int p_floor_height,
			int move_dest_row, int move_dest_col, int p_move_dest_floor_height,
			string p_container_id)
			:
			type(p_type), row_idx(p_row_idx), col_idx(p_col_idx),
			floor_height(p_floor_height),
			move_dest_row_idx(move_dest_row),
			move_dest_col_idx(move_dest_col),
			move_dest_floor_height(p_move_dest_floor_height),
			container_id(p_container_id)
			{}


#ifdef DEBUG
void Instruction::print() const {
	std::cout << string_rep() << std::endl;
}
#endif

string Instruction::string_rep() const
{
    if (this->type==reject){
        return string(1, (char)type) + ", " + container_id;
    }
    if(this->type==move){
        return string(1, (char)type) + ", " + container_id + ", "+ to_string(floor_height)
	 	+ ", " + to_string(row_idx) + ", " + to_string(col_idx) + ", " + to_string(move_dest_floor_height)
        + ", " + to_string(move_dest_row_idx) + ", " + to_string(move_dest_col_idx);
    }
	return string(1, (char)type) + ", " + container_id + ", " + to_string(floor_height)
		   + ", " + to_string(row_idx) + ", " + to_string(col_idx);
}

