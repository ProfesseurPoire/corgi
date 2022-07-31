#include <corgi/main/Game.h>
#include <corgi/ui/TextBox.h>
using namespace corgi::ui;

TextBox::TextBox(const std::string& text)
    : mText(mLocalText)
{
}

TextBox::TextBox(std::reference_wrapper<std::string> text)
    : mText(text.get())
{
}

void TextBox::init()
{
    initializeRectangle();
    initializeSelectionRectangle();
    initializeText();
    initializeVerticalBar();

    mOnMouseClick += [&](int x, int y)
    {
        //mPropagateEvent = false;
    };
}

TextBox::Point TextBox::cursorPosition(int cursorPosition)
{
    Point offset;

    if(uiText_->shapedGlyphs().size() == 0)
    {
        return {0, 0};
    }

    if(cursorPosition == 0)
    {
        return {uiText_->distanceLeft(0), uiText_->text_top_distance()};
    }

    int line = 0;
    offset.y += uiText_->text_top_distance();

    for(int i = 1; i <= cursorPosition; i++)
    {
        if(uiText_->shapedGlyphs()[i - 1].codepoint == 0)
        {
            offset.y += uiText_->font_view.height();
            offset.x = 0;
            line++;
            continue;
        }

        offset.x += uiText_->shapedGlyphs()[i - 1].advance.x;
    }

    offset.x += uiText_->distanceLeft(line);
    return offset;
}

void TextBox::updateCursorRealPosition()
{
    const auto& shapedGlyphs = uiText_->shapedGlyphs();
    auto        offset       = cursorPosition(cursorIndexPosition_);

    mVerticalBar->setLeft(offset.x);
    mVerticalBar->setTop(offset.y);
}

corgi::Event<std::string>& TextBox::textUpdated()
{
    return mTextUpdated;
}

void TextBox::initializeSelectionRectangle()
{
    selectionRectangles_ = mRectangle->emplaceBack<corgi::ui::Widget>();
    selectionRectangles_->disableEvents();
}

void TextBox::initializeRectangle()
{
    mRectangle = &emplace_back<corgi::ui::Rectangle>();
    mRectangle->set_name("TextBox Background Rectangle");

    mRectangle->setAnchorsToFillParentSpace();
    mRectangle->setRadius(10);
    mRectangle->setPropagateEvent(true);

    onMouseEnter() += [&](int x, int y)
    {
        mRectangle->setColor(mBackgroundColor);
    };

    onMouseExit() += [&](int x, int y)
    {
        mRectangle->setColor(mMouseOverBackgroundColor);
    };
}

void TextBox::initializeVerticalBar()
{
    mVerticalBar = &emplace_back<Rectangle>();

    mVerticalBar->setWidth(1);
    mVerticalBar->setTop(5);
    mVerticalBar->setHeight(uiText_->font_view.height());

    mVerticalBar->setColor(Color(0, 0, 0, 255));
    mVerticalBar->disable();
    mVerticalBar->setPropagateEvent(true);
}

void TextBox::setText(const std::string& str)
{
    mText = str;

    if(uiText_)
    {
        uiText_->text(mText.c_str());
        mTextUpdated(str);
    }
}

std::string TextBox::text()
{
    return mText.c_str();
}

void TextBox::setBackgroundColor(Color color)
{
    mBackgroundColor = color;
}

void TextBox::setBackgroundMouseOverColor(Color color)
{
    mMouseOverBackgroundColor = color;
}

void TextBox::setTextColor(Color color)
{
    uiText_->setColor(color);

    if(uiText_)
        uiText_->text(mText.c_str());
}

void TextBox::setHorizontalAlignment(HorizontalAlignment align)
{
    uiText_->setHorizontalAlignment(align);
}

void TextBox::removeRectangle()
{
    remove(mRectangle);
}

corgi::ui::Rectangle& TextBox::getRectangle()
{
    return *mRectangle;
}

corgi::ui::Text& TextBox::textWidget()
{
    return *uiText_;
}

