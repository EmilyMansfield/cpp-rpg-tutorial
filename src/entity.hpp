#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "JsonBox.h"
#include <string>

class Entity
{
	public:

	std::string id;

	Entity(std::string id)
	{
		this->id = id;
	}

	virtual void load(std::string id, JsonBox::Value& v)
	{
		this->id = id;

		return;
	}
};

#endif /* ENTITY_HPP */