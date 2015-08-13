#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "creature.hpp"
#include "area.hpp"
#include "door.hpp"
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

#endif /* ENTITY_MANAGER_HPP */
