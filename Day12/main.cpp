#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/Enum.h"
#include "ACUtils/IntVec.h"
#include <queue>

enum class Edges : uint8_t
{
	None = 0,
	Top = 1 << 0,
	Left = 1 << 1,
	Bottom = 1 << 2,
	Right = 1 << 3,
	TopLeft = Top | Left,
	TopRight = Top | Right,
	BottomLeft = Bottom | Left,
	BottomRight = Bottom | Right,
};
DECLARE_ENUM_BITFIELD_OPERATORS(Edges);

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		m_mapWidth = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_mapWidth == 0)
			{
				m_mapWidth = line.size();
			}
			assert(m_mapWidth == line.size());
			m_map.append(line);
		}

		m_mapHeight = m_map.size() / m_mapWidth;
	}

	char GetData(int x, int y) const
	{
		if (x < 0 || x >= m_mapWidth || y < 0 || y >= m_mapHeight)
		{
			return '\0';
		}

		return m_map[y * m_mapWidth + x];
	}

	char GetData(const IntVec2& xy) const
	{
		return GetData(xy.x, xy.y);
	}

#define LOOKUP(x, y) (y * m_mapWidth + x)

	void price(IntVec2 pos, uint32_t& outArea, uint32_t& outPerimeter, uint32_t& outNumSides, std::vector<bool>& outSeen, std::vector<Edges>& edges)
	{
		std::queue<IntVec2> scanQueue;
		scanQueue.emplace(pos);
		IntVec2 curr;
		char c;
		outPerimeter = 0;
		outArea = 0;
		outNumSides = 0;

		int i, j = 0;

		do {
			curr = scanQueue.front();
			scanQueue.pop();
			i = curr.x;
			j = curr.y;

			if (outSeen[LOOKUP(i, j)])
			{
				continue;
			}

			outSeen[LOOKUP(i, j)] = true;
			const char plant = GetData(pos);

			++outArea; 
			if (GetData(i - 1,j) == plant) { edges[LOOKUP(i,j)] |= Edges::Top; scanQueue.emplace(i - 1, j); }
			else ++outPerimeter;
			if (GetData(i + 1, j) == plant) { edges[LOOKUP(i, j)] |= Edges::Bottom; scanQueue.emplace(i + 1, j); }
			else ++outPerimeter;
			if (GetData(i, j - 1) == plant) { edges[LOOKUP(i, j)] |= Edges::Left; scanQueue.emplace(i, j - 1); }
			else ++outPerimeter;
			if (GetData(i, j + 1) == plant) { edges[LOOKUP(i, j)] |= Edges::Right; scanQueue.emplace(i, j + 1); }
			else ++outPerimeter;
			// Part 2
			switch (edges[LOOKUP(i,j)] & Edges::TopLeft) {
			case Edges::None: ++outNumSides; break;
			case Edges::TopLeft: outNumSides += GetData(i - 1, j - 1) != plant; break;
			}
			switch (edges[LOOKUP(i, j)] & Edges::BottomLeft) {
			case Edges::None: ++outNumSides; break;
			case Edges::BottomLeft: outNumSides += GetData(i + 1,j - 1) != plant; break;
			}
			switch (edges[LOOKUP(i, j)] & Edges::TopRight) {
			case Edges::None: ++outNumSides; break;
			case Edges::TopRight: outNumSides += GetData(i - 1, j + 1) != plant; break;
			}
			switch (edges[LOOKUP(i, j)] & Edges::BottomRight) {
			case Edges::None: ++outNumSides; break;
			case Edges::BottomRight: outNumSides += GetData(i + 1,j + 1) != plant; break;
			}
		} while (!scanQueue.empty());
	}

	void Fill(IntVec2 pos, uint32_t& outArea, uint32_t& outPerimeter, uint32_t& outNumSides, std::vector<bool>& outSeen, std::vector<Edges>& edges) const
	{
		std::queue<IntVec2> scanQueue;
		scanQueue.emplace(pos);
		IntVec2 curr;
		char c;
		outPerimeter = 0;
		outArea = 0;
		outNumSides = 0;

		const IntVec2 offsets[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
		const Edges offEdge[] = { Edges::Top, Edges::Right, Edges::Bottom, Edges::Left };
		const IntVec2 diagOffsets[] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
		const Edges diagEdge[]= { Edges::TopLeft, Edges::TopRight, Edges::BottomLeft, Edges::BottomRight };
		IntVec2 next;
		while (!scanQueue.empty())
		{
			curr = scanQueue.front();
			scanQueue.pop();

			size_t i = curr.y * m_mapWidth + curr.x;
			c = m_map[i];

			if (outSeen[i])
			{
				continue;
			}

			outSeen[i] = true;
			++outArea;
			for(size_t j = 0; j < 4; ++j)
			{
				const IntVec2& off = offsets[j];
				next = curr + off;
				if (GetData(next) == c)
				{
					scanQueue.push(curr + off);
				}
				else
				{
					edges[i] |= offEdge[j];
					++outPerimeter;
				}
			}


			if ((edges[i] & Edges::TopLeft) == Edges::TopLeft)
			{
				++outNumSides;
			}

			if ((edges[i] & Edges::TopRight) == Edges::TopRight)
			{
				++outNumSides;
			}

			if ((edges[i] & Edges::BottomLeft) == Edges::BottomLeft)
			{
				++outNumSides;
			}

			if ((edges[i] & Edges::BottomRight) == Edges::BottomRight)
			{
				++outNumSides;
			}
					
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<bool> seenLocs;
		seenLocs.reserve(m_map.size());
		seenLocs.insert(seenLocs.begin(), m_map.size(), false);
		
		std::vector<Edges> edgeInfo;
		edgeInfo.reserve(m_map.size());
		edgeInfo.insert(edgeInfo.begin(), m_map.size(), Edges::None);


		uint64_t sum = 0;
		uint64_t sumTwo = 0;
		uint32_t area = 0;
		uint32_t perimeter = 0;
		uint32_t sides = 0;

		for (size_t i = 0; i < m_map.size(); ++i)
		{
			if (!seenLocs[i])
			{
				price(IntVec2((int)(i % m_mapWidth), (int)(i / m_mapWidth)), area, perimeter, sides, seenLocs, edgeInfo);
				assert(seenLocs[i]);

				Log("Found Plot [%c] Area [%u] Sides [%u] = %u", m_map[i], area, perimeter, area * perimeter);
				sum += area * perimeter;
				sumTwo += area * sides;
			}
		}

		Log("Total = %llu", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::string m_map;
	size_t m_mapWidth;
	size_t m_mapHeight;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 12;
	newParams.year = 2024;
	newParams.puzzleTitle = "Garden Groups";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
