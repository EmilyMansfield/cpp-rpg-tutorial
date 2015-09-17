#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <vector>

#include "dialogue.hpp"

class Creature;

// Possible event types, should equate to what the player
// can do in a battle
enum class BattleEventType { ATTACK, DEFEND };

class BattleEvent
{
	public:

	// Creature that initiated the event, e.g. the attacker
	Creature* source;
	// Creature being affected, e.g. the one being attacked
	Creature* target;
	// Type of event, e.g. attack, or defense
	BattleEventType type;

	// Constructor
	BattleEvent(Creature* source, Creature* target, BattleEventType type);

	// Convert the event type to the corresponding function and call it
	// on the source and target
	int run();
};

class Battle
{
	private:

	// All the creatures that are participating in the fight.
	// We assume the player is a Creature with id "player".
	// A vector is used because we need to get the nth element
	// for use with a Dialogue
	std::vector<Creature*> combatants;

	// Actions that the player can take in the battle
	Dialogue battleOptions;

	// Remove a creature from the combatants list, and report that it's dead
	void kill(Creature* creature);

	// Run the next turn for the enemies and the player.
	// Computes what the enemies should do and asks for the player's
	// action, then compiles an event queue of the actions before
	// proceeding through the queue and running each action.
	void nextTurn();

	public:

	// Constructor
	Battle(std::vector<Creature*>& combatants);

	// Run the battle until either the player dies, or all the opposing
	// combatants do
	void run();
};

#endif /* BATTLE_HPP */
