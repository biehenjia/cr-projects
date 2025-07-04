#pragma once 
#include "crobj.hpp"



class CRtrig : public CRobj { 
    public:
        CRtrig(oc t, size_t l);

        CRobj* add(const CRobj& target) const override;
        CRobj* mul(const CRobj& target) const override;
        CRobj* pow(const CRobj& target) const override;

        CRobj* exp() const override;
        CRobj* ln() const override;

        CRobj* sin() const override;
        CRobj* cos() const override;

        void simplify() override; 
        CRobj* copy() const override;
        void shift() override;
        // double initialize();
        double valueof() const;

        oc trigtype;
        CRobj* correctt(size_t nl) const;
};