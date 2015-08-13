#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "JsonBox.h"
#include <string>

class EntityManager;

class Entity
{
	public:

	std::string id;

	Entity(std::string id)
	{
		this->id = id;
	}

	// Pure virtual function stops Entity from being instantiated
	virtual void load(std::string id, JsonBox::Value& v, EntityManager* mgr) = 0;
};

#endif /* ENTITY_HPP */
