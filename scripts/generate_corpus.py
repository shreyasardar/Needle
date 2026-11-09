
import random
import argparse
import os

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


def generate_corpus(num_documents, output_file):

    random.seed(42)

    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    with open(output_file, "w", encoding="utf-8") as file:

        for document_id in range(1, num_documents + 1):

            topic = random.choice(topics)

            text = " ".join(
                sentence.format(topic)
                for sentence in random.sample(sentences, 3)
            )

            title = topic.title()

            file.write(
                f"{document_id}|{title}|{text}\n"
            )

    print(f"Generated {num_documents:,} documents.")
    print(f"Output file: {output_file}")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Generate synthetic documents for Needle."
    )

    parser.add_argument(
        "--count",
        type=int,
        default=100000,
        help="Number of documents to generate."
    )

    args = parser.parse_args()

    if args.count <= 0:
        parser.error("Document count must be positive.")

    output_file = (
        f"data/benchmark/documents_{args.count}.txt"
    )

    generate_corpus(args.count, output_file)
