# ECO4AI: Power consumption

This branch presents the work done during the second phase of ECO4AI project. The aim was to explore solutions for multi-objectives multi arm bandits applied to collaborative edge-cloud inference.

## Data collection:
We decided to construct look up tables for energy consumption:
  - Using Tegrastats on jetson
  - Using built-in triton metrics on server

## The strategy:
We made changed to dronebandit to consider the power consumption in computation. The idea for power consumption is the same as the time one: We observe the power cons on the edge device and we predict the one from server.
We modified the algorithm as follow:
  - The update of b vector is now bidimensional : x_p*(d,p): This enables power consideration
  - We predict ~d and ~p as usual
  - We first choose the smallest time accross cutting points, and introduce a sigma constant; This constant defines a range of values around the best time:
    - If sigma = 0, only the best time is considered in decision
    - If sigma = epsilon, all the cutting points that are in the range t*(1+epsilon) will be candidate for the best cutting point. These cutting points are then sorted by energy and final decision is made
    - Big sigma means that time is not relevant, only energy will be considered
## Test:
We tested our implementation in different scenarios:
  - A fixed scenario that was not relevant to our case
  - A random scenario generator has been implemented to improve it:
    - We observe that increasing sigma leads to a decrease in accuracy, which means that the modelisation became harder once power is considered
    - Longer scenarios leads to better accuracy
    - A tendency to predict full edge inference is observed, as edge power consumptions values are very small compared to server ones.
## Future work:
  - Explore other strategies (Pareto, linear combination of 2 dronebandits, etc )
  - Online power reading
