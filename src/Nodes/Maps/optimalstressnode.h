#ifndef OPTIMALSTRESSNODE_H
#define OPTIMALSTRESSNODE_H

#include "src/Nodes/nodebase.h"

class OptimalStressNode : public NodeBase
{
    Q_OBJECT
public:
    OptimalStressNode(double mu_d = 0., double mu_s = 0.,
                      double strike = 0., double dip = 0.,
                      double rake = 0., double cohesion = 0.,
                      double s2ratio = 0., double r = 0.,
                      double effectiveConfiningStress = 0.);

private:
    double mu_d;
    double mu_s;
    double strike;
    double dip;
    double rake;
    double cohesion;
    double s2ratio;
    double r;
    double effectiveConfiningStress;

    void createLayout();
    void addMu_dLayout(QVBoxLayout* globalLayout);
    void addMu_sLayout(QVBoxLayout* globalLayout);
    void addStrikeLayout(QVBoxLayout* globalLayout);
    void addDipLayout(QVBoxLayout* globalLayout);
    void addRakeLayout(QVBoxLayout* globalLayout);
    void addCohesionLayout(QVBoxLayout* globalLayout);
    void addS2ratioLayout(QVBoxLayout* globalLayout);
    void addRLayout(QVBoxLayout* globalLayout);
    void addEffectiveConfiningStressLayout(QVBoxLayout* globalLayout);

private slots:
    void setMu_d(double newMu_d);
    void setMu_s(double newMu_s);
    void setStrike(double newStrike);
    void setDip(double newDip);
    void setRake(double newRake);
    void setCohesion(double newCohesion);
    void setS2ratio(double newS2ratio);
    void setR(double newR);
    void setEffectiveConfiningStress(double newEffectiveConfiningStress);

    // NodeBase interface
protected:
    void saveValues(YAML::Emitter* out);
};

#endif // OPTIMALSTRESSNODE_H
