#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"

#include <unordered_set>

struct Node
{
	Node(const char* _name)
	{
		memcpy(m_name, _name, 2);
		memset(m_connections, 0, sizeof(const Node*) * 8);
		m_hash = 0;
		m_hash = ((uint32_t)m_name[0]) | ((uint32_t)m_name[0]) << 16;
		m_totalConnections = 0;
	}

	bool HasConnection(const Node* node)
	{
		for (uint8_t i = 0; i < m_totalConnections; ++i)
		{
			if (m_connections[i] == node)
			{
				return true;
			}
		}

		return false;
	}

	bool HasConnection(const char* nodeName) const
	{
		for (uint8_t i = 0; i < m_totalConnections; ++i)
		{
			if (memcmp(m_name, nodeName, 2) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void AddConnection(const Node* node)
	{
		m_connections[m_totalConnections++] = node;
	}

	size_t GetHash() const { return m_hash; }

	char m_name[2];
	const Node* m_connections[8];
	uint8_t m_totalConnections;
	size_t m_hash;
};

class NodeHasher
{
public:
	std::size_t operator()(const Node* lhs) const
	{
		return lhs->GetHash();
 	}
};

class NodeCompare
{
public:
	bool operator()(const Node* LHS, const Node* RHS) const
	{
		return LHS->GetHash() == RHS->GetHash();
	}
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
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			assert(line.size() == 5);
			Node* nodeA = GetOrAddNode(line.substr(0, 2).c_str());
			Node* nodeB = GetOrAddNode(line.substr(3, 2).c_str());

			ConnectNodes(nodeA, nodeB);
		}

		Log("Total Nodes Parsed %zd", m_Nodes.size());
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	void ConnectNodes(Node* A, Node* B)
	{
		assert(A && B);
		if(!A->HasConnection(B))
		{
			A->AddConnection(B);
		}

		if (!B->HasConnection(A))
		{
			B->AddConnection(A);
		}
	}

	Node* GetOrAddNode(const char* name)
	{
		Node* outNode = nullptr;

		Node tmp(name);
		std::unordered_set<Node*, NodeHasher, NodeCompare>::iterator itFind = m_Nodes.find(&tmp);
		if (itFind == m_Nodes.end())
		{
			outNode = new Node(name);
			m_Nodes.insert(outNode);
		}
		else
		{
			outNode = *itFind;
		}
		assert(outNode);
		return outNode;
	}

	std::unordered_set<Node*, NodeHasher, NodeCompare> m_Nodes;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 23;
	newParams.year = 2024;
	newParams.puzzleTitle = "LAN Party";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
