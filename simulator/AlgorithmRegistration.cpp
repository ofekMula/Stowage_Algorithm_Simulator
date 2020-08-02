//
// Created by Owner on 12-May-20.
//
#include "../interfaces/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(
		std::function<std::unique_ptr<AbstractAlgorithm>()> algorithm_factory)
{
	AlgorithmRegistrar::get_instance().register_algorithm_factory(algorithm_factory);
}
