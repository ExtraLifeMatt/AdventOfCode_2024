#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"

#include <unordered_map>
#include <unordered_set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) { m_mostBananas = 0; };

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_MemNumbers.push_back((uint32_t)atoi(line.c_str()));
			m_buyers.emplace_back();
			InitializeBuyer(m_MemNumbers.back(), m_buyers.back());
		}

		uint32_t expectedValues[] = { 15887950,
										16495136,
										527345,
										704524,
										1553684,
										12683156,
										11100544,
										12249484,
										7753432,
										5908254 };
		uint32_t testValue = 123;
		for (int i = 0; i < ARRAY_SIZE(expectedValues); ++i)
		{
			testValue = MutateN(testValue, 1);
			assert(testValue == expectedValues[i]);
		}
	}

	struct alignas(16) Buyer
	{
		uint32_t seeds[2000];
		int8_t deltas[2000];
		uint32_t values[2000];
	};

	uint32_t EncodeSequenceAndValue(int8_t a, int8_t b, int8_t c, int8_t d) const
	{
		constexpr uint32_t channel_mask = (1 << 4) - 1;
		constexpr uint32_t channel_two_comp_bit = 1 << 4;

		uint32_t out = 0;
		out |= ((a & channel_mask) | (a < 0 ? channel_two_comp_bit : 0));
		out |= ((b & channel_mask) | (b < 0 ? channel_two_comp_bit : 0)) << 5;
		out |= ((c & channel_mask) | (c < 0 ? channel_two_comp_bit : 0)) << 10;
		out |= ((d & channel_mask) | (d < 0 ? channel_two_comp_bit : 0)) << 15;

		return out;
	}

	bool SequenceCompare(uint32_t a, uint32_t b) const
	{
		constexpr uint32_t sequence_only_mask = (1 << 20) - 1;
		return (a & sequence_only_mask) == (b & sequence_only_mask);
	}

	uint16_t GetSequenceValue(uint32_t a) const
	{
		return a >> 20;
	}

	uint32_t GetLastDigit(uint32_t v)
	{
		return v % 10;
	}

	typedef std::unordered_map<uint32_t, uint32_t> SequenceToTotalMap;
	
	void InitializeBuyer(uint32_t seed, Buyer& outBuyer)
	{
		uint32_t lastValue = seed;
		uint32_t currentValue = seed;
		outBuyer.seeds[0] = seed;
		outBuyer.deltas[0] = 0;
		outBuyer.values[0] = GetLastDigit(seed);

		std::unordered_set<uint32_t> addedSequences;
		for (int i = 1; i < 2000; ++i)
		{
			currentValue = Mutate(lastValue);
			outBuyer.seeds[i] = currentValue;
			outBuyer.values[i] = GetLastDigit(currentValue);
			outBuyer.deltas[i] = outBuyer.values[i - 1] - outBuyer.values[i];

			lastValue = currentValue;

			if (i >= 3)
			{
				uint32_t sequence = EncodeSequenceAndValue(outBuyer.deltas[i - 3], outBuyer.deltas[i - 2], outBuyer.deltas[i - 1], outBuyer.deltas[i]);
				if (addedSequences.find(sequence) == addedSequences.end())
				{
					SequenceToTotalMap::iterator itFind = m_sequenceToTotal.find(sequence);
					if (itFind == m_sequenceToTotal.end())
					{
						itFind = m_sequenceToTotal.insert(std::make_pair(sequence, 0)).first;
					}
					assert(itFind != m_sequenceToTotal.end());
					itFind->second += outBuyer.values[i];

					if (itFind->second > m_mostBananas)
					{
						m_mostBananas = itFind->second;
					}

					addedSequences.insert(sequence);
				}
			}
		}
	}

	uint32_t Mutate(uint32_t a) const
	{
		constexpr uint32_t modMask = (1 << 24) - 1; // % 16777216UL
		uint32_t a0 = a;
		a0 = (a0 ^ (a0 << 6)) & modMask;
		a0 = (a0 ^ (a0 >> 5)) & modMask;
		a0 = (a0 ^ (a0 << 11)) & modMask;
		return a0;
	}

	uint32_t MutateN(uint32_t a, int32_t numSteps) const
	{
		uint32_t out = a;

		for (int32_t i = 0; i < numSteps; ++i)
		{
			out = Mutate(out);
		}

		return out;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<uint32_t> mutatedSeeds;
		mutatedSeeds.reserve(m_MemNumbers.size());

		uint64_t sum = 0;
		for (uint32_t seed : m_MemNumbers)
		{
			mutatedSeeds.push_back(MutateN(seed, 2000));
			sum += mutatedSeeds.back();
		}

		Log("Sum = %llu", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		Log("Most Bananas: %u", m_mostBananas);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<uint32_t> m_MemNumbers;
	std::vector<Buyer> m_buyers;

	SequenceToTotalMap m_sequenceToTotal;
	uint32_t m_mostBananas;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 22;
	newParams.year = 2024;
	newParams.puzzleTitle = "Monkey Market";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
