#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <string>
#include "JsonBox.h"

#include "item.hpp"

class EntityManager;

class Weapon : public Item
{
	public:

	int damage;

	// Constructors
	Weapon(std::string id, std::string name, std::string description, int damage);
	Weapon(std::string id, JsonBox::Value& v, EntityManager* mgr);
	Weapon();

	void load(std::string id, JsonBox::Value& v, EntityManager* mgr);
};

#endif /* WEAPON_HPP */
