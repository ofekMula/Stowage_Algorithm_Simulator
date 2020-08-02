COMP = g++-9.3.0

SHARED_OBJS = _308570647_a.so _308570647_b.so
COMMON_OBJS = FileParser.o Ship.o Container.o Instruction.o AlgErrorCodes.o ShipPlan.o Port.o PortFile.o TravelFiles.o ErrorFilesMutex.o #WeightBalanceCalculator.o
OBJS = StowageAlgorithm.o $(COMMON_OBJS)
OBJS_A = _308570647_a.o $(OBJS)
OBJS_B = _308570647_b.o $(OBJS)
ALL_OBJS = _308570647_b.o $(OBJS_A)
COMMON_HEADERS = ../common/common.h ../common/Ship.h ../interfaces/AbstractAlgorithm.h ../interfaces/WeightBalanceCalculator.h ../interfaces/AlgorithmRegistration.h ../common/FileParser.h ../common/AlgErrorCodes.h ../common/Container.h ../common/Instruction.h ../common/ShipPlan.h ../common/Port.h ../common/PortFile.h ../common/TravelFiles.h ../common/ErrorFilesMutex.h
ALL_HEADERS = $(COMMON_HEADERS) StowageAlgorithm.h

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden -g
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

_308570647_a.so: $(OBJS_A)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_308570647_b.so: $(OBJS_B)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_308570647_a.o: _308570647_a.cpp $(ALL_HEADERS) 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 

_308570647_b.o: _308570647_b.cpp $(ALL_HEADERS) 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 

FileParser.o: ../common/FileParser.cpp $(COMMON_HEADERS)
	$(COMP) $(CPP_COMP_FLAG) -c $<

Ship.o: ../common/Ship.cpp ../common/Ship.h ../common/common.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

#WeightBalanceCalculator.o: ../common/WeightBalanceCalculator.cpp ../interfaces/WeightBalanceCalculator.h
#	$(COMP) $(CPP_COMP_FLAG) -c $<

StowageAlgorithm.o: StowageAlgorithm.cpp StowageAlgorithm.h $(COMMON_HEADERS)
	$(COMP) $(CPP_COMP_FLAG) -c $<

Container.o: ../common/Container.cpp ../common/Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

Instruction.o: ../common/Instruction.cpp ../common/Instruction.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

ShipPlan.o: ../common/ShipPlan.cpp ../common/ShipPlan.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

ErrorFilesMutex.o: ../common/ErrorFilesMutex.cpp ../common/ErrorFilesMutex.h
	$(COMP) $(CPP_COMP_FLAG) -c $<
	
AlgErrorCodes.o: ../common/AlgErrorCodes.cpp ../common/AlgErrorCodes.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

Port.o: ../common/Port.cpp ../common/Port.h ../common/common.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

PortFile.o: ../common/PortFile.cpp ../common/PortFile.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

TravelFiles.o: ../common/TravelFiles.cpp ../common/TravelFiles.h ../common/PortFile.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

clean:
	rm -f $(ALL_OBJS) $(SHARED_OBJS)
