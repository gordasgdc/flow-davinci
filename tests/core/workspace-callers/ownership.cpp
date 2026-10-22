// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QPushButton>
#include <QTemporaryDir>
#include <cstdio>
#include "../../../src/core/nativepanelposition.h"
#include "../../../src/core/nativerootlayout.h"
#include "../../../src/core/panellayoutsurface.h"
#include "../../../src/core/panellayoutvisibility.h"
#include "../../../src/core/vacatedcontents.h"

int main(int argc,char **argv) {
    QApplication app(argc,argv);app.setQuitOnLastWindowClosed(false);QTemporaryDir logs;
    qputenv("UNBOUND_DIAGNOSTICS_DIR",logs.path().toUtf8());int failures=0;
    auto check=[&](bool ok,const char *why){if(!ok){++failures;std::fprintf(stderr,"FAIL: %s\n",why);}};
    using namespace Unbound;
    for(int kind=0;kind<4;++kind) {
        QWidget main;main.setAttribute(Qt::WA_DontShowOnScreen);main.resize(1100,700);auto outer=new QVBoxLayout(&main);
        auto page=new QWidget(&main);outer->addWidget(page);auto native=new QVBoxLayout(page);
        QWidget *owner=nullptr;QBoxLayout *box=nullptr;QGridLayout *grid=nullptr;QSplitter *split=nullptr;QStackedWidget *stack=nullptr;
        if(kind==0){owner=new QWidget(page);box=new QHBoxLayout(owner);}
        if(kind==1){owner=new QWidget(page);grid=new QGridLayout(owner);}
        if(kind==2)owner=split=new QSplitter(Qt::Horizontal,page);
        if(kind==3)owner=stack=new QStackedWidget(page);
        native->addWidget(owner);
        auto before=new QWidget(owner),panel=new QWidget(owner),after=new QWidget(owner);
        panel->setObjectName("timeline");panel->setMinimumSize(140,100);
        const auto policy=panel->sizePolicy();const auto minimum=panel->minimumSize();
        auto timeline=new QVBoxLayout(panel);auto toolbar=new QPushButton("Native Timeline Toolbar",panel);auto content=new QPushButton("Native Timeline",panel);
        timeline->addWidget(toolbar);timeline->addWidget(content);
        if(box){box->addWidget(before,1);box->addWidget(panel,3,Qt::AlignVCenter);box->addWidget(after,2);}
        if(grid){grid->addWidget(before,0,0);grid->addWidget(panel,1,1,2,3,Qt::AlignVCenter);grid->addWidget(after,3,4);}
        if(split){split->addWidget(before);split->addWidget(panel);split->addWidget(after);split->setSizes({100,700,200});}
        if(stack){stack->addWidget(before);stack->addWidget(panel);stack->addWidget(after);stack->setCurrentWidget(panel);}
        main.show();QApplication::processEvents();const auto frame=main.geometry();const auto sizes=split?split->sizes():QList<int>();
        int clicks=0;QObject::connect(content,&QPushButton::clicked,&main,[&]{++clicks;});
        for(int cycle=0;cycle<3;++cycle) {
            NativePanelPosition position;check(position.acquire(panel,page).isEmpty(),"native box/grid/split/stack position is leased");
            const auto parking=position.contentParking();auto marker=position.surface();
            check(marker&&panel->parentWidget()==parking,"original content and independent restoration slot are retained");
            auto workspace=new QWidget(page);auto managed=new QVBoxLayout(workspace);managed->setContentsMargins(0,0,0,0);
            panel->setParent(workspace);managed->addWidget(panel);panel->show();
            NativeRootLayout lease;check(lease.mount(page,workspace).isEmpty(),"existing root lease mounts the managed surface");
            check(page->layout()!=native&&NativeRootLayout::contains(page,workspace)&&main.geometry()==frame,"workspace replaces the page layout without changing the primary frame");
            check(toolbar->parentWidget()==panel&&content->parentWidget()==panel&&panel->layout()==timeline,"Timeline and toolbar remain one native arranged element");
            content->click();check(clicks==cycle+1,"real native signal connection survives ownership changes");
            panel->setParent(parking);panel->hide();
            check(lease.restore()&&page->layout()==native,"Native return restores the exact original root layout");
            check(position.restore()&&panel->parentWidget()==owner&&panel->layout()==timeline,"Native return restores exact panel ownership and contents");
            check(panel->minimumSize()==minimum&&panel->sizePolicy()==policy,"constraints are preserved across leases");
            if(box)check(box->indexOf(panel)==1&&box->stretch(1)==3,"box slot and stretch restored");
            if(grid){int row,column,rows,columns;grid->getItemPosition(grid->indexOf(panel),&row,&column,&rows,&columns);check(row==1&&column==1&&rows==2&&columns==3,"grid span restored");}
            if(split)check(split->indexOf(panel)==1&&split->sizes()==sizes,"split position and sizes restored");
            if(stack)check(stack->indexOf(panel)==1&&stack->currentWidget()==panel,"stack selection restored");
            delete workspace;
        }
    }
    QWidget slot;slot.setAttribute(Qt::WA_DontShowOnScreen);slot.setMinimumSize(130,70);slot.show();
    const auto policy=slot.sizePolicy();{VacatedContents vacancy;vacancy.hide(&slot);slot.show();QApplication::processEvents();
        check(slot.isHidden(),"leased empty native slot cannot reopen from a native show");vacancy.restore();}
    check(slot.minimumSize()==QSize(130,70)&&slot.sizePolicy()==policy&&!slot.isHidden(),"vacancy release restores native constraints and visibility");
    std::printf("shared ownership failures=%d\n",failures);return failures?1:0;
}
