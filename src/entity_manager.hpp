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

	template<typename T>
	void loadJson(std::string filename, std::map<std::string, T>& data);

	public:

	template<typename T>
	void loadJson(std::string filename);

	template<typename T>
	T* getEntity(std::string id);

	EntityManager();

};
#endif /* ENTITY_MANAGER_HPP */