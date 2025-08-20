#include "cr.hpp"


std::unique_ptr<CRnode> CRnode::add(const CRnode& target) const noexcept
{
    if (index >= target.index){ 
        if (type == target.type){
            if (type == CRSUM){
                
            } else if (type == CRNUM) {

            } else { 
                // invoke CRexpr
            }
        }
    }
    else {
        return target.add(*this);
    }
} 