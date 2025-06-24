import subprocess

result = subprocess.run(
    ["./cmcr.exe", 'x,0,0.01,100 y,0,0.01,100 ;','1.3*x^3*y^2+2.5*x^2*y-1.5' ],
    capture_output=True,
    text=True,
    check=True
)

print("OUTPUT:", result.stdout)