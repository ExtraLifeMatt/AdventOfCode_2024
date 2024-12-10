#include "AdventGUI/AdventGUI.h"
#include "ACUtils/BDFS.h"
#include "ACUtils/IntVec.h"
#include <unordered_set>
#include "imgui.h"

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params), m_mapWidth(0), m_mapHeight(0), m_mapData() {};

private:
	struct Pathway 
	{
		Pathway(const std::vector<IntVec2>& _path)
		: path(_path)
		{
			hash = 0;
			for (const IntVec2& pt : path)
			{
				hash = Hash::HashCombineSize(hash, pt.ToHash());
			}
		}
		std::vector<IntVec2> path;
		size_t hash;
	};

	class PathwayNode : public BDFS::BDFSNode<Pathway>
	{
	public:
		PathwayNode(const Pathway& pathway) : BDFSNode(pathway){}
		virtual size_t GetHash() const { return GetState().hash; }
	};

	class PathwayExec : public BDFS::BDFSExecuter<Pathway>
	{
	public:
		PathwayExec(size_t mapWidth, const std::vector<int8_t>& mapData):BDFSExecuter(BDFSExecuter::BDFSExecuterMode::BDFSExecuterMode_DepthFirst)
		{
			m_mapWidth = mapWidth;
			m_mapData = mapData;
			m_mapHeight = m_mapData.size() / m_mapWidth;
		}

		int8_t GetMapValue(const IntVec2& pos) const
		{
			return m_mapData[pos.y * m_mapWidth + pos.x];
		}

		virtual bool ProcessNode(const BDFS::BDFSNode<Pathway>* node) override
		{
			const Pathway& currState = node->GetState();

			if (GetMapValue(currState.path.back()) == 9)
			{
				m_solutions.push_back(currState);

				const IntVec2& eightPos = currState.path[8];
				const IntVec2 dirstwo[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };


				IntVec4 trailhead(currState.path.front(), currState.path.back());

				auto itFind = m_trailHeads.find(trailhead);
				if (itFind == m_trailHeads.end())
				{
					AdventGUIConsole::Get().Log("Added Trailhead [%d, %d] -> [%d, %d]", trailhead.x, trailhead.y, trailhead.z, trailhead.w);
					m_trailHeads.insert(trailhead);
				}
				else
				{
					assert(*itFind == trailhead);
				}

				return false;
			}

			const IntVec2 dirs[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0}};
			std::vector<IntVec2> newPath;
			int8_t currValue = 0;
			int8_t nextValue = 0;

			bool addedNodes = false;
			for (const IntVec2& off : dirs)
			{
				IntVec2 candidatePos = currState.path.back() + off;
				if (candidatePos.x < 0 || candidatePos.y < 0 || candidatePos.x >= m_mapWidth || candidatePos.y >= m_mapHeight)
				{
					continue;
				}

				currValue = GetMapValue(currState.path.back());
				nextValue = GetMapValue(candidatePos);
				int8_t delta = nextValue - currValue;

				if (delta == 1)
				{
					assert(currValue + 1 == nextValue);
					newPath.clear();
					newPath = currState.path;
					newPath.push_back(candidatePos);

					Pathway newState(newPath);
					PathwayNode tmp(newState);

					if (!IsDiscovered(&tmp))
					{
						QueueNode(new PathwayNode(newState));
						addedNodes = true;
					}
				}
			}

			return addedNodes;
		}

		const std::vector<Pathway>& GetSolutions() const { return m_solutions; }
		const size_t GetTrailheads() const { return m_trailHeads.size(); }
	private:
		size_t m_mapWidth;
		size_t m_mapHeight;
		std::vector<int8_t> m_mapData;
		std::vector<Pathway> m_solutions;
		std::unordered_set<IntVec4> m_trailHeads;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		m_mapData.reserve(1024);
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_mapWidth == 0)
			{
				m_mapWidth = line.size();
			}
			assert(m_mapWidth == line.size());
			
			for(size_t i = 0; i < line.size(); ++i)
			{
				m_mapData.push_back(line[i] - '0');

				if (m_mapData.back() == 0)
				{
					m_startLocs.emplace_back((int32_t)i, (int32_t)(m_mapData.size() / m_mapWidth));
				}
			}
		}

		m_mapHeight = m_mapData.size() / m_mapWidth;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		PathwayExec exec(m_mapWidth, m_mapData);

		for (const IntVec2& startPos : m_startLocs)
		{
			std::vector<IntVec2> startPath;
			startPath.push_back(startPos);
			exec.QueueNode(new PathwayNode(Pathway(startPath)));
		}
		
		exec.Solve();

		Log("Solutions = %zd", exec.GetTrailheads());

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	size_t m_mapWidth;
	size_t m_mapHeight;
	std::vector<int8_t> m_mapData;
	std::vector<IntVec2> m_startLocs;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 10;
	newParams.year = 2024;
	newParams.puzzleTitle = "Hoof It";
	newParams.inputFilename = "input2.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
