#include <UIEdit.h>
#include <UIPaintManager.h>
#include "EncodingTransform.h"
#include "UIResourceMgr.h"
#include "UIRect.h"
#include "UIRenderEngine.h"
#include <vector>
#include <iostream>

using namespace std;

struct UIEditInternal
{
    explicit        UIEditInternal(UIEdit *uiEdit);
    ~UIEditInternal();
    void            DoEvent(TEventUI &event);
    void            CreateImmContext();
    void            ReleaseImmContext();
    uint32_t        GetCharNumber()const{
        return m_text.length();
    }
private:
    void            ShowCaretAndSetImmPosition();
    uint32_t        CalculateTextOffset();
    void            CalculateForCharactersWidth();
    //从鼠标点时的坐标计算出当前光标应该插入到哪个字符后进行显示。
    void            CalculateCurrentEditPositionFromMousePoint(POINT pt);
    //初始化光标显示位置以及输入法跟随的位置。
    void            InitCaretAndImmPosition();
    //从当前编辑点处删除一个字符
    void            DeleteCharAtEditPosition();
    void            InsertCharAtEditPosition(wchar_t character);
private:
    std::wstring    m_text;
    UIPtrArray      m_textWidthList;
    UIEdit          *m_uiEdit;
    int             m_currentEditPos;
    HIMC            m_hIMC;

    void CalculateForPasswordCharactersWidth();

    void CalculateForNormalChartersWidth();
};

UIEditInternal::UIEditInternal(UIEdit *uiEdit)
        : m_uiEdit {uiEdit},
          m_currentEditPos {0},
          m_hIMC{nullptr}
{

}

UIEditInternal::~UIEditInternal() {
    if(m_hIMC!=nullptr){
        ::ImmReleaseContext(m_uiEdit->GetManager()->GetPaintWindow(),
                            m_hIMC);
    }
}

void UIEditInternal::DoEvent(TEventUI &event) {
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
    {
        this->CalculateForCharactersWidth();
        this->CalculateCurrentEditPositionFromMousePoint(event.ptMouse);
        this->InitCaretAndImmPosition();
    }
    if(event.Type == UIEVENT_KEYDOWN){
        if(event.wParam == VK_LEFT){
            if(m_currentEditPos != 0){
                m_currentEditPos--;
                this->ShowCaretAndSetImmPosition();
            }
        }
        if(event.wParam == VK_RIGHT){
            if(m_currentEditPos >= m_text.length()){
                m_currentEditPos = m_text.length();
            }else{
                m_currentEditPos += 1;
            }
            this->ShowCaretAndSetImmPosition();
        }
        if(event.wParam == VK_HOME){
            //响应Home键,光标定位到编辑框的开始处
            m_currentEditPos = 0;
            this->ShowCaretAndSetImmPosition();
        }
        if(event.wParam == VK_END){
            //响应End键，光标定位到编辑框文本结尾处
            m_currentEditPos = (int)m_text.length();
            this->ShowCaretAndSetImmPosition();
        }
        if(event.wParam == VK_DELETE){
            if(m_currentEditPos>=m_textWidthList.GetSize()){
                m_currentEditPos = m_textWidthList.GetSize();
            }else{
                m_currentEditPos++;
            }
            this->DeleteCharAtEditPosition();
        }
    }
    if(event.Type == UIEVENT_CHAR)
    {
        if(event.wParam == VK_BACK){
            this->DeleteCharAtEditPosition();
            return;
        }
        this->InsertCharAtEditPosition((wchar_t)event.wParam);
    }
}

uint32_t UIEditInternal::CalculateTextOffset() {
    uint32_t result = 0;
    for(int i=0;i<=m_currentEditPos && i<m_textWidthList.GetSize();i++){
        result += (int)m_textWidthList.GetAt(i);
    }
    return result;
}

void UIEditInternal::CalculateForCharactersWidth() {
    if(m_uiEdit->IsPasswordMode()){
        CalculateForPasswordCharactersWidth();
        return;
    }
    CalculateForNormalChartersWidth();
}

