//
//  main.cpp
//  HW3-MapReduce
//
//  Created by Sofia Guo  on 4/13/24.

#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

//const int input_size = 9;
const string FILE_NAME = "4727.txt";
int input_size;
const int no_of_threads = 3;

// initialize other data structures
vector<string> WordList;
map<string, int> ResultTable;


// helper function: check if isNumeric
bool isNumeric(const string& word)
{
    try
    {
        // Handle special cases where the word starts with specific prefixes
        if (word.substr(0, 3) == "inf") {
            // Prefix "inf" is not considered numeric
            return false;
        }
        // Attempt to convert the string to a float
        stod(word);
        // If conversion suceeds, the word is numeric
        return true;
    }
    catch(const invalid_argument& e)
    {
        // If conversion fails, the word is not numeric
        return false;
    }
}

// helper: trim both leading and trailing whitespace from each word
void trim(string& str)
{
    str.erase(0, str.find_first_not_of(" \t\r\n")); // Trim leading whitespace
    str.erase(str.find_last_not_of(" \t\r\n") + 1); // Trim trailing whitespace
}


class Parser
{
public:
    void operator()(const string& filename, char delimiter)
    {
        ifstream inputFile(filename);
        if (!inputFile.is_open())
        {
            std::cerr << "Error: Unable to open file" << endl;
            return;
        }
        
        string line;
        
        // loop through each line
        while(getline(inputFile, line))
        {
            istringstream wordStream(line);
            // remove carriage return character
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());
            // initialize word
            string word;
            // loop through each word in wordStream
            while(getline(wordStream, word, delimiter))
            {
                // Remove leading and trailing whitespace
                // word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
                trim(word); // Trim whitespace from the word
                
                // if the word is not numeric, convert to lower case
                if(!isNumeric(word))
                {
                    for(char &c: word)
                        c = tolower(c);
                    // then add it to WordList
                    WordList.push_back(word);
                }
            }
        }
        inputFile.close();
    }
};

class Counter
{
public:
    void operator()(map<string, int> *pResultTable, int start)
    {
        for(int i = start; i < start + input_size/no_of_threads; i++)
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
        thread parser(Parser(),FILE_NAME,' ');
        parser.join();
    }
    void Reduce()
    {
        thread counter1(Counter(), &ResultTables[0],0);
        thread counter2(Counter(), &ResultTables[1],input_size/no_of_threads);
        thread counter3(Counter(), &ResultTables[2],2 * input_size/no_of_threads);
        
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
    // create WordCounter object
    WordCounter wordcounter;
    
    // Map the contents of the file to WordList
    wordcounter.Map();
    
    // print the contents of WordList
//    for(const string& word: WordList)
//    {
//        cout <<  word << endl;
//    }
    input_size = WordList.size();
    cout << "WordList size: " << input_size << endl;
    
    wordcounter.Reduce();
    
    // print the contents of the final ResultTable debug
    for(map<string, int>::iterator itr = ResultTable.begin(); itr != ResultTable.end(); itr++)
        cout << itr->first << " " << itr->first.size() <<"  " <<itr->second << endl;
    
    // write the final ResultTable to a txt file
    ofstream outfile("outputs_mine.txt");
    if(!outfile.is_open())
    {
        cerr << "Failed to open the output file. " <<  endl;
    }
    
    for(map<string,int>::iterator itr = ResultTable.begin(); itr != ResultTable.end(); itr++)
        // setw() for formatting
        outfile << setw(25) << right << itr-> first << setw(10) << right << itr->second << endl;
    
    outfile.close();
    
    return 0;
}

/*
 WordList size: 706
 */
