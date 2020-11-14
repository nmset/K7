/* 
 * File:   TransientMessageWidget.cpp
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 * 
 * Created on 16 octobre 2019, 18:25
 */

#include "TransientMessageWidget.h"

TransientMessageWidget::TransientMessageWidget()
: WText()
{
    Init();
}

TransientMessageWidget::TransientMessageWidget(const WString& text)
: WText(text)
{
    Init();
}

TransientMessageWidget::TransientMessageWidget(const WString& text, TextFormat textFormat)
: WText(text, textFormat)
{
    Init();
}

TransientMessageWidget::~TransientMessageWidget()
{
    m_timer.stop();
}

void TransientMessageWidget::Init()
{
    m_stackSize = 8;
    m_interval = (std::chrono::milliseconds) 10 * 1000;
    m_timer.setSingleShot(true);
    SetInterval(m_interval);
    m_timer.timeout().connect(this, &TransientMessageWidget::OnTimer);
}

void TransientMessageWidget::OnTimer()
{
    WText::setText(WString::Empty);
}

void TransientMessageWidget::SetText(const WString& text, bool stack)
{
    m_timer.stop();
    if (stack)
    {
        // Remove the oldest message if stack is full
        if (m_stack.size() == m_stackSize)
        {
            m_stack.pop_back();
        }
        m_stack.push_front(text);
    }
    WText::setText(text);
    // List messages in the tool tip
    WString ttt = WString::Empty;
    list<WString>::iterator it;
    uint idx = m_stack.size();
    for (it = m_stack.begin(); it != m_stack.end(); it++)
    {
        ttt += WString(to_string(idx)) + WString(". ") + *it + WString("\n");
        idx--;
    }
    setToolTip(ttt);
    m_timer.start();
}