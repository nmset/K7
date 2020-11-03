/* 
 * File:   SensitiveTreeTableNodeText.cpp
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 * 
 * Created on November 3, 2020, 8:26 PM
 */

#include "SensitiveTreeTableNodeText.h"
#include <cstdlib>

using namespace std;

TreeTableNodeText::TreeTableNodeText(WTreeTableNode * node, uint colIndex)
: WText()
{
    Init(node, colIndex);
}

TreeTableNodeText::~TreeTableNodeText()
{
}

TreeTableNodeText::TreeTableNodeText(const WString& text,
                                     WTreeTableNode * node, uint colIndex)
: WText(text)
{
    Init(node, colIndex);
}

TreeTableNodeText::TreeTableNodeText(const WString& text, TextFormat textFormat,
                                     WTreeTableNode * node, uint colIndex)
: WText(text, textFormat)
{
    Init(node, colIndex);
}

void TreeTableNodeText::Init(WTreeTableNode * node, uint colIndex)
{
    m_node = node;
    m_colIndex = colIndex;
    m_lineEdit = NULL;
    clicked().connect(this, &TreeTableNodeText::OnClick);
    setToolTip(WString::tr("PrepareCopy"));
}

void TreeTableNodeText::OnClick()
{
    m_lineEdit = new TreeTableNodeLineEdit(text(), m_node, m_colIndex);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->blurred().connect(m_lineEdit, &TreeTableNodeLineEdit::OnBlurred);
    m_lineEdit->setSelection(0, text().toUTF8().length());
    m_node->setColumnWidget(m_colIndex, unique_ptr<WLineEdit> (m_lineEdit));
    m_lineEdit->setFocus(true); // +++
}

///////////////////////////////////////////////////////////////////////////////

TreeTableNodeLineEdit::TreeTableNodeLineEdit(WTreeTableNode* node, uint colIndex)
: WLineEdit()
{
    Init(node, colIndex);
}

TreeTableNodeLineEdit::~TreeTableNodeLineEdit()
{
}

TreeTableNodeLineEdit::TreeTableNodeLineEdit(const WString& content,
                                             WTreeTableNode* node, uint colIndex)
: WLineEdit(content)
{
    Init(node, colIndex);
}

void TreeTableNodeLineEdit::Init(WTreeTableNode* node, uint colIndex)
{
    m_node = node;
    m_colIndex = colIndex;
    m_text = NULL;
    clicked().connect(this, &TreeTableNodeLineEdit::OnBlurred);
}

void TreeTableNodeLineEdit::OnBlurred()
{
    m_text = new TreeTableNodeText(text(), m_node, m_colIndex);
    m_text->clicked().connect(m_text, &TreeTableNodeText::OnClick);
    m_node->setColumnWidget(m_colIndex, unique_ptr<TreeTableNodeText> (m_text));
}
