//
// Created by Owner on 16-May-20.
//

#ifndef EX2_ADV_CPP__308570647_A_H
#define EX2_ADV_CPP__308570647_A_H

#include "StowageAlgorithm.h"

class _308570647_a: public StowageAlgorithm
{
public:
	~_308570647_a() override = default;

protected:
	void order_containers_for_load(
			vector<Container> &containers_to_load) const override;

};


#endif //EX2_ADV_CPP__308570647_A_H
