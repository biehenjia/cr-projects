// tests/testing.cpp

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "_ast.hpp"    // ASTnum, ASTvar, ASTbin, ASTun, ASTnode
#include "crobj.hpp"   // CRobj

int main() {
    // 1) Test ASTnum + creval
    {
        ASTnum* n = new ASTnum(3.5);
        n->crinit(0.0, 0.2);
        std::vector<double> vals = n->creval(4);
        assert(vals.size() == 4);
        for (double v : vals) {
            assert(std::fabs(v - 3.5) < 1e-12);
        }
        delete n;
        std::cout << "[PASS] ASTnum::creval\n";
    }

    // 2) Test simple addition: 1 + 2
    {
        ASTnum* a = new ASTnum(1.0);
        ASTnum* b = new ASTnum(2.0);
        ASTbin* add = new ASTbin(bt::ADD, a, b);
        add->crinit(0.0, 0.1);
        std::vector<double> v = add->creval(3);
        // First sample = 1 + 2 = 3.0
        assert(v.size() == 3);
        assert(std::fabs(v[0] - 3.0) < 1e-12);
        // next sample = value + h*(#operands−1) = 3.0 + 0.1*2 = 3.2
        assert(std::fabs(v[1] - 3.2) < 1e-12);
        delete add;  // this also deletes a and b
        std::cout << "[PASS] ASTbin ADD + creval\n";
    }

    // 3) Test crmake + CRobj::valueof
    {
        ASTnum* n = new ASTnum(5.0);
        n->crinit(0.0, 0.5);
        CRobj* cr = n->crmake(0.0, 0.5);
        double val = cr->valueof();
        assert(std::fabs(val - 5.0) < 1e-12);
        delete cr;
        delete n;
        std::cout << "[PASS] ASTnum crmake + valueof\n";
    }

    // 4) Test a more complex tree: (1 + 2) * sin(3)
    {
        ASTnum* one = new ASTnum(1.0);
        ASTnum* two = new ASTnum(2.0);
        ASTbin* sum = new ASTbin(bt::ADD, one, two);

        ASTnum* three = new ASTnum(3.0);
        ASTun*  s = new ASTun(ut::SIN, three);

        ASTbin* prod = new ASTbin(bt::MUL, sum, s);

        prod->crinit(0.0, 0.1);
        std::vector<double> out = prod->creval(2);
        // first value = (1+2) * sin(3)
        double expected = 3.0 * std::sin(3.0);
        assert(std::fabs(out[0] - expected) < 1e-12);

        delete prod;  // cascades deletion to sum, s, and their children
        std::cout << "[PASS] complex AST tree\n";
    }

    std::cout << "All AST tests passed!\n";
    return 0;
}
