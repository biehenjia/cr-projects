import time
import numpy as np
import numexpr as ne

# 1. Prepare your data
N = 10**7
x = np.linspace(0.0, 1.0, N, dtype=np.float64)

# 2. The expression you want to evaluate
expr = "x**5 + 3*x"

# 3. Warm up (first call pays some setup cost)
_ = ne.evaluate(expr, local_dict={"x": x})

# 4. Now do it repeatedly in a loop
start = time.perf_counter()

for i in range(1000000):
    y = ne.evaluate(expr, local_dict={"x": x})

print(f"Iteration {i+1}: {start-time.perf_counter()*1000:.2f} ms")