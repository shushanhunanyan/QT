#include "widget.h"
#include <QFile>
#include <QDebug>
#include <map>
#include <QCoreApplication>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    layout(new QGridLayout(this)),
    display(new QLineEdit("", this))
{
    setGeometry(800, 300, 300, 300);
    _createLayout();
    _createButtons();
    _setCustomStyle();
    _connectButtons();
}

Widget::~Widget()
{
    delete layout;
    delete display;
    for (int i = 0; i < m_digitCount; ++i) {
        delete buttons[i];
    }
    for (int i = 0; i < m_opCount; ++i) {
        delete operationButtons[i];
    }
}


void Widget::_createLayout()
{
    layout->addWidget(display, 0, 0, 1, 5);
    setFixedSize(350, 450);

    display->setAlignment(Qt::AlignRight);
    display->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    display->setReadOnly(false);
}


void Widget::_setCustomStyle()
{
    // Get the application's executable path
    QString executablePath = QCoreApplication::applicationDirPath();

    // Build the absolute path to the styles.qss file
    QString stylesFilePath = executablePath + "/qstyle/styles.qss";

    // Use the absolute path to open the QFile
    QFile styleFile(stylesFilePath);

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(styleFile.readAll());
        qDebug() << "Stylesheet applied successfully.";
    } else {
        qDebug() << "Failed to open stylesheet file:" << styleFile.errorString();
    }

   /* QFile styleFile(":/resources/styles.qss");

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(styleFile.readAll());
        qDebug() << "Stylesheet applied successfully.";
    } else {
        qDebug() << "Failed to open stylesheet file:" << styleFile.errorString();
    }*/
}

//don't look at magic numbers
void Widget::_createButtons()
{
    auto createButton = [this](const QString &text, int row, int col) {
        QPushButton *button = new QPushButton(text, this);
        button->setFixedSize(60, 60);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        layout->addWidget(button, row, col);
        return button;
    };

    for (int i = 1; i < m_digitCount; ++i)
    {
        int row = (9 - i) / 3 + 1;
        int col = (i - 1) % 3;
        buttons[i] = createButton(QString::number(i), row, col);
    }
    buttons[0] = createButton("0", 4, 0);
    buttons[0]->setFixedSize(130, 60);


    QString operations[] = {"/", "*", "-", "+"};
    int col = 3;
    int row = 1;


    for (int i = 0; i < std::size(operations); ++i)
    {
        operationButtons[i] = createButton(operations[i], row, col);
        ++row;
        if (i == 1) {
            col = 4;
            row = 1;
        }
    }

    operationButtons[4] = createButton(".", 4, 2);

    operationButtons[5] = new QPushButton("C", this);
    operationButtons[5]->setFixedSize(60, 130);
    operationButtons[5]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(operationButtons[5], 3, 3, 2, 1);

    operationButtons[6] = new QPushButton("=", this);
    operationButtons[6]->setFixedSize(60, 130);
    operationButtons[6]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(operationButtons[6], 3, 4, 2, 1);

}

//here too
void Widget::_connectButtons()
{
    for (int i = 0; i < m_digitCount; ++i)
    {
        connect(buttons[i], &QPushButton::clicked, this, &Widget::DisplayAction);
    }

    for (int i = 0; i < m_opCount; ++i)
    {
        if (i == 6) {
            connect(operationButtons[i], &QPushButton::clicked, this, &Widget::EqualButtonClicked);
        } else if (i == 5) {
            connect(operationButtons[i], &QPushButton::clicked, this, &Widget::ClearButtonClicked);
        } else {
            connect(operationButtons[i], &QPushButton::clicked, this, &Widget::DisplayAction);
        }
    }
}

void Widget::DisplayAction()
{
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());

    if (senderButton)
    {
        QString buttonText = senderButton->text();
        display->setText(display->text() + buttonText);
    }
}

void Widget::EqualButtonClicked()
{
    QString inputString = display->text();

    QString postfixExpression = _infixToPostfix(inputString);

    double result = _calculatePostfix(postfixExpression);
    display->setText(QString::number(result));
}

void Widget::ClearButtonClicked()
{
    display->clear();
}

int Widget::_getPrecedence(QChar op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    } else {
        return 0;
    }
}

double Widget::_calculatePostfix(const QString& postfix)
{
    QStack<double> stack;

    QStringList tokens = postfix.split(' ', QString::SkipEmptyParts);

    for (const QString& token : tokens) {
        if (token[0].isDigit() || (token.length() > 1 && (token[0] == '-' && token[1].isDigit()))) {
            stack.push(token.toDouble());
            qDebug() << token;
        } else if (_isOperator(token)) {
            if (stack.size() < 2) {
                qDebug() << "Invalid expression";
                return 0.0;
            }
            double operand2 = stack.pop();
            double operand1 = stack.pop();
            stack.push(_performOperation(token, operand1, operand2));
        } else {
            qDebug() << "Unsupported operator or invalid token:" << token;
            return 0.0;
        }
    }

    if (stack.size() == 1) {
        return stack.pop();
    } else {
        qDebug() << "Invalid expression";
        return 0.0;
    }
}

bool Widget::_isOperator(const QString& token)
{
    return token == "+" || token == "-" || token == "*" || token == "/";
}

double Widget::_performOperation(const QString& operatorToken, double operand1, double operand2)
{
    if (operatorToken == "+") {
        return operand1 + operand2;
    } else if (operatorToken == "-") {
        return operand1 - operand2;
    } else if (operatorToken == "*") {
        return operand1 * operand2;
    } else if (operatorToken == "/") {
        if (operand2 != 0) {
            return operand1 / operand2;
        } else {
            qDebug() << "Division by zero";
            return 0.0;
        }
    }
    return 0.0;
}


QString Widget::_infixToPostfix(const QString& infix)
{
    QString postfix;
    QStack<QChar> operators;

    QString number;

    for (const QChar& token : infix) {
        if (token.isDigit() || token == '.') {
            number.append(token);
        } else if (token == '-' && number.isEmpty()) {
            number.append(token);
        } else {
            if (!number.isEmpty()) {
                postfix.append(number).append(" ");
                number.clear();
            }

            while (!operators.isEmpty() && _getPrecedence(token) <= _getPrecedence(operators.top())) {
                postfix.append(operators.pop()).append(" ");
            }
            operators.push(token);
        }
    }

    if (!number.isEmpty()) {
        postfix.append(number).append(" ");
    }

    while (!operators.isEmpty()) {
        postfix.append(operators.pop()).append(" ");
    }

    postfix = postfix.trimmed();

    qDebug() << postfix;
    return postfix;
}
