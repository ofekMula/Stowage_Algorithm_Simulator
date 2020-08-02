//
// Created by Owner on 28-Mar-20.
//

#include "Simulation.h"
#include "../common/FileParser.h"
#include <thread>


Simulation:: Simulation(vector<TravelFiles>& p_travels, FileParser& p_file_parser,
        std::map<string,TravelsResults>& algorithm_to_stats):
		travels(p_travels), file_parser((p_file_parser)), statistics(algorithm_to_stats){}

Simulation::Simulation(vector<TravelFiles> &p_travels,
					   FileParser &p_file_parser,
					   int p_num_threads)
					   :
					   travels(p_travels),
					   file_parser(p_file_parser),
					   num_threads(p_num_threads)
{
    this->statistics=std::map<string,TravelsResults>();
}

Simulation::Simulation(Simulation&& simulation)
	:travels(simulation.travels), file_parser(simulation.file_parser),
	 statistics(simulation.statistics),
	 curr_algorithm_idx(simulation.curr_algorithm_idx.load()),
	 curr_travel_idx(simulation.curr_travel_idx.load()){}

void Simulation :: run_simulations()
{
    init_simulation_results_table();
	if (num_threads > 1)
	{
		// Spawn new threads only if parameter is larger than 1
		ThreadPoolExecutor thread_pool_executor {*this, num_threads};
		thread_pool_executor.start();
		thread_pool_executor.wait_till_finish();
	}
	else
	{
		AlgorithmRegistrar &algorithm_registrar = AlgorithmRegistrar::get_instance();

		for (int i = 0; i < algorithm_registrar.get_num_algorithms(); i++)
		{
			string algorithm_name =
					algorithm_registrar.get_algorithm_name_by_idx(i);


			for (TravelFiles travel : this->travels) {
				unique_ptr<AbstractAlgorithm> algorithm =
						algorithm_registrar.get_factory_by_idx(i)();
				run_single_travel(travel, *algorithm, algorithm_name);
			}
		}
	}
	update_sum_results_stats();
    file_parser.write_simulation_results_file(statistics,travels);
    file_parser.remove_empty_file();
}

void Simulation::run_single_travel(TravelFiles &travel,
								   AbstractAlgorithm &algorithm,
								   const string &algorithm_name)
{
    int errors = SUCCESS;
    int total_operations=INIT_VAL_ZERO;
    int port_errors=INIT_VAL_ZERO;
	ShipPlan ship_Plan;
	errors |= FileParser::read_ship_plan_file(travel.get_ship_plan_filepath(), ship_Plan);
	if (AlgErrorCodes::is_cannot_run_error(errors))
	{
		return;
	}

    Ship simulation_ship = Ship(ship_Plan);
	vector<string> route_string_vector;
	travel.get_route_string_vector(route_string_vector);
	WeightBalanceCalculator balance_calculator;
//	balance_calculator.readShipPlan(travel.ship_plan_filepath);

	errors = initialize_algorithm(algorithm, travel, balance_calculator, algorithm_name);

	if (AlgErrorCodes::is_cannot_run_error(errors))
	{
        return;
	}

    for (int port_idx = 0; port_idx < (int)travel.get_port_files().size(); port_idx++)
    {
    	const PortFile& port_file = travel.get_port_files()[port_idx];
		port_errors= simulate_algorithm_on_port(travel, algorithm,
												simulation_ship,
												balance_calculator,
                                                total_operations,//this->statistics[algorithm_name].get_travel_name_stats_map()[travel.get_name()].get_total_operations_ref(),
												travel.get_port_files(), port_idx,
												algorithm_name);
        file_parser.write_error_message_by_error_codes(port_errors,
                                                       port_file.get_file_path(),
                                                       ALGORITHM_ERROR_CODE, algorithm_name, travel.get_name());
        file_parser.write_warning_message_by_error_codes(port_errors,
                                                         port_file.get_file_path(),
                                                         ALGORITHM_ERROR_CODE, algorithm_name,travel.get_name());
    }
    this->statistics[algorithm_name].add_travel_stats(travel.get_name(),total_operations,TOTAL_OPERATIONS_FLAG);
}

