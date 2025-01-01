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
		m_mapWidth = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_mapWidth == 0)
			{
				m_mapWidth = line.size();
			}
			assert(m_mapWidth == line.size());

			if (m_startPos.IsZero())
			{
				size_t foundIndex = line.find('S');
				if (foundIndex != std::string::npos)
				{
					m_startPos.y = (int32_t)(m_map.size() / m_mapWidth);
					m_startPos.x = (int32_t)foundIndex;
					line[foundIndex] = '.';
				}
			}

			if (m_goalPos.IsZero())
			{
				size_t foundIndex = line.find('E');
				if (foundIndex != std::string::npos)
				{
					m_goalPos.y = (int32_t)(m_map.size() / m_mapWidth);
					m_goalPos.x = (int32_t)foundIndex;
					line[foundIndex] = '.';
				}
			}

			m_map.insert(m_map.end(), line.begin(), line.end());
		}

		m_mapHeight = m_map.size() / m_mapWidth;

		// Map has no branches, just walk until we hit the goal
		IntVec2 last;
		IntVec2 walk = m_startPos;
		m_pathway.push_back(walk);

		while (walk != m_goalPos)
		{
			const IntVec2 dirstwo[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

			IntVec2 delta;
			for (const IntVec2& dir : dirstwo)
			{
				delta = walk + dir;

				if (IsBlocked(delta) || delta == last)
				{
					continue;
				}

				last = walk;
				walk = delta;
				m_pathway.push_back(walk);
				break;
			}
		}
	}

	bool IsBlocked(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
		{
			return true;
		}

		return m_map[pos.y * m_mapWidth + pos.x] == '#';
	}

	int32_t GetManhattanDistance(const IntVec2& A, const IntVec2& B) const
	{
		return abs(A.x - B.x) + abs(A.y - B.y);
	}

	int32_t CountShortcuts(int32_t shortcutLength, int32_t minCost) const
	{
		int totalShortcuts = 0;
		int32_t dist = 0;
		for (int32_t i = 0; i < (int32_t)m_pathway.size() - 1; ++i)
		{
			for (int32_t j = i + 1; j < (int32_t)m_pathway.size(); ++j)
			{
				dist = GetManhattanDistance(m_pathway[i], m_pathway[j]);
				if (dist <= shortcutLength)
				{
					int delta = j - i - dist;
					if (delta >= minCost)
					{
						++totalShortcuts;
					}
				}
			}
		}

		return totalShortcuts;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		Log("Shortcuts = %d", CountShortcuts(2, 100) );
	
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		Log("Shortcuts = %d", CountShortcuts(20, 100));

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	IntVec2 m_startPos;
	IntVec2 m_goalPos;
	size_t m_mapWidth;
	size_t m_mapHeight;
	std::vector<char> m_map;
	std::vector<IntVec2> m_pathway;

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 20;
	newParams.year = 2024;
	newParams.puzzleTitle = "Race Condition";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
