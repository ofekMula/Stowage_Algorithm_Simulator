COMP = g++-9.3.0

COMMON_OBJS = FileParser.o Ship.o  Container.o Instruction.o AlgErrorCodes.o ShipPlan.o Port.o PortFile.o TravelFiles.o ErrorFilesMutex.o #WeightBalanceCalculator.o
OBJS = main.o  Simulation.o AlgorithmRegistration.o AlgorithmRegistrar.o TravelStats.o TravelsResults.o ThreadPoolExecutor.o WeightBalanceCalculator.o $(COMMON_OBJS)
EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -g
CPP_LINK_FLAG = -ldl -export-dynamic -pthread #(-rdynamic)

COMMON_HEADERS = ../common/common.h ../common/Ship.h ../interfaces/AbstractAlgorithm.h ../interfaces/WeightBalanceCalculator.h ../interfaces/AlgorithmRegistration.h ../common/FileParser.h ../common/AlgErrorCodes.h ../common/Container.h ../common/Instruction.h ../common/ShipPlan.h ../common/Port.h ../common/PortFile.h ../common/TravelFiles.h ../common/ErrorFilesMutex.h
ALL_HEADERS = $(COMMON_HEADERS) Simulation.h AlgorithmRegistrar.h TravelStats.h TravelsResults.h ThreadPoolExecutor.h

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@ 
	
AlgorithmRegistrar.o: AlgorithmRegistrar.cpp AlgorithmRegistrar.h ../interfaces/AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

AlgorithmRegistration.o: AlgorithmRegistration.cpp ../interfaces/AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

FileParser.o: ../common/FileParser.cpp $(COMMON_HEADERS)
	$(COMP) $(CPP_COMP_FLAG) -c $<

Ship.o: ../common/Ship.cpp ../common/Ship.h ../common/common.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

WeightBalanceCalculator.o: WeightBalanceCalculator.cpp ../interfaces/WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

#Change to all headers?
Simulation.o: Simulation.cpp Simulation.h TravelStats.h TravelsResults.h $(COMMON_HEADERS)
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

TravelStats.o: TravelStats.cpp TravelStats.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

TravelsResults.o: TravelsResults.cpp TravelsResults.h TravelStats.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

ThreadPoolExecutor.o: ThreadPoolExecutor.cpp ThreadPoolExecutor.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

ErrorFilesMutex.o: ../common/ErrorFilesMutex.cpp ../common/ErrorFilesMutex.h
	$(COMP) $(CPP_COMP_FLAG) -c $<
	
Container.o: ../common/Container.cpp ../common/Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

Instruction.o: ../common/Instruction.cpp ../common/Instruction.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

ShipPlan.o: ../common/ShipPlan.cpp ../common/ShipPlan.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

AlgErrorCodes.o: ../common/AlgErrorCodes.cpp ../common/AlgErrorCodes.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

Port.o: ../common/Port.cpp ../common/Port.h ../common/common.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

PortFile.o: ../common/PortFile.cpp ../common/PortFile.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

TravelFiles.o: ../common/TravelFiles.cpp ../common/TravelFiles.h ../common/PortFile.h
	$(COMP) $(CPP_COMP_FLAG) -c $<

#use g++ -MM main.cpp to see dependencies
main.o: main.cpp $(ALL_HEADERS)
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
