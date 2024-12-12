#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/AABB.h"
class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	typedef std::vector<IntAABB2D> CubeVector;
	typedef std::vector<IntVec2> PointVector;
	typedef std::vector<IntVec2> EdgeVector;

	uint32_t GetArea(const CubeVector& cubes) const
	{
		uint32_t sum = 0;
		for (const IntAABB2D& aabb : cubes)
		{
			sum += aabb.GetArea();
		}

		return sum;
	}

	uint32_t GetPerimeter(const CubeVector& cubes) const
	{
		uint32_t sum = 0;
		for (const IntAABB2D& aabb : cubes)
		{
			sum += aabb.GetParameter();
		}

		// Remove overlap
		for (size_t i = 0; i < cubes.size(); ++i)
		{
			for (size_t j = i + 1; j < cubes.size(); ++j)
			{
				if (cubes[i].Intersects(cubes[j]))
				{
					const IntAABB2D& overlap = cubes[i].Intersection(cubes[j]);

					sum -= std::max(overlap.GetSize().x, overlap.GetSize().y) * 2;
				}
			}
		}

		return sum;
	}

	struct GardenPlot
	{
		GardenPlot(char _type):type(_type) {};
		char type;
		PointVector points;
		CubeVector cubes;
		std::vector<CubeVector> subPlots;

		void SimplifyCubes()
		{
			bool canMerge = false;
			for (size_t i = 0; i < cubes.size() - 1; )
			{
				const IntAABB2D& srcA = cubes[i];
				canMerge = false;
				for (size_t j = i + 1; j < cubes.size(); ++j)
				{
					const IntAABB2D& srcB = cubes[j];
					if (srcA.GetSize().x == srcB.GetSize().x) // Same Horiz, check for connection 
					{
						canMerge = srcA.GetMinXMaxY() == srcB.GetMin() || srcB.GetMinXMaxY() == srcA.GetMin();
					}

					if (!canMerge && srcA.GetSize().y == srcB.GetSize().y) // Same Vert, check for connection
					{
						canMerge = srcA.GetMaxXMinY() == srcB.GetMin() || srcB.GetMin() == srcA.GetMaxXMinY();
					}

					if (canMerge)
					{
						IntAABB2D mergedCube = IntAABB2D(srcA.GetMin().PerComponentMin(srcB.GetMin()), srcA.GetMax().PerComponentMax(srcB.GetMax()));
						cubes[i] = mergedCube;
						cubes.erase(cubes.begin() + j);

						break;
					}
				}

				if (canMerge)
				{
					i = 0;
				}
				else
				{
					++i;
				}
			}

			// Sort into islands.
			for (size_t i = 0; i < cubes.size(); ++i)
			{
				IntAABB2D& cubeToSort = cubes[i];
				int foundIsland = Algorithm::find_index_of(subPlots.begin(), subPlots.end(), [&](const CubeVector& island)
					{
						for (const IntAABB2D& islandCube : island)
						{
							if (islandCube.Intersects(cubeToSort))
							{
								return true;
							}
						}

						return false;
					});

				if (foundIsland == -1)
				{
					subPlots.push_back(CubeVector());
					subPlots.back().push_back(cubeToSort);
				}
				else
				{
					subPlots[foundIsland].push_back(cubeToSort);
				}
			}
		}
	};
	
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

		for (size_t y = 0; y < m_mapHeight; ++y)
		{
			for (size_t x = 0; x < m_mapWidth; ++x)
			{
				int32_t plotIdx = GetOrAdd(m_map[y * m_mapWidth + x]);
				m_plots[plotIdx].cubes.emplace_back(IntVec2((int32_t)x, (int32_t)y), IntVec2((int32_t)x + 1, (int32_t)y + 1));
				if (IsCorner((int32_t)x, (int32_t)y))
				{

					m_plots[plotIdx].points.emplace_back((int32_t)x, (int32_t)y);
				}
			}
		}

		// Simplify cubes.
		uint32_t a = 0;
		uint32_t p = 0;
		uint32_t total = 0;
		for (GardenPlot& plot : m_plots)
		{
			plot.SimplifyCubes();
		}

		// Plots.
		Log("Parsed %zd plots, Total %u", m_plots.size(), total);
	}

	bool IsCorner(int x, int y) const
	{
		static const IntVec2 offsets[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0}};
		int numDiffNeighbors = 0;
		IntVec2 candidate;
		char value = m_map[y * m_mapWidth + x];
		for (const IntVec2& offset : offsets)
		{
			candidate = IntVec2(x, y) + offset;
			if (candidate.x < 0 || candidate.y < 0 || candidate.x >= m_mapWidth || candidate.y >= m_mapHeight)
			{
				++numDiffNeighbors;
			}
			else if (m_map[candidate.y * m_mapWidth + candidate.x] != value)
			{
				++numDiffNeighbors;
			}
		}

		return numDiffNeighbors > 1;
	}

	int32_t GetOrAdd(char c)
	{
		int foundIndex = Algorithm::find_index_of(m_plots.begin(), m_plots.end(), [&](const GardenPlot& LHS) { return LHS.type == c; });
		if (foundIndex < 0)
		{
			m_plots.emplace_back(c);
			return (int32_t)m_plots.size() - 1;
		}

		return foundIndex;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t total = 0;
		uint64_t plotTotal = 0;
		uint32_t plotArea = 0;
		uint32_t plotPerimeter = 0;
		for (const GardenPlot& plot : m_plots)
		{
			plotTotal = 0;
			for (const CubeVector& subPlot : plot.subPlots)
			{
				plotArea = GetArea(subPlot);
				plotPerimeter = GetPerimeter(subPlot);

				Log("Subplot Area %u, Perimeter %u", plotArea, plotPerimeter);
				plotTotal += plotArea * plotPerimeter;
			}

			Log("Plot total for Plot %c - %llu", plot.type, plotTotal);
			total += plotTotal;
		}

		Log("Total Cost %llu", total);
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
	std::vector<GardenPlot> m_plots;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 12;
	newParams.year = 2024;
	newParams.puzzleTitle = "Garden Groups";
	newParams.inputFilename = "sample3.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
