#include "e2d_label.h"
#include "e2d_text_component.h"
#include "e2d_transform_component.h"

/* Easy2D */
using namespace Easy2D;

UILabel::UILabel(const String& name) : UIWidget(name)
{
}

UILabel::~UILabel(void)
{
}

bool UILabel::setup()
{
    if (!Entity::setup())
    {
        LOG_WARN << ("UILabel::setup: Entity setup failed!");
        return false;
    }
    mTextComponent = createComponent<TextComponent>();
    if (nullptr == mTextComponent)
    {
        LOG_WARN << ("UILabel::setup: create TextureComponent failed!");
        return false;
    }
    mTextComponent->setHUDEnabled(true);
    return true;
}

void UILabel::setHorizontalAlign(HorizontalAlign align)
{
    mTextComponent->setHorizontalAlign(align);
}

void UILabel::setVerticalAlign(VerticalAlign align)
{
    mTextComponent->setVerticalAlign(align);
}

void UILabel::setText(const Wtring& text)
{
    mTextComponent->setText(text);
}

const Wtring& UILabel::getText() const
{
    return mTextComponent->getText();
}

void UILabel::setColor(const Color& color)
{
    mTextComponent->setColor(color);
}

const Color& UILabel::getColor() const
{
    return mTextComponent->getColor();
}

void UILabel::setShadowColor(const Color& color, uint16 shodowSize /* = 1 */)
{
    mTextComponent->setShadowColor(color, shodowSize);
}

const Color& UILabel::getShadowColor() const
{
    return mTextComponent->getShadowColor();
}

void UILabel::setOutlineColor(const Color& color, uint16 outlineSize /* = 1 */)
{
    mTextComponent->setOutlineColor(color, outlineSize);
}

const Color& UILabel::getOutlineColor() const
{
    return mTextComponent->getOutlineColor();
}

void UILabel::setBold(bool bold)
{
    mTextComponent->setBold(bold);
}

bool UILabel::isBold() const
{
    return mTextComponent->isBold();
}

void UILabel::setItalic(bool italoc)
{
    mTextComponent->setItalic(italoc);
}

bool UILabel::isItalic() const
{
    return mTextComponent->isItalic();
}

void UILabel::setLineWidth(uint32 lineWrap)
{
    mTextComponent->setLineWidth(lineWrap);
}

uint32 UILabel::getLineWidth() const
{
    return mTextComponent->getLineWidth();
}

bool UILabel::isLineWarp() const
{
    return mTextComponent->isLineWarp();
}

void UILabel::setContentFollow(bool follow)
{
    mTextComponent->setContentFollow(follow);
}

bool UILabel::isContentFollow() const
{
    return mTextComponent->isContentFollow();
}

void UILabel::setSpacing(uint32 spacing)
{
    mTextComponent->setSpacing(spacing);
}

uint32 UILabel::getSpacing() const
{
    return mTextComponent->getSpacing();
}

void UILabel::setFrameOffset(uint32 offset)
{
    mTextComponent->setFrameOffset(offset);
}

uint32 UILabel::getFrameOffset() const
{
    return mTextComponent->getFrameOffset();
}

bool UILabel::setFont(const String& fontName)
{
    auto font = FontManager::instance()->getFont(fontName);
    if (nullptr == font)
    {
        LOG_ERROR << "UILabel::setFont error: font(" << fontName << ") not load!";
        return false;
    }
    setFont(font);
    return true;
}

void UILabel::setFont(const SPtr<Font> font)
{
    mTextComponent->setFont(font);
}

const SPtr<Font> UILabel::getFont() const
{
    return mTextComponent->getFont();
}

