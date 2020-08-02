//
// Created by Owner on 09-Apr-20.
//

#include <map>
#include "FileParser.h"
#include <memory>


FileParser::FileParser() {
    this->error_file_to_mutex=std::make_shared<ErrorsFilesMutex>();
}
int FileParser::read_ship_plan_file(const string &file_path, ShipPlan &ship_plan) {
	int errors = SUCCESS;
    vector<vector<string> > data_list;
    int num_floors,num_containers_length,num_containers_width;
    vector<vector<int>> floor_start;
    std::ifstream file;
    file.open(file_path);
    //checks
    if (!file.is_open())
    {
		errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
		return errors;
    }
    parse_file_to_vector(file,data_list);
    if(data_list.empty()){
        errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
        return errors;
    }
    try
    {
        if (data_list[FIRST_LINE_INDEX].size()!=SHIP_PLAN_FIRST_LINE_SIZE){
            errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
            return errors;
        }
        if(!is_number(data_list[CONTAINER_ID_INDEX][CONTAINER_ID_INDEX]) ||
        !is_number(data_list[CONTAINER_ID_INDEX][CONTAINER_WEIGHT_INDEX]) ||
                !is_number(data_list[CONTAINER_ID_INDEX][CONTAINER_DEST_INDEX])){
            errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
            return errors;
        }

        num_floors = stoi(data_list[CONTAINER_ID_INDEX][CONTAINER_ID_INDEX]);
        num_containers_length = stoi(data_list[CONTAINER_ID_INDEX][CONTAINER_WEIGHT_INDEX]);
        num_containers_width = stoi(data_list[CONTAINER_ID_INDEX][CONTAINER_DEST_INDEX]);
        if(num_floors <=INIT_VAL_ZERO || num_containers_length <=INIT_VAL_ZERO || num_containers_width <= INIT_VAL_ZERO){
            errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
            return errors;
        }
    }
    catch (const std::invalid_argument&){
        errors |= AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL;
        return errors;

    }
    if(data_list.size()>1){//check positions data
        floor_start=vector<vector<int>>( num_containers_length , vector<int> (num_containers_width, 0));
        check_pos_desc_ship_plan_file(data_list, num_containers_length, num_containers_width, num_floors, errors,floor_start);
    }
    else{
        floor_start=vector<vector<int>>( num_containers_length , vector<int> (num_containers_width, 0));
    }
    ship_plan=ShipPlan(num_containers_width,num_containers_length,num_floors,floor_start);
    return errors;
}

void FileParser::check_pos_desc_ship_plan_file(vector<vector<string>> data_list, int &num_containers_length,
                                               int &num_containers_width,
                                               int &num_floors, int &errors, vector<vector<int>>& floor_start) {
    std::map<std::pair<int,int>,int> map_pair_to_floor;
    int pos_x,pos_y,actual_num_floors;
    for(int i=1;i<(int)data_list.size();i++){
        if(data_list[i].size() != NUM_FIELDS_OF_CONTAINER_PORT){  //3 values each line in ship_plan file
            errors|=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
            continue;//ignore position
        }
        try {//validate these are numbers
            if(!is_number(data_list[i][CONTAINER_ID_INDEX]) ){
                errors |=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
                continue;
            }
            if(!is_number(data_list[i][CONTAINER_WEIGHT_INDEX])){
                errors |=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
                continue;
            }
            if(!is_number(data_list[i][CONTAINER_DEST_INDEX])){
                errors |=AlgErrorCodes::SHIP_PLAN_ILLEGAL_NUM_FLOORS;
                continue;
            }
            pos_x = stoi(data_list[i][CONTAINER_ID_INDEX]);
            pos_y = stoi(data_list[i][CONTAINER_WEIGHT_INDEX]);
            actual_num_floors = stoi(data_list[i][CONTAINER_DEST_INDEX]);
            if (actual_num_floors <0 || pos_x < 0 || pos_y <0 ){
                errors |=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
                continue;
            }
            if(actual_num_floors>=num_floors){
                errors |=AlgErrorCodes::SHIP_PLAN_ILLEGAL_NUM_FLOORS;
                continue;
            }
            if(pos_x>=num_containers_length|| pos_y>=num_containers_width){
                errors |= AlgErrorCodes::SHIP_PLAN_XY_IDX;
                //display_error_message_to_file(WARNING_POS_LIMIT+file_path,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;//ignore bad position data as input
            }

        }
        catch (const std::invalid_argument&){
            errors |=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
            continue;
        }
        std::pair<int,int> pos(pos_x,pos_y);
        if(map_pair_to_floor.find(pos)!=map_pair_to_floor.end()){
            if(map_pair_to_floor[pos]!=actual_num_floors){
                errors |=AlgErrorCodes::SHIP_PLAN_DUPLICATE_XY;
            }
            else{
                errors |=AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT;
            }
        }
        else {
            floor_start[pos_x][pos_y] =num_floors - actual_num_floors;// in this position , all the top actual_num_floors available
            map_pair_to_floor[pos]=actual_num_floors;
        }
    }
}

int FileParser  ::read_route_file(const string &file_path, vector<string> &routes) {
    int errors = SUCCESS;
    vector<vector<string> > data_list;
    vector<vector<string>> cleaned_data_list=vector<vector<string>>();
    std::ifstream file;
    file.open(file_path);
    if (!file.is_open()){
        errors |= AlgErrorCodes::ROUTE_READ_FAIL_OR_FILE_EMPTY;
        return errors;
    }
    parse_file_to_vector(file, data_list);
    //print_datalist(data_list);
    if(input_file_routes_validation(data_list, cleaned_data_list, errors) == SUCCESS){
        for (int i=0;i<(int)cleaned_data_list.size(); i++){
            routes.push_back(cleaned_data_list[i][CONTAINER_ID_INDEX]);
        }
    }
    return errors;
}


