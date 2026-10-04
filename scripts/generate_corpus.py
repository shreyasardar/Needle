import random

random.seed(42)

topics = [
    "computer networks",
    "operating systems",
    "database systems",
    "distributed computing",
    "search engines",
    "machine learning",
    "data structures",
    "algorithms",
    "cloud computing",
    "software engineering"
]

sentences = [
    "This document contains information about {}.",
    "{} is an important topic in computer science.",
    "Researchers study {} to improve modern systems.",
    "Efficient {} systems require careful design and optimization.",
    "Modern applications depend on reliable {} technologies."
]

with open("data/benchmark/documents_100000.txt", "w") as file:
    for document_id in range(1, 100001):
        topic = random.choice(topics)

        text = " ".join(
            sentence.format(topic)
            for sentence in random.sample(sentences, 3)
        )

        title = topic.title()

        file.write(
            f"{document_id}|{title}|{text}\n"
        )

print("Generated 50,000 documents.")