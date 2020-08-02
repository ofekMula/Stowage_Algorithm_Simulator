//
// Created by Owner on 19-May-20.
//

#ifndef EX2_ADV_CPP_PORT_H
#define EX2_ADV_CPP_PORT_H
#include "common.h"

class Port
{
private:
	string seaport_code;
	vector<Container> cargo;

public:
	const string &get_seaport_code() const {return seaport_code;}

	vector<Container> &get_cargo() {return cargo;}

	const vector<Container> &get_cargo() const {return cargo;}

	Port();

	Port(const string& p_seaport_code);

	Port(const string& p_seaport_code,vector<Container> cargo);

	/**
	 * @param p_containers
	 * @param container_id
	 * @return container with id of container_id if exists in p_containers
	 */
	static int get_container_idx_by_id(const vector<Container> &p_containers,
									   const string& container_id);

	int get_container_idx_by_id(const string& container_id);

	const Container& get_container_by_id(const string& container_id);

	/**
	 * @param code
	 * @return true if code is valid, false otherwise
	 */
	static bool verify_seaport_code(const string& code);
};


#endif //EX2_ADV_CPP_PORT_H
