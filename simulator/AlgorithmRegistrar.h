//
// Created by Owner on 12-May-20.
//

#ifndef EX2_ADV_CPP_ALGORITHMREGISTRAR_H
#define EX2_ADV_CPP_ALGORITHMREGISTRAR_H
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <dlfcn.h>
#include "../common/FileParser.h"
#include "../interfaces/AbstractAlgorithm.h"


using std::unique_ptr;
using std::string;
using std::map;
using std::pair;
using std::vector;

class AlgorithmRegistrar
{
	friend class AlgorithmRegistration;

	struct DlCloser
	{
		void operator()(void *dlhandle) const noexcept;
	};

public:
	typedef std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmFactory;
	typedef std::unique_ptr<void, DlCloser> DlHandler;

private:
	// AlgorithmRegistration needs to internally registerFactory
//	static AlgorithmRegistrar algorithm_registrar;

	/*
	 * The registrar has a vector of algorithm factories
	 * And a vector of algorithm names.
	 * The user has to call add_name after registering an algorithm
	 * to make sure the indices match.
	 */
	vector<AlgorithmFactory> algorithm_factories;
	vector<string> algorithm_names;

	// Dynamic-linker handles map (to keep algorithms code in memory)
	std::map<std::string, DlHandler> dl_handles;

	// Default Constructor
	AlgorithmRegistrar() = default;
	~AlgorithmRegistrar();

	void register_algorithm_factory(AlgorithmFactory algorithm_factory);

public:

	const vector<AlgorithmFactory> &get_algorithm_factories() const;

	const AlgorithmFactory& get_factory_by_idx(int idx);

	const vector<string> &get_algorithm_names() const;
	const string& get_algorithm_name_by_idx(int idx);

	int get_num_algorithms() const;

	static AlgorithmRegistrar& get_instance();

	void add_algorithm_name(string name);


	// Prevent copy - this is singleton
	AlgorithmRegistrar(const AlgorithmRegistrar&) = delete;
	AlgorithmRegistrar& operator=(const AlgorithmRegistrar&) = delete;

	// Loads an algorithm from given shared object file into the registrar
	// parameters:
	//   file_path - shared object file path to load
	//   error - On failure, it will be filled with a Human-readable error message.
	// Returns:
	//   true if given shared object's algorithm was registered successfully, false oterwise
	bool load_algorithm_from_file(const char *file_path, std::string& error);
};


#endif //EX2_ADV_CPP_ALGORITHMREGISTRAR_H
