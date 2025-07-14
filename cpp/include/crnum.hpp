#pragma once 
#include "crobj.hpp"

class CRnum : public CRobj {
    public:
        CRnum(double v);
        std::unique_ptr<CRobj> add(const CRobj& target) const override;
        std::unique_ptr<CRobj> mul(const CRobj& target)  const override;
        std::unique_ptr<CRobj> pow(const CRobj& target) const  override;

        std::unique_ptr<CRobj> exp() const override;
        std::unique_ptr<CRobj> ln() const override;
        std::unique_ptr<CRobj> sin()  const override;
        std::unique_ptr<CRobj> cos() const override;

        void simplify() override;
        std::unique_ptr<CRobj> copy() const override;

        double initialize(); 
        double valueof() const;
        bool isnumber() const override;
    
        void shift(size_t index) override;
        double value;

};