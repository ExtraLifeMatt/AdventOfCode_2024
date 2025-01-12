#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"

#include <set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	typedef std::pair<std::string, std::string> EdgePair;
	typedef std::unordered_map<std::string, std::set<std::string>> AdjacencyGraph;
	std::vector<EdgePair> m_allEdges;
	AdjacencyGraph m_adjGraph;

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		AdjacencyGraph::iterator itFind = m_adjGraph.end();
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			assert(line.size() == 5);
			m_allEdges.emplace_back(std::make_pair(line.substr(0, 2), line.substr(3, 2)));
		}

		// Build Adjacency
		std::for_each(m_allEdges.begin(), m_allEdges.end(), 
		[&](const EdgePair& pair)
		{ 
			m_adjGraph[pair.first].insert(pair.second); 
			m_adjGraph[pair.second].insert(pair.first); 
		});

		// Parse the data.
		std::set<std::string> current;
		std::set<std::string> candidates;
		std::set<std::string> processed;

		std::for_each(m_adjGraph.begin(), m_adjGraph.end(), [&](const auto& kvp){ candidates.insert(kvp.first); });

		BronKerbosch(current, candidates, processed);

		std::sort(m_allCliques.begin(), m_allCliques.end(), [](const std::vector<std::string>& LHS, const std::vector<std::string>& RHS)
			{
				return LHS.size() < RHS.size();
			});
	}

	void BronKerbosch(std::set<std::string> R, std::set<std::string> P, std::set<std::string> X)
	{
		if (P.empty() && X.empty())
		{
			std::vector<std::string> clique(R.begin(), R.end());
			m_allCliques.emplace_back(clique);

			return;
		}

		std::set<std::string> unionPX = P;
		unionPX.insert(X.begin(), X.end());
		
		std::string pvt = *unionPX.begin();

		for (const std::string& v : unionPX)
		{
			if (m_adjGraph[v].size() > m_adjGraph[pvt].size())
			{
				pvt = v;
			}
		}

		std::set<std::string> excludedNeighbors;
		for (const std::string& v : m_adjGraph[pvt])
		{
			if (P.find(v) != P.end())
			{
				excludedNeighbors.insert(v);
			}
		}

		std::vector<std::string> candidates;
		for (const std::string& v : P)
		{
			if (excludedNeighbors.find(v) == excludedNeighbors.end())
			{
				candidates.push_back(v);
			}
		}

		for (const std::string& v : candidates)
		{
			std::set<std::string> newR = R;
			newR.insert(v);

			std::set<std::string> newP, newX;
			for (const std::string& u : P)
			{
				if (m_adjGraph[v].find(u) != m_adjGraph[v].end())
				{
					newP.insert(u);
				}
			}

			for (const std::string& u : X)
			{
				if (m_adjGraph[v].find(u) != m_adjGraph[v].end())
				{
					newX.insert(u);
				}
			}

			BronKerbosch(newR, newP, newX);

			P.erase(v);
			X.insert(v);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int totalTs = 0;

		std::vector<std::vector<std::string>> foundTs;

		for (const auto& kvp : m_adjGraph)
		{
			if (kvp.first[0] == ('t'))
			{
				for (const std::string& pair : kvp.second)
				{
					for (const std::string& pair2 : m_adjGraph[pair])
					{
						if (m_adjGraph[pair2].find(kvp.first) != m_adjGraph[pair2].end())
						{
							std::vector<std::string> newGroup;
							newGroup.push_back(kvp.first);
							newGroup.push_back(pair);
							newGroup.push_back(pair2);

							std::sort(newGroup.begin(), newGroup.end(), [](const std::string& LHS, const std::string& RHS) { return LHS < RHS; });

							const auto itFind = std::find_if(foundTs.begin(), foundTs.end(), [&](const std::vector<std::string>& LHS) {
								return !LHS[0].compare(newGroup[0]) && !LHS[1].compare(newGroup[1]) && !LHS[2].compare(newGroup[2]);
								});


							if (itFind == foundTs.end())
							{
								foundTs.push_back(newGroup);
								++totalTs;
							}

						}
					}
				}
			}
		}

		Log("Total Ts [%d]", totalTs);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::string password;
		const std::vector<std::string>& largestClique = m_allCliques.back();

		for (size_t i = 0; i < largestClique.size(); ++i)
		{
			if (i == 0)
			{
				password.append(largestClique[i]);
			}
			else
			{
				password.insert(password.end(), ',');
				password.append(largestClique[i]);
			}
		}

		Log("Password = [%s]", password.c_str());
		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<std::vector<std::string>> m_allCliques;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 23;
	newParams.year = 2024;
	newParams.puzzleTitle = "LAN Party";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