int
FileParser::input_file_routes_validation(vector<vector<string>> &dataList, vector<vector<string>> &cleansed_data_list,
                                         int &errors) {
    int sequential_count=0;
    vector<vector<string>> temp;
    for (int i = 0; i <(int) dataList.size(); i++) {
        if (dataList[i].size()!= 1) {
            errors |= AlgErrorCodes::ROUTE_ILLEGAL_PORT_SYMBOL;
            continue;

        }
        else if(!Port::verify_seaport_code(dataList[i][CONTAINER_ID_INDEX])){
                errors |= AlgErrorCodes::ROUTE_ILLEGAL_PORT_SYMBOL;
                continue;
        }
        else{
            temp.push_back(dataList[i]);
        }
    }

    for (int j=0;j<(int)temp.size();j++){
        if(j<(int)temp.size()-1){
            if (temp[j][CONTAINER_ID_INDEX].compare(temp[j + 1][CONTAINER_ID_INDEX]) == CONTAINER_ID_INDEX){
                //display_error_message_to_file(ERROR_ROUTES_CONSECUTIVE+file_path,TRAVEL_ERROR_CODE);
                errors |= AlgErrorCodes::ROUTE_DUPLICATE_CONSECUTIVE_PORT;
                if (sequential_count == 0){

                    cleansed_data_list.push_back(temp[j]);
                }
                sequential_count++;

            }
            else{
                if (sequential_count > 0){
                    sequential_count=0;
                }
                else{
                    cleansed_data_list.push_back(temp[j]);
                }

            }
        }
        else{
            if(sequential_count==0){
                cleansed_data_list.push_back(temp[j]);
            }
        }
    }
    if (cleansed_data_list.size()==1){
        errors |= AlgErrorCodes::ROUTE_ONLY_ONE_VALID_PORT;
        return FAILURE;
    }
    if(cleansed_data_list.empty()){
        errors |= AlgErrorCodes::ROUTE_READ_FAIL_OR_FILE_EMPTY;
        return FAILURE;

    }
    return  SUCCESS;
}


int FileParser ::read_Port_file(const string &file_path, Port& port) {
    int errors=SUCCESS;
    vector<vector<string>> data_list;
    std::ifstream file;
    file.open(file_path);
    string cur_port_symbol,dest_port_symbol,container_ID;;
    vector<Container> cargo;
    int container_weight;
    if (!file.is_open()){
        errors |= AlgErrorCodes::CONTAINERS_READ_FAIL;
        return errors;
    }
    cur_port_symbol=get_port_symbol_from_file_name(extract_file_name_from_path(file_path));
    parse_file_to_vector(file,data_list);

    for (int i=0;i<(int)data_list.size();i++){
        if(data_list[i].size() != NUM_FIELDS_OF_CONTAINER_PORT){
            errors |= AlgErrorCodes::CONTAINER_WEIGHT ;
            continue;
        }
        try {
            container_ID = data_list[i][CONTAINER_ID_INDEX];
            if(container_ID.length()!=ISO_ID_SIZE){
                errors |= AlgErrorCodes::CONTAINER_ID;
                continue;
            }
            if (!Container :: verify_id(container_ID)){
                errors |= AlgErrorCodes::CONTAINER_ID_ISO;
            }
            dest_port_symbol = data_list[i][CONTAINER_DEST_INDEX];
            if (!Port::verify_seaport_code(dest_port_symbol)){
                errors |= AlgErrorCodes::CONTAINER_PORT_DEST;
            }
            if(!is_number(data_list[i][CONTAINER_WEIGHT_INDEX])){
                errors |= AlgErrorCodes::CONTAINER_WEIGHT;
                container_weight=BAD_WEIGHT;
            }
            else {
                container_weight = stoi(data_list[i][CONTAINER_WEIGHT_INDEX]);
            }
        }
        catch (const std::invalid_argument&){
            errors |= AlgErrorCodes::CONTAINER_WEIGHT;
            container_weight=BAD_WEIGHT;
        }
        if (container_id_already_loaded(cargo,container_ID)){
            errors |= AlgErrorCodes::CONTAINER_DUPLICATE_ID_ON_PORT;
        }
        cargo.push_back(Container(container_ID,dest_port_symbol,container_weight));

    }
    port=Port(cur_port_symbol,cargo);
    return  errors;

}


vector <string> FileParser :: split(std:: string line,char del){
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> entries;
    ss>>std :: ws;
    while (std::getline(ss>>std::ws, item, del)) {
        if ( item==EMPTY_STRING || only_white_spaces_string(item) ){
            continue;
        }
		int index_last=item.find_last_not_of(" ");
		item=item.substr(0,index_last+1);
		entries.push_back(item);
    }
    return entries;
}


void FileParser::parse_file_to_vector(std::ifstream& file, vector<vector<string> > &dataList) {

    std::string line = EMPTY_STRING;
    std::vector<string> splitted_line;
    // Iterate through each line and split the content using delimeter
    while (getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), CR_CHAR), line.end());
        //std::cout<<line<<std::endl;
        if (line.empty()){
            continue;
        }
        if (line.at(CONTAINER_ID_INDEX) == COMMENT_CHAR ) {//ignore notes in csv files
            continue;
        }
        splitted_line=split(line, COMMA_DEL);
        if(splitted_line.empty()){
            //NOTHING HERE
        }
        else{
            dataList.push_back(splitted_line);
        }
    }
    file.close();
}

