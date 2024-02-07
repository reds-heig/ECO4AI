# ECO4AI

This is the official repository for the ECO4AI project.

Authors: *Guillaume Chacun*, *Mehdi Akeddar*, *Thomas Rieder*, *Bruno Da Rocha Carvalho* and *Marina Zapater*
<br>
REDS. School of Engineering and Management Vaud, HES-SO University of Applied sciences and Arts Western Switzerland
<br>
Email: {guillaume.chacun, mehdi.akeddar, thomas.rieder, bruno.darochacarvalho, marina.zapater}@heig-vd.ch

## Introduction

In recent years, Artificial Intelligence (AI) has seen a remarkable expansion. Consequently, AI adoption has become widespread, spanning domains like autonomous driving, video surveillance, and e-health. Traditionally, applications primarily relied on edge computing, closer to data sources. However, edge devices alone may not be able to handle modern AI applications. The primary objective of this paper is to propose a workload allocation framework that efficiently distributes inference tasks between edge and cloud, enhancing performance for AI-based IoT applications. Our test environment consists of a resource-constrained drone running a custom DNN model for obstacle detection that showcases 80\% accuracy and 71\% F2-score on the test set. Our proposed online decision algorithm, based on multi-armed contextual bandits, runs on the drone and chooses the optimal cutting point for each iteration on the fly. 
The decision is based on both predicted back-end delays (i.e. data transfer and inference time on the server) and observed front-end delays (i.e. inference time on the edge device).
The algorithm achieves 83% accuracy and 89% Top3 accuracy in predicting the ideal cutting point on simulations. 
Our findings demonstrate the flexibility of the allocation in response to dynamic real-world environments, allowing computation offloading when the network becomes efficient while relying on edge computing when network quality deteriorates.
Our proposed DNN and partitioning algorithms are deployed and tested in an ultra-low power GAP8 microprocessor. This involves key steps including quantization, GAP8 code generation from an ONNX model, and optimization to make our algorithms run on the drone and avoid obstacles.

## Content of this repository

- *DroneBandit/ai-deck/* : DroneBandit implementation for a GAP8 processor
- *DroneBandit/simulation_and_figs/* : DroneBandit's Python implementation, and code to reproduce our experiments
- *classification/* : obstacle detection use case for DroneBandit

## Acknowledgements
This work has been funded by the ECO4AI project granted by HES-SO, under the call for young researchers 2021.
