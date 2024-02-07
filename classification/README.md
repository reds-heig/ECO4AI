# Obstacle Detection Use Case

## Introduction

This directory contains the code to train and evaluate our use case scenario for our DroneBandit algorithm. The use case is a simple obstacle detection neural network.

## Datasets

We used three public datasets and a fourth small private one.

- Collision dataset: [https://rpg.ifi.uzh.ch/dronet.html](https://rpg.ifi.uzh.ch/dronet.html) (last visited Feb. 7, 2024)
- Himax: [https://github.com/pulp-platform/Himax_Dataset](https://github.com/pulp-platform/Himax_Dataset) (last visited Feb. 7, 2024)
- HDIN: [https://github.com/Yingxiu-Chang/HDIN-Dataset](https://github.com/Yingxiu-Chang/HDIN-Dataset) (last visited Feb. 7, 2024)

The datasets should be organized in the following way:
```
  col_dataset/
    training/
      DSCN2561/
        labels.txt
        images/
          frame_00452.jpg
          ...
      ...
    validation/
      ...
    testing/
      ...
```

## Setup & Usage
Install dependencies:

```bash
cd classification/
poetry install
```

Activate Poetry's virtual environment:
```bash
poetry shell
```

Start your Jupyter Lab server:
```bash
jupyter lab
```