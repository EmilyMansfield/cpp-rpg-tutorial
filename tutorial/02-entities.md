# Entities

Now that we know what we're working towards, we can begin to build the
program. At this stage it doesn't matter too much where we start, but
since a lot of things will be using the entity manager we'll create that
and the `Entity` class first.

```cpp
/* entity.hpp */
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <JsonBox.h>
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

	// Destructor must be made virtual as all derived classes are
	// treated as Entity in the EntityManager
	virtual ~Entity() {}

	// Pure virtual function stops Entity from being instantiated and forces it
	// to be implemented in all derived types
	virtual void load(JsonBox::Value& v, EntityManager* mgr) = 0;
};

#endif /* ENTITY_HPP */
```

First we have a forward declaration of the `EntityManager` class; this
is necessary because whilst the `EntityManager` needs to know about the
`Entity` class, each `Entity` may also need access to the
`EntityManager`! The first JSON example shows a case when this occurs;
to make a new door we need to know about another item found in the
entity manager, namely the iron key. Because two files can't `#include`
each other we use a forward declaration to tell the compiler that
`EntityManager` will exist, even if it doesn't yet.

The `id` member variable is a string that helps identify the object, and
will be used as the entity's key in the JSON file as well as internally
when getting an entity from the `EntityManager`. Next we have a
constructor and a virtual destructor, neither of which do anything
interesting. We have to make the destructor virtual because of how the
`EntityManager` stores the entities, but we'll get to that soon.

Finally we have a pure virtual function called `load`, which every
entity derived from `Entity` must implement. By making it virtual we're
allowing it to be overridden in the derived classes, and by making it
pure (the `= 0`) we turn `Entity` into an *abstract class*. Abstract
classes cannot have any objects, so whilst the `Item`, `Area`,
`Creature` etc. entities we define in the future will all be a kind of
`Entity`, we can't create an `Entity` by itself. One of the arguments is
a `JsonBox::Value`, which is a representation of the entity being loaded
in JSON form. In order to load `"door_01_02"` for example, we'll pass
the `JsonBox::Value` described by the JSON value

```json
{
	"description": "sturdy wooden door",
	"areas": ["area_01", "area_02"],
	"locked": 1,
	"key": "item_iron_key"
}
```

Now for the `EntityManager` itself.

```cpp
/* entity_manager.hpp */
#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

#include "entity.hpp"

class EntityManager
{
	private:

	std::map<std::string, Entity*> data;

	public:

	// Load the JSON file and determine which map to save the data to
	// according to the type T
	template<typename T>
	void loadJson(std::string filename);

	// Return the entity given by id
	template<typename T>
	T* getEntity(std::string id);

	// Constructor
	EntityManager();

	// Destructor
	~EntityManager();
};

// Convert a derived entity type to its id prefix. e.g. Item -> "item"
template <typename T>
std::string entityToString();

#endif /* ENTITY_MANAGER_HPP */
```

Each entity is stored in an `std::map`, which is accessible in ways very
similar to an array, but instead of indexing each element by an integer
each element (in this case a pointer to an `Entity`) is indexed by a
string. This very nicely mirrors the structure of a JSON file, where
each unique key gives a JSON value. An important thing to note is that
whilst abstract classes such as `Entity` can't exist exist as objects,
they can exist as pointers. But if we can't have any actual `Entity`
objects, what do the pointers point to?

C++ has a nice feature where a pointer to a derived class can be
stripped back and reduced to a pointer to its base class. This means
that we can create `Item` or `Door` entities and then store pointers to
them in the `std::map`, even though the types don't match up. The
problem is, we won't then be able to access any member variables or
functions specific to `Item` or `Door`, because an `Entity` doesn't know
about those. Luckily, C++ allows us to convert a pointer to a base class
back to a pointer to a derived class, giving us access to all those
member variables!

We'll use that functionality in the `.cpp` file soon, but for now lets
continue looking at `EntityManager`. Next we have a *function template*
called `loadJson` which will read the JSON file `filename` and add all
the entities described in that file to the `std::map`. The template
argument passed to `loadJson` determines what kind of entity it should
try and load from the file. `loadJson` won't handle any loading of
individual entities, but instead will find all the keys in the JSON file
and create a new entity by passing the corresponding value to the
entity's constructor.  This is where we'll be converting derived
pointers to base pointers.

Next we have `getEntity`, which when given an entity type and an id will
find the entity with that id and return a pointer to it. This is where
we'll be converting base pointers back to derived pointers.

Finally there's a constructor and a destructor, which don't need much
said about them!

Outside of the `EntityManager` class we have an additional function
template called `entityToString`. This function is specialised to each
possible template argument `T`, and given an entity type---such as
`Door` or `Item`---will return a string corresponding to that
entity---such as `"door"` or `"item"`.

```cpp
/* entity_manager.cpp */
#include <string>
#include <map>

#include "entity_manager.hpp"

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

```

`loadJson` loads the JSON file as a JSON value (the entire set of data
between the `{}`) and then converts it into a JSON object that will have
keys and corresponding values. A `JsonBox::Object` is very similar to an
`std::map`, where each element contains not just a value, but also the
key. As we iterate over the object `entity` will contain both the key
and the value, so we extract the key and then create a new entity using
the corresponding value (of type `JsonBox::Value`). To create the entity
we assume that the type `T` (which will be some class derived from
`Entity` such as `Item` or `Door`) has a constructor with the same
arguments as `Entity::load` and call it, passing to it the key, value,
and the `EntityManager` itself. Because `data` contains pointers we use
the `new` keyword to allocate memory for the new entity and then use
`dynamic_cast<Entity*>` to covert the resulting pointer of type `T*` to
a pointer of type `Entity*` which can then be stored in `data`.

`getEntity` is shorter, and simply gets the entity with key `id` from
`data` before using `dynamic_cast<T*>` to convert it from a pointer of
type `Entity*` back to a pointer of type `T*`, which it then returns.
Before doing so however it uses the `entityToString` function to check
that the `id` matches up to the type `T`. It assumes that all
`Item`s---for example---have an `id` beginning with `"item"`, and if
they don't it will return a `nullptr` instead. This still assumes that
the data is named correctly, of course, which we aren't enforcing!

Then comes the constructor---which does nothing at all---and the
destructor, which deallocates all the memory allocated by `new` in
`loadJson`. That's all with this class for now, but whenever we add a
new entity class you must make sure to add an explicit instantiation or
specialisation for each of the function templates that corresponds to
the new entity. When adding an `Item` class for example, you should add

```cpp
// Specialisation
template <> std::string entityToString<Item>() { return "item"; }

// Instantiation
template void EntityManager::loadJson<Item>(std::string);
template Item* EntityManager::getEntity<Item>(std::string);
```
