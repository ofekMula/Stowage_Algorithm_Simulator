//
// Created by Ofek Mula on 14/05/2020.
//

#include "TravelsResults.h"

TravelsResults::TravelsResults(): sum_operations(INIT_SUM),sum_errors(INIT_SUM){}

TravelsResults::TravelsResults(std::map<std::string, TravelStats> travel_name_to_stats_map) : travel_name_to_stats_map(travel_name_to_stats_map),
sum_operations(INIT_SUM),sum_errors(INIT_SUM){}

void TravelsResults::add_travel_stats(std::string travel_name) {
    this->travel_name_to_stats_map[travel_name]=TravelStats();
    //update_sum_fields();
}
void TravelsResults::add_travel_stats(std::string travel_name,TravelStats &stats) {
    this->travel_name_to_stats_map[travel_name]=stats;
    //update_sum_fields();
}

void TravelsResults::add_travel_stats(std::string travel_name,int total_operations,int num_errors) {
    this->travel_name_to_stats_map[travel_name]=TravelStats(total_operations,num_errors);
   // update_sum_fields();
}


void TravelsResults::add_travel_stats(std::string travel_name,int stat,bool total_operations_flag)
{
    if(this->travel_name_to_stats_map.find(travel_name)==this->travel_name_to_stats_map.end())
    {
        if(total_operations_flag){
            this->travel_name_to_stats_map[travel_name]=TravelStats(stat, DEFAULT_STAT_VAL);
        }
        else{
            this->travel_name_to_stats_map[travel_name]=TravelStats(ERROR_SCORE, stat);
        }
    }
    else{
        if(total_operations_flag){
            this->travel_name_to_stats_map[travel_name].set_total_operations(stat);

        }
        else {
            int old_num_errors=this->travel_name_to_stats_map[travel_name].get_num_errors();
            this->travel_name_to_stats_map[travel_name].set_num_errors(stat+old_num_errors);
        }
    }
    //update_sum_fields();
}

std::map<std::string, TravelStats> &TravelsResults::get_travel_name_stats_map()  {
    return travel_name_to_stats_map;
}

int TravelsResults::getSumOperations() const {
    return sum_operations;
}


int TravelsResults::getSumErrors() const {
    return sum_errors;
}


void TravelsResults::update_sum_fields() {
    this->sum_errors=INIT_STAT_VAL;
    this->sum_operations=INIT_STAT_VAL;
    TravelStats stats;
    for (auto it = this->travel_name_to_stats_map.begin(); it != this->travel_name_to_stats_map.end(); ++it){
        stats=it->second;
        if(stats.get_total_operations()>=INIT_STAT_VAL) {
            this->sum_operations += stats.get_total_operations();
        }
        else{
            this->sum_errors++;
        }
//        this->sum_errors+=stats.get_num_errors();
    }
}

bool TravelsResults::compare(const TravelsResults& b) const{
    if (this->getSumErrors()<b.getSumErrors()){
        return true;
    }
    else if(this->getSumErrors()==b.getSumErrors()){
        return (this->getSumOperations()<b.getSumOperations());
    }
    else{
        return false;
    }
}