void UIEditInternal::CalculateForNormalChartersWidth() {
    wchar_t *wideText = Utf8ToUcs2(m_uiEdit->GetText().GetData());
    m_text = wstring{wideText};
    delete wideText;
    UIFont *fontHandle = UIResourceMgr::GetInstance().GetFont(m_uiEdit->GetFont());
    auto hOldFont = (HFONT) SelectObject(m_uiEdit->GetManager()->GetPaintDC(),
                                         fontHandle->GetHandle());
    SIZE szSpace = {0};
    m_textWidthList.Empty();
    m_textWidthList.Add((LPVOID)0);
    for(int i=0; i < m_text.length(); i++){
        GetTextExtentPoint32W(m_uiEdit->GetManager()->GetPaintDC(),
                              m_text.c_str() + i, 1, &szSpace);
        m_textWidthList.Add((LPVOID)szSpace.cx);
    }
    SelectObject(m_uiEdit->GetManager()->GetPaintDC(),
                 hOldFont);
}

void UIEditInternal::CalculateForPasswordCharactersWidth() {
    SIZE szSpace = {0};
    UIFont *fontHandle = UIResourceMgr::GetInstance().GetFont(m_uiEdit->GetFont());
    auto hOldFont = (HFONT) SelectObject(m_uiEdit->GetManager()->GetPaintDC(),
                                         fontHandle->GetHandle());
    char passwordChar = m_uiEdit->GetPasswordChar();
    GetTextExtentPoint32A(m_uiEdit->GetManager()->GetPaintDC(),
                          &passwordChar, 1, &szSpace);
    SelectObject(m_uiEdit->GetManager()->GetPaintDC(),
                 hOldFont);
    m_textWidthList.Empty();
    m_textWidthList.Add((LPVOID)0);
    for(int i=0; i < m_text.length(); i++){
        m_textWidthList.Add((LPVOID)szSpace.cx);
    }
}

void UIEditInternal::CalculateCurrentEditPositionFromMousePoint(POINT pt) {
    int    width = 9999;
    m_currentEditPos = -1;
    int totalLength = 0;
    for(int i=0; i < m_textWidthList.GetSize(); i++){
        totalLength += (int)m_textWidthList.GetAt(i);
        int textPos = m_uiEdit->GetPos().left + m_uiEdit->GetTextPadding().left + totalLength;
        if(abs(pt.x - textPos) < width){
            m_currentEditPos = i;
            width = abs(pt.x - textPos);
        }
    }
}

void UIEditInternal::DeleteCharAtEditPosition() {
    if(m_currentEditPos == 0){
        return;
    }
    m_text.erase(m_currentEditPos-1,1);
    m_textWidthList.Remove(m_currentEditPos);
    m_currentEditPos--;
    const char *utf8Text = Ucs2ToUtf8(m_text.c_str());
    m_uiEdit->SetText(UIString{utf8Text});
    this->ShowCaretAndSetImmPosition();
    delete []utf8Text;
}

void UIEditInternal::InsertCharAtEditPosition(wchar_t character) {
    wchar_t str[2] = {character, 0};
    m_text.insert(m_currentEditPos, str);
    m_currentEditPos++;
    char *utf8Text = Ucs2ToUtf8(m_text.c_str());
    m_uiEdit->SetText(UIString{utf8Text});
    delete []utf8Text;
    UIFont *fontHandle = UIResourceMgr::GetInstance().GetFont(m_uiEdit->GetFont());
    auto hOldFont = (HFONT)::SelectObject(m_uiEdit->GetManager()->GetPaintDC(),
                                          fontHandle->GetHandle());
    SIZE szSpace = {0};
    if(m_uiEdit->IsPasswordMode()){
        //文本框显示密码字符，这时需要计算密码字符的宽度
        char passwordChar = m_uiEdit->GetPasswordChar();
        ::GetTextExtentPoint32A(m_uiEdit->GetManager()->GetPaintDC(),
                                &passwordChar,1, &szSpace);
    }else{
        ::GetTextExtentPoint32W(m_uiEdit->GetManager()->GetPaintDC(),
                                str,1, &szSpace);
    }
    ::SelectObject(m_uiEdit->GetManager()->GetPaintDC(),hOldFont);
    m_textWidthList.InsertAt(m_currentEditPos, (LPVOID)szSpace.cx);
    this->ShowCaretAndSetImmPosition();
}

