import numpy as np
import pandas as pd
df = pd.read_csv('messages.csv')
# print(df.sample(5))
df.drop(columns=['Unnamed: 2', 'Unnamed: 3', 'Unnamed: 4'], inplace=True)

df=df.drop_duplicates(keep="first")
print(df["label"].value_counts())
df.to_csv('messages.csv', index=False)