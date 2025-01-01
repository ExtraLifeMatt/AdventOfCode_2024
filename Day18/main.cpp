#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/AStar.h"

using namespace AStar;

class PathState : public AStarNodeBase<uint32_t>
{
public:
	PathState(const IntVec2& pos, const IntVec2& goal):m_pos(pos), m_goal(goal)	
	{
		m_hash = m_pos.x | m_pos.y << 16;
	}
	virtual size_t GetHash() const override { return m_hash; }
	virtual bool IsAtGoal() const override { return m_pos == m_goal; } 
	const IntVec2& GetPos() const { return m_pos; }
private:
	IntVec2 m_pos;
	IntVec2 m_goal;
	size_t m_hash;
};

class GridExecutor : public AStarExecuter<uint32_t>
{
public:
	GridExecutor(const std::vector<char>& mapData, size_t mapWidth, const IntVec2& goal)
		: m_mapData(mapData),
		m_mapWidth(mapWidth),
		m_mapHeight(mapData.size() / mapWidth),
		m_goalPos((int32_t)m_mapWidth - 1, (int32_t)m_mapHeight - 1)
	{
	}

	virtual void OnProcessNode(AStarNodeBase<uint32_t>& CurrentNode) override
	{
		PathState* pathNode = CurrentNode.As<PathState>();
		assert(pathNode);

		static const IntVec2 offsets[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
		IntVec2 delta;
		for (const IntVec2& off : offsets)
		{
			delta = pathNode->GetPos() + off;

			if (IsBlocked(delta))
			{
				continue;
			}

			if (pathNode->GetParent() && pathNode->GetParent()->As<PathState>()->GetPos() == delta)
			{
				continue;
			}

			PathState* newState = new PathState(delta, m_goalPos);
			newState->SetCost(pathNode->GetCost() + 1);
			newState->SetHeuristic(GetHeuristic(delta));
			newState->SetParent(pathNode);
			
			// Check open list.
			const AStarNodeBase<uint32_t>* closedNode = GetClosedListNode(newState);
			if (closedNode && closedNode->GetTotalCost() <= newState->GetTotalCost())
			{
				assert(closedNode->As<PathState>()->GetPos() == newState->GetPos());
				// Skip it.
				delete newState;
				continue;
			}

			// See if we have a better cost than an existing entry in the open list.
			int32_t existingIndex = FindIndexInOpenList(newState);
			if (existingIndex != -1)
			{
				AStarNodeBase<uint32_t>* existingNode = m_openList[existingIndex];
				if (newState->GetTotalCost() >= existingNode->GetTotalCost())
				{
					delete newState;
					continue;
				}
			}

			InsertNode(newState);
		}
	}

	bool IsBlocked(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
		{
			return true;
		}

		return m_mapData[pos.y * m_mapWidth + pos.x] == '#';
	}

	uint32_t GetHeuristic(const IntVec2& pos) const
	{
		// Manhattan Distance should be fine.
		return abs(m_goalPos.x - pos.x) + abs(m_goalPos.y - pos.y);
	}

	const std::vector<char> m_mapData;
	const size_t m_mapWidth;
	const size_t m_mapHeight;
	IntVec2 m_goalPos;
};

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		size_t commaIdx = std::string::npos;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			commaIdx = line.find(',');
			assert(commaIdx != std::string::npos);
			m_fallingBytes.emplace_back(atoi(line.substr(0, commaIdx).c_str()), atoi(line.substr(commaIdx + 1).c_str()));
		}

	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		const size_t mapWidth = 71;
		const size_t mapHeight = 71;
		const size_t totalBytesToAdd = 1024;

		std::vector<char> settledMap;
		settledMap.insert(settledMap.end(), mapWidth * mapHeight, '.');

		for (size_t i = 0; i < totalBytesToAdd; ++i)
		{
			const IntVec2& byte = m_fallingBytes[i];
			settledMap[byte.y * mapWidth + byte.x] = '#';
		}


		GridExecutor executer(settledMap, mapWidth, IntVec2(mapWidth - 1, mapHeight - 1));
		PathState* initialNode = new PathState(IntVec2(0,0), IntVec2(mapWidth - 1, mapHeight - 1));
		initialNode->SetHeuristic(executer.GetHeuristic(initialNode->GetPos()));
		initialNode->SetCost(0);

		executer.InsertNode(initialNode);

		std::vector<const PathState*> solvedPath;
		const AStarNodeBase<uint32_t>* pathNode = nullptr;
		if (executer.Solve(pathNode))
		{
			// Chain up for the path route
			while (pathNode)
			{
				solvedPath.push_back(pathNode->As<PathState>());
				pathNode = pathNode->GetParent();
			}

			Log("Total Steps = %zd", solvedPath.size() - 1);
		}
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		const size_t mapWidth = 71;
		const size_t mapHeight = 71;
		const size_t totalBytesToAdd = 1024;

		std::vector<char> settledMap;
		settledMap.insert(settledMap.end(), mapWidth * mapHeight, '.');

		for (size_t i = 0; i < totalBytesToAdd; ++i)
		{
			const IntVec2& byte = m_fallingBytes[i];
			settledMap[byte.y * mapWidth + byte.x] = '#';
		}

		for (size_t i = totalBytesToAdd; i < m_fallingBytes.size(); ++i)
		{
			const IntVec2& byte = m_fallingBytes[i];
			settledMap[byte.y * mapWidth + byte.x] = '#';

			GridExecutor executer(settledMap, mapWidth, IntVec2(mapWidth - 1, mapHeight - 1));
			PathState* initialNode = new PathState(IntVec2(0, 0), IntVec2(mapWidth - 1, mapHeight - 1));
			initialNode->SetHeuristic(executer.GetHeuristic(initialNode->GetPos()));
			initialNode->SetCost(0);

			executer.InsertNode(initialNode);
			const AStarNodeBase<uint32_t>* pathNode = nullptr;
			if (!executer.Solve(pathNode))
			{
				Log("Byte: %d,%d broke our path.", byte.x, byte.y);
				break;
			}
		}

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<IntVec2> m_fallingBytes;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 18;
	newParams.year = 2024;
	newParams.puzzleTitle = "RAM Run";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
