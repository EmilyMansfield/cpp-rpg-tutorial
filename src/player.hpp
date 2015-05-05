#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "creature.hpp"

#include <cmath>

class Player : public Creature
{
	public:

	// Name of the player's class
	// Class may be Fighter, Rogue etc
	std::string className;

	// Current experience. 0-1M is reasonable, see the levelup() function
	// for a decent scale
	unsigned int xp;

	Player(std::string name, int hp, int strength, int agility, double evasion,
		unsigned int level, std::string className) :
		Creature("player", name, hp, strength, agility, evasion, level)
	{
		this->className = className;
		this->xp = 0;
	}

	Player() : Player("", 0, 0, 0, 0.0, 0, "nullid")
	{
	}

	Player(JsonBox::Value v, EntityManager* mgr) : Player()
	{
		this->load("player", v, mgr);
	}

	// Calculates the total experience required to reach a certain level
	unsigned int xpToLevel(unsigned int level)
	{
		return (unsigned int)(1.5 * std::pow(this->level, 3));
	}

	// Level the player to the next level if it has enough experience
	// to do so, returning true if it could level up and false otherwise.
	bool levelUp()
	{
		// Can't level up if there's not enough experience
		if(this->xp < xpToLevel(this->level+1))
		{
			return false;
		}

		// Advance to the next level
		++level;

		// Variables to keep track of stat changes, and their associated
		// multipliers, which depend on the class. The multiplier affects
		// how much that stat increases each level, and is higher if the
		// class specialises in that stat
		// [hp, strength, agility]
		unsigned int statIncreases[3] = {0, 0, 0};
		float statMultipliers[3] = {0, 0, 0};
		statMultipliers[0] = 1.3;
		statMultipliers[1] = this->className == "Fighter" ? 8.0 : 6.0;
		statMultipliers[2] = this->className == "Rogue" ? 8.0 : 6.0;

		// Compute the stat increases for each stat
		for(int i = 0; i < 3; ++i)
		{
			float base = std::tanh(this->level / 30.0) * ((this->level % 2) + 1);
			statIncreases[i] += int(1 + statMultipliers[i] * base);
		}

		// Adjust all of the stats accordingly
		this->hp += statIncreases[0];
		this->maxHp += statIncreases[0];
		this->strength += statIncreases[1];
		this->agility += statIncreases[2];

		// Tell the user that they grew a level, what the increases were
		// and what their stats are now
		std::cout << this->name << " grew to level " << level << "!\n";
		std::cout << "Health   +" << statIncreases[0] << " -> " << this->maxHp << std::endl;
		std::cout << "Strength +" << statIncreases[1] << " -> " << this->strength << std::endl;
		std::cout << "Agility  +" << statIncreases[2] << " -> " << this->agility << std::endl;
		std::cout << "----------------\n";

		return true;
	}

	JsonBox::Object toJson()
	{
		JsonBox::Object o = Creature::toJson();

		o["className"] = JsonBox::Value(this->className);
		o["xp"] = JsonBox::Value(int(this->xp));

		return o;
	}

	void save()
	{
		JsonBox::Value v(this->toJson());
		v.writeToFile(this->name + ".json");

	}

	// Attempt to load all data from the JSON value
	void load(std::string id, JsonBox::Value v, EntityManager* mgr)
	{
		// Load data shared with Creature
		Creature::load(id, v, mgr);

		// Load optional variables
		JsonBox::Object o = v.getObject();

		this->className = o["className"].getString();

		if(o.find("xp") != o.end())
		{
			this->xp = o["xp"].getInteger();
		}
		else
		{
			this->xp = 0;
		}

		return;
	}
};

#endif /* PLAYER_HPP */