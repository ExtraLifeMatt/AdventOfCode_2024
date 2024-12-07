#include "AdventGUI/AdventGUI.h"
#include <ACUtils/IntVec.h>
#include <ACUtils/Math.h>
#include <unordered_set>

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

			if (m_MapWidth == 0)
			{
				m_MapWidth = (int)line.size();
			}
			else
			{
				assert(m_MapWidth == (int)line.size());
			}

			size_t startLocIdx = line.find('^');
			if (startLocIdx != std::string::npos)
			{
				m_StartPos.x = (int)startLocIdx;
				m_StartPos.y = (int)m_Map.size() / m_MapWidth;
				line[startLocIdx] = '.';
			}
			
			m_Map.append(line);
		}

		m_MapHeight = (int)m_Map.size() / m_MapWidth;
	}

	bool IsValidPos(const IntVec2& pos) const { return m_Map[pos.y * m_MapWidth + pos.x] == '.'; }

	bool CanLeaveMap(const std::string& mapData, uint64_t& outPathHash) const
	{
		std::unordered_set<IntVec4> history;

		IntVec2 currPos = m_StartPos;
		IntVec2 nextPos = currPos;
		IntVec2 cwDir[4] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
		int dirIdx = 0; // Always start facing "up" which is -1 since we are in a top to bottom array.
		while (true)
		{
			history.insert(IntVec4(currPos.x, currPos.y, cwDir[dirIdx].x, cwDir[dirIdx].y));

			nextPos = currPos + cwDir[dirIdx];

			if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= m_MapWidth || nextPos.y >= m_MapHeight)
			{
				// left the area.
				return true;
			}

			if (mapData[nextPos.y * m_MapWidth + nextPos.x] == '#') // Blocked?
			{
				dirIdx = (dirIdx + 1) & 3; // %4;
				nextPos = currPos;
			}

			currPos = nextPos;

			if (history.find(IntVec4(currPos.x, currPos.y, cwDir[dirIdx].x, cwDir[dirIdx].y)) != history.end())
			{
				outPathHash = 0;
				for (const IntVec4& pathHistory : history)
				{
					outPathHash += pathHistory.ToHash();
				}
				return false;
			}
		}

		return false;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		IntVec2 currPos = m_StartPos;
		IntVec2 nextPos = currPos;

		IntVec2 cwDir[4] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
		int dirIdx = 0; // Always start facing "up" which is -1 since we are in a top to bottom array.
		while (true)
		{
			m_path.insert(currPos);

			nextPos = currPos + cwDir[dirIdx];

			if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= m_MapWidth || nextPos.y >= m_MapHeight)
			{
				// left the area.
				break;
			}

			if (!IsValidPos(nextPos)) // Blocked?
			{
				dirIdx = (dirIdx + 1) & 3; // %4;
			}

			currPos = nextPos;
		}

		Log("Unique steps = %zd", m_path.size());

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::string candidateMap;
		std::unordered_set<uint64_t> variations;
		uint64_t pathCrc = 0;
		for (const IntVec2 tile : m_path)
		{
			if (tile == m_StartPos)
			{
				continue;
			}

			candidateMap = m_Map;
			candidateMap[tile.y * m_MapWidth + tile.x] = '#';
			
			if (!CanLeaveMap(candidateMap, pathCrc))
			{
				variations.insert(pathCrc);
			}
		}

		Log("Variations = %zd", variations.size());

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	IntVec2 m_StartPos;
	int m_MapWidth;
	int m_MapHeight;
	std::string m_Map;
	std::unordered_set<IntVec2> m_path; // for Part 2

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 6;
	newParams.year = 2024;
	newParams.puzzleTitle = "Print Queue";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
