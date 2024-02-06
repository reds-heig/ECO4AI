import numpy as np
import os
import pandas as pd
from PIL import Image
import torch
from torch.utils.data import Dataset

class CustomDataset(Dataset):
    def __init__(self, root, img_width, img_height, transform=None):
        """
        Parameters
        ----------
        root : str
            Path to the dataset's files
        img_width : int
            Width of the dataset's images
        img_height : int
            Height of the dataset's images
        transform : torchvision.transforms.Compose
            Transformations to apply to each dataset's image
        """
        self.root = root
        self.img_width = img_width
        self.img_height = img_height
        self.transform = transform

        self.filenames = []
        self.labels = []

        # loads labels and images' filenames
        dirs = os.listdir(root)
        # sorted directory names
        for dir in sorted(dirs):
            full_dir_path = os.path.join(root, dir)
            for sub_root, sub_dirs, files in os.walk(full_dir_path):
                # sorted file names 
                for file in sorted(files):
                    if 'checkpoint' in file:
                        continue
                    if file.endswith('.jpg') or file.endswith('.jpeg') or file.endswith('.pgm') or file.endswith('.ppm'):
                        self.filenames.append(os.path.join(full_dir_path+'/images', file))
                    elif file.endswith('.txt'):
                        l_collision = self._read_txt_label(os.path.join(full_dir_path, file))
                        self.labels.extend(l_collision)
                    elif file.endswith('.csv'):
                        df = pd.read_csv(os.path.join(full_dir_path, file), header=0, sep=',')
                        try:
                            l_collision = df.col.to_numpy()
                        except:
                            l_collision = df.label.to_numpy()
                        l_collision = l_collision / l_collision.max()
                        self.labels.extend(l_collision.tolist())
                    else:
                        print('WARNING: file not loaded', os.path.join(full_dir_path, file))
        assert len(self.filenames) == len(self.labels), f'Dataset size error: {len(self.filenames)}'

    def __len__(self):
        """Returns the dataset's length.

        Returns
        -------
        int
            length of dataset
        """
        return len(self.filenames)

    def __getitem__(self, ix):
        """Returns a dataset's element (image and corresponding label).

        Parameters
        ----------
        ix : int
            Index of the dataset's element to return
        
        Returns
        -------
        Tensor
            Requested image as Tensor
        Tensor
            Requested label as Tensor
        """
        if torch.is_tensor(ix):
            ix = ix.tolist()

        # read image as Pillow
        with open(self.filenames[ix], 'rb') as f:
            _image = Image.open(f)
            _image.load()
        _label = self.labels[ix]

        # transform the image
        _tensor_img = self.transform(_image) if self.transform is not None else _image
        
        # convert the label to Tensor
        _tensor_lbl = torch.tensor(_label, dtype=torch.float32)

        return _tensor_img, _tensor_lbl

    def _read_txt_label(self, filename):
        """Reads a file of labels and returns them.

        Parameters
        ----------
        filename : str
            Path to the file of labels
        
        Returns
        -------
        List
            Labels read from the file
        """
        try:
            return np.loadtxt(filename, usecols=0)
        except OSError as e:
            print('No labels found in dir', filename)