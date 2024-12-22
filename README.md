# DESCRIPTION OF OUR PROJECT
This project implements a Naive Bayes classifier to classify messages as either ham (non-spam) or spam based on their content. The system is designed to preprocess text data, remove stop words, tokenize the input, and apply the Naive Bayes algorithm to predict whether a given message is spam or ham.

The code allows users to train the model on labeled messages, predict the label of new messages, and even update the model based on user feedback. It uses techniques like Laplace Smoothing to handle the issue of zero frequency for unseen words during prediction.
# FEATURES
Text Preprocessing:

Removal of special characters and punctuation.
Conversion of text to lowercase for case-insensitive comparison.
Tokenization of messages into words.
Removal of common stop words (e.g., "is", "the", "and", etc.).
Naive Bayes Classifier:

The classifier uses the Naive Bayes algorithm to calculate the probabilities of a message being spam or ham based on the frequency of words in spam and ham messages.
Laplace smoothing is applied to avoid the zero-frequency problem, where words that are not found in the training data are assigned a small probability.
Training the Model:

The system reads a CSV file containing labeled messages (spam or ham) and builds frequency distributions for words in both categories (spam and ham).
It stores the trained model, including word counts, probabilities, and other relevant metrics.
Prediction:

The system predicts whether a new message is spam or ham based on the trained model.
Updating the Model:

If a user provides feedback on the prediction (i.e., marks it as spam or ham), the model can be updated dynamically to adjust word frequencies accordingly.
File Handling:

The system can read training data from a file, predict labels for messages in a file, and store results in an output file.
Model Storage:

After training, the model's state (word counts, spam/ham probabilities, etc.) is saved to a file, allowing for persistence and reusability of the model without needing to retrain it.
# WORKING OF OUR MODEL
Text Preprocessing:

The CleanData class is responsible for cleaning and preprocessing the text data. It removes special characters, converts the text to lowercase, tokenizes the message into words, and removes common stop words.
Training:

The NaiveBayes class reads the labeled training data (spam and ham messages) from a CSV file. It then tokenizes each message, counts the frequency of each word, and stores these counts separately for spam and ham messages. The class also calculates probabilities based on these frequencies.
Prediction:

When a new message is received for classification, the system tokenizes the message and computes the probabilities of it being spam or ham based on the trained model.
The model uses Laplace smoothing to handle words that did not appear in the training data.
User Feedback and Model Update:

If the prediction is incorrect, the user can provide feedback (marking a message as spam or ham). The model is then updated by adjusting the word counts and recalculating the probabilities.
Storing and Loading the Model:

After training, the model can be saved to a file and loaded later for predictions. This eliminates the need to retrain the model every time.
# HOW TO RUN THE MAIN.EXE FILE
After configuring the raylib setup on your system by following the below link you have do the following setups.
1.First you have to open the man.code-workplace.
2.Click on main.cpp file.
3.You just have to press control+f5 button in order to run the main.exe file.It will automatically do the linking work and run your program automatically with out mentioning the commands on the terminal.






# Video Tutorial

<p align="center">
  <img src="preview.jpg" alt="" width="800">
</p>

<p align="center">
🎥 <a href="https://www.youtube.com/watch?v=PaAcVk5jUd8">Video Tutorial on YouTube</a>
</p>

<br>
<br>
<p align="center">
| 📺 <a href="https://www.youtube.com/channel/UC3ivOTE5EgpmF2DHLBmWIWg">My YouTube Channel</a>
| 🌍 <a href="http://www.educ8s.tv">My Website</a> | <br>
</p>

