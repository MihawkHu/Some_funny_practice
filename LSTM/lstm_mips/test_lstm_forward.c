// lstm forward test file
// Mihawk Hu 2016/9/1
// all data is random generate

#include "test_lstm_forward.h"
#include "lstm_time.h"

#define TYPE FLOAT32

int test_lstm_forward(struct Matrix *in, struct Matrix *out, struct Matrix *ft, struct Lstm *lstm)
{
    int ret = 0;
    
    // feature transform
    input_feature_trans(in, ft);
    
    // 3 layer lstm nnet
    lstm_forward_propagate(&lstm[0], in, &out[0]);
    lstm_forward_propagate(&lstm[1], &out[0], &out[1]);
    lstm_forward_propagate(&lstm[2], &out[1], &out[2]);
    
    return ret;
}

int main(int argc, const char *argv[])
{
    int ret;
    
    struct Lstm lstm[3];
    struct Matrix in;
    struct Matrix ft;
    struct Matrix out[3];
    
    read_weight_data(lstm, TYPE);
    read_input_data(&in, TYPE);
    read_ft_data(&ft, TYPE);
    
    TIME(ret = test_lstm_forward(&in, out, &ft, lstm););
    
    // clean lstm and free mem
    lstm_clean(&in, out, &ft, lstm);
    
    return ret;
}