int Simulation::initialize_algorithm(AbstractAlgorithm &algorithm, const TravelFiles &travel,
                                     WeightBalanceCalculator &balance_calculator, const string &algorithm_name)
{
	int errors = SUCCESS;
	errors |= algorithm.readShipRoute(travel.get_route_filepath());
    file_parser.write_error_message_by_error_codes(errors,
                                                   travel.get_route_filepath(),
                                                   ALGORITHM_ERROR_CODE, algorithm_name, travel.get_name());
    file_parser.write_warning_message_by_error_codes(errors,
                                                     travel.get_route_filepath(),
                                                     ALGORITHM_ERROR_CODE, algorithm_name, travel.get_name());
	if (AlgErrorCodes::is_cannot_run_error(errors))
	{
		return errors;
	}

	errors = algorithm.readShipPlan(travel.get_ship_plan_filepath());
    file_parser.write_error_message_by_error_codes(errors,
                                                   travel.get_ship_plan_filepath(),
                                                   ALGORITHM_ERROR_CODE, algorithm_name, travel.get_name());
    file_parser.write_warning_message_by_error_codes(errors,
                                                     travel.get_ship_plan_filepath(),
                                                     ALGORITHM_ERROR_CODE, algorithm_name, travel.get_name());

	if (AlgErrorCodes::is_cannot_run_error(errors))
	{
		return errors;
	}

	algorithm.setWeightBalanceCalculator(balance_calculator);

	return errors;
}

int Simulation::simulate_algorithm_on_port(const TravelFiles &travel,
										   AbstractAlgorithm &algorithm,
										   Ship &simulation_ship,
										   WeightBalanceCalculator &balance_calculator,
										   int &instruction_scores,
										   const vector<PortFile> &port_files,
										   int port_idx,
										   const string &algorithm_name)
{
	int port_errors = SUCCESS;
	Port port;
	const PortFile& port_file = port_files[port_idx];

	FileParser::read_Port_file(port_file.get_file_path(), port);

	// Copy ship and port for algorithm verification
	Ship ship_before_instructions = simulation_ship;
	Port port_before_instructions = port;

	std::vector<Instruction> instructions_at_port;

	string instruction_output_file = file_parser.get_instructions_output_file(travel.get_name(), algorithm_name, port_file);
	port_errors |= algorithm.getInstructionsForCargo(port_file.get_file_path(), instruction_output_file);

	FileParser::read_crane_instructions_file(instruction_output_file, instructions_at_port);

	instruction_scores += get_instruction_scores(instructions_at_port);
	for (const Instruction& instruction: instructions_at_port)
	{
		verify_and_perform_instruction(instruction, simulation_ship,
									   port,
									   balance_calculator, algorithm_name,
									   travel);
	}

	if (port_idx != (int)port_files.size() - 1 ) // Don't verify on last port
	{
		verify_cargo_loaded(instructions_at_port, port_before_instructions,
							travel, algorithm_name);
	}

	verify_cargo_unloaded(instructions_at_port, port_before_instructions,
						  ship_before_instructions, travel.get_name(),
						  algorithm_name);

#ifdef DEBUG
//	std::cout << "Instructions received by simulation at port "
//	<< port_file.seaport_code << std::endl;
//	for (const Instruction& instruction: instructions_at_port)
//	{
//		instruction.print();
//	}
//	std::cout << std::endl << std::endl;
		std::cout << "Simulation ship after instructions performed at port " << port.get_seaport_code() <<
		" by algorithm "  << algorithm_name << " in travel " << travel.get_name() <<  std::endl;
		simulation_ship.print_container_destinations();
#endif
	return port_errors;
}

Simulation::InstructionStatus
Simulation::verify_instruction(const Instruction &instruction, const Ship &ship,
							   const vector<Container> &containers_at_port)
{
	if (instruction.get_type() == Instruction::reject)
		return LEGAL;

	int row_idx = instruction.get_row_idx();
	int col_idx = instruction.get_col_idx();
	int floor_height = instruction.get_floor_height();
	if (col_idx >= ship.get_width() || row_idx >= ship.get_length()
		|| floor_height >= ship.get_height() || row_idx < 0 || col_idx < 0
		|| floor_height < 0)
	{
		return ILLEGAL_IDX;
	}

	// This variable can only be initialized now that we know idx is legal
	int tower_height = ship.get_container_tower_height(row_idx, col_idx);
	if ((instruction.get_type() == Instruction::load && floor_height != tower_height)
		|| (instruction.get_type() == Instruction::unload && floor_height != tower_height - 1))
	{
		return ILLEGAL_HEIGHT;
	}

	if (instruction.get_type() == Instruction::move)
	{
		row_idx = instruction.get_move_dest_row_idx();
		col_idx = instruction.get_move_dest_col_idx();
		floor_height = instruction.get_move_dest_floor_height();

		if (col_idx >= ship.get_width() || row_idx >= ship.get_length()
			|| floor_height >= ship.get_height() || row_idx < 0 || col_idx < 0
			|| floor_height < 0)
		{
			return ILLEGAL_IDX;
		}

		if (floor_height != ship.get_container_tower_height(row_idx, col_idx))
		{
			return ILLEGAL_HEIGHT;
		}
	}

	if (instruction.get_type() == Instruction::load)
	{
		int container_idx = Port::get_container_idx_by_id(containers_at_port, instruction.get_container_id());
		if (container_idx == NOT_FOUND)
		{
			return NON_EXISTANT_CONT_ID;
		}
	}

	return LEGAL;
}

