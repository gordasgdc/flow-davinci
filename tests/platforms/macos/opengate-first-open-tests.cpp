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
 // Resolve's toolbar lives inside the page tree that gets parked, not as a
 // sibling supplied by our fixture. Keep that ownership topology realistic.
 outer->removeWidget(toolbar);native->insertWidget(0,toolbar);
 main.show();settle();
 QWidget projectStrip(&main);projectStrip.setObjectName("UiProjectsMainBarImp");
 projectStrip.setGeometry(0,page->mapTo(&main,QPoint()).y(),100,32);projectStrip.show();
 {
 PanelControls controls;settle(2500);const auto frame=main.geometry();auto binding=component<Unbound::NativeWorkspaceBinding>(&controls);auto menu=bar->findChild<QMenu *>("unboundWorkspaceUI");
 if(restart){
  check(binding&&binding->active()&&!binding->contains("frameEditPageMediaPool"),"restart retains the unsaved detached panel instead of reloading Resolve Layout");
  auto header=component<Unbound::PinnedWorkspaceHeader>(&controls);check(header&&!header->shown(),"restart retains the unsaved hidden header");
  return failures?1:0;
 }
 check(binding&&binding->active(),"first open mounts existing panel tree in the primary workspace");
 check(!controls.isVisible(),"first open requires no Panel Manager");
 auto timelineCell=main.findChild<QWidget *>("unboundGroupCell_frameConformTimelines");
 auto timelineHeader=timelineCell?timelineCell->findChild<QLabel *>("unboundGroupHeader"):nullptr;
 check(timelineHeader&&timelineHeader->isVisible(),"embedded Timeline reuses the visible draggable group title");
 auto legacyTimeline=timelineCell?timelineCell->findChild<QWidget *>("unboundTimelineHeader"):nullptr;
 check(!legacyTimeline||legacyTimeline->isHidden(),"embedded Timeline does not show a duplicate older title row");
 auto mediaCell=main.findChild<QWidget *>("unboundGroupCell_frameEditPageMediaPool");
 auto mediaHeader=mediaCell?mediaCell->findChild<QWidget *>("openGateGroupHeaderBar"):nullptr;
 check(mediaHeader&&mediaHeader->isHidden(),"tabbed Media Pool has a tab title without a repeated panel title row");
 for(auto tabs:page->findChildren<QTabWidget *>("unboundPanelTabs"))for(int index=0;index<tabs->count();++index)
  check(!tabs->tabText(index).startsWith("widget")&&!tabs->tabText(index).startsWith("frame"),"tab titles never expose internal widget identifiers");
 auto mixerGlyph=mixer->findChild<QToolButton *>("unboundNativeUndock_frameEditAudioMeter");
 check(mixerGlyph&&mixerGlyph->isVisible(),"Mixer nested inside managed Timeline retains its own native placement control");
 if(mixerGlyph)mixerGlyph->click();settle();
 check(mixerTitle->window()!=&main,"Mixer placement control floats its content independently of Timeline");
 auto dockMixer=controls.findChild<QPushButton *>("dock_frameEditAudioMeter");if(dockMixer)dockMixer->click();settle();
 check(mixerTitle->window()==&main,"Mixer docks back without changing Timeline ownership");
 auto primary=component<Unbound::FooterPanel>(&controls);check(primary&&primary->primarySurface()==&main,"primary band owner is the main workspace");
 check(toolbar->window()==&main&&footer->window()==&main,"native header and footer stay in primary window");
 check(toolbar->isVisible()&&page->isAncestorOf(toolbar),"parked native toolbar is reclaimed into the primary page's reserved top band");
 auto panelsButton=right->findChild<QToolButton *>("openGatePrimaryPanels");check(!panelsButton,"Panels uses the persistent main title bar instead of a hideable toolbar slot");
 // Exercise the actual assembled layout at several widths, rather than merely
 // checking its initial size. Native GPU painting remains a separate live gate.
 for(int width:{1400,1000,760,1400}){
  main.resize(width,900);settle(100);
  auto content=page->findChild<QWidget *>("unboundNativeArrangement");
  const QRect body(content?QRect(content->mapTo(&main,QPoint()),content->size()):QRect());
  const auto visibleTop=toolbar->visibleRegion().boundingRect();
  const QRect top(toolbar->mapTo(&main,visibleTop.topLeft()),visibleTop.size());
  const QRect bottom(footer->mapTo(&main,QPoint()),footer->size());
  check(!body.intersects(top)&&!body.intersects(bottom),"resizing keeps panel content outside visible header/footer bands");
  check(body.top()>=projectStrip.geometry().bottom(),"panel content never enters Resolve's permanent project/window strip");
  check(Unbound::workspaceViewportBounds(&main,page)==QRect(content->mapToGlobal(QPoint()),content->size()),"maximize bounds use the laid-out body below the permanent strip and toolbar");
  if(panelsButton)check(right->rect().contains(QRect(panelsButton->mapTo(right,QPoint()),panelsButton->size())),"Panels control remains inside its allocated header slot");
  if(timelineHeader&&timelineHeader->parentWidget()){
   auto row=timelineHeader->parentWidget();
   check(timelineCell->rect().contains(QRect(row->mapTo(timelineCell,QPoint()),row->size())),"single Timeline title row remains within its panel cell");
  }
 }
 main.setGeometry(frame);settle(100);
 auto update=menu?menu->findChild<QAction *>("unboundUpdateWorkspace"):nullptr;check(update&&!update->isEnabled(),"Resolve Layout cannot be updated from the menu");
 auto choices=menu?menu->findChild<QMenu *>("openGateWorkspaces"):nullptr;check(choices&&!choices->actions().isEmpty()&&choices->actions().first()->text()=="Resolve Layout","read-only Resolve Layout is pinned first");
 auto nativeTitle=left->findChild<QPushButton *>();
 auto sendTitle=[&](QEvent::Type type,QPoint point,Qt::MouseButton button,Qt::MouseButtons buttons){QMouseEvent e(type,point,nativeTitle->mapToGlobal(point),button,buttons,Qt::NoModifier);QApplication::sendEvent(nativeTitle,&e);};
 sendTitle(QEvent::MouseButtonPress,{5,5},Qt::LeftButton,Qt::LeftButton);sendTitle(QEvent::MouseMove,{90,5},Qt::NoButton,Qt::LeftButton);sendTitle(QEvent::MouseButtonRelease,{90,5},Qt::LeftButton,Qt::NoButton);settle();
 check(binding&&!binding->contains("frameEditPageMediaPool"),"dragging the native Media Pool title removes it from the managed tree");
 check(nativePanels["frameEditPageMediaPool"]->isHidden(),"floating Media Pool leaves no visible native placeholder");
 nativePanels["frameEditPageMediaPool"]->show();settle();check(nativePanels["frameEditPageMediaPool"]->isHidden(),"native visibility refresh cannot reopen placeholder");
 check(main.geometry()==frame,"detachment does not resize primary workspace");
 auto callers=component<Unbound::PanelCallerMenu>(&controls);
 auto visibility=[&](const char *label){QMenu flat;callers->populateFlat(&flat);bool found=false;for(auto action:flat.actions()){check(!action->menu(),"Panels inventory has no subtrees");check(action->text()!="Timeline Toolbar","Timeline toolbar is not separately arranged");if(action->text()==label){action->trigger();found=true;}}settle();check(found,label);
  if(auto body=page->findChild<QWidget *>("unboundNativeArrangement"))check(Unbound::workspaceViewportBounds(&main,page)==QRect(body->mapToGlobal(QPoint()),body->size()),"each band visibility combination updates maximize to actual content bounds");};
 auto bodyHeight=[&]{auto body=page->findChild<QWidget *>("unboundNativeArrangement");return body?body->height():-1;};
 const auto both=bodyHeight();visibility("Workspace Header");const auto footerOnly=bodyHeight();
 check(footerOnly>both,"hiding header returns height to the panel area");
 visibility("Page Navigation");const auto neither=bodyHeight();check(neither>footerOnly,"hiding footer also returns its height");
 visibility("Workspace Header");check(bodyHeight()<neither&&!footer->isVisible(),"header-only state reserves header with footer still hidden");
 visibility("Page Navigation");check(toolbar->isVisible()&&footer->isVisible()&&bodyHeight()==both,"both bands return to exact original allocation");
 visibility("Metadata");check(binding&&binding->contains("frameEditPageMetadata"),"Panels reveals a parked optional panel into the existing primary arrangement");
 check(nativePanels["frameEditPageMetadata"]->isHidden(),"optional panel reveal does not expose the parked native page");
 visibility("Metadata");visibility("Metadata");check(binding&&binding->selected("frameEditPageMetadata"),"optional panel hide/reveal uses the same retained placement");
 QTimer answer;answer.setInterval(10);QObject::connect(&answer,&QTimer::timeout,[&]{for(auto widget:QApplication::topLevelWidgets())if(auto dialog=qobject_cast<QInputDialog *>(widget)){dialog->setAttribute(Qt::WA_DontShowOnScreen);dialog->setTextValue("Flow Workspace");dialog->accept();}});
 answer.start();auto saveNew=menu->findChild<QAction *>("unboundSaveWorkspaceAs");if(saveNew)saveNew->trigger();settle();answer.stop();
 auto selection=controls.findChild<QComboBox *>("arrangements");const auto userId=selection?selection->currentData().toString():QString();
 check(Unbound::WorkspacePresetPolicy::writable(userId)&&selection->currentText()=="Flow Workspace","Save as New creates and selects a writable workspace");
 update=menu->findChild<QAction *>("unboundUpdateWorkspace");check(update&&update->isEnabled(),"Update Current Workspace is enabled for a user workspace");
 visibility("Workspace Header");Unbound::ArrangementStore beforeUpdate;
 check(beforeUpdate.find(userId)["workspace"].toObject()["header"].toObject()["visible"].toBool(),"unsaved changes do not update the named record automatically");
 if(update)update->trigger();settle();Unbound::ArrangementStore afterUpdate;
 check(!afterUpdate.find(userId)["workspace"].toObject()["header"].toObject()["visible"].toBool(),"Update writes the current layout into only the selected user workspace");
 visibility("Workspace Header");
 auto nativeAction=menu?menu->findChild<QAction *>("unboundRestoreNativeWorkspace"):nullptr;if(nativeAction)nativeAction->trigger();settle();
 check(binding&&!binding->active()&&page->layout()==native,"Native returns original native layout");
 auto expandSource=controls.findChild<QPushButton *>("expand_widgetEditSourcePlayer");if(expandSource)expandSource->click();settle();
 check(nativePanels["widgetEditSourcePlayer"]->window()!=&main,"temporary maximize remains available in Native");
 Unbound::WorkspaceSettings duringNativeMaximize;check(duringNativeMaximize.data["layout_mode"]=="native","temporary maximize does not enter OpenGate mode");
 if(expandSource)expandSource->click();settle();check(nativePanels["widgetEditSourcePlayer"]->parentWidget()==page,"Native maximize restores the exact native panel parent");
 sendTitle(QEvent::MouseButtonPress,{5,5},Qt::LeftButton,Qt::LeftButton);sendTitle(QEvent::MouseMove,{90,5},Qt::NoButton,Qt::LeftButton);sendTitle(QEvent::MouseButtonRelease,{90,5},Qt::LeftButton,Qt::NoButton);settle();
 Unbound::WorkspaceSettings gestureState;check(gestureState.data["layout_mode"]=="managed"&&page->layout()==native,"Native title drag enters OpenGate without loading the default or moving unrelated native slots");
 check(nativePanels["frameEditPageMediaPool"]->isHidden(),"Native-origin drag also leaves no visible empty slot");
 if(choices&&!choices->actions().isEmpty())choices->actions().first()->trigger();settle(1800);
 check(binding&&binding->active(),"choosing Resolve Layout re-enters OpenGate through existing restoration");
 update=menu->findChild<QAction *>("unboundUpdateWorkspace");check(update&&!update->isEnabled(),"Resolve Layout remains protected after returning from Native");
 auto undock=controls.findChild<QPushButton *>("undock_frameEditPageMediaPool");if(undock)undock->click();settle();
 check(binding&&!binding->contains("frameEditPageMediaPool"),"placement button also detaches the managed panel");
 visibility("Workspace Header");settle();
 const auto retainedPlan=binding->saved();
 for(int i=0;i<8;++i){
  pageStack->setCurrentWidget(i%2?color:fusion);settle(180);
  check(footer->window()==&main,"page switching never transfers the global native footer to a panel");
  check(Unbound::currentWorkspacePage(&main)==(i%2?QStringLiteral("color"):QStringLiteral("fusion")),"fixture switches to the actual destination page");
  auto recovery=menu->findChild<QAction *>("unboundRestoreNativeWorkspace");check(recovery&&recovery->isChecked(),"unsupported pages truthfully display Native mode");
  pageStack->setCurrentWidget(page);settle(300);
  check(binding->active()&&binding->saved()==retainedPlan,"page return preserves the exact Edit arrangement instead of reloading a preset");
  check(nativePanels["frameEditPageMediaPool"]->isHidden(),"page return cannot resurrect a floated panel vacancy");
 }
 }
 check(page->layout()==native,"shutdown returns exact native layout ownership");
 QProcess child;child.setProgram(app.applicationFilePath());child.setArguments({"--restart-check"});child.start();
 check(child.waitForFinished(30000),"fresh-process managed restart completes");
 const auto childErrors=child.readAllStandardError();if(!childErrors.isEmpty())std::fprintf(stderr,"%s",childErrors.constData());
 check(child.exitStatus()==QProcess::NormalExit&&child.exitCode()==0,"last working OpenGate arrangement persists across a new process");
 if(failures){files.setAutoRemove(false);std::fprintf(stderr,"Fixture logs retained: %s\n",qPrintable(files.path()));}
 if(!failures)std::puts("PASS: first open -> managed undock/reclaim -> Native -> OpenGate, primary bands and immutable default");return failures?1:0;
}
