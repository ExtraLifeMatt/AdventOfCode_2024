#include "AdventGUI/AdventGUI.h"
#include <regex>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		fileReader.ReadAllLines(true, m_input);
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::regex mulRegex("(mul\\(\\d+,\\d+\\))");
		std::string foundMatch;
		size_t leftParen = std::string::npos;
		size_t rightParen = std::string::npos;
		size_t comma = std::string::npos;

		std::string leftValue;
		std::string rightValue;

		int64_t sum = 0;

		for (const std::string& inputline : m_input)
		{
			std::sregex_iterator it = std::sregex_iterator(inputline.begin(), inputline.end(), mulRegex);
			while (it != std::sregex_iterator())
			{
				foundMatch = it->str();
				leftParen = foundMatch.find('(');
				assert(leftParen != std::string::npos);
				comma = foundMatch.find(',');
				assert(comma != std::string::npos);
				rightParen = foundMatch.find(')');
				assert(rightParen != std::string::npos);

				leftValue = foundMatch.substr(leftParen + 1, comma - leftParen - 1);
				rightValue = foundMatch.substr(comma + 1, rightParen - comma - 1);

				sum += atoi(leftValue.c_str()) * atoi(rightValue.c_str());

				++it;
			}
		}

		Log("Sum = %lld", sum);

		// Done.
		 AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::regex mulRegex("((do[^n])|(don\\'t)|(mul\\(\\d+,\\d+\\)))");
		std::string foundMatch;
		size_t leftParen = std::string::npos;
		size_t rightParen = std::string::npos;
		size_t comma = std::string::npos;

		std::string leftValue;
		std::string rightValue;
		
		bool enabled = true;
		int64_t sum = 0;

		for (const std::string& inputline : m_input)
		{
			std::sregex_iterator it = std::sregex_iterator(inputline.begin(), inputline.end(), mulRegex);
			while (it != std::sregex_iterator())
			{
				foundMatch = it->str();
				
				if (foundMatch.find("don't") != std::string::npos)
				{
					enabled = false;
				}
				else if (foundMatch.find("do") != std::string::npos)
				{
					enabled = true;
				}
				else if (enabled)
				{					
					leftParen = foundMatch.find('(');
					assert(leftParen != std::string::npos);
					comma = foundMatch.find(',');
					assert(comma != std::string::npos);
					rightParen = foundMatch.find(')');
					assert(rightParen != std::string::npos);

					leftValue = foundMatch.substr(leftParen + 1, comma - leftParen - 1);
					rightValue = foundMatch.substr(comma + 1, rightParen - comma - 1);

					
					sum += atoi(leftValue.c_str()) * atoi(rightValue.c_str());
				}
				
				++it;
			}
		}

		Log("Sum = %lld", sum);
		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<std::string> m_input; 
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 3;
	newParams.year = 2024;
	newParams.puzzleTitle = "Mull It Over";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
