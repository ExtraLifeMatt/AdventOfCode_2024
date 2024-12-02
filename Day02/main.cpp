#include "AdventGUI/AdventGUI.h"

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
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
	newParams.day = 2;
	newParams.year = 2024;
	newParams.puzzleTitle = "";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
