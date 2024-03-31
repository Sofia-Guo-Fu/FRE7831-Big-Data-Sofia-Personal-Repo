#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream> // header for file stream operations
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
	~DailyTrade() {}
	friend ostream& operator << (ostream& out, const DailyTrade& t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};

// --------------------- added class: IntradayTrade  --------------------------
class IntradayTrade
{
private:
    long timestamp;
    string datetime;
    float open;
    float high;
    float low;
    float close;
    int volume;
public:
    IntradayTrade(long timestamp_, string datetime_, float open_, float high_, float low_, float close_, int volume_):timestamp(timestamp_), datetime(datetime_), open(open_), high(high_), low(low_), close(close_), volume(volume_){}
    ~IntradayTrade(){}
    friend ostream& operator << (ostream& out, const IntradayTrade& t)
    {
        out << " Datetime: "<<t.datetime<<" Open: "<<t.open<<" High: "<<t.high<<" Low: "<<t.low<<" Close: "<<t.close<<" Volume: "<<t.volume<<endl;
        return out;
    }
    
    friend ofstream& operator << (ofstream& fout, const IntradayTrade& t)
    {
        fout<<" Datetime: "<<t.datetime<<" Open: "<<t.open<<" High: "<<t.high<<" Low: "<<t.low<<" Close: "<<t.close<<" Volume: "<<t.volume<<endl;
        return fout;
    }
};



class Stock
{
private:
	string symbol;
	vector<DailyTrade> trades;
    // added data member
    vector<IntradayTrade> intradayTrades;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addDailyTrade(DailyTrade aTrade)
	{
		trades.push_back(aTrade);
	}
    
    // added
    void addIntraTrade(IntradayTrade aTrade)
    {
        intradayTrades.push_back(aTrade); 
    }
    
	friend ostream& operator << (ostream& out, const Stock& s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<DailyTrade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
        
         //added: print out vector<IntradayTrade> intradayTrades;
        for (vector<IntradayTrade>::const_iterator itr = s.intradayTrades.begin(); itr != s.intradayTrades.end(); itr++)
            out << *itr;
		return out;
	}
    
    
};