void FileParser::display_error_message_to_file(const string message,int error_type_code) const {
    string error_filepath=output_folder_path+PATH_DELI+FOLDER_ERRORS;
    if(!directory_exists(error_filepath)){
        create_directory(error_filepath);
    }

    switch (error_type_code){
        case FATAL_ERROR_CODE :{
            std::lock_guard<std::mutex> guard(error_file_to_mutex->get_mutex_by_file_name(FATAL_ERRORS_FILE));

            write_line_to_file(output_folder_path + PATH_DELI + FOLDER_ERRORS + PATH_DELI + FATAL_ERRORS_FILE,
                               ERROR_WITH_BRACKETS + message);
            std::cout << ERROR_WITH_BRACKETS+message << std::endl;
            break;
        }
        case TRAVEL_ERROR_CODE : {
            std::lock_guard<std::mutex> guard(error_file_to_mutex->get_mutex_by_file_name(TRAVEL_ERRORS_FILE));
            write_line_to_file(output_folder_path + PATH_DELI + FOLDER_ERRORS + PATH_DELI + TRAVEL_ERRORS_FILE,
                               message);
            break;
        }
        case BAD_ALGORITHM_BEHAVIOR_CODE : {
            std::lock_guard<std::mutex> guard(this->error_file_to_mutex->get_mutex_by_file_name(BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE));
            write_line_to_file(
                    output_folder_path + PATH_DELI + FOLDER_ERRORS + PATH_DELI + BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE,
                    message);
            break;
        }
        case IGNORABLE_BAD_EXTERNAL_INPUT_CODE : {
            std::lock_guard<std::mutex> guard(this->error_file_to_mutex->get_mutex_by_file_name(IGNORABLE_ERRORS_FILE));
            write_line_to_file(output_folder_path + PATH_DELI + FOLDER_ERRORS + PATH_DELI + IGNORABLE_ERRORS_FILE,
                               message);
            break;
        }
        case ALGORITHM_ERROR_CODE : {
            std::lock_guard<std::mutex> guard(this->error_file_to_mutex->get_mutex_by_file_name(ALGORITHM_ERRORS_FILES));
            write_line_to_file(output_folder_path + PATH_DELI + FOLDER_ERRORS + PATH_DELI + ALGORITHM_ERRORS_FILES,
                               message);
            break;
        }

    }
}

string FileParser::get_port_symbol_from_file_name(const string file_name ) {
    std::string object_field;

        size_t lastindex = file_name.find_last_of("_");
        object_field = file_name.substr(0, lastindex);

    return object_field;

}
int FileParser::get_port_number_from_file_name(const string file_name) {
    std::string port_num_string;
    int port_num=-1;
    size_t last_index = file_name.find_last_of(UNDER_SCORE);
    port_num_string = file_name.substr(last_index+1);
    try {
        if(!is_number(port_num_string)){
            return FAILURE;
        }
        port_num = stoi(port_num_string);
        return port_num;
    }
    catch (std::exception& e){
        return FAILURE;
    }
}


string FileParser::extract_file_name_from_path(const string file_path) {
    string filename;
    int index = file_path.find_last_of(PATH_DELI);
    int last_index=file_path.find_last_of(".");
    if(last_index!=(int)string::npos) {
        size_t length = last_index - (index + 1);
        filename = file_path.substr(index + 1, length);
    }
    else{
        filename = file_path.substr(index+1);
    }
    return filename;
}


string FileParser::extract_folder_name_from_path(const string& file_path) {
    int index = file_path.find_last_of(PATH_DELI);
    if(index!=(int)string::npos) {
        return file_path.substr(index+1);
    }
    return EMPTY_STRING;
}


string FileParser::extract_file_name_without_type(const string& file_path) {
    size_t length = file_path.find_last_of(FILE_TYPE_STRING_SIGN);
    string filename = file_path.substr(FILE_NAME_START_INDEX,length);
    return filename;
}

string FileParser::extract_file_name_with_type(const string& file_path) {
    size_t last_index = file_path.find_last_of(PATH_DELI);
    string filename = file_path.substr(last_index+1);
    return filename;
}


int FileParser::build_travel(const string& folder_path, TravelFiles &travel) {
    ShipPlan ship_plan;
    vector<string> routes;
    vector<PortFile> cargo_files;
    vector<PortFile> ports;
    int ship_plan_val=SUCCESS,route_val=SUCCESS;
    string travel_name;
    vector <string> ship_plan_filepaths,routes_filepaths;
    string ship_plan_filepath,routes_filepath;
    travel_name=extract_file_name_from_path(folder_path);
    ship_plan_val=get_files_from_folder_by_type(folder_path, FILE_TYPE_SHIP_PLAN,ship_plan_filepaths);
    route_val=get_files_from_folder_by_type(folder_path, FILE_TYPE_ROUTES,routes_filepaths);

    if(ship_plan_val == FAILURE || route_val == FAILURE){
        build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, NO_ROUTE_OR_SHIP_PLAN_FILE,
                                            ERROR_TRAVEL_BAD_FIELDS,TRAVEL_ERROR_CODE );
        return FAILURE;
    }
    if(ship_plan_val==SUCCESS){
        if(ship_plan_filepaths.size()>1){
            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, "",
                                                WARNING_TOO_MANY_SHIP_PLAN_FILES, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
        }
        ship_plan_filepath=ship_plan_filepaths[INIT_VAL_ZERO];

    }
    if(route_val==SUCCESS){
        if(routes_filepaths.size()>1){
            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, "",
                                    WARNING_TOO_MANY_ROUTE_FILES, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);}
        routes_filepath=routes_filepaths[INIT_VAL_ZERO];
    }

    int read_route_errors,read_ship_plan_errors;
    read_route_errors=read_route_file(routes_filepath, routes);
    write_errors_to_travel_and_fixable_files_by_errors_code(read_route_errors,
                                                            routes_filepath, SIMULATOR_BRACKET,
                                                            extract_file_name_from_path(folder_path) );

    if (!AlgErrorCodes::is_cannot_run_error(read_route_errors)){
        read_ship_plan_errors=read_ship_plan_file(ship_plan_filepath, ship_plan);
        write_errors_to_travel_and_fixable_files_by_errors_code(
                read_ship_plan_errors,
                ship_plan_filepath,  SIMULATOR_BRACKET,
                extract_file_name_from_path(folder_path));

        if (!(AlgErrorCodes::is_cannot_run_error(read_ship_plan_errors))){
             if(scan_cargo_files_by_simulation(folder_path, routes, cargo_files) == SUCCESS){
                travel = TravelFiles(extract_file_name_from_path(folder_path), routes_filepath, ship_plan_filepath, cargo_files);//create object travel
                return SUCCESS;
             }
             else{
                 return FAILURE;
             }
        }
    }
    return FAILURE;
}


