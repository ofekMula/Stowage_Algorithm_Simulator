//
// Created by Ofek on 09-Apr-20.
//

#ifndef EX1_ADV_CPP_FILEPARSER_H
#define EX1_ADV_CPP_FILEPARSER_H
#include "common.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mutex>
#include <memory>
#include "../simulator/TravelsResults.h"
#include "ErrorFilesMutex.h"

//**************************GENERAL*****************************//
#define COMMA_DEL ','
#define COMMENT_CHAR '#'
#define WHITE_SPACE_CHAR ' '
#define EMPTY_STRING ""
#define PATH_DELI "/"
#define NEW_LINE_STR "\r\n"
#define CR_CHAR '\r'
#define FILE_TYPE_STRING_SIGN "."
#define UNDER_SCORE "_"
#define TAB_ASCII_VALUE 13
#define CONTAINER_ID_INDEX 0
#define CONTAINER_WEIGHT_INDEX 1
#define CONTAINER_DEST_INDEX 2
#define NUM_FIELDS_OF_CONTAINER_PORT 3
#define FAILURE -1
#define MAX_PATH_SIZE 100
#define FILE_NAME_START_INDEX 0
#define INIT_VAL_ZERO 0
#define FIRST_LINE_INDEX 0
#define SHIP_PLAN_FIRST_LINE_SIZE 3
#define BAD_WEIGHT -1
#define ROUTE_FILE_LINE_SIZE 1
#define INIT_ALREADY_USED_PORTS_INDEX_SIZE 0
#define SEAPORT_CODE_SIZE 5
#define BAD_WEIGHT -1
//**************************FOLDERS NAMES*****************************//
#define FOLDER_CRANE_INSTRUCTIONS "crane instructions"
#define FOLDER_ERRORS "errors"
//**************************Files types and names*******************************//
#define FILE_TYPE_CARGO_DATA "cargo_data"
#define FILE_TYPE_SHIP_PLAN "ship_plan"
#define FILE_TYPE_ROUTES "route"
#define FILE_TYPE_SO "so"
#define FILE_TYPE_RESULTS "results"
#define FILE_TYPE_INSTRUCTIONS "crane_instructions"
#define OUTPUT_FILE_RESULTS "simulation.results"
#define EMPTY_CARGO_DATA_FILE "empty.cargo_data"
//#define FATAL_ERRORS_FILE "fatal_errors.txt"
//#define TRAVEL_ERRORS_FILE "travel_errors.txt"
//#define BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE "bad_algorithm_behavior_errors.txt"
//#define IGNORABLE_ERRORS_FILE "ignorable_errors.txt"
//#define ALGORITHM_ERRORS_FILES "Error code returned by the Algorithm.txt"

//**************************ERRORS AND WARNINGS MESSAGES*******************************//
#define ERROR_NO_TRAVEL "[simulator][ERROR] no valid travels loaded"
#define ERROR_BAD_CARGO "ERROR: no ports file loaded successfully in Travel: "
#define ERROR_TRAVEL_BAD_FIELDS "failed to load this travel."
#define ERROR_TOO_MANY_ARGS "too many arguments in command line. the program demands only the simulation folder name"
#define ERROR_SIMULATION_FOLDER_NOT_EXISTS "ERROR: the simulation directory name you gave does not exist"
#define ERROR_TRAVEL_FOLDER_NOT_EXISTS "[ERROR] the travel directory name does not exist"
#define ERROR_CREATE_FILE_FAIL "ERROR: failed to create new file"
#define ERROR_MISSING_TRAVEL_PATH "[ERROR] -travel_path argument is missing"
#define ERROR_INSTRUCTION_FILE "ERROR: crane instructions file has a line with bad format"
#define WARNING_LAST_PORT_FILE_HAS_CONTAINERS "[cargo_data] last port has waiting containers (ignored)"
#define WARNING_PORT_NO_CONTAINERS "[cargo_data] no containers awaiting in this port"
#define WARNING_PORT_BAD_LINE_FORMAT "[cargo_data] port bad line format"
#define WARNING_PORT_FILE_IS_NOT_IN_ROUTE "[cargo_data] found cargo_data file which is not in ship route"
#define WARNING_EXTRA_PORT_FILE "[cargo_data] unnecessary cargo_data file: "
#define WARNING_BAD_OUTPUT_PATH_ARG "[WARNING] output path given in command line does not exist. the output dir placed here: "
#define WARNING_BAD_ALGORITHM_PATH_ARG "[WARNING] algorithm path given in command line does not exist. the algorithm dir placed here: "
#define WARNING_TOO_MANY_SHIP_PLAN_FILES "found more than one ship plan file in travel folder"
#define WARNING_TOO_MANY_ROUTE_FILES "found more than one travel route file travel folder"
#define WARNING_BAD_PORT_FILE_NAME "[cargo_data] bad format visit number at port file name should be at least 1"
#define WARNING_PARAM_NUM_THREADS_NOT_NUMBER "[WARNING] the given number of threads parameter is not a number"
#define WARNING_PARAM_NUM_THREADS_BAD_NUMBER "[WARNING] the given number of threads must be a number 1 or above"

