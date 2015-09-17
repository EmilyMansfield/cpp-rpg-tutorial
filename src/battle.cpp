#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <map>

#include "battle.hpp"
#include "creature.hpp"
#include "dialogue.hpp"

BattleEvent::BattleEvent(Creature* source, Creature* target, BattleEventType type)
{
	this->source = source;
	this->target = target;
	this->type = type;
}

int BattleEvent::run()
{
	switch(type)
	{
		case BattleEventType::ATTACK:
			return source->attack(target);
		case BattleEventType::DEFEND:
			return 0;
		default:
			return 0;
	}
}

void Battle::kill(Creature* creature)
{
	// Find the creature's position in the combatants vector
	auto pos = std::find(this->combatants.begin(), this->combatants.end(), creature);

	// Don't try and delete the creature if it doesn't exist
	if(pos != this->combatants.end())
	{
		std::cout << creature->name << " is slain!\n";

		// Health == 0 is used in main as a condition to check if the creature is
		// dead, but this function could be called when the creature is not killed
		// by reducing their health to zero (by a death spell, for example), so we
		// ensure the creature's health is 0 and is marked as dead
		creature->hp = 0;
		// Remove the creature
		this->combatants.erase(pos);
	}

	return;
}

Battle::Battle(std::vector<Creature*>& combatants)
{
	this->combatants = combatants;

	// Construct the menu
	this->battleOptions = Dialogue("What will you do?",
	{
		"Attack",
		"Defend"
	});

	// Store the unique creature names and whether there is
	// only one or more of them. This code assumes that the
	// creatures have not already been assigned unique names,
	// which should be the case as a battle cannot be left
	// and then resumed again
	std::map<std::string, int> names;
	for(auto com : this->combatants)
	{
		// Skip the player, who shouldn't be renamed
		if(com->id == "player") continue;
		// If the name hasn't been recorded and the creature
		// isn't the player, record the name
		if(names.count(com->name) == 0)
		{
			names[com->name] = 0;
		}
		// If there is already one creature, record there are being
		// more than one. We don't want the actual number, simply
		// that there's more and so we should label them.
		else if(names[com->name] == 0)
		{
			names[com->name] = 1;
		}
	}

	// Creature unique names for the combatants
	for(auto& com : this->combatants)
	{
		std::string newName = com->name;
		// If the name is marked as being shared by more than
		// one creature
		if(names.count(com->name) > 0 && names[com->name] > 0)
		{
			// Append (1) to the end of the name, and then increase the
			// number for the next creature
			newName += " (" + std::to_string(names[com->name]) + ")";
			names[com->name] += 1;
		}
		// Change the creature name to the new one, which might just be
		// the same as the original
		com->name = newName;
	}
}

void Battle::run()
{
	std::vector<Creature*>::iterator player;
	std::vector<Creature*>::iterator end;
	do
	{
		// Continue the battle until either the player dies,
		// or there is only the player left
		player = std::find_if(this->combatants.begin(), this->combatants.end(),
			[](Creature* a) { return a->id == "player"; });
		end = this->combatants.end();

		this->nextTurn();
	}
	while(player != end && this->combatants.size() > 1);

	return;
}

void Battle::nextTurn()
{
	// Queue of battle events. Fastest combatants will be
	// at the start of the queue, and so will go first,
	// whereas slower ones will be at the back
	std::queue<BattleEvent> events;

	// Sort the combatants in agility order
	std::sort(combatants.begin(), combatants.end(), [](Creature* a, Creature* b) { return a->agility > b->agility; });

	// Iterate over the combatants and decide what they should do,
	// before adding the action to the event queue.
	for(auto com : this->combatants)
	{
		if(com->id == "player")
		{
			// Create the target selection dialogue
			Dialogue targetSelection = Dialogue("Who?", {});
			// Created every turn because some combatants may die
			for(auto target : this->combatants)
			{
				if(target->id != "player")
				{
					targetSelection.addChoice(target->name);
				}
			}

			// Ask the player for their action (attack or defend)
			int choice = this->battleOptions.activate();

			switch(choice)
			{
				default:
				case 1:
				{
					// Player is attacking, so ask for the target.
					// Dialogue returns the number of the choice but with
					// the player removed, so we have to do some fancy
					// arithmetic to find the actual location of the target
					// and then convert that to a pointer
					int position = targetSelection.activate();
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::ATTACK));
					break;
				}
				case 2:
				{
					// Player is defending, so do nothing
					events.push(BattleEvent(com, nullptr, BattleEventType::DEFEND));
					break;
				}
			}
		}
		else
		{
			// Simple enemy AI where enemy constantly attacks player
			Creature* player = *std::find_if(this->combatants.begin(), this->combatants.end(),
				[](Creature* a) { return a->id == "player"; });

			events.push(BattleEvent(com, player, BattleEventType::ATTACK));
		}
	}

	// Take each event from the queue in turn and process them,
	// displaying the results
	while(!events.empty())
	{
		// Take event from the front of the queue
		BattleEvent event = events.front();
		switch(event.type)
		{
			case BattleEventType::ATTACK:
			{
				// The event can't be run if either the source or the
				// target were slain previously in this turn, so we
				// must check that they're valid first
				auto a = this->combatants.begin();
				auto b = this->combatants.end();
				if(std::find(a, b, event.source) == b || std::find(a, b, event.target) == b)
				{
					break;
				}
				std::cout << event.source->name
					<< " attacks "
					<< event.target->name
					<< " for "
					<< event.run()
					<< " damage!\n";
				// Delete slain enemies
				if(event.target->hp <= 0)
				{
					this->kill(event.target);
				}
				break;
			}
			case BattleEventType::DEFEND:
				std::cout << event.source->name
					<< " defends!\n";
				break;
			default:
				break;
		}
		events.pop();
	}
}
