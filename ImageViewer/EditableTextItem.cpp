#include "EditableTextItem.h"
#include <QIntValidator>

EditableTextItem::EditableTextItem(const QString& text) : QGraphicsTextItem(text) {
    setFlags(QGraphicsItem::ItemIsMovable | // 允许移动
        QGraphicsItem::ItemIsFocusable | // 接收键盘事件
        QGraphicsItem::ItemIsSelectable);
    setTextInteractionFlags(Qt::TextEditorInteraction); // 启用编辑模式
}

// 焦点离开时保留移动功能
void EditableTextItem::focusOutEvent(QFocusEvent* event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
}

// 双击恢复编辑
void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}