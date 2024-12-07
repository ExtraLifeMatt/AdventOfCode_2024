#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include <unordered_set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	struct Rule
	{
		Rule(int _value):value(_value) {};

		int value;
		std::unordered_set<int> before;
	};

	struct Update
	{
		std::vector<int> input;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		size_t barIdx = std::string::npos;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			barIdx = line.find('|');
			if (barIdx != std::string::npos)
			{
				assert(barIdx != std::string::npos);
				tokens.clear();
				tokens.push_back(line.substr(0, barIdx));
				tokens.push_back(line.substr(barIdx + 1));
				int page = atoi(tokens[0].c_str());
				int before = atoi(tokens[1].c_str());

				int existingIndex = GetRuleIndexForPage(page);
				if (existingIndex == -1)
				{
					m_rules.emplace_back(page);
					m_rules.back().before.insert(before);
				}
				else
				{
					m_rules[existingIndex].before.insert(before);
				}

				existingIndex = GetRuleIndexForPage(before);
				if (existingIndex == -1)
				{
					m_rules.emplace_back(before);
				}
			}
			else if (!line.empty())
			{
				StringUtil::SplitBy(line, ",", tokens);
				m_updates.emplace_back();
				Update& newUpdate = m_updates.back();
				for (const std::string& tok : tokens)
				{
					newUpdate.input.push_back(atoi(tok.c_str()));
				}
			}
		}
	}

	int GetRuleIndexForPage(int page) const
	{
		return Algorithm::find_index_of(m_rules.begin(), m_rules.end(), [=](const Rule& LHS) { return LHS.value == page; });
	}

	bool IsUpdateValid(const Update& update) const
	{
		int ruleIdx = -1;
		for(size_t i = 0; i < update.input.size(); ++i)
		{
			ruleIdx = GetRuleIndexForPage(update.input[i]);
			if (ruleIdx != -1)
			{
				const Rule& pageRule = m_rules[ruleIdx];
				for (size_t j = 0; j < update.input.size(); ++j)
				{
					if (j == i)
					{
						continue;
					}

					int otherPage = update.input[j];
					if (pageRule.before.find(otherPage) != pageRule.before.end())
					{
						if (j < i)
						{
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int sum = 0;
		std::vector<int> temp;
		for (const Update& update : m_updates)
		{
			if (IsUpdateValid(update))
			{
				sum += update.input[update.input.size() / 2];
				continue;
			}
		}

		Log("Sum = %d", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		int sum = 0;
		std::vector<int> temp;
		for (const Update& update : m_updates)
		{
			if (IsUpdateValid(update))
			{
				continue;
			}

			temp.clear();
			temp.reserve(update.input.size());
			temp.insert(temp.begin(), update.input.begin(), update.input.end());
			std::sort(temp.begin(), temp.end(), [&, this](const int LHS, const int RHS)
				{
					int LHSpageIndex = GetRuleIndexForPage(LHS); 
					int RHSpageIndex = GetRuleIndexForPage(RHS);
						
					if (LHSpageIndex != -1) // See if A must come before B
					{
						const Rule& LHSRule = m_rules[LHSpageIndex];
						if (LHSRule.before.find(RHS) != LHSRule.before.end())
						{
							return true;
						}
					}

					if (RHSpageIndex != -1)
					{
						const Rule& RHSRule = m_rules[RHSpageIndex];
						if (RHSRule.before.find(LHS) != RHSRule.before.end()) // If B must come before A
						{
							return false;
						}
					}

					// Default, no rules so they are fine.
					return LHS < RHS;
				});

			sum += temp[temp.size() / 2];
		}

		Log("Sum = %d", sum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<Rule> m_rules;
	std::vector<Update> m_updates;

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 5;
	newParams.year = 2024;
	newParams.puzzleTitle = "Print Queue";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
