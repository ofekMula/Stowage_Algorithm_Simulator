//
// Created by Ofek Mula on 14/05/2020.
//

#ifndef EX2_ADV_CPP_TRAVELSTATS_H
#define EX2_ADV_CPP_TRAVELSTATS_H

#define ERROR_SCORE -1
#define INIT_STAT_VAL 0
class TravelStats{
private:
    int total_operations;
    int num_errors;
public:
    TravelStats();
    TravelStats(int total_operations,int num_errors);
    int get_total_operations();
    int get_num_errors();
    int& get_total_operations_ref();
    int& get_num_errors_ref();
    void set_total_operations(int new_total_operations);
    void set_num_errors(int new_num_errors);


};

#endif //EX2_ADV_CPP_TRAVELSTATS_H
