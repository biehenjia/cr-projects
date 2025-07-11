#pragma once 
#include "crobj.hpp"

class CRnum : public CRobj {
    public:
        CRnum(double v);
        ~CRnum() override = default;

        CRobj* add(const CRobj& target) const override;
        CRobj* mul(const CRobj& target)  const override;
        CRobj* pow(const CRobj& target) const  override;

        CRnum* exp() const override;
        CRnum* ln() const override;
        CRnum* sin()  const override;
        CRnum* cos() const override;

        void simplify() override;
        CRnum* copy() const override;

        double initialize(); 
        double valueof() const;
        bool isnumber() const override;
    
        void shift(size_t index) override;
        double value;

};