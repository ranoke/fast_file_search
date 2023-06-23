import random
import string

# Number of words you want in the file
num_words = 10**8

with open("large_file.txt", "w") as f:
    for _ in range(num_words):
        # Generate a random word of length between 1 and 10
        word_length = random.randint(1, 10)
        word = ''.join(random.choices(string.ascii_lowercase, k=word_length))

        # Write the word to the file, followed by a space
        f.write(word + ' ')
