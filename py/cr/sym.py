from math import sin, factorial, cos, tan, exp
from cr import * 

class Expression:
    def __init__(self, subexp = None, environment = None):
        self.environment = {} if environment is None else environment
        self.subexp = subexp

    # Evaluate subexpression
    def evaluate(self):
        if self.subexp is None:
            return 0
        return self.subexp.evaluate()
    
    def crmake(self,start,step):
        return self.subexp.crmake(start,step)


    # TODO: implement simplification algorithm 
    # Can assume that given expression is simplified.
    # CRMAKE implicitly simplifies?
    def simplify(self): 
        if self.subexp is None:
            return
        self.simplify(self.subexp)

    # remap environment variables to different configuration
    def remap(self, remapping):
        for k,v in remapping.items():
            self.environment[k] = v
        self.subexp.remap(remapping)

    def contextualize(self,mapping):
        self.environment = mapping
        if self.subexp is None:
            return
        # mutable environment assignment
        self.subexp.contextualize(mapping)

    def __repr__(self):
        return f"Exp({repr(self.subexp)})"
    
    # WIP: Add tree representation
    # WIP: Fix inheritence logic, some bloat here
    def hierarchize(self, prefix = '', terminal = True):
        sublabel = ""
        if self.subexp is not None:
            sublabel += '\n' +self.subexp.hierarchize(prefix + ("    " if terminal else "│   "),True)
        return f"{prefix + ('└── ' if terminal else '├── ')}{self.label()}{sublabel}"
    
    def label(self):
        return self.__class__.__name__
    


# Atomic subexpressions have no child subexpresisons 
class Atomic(Expression):
    def __init__(self):
        super().__init__()


class Symbolic(Atomic): 
    def __init__(self,name):
        super().__init__()
        self.name = name
    

    def evaluate(self):
        if not self.name in self.environment:
            print(f'{self.name} undefined')
        return self.environment.get(self.name,0)

    def crmake(self,start, step):
        return CRpuresum((start,step))

    
    def label(self):
        return f'{self.__class__.__name__}({self.name})'
    

class Numeric(Atomic):
    def __init__(self,value):
        super().__init__()
        self.value = value
    
    def evaluate(self):
        return self.value

    def crmake(self,start,step):
        return self.value

    
    def label(self):
        return f'{self.__class__.__name__}({self.value})'
    
    def __repr__(self):
        return f"Num({self.value})"
    

class Operator(Expression):
    def __init__(self, subexp):
        super().__init__(subexp= subexp)
    
class Unary(Operator):
    def __init__(self, subexp):
        super().__init__(subexp = subexp)
    
class Factorial(Unary):
    def __init__(self, subexp):
        super().__init__(subexp)
    
    def label(self):
        return "Fact"
    
    def evaluate(self):
        return factorial(self.subexp)

class Negative(Unary):
    def evaluate(self):
        return -1 * self.subexp.evaluate()
    
    def label(self):
        return "Neg"
    
    def crmake(self, start, step):
        right = self.subexp.crmake(start, step)
        left = -1.0
        return right.multo(left)
    
class Sinusoid(Unary):
    def evaluate(self):
        return sin(self.subexp.evaluate())
    
    def label(self):
        return "Sin"
    
    def crmake(self, start, step):
        right = self.subexp.crmake(start, step)
        return right.trig("sin")
    

class Cosinusoid(Unary):
    def evaluate(self):
        return cos(self.subexp.evaluate())
    
    def label(self):
        return "Cos"
    
    def crmake(self, start, step):
        right = self.subexp.crmake(start, step)
        return right.trig('cos')
    
class Tangent(Unary):
    def evaluate(self):
        return tan(self.subexp.evaluate())
    
    def label(self):
        return "Tan"
    
    def crmake(self, start, step):
        right = self.subexp.crmake(start, step)
        return right.trig('tan')
    
class Exponential(Unary):
    def evaluate(self):
        return exp(self.subexp.evaluate())
    
    def label(self):
        return "Exp"
    
    def crmake(self, start, step):
        right = self.subexp.crmake(start, step)
        return right.crexp()

class Binary(Operator):
    def __init__(self, leftexp, rightexp):
        super().__init__(None)
        self.leftexp = leftexp
        self.rightexp = rightexp
    
    def contextualize(self, mapping):
        # should not happen 
        if not self.leftexp and not self.rightexp:
            return
        
        # contextualize left and right subexpressions
        self.leftexp.contextualize(mapping)
        self.rightexp.contextualize(mapping)
    
    def remap(self, remapping):
        for k,v in remapping.items():
            self.environment[k] = v
        self.rightexp.remap(remapping)
        self.leftexp.remap(remapping)
    
    def hierarchize(self, prefix='', terminal=True):
        label = prefix + ("└── " if terminal else "├── ") + self.label()
        prefix += ("    " if terminal else "│   ")
        # right label will always be terminal
        leftlabel = self.leftexp.hierarchize(prefix, False)
        rightlabel = self.rightexp.hierarchize(prefix, True)
        return f'{label}\n{leftlabel}\n{rightlabel}'

    def label():
        return 'Bin'
    

class Summation(Binary):
    def evaluate(self): 
        return  self.rightexp.evaluate() + self.leftexp.evaluate()
    
    def label(self):
        return 'Add'

    def crmake(self, start, step):
        
        right = self.rightexp.crmake(start,step)
        left = self.leftexp.crmake(start,step)

        if isinstance(left,(float,int)):
            return right.addto(left)
        return left.addto(right)
    
class Multiplication(Binary):
    def evaluate(self):
        return self.rightexp.evaluate() * self.leftexp.evaluate()
    
    def label(self):
        return 'Mul'

    def crmake(self, start, step):
        right = self.rightexp.crmake(start,step)

        left = self.leftexp.crmake(start,step)

        if isinstance(left,(float,int)):
            return right.multo(left)
        return left.multo(right)

class Exponentiate(Binary):
    def evaluate(self):
        return self.leftexp.evaluate() ** self.rightexp.evaluate()
    
    def label(self):
        return 'Pow'
    
    def crmake(self, start, step):
        right = self.rightexp.crmake(start,step)
        left = self.leftexp.crmake(start,step)
        if isinstance(left,(float,int)):
            return right.rpowto(left)
        return left.powto(right)
        
    
class Division(Binary):
    def evaluate(self):
        return self.leftexp.evaluate() / self.rightexp.evaluate()

    def label(self):
        return 'Div'
    
    def crmake(self, start, step):
        right = self.rightexp.crmake(start,step)
        left = self.leftexp.crmake(start,step).expto(-1)

        # if right.index > left.index:
        #     return right.multo(left)
        return left.multo(right)

