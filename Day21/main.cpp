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
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_keycodes.push_back(line);
		}

	}

	std::string GetKeyPresses(char from, char to, int depth) const
	{
		static std::string numpad = "789456123#0A";
		static std::string dirpad = "#^A<v>";
		static std::string keys[] = { numpad, dirpad };
		static const int height[] = { 3, 2 };
		static const int width[] = { 3, 3 };
		static const int errorIdx[] = { 9, 0 }; // Where our # is.

		static IntVec2 errorPos[] = { { 0, 3 }, { 0, 0} };
		assert(keys[0][errorIdx[0]] == '#' && keys[1][errorIdx[1]] == '#');

		int index = std::min(depth, 1); // 0 is the numpad, everything else is dirpad.

		int fromIdx = (int32_t)keys[index].find(from);
		int toIdx = (int32_t)keys[index].find(to);

		IntVec2 fromPos(fromIdx % width[index], fromIdx / width[index]);
		IntVec2 toPos(toIdx % width[index], toIdx / width[index]);

		// Just use our axis difference to find our most direct route.
		IntVec2 delta = toPos - fromPos;

		// Never zigzag, which actually makes this easier.
		std::string vKeys, hKeys;
		vKeys.insert(vKeys.end(), abs(delta.y), delta.y < 0 ? '^' : 'v');
		hKeys.insert(hKeys.end(), abs(delta.x), delta.x < 0 ? '<' : '>');

		if (toPos.y == errorPos[index].y && fromPos.x == errorPos[index].x)
		{
			return hKeys + vKeys;
		}

		if (fromPos.y == errorPos[index].y && toPos.x == errorPos[index].x)
		{
			return vKeys + hKeys;
		}
		// Check if we would go over the void.
		//if (fromPos.y + delta.y == errorPos[index].y) // vertical, so must go horizontal first.
		//{
		//	return hKeys + vKeys;
		//}

		//if (fromPos.x + delta.x == errorPos[index].x) // horizontal, so must go vertical first.
		//{
		//	return vKeys + hKeys;
		//}

		// If we're going left, go horizontal first.
		if (delta.x < 0)
		{
			return hKeys + vKeys;
		}

		//  If going right, go vertical first.
		return vKeys + hKeys;
	}

	size_t Solve(const std::string& keycode, int totalRobots, std::unordered_map<size_t, std::pair<size_t, std::string>>& cache, std::vector<std::string>& debugStr) const
	{
		size_t totalLength = 0;
		size_t complexity = atoi(keycode.substr(0, 3).c_str()); // Always ends with 'A'.

		debugStr.clear();
		while (debugStr.size() < totalRobots)
		{
			debugStr.emplace_back("");
		}

		char curr = 'A';
		for (char c : keycode)
		{
			totalLength += Solve_R(curr, c, 0, totalRobots - 1, cache, debugStr);
			curr = c;
		}

		return totalLength * complexity;
	}

	size_t Solve_R(char from, char to, int depth, int maxDepth, std::unordered_map<size_t, std::pair<size_t, std::string>>& cache, std::vector<std::string>& debugStr ) const
	{
		size_t cacheEntryId = (size_t)from | (size_t)to << 8 | (size_t)depth << 16;

		std::unordered_map<size_t, std::pair<size_t, std::string>>::const_iterator itFind = cache.find(cacheEntryId);
		if (itFind != cache.end())
		{
			Log("Cached Result [%zd] [%d] [%c] -> [%c] = [%zd] (%s)", cacheEntryId, depth, from, to, itFind->second.first, itFind->second.second.c_str());
			debugStr[depth].append(itFind->second.second);
			Log("Depth [%d] is now [%s]", depth, debugStr[depth].c_str());
			return itFind->second.first;
		}

		std::string presses = GetKeyPresses(from, to, depth) + "A";
		Log("[%d] %c -> %c : %s", depth, from, to, presses.c_str());

		debugStr[depth].append(presses.c_str());
		Log("Depth [%d] is now [%s]", depth, debugStr[depth].c_str());

		size_t currentLength = presses.size();

		// Chain up to the robot behind us.
		if (depth < maxDepth)
		{
			char curr = 'A'; // All robots start on A.
			for (char c : presses)
			{
				currentLength += Solve_R(curr, c, depth + 1, maxDepth, cache, debugStr);
				curr = c;
			}
		}

		cache.insert(std::make_pair(cacheEntryId, std::make_pair(currentLength, presses)));

		return currentLength;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		size_t sum = 0;
		std::unordered_map<size_t, std::pair<size_t, std::string>> cache;
		std::vector<std::string> debug;
		for (const std::string& keycode : m_keycodes)
		{
			sum += Solve(keycode, 3, cache, debug);
		}
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<std::string> m_keycodes;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 21;
	newParams.year = 2024;
	newParams.puzzleTitle = "Keypad Conundrum";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
