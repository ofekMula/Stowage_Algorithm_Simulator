//
// Created by Owner on 12-May-20.
//

#include "AlgorithmRegistrar.h"
using AlgorithmFactory = AlgorithmRegistrar::AlgorithmFactory;

void AlgorithmRegistrar::DlCloser::operator()(void *dlhandle) const noexcept
{
	dlclose(dlhandle);
}

AlgorithmRegistrar::~AlgorithmRegistrar()
{
	// Remove all factories - before closing all handles!
	algorithm_factories.clear();
	dl_handles.clear();
}

void AlgorithmRegistrar::register_algorithm_factory(AlgorithmFactory algorithm_factory)
{
	algorithm_factories.push_back(algorithm_factory);
}

void AlgorithmRegistrar::add_algorithm_name(string name)
{
	algorithm_names.push_back(name);
}

const vector<AlgorithmFactory>&AlgorithmRegistrar::get_algorithm_factories() const
{
	return algorithm_factories;
}

const vector<string>& AlgorithmRegistrar::get_algorithm_names() const
{
	return algorithm_names;
}

int AlgorithmRegistrar::get_num_algorithms() const {
	return (int)algorithm_names.size();
}

const AlgorithmFactory &
AlgorithmRegistrar::get_factory_by_idx(int idx)
{
	return algorithm_factories[idx];
}

const string &AlgorithmRegistrar::get_algorithm_name_by_idx(int idx)
{
	return algorithm_names[idx];
}

bool AlgorithmRegistrar::load_algorithm_from_file(const char *file_path, std::string& error)
{
#ifndef WIN_DEBUG
	// To check if algorithm was registered
	unsigned num_factories_before_register = algorithm_factories.size();
	/* When debugging on Windows, SO loading will always fail and the
	 * algorithms are loaded statically.
	 */
#endif
	// Try to load given module
	DlHandler algo_handle(dlopen(file_path, RTLD_LAZY));

	// Check if dlopen succeeded
	if (algo_handle == nullptr)
	{
#ifdef WIN_DEBUG
		// In debug on Windows, add the algorithm anyway.
		add_algorithm_name(FileParser::extract_file_name_from_path(file_path));
#endif
		const char *dlopen_error = dlerror();
		error = dlopen_error ? dlopen_error : ("Failed to open shared object "
												+ FileParser::extract_file_name_from_path(file_path) + ".so");
		return false;
	}

#ifndef WIN_DEBUG
	// Check if algorithm was registered
	if (num_factories_before_register == algorithm_factories.size())
	{
		error = string("File loaded but no algorithm registered: ")
				+ FileParser::extract_file_name_from_path(file_path) + ".so";
		return false;
	}
#endif

	dl_handles[file_path] = std::move(algo_handle);
	add_algorithm_name(FileParser::extract_file_name_from_path(file_path));

	return true;
}

AlgorithmRegistrar &AlgorithmRegistrar::get_instance()
{
	static AlgorithmRegistrar instance;
	return instance;
}