string
Simulation::instruction_status_to_string(Simulation::InstructionStatus status)
{
	switch(status)
	{
		case LEGAL:
			return "Legal";
		case ILLEGAL_IDX:
			return "Illegal XY position index";
		case ILLEGAL_HEIGHT:
			return "Container can't be loaded or unloaded in this height";
		case NON_EXISTANT_CONT_ID:
			return "A container with this ID doesn't exist in port";
		default:
			return "";
	}
}

void Simulation::verify_and_perform_instruction(const Instruction &instruction,
												Ship &simulation_ship,
												Port &port,
												WeightBalanceCalculator &balance_calculator,
												const string &algorithm_name,
												const TravelFiles &travel)
{
    string error_line;
	auto instruction_status = verify_instruction(instruction,
												 simulation_ship,
												 port.get_cargo());
	auto balance_status = WeightBalanceCalculator::BalanceStatus::X_Y_IMBALANCED;
	if (instruction_status == LEGAL)
	{
		int row = instruction.get_row_idx();
		int col = instruction.get_col_idx();
		int floor_idx = simulation_ship.floor_height_to_idx(row, col,
															instruction.get_floor_height());

		Container instruction_container;

		if (instruction.get_type() == Instruction::unload)
		{
			instruction_container = simulation_ship.get_container_at_idx(row, col, floor_idx);
		} else if (instruction.get_type() == Instruction::load)
		{
			instruction_container = port.get_container_by_id(instruction.get_container_id());
		}

		balance_status = balance_calculator.tryOperation((char)instruction.get_type(),
														 instruction_container.get_weight(),
														 instruction.get_col_idx(),
														 instruction.get_row_idx());
		if (balance_status != WeightBalanceCalculator::APPROVED)
		{
            file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
                                                            travel.get_name(), port.get_seaport_code(),
                                                            "Instruction " + instruction.string_rep()
                                                            + " causes imbalance of type " + string(1, balance_status),
                                                            BAD_ALGORITHM_BEHAVIOR_CODE);

			statistics[algorithm_name].add_travel_stats(travel.get_name(),ERROR_INCREMENT,NUM_ERRORS_FLAG);
        }
	}
	else
	{
        file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
                                                        travel.get_name(), port.get_seaport_code(),
                                                        "Illegal instruction " +
                                                        instruction.string_rep()+": " + instruction_status_to_string(instruction_status), BAD_ALGORITHM_BEHAVIOR_CODE);

        statistics[algorithm_name].add_travel_stats(travel.get_name(),ERROR_INCREMENT,NUM_ERRORS_FLAG);

    }

	if (balance_status == WeightBalanceCalculator::APPROVED
		&& instruction_status == LEGAL)
	{
		simulation_ship.perform_instruction(instruction, port.get_cargo());
	}
}

void Simulation::verify_cargo_loaded(const vector<Instruction> &instructions,
									 const Port &port_before_instructions,
									 const TravelFiles &travel,
									 const string &algorithm_name)
{
    string error_line;
    const string& seaport_code = port_before_instructions.get_seaport_code();
	for (const Container& container: port_before_instructions.get_cargo())
	{
		verify_container_loaded(instructions, travel, algorithm_name,
								seaport_code, container);
	}
}

void
Simulation::verify_container_loaded(const vector<Instruction> &instructions,
									const TravelFiles &travel,
									const string &algorithm_name,
									const string &seaport_code,
									const Container &container)
{
	verify_container_loaded_unloaded_by_type(instructions, travel.get_name(),
											 algorithm_name, seaport_code,
											 container, Instruction::load);

}