string FileParser::get_file_type(const string file_path) {
    string file_type;
    int index;
    index=file_path.find_last_of(".");
    if (index==(int)string::npos){
        file_type=EMPTY_STRING;
    }
    else{
        file_type=file_path.substr(index+1);
    }
    return file_type;
}

int FileParser::get_files_from_folder_by_type(const string folder_path, string type, vector<string>& file_paths) {
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    dp = opendir(folder_path.c_str());
    string file_type,semi_file_name,full_file_name=EMPTY_STRING;
    if (dp != nullptr) {
        while ((entry = readdir(dp))){
            semi_file_name=string(entry->d_name);
            file_type=get_file_type(string(entry->d_name));
            if (file_type==type){//the type that i asked for is in file name
                full_file_name=folder_path+PATH_DELI+semi_file_name;
                file_paths.push_back(full_file_name);
                //break;
            }
        }
    }
    closedir(dp);
    if(file_paths.empty()){
        return FAILURE;
    }
    return SUCCESS;
}

void FileParser::init_outputs_files() {
    vector<string> files;
    if(directory_exists(output_folder_path)) {
        string error_folder_path = output_folder_path + PATH_DELI + FOLDER_ERRORS;
        if (directory_exists(error_folder_path)) {
            files.push_back(error_folder_path + PATH_DELI + FATAL_ERRORS_FILE);
            files.push_back(error_folder_path + PATH_DELI + TRAVEL_ERRORS_FILE);
            files.push_back(error_folder_path + PATH_DELI + BAD_ALGORITHM_BEHAVIOR_ERRORS_FILE);
            files.push_back(error_folder_path + PATH_DELI + IGNORABLE_ERRORS_FILE);
            files.push_back(error_folder_path + PATH_DELI + ALGORITHM_ERRORS_FILES);
        }
        files.push_back(output_folder_path+PATH_DELI+OUTPUT_FILE_RESULTS);
        for (int index=0;index<(int)files.size();index++){
            if(is_file_exist(files[index])){
                create_file(files[index]);
            }
        }
    }

}

void FileParser::write_crane_instruction_to_file(const Instruction &instruction_performed, const string& crane_inst_filepath) {
    string instruction= instruction_performed.string_rep();
    write_line_to_file(crane_inst_filepath,instruction);

}

void FileParser::print_datalist(vector<vector<string>> &list) {
    for (int i=0;i<(int)list.size();i++){
        for (int j=0;j<(int)list[i].size();j++){
            std ::cout <<list[i][j]<<std::endl;
        }
    }
}

bool FileParser::only_white_spaces_string(const string &word) {
    for (int i=0;i<(int)word.length();i++) {
        if (word.at(i) != WHITE_SPACE_CHAR && int(word.at(i))!=TAB_ASCII_VALUE) {
            return false;
        }
    }
    return true;
}

bool FileParser::container_id_already_loaded(vector<Container> &cargo, const string &container_id) {
    if (cargo.empty()){
        return false;
    }
    for (int i=0;i<(int)cargo.size();i++){
        if (container_id==cargo[i].get_id()){
            return true;
        }
    }
    return false;
}

int FileParser::scan_cargo_files_by_simulation(const string &cargo_folder_path, vector<string> &routes, vector<PortFile> &ports) {
    struct dirent *entry = nullptr;
    PortFile port_file;
    string file_name,file_type,short_file_name;
    DIR *dp = nullptr;
    dp = opendir(cargo_folder_path.c_str());
    if (dp != nullptr) {
        while ((entry = readdir(dp))){
            file_name=string(entry->d_name);
            file_type=get_file_type(file_name);
            if (file_type==EMPTY_STRING){//aka - directory
                continue;
            }
            else{//file
                if(file_type == FILE_TYPE_CARGO_DATA){//port_file file
                    short_file_name= extract_file_name_without_type(file_name);
                    if(std::find(routes.begin(),routes.end(),get_port_symbol_from_file_name(short_file_name))!=routes.end()){//check that cargo data is in the route
                        if(get_port_number_from_file_name(short_file_name)>=1) {
                            port_file = PortFile();
                            if (SUCCESS ==
                                    scan_Port_file_by_simulation(cargo_folder_path + PATH_DELI + file_name, port_file,
                                                                 extract_file_name_from_path(cargo_folder_path))) {
                                ports.push_back(port_file);
                            }
                        }
                        else{
                            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER,
                                    extract_file_name_from_path(cargo_folder_path),
                                    file_name, WARNING_BAD_PORT_FILE_NAME,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                        }
                    }
                    else{
                        build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER,
                                extract_folder_name_from_path(cargo_folder_path), file_name,
                                WARNING_PORT_FILE_IS_NOT_IN_ROUTE, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                    }
                }
                else{
                    continue;
                }
            }
        }
    }
    closedir(dp);
    organize_Port_files_by_routes(routes, ports, cargo_folder_path);
    return  SUCCESS;

}

