//
// Created by Owner on 19-May-20.
//

#include "TravelFiles.h"
TravelFiles :: TravelFiles(){}

TravelFiles::TravelFiles(string name, string &route_filepath, string &ship_plan_filepath,
						 vector<PortFile> &port_files) :name(name),route_filepath(route_filepath),
														ship_plan_filepath(ship_plan_filepath),port_files(port_files){}

void TravelFiles::get_route_string_vector(vector<string>& route_string_vector) {
	for (int i=0;i<(int)this->port_files.size();i++){
		route_string_vector.push_back(this->port_files[i].get_seaport_code());
	}
}

