#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "matrix_cal_mips.h"

#define DD 5

#define ROW_MERGE 0
#define COL_MERGE 1
#define FIX16 0
#define FLOAT32 1

struct Matrix {
    int row;
    int col;
    
    int store_type;
    int data_type;
    
    int16_t *data;
    float *data_;
};

void matrix_config(struct Matrix *mat, int r, int c, int st, int dt, int16_t *d, float *d_)
{
    mat->row = r;
    mat->col = c;
    mat->store_type = st;
    mat->data_type = dt;
    
    if (dt == FIX16) {
        mat->data = (int16_t*)malloc(r * c * sizeof(int16_t));
        if (d != NULL) {
            memcpy(mat->data, d, r * c * sizeof(int16_t));
        }
        else {
            memset(mat->data, 0, r * c * sizeof(int16_t));
        }
    }
    else if (dt == FLOAT32) {
        mat->data_ = (float*)malloc(r * c * sizeof(float));
        if (d_ != NULL) {
            memcpy(mat->data_, d_, r * c * sizeof(float));
        }
        else {
            memset(mat->data_, 0, r * c * sizeof(float));
        }
    }
}

struct Lstm {
    int data_type;
    
    int learn_rate_coef_;
    int bias_learn_rate_coef_;
    int max_norm_;
    int cell_dim_;
    int grad_clip_;
    int proj_dim_;
    
    struct Matrix w_gifo_x_;
    struct Matrix w_gifo_r_;
    
    struct Matrix bias_;
    
    struct Matrix peephole_i_c_;
    struct Matrix peephole_f_c_;
    struct Matrix peephole_o_c_;
    
    struct Matrix w_r_m_;
    
    
    struct Matrix propagate_buf_;
    struct Matrix prev_nnet_state_;
    
};

void random_init_mat(struct Matrix *mat, int avg, int range, int data_type) 
{
    srand(time(NULL));
    if (data_type == FLOAT32) {
        for (int i = 0; i < mat->row * mat->col; ++i) {
            mat->data_[i] = rand() / (float)(RAND_MAX / range) - range / 2 - avg;
        }
    }
    else if (data_type == FIX16) {
        for (int i = 0; i < mat->row * mat->col; ++i) {
            mat->data[i] = (int16_t)rand() % (range * 128) - (range / 2 + avg) * 128;
        }
    }
}

void read_weight_data(struct Lstm lstm[], int data_type)
{
    if (data_type == FLOAT32) {
        for (int r = 0; r < 3; ++r) {
            lstm[r].data_type = FLOAT32;
            lstm[r].bias_learn_rate_coef_ = 1;
            lstm[r].bias_learn_rate_coef_ = 1;
            lstm[r].max_norm_ = 0;
            lstm[r].cell_dim_ = 400;
            lstm[r].grad_clip_ = 20;
            lstm[r].proj_dim_ = 128;
            
            // default config matrix
            matrix_config(&(lstm[r].w_gifo_x_), 40, 1600, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].w_gifo_r_), 128, 1600, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].bias_), 1, 1600, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].peephole_i_c_), 1, 400, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].peephole_f_c_), 1, 400, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].peephole_o_c_), 1, 400, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].w_r_m_), 400, 128, ROW_MERGE, FLOAT32, NULL, NULL);
            matrix_config(&(lstm[r].prev_nnet_state_), 0, 0, COL_MERGE, FLOAT32, NULL, NULL);
            
            // random asgin data
            random_init_mat(&(lstm[r].w_gifo_x_), 0, 2, FLOAT32);
            random_init_mat(&(lstm[r].w_gifo_r_), 0, 2, FLOAT32);
            random_init_mat(&(lstm[r].bias_), 0, 2, FLOAT32);
            random_init_mat(&(lstm[r].peephole_i_c_), 0, 3, FLOAT32);
            random_init_mat(&(lstm[r].peephole_f_c_), 0, 3, FLOAT32);
            random_init_mat(&(lstm[r].peephole_o_c_), 0, 3, FLOAT32);
            random_init_mat(&(lstm[r].w_r_m_), 0, 2, FLOAT32);
            random_init_mat(&(lstm[r].prev_nnet_state_), 0, 2, FLOAT32);
            
        }
    }
    else if (data_type == FIX16) {
        for (int r = 0; r < 3; ++r) {
            lstm[r].data_type = FIX16;
            lstm[r].bias_learn_rate_coef_ = 1;
            lstm[r].bias_learn_rate_coef_ = 1;
            lstm[r].max_norm_ = 0;
            lstm[r].cell_dim_ = 400;
            lstm[r].grad_clip_ = 20;
            lstm[r].proj_dim_ = 128;
            
            // default config matrix
            matrix_config(&(lstm[r].w_gifo_x_), 40, 1600, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].w_gifo_r_), 128, 1600, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].bias_), 1, 1600, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].peephole_i_c_), 1, 400, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].peephole_f_c_), 1, 400, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].peephole_o_c_), 1, 400, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].w_r_m_), 400, 128, ROW_MERGE, FIX16, NULL, NULL);
            matrix_config(&(lstm[r].prev_nnet_state_), 0, 0, COL_MERGE, FIX16, NULL, NULL);
            
            // random asgin data
            random_init_mat(&(lstm[r].w_gifo_x_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].w_gifo_r_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].bias_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].peephole_i_c_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].peephole_f_c_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].peephole_o_c_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].w_r_m_), 0, 2, FIX16);
            random_init_mat(&(lstm[r].prev_nnet_state_), 0, 2, FIX16);

        }
    }
}

