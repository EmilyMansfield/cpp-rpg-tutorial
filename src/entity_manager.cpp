#include <string>
#include <map>

#include "entity_manager.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "creature.hpp"
#include "area.hpp"
#include "door.hpp"

template <class T>
void EntityManager::loadJson(std::string filename)
{
	JsonBox::Value v;
	v.loadFromFile(filename);

	JsonBox::Object o = v.getObject();
	for(auto entity : o)
	{
		std::string key = entity.first;
		this->data[key] = dynamic_cast<Entity*>(new T(key, entity.second, this));
	}
}

template <class T>
T* EntityManager::getEntity(std::string id)
{
	// The id prefix should match to the type T, so take the
	// first characters of the id up to the length of the
	// prefix and compare the two
	if(id.substr(0, entityToString<T>().size()) == entityToString<T>())
		return dynamic_cast<T*>(this->data.at(id));
	else
		return nullptr;
}

EntityManager::EntityManager() {}

EntityManager::~EntityManager()
{
	for(auto& entity : this->data)
	{
		delete entity.second;
	}
}

// Template specialisations
template <> std::string entityToString<Item>() { return "item"; }
template <> std::string entityToString<Weapon>() { return "weapon"; }
template <> std::string entityToString<Armor>() { return "armor"; }
template <> std::string entityToString<Creature>() { return "creature"; }
template <> std::string entityToString<Area>() { return "area"; }
template <> std::string entityToString<Door>() { return "door"; }

// Template instantiations
template void EntityManager::loadJson<Item>(std::string);
template void EntityManager::loadJson<Weapon>(std::string);
template void EntityManager::loadJson<Armor>(std::string);
template void EntityManager::loadJson<Creature>(std::string);
template void EntityManager::loadJson<Area>(std::string);
template void EntityManager::loadJson<Door>(std::string);

template Item* EntityManager::getEntity<Item>(std::string);
template Weapon* EntityManager::getEntity<Weapon>(std::string);
template Armor* EntityManager::getEntity<Armor>(std::string);
template Creature* EntityManager::getEntity<Creature>(std::string);
template Area* EntityManager::getEntity<Area>(std::string);
template Door* EntityManager::getEntity<Door>(std::string);

