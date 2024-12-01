// Day 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

#include "ACUtils/StringUtil.h"
#include <array>
#include <string>
#include <algorithm>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (!line.empty())
			{
				StringUtil::SplitBy(line, " ", tokens, true);
				assert(tokens.size() == 2);
				m_leftSide.push_back(atoi(tokens[0].c_str()));
				m_rightSide.push_back(atoi(tokens[1].c_str()));

				std::unordered_map<int, int>::iterator itFind = m_occuranceMap.find(m_rightSide.back());
				if (itFind == m_occuranceMap.end())
				{
					m_occuranceMap.insert(std::make_pair(m_rightSide.back(), 1));
				}
				else
				{
					itFind->second++;
				}
			}
		}

		std::sort(m_leftSide.begin(), m_leftSide.end());
		std::sort(m_rightSide.begin(), m_rightSide.end());
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int sum = 0;
		for (size_t i = 0; i < m_leftSide.size(); ++i)
		{
			sum += abs(m_leftSide[i] - m_rightSide[i]);
		}

		Log("Total Sum = %d", sum);

		// Done.
		 AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t sum = 0;
		std::unordered_map<int, int>::const_iterator itFind;
		for (size_t i = 0; i < m_leftSide.size(); ++i)
		{
			itFind = m_occuranceMap.find(m_leftSide[i]);
			if (itFind != m_occuranceMap.end())
			{
				sum += m_leftSide[i] * itFind->second;
			}
		}

		Log("Total Sum = %llu", sum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<int> m_leftSide;
	std::vector<int> m_rightSide;
	std::unordered_map<int, int> m_occuranceMap;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 1;
	newParams.year = 2024;
	newParams.puzzleTitle = "Historian Hysteria";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
