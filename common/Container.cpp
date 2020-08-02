//
// Created by Owner on 17-May-20.
//


#include "Container.h"

Container::Container(string p_identifier, string p_destination_port_code, int p_weight)
		: identifier(p_identifier), dest_port_code(p_destination_port_code), weight(p_weight){}

bool Container::verify_id(const string &id) {
	string owner_code,category_id,serial_number,check_digit;
	owner_code=id.substr(OWNER_CODE_INDEX,CATEGORY_ID_INDEX);
	category_id=id.substr(CATEGORY_ID_INDEX,SERIAL_NUMBER_INDEX-CATEGORY_ID_INDEX);
	serial_number=id.substr(SERIAL_NUMBER_INDEX,CHECK_DIGIT_INDEX-SERIAL_NUMBER_INDEX);
	check_digit=id.substr(CHECK_DIGIT_INDEX,CHECK_DIGIT_LEN);

	//check owner code
	for (int i=0;i<(int)owner_code.length();i++){
		if (owner_code.at(i) < OWNER_CODE_CHAR_SPECIAL_A || owner_code.at(i) > OWNER_CODE_CHAR_MAX) {
			return false;
		}
	}
	//check category identifier
	if(category_id!=CATEGORY_ID_SIGN1 && category_id!=CATEGORY_ID_SIGN2 && category_id!=CATEGORY_ID_SIGN3) {
		return false;
	}
	//check serial number and check digit
	try {
		stoi(serial_number);
		stoi(check_digit);
	}
	catch (const std::invalid_argument&){
		return false;
	}

	string word;
	word=id;
	int sum=0, val=0,digit=0,sum_after_division,serial_digit=0;
	//calculate numerical values
	for (int index=0;index<(int)word.size()-1;index++) {
		if (word.at(index) >= OWNER_CODE_CHAR_MIN && word.at(index) <= OWNER_CODE_CHAR_MAX) {
			if (word.at(index) == OWNER_CODE_CHAR_SPECIAL_A) {//first set
				val = A_NUMERICAL_VALUE * ((int)pow(2,index));
			}
			else if (word.at(index) >= OWNER_CODE_CHAR_SPECIAL_B && word.at(index) <= OWNER_CODE_CHAR_SPECIAL_K) {
				val = ((B_NUMERICAL_VALUE)+(word.at(index)-OWNER_CODE_CHAR_SPECIAL_B) )*((int)pow(2,index));
			}
			else if (word.at(index) >= OWNER_CODE_CHAR_SPECIAL_L && word.at(index) <= OWNER_CODE_CHAR_SPECIAL_U) {
				val = ((L_NUMERICAL_VALUE)+(word.at(index)-OWNER_CODE_CHAR_SPECIAL_L) )* ((int)pow(2,index));

			}
			else if (word.at(index) >= OWNER_CODE_CHAR_SPECIAL_V && word.at(index) <= OWNER_CODE_CHAR_MAX) {
				val = ((V_NUMERICAL_VALUE)+(word.at(index)-OWNER_CODE_CHAR_SPECIAL_V) )* ((int)pow(2,index));
			}
		}
		else{
			serial_digit=int(word.at(index))-ASCII_VAL_0;
			val = serial_digit*((int)pow(2,index));
		}
		sum += val;
	}

	sum_after_division=(int(sum/MULTIPLE_VAL))*MULTIPLE_VAL;
	digit=sum-sum_after_division;
	if(digit==FINAL_DIFFERENCE){
		digit=INIT_CHECK_DIGIT;
	}
	if(digit!=stoi(check_digit)){
		return false;
	}
	//std::cout<<id<<std::endl;
	return true;
}

bool Container::verify_id() const
{
	return verify_id(identifier);
}