//need to make smaller
int FileParser::scan_Port_file_by_simulation(const string &file_path, PortFile &port_file, const string &travel_name) {
    vector<vector<string>> data_list;
    std::ifstream file;
    file.open(file_path);
    string cur_port_symbol;
    string file_name;
    int container_weight,visit_number;
    file_name=extract_file_name_from_path(file_path);
    if (!file.is_open()){
        build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,
                                                              file_name, WARNING_PORT_UNREADABLE, TRAVEL_ERROR_CODE);
    }
    vector<Container> cargo;
    string dest_port_symbol,container_ID;
    cur_port_symbol=get_port_symbol_from_file_name(extract_file_name_from_path(file_path));
    visit_number=int(get_port_number_from_file_name(extract_file_name_from_path(file_path)));
    parse_file_to_vector(file,data_list);
    for (int i=0;i<(int)data_list.size();i++){
        if(data_list[i].size() != NUM_FIELDS_OF_CONTAINER_PORT){
            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, file_name,
                                    WARNING_PORT_BAD_LINE_FORMAT, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            continue;
        }
        try {
            container_ID = data_list[i][CONTAINER_ID_INDEX];
            if (container_ID.length()!=ISO_ID_SIZE){
                build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,file_name,
                        container_ID,WARNING_CONTAINER_BAD_FORMAT_UNREADABLE_ID, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;
            }
            if (!Container :: verify_id(container_ID)){
                build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,
                        file_name,container_ID, WARNING_CONTAINER_ILLEGAL_ISO, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;
            }

            dest_port_symbol = data_list[i][CONTAINER_DEST_INDEX];

            if (!Port::verify_seaport_code(dest_port_symbol)){
                build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,file_name, container_ID,
                                                   WARNING_CONTAINER_BAD_FORMAT_DEST"- "+dest_port_symbol, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;//this container will not be added to cargo
            }
            if(!is_number(data_list[i][CONTAINER_WEIGHT_INDEX])){
                build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,file_name,
                                        container_ID,WARNING_CONTAINER_BAD_FORMAT_WEIGHT, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;
            }
            container_weight = stoi(data_list[i][CONTAINER_WEIGHT_INDEX]);
            if (container_weight<INIT_VAL_ZERO){
                build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,file_name, container_ID ,
                                                   WARNING_CONTAINER_BAD_FORMAT_WEIGHT,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                continue;
            }
        }
        catch (const std::invalid_argument&){
            build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,file_name, container_ID ,
                                                WARNING_CONTAINER_BAD_FORMAT_WEIGHT,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            continue;
        }
        if (!container_id_already_loaded(cargo,container_ID)){
            cargo.push_back(Container(container_ID,dest_port_symbol,container_weight));
        }
        else{//skip this container
            build_and_display_error_cargo_data(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,
                    file_name, container_ID,WARNING_SAME_CONTAINER_ID_AT_PORT, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            continue;
        }

    }
    port_file=PortFile(cur_port_symbol, file_path,extract_file_name_from_path(file_path),visit_number);
    return  SUCCESS;
}

int FileParser::load_command_line_parameters(int argc, const char **argv, int &num_threads) {
    int algorithm_path_index=DEFAULT_PARAM_INDEX,travel_path_index=DEFAULT_PARAM_INDEX,
        output_path_index=DEFAULT_PARAM_INDEX,num_threads_index=DEFAULT_PARAM_INDEX;
    load_current_directory();
    for (int index=1;index<argc-1;index++) {
        if(string(argv[index])==TRAVEL_PATH_PARAM) {
            travel_path_index=index+1;
        }
        else if (string(argv[index])==OUTPUT_PATH_PARAM){
            output_path_index=index+1;
        }
        else if (string(argv[index])==ALGORITHM_PATH_PARAM){
            algorithm_path_index=index+1;
        }
        else if(string(argv[index])==NUM_THREADS_PATH_PARAM){
            num_threads_index=index+1;
        }
    }
    if(output_path_index==DEFAULT_PARAM_INDEX){
        output_folder_path= cwd_folder_path;
        if(create_directory(output_folder_path)==FAILURE){
            //display_error_message_to_file(WARNING_BAD_OUTPUT_PATH_ARG+output_folder_path,FATAL_ERROR_CODE);
            display_error_message_to_file(WARNING_BAD_OUTPUT_PATH_ARG,FATAL_ERROR_CODE);
            return FAILURE;
        }
    }
    else {
        output_folder_path=argv[output_path_index];
        if(!directory_exists(output_folder_path)){
            output_folder_path= cwd_folder_path;
            std::cout<<WARNING_BAD_OUTPUT_PATH_ARG+output_folder_path<<std::endl;

        }
    }
    if (travel_path_index==DEFAULT_PARAM_INDEX){
        display_error_message_to_file(ERROR_MISSING_TRAVEL_PATH,FATAL_ERROR_CODE);
        return FAILURE;
    }
    else{
        travels_folder_path=argv[travel_path_index];
        if (!directory_exists(travels_folder_path)){
            display_error_message_to_file(ERROR_MISSING_TRAVEL_PATH,FATAL_ERROR_CODE);
            return FAILURE;
        }
    }
    if (algorithm_path_index==DEFAULT_PARAM_INDEX){
        algorithm_folder_path= cwd_folder_path;
        if (!directory_exists(algorithm_folder_path)){
            //display_error_message_to_file(WARNING_BAD_OUTPUT_PATH_ARG+output_folder_path,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            std::cout<<WARNING_BAD_ALGORITHM_PATH_ARG+algorithm_folder_path<<std::endl;
        }
    }
    else{
        algorithm_folder_path=argv[algorithm_path_index];
        if (!directory_exists(algorithm_folder_path)){
            algorithm_folder_path= cwd_folder_path;
            //display_error_message_to_file(WARNING_BAD_ALGORITHM_PATH_ARG+algorithm_folder_path,IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            std::cout<<WARNING_BAD_ALGORITHM_PATH_ARG+algorithm_folder_path<<std::endl;

        }
    }

    if (num_threads_index==DEFAULT_PARAM_INDEX){
        num_threads=1;
    }
    else{
        if(!is_number(argv[num_threads_index])){
            std::cout<<WARNING_PARAM_NUM_THREADS_NOT_NUMBER<<std::endl;
        }
        else {
            num_threads = std::stoi(argv[num_threads_index]);
            if(num_threads<=INIT_VAL_ZERO){
                std::cout<<WARNING_PARAM_NUM_THREADS_BAD_NUMBER<<std::endl;
                num_threads=1;
            }
        }
    }
    init_outputs_files();
    return SUCCESS;

}