int TextBox::findCursorIndex(int x, int y)
{
    //TODO : This function is actually quite a mess
    auto line = uiText_->selectedLine(x, y);
    if(line == -1)
        return 0;

    double offset = uiText_->real_x() + uiText_->distanceLeft(line);

    int index       = 0;
    int currentLine = 0;
    for(auto glyph : uiText_->shapedGlyphs())
    {
        if(glyph.codepoint == 0)
        {
            currentLine++;
            index++;
            continue;
        }

        if(currentLine != line)
        {
            index++;
            continue;
        }

        if(x >= offset && x <= offset + glyph.advance.x)
        {
            if(x >= offset + (glyph.advance.x / 2.0f))
            {
                return corgi::math::clamp(index + 1, 0, uiText_->shapedGlyphs().size());
            }

            return corgi::math::clamp(index, 0, uiText_->shapedGlyphs().size());
        }
        offset += glyph.advance.x;
        index++;
    }
    return 0;
}

bool TextBox::isCursorOutOfBound(int x, int y)
{
    auto line = uiText_->selectedLine(x, y);
    if(line == -1)
        return true;

    double offset = uiText_->real_x() + uiText_->distanceLeft(line);

    int index       = 0;
    int currentLine = 0;

    for(auto glyph : uiText_->shapedGlyphs())
    {
        if(glyph.codepoint == 0)
        {
            currentLine++;
            index++;
            continue;
        }

        if(currentLine != line)
        {
            index++;
            continue;
        }

        if(x >= offset && x <= offset + glyph.advance.x)
        {
            if(x >= offset + (glyph.advance.x / 2.0f))
            {
                return false;
            }

            return false;
        }
        offset += glyph.advance.x;
        index++;
    }
    return true;
}

int TextBox::cursorAtEndLine(int lineIndex)
{
    int currentLine = 0;
    int index       = 0;

    for(auto glyph : uiText_->shapedGlyphs())
    {
        if(glyph.codepoint == 0)
        {
            if(lineIndex == currentLine)
            {
                return index;
            }
            currentLine++;
        }
        index++;
    }
    return 0;
}

int TextBox::cursorAtStartLine(int lineIndex)
{
    if(lineIndex == 0)
        return 0;

    int currentLine = 0;
    int index       = 0;

    for(auto glyph : uiText_->shapedGlyphs())
    {
        if(glyph.codepoint == 0)
        {
            currentLine++;
            if(lineIndex == currentLine)
            {
                if((index + 1) < uiText_->shapedGlyphs().size())
                    return index + 1;
            }
        }
        index++;
    }
    return 0;
}

