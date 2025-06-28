import math 
import numpy
import cProfile
import functools, time

compute = [0]

def timing(function):
    @functools.wraps(function)
    def wrapper(*args, **kwargs):
        t0 = time.perf_counter()
        res = function(*args, **kwargs)
        t1 = time.perf_counter()
        compute[0] += t1-t0
        return res
    return wrapper


class CRobject:

    def __init__(self):
        self.initialized = False 
        self.operands = None
        self.length = None
        self.fastvalues = None
        self.isnotnumbers = None
    
    def valueof(self):
        if self.fastvalues is not None:
            return self.fastvalues[0]
        return 0

    def selfsimplify(self):
        return self
    
    def initialize(self):
        fastvalues = numpy.zeros(self.length,dtype = float)
        isnotnumbers = numpy.zeros(self.length,dtype = int)

        #print(self.operands)
        for i in range(self.length):
            if isinstance(self.operands[i],(float,int)):
                fastvalues[i] = self.operands[i]
                isnotnumbers[i] = False
            else:
                fastvalues[i] = self.operands[i].initialize()
                isnotnumbers[i] = True
        
        self.initialized = True
        self.fastvalues = fastvalues
        self.isnotnumbers = isnotnumbers 
        return self.valueof()

    def correctP(self,arg, newlength):
        res = numpy.zeros(newlength,dtype = float)
        for i in range(len(arg)):
            res[i] = arg[i]
        for i in range(len(arg),newlength):
            res[i] = 1.0
        return res 

    def correctT(self, arg, newlength):
        res = numpy.zeros(2*newlength, dtype=float)
        for i in range(len(arg)):
            res[i] = arg[i].copy()
            res[i+newlength] = arg[i+len(arg)//2]
        
        for i in range(len(arg)//2,newlength):
            res[i] = 0.0
            res[i+newlength] = 1.0
        
        return res 

class CRexpression(CRobject):

    def __init__(self, optype, *args):
        super().__init__()
        self.length = len(args)
        self.optype = optype 
        self.operands = list(args)
    
    def copy(self):
        temperands = [0] * self.length
        rnew = CRexpression(self.optype)
        operands = self.operands
        for i in range(self.length):
            if isinstance(operands[i],CRobject):
                temperands[i] = operands[i].copy()
            else:
                temperands[i] = operands[i]
        
        if self.initialized:
            tempfastvalues = numpy.zeros(self.length, dtype = float)
            tempnotnumbers = numpy.zeros(self.length, dtype = int)

            for i in range(self.length):
                tempfastvalues[i] = self.fastvalues[i]
                tempnotnumbers[i] = self.isnotnumbers[i]
            
            rnew.initialized = True
            rnew.fastvalues = tempfastvalues 
            rnew.isnotnumbers = tempnotnumbers 
        rnew.operands = temperands
        return rnew 
    
    def addto(self, target):
        return CRexpression('add', self, target)
    
    def multo(self, target):
        return CRexpression('mul', self, target)

    def powto(self, target):
        return CRexpression('pow', self, target)

    @timing
    def shift(self):
        for i in range(self.length):
            if self.isnotnumbers[i]:
                self.operands[i].shift()
                self.fastvalues[i] = self.operands[i].valueof()

    def valueof(self):

        #print(self.fastvalues,'hi')

        match self.optype:

            case 'add':
                return self.fastvalues[0] + (self.fastvalues[1])
            
            case 'mul':
                return self.fastvalues[0] * (self.fastvalues[1])
            
            case 'pow':
                return self.fastvalues[0] ** (self.fastvalues[1])
            
            case 'sin':
                return math.sin(self.fastvalues[0])
    
    def trig(self,optype):
        return CRexpression(optype, self)

class CRpuresum(CRobject):
    
    def __init__(self,startstep=None):
        # optional argument for startstep = (start,step)
        super().__init__()
        if not (startstep is None):
            start,step = startstep
            self.operands = numpy.zeros(2, dtype = float)
            self.operands[0] = start
            self.operands[1] = step
            self.length = 2
        
    def copy(self):
        rnew = CRpuresum()
        rnew.length = self.length 
        # puresum
        temperands = numpy.zeros(self.length, dtype = float)
        
        for i in range(self.length):
            temperands[i] = self.operands[i]
        
        if self.initialized:
            tempfastvalues = numpy.zeros(self.length,dtype= float)
            tempisnotnumbers = numpy.zeros(self.length,dtype = int)

            for i in range(self.length):
                tempfastvalues[i] = self.fastvalues[i]
                tempisnotnumbers[i] = self.isnotnumbers[i]
            
            rnew.fastvalues = tempfastvalues
            rnew.isnotnumbers = tempisnotnumbers 
        rnew.initialized = self.initialized
        rnew.operands = temperands
        return rnew 

    def addto(self, target):
        result = self.copy()
        # treat as constant
        if isinstance(target, (float,int)):
            temperand = self.operands[0]
            result.operands[0] += target
            return result.selfsimplify()

        if isinstance(target, CRpuresum):
            if target.length > self.length:
                newlength = target.length
                minlength = self.length
                rarray = numpy.array(target.operands,copy=True) 
            else:
                newlength = self.length 
                minlength = target.length 
                rarray = numpy.array(self.operands,copy=True)
            
            for i in range(minlength):
                rarray[i] = self.operands[i] + target.operands[i]
            result.operands = rarray
            result.length = newlength
            return result.selfsimplify()

        else:
            return CRexpression('add', self, target)
    
    def multo(self, target):
        #print('here')
        result = self.copy()
        
        if isinstance(target, (float,int)):

            for i in range(self.length):
                result.operands[i] = result.operands[i] * target
            return result.selfsimplify()

        if isinstance(target,CRpuresum):
            #print('here')
            #crprod algorithm
            if self.length >= target.length:
                newlength = self.length + target.length - 1

                resarray = numpy.zeros(newlength, dtype = float)
                n = self.length - 1
                m = target.length - 1
                for i in range(newlength):
                    r1 = 0.0
                    ibound11 = max(0,i-m)
                    ibound12 = min(i,n)
                    for j in range(ibound11,ibound12+1):
                        r2 = 0.0
                        ibound21 = i-j
                        ibound22 = min(i,m)
                        for k in range(ibound21, ibound22 + 1):
                            rtemp1 = math.comb(j,i-k)
                            rtemp11 = target.operands[k]
                            rtemp1 *= rtemp11

                            r2 += rtemp1 
                        rtemp2 = math.comb(i,j)
                        r2 *= rtemp2
                        # numpy supports copy
                        roperand = result.operands[j]
                        

                        rtemp2 *= roperand
                        r1 += rtemp2
                    resarray[i] = r1
                result.operands = resarray
                result.length = newlength 

                return result.selfsimplify()
            else:
                return target.multo(self).selfsimplify()
        else:
            return CRexpression('mul',self, target).selfsimplify()
    
    def powto(self, target): 
        result = self.copy()
        
        if isinstance(target,(float,int)):
            if int(target) == target:
                ipower = int(target)
                abspower = abs(ipower)

                rmult = self.copy()

                for i in range(1,abspower): 
                    result = result.multo(rmult)
                if ipower < 0:
                    rres = CRexpression('div',1,result )
                    return rres 
                
                return result.selfsimplify()
        else:
            return CRexpression('pow', self, target).selfsimplify()
    
    def trig(self, trigtype):
        # we know it is puresum (pure floats)
        temperands = numpy.zeros(self.length*2, dtype= float)
        for i in range(self.length):
            temperands[i] = math.sin(self.operands[i])
            temperands[i+self.length] = math.cos(self.operands[i])
        res = CRtrigonometry(trigtype)
        res.operands = temperands
        res.length = self.length * 2
        return res.selfsimplify()
    
    @timing
    def shift(self):
        self.fastvalues[:-1] += self.fastvalues[1:]
        #for i in range(self.length-1):
            
            #self.fastvalues[i] = self.fastvalues[i] + self.fastvalues[i+1]

    def selfsimplify(self):
        j = self.length - 1

        while j > 0 and (isinstance(self.operands[j],float)) and (self.operands[j] == 0):
            j -= 1 
        
        if j == 0:
            res = self.operands[0]
        
        else:
            temp = numpy.zeros(j+1,dtype = float)
            for i in range(j+1):
                temp[i] = self.operands[i]
            
            self.operands = temp 
            self.length = j + 1 
            res = self 
        return res

    # reverse power case
    def rpowto(self, target):
        temperands = numpy.zeros(self.length)
        for i in range(self.length):
            temperands[i] = target ** self.operands[i]
        result = CRpureprod()
        result.operands=  temperands
        result.length = self.length
        return result

class CRpureprod(CRobject):

    def __init__(self):
        super().__init__()
    
    def copy(self):
        temp = numpy.zeros(self.length, dtype = float)
        new = CRpureprod()
        new.length = self.length 

        for i in range(self.length):
            temp[i] = self.operands[i]
        
        if self.initialized:
            tempfastvalues = numpy.zeros(self.length,dtype=float)
            tempisnotnumbers = numpy.zeros(self.length,dtype = int)

            for i in range(self.length):
                tempfastvalues[i] = self.fastvalues[i]
            
            new.initialized = True
            new.fastvalues = tempfastvalues 
        new.operands = temp
        return new 
    
    def addto(self, target):
        return CRexpression('add', self, target)
    
    def multo(self, target):
        result = self.copy()

        if isinstance(target, float):
            result.operands[0] = result.operands[0] * target
        
        else:
            if isinstance(target, CRpureprod):
                temp = target.operands

                if self.length >= target.length:
                    for i in range(target.length):
                        result.operands[i] = self.operands[i] * temp[i]
                else:
                    for i in range(self.length):
                        temp[i] *= self.operands[i]
                    result.operands = temp
            elif isinstance(target, CRtrigonometry):
                return target.multo(self).selfsimplify()

    def powto(self, target):
        result = self.copy()

        if isinstance(target, float):
            for i in range(self.length):
                result.operands[i] = self.operands[i] ** target
        else:
            if isinstance(target, CRpuresum):
                temp1 = target.operands
                flength = self.length + target.length - 1
                temp = numpy.zeros(flength, dtype = float)
                m = min(self.length, target.length) - 1
                n = max(self.length, target.length) - 1 
                
                for count in range(flength):
                    crs1 = 1.0 
                    bound11 = max(0,count - m)
                    bound12 = min(count, n)

                    for count1 in range(bound11, bound12+ 1):
                        bound21 = count - count1 
                        bound22 = min(count, m)

                        for count2 in range(bound21, bound22 + 1):
                            crtemp1 = temp1[count2] * math.comb(count,count-count2)
                            crtemp2 = crtemp1 * math.comb(count, count1)
                            crtemp3 = self.operands[count1] ** crtemp2

                            crtemp = crtemp3 * 1.0
                            crs2 = crtemp
                        
                        crtemp4 = crs2 * crs1
                    crs1 = crtemp4
                temp[count] = crs1
                result.operands = temp 
                result.length = flength
            else:
                return CRexpression('pow', self, target).selfsimplify()
            return result.selfsimplify()
    
    @timing
    def shift(self):
        self.fastvalues[:-1] *= self.fastvalues[1:]
    
    def selfsimplify(self):
        found = -1
        for i in range(self.length):
            if isinstance(self.operands[-i-1], float) and self.operands[-i-1] == 0:
                found = self.length - i-1
        
        if found == 0:
            res = 0.0
        elif found > 0:
            temp = numpy.zeros(found+2,dtype = float)
            for i in range(found + 1):
                temp[i] = self.operands[i]
            self.operands = temp 
            self.length = found + 1 
            res = self 
        else:
            j = self.length - 1
            while j > 0 and isinstance(self.operands[j], float) and self.operands[j] == 1:
                j -= 1
            
            if j == 0:
                res = self.operands[0]
            
            else:
                temp = numpy.zeros(j+1, dtype = int )
                for i in range(j+1):
                    temp[i] = self.operands[i]
                self.operands = temp 
                self.length = j+1 
                res = self 
        return res

    def trig(self, trigtype):
        match trigtype:
            case 'sin':
                return CRexpression(self,trigtype).selfsimplify()
            case 'cos':
                return CRexpression(self,trigtype).selfsimplify()
            case 'cot':
                return CRexpression(self,trigtype).selfsimplify()
            case 'tan':
                return CRexpression(self,trigtype).selfsimplify()



class CRtrigonometry(CRobject):
    
    def __init__(self, trigtype):
        super().__init__()
        self.trigtype = trigtype

    def copy(self):
        rnew = CRtrigonometry(self.trigtype)
        rnew.length = self.length 
        crotemp = numpy.zeros(self.length, dtype = float)
        for i in range(self.length):
            crotemp[i] = self.operands[i]
        
        if self.initialized:
            tempfastvalues = numpy.zeros(self.length,dtype = float)
            tempnotnumbers = numpy.zeros(self.length,dtype = int)
            for i in range(self.length):
                tempfastvalues[i] = self.fastvalues[i]
            self.initialized = True
            self.fastvalues = tempfastvalues
            self.isnotnumbers = tempnotnumbers 
        self.operands = crotemp
        return rnew
    
    def addto(self, target):
        return CRexpression('add', self, target)
    
    def powto(self, target):
        return CRexpression('pow', self, target)
    
    def multo(self, target):
        if isinstance(target,float):
            if self.trigtype == 'sin' or self.trigtype == 'cos':
                res = CRtrigonometry(self.trigtype)
                temparray = numpy.zeros(self.length, dtype = float)
                for i in range(self.length):
                    temparray[i] = self.operands[i]
                temparray[0] *= target
                temparray[self.length//2] *= target

                res.operands = temparray
                res.length = self.length 

            # this can't happen
            else:
                res = CRexpression('mul', self, target)
        else:
            if isinstance(target, CRpureprod):
                res = CRtrigonometry(self.trigtype)
                
                if self.length//2 > target.length:
                    op1 = self.operands 
                    op2 = self.correctP(target.operands, self.length//2)
                elif self.length//2 < target.length:
                    op1 = self.correctT(self.operands, target.length)
                    op2 = target.operands 
                    targetlength = target.length
                else:
                    op1 = self.operands
                    op2 = target.operands 
                    targetlength = target.length//2 
                # pureprod and crtrig are both float only
                opres = numpy.zeros(targetlength)
                for i in range(targetlength):
                    opres[i] = op1[i] * op2[i]
                
                    opres[i+targetlength] = op1[i+targetlength] * op2[i]
                
                res.length = targetlength * 2
                res.operands = opres 
            else:
                res = CRexpression('mul', self, target)
        return res.selfsimplify()


    def selfsimplify(self):
        a = isinstance(self.operands[0],float)
        b = a and self.operands[0] == 0.0
        c = b and isinstance(self.operands[self.length//2], float)
        d = c and self.operands[self.length//2] == 0.0
        if d:
            res = 0.0
        else:
            res = self
        return res
    
    def initialize(self):
        super().initialize()
        return self.valueof()
    
    @timing
    def shift(self):
        t = self.length // 2
        for i in range(t-1):
            r1 = self.fastvalues[i] * self.fastvalues[i+t+1]
            r2 = self.fastvalues[i+t] * self.fastvalues[i+1]
            z = r1+ r2
            r3 = self.fastvalues[i+t] * self.fastvalues[i+1+t]
            r4 = self.fastvalues[i] * self.fastvalues[i+1]
            self.fastvalues[i+t] = r3 - r4
            self.fastvalues[i] = z
    
    def valueof(self):
        match self.trigtype:
            case 'sin': 
                return self.fastvalues[0]
            case 'cos':
                return self.fastvalues[self.length//2]

            case 'tan':
                return self.fastvalues[0] / self.fastvalues[self.length//2]

            case 'cot':
                return self.fastvalues[self.length//2] / self.fastvalues[0]
        return 0

