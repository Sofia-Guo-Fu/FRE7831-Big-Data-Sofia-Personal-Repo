#include "json/json.h"
#include "curl/curl.h"

#include "Stock.h"
#include "MarketData.h"
#include "Util.h"

int RetrieveMarketData(const string& url_request, string& read_buffer)
{
	// global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	// creating session handle
	CURL* handle;

	// Store the result of CURL’s retrieval status
	CURLcode status;

	handle = curl_easy_init();

	if (!handle)
	{
		cout << "curl_easy_init failed" << endl;
		return -1;
	}

	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

	// adding a user agent
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

	// send all data to this function 
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// pass read buffer storage to the callback function 
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);

	// retrieve data
	status = curl_easy_perform(handle);

	// check for errors 
	if (status != CURLE_OK) {
		cout << "curl_easy_perform() failed:" << curl_easy_strerror(status) << endl;
		return -1;
	}

	cout << read_buffer << endl;

	curl_easy_cleanup(handle);

	return 0;
}

int PopulateDailyTrades(const string& read_buffer, Stock& stock)
{
	// json parsing
	Json::Reader reader;
	Json::Value root;   // will contains the root value after parsing.

	bool parsingSuccessful = reader.parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), root);
	if (!parsingSuccessful)
	{
		// report failures and their locations in the document.
		cout << "Failed to parse JSON: " << reader.getFormattedErrorMessages();;
		return -1;
	}

	std::cout << "\nSucceed in parsing json\n" << root << endl;
	string date;
	float open, high, low, close, adjusted_close;
	long int volume;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		date = (*itr)["date"].asString();	
		open = (*itr)["open"].asFloat();
		high = (*itr)["high"].asFloat();
		low = (*itr)["low"].asFloat();
		close = (*itr)["close"].asFloat();
		adjusted_close = (*itr)["adjusted_close"].asFloat();
		volume = (*itr)["volume"].asInt64();
		DailyTrade aTrade(date, open, high, low, close, adjusted_close, volume);
		stock.addDailyTrade(aTrade);
	}
	return 0;
}
