#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

class Item;
class Weapon;
class Armour;
class Creature;
class Area;

class EntityManager
{
	private:

	std::map<std::string, Item> dataItem;
	std::map<std::string, Weapon> dataWeapon;
	std::map<std::string, Armour> dataArmour;
	std::map<std::string, Creature> dataCreature;
	std::map<std::string, Area> dataArea;

	template<typename T>
	void loadJson(std::string filename, std::map<std::string, T>& data)
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

	public:

	template<typename T>
	void loadJson(std::string filename)
	{
		if(std::is_same<T, Item>::value)			loadJson(filename, dataItem);
		else if(std::is_same<T, Weapon>::value)		loadJson(filename, dataWeapon);
		else if(std::is_same<T, Armour>::value)		loadJson(filename, dataArmour);
		else if(std::is_same<T, Creature>::value)	loadJson(filename, dataCreature);
		else if(std::is_same<T, Area>::value)		loadJson(filename, dataArea);

		return;
	}

	template<typename T>
	T* getEntity(std::string id)
	{
		if(std::is_same<T, Item>::value)			return dynamic_cast<T*>(&dataItem.at(id));
		else if(std::is_same<T, Weapon>::value)		return dynamic_cast<T*>(&dataWeapon.at(id));
		else if(std::is_same<T, Armour>::value)		return dynamic_cast<T*>(&dataArmour.at(id));
		else if(std::is_same<T, Creature>::value)	return dynamic_cast<T*>(&dataCreature.at(id));
		else if(std::is_same<T, Area>::value)		return dynamic_cast<T*>(&dataArea.at(id));
		else										return nullptr;
	}

	EntityManager() {}

};
#endif /* ENTITY_MANAGER_HPP */