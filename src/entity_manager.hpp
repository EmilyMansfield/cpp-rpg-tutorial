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

class EntityManager
{
	private:

	std::map<std::string, Item> dataItem;
	std::map<std::string, Weapon> dataWeapon;
	std::map<std::string, Armor> dataArmor;
	std::map<std::string, Creature> dataCreature;
	std::map<std::string, Area> dataArea;
	std::map<std::string, Door> dataDoor;

	// Read the JSON file given by filename and interpret it according
	// to the type T, before storing all entities read in the data map
	template<typename T>
	void loadJson(std::string filename, std::map<std::string, T>& data);

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

};
#endif /* ENTITY_MANAGER_HPP */