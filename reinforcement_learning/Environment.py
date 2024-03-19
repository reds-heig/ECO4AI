
from collections import namedtuple
import numpy as np
import numpy.random as rdm
import pandas as pd
from torch import Tensor

HiddenState = namedtuple('HiddenState',
                        ('action', 'front_delays', 'transmission_delays', 'back_delays', 'reward'))

class Environment():
    def __init__(self, seed):
        rdm.seed(seed)
        self.n_actions = 9
        self.data_sizes = [
            40_000,
            40_000,
            40_000,
            20_000,
            20_000,
            10_000,
            10_000,
            64,
            0,
        ]
        self._fps = 5 # approximation
        self.bonus_point_T = 50 # in ms
        self.termination_T = 1000 / self._fps # in ms
        self.episode_length = 1000
        self.i_episode = 0
        self.history = []

        self.transmission_delays = {
            40_000: self._load_transmission_delays('means40000_new.txt'),
            20_000: self._load_transmission_delays('means20000_new.txt'),
            10_000: self._load_transmission_delays('means10000_new.txt'),
            64: self._load_transmission_delays('means64_new.txt'),
        }
        self.end_inference_delay_means, self.end_inference_delay_stds = self._load_end_delays('end_times.csv')
        self.front_inference_delay_means = [ # in ms
            25.060201,
            52.314516,
            80.681648,
            106.488372,
            132.951220,
            159.488372,
            186.000000,
            215.631579,
            240.583878,
        ]
        self.front_inference_delay_stds = [ # in ms
            0.238257,
            0.466207,
            0.466712,
            0.501814,
            0.218085,
            0.505781,
            0.471405,
            0.495595,
            0.493452,
        ]


    def _load_transmission_delays(self, filename):
        # these files contain delays measured in ms
        with open(filename, 'r') as f:
            content = ','.join(f.readlines())
            return list(map(float, content.split(',')))

    
    def _load_end_delays(self, filename):
        # delays are measured in s
        df = pd.read_csv(filename)
        groups = df.groupby('cp')
        means = groups.mean()['execution_time'].to_numpy()
        stds = groups.std()['execution_time'].to_numpy()
        return means * 1000, stds * 1000 # convert to ms


    def _sample_end_delay(self, action):
        if action == self.n_actions-1:
            return 0.
            
        back_delay = rdm.normal(
            loc=self.end_inference_delay_means[action],
            scale=self.end_inference_delay_stds[action],
        )
        return back_delay

    
    def _sample_transmission_delay(self, action):
        if action == self.n_actions-1:
            return 0.

        data_size = self.data_sizes[action]
        transmission_delay = rdm.choice(self.transmission_delays[data_size])
        return transmission_delay

    
    def _sample_front_delay(self, action):
        if action == 0:
            return 0.
        
        front_delay = rdm.normal(
            loc=self.front_inference_delay_means[action],
            scale=self.front_inference_delay_stds[action],
        )
        return front_delay
    

    def reset(self): 
        self.i_episode = 0
        action = rdm.randint(1, self.n_actions-2)
        front_delay = self._sample_front_delay(action)
        back_delay = self._sample_end_delay(action)
        data_size = self.data_sizes[action]
        state = [action, front_delay, back_delay, data_size]
        self.history.append([])
        return state

    
    def get_reward_(self, action, all_front_delays, all_transmission_delays, all_end_delays):
        best_action = np.sum([all_front_delays, all_transmission_delays, all_end_delays], axis=0).argsort()[0]
        return int(action == best_action)

    
    def step(self, action):
        # action is an index => chosen P
        # "compute" next state
        front_delay = self._sample_front_delay(action)
        transmission_delay = self._sample_transmission_delay(action)
        end_inference_delay = self._sample_end_delay(action)
        total_end_delay = end_inference_delay + transmission_delay
        data_size = self.data_sizes[action]
        observation = [action, front_delay, total_end_delay, data_size]
        # "measure" simulated delays for other actions
        all_front_delays = [self._sample_front_delay(a_) if a_ != action else front_delay for a_ in range(self.n_actions)]
        all_transmission_delays = [self._sample_transmission_delay(a_) if a_ != action else transmission_delay for a_ in range(self.n_actions)]
        all_end_delays = [self._sample_end_delay(a_) if a_ != action else end_inference_delay for a_ in range(self.n_actions)]
        # compute reward
        reward = self.get_reward_(action, all_front_delays, all_transmission_delays, all_end_delays)
        terminated = front_delay + total_end_delay >= self.termination_T
        truncated = self.i_episode >= self.episode_length
        # keep track of the iteration we are in current episode
        self.i_episode += 1
        self.history[-1].append(
            HiddenState(
                action,
                all_front_delays,
                all_transmission_delays,
                all_end_delays,
                reward,
            )
        )
        return observation, reward, terminated, truncated

    
    def sample(self):
        # randomly sample one action (return the index of this action)
        action = rdm.randint(0, self.n_actions)
        return action