#include "AdventGUI/AdventGUI.h"
#include <unordered_set>
#include "ACUtils/BDFS.h"
#include "ACUtils/Math.h"
#include "ACUtils/IntVec.h"

using namespace BDFS;

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	static constexpr uint16_t INVALID_COORD = 0xFFFF;
	static constexpr uint16_t MakeCoord(int x, int y)
	{
		assert(x <= 0xFF && y <= 0xFF);
		return ((y & 0xFF) << 8) | (x & 0xFF);
	}

	struct XMASState
	{
		XMASState():m_path{ INVALID_COORD, INVALID_COORD, INVALID_COORD, INVALID_COORD } {};
		XMASState(const XMASState& RHS){ memcpy(m_path, RHS.m_path, sizeof(uint16_t) * ARRAY_SIZE(m_path)); }
		XMASState(uint64_t state)
		{
			memcpy(m_path, &state, sizeof(uint64_t));
		}

		uint16_t m_path[4];
		uint64_t GetHash() const { return *reinterpret_cast<const uint64_t*>(m_path); }
	};

	class XMASNode : public BDFSNode<XMASState>
	{
	public:
		XMASNode(const XMASState& state): BDFSNode(state){};
		virtual size_t GetHash() const { return GetState().GetHash(); }
	};

	class XMASBDFS : public BDFSExecuter<XMASState>
	{
	public:
		XMASBDFS(uint32_t tableWidth,const std::string& tableData)
			: BDFSExecuter(BDFSExecuterMode::BDFSExecuterMode_DepthFirst),
			m_tableWidth(tableWidth),
			m_tableData(tableData)
		{
			m_tableHeight = (uint32_t)m_tableData.size() / m_tableWidth;
		}

		virtual bool ProcessNode(const BDFSNode<XMASState>* node) override
		{
			assert(node);
			const XMASState& currState = node->GetState();
			uint64_t xmasState = currState.GetHash();

			// Which letter are we on? X is high bit, S is low.
			static const char XMASChars[] = "XMAS";
			int nextCharIndex = -1;
			uint16_t currPos = INVALID_COORD;
			uint16_t lastPosition = INVALID_COORD;
			for (int i = 0; i < ARRAY_SIZE(XMASChars) - 1; ++i)
			{
				currPos = (xmasState >> (48 - (i * 16))) & INVALID_COORD;
				if (currPos == INVALID_COORD)
				{
					nextCharIndex = i;
					break;
				}
				else
				{
					lastPosition = currPos;
				}
			}

			if (nextCharIndex == -1)
			{
				return false;
			}

			int x = lastPosition & 0xFF;
			int y = lastPosition >> 8;
			int candidateX = 0;
			int candidateY = 0;
			
			bool addedNodes = false;

			for (int i = -1; i < 2; ++i)
			{
				candidateY = y + i;
				if (candidateY < 0 || candidateY >= (int)m_tableHeight)
				{
					continue;
				}

				for (int j = -1; j < 2; ++j)
				{
					candidateX = x + j;
					if (candidateY == y && candidateX == x)
					{
						continue;
					}

					if (candidateX < 0 || candidateX >= (int)m_tableWidth)
					{
						continue;
					}

					if (m_tableData[candidateY * m_tableWidth + candidateX] == XMASChars[nextCharIndex])
					{
						XMASState newState(xmasState);
						newState.m_path[3 - nextCharIndex] = MakeCoord(candidateX, candidateY);
						if (nextCharIndex == 3)
						{
							m_validPermutations.insert(newState.GetHash());
						}
						else
						{
							XMASNode newNode(newState);
							if (!IsDiscovered(&newNode)) // no need to check this state again.
							{
								QueueNode(new XMASNode(newState)); // Possible valid choice.
								addedNodes = true;
							}
						}

					}
				}
			}

			return addedNodes;
		}

		uint32_t m_tableWidth;
		uint32_t m_tableHeight;
		std::string m_tableData;
		std::unordered_set<uint64_t> m_validPermutations;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		m_lineWidth = 0;
		m_charTable.clear();
		std::string line;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_lineWidth == 0)
			{
				m_lineWidth = (int)line.size();
			}
			else
			{
				assert(m_lineWidth == line.size());
			}
			m_charTable.append(line);
		}
		m_tableHeight = (int)m_charTable.size() / m_lineWidth;
	}

	bool CanMakeXMAS(int x, int y, IntVec2 dir)
	{
		static const char XMASChars[] = "XMAS";
		int dx = x;
		int dy = y;
		for (int i = 1; i < ARRAY_SIZE(XMASChars) - 1; ++i)
		{
			dx += dir.x;
			dy += dir.y;

			if (dx < 0 || dy < 0 || dx >= m_lineWidth || dy >= m_tableHeight)
			{
				return false;
			}

			if (m_charTable[dy * m_lineWidth + dx] != XMASChars[i])
			{
				return false;
			}
		}

		return true;
	}

	bool CanMakeCrossMAS(int x, int y, IntVec2 dir)
	{
		static const char XMASChars[] = "MS";
		int dx = x + dir.x;
		int dy = y + dir.y;

		if (dx < 0 || dy < 0 || dx >= m_lineWidth || dy >= m_tableHeight)
		{
			return false;
		}

		if (m_charTable[dy * m_lineWidth + dx] == 'M')
		{
			int ddx = x - dir.x;
			int ddy = y - dir.y;

			if (ddx < 0 || ddy < 0 || ddx >= m_lineWidth || ddy >= m_tableHeight)
			{
				return false;
			}

			return m_charTable[ddy * m_lineWidth + ddx] == 'S';
		}

		return false;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One (Dante Must Die Mode apparently)
		//XMASBDFS bdfs((uint32_t)m_lineWidth, m_charTable);
		//for (int i = 0; i < m_tableHeight; ++i)
		//{
		//	for (int j = 0; j < m_lineWidth; ++j)
		//	{
		//		if (m_charTable[i * m_lineWidth + j] == 'X')
		//		{
		//			XMASState startState;
		//			startState.m_path[3] = MakeCoord(j, i);
		//			bdfs.QueueNode(new XMASNode(startState));
		//		}
		//	}
		//}

		//bdfs.Solve();

		//Log("Total permutations = %zd", bdfs.m_validPermutations.size());

		const IntVec2 directions[] = { {-1,-1}, {0, -1}, {1, -1},
										{-1, 0}, {0, 0}, {1, 0},
										{-1, 1}, {0, 1}, {1, 1} };
														
		uint32_t matches = 0;
		for (int i = 0; i < m_tableHeight; ++i)
		{
			for (int j = 0; j < m_lineWidth; ++j)
			{
				if (m_charTable[i * m_lineWidth + j] == 'X')
				{
					for (const IntVec2& dir : directions)
					{
						if (CanMakeXMAS(j, i, dir))
						{
							++matches;
						}
					}
				}
			}
		}

		Log("Total matches: %u", matches);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		const IntVec2 directions[] = { {-1,-1}, {0, -1}, {1, -1},
										{-1, 0}, {0, 0}, {1, 0},
										{-1, 1}, {0, 1}, {1, 1} };

		uint32_t matches = 0;
		for (int i = 0; i < m_tableHeight; ++i)
		{
			for (int j = 0; j < m_lineWidth; ++j)
			{
				if (m_charTable[i * m_lineWidth + j] == 'A')
				{
					for (const IntVec2& dir : directions)
					{
						if (CanMakeCrossMAS(j, i, dir))
						{
							IntVec2 xFlip = dir;
							xFlip.x *= -1;
							if (xFlip != dir && CanMakeCrossMAS(j, i, xFlip))
							{
								++matches;
								break;
							}
							IntVec2 yFlip = dir;
							yFlip.y *= -1;

							if (yFlip != dir && CanMakeCrossMAS(j, i, yFlip))
							{
								++matches;
								break;
							}
						}
					}
				}
			}
		}

		Log("Total matches: %U", matches);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	int m_lineWidth;
	int m_tableHeight;
	std::string m_charTable;

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 4;
	newParams.year = 2024;
	newParams.puzzleTitle = "Ceres Search";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
