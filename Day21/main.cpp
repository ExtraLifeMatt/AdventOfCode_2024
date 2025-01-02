#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"

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
		line = fileReader.ReadLine();
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

};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 21;
	newParams.year = 2024;
	newParams.puzzleTitle = "Race Condition";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
