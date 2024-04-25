# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import pyodbc
print('Done')

pyodbc.autocommit = True
conn = pyodbc.connect("DSN=Native Hortonworks Hive DSN; Host=184.73.122.68; Port=10000;", autocommit=True)

cursor = conn.cursor();

cursor.execute("select symbol,trade_date as dividend_date, dividend from default.dividends_data where symbol = 'IBM' and year(trade_date) = '1990'")

result = cursor.fetchall() 
for r in result:
    print(r)
