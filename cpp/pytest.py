results = [[0 for _ in range(5)] for _ in range(5)]
A2=[0,1]
A1=[0,1]
base = [0,1]
A1 = base[:]
for _0 in range(5):
    for i in range(1):
        A2[i]+=A2[i+1]
    A1[0]=A2[0]

    A1 = base[:]
    for _1 in range(5):
        results[_0][_1] = A1[0]
        for i in range(1):
            A1[i]+=A1[i+1]
print(results)
for line in results:
    print(" ".join(list(map(str,results))))