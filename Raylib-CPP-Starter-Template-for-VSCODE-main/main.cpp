
#include "raylib.h"
#include"naivebayes.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <deque>
#include <cmath>
#include <cstring>

using namespace std;


Color bgClr = { 254, 249, 242, 255 };
Color btnClr = { 145, 212, 172, 255 };
Color borderClr = { 201, 233, 210, 255 }; // Set the border color


// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width*scaleFactor : font.glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize*scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize*scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
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
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}
int main() {
    NaiveBayes nb;
    nb.readData("messages.csv");

    // Initialize Raylib window
    const int screenWidth = 1000;
    const int screenHeight = 550;
    InitWindow(screenWidth, screenHeight, "Spam Detection");

    // Message input
    char message[256] = "\0";
    string result = "";
    bool predictPressed = false;
    SetTargetFPS(60);
    Font font = LoadFont("Roboto-Light.ttf");

    while (!WindowShouldClose()) {
        // Input handling
        if (IsKeyPressed(KEY_ENTER)) {
            predictPressed = true;
            result = nb.predict(message);
        }


        //Handling pasting from clipboard
        if (IsKeyPressed(KEY_V) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            const char *buffer = GetClipboardText(); // Get clipboard text
            if (buffer != NULL && buffer[0] != '\0') { // Check if the clipboard is not empty
                // Ensure we don't overflow the message buffer
                if (strlen(message) + strlen(buffer) < sizeof(message) - 1) {
                    strcat(message, buffer); // Append buffer to message
                }
            }
        }

        // Handling text input
        else{
            int key = GetKeyPressed();
            if (key >= 32 && key <= 125 && strlen(message) < 255) {
                int len = strlen(message);
                message[len] = (char)key;
                message[len + 1] = '\0';
            }
        }
        if (IsKeyDown(KEY_BACKSPACE) && strlen(message) > 0) {
            message[strlen(message) - 1] = '\0';  // Delete last character
        }

        // Start drawing
        BeginDrawing();
        ClearBackground(bgClr);

        // Draw input box
        // DrawText("Enter your message:", 100, 100, 20, DARKGRAY);
        DrawTextEx(font, "Enter you message: ",{100, 100} , 29, 1.0f, DARKGRAY);
        DrawRectangle(100, 150, 600, 100, LIGHTGRAY);
        // DrawText(message, 110, 160, 20, BLACK);

        //input text box
        float borderThickness = 2.0f; // Set the border thickness
        Rectangle inputBox = { 100, 150, 600, 200 };
        // Draw the border rectangle
        DrawRectangle(inputBox.x - borderThickness, inputBox.y - borderThickness,inputBox.width + 2 * borderThickness, inputBox.height + 2 * borderThickness, borderClr);
        // Rectangle container = { 100, 150, 600, 200 };

        // Draw the inner input rectangle
        DrawRectangle(inputBox.x, inputBox.y, inputBox.width, inputBox.height, WHITE);
        DrawTextBoxed(font, message, inputBox, 23, 1.0f, true, BLACK);

        // Predict button
        float predict_x = 450, predict_y = 400;
        DrawRectangle(predict_x, predict_y, 100, 40, btnClr);
        DrawText("Predict", predict_x+10, predict_y+10, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, {predict_x+10, predict_y+10, 100, 40})) {
                predictPressed = true;
                result = nb.predict(message);
            }
        }

        // Reset button
        float reset_x = 100, reset_y = 400;
        DrawRectangle(reset_x, reset_y, 100, 40, btnClr);
        DrawText("Reset", reset_x+10, reset_y+10, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, {reset_x+10, reset_y+10, 100, 40})) {
                strcpy(message, "");  // Clear the message input
                result = "";  // Clear the prediction result
                predictPressed = false;  // Reset the predictPressed flag
            }
        }

        // Show result
        if (predictPressed) {
            DrawText(result.c_str(), 350, 330, 20, result == "spam" ? RED : GREEN);
        }

        EndDrawing();
    }
        CloseWindow();
    return 0;
}
