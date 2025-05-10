#pragma once
#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    EditableTextItem(const QString& text = "");

protected:
    // �����뿪ʱ�����ƶ�����
    void focusOutEvent(QFocusEvent* event) override;

    // ˫���ָ��༭
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneEvent* event);
};