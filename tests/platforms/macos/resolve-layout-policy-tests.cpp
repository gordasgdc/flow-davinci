#include <QApplication>
#include <QTemporaryDir>
#include <cstdio>
#include "resolvelayout.h"
#include "nativearrangementplan.h"
#include "arrangementstore.h"
int main(int argc,char **argv){
 QApplication app(argc,argv);QTemporaryDir dir;qputenv("UNBOUND_ARRANGEMENTS_FILE",(dir.path()+"/arrangements.json").toUtf8());
 const auto entry=Unbound::resolveLayoutEntry("edit");const auto value=entry["workspace"].toObject();const auto plan=value["native_arrangement"].toObject();
 QSet<QString> available;for(const auto &id:plan["panels"].toObject().keys())available.insert(id);
 Unbound::NativeArrangementPlan parsed;const auto error=Unbound::NativeArrangementPlan::parse(plan,"edit",available,parsed);
 if(!error.isEmpty()){std::fprintf(stderr,"FAIL: %s\n",qPrintable(error));return 1;}
 QStringList names;Unbound::groupTreeNames(parsed.tree,names);
 if(!parsed.managed||names.contains("unboundWorkspaceToolbar")||names.contains(Unbound::nativeRemainderId())||names.size()!=6)return 2;
 Unbound::ArrangementStore store;const auto id=entry["id"].toString();
 if(store.update(id,value).isEmpty()||store.remove(id).isEmpty()||store.rename(id,"Changed").isEmpty())return 3;
 QString saved;if(!store.create("My workspace",value,saved).isEmpty()||!store.update(saved,value).isEmpty())return 4;
 if(!Unbound::resolveLayoutEntry("fusion").isEmpty())return 5;
 std::puts("PASS: existing emulator tree becomes band-free embedded recipe; Resolve Layout cannot be updated, renamed or deleted; user copy remains writable");return 0;
}
