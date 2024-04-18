import bisect
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoLocator
import numpy as np
import pandas as pd
from sklearn.metrics import accuracy_score
from typing import Tuple


class DroneBandit:
    def __init__(self, x: list, alpha: float, beta: float, gamma: float, delta: float) -> None:
        self.X = np.array(x)
        
        # p, number of cutting points
        # d, number of context features
        p, d = self.X.shape

        self.alpha = alpha
        self.beta = beta

        self.gamma = gamma # factor to scale the uncertainty of time
        self.delta = delta # factor to scale the uncertainty of power cons

        self.A = self.beta * np.identity(d, np.float32)
        self.B = np.zeros((d, 2), np.float32)

    
    def estimate(self) -> list:
        d_serv_hat, p_serv_hat = [], []

        A_inv = np.linalg.inv(self.A)
        theta_t = A_inv @ self.B
        for p in range(self.X.shape[0]):
            xp = np.reshape(self.X[p], (-1,1))
            # Expected value
            predictions = theta_t.T @ xp
            # Standard deviation
            uncertainty = self.alpha * np.sqrt(xp.T @ A_inv @ xp) # shape (1,1)
            uncertainty = np.array([
                uncertainty * self.gamma,
                uncertainty * self.delta,
            ])
            predictions = predictions.flatten() - uncertainty.flatten()
            d_serv_hat.append(predictions[0])
            p_serv_hat.append(predictions[1])
        return d_serv_hat, p_serv_hat

    
    def update(self, p: int, d_server: float, p_server: float) -> None:
        xp = np.reshape(self.X[p], (-1,1))
        self.A = self.A + xp @ xp.T
        v = np.array([d_server, p_server])
        self.B = self.B + xp * v

    
    @staticmethod
    def find_nearest_lower_value_(arr: list, target: int) -> int:
        index = bisect.bisect_left(arr, target)
        return arr[index - 1]


    @staticmethod
    def solve_argmin(d_total: list, p_total: list, sigma: float) -> Tuple[list, float, float, list]:
        min_d = np.min(d_total)
        t = min_d * (1 + np.sign(min_d) * sigma)
        cp_candidates = np.where(d_total <= t)[0]
        cp_candidates_sorted = cp_candidates[p_total[cp_candidates].argsort()]
        cutting_point = cp_candidates_sorted[0]
        true_cp_ranking = [*cp_candidates_sorted, *(set(d_total.argsort()) - set(cp_candidates_sorted))]
        return cutting_point, d_total[cutting_point], p_total[cutting_point], true_cp_ranking


    @staticmethod
    def run_scenario(context_vector_fname, scenario, params):

        P = params['P']
        cutting_points = []
        target_cutting_points = []
        true_cp_rankings = []
        system_params = {'d_serv': [], 'd_edge': [], 'p_serv': [], 'p_edge': []}

        D_EDGE_SIM = np.full(P, params['D_EDGE_def_const'])
        D_SERV_SIM = np.full(P-1, params['D_SERV_def_const'])
        
        P_EDGE_SIM = np.full(P, params['P_EDGE_def_const'])
        P_SERV_SIM = np.full(P, params['P_SERV_def_const'])
        
        # context vector
        #  #conv_mac_units, #linear_mac_units, #activation_mac_units, #conv, #linear, #activation, #input_size
        x_df = pd.read_csv(context_vector_fname, header=None)
        max_macunits = x_df[[0,1,2]].max().max()
        if params['USE_BETTER_ANS']:
            x_df[[0,1,2]] = x_df[[0,1,2]].map(lambda v: v / max_macunits * params['MACUNITS_MAP_SPREAD'])
        x = x_df.to_numpy().tolist()
    
        # init ANS
        inference_count = 0
        x_n = np.copy(x).astype(np.float64)
        ucb = DroneBandit(x_n, params['ANS_ALPHA'], params['ANS_BETA'], params['ANS_GAMMA'], params['ANS_DELTA'])
    
        # for our proposition of improvement
        correct_pred_counter = 0
        last_d_serv = None
        last_d_serv_hat = None
        T_e = 5 * 10**(params['DELAYS_MAGNITUDE']-1) # a prediction is considered good enough if (d_serv - d_serv_hat) < T_e
        T_c = 10 # passed this number of good predictions, the system isn't updated anymore until it is wrong again

        tick_id = 0
        while True:

            # retrieve scenario
            if tick_id in scenario.keys():
                args = scenario[tick_id]
                if args is None: # end of scenario
                    return cutting_points, target_cutting_points, true_cp_rankings, system_params
                target_cp, scenario_params = args

                # update system params
                for key in scenario_params.keys():
                    if key == 'd_edge':
                        assert len(scenario_params[key]) == len(D_EDGE_SIM)
                        for i, new_val in enumerate(scenario_params[key]):
                            D_EDGE_SIM[i] = new_val
                            
                    elif key == 'd_serv':
                        assert len(scenario_params[key]) == len(D_SERV_SIM)
                        for i, new_val in enumerate(scenario_params[key]):
                            D_SERV_SIM[i] = new_val
                            
                    elif key == 'p_edge':
                        for i, new_val in enumerate(scenario_params[key]):
                            P_EDGE_SIM[i] = new_val
                            
                    elif key == 'p_serv':
                        for i, new_val in enumerate(scenario_params[key]):
                            P_SERV_SIM[i] = new_val
            else:
                target_cp = scenario[DroneBandit.find_nearest_lower_value_(list(scenario.keys()), tick_id)][0]
            
            if target_cp is None: # end of scenario
                return cutting_points, target_cutting_points, true_cp_rankings, system_params

            
            # Estimate back inference time & back power cons
            d_serv_hat, p_serv_hat = ucb.estimate()
            
            # Estimate total inference time
            d_edge = [val for val in D_EDGE_SIM]
            p_edge = [val for val in P_EDGE_SIM]
            d_total_hat = np.asarray(d_edge) + np.asarray(d_serv_hat)
            p_total_hat = np.asarray(p_edge) + np.asarray(p_serv_hat)

            # Choose cutting point for min estimated total time & min power cons
            cp = DroneBandit.solve_argmin(d_total_hat, p_total_hat, params['ANS_SIGMA'])[0]
            
            # Every few iteration, randomize decision for p if selected cutting point is set to NO partial inference
            if cp == P-1 and inference_count % params['RDM_P_TRIGGER'] == 0:
                cp = np.random.randint(1, P-1)
                
            # Run inference
            if cp == P-1:
                # Full edge, we do not update UCB
                d_edge[cp] = D_EDGE_SIM[cp] # Measured front inference time
                p_edge[cp] = P_EDGE_SIM[cp] # Measured front power cons
            else:
                d_edge[cp] = D_EDGE_SIM[cp] # Measured front inference time
                p_edge[cp] = P_EDGE_SIM[cp] # Measured front power cons
                d_serv = D_SERV_SIM[cp] # Measured back inference time
                p_serv = P_SERV_SIM[cp] # Measured back inference time

                # keep count of the number of times the estimation was "close enough" to the measured value
                if last_d_serv is not None and last_d_serv_hat is not None and abs(last_d_serv - last_d_serv_hat) < T_e:
                    correct_pred_counter += 1
                else:
                    correct_pred_counter = 0
                    
                last_d_serv = d_serv
                last_d_serv_hat = d_serv_hat[cp]
        
                # only update the system if it made multiple mistakes in a row
                if not params['USE_BETTER_ANS'] or correct_pred_counter < T_c:
                    ucb.update(cp, d_serv, p_serv)
            inference_count += 1
            
            # just for simulation:
            # compute "true ranking"
            d_total = np.array([sum(x) for x in zip(D_EDGE_SIM, D_SERV_SIM)] + [D_EDGE_SIM[-1]])
            p_total = np.array([sum(x) for x in zip(P_EDGE_SIM, P_SERV_SIM)])
            true_ranking = DroneBandit.solve_argmin(d_total, p_total, params['ANS_SIGMA'])[3]

            # save iteration results
            cutting_points.append(cp)
            target_cutting_points.append(target_cp)
            true_cp_rankings.append(true_ranking)
            system_params['d_edge'].append(D_EDGE_SIM)
            system_params['d_serv'].append(D_SERV_SIM)
            system_params['p_edge'].append(P_EDGE_SIM)
            system_params['p_serv'].append(P_SERV_SIM)
            
            tick_id += 1

    @staticmethod
    def plot_scenario(cutting_points, target_cutting_points, true_cp_rankings, P, linewidth):
        fig, ax = plt.subplots(constrained_layout=True, figsize=(10, 3.5))
        line, = ax.plot([], [], lw=linewidth, color='red', label='Prediction', alpha=1., marker='', linestyle='-')
        line_target, = ax.plot([], [], lw=linewidth, label='Optimal', alpha=1, marker='', linestyle='--')
    
        fig.suptitle('Simulated cutting point selection')
        fig.canvas.toolbar_position = 'bottom'
        ax.set_xlabel('Algorithm step')
        ax.set_ylabel('Cutting point')
        ax.set_ylim([0, P-1])
        ax.set_yticks([0,1,2,3,4,5,6,7,8], ['cloud, 0', 1,2,3,4,5,6,7, 'edge, 8'])
        ax.set_xticks([0])
        ax.xaxis.set_major_locator(AutoLocator())
        ax.grid(True)
        
        y_margin_fraction = 0.1  # Adjust the fraction as needed
        y_lower_limit, y_upper_limit = ax.get_ylim()
        y_margin = (y_upper_limit - y_lower_limit) * y_margin_fraction
        ax.set_ylim(y_lower_limit - y_margin, y_upper_limit + y_margin)
        
        ax.legend(bbox_to_anchor=(.44, .01), loc='lower center', borderaxespad=0, fontsize=18) # bbox_to_anchor=(.42, .01), 
        plt.rcParams.update({'font.size': 22})
        
        x_axis_ticks = list(range(len(cutting_points)))
        line.set_data(x_axis_ticks, cutting_points)
        line_target.set_data(x_axis_ticks, target_cutting_points)
        line_target.set_linewidth(linewidth)
        fig.canvas.draw()
        ax.set_xlim([x_axis_ticks[0], x_axis_ticks[-1] if len(x_axis_ticks) > 1 else x_axis_ticks[0]+1])
        line.set_linewidth(linewidth)
        plt.plot()


    @staticmethod
    def compute_metrics(P, target_cutting_points, cutting_points, true_cp_rankings):
        """
        Accuracy: [0,1], 1 is best
            Normalized number of correct predictions of p
        
        Dumb Weighted Accuracy: [0,1], 1 is best
            Engourages predictions to be close values of the truth p
            1/N * [1 - (1-(truth==pred)) * abs(truth-pred) / (P-1)]
        
        Smart Weighted Accuracy: [0,1], 1 is best
            The higher the prediction is in the "true ranking" the best it gets
            1/N * [1 - ( int(pred == true_ranking[0]) * .00 + int(pred == true_ranking[1]) * .20 + int(pred == true_ranking[2]) * .50 + int(pred in true_ranking[3:]) )]
        Smart Accuracy: [0,1], 1 is best
            Best when the prediction is in the top 3 of the "true ranking" no matter it's position
            1/N * [ pred in true_ranking[:3] ]
        """
    
        N = len(target_cutting_points)
    
        dumb_weighted_acc = 0
        smart_weighted_acc = 0
        smart_acc = 0
        for pt, pp, true_ranking in zip(target_cutting_points, cutting_points, true_cp_rankings):
            
            dumb_weighted_acc += (1 - (pt == pp)) * abs(pt - pp) / (P - 1)
    
            smart_weighted_acc += int(pp == true_ranking[0]) * .00 + \
                                  int(pp == true_ranking[1]) * .20 + \
                                  int(pp == true_ranking[2]) * .50 + \
                                  int(pp in true_ranking[3:])
    
            smart_acc += int(pp in true_ranking[:3])
    
        dumb_weighted_acc = 1 - (dumb_weighted_acc / N)
        smart_weighted_acc = 1 - (smart_weighted_acc / N)
        smart_acc /= N
    
        
        metrics = {}
    
        # accuracy
        metrics['Accuracy'] = accuracy_score(target_cutting_points, cutting_points)
    
        # "dumb" weighted accuracy
        metrics['Dumb Weighted Accuracy'] = dumb_weighted_acc
    
        # "smart" weighted accuracy
        metrics['Smart Weighted Accuracy'] = smart_weighted_acc
    
        # "smart" accuracy
        metrics['Smart Accuracy'] = smart_acc
        
        return metrics