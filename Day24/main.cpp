#include "AdventGUI/AdventGUI.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:

	enum class GateOp : uint8_t
	{
		AND = 0,
		OR,
		XOR
	};

	struct Gate
	{
		Gate(const std::string& _lhs, const std::string& _rhs, const std::string& _op, const std::string& _out)
		: LHS(_lhs),
		RHS(_rhs),
		OutReg(_out),
		OpStr(_op)
		{
			if (!_op.compare("AND"))
			{
				Op = GateOp::AND;
			}
			else if (!_op.compare("OR"))
			{
				Op = GateOp::OR;
			}
			else
			{
				assert(!_op.compare("XOR"));
				Op = GateOp::XOR;
			}
		}
		std::string LHS;
		std::string RHS;
		std::string OutReg;
		std::string OpStr;
		GateOp Op;
	};

	bool GateCanExecute(const Gate& gate, std::unordered_map<std::string, uint8_t>& registers)
	{
		return GetRegisterValue(gate.LHS, registers) != 0xFF && GetRegisterValue(gate.RHS, registers) != 0xFF;
	}

	void GateExecute(const Gate& gate, std::unordered_map<std::string, uint8_t>& registers)
	{
		uint8_t lhsV = GetRegisterValue(gate.LHS, registers);
		uint8_t rhsV = GetRegisterValue(gate.RHS, registers);
		assert(lhsV != 0xFF && rhsV != 0xFF);
		switch(gate.Op)
		{
			case GateOp::AND: SetRegisterValue(gate.OutReg, lhsV & rhsV, registers); break;
			case GateOp::OR: SetRegisterValue(gate.OutReg, lhsV | rhsV, registers); break;
			case GateOp::XOR: SetRegisterValue(gate.OutReg, lhsV ^ rhsV, registers); break;
			default: break;
		}
	}

	uint8_t GetRegisterValue(const std::string& reg, std::unordered_map<std::string, uint8_t>& registers)
	{
		std::unordered_map<std::string, uint8_t>::const_iterator itFind = registers.find(reg);
		if (itFind == registers.end())
		{
			itFind = registers.insert(std::make_pair(reg, 0xFF)).first;
		}
		return itFind->second;
	}

	void SetRegisterValue(const std::string& reg, uint8_t val, std::unordered_map<std::string, uint8_t>& registers)
	{
		registers[reg] = val;
	}

	std::unordered_map<std::string, std::vector<Gate>> registersToOps;

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

			size_t colonIdx = line.find(':');
			if (colonIdx != std::string::npos)
			{
				SetRegisterValue(line.substr(0, colonIdx), atoi(line.substr(colonIdx + 1).c_str()), m_regs);
			}
			else
			{
				StringUtil::SplitBy(line, " ", tokens);
				assert(tokens.size() == 5);
				m_gates.emplace_back(tokens[0], tokens[2], tokens[1], tokens[4]);
				registersToOps[m_gates.back().LHS].push_back(m_gates.back());
				registersToOps[m_gates.back().RHS].push_back(m_gates.back());
			}
		}

		for (auto& kvp : registersToOps)
		{
			std::sort(kvp.second.begin(), kvp.second.end(), [](const Gate& LHS, const Gate& RHS){ return (uint8_t)LHS.Op < (uint8_t)RHS.Op; });
		}
	}


	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::unordered_map<std::string, uint8_t> registers = m_regs;
		std::vector<Gate> gatesToRun = m_gates;

		while (gatesToRun.size())
		{
			for (size_t i = 0; i < gatesToRun.size(); )
			{
				if (GateCanExecute(gatesToRun[i], registers))
				{
					GateExecute(gatesToRun[i], registers);
					gatesToRun.erase(gatesToRun.begin() + i);
				}
				else
				{
					++i;
				}
			}
		}
		
		uint64_t i = 0;
		for (const std::pair<std::string, uint8_t>& kvp : registers)
		{
			if (kvp.first[0] == 'z' && kvp.second == 1)
			{
				i |= 1ULL << atoi(kvp.first.substr(1).c_str());
			}
		}

		Log("Value = %u", i);


		// Done.
		AdventGUIInstance::PartOne(context);
	}

	int GetGateIndex(const std::string& name) const
	{
		return Algorithm::find_index_of(m_gates.begin(), m_gates.end(), [&](const Gate& gate){ return gate.OutReg == name; });
	}

	bool IsInputRegister(const std::string& reg) const { return reg[0] == 'x' || reg[0] == 'y'; }

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::set<std::string> invalidNodes;
		for (const Gate& gate : m_gates)
		{
			// Check Adder rules.
			// If writing to final register (Z):
			if (gate.OutReg[0] == 'z')
			{
				int bit = atoi(gate.OutReg.substr(1).c_str());
				if (bit == 0 || bit == 45)
				{
					continue;
				}
			}

			bool lhsInput = IsInputRegister(gate.LHS);
			bool rhsInput = IsInputRegister(gate.RHS);

			if (lhsInput && !rhsInput)
			{
				invalidNodes.insert(gate.OutReg);
			}

			switch (gate.Op)
			{
				case GateOp::XOR:
				{
					if (lhsInput)
					{
						if (gate.OutReg[0] == 'z')
						{
							invalidNodes.insert(gate.OutReg);
						}
						else if (registersToOps[gate.OutReg][0].Op != GateOp::AND || registersToOps[gate.OutReg][1].Op != GateOp::XOR)
						{
							invalidNodes.insert(gate.OutReg);
						}
					}
					else if (gate.OutReg[0] != 'z')
					{
						invalidNodes.insert(gate.OutReg);
					}
				}
				break;
				case GateOp::AND:
				{
					// OR must be the operation AFTER the gate.
					if (registersToOps[gate.OutReg].size() < 1 || registersToOps[gate.OutReg][0].Op != GateOp::OR)
					{
						invalidNodes.insert(gate.OutReg);
					}
				}
				break;
				case GateOp::OR:
				{
					if (lhsInput || rhsInput)
					{
						invalidNodes.insert(gate.OutReg);
					}

					if (registersToOps.find(gate.OutReg) == registersToOps.end() || 
						registersToOps[gate.OutReg].size() < 2 || 
						registersToOps[gate.OutReg][0].Op != GateOp::AND || 
						registersToOps[gate.OutReg][1].Op != GateOp::XOR)
					{
						invalidNodes.insert(gate.OutReg);
					}
				}
				break;

			}
		}

		for (const Gate& gate : m_gates)
		{
			if (!gate.LHS.compare("x00") || !gate.LHS.compare("y00") || !gate.RHS.compare("x00") || !gate.LHS.compare("y00"))
			{
				invalidNodes.erase(gate.OutReg);
			}
		}

		std::string passcode;
		for (const std::string& str : invalidNodes)
		{
			if (passcode.size() == 0)
			{
				passcode.append(str);
			}
			else
			{
				passcode.insert(passcode.end(), ',');
				passcode.append(str);
			}
		}
		Log("Passcode: %s", passcode);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::unordered_map<std::string, uint8_t> m_regs;
	std::vector<Gate> m_gates;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 24;
	newParams.year = 2024;
	newParams.puzzleTitle = "Crossed Wires";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
