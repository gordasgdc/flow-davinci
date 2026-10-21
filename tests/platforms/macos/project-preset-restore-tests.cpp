#include "native-fixture-start.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QMenuBar>
#include <QProcess>
#include <QInputDialog>
#include <QTemporaryDir>
#include <QThread>
#include <cstdio>
#include "panelcontrols.h"
static void settle(int ms=600){QElapsedTimer t;t.start();while(t.elapsed()<ms){QApplication::processEvents();QThread::msleep(1);}}
template<class T>T *component(QObject *owner){for(auto child:owner->children())if(auto v=dynamic_cast<T *>(child))return v;return nullptr;}
int main(int argc,char **argv){
 QApplication app(argc,argv);app.setQuitOnLastWindowClosed(false);QTemporaryDir files;
 const bool restart=app.arguments().contains("--restart-check");
 if(!restart){qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());qputenv("UNBOUND_WORKSPACE_FILE",(files.path()+"/Workspace.json").toUtf8());qputenv("UNBOUND_ARRANGEMENTS_FILE",(files.path()+"/Arrangements.json").toUtf8());}qputenv("UNBOUND_TEST_HIDDEN_HOSTS","1");
 int failures=0;auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL: %s\n",why);}};
 QWidget main;main.setAttribute(Qt::WA_DontShowOnScreen);main.setObjectName("UiMainWindow");main.resize(1400,900);auto outer=new QVBoxLayout(&main);
 auto bar=new QMenuBar(&main);bar->setNativeMenuBar(false);outer->setMenuBar(bar);auto workspace=bar->addMenu("Workspace");workspace->setObjectName("workspace");
 auto toolbar=new QStackedWidget(&main);auto tools=new QWidget;tools->setObjectName("pageSingleScreenModeTools");toolbar->addWidget(tools);outer->addWidget(toolbar);auto toolrow=new QHBoxLayout(tools);auto left=new QWidget(tools);left->setObjectName("frameEditTopBarLeftSSM");toolrow->addWidget(left);auto leftrow=new QHBoxLayout(left);leftrow->addWidget(new QPushButton("Media Pool",left));auto right=new QWidget(tools);right->setObjectName("frameEditTopBarRightSSM");right->setLayout(new QHBoxLayout);toolrow->addWidget(right);
 auto pageStack=new QStackedWidget(&main);outer->addWidget(pageStack,1);auto page=new QWidget(pageStack);page->setObjectName("WStackPage_Conform");pageStack->addWidget(page);auto fusion=new QWidget(pageStack);fusion->setObjectName("WStackPage_Fusion");pageStack->addWidget(fusion);auto color=new QWidget(pageStack);color->setObjectName("WStackPage_Color");pageStack->addWidget(color);auto native=new QHBoxLayout(page);
 QHash<QString,QWidget *> nativePanels;for(const auto &id:{"frameConformTimelines","widgetEditSourcePlayer","widgetEditTimelinePlayer","frameEditPageMediaPool","frameEditPageEditIndex","widgetEditInspector","frameEditPageMetadata"}){
  auto panel=new QWidget(page);panel->setObjectName(id);auto layout=new QVBoxLayout(panel);layout->addWidget(new QPushButton(id,panel));native->addWidget(panel);nativePanels[id]=panel;if(QString(id)=="frameEditPageMetadata")panel->hide();
 }
 auto mixer=new QWidget(nativePanels["frameConformTimelines"]);mixer->setObjectName("frameEditAudioMeter");
 auto mixerLayout=new QVBoxLayout(mixer);auto mixerRow=new QWidget(mixer);auto mixerTools=new QHBoxLayout(mixerRow);
 auto mixerTitle=new QLabel("Mixer",mixerRow);mixerTools->addWidget(mixerTitle);auto mixerOptions=new QToolButton(mixerRow);mixerOptions->setObjectName("btnAudioMixerOptions");mixerTools->addWidget(mixerOptions);mixerLayout->addWidget(mixerRow);
 nativePanels["frameConformTimelines"]->layout()->addWidget(mixer);
 auto footer=new QWidget(&main);footer->setObjectName("m_PageSwitcherContainer");outer->addWidget(footer);auto footrow=new QHBoxLayout(footer);for(const auto &id:{"buttonConformPanel","buttonFusionPanel","buttonHome","buttonConfig"}){auto b=new QPushButton(id,footer);b->setObjectName(id);footrow->addWidget(b);}
 auto navigation=workspace->addAction("Show Page Navigation");navigation->setCheckable(true);navigation->setChecked(true);QObject::connect(navigation,&QAction::toggled,footer,&QWidget::setVisible);

 auto pool=nativePanels["frameEditPageMediaPool"],index=nativePanels["frameEditPageEditIndex"];
 auto poolButton=new QPushButton("Media Pool",left);poolButton->setObjectName("btnEditToggleMediaPoolInSSM");poolButton->setCheckable(true);leftrow->addWidget(poolButton);
 auto indexButton=new QPushButton("Index",left);indexButton->setObjectName("btnEditToggleEditIndexInSSM");indexButton->setCheckable(true);leftrow->addWidget(indexButton);
 // Native pages can share mutually exclusive sidebar slots. Reveal is deferred,
 // matching Resolve initialization; the toolbar is outside the Edit page.
 QObject::connect(poolButton,&QPushButton::clicked,[&](bool checked){QTimer::singleShot(0,pool,[=]{pool->setVisible(checked);if(checked)index->hide();});});
 QObject::connect(indexButton,&QPushButton::clicked,[&](bool checked){QTimer::singleShot(0,index,[=]{index->setVisible(checked);if(checked)pool->hide();});});
 pool->hide();index->hide();main.show();settle();
 prepareNativeFixture();
 {
 PanelControls controls;settle();
 auto binding=component<Unbound::NativeWorkspaceBinding>(&controls);
 auto menu=bar->findChild<QMenu *>("unboundWorkspaceUI");auto choices=menu?menu->findChild<QMenu *>("openGateWorkspaces"):nullptr;
 check(choices&&!choices->actions().isEmpty(),"workspace caller available");
 if(choices&&!choices->actions().isEmpty())choices->actions().first()->trigger();settle(3000);
 check(binding&&binding->active()&&binding->contains("frameEditPageMediaPool")&&binding->contains("frameEditPageEditIndex"),"Resolve Layout awaits and acquires mutually exclusive native sidebars sequentially");
 if(binding&&binding->active()) {
  const auto expected=binding->saved();
  auto preset=QJsonObject{{"id","fixture.saved"},{"workspace",QJsonObject{{"page","edit"},{"layout_mode","managed"},{"native_arrangement",expected}}}};
  // Simulate a project boundary returning the native slots, then request a saved
  // managed tree with both optional surfaces unavailable until native activation.
  auto recovery=menu->findChild<QAction *>("unboundRestoreNativeWorkspace");if(recovery)recovery->trigger();settle();poolButton->setChecked(false);indexButton->setChecked(false);pool->hide();index->hide();
  auto restoration=component<Unbound::WorkspaceRestoration>(&controls);if(restoration)restoration->preset(preset,true);settle(3000);
  check(binding->active()&&binding->saved()["tree"]==expected["tree"],"saved preset rebuilds its complete tree after native/project reset");
 }
 }
 if(failures){files.setAutoRemove(false);std::fprintf(stderr,"logs: %s\n",qPrintable(files.path()));}
 return failures?1:0;
}
