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
		this->data[key] = new T(key, entity.second, this);
	}
}

template <class T>
T* EntityManager::getEntity(std::string id)
{
	return dynamic_cast<T*>(this->data.at(id));
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
template void EntityManager::loadJson<Item>(std::string);
template void EntityManager::loadJson<Weapon>(std::string);
template void EntityManager::loadJson<Armor>(std::string);
template void EntityManager::loadJson<Creature>(std::string);
template void EntityManager::loadJson<Area>(std::string);
template void EntityManager::loadJson<Door>(std::string);

template Item* EntityManager::getEntity(std::string);
template Weapon* EntityManager::getEntity(std::string);
template Armor* EntityManager::getEntity(std::string);
template Creature* EntityManager::getEntity(std::string);
template Area* EntityManager::getEntity(std::string);
template Door* EntityManager::getEntity(std::string);

