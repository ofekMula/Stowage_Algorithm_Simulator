//
// Created by Owner on 19-May-20.
//

#ifndef EX2_ADV_CPP_TRAVELFILES_H
#define EX2_ADV_CPP_TRAVELFILES_H
#include <string>
#include <vector>
#include "PortFile.h"

using std::vector;
using std::string;

class TravelFiles
{
	friend class FileParser;
	string name;

	string route_filepath;
	string ship_plan_filepath;
	vector<PortFile> port_files;

public:
	TravelFiles(string name, string& routes_filepath, string& ship_plan_filepath, vector<PortFile>& port_files);
	void get_route_string_vector(vector<string> &route_string_vector);
	TravelFiles();

	inline const string &get_name() const {return name;}

	inline const string &get_route_filepath() const {return route_filepath;}

	inline const string &get_ship_plan_filepath() const {return ship_plan_filepath;}

	inline const vector<PortFile> &get_port_files() const {return port_files;}
};



#endif //EX2_ADV_CPP_TRAVELFILES_H