void UIEditInternal::InitCaretAndImmPosition() {
    RECT rcItem{m_uiEdit->GetPos()};
    RECT textPadding{m_uiEdit->GetTextPadding()};
    int caretHeight = rcItem.bottom - rcItem.top - textPadding.top - textPadding.bottom - 4;
    ::CreateCaret(m_uiEdit->GetManager()->GetPaintWindow(),nullptr, 1, caretHeight);
    ::SetCaretPos(rcItem.left + textPadding.left + this->CalculateTextOffset(),
                  rcItem.top + textPadding.top + 2);
    COMPOSITIONFORM composition;
    composition.dwStyle = CFS_POINT;
    composition.ptCurrentPos.x = rcItem.left + textPadding.left + this->CalculateTextOffset();
    composition.ptCurrentPos.y = rcItem.bottom;
    ::ImmSetCompositionWindow(m_hIMC, &composition);
    ::ShowCaret(m_uiEdit->GetManager()->GetPaintWindow());
}

void UIEditInternal::CreateImmContext() {
    m_hIMC = ::ImmGetContext(m_uiEdit->GetManager()->GetPaintWindow());
}

void UIEditInternal::ReleaseImmContext() {
    ::ImmReleaseContext(m_uiEdit->GetManager()->GetPaintWindow(), m_hIMC);
    m_hIMC = nullptr;
}

void    UIEdit::InitInternal() {
    m_internalImpl = make_shared<UIEditInternal>(this);
}

uint32_t UIEdit::GetCharNumber() const {
    return m_internalImpl->GetCharNumber();
}

void UIEdit::DoEvent(TEventUI &event) {
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_parent != nullptr ) m_parent->DoEvent(event);
        else UILabel::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
    {
        UILoadCursor(m_manager, UI_IDC_TEXT);
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS && IsEnabled() )
    {
        m_internalImpl->CreateImmContext();
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() )
    {
        m_internalImpl->ReleaseImmContext();
        ::HideCaret(m_manager->GetPaintWindow());
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
    {
        m_internalImpl->DoEvent(event);
        if( IsEnabled() ) {
            GetManager()->ReleaseCapture();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        UIRect rcItem {m_rcItem};
        if( rcItem.IsPtIn(event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_buttonState & UISTATE_HOT) == 0  ) {
                    m_buttonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        UIRect rcItem{m_rcItem};
        if( rcItem.IsPtIn(event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_buttonState & UISTATE_HOT) != 0  ) {
                    m_buttonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }
            if (m_manager) m_manager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_manager) m_manager->AddMouseLeaveNeeded(this);
            return;
        }
    }
    if(event.Type == UIEVENT_CHAR || event.Type == UIEVENT_KEYDOWN){
        m_internalImpl->DoEvent(event);
        return;
    }
    UILabel::DoEvent(event);
}

void UIEditInternal::ShowCaretAndSetImmPosition() {
    RECT rcItem{m_uiEdit->GetPos()};
    RECT textPadding{m_uiEdit->GetTextPadding()};
    ::HideCaret(m_uiEdit->GetManager()->GetPaintWindow());
    ::SetCaretPos(rcItem.left + textPadding.left + this->CalculateTextOffset(),
                  rcItem.top + textPadding.top + 2);
    ::ShowCaret(m_uiEdit->GetManager()->GetPaintWindow());
    COMPOSITIONFORM composition;
    composition.dwStyle = CFS_POINT;
    composition.ptCurrentPos.x = rcItem.left + textPadding.left + this->CalculateTextOffset();
    composition.ptCurrentPos.y = rcItem.bottom;
    ::ImmSetCompositionWindow(m_hIMC, &composition);
}

