#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/Hash.h"
#include "ACUtils/Math.h"
#include <unordered_map>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:

	typedef std::unordered_map<uint64_t, uint64_t> PebbleMap;

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, " ", tokens);
			for (const std::string& tok : tokens)
			{
				m_pebbles.push_back(StringUtil::AtoiU64(tok.c_str()));
			}
		}

		uint64_t testValues[] = { 1, 14, 32, 129, 3329, 23409, 1238902, 129384, 3490250, 98643276, 122893098, 1999992314 };
		for (uint64_t tst : testValues)
		{
			assert(Math::Log10(tst) == (uint32_t)log10l((long double)tst));
		}

	}

	void ResetMap(PebbleMap& dst) const
	{
		for (std::pair<const uint64_t, uint64_t>& kvp : dst)
		{
			kvp.second = 0;
		}
	}

	void AddEntry(uint64_t value, uint64_t count, PebbleMap& dst) const
	{
		PebbleMap::iterator itFind = dst.find(value);
		if (itFind == dst.end())
		{
			dst.insert(std::make_pair(value, count));
		}
		else
		{
			itFind->second += count;
		}
	}

	void Step(const PebbleMap& src, PebbleMap& dst) const
	{
		char splitBuffer[32] = { 0 };
		char left[16] = { 0 };
		char right[16] = { 0 };

		for (const std::pair<const uint64_t, uint64_t>& kvp : src)
		{
			if (kvp.first == 0)
			{
				AddEntry(1, kvp.second, dst);
			}
			else if (((Math::Log10(kvp.first) + 1) & 1) == 0)
			{
				sprintf_s(splitBuffer, "%llu", kvp.first);
				size_t len = strlen(splitBuffer) / 2;
				memcpy(left, splitBuffer, len);
				left[len] = '\0';
				memcpy(right, splitBuffer + len, len);
				right[len] = '\0';

				AddEntry(StringUtil::AtoiU64(left), kvp.second, dst);
				AddEntry(StringUtil::AtoiU64(right), kvp.second, dst);
			}
			else
			{
				AddEntry(kvp.first * 2024ULL, kvp.second, dst);
			}
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<uint64_t> pingPong[2];
		PebbleMap mapPing[2];
		uint32_t idx = 0;
		uint32_t numSteps = 25;

		pingPong[0].insert(pingPong[0].begin(), m_pebbles.begin(), m_pebbles.end());
		for (const uint64_t peb : m_pebbles)
		{
			AddEntry(peb, 1, mapPing[0]);
		}

		for (uint32_t i = 0; i < numSteps; ++i)
		{
			// Map version
			mapPing[idx ^ 1] = mapPing[idx];
			ResetMap(mapPing[idx ^ 1]);
			Step(mapPing[idx], mapPing[idx ^1]);

			idx ^=1;
		}

		uint64_t mapTotal = 0;
		for (const std::pair<uint64_t, uint64_t>& kvp : mapPing[idx])
		{
			mapTotal += kvp.second;
		}

		Log("Total After %u steps - %llu", numSteps, mapTotal);

		// Keep the party going...
		uint32_t additionalSteps = 50;
		for (uint32_t i = 0; i < additionalSteps; ++i)
		{
			// Map version
			mapPing[idx ^ 1] = mapPing[idx];
			ResetMap(mapPing[idx ^ 1]);
			Step(mapPing[idx], mapPing[idx ^ 1]);

			idx ^= 1;
		}

		mapTotal = 0;
		for (const std::pair<uint64_t, uint64_t>& kvp : mapPing[idx])
		{
			mapTotal += kvp.second;
		}

		Log("Total After %u steps - %llu", numSteps + additionalSteps, mapTotal);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<uint64_t> m_pebbles;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 11;
	newParams.year = 2024;
	newParams.puzzleTitle = "Plutonian Pebbles";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
