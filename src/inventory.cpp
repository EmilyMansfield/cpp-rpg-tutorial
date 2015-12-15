#include <list>
#include <string>
#include <utility>
#include <iostream>
#include <JsonBox.h>

#include "inventory.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "entity_manager.hpp"

template <typename T>
void Inventory::load(JsonBox::Value& v, EntityManager* mgr)
{
	for(auto item : v.getArray())
	{
		std::string itemId = item.getArray()[0].getString();
		int quantity = item.getArray()[1].getInteger();
		this->items.push_back(std::make_pair(mgr->getEntity<T>(itemId), quantity));
	}
}

template <typename T>
JsonBox::Array Inventory::jsonArray()
{
	JsonBox::Array a;
	for(auto item : this->items)
	{
		// Skip if the id does not match to the type T
		if(item.first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
			continue;
		// Otherwise add the item to the array
		JsonBox::Array pair;
		pair.push_back(JsonBox::Value(item.first->id));
		pair.push_back(JsonBox::Value(item.second));
		a.push_back(JsonBox::Value(pair));
	}

	return a;
}

void Inventory::add(Item* item, int count)
{
	for(auto& it : this->items)
	{
		if(it.first->id == item->id)
		{
			it.second += count;
			return;
		}
	}
	this->items.push_back(std::make_pair(item, count));
}

void Inventory::remove(Item* item, int count)
{
	// Iterate through the items, and if they are found then decrease
	// the quantity by the quantity removed
	for(auto it = this->items.begin(); it != this->items.end(); ++it)
	{
		if((*it).first->id == item->id)
		{
			(*it).second -= count;
			if((*it).second < 1) this->items.erase(it);
			return;
		}
	}
}

template <typename T>
T* Inventory::get(unsigned int n)
{
	// Using a list so we don't have random access, and must
	// step through n times from the start instead
	unsigned int i = 0;
	auto it = this->items.begin();
	for(; it != this->items.end(); ++it)
	{
		if((*it).first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
			continue;
		if(i++ == n) break;
	}
	if(it != this->items.end())
		return dynamic_cast<T*>((*it).first);
	else
		return nullptr;
}

int Inventory::count(Item* item)
{
	for(auto it : this->items)
	{
		if(it.first->id == item->id)
			return it.second;
	}
	return 0;
}

template <typename T>
int Inventory::count(unsigned int n)
{
	return count(get<T>(n));
}

template <typename T>
int Inventory::print(bool label)
{
	unsigned int i = 1;

	for(auto it : this->items)
	{
		// Skip if the id does not match to the type T
		if(it.first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
			continue;
		// Number the items if asked
		if(label) std::cout << i++ << ": ";
		// Output the item name, quantity and description, e.g.
		// Gold Piece (29) - Glimmering discs of wealth
		std::cout << it.first->name << " (" << it.second << ") - ";
		std::cout << it.first->description << std::endl;
	}

	// Return the number of items outputted, for convenience
	return this->items.size();
}

// Overload of print to print all items when the template argument is empty
int Inventory::print(bool label)
{
	unsigned int i = 0;

	if(items.empty())
	{
		std::cout << "Nothing" << std::endl;
	}
	else
	{
		i += print<Item>(label);
		i += print<Weapon>(label);
		i += print<Armor>(label);
	}

	return i;
}

void Inventory::clear()
{
	this->items.clear();
}

void Inventory::merge(Inventory* inventory)
{
	// You can't merge an inventory with itself!
	if(inventory == this) return;

	// Loop through the items to be added, and add them. Our addition
	// function will take care of everything else for us
	for(auto it : inventory->items) this->add(it.first, it.second);

	return;
}

Inventory::Inventory(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	load<Item>(o["items"], mgr);
	load<Weapon>(o["weapons"], mgr);
	load<Armor>(o["armor"], mgr);
}

JsonBox::Object Inventory::getJson()
{
	JsonBox::Object o;

	o["items"] = JsonBox::Value(jsonArray<Item>());
	o["weapons"] = JsonBox::Value(jsonArray<Weapon>());
	o["armor"] = JsonBox::Value(jsonArray<Armor>());

	return o;
}

// Template instantiations
template void Inventory::load<Item>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Weapon>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Armor>(JsonBox::Value&, EntityManager*);

template JsonBox::Array Inventory::jsonArray<Item>();
template JsonBox::Array Inventory::jsonArray<Weapon>();
template JsonBox::Array Inventory::jsonArray<Armor>();

template int Inventory::count<Item>(unsigned int);
template int Inventory::count<Weapon>(unsigned int);
template int Inventory::count<Armor>(unsigned int);

template Item* Inventory::get<Item>(unsigned int);
template Weapon* Inventory::get<Weapon>(unsigned int);
template Armor* Inventory::get<Armor>(unsigned int);

template int Inventory::print<Item>(bool);
template int Inventory::print<Weapon>(bool);
template int Inventory::print<Armor>(bool);
