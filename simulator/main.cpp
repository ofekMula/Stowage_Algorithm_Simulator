#include <iostream>
#include <cstdlib>
#include "../common/common.h"
#include "Simulation.h"
#include "../common/FileParser.h"
#include "AlgorithmRegistrar.h"

int main(int argc, const char **argv)
{
    int status,threads_num;
    FileParser file_parser;
    file_parser.load_command_line_parameters(argc, argv, threads_num);
    vector<TravelFiles> travels;
    status = file_parser.load_travels_for_simulation( travels);
#ifdef DEBUG
    std::cout << status;
#endif

	try
	{
		if (status == SUCCESS)
		{
			Simulation simulation = Simulation(travels, file_parser, threads_num);
			if(simulation.load_algorithms())
			{
				simulation.run_simulations();
			}
		}
    }
    catch(std::exception& ex)
	{
    	file_parser.display_error_message_to_file(string("Exception Caught: ") + ex.what(),FATAL_ERROR_CODE);
    	status = FAILURE;
	}

    if (status == SUCCESS)
    	return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

