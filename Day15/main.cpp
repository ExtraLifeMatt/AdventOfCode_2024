#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"


class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params), m_mapWidth(0), m_mapHeight(0) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.empty())
			{
				continue;
			}
			
			if (line[0] == '#')
			{
				if (m_mapWidth == 0)
				{
					m_mapWidth = line.size();
				}
				assert(m_mapWidth == line.size());

				m_map.insert(m_map.end(), line.begin(), line.end());
			}
			else
			{
				assert(line[0] == '<' || line[0] == '>' || line[0] == 'v' || line[0] == '^');
				m_input.insert(m_input.end(), line.begin(), line.end());
			}
		}

		m_mapHeight = m_map.size() / m_mapWidth;

		// Find start pos, replace char.
		std::vector<char>::iterator itFind = std::find(m_map.begin(), m_map.end(), '@');
		assert(itFind != m_map.end());
		size_t delta = itFind - m_map.begin();
		m_startPos.x = (int32_t)(delta % m_mapWidth);
		m_startPos.y = (int32_t)(delta / m_mapWidth);

		*itFind = '.'; 

	}

	char GetData(const std::vector<char>& state, const IntVec2& pos, int32_t mapWidth) const
	{
		return state[pos.y * mapWidth + pos.x];
	}

	bool CanShift(const std::vector<char>& state, const IntVec2& pos, const IntVec2& dir, uint8_t& outSteps) const
	{
		IntVec2 nextPos = pos + dir;
		char c = '\0';
		outSteps = 0;
		while (true)
		{
			char c = GetData(state, nextPos, m_mapWidth);
			if (c == '.')
			{
				return true;
			}

			if (c == '#')
			{
				break;
			}

			assert(c == 'O');
			nextPos += dir;
			++outSteps;
		}

		return false;
	}


	void Step(const std::vector<char>& state, const IntVec2& pos, const IntVec2& dir, int32_t mapWidth, std::vector<char>& outSim, IntVec2& outPos) const
	{
		assert(state.size() == outSim.size());
		memcpy(outSim.data(), state.data(), sizeof(char) * state.size());

		IntVec2 delta = pos + dir;
		uint8_t numSteps = 0;
		char c = GetData(state, delta, mapWidth);
		switch (c)
		{
			case '.':
			{
				outPos = delta;
			}
			break;
			case '#':
			{
				outPos = pos;
			}
			break;
			case 'O':
			{
				if (CanShift(state, pos, dir, numSteps))
				{
					IntVec2 stepPos;
					for(uint8_t i = 0; i < numSteps; ++i)
					{
						stepPos = delta + dir * (int32_t)(i + 1);
						outSim[stepPos.y * m_mapWidth + stepPos.x] = 'O';
					}

					outSim[delta.y * m_mapWidth + delta.x] = '.';

					outPos = delta;
				}
				else
				{
					outPos = pos;
				}
			}
			break;
			case '[':
			case ']':
			{
				if (dir.y == 0)
				{
					if (CanMoveHorizontal(state, pos, dir, mapWidth))
					{
						HorizontalMove(state, pos, '.', dir, mapWidth, outSim);

						outPos = delta;
					}
					else
					{
						outPos = pos;
					}

				}
				else
				{
					if (CanMoveVertically(state, pos, dir, mapWidth) && CanMoveVertically(state, pos + (c == ']' ? IntVec2(-1, 0) : IntVec2(1, 0)), dir, mapWidth))
					{
						MoveVertically(pos, dir, mapWidth, outSim);
						//VerticalMove(state, pos,  dir, mapWidth, outSim);
						//IntVec2 ddx = pos + (c == ']' ? IntVec2(-1, 0) : IntVec2(1, 0)) + dir;
						//outSim[ddx.y * mapWidth + ddx.x] = '.';
			
						outPos = delta;
					}
					else
					{
						outPos = pos;
					}
				}

			}
			break;
			default: 
			assert(false); 
			break;
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<char> pingPong[2] = {m_map, m_map};
		int32_t ppIdx = 0;
		IntVec2 dir;

		IntVec2 currPos = m_startPos;
		IntVec2 nextPos;

		for (size_t i = 0; i < m_input.size(); ++i)
		{
			switch (m_input[i])
			{
				case '<': dir = IntVec2(-1, 0); break;
				case '>': dir = IntVec2(1, 0); break;
				case '^': dir = IntVec2(0, -1); break;
				case 'v': dir = IntVec2(0, 1); break;
				default: assert(false); break;
			}

			Step(pingPong[ppIdx], currPos, dir, m_mapWidth, pingPong[ppIdx ^ 1], nextPos);
			//Log("Step %zd: Pos [%d, %d] -> [%d, %d]", i + 1, currPos.x, currPos.y, nextPos.x, nextPos.y);
			currPos = nextPos;
			ppIdx^=1;
		}

		uint32_t GPSSum = 0;
		const std::vector<char>& lastState = pingPong[ppIdx ^ 1];
		for (size_t i = 0; i < lastState.size(); ++i)
		{
			if (lastState[i] == 'O')
			{
				GPSSum += (i / m_mapWidth) * 100 + i % m_mapWidth;
			}
		}

		Log("GPS Sum = %u", GPSSum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	void Expand(const std::vector<char>& inMap, std::vector<char>& outMap) const
	{
		outMap.reserve(inMap.size() * 2);
		for (char c : inMap)
		{
			switch (c)
			{
			case '#':outMap.insert(outMap.end(), 2, '#'); break;
			case 'O':outMap.insert(outMap.end(), '['); outMap.insert(outMap.end(), ']'); break;
			case '.':outMap.insert(outMap.end(), 2, '.'); break;
			default: assert(false); break;
			}
		}
	}

	bool CanMoveHorizontal(const std::vector<char>& state, const IntVec2& boxPos, const IntVec2& dir, int32_t mapWidth) const
	{
		IntVec2 ddx = boxPos + dir;
		switch(GetData(state, ddx, mapWidth))
		{
		case '#': return false;
		case '.': return true;
		default: break;
		}

		return CanMoveHorizontal(state, ddx, dir, mapWidth);
	}

	bool CanMoveVertically(const std::vector<char>& state, const IntVec2& boxPos, const IntVec2& dir, int32_t mapWidth) const
	{
		IntVec2 ddy = boxPos + dir;
		switch (GetData(state, ddy, mapWidth))
		{
			case '#': return false;
			case '.': return true;
			case '[':
			{
				return CanMoveVertically(state, ddy, dir, mapWidth) && CanMoveVertically(state, ddy + IntVec2(1,0), dir, mapWidth);
			}
			break;
			default:
			break;
		}

		return CanMoveVertically(state, ddy, dir, mapWidth) && CanMoveVertically(state, ddy + IntVec2(-1, 0), dir, mapWidth);
	}

	void HorizontalMove(const std::vector<char>& state, const IntVec2& boxPos, char c, const IntVec2& dir, int32_t mapWidth, std::vector<char>& outState) const
	{
		assert(dir.y == 0);
		// ...[][]@ <
		IntVec2 ddx = boxPos + dir;
		char curr = GetData(state, ddx, mapWidth);

		if (curr != '.')
		{
			HorizontalMove(state, ddx, curr, dir, mapWidth, outState);
		}

		outState[ddx.y * mapWidth + ddx.x] = c;
		outState[boxPos.y * mapWidth + boxPos.x] = '.';
	}


	void MoveVertically(const IntVec2& pos, const IntVec2& dir, int32_t mapWidth, std::vector<char>& outSim) const
	{
		IntVec2 ddy = pos + dir;
		char c = GetData(outSim, ddy, mapWidth);
		if (c == '[' || c == ']')
		{
			MoveVertically(ddy + (c == ']' ? IntVec2(-1, 0) : IntVec2(1, 0)), dir, mapWidth, outSim);
			MoveVertically(ddy, dir, mapWidth, outSim);
		}
		outSim[ddy.y * mapWidth + ddy.x] = outSim[pos.y * mapWidth + pos.x];
		outSim[pos.y * mapWidth + pos.x] = '.';
	}

	void PrintState(const std::vector<char>& state, const IntVec2& playerPos, int32_t mapWidth) const
	{
		int32_t mapHeight = (int32_t)state.size() / mapWidth;
		std::string formattedString;
		formattedString.reserve(state.size() + m_mapHeight + 1);
		formattedString.insert(formattedString.end(), '\n');
		for (int i = 0; i < mapHeight; ++i)
		{
			formattedString.insert(formattedString.size(), state.data() + i * mapWidth, mapWidth);
			if (i == playerPos.y)
			{
				formattedString[formattedString.size() - (mapWidth - playerPos.x)] = '@';
			}
			formattedString.insert(formattedString.end(), '\n');
		}

		Log("%s", formattedString.c_str());
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::vector<char> bigMap;
		Expand(m_map, bigMap);

		std::vector<char> pingPong[2] = { bigMap, bigMap };
		int32_t ppIdx = 0;
		IntVec2 dir;

		IntVec2 currPos = m_startPos;
		currPos.x *= 2;
		IntVec2 nextPos;
		
		//PrintState(pingPong[0], currPos, m_mapWidth * 2);

		for (size_t i = 0; i < m_input.size(); ++i)
		{
			switch (m_input[i])
			{
			case '<': dir = IntVec2(-1, 0); break;
			case '>': dir = IntVec2(1, 0); break;
			case '^': dir = IntVec2(0, -1); break;
			case 'v': dir = IntVec2(0, 1); break;
			default: assert(false); break;
			}

			Step(pingPong[ppIdx], currPos, dir, m_mapWidth * 2, pingPong[ppIdx ^ 1], nextPos);
			//Log("Step %zd: Pos [%d, %d] -> [%d, %d]", i + 1, currPos.x, currPos.y, nextPos.x, nextPos.y);
			//PrintState(pingPong[ppIdx ^ 1], nextPos, m_mapWidth * 2);
			currPos = nextPos;
			ppIdx ^= 1;
		}

		uint32_t GPSSum = 0;
		const std::vector<char>& lastState = pingPong[ppIdx ^ 1];
		for (size_t i = 0; i < lastState.size(); ++i)
		{
			if (lastState[i] == '[')
			{
				GPSSum += (i / (m_mapWidth * 2)) * 100 + i % (m_mapWidth * 2);
			}
		}

		Log("GPS Sum = %u", GPSSum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	IntVec2 m_startPos;
	size_t m_mapWidth;
	size_t m_mapHeight;
	std::vector<char> m_map;
	std::vector<char> m_input;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 15;
	newParams.year = 2024;
	newParams.puzzleTitle = "Warehouse Woes";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
