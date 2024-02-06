from CustomDataset import CustomDataset

import matplotlib.pyplot as plt
import numpy as np
from sklearn.metrics import accuracy_score, fbeta_score, confusion_matrix, ConfusionMatrixDisplay
import torch
from torch.utils.data import ConcatDataset, Dataset, DataLoader
from tqdm.notebook import tqdm

def get_test_loader(paths_test, img_width, img_height, general_transforms):
    """Returns the DataLoaders for the given datasets' paths.
    
    Parameters
    ----------
    paths_test : List of str
        path to the test datasets directories
    
    Returns
    -------
    torch.utils.data.DataLoader
        Test DataLoader
    """
    test_dataset = ConcatDataset([CustomDataset(path, img_width, img_height, general_transforms) 
                                  for path in paths_test])
    test_loader = DataLoader(
        test_dataset,
        batch_size=16,
        shuffle=False,
        num_workers=8,
        prefetch_factor=4,
    )
    return test_loader

def test_model(model_path, model_cls, test_loaders, test_loaders_names, input_type=None, use_sigmoid=True, figsize=(15, 5), xlabel='Predicted label', ylabel='True label'):
    """Test model
    
    Parameters
    ----------
    model_path : str
        Path to the model's save file
    test_loaders : List of torch.utils.data.DataLoader
        DataLoaders for testing sets
    test_loaders_names : List of str
        DataLoaders' name to print on the plot's legend
    input_type : Python or Numpy type default None
        If None uses np.float32 otherwise casts the input images to specified type
    use_sigmoid : bool
        True if model outputs raw logit value and if a Sigmoid should be used, False otherwise
    figsize : Tuple of int
        Matplotlib figure's size
    xlabel : str
        figure's x-label
    ylabel : str
        figure's y-label
    """
    model_type = None
    if model_path.endswith('.pth'):
        model = model_cls()
        try: model.load_state_dict(torch.load(model_path)['model_state_dict'])
        except: model.load_state_dict(torch.load(model_path))
        model.eval()
        model_type = 'torch'
    elif model_path.endswith('.onnx'):
        model = onnxruntime.InferenceSession(model_path)
        model_type = 'onnx'
    elif model_path.endswith('.tflite'):
        model = tf.lite.Interpreter(model_path=model_path)
        model.allocate_tensors()
        model_type = 'tflite'
    else: raise Exception('Unknown model type')
    
    f, axes = plt.subplots(1, len(test_loaders), figsize=figsize, sharey='row')
    if len(test_loaders) < 2:
        axes = [axes]
    
    with torch.no_grad():
        for i, (test_loader, test_loader_name) in tqdm(enumerate(zip(test_loaders, test_loaders_names)), total=len(test_loaders), desc='DataLoaders', leave=False):
            history_test = {
                'targets': [],
                'preds': [],
            }

            for data in tqdm(test_loader, total=len(test_loader), desc='Batches', leave=False):
                inputs, targets = data
                inputs = inputs.cpu().detach()
                targets = targets.cpu().detach()
                if model_type == 'torch':
                    preds = model(inputs)
                elif model_type == 'onnx':
                    inputs = inputs.numpy().astype(np.float32 if input_type is None else input_type)
                    preds = model.run(['output'], {"input": inputs})[0]
                elif model_type == 'tflite':
                    input_details = model.get_input_details()
                    output_details = model.get_output_details()
                    input_shape = input_details[0]['shape']
                    inputs = inputs.numpy().reshape(1, 200, 200, 1).astype(np.float32 if input_type is None else input_type)
                    model.set_tensor(input_details[0]['index'], inputs)
                    model.invoke()
                    preds = model.get_tensor(output_details[0]['index'])
                preds = preds.squeeze()

                if use_sigmoid:
                    preds = torch.sigmoid(torch.Tensor(preds))
                    if model_type != 'onnx': 
                        preds = preds.cpu().detach().tolist()
                    if not isinstance(preds, list):
                        preds = [preds]
                else:
                    if model_type == 'torch': 
                        preds = preds.cpu().detach().tolist()
                    if int(preds) != 1:
                        preds = 0
                    if not isinstance(preds, list):
                        preds = [preds]
                
                history_test['targets'].extend(targets.tolist())
                history_test['preds'].extend(preds)
                
                
            if True: # smoothing preds
                #print(history_test['preds'][:4]) # [0.38037407398223877, 0.3886772394180298, 0.4015231132507324, 0.43033134937286377]
                win_s = 1
                history_test['preds'] = [#(history_test['preds'][i-2] + history_test['preds'][i-1] + history_test['preds'][i-0]) / win_s
                                         np.mean(np.array(history_test['preds'][i-win_s+1:i+1]))
                                         for i in range(win_s-1, len(history_test['preds']))]
                history_test['targets'] = history_test['targets'][win_s-1:]
                
                
            acc = accuracy_score(history_test['targets'], 
                                 np.array(history_test['preds']).round())
            f05_score = fbeta_score(history_test['targets'], 
                                    np.array(history_test['preds']).round(), 
                                    beta=.5)
            f2_score = fbeta_score(history_test['targets'], 
                                    np.array(history_test['preds']).round(), 
                                    beta=2.)
            
            cm = confusion_matrix(history_test['targets'], np.array(history_test['preds']).round(), normalize=None)
            disp = ConfusionMatrixDisplay(cm)
            disp.plot(ax=axes[i])
            #disp.ax_.set_title(test_loader_name)
            disp.im_.colorbar.remove()
            disp.ax_.set_xlabel(f'Accuracy: %.4f \nF0.5 score: %.4f \nF2 score: %.4f' % (acc, f05_score, f2_score))
            if i == 0:
                disp.ax_.set_ylabel(ylabel)
            else:
                disp.ax_.set_ylabel('')
            
    f.text(0.4, -0.0, xlabel, ha='left')
    plt.subplots_adjust(wspace=0.40, hspace=0.10)

    f.colorbar(disp.im_, ax=axes)
    f.suptitle(model_path, fontsize=14)
    plt.show()
    
def __test_model(model_path, test_loaders):
    """Test model
    
    Parameters
    ----------
    model_path : str
        Path to the model's save file
    test_loader : torch.utils.data.DataLoader
        data loader for testing set
    """
    print(model_path)
    model = Net()
    try: model.load_state_dict(torch.load(model_path)['model_state_dict'])
    except: model.load_state_dict(torch.load(model_path))
    model.eval()
    
    history_test = {
        'targets': [],
        'preds': [],
    }
    with torch.no_grad():
        for i,data in tqdm(enumerate(test_loader), total=len(test_loader), desc='Batches'):
            inputs, targets = data
            inputs = inputs.cpu().detach()
            targets = targets.cpu().detach()
            preds = model(inputs)
            preds = preds.squeeze()
            history_test['targets'].extend(targets.tolist())
            history_test['preds'].extend(torch.sigmoid(torch.Tensor(preds)).cpu().detach().tolist())

        acc = accuracy_score(history_test['targets'], 
                             np.array(history_test['preds']).round())
        f05_score = fbeta_score(history_test['targets'], 
                                np.array(history_test['preds']).round(), 
                                beta=.5)
        print(f'Accuracy on test dataset:', acc)
        print(f'F0.5 score on test dataset:', f05_score)
        
        cm = confusion_matrix(history_test['targets'], np.array(history_test['preds']).round(), normalize=None)
        fig = ConfusionMatrixDisplay(cm).plot().figure_
        plt.show()