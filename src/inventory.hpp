#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <list>
#include <utility>
#include <JsonBox.h>

#include "entity_manager.hpp"

class Item;
class Weapon;
class Armor;

class Inventory
{
	private:

	// We use a similar method here to in EntityManager where we store
	// a list of base pointers. We use a list and not a vector as inventories
	// are highly mutable. This way they can also be efficiently sorted.
	// The first element of the pair stores a pointer to the item in
	// the EntityManager, and the second element stores the quantity of the item
	std::list<std::pair<Item*, int>> items;

	// Given the Json value v which contains a list of items, weapons, or armor of type T
	// load the Ts into the storage list (either items, weapons, or armor)
	template <typename T>
	void load(JsonBox::Value& v, EntityManager* mgr);

	// Return a JSON representation of all the items of the type T
	template <typename T>
	JsonBox::Array jsonArray();

	public:

	// Add an item to the inventory
	void add(Item* item, int count);

	// Remove the specified number of items from the inventory
	void remove(Item* item, int count);

	// Returns the count of the specified item
	int count(Item* item);
	template <typename T>
	int count(unsigned int n);

	// Return the nth item in the storage list
	template <typename T>
	T* get(unsigned int n);

	// Output a list of the items onto stdout, formatted nicely and
	// numbered if required
	template <typename T>
	int print(bool label = false);

	// Remove all items from the inventory
	void clear();

	// Merge the specified inventory with the current one, adding
	// item quantities together if they already exist and adding the item
	// into a new slot if they do not
	void merge(Inventory* inventory);

	// Load the inventory from a JSON value
	Inventory(JsonBox::Value& v, EntityManager* mgr);
	Inventory() {}

	// Print the entire inventory; items, then weapons, then armor,
	// but if the inventory is empty then output "Nothing"
	int print(bool label = false);

	// Get a Json object representation of the inventory
	JsonBox::Object getJson();
};

#endif /* INVENTORY_HPP */
