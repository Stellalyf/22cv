#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "matrix.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        double sum = 0;
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            if(a == LOGISTIC){
                // TODO
                m.data[i][j] = 1.0 / (1.0 + exp(-x));
            } else if (a == RELU){
                // TODO
                m.data[i][j] = x > 0 ? x : 0;
            } else if (a == LRELU){
                // TODO
                m.data[i][j] = x > 0 ? x : 0.1 * x;
            } else if (a == SOFTMAX){
                // TODO
                m.data[i][j] = exp(x);
            }
            sum += m.data[i][j];
        }
        if (a == SOFTMAX) {
            // TODO: have to normalize by sum if we are using SOFTMAX
            for (j = 0; j < m.cols; ++j) { m.data[i][j] /= sum; }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            // TODO: multiply the correct element of d by the gradient
            double s = d.data[i][j];
            if (a == LOGISTIC) {
                d.data[i][j] = x * (1 - x) * s;
            }
            else if (a == RELU) {
                d.data[i][j] = x > 0 ? s : 0;
            }
            else if (a == LRELU) {
                d.data[i][j] = x > 0 ? s : 0.1 * s;
            }
            else if (a == SOFTMAX) {
                d.data[i][j] = s;
            }
        }
    }
}

// Forward propagate information through a layer
// layer *l: pointer to the layer
// matrix in: input to layer
// returns: matrix that is output of the layer
matrix forward_layer(layer *l, matrix in)
{

    l->in = in;  // Save the input for backpropagation


    // TODO: fix this! multiply input by weights and apply activation function.
    matrix out = make_matrix(in.rows, l->w.cols);
    out = matrix_mult_matrix(in, l->w);
 


    free_matrix(l->out);// free the old output
    l->out = out;       // Save the current output for gradient calculation
    activate_matrix(out, l->activation);
    return out;
}

// Backward propagate derivatives through a layer
// layer *l: pointer to the layer
// matrix delta: partial derivative of loss w.r.t. output of layer
// returns: matrix, partial derivative of loss w.r.t. input to layer
matrix backward_layer(layer *l, matrix delta)
{
    // 1.4.1
    // delta is dL/dy
    // TODO: modify it in place to be dL/d(xw)
    gradient_matrix(l->out, l->activation, delta);


    // 1.4.2
    // TODO: then calculate dL/dw and save it in l->dw
    free_matrix(l->dw);
    matrix dw = matrix_mult_matrix(transpose_matrix(l->in), delta); // replace this
    l->dw = dw;
    
    // 1.4.3
    // TODO: finally, calculate dL/dx and return it.
    matrix dx = matrix_mult_matrix(delta, transpose_matrix(l->w)); // replace this

    return dx;
}


// Update the weights at layer l
// layer *l: pointer to the layer
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_layer(layer *l, double rate, double momentum, double decay)
{
    // TODO:
    // Calculate Δw_t = dL/dw_t - λw_t + mΔw_{t-1}
    // save it to l->v
    l->v = axpy_matrix(momentum, l->v, axpy_matrix(-decay, l->w, l->dw));
    //

    // Update l->w
    l->w = axpy_matrix(rate, l->v, l->w);

    // Remember to free any intermediate results to avoid memory leaks

}

// Make a new layer for our model
// int input: number of inputs to the layer
// int output: number of outputs from the layer
// ACTIVATION activation: the activation function to use
layer make_layer(int input, int output, ACTIVATION activation)
{
    layer l;
    l.in  = make_matrix(1,1);
    l.out = make_matrix(1,1);
    l.w   = random_matrix(input, output, sqrt(2./input));
    l.v   = make_matrix(input, output);
    l.dw  = make_matrix(input, output);
    l.activation = activation;
    return l;
}

// Run a model on input X
// model m: model to run
// matrix X: input to model
// returns: result matrix
matrix forward_model(model m, matrix X)
{
    int i;
    for(i = 0; i < m.n; ++i){
        X = forward_layer(m.layers + i, X);
    }
    return X;
}

// Run a model backward given gradient dL
// model m: model to run
// matrix dL: partial derivative of loss w.r.t. model output dL/dy
void backward_model(model m, matrix dL)
{
    matrix d = copy_matrix(dL);
    int i;
    for(i = m.n-1; i >= 0; --i){
        matrix prev = backward_layer(m.layers + i, d);
        free_matrix(d);
        d = prev;
    }
    free_matrix(d);
}

// Update the model weights
// model m: model to update
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_model(model m, double rate, double momentum, double decay)
{
    int i;
    for(i = 0; i < m.n; ++i){
        update_layer(m.layers + i, rate, momentum, decay);
    }
}

