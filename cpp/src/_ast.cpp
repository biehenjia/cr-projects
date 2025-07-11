#include "_ast.hpp"
#include "crsum.hpp"
#include "crnum.hpp"

/*
in python, we construct the AST, and each symbolic node (variable) is initialized with
the proper start and step and index. Then, we call crinit that passes the number of evaluations 
needed for each one

*/

CRnum* m1 = new CRnum(-1);

CRobj* ASTvar::crmake(){
    return new CRsum(index, start, step);
}

CRobj* ASTnum::crmake(){
    return new CRnum(value);
}

ASTbin::~ASTbin(){
    //std::cout<<"deleted binary\n";

    delete left;
    delete right;
    if (cr != nullptr){
        delete cr;
    }
}

CRobj* ASTbin::crmake(){
    //std::cout<<"crmake on astbin called \n";
    CRobj* result;
    CRobj* crleft = left->crmake();
    CRobj* crright = right->crmake();
    switch (optype) {
        case bt::ADD:
            result = crleft->add(*crright);
            break;
        case bt::MUL:
            
            result = crleft->mul(*crright);
            break;
        case bt::DIV:
            result = (crleft->mul(*crright))->pow(*m1);
            break;
        case bt::SUB:
            result = (crleft->add((*crright->mul(*m1))));
            break;
        case bt::POW:
            //std::cout << "here! \n";
            result = (crleft->pow(*crright));
            break;
    }
    delete crleft;
    delete crright;
    return result;
}

CRobj* ASTun::crmake(){
    //std::cout<<"making AST unary \n";
    CRobj* result;
    CRobj* crleft = left->crmake();
    switch (optype) {
        case ut::COS:
            result = crleft->cos();
            break;
        case ut::SIN:
            result = crleft->sin(); 
            break;
        case ut::EXP:
            result  = crleft->exp();
            break;
        case ut:: LN:
            result = crleft->ln();
            break;
    }
    delete crleft;
    //std::cout<<"returned AST unary\n";
    // for (size_t i = 0; i < result->length; i++){ 
    //     std::cout<<result->operands[i]->valueof()<<" ";
    // }
    //std::cout<<"\n";
    return result;
}

ASTun::~ASTun(){
    //std::cout<<"deleted unary\n";
    delete left;
    if (cr != nullptr){
        delete cr;
    }
}

void ASTnode::crinit(std::vector<size_t> p){
    cr = crmake();
    cr->initialize();
    params = p;
}

// potentially can use CRtable? 
// maybe optimization?
void ASTnode::_creval(const CRobj& c, size_t i){
    if (i >= params.size()){
        return;
    } else { 
        // expensive
        auto cc = c.copy();
        auto val = 0;
        for (size_t j = 0; j < params[i]; j++){
            result.push_back(cc->valueof());
            _creval(*cc, i+1);
            cc->shift(i);
        }
    }
}

std::vector<double> ASTnode::creval(){
    _creval(*cr,0);
    return result;
}


void ASTnode::view(){
    //std::cout<<"astnode\n";
    left->view();
    right->view();
    
}

void ASTvar::view(){
    std::cout<<"astvar(x)\n";
}

void ASTnum::view(){
    std::cout<<"astnum("<< value <<")\n";
}

void ASTun::view(){ 
    std::cout<<"astun\n";
    left->view();
    
}

void ASTbin::view(){
    std::cout<<"astbin\n";
    left->view();
    right->view();
}