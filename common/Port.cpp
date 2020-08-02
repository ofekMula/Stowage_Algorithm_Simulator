//
// Created by Owner on 19-May-20.
//

#include "Port.h"


Port::Port() {

}

Port::Port(const string &p_seaport_code, vector<Container> cargo) :seaport_code(p_seaport_code),cargo(cargo) {

}

Port::Port(const string &p_seaport_code) : seaport_code(p_seaport_code){
	this->cargo=vector<Container>();
}

int Port::get_container_idx_by_id(const vector<Container> &p_containers,
								  const string& container_id)
{
	for (int i = 0; i < (int)p_containers.size(); i++)
	{
		if (p_containers[i].get_id() == container_id)
			return i;
	}
	return NOT_FOUND;
}

int Port::get_container_idx_by_id(const string &container_id) {
	return get_container_idx_by_id(cargo, container_id);
}

const Container& Port::get_container_by_id(const string &container_id)
{
	return cargo[get_container_idx_by_id(container_id)];
}

bool Port::verify_seaport_code(const string &code) {
	if (code.size()!=SEAPORT_CODE_LEN){

		return false;
	}
	for (int i=0;i<(int)code.size();i++){
		if ((code.at(i)<VALID_MIN_LIMIT1_CHAR|| code.at(i)>VALID_MAX_LIMIT1_CHAR) &&
			(code.at(i)<VALID_MIN_LIMIT2_CHAR || code.at(i)>VALID_MAX_LIMIT2_CHAR)){
			return false;
		}
	}
	return true;
}