void Simulation::verify_cargo_unloaded(const vector<Instruction> &instructions,
									   const Port &port_before_instructions,
									   const Ship &ship_before_instructions,
									   const string &travel_name,
									   const string &algorithm_name)
{
    string error_line;
	const string& seaport_code = port_before_instructions.get_seaport_code();
	for (int row = 0; row < ship_before_instructions.get_length(); row++)
	{
		for (int col = 0; col < ship_before_instructions.get_width(); col++)
		{
			for (const Container& container: ship_before_instructions.get_container_tower(row, col))
			{
				if (container.get_dest_port() == port_before_instructions.get_seaport_code())
				{
					verify_container_unloaded(instructions, travel_name,
											  algorithm_name, seaport_code,
											  container);
				}
			}
		}
	}
}

void
Simulation::verify_container_unloaded(const vector<Instruction> &instructions,
									  const string &travel_name,
									  const string &algorithm_name,
									  const string &seaport_code,
									  const Container &container)
{
	verify_container_loaded_unloaded_by_type(instructions, travel_name,
											 algorithm_name, seaport_code,
											 container, Instruction::unload);
}

void Simulation::verify_container_loaded_unloaded_by_type(const vector<Instruction> &instructions,
											   const string &travel_name,
											   const string &algorithm_name,
											   const string &seaport_code,
											   const Container &container,
											   Instruction::Type verify_type)
{
	Instruction::Type other_type = verify_type == Instruction::load ?
												   Instruction::unload
												   :Instruction::load;
	int last_inst_type_idx
			= get_idx_last_instruction_of_type_for_container(instructions,
															 verify_type,
															 container.get_id());
	if (last_inst_type_idx == NOT_FOUND)
	{
		if (verify_type == Instruction::load)
		{
			// If the container needs to be loaded, it's okay if it is rejected
			int last_reject_idx = get_idx_last_instruction_of_type_for_container(instructions,
																				 Instruction::reject,
																				 container.get_id());
			if (last_reject_idx == NOT_FOUND)
			{
				file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
													travel_name,
													seaport_code,
													"No load or reject instruction for container " + container.get_id()
													+ " to be loaded", BAD_ALGORITHM_BEHAVIOR_CODE);
				statistics[algorithm_name].add_travel_stats(travel_name, ERROR_INCREMENT, NUM_ERRORS_FLAG);
			}
		}
		else
		{
			file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
												travel_name,
												seaport_code,
												"No unload instruction for container " +
												container.get_id()
												+ " to be unloaded",
												BAD_ALGORITHM_BEHAVIOR_CODE);
			statistics[algorithm_name].add_travel_stats(travel_name, ERROR_INCREMENT, NUM_ERRORS_FLAG);
		}
	}
	else
	{
		// If instruction to verify found, make sure there is no unload afterwards
		int other_type_after_verify_idx
				= find_idx_next_instruction_of_type_for_container(instructions,
																  last_inst_type_idx + 1,
																  other_type,
																  container.get_id());
		if (other_type_after_verify_idx != NOT_FOUND)
		{
			write_error_load_after_unload_or_otherway(Instruction::reject,
													  travel_name,
													  algorithm_name,
													  seaport_code, container);
			statistics[algorithm_name].add_travel_stats(travel_name, ERROR_INCREMENT, NUM_ERRORS_FLAG);
		}
	}
}

void Simulation::write_error_load_after_unload_or_otherway(
		Instruction::Type verify_type, const string &travel_name,
		const string &algorithm_name, const string &seaport_code,
		const Container &container) const
{
	if (verify_type == Instruction::load)
	{
		file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
											travel_name,
											seaport_code,
											"Container "
											+ container.get_id()
											+ " unloaded back to port but needed to be loaded",
											BAD_ALGORITHM_BEHAVIOR_CODE);
	}
	else
	{
		file_parser.build_and_display_error(algorithm_name, ERROR_HEADER,
											travel_name,
											seaport_code,
											"Container " + container.get_id()
											+ " loaded back after being unloaded",
											BAD_ALGORITHM_BEHAVIOR_CODE);
	}
}

