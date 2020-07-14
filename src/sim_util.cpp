//
// Created by joschka on 24/02/2020.
//

#include <stdio.h>
#include <vector>
#include <iostream>
#include <time.h>
#include <string.h>



//C include
extern "C" {
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"

#include "load_alist.h"
#include "syndrome.h"
#include "binary_char.h"
#include "bp_decoder_ms.h"
#include "mod2sparse_extra.h"
#include "mtwister.h"
}

#include "sim_util.h"

using namespace std;

int gen_error(char *bit_error, int error_len, double bit_error_rate, MTRand *r){
    int sum=0;
    for (int bit_no = 0; bit_no < error_len; bit_no++) {
        bit_error[bit_no] = genRand(r) < bit_error_rate;
        sum+=bit_error[bit_no];
    }

    return sum;

}

int check_logical_error_hz(mod2sparse* h,char *orig_error,char *decoding){

    int logical_error = 0;

    int n = mod2sparse_cols(h);
    int c = mod2sparse_rows(h);

    char *residual=new char[n]();

    bin_char_add(orig_error,decoding,residual,n);

    mod2sparse* ht = mod2sparse_allocate(n,c);
    mod2sparse_transpose(h,ht);

    mod2sparse* htf = mod2sparse_allocate(n,n); 

    mod2sparse_merge_vec(ht, residual, n, htf);

    int rank_ht = mod2sparse_rank(h); //we have rank(A^T) = rank(A)

    int rank_htf = mod2sparse_rank(htf);

    if (rank_ht < rank_htf) {
        logical_error = 1;
    }

    delete[] residual;

    mod2sparse_free(ht);
    mod2sparse_free(htf);

    return logical_error;

}

int check_logical_error_lx(mod2sparse* l,char *orig_error,char *decoding){

    int n = mod2sparse_cols(l);
    int k = mod2sparse_rows(l);
    char *residual=new char[n]();
    char *log_check=new char[k]();

    int logical_error=0;
    bin_char_add(orig_error,decoding,residual,n);
    mod2sparse_mulvec(l,residual,log_check);
    if(!bin_char_is_zero(log_check,k)) logical_error=1;

    delete[] log_check;
    delete[] residual;

    return logical_error;

}

mod2sparse *load_alist_cpp(string filename){
    char *pcm_filename= const_cast<char*> (filename.c_str());
    cout<<"Loading '"<<pcm_filename<<"'"<<endl;
    mod2sparse *pcm=load_alist(pcm_filename);
    return pcm;
}


