#!/usr/bin/env python3
"""
Generate a named network file from the SNAP "facebook_combined" edge list.

Reads data/facebook_combined.txt.gz (pairs of node ids, one edge per line),
assigns a random human-readable name to every node, and writes the result in
the #USERS / #EDGES format that io.c's load_network() expects:

    #USERS
    <id> <First_Last>
    ...
    #EDGES
    <id1> <id2>
    ...

Edges are copied through unchanged (same node ids, same pairs) -- only the
per-node names are invented.

Usage:
    python generate_named_network.py
    python generate_named_network.py --input data/facebook_combined.txt.gz \
                                      --output data/facebook_named.txt \
                                      --seed 42
"""

import argparse
import gzip
import random

MAX_NAME = 32  # must match MAX_NAME in graph.h (including the null terminator)

FIRST_NAMES = [
    "Ayoub", "Ismail", "Omar", "Youssef", "Amine", "Hamza", "Adam", "Rayan",
    "Karim", "Sami", "Nabil", "Reda", "Yassine", "Anas", "Bilal", "Zakaria",
    "Mehdi", "Tariq", "Walid", "Idris", "Nadir", "Saad", "Hicham", "Aziz",
    "Fahd", "Marwan", "Younes", "Soufiane", "Othmane", "Rachid",
    "Salma", "Lina", "Sara", "Nour", "Yasmine", "Imane", "Hind", "Rim",
    "Fatima", "Khadija", "Meryem", "Zineb", "Amal", "Asma", "Dounia",
    "Ghita", "Hajar", "Ikram", "Jihane", "Kenza", "Lamia", "Malak",
    "Nadia", "Ouafa", "Rania", "Siham", "Wafa", "Yousra", "Zahra",
    "Alice", "Bob", "Carla", "David", "Elena", "Frank", "Grace", "Hugo",
    "Ines", "Jack", "Kate", "Liam", "Mona", "Noah", "Olivia", "Paul",
]

LAST_NAMES = [
    "Elboukhari", "Bennani", "Alaoui", "Idrissi", "Chraibi", "Fassi",
    "Tahiri", "Amrani", "Berrada", "Cherkaoui", "Doukkali", "Sqalli",
    "Guessous", "Haddad", "Jamal", "Kabbaj", "Lahlou", "Mansouri",
    "Naciri", "Oudghiri", "Qadiri", "Rifai", "Saidi", "Tazi", "Wahbi",
    "Zerhouni", "Bakkali", "Chaoui", "Daoudi", "Elazizi", "Filali",
    "Ghazi", "Hakimi", "Jabri", "Karimi", "Lamrani", "Moukrim",
    "Nejjar", "Ouazzani", "Qasmi", "Rguibi", "Sabri", "Toumi",
    "Smith", "Johnson", "Brown", "Miller", "Davis", "Wilson", "Moore",
    "Taylor", "Clark", "Lewis", "Walker", "Hall", "Allen", "Young",
    "King", "Wright", "Green", "Baker", "Adams", "Nelson", "Carter",
]


def build_name_pool(count, seed):
    """Return `count` unique "First Last" names, each short enough for MAX_NAME."""
    rng = random.Random(seed)
    combos = [f"{f}_{l}" for f in FIRST_NAMES for l in LAST_NAMES
              if len(f"{f}_{l}") <= MAX_NAME - 1]
    combos = list(dict.fromkeys(combos))  # dedupe, e.g. if a name list has a repeat
    rng.shuffle(combos)

    if len(combos) >= count:
        return combos[:count]

    # Not enough unique combos (shouldn't happen with the lists above, but be
    # safe): pad with a numeric suffix until we have enough unique names.
    names = list(combos)
    i = 1
    while len(names) < count:
        for f in FIRST_NAMES:
            for l in LAST_NAMES:
                candidate = f"{f}_{l}{i}"
                if len(candidate) <= MAX_NAME - 1:
                    names.append(candidate)
                if len(names) >= count:
                    break
            if len(names) >= count:
                break
        i += 1
    return names[:count]


def read_edges(path):
    edges = []
    node_ids = set()
    opener = gzip.open if path.endswith(".gz") else open
    with opener(path, "rt") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            a_str, b_str = line.split()
            a, b = int(a_str), int(b_str)
            edges.append((a, b))
            node_ids.add(a)
            node_ids.add(b)
    return edges, node_ids


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", default="data/facebook_combined.txt.gz",
                         help="gzip'd edge list to read (default: %(default)s)")
    parser.add_argument("--output", default="data/facebook_named.txt",
                         help="output network file (default: %(default)s)")
    parser.add_argument("--seed", type=int, default=42,
                         help="random seed for reproducible names (default: %(default)s)")
    args = parser.parse_args()

    edges, node_ids = read_edges(args.input)
    n = len(node_ids)
    lo, hi = min(node_ids), max(node_ids)
    if sorted(node_ids) != list(range(lo, hi + 1)):
        raise SystemExit("node ids are not a contiguous 0..N-1 range; "
                          "this script assumes they are, like facebook_combined.txt")

    names = build_name_pool(n, args.seed)

    with open(args.output, "w", newline="\n") as f:
        f.write("#USERS\n")
        for idx in range(lo, hi + 1):
            f.write(f"{idx} {names[idx - lo]}\n")
        f.write("#EDGES\n")
        for a, b in edges:
            f.write(f"{a} {b}\n")

    print(f"wrote {n} users and {len(edges)} edges to {args.output}")


if __name__ == "__main__":
    main()
