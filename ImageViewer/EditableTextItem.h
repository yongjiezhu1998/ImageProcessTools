#pragma once
#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    EditableTextItem(const QString& text = "");

protected:
    // 焦点离开时保留移动功能
    void focusOutEvent(QFocusEvent* event) override;

    // 双击恢复编辑
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneEvent* event);
};