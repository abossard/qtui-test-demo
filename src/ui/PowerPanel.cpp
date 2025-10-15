#include "ui/PowerPanel.h"
#include "services/PowerDistributionService.h"
#include <QVBoxLayout>

PowerPanel::PowerPanel(QWidget* parent) : QWidget(parent) {
  auto* layout = new QGridLayout(this);
  life_ = new QSlider(Qt::Horizontal); nav_ = new QSlider(Qt::Horizontal); weapons_ = new QSlider(Qt::Horizontal); research_ = new QSlider(Qt::Horizontal);
  reserveLabel_ = new QLabel("Reserve: 0%", this);
  const int sliders[] = {0,100};
  for (QSlider* s : {life_, nav_, weapons_, research_}) { s->setRange(sliders[0], sliders[1]); s->setValue(25); }
  layout->addWidget(new QLabel("Life Support"), 0,0); layout->addWidget(life_,0,1);
  layout->addWidget(new QLabel("Navigation"), 1,0); layout->addWidget(nav_,1,1);
  layout->addWidget(new QLabel("Weapons"), 2,0); layout->addWidget(weapons_,2,1);
  layout->addWidget(new QLabel("Research"), 3,0); layout->addWidget(research_,3,1);
  layout->addWidget(reserveLabel_,4,0,1,2);
  setLayout(layout);
  setObjectName("PowerPanel");
  connect(life_, &QSlider::valueChanged, this, &PowerPanel::onLifeChanged);
  connect(nav_, &QSlider::valueChanged, this, &PowerPanel::onNavChanged);
  connect(weapons_, &QSlider::valueChanged, this, &PowerPanel::onWeaponsChanged);
  connect(research_, &QSlider::valueChanged, this, &PowerPanel::onResearchChanged);
}

void PowerPanel::attachService(PowerDistributionService* svc) {
  service_ = svc;
  if (service_) connect(service_, SIGNAL(powerChanged(PowerAllocation)), this, SLOT(onPowerChanged(PowerAllocation)));
}

void PowerPanel::onLifeChanged(int v)      { if (service_) service_->setLifeSupport(v); }
void PowerPanel::onNavChanged(int v)       { if (service_) service_->setNavigation(v); }
void PowerPanel::onWeaponsChanged(int v)   { if (service_) service_->setWeapons(v); }
void PowerPanel::onResearchChanged(int v)  { if (service_) service_->setResearch(v); }

void PowerPanel::onPowerChanged(const PowerAllocation& alloc) {
  // Avoid feedback loops by blocking signals while syncing slider values.
  const auto sync = [](QSlider* s, int value){ if (s->value() != value) { QSignalBlocker b(s); s->setValue(value);} };
  sync(life_, alloc.lifeSupport);
  sync(nav_, alloc.navigation);
  sync(weapons_, alloc.weapons);
  sync(research_, alloc.research);
  reserveLabel_->setText(QString("Reserve: %1%" ).arg(alloc.reserve));
}
