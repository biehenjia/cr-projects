#include "_ast.hpp"
#include <chrono>

double compute(size_t q){
    ASTnode* c = new ASTvar();
    ASTnode* a = new ASTnum(5.0);
    ASTnode* b = new ASTbin(bt::POW, c, a);
    b->crinit(0.0,1.0);

    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<double> x = b->creval(q);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    return duration;
}

int main() { 
    auto duration = compute(1000000);
    std::cout<< duration;
    return 0;
}

