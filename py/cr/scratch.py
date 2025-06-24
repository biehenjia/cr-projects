import parsing

expr = input('Enter the expr ')
start = int(input('Enter the start '))
step = int(input('Enter the step '))
count = int(input('Enter the count '))


p = parsing.Parser(expr).parse().crmake(start,step)
p.initialize()

p.initialize()
print(p.operands)
print(p)

for i in range(count):
    print(f'{p.valueof():.5f}')
    p.shift()