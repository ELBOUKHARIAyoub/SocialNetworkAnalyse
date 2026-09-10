# Social Network Analyse

A small social-network graph engine in C (adjacency list, BFS shortest path,
mutual-friend recommendations, connected components / community count),
with a Python/Jupyter notebook for visualisation and descriptive stats.
All graph algorithms are hand-implemented in C; NetworkX is used only to
draw figures and compute stats for the report.

## Structure

```
SocialNetworkAnalyse/
├── src/                      C source
│   ├── graph.h / graph.c         adjacency-list graph + user table
│   ├── io.h / io.c               load/save network files
│   ├── algorithms.h / algorithms.c   BFS, recommendations, components
│   ├── colors.h                  ANSI color codes for CLI output
│   └── main.c                    interactive menu / entry point
├── data/
│   ├── network.txt               small hand-built demo network
│   ├── facebook_combined.txt.gz  raw SNAP ego-Facebook edge list (tracked)
│   ├── facebook_combined.txt     decompressed edge list (generated, gitignored)
│   └── facebook_named.txt        SNAP graph converted to the project's format
├── tools/
│   └── convert_snap.py           assigns names to the SNAP graph -> facebook_named.txt
├── viz/
│   └── network_analysis.ipynb    figures + stats for the report
├── output/
│   └── timings.csv               written by the C program's timing harness
├── figures/                      PNGs written by the notebook
├── Makefile
└── .gitignore
```

## Build & run (C)

```
make            # builds ./main.exe from src/*.c
make run        # build + run
make clean
```

Run `main.exe` from the project root (the `Makefile` does this) so its
relative data paths (`data/facebook_named.txt`, etc.) resolve correctly.

## Data

`data/facebook_combined.txt.gz` is the tracked source; the decompressed
`.txt` and the named conversion are regenerated with:

```
make data
```

which unzips the raw edge list and reruns `tools/convert_snap.py` to
produce `data/facebook_named.txt`.

## Notebook

Open `viz/network_analysis.ipynb` (paths inside it are relative to `viz/`,
e.g. `../data`, `../figures`). Requires `matplotlib`, `networkx`, `numpy`:

```
pip install matplotlib networkx numpy
```
