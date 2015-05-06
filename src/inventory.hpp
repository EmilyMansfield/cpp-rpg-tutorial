#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <list>
#include <utility>
#include <iostream>
#include "JsonBox.h"

class EntityManager;
class Item;
class Weapon;
class Armor;

class Inventory
{
	private:

	// Given the Json value v which contains a list of items, weapons, or armor of type T
	// load the Ts into the storage list (either items, weapons, or armor)
	template <typename T>
	void loadItems(JsonBox::Value v, std::list<std::pair<T*, int>>& storage, EntityManager* mgr);

	// Add an item to the inventory, specified by a pointer to it
	// Should be from the entity manager
	template <typename T>
	void addItem(T* item, int count, std::list<std::pair<T*, int>>& storage);

	// Remove the specified number of items from the inventory
	template <typename T>
	void removeItem(T* item, int count, std::list<std::pair<T*, int>>& storage);

	// Returns the count of the specified item
	template <typename T>
	unsigned int hasItem(T* item, std::list<std::pair<T*, int>>& storage);

	// Output a list of the items onto stdout, formatted nicely and
	// numbered if required
	template <typename T>
	int printItems(std::list<std::pair<T*, int>>& storage, bool label = false);

	template <typename T>
	JsonBox::Array getJson(std::list<std::pair<T*, int>>& storage);

	public:

	// Whilst weapons and armor are also items, they have their own
	// specific properties and so cannot be stored inside the same
	// list as the items. We use a list and not a vector as inventories
	// are highly mutable. This way they can also be efficiently sorted
	// The first element of the pair stores a pointer to the item in
	// the item/weapon/armor atlas, defined in main(), and the second
	// element stores the quantity of the item
	std::list<std::pair<Item*, int>> items;
	std::list<std::pair<Weapon*, int>> weapons;
	std::list<std::pair<Armor*, int>> armor;

	Inventory();

	// Load the inventory from a JSON value
	Inventory(JsonBox::Value v, EntityManager* mgr);

	// Remove all items from the inventory, destroying them in the process
	// (They remain in the entity manager though)
	void clear();

	// Template specialisations
	void addItem(Item* item, int count);
	void addItem(Weapon* weapon, int count);
	void addItem(Armor* armor, int count);

	void removeItem(Item* item, int count);
	void removeItem(Weapon* weapon, int count);
	void removeItem(Armor* armor, int count);

	unsigned int hasItem(Item* item);
	unsigned int hasItem(Weapon* weapon);
	unsigned int hasItem(Armor* armor);

	int printItems(bool label = false);
	int printWeapons(bool label = false);
	int printArmor(bool label = false);

	// Merge the specified inventory with the current one, adding
	// item quantities together if they already exist and adding the item
	// into a new slot if they do not
	void merge(Inventory* inventory);

	// Print the entire inventory; items, then weapons, then armor,
	// but if the inventory is empty then output "Nothing"
	void print(bool label = false);

	// Get a Json object representation of the inventory
	JsonBox::Object getJson();
};

#endif /* INVENTORY_HPP */