#define IN_TRAVEL " in Travel: "
#define IN_PORT " in Port: "
//***************************ERROR TYPE CODES**************************************
#define FATAL_ERROR_CODE 0
#define TRAVEL_ERROR_CODE 1
#define BAD_ALGORITHM_BEHAVIOR_CODE 2
#define IGNORABLE_BAD_EXTERNAL_INPUT_CODE 3
#define ALGORITHM_ERROR_CODE 4
//**************************COMMAND LINE PARAMETERS*******************************//
#define TRAVEL_PATH_PARAM "-travel_path"
#define OUTPUT_PATH_PARAM "-output"
#define ALGORITHM_PATH_PARAM "-algorithm_path"
#define NUM_THREADS_PATH_PARAM "-num_threads"
#define DEFAULT_PARAM_INDEX 0
//**************************CRANE INSTRUCTIONS**************************
#define L_INSTRUCTION "L"
#define U_INSTRUCTION "U"
#define M_INSTRUCTION "M"
#define R_INSTRUCTION "R"
#define INSTRUCTION_TYPE_INDEX 0
#define INSTRUCTION_CONTAINER_ID_INDEX 1
#define INSTRUCTION_FLOOR_INDEX 2
#define INSTRUCTION_X_INDEX 3
#define INSTRUCTION_y_index 4
#define INSTRUCTION_MOVE_FLOOR_INDEX 5
#define INSTRUCTION_MOVE_X_INDEX 6
#define INSTRUCTION_MOVE_Y_INDEX 7
#define INSTRUCTION_LINE_SIZE 5
#define INSTRUCTION_REJECT_LINE_SIZE 2
#define INSTRUCTION_MOVE_LINE_SIZE 8
//**************************ERRORS MACROS**************************
#define WARNING__HEADER "WARNING"
#define ERROR_HEADER "ERROR"
#define ERROR_WITH_BRACKETS "[ERROR]"
#define SIMULATOR_BRACKET "simulator"
#define NO_ROUTE_OR_SHIP_PLAN_FILE "no ship plan or route file"
#define CONTAINER_HEADER "[container]"
//**************************EXTRA MACROS**************************
#define RESULTS_HEADER "RESULTS"
#define SUM_HEADER "Sum"
#define NUM_ERRORS_HEADER "Num Errors"
#define TRAVEL_NAME "Travel: "
#define FILE_NAME "file: "
#define CONTEXT_NAME "context:"
#define ALGORITHM_ERROR_CODE_FLAG true
class FileParser {
private:
    //fields which holds useful paths for simulation
    string cwd_folder_path;
    string travels_folder_path;
    string output_folder_path;
    string algorithm_folder_path;
    vector<string> empty_files_paths;

    std::shared_ptr<ErrorsFilesMutex> error_file_to_mutex;
    // The mutexes are wrapped in a shared_ptr because mutexes don't have a
    // copy constructor and thus if we had a ErrorsFilesMutex field
    // for FileParser, it wouldn't have a copy constructor either.
    // So to preserve the rule of zero and avoid implementation of a copy
    // constructor for both FileParser and Simulation we used a shared_ptr which
    // has a copy constructor (unique_ptr doesn't).

    /**
    * creates  simulation folder in current working directory
    * there there will be the  output+input directories by default
    */
    void load_current_directory();
    /**
     * iteratte over carge_data files for building a travel for simulation
     * which after will simulate the algorithm on it
     * @param cargo_folder_path
     * @param routes
     * @param ports
     * @return
     */
    int scan_cargo_files_by_simulation(const string &cargo_folder_path, vector<string> &routes, vector<PortFile> & ports);
    /**
     * scan port file and checks its validity
     * according to the requirements
     * @param file_path
     * @param port_file
     * @return
     */
    int scan_Port_file_by_simulation(const string &file_path, PortFile &port_file, const string &travel_name);
    /**
     * gets a folder path for travel input and their loads the valid
     * ship_plan,route,cargo_data files for the travel
     * @param folder_path
     * @param travel
     * @return
     */
    int build_travel(const string& folder_path, TravelFiles &travel);
    /**
     * helper function for read port function
     * validate that the container ID in unique and
     * there is no duplication of it in the same port
     * @param cargo
     * @param container_id
     * @return
     */
    static bool container_id_already_loaded(vector<Container>& cargo,const string& container_id);
    /**checks the correctness of routes file for travel object
     *a. verify that eacxh line has 1 element
     *b. verify that the same port doesnt appear 2 in a row
     * c. checks that each symbol is written correct
     * @param
     * @param dataList
     * @return 0 for valid file -1 for bad file
     */
    static int input_file_routes_validation(vector<vector<string>> &dataList, vector<vector<string>> &cleansed_data_list,int &errors);

