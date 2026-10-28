#pragma once
// Flow DaVinci: catalogul central de traduceri (română). Textul original (engleză) este cheia;
// lipsa unei chei returnează textul original, deci sincronizarea cu upstream nu pierde texte noi.
// FLOW_DAVINCI_LANG=en dezactivează traducerea (testele upstream compară textul englezesc).
#include <QByteArray>
#include <QHash>
#include <QString>
#include <cstdlib>
#include <cstring>
#include "flowlocale_ro.h"

namespace Flow {
inline bool translationEnabled() {
    static const bool enabled = [] { const char *v = std::getenv("FLOW_DAVINCI_LANG"); return !(v && std::strcmp(v, "en") == 0); }();
    return enabled;
}
inline QString translate(const char *source) {
    if (translationEnabled()) {
        static const QHash<QByteArray, QString> table = [] {
            QHash<QByteArray, QString> h;
            for (const auto &e : kRomanianCatalog) h.insert(QByteArray(e.source), QString::fromUtf8(e.target));
            return h;
        }();
        const auto it = table.constFind(QByteArray(source));
        if (it != table.constEnd()) return it.value();
    }
    return QString::fromUtf8(source);
}
}
inline QString FD(const char *source) { return Flow::translate(source); }