bool Simulation::load_algorithms()
{
	vector<string> algorithm_file_paths;
	file_parser.find_so_files(algorithm_file_paths);
	for (const string& algorithm_path: algorithm_file_paths)
	{
		string error;
		if(!AlgorithmRegistrar::get_instance().load_algorithm_from_file((algorithm_path).c_str(), error))
		{
			file_parser.display_error_message_to_file(ERROR_WITH_BRACKETS+error, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
		}
	}

	bool no_algorithms_loaded = AlgorithmRegistrar::get_instance().get_num_algorithms() <= 0;
	if (no_algorithms_loaded)
	{
		file_parser.display_error_message_to_file(ERROR_NO_ALGORITHMS, FATAL_ERROR_CODE);
	}
	return !no_algorithms_loaded;
}


int Simulation::get_idx_last_instruction_of_type_for_container(
		const vector<Instruction> &instructions,
		Instruction::Type instruction_type, string container_id)
{
	for (int i = instructions.size() - 1; i >= 0; i--)
	{
		if (instructions[i].get_container_id() == container_id
			&& instruction_type == instructions[i].get_type())
		{
			return i;
		}
	}

	return NOT_FOUND;
}

int Simulation::find_idx_next_instruction_of_type_for_container(
		const vector<Instruction> &instructions, int start_idx,
		Instruction::Type instruction_type, string container_id)
{
	for (int i = start_idx; i < (int)instructions.size(); i++)
	{
		if (instructions[i].get_container_id() == container_id
			&& instruction_type == instructions[i].get_type())
		{
			return i;
		}
	}

	return NOT_FOUND;
}

int Simulation::get_instruction_scores(
		const vector<Instruction> &instructions_at_port)
{
	int res_scores = 0;
	for (const Instruction& instruction: instructions_at_port)
	{
		switch (instruction.get_type())
		{

			case Instruction::load:
			case Instruction::unload:
				res_scores += LOAD_UNLOAD_SCORE;
				break;
			case Instruction::move:
				res_scores += MOVE_SCORE;
				break;
			case Instruction::reject:
				break;
		}
	}

	return res_scores;
}



std::optional<std::function<void(void)>> Simulation::get_task()
{
	std::optional<int> travel_idx;
	std::optional<int> algorithm_idx = get_next_algorithm_idx();
	bool flag=true;

	while(flag)
	{
        if (algorithm_idx)
        {
            travel_idx = curr_travel_idx.load();
            flag=false;
            if(travel_idx==travels.size())
            {
                travel_idx = get_next_travel_idx();
                break;
            }
        }
        else
		{
            curr_algorithm_idx.store(INIT_VAL_ZERO);

            // Will cause if (algorithm_idx) to be true in next while iteration
            algorithm_idx = get_next_algorithm_idx();

            travel_idx = get_next_travel_idx();
            if(!travel_idx)
            {
                flag=false;
            }
        }
    }

	if (algorithm_idx && travel_idx)
	{
		return [this, algorithm_idx, travel_idx]
		{
#ifdef THREAD_DEBUG
			std::cout << std::this_thread::get_id() << " - " << "Algorithm idx: " << *algorithm_idx << " Travel idx: " << *travel_idx <<std::endl;
#endif
			AlgorithmRegistrar& registrar_instance = AlgorithmRegistrar::get_instance();
			this->run_single_travel(travels[*travel_idx],
									*(registrar_instance.get_factory_by_idx(*algorithm_idx)()),
									registrar_instance.get_algorithm_name_by_idx(*algorithm_idx));
		};
	} else
	{
		return {};
	}
}

std::optional<int> get_next_idx_general(std::atomic_int& index, int size)
{
    int next_counter;
	for(int curr_counter = index.load(); curr_counter < size; )
	{
	    next_counter=curr_counter+1;
		// see: https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
		// note that in case compare_exchange_weak fails because the value of
		// task_counter != curr_counter than task_counter.load() is copied into curr_counter
		// in case of spurious failure (value not checked) curr_counter would not change
		if(index.compare_exchange_strong(curr_counter, next_counter)) {
			return {curr_counter}; // zero based
		}
	}
	return {};
}

std::optional<int> Simulation::get_next_algorithm_idx()
{
	return get_next_idx_general(curr_algorithm_idx,
								AlgorithmRegistrar::get_instance().get_num_algorithms());
}

std::optional<int> Simulation::get_next_travel_idx()
{
	return get_next_idx_general(curr_travel_idx, (int) travels.size());
}

void Simulation::init_simulation_results_table()
{
    AlgorithmRegistrar& algorithm_registrar = AlgorithmRegistrar::get_instance();

    for(int i = 0; i < algorithm_registrar.get_num_algorithms(); i++)
    {
        string algorithm_name = algorithm_registrar.get_algorithm_name_by_idx(i);
        this->statistics[algorithm_name]=TravelsResults();

        for (TravelFiles travel : this->travels)
        {
            this->statistics[algorithm_name].add_travel_stats(travel.get_name());
        }
    }

}

void Simulation::update_sum_results_stats() {
    map<string,TravelsResults>::iterator algo_travels=statistics.begin();
    while(algo_travels!=statistics.end()){
        statistics[algo_travels->first].update_sum_fields();
        algo_travels++;
    }

}






