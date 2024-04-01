#include <stdio.h>

#include <json/json.h>
#include <curl/curl.h>
#include <sqlite3.h>

#include "Stock.h"
#include "Util.h"
#include "MarketData.h"
#include "Database.h"

int main(void)
{
	// If you are using Xcode, use config_mac.csv for the following line
	string sConfigFile = "config_mac.csv";
	map<string, string> configMap = ProcessConfigData(sConfigFile);
	string url_common = configMap["url_common"];
	string start_date = configMap["start_date"];
	string end_date = configMap["end_date"];
	string api_token = configMap["api_token"];
	string symbol = configMap["symbol"];
	string urlRequest = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
	cout << urlRequest << endl;
	
	string readBuffer;

	int runStatus = 0;

	runStatus = RetrieveMarketData(urlRequest, readBuffer);
	if (runStatus != 0)
	{
		cout << "Failed in retrieving market data!" << endl;
		return -1;
	}

	Stock myStock(symbol);
	runStatus = PopulateDailyTrades(readBuffer, myStock);
	if (runStatus != 0)
	{
		cout << "Failed in parsing market data!" << endl;
		return -1;
	}
	cout << myStock;
	
	string database_name = "Stocks.db";
	cout << "Opening Stocks.db ..." << endl;
	sqlite3* db = NULL;
	if (OpenDatabase(database_name.c_str(), db) == -1)
		return -1;

	// Drop the table if exists
	cout << "Drop DailyTrades table if exists" << endl;
	string sql_DropaTable = "DROP TABLE IF EXISTS DailyTrades";
	if (DropTable(db, sql_DropaTable.c_str()) == -1)
		return -1;

	// Create the table
	cout << "Creating DailyTrades table ..." << endl;
	const char* sql_CreateTable = "CREATE TABLE DailyTrades (" \
		"symbol CHAR(20) NOT NULL," \
		"date CHAR(20) NOT NULL," \
		"open REAL NOT NULL," \
		"high REAL NOT NULL," \
		"low REAL NOT NULL," \
		"close REAL NOT NULL," \
		"adjusted_close REAL NOT NULL," \
		"volume INT NOT NULL," \
		"PRIMARY KEY(symbol, date));";

	if (ExecuteSQL(db, sql_CreateTable) == -1)
		return -1;

	vector<DailyTrade> dailyTrades = myStock.getDailyTrade();
	for (auto itr = dailyTrades.begin(); itr != dailyTrades.end(); itr++)
	{
		cout << "Inserting daily data for a stock into table DailyTrades ..." << endl << endl;
		char sql_Insert[512];

		sprintf(sql_Insert, "INSERT INTO DailyTrades(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", 
			symbol.c_str(), itr->getDate().c_str(), itr->getOpen(), itr->getHigh(), itr->getLow(), itr->getClose(), itr->getAdjustedClose(), itr->getVolume());
		if (ExecuteSQL(db, sql_Insert) == -1)
			return -1;
	}

	// Display DailyTrades Table
	cout << "Retrieving values in table DailyTrades ..." << endl;
	const char* sql_Select = "SELECT * FROM DailyTrades;";
	if (ShowTable(db, sql_Select) == -1)
		return -1;

	return 0;
}
/*
mt=json
[{"date":"2024-03-01","open":411.27,"high":415.87,"low":410.88,"close":415.5,"adjusted_close":415.5,"volume":17800300},{"date":"2024-03-04","open":413.44,"high":417.35,"low":412.32,"close":414.92,"adjusted_close":414.92,"volume":17596000},{"date":"2024-03-05","open":413.96,"high":414.25,"low":400.64,"close":402.65,"adjusted_close":402.65,"volume":26919200},{"date":"2024-03-06","open":402.97,"high":405.16,"low":398.39,"close":402.09,"adjusted_close":402.09,"volume":22344100},{"date":"2024-03-07","open":406.12,"high":409.78,"low":402.24,"close":409.14,"adjusted_close":409.14,"volume":18718500},{"date":"2024-03-08","open":407.96,"high":410.42,"low":404.33,"close":406.22,"adjusted_close":406.22,"volume":17971700},{"date":"2024-03-11","open":403.76,"high":405.68,"low":401.26,"close":404.52,"adjusted_close":404.52,"volume":16120800},{"date":"2024-03-12","open":407.62,"high":415.57,"low":406.79,"close":415.28,"adjusted_close":415.28,"volume":22457000},{"date":"2024-03-13","open":418.1,"high":418.18,"low":411.45,"close":415.1,"adjusted_close":415.1,"volume":17115900},{"date":"2024-03-14","open":420.24,"high":427.82,"low":417.99,"close":425.22,"adjusted_close":425.22,"volume":34157300},{"date":"2024-03-15","open":419.29,"high":422.6,"low":412.79,"close":416.42,"adjusted_close":416.42,"volume":45049800},{"date":"2024-03-18","open":414.25,"high":420.73,"low":413.78,"close":417.32,"adjusted_close":417.32,"volume":20106000},{"date":"2024-03-19","open":417.83,"high":421.67,"low":415.55,"close":421.41,"adjusted_close":421.41,"volume":19837900},{"date":"2024-03-20","open":422,"high":425.96,"low":420.66,"close":425.23,"adjusted_close":425.23,"volume":17860100},{"date":"2024-03-21","open":429.83,"high":430.82,"low":427.16,"close":429.37,"adjusted_close":429.37,"volume":21296200},{"date":"2024-03-22","open":429.7,"high":429.86,"low":426.07,"close":428.74,"adjusted_close":428.74,"volume":17636500}]

Succeed in parsing json
[
        {
                "adjusted_close" : 415.5,
                "close" : 415.5,
                "date" : "2024-03-01",
                "high" : 415.87,
                "low" : 410.88,
                "open" : 411.26999999999998,
                "volume" : 17800300
        },
        {
                "adjusted_close" : 414.92000000000002,
                "close" : 414.92000000000002,
                "date" : "2024-03-04",
                "high" : 417.35000000000002,
                "low" : 412.31999999999999,
                "open" : 413.44,
                "volume" : 17596000
        },
        {
                "adjusted_close" : 402.64999999999998,
                "close" : 402.64999999999998,
                "date" : "2024-03-05",
                "high" : 414.25,
                "low" : 400.63999999999999,
                "open" : 413.95999999999998,
                "volume" : 26919200
        },
        {
                "adjusted_close" : 402.08999999999997,
                "close" : 402.08999999999997,
                "date" : "2024-03-06",
                "high" : 405.16000000000003,
                "low" : 398.38999999999999,
                "open" : 402.97000000000003,
                "volume" : 22344100
        },
        {
                "adjusted_close" : 409.13999999999999,
                "close" : 409.13999999999999,
                "date" : "2024-03-07",
                "high" : 409.77999999999997,
                "low" : 402.24000000000001,
                "open" : 406.12,
                "volume" : 18718500
        },
        {
                "adjusted_close" : 406.22000000000003,
                "close" : 406.22000000000003,
                "date" : "2024-03-08",
                "high" : 410.42000000000002,
                "low" : 404.32999999999998,
                "open" : 407.95999999999998,
                "volume" : 17971700
        },
        {
                "adjusted_close" : 404.51999999999998,
                "close" : 404.51999999999998,
                "date" : "2024-03-11",
                "high" : 405.68000000000001,
                "low" : 401.25999999999999,
                "open" : 403.75999999999999,
                "volume" : 16120800
        },
        {
                "adjusted_close" : 415.27999999999997,
                "close" : 415.27999999999997,
                "date" : "2024-03-12",
                "high" : 415.56999999999999,
                "low" : 406.79000000000002,
                "open" : 407.62,
                "volume" : 22457000
        },
        {
                "adjusted_close" : 415.10000000000002,
                "close" : 415.10000000000002,
                "date" : "2024-03-13",
                "high" : 418.18000000000001,
                "low" : 411.44999999999999,
                "open" : 418.10000000000002,
                "volume" : 17115900
        },
        {
                "adjusted_close" : 425.22000000000003,
                "close" : 425.22000000000003,
                "date" : "2024-03-14",
                "high" : 427.81999999999999,
                "low" : 417.99000000000001,
                "open" : 420.24000000000001,
                "volume" : 34157300
        },
        {
                "adjusted_close" : 416.42000000000002,
                "close" : 416.42000000000002,
                "date" : "2024-03-15",
                "high" : 422.60000000000002,
                "low" : 412.79000000000002,
                "open" : 419.29000000000002,
                "volume" : 45049800
        },
        {
                "adjusted_close" : 417.31999999999999,
                "close" : 417.31999999999999,
                "date" : "2024-03-18",
                "high" : 420.73000000000002,
                "low" : 413.77999999999997,
                "open" : 414.25,
                "volume" : 20106000
        },
        {
                "adjusted_close" : 421.41000000000003,
                "close" : 421.41000000000003,
                "date" : "2024-03-19",
                "high" : 421.67000000000002,
                "low" : 415.55000000000001,
                "open" : 417.82999999999998,
                "volume" : 19837900
        },
        {
                "adjusted_close" : 425.23000000000002,
                "close" : 425.23000000000002,
                "date" : "2024-03-20",
                "high" : 425.95999999999998,
                "low" : 420.66000000000003,
                "open" : 422,
                "volume" : 17860100
        },
        {
                "adjusted_close" : 429.37,
                "close" : 429.37,
                "date" : "2024-03-21",
                "high" : 430.81999999999999,
                "low" : 427.16000000000003,
                "open" : 429.82999999999998,
                "volume" : 21296200
        },
        {
                "adjusted_close" : 428.74000000000001,
                "close" : 428.74000000000001,
                "date" : "2024-03-22",
                "high" : 429.86000000000001,
                "low" : 426.06999999999999,
                "open" : 429.69999999999999,
                "volume" : 17636500
        }
]
Symbol: MSFT
Date: 2024-03-01 Open: 411.27 High: 415.87 Low: 410.88 Close: 415.5 Adjusted_Close: 415.5 Volume: 17800300
Date: 2024-03-04 Open: 413.44 High: 417.35 Low: 412.32 Close: 414.92 Adjusted_Close: 414.92 Volume: 17596000
Date: 2024-03-05 Open: 413.96 High: 414.25 Low: 400.64 Close: 402.65 Adjusted_Close: 402.65 Volume: 26919200
Date: 2024-03-06 Open: 402.97 High: 405.16 Low: 398.39 Close: 402.09 Adjusted_Close: 402.09 Volume: 22344100
Date: 2024-03-07 Open: 406.12 High: 409.78 Low: 402.24 Close: 409.14 Adjusted_Close: 409.14 Volume: 18718500
Date: 2024-03-08 Open: 407.96 High: 410.42 Low: 404.33 Close: 406.22 Adjusted_Close: 406.22 Volume: 17971700
Date: 2024-03-11 Open: 403.76 High: 405.68 Low: 401.26 Close: 404.52 Adjusted_Close: 404.52 Volume: 16120800
Date: 2024-03-12 Open: 407.62 High: 415.57 Low: 406.79 Close: 415.28 Adjusted_Close: 415.28 Volume: 22457000
Date: 2024-03-13 Open: 418.1 High: 418.18 Low: 411.45 Close: 415.1 Adjusted_Close: 415.1 Volume: 17115900
Date: 2024-03-14 Open: 420.24 High: 427.82 Low: 417.99 Close: 425.22 Adjusted_Close: 425.22 Volume: 34157300
Date: 2024-03-15 Open: 419.29 High: 422.6 Low: 412.79 Close: 416.42 Adjusted_Close: 416.42 Volume: 45049800
Date: 2024-03-18 Open: 414.25 High: 420.73 Low: 413.78 Close: 417.32 Adjusted_Close: 417.32 Volume: 20106000
Date: 2024-03-19 Open: 417.83 High: 421.67 Low: 415.55 Close: 421.41 Adjusted_Close: 421.41 Volume: 19837900
Date: 2024-03-20 Open: 422 High: 425.96 Low: 420.66 Close: 425.23 Adjusted_Close: 425.23 Volume: 17860100
Date: 2024-03-21 Open: 429.83 High: 430.82 Low: 427.16 Close: 429.37 Adjusted_Close: 429.37 Volume: 21296200
Date: 2024-03-22 Open: 429.7 High: 429.86 Low: 426.07 Close: 428.74 Adjusted_Close: 428.74 Volume: 17636500
Opening Stocks.db ...
Opened Stocks.db
Drop DailyTrades table if exists
Creating DailyTrades table ...
Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Inserting daily data for a stock into table DailyTrades ...

Retrieving values in table DailyTrades ...
symbol       date         open         high         low          close        adjusted_close volume
~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~
MSFT         2024-03-01   411.269989   415.869995   410.880005   415.5        415.5        17800300
MSFT         2024-03-04   413.440002   417.350006   412.320007   414.920013   414.920013   17596000
MSFT         2024-03-05   413.959991   414.25       400.640015   402.649994   402.649994   26919200
MSFT         2024-03-06   402.970001   405.160004   398.390015   402.089996   402.089996   22344100
MSFT         2024-03-07   406.119995   409.779999   402.23999    409.140015   409.140015   18718500
MSFT         2024-03-08   407.959991   410.420013   404.329987   406.220001   406.220001   17971700
MSFT         2024-03-11   403.76001    405.679993   401.26001    404.519989   404.519989   16120800
MSFT         2024-03-12   407.619995   415.570007   406.790009   415.279999   415.279999   22457000
MSFT         2024-03-13   418.100006   418.179993   411.450012   415.100006   415.100006   17115900
MSFT         2024-03-14   420.23999    427.820007   417.98999    425.220001   425.220001   34157300
MSFT         2024-03-15   419.290009   422.600006   412.790009   416.420013   416.420013   45049800
MSFT         2024-03-18   414.25       420.730011   413.779999   417.320007   417.320007   20106000
MSFT         2024-03-19   417.829987   421.670013   415.549988   421.410004   421.410004   19837900
MSFT         2024-03-20   422.0        425.959991   420.660004   425.230011   425.230011   17860100
MSFT         2024-03-21   429.829987   430.820007   427.160004   429.369995   429.369995   21296200
MSFT         2024-03-22   429.700012   429.859985   426.070007   428.73999    428.73999    17636500

*/
