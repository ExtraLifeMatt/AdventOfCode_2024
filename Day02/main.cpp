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
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, " ", tokens);
			m_reports.emplace_back(tokens);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One

		int numSafe = 0;
		for (const Report& rpt : m_reports)
		{
			if (rpt.IsSafe())
			{
				++numSafe;
			}

		}

		Log("Total Safe = %d", numSafe);

		// Done.
		 AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		int numSafe = 0;

		for (const Report& rpt : m_reports)
		{
			if (rpt.IsSafe() || CanBeMadeSafe(rpt))
			{
				++numSafe;
			}
		}

		Log("Total Safe = %d", numSafe);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	struct Report
	{
	public:
		Report(const std::vector<std::string>& valueArray)
		: values(),
		safe(true)
		{
			int lastValue = -1;
			uint32_t sign = ~0U;
			int delta = 0;
			uint32_t deltaSign = 0;

			values.reserve(valueArray.size());
			for (const std::string& valueStr : valueArray)
			{
				values.push_back(atoi(valueStr.c_str()));
				if (lastValue < 0)
				{
					lastValue = values.back();
				}
				else if (safe)
				{
					if (lastValue == values.back())
					{
						safe = false;
					}
					else 
					{
						delta = values.back() - lastValue;
						deltaSign = (uint32_t)delta & (1 << 7);
						if (sign == ~0U)
						{
							sign = deltaSign;
						}
						else
						{
							safe &= (deltaSign == sign);
						}
						safe &= abs(delta) <= 3;
						lastValue = values.back();
					}
				}
			}
		}

		bool IsSafe() const
		{
			return safe;
		}

		std::vector<int> values;
		bool safe;
	};


	bool CheckSafety(const std::vector<int>& values) const
	{
		uint32_t sign = ~0U;
		int delta = 0;
		uint32_t deltaSign = 0;
		for (int i = 1; i < values.size(); ++i)
		{
			delta = values[i] - values[i - 1];
			deltaSign = (uint32_t)delta & (1 << 7);
			if (delta == 0)
			{
				return false;
			}

			if (sign == ~0U)
			{
				sign = deltaSign;
			}
			else if (sign != deltaSign)
			{
				return false;
			}

			if (abs(delta) > 3)
			{
				return false;
			}
		}

		return true;
	}
	
	bool CanBeMadeSafe(const Report& rpt) const
	{
		if (rpt.IsSafe())
		{
			return true;
		}

		std::vector<int> reportVariant;
		reportVariant.reserve(rpt.values.size());
		for (int i = 0; i < rpt.values.size(); ++i)
		{
			reportVariant.clear();
			reportVariant.insert(reportVariant.begin(), rpt.values.begin(), rpt.values.end());
			reportVariant.erase(reportVariant.begin() + i);
			if (CheckSafety(reportVariant))
			{
				return true;
			}
		}

		return false;
	}



	std::vector<Report> m_reports;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 2;
	newParams.year = 2024;
	newParams.puzzleTitle = "Red-Nosed Reports";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
