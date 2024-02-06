import numpy as np

class EarlyStopper:
    
    # adapted from: https://stackoverflow.com/a/73704579
    # last consulted on January 10th 2024.
    
    def __init__(self, patience=1, min_delta=0, verbose=True):
        """
        Parameters
        ----------
        patience : int
            Number of epochs with no improvement after which training will be stopped.
        min_delta : int
            Minimum change in the monitored quantity to qualify as an improvement, i.e. an absolute change of less than min_delta, will count as no improvement.
        verbose : bool
            True if class should print improvements
        """
        self.patience = patience
        self.min_delta = min_delta
        self.counter = 0
        self.min_validation_loss = np.inf
        self.verbose = verbose

    def early_stop(self, validation_loss):
        """Returns True if training has not improved and should be stopped, False otherwise.

        Parameters
        ----------
        validation_loss : float
            validation loss to monitor
        
        Returns
        -------
        bool
            True if training should be stopped, False otherwise
        """
        self._has_model_improved = False
        if validation_loss < self.min_validation_loss:
            self._has_model_improved = True
            self.min_validation_loss = validation_loss
            self.counter = 0
        elif validation_loss > (self.min_validation_loss + self.min_delta):
            self.counter += 1
            if self.verbose:
                print('Model did not improve.')
            if self.counter >= self.patience:
                return True
        return False
    
    def has_model_improved(self):
        """Returns True if model has improved after last iteration, False otherwise.
        
        Returns
        -------
        bool
            True if model has improved after last iteration, False otherwise
        """
        return self._has_model_improved