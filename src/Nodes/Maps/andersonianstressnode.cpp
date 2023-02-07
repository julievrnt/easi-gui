#include "andersonianstressnode.h"


AndersonianStressNode::AndersonianStressNode(double mu_d, double mu_s, double sh_max, double s_v, double cohesion, double s2ratio, double s, double sig_zz)
{
    typeOfNode = ANDERSONIANSTRESSNODE;
    localTag = "AndersonianStress";
    setWindowTitle("Andersonian Stress");

    this->mu_d = mu_d;
    this->mu_s = mu_s;
    this->sh_max = sh_max;
    this->s_v = s_v;
    this->cohesion = cohesion;
    this->s2ratio = s2ratio;
    this->s = s;
    this->sig_zz = sig_zz;

    createLayout();

    outputs->append("s_xx");
    outputs->append("s_yy");
    outputs->append("s_zz");
    outputs->append("s_xy");
    outputs->append("s_yz");
    outputs->append("s_xz");

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void AndersonianStressNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);

    addMu_dLayout(globalLayout);
    addMu_sLayout(globalLayout);
    addSH_maxLayout(globalLayout);
    addS_vLayout(globalLayout);
    addCohesionLayout(globalLayout);
    addS2ratioLayout(globalLayout);
    addSLayout(globalLayout);
    addSig_zzLayout(globalLayout);

    addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void AndersonianStressNode::addMu_dLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "mu_d");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(mu_d);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setMu_d(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addMu_sLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "mu_s");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(mu_s);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setMu_s(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addSH_maxLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "SH_max");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(sh_max);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setSh_max(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addS_vLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "S_v");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(s_v);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setS_v(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addCohesionLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "cohesion");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(cohesion);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setCohesion(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addS2ratioLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "s2ratio");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(s2ratio);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setS2ratio(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addSLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "S");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(s);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setS(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::addSig_zzLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();
    addLabel(constantLayout, "sig_zz");

    QDoubleSpinBox* constantValue = addDoubleSpinBox(constantLayout);
    constantValue->setValue(sig_zz);
    connect(constantValue, SIGNAL(valueChanged(double)), this, SLOT(setSig_zz(double)));

    globalLayout->addLayout(constantLayout);
}

void AndersonianStressNode::setMu_d(double newMu_d)
{
    mu_d = newMu_d;
}

void AndersonianStressNode::setMu_s(double newMu_s)
{
    mu_s = newMu_s;
}

void AndersonianStressNode::setSh_max(double newSh_max)
{
    sh_max = newSh_max;
}

void AndersonianStressNode::setS_v(double newS_v)
{
    s_v = newS_v;
}

void AndersonianStressNode::setCohesion(double newCohesion)
{
    cohesion = newCohesion;
}

void AndersonianStressNode::setS2ratio(double newS2ratio)
{
    s2ratio = newS2ratio;
}

void AndersonianStressNode::setS(double newS)
{
    s = newS;
}

void AndersonianStressNode::setSig_zz(double newSig_zz)
{
    sig_zz = newSig_zz;
}

void AndersonianStressNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "constants";
    *out << YAML::BeginMap;

    *out << YAML::Key << "mu_d" << YAML::Value << mu_d;
    *out << YAML::Key << "mu_s" << YAML::Value << mu_s;
    *out << YAML::Key << "SH_max" << YAML::Value << sh_max;
    *out << YAML::Key << "S_v" << YAML::Value << s_v;
    *out << YAML::Key << "cohesion" << YAML::Value << cohesion;
    *out << YAML::Key << "s2ratio" << YAML::Value << s2ratio;
    *out << YAML::Key << "S" << YAML::Value << s;
    *out << YAML::Key << "sig_zz" << YAML::Value << sig_zz;

    *out << YAML::EndMap;
}
