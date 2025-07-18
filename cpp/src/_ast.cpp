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
    std::cout<<crleft->index<<" "<<crright->index<<"\n";
    switch (optype) {
        case bt::ADD:
            if (crleft->index > crright->index){ 
                result = crleft->add(*crright);
            } else {
                result = crright->add(*crleft);
            }
            break;
        case bt::MUL:
            if (crleft->index > crright->index){ 
                result = crleft->mul(*crright);
            } else { 
                result = crright->mul(*crleft);
            }
            break;
        case bt::DIV:
            result = (crleft->mul(*crright))->pow(*m1);
            break;
        case bt::SUB:
            if (crleft->index > crright->index){ 
                result = crleft->add((*crright->mul(*m1)));
            } else {
                result = (crright->mul(*m1))->add(*crleft);
            }
            break;
        case bt::POW:
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
    std::cout<<"crinit called\n";
    cr = crmake();
    cr->print_tree();
    std::cout<<"crmade!\n";
    cr->initialize();
    std::cout<<"initialized!\n";
    params = p;
    _creval();
    
}

// potentially can use CRtable? 
// maybe optimization?





//nonrecursive for any number of parameters
void ASTnode::_creval(){ 

    size_t n = params.size();
    std::vector<size_t> ind;

    std::vector<std::unique_ptr<CRobj>> crs;

    ind.resize(n,0);
    crs.reserve(n);

    crs.push_back(cr->copy());

    for (size_t i = 1; i < n; i++){
        crs.push_back(crs[i-1]->copy());
    }

    ssize_t i = n-1 ;
    while (true) {

        
        result.push_back(crs[n-1]->valueof());        
    

        i = params.size()-1;
        crs[i]->shift(i);
        
        
        while (i >= 0){
            ind[i] ++;
            if (ind[i] < params[i]){
                for (size_t j = i+1; j < n; j   ++){
                    crs[j] = crs[j-1]->copy();
                    ind[j] = 0;
                }
                break;
            }

            ind[i] = 0;
            if (i > 0){
                crs[i-1]->shift(i-1);
                std::cout<<"Outer loop\n";
                crs[i] = crs[i-1]->copy(); 
            }

            i--;
        }

        //std::cout<<ct<<" "<<i<<"\n";
        if (i < 0){
            break;
        }
    }



    for (size_t i = 0; i < result.size(); i++){ 
        std::cout<<result[i]<<" ";
    }
    std::cout<<"\n";
}



std::vector<double> ASTnode::creval(){
    _creval();
    return result;
}


void ASTnode::view(){
    //std::cout<<"astnode\n";
    left->view();
    right->view();
    
}

void ASTvar::view(){
    std::cout<<"astvar("<<index<<")\n";
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
