#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class DailyTrade
{
private:
	string date;
	float open;
	float high;
	float low;
	float close;
	float adjusted_close;
	long volume;
public:
	DailyTrade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, long volume_) :
		date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
	{}
	string getDate() { return date; }
	float getOpen() { return open; }
	float getHigh() { return high; }
	float getLow() { return low; }
	float getClose() { return close; }
	float getAdjustedClose() { return adjusted_close; }
	long getVolume() { return volume; }
	~DailyTrade() {}
	friend ostream& operator << (ostream& out, const DailyTrade& t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};

class Stock
{
private:
	string symbol;
	vector<DailyTrade> trades;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addDailyTrade(DailyTrade aTrade)
	{
		trades.push_back(aTrade);
	}
	vector<DailyTrade>& getDailyTrade()
	{
		return trades;
	}
	friend ostream& operator << (ostream& out, const Stock& s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<DailyTrade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		return out;
	}
};
