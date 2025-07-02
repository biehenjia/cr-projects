#include "_ast.hpp"
#include "crsum.hpp"
#include "crnum.hpp"

CRnum* m1 = new CRnum(-1);

CRobj* ASTnum::crmake(double x, double h){
    return new CRnum(value);
}

CRobj* ASTvar::crmake(double x, double h){
    return new CRsum(x, h);
}

ASTbin::~ASTbin(){
    //std::cout<<"deleted binary\n";

    delete left;
    delete right;
    if (cr != nullptr){
        delete cr;
    }
}

CRobj* ASTbin::crmake(double x, double h){
    //std::cout<<"crmake on astbin called \n";
    CRobj* result;
    CRobj* crleft = left->crmake( x,  h);
    CRobj* crright = right->crmake( x,  h);
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

CRobj* ASTun::crmake(double x, double h){
    CRobj* result;
    CRobj* crleft = left->crmake( x,  h);
    switch (optype) {
        case ut::COS:
            result = crleft->cos();
        case ut::SIN:
            result = crleft->sin(); 
        case ut::EXP:
            result  = crleft->exp();
        case ut:: LN:
            result = crleft->ln();
    }
    delete crleft;
    return result;
}

ASTun::~ASTun(){
    //std::cout<<"deleted unary\n";
    delete left;
    if (cr != nullptr){
        delete cr;
    }
}

void ASTnode::crinit(double x, double h){
    cr = crmake(x,h);
    std::cout<<"crmade\n";
    for (size_t i = 0; i < cr->operands.size(); i++){
        std::cout<< cr->operands[i]->valueof()<<" ";
    }
    std::cout<<"\n";
    cr->initialize();
    std::cout<<"initialized\n";

}

std::vector<double> ASTnode::creval(double q){

    std::cout<<"Creval called\n";
    std::vector<double> result;
    result.reserve(q);
    for (size_t i = 0; i < q; i++){ 
        result.push_back(cr->valueof());
        cr->shift();
    }
    std::cout<<"creval called\n";
    return result;
}

void ASTnode::view(){
    std::cout<<"astnode\n";
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

