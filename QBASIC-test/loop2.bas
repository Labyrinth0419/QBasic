1 LET a = 50
2 LET b = 2
3 LET c = 9
4 LET d = a * b - c
5 IF d > 0 THEN 7
6 GOTO 10
7 PRINT d
8 LET c = c + 1
9 GOTO 4
10 PRINT c
11 END