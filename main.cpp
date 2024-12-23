#include "raylib.h"
#include "naiveBayes.h"
#include "utility.h"

using namespace std;

Color btnClr = {0, 90, 129, 255};
Color bgClr = {248, 248, 249, 255};
Color borderClr = {201, 233, 210, 255};

int main()
{
    NaiveBayes nb;
    nb.loadModelResults("./modelResults.txt");

    // Initialize Raylib window
    const int screenWidth = 1000;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Spam Detection");

    // loading ham and spam textures
    Image spamImage = LoadImage("./img/sp2.png");
    Image hamImage = LoadImage("./img/h2.png");
    Texture2D spamTexture = LoadTextureFromImage(spamImage);
    Texture2D hamTexture = LoadTextureFromImage(hamImage);
    hamTexture.width -= hamTexture.width / 3;
    hamTexture.height -= hamTexture.height / 3;
    spamTexture.height = hamTexture.height, spamTexture.width = hamTexture.width;
    UnloadImage(spamImage);
    UnloadImage(hamImage);

    // loading feedback section
    Image thumbsUp = LoadImage("./img/UpWObg.png");
    Image thumbsDown = LoadImage("./img/DownWObg.png");
    Texture2D good = LoadTextureFromImage(thumbsUp);
    Texture2D bad = LoadTextureFromImage(thumbsDown);

    // Message input
    char message[500] = "\0";
    string message_trim = "";
    string result = "";
    bool predictPressed = false;
    bool feedback = false;
    SetTargetFPS(60);

    // Start with no scroll offset (for scrolling purpose)
    int scrollOffsetY = 0;  

    Font myFont = LoadFont("./Roboto-Regular.ttf");

    while (!WindowShouldClose())
    {

        // Input handling
        

        // Handling pasting from clipboardu
        if (IsKeyPressed(KEY_V) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
        {
            // Get clipboard text
            const char *buffer = GetClipboardText(); 
            if (buffer != NULL && buffer[0] != '\0')
            { // Check if the clipboard is not empty
                // Ensure we don't overflow the message buffer
                if (strlen(message) + strlen(buffer) < sizeof(message) - 1)
                {
                    // Append buffer to message
                    strcat(message, buffer); 
                }
                
            }
        }

        // Handling text input
        else
        {
            int key = GetKeyPressed();
            if (((key >= 32 && key <= 126) || (key >= 128 && key <= 255)) && strlen(message) < 499)
            {
                int len = strlen(message);

                // Check if the pressed key is a letter (A-Z) and whether shift is held down
                if ((key >= 65 && key <= 90) || (key >= 97 && key <= 122))
                {
                    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
                    {
                        // Shift is pressed, capitalize letter
                        message[len] = (char)toupper(key);
                    }
                    else
                    {
                        // No shift, use lowercase
                        message[len] = (char)tolower(key);
                    }
                }
                else if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
                {
                    // For special characters using shift (numbers, symbols, etc.)
                    message[len] = handlingSpecialChars(key);
                }
                
                else
                {
                    message[len] = (char)key;
                }
                message[len + 1] = '\0'; // Null-terminate the string
            }
            int len = strlen(message);
            if (IsKeyPressed(KEY_ENTER) && strlen(message) > 0)
{
    message[len] = '\n';
    message[len + 1] = '\0'; // Ensure null termination
}
            
        }
        // Delete last character
        if (IsKeyReleased(KEY_BACKSPACE) && strlen(message) > 0)
        {
            message[strlen(message) - 1] = '\0';
        }

        // Start drawing
        BeginDrawing();
        ClearBackground(bgClr);

        // Draw input box
        DrawTextEx(myFont, "Enter your message: ", {100, 100}, 29, 1.0f, DARKGRAY);
        DrawRectangle(100, 150, 600, 100, LIGHTGRAY);

        // input text box
        float borderThickness = 2.0f;
        Rectangle inputBox = {100, 150, 600, 200};

        // Drawing the border rectangle
        DrawRectangle(inputBox.x - borderThickness, inputBox.y - borderThickness, inputBox.width + 2 * borderThickness, inputBox.height + 2 * borderThickness, GRAY);

        // Drawing the inner input rectangle
        DrawRectangle(inputBox.x, inputBox.y, inputBox.width, inputBox.height, WHITE);
        // DrawTextBoxed(myFont, message, inputBox, 23, 1.0f, true, BLACK);

        // handling scrolling feature
        if (IsKeyPressed(KEY_DOWN)) {
            scrollOffsetY += 10; // Scroll down
        }
        if (IsKeyPressed(KEY_UP)) {
            scrollOffsetY -= 10; // Scroll up
        }

        // Prevent scrolling past the top or bottom of the text box
        scrollOffsetY = Clamp(scrollOffsetY, 0, 600);

        // drawing message in input box
        DrawTextBoxedSelectable(myFont, message, {100, 155, 600, 180}, 22, 1, true, DARKGRAY, 0, 0, WHITE, LIGHTGRAY, &scrollOffsetY);
        
        //showing range of characters
        string range = to_string(strlen(message)) + "/500";
        DrawTextEx(myFont,range.c_str(), {650
        , 355}, 18, 0, BLACK);

        // Predict button
        float predict_x = 450, predict_y = 400;
        DrawRectangle(predict_x, predict_y, 100, 40, btnClr);
        DrawText("Predict", predict_x + 10, predict_y + 10, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, {predict_x, predict_y, 100, 40}))
            {
                message_trim = trim(message);
                if (message_trim.length() == 0)
                {
                    result = "";
                    // cout << message_trim << endl; // testing
                }
                else
                {
                    // cout << message_trim << endl; // testing
                    predictPressed = true;
                    result = nb.predict(message_trim);
                }
            }
        }

        // Reset button
        float reset_x = 100, reset_y = 400;
        DrawRectangle(reset_x, reset_y, 100, 40, btnClr);
        DrawText("Reset", reset_x + 10, reset_y + 10, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, {reset_x, reset_y, 100, 40}))
            {
                strcpy(message, "");    
                result = "";            
                predictPressed = false; 
                feedback = false;
            }
        }
        // Show result
        if (CheckCollisionPointRec(GetMousePosition(), {predict_x, predict_y, 100, 40}) || CheckCollisionPointRec(GetMousePosition(), {reset_x, reset_y, 100, 40}))
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        else if (CheckCollisionPointRec(GetMousePosition(), inputBox))
        {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        }
        else
        {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        if (predictPressed && message != "")
        {
            float posX = screenWidth, posY = screenHeight;
            if (result == "spam")
            {
                DrawTexture(spamTexture, posX - posX / 2.6, posY - posY / 1.25, WHITE);
            }
            if (result == "ham")
                DrawTexture(hamTexture, posX - posX / 2.6, posY - posY / 1.25, WHITE);

            // ask for feedback
            Rectangle goodRec = {posX - posX / 4.2, posY - posY / 4.8, 25, 25};
            Rectangle badRec = {posX - posX / 7.3, posY - posY / 4.8, 25, 25};

            if (!feedback && result != "")
            {
                DrawTextEx(myFont, "Rate this result ", {posX - posX / 4, posY - posY / 3}, 24, 1.0f, DARKGRAY);

                DrawTexture(good, posX - posX / 1.8, posY - posY / 2, GRAY);
                DrawTexture(bad, posX - posX / 2.2, posY - posY / 2, GRAY);

                if (CheckCollisionPointRec(GetMousePosition(), {predict_x, predict_y, 100, 40}) || CheckCollisionPointRec(GetMousePosition(), {reset_x, reset_y, 100, 40}) || CheckCollisionPointRec(GetMousePosition(), goodRec) || CheckCollisionPointRec(GetMousePosition(), badRec))
                {
                    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
                }
                else if (CheckCollisionPointRec(GetMousePosition(), inputBox))
                {
                    SetMouseCursor(MOUSE_CURSOR_IBEAM); 
                }
                else
                {
                    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (CheckCollisionPointRec(GetMousePosition(), badRec))
                {
                    feedback = true;
                    nb.updateModel((result == "spam" ? "ham" : "spam"), message);
                    cout << "Bad clicked!" << endl;
                }
                if (CheckCollisionPointRec(GetMousePosition(), goodRec))
                {
                    feedback = true;
                    cout << "good clicked!" << endl;
                }
            }
            if (feedback)
            {
                DrawTextEx(myFont, "Thanks for feedback!", {posX - posX / 4, posY - posY / 3}, 24, 1.0f, DARKGRAY);
            }
        }

        EndDrawing();
    }
    UnloadTexture(spamTexture);
    UnloadTexture(hamTexture);
    UnloadTexture(good);
    UnloadTexture(bad);
    UnloadFont(myFont);
    CloseWindow();
    return 0;
}

// code to train model
// int main(){
//     // reading data from file and storing it in modelResults.txt file without UI
//     NaiveBayes nb;
//     nb.readData("messages.csv");
//     nb.storeResults("modelResults.txt");
//     nb.loadModelResults("modelResults.txt");
//     nb.checking();
//     return 0;

// }
