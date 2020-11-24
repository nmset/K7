/* 
 * File:   SensitiveTreeTableNodeText.h
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 *
 * Created on November 3, 2020, 8:26 PM
 */

#ifndef TREETABLENODETEXT_H
#define TREETABLENODETEXT_H

#include <Wt/WText.h>
#include <Wt/WTreeTableNode.h>
#include <Wt/WLineEdit.h>

using namespace Wt;

class TreeTableNodeText;
class TreeTableNodeLineEdit;

/**
 * An extended WText in a WTreeTableNode.
 * It is replaced by a TreeTableNodeLineEdit on click.
 * \n This allows to copy the text to clipboard.
 */
class TreeTableNodeText : public WText
{
public:
    TreeTableNodeText(WTreeTableNode * node, uint colIndex);
    TreeTableNodeText(const WString& text,
                      WTreeTableNode * node, uint colIndex);
    TreeTableNodeText(const WString& text, TextFormat textFormat,
                      WTreeTableNode * node, uint colIndex);
    virtual ~TreeTableNodeText();
    /**
     * Creates a read-only TreeTableNodeLineEdit replacing this
     * TreeTableNodeText.
     */
    void OnClick();
private:
    WTreeTableNode * m_node;
    uint m_colIndex;
    TreeTableNodeLineEdit * m_lineEdit;

    void Init(WTreeTableNode * node, uint colIndex);
};

/**
 * An extended WLineEdit replacing a WText in a WTreeTableNode.
 * Switches back to a WTreeTableNodeText when it loses focus.
 */
class TreeTableNodeLineEdit : public WLineEdit
{
public:
    TreeTableNodeLineEdit(WTreeTableNode * node, uint colIndex);
    TreeTableNodeLineEdit(const WString& content,
                          WTreeTableNode * node, uint colIndex);
    virtual ~TreeTableNodeLineEdit();
    /**
     * Move back the original TreeTableNodeText replacing this
     * TreeTableNodeLineEdit.
     */
    void OnBlurred();
    /**
     * Keep the original TreeTableNodeText verbatim.
     * @param text
     */
    void HoldSourceWidget(std::unique_ptr<WWidget> sourceWidget);
private:
    WTreeTableNode * m_node;
    uint m_colIndex;
    std::unique_ptr<WWidget> m_sourceWidget;

    void Init(WTreeTableNode * node, uint colIndex);
};

#endif /* TREETABLENODETEXT_H */