// Find the index of the maximum element in an array
// double *a: array
// int n: size of a, |a|
// returns: index of maximum element
int max_index(double *a, int n)
{
    if(n <= 0) return -1;
    int i;
    int max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max){
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

// Calculate the accuracy of a model on some data d
// model m: model to run
// data d: data to run on
// returns: accuracy, number correct / total
double accuracy_model(model m, data d)
{
    matrix p = forward_model(m, d.X);
    int i;
    int correct = 0;
    for(i = 0; i < d.y.rows; ++i){
        if(max_index(d.y.data[i], d.y.cols) == max_index(p.data[i], p.cols)) ++correct;
    }
    return (double)correct / d.y.rows;
}

// Calculate the cross-entropy loss for a set of predictions
// matrix y: the correct values
// matrix p: the predictions
// returns: average cross-entropy loss over data points, 1/n Σ(-ylog(p))
double cross_entropy_loss(matrix y, matrix p)
{
    int i, j;
    double sum = 0;
    for(i = 0; i < y.rows; ++i){ // y.rows = 128
        for(j = 0; j < y.cols; ++j){
            sum += -y.data[i][j]*log(p.data[i][j]);
        }
    }
    return sum/y.rows;
}


// Train a model on a dataset using SGD
// model m: model to train
// data d: dataset to train on
// int batch: batch size for SGD
// int iters: number of iterations of SGD to run (i.e. how many batches)
// double rate: learning rate
// double momentum: momentum
// double decay: weight decay
void train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay)
{
    int e;
    for(e = 0; e < iters; ++e){
        data b = random_batch(d, batch); // X: [128,785]  y: [128,10]
        matrix p = forward_model(m, b.X);
        fprintf(stderr, "%06d: Loss: %f\n", e, cross_entropy_loss(b.y, p));
        matrix dL = axpy_matrix(-1, p, b.y); // partial derivative of loss dL/dy
        backward_model(m, dL);
        update_model(m, rate/batch, momentum, decay);
        free_matrix(dL);
        free_data(b);
    }
}


// Questions 
//
// 5.2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
// TODO
// Because the training accuracy represents the performance of the model in the training set, we need it to optimize the model. 
//  The test accuracy represents the performance of the model in the test set. We need to use it to predict the performance of the model on the real data, so as to judge the quality of the model.
//
// 5.2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
// TODO
//Too large or too small learning rate will lead to the decline of accuracy, especially when it is too large. 
//The accuracy performance is the best when the learning rate is around 10^-1 and 10^-2.
// 
// 5.2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the final model training and test accuracy?
// TODO
//With the decay become smaller the training accuracy and testing accuracy of the model are improved. 
//However, when the decay decreases to a certain extent, these two accuracy rates will be stable at a certain value.
//
// 5.2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they perform? What's best?
// TODO
//For LOGISTIC activation:training accuracy: 88.76% ;  test accuracy:     89.34%
//For RELU activation:training accuracy: 92.30% ;  test accuracy:     92.4%
//For LRELU activation:training accuracy: 92.07% ;  test accuracy:     92.17%
//For SOFTMAX activation:training accuracy: 58.14% ;  test accuracy:     58.67%
//All models performed well exxcept softmax. The best of them is the model with RELU activation.
//
// 5.2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
// TODO
//Using RELU activation, the best learing rate is 10^-1.
//	training accuracy: 12.3% ;  test accuracy:     12.63%  (learning rate: 10)
//	training accuracy: 96.115% ;  test accuracy:     95.64%  (learning rate: 10^-1)
//	training accuracy: 92.31% ;  test accuracy:     92.4%  (learning rate: 10^-2)
//	training accuracy: 86.44% ;  test accuracy:     86.82%  (learning rate: 10^-3)
//	training accuracy: 59.24% ;  test accuracy:     59.15%  (learning rate: 10^-4)
//
// 5.2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
// TODO
//From decay 10->10^-4, with the decay become smaller the training accuracy and testing accuracy of the model are improved.
//But when it is less than 10^-2, the change in accuracy starts to become so small that it is almost negligible. It can be useful because it can give us hints about overfitting.
// 5.2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
// TODO
//When values of decay is 10^-4, the model is best. The model with decay of 10^-3 is also very close. Because their test accuracy is highest.
//	training accuracy: 11.24% ;  test accuracy:     11.35%  (decay:10)
//	training accuracy: 89.14% ;  test accuracy:     89.63%  (decay:10^0)
//	training accuracy: 92.645% ;  test accuracy:     92.65%  (decay:10^-1)
//	training accuracy: 93.14% ;  test accuracy:     93.08%  (decay:10^-2)
//	training accuracy: 93.19% ;  test accuracy:     93.12%  (decay:10^-3)
//	training accuracy: 93.195% ;  test accuracy:     93.12%  (decay:10^-4)
//
// 5.3.2.1 How well does your network perform on the CIFAR dataset?
// TODO
//I got training accuracy: 36.318% ;  test accuracy:     35.09%  (SOFTMAX MODEL)
//	training accuracy: 35.172% ;  test accuracy:     35.15%  (neural net:LOGISTIC)
//	training accuracy: 38.738% ;  test accuracy:     38.09%  (neural net:RELU)
//	training accuracy: 40.398% ;  test accuracy:     39.82%  (neural net:LRELU)
//	training accuracy: 20.866% ;  test accuracy:     20.56%  (neural net:SOFTMAX)
//The accuracy rate is very low and the model performance is not good. The accuracy of all models was less tha 50%.



