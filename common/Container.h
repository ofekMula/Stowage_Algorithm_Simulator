//
// Created by Owner on 17-May-20.
//

#ifndef EX2_ADV_CPP_CONTAINER_H
#define EX2_ADV_CPP_CONTAINER_H
#include <string>
#include <stdexcept>
#include <math.h>

#define ISO_ID_SIZE 11
#define OWNER_CODE_INDEX 0
#define CATEGORY_ID_INDEX 3
#define SERIAL_NUMBER_INDEX 4
#define CHECK_DIGIT_INDEX 10
#define CHECK_DIGIT_LEN 1
#define OWNER_CODE_CHAR_SPECIAL_A 'A'
#define OWNER_CODE_CHAR_SPECIAL_B 'B'
#define OWNER_CODE_CHAR_SPECIAL_K 'K'
#define OWNER_CODE_CHAR_SPECIAL_L 'L'
#define OWNER_CODE_CHAR_SPECIAL_U 'U'
#define OWNER_CODE_CHAR_SPECIAL_V 'V'
#define OWNER_CODE_CHAR_MIN 'A'
#define OWNER_CODE_CHAR_MAX 'Z'
#define CATEGORY_ID_SIGN1 "U"
#define CATEGORY_ID_SIGN2 "J"
#define CATEGORY_ID_SIGN3 "Z"
#define A_NUMERICAL_VALUE 10
#define B_NUMERICAL_VALUE 12
#define L_NUMERICAL_VALUE 23
#define V_NUMERICAL_VALUE 34
#define MULTIPLE_VAL 11
#define ASCII_VAL_0 48
#define FINAL_DIFFERENCE 10
#define INIT_CHECK_DIGIT 0

using std::string;

class Container
{
	string identifier;
	string dest_port_code;
	int weight;

public:
	inline const string &get_dest_port() const {return dest_port_code;}
	inline int get_weight() const {return weight;}
	inline const string &get_id() const {return identifier;}

	Container() = default;
	Container(string p_identifier, string p_destination_port_code, int p_weight);
	static bool verify_id(const string& id);

	// Non-static overloaded wrapper for static version
	bool verify_id() const;
};


#endif //EX2_ADV_CPP_CONTAINER_H
