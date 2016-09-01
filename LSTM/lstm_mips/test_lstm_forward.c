// lstm forward test file
// Mihawk Hu 2016/9/1
// all data is random generate

#include "test_lstm_forward.h"
#include "lstm_time.h"

#define TYPE FLOAT32

int test_lstm_forward(int argc, const char *argv[])
{
    int ret = 0;
    
    // load lstm nnet data
    struct Lstm lstm[3];
    read_weight_data(lstm, TYPE);
    
    // load input data
    struct Matrix in;
    read_input_data(&in, TYPE);
    
    // load feature transform data
    struct Matrix ft;
    read_ft_data(&ft, TYPE);
    
    // feature transform
    input_feature_trans(&in, &ft);
    
    // 3 layer lstm nnet
    struct Matrix out[3];
    lstm_forward_propagate(&lstm[0], &in, &out[0]);
    
    lstm_forward_propagate(&lstm[1], &out[0], &out[1]);
    
    lstm_forward_propagate(&lstm[2], &out[1], &out[2]);
    
    // clean lstm and free mem
    lstm_clean(&in, out, &ft, lstm);
    
    return ret;
}

int main(int argc, const char *argv[])
{
    int ret;
    
    TIME(ret = test_lstm_forward(argc, argv););
    
    return ret;
}