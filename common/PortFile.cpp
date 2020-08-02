//
// Created by Owner on 19-May-20.
//

#include "PortFile.h"
PortFile::PortFile(){}

//PortFile::PortFile(const string &p_seaport_code) : seaport_code(p_seaport_code) {
//    this->file_path="";
//}
PortFile::PortFile(const string& seaport_code ,const string &file_path) :seaport_code(seaport_code),file_path(file_path) {
	file_name="";
	visit_number=0;
}
PortFile::PortFile(const string &p_seaport_code, const string &file_path, const string &file_name,
				   const int &visit_number): seaport_code(p_seaport_code), file_path(file_path),
											 file_name(file_name),visit_number(visit_number) {

}

const string &PortFile::get_seaport_code() const {
    return seaport_code;
}



const string &PortFile::get_file_path() const {
    return file_path;
}



const string &PortFile::get_file_name() const {
    return file_name;
}


int PortFile::get_visit_number() const {
    return visit_number;
}


