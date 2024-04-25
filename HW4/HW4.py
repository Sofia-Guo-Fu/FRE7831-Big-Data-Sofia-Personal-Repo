# aggregated code in a .py script
# Author: Sofia Guo

import yfinance as yf 
import datetime as dt 
import pandas as pd 
import numpy as np 
import sqlite3

# Download data from yahoo finance
start_date = dt.date(2024,4,1)
end_date = dt.datetime.today().strftime("%Y-%m-%d")

# download daily market data for TSLA and GS
all_data = {}
for ticker in ['TSLA','GS']:
    all_data[ticker] = yf.download(ticker, start_date, end_date)

price = pd.DataFrame({symbol:data['Close'] for symbol, data in all_data.items()})
volume = pd.DataFrame({symbol: data['Volume'] for symbol, data in all_data.items()})


# put the downloaded data into a tuple list 
insert_list = []

for symbol in ['TSLA','GS']:
    data = all_data[symbol]
    for index, row in data.iterrows():
        index = index.strftime("%Y-%m-%d")
        data_tuple = (symbol, index) + tuple(round(element,4) for element in row)
        # print(data_tuple)
        insert_list.append(data_tuple)

# connect and insert data to database 'MarketData.db'
connection = sqlite3.connect("MarketData.db")
cursor = connection.cursor()

#If the record exists, it will be overwritten; if it does not yet exist, it will be created.
cursor.executemany("replace into DailyTrades Values(?,?,?,?,?,?,?,?)", insert_list)

connection.commit()
connection.close()

# Print out and check data in the DailyTrades table 
connection = sqlite3.connect("MarketData.db")
cursor = connection.cursor()

cursor.execute("SELECT * FROM  DailyTrades; ")

result = cursor.fetchall()
for r in result:
    print(r)


print('Done!')