results = [0 for _ in range(5)]
A1=[2,4,2]
base = [2,4,2]
A1 = base[:]
for _0 in range(5):
    results[_0] = A1[0]
    for i in range(2):
        A1[i]+=A1[i+1]
print(results)