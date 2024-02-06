import bisect
import ipywidgets as widgets
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoLocator
import numpy as np
import pandas as pd
from sklearn.metrics import accuracy_score


class DroneBandit:
    def __init__(self, x: list, alpha: float, beta: float = 1) -> None:
        self.X = np.array(x)
        
        # p, number of partition points
        # d, number of context features
        p, d = self.X.shape

        self.alpha = alpha
        self.beta = beta

        self.A = self.beta * np.identity(d, np.float32)
        self.B = np.zeros((d, 1), np.float32)

    
    def calc_d_e(self) -> list:
        estimated_delays = []

        A_inv = np.linalg.inv(self.A)
        theta_t = A_inv @ self.B
        for p in range(self.X.shape[0]):
            xp = np.reshape(self.X[p], (-1,1))
            # Expected value
            delay_prediction = theta_t.T @ xp
            # Standard deviation
            delay_confidence = self.alpha * np.sqrt(xp.T @ A_inv @ xp) # shape (1,1)
            estimated_delays.append((delay_prediction - delay_confidence)[0][0])
        return estimated_delays

    
    def update(self, p: int, d_e_p: float) -> None:
        xp = np.reshape(self.X[p], (-1,1))
        self.A = self.A + xp @ xp.T
        self.B = self.B + xp * d_e_p 

    
    @staticmethod
    def find_nearest_lower_value_(arr: list, target: int) -> int:
        index = bisect.bisect_left(arr, target)
        return arr[index - 1]


    @staticmethod
    def run_scenario(context_vector_fname, scenario, params):

        P = params['P']
        ps = []
        target_ps = []
        true_rankings = []
        system_params = {'d_e': [], 'd_f': []}

        D_F_simulated = np.full(P, params['D_F_def_const'])
        D_E_Pt_simulated = np.full(P-1, params['D_E_def_const'])
        
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
        ucb = DroneBandit(x_n, params['ANS_ALPHA'], params['ANS_BETA'])
    
        # for our proposition of improvement
        correct_pred_counter = 0
        last_d_e_pt = None
        last_d_e_pt_hat = None
        T_e = 5 * 10**(params['DELAYS_MAGNITUDE']-1) # a prediction is considered good enough if (d_e_pt - d_e_pt_hat) < T_e
        T_c = 10 # passed this number of good predictions, the system isn't updated anymore until it is wrong again

        tick_id = 0
        while True:

            # retrieve scenario
            if tick_id in scenario.keys():
                args = scenario[tick_id]
                if args is None: # end of scenario
                    return ps, target_ps, true_rankings, system_params
                target_p_t_, params_t_ = args

                # update system params
                for key in params_t_.keys():
                    if key == 'd_f':
                        assert len(params_t_[key]) == len(D_F_simulated)
                        for i, new_val in enumerate(params_t_[key]):
                            D_F_simulated[i] = new_val
                            
                    elif key == 'd_e':
                        assert len(params_t_[key]) == len(D_E_Pt_simulated)
                        for i, new_val in enumerate(params_t_[key]):
                            D_E_Pt_simulated[i] = new_val
            else:
                target_p_t_ = scenario[DroneBandit.find_nearest_lower_value_(list(scenario.keys()), tick_id)][0]
            
            if target_p_t_ is None: # end of scenario
                return ps, target_ps, true_rankings, system_params

            
            # Estimate back inference time
            d_e_hat = ucb.calc_d_e()
            
            # Estimate total inference time
            d_f = [val for val in D_F_simulated]
            d_hat = np.asarray(d_f) + np.asarray(d_e_hat)
            
            # Choose partition p point for min estimated total time
            d_p_estimation_argsort = np.argsort(d_hat)
            p_t_ = d_p_estimation_argsort[0]
            d_p_hat = np.min(d_hat)
            
            # Every 1/4 times, randomize decision for p if selected partition point is set to NO partial inference
            if p_t_ == P-1 and inference_count % params['RDM_P_TRIGGER'] == 0:
                p_t_ = d_p_estimation_argsort[np.random.randint(1,P-1)]
                
            # Run inference
            if p_t_ == P-1:
                # Full edge, we do not update UCB
                d_f[p_t_] = D_F_simulated[p_t_] # Measured front inference time
            else:
                d_f[p_t_] = D_F_simulated[p_t_] # Measured front inference time
                d_e_pt = D_E_Pt_simulated[p_t_] # Measured back inference time
        
                # keep count of the number of times the estimation was "close enough" to the measured value
                if last_d_e_pt is not None and last_d_e_pt_hat is not None and abs(last_d_e_pt - last_d_e_pt_hat) < T_e:
                    correct_pred_counter += 1
                else:
                    correct_pred_counter = 0
                    
                last_d_e_pt = d_e_pt
                last_d_e_pt_hat = d_e_hat[p_t_]
        
                # only update the system if it made multiple mistakes in a row
                if not params['USE_BETTER_ANS'] or correct_pred_counter < T_c:
                    ucb.update(p_t_, d_e_pt)
            inference_count += 1
            
            # just for simulation:
            # compute "true ranking"
            d_ = [sum(x) for x in zip(D_F_simulated, D_E_Pt_simulated)] + [D_F_simulated[-1]]
            true_ranking = np.argsort(d_)

            # save iteration results
            ps.append(p_t_)
            target_ps.append(target_p_t_)
            true_rankings.append(true_ranking)
            system_params['d_f'].append(D_F_simulated)
            system_params['d_e'].append(D_E_Pt_simulated)
            
            tick_id += 1

    @staticmethod
    def plot_scenario(ps, target_ps, true_rankings, P, linewidth):
        fig, ax = plt.subplots(constrained_layout=True, figsize=(10, 3.5))
        line, = ax.plot([], [], lw=linewidth, color='red', label='Prediction', alpha=1., marker='', linestyle='-')
        line_target, = ax.plot([], [], lw=linewidth, label='Optimal', alpha=1, marker='', linestyle='--')
    
        fig.suptitle('Simulated partition point selection')
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
        
        x_axis_ticks = list(range(len(ps)))
        line.set_data(x_axis_ticks, ps)
        line_target.set_data(x_axis_ticks, target_ps)
        line_target.set_linewidth(linewidth)
        fig.canvas.draw()
        ax.set_xlim([x_axis_ticks[0], x_axis_ticks[-1] if len(x_axis_ticks) > 1 else x_axis_ticks[0]+1])
        line.set_linewidth(linewidth)
        plt.plot()


    @staticmethod
    def compute_metrics(P, p_true, p_pred, true_rankings):
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
    
        N = len(p_true)
    
        dumb_weighted_acc = 0
        smart_weighted_acc = 0
        smart_acc = 0
        for pt, pp, true_ranking in zip(p_true, p_pred, true_rankings):
            
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
        metrics['Accuracy'] = accuracy_score(p_true, p_pred)
    
        # "dumb" weighted accuracy
        metrics['Dumb Weighted Accuracy'] = dumb_weighted_acc
    
        # "smart" weighted accuracy
        metrics['Smart Weighted Accuracy'] = smart_weighted_acc
    
        # "smart" accuracy
        metrics['Smart Accuracy'] = smart_acc
        
        return metrics