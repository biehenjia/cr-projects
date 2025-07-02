#include "_ast.hpp"
#include <chrono>

double compute(size_t q){
    ASTnode* c1 = new ASTvar();
    ASTnode* c2 = new ASTnum(2);
    ASTnode* c3 = new ASTvar();
    ASTnode* c4 = new ASTnum(3);
    ASTnode * c5 = new ASTvar();
    ASTnode * b1 = new ASTbin(bt::ADD, c1, c2);
    ASTnode * b3 = new ASTbin(bt::ADD, b1, c5);
    // ASTnode * b1 = new ASTbin(bt::POW, c1,c2);
    // ASTnode * b2 = new ASTbin(bt::POW, c3, c4);
    // ASTnode* b3 = new ASTbin(bt::ADD, b1, b2);
    b3->crinit(0.0,1.0);

    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<double> x = b3->creval(q);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    return duration;
}

int main() { 
    auto duration = compute(100000);
    std::cout<< duration <<"DONE!\n";
    return 0;
}

