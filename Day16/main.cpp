#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/AStar.h"

using namespace AStar;

enum class Direction : size_t
{
	North = 0,
	East,
	South,
	West
};

class PathState : public AStarNodeBase<uint32_t>
{
public:
	PathState(const IntVec2& pos, const Direction dir, const IntVec2& goal) :AStarNodeBase<uint32_t>(0, 0), m_hash(0), m_pos(pos), m_goalPos(goal), m_dir(dir)
	{
		m_hash = pos.x | pos.y << 8 | (static_cast<size_t>(dir) << 16);
	}
	virtual size_t GetHash() const override { return m_hash; }
	virtual bool IsAtGoal() const override { return false; /* m_pos == m_goalPos; */} // For part two.
	const IntVec2& GetPos() const { return m_pos; }
	const Direction GetDirection() const { return m_dir; }
public:
	size_t m_hash;
	IntVec2 m_pos;
	IntVec2 m_goalPos;
	Direction m_dir;
};

class GridExecutor : public AStarExecuter<uint32_t>
{
public:
	GridExecutor(const std::vector<char>& mapData, size_t mapWidth, const IntVec2& goal)
	: m_mapData(mapData),
	m_mapWidth(mapWidth),
	m_mapHeight(mapData.size() / mapWidth),
	m_goalPos(goal),
	m_lowestGoalScore(~0ULL)
	{
	}
	
	virtual void OnProcessNode(AStarNodeBase<uint32_t>& CurrentNode) override
	{
		PathState* pathNode = CurrentNode.As<PathState>();
		assert(pathNode);

		static Direction dirs[] = { Direction::North, Direction::East, Direction::South, Direction::West };
		static IntVec2 dirVec[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

		const IntVec2& currentPos = pathNode->GetPos();
		const Direction currentDir = pathNode->GetDirection();
		const uint32_t currentCost = pathNode->GetCost();

		if (pathNode->GetPos() == m_goalPos)
		{
			if (pathNode->GetTotalCost() < m_lowestGoalScore)
			{
				m_lowestGoalScore = pathNode->GetTotalCost();
				m_validPositions.clear();
			}
			
			if (pathNode->GetTotalCost() == m_lowestGoalScore)
			{
				const AStarNodeBase<uint32_t>* currState = pathNode;
				while (currState)
				{
					m_validPositions.insert(currState->As<PathState>()->GetPos());
					currState = currState->GetParent();
				}
			}

			return;
		}

		// 3 options, forward, left, right
		Direction cw = static_cast<Direction>((static_cast<size_t>(currentDir) + 1) % 4);
		Direction ccw = static_cast<Direction>((static_cast<size_t>(currentDir) + 3) % 4);

		PathState* cwState = new PathState(currentPos, cw, m_goalPos);
		cwState->SetCost(pathNode->GetCost() + 1000);
		cwState->SetHeuristic(pathNode->GetHeuristic());
		cwState->SetParent(pathNode);

		PathState* ccwState = new PathState(currentPos, ccw, m_goalPos);
		ccwState->SetCost(pathNode->GetCost() + 1000);
		ccwState->SetHeuristic(pathNode->GetHeuristic());
		ccwState->SetParent(pathNode);

		PathState * forwardState = new PathState(currentPos + dirVec[static_cast<size_t>(currentDir)], currentDir, m_goalPos);
		forwardState->SetCost(pathNode->GetCost() + 1);
		forwardState->SetHeuristic(GetHeuristic(forwardState->GetPos()));
		forwardState->SetParent(pathNode);

		PathState* allNodes[] = { cwState, ccwState, forwardState };

		IntVec2 delta;
		for (PathState* node : allNodes)
		{
			if (node->GetDirection() != pathNode->GetDirection())
			{
				delta = currentPos + dirVec[static_cast<size_t>(node->GetDirection())];
			}
			else
			{
				delta = node->GetPos();
			}

			if (!IsInMap(delta) || IsBlocked(delta))
			{
				continue;
			}

			// Check open list.
			const AStarNodeBase<uint32_t>* closedNode = GetClosedListNode(node);
			if (closedNode && closedNode->GetTotalCost() < node->GetTotalCost())
			{
				assert(closedNode->As<PathState>()->GetPos() == node->GetPos());
				// Skip it.
				delete node;
				continue;
			}

			// See if we have a better cost than an existing entry in the open list.
			int32_t existingIndex = FindIndexInOpenList(node);
			if (existingIndex != -1)
			{
				AStarNodeBase<uint32_t>* existingNode = m_openList[existingIndex];
				if (node->GetTotalCost() > existingNode->GetTotalCost())
				{
					delete node;
					continue;
				}
			}

			InsertNode(node);
		}
	}

	bool IsInMap(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
		{
			return false;
		}

		return true;
	}

	bool IsBlocked(const IntVec2& pos) const
	{
		return m_mapData[pos.y * m_mapWidth + pos.x] == '#';
	}

	uint32_t GetTurnCost(Direction from, Direction to) const
	{
		if (from == to)
		{
			 return 0;
		}

		size_t dirDelta = to > from ? (static_cast<size_t>(to) - static_cast<size_t>(from)) : (static_cast<size_t>(from) - static_cast<size_t>(to));

		if (dirDelta == 2)
		{
			return 2000;
		}

		return 1000;
	}

	uint32_t GetHeuristic(const IntVec2& pos) const
	{
		// Manhattan Distance should be fine.
		return abs(pos.x - m_goalPos.x) + abs(pos.y - m_goalPos.y);
	}

	size_t GetLowestScore() const { return m_lowestGoalScore; }
	const std::unordered_set<IntVec2>& GetLowestScorePaths() const { return m_validPositions; }

	const std::vector<char> m_mapData;
	const size_t m_mapWidth;
	const size_t m_mapHeight;
	const IntVec2 m_goalPos;
	size_t m_lowestGoalScore;
	std::unordered_set<IntVec2> m_validPositions;
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
		std::vector<std::string> tokens;
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

	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		GridExecutor executer(m_map, m_mapWidth, m_goalPos);
		PathState* initialNode = new PathState(m_startPos, Direction::East, m_goalPos);
		initialNode->SetHeuristic(executer.GetHeuristic(m_startPos));
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

			Log("Least score = %u", solvedPath[0]->GetCost());
		}
		else
		{
			// Part Two
			Log("Lowest Score = %zd", executer.GetLowestScore());
			Log("Seat Positions = %zd", executer.GetLowestScorePaths().size());
		}

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	IntVec2 m_startPos;
	IntVec2 m_goalPos;
	size_t m_mapWidth;
	size_t m_mapHeight;
	std::vector<char> m_map;

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 16;
	newParams.year = 2024;
	newParams.puzzleTitle = "Reindeer Maze";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
