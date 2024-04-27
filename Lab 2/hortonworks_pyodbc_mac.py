# -*- coding: utf-8 -*-
"""
Sofia Guo Lab 2
Remeber to change IP address "Host=" each time when running! 
Changed SQL command to download 
pull IBM yearly dividend from yearly_aggregates table for the year 1998
"""

import pyodbc

pyodbc.autocommit = True
conn = pyodbc.connect("DSN=Cloudera Hive DSN; Host=184.73.142.66; Port=10000;", autocommit=True)

cursor = conn.cursor();

# cursor.execute("select symbol,trade_date as dividend_date, dividend from default.dividends_data where symbol = 'IBM' and year(trade_date) = '1990'")
cursor.execute("select year, symbol, total_dividends as dividend from default.yearly_aggregates where symbol = 'IBM' and year = '1998' ") 

result = cursor.fetchall() 
for r in result:
    print(r)

'''
('1998', 'IBM', 0.4300000071525574)
'''