void read_input_data(struct Matrix *in, int data_type)
{
    if (data_type == FLOAT32) {
        matrix_config(in, DD, 40, COL_MERGE, FLOAT32, NULL, NULL);
        
        random_init_mat(in, 3, 6, FLOAT32);
    }
    else if (data_type == FIX16) {
        matrix_config(in, DD, 40, COL_MERGE, FIX16, NULL, NULL);
        
        random_init_mat(in, 2, 4, FIX16);
    }
}

void read_ft_data(struct Matrix *ft, int data_type)
{    
    if (data_type == FLOAT32) {
        matrix_config(ft, 1, 80, ROW_MERGE, FLOAT32, NULL, NULL);
        
        srand(time(NULL));
        for (int i = 0; i < 40; ++i) {
            ft->data_[i] = (rand() / (float)(RAND_MAX) - 0.5) / (10000.0);
        }
        for (int i = 40; i < 80; ++i) {
            ft->data_[i] = rand() / (float)(RAND_MAX) / 2.0;
        }
        
    }
    else if (data_type == FIX16) {
        matrix_config(ft, 1, 80, ROW_MERGE, FIX16, NULL, NULL);
        
        srand(time(NULL));
        for (int i = 0; i < 40; ++i) {
            ft->data[i] = (int16_t)rand() % 128 - 64;
        }
        for (int i = 40; i < 80; ++i) {
            ft->data[i] = (int16_t)rand() % 256;
        }
    }
    
}

void input_feature_trans(struct Matrix *in, struct Matrix *ft)
{
    if (in->data_type == FLOAT32) {
        float *addshift = &(ft->data_[0]);
        float *rescale = &(ft->data_[40]);
        
        for (int j = 0; j < in->col; ++j) {
            for (int i = 0; i < in->row; ++i) {
                in->data_[j * in->row + i] = rescale[j] * (in->data_[j * in->row + j] + addshift[j]);
            }
        }
        
    }
    else if (in->data_type == FIX16) {
        int16_t *addshift = &(ft->data[0]);
        int16_t *rescale = &(ft->data[40]);
        
        for (int j = 0; j < in->col; ++j) {
            for (int i = 0; i < in->row; ++i) {

                in->data[j * in->row + i] = fixed16_xmul(rescale[j], fixed16_add(in->data[j * in->row + j], addshift[j]));
            }
        }
    }
}

float Sigmoid_float32(float x)
{
    return 1 / (1 + exp(-x));
}

int16_t Sigmoid_fix16(int16_t x)
{
    return fixed16_xdiv(FIXED16_ONE, (FIXED16_ONE + fixed16_exp(-x)));
}

int16_t Tanh_fix16(int16_t x)
{
    return fixed16(tanhf(fixed16_tofloat(x)));
}

