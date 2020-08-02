//
// Created by Owner on 17-May-20.
//

#ifndef EX2_ADV_CPP_ALGERRORCODES_H
#define EX2_ADV_CPP_ALGERRORCODES_H

//**************************ERRORS AND WARNINGS MESSAGES FOR ERROR CODES*******************************//
#define ERROR_SHIP_PLAN_BAD_FIRST_LINE_OR_UNREADABLE "[ship_plan] bad first line or file cannot be read altogether (cannot run this travel)"
#define ERROR_SHIP_PLAN_DUP_XY_APPEARANCE "[ship_plan] duplicate x,y appearance with different data (cannot run this travel)"
#define ERROR_ROUTE_EMPTY_OR_UNREADABLE "[route] travel error- empty file or file cannot be read altogether (cannot run this travel)"
#define ERROR_ROUTE_SINGLE_PORT "[route] file with only a single valid port (cannot run this travel)"
#define WARNING_SHIP_PLAN_POS_FLOORS_LIMIT " [ship_plan] a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)"
#define WARNING_SHIP_PLAN_XY_LIMIT "[ship_plan] a given position exceeds the X/Y ship limits (ignored)"
#define WARNING_SHIP_PLAN_POS_DESCRIPTION "[ship_plan] bad line format after first line or duplicate x,y appearance with same data (ignored)"
#define WARNING_ROUTE_PORT_APPEARS_CONSECUTIVELY "[route] a port appears twice or more consecutively (ignored)"
#define WARNING_BAD_SYMBOL_FORMAT "[route] bad port symbol format (ignored)"

#define WARNING_SAME_CONTAINER_ID_AT_PORT "[container] duplicate ID on port (ID rejected)"
#define WARNING_CONTAINER_BAD_FORMAT_WEIGHT "[container] bad line format, missing or bad weight (ID rejected)"
#define WARNING_CONTAINER_BAD_FORMAT_DEST "[container] bad line format, missing or bad port dest (ID rejected)"
#define WARNING_CONTAINER_BAD_FORMAT_UNREADABLE_ID "[container] bad line format, ID cannot be read (ignored)"
#define WARNING_CONTAINER_ILLEGAL_ISO "[container] illegal ID check ISO 6346 (ID rejected)"
#define WARNING_CONTAINER_ID_ALREADY_ON_SHIP "[container] ID already on ship (ID rejected)"
#define WARNING_PORT_UNREADABLE "[cargo_data] file cannot be read altogether (assuming no cargo to be loaded at this port)"
#define WARNING_LAST_PORT_HAS_CONTAINERS "[cargo_data] last port has waiting containers (ignored) "
#define WARNING_TOTAL_CONTAINERS_EXCEEDS_SHIP_CAPACITY "[cargo_data] total containers amount exceeds ship capacity (rejecting far containers)"

#include <string>

using std::string;

class AlgErrorCodes
{
public:
	enum Value
	{
		// Ship plan
		// ignored
		SHIP_PLAN_ILLEGAL_NUM_FLOORS		= 1<<0 ,
		SHIP_PLAN_XY_IDX 					= 1<<1 ,
		SHIP_PLAN_BAD_POS_FORMAT		 				= 1 << 2 ,
		// cannot run with this ship plan - fatal
		SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL			= 1 << 3 ,
		SHIP_PLAN_DUPLICATE_XY				= 1<<4 ,
		// travel route
		// ignored
		ROUTE_DUPLICATE_CONSECUTIVE_PORT	= 1<<5 , // 2<<4 reserved
		ROUTE_ILLEGAL_PORT_SYMBOL			= 1<<6 ,
		// cannot run this travel - fatal
		ROUTE_READ_FAIL_OR_FILE_EMPTY		= 1<<7 ,
		ROUTE_ONLY_ONE_VALID_PORT			= 1<<8 ,

		// containers at port
		// ID rejected
		CONTAINER_DUPLICATE_ID_ON_PORT		= 1<<10, // 2<<9 reserved
		CONTAINER_ID_ALREADY_ON_SHIP		= 1<<11,
		CONTAINER_WEIGHT					= 1<<12,
		CONTAINER_PORT_DEST					= 1<<13,
		CONTAINER_ID						= 1<<14,
		CONTAINER_ID_ISO					= 1<<15,
		CONTAINERS_READ_FAIL				= 1<<16,
		CONTAINERS_LAST_PORT_HAS_CONTAINERS	= 1<<17,
		CONTAINERS_EXCEEDS_CAPACITY			= 1<<18
	};


	inline static bool is_cannot_run_error(int errors)
	{
		return (errors & ROUTE_READ_FAIL_OR_FILE_EMPTY)
			   || (errors & ROUTE_ONLY_ONE_VALID_PORT)
			   || (errors & SHIP_PLAN_BAD_FIRST_LINE_OR_READ_FAIL)
			   || (errors & SHIP_PLAN_DUPLICATE_XY);
	}

	static string get_message(Value value);
};

#endif //EX2_ADV_CPP_ALGERRORCODES_H
