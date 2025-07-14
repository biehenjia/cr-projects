#include "_ast.hpp"
#include "crsum.hpp"
#include "crnum.hpp"

/*
in python, we construct the AST, and each symbolic node (variable) is initialized with
the proper start and step and index. Then, we call crinit that passes the number of evaluations 
needed for each one
*/



std::unique_ptr<CRobj> m1 = std::make_unique< CRnum>(-1);

std::unique_ptr<CRobj> ASTvar::crmake(){
    return std::make_unique< CRsum>(index, start, step);
}

std::unique_ptr<CRobj> ASTnum::crmake(){
    return std::make_unique< CRnum>(value);
}

std::unique_ptr<CRobj> ASTbin::crmake(){
    //std::cout<<"crmake on astbin called \n";
    std::unique_ptr<CRobj> result;
    auto crleft = left->crmake();
    auto  crright = right->crmake();
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
    return result;
}

std::unique_ptr<CRobj> ASTun::crmake(){
    //std::cout<<"making AST unary \n";
    std::unique_ptr<CRobj> result;
    auto crleft = left->crmake();
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
    return result;
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
