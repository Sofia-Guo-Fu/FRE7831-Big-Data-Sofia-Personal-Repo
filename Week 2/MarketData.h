#pragma once

#include <string>

class Stock;

int RetrieveMarketData(const std::string& url_request, std::string& read_buffer);  // Libcurl related
int PopulateDailyTrades(const std::string& read_buffer, Stock& stock);		// Parse JSON messages to populate daily trade vector for stock
