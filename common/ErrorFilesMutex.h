//
// Created by Ofek Mula on 04/06/2020.
//

#include <mutex>

#ifndef EX3_ADV_CPP_ERRORSFILESMUTEX_H
#define EX3_ADV_CPP_ERRORSFILESMUTEX_H

#endif //EX3_ADV_CPP_ERRORSFILESMUTEX_H


#define FATAL_ERRORS_FILE "fatal_errors.txt"
#define TRAVEL_ERRORS_FILE "travel_errors.txt"
#define BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE "bad_algorithm_behavior_errors.txt"
#define IGNORABLE_ERRORS_FILE "ignorable_errors.txt"
#define ALGORITHM_ERRORS_FILES "Error code returned by the Algorithm.txt"
class ErrorsFilesMutex{
private:
    std::mutex fatal_error_mutex;
    std::mutex ignorable_error_mutex;
     std::mutex bad_algorithm_error_mutex;
     std::mutex error_code_mutex;
    std::mutex travel_error_mutex;

public :
    std::mutex & get_mutex_by_file_name(const std::string &file_name) ;
};