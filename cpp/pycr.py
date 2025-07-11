import re
import json
from pathlib import Path
import pycrlib

# input a list of parameters
a = [("x,0,1,100"),("")]

# ll1 stuff
# TODO : Still need to learn more about parse table in general
p = Path(__file__).parent / "ll1.json"
with open(p) as f:
    ll1 = json.load(f)

terminals = set()
for x,y in ll1.items():
    terminals.update(y.keys())

terminals.add('EOF')

lex = {
    'NUMERIC' : r'(?:\d+\.\d*|\d+|\.\d+)',
    'SIN' : r'\bsin\b',
    'COS' : r'\bcos\b',
    'TAN' : r'\btan\b',
    'EXP' : r'\bexp\b',
    'SYMBOLIC' : r'[A-Za-z]\w*',
    'OPERATOR' : r'[\+\-\*/\^!\(\),]',
    'SKIP' : r'[ \t]+'
}

master = re.compile('|'.join(f"(?P<{group}>{pattern})" for group, pattern in lex.items()))

# lexing stuff
class Token:
    def __init__(self, kind, text, pos):
        self.kind = kind
        self.text = text
        self.pos = pos 
    
    def __repr__(self):
        return f"{self.kind} Token '{self.text}' @ {self.pos}"

def tokenize(src):
    tokens = []
    for m in master.finditer(src):
        kind = m.lastgroup
        text = m.group(kind)
        pos = m.start()
        if kind == 'SKIP':
            continue 
        if kind == 'OPERATOR':
            kind = text
        tokens.append(Token(kind, text, pos))
    tokens.append(Token('EOF', '', len(src)))
    return tokens

# ast stuff
def astadd(stack,text):
    right = stack.pop()
    left = stack.pop()
        
    result = pycrlib.ASTbin(pycrlib.bt.ADD, left, right)
    #result._children = (left, right)
    stack.append(result)
    #print(type(stack[-1]))
    result._children = (left, right)

def astsub(stack,text):
    right = stack.pop()
    left = stack.pop()
    result = pycrlib.ASTbin(pycrlib.bt.SUB, left, right)
    #result._children = (left, right)

    stack.append(result)
    result._children = (left, right)


def astmul(stack,text):
    right = stack.pop()
    left = stack.pop()
    result = pycrlib.ASTbin(pycrlib.bt.MUL, left, right)
    #result._children = (left, right)

    stack.append(result)
    result._children = (left, right)



def astdiv(stack,text):
    right = stack.pop()
    left = stack.pop()
    result = pycrlib.ASTbin(pycrlib.bt.DIV, left, right)
    #result._children = (left, right)

    stack.append(result)
    result._children = (left, right)



def astexp(stack,text):
    right = stack.pop()
    left = stack.pop()
    result = pycrlib.ASTbin(pycrlib.bt.POW, left, right)
    #result._children = (left, right)

    stack.append(result)
    #print(type(stack[-1]))
    result._children = (left, right)



def astneg(stack,text):
    pass

def astfac(stack,text):
    pass

def astsin(stack,text):
    pass

def astcos(stack,text):
    pass

def asttan(stack,text):
    pass

def astexponential(stack,text):
    pass

def astnum(stack,text):
    l = pycrlib.ASTnum(float(text))
    stack.append(l)

    #stack[-1].view()
    #print(type(stack[-1]))


def astsym(stack,text):
    l = pycrlib.ASTvar()
    stack.append(l)
    #print(type(stack[-1]))

# TODO: Modularize, strange code writing ahead...
functions = { 
    "#B+" : astadd,
    "#B-" : astsub,
    "#B*" : astmul,
    "#B/" : astdiv,
    "#B^" : astexp,
    "#U!" : astfac,
    "#U-" : astneg,
    "#N" : astnum, 
    "#SIN" : astsin,
    "#COS" : astcos,
    "#TAN" : asttan,
    "#E" : astexponential,
    "#S" : astsym
}


# simple LL1 parsing
class Parser:
    def __init__(self,src):
        self.tokens = tokenize(src)
        self.kinds = [token.kind for token in self.tokens]
        # add terminating EOF
        self.stack = ['EOF', 'E']
        self.pos = 0
        self.ast = []
    
    # TODO: fix parse table logic
    def parse(self): 
        while self.stack: 
            next = self.stack.pop()
            lookahead = self.kinds[self.pos]
            # ast stuff
            if next.startswith("#"): 
                # handle node construction
                text = None
                if next in ("#N", "#S", "#SIN", "#COS", "#TAN", "#E"):
                    text = self.tokens[self.pos-1].text
                functions[next](self.ast,text)
                continue
            # handle terminal 
            if next in terminals:
                if next != lookahead: 
                    # debugging; turn off
                    raise SyntaxError(f"EXPECTED {next} GOT {lookahead}!!!")
                self.pos +=1
                continue
            # nonterminal
            production = ll1[next].get(lookahead)
            if production is None:
                # debugging; turn off
                raise SyntaxError(f"NO RULE FOR {next} FOR {lookahead}")
            for symbol in reversed(production):
                self.stack.append(symbol)
        #print("Parsing done!")
        #self.ast[0].view()

        return self.ast[0]
def cleargarbage():
    garbage = []

def mtp(expr):
    expr = re.sub(r'\^', r'**', expr)
    return expr