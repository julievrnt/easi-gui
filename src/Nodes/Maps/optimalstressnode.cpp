#include "optimalstressnode.h"
#include <QDoubleSpinBox>


OptimalStressNode::OptimalStressNode(double mu_d, double mu_s, double strike, double dip, double rake, double cohesion, double s2ratio, double r, double effectiveConfiningStress)
{
    typeOfNode = OPTIMALSTRESSNODE;
    localTag = "OptimalStress";
    setWindowTitle("Optimal Stress");

    this->mu_d = mu_d;
    this->mu_s = mu_s;
    this->strike = strike;
    this->dip = dip;
    this->rake = rake;
    this->cohesion = cohesion;
    this->s2ratio = s2ratio;
    this->r = r;
    this->effectiveConfiningStress = effectiveConfiningStress;

    createLayout();

    outputs->append("s_xx");
    outputs->append("s_yy");
    outputs->append("s_zz");
    outputs->append("s_xy");
    outputs->append("s_yz");
    outputs->append("s_xz");

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void OptimalStressNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);

    addMu_dLayout(globalLayout);
    addMu_sLayout(globalLayout);
    addStrikeLayout(globalLayout);
    addDipLayout(globalLayout);
    addRakeLayout(globalLayout);
    addCohesionLayout(globalLayout);
    addS2ratioLayout(globalLayout);
    addRLayout(globalLayout);
    addEffectiveConfiningStressLayout(globalLayout);

    addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void OptimalStressNode::addMu_dLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "mu_d");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(mu_d);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setMu_d(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addMu_sLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "mu_s");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(mu_s);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setMu_s(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addStrikeLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "strike");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(strike);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setStrike(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addDipLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "dip");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(dip);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setDip(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addRakeLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "rake");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(rake);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setRake(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addCohesionLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "cohesion");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(cohesion);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setCohesion(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addS2ratioLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "s2ratio");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(s2ratio);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setS2ratio(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addRLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "R");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(r);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setR(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::addEffectiveConfiningStressLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "effectiveConfiningStress");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(effectiveConfiningStress);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setEffectiveConfiningStress(double)));

    globalLayout->addLayout(constantLayout);
}

void OptimalStressNode::setMu_d(double newMu_d)
{
    mu_d = newMu_d;
}

void OptimalStressNode::setMu_s(double newMu_s)
{
    mu_s = newMu_s;
}

void OptimalStressNode::setStrike(double newStrike)
{
    strike = newStrike;
}

void OptimalStressNode::setDip(double newDip)
{
    dip = newDip;
}

void OptimalStressNode::setRake(double newRake)
{
    rake = newRake;
}

void OptimalStressNode::setCohesion(double newCohesion)
{
    cohesion = newCohesion;
}

void OptimalStressNode::setS2ratio(double newS2ratio)
{
    s2ratio = newS2ratio;
}

void OptimalStressNode::setR(double newR)
{
    r = newR;
}

void OptimalStressNode::setEffectiveConfiningStress(double newEffectiveConfiningStress)
{
    effectiveConfiningStress = newEffectiveConfiningStress;
}

void OptimalStressNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "constants";
    *out << YAML::BeginMap;

    *out << YAML::Key << "mu_d" << YAML::Value << mu_d;
    *out << YAML::Key << "mu_s" << YAML::Value << mu_s;
    *out << YAML::Key << "strike" << YAML::Value << strike;
    *out << YAML::Key << "dip" << YAML::Value << dip;
    *out << YAML::Key << "rake" << YAML::Value << rake;
    *out << YAML::Key << "cohesion" << YAML::Value << cohesion;
    *out << YAML::Key << "s2ratio" << YAML::Value << s2ratio;
    *out << YAML::Key << "R" << YAML::Value << r;
    *out << YAML::Key << "effectiveConfiningStress" << YAML::Value << effectiveConfiningStress;

    *out << YAML::EndMap;
}
