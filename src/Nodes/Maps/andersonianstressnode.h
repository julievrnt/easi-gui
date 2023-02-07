#ifndef ANDERSONIANSTRESSNODE_H
#define ANDERSONIANSTRESSNODE_H

#include "src/Nodes/nodebase.h"

class AndersonianStressNode : public NodeBase
{
    Q_OBJECT
public:
    AndersonianStressNode(double mu_d = 0, double mu_s = 0,
                          double sh_max = 0, double s_v = 0,
                          double cohesion = 0, double s2ratio = 0,
                          double s = 0, double sig_zz = 0);

private:
    double mu_d;
    double mu_s;
    double sh_max;
    double s_v;
    double cohesion;
    double s2ratio;
    double s;
    double sig_zz;

    void createLayout();
    void addMu_dLayout(QVBoxLayout* globalLayout);
    void addMu_sLayout(QVBoxLayout* globalLayout);
    void addSH_maxLayout(QVBoxLayout* globalLayout);
    void addS_vLayout(QVBoxLayout* globalLayout);
    void addCohesionLayout(QVBoxLayout* globalLayout);
    void addS2ratioLayout(QVBoxLayout* globalLayout);
    void addSLayout(QVBoxLayout* globalLayout);
    void addSig_zzLayout(QVBoxLayout* globalLayout);

private slots:
    void setMu_d(double newMu_d);
    void setMu_s(double newMu_s);
    void setSh_max(double newSh_max);
    void setS_v(double newS_v);
    void setCohesion(double newCohesion);
    void setS2ratio(double newS2ratio);
    void setS(double newS);
    void setSig_zz(double newSig_zz);

    // NodeBase interface
protected:
    void saveValues(YAML::Emitter* out);
};

#endif // ANDERSONIANSTRESSNODE_H
