import numpy as np
from sklearn.metrics import accuracy_score, fbeta_score, confusion_matrix, ConfusionMatrixDisplay
from timeit import default_timer as timer
import torch
import torch.nn as nn
from torch.utils.tensorboard import SummaryWriter
from tqdm.notebook import tqdm

def initialize_weights(m):
    """Initializes the model's weights with Xavier uniform.

    Parameters
    ----------
    m : Model (nn.Module)
        model to initialize
    """
    if type(m) == nn.Linear or type(m) == nn.Conv2d :
        torch.nn.init.xavier_uniform_(m.weight)
        
def _save_history(targets, preds, history_):
    """Saves targets and preds to history.

    Parameters
    ----------
    targets : Tensor
        Batch targets
    preds : Tensor
        Batch model predictions
    history_ : dict
        Dict with keys 'targets', 'keys' to store epochs' history.
        
    Returns
    -------
    dict 
        Updated variable history_ with batch targets, and preds
    """
    local_targets = targets.cpu().detach().numpy()
    history_['targets'][-1].extend(local_targets.tolist())
    history_['preds'][-1].extend(torch.sigmoid(preds).cpu().detach().tolist())
    return history_

def _log_cm(targets, preds, title, tb_writer, epoch):
    """Logs the confusion matrix to Tensorboard.
    
    Parameters
    ----------
    targets : Tensor
        Batch targets
    preds : Tensor
        Batch model predictions
    title : string
        Confusion matrix title
    tb_writer : SummaryWriter
        Tensorboard's SummaryWriter
    epoch : int
        epoch's index
    """
    cm = confusion_matrix(targets, preds, normalize=None)
    fig = ConfusionMatrixDisplay(cm).plot().figure_
    tb_writer.add_figure(title, fig, epoch+1)
    
# training related utility functions
def train_one_epoch(model, optimizer, criterion, training_loader, epoch, tb_writer, history, device):
    """Train for one epoch
    
    Parameters
    ----------
    model : Model (nn.Module)
        model to initialize
    optimizer : torch.optim
        optimizer object
    criterion : function
        loss function
    training_loader : torch.utils.data.DataLoader
        data loader for training set
    epoch : int
        epoch's index
    tb_writer : SummaryWriter
        Tensorboard's SummaryWriter
    history : dict
        dict to store epochs' history
    
    Returns
    -------
    float
        average loss for this epoch
    float
        F0.5 score for this epoch
    float
        F2 score for this epoch
    """
    _start_t = timer()
    epoch_loss = 0.0
    model.train()
    for i, data in tqdm(enumerate(training_loader, 0), leave=False, total=len(training_loader), desc='Mini-batches'): # for each mini-batch

        # get the inputs and put them on the computation device
        inputs, targets = data
        inputs = inputs.to(device)
        targets = targets.to(device)

        # zero the gradients
        optimizer.zero_grad()

        # forward pass
        preds = model(inputs)
        preds = preds.squeeze()

        # compute loss
        loss = criterion(preds, targets)

        # backward pass
        loss.backward()

        # optimization
        optimizer.step()

        # log statistics
        ## save history
        history['train'] = _save_history(targets, preds, history['train'])
        ## log loss
        epoch_loss += loss.item()
        minibatch_ix = epoch*len(training_loader)+i
        if minibatch_ix % 50 == 0:
            tb_writer.add_scalar('Running loss (mini-batches)', loss.item(), minibatch_ix)
    _end_t = timer()
    print('Time elapsed for epoch %i: %.2f minutes.' % (epoch+1, (_end_t-_start_t)/60.))

    # log epoch metrics
    f05_score = fbeta_score(history['train']['targets'][-1], 
                            np.array(history['train']['preds'][-1]).round(), 
                            beta=.5)
    f2_score = fbeta_score(history['train']['targets'][-1], 
                           np.array(history['train']['preds'][-1]).round(), 
                           beta=2.)
    
    # log epoch conf matrix
    _log_cm(history['train']['targets'][-1], np.array(history['train']['preds'][-1]).round(), 'Confusion matrix/train', tb_writer, epoch)
    
    avg_epoch_loss = epoch_loss / len(training_loader)
    return avg_epoch_loss, f05_score, f2_score

