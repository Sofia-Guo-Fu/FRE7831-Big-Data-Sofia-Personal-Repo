//
//  main.cpp
//  FRE-7831-Lab-1
//
//  Created by Sofia Guo  on 4/8/24.
//

#include <iostream>
#include "Stock.h"
#include "MarketData.h"
#include "Database.h"
#include "Util.h"
#include <map>

int main(void)
{
    string database_name = "MarketData.db";
    cout << "Opening MarketData.db ..."<<endl;
    sqlite3* db = NULL;
    if (OpenDatabase(database_name.c_str(), db) == -1)
        return -1;

    bool bCompleted = false;
    char selection;
    
    // initialize a stockMap
    map<string, Stock> stockMap;
    
    while(!bCompleted)
    {
        cout << endl;
        cout << "Menu"<<endl;
        cout << "========" <<endl;
        cout << "A - Create DailyTrades and Intraday Trades Tables" << endl;
        cout << "B - Retrieve Daily Trade and Intraday Trade Data" << endl;
        cout << "C - Populate DailyTrades and IntradayTrades Table" << endl;
        cout << "D - Retrieve Data from DailyTrades and IntradayTrades Tables" << endl;
        cout << "X - Exit" << endl << endl;
        cout << "Enter selection: ";
        std::cin >> selection;
        switch(selection)
        {
            case 'A':
            case 'a':
            {
                // Drop DailyTrades table if exists
                cout << "Drop DailyTrades table if exists" << endl;
                string sql_DropaTable = "DROP TABLE IF EXISTS DailyTrades";
                if (DropTable(db, sql_DropaTable.c_str()) == -1)
                    return -1; 

                // Create DailyTrades table
                cout << "Creating DailyTrades table..." << endl;
                string sql_CreateTable = string("CREATE TABLE IF NOT EXISTS DailyTrades ")
                + "(Symbol CHAR(20) NOT NULL,"
                + "Date CHAR(20) NOT NULL,"
                + "Open REAL NOT NULL,"
                + "High REAL NOT NULL,"
                + "Low REAL NOT NULL,"
                + "Close REAL NOT NULL,"
                + "Adjusted_close REAL NOT NULL,"
                + "Volume INT NOT NULL,"
                + "PRIMARY KEY(Symbol, Date),"
                + "Foreign Key(Symbol) references Stocks(Symbol)\n"
                + "ON DELETE CASCADE\n"
                + "ON UPDATE CASCADE"
                + ");";
                
                if (ExecuteSQL(db, sql_CreateTable.c_str()) == -1)
                    return -1;
                
                // Drop IntradayTrades table if exists
                cout << "Drop IntradayTrades table if exists" << endl;
                // assign a new value (SQL statement)
                sql_DropaTable = "DROP TABLE IF EXISTS IntradayTrades";
                if (DropTable(db, sql_DropaTable.c_str()) == -1)
                    return -1;
                
                // Create IntradayTrades table
                cout << "Creating IntradayTrades table..." << endl;
                sql_CreateTable = string("CREATE TABLE IF NOT EXISTS IntradayTrades ")
                + "(Symbol CHAR(20) NOT NULL,"
                + "Date CHAR(20) NOT NULL,"
                + "Timestamp CHAR(20) NOT NULL,"
                + "Open REAL NOT NULL,"
                + "High REAL NOT NULL,"
                + "Low REAL NOT NULL,"
                + "Close REAL NOT NULL,"
                + "Volume INT NOT NULL,"
                + "PRIMARY KEY(Symbol, Date, Timestamp),"
                + "Foreign Key(Symbol, Date) references DailyTrades(Symbol, Date)\n"
                + "ON DELETE CASCADE\n"
                + "ON UPDATE CASCADE\n"
                + ");";
                
                if (ExecuteSQL(db, sql_CreateTable.c_str()) == -1)
                    return -1;
                
                break;
            }
            case 'B':
            case 'b':
            {
                cout << "Retrieving Daily Trade and Intraday Trade Data..." << endl;
                string sConfigFile = "config_mac.csv";
                map<string, string> configMap = ProcessConfigData(sConfigFile);
                
                string daily_url_common = configMap["daily_url_common"];
                string intraday_url_common = configMap["intraday_url_common"];
                string start_date = configMap["start_date"];
                string end_date = configMap["end_date"];
                string api_token = configMap["api_token"];
                // a vector of symbols, use the split() helper function here
                vector<string> symbols = split(configMap["symbol"], ',');
                
                // loop through vector of symbols
                for(vector<string>::iterator itr = symbols.begin(); itr != symbols.end(); itr++)
                {
                    string symbol = *itr;
                    // initialize a Stock object aStock
                    Stock aStock(*itr);
                    string daily_urlRequest = daily_url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
                    string intraday_urlRequest = intraday_url_common + symbol + ".US?" + "&api_token=" + api_token + "&interval=5m&fmt=json";
                    cout << " daily_urlRequest: "<< daily_urlRequest << endl;
                    cout << "intraday_urlRequest: "<<intraday_urlRequest << endl;
                    
                    // Pull & pase dailyTrade data
                    string daily_buffer;
                    int runStatus_daily = 0;
                    runStatus_daily = PullMarketData(daily_urlRequest, daily_buffer);
                    if (runStatus_daily != 0)
                    {
                        cout << "Failed in retrieving daily market data for symbol " << symbol << endl;
                        return -1;
                    }
                    runStatus_daily = PopulateDailyTrades(daily_buffer, aStock);
                    if (runStatus_daily != 0)
                    {
                        cout << "Failed in parsing daily market data for symbol " << symbol << endl;
                        return -1;
                    }
                    
                    // Pull & parse intradayTrade data
                    string intraday_buffer;
                    int runStatus_intraday = 0;
                    runStatus_intraday = PullMarketData(intraday_urlRequest, intraday_buffer);
                    if (runStatus_intraday != 0)
                    {
                        cout << "Failed in retrieving intraday market data for symbol " << symbol << endl;
                        return -1;
                    }
                    // specify unix timestamp of start_Date for intraday data download
                    long April_1_2024 = 1711944000;
                    runStatus_intraday = PopulateIntradayTrades(intraday_buffer, aStock, April_1_2024);
                    if (runStatus_intraday != 0)
                    {
                        cout << "Failed in parsing intraday market data for symbol " << symbol << endl;
                        return -1;
                    }
                    
                    cout << aStock << endl;
                    stockMap[*itr] = aStock;
                }
                break;
            }
            case 'C':
            case 'c':
            {
                cout << " Populating DailyTrades and IntradayTrades Table...." << endl;
                // loop through stockMap
                for(map<string, Stock>:: iterator itr = stockMap.begin(); itr != stockMap.end(); itr++)
                {
                    // initialize SQL Insert statement
                    char sql_Insert[512];
                    string symbol = itr->first;
                    vector<DailyTrade> dailyTrades = itr->second.GetDailyTrade();
                    vector<IntradayTrade> intradayTrades = itr->second.GetIntradayTrade();
                    
                    // insert daily data to table DailyTrades
                    for(auto itr = dailyTrades.begin(); itr != dailyTrades.end(); itr++)
                    {
                        cout << "Inserting daily data for stock "<< symbol<< " into table DailyTrades..." << endl << endl;
                        memset(sql_Insert, '\0', sizeof(sql_Insert));
                        sprintf(sql_Insert, "INSERT INTO DailyTrades(Symbol, Date, Open, High, Low, Close, Adjusted_close, Volume) VALUES (\"%s\", \"%s\",%f, %f, %f, %f, %f, %d)",
                                symbol.c_str(), itr->GetDate().c_str(), itr->GetOpen(), itr->GetHigh(), itr->GetLow(), itr->GetClose(), itr->GetAdjustedClose(), itr->GetVolume());
                        
                        if (ExecuteSQL(db, sql_Insert) == -1)
                            return -1;
                    }
                    
                    // insert intraday data to table IntradayTrades
                    for(auto itr = intradayTrades.begin(); itr != intradayTrades.end(); itr++)
                    {
                        cout << "Inserting intraday data for stock "<< symbol << " into table IntradayTrades... " << endl << endl;
                        memset(sql_Insert, '\0', sizeof(sql_Insert));
                        sprintf(sql_Insert, "INSERT INTO IntradayTrades(Symbol, Date, Timestamp, Open, High, Low, Close, Volume) VALUES (\"%s\", \"%s\", \"%s\", %f, %f, %f, %f, %d)",
                                symbol.c_str(), itr->GetDate().c_str(), itr->GetTimestamp().c_str(), itr->GetOpen(), itr->GetHigh(), itr->GetLow(), itr->GetClose(), itr->GetVolume());
                        
                        if (ExecuteSQL(db, sql_Insert) == -1)
                            return -1;
                    }
                }
                break;
            }
            case 'D':
            case 'd':
            {
                // Display DailyTrades Table
                cout << "Retrieving Data from DailyTrades Table..." << endl;
                const char* sql_Select = "SELECT * FROM DailyTrades;";
                if (ShowTable(db, sql_Select) == -1)
                    return -1;
                
                // Display IntradayTrades Table
                cout << "Retrieving Data from IntradayTrades Table..." << endl;
                sql_Select = "SELECT * FROM IntradayTrades;";
                if (ShowTable(db, sql_Select) == -1)
                    return -1;
                
                break;
            }
            case 'X':
            case 'x':
            {
                bCompleted = true;
                break;
            }
        }
    }
    cout << " Exiting MarketDataDB.db... " << endl;
    CloseDatabase(db);
    
    return 0;
}
