#pragma once 
#include "crobj.hpp"

class CRnum : public CRobj {
    public:
        CRnum(double v);
        ~CRnum() override = default;

        //addition crexpr, crnum, crsum case
        CRobj* add(const CRobj& target) const override;
        CRnum* add(const CRnum& target) const ;
        CRsum* add(const CRsum& target) const;
        
        CRobj* mul(const CRobj& target)  const override;
        CRnum* mul(const CRnum& target) const ;
        CRsum* mul(const CRsum& target) const ;
        CRprod* mul(const CRprod& target) const ;

        CRobj* pow(const CRobj& target) const  override;
        CRnum* pow(const CRnum& target) const ;
        CRsum* pow(const CRsum& target) const;

        CRnum* exp() const override;
        CRnum* ln() const override;
        CRnum* sin()  const override;
        CRnum* cos() const override;

        void simplify() override;
        CRnum* copy() const override;

        
        
        double initialize(); 
        double valueof() const;
        bool isnumber() const override;
    
        void shift() override;
        double value;

};