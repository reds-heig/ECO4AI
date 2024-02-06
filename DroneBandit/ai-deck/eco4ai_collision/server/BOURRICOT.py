import torch
import torch.nn as nn

class BOURRICOT(nn.Module): # v11
    def __init__(self, binarize_output=False):
        super().__init__()
        self.binarize_output = binarize_output
        # features extractor
        self.features = nn.Sequential(
            nn.Conv2d(1, 4, kernel_size=5, stride=2, padding=2),
            nn.ReLU(),
            nn.Conv2d(4, 4, kernel_size=3, stride=1, padding=1),
            nn.ReLU(),
            
            nn.Conv2d(4, 8, kernel_size=3, stride=2, padding=1),
            nn.ReLU(),
            nn.Conv2d(8, 8, kernel_size=3, stride=1, padding=1),
            nn.ReLU(),
            
            nn.Conv2d(8, 16, kernel_size=5, stride=2, padding=2),
            nn.ReLU(),
            nn.Conv2d(16, 16, kernel_size=3, stride=1, padding=1),
            nn.ReLU(),
        )
        # classification layers
        self.classifier = nn.Sequential(
            nn.Linear(10000, 64),
            nn.ReLU(),
            nn.Dropout(.5),
            nn.Linear(64, 1)
        )

    def forward(self, x):
        """Returns the model's predictions for a mini-batch.

        Parameters
        ----------
        x : Tensor
            Mini-batch of input images, grayscale without normalization
        
        Returns
        -------
        Tensor
            Model predictions for given mini-batch
        """
        x = self.features(x)
        #import numpy as np; print(np.prod(x.shape[1:]))
        x = x.view(-1, 10000)
        x = self.classifier(x)
        if self.binarize_output:
            x = torch.Tensor.int(torch.relu(abs(x) / x)).type(torch.uint8) # binary step activation function
            #x = torch.Tensor.int(torch.sigmoid(x)) # not working with NN tools
        return x