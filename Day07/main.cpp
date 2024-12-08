#include "AdventGUI/AdventGUI.h"

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	enum class InputOp : uint8_t
	{
		InputOp_Add = 0,
		InputOp_Mul,
		InputOp_Concat
	};

	struct Calibration
	{
		uint64_t targetValue;
		std::vector<uint32_t> readings;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, ":| ", tokens);
			Calibration newCalibration;
			newCalibration.targetValue = StringUtil::AtoiU64(tokens[0].c_str());
			for (size_t i = 1; i < tokens.size(); ++i)
			{
				newCalibration.readings.push_back((uint32_t)atoi(tokens[i].c_str()));
			}

			m_calibrations.emplace_back(newCalibration);
		}
	}

	bool IsValidCalibration(const Calibration& calib, bool allowConcat = false) const
	{
		if (RecursiveCheck(calib, 1, calib.readings[0], InputOp::InputOp_Add, allowConcat))
		{
			return true;
		}
		
		if (RecursiveCheck(calib, 1, calib.readings[0], InputOp::InputOp_Mul, allowConcat))
		{
			return true;
		}

		if (allowConcat && RecursiveCheck(calib, 1, calib.readings[0], InputOp::InputOp_Concat, allowConcat))
		{
			return true;
		}

		return false;
	}

	bool RecursiveCheck(const Calibration& calib, int index, uint64_t current, InputOp op, bool allowConcat) const
	{
		if (index >= (int)calib.readings.size())
		{
			 return current == calib.targetValue;
		}

		if (current > calib.targetValue)
		{
			return false;
		}

		if (op == InputOp::InputOp_Mul && (~0ULL / calib.readings[index]) < current)
		{
			return false;
		}

		uint64_t newCurrent = current;
		switch (op)
		{
			 case InputOp::InputOp_Add: newCurrent += (uint64_t)calib.readings[index]; break;
			 case InputOp::InputOp_Mul: newCurrent *= (uint64_t)calib.readings[index]; break;
			 case InputOp::InputOp_Concat:
			 {
				 char concatBuf[64] = { '\0' };
				 sprintf_s(concatBuf, "%llu%llu", current, (uint64_t)calib.readings[index]);
				 newCurrent = StringUtil::AtoiU64(concatBuf);
			 }
		}

		//return RecursiveCheck(calib, index + 1, newCurrent, InputOp::InputOp_Add) + RecursiveCheck(calib, index + 1, newCurrent, InputOp::InputOp_Mul);
		if (RecursiveCheck(calib, index + 1, newCurrent, InputOp::InputOp_Add, allowConcat))
		{
			return true;
		}

		if (RecursiveCheck(calib, index + 1, newCurrent, InputOp::InputOp_Mul, allowConcat))
		{
			return true;
		}

		if (allowConcat && RecursiveCheck(calib, index + 1, newCurrent, InputOp::InputOp_Concat, allowConcat))
		{
			return true;
		}

		return false;
	}
	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One

		uint64_t sum = 0;
		for (const Calibration& calib : m_calibrations)
		{
			if (IsValidCalibration(calib) != 0)
			{
				sum += calib.targetValue;
			}			
		}

		Log("Sum = %u", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t sum = 0;
		for (const Calibration& calib : m_calibrations)
		{
			if (IsValidCalibration(calib, true) != 0)
			{
				sum += calib.targetValue;
			}
		}

		Log("Sum = %u", sum);
		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<Calibration> m_calibrations;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 7;
	newParams.year = 2024;
	newParams.puzzleTitle = "Bridge Repair";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
