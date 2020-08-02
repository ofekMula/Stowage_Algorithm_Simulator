//
// Created by Meir on 28-Mar-20.
//

#ifndef EX1_ADV_CPP_COMMON_H
#define EX1_ADV_CPP_COMMON_H

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include "../interfaces/AbstractAlgorithm.h"
#include "Container.h"
#include "AlgErrorCodes.h"
#include "Instruction.h"
#include "PortFile.h"
#include "TravelFiles.h"
#include "ShipPlan.h"
#include "../interfaces/WeightBalanceCalculator.h"

#define NOT_FOUND -1

#define ISO_ID_SIZE 11

using std::vector;
using std::string;
using std::to_string;

//**********************************EXTRA MACROS*************************//
#define VALID_MIN_LIMIT1_CHAR 'a'
#define VALID_MAX_LIMIT1_CHAR 'z'
#define VALID_MIN_LIMIT2_CHAR 'A'
#define VALID_MAX_LIMIT2_CHAR 'Z'

#define SEAPORT_CODE_LEN 	5
#define SUCCESS				0

// This include has to be here
#include "Port.h"


#endif //EX1_ADV_CPP_COMMON_H
