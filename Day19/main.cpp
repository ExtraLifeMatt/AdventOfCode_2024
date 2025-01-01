#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"


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
		line = fileReader.ReadLine();
		while (!line.empty())
		{
			StringUtil::SplitBy(line, ",", tokens);
			m_keys.insert(m_keys.end(), tokens.begin(), tokens.end());
			line = fileReader.ReadLine();
		}

		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_inputs.push_back(line);
		}

		std::sort(m_keys.begin(), m_keys.end(), [](const std::string& LHS, const std::string& RHS){ return LHS.size() > RHS.size(); });

	}

	bool IsValidPerm(const std::string& desired, size_t offset) const
	{
		if (offset == desired.size())
		{
			return true;

		}

		for (const std::string& key : m_keys)
		{
			if (desired.compare(offset, key.size(), key) == 0)
			{
				if (IsValidPerm(desired, offset + key.size()))
				{
					return true;
				}
			}
		}

		return false;
	}

	uint64_t CountValidPerm(const std::string& desired, size_t offset, std::unordered_map<std::string, uint64_t>& cache) const
	{
		if (offset == desired.size())
		{
			return 1;
		}

		std::unordered_map<std::string, uint64_t>::const_iterator itFind = cache.find(desired.substr(offset));
		if (itFind != cache.end())
		{
			return itFind->second;
		}

		uint64_t validCount = 0;

		for (const std::string& key : m_keys)
		{
			if (desired.compare(offset, key.size(), key) == 0)
			{
				validCount += CountValidPerm(desired, offset + key.size(), cache);
			}
		}

		cache.insert(std::make_pair(desired.substr(offset), validCount));

		return validCount;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int totalValid = 0;

		for (const std::string& input : m_inputs)
		{
			if (IsValidPerm(input, 0))
			{
				++totalValid;
			}
		}

		Log("Total Valid = %d", totalValid);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t totalValid = 0;
		std::unordered_map<std::string, uint64_t> cache;

		for (const std::string& input : m_inputs)
		{
			totalValid += CountValidPerm(input, 0, cache);
		}

		Log("Total Valid = %u", totalValid);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<std::string> m_keys;
	std::vector<std::string> m_inputs;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 19;
	newParams.year = 2024;
	newParams.puzzleTitle = "Linen Layout";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
