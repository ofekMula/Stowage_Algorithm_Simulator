//
// Created by Owner on 02-May-20.
//

#include "../interfaces/AbstractAlgorithm.h"
#include "../interfaces/WeightBalanceCalculator.h"

WeightBalanceCalculator::BalanceStatus
WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y) {
//	 To avoid unused parameter warnings
    if (loadUnload||kg||X||Y){

    }
	return WeightBalanceCalculator::APPROVED;
}

int WeightBalanceCalculator::readShipPlan(
		const std::string &full_path_and_file_name) {
	if (full_path_and_file_name.size() == 0){}

	return 0;
}