void TextBox::createSelectionRectangles(int x, int y)
{
    selectionRectangles_->clearChildren();

    int firstIndex = startSelectionIndex_;

    bool outOfBound = isCursorOutOfBound(x, y);

    auto secondIndex = findCursorIndex(x, y);

    //secondSelectionIndex_ = findCursorIndex(x, y);

    // Basically here we save the second cursor so if we're out of bound we use the last one
    // we hav
    if(!outOfBound)
    {
        secondSelectionIndex_ = secondIndex;
    }
    else
    {
        secondIndex = secondSelectionIndex_;
    }

    if(firstIndex > secondIndex)
    {
        auto temp   = secondIndex;
        secondIndex = firstIndex;
        firstIndex  = temp;
    }

    // First case, both are on the same line
    auto firstCursorLine  = cursorLinePosition(firstIndex);
    auto secondCursorLine = cursorLinePosition(secondIndex);

    // Both cursor are on the same line
    if(firstCursorLine == secondCursorLine)
    {
        auto firstCursorPosition  = cursorPosition(firstIndex);
        auto secondCursorPosition = cursorPosition(secondIndex);

        auto rectangle = selectionRectangles_->emplaceBack<corgi::ui::Rectangle>();

        rectangle->setLeft(firstCursorPosition.x);
        rectangle->setWidth(secondCursorPosition.x - firstCursorPosition.x);
        rectangle->setHeight(uiText_->font_view.height());
        rectangle->setTop(firstCursorPosition.y);
        rectangle->setColor(Color(11, 104, 217));
        rectangle->setPropagateEvent(true);
    }
    else
    {
        // We first handle the first line

        auto firstCursorPosition        = cursorPosition(firstIndex);
        auto endFirstLineCursorIndex    = cursorAtEndLine(firstCursorLine);
        auto endFirstLineCursorPosition = cursorPosition(endFirstLineCursorIndex);

        auto rectangle = selectionRectangles_->emplaceBack<corgi::ui::Rectangle>();
        rectangle->setLeft(firstCursorPosition.x);
        rectangle->setWidth(endFirstLineCursorPosition.x - firstCursorPosition.x);
        rectangle->setHeight(uiText_->font_view.height());
        rectangle->setTop(endFirstLineCursorPosition.y);
        rectangle->setColor(Color(11, 104, 217));
        rectangle->setPropagateEvent(true);

        // We handle the other lines, if any

        for(int i = firstCursorLine + 1; i < secondCursorLine; i++)
        {
            auto lineStartCursor = cursorPosition(cursorAtStartLine(i));
            auto lineEndCursor   = cursorPosition(cursorAtEndLine(i));

            auto rect = selectionRectangles_->emplaceBack<corgi::ui::Rectangle>();
            rect->setLeft(lineStartCursor.x);
            rect->setWidth(lineEndCursor.x - lineStartCursor.x);
            rect->setHeight(uiText_->font_view.height());
            rect->setTop(lineEndCursor.y);
            rect->setColor(Color(11, 104, 217));
            rect->setPropagateEvent(true);
        }

        // We handle the last line now

        auto secondCursorPosition = cursorPosition(secondIndex);
        auto lastLinePosition     = cursorPosition(cursorAtStartLine(secondCursorLine));

        auto lastRectangle = selectionRectangles_->emplaceBack<corgi::ui::Rectangle>();
        lastRectangle->setLeft(lastLinePosition.x);
        lastRectangle->setWidth(secondCursorPosition.x - lastLinePosition.x);
        lastRectangle->setHeight(uiText_->font_view.height());
        lastRectangle->setTop(lastLinePosition.y);
        lastRectangle->setColor(Color(11, 104, 217));
        lastRectangle->setPropagateEvent(true);
    }

    // double startGlyph = 0.0;
    // double endGlyph   = 0.0;

    // if(startSelectionIndex_ != 0)
    // {
    //     for(int i = 0; i < startSelectionIndex_; i++)
    //     {
    //         startGlyph += uiText_->shapedGlyphs()[i].advance.x;
    //     }
    // }

    // if(startSelectionIndex_ > secondIndex)
    // {
    //     for(int i = 0; i < secondIndex; i++)
    //     {
    //         endGlyph += uiText_->shapedGlyphs()[i].advance.x;
    //     }
    // }
    // else
    // {
    //     for(int i = 0; i <= secondIndex; i++)
    //     {
    //         endGlyph += uiText_->shapedGlyphs()[i].advance.x;
    //     }
    // }
}

int TextBox::cursorLinePosition(int index)
{
    int line = 0;

    for(int i = 0; i < index; i++)
    {
        if(uiText_->shapedGlyphs()[i].codepoint == 0)
        {
            line++;
        }
    }
    return line;
}

void TextBox::initializeText()
{
    uiText_ = &mRectangle->emplace_back<corgi::ui::Text>(mText.c_str());
    uiText_->setAnchorsToFillParentSpace();
    uiText_->setPropagateEvent(true);
    uiText_->setDepth(0.1f);

    // On mouse click we move the cursor depending on the character we clicked on
    uiText_->mouseDownEvent() += [&](const Mouse& mouse)
    {
        selectionRectangles_->clearChildren();
        cursorIndexPosition_ = findCursorIndex(mouse.x(), mouse.y());
    };

    uiText_->mouse_drag_start_event() += [&](const Mouse& mouse)
    {
        startSelectionIndex_ = findCursorIndex(mouse.x(), mouse.y());
    };

    uiText_->mouse_drag_event() += [&](const Mouse& mouse)
    {
        createSelectionRectangles(mouse.x(), mouse.y());
    };

    uiText_->mouse_drag_end_event() += [&](const Mouse& mouse)
    {
        //selection_rectangle_->disable();
    };
}

void TextBox::setCursorFlashingTime(float value)
{
    cursorFlashingTime_ = value;
}