void UIEdit::DrawCaret() {
    
}

UIEdit::UIEdit()
    :m_maxChar{255},
    m_readOnly{false},
    m_numberOnly{false},
    m_passwordMode{false},
    m_passwordChar{'*'},
    m_autoSelAll{false},
    m_buttonState{0},
    m_editBkColor{0xFFFFFFFF}
{
    InitInternal();
    SetTextPadding(RECT{4,0, 4, 0});
    SetBkColor(0xFFFFFFFF);
}

uint32_t UIEdit::GetControlFlags() const {
    if(!IsEnabled()){
        return UILabel::GetControlFlags();
    }
    return UIFLAG_SETCURSOR|UIFLAG_TABSTOP;
}

UIString UIEdit::GetClass() const {
    return UIString{DUI_CTR_EDIT};
}

LPVOID UIEdit::GetInterface(const UIString &name) {
    if(name == DUI_CTR_EDIT){
        return static_cast<UIEdit*>(this);
    }
    return UILabel::GetInterface(name);
}

void UIEdit::SetEnabled(bool enabled) {
    UIControl::SetEnabled(enabled);
    if(!IsEnabled()){
        m_buttonState = 0;
    }
}

void UIEdit::SetMaxChar(uint32_t maxChar) {
    m_maxChar = maxChar;
}

uint32_t UIEdit::GetMaxChar() const {
    return m_maxChar;
}

void UIEdit::SetReadOnly(bool readOnly) {
    if(m_readOnly == readOnly){
        return;
    }
    m_readOnly = readOnly;
    Invalidate();
}

bool UIEdit::IsReadOnly() const {
    return m_readOnly;
}

void UIEdit::SetPasswordMode(bool passwordMode) {
    if(m_passwordMode == passwordMode){
        return;
    }
    m_passwordMode = passwordMode;
    Invalidate();
}

bool UIEdit::IsPasswordMode() const {
    return m_passwordMode;
}

void UIEdit::SetPasswordChar(char passwordChar) {
    if(m_passwordChar == passwordChar){
        return;
    }
    m_passwordChar = passwordChar;
    Invalidate();
}

char UIEdit::GetPasswordChar() const {
    return m_passwordChar;
}

bool UIEdit::IsAutoSelAll() const {
    return m_autoSelAll;
}

void UIEdit::SetAutoSelAll(bool autoSelAll) {
    m_autoSelAll = autoSelAll;
}

void UIEdit::SetNumberOnly(bool numberOnly) {
    m_numberOnly = numberOnly;
}

bool UIEdit::IsNumberOnly() const {
    return m_numberOnly;
}

UIString UIEdit::GetNormalImage() const {
    return m_diNormal.sDrawString;
}

void UIEdit::SetNormalImage(const UIString &normalImage) {
    if( m_diNormal.sDrawString == normalImage && m_diNormal.pImageInfo != nullptr ) return;
    m_diNormal.Clear();
    m_diNormal.sDrawString = normalImage;
    Invalidate();
}

UIString UIEdit::GetHotImage() const {
    return m_diHot.sDrawString;
}

void UIEdit::SetHotImage(const UIString &hotImage) {
    if( m_diHot.sDrawString == hotImage && m_diHot.pImageInfo != nullptr ) return;
    m_diHot.Clear();
    m_diHot.sDrawString = hotImage;
    Invalidate();
}

UIString UIEdit::GetFocusedImage() const {
    return m_diFocused.sDrawString;
}

void UIEdit::SetFocusedImage(const UIString &focusedImage) {
    if( m_diFocused.sDrawString == focusedImage && m_diFocused.pImageInfo != nullptr ) return;
    m_diFocused.Clear();
    m_diFocused.sDrawString = focusedImage;
    Invalidate();
}

UIString UIEdit::GetDisabledImage() const {
    return m_diDisabled.sDrawString;
}