def validate_one_epoch(model, criterion, validation_loader, epoch, tb_writer, history, device):
    """Evaluate model after an epoch
    
    Parameters
    ----------
    model : Model (nn.Module)
        model to initialize
    criterion : function
        loss function
    validation_loader : torch.utils.data.DataLoader
        data loader for validation set
    epoch : int
        epoch's index
    tb_writer : SummaryWriter
        Tensorboard's SummaryWriter
    history : dict
        dict to store epochs' history
    
    Returns
    -------
    float
        validation loss for this epoch
    float
        F0.5 score for this epoch
    float
        F2 score for this epoch
    """
    model.eval()
    with torch.no_grad():
        val_loss = 0.0
        for inputs, targets in tqdm(validation_loader, leave=False, total=len(validation_loader), desc='Mini-batches'):
            inputs = inputs.to(device)
            targets = targets.to(device)
            
            # predict & compute loss
            preds = model(inputs)
            preds = preds.squeeze()
            loss = criterion(preds, targets)
            
            # log statistics
            val_loss += loss.item()
            history['validation'] = _save_history(targets, preds, history['validation'])

        # log epoch loss (val)
        val_loss /= len(validation_loader)
        
        # log epoch accuracy (val)
        val_accuracy = accuracy_score(history['validation']['targets'][-1], 
                                      np.array(history['validation']['preds'][-1]).round())
        tb_writer.add_scalar('Accuracy/validation', val_accuracy, epoch+1)
        
        # log epoch F0.5 score (val)
        f05_score = fbeta_score(history['validation']['targets'][-1], 
                                np.array(history['validation']['preds'][-1]).round(), 
                                beta=.5)
        f2_score = fbeta_score(history['validation']['targets'][-1], 
                               np.array(history['validation']['preds'][-1]).round(), 
                               beta=2.)
        
        # write conf matrix for epoch
        _log_cm(history['validation']['targets'][-1], np.array(history['validation']['preds'][-1]).round(), 'Confusion matrix/validation', tb_writer, epoch)
        
        return val_loss, f05_score, f2_score

def train(model, optimizer, criterion, training_loader, validation_loader, n_epochs, history, early_stopper, model_path, device, lr_scheduler=None):
    """Train model
    
    Parameters
    ----------
    model : Model (nn.Module)
        model to initialize
    optimizer : torch.optim
        optimizer object
    criterion : function
        loss function
    training_loader : torch.utils.data.DataLoader
        data loader for training set
    validation_loader : torch.utils.data.DataLoader
        data loader for validation set
    n_epochs : int
        maximum number of epochs
    history : dict
        dict to store epochs' history
    early_stopper : EarlyStopper
        EarlyStopper object that stop training if validation loss stops decreasing
    model_path : str
        Path to the model's save file
    lr_scheduler : torch.optim.lr_scheduler, optional, default: None
        optimizer's scheduler
    
    """
    with SummaryWriter() as tb_writer: # TensorBoard logger
        for epoch in tqdm(range(n_epochs), desc='Epochs'):  # loop over the dataset multiple times

            # init history
            for k1,k2 in [(k1,k2) for k1 in history.keys() for k2 in history[k1].keys()]:
                history[k1][k2].append([])

            # training
            training_loss, training_f05score, training_f2score = train_one_epoch(model, optimizer, criterion, training_loader, epoch, tb_writer, history, device)

            # validation
            validation_loss, validation_f05score, validation_f2score = validate_one_epoch(model, criterion, validation_loader, epoch, tb_writer, history, device)
            
            # log metrics
            print('Losses train/val (epoch %i): %.4f %.4f' % (epoch+1, training_loss, validation_loss))
            print('F0.5 scores train/val (epoch %i): %.4f %.4f' % (epoch+1, training_f05score, validation_f05score))
            print('F2 scores train/val (epoch %i): %.4f %.4f\n' % (epoch+1, training_f2score, validation_f2score))
            tb_writer.add_scalar('F0.5 score/train', training_f05score, epoch+1)
            tb_writer.add_scalar('F0.5 score/validation', validation_f05score, epoch+1)
            tb_writer.add_scalar('F2 score/train', training_f2score, epoch+1)
            tb_writer.add_scalar('F2 score/validation', validation_f2score, epoch+1)
            tb_writer.add_scalar('Loss/train', training_loss, epoch+1)
            tb_writer.add_scalar('Loss/validation', validation_loss, epoch+1)
            
            # learning rate scheduler
            if lr_scheduler is not None:
                lr_scheduler.step()
                
            # early stopping
            if early_stopper.early_stop(validation_loss):             
                break
            if early_stopper.has_model_improved(): # save model if it has improved
                torch.save({'epoch': epoch,
                            'model_state_dict': model.state_dict(),
                            'optimizer_state_dict': optimizer.state_dict(),
                            'criterion': criterion,
                            'training_f05score': training_f05score,
                            'validation_f05score': validation_f05score,
                            'training_f2score': training_f2score,
                            'validation_f2score': validation_f2score,
                            'training_loss': training_loss,
                            'validation_loss': validation_loss,
                           }, model_path)

    print('Finished Training')