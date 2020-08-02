//
// Created by Ofek Mula on 04/06/2020.
//

#include "ErrorFilesMutex.h"


std::mutex& ErrorsFilesMutex::get_mutex_by_file_name(const std::string &file_name)  {
    if(file_name==FATAL_ERRORS_FILE){
        return this->fatal_error_mutex;
    }
    if(file_name==TRAVEL_ERRORS_FILE){
        return this->travel_error_mutex;
    }
    if(file_name==BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE){
        return this->bad_algorithm_error_mutex;
    }
    if(file_name==IGNORABLE_ERRORS_FILE){
        return this->ignorable_error_mutex;
    }
    else{
        return this->error_code_mutex;
    }

}

