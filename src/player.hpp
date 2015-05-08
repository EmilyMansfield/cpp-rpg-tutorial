#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <unordered_set>
#include <string>
#include "JsonBox.h"

#include "creature.hpp"

class EntityManager;

class Player : public Creature
{
	public:

	// Name of the player's class
	// Class may be Fighter, Rogue etc
	std::string className;

	// Level of the player
	unsigned int level;

	// Ids of areas visited by the player
	std::unordered_set<std::string> visitedAreas;

	// Constructors
	Player(std::string name, int hp, int strength, int agility, double evasion,
		unsigned int xp, unsigned int level, std::string className);
	Player();
	Player(JsonBox::Value v, EntityManager* mgr);

	// Calculates the total experience required to reach a certain level
	unsigned int xpToLevel(unsigned int level);

	// Level the player to the next level if it has enough experience
	// to do so, returning true if it could level up and false otherwise.
	bool levelUp();

	// Create a Json object representation of the player
	JsonBox::Object toJson();

	// Save the player to a file named after them
	void save(EntityManager* mgr);

	// Attempt to load all data from the JSON value
	void load(std::string id, JsonBox::Value v, EntityManager* mgr);
};

#endif /* PLAYER_HPP */