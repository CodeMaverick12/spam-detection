#include <iostream>
#include "raylib.h"
using namespace std;
string trim(const string& str){
    size_t i=0;
    while(i<str.length() && str[i] == ' '){
        i++;
    }
    return str.substr(i);
}

//handling special char using shift key
char handlingSpecialChars(int key){
    switch (key) {
        case '`': return '~'; break;
        case '1': return '!'; break;
        case '2': return '@'; break;
        case '3': return '#'; break;
        case '4': return '$'; break;
        case '5': return '%'; break;
        case '6': return '^'; break;
        case '7': return '&'; break;
        case '8': return '*'; break;
        case '9': return '('; break;
        case '0': return ')'; break;
        case '-': return '_'; break;
        case '=': return '+'; break;
        case '[': return '{'; break;
        case ']': return '}'; break;
        case '\\': return '|'; break;
        case ';': return ':'; break;
        case '\'': return '\"'; break;
        case ',': return '<'; break;
        case '.': return '>'; break;
        case '/': return '?'; break;
        default: return (char)key;break; // Default to the character itself
    }
}
// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint, int *scrollOffsetY)
{
    int length = TextLength(text);  

    float textOffsetY = 0;          
    float textOffsetX = 0.0f;       

    float scaleFactor = fontSize / (float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;        
    int endLine = -1;          
    int lastk = -1;             
    for (int i = 0, k = 0; i < length; i++, k++) {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n') {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        if (state == MEASURE_STATE) {
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width) {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length) {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE) {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        } else {
            // Handle line breaks and drawing
            if (codepoint == '\n') {
                if (!wordWrap) {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            } else {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width)) {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // Draw the text only if it fits within the scrollable area
                if ((textOffsetY + font.baseSize * scaleFactor) > *scrollOffsetY && (textOffsetY - *scrollOffsetY) < rec.height) {
                    // Apply the scroll offset and only draw visible text
                    bool isGlyphSelected = false;
                    if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength))) {
                        DrawRectangleRec((Rectangle){ rec.x + textOffsetX - 1, rec.y + textOffsetY - *scrollOffsetY, glyphWidth, (float)font.baseSize * scaleFactor }, selectBackTint);
                        isGlyphSelected = true;
                    }

                    // Draw current character glyph
                    if ((codepoint != ' ') && (codepoint != '\t')) {
                        DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY - *scrollOffsetY }, fontSize, isGlyphSelected ? selectTint : tint);
                    }
                }
            }

            if (wordWrap && (i == endLine)) {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;

                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
    }
}
int Clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
