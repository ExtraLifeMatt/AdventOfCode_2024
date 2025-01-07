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
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_MemNumbers.push_back(StringUtil::AtoiU64(line.c_str()));
		}

		uint64_t expectedValues[] = { 15887950,
										16495136,
										527345,
										704524,
										1553684,
										12683156,
										11100544,
										12249484,
										7753432,
										5908254 };
		uint64_t testValue = 123;
		for (int i = 0; i < ARRAY_SIZE(expectedValues); ++i)
		{
			testValue = MutateN(testValue, 1);
			assert(testValue == expectedValues[i]);
		}
	}

	uint64_t MixSlow(uint64_t a, uint64_t b) const
	{
		return a ^ b;
	}

	uint64_t PruneSlow(uint64_t a) const
	{
		return a % 16777216;
	}

	uint64_t MutateSlow(uint64_t a) const
	{
		uint64_t a0 = a;
		a0 = PruneSlow(MixSlow(a0, a0 * 64));
		a0 = PruneSlow(MixSlow(a0, a0 / 32));
		a0 = PruneSlow(MixSlow(a0, a0 * 2048));

		return a0;
	}

	uint64_t Mutate(uint64_t a) const
	{
		constexpr uint64_t modMask = (1UL << 24UL) - 1; // % 16777216UL
		uint64_t a0 = a;
		a0 = (a0 ^ (a0 << 6UL)) & modMask;
		a0 = (a0 ^ (a0 >> 5UL)) & modMask;
		a0 = (a0 ^ (a0 << 11UL)) & modMask;
		return a0;
	}

	uint64_t MutateN(uint64_t a, int32_t numSteps) const
	{
		uint64_t out = a;

		for (int32_t i = 0; i < numSteps; ++i)
		{
			out = Mutate(out);
		}

		return out;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<uint64_t> mutatedSeeds;
		mutatedSeeds.reserve(m_MemNumbers.size());

		uint64_t sum = 0;
		for (uint64_t seed : m_MemNumbers)
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


		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<uint64_t> m_MemNumbers;
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
