//
// Created by Owner on 28-Mar-20.
//

#ifndef EX1_ADV_CPP_SIMULATION_H
#define EX1_ADV_CPP_SIMULATION_H


#include "../common/common.h"
#include "../common/Ship.h"
#include "../common/FileParser.h"
#include "AlgorithmRegistrar.h"
#include <ctime>
#include <map>
#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include "ThreadPoolExecutor.h"

#define INSTRUCTIONS_PERFORMED_MESSAGE "At port: "
#define INSTRUCTIONS_FOR_TRAVEL_MESSAGE "Instructions performed for Travel: "
#define SUM_OPERATION_OF_ALGORITHM_FOR_TRAVEL "sum of instructions for:"
#define RESULT_OF_BAD_BEHAVIOUR_ALGORITHM -1
#define ERROR_INCREMENT 1
#define ERROR_NO_ALGORITHMS "No algorithms successfully loaded!"
#define LOAD_UNLOAD_SCORE 5
#define MOVE_SCORE 3


class Simulation
{
	vector<TravelFiles> travels;

	FileParser file_parser;

	enum InstructionStatus {LEGAL, ILLEGAL_IDX, ILLEGAL_HEIGHT, NON_EXISTANT_CONT_ID};

	std::map<string,TravelsResults> statistics;

	int num_threads;
	std::atomic_int curr_algorithm_idx = 0;
	std::atomic_int curr_travel_idx = 0;


//	std::mutex statistics_lock;
public:
	Simulation(vector<TravelFiles>& p_travels, FileParser& p_file_parser,
               std::map<string,TravelsResults>& algorithm_to_stats);


    Simulation(vector<TravelFiles> &p_travels, FileParser &p_file_parser,
			   int p_num_threads);

    Simulation(Simulation&& simulation);
    /**
     * Top run function. Runs all travel algorithm pairs
     */
	void run_simulations();

	/**
	 * Finds SO files and loads the algorithms using AlgorithmRegistrar
	 * Writes error messages to file
	 * @return true if at least one algorithm was successfully loaded and registered
	 */
	bool load_algorithms();

	/**
	 * @return a task for the thread pool to run a specific algorithm, travel pair
	 */
	std::optional<std::function<void(void)>> get_task();


private:
	void run_single_travel(TravelFiles &travel,
						   AbstractAlgorithm &algorithm,
						   const string &algorithm_name);

	Simulation::InstructionStatus
	verify_instruction(const Instruction &instruction, const Ship &ship,
					   const vector<Container> &containers_at_port);

	string instruction_status_to_string(InstructionStatus status);

	/**
	 * Does all the instruction verification of the simulation.
	 * Mainly a helper function of run_single_travel, it probably shouldn't
	 * be used elsewhere
	 *
	 * simulation_ship will be updated with the perform instruction if it's legal
	 *
	 * @param instruction
	 * @param simulation_ship
	 * @param port
	 * @param algorithm_name - used to write to file
	 * @param balance_calculator - used to verify against balance calculator
	 */
	void
	verify_and_perform_instruction(const Instruction &instruction,
								   Ship &simulation_ship, Port &port,
								   WeightBalanceCalculator &balance_calculator,
								   const string &algorithm_name,
								   const TravelFiles &travel);

	/**
	 * Verifies that the all the cargo in port_before_instructions
	 * has a load or reject instruction in instructions.
	 * Writes any errors to the error file.
	 * @param instructions
	 * @param port_before_instructions
	 * @param ship_before_instructions
	 * @param travel - Currently used mostly to display the travel name in
	 * 					error messages but in the future
	 * 					can be used to check whether
	 * 					the algorithm should reject or load
	 */
	void verify_cargo_loaded(const vector<Instruction> &instructions,
							 const Port &port_before_instructions,
							 const TravelFiles &travel,
							 const string &algorithm_name);

	/**
	 * Verifies all cargo that needs to be unloaded in port_before_instructions
	 * is truly unloaded
	 * @param instructions
	 * @param port_before_instructions
	 * @param ship_before_instructions
	 * @param travel_name
	 * @param algorithm_name
	 */
	void verify_cargo_unloaded(const vector<Instruction> &instructions,
							   const Port &port_before_instructions,
							   const Ship &ship_before_instructions,
							   const string &travel_name,
							   const string &algorithm_name);

	/**
	 *
	 * @param instructions
	 * @param container_id
	 * @return The index of the last instruction in instructions that has container_id
	 * 			NOT_FOUND if there is no such instruction
	 */
	int
	get_last_instruction_idx_for_container(const vector<Instruction> &instructions,
										   const string &container_id);

	/**
	 *
	 * @param instructions
	 * @param container_id
	 * @return The index of the last instruction in instructions that has container_id
	 * 			and is of type
	 * 			NOT_FOUND if there is no such instruction
	 */
	int
	get_idx_last_instruction_of_type_for_container(
					const vector<Instruction> &instructions,
					Instruction::Type instruction_type, string container_id);


	int simulate_algorithm_on_port(const TravelFiles &travel,
								   AbstractAlgorithm &algorithm,
								   Ship &simulation_ship,
								   WeightBalanceCalculator &balance_calculator,
								   int &instruction_scores,
								   const vector<PortFile> &port_files,
								   int port_idx,
								   const string &algorithm_name);

	int
	initialize_algorithm(AbstractAlgorithm &algorithm, const TravelFiles &travel,
                         WeightBalanceCalculator &balance_calculator, const string &algorithm_name);


	int find_idx_next_instruction_of_type_for_container(
			const vector<Instruction> &instructions, int start_idx,
			Instruction::Type instruction_type, string container_id);

	void verify_container_loaded(const vector<Instruction> &instructions,
								 const TravelFiles &travel,
								 const string &algorithm_name,
								 const string &seaport_code,
								 const Container &container);

	void verify_container_unloaded(const vector<Instruction> &instructions,
								   const string &travel_name,
								   const string &algorithm_name,
								   const string &seaport_code,
								   const Container &container);

	/**
	 * Verifies if a container was loaded if type is load
	 * or that it was unloaded if type is unload
	 * @param instructions
	 * @param travel_name
	 * @param algorithm_name
	 * @param seaport_code
	 * @param container
	 * @param verify_type
	 */
	void verify_container_loaded_unloaded_by_type(
			const vector<Instruction> &instructions, const string &travel_name,
			const string &algorithm_name, const string &seaport_code,
			const Container &container, Instruction::Type verify_type);

	void
	write_error_load_after_unload_or_otherway(Instruction::Type verify_type,
											  const string &travel_name,
											  const string &algorithm_name,
											  const string &seaport_code,
											  const Container &container) const;

	/**
	 * @param instructions_at_port
	 * @return The score of all instructions in @param instructions_at_port
	 */
	static int get_instruction_scores(const vector<Instruction> &instructions_at_port);

	std::optional<int> get_next_algorithm_idx();
	std::optional<int> get_next_travel_idx();

	void init_simulation_results_table();
	void update_sum_results_stats();


};



#endif //EX1_ADV_CPP_SIMULATION_H