# ECO4AI

This is the official repository for the ECO4AI project and the resulting paper "DroneBandit: Multi-armed contextual bandits for collaborative edge-to-cloud inference in resource-constrained nanodrones".

Authors: *Guillaume Chacun*, *Mehdi Akeddar*, *Thomas Rieder*, *Bruno Da Rocha Carvalho* and *Marina Zapater*
<br>
REDS. School of Engineering and Management Vaud, HES-SO University of Applied sciences and Arts Western Switzerland
<br>
Email: {guillaume.chacun, mehdi.akeddar, thomas.rieder, bruno.darochacarvalho, marina.zapater}@heig-vd.ch

## Introduction

In recent years, Artificial Intelligence (AI) has seen a remarkable expansion. Traditionally, applications primarily relied on edge computing, closer to data sources. However, today edge computing approaches are outstripped by modern AI demands. In this paper, we introduce a novel framework together with an online decision algorithm based on multi-armed contextual bandits (DroneBandit), for the dynamic allocation of inference tasks between edge and cloud, to increase inference performance. Our test environment consists of a resource-constrained nanodrone equipped with a custom DNN for obstacle detection, able to achieve 80\% accuracy  and 71\% F2-score. DroneBandit runs on the drone and chooses the optimal cutting point for each iteration on the fly. The decision is based on predicted back-end delays (i.e. data transfer and inference time on the cloud) and observed front-end delays (i.e. inference time on the edge). DroneBandit achieves 83\% accuracy and 89\% Top3 accuracy in predicting the ideal cutting point on simulations. 
Our framework demonstrates enhanced task allocation adaptability, enabling efficient computation offloading and edge computing reliance in varying network conditions. Our experiments, conducted on a nanodrone with an ARM CPU and GAP8 RISC-V accelerator, incorporate quantization and optimization, showcasing efficient obstacle detection in dynamic scenarios.

## Content of this repository

- *DroneBandit/ai-deck/* : DroneBandit implementation for a GAP8 processor
- *DroneBandit/simulation_and_figs/* : Python implementation of DroneBandit, and code to reproduce our experiments
- *classification/* : obstacle detection use case for DroneBandit

## Acknowledgements
This work has been funded by the ECO4AI project granted by HES-SO, under the call for young researchers 2021.
