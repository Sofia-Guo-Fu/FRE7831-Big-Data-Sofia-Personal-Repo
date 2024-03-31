#include <stdio.h>

#include <json/json.h>
#include <curl/curl.h>
#include <sqlite3.h>

#include "Stock.h"
#include "Util.h"
#include "MarketData.h"

int main(void)
{
	// If you are using Xcode, use config_mac.csv for the following line
	string sConfigFile = "config.csv";
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
	
	return 0;
}

/*

Sucess parsing json
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
				"low" : 404.35000000000002,
				"open" : 407.95999999999998,
				"volume" : 17224588
		}
]
Symbol: MSFT
Date: 2024-03-01 Open: 411.27 High: 415.87 Low: 410.88 Close: 415.5 Adjusted_Close: 415.5 Volume: 17800300
Date: 2024-03-04 Open: 413.44 High: 417.35 Low: 412.32 Close: 414.92 Adjusted_Close: 414.92 Volume: 17596000
Date: 2024-03-05 Open: 413.96 High: 414.25 Low: 400.64 Close: 402.65 Adjusted_Close: 402.65 Volume: 26919200
Date: 2024-03-06 Open: 402.97 High: 405.16 Low: 398.39 Close: 402.09 Adjusted_Close: 402.09 Volume: 22344100
Date: 2024-03-07 Open: 406.12 High: 409.78 Low: 402.24 Close: 409.14 Adjusted_Close: 409.14 Volume: 18718500
Date: 2024-03-08 Open: 407.96 High: 410.42 Low: 404.35 Close: 406.22 Adjusted_Close: 406.22 Volume: 17224588

*/