// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QBoxLayout>
#include <QHash>
#include <QIconEngine>
#include <QPainter>
#include <QPointer>
#include <QToolButton>
#include <functional>
#include "diagnostics.h"
#include "keyboardlauncher.h"
namespace Unbound {
// Dedicated native-header button; never replaces or overlays Resolve's options.
class PanelUndockGlyph : public QIconEngine {
public:
    QIconEngine *clone() const override { return new PanelUndockGlyph; }
    void paint(QPainter *p,const QRect &r,QIcon::Mode mode,QIcon::State) override {
        p->save(); p->setRenderHint(QPainter::Antialiasing);
        p->translate(r.topLeft()); p->scale(r.width()/20.,r.height()/20.);
        p->setPen(QPen(mode == QIcon::Active ? QColor(220,220,220) : QColor(132,132,132),1.5));
        p->setBrush(Qt::NoBrush); p->drawRoundedRect(QRectF(2.5,3.5,13,13),1,1);
        p->drawRoundedRect(QRectF(12.5,7,6,6),.75,.75); p->restore();
    }
    QPixmap pixmap(const QSize &size,QIcon::Mode mode,QIcon::State state) override {
        QPixmap result(size); result.fill(Qt::transparent); QPainter p(&result);
        paint(&p,QRect(QPoint(),size),mode,state); return result;
    }
};
class NativePanelDecoration : public QObject {
public:
    explicit NativePanelDecoration(QObject *owner) : QObject(owner) {}
    void wire(QWidget *panel,const QString &id,const QString &title,bool floating,std::function<void()> toggle) {
        if (!panel) return;
        auto button = buttons.value(id);
        if (!button) {
            const QStringList anchors{"btnMediaPoolOptions","btnTitleOptions","btnAudioMixerOptions", "btnOptions",
                "m_BtnDisplayOptions","btnEdlOptions","tbtnPlayerOptions","metadataSelectionMenuBtn","btnCurveSettingsOptions"};
            QAbstractButton *anchor = nullptr; QBoxLayout *layout = nullptr;
            for (auto candidate : panel->findChildren<QAbstractButton *>()) {
                if (!anchors.contains(candidate->objectName())) continue;
                auto row = qobject_cast<QBoxLayout *>(candidate->parentWidget()->layout());
                if (row && row->direction() == QBoxLayout::LeftToRight && row->indexOf(candidate) >= 0) { anchor=candidate; layout=row; break; }
            }
            if (!anchor) return; // No fallback strip or guessed overlay over native input.
            button = new QToolButton(anchor->parentWidget()); button->setObjectName("unboundNativeUndock_"+id);
            button->setIcon(QIcon(new PanelUndockGlyph)); button->setIconSize(QSize(20,20)); button->setFixedSize(24,24);
            button->setAutoRaise(true); button->setFocusPolicy(Qt::NoFocus);
            button->setProperty("unboundPanelId",id);
            layout->insertWidget(layout->indexOf(anchor),button,0,Qt::AlignVCenter);
            auto keyboard = new QToolButton(anchor->parentWidget());
            keyboard->setObjectName("unboundNativeKeyboard_"+id); keyboard->setText(QString::fromUtf8("⌨"));
            keyboard->setToolTip("Keyboard Customization"); keyboard->setAccessibleName(keyboard->toolTip());
            keyboard->setAutoRaise(true); keyboard->setFocusPolicy(Qt::NoFocus); keyboard->setFixedSize(24,24);
            layout->insertWidget(layout->indexOf(anchor),keyboard,0,Qt::AlignVCenter);
            connect(keyboard,&QToolButton::clicked,this,[] { openNativeKeyboardCustomization(); });
            connect(button,&QToolButton::clicked,this,[id,toggle] {
                logEvent("panel.header.toggle",{{"panel",id},{"origin","native-header"}}); toggle();
            });
            buttons[id]=button;
        }
        button->setToolTip((floating ? "Dock " : "Undock ")+title);
        button->setAccessibleName(button->toolTip()); button->show();
    }
private:
    QHash<QString,QPointer<QToolButton>> buttons;
};
}