float TextBox::cursorFlashingTime() const noexcept
{
    return cursorFlashingTime_;
}

void TextBox::update(float elapsedTime)
{
    bool addedCharacter = false;
    if(std::string(uiText_->text().c_str()) != mText)
        uiText_->text(mText.c_str());

    if(mFocus)
    {
        mElapsedTimeSinceLastVerticalTick += elapsedTime;

        if(mElapsedTimeSinceLastVerticalTick > cursorFlashingTime_)
        {
            if(mVerticalBar->isEnabled())
            {
                mVerticalBar->disable();
            }
            else
            {
                mVerticalBar->enable();
            }
            mElapsedTimeSinceLastVerticalTick = 0.0f;
        }

        bool updateTextMesh = false;

        if(Game::instance().inputs().keyboard().key_down(Key::Left))
        {
            cursorIndexPosition_ = corgi::math::clamp(cursorIndexPosition_ - 1, 0,
                                                      uiText_->shapedGlyphs().size());
        }

        if(Game::instance().inputs().keyboard().key_down(Key::Right))
        {
            cursorIndexPosition_ = corgi::math::clamp(cursorIndexPosition_ + 1, 0,
                                                      uiText_->shapedGlyphs().size());
            std::cout << uiText_->shapedGlyphs().size() - 1 << std::endl;
        }

        for(const auto& key : Game::instance().inputs().keyboard().keyPressed())
        {
            int keyCode = static_cast<int>(key);

            if(key == corgi::Key::Return)
            {
                updateTextMesh = true;

                int textOffset = 0;

                if(cursorIndexPosition_ != 0)
                {
                    textOffset =
                        uiText_->shapedGlyphs()[cursorIndexPosition_ - 1].textOffset + 1;
                }
                addedCharacter = true;
                mText.insert(mText.begin() + textOffset, '\n');
            }

            if(key == corgi::Key::Backspace)
            {
                if(cursorIndexPosition_ > 0)
                {
                    std::string s = mText.c_str();

                    auto textOffset =
                        uiText_->shapedGlyphs()[cursorIndexPosition_ - 1].textOffset;

                    auto charactersSize =
                        uiText_->shapedGlyphs()[cursorIndexPosition_ - 1]
                            .characters.size();

                    s.erase(s.begin() + textOffset);
                    mText          = s.c_str();
                    updateTextMesh = true;

                    int glyphCount = uiText_->shapedGlyphs().size();

                    setText(mText);

                    cursorIndexPosition_ -= glyphCount - uiText_->shapedGlyphs().size();

                    continue;
                }
            }

            if(keyCode == ' ')
            {
                updateTextMesh = true;
                mText += " ";
                cursorIndexPosition_++;
                continue;
            }

            if(keyCode >= '0' && keyCode <= '@' || keyCode >= 'a' && keyCode <= 'z')
            {
                auto mod = Game::instance().inputs().keyboard().getKeyModifiers();
                // We try to check if either shift or caps is pressed
                // if they are, we write caps versions
                auto mask        = (static_cast<int>(KeyModifiers::SHIFT) |
                             static_cast<int>(KeyModifiers::CAPS));
                auto isInCapMode = (mod & mask) != 0;

                updateTextMesh = true;

                int textOffset = 0;

                if(cursorIndexPosition_ != 0)
                {
                    textOffset =
                        uiText_->shapedGlyphs()[cursorIndexPosition_ - 1].textOffset + 1;
                }

                if(isInCapMode)
                {
                    if(keyCode >= 'a' && keyCode <= 'z')
                    {
                        mText.insert(mText.begin() + textOffset,
                                     static_cast<char>(key) - 32);
                    }
                }
                else
                    mText.insert(mText.begin() + textOffset, static_cast<char>(key));

                addedCharacter = true;
            }

            if(updateTextMesh)
            {
                int glyphCount = uiText_->shapedGlyphs().size();

                setText(mText);

                if(addedCharacter)
                {
                    addedCharacter = false;
                    cursorIndexPosition_ += uiText_->shapedGlyphs().size() - glyphCount;
                }
            }
        }
    }
    else
    {
        mVerticalBar->disable();
    }
    updateCursorRealPosition();
}
