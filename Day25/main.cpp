#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"
#include <xmmintrin.h>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	static constexpr size_t MAX_LOCKS_AND_KEYS = 250;
	int m_totalKeys;
	int m_totalLocks;
	__m128i m_keys[MAX_LOCKS_AND_KEYS];
	__m128i m_locks[MAX_LOCKS_AND_KEYS];

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;

		uint8_t alignas(16) tumbler[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		int totalLines = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.empty())
			{
				memset(tumbler, 0, sizeof(tumbler) * sizeof(uint8_t));
				continue;
			}

			// It's a Lock
			if (!line.compare("#####"))
			{
				//for (size_t k = 0; k < line.size(); ++k)
				//{
				//	if (line[k] == '#')
				//	{
				//		++tumbler[k];
				//	}
				//}

				for (int i = 1; i < 7; ++i)
				{
					line = fileReader.ReadLine();
					for (size_t k = 0; k < line.size(); ++k)
					{
						if (line[k] == '#')
						{
							++tumbler[k];
						}
					}
				}

				for (int i = 0; i < 5; ++i)
				{
					tumbler[i] = 6 - tumbler[i];
				}

				m_locks[m_totalLocks++] = _mm_load_si128((const __m128i*)tumbler);
			}
			else
			{
				for (int i = 1; i < 6; ++i)
				{
					line = fileReader.ReadLine();
					for (size_t k = 0; k < line.size(); ++k)
					{
						if (line[k] == '#')
						{
							++tumbler[k];
						}
					}
				}
				line = fileReader.ReadLine();
				m_keys[m_totalKeys++] = _mm_load_si128((const __m128i*)tumbler);
			}
		}

		Log("Read [%d] Keys + Locks", m_totalKeys + m_totalLocks);

	}

	bool KeyFitsLock(const __m128i& lock, const __m128i& key) const
	{
		__m128i delta = _mm_sub_epi8(lock, key);
		int mm = _mm_movemask_epi8(_mm_cmpgt_epi8( delta, _mm_setzero_si128()));
		return mm == ((1 << 5) - 1);
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int totalValidCombos = 0;
		for (int i = 0; i < m_totalKeys; ++i)
		{
			for (int j = 0; j < m_totalLocks; ++j)
			{
				if (KeyFitsLock(m_locks[j], m_keys[i]))
				{
					++totalValidCombos;
				}
			}
		}

		Log("Total Combos: %d", totalValidCombos);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 25;
	newParams.year = 2024;
	newParams.puzzleTitle = "Code Chronicle";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
