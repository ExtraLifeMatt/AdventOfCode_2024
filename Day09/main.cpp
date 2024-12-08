#include "AdventGUI/AdventGUI.h"

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
		}

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
	int m_mapWidth;
	int m_mapHeight;
	std::string m_mapData;
	std::vector<Antenna> m_antennas;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 9;
	newParams.year = 2024;
	newParams.puzzleTitle = "";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
