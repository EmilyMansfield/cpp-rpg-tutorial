#include <string>
#include <map>

#include "entity_manager.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "creature.hpp"
#include "area.hpp"
#include "door.hpp"

template<typename T>
void EntityManager::loadJson(std::string filename, std::map<std::string, T>& data)
{
	// Fill the data map
	JsonBox::Value v;
	v.loadFromFile(filename);

	JsonBox::Object o = v.getObject();
	for(auto entity : o)
	{
		std::string key = entity.first;
		data[key] = T(key, entity.second, this);
	}

	return;
}

template<typename T>
void EntityManager::loadJson(std::string filename)
{
	if(std::is_same<T, Item>::value)			loadJson(filename, dataItem);
	else if(std::is_same<T, Weapon>::value)		loadJson(filename, dataWeapon);
	else if(std::is_same<T, Armor>::value)		loadJson(filename, dataArmor);
	else if(std::is_same<T, Creature>::value)	loadJson(filename, dataCreature);
	else if(std::is_same<T, Area>::value)		loadJson(filename, dataArea);
	else if(std::is_same<T, Door>::value)		loadJson(filename, dataDoor);

	return;
}

template<typename T>
T* EntityManager::getEntity(std::string id)
{
	if(std::is_same<T, Item>::value)			return dynamic_cast<T*>(&dataItem.at(id));
	else if(std::is_same<T, Weapon>::value)		return dynamic_cast<T*>(&dataWeapon.at(id));
	else if(std::is_same<T, Armor>::value)		return dynamic_cast<T*>(&dataArmor.at(id));
	else if(std::is_same<T, Creature>::value)	return dynamic_cast<T*>(&dataCreature.at(id));
	else if(std::is_same<T, Area>::value)		return dynamic_cast<T*>(&dataArea.at(id));
	else if(std::is_same<T, Door>::value)		return dynamic_cast<T*>(&dataDoor.at(id));
	else										return nullptr;
}

EntityManager::EntityManager() {}