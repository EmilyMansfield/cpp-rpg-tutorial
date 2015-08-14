#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <string>
#include <vector>
#include <iostream>
#include "JsonBox.h"

// Gameplay is expressed using dialogues, which present a piece of
// information and some responses, and the ask the user to pick one. If
// they do not pick a valid one then the dialogue loops until they do
class Dialogue
{
	private:

	// Initial piece of information that the dialogue displays
	std::string description;

	// A vector of choices that will be outputted. No numbering is
	// necessary, the dialogue does that automatically
	std::vector<std::string> choices;

	public:

	// Run the dialogue
	int activate()
	{
		// Output the information
		std::cout << description << std::endl;

		// Output and number the choices
		for(int i = 0; i < this->choices.size(); ++i)
			std::cout << i+1 << ": " << this->choices[i] << std::endl;

		// Repeatedly read input from stdin until a valid option is
		// chosen
		int userInput = -1;
		while(true)
		{
			std::cin >> userInput;
			// 'Valid' means within the range of numbers outputted
			if(userInput >= 0 && userInput <= this->choices.size())
			{
				return userInput;
			}
		}
	}

	// Note that the vector is not passed by reference. Whilst that would
	// be more efficient, it forces us to create a vector outside of the
	// constructor. By passing by value we can call the constructor using
	// an initialisation list such as
	// Dialogue my_dialogue("Hello", {"Choice1", "Choice"});
	Dialogue(std::string description, std::vector<std::string> choices)
	{
		this->description = description;
		this->choices = choices;
	}

	// Create a dialogue from a JSON value
	Dialogue(JsonBox::Value& v)
	{
		JsonBox::Object o = v.getObject();
		description = o["description"].getString();
		for(auto choice : o["choices"].getArray())
			choices.push_back(choice.getString());
	}

	Dialogue() {}

	void addChoice(std::string choice)
	{
		this->choices.push_back(choice);
	}

	unsigned int size()
	{
		return this->choices.size();
	}
};

#endif /* DIALOGUE_HPP */
