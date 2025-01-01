#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"


class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	struct OpContext
	{
		int32_t registers[3] = { 0, 0, 0};
		int instPtr = 0;

		int GetComboParam(int value) const
		{
			value = value & 0x7;
			constexpr int highBit = 1 << 2;
			if (value & highBit)
			{
				// Just mask off the high bit.
				return registers[value & ~highBit];
			}
			return value;
		}
	};

	bool Exec(const OpContext& input, const std::vector<int>& program, OpContext& output)
	{
		if (input.instPtr >= (int)program.size())
		{
			return false;
		}

		int opCode = program[input.instPtr];
		int arg = program[input.instPtr + 1];
		memcpy(output.registers, input.registers, sizeof(int) * 3);
		output.instPtr = input.instPtr;
		switch (opCode)
		{
			case 0: // adv - A = A / 2^C(arg)
			{
				int nom = input.registers[0];
				int denom = Math::Pow(2, input.GetComboParam(arg));
				output.registers[0] = nom / denom;
				output.instPtr += 2;

				Log("A = A / 2^C = %d = %d / 2^%d (%d)", output.registers[0], nom, arg, denom);
				Log("A is now [%d] Octal [%o]", output.registers[0], output.registers[0]);
			}
			break;
			case 1: // bxl B = B ^ arg
			{
				output.registers[1] = input.registers[1] ^ arg;
				output.instPtr += 2;

				Log("B = B ^ arg  %d = %d ^ %d", output.registers[1], input.registers[1], arg);
			}
			break;
			case 2: // bst B = C(arg) % 8
			{
				int value = input.GetComboParam(arg);
				output.registers[1] = value & 0x7;
				output.instPtr += 2;
				Log("B = C(arg) mod 8 %d = %d mod 8", output.registers[1], value, arg);
			}
			break;
			case 3: // jnz - jump
			{
				if (input.registers[0])
				{
					output.instPtr = arg;
				}
				else
				{
					output.instPtr += 2;
				}
			}
			break;
			case 4: // bxc B = B ^ C
			{
				output.registers[1] = input.registers[1] ^ input.registers[2];
				output.instPtr += 2;

				Log("B = B ^ C  %d = %d ^ %d", output.registers[1], input.registers[1], input.registers[2]);
			}
			break;
			case 5: // output
			{
				int value = input.GetComboParam(arg) & 0x7;
				if (m_output.size() == 0)
				{
					m_output.insert(m_output.end(), '0' + value);
				}
				else
				{
					m_output.insert(m_output.end(), ',');
					m_output.insert(m_output.end(), '0' + value);
				}
				output.instPtr += 2;

				Log("Arg %d Output %d (%d) Octal [%o]", arg, value, input.GetComboParam(arg), input.GetComboParam(arg));
			}
			break;
			case 6:
			{
				int nom = input.registers[0];
				int denom = Math::Pow(2, input.GetComboParam(arg));
				output.registers[1] = nom / denom;
				output.instPtr += 2;

				Log("B = A / 2^C = %d = %d / 2^%d (%d)", output.registers[1], nom, arg, denom);
			}
			break;
			case 7:
			{
				int nom = input.registers[0];
				int denom = Math::Pow(2, input.GetComboParam(arg));
				output.registers[2] = nom / denom;
				output.instPtr += 2;

				Log("C = A / 2^C = %d = %d / 2^%d (%d)", output.registers[2], nom, arg, denom);
			}
			break;
			default:
			assert(false);
			break;
		}

		return true;
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		if (!fileReader.IsEOF())
		{
			// Reg A
			line = fileReader.ReadLine();
			size_t colonIdx = line.rfind(':');
			assert(colonIdx != std::string::npos);
			m_startContext.registers[0] = atoi(line.substr(colonIdx + 1).c_str());

			// Reg B
			line = fileReader.ReadLine();
			colonIdx = line.rfind(':');
			assert(colonIdx != std::string::npos);
			m_startContext.registers[1] = atoi(line.substr(colonIdx + 1).c_str());

			// Reg C
			line = fileReader.ReadLine();
			colonIdx = line.rfind(':');
			assert(colonIdx != std::string::npos);
			m_startContext.registers[2] = atoi(line.substr(colonIdx + 1).c_str());

			line = fileReader.ReadLine();
			assert(line.size() == 0);

			line = fileReader.ReadLine();
			colonIdx = line.rfind(':');
			assert(colonIdx != std::string::npos);
			m_rawProgram = line.substr(colonIdx + 2).c_str();

			StringUtil::SplitBy(m_rawProgram, ",", tokens);
			for (size_t i = 0; i < tokens.size(); ++i)
			{
				m_program.push_back(atoi(tokens[i].c_str()));
			}
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		OpContext pingPong[2] = { m_startContext, OpContext()};
		int ppIdx = 0;
		initialA = m_startContext.registers[0];
		while (Exec(pingPong[ppIdx], m_program, pingPong[ppIdx ^ 1]))
		{
			ppIdx ^= 1;
		}
		Log("Output: %s", m_output.c_str());

		Log("Done.");

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	std::vector<int> GetOctal(uint64_t A) const
	{
		uint64_t B = 0;
		uint64_t C = 0;
		uint64_t lastValue = 0;
		std::vector<int> outValues;

		while (A != 0)
		{
			B = A % 8ULL;
			B ^= 3ULL;
			C = A / (1ULL << B);
			B ^= C;
			B ^= 3ULL;
			A = A / (1ULL << 3ULL);

			outValues.push_back(B % 8ULL);
		}

		return outValues;
	}

	uint64_t RecursiveCheck(uint64_t A, int Index) const
	{	
		std::vector<int> testValues;
		for (uint64_t i = 0; i < 8; ++i)
		{
			testValues = GetOctal(A + i);
			if (testValues.size() && testValues.front() == m_program[m_program.size() - Index - 1])
			{
				if (Index + 1 == m_program.size())
				{
					return A + i;
				}

				uint64_t next = RecursiveCheck((A + i) << 3ULL, Index + 1);
				if (next != 0)
				{
					return next;
				}
			}
		}

		return 0;
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t key = RecursiveCheck(0, 0);

		Log("Key = %u", key);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	OpContext m_startContext;
	std::vector<int> m_program;
	std::string m_rawProgram;
	std::string m_output;
	int initialA;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 17;
	newParams.year = 2024;
	newParams.puzzleTitle = "Chronospatial Computer";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
