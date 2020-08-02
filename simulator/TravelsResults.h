//
// Created by Ofek Mula on 14/05/2020.
//

#include <map>
#include "TravelStats.h"

#ifndef EX2_ADV_CPP_TRAVELSRESULTS_H
#define EX2_ADV_CPP_TRAVELSRESULTS_H

#define TOTAL_OPERATIONS_FLAG true
#define NUM_ERRORS_FLAG false
#define DEFAULT_STAT_VAL 0
#define ERROR_SCORE -1
#define INIT_SUM 0

class TravelsResults {
private:
    std::map<std::string, TravelStats> travel_name_to_stats_map;
    int sum_operations;
    int sum_errors;
public:
    std::map<std::string, TravelStats> &get_travel_name_stats_map() ;

    TravelsResults();
    TravelsResults(std::map<std::string, TravelStats>travel_name_to_stats_map );
    void add_travel_stats(std::string travel_name,TravelStats& stats);

    void add_travel_stats(std::string travel_name);

    void add_travel_stats(std::string travel_name, int total_operations, int num_errors);

    void add_travel_stats(std::string travel_name, int stat, bool total_operations_flag);

    int getSumOperations() const;

    int getSumErrors() const;

    bool compare(const TravelsResults &b) const;

    void update_sum_fields();
};

#endif //EX2_ADV_CPP_TRAVELSRESULTS_H
