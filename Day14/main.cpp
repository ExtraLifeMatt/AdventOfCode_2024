#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"


class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	struct Robot
	{
		IntVec2 pos;
		IntVec2 vel;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, "=|,| ", tokens);
			assert(tokens.size() == 6);
			Robot newBot;
			newBot.pos.x = atoi(tokens[1].c_str());
			newBot.pos.y = atoi(tokens[2].c_str());
			newBot.vel.x = atoi(tokens[4].c_str());
			newBot.vel.y = atoi(tokens[5].c_str());

			m_robots.push_back(newBot);
		}

	}

	void Simulate(int32_t numSteps, std::vector<IntVec2>& outPos) const
	{
		outPos.clear();
		outPos.reserve(m_robots.size());
		for (const Robot& rbt : m_robots)
		{
			int32_t xRem = (rbt.pos.x + (rbt.vel.x * numSteps)) % BOARD_WIDTH;
			int32_t yRem = (rbt.pos.y + (rbt.vel.y * numSteps)) % BOARD_HEIGHT;

			if (xRem < 0)
			{
				xRem = BOARD_WIDTH + xRem;
			}

			if (yRem < 0)
			{
				yRem = BOARD_HEIGHT + yRem;
			}

			outPos.emplace_back(xRem, yRem);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int32_t quadCount[4] = { 0, 0, 0, 0};

		const int32_t halfX = (BOARD_WIDTH - 1) / 2;
		const int32_t halfY = (BOARD_HEIGHT - 1) / 2;

		std::vector<IntVec2> outPos;
		Simulate(100, outPos);

		for (const IntVec2& pos : outPos)
		{
			if (pos.y < halfY)
			{
				if (pos.x < halfX)
				{
					quadCount[0]++;
				}
				else if (pos.x > halfX)
				{
					quadCount[1]++;
				}
			}
			else if (pos.y > halfY)
			{
				if (pos.x < halfX)
				{
					quadCount[2]++;
				}
				else if (pos.x > halfX)
				{
					quadCount[3]++;
				}
			}
		}

		int64_t total = (int64_t)quadCount[0] * (int64_t)quadCount[1] * (int64_t)quadCount[2] * (int64_t)quadCount[3];
		Log("Result = %lld", total);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	void ToBytes(const std::vector<IntVec2>& data, std::vector<Bitfield128>& outFields) const
	{
		outFields.clear();
		outFields.insert(outFields.begin(), BOARD_HEIGHT, Bitfield128(0ULL, 0ULL));

		for (const IntVec2& pos : data)
		{
			outFields[pos.y].SetBit(pos.x);
		}
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		bool treeDetected = false;
		int32_t numSteps = 0;

		const int32_t halfX = (BOARD_WIDTH - 1) / 2;
		const int32_t halfY = (BOARD_HEIGHT - 1) / 2;

		std::vector<IntVec2> outPos;	
		std::vector<Bitfield128> bits;
		while (!treeDetected)
		{
			Simulate(++numSteps, outPos);

			ToBytes(outPos, bits);

			// Search for a group of contiguous values (5 is entirely arbitrary)
			for (size_t i = 0; i < BOARD_HEIGHT-1; ++i)
			{
				Bitfield128 AB = bits[i] & bits[i + 1];
				uint32_t bitIndex = 0;
				uint32_t contiguousSize = 0;
				while (!AB.IsZero())
				{
					Bits::GetContiguousBitsLSB128(AB, bitIndex, contiguousSize);
					if (contiguousSize >= 5)
					{
						Bitfield128 C = bits[i + 2] & Bits::CreateBitMask128(bitIndex, contiguousSize);
						if (Bits::PopCount128(C) == contiguousSize) // 5 x 3 at the least, probably it.
						{
							treeDetected = true;
							break;
						}
					}

					// Clear the bits, move on.
					AB &= ~Bits::CreateBitMask128(bitIndex, contiguousSize);
				}
			}
		}

		Log("Tree found on Step %d", numSteps);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	const int32_t BOARD_WIDTH = 101;
	const int32_t BOARD_HEIGHT = 103;
	std::vector<Robot> m_robots;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 14;
	newParams.year = 2024;
	newParams.puzzleTitle = "Restroom Redoubt";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
