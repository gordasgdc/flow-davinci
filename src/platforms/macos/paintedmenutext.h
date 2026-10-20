// SPDX-License-Identifier: MIT
#pragma once
#include <QPaintDevice>
#include <QPaintEngine>
#include <QPainter>
#include <QWidget>
namespace Unbound {
// A text-only paint sink for native custom-painted menu captions. No OCR,
// object-memory offsets, replacement styles, pixels or on-screen popup needed.
class MenuTextPaintEngine final : public QPaintEngine {
public:
    MenuTextPaintEngine():QPaintEngine(QPaintEngine::AllFeatures) {}
    QStringList text;
    bool begin(QPaintDevice *) override { return true; }
    bool end() override { return true; }
    Type type() const override { return QPaintEngine::User; }
    void updateState(const QPaintEngineState &) override {}
    void drawPixmap(const QRectF &,const QPixmap &,const QRectF &) override {}
    void drawPath(const QPainterPath &) override {}
    void drawPolygon(const QPointF *,int,PolygonDrawMode) override {}
    void drawTextItem(const QPointF &,const QTextItem &item) override {
        if(text.size()<32 && item.text().size()<=512)text.append(item.text());
    }
};
class MenuTextPaintDevice final : public QPaintDevice {
public:
    mutable MenuTextPaintEngine engine;
    QPaintEngine *paintEngine() const override { return &engine; }
protected:
    int metric(PaintDeviceMetric value) const override {
        switch(value) {
        case PdmWidth: return 2048;
        case PdmHeight: return 256;
        case PdmWidthMM: return 542;
        case PdmHeightMM: return 68;
        case PdmNumColors: return 16777216;
        case PdmDepth: return 32;
        case PdmDpiX: case PdmDpiY: case PdmPhysicalDpiX: case PdmPhysicalDpiY: return 96;
        case PdmDevicePixelRatio: return 1;
        case PdmDevicePixelRatioScaled: return QPaintDevice::devicePixelRatioFScale();
        }
        return 0;
    }
};
inline QStringList paintedMenuText(QWidget *widget) {
    if(!widget || widget->width()>2048 || widget->height()>256)return {};
    MenuTextPaintDevice device;
    QPainter painter(&device);
    widget->render(&painter,QPoint(),QRegion(widget->rect()),QWidget::DrawWindowBackground);
    painter.end();
    return device.engine.text;
}
}
