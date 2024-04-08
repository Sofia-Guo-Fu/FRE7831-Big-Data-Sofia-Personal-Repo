//HW2 Testing

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

// helper function: split string
vector<string> split(const string& s, char delimiter)
{
    vector<string> tokens;
    istringstream tokenStream(s);
    string token;
    
    while(getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
    
}

// helper function: generate a vector of time intervals
vector<string> genTimeInterval(string start_interval)
{
    vector<string> intervals(27);
    
    for (int i=0; i <= intervals.size(); i++)
    {
        intervals[i] = start_interval;
        int start_interval_hour = stoi(split(start_interval,':')[0]);
        //update the next interval accordingly
        int start_interval_min = stoi(split(start_interval,':')[1])+15;
        
        if (start_interval_min == 60)
        {
            start_interval_hour += 1;
            start_interval_min = 0;
            start_interval = to_string(start_interval_hour) + ":00:00";
        }
        else
        {
            start_interval = to_string(start_interval_hour) + ":" + to_string(start_interval_min) + ":00";
        }
        
    }
    return intervals;
}

//helper function: convert time to corresponding intervals
string convertTime(string timestamp)
{
    string hour = split(timestamp,':')[0];
    int min = stoi(split(timestamp, ':')[1]);
    string converted_min;
    string converted_time;
    
    switch (int(min/15))
    {
        case 0:
            converted_min = "00";
            break;
        case 1:
            converted_min = "15";
            break;
        case 2:
            converted_min = "30";
            break;
        case 3:
        default:
            converted_min = "45";
            break;
    }
    converted_time = hour + ":" + converted_min + ":00";
    return converted_time;
}

int main()
{
//    string start_date = "01-MAY-2012";
    string start_interval = "13:30:00"; //GMT
    
    // generate a vector of strings for time interval grid
    vector<string> intervals = genTimeInterval(start_interval);
    
//    for(const auto& interval: intervals)
//    {
//        cout << interval <<endl;
//    }
    
    // initialize a map structure to store the output
    map<string, pair<long, double>> QtyPriceMap;
    for (const auto& interval: intervals)
    {
        // initialize each key with default value
        QtyPriceMap[interval] = {0, 0.0};
    }
    
    //----------------------- read csv file ---------------------------
    ifstream file("SPY_May_2012.csv");
    
    if (!file.is_open())
    {
        cerr << "Failed to open the file. "<<endl;
        return 1;
    }
    
    string line, header;
    //skip header
    getline(file, header);
    
    while (getline(file, line))
    {
        vector<string> row = split(line, ',');
        string date = row[1];
        string type = row[4];
        
        if(type =="Trade")
        {
            int day_date = stoi(split(date,'-')[0]);
            string month_date = split(date,'-')[1];
            string time = row[2];
            
            //  MAY-01-2012 ~  MAY-20-2012
            if (month_date=="MAY" && day_date>=1 && day_date <= 20)
            {
                string converted_time = convertTime(time);
                string volumeStr = row[6];
                long volume;
                // convert empty string to 0
                if (volumeStr.empty())
                {
                    volume = 0;
                }else{
                    volume = stol(volumeStr);
                }
                // update volume if the 'converted_time' exists as keys in QtyPriceMap
                auto it = QtyPriceMap.find(converted_time);
                if (it != QtyPriceMap.end())
                {
                    it->second.first += volume;
//                    cout <<"added "<< volume <<" to time "<<converted_time<<endl;
                }
            }
            // MAY-21-2012
            if (month_date=="MAY" && day_date==21)
            {
                string converted_time = convertTime(time);
                string price_str = row[5];
                double price;
                if (price_str.empty())
                {
                    price = 0;
                }else{
                    price = stod(price_str);
                }
                
                // check if the price for this time interval has already been updated
                auto it = QtyPriceMap.find(converted_time);
                if (it != QtyPriceMap.end() && it->second.second == 0.0)
                {
                    // update the price of this time interval with the 1st encountered price
                    it->second.second = price;
                }
            }
       }
    }
    file.close();
    
    // output to csv file
    ofstream outfile("outputs.csv");
    if (!outfile.is_open())
    {
        cerr << "Failed to open the output file. "<<endl;
    }
    
    // Write header
    outfile <<"Time Interval (NY local time), Volume(Avg Qty from 5/1-5/20), Price(Trading price of 5/21) "<<endl;
    // Final update to QtyPriceMap while writing and printing out
    for(auto& pair: QtyPriceMap)
    {
        // Deduct 4 hours from the timestamp
        string timestamp = pair.first;
        string hour = timestamp.substr(0,2);
        int new_hour = (stoi(hour)-4+24) % 24; // add 24 to ensure positive
        timestamp = (new_hour < 10 ? "0" : "") + to_string(new_hour) + timestamp.substr(2);
        
        // Divide volume by 14
        long volume = (pair.second.first / 14);
        cout << "Time: "<< timestamp <<" Volume: "<<volume<< " Price: "<<pair.second.second<<endl;
        // write to file
        outfile << timestamp << "," << volume << "," << pair.second.second<<endl;
    }
    
    outfile.close();
    
    return 0;
    
}