void FileParser::load_current_directory() {
    int size=MAX_PATH_SIZE;
    char current_directory_path[MAX_PATH_SIZE];
    getcwd(current_directory_path,size);
    cwd_folder_path= string(current_directory_path);
    if (!directory_exists(cwd_folder_path)){
        create_directory(cwd_folder_path);
    }
}

int FileParser::create_directory(const string directory_path) const {
    int valid=0;
    struct stat info;
    if(stat (directory_path.c_str(), &info) == 0){
        return SUCCESS;
    }
//    #ifdef __linux__
        valid=mkdir(directory_path.c_str(), 0700);
//    #else //_WIN32
//        valid=mkdir(directory_path.c_str());
//    #endif
    if (valid==-1){
        return FAILURE;//broekn path
    }
    return SUCCESS;
}

bool FileParser::directory_exists(const string &directory_path) const {
    DIR * dp;
    dp=opendir(directory_path.c_str());
    if(dp== nullptr){
        return false;
    }

    closedir(dp);
    return true;
}

int FileParser::load_travels_for_simulation(vector<TravelFiles> &travels) {
    TravelFiles travel;
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;//convert string to char*
    dp = opendir(travels_folder_path.c_str());
    if(dp== nullptr){//travels folder not exists
        display_error_message_to_file(ERROR_TRAVEL_FOLDER_NOT_EXISTS,FATAL_ERROR_CODE);
        return FAILURE;
    }
    if (dp != nullptr) {
        while ((entry = readdir(dp))){
            if(string(entry->d_name).find(FILE_TYPE_STRING_SIGN) == string::npos && (string(entry->d_name)!="." && string(entry->d_name)!="..")){//we in directory
                travel=TravelFiles();
                if(build_travel(travels_folder_path + PATH_DELI + string(entry->d_name), travel) == SUCCESS){
                    travels.push_back(travel);
                }
            }
        }
    }
    if(travels.empty()){
        display_error_message_to_file(ERROR_NO_TRAVEL,TRAVEL_ERROR_CODE);
        closedir(dp);
        return SUCCESS;
    }
    closedir(dp);
    return SUCCESS;
}

void FileParser::organize_Port_files_by_routes(vector<string> &routes, vector<PortFile> &ports, const string &travel_path) {
    vector<PortFile> temp = vector<PortFile>(ports);
    ports = vector<PortFile>();
    Port port;
    string error_line=EMPTY_STRING,travel_name;
    string last_route = routes.back();
    std::map<string, int> map_port_to_num = std::map<string, int>();
    bool found_port = false;
    PortFile empty_port;
    string empty_port_filepath = travel_path + PATH_DELI + EMPTY_CARGO_DATA_FILE;
    int empty_port_visit_number;
    travel_name=extract_file_name_from_path(travel_path);
    for (int i = 0; i < (int) routes.size(); i++) {
        map_port_to_num[routes[i]] = 0;
    }
    for (int i = 0; i < (int) routes.size() ; i++) {
        for (int j = 0; j < (int) temp.size(); j++) {
            if (temp[j].get_seaport_code() == routes[i]) {
                if (map_port_to_num[routes[i]] == temp[j].get_visit_number() - 1) {//this port need to be added
                    map_port_to_num[routes[i]] += 1;
                    ports.push_back(temp[j]);
                    found_port = true;
                    if (i==(int)routes.size()-1){
                        port=Port();
                        read_Port_file(temp[j].get_file_path(),port);
                        if (!port.get_cargo().empty()){
                            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,
                                                                temp[j].get_file_name(),
                                                                WARNING_LAST_PORT_FILE_HAS_CONTAINERS,
                                                                IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                        }
                    }
                    else{
                        port=Port();
                        read_Port_file(temp[j].get_file_path(),port);
                        if (port.get_cargo().empty()){
                            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name,
                                                                temp[j].get_file_name(), WARNING_PORT_NO_CONTAINERS, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
                        }
                    }
                    break;
                }
            }
        }
        if (found_port)
        {
            found_port = false;
        }
        else {//port from route is not in ports vector read from files. simulation need to create empty_portfile
            empty_port_visit_number = map_port_to_num[routes[i]] + 1;
            create_file(empty_port_filepath);
            empty_files_paths.push_back(empty_port_filepath);
            ports.push_back(PortFile(routes[i], empty_port_filepath, EMPTY_CARGO_DATA_FILE, empty_port_visit_number));
            map_port_to_num[routes[i]] += 1;
            if(i!=(int)routes.size()-1) {
                build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, routes[i] +
                                                                                                     " visit number:" +
                                                                                                     to_string(
                                                                                                             map_port_to_num[routes[i]]),
                                                    WARNING_PORT_NO_CONTAINERS, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
            }
        }
    }

    for (int index = 0; index < (int) temp.size(); index++) {
        if (temp[index].get_visit_number() > map_port_to_num[temp[index].get_seaport_code()]&&temp[index].get_file_name()!=EMPTY_CARGO_DATA_FILE) {
            build_and_display_error(SIMULATOR_BRACKET, WARNING__HEADER, travel_name, temp[index].get_file_name(),
                                                WARNING_EXTRA_PORT_FILE, IGNORABLE_BAD_EXTERNAL_INPUT_CODE);
        }

    }
}

