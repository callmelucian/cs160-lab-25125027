import matplotlib.pyplot as plt

s = "134 76 72 47 60 54 27 63 16 17 41 42 8 35 47 24 14 11 9 22 27 19 20 6 10 0 12 0 16 2 8 4 26 2 0 8 6 4 4 6 8 4 11 16 8 21 2 2 4 9 0 0 8 0 2 0 2 4 10 0 4 4 4 4 0 0 0 4 0 0 9 5 0 0 2 0 4 2 0 0 0 0 2 0 0 0 4 2 0 8 0 0 4 0 0 11 0 0 0 0 8 21 4 0 0 10 0 0 0 0 0 4 0 0 0 0 0 16 2 4 0 0 0 0 0 0 0 0 0 0 0 4 0 0 0 0 2 0 0 0 0 0 0 0 8 4 2 0 0 0 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 9 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 4 0 16 0 2 0 0 0 0 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 17 0 0 0 0 0 0 0 0 0 9"
freq = [int(x) for x in s.split()]
index = range(len(freq))

plt.bar(index, freq, color = 'orange')
plt.title('Analyzing the number of loops within a Dijkstra call')
plt.xlabel('Number of loops on within a Dijkstra call')
plt.ylabel('Frequency')
plt.show()