    /**
     * takes file and converts it to vector
     * @param file_path
     * @param dataList
     */
    static void parse_file_to_vector(std::ifstream& file,vector<vector<string> >& dataList);
    /**
     * split line read from csv file by ','
     * @param line
     * @param del
     * @return vector of strings which represents fields of object
     */
    static vector<string> split(std:: string line,char del);
    /**
     * gets filename
     * type is a number for each object
     * @param file_name
     * return field from name
     */
    static string get_port_symbol_from_file_name(const string file_name);
    /**
     * assuming the file_name comes without
     * the path and file type: "a_1"
     * extract the number after the under score
     * @param file_name
     * @return  the correct number visit .else rerturns
     * -1 in case of failure
     */
    int get_port_number_from_file_name(const string file_name);

    /**
     * assuming dot "." is not in file name
     * @param relative file_path
     * return file name
     */
    string extract_file_name_without_type(const string& file_path);

    static string extract_file_name_with_type(const string& file_path);

    /**
     * @param file_path
     * @return file ending
     */
    string get_file_type(const string file_path);
    /**
     * checks if certain file type is in the directory
     * each file which has the same type
     * his path will be recorded in the file_paths vector
     * types are : cargo_data,routes,error,ship_plan,so
     * @param folder_path
     * @param type
     * @return SUCCESS if atleast one file has found
     * else returns FAILURE
     */
    int get_files_from_folder_by_type(const string folder_path, string type, vector<string> &file_paths);
    /**
    * reorganize the cargo_data files which scanned by simulation by the order of routes file
    * adds empty files in cases of missing files.
    * @param routes
    * @param ports
    * @param travel_path
    */
    void organize_Port_files_by_routes(vector<string> &routes, vector<PortFile> &ports, const string &travel_path);
    static bool only_white_spaces_string(const string &word);
    /**
 * creates directory by the path given
 * @param directory_path
 * @return -1 on failure
 */
    int create_directory(const string directory_path) const;
    /**
     * checks is the folder path is an exsit path of directory
     * @param directory_path
     * @return true by success.
     */
    bool directory_exists(const string & directory_path) const;
    static bool is_file_exist(const string& file_path);

    static void write_line_to_file(const string& filepath,const string& message);

    /**
     * truncates existing ouput files
     */
    void init_outputs_files();
    static void print_datalist(vector<vector<string>>& list);
    /**
     * helper function for read _ship_plan_file method
     * iterates over the positions lines in the ship plan file
     * and checks their validity
     * @param data_list
     * @param num_containers_length
     * @param num_containers_width
     * @param num_floors
     * @param errors
     * @param floor_start
     */
    static void check_pos_desc_ship_plan_file(vector<vector<string>> data_list, int &num_containers_length,
                                              int &num_containers_width,
                                              int &num_floors, int &errors, vector<vector<int>>& floor_start);

    /**
     * method for comparing elements  in results simulation vector
     * @param a
     * @param b
     * @return
     */
    static bool sort_by_travels_results(const std::pair<string, TravelsResults> &a, const std::pair<string, TravelsResults> &b);
    /**
     * sort results simulation results vector
     * @param statistics
     * @param sorted_results
     */
    void sort_simulation_vector_results(std::map<string, TravelsResults> &statistics,
                                        vector<std::pair<string, TravelsResults>> &sorted_results);
    static bool is_number(const string& word);

    bool valid_port_file_name(const string &seaport, int &number);


    string extract_folder_name_from_path(const string &file_path);


    void build_and_display_error_cargo_data(const string &context, const string &error_type, const string &travel_name,
                                            const string &file_name, const string &container_id,
                                            const string &message, const int error_code) const;


public:
    FileParser();
    /**
    * called in main, loads all correct paths given.
    * algorithm folder is saved in Simulation object and not in FileParser
    * read command line parameters and saves all directory pathes in globals var
    * if travel_path is not given or its path does no exist - fatal error
    * if output_path is not given - creates it
    * if algorithms_path is not given - checks if the defaults exist
    * if the given algorithm_path does not exist - fatal error
    * @param argc
    * @param argv
    * @param algorithm_folder_path
    * @return 0 - Success , -1 Failure
    */
    int load_command_line_parameters(int argc, const char **argv, int &num_threads);

    /**
    * assume the function gets non-initiated routes vector
    * @param file_path
    * @param routes
    * @param dataList
    */
     static int read_route_file(const string &file_path, vector<string> &routes);
    /**
    *
    * @param file_path
    * @param ship_plan
    * @return 0 if successfully read ship_plan file
    * 1 if failed
    */
    static int read_ship_plan_file(const string &file_path, ShipPlan &ship_plan);

