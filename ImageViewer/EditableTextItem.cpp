#include "EditableTextItem.h"
#include <QIntValidator>

EditableTextItem::EditableTextItem(const QString& text) : QGraphicsTextItem(text) {
    setFlags(QGraphicsItem::ItemIsMovable | // �����ƶ�
        QGraphicsItem::ItemIsFocusable | // ���ռ����¼�
        QGraphicsItem::ItemIsSelectable);
    setTextInteractionFlags(Qt::TextEditorInteraction); // ���ñ༭ģʽ
}

// �����뿪ʱ�����ƶ�����
void EditableTextItem::focusOutEvent(QFocusEvent* event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
}

// ˫���ָ��༭
void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}