/*
The MIT License (MIT)

Copyright (c) 2013 Daniel Mansfield

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <string>
#include <vector>
#include <iostream>

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
		{
			std::cout << i+1 << ": " << this->choices[i] << std::endl;
		}

		int userInput = -1;

		// Repeatedly read input from stdin until a valid option is
		// chosen
		while(true)
		{
			std::cin >> userInput;
			// 'Valid' means within the range of numbers outputted
			if(userInput >= 0 && userInput <= this->choices.size())
			{
				return userInput;
			}
		}

		return 0;
	}

	Dialogue(std::string description, std::vector<std::string> choices)
	{
		this->description = description;
		this->choices = choices;
	}

	Dialogue()
	{
	}
};

#endif /* DIALOGUE_HPP */
