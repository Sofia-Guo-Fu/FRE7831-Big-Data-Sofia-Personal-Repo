# -*- coding: utf-8 -*-
"""
Sofia Guo Quizz 2
Remeber to change IP address "Host=" each time when running! 
"""

import pyodbc
import sqlite3 


# question 1: From the yearly_aggregates table, find out the total dividend for each symbol
pyodbc.autocommit = True
conn1 = pyodbc.connect("DSN=Cloudera Hive DSN; Host= 3.84.78.120; Port=10000;", autocommit=True)
cursor1 = conn1.cursor();

print("Task 1: Total Dividend for Each Symbol")
print("--------------------------------------")
cursor1.execute("""
    SELECT symbol as Symbol, SUM(total_dividends) AS Symbol_Total_Dividend
    FROM yearly_aggregates
    GROUP BY symbol
""")
total_dividends = cursor1.fetchall()

# only print out the first 5 rows to look 
for row in total_dividends[:5]:
    print(row)

# ('AA', 11.737260209396482)
# ('AAI', None)
# ('AAN', 0.46235001645982265)
# ('AAP', 0.9599999785423279)
# ('AAR', 20.660000503063202)

# question 2: Find out the average adjusted close price for each symbol from the DailyTrades table
print("Task 2: Average Adjusted Close Price for Each Symbol")
print("-----------------------------------------------------")
conn2 = sqlite3.connect('MarketData.db')
cursor2 = conn2.cursor()
cursor2.execute("""
    SELECT Symbol, AVG(Adjusted_close) AS Avg_Adjusted_Close_Price
    FROM DailyTrades
    GROUP BY Symbol
""")
avg_close_prices = cursor2.fetchall()

for r in avg_close_prices:
    print(r)

# ('GOOGL', 154.448751375)
# ('GS', 406.5335294117647)
# ('IBM', 188.98250000000002)
# ('MSFT', 422.9987525)
# ('TSLA', 163.01529411764707)

# question 3: Combine the above results based on symbols and print out the output
print(" Task 3: Combined Results")
print("-----------------------------------------------------")
total_dividends_dict = {row[0]: row[1] for row in total_dividends}
avg_close_dict = {row[0]: row[1] for row in avg_close_prices}

print("Symbol  Avg_Adjusted_Close_Price  Symbol_Total_Dividend")
for symbol in set(avg_close_dict.keys()) & set(total_dividends_dict.keys()):
    avg_close_price = avg_close_dict[symbol]
    total_dividend = total_dividends_dict[symbol]
    print(f"{symbol:<7} {avg_close_price:^27.6f} {total_dividend:^23.6f}")

conn1.close()
conn2.close()

# Symbol  Avg_Adjusted_Close_Price  Symbol_Total_Dividend
# IBM             188.982500                 31.912831       
# GS              406.533529                 10.157000  