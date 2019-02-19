#ifndef FHISTORYSTACK_H
#define FHISTORYSTACK_H

#include <QStack>
#include <QObject>

class FHistoryStack : public QObject
{
    Q_OBJECT
public:
    FHistoryStack(QObject* parent = nullptr);
    QString forwardPath() const;    //一次前进或返回需要判断路径是否仍然存在，需要分开两步
    QString backPath() const;
    QString currentPath() const;
    void forward();
    void back();
    void push(const QString&);
    inline bool isEmpty() {return m_stack.isEmpty() || m_curIndex == -1;}

signals:
    void sigBackEnable(bool);
    void sigForwardEnable(bool);

private:
    QStack<QString> m_stack;  //QStack不方便继承
    int m_curIndex;
    bool m_back;
    bool m_forward;
};

#endif // FHISTORYSTACK_H
