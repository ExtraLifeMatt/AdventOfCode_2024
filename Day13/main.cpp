#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"


class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	struct ClawContext
	{
		Int64Vec2 BtnA;
		Int64Vec2 BtnB;
		Int64Vec2 Target;
	};

	uint64_t Score(const ClawContext& context, bool partTwo = false) const
	{
		/*
		Button A: X+94, Y+34
		Button B: X+22, Y+67
		Prize: X=8400, Y=5400

		8400 = 80 * 94 + 40 * 22
		5400 = 80 * 34 + 40 * 67
		*/

		// Tx = A * Ax + B * Bx 
		// Ty = A * Ay + B * By

		// Solve for A
		// Tx * By = A * Ax * By + B * Bx * By
		// Ty * Bx = A * Ay * Bx + B * By * Bx
		// 
		// Tx * By - Ty * Bx = A * Ax * By + B * Bx * By - A * Ay * Bx + B * By * Bx
		// 
		// Tx * By - Ty * Bx = A * (Ay * Bx - Ax * By) // This step took FOREVER to figure out
		// 
		// A = (Tx * By - Ty * Bx) / (Ay * Bx - Ax * By);
		// 
		// Solve for B (We plug in the A above)
		// Tx = A * Ax + B * Bx
		// B = (Tx - A * Ax) / Bx
		const Int64Vec2& aXY = context.BtnA;
		const Int64Vec2& bXY = context.BtnB;
		Int64Vec2 tXY = context.Target;

		if (partTwo)
		{
			tXY += 10000000000000LL;
		}

		int64_t Adet = (aXY.y * bXY.x - aXY.x * bXY.y);
		int64_t Anom = (bXY.x * tXY.y - bXY.y * tXY.x);
		if ((Anom % Adet) != 0)
		{
			return -1;
		}

		int64_t A = Anom / Adet;

		int64_t Bdet = bXY.x;
		int64_t Bnom = tXY.x - A * aXY.x;
		if ((Bnom % Bdet) != 0)
		{
			return -1;
		}
		int64_t B = Bnom / Bdet;

		return A * 3LL + B;
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.empty())
			{
				continue;
			}

			StringUtil::SplitBy(line, ":|,", tokens);
			assert(tokens.size() == 3);
			ClawContext newClaw;
			newClaw.BtnA.x = StringUtil::AtoiI64(tokens[1].substr(2).c_str());
			newClaw.BtnA.y = StringUtil::AtoiI64(tokens[2].substr(2).c_str());
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, ":|,", tokens);
			assert(tokens.size() == 3);
			newClaw.BtnB.x = StringUtil::AtoiI64(tokens[1].substr(2).c_str());
			newClaw.BtnB.y = StringUtil::AtoiI64(tokens[2].substr(2).c_str());
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, ":|,", tokens);
			assert(tokens.size() == 3);
			newClaw.Target.x = StringUtil::AtoiI64(tokens[1].substr(2).c_str());
			newClaw.Target.y = StringUtil::AtoiI64(tokens[2].substr(2).c_str());

			m_claws.push_back(newClaw);
		}

	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int64_t totalSum = 0;
		int64_t clawSum = 0;
		for (const ClawContext& claw : m_claws)
		{
			clawSum = Score(claw);
			if (clawSum != -1LL)
			{
				totalSum += clawSum;
			}
		}

		Log("Total Tokens: %lld", totalSum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		int64_t totalSum = 0;
		int64_t clawSum = 0;
		for (const ClawContext& claw : m_claws)
		{
			clawSum = Score(claw, true);
			if (clawSum != -1LL)
			{
				totalSum += clawSum;
			}
		}

		Log("Total Tokens: %lld", totalSum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<ClawContext> m_claws;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 13;
	newParams.year = 2024;
	newParams.puzzleTitle = "Claw Contraption";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
