/* 
 * File:   TransientMessageWidget.h
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 *
 * Created on 16 octobre 2019, 18:25
 */

#ifndef TRANSIENTMESSAGEWIDGET_H
#define TRANSIENTMESSAGEWIDGET_H

#include <Wt/WText.h>
#include <Wt/WTimer.h>
#include <list>

using namespace Wt;
using namespace std;

/**
 * A transient stacked message zone.
 * By default, 8 messages can be stacked in a list shown
 * in the tool tip; each message is shown for 10 seconds. 
 */
class TransientMessageWidget : public WText
{
public:
    TransientMessageWidget();
    TransientMessageWidget(const WString& text);
    TransientMessageWidget(const WString& text, TextFormat textFormat);
    virtual ~TransientMessageWidget();

    void SetInterval(std::chrono::milliseconds interval)
    {
        m_timer.setInterval(interval);
    }

    std::chrono::milliseconds Interval()
    {
        return m_timer.interval();
    }

    void SetStackSize(uint sz)
    {
        m_stackSize = sz;
    }

    uint GetStackSize()
    {
        return m_stackSize;
    }
    /**
     * Main function to show and pile up messages.
     * @param text
     * @param stack
     */
    void SetText(const WString& text, bool stack = true);
private:
    uint m_stackSize;
    std::chrono::milliseconds m_interval;
    WTimer m_timer;
    list<WString> m_stack;

    void Init();
    void OnTimer();
};

#endif /* TRANSIENTMESSAGEWIDGET_H */