    /**read port file in travel and creates port object
     * file name is port symbol + number
     * each line represents a container that waiting in the port to be loaded
     * if line is coruptted the container is not counts
     * if no written right containers at port - displays warning in the console
     *
     * @param file_path
     * @param port
     * @return 0 if success - if fail
    */
    static int read_Port_file(const string& file_path, Port& port);
    /**
     * creates travel object
     * gets directory of travel and reads files from there
     * @param folder_path
     * @param travel
     * @return
     */
    void port_file_validation(const string& file_path,
                                    int& errors,
                                    vector<vector<string>>& data_list,
                                     string& container_ID,
                                     string& dest_port_symbol,
                                     int& container_weight,
                                     vector<Container>& cargo,
                                     bool algorithm_error_code_flag);

    /**
     * iterate over all travels in the travel_path folder and checks their validity
     * @param travels
     * @return
     */
    int load_travels_for_simulation(vector<TravelFiles>& travels);
    /**
     * statis method for algorithms to write their instructions they made
     * at each port during their run
     * @param instruction_performed
     * @param crane_inst_filepath
     */
    static void write_crane_instruction_to_file(const Instruction &instruction_performed, const string& crane_inst_filepath);
    /**
     * prints to simulation.results file statistics of each algorithm
     * on the travels input files
     * @param results
     */
    void write_simulation_results_file(const vector<string>& results);
    /**
     * gives the algorithm the path to the file he will write the instructions
     * he made during his run on a trvael
     * @param travel_name
     * @param algorithm_name
     * @param cargo_port_file
     * @return
     */
    const string get_instructions_output_file(const string &travel_name,
                                                         const string &algorithm_name,
                                                         const PortFile &cargo_port_file);

    /**display error to console and wirte them to simulation.error file
     * @param message
     */
    void display_error_message_to_file(const string message,int error_type_code) const;

	/**
	 * each error has an error code. by getting its code this method
	 * decides which error message will be written by that
	 * @param errors - An in encoding error flags defined in AlgErrorCodes
	 * @param file_path - The file in which the error occurred
	 */
    void write_errors_to_travel_and_fixable_files_by_errors_code(int errors, const string &file_path, string context,
                                                                 string travel_name);

	/**
	 * helper function for writing the errors messages
	 * @param errors
	 * @param file_path
	 */
    void write_error_message_by_error_codes(int errors, const string &file_path, int error_type,const string &context,
                                            const string &travel_name);


	/**
	 * hekper function for writing the warning messages
	 * @param errors
	 * @param file_path
	 */
    void write_warning_message_by_error_codes(int errors, const string &file_path, int error_type,
                                              const string &context, const string &travel_name);

    const string &get_cwd_folder_path() const;

    const string &get_travels_folder_path() const;

    const string &get_output_folder_path() const;

    const string &get_Algorithm_folder_path() const;
    /**
     * loads all crane instruction file made by algorithm
     * @param file_path
     * @param instructions
     */
    static void read_crane_instructions_file(const string & file_path,vector<Instruction>& instructions);
    /**
     * convert instruction type from  string to enum
     * @param instruction
     * @param type
     */
    static void convert_instruction_type(Instruction& instruction, const string& type);

    static void write_instruction_vector_to_file(const vector<Instruction> &instructions, const string& crane_inst_filepath);
    /**
     * writes the simulation results file at /output_path/"simulation.results"
     * @param statistics
     * @param travels
     */
    void write_simulation_results_file(std::map<string,TravelsResults>& statistics,const vector<TravelFiles>& travels);

	/**
	* assuming dot "." is not in file name
	* @param full file_path
	* return file name
	*/
	static string extract_file_name_from_path(const string file_path);

	/**
	 * method which returns all paths to the
	 * .so files in the given directory
	 * @param so_paths
	 */
	void find_so_files(vector<string>& so_paths);

    /**
     * with the given parameters builds the error according to the next format:
     * [context][error_type][travel][file]message text
     * and then uses the display error method ti write it to the match file
     * @param context
     * @param error_type
     * @param travel_name
     * @param file_name
     * @param message
     * @param error_code
     */
	void build_and_display_error(const string &context, const string &error_type, const string &travel_name,
								 const string &file_name, const string &message, const int error_code) const;

	/**
	 * this for deleting the empty files we created
	 * during the simulation
	 */
    void remove_empty_file();
    /**
     * encapsulate string with brackets: "[string]"
     * @param word
     * @return
     */
    string string_brackets_cover(string word) const;

	static int create_file(const string& filepath);

};

#endif //EX1_ADV_CPP_FILEPARSER_H
