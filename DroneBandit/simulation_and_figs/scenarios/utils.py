from itertools import accumulate
import numpy as np
import pandas as pd
from DroneBandit import DroneBandit


def load_scenario(scenario, sigma):
    ticks = [0] + list(accumulate(map(lambda args: args[0], scenario)))
    
    df = pd.read_csv('power_cons_serv.csv')
    CP_MAP = [0, 1, 4, 6, 8, 10, 12, 16, 17]
    df = df[df['cutting point'].isin(CP_MAP)]
    df['cutting point'] = df['cutting point'].map(lambda p: CP_MAP.index(p))
    power_cons_serv = df.groupby('cutting point').mean()

    df = pd.read_csv('power_cons_edge.csv')
    power_cons_edge = df.set_index('cutting point')

    scenario_ = {}
    for i in range(len(ticks)):
        if i < len(ticks)-1:
            scenario[i] = list(scenario[i])
            scenario[i][2]['p_edge'] = power_cons_edge.iloc[:, 0].to_numpy()
            scenario[i][2]['p_serv'] = power_cons_serv.iloc[:, 0].to_numpy()
            scenario[i][1] = DroneBandit.solve_argmin(
                scenario[i][2]['d_edge'] + np.array(list(scenario[i][2]['d_serv']) + [0]),
                scenario[i][2]['p_edge'] + scenario[i][2]['p_serv'],
                sigma,
            )[0]
            scenario_[ticks[i]] = scenario[i][1:]
        else:
            scenario_[ticks[i]] = None
    return scenario_


def generate_scenario(sigma, length, n_ticks, P, min_d_edge, max_d_edge, min_d_serv, max_d_serv, seed):
    np.random.seed(seed)
    scenario = {}

    min_tick, max_tick = 3, 40
    ticks = list(np.random.randint(min_tick, max_tick + 1, size=n_ticks - 1))
    ticks.append(length - sum(ticks))
    ticks = [length // n_ticks for _ in range(n_ticks)]
    ticks = [0] + list(accumulate(ticks))
    print(length // n_ticks, length)

    df = pd.read_csv('power_cons_serv.csv')
    CP_MAP = [0, 1, 4, 6, 8, 10, 12, 16, 17]
    df = df[df['cutting point'].isin(CP_MAP)]
    df['cutting point'] = df['cutting point'].map(lambda p: CP_MAP.index(p))
    power_cons_serv = df.groupby('cutting point').mean()

    df = pd.read_csv('power_cons_edge.csv')
    power_cons_edge = df.set_index('cutting point')

    for i in range(len(ticks)):
        k = ticks[i]
        if i < len(ticks)-1:
            scenario[k] = [None, {}]
            scenario[k][1]['d_edge'] = np.random.uniform(min_d_edge, max_d_edge, size=P)
            scenario[k][1]['d_edge'][-1] *= 1.5
            scenario[k][1]['d_serv'] = np.random.uniform(min_d_serv, max_d_serv, size=P-1)
            scenario[k][1]['p_edge'] = power_cons_edge.iloc[:, 0].to_numpy()
            scenario[k][1]['p_serv'] = power_cons_serv.iloc[:, 0].to_numpy()
            scenario[k][0] = DroneBandit.solve_argmin(
                scenario[k][1]['d_edge'] + np.array(list(scenario[k][1]['d_serv']) + [0]),
                scenario[k][1]['p_edge'] + scenario[k][1]['p_serv'],
                sigma,
            )[0]
        else:
            scenario[k] = None

    return scenario