void FileParser::write_simulation_results_file(const vector<string>& results) {
    for(int i=0;i<(int)results.size();i++){
        write_line_to_file(output_folder_path+PATH_DELI+OUTPUT_FILE_RESULTS,results[i]);
    }
}

int FileParser::create_file(const string &filepath) {
    std :: ofstream file;
    file.open(filepath,std::ios::trunc);
    if(!file){
        return  FAILURE;
    }
    file.close();
    return SUCCESS;
}

const string FileParser::get_instructions_output_file(const string &travel_name, const string &algorithm_name,
                                                       const PortFile &cargo_port_file) {

    string algo_port_instructions_filename=EMPTY_STRING,folder_name,algo_travel_folderpath;
    folder_name=algorithm_name+UNDER_SCORE+travel_name+UNDER_SCORE+FOLDER_CRANE_INSTRUCTIONS;
    algo_travel_folderpath=output_folder_path+PATH_DELI+folder_name;
    create_directory(algo_travel_folderpath);
    algo_port_instructions_filename=algo_travel_folderpath+PATH_DELI
            +cargo_port_file.get_seaport_code()+UNDER_SCORE+to_string(cargo_port_file.get_visit_number())+
            "."+string(FILE_TYPE_INSTRUCTIONS);
    create_file(algo_port_instructions_filename);
    return algo_port_instructions_filename;

}

void FileParser::write_line_to_file(const string &filepath, const string &message) {
    std :: ofstream output_file;
    std::fstream fs;
    if(!is_file_exist(filepath)){
        create_file(filepath);
    }
    output_file.open(filepath,std::ios::app);//append mode
    output_file << message<<std::endl;
    output_file.close();
}

bool FileParser::is_file_exist(const string& file_path) {
    struct stat buffer;
    if (stat (file_path.c_str(), &buffer) == 0){
        return true;
    }
    return false;
}

void
FileParser::write_errors_to_travel_and_fixable_files_by_errors_code(int errors, const string &file_path, string context,
                                                                    string travel_name)
{
    write_error_message_by_error_codes(errors, extract_file_name_from_path(file_path), TRAVEL_ERROR_CODE, context, travel_name);
    write_warning_message_by_error_codes(errors, extract_file_name_from_path(file_path), IGNORABLE_BAD_EXTERNAL_INPUT_CODE,
                                         context, travel_name);
}

void
FileParser::write_error_message_by_error_codes(int errors, const string &file_path, int error_type,const string &context,
                                               const string &travel_name) {

	for (AlgErrorCodes::Value error_code : {
			AlgErrorCodes::SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL,
			AlgErrorCodes::SHIP_PLAN_DUPLICATE_XY,
			AlgErrorCodes::ROUTE_READ_FAIL_OR_FILE_EMPTY,
			AlgErrorCodes::ROUTE_ONLY_ONE_VALID_PORT
	})
	{
		if (int pure_errors = errors & error_code)
		{
            build_and_display_error(context, ERROR_HEADER, travel_name, file_path,
                                                    AlgErrorCodes::get_message(
                                                            static_cast<AlgErrorCodes::Value>(pure_errors)), error_type);
		}
	}
}

void FileParser::write_warning_message_by_error_codes(int errors, const string &file_path, int error_type,
                                                      const string &context, const string &travel_name) {

	for (AlgErrorCodes::Value error_code : {
			AlgErrorCodes::SHIP_PLAN_ILLEGAL_NUM_FLOORS,
			AlgErrorCodes::SHIP_PLAN_XY_IDX,
			AlgErrorCodes::SHIP_PLAN_BAD_POS_FORMAT,
			AlgErrorCodes::ROUTE_DUPLICATE_CONSECUTIVE_PORT,
			AlgErrorCodes::ROUTE_ILLEGAL_PORT_SYMBOL,
			AlgErrorCodes::CONTAINER_DUPLICATE_ID_ON_PORT,
			AlgErrorCodes::CONTAINER_ID_ALREADY_ON_SHIP,
			AlgErrorCodes::CONTAINER_WEIGHT,
			AlgErrorCodes::CONTAINER_PORT_DEST,
			AlgErrorCodes::CONTAINER_ID,
			AlgErrorCodes::CONTAINER_ID_ISO,
			AlgErrorCodes::CONTAINERS_READ_FAIL,
			AlgErrorCodes::CONTAINERS_LAST_PORT_HAS_CONTAINERS,
			AlgErrorCodes::CONTAINERS_EXCEEDS_CAPACITY
	})
	{
		if (int pure_errors = errors & error_code)
		{
		    build_and_display_error(context, WARNING__HEADER, travel_name,
                                                    extract_file_name_from_path(file_path), AlgErrorCodes::get_message(
                            static_cast<AlgErrorCodes::Value>(pure_errors)), error_type);
		}
	}

}

const string &FileParser::get_cwd_folder_path() const {
    return cwd_folder_path;
}

const string &FileParser::get_travels_folder_path() const {
    return travels_folder_path;
}

const string &FileParser::get_output_folder_path() const {
    return output_folder_path;
}

const string &FileParser::get_Algorithm_folder_path() const {
    return algorithm_folder_path;
}

