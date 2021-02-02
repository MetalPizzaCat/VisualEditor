#pragma once
#include <QLabel>



class ToolBoxLabel :
    public QLabel
{
    Q_OBJECT

public:
    
    enum class Type :uint
    {
        Property,
        Object,
        CustomObject
    };

    ToolBoxLabel(QWidget* parent = Q_NULLPTR,Type type = Type::Property);
    ToolBoxLabel(QString text,QWidget* parent = Q_NULLPTR, Type type = Type::Property);
    ~ToolBoxLabel();

    Type ItemType = Type::Property;
protected:
    virtual void mousePressEvent(QMouseEvent* event)override;

    virtual void mouseReleaseEvent(QMouseEvent* event)override;
};

