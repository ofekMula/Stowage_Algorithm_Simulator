//
// Created by Owner on 19-May-20.
//

#ifndef EX2_ADV_CPP_PORTFILE_H
#define EX2_ADV_CPP_PORTFILE_H

#include <string>
using std::string;

class PortFile
{
private:
    string seaport_code;
    string file_path;	// Full path including name
    string file_name;	// Just the file name
    int visit_number;

public:
	PortFile();
    const string &get_seaport_code() const;
    const string &get_file_path() const;
    const string &get_file_name() const;
    int get_visit_number() const;

	//PortFile(const string& p_seaport_code);
	PortFile(const string& seaport_code,const string &file_path);
	PortFile(const string& p_seaport_code, const string& file_path,const string& file_name,const int& visit_number );

};


#endif //EX2_ADV_CPP_PORTFILE_H
