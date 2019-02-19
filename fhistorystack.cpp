#include "fhistorystack.h"

FHistoryStack::FHistoryStack(QObject* parent) : QObject (parent)
  , m_curIndex(-1)
  , m_back(false)
  , m_forward(false)
{

}


QString FHistoryStack::forwardPath() const
{
    return m_curIndex < m_stack.size() - 1 && m_curIndex >= -1 ? m_stack[m_curIndex + 1] : QString();
}

QString FHistoryStack::backPath() const
{
    return m_curIndex > 0 && m_curIndex <= m_stack.size() ? m_stack[m_curIndex - 1] : QString();
}

QString FHistoryStack::currentPath() const
{
    return m_curIndex >= 0 && m_curIndex < m_stack.size() ? m_stack[m_curIndex] : QString();
}

void FHistoryStack::forward()
{
    if(m_curIndex < m_stack.size() - 1) m_curIndex++;
    if(m_curIndex > 0 && !m_back){
        m_back = true;
        emit sigBackEnable(true);
    }
    if(m_curIndex >= m_stack.size() - 1){
        m_forward = false;
        emit sigForwardEnable(false);
    }
}

void FHistoryStack::back()
{
    if(m_curIndex > 0) m_curIndex--;
    if(m_curIndex < m_stack.size() - 1 && !m_forward){
        m_forward = true;
        emit sigForwardEnable(true);
    }
    if(m_curIndex <= 0){
        m_back = false;
        emit sigBackEnable(false);
    }
}

void FHistoryStack::push(const QString& val)
{
    while(m_stack.size() - 1 > m_curIndex)
        m_stack.pop();
    m_stack.push(val);
    forward();
}
