#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

#include "entity.hpp"

class EntityManager
{
	private:

	std::map<std::string, Entity*> data;

	public:

	// Load the JSON file and determine which map to save the data to
	// according to the type T
	template<typename T>
	void loadJson(std::string filename);

	// Return the entity given by id
	template<typename T>
	T* getEntity(std::string id);

	// Constructor
	EntityManager();

	// Destructor
	~EntityManager();
};

// Convert a derived entity type to its id prefix. e.g. Item -> "item"
template <typename T>
std::string entityToString();

#endif /* ENTITY_MANAGER_HPP */