void UIEdit::SetDisabledImage(const UIString &disabledImage) {
    if( m_diDisabled.sDrawString == disabledImage && m_diDisabled.pImageInfo != nullptr ) return;
    m_diDisabled.Clear();
    m_diDisabled.sDrawString = disabledImage;
    Invalidate();
}

void UIEdit::SetSel(long startChar, long endChar) {
    //TODO SetSel?
}

void UIEdit::SetSelAll() {
    //TODO SetSelAll
}

void UIEdit::SetReplaceSel(const UIString &replace) {
    //TODO SetReplaceSel ??
}

void UIEdit::SetVisible(bool visible) {
    UIControl::SetVisible(visible);
    if(!IsVisible()){
        m_manager->SetFocus(nullptr);
    }
}

void UIEdit::SetInternVisible(bool visible) {
    if(!IsVisible()){
        m_manager->SetFocus(nullptr);
    }
}

void UIEdit::SetAttribute(const char *pstrName, const char *pstrValue) {
    if( strcasecmp(pstrName, "readonly") == 0 ) SetReadOnly(strcasecmp(pstrValue, "true") == 0);
    else if( strcasecmp(pstrName, "numberonly") == 0 ) SetNumberOnly(strcasecmp(pstrValue, "true") == 0);
    else if( strcasecmp(pstrName, "password") == 0 ) SetPasswordMode(strcasecmp(pstrValue, "true") == 0);
    else if( strcasecmp(pstrName, "autoselall") == 0 ) SetAutoSelAll(strcasecmp(pstrValue, "true") == 0);
    else if( strcasecmp(pstrName, "maxchar") == 0 ) SetMaxChar(atoi(pstrValue));
    else if( strcasecmp(pstrName, "normalimage") == 0 ) SetNormalImage(UIString{pstrValue});
    else if( strcasecmp(pstrName, "hotimage") == 0 ) SetHotImage(UIString{pstrValue});
    else if( strcasecmp(pstrName, "focusedimage") == 0 ) SetFocusedImage(UIString{pstrValue});
    else if( strcasecmp(pstrName, "disabledimage") == 0 ) SetDisabledImage(UIString{pstrValue});
    else UILabel::SetAttribute(pstrName, pstrValue);
}

void UIEdit::PaintStatusImage(HANDLE_DC hDC) {
    if( IsFocused() ) m_buttonState |= UISTATE_FOCUSED;
    else m_buttonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_buttonState |= UISTATE_DISABLED;
    else m_buttonState &= ~ UISTATE_DISABLED;

    if( (m_buttonState & UISTATE_DISABLED) != 0 ) {
        if( DrawImage(hDC, m_diDisabled) ) return;
    }
    else if( (m_buttonState & UISTATE_FOCUSED) != 0 ) {
        if( DrawImage(hDC, m_diFocused) ) return;
    }
    else if( (m_buttonState & UISTATE_HOT) != 0 ) {
        if( DrawImage(hDC, m_diHot) ) return;
    }

    if( DrawImage(hDC, m_diNormal) ) return;
}

void UIEdit::PaintText(HANDLE_DC hDC) {
    if( m_textColor == 0 ) m_textColor = m_manager->GetDefaultFontColor();
    if( m_disabledTextColor == 0 ) m_disabledTextColor = m_manager->GetDefaultDisabledColor();
    this->DrawCaret();
    if( m_text.IsEmpty() ) return;

    //CDuiString sText = m_sText;
    UIString  sText{m_text};
    if( m_passwordMode ) {
        sText.Empty();
        uint32_t numberOfChar = this->GetCharNumber();
        while(numberOfChar--){
           sText += m_passwordChar;
        }
    }

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;
    if( IsEnabled() ) {
        UIRenderEngine::DrawText(hDC, m_manager, rc, sText, m_textColor, \
				m_fontId, DT_SINGLELINE | m_textStyle);
    }
    else {
        UIRenderEngine::DrawText(hDC, m_manager, rc, sText, m_disabledTextColor, \
				m_fontId, DT_SINGLELINE | m_textStyle);

    }
}
