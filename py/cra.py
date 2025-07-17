x = [0,0,0]
for i in range(10):
    y = x[:]
    for j in range(10):
        z = y[:] 
        for k in range(10):
            print(z)
            
            z[2] += 1
        y[1] += 1
    x[0] += 1
