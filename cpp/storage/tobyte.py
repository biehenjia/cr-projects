import time

start = time.perf_counter()
for i in range(1000):
    pass 
print(time.perf_counter()-start)


start = time.perf_counter()
for i in range(1000):
    i**10+i**9+i**8+i**7
print(time.perf_counter()-start)