void FileParser::read_crane_instructions_file(const string &file_path, vector<Instruction>& instructions) {
    vector<vector<string> > data_list;
    std::ifstream file;
    file.open(file_path);
    bool load_instruction=false;
    if (!file.is_open()){
        return;
    }
    parse_file_to_vector(file, data_list);
    for (int i=0;i<(int)data_list.size();i++){
        Instruction instruction=Instruction();
        if (data_list[i].size()>=INSTRUCTION_REJECT_LINE_SIZE) {
            convert_instruction_type(instruction, data_list[i][INSTRUCTION_TYPE_INDEX]);
            instruction.set_container_id(data_list[i][INSTRUCTION_CONTAINER_ID_INDEX]);
            if(instruction.get_type()== Instruction::Type::reject){
                load_instruction=true;

            }
            else if (data_list[i].size() >= INSTRUCTION_LINE_SIZE) {
                instruction.set_floor_height(stoi(data_list[i][INSTRUCTION_FLOOR_INDEX]));
                instruction.set_row_idx(stoi(data_list[i][INSTRUCTION_X_INDEX]));
                instruction.set_col_idx(stoi(data_list[i][INSTRUCTION_y_index]));
                if (data_list[i].size() == INSTRUCTION_MOVE_LINE_SIZE &&
                    instruction.get_type() == Instruction::Type::move) {
                    instruction.set_move_dest_floor_height(stoi(data_list[i][INSTRUCTION_MOVE_FLOOR_INDEX]));
                    instruction.set_move_dest_row_idx(stoi(data_list[i][INSTRUCTION_MOVE_X_INDEX]));
                    instruction.set_move_dest_col_idx(stoi(data_list[i][INSTRUCTION_MOVE_Y_INDEX]));

                }
                load_instruction = true;
            }
        }
        if(load_instruction){
            instructions.push_back(instruction);
        }
        else{
            load_instruction=false;
        }
    }

}

void FileParser::convert_instruction_type(Instruction& instruction,const string& type){
    if (type==L_INSTRUCTION){
        instruction.set_type(Instruction::Type::load);
    }
    else if (type==U_INSTRUCTION){
        instruction.set_type(Instruction::Type::unload);
    }
    else if (type==R_INSTRUCTION){
        instruction.set_type(Instruction::Type::reject);
    }
    else if (type==M_INSTRUCTION){
        instruction.set_type(Instruction::Type::move);
    }
    return;
}

void FileParser::write_instruction_vector_to_file(const vector<Instruction> &instructions, const string &crane_inst_filepath) {
    for (int i=0;i<(int)instructions.size();i++){
        write_crane_instruction_to_file(instructions[i],crane_inst_filepath);
    }
}

void FileParser::write_simulation_results_file(std::map<string,TravelsResults>& statistics,const vector<TravelFiles>& travels){
    vector<string> algorithms;
    vector<std::pair<string,TravelsResults>> sorted_results;
    string result_line;
    TravelStats stats;
    result_line=RESULTS_HEADER;
    string simulation_results_file_path=this->output_folder_path+PATH_DELI+OUTPUT_FILE_RESULTS;
    create_file(simulation_results_file_path);
    sort_simulation_vector_results(statistics,sorted_results);

    //write first line of results file
    for (int i=0;i<(int)travels.size();i++){
        result_line=result_line+ " , "+travels[i].name;
    }
    result_line =result_line+" , "+SUM_HEADER+ " , "+NUM_ERRORS_HEADER;
    write_line_to_file(simulation_results_file_path,result_line);
    //write results lines
    for (int i=0;i<(int)sorted_results.size();i++){
        result_line=sorted_results[i].first;
        for (int j=0;j<(int)travels.size();j++){
            stats=sorted_results[i].second.get_travel_name_stats_map()[travels[j].name];
            result_line=result_line+ " , " +to_string(stats.get_total_operations());
        }
        result_line=result_line+" , "+to_string(sorted_results[i].second.getSumOperations())+ " , " +
                    to_string(sorted_results[i].second.getSumErrors());
        write_line_to_file(simulation_results_file_path,result_line);
    }
}
bool FileParser::sort_by_travels_results(const std::pair<string,TravelsResults> &a,
               const std::pair<string,TravelsResults> &b)
{

        return a.second.compare(b.second);
}

void FileParser::sort_simulation_vector_results(std::map<string, TravelsResults> &statistics,vector<std::pair<string,TravelsResults>>& sorted_results) {
    for (auto it = statistics.begin(); it != statistics.end(); ++it){
        //algorithms.push_back(it->first);
        sorted_results.emplace_back(std::pair<string,TravelsResults>(it->first,it->second));
    }
    sort(sorted_results.begin(), sorted_results.end(), sort_by_travels_results);
}


void FileParser::find_so_files(vector<string> &so_paths) {
    get_files_from_folder_by_type(algorithm_folder_path,FILE_TYPE_SO,so_paths);
}

bool FileParser::is_number(const string &word) {
    if( word.empty()){
        return false;
    }
    for (int index=0;index<(int)word.size();index++){
        if (word.at(index)<'0' || word.at(index)>'9'){
            return false;
        }
    }
    return true;
}

bool FileParser::valid_port_file_name(const string &seaport, int &number) {
    Port port;
    return ( port.verify_seaport_code(seaport)&& number>=1);

}


void FileParser::remove_empty_file() {
    for (int i=0;i<(int)this->empty_files_paths.size();i++){
        if(is_file_exist(empty_files_paths[i])){
            remove(empty_files_paths[i].c_str());
        }
    }
}

string FileParser::string_brackets_cover(string word) const {
    word="["+word+"]";
    return word;
}

void FileParser::build_and_display_error(const string &context, const string &error_type, const string &travel_name,
										 const string &file_name, const string &message, const int error_code) const {
    string error_line;
    error_line+=string_brackets_cover(context)+string_brackets_cover(error_type)+
           string_brackets_cover(TRAVEL_NAME+travel_name)+ string_brackets_cover(FILE_NAME+file_name)+message;

    display_error_message_to_file(error_line,error_code);
}

void FileParser::build_and_display_error_cargo_data(const string &context, const string &error_type, const string &travel_name,
                                         const string &file_name, const string& container_id,const string &message, const int error_code) const {
    string message_prefix=EMPTY_STRING;
    message_prefix=message_prefix+"[ID :"+container_id+"]";
    build_and_display_error(context,error_type,travel_name,file_name,message_prefix+message,error_code );
}













