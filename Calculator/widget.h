#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QStyle>
#include <QToolButton>
#include <QStack>


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void DisplayAction();
    void EqualButtonClicked();
    void ClearButtonClicked();

private:
    static const int m_digitCount = 10;
    static const int m_opCount = 7;

private:
    QGridLayout *layout;
    QLineEdit *display;
    QPushButton *buttons[m_digitCount];
    QPushButton *operationButtons[m_opCount];

private:
    void _createLayout();
    void _createButtons();
    void _cetCustomStyle();
    void _connectButtons();
    void _setCustomStyle();
    double _calculatePostfix(const QString&);
    QString _infixToPostfix(const QString&);
    int _getPrecedence(QChar);
    bool _isOperator(const QString&);
    double _performOperation(const QString&, double, double);
};

#endif // WIDGET_H