void lstm_forward_propagate(struct Lstm *lstm, struct Matrix *in, struct Matrix *out)
{
    
    int cell_dim_ = lstm->cell_dim_;
    int proj_dim_ = lstm->proj_dim_;
    
    struct Matrix *propagate_buf_ = &(lstm->propagate_buf_);
    struct Matrix *w_gifo_x_ = &(lstm->w_gifo_x_);
    struct Matrix *w_gifo_r_ = &(lstm->w_gifo_r_);
    struct Matrix *w_r_m_ = &(lstm->w_r_m_);
    struct Matrix *peephole_i_c_ = &(lstm->peephole_i_c_);
    struct Matrix *peephole_f_c_ = &(lstm->peephole_f_c_);
    struct Matrix *peephole_o_c_ = &(lstm->peephole_o_c_);
    struct Matrix *prev_nnet_state_ = &(lstm->prev_nnet_state_);
    struct Matrix *bias_ = &(lstm->bias_);
    
    int T = in->row;
    int S = 1;
    
    if (lstm->data_type == FLOAT32) {
        
        matrix_config(propagate_buf_, (T + 2) * S, 7 * cell_dim_ + proj_dim_, ROW_MERGE, 
                        FLOAT32, NULL, NULL);
        if (prev_nnet_state_->row != S) {
            matrix_config(prev_nnet_state_, S, 7 * cell_dim_ + proj_dim_, ROW_MERGE, FLOAT32, NULL, NULL);
        }
        else {
            memcpy(propagate_buf_->data_, prev_nnet_state_, S * prev_nnet_state_->col * sizeof(float));
        }

        
        // split activations by neuron types,
        struct Matrix YG;
        matrix_config(&YG, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YG.col; ++j) {
            memcpy(&(YG.data_[j * YG.row]), &(propagate_buf_->data_[j * propagate_buf_->row]), YG.row * sizeof(float));
        }
        
        struct Matrix YI;
        matrix_config(&YI, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YI.col; ++j) {
            memcpy(&(YI.data_[j * YI.row]), &(propagate_buf_->data_[(j + cell_dim_) * propagate_buf_->row + cell_dim_]), YG.row * sizeof(float));
        }
        struct Matrix YF;
        matrix_config(&YF, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YF.col; ++j) {
            memcpy(&(YF.data_[j * YF.row]), &(propagate_buf_->data_[(j + 2 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        struct Matrix YO;
        matrix_config(&YO, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YO.col; ++j) {
            memcpy(&(YO.data_[j * YO.row]), &(propagate_buf_->data_[(j + 3 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        struct Matrix YC;
        matrix_config(&YC, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YC.col; ++j) {
            memcpy(&(YC.data_[j * YC.row]), &(propagate_buf_->data_[(j + 4 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        
        struct Matrix YH;
        matrix_config(&YH, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YH.col; ++j) {
            memcpy(&(YH.data_[j * YH.row]), &(propagate_buf_->data_[(j + 5 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        struct Matrix YM;
        matrix_config(&YM, propagate_buf_->row, cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YM.col; ++j) {
            memcpy(&(YM.data_[j * YM.row]), &(propagate_buf_->data_[(j + 6 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        struct Matrix YR;
        matrix_config(&YR, propagate_buf_->row, proj_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YR.col; ++j) {
            memcpy(&(YR.data_[j * YR.row]), &(propagate_buf_->data_[(j + 7 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(float));
        }
        struct Matrix YGIFO;
        matrix_config(&YGIFO, propagate_buf_->row, 4 * cell_dim_, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YGIFO.col; ++j) {
            memcpy(&(YGIFO.data_[j * YGIFO.row]), &(propagate_buf_->data_[j * propagate_buf_->row]), YG.row * sizeof(float));
        }
        
        // x -> g, i, f, o, not recurrent, do it all in once
        matrix_mul_float32(in->row, w_gifo_x_->col, in->col, 1.0, in->data_, in->row, w_gifo_x_->data_, w_gifo_x_->col, 0.0, &(YGIFO.data_[S]), YGIFO.row);
        
        // bias -> g, i, f, o
        for (int i = 0; i < bias_->col; ++i) {
            float tt = bias_->data_[i];
            for (int j = 0; j < T * S; ++j) {
                YGIFO.data_[i * YGIFO.row + j + S] += tt;
            }
        }
        
        for (int t = 1; t <= T; ++t) {
            // multistream buffers for current time-step,
            struct Matrix y_all;
            matrix_config(&y_all, S, propagate_buf_->col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_all.row; ++i) {
                for (int j = 0 ; j < y_all.col; ++j) {
                    y_all.data_[j * y_all.row + i] = propagate_buf_->data_[(j + t * S) * propagate_buf_->row + i];
                }
            }
            
            struct Matrix y_g;
            matrix_config(&y_g, S, YG.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_g.row; ++i) {
                for (int j = 0 ; j < y_g.col; ++j) {
                    y_g.data_[j * y_g.row + i] = YG.data_[(j + t * S) * YG.row + i];
                }
            }
            struct Matrix y_i;
            matrix_config(&y_i, S, YI.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_i.row; ++i) {
                for (int j = 0 ; j < y_i.col; ++j) {
                    y_i.data_[j * y_i.row + i] = YI.data_[(j + t * S) * YI.row + i];
                }
            }
            struct Matrix y_f;
            matrix_config(&y_f, S, YF.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_f.row; ++i) {
                for (int j = 0 ; j < y_f.col; ++j) {
                    y_f.data_[j * y_f.row + i] = YF.data_[(j + t * S) * YF.row + i];
                }
            }
            struct Matrix y_o;
            matrix_config(&y_o, S, YO.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_o.row; ++i) {
                for (int j = 0 ; j < y_o.col; ++j) {
                    y_o.data_[j * y_o.row + i] = YO.data_[(j + t * S) * YO.row + i];
                }
            }
            struct Matrix y_c;
            matrix_config(&y_c, S, YC.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_c.row; ++i) {
                for (int j = 0 ; j < y_c.col; ++j) {
                    y_c.data_[j * y_c.row + i] = YC.data_[(j + t * S) * YC.row + i];
                }
            }
            struct Matrix y_h;
            matrix_config(&y_h, S, YH.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_h.row; ++i) {
                for (int j = 0 ; j < y_h.col; ++j) {
                    y_h.data_[j * y_h.row + i] = YH.data_[(j + t * S) * YH.row + i];
                }
            }
            struct Matrix y_m;
            matrix_config(&y_m, S, YM.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_m.row; ++i) {
                for (int j = 0 ; j < y_m.col; ++j) {
                    y_m.data_[j * y_m.row + i] = YM.data_[(j + t * S) * YM.row + i];
                }
            }
            struct Matrix y_r;
            matrix_config(&y_r, S, YR.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_r.row; ++i) {
                for (int j = 0 ; j < y_r.col; ++j) {
                    y_r.data_[j * y_r.row + i] = YR.data_[(j + t * S) * YR.row + i];
                }
            }
            struct Matrix y_gifo;
            matrix_config(&y_gifo, S, YGIFO.col, COL_MERGE, FLOAT32, NULL, NULL);
            for (int i = 0; i < y_gifo.row; ++i) {
                for (int j = 0 ; j < y_gifo.col; ++j) {
                    y_gifo.data_[j * y_gifo.row + i] = YGIFO.data_[(j + t * S) * YGIFO.row + i];
                }
            }
            
            
            // r(t-1) -> g, i, f, o
            matrix_mul_float32(S, w_gifo_r_->col, YR.col, 1.0, &(YR.data_[(t - 1) * S]), YR.row, w_gifo_x_->data_, w_gifo_x_->col, 1.0, y_gifo.data_, y_gifo.row);
            
            // c(t-1) -> i(t) via peephole
            for (int j = 0; j < y_i.col; ++j) {
                float tt = peephole_i_c_->data_[j];
                for (int i = 0; i < y_i.row; ++i) {
                    y_i.data_[y_i.row * j + i] += YC.data_[YC.row * j + (t - 1) * S] * tt;
                }
            }
            
            // c(t-1) -> f(t) via peephole
            for (int j = 0; j < y_f.col; ++j) {
                float tt = peephole_f_c_->data_[j];
                for (int i = 0; i < y_f.row; ++i) {
                    y_f.data_[y_f.row * j + i] += YC.data_[YC.row * j + (t - 1) * S] * tt;
                }
            }
            
            // i, f sigmoid squashing
            for (int i = 0; i < y_i.row * y_i.col; ++i) y_i.data_[i] = Sigmoid_float32(y_i.data_[i]);
            for (int i = 0; i < y_f.row * y_f.col; ++i) y_f.data_[i] = Sigmoid_float32(y_f.data_[i]);
            
            // g tanh squashing
            for (int i = 0; i < y_g.row * y_g.col; ++i) y_g.data_[i] = tanh(y_g.data_[i]);
            
            // g * i -> c
            for (int j = 0; j < y_c.col; ++j) {
                for (int i  = 0; i < y_c.row; ++i) {
                    y_c.data_[y_c.row * j + i] = y_g.data_[y_g.row * j + i] * y_i.data_[y_i.row * j + i];
                }
            }
            
            // c(t-1) * f -> c(t) via forget-gate
            for (int j = 0; j < y_c.col; ++j) {
                for (int i  = 0; i < y_c.row; ++i) {
                    y_c.data_[y_c.row * j + i] += YC.data_[YC.row * j + i + (t - 1) * S] * y_f.data_[y_f.row * j + i];
                }
            }
            
            // c(t) -> o(t) via peephole (non-recurrent, using c(t))
            for (int j = 0; j < y_o.col; ++j) {
                float tt = peephole_o_c_->data_[j];
                for (int i = 0; i < y_o.row; ++i) {
                    y_o.data_[y_o.row * j + i] += y_c.data_[y_c.row * j] * tt;
                }
            }
            
            // o sigmoid squashing
            for (int i = 0; i < y_o.row * y_o.col; ++i) y_o.data_[i] = Sigmoid_float32(y_o.data_[i]);
            
            // h tanh squashing
            for (int i = 0; i < y_c.row * y_c.col; ++i) y_h.data_[i] = tanh(y_c.data_[i]);
            
            // h * o -> m via output gate
            for (int j = 0; j < y_m.col; ++j) {
                for (int i  = 0; i < y_m.row; ++i) {
                    y_m.data_[y_m.row * j + i] = y_h.data_[y_h.row * j + i] * y_o.data_[y_o.row * j + i];
                }
            }
            
            // m -> r
            matrix_mul_float32(y_m.row, w_r_m_->col, y_m.col, 1.0, y_m.data_, y_m.row, w_r_m_->data_, w_r_m_->col, 0.0, y_r.data_, y_r.row);
            
            free(y_all.data_);
            free(y_g.data_);
            free(y_i.data_);
            free(y_f.data_);
            free(y_o.data_);
            free(y_c.data_);
            free(y_h.data_);
            free(y_m.data_);
            free(y_r.data_);
            free(y_gifo.data_);
            
        }
        
        // set the 'projection layer' output as the LSTM output
        matrix_config(out, T * S, YR.col, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < YR.col; ++j) {
            memcpy(&(out->data_[j * out->row]), &(YR.data_[j * YR.row + S]), out->row * sizeof(float));
        }
        
        // the state in the last 'frame' is transferred (can be zero vector)
        matrix_config(prev_nnet_state_, S, propagate_buf_->col, COL_MERGE, FLOAT32, NULL, NULL);
        for (int j = 0; j < propagate_buf_->col; ++j) {
            for (int i = 0; i < prev_nnet_state_->row; ++i) {
                prev_nnet_state_->data_[j * prev_nnet_state_->row] = propagate_buf_->data_[j * propagate_buf_->row + T * S];
            }
        }
        
        free(YG.data_);
        free(YI.data_);
        free(YF.data_);
        free(YO.data_);
        free(YC.data_);
        free(YH.data_);
        free(YM.data_);
        free(YR.data_);
        free(YGIFO.data_);
        
    }
    else if (lstm->data_type == FIX16) {
        
        matrix_config(propagate_buf_, (T + 2) * S, 7 * cell_dim_ + proj_dim_, ROW_MERGE, 
                        FIX16, NULL, NULL);
        if (prev_nnet_state_->row != S) {
            matrix_config(prev_nnet_state_, S, 7 * cell_dim_ + proj_dim_, ROW_MERGE, FIX16, NULL, NULL);
        }
        else {
            memcpy(propagate_buf_->data, prev_nnet_state_, S * prev_nnet_state_->col * sizeof(int16_t));
        }
        
        
        // split activations by neuron types,
        struct Matrix YG;
        matrix_config(&YG, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YG.col; ++j) {
            memcpy(&(YG.data[j * YG.row]), &(propagate_buf_->data[j * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        
        struct Matrix YI;
        matrix_config(&YI, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YI.col; ++j) {
            memcpy(&(YI.data[j * YI.row]), &(propagate_buf_->data[(j + cell_dim_) * propagate_buf_->row + cell_dim_]), YG.row * sizeof(int16_t));
        }
        struct Matrix YF;
        matrix_config(&YF, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YF.col; ++j) {
            memcpy(&(YF.data[j * YF.row]), &(propagate_buf_->data[(j + 2 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YO;
        matrix_config(&YO, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YO.col; ++j) {
            memcpy(&(YO.data[j * YO.row]), &(propagate_buf_->data[(j + 3 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YC;
        matrix_config(&YC, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YC.col; ++j) {
            memcpy(&(YC.data[j * YC.row]), &(propagate_buf_->data[(j + 4 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YH;
        matrix_config(&YH, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YH.col; ++j) {
            memcpy(&(YH.data[j * YH.row]), &(propagate_buf_->data[(j + 5 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YM;
        matrix_config(&YM, propagate_buf_->row, cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YM.col; ++j) {
            memcpy(&(YM.data[j * YM.row]), &(propagate_buf_->data[(j + 6 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YR;
        matrix_config(&YR, propagate_buf_->row, proj_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YR.col; ++j) {
            memcpy(&(YR.data[j * YR.row]), &(propagate_buf_->data[(j + 7 * cell_dim_) * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        struct Matrix YGIFO;
        matrix_config(&YGIFO, propagate_buf_->row, 4 * cell_dim_, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YGIFO.col; ++j) {
            memcpy(&(YGIFO.data[j * YGIFO.row]), &(propagate_buf_->data[j * propagate_buf_->row]), YG.row * sizeof(int16_t));
        }
        
        // x -> g, i, f, o, not recurrent, do it all in once
        matrix_mul_fix16(in->row, w_gifo_x_->col, in->col, FIXED16_ONE, in->data, in->row, w_gifo_x_->data, w_gifo_x_->col, FIXED16_ZERO, &(YGIFO.data[S]), YGIFO.row);
        
        // bias -> g, i, f, o
        for (int i = 0; i < bias_->col; ++i) {
            int16_t tt = bias_->data[i];
            for (int j = 0; j < T * S; ++j) {
                YGIFO.data[i * YGIFO.row + j + S] = fixed16_add(YGIFO.data[i * YGIFO.row + j + S], tt);
            }
        }
        
        for (int t = 1; t <= T; ++t) {
            // multistream buffers for current time-step,
            struct Matrix y_all;
            matrix_config(&y_all, S, propagate_buf_->col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_all.row; ++i) {
                for (int j = 0 ; j < y_all.col; ++j) {
                    y_all.data[j * y_all.row + i] = propagate_buf_->data[(j + t * S) * propagate_buf_->row + i];
                }
            }
            
            struct Matrix y_g;
            matrix_config(&y_g, S, YG.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_g.row; ++i) {
                for (int j = 0 ; j < y_g.col; ++j) {
                    y_g.data[j * y_g.row + i] = YG.data[(j + t * S) * YG.row + i];
                }
            }
            struct Matrix y_i;
            matrix_config(&y_i, S, YI.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_i.row; ++i) {
                for (int j = 0 ; j < y_i.col; ++j) {
                    y_i.data[j * y_i.row + i] = YI.data[(j + t * S) * YI.row + i];
                }
            }
            struct Matrix y_f;
            matrix_config(&y_f, S, YF.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_f.row; ++i) {
                for (int j = 0 ; j < y_f.col; ++j) {
                    y_f.data[j * y_f.row + i] = YF.data[(j + t * S) * YF.row + i];
                }
            }
            struct Matrix y_o;
            matrix_config(&y_o, S, YO.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_o.row; ++i) {
                for (int j = 0 ; j < y_o.col; ++j) {
                    y_o.data[j * y_o.row + i] = YO.data[(j + t * S) * YO.row + i];
                }
            }
            struct Matrix y_c;
            matrix_config(&y_c, S, YC.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_c.row; ++i) {
                for (int j = 0 ; j < y_c.col; ++j) {
                    y_c.data[j * y_c.row + i] = YC.data[(j + t * S) * YC.row + i];
                }
            }
            struct Matrix y_h;
            matrix_config(&y_h, S, YH.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_h.row; ++i) {
                for (int j = 0 ; j < y_h.col; ++j) {
                    y_h.data[j * y_h.row + i] = YH.data[(j + t * S) * YH.row + i];
                }
            }
            struct Matrix y_m;
            matrix_config(&y_m, S, YM.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_m.row; ++i) {
                for (int j = 0 ; j < y_m.col; ++j) {
                    y_m.data[j * y_m.row + i] = YM.data[(j + t * S) * YM.row + i];
                }
            }
            struct Matrix y_r;
            matrix_config(&y_r, S, YR.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_r.row; ++i) {
                for (int j = 0 ; j < y_r.col; ++j) {
                    y_r.data[j * y_r.row + i] = YR.data[(j + t * S) * YR.row + i];
                }
            }
            struct Matrix y_gifo;
            matrix_config(&y_gifo, S, YGIFO.col, COL_MERGE, FIX16, NULL, NULL);
            for (int i = 0; i < y_gifo.row; ++i) {
                for (int j = 0 ; j < y_gifo.col; ++j) {
                    y_gifo.data[j * y_gifo.row + i] = YGIFO.data[(j + t * S) * YGIFO.row + i];
                }
            }
            
            
            // r(t-1) -> g, i, f, o
            matrix_mul_fix16(S, w_gifo_r_->col, YR.col, FIXED16_ONE, &(YR.data[(t - 1) * S]), YR.row, w_gifo_x_->data, w_gifo_x_->col, FIXED16_ONE, y_gifo.data, y_gifo.row);
            
            // c(t-1) -> i(t) via peephole
            for (int j = 0; j < y_i.col; ++j) {
                int16_t tt = peephole_i_c_->data[j];
                for (int i = 0; i < y_i.row; ++i) {
                    int16_t temp = fixed16_xmul(YC.data[YC.row * j + (t - 1) * S], tt);
                    y_i.data[y_i.row * j + i] = fixed16_add(y_i.data[y_i.row * j + i], temp);
                }
            }
            
            // c(t-1) -> f(t) via peephole
            for (int j = 0; j < y_f.col; ++j) {
                int16_t tt = peephole_f_c_->data[j];
                for (int i = 0; i < y_f.row; ++i) {
                    int16_t temp = fixed16_xmul(YC.data[YC.row * j + (t - 1) * S], tt);
                    y_f.data[y_f.row * j + i] = fixed16_add(y_f.data[y_f.row * j + i], temp);
                }
            }
            
            // i, f sigmoid squashing
            for (int i = 0; i < y_i.row * y_i.col; ++i) y_i.data[i] = Sigmoid_fix16(y_i.data[i]);
            for (int i = 0; i < y_f.row * y_f.col; ++i) y_f.data[i] = Sigmoid_fix16(y_f.data[i]);
            
            // g tanh squashing
            for (int i = 0; i < y_g.row * y_g.col; ++i) y_g.data[i] = Tanh_fix16(y_g.data[i]);
            
            // g * i -> c
            for (int j = 0; j < y_c.col; ++j) {
                for (int i  = 0; i < y_c.row; ++i) {
                    y_c.data[y_c.row * j + i] = fixed16_xmul(y_g.data[y_g.row * j + i], y_i.data[y_i.row * j + i]);
                }
            }
            
            // c(t-1) * f -> c(t) via forget-gate
            for (int j = 0; j < y_c.col; ++j) {
                for (int i  = 0; i < y_c.row; ++i) {
                    int16_t temp = fixed16_xmul(YC.data[YC.row * j + i + (t - 1) * S], y_f.data[y_f.row * j + i]);
                    y_c.data[y_c.row * j + i] = fixed16_add(y_c.data[y_c.row * j + i], temp);
                }
            }
            
            // c(t) -> o(t) via peephole (non-recurrent, using c(t))
            for (int j = 0; j < y_o.col; ++j) {
                int16_t tt = peephole_o_c_->data[j];
                for (int i = 0; i < y_o.row; ++i) {
                    int16_t temp = fixed16_xmul(y_c.data[y_c.row * j], tt);
                    y_o.data[y_o.row * j + i] = fixed16_add(y_o.data[y_o.row * j + i], temp);
                }
            }
            
            // o sigmoid squashing
            for (int i = 0; i < y_o.row * y_o.col; ++i) y_o.data[i] = Sigmoid_fix16(y_o.data[i]);
            
            // h tanh squashing
            for (int i = 0; i < y_c.row * y_c.col; ++i) y_h.data[i] = Tanh_fix16(y_c.data[i]);
            
            // h * o -> m via output gate
            for (int j = 0; j < y_m.col; ++j) {
                for (int i  = 0; i < y_m.row; ++i) {
                    y_m.data[y_m.row * j + i] = fixed16_xmul(y_h.data[y_h.row * j + i], y_o.data[y_o.row * j + i]);
                }
            }
            
            // m -> r
            matrix_mul_fix16(y_m.row, w_r_m_->col, y_m.col, FIXED16_ONE, y_m.data, y_m.row, w_r_m_->data, w_r_m_->col, FIXED16_ZERO, y_r.data, y_r.row);
            
            free(y_all.data);
            free(y_g.data);
            free(y_i.data);
            free(y_f.data);
            free(y_o.data);
            free(y_c.data);
            free(y_h.data);
            free(y_m.data);
            free(y_r.data);
            free(y_gifo.data);
            
        }
        
        // set the 'projection layer' output as the LSTM output
        matrix_config(out, T * S, YR.col, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < YR.col; ++j) {
            memcpy(&(out->data[j * out->row]), &(YR.data[j * YR.row + S]), out->row * sizeof(int16_t));
        }
        
        // the state in the last 'frame' is transferred (can be zero vector)
        matrix_config(prev_nnet_state_, S, propagate_buf_->col, COL_MERGE, FIX16, NULL, NULL);
        for (int j = 0; j < propagate_buf_->col; ++j) {
            for (int i = 0; i < prev_nnet_state_->row; ++i) {
                prev_nnet_state_->data[j * prev_nnet_state_->row] = propagate_buf_->data[j * propagate_buf_->row + T * S];
            }
        }
        
        free(YG.data);
        free(YI.data);
        free(YF.data);
        free(YO.data);
        free(YC.data);
        free(YH.data);
        free(YM.data);
        free(YR.data);
        free(YGIFO.data);
    }
}

void lstm_clean(struct Matrix *in, struct Matrix out[], struct Matrix *ft, struct Lstm lstm[])
{
    if (in->data_type == FLOAT32) {
        free(in->data_);
        for (int i = 0; i < 3; ++i) free(out[i].data_);
        free(ft->data_);
        for (int i = 0; i < 3; ++i) {
            free(lstm[i].w_gifo_x_.data_);
            free(lstm[i].w_gifo_r_.data_);
            free(lstm[i].bias_.data_);
            free(lstm[i].peephole_i_c_.data_);
            free(lstm[i].peephole_f_c_.data_);
            free(lstm[i].peephole_o_c_.data_);
            free(lstm[i].w_r_m_.data_);
            free(lstm[i].propagate_buf_.data_);
            free(lstm[i].prev_nnet_state_.data_);
        }

    }
    else if (in->data_type == FIX16) {
        free(in->data);
        for (int i = 0; i < 3; ++i) free(out[i].data);
        free(ft->data);
        for (int i = 0; i < 3; ++i) {
            free(lstm[i].w_gifo_x_.data);
            free(lstm[i].w_gifo_r_.data);
            free(lstm[i].bias_.data);
            free(lstm[i].peephole_i_c_.data);
            free(lstm[i].peephole_f_c_.data);
            free(lstm[i].peephole_o_c_.data);
            free(lstm[i].w_r_m_.data);
            free(lstm[i].propagate_buf_.data);
            free(lstm[i].prev_nnet_state_.data);
        }
    }

}



