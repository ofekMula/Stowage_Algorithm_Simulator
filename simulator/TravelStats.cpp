//
// Created by Ofek Mula on 14/05/2020.
//

#include "TravelStats.h"

TravelStats::TravelStats() : total_operations(INIT_STAT_VAL),num_errors(INIT_STAT_VAL) {}

TravelStats::TravelStats(int totalOperations,int num_errors) : total_operations(totalOperations),num_errors(num_errors) {}

int TravelStats::get_total_operations() {
    return this->total_operations;
}
int TravelStats::get_num_errors() {
    return this->num_errors;
}

void TravelStats::set_total_operations(int new_total_operations) {
    if(this->total_operations>=0) {
        this->total_operations = new_total_operations;
    }
}

void TravelStats::set_num_errors(int new_num_errors) {
    this->num_errors=new_num_errors;
    this->total_operations=ERROR_SCORE;


}

int& TravelStats::get_total_operations_ref() {
    return this->total_operations;
}




