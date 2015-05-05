#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "creature.hpp"

class Player : public Creature
{
	public:

	// Name of the player's class
	// Class may be Fighter, Rogue etc
	std::string className;

	// Current experience. 0-1M is reasonable, see the levelup() function
	// for a decent scale
	unsigned int exp;

	Player(std::string name, int health, int str, int end, int dex, double hitRate,
		unsigned int level, std::string className) :
		Creature("player", name, health, str, end, dex, hitRate, level)
	{
		this->className = className;
		this->exp = 0;
	}

	Player() : Player("", 0, 0, 0, 0, 0, 0, "nullid")
	{
	}

	Player(JsonBox::Value v, EntityManager* mgr) : Player()
	{
		this->load("player", v, mgr);
	}

	// Calculates the experience required to reach a certain level,
	// *in total*. Really this is class specific and not object specific
	unsigned int expToLevel(unsigned int level)
	{
		// Exp to level x = 128*x^2
		return 128 * level * level;
	}

	// Level the player to the next level if it has enough experience
	// to do so, returning true if it could level up and false otherwise.
	bool levelUp()
	{
		// We want the experience to the next level, not the current level
		if(this->exp >= expToLevel(this->level+1))
		{
			// Advance to the next level
			++level;

			// Variables to keep track of stat changes. Neater than
			// having a bunch of stat increases all over the place,
			// and removes the issue of the next level's stats affecting
			// themselves (increasing endurance then increasing health
			// based on the boosted instead of the original value, for
			// example
			unsigned int healthBoost = 0;
			unsigned int strBoost = 0;
			unsigned int endBoost = 0;
			unsigned int dexBoost = 0;

			// Give a large boost to health every third level
			if(level % 3 == 0)
			{
				// Randomly increase health, but always give a sizeable
				// chunk proportional to the player's endurance
				healthBoost = 10 + (rand() % 4) + this->end / 4;
			}
			else
			{
				// Just increase health by a small amount
				healthBoost = this->end / 4;
			}
			// If the player is a fighter, then favour strength and
			// endurance boosts over dexterity, but increase dexterity
			// 50% of the time too
			if(this->className == "Fighter")
			{
				strBoost = 1;
				endBoost = 1;
				if(rand() % 2 == 0) dexBoost = 1;
			}
			// Same as for fighter but favour dexterity and endurance
			// instead. Rogue's favour endurance too in order to keep
			// them at roughly the same capability
			else if(this->className == "Rogue")
			{
				endBoost = 1;
				dexBoost = 1;
				if(rand() % 2 == 0) strBoost = 1;
			}

			// Adjust all of the variables accordingly
			this->maxHealth += healthBoost;
			this->str += strBoost;
			this->end += endBoost;
			this->dex += dexBoost;

			// Tell the user that they grew a level, what the boosts where
			// and what their stats are now
			std::cout << this->name << " grew to level " << level << "!\n";
			std::cout << "Health +" << healthBoost << " -> " << this->maxHealth << std::endl;
			std::cout << "Str +" << strBoost << " -> " << this->str << std::endl;
			std::cout << "End +" << endBoost << " -> " << this->end << std::endl;
			std::cout << "Dex +" << dexBoost << " -> " << this->dex<< std::endl;
			std::cout << "----------------\n";

			return true;
		}
		return false;
	}

	JsonBox::Object toJson()
	{
		JsonBox::Object o = Creature::toJson();

		o["className"] = JsonBox::Value(this->className);
		o["exp"] = JsonBox::Value(int(this->exp));

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

		if(o.find("exp") != o.end())
		{
			this->exp = o["exp"].getInteger();
		}
		else
		{
			this->exp = 0;
		}

		return;
	}
};

#endif /* PLAYER_HPP */