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

// Specialisations of loadJson to convert the template argument to the
// correct std::map destination
#define loadJson_specialised(type, member) \
template <> void EntityManager::loadJson<type>(std::string filename) \
{ loadJson<type>(filename, member); }

loadJson_specialised(Item, dataItem);
loadJson_specialised(Weapon, dataWeapon);
loadJson_specialised(Armor, dataArmor);
loadJson_specialised(Creature, dataCreature);
loadJson_specialised(Area, dataArea);
loadJson_specialised(Door, dataDoor);

#define getEntity_specialised(type, member) \
template <> type* EntityManager::getEntity<type>(std::string id) \
{ return &member.at(id); }

getEntity_specialised(Item, dataItem);
getEntity_specialised(Weapon, dataWeapon);
getEntity_specialised(Armor, dataArmor);
getEntity_specialised(Creature, dataCreature);
getEntity_specialised(Area, dataArea);
getEntity_specialised(Door, dataDoor);

EntityManager::EntityManager() {}
