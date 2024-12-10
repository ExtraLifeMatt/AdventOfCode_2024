#include "AdventGUI/AdventGUI.h"

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
			m_input.append(line);
		}

	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<uint32_t> expandedBlocks;
		expandedBlocks.reserve(m_input.size() * 9);
		uint32_t currentId = 0;
		uint32_t numBlocks = 0;
		std::string::const_iterator itIter = m_input.begin();
		while (itIter != m_input.end())
		{
			// ID
			expandedBlocks.insert(expandedBlocks.end(), *itIter - '0', currentId++);
			++itIter;

			if (itIter == m_input.end())
			{
				break;
			}

			expandedBlocks.insert(expandedBlocks.end(), *itIter - '0', ~0U);
			++itIter;
		}

		size_t i = 0;
		size_t j = expandedBlocks.size() -1;
		while (true)
		{
			if (i >= j)
			{
				break;
			}

			if (expandedBlocks[i] != ~0U)
			{
				++i;
			}
			else if (expandedBlocks[j] == ~0U)
			{
				--j;
			}
			else
			{
				std::swap(expandedBlocks[i], expandedBlocks[j]);
			}
		}

		uint64_t totalSum = 0;
		for (size_t i = 0; i < expandedBlocks.size(); ++i)
		{
			if (expandedBlocks[i] == ~0U)
			{
				break;
			}

			totalSum += expandedBlocks[i] * (uint64_t)i;
		}

		Log("Total Sum = %llu", totalSum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	struct FileSector
	{
		FileSector(int _id, int _size)
		: id(static_cast<uint32_t>(_id)),
		size(static_cast<uint16_t>(_size))
		{
		}

		uint32_t id;
		uint16_t size;
	};

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::vector<FileSector> expandedBlocks;
		expandedBlocks.reserve(m_input.size() * 9);
		uint32_t currentId = 0;
		uint32_t numBlocks = 0;
		std::string::const_iterator itIter = m_input.begin();
		while (itIter != m_input.end())
		{
			// ID
			expandedBlocks.push_back(FileSector(currentId++, *itIter - '0'));
			++itIter;

			if (itIter == m_input.end())
			{
				break;
			}

			expandedBlocks.push_back(FileSector(~0U, *itIter - '0'));
			++itIter;
		}

		size_t i = 0;
		size_t j = expandedBlocks.size() - 1;
		while (i < expandedBlocks.size())
		{
			if (expandedBlocks[i].id != ~0U)
			{
				++i;
			}
			else if (expandedBlocks[j].id == ~0U || expandedBlocks[j].size > expandedBlocks[i].size)
			{
				if (j <= i)
				{
					++i;
					j = expandedBlocks.size() - 1;
				}
				else
				{
					--j;
				}
			}
			else
			{
				if (expandedBlocks[i].size == expandedBlocks[j].size) // Same size, just swap.
				{
					std::swap(expandedBlocks[i], expandedBlocks[j]);
				}
				else
				{
					FileSector remainder(~0U, expandedBlocks[i].size - expandedBlocks[j].size);
					std::swap(expandedBlocks[i], expandedBlocks[j]);
					expandedBlocks[j].id = ~0U;
					expandedBlocks[j].size = expandedBlocks[i].size;
					expandedBlocks.insert(expandedBlocks.begin() + i + 1, remainder);
				}

				j = expandedBlocks.size() - 1;
			}
		}

		uint64_t totalSum = 0;
		uint64_t offset = 0;
		for (size_t i = 0; i < expandedBlocks.size(); ++i)
		{
			const FileSector& currentFile = expandedBlocks[i];
			if (currentFile.id != ~0U)
			{
				for (size_t j = 0; j < currentFile.size; ++j)
				{
					totalSum += currentFile.id * (offset + j);
				}
			}

			offset += currentFile.size;
		}

		Log("Total Sum = %llu", totalSum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::string m_input;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 9;
	newParams.year = 2024;
	newParams.puzzleTitle = "Disk Fragmenter";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
