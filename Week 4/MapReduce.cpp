#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

string DataCollection = "web,wed,green,sun,moon,land,part,web,green";
vector<string> WordList;
map<string, int> ResultTable;

const int input_size = 9;
const int no_of_threads = 3;

class Parser {
public:
	void operator()(char delimiter)
	{
		string word;
		istringstream wordStream(DataCollection);
		while (getline(wordStream, word, delimiter))
			WordList.push_back(word);
	}
};

class Counter
{
public:
	void operator()(map<string, int> *pResultTable, int start)
	{
		while (WordList.size() != input_size);
		for (int i = start; i < start + input_size/no_of_threads; i++)
		{
			int count = (*pResultTable)[string(WordList[i])];
			count = count + 1;
			(*pResultTable)[string(WordList[i])] = count;
		}
	}
};

class WordCounter
{
private:
	map<string, int> ResultTables[no_of_threads];
public:
	void Map()
	{
		thread parser(Parser(), ',');
		parser.join();
		
	}
	void Reduce()
	{
		thread counter1(Counter(), &ResultTables[0], 0);
		thread counter2(Counter(), &ResultTables[1], 3);
		thread counter3(Counter(), &ResultTables[2], 6);
		
		counter1.join();
		counter2.join();
		counter3.join();

		for (int i = 0; i < no_of_threads; i++)
		{
			for (map<string, int>::iterator itr = ResultTables[i].begin();
				itr != ResultTables[i].end(); itr++)
				if (ResultTable.find(itr->first) == ResultTable.end())
					ResultTable[itr->first] = itr->second;
				else
					ResultTable[itr->first] += itr->second;
		}
	}
};

int main()
{
	WordCounter wordcounter;

	wordcounter.Map();
	wordcounter.Reduce();

	for (map<string, int>::iterator itr = ResultTable.begin(); itr != ResultTable.end(); itr++)
		cout << itr->first << "	" << itr->second << endl;

	return 0;
}
/*
green   2
land    1
moon    1
part    1
sun     1
web     2
wed     1
Press any key to continue . . .
*/