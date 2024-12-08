#include "AdventGUI/AdventGUI.h"
#include <ACUtils/Algorithm.h>
#include <ACUtils/IntVec.h>
#include <unordered_set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	struct Antenna
	{
		std::vector<IntVec2> Locations;
		char Frequency;
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
				m_mapWidth = (int)line.size();
			}
			assert(m_mapWidth == (int)line.size());

			for(size_t i = 0; i < line.size(); ++i)
			{
				if (line[i] != '.')
				{
					int existingAntenna = Algorithm::find_index_of(m_antennas.begin(), m_antennas.end(), [&](const Antenna& LHS) { return LHS.Frequency == line[i]; });
					if (existingAntenna < 0)
					{
						m_antennas.emplace_back();
						existingAntenna = (int)m_antennas.size() - 1;
						m_antennas[existingAntenna].Frequency = line[i];
					}
					assert(existingAntenna >= 0);
					m_antennas[existingAntenna].Locations.emplace_back((int)i, (int)m_mapData.size() / m_mapWidth);
				}
				m_mapData.push_back(line[i]);
			}
		}

		m_mapHeight = (int)m_mapData.size() / m_mapWidth;
	}

	bool IsInBounds(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
		{
			return false;
		}

		return true;
	}
	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::unordered_set<IntVec2> antiNodes;

		IntVec2 delta;
		IntVec2 candidateA;
		IntVec2 candidateB;
		for (const Antenna& antenna : m_antennas)
		{
			for (size_t i = 0; i < antenna.Locations.size(); ++i)
			{
				IntVec2 A = antenna.Locations[i];
				for (size_t j = i + 1; j < antenna.Locations.size(); ++j)
				{
					IntVec2 B = antenna.Locations[j];
					delta = B - A;
					if (A + delta == B)
					{
						candidateA = A - delta;
						assert(B - delta == A);
						candidateB = B + delta;
						// A'
						if (IsInBounds(candidateA))
						{
							antiNodes.insert(candidateA);
						}
						// B'
						if (IsInBounds(candidateB))
						{
							antiNodes.insert(candidateB);
						}
					}		
				}
			}
		}

		Log("Total Antinodes = %zd", antiNodes.size());


		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::unordered_set<IntVec2> antiNodes;

		IntVec2 delta;
		IntVec2 candidateA;
		IntVec2 candidateB;
		for (const Antenna& antenna : m_antennas)
		{
			for (size_t i = 0; i < antenna.Locations.size(); ++i)
			{
				IntVec2 A = antenna.Locations[i];
				for (size_t j = i + 1; j < antenna.Locations.size(); ++j)
				{
					IntVec2 B = antenna.Locations[j];
					delta = B - A;
					if (A + delta == B)
					{
						antiNodes.insert(A);
						antiNodes.insert(B);

						candidateA = A - delta;
						assert(B - delta == A);
						candidateB = B + delta;
						// A'
						while (IsInBounds(candidateA))
						{
							antiNodes.insert(candidateA);
							candidateA -= delta;
						}
						// B'
						while (IsInBounds(candidateB))
						{
							antiNodes.insert(candidateB);
							candidateB += delta;
						}
					}
				}
			}
		}

		Log("Total Antinodes = %zd", antiNodes.size());
		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	int m_mapWidth;
	int m_mapHeight;
	std::string m_mapData;
	std::vector<Antenna> m_antennas;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 8;
	newParams.year = 2024;
	newParams.puzzleTitle = "Resonant Collinearity";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
