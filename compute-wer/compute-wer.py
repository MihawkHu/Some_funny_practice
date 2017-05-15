# compute wer of two kaldi ark text file
# record the number of specific ins, del, sub errors
# record the specific ins, del, sub error words
import os
import re
import sys

text_file_path = sys.argv[1]
trans_file_path = sys.argv[2]

text_file_path = open(text_file_path)
trans_file = open(trans_file_path)

text = text_file_path.readlines()
trans = trans_file.readlines()
if len(text) != len(trans):
    print "Error1: Length not consistent"

# split strings, the first ele is the index name
for i in range(len(text)):
    text[i] = text[i].split()
    trans[i] = trans[i].split()
    if trans[i][0] != text[i][0]:
        print "Error2: Index not match"
    text[i] = text[i][1:]
    trans[i] = trans[i][1:]


# stat variable
num_words = 0
word_errs = 0
num_sent = 0
sent_errs = 0
num_ins = 0
num_del = 0
num_sub = 0

# ins, del, sub words recoder
ins_dict = dict()
del_dict = dict()
sub_dict = dict()


# merge two dict, add the counter number
def merge_dict(dict1, dict2):
    if dict2 == {}:
        return dict1
    for key in dict2.keys():
        if dict1.has_key(key):
            dict1[key] += dict2[key]
        else:
            dict1[key] = dict2[key]
    return dict1


# compute number of words and sentences
num_sent = len(text)
for i in range(num_sent):
    num_words += len(text[i])

# struct to record ins, del, sub errors
class wer_item:
    value = 0
    ins_num = 0
    del_num = 0
    sub_num = 0
    ins_count = dict()
    del_count = dict()
    sub_count = dict()


# compute Levenshtein distance
for p in range(num_sent):
    # initialisation
    temp = wer_item()
    d = [[temp for j in range(len(trans[p])+1)] for i in range(len(text[p])+1)]

    for i in range(len(text[p])+1):
        for j in range(len(trans[p])+1):
            d[i][j] = wer_item()
            d[i][j].ins_count = dict()
            d[i][j].del_count = dict()
            d[i][j].sub_count = dict()
            if i == 0:
                d[0][j].value = j
                d[0][j].ins_num = j
                if j != 0:
                    d[0][j].ins_count = d[0][j-1].ins_count.copy()
                    s = trans[p][j-1]
                    if d[0][j].ins_count.has_key(s):
                        d[0][j].ins_count[s] += 1
                    else:
                        d[0][j].ins_count[s] = 1
            elif j == 0:
                d[i][0].value = i
                d[i][0].del_num = i
                if i != 0:
                    d[i][0].del_count = d[i-1][0].del_count.copy()
                    s = text[p][i-1]
                    if d[i][0].del_count.has_key(s):
                        d[i][0].del_count[s] += 1
                    else:
                        d[i][0].del_count[s] = 1


    # computation
    for i in range(1, len(text[p])+1):
        for j in range(1, len(trans[p])+1):
            if text[p][i-1] == trans[p][j-1]:
                d[i][j].value = d[i-1][j-1].value
                d[i][j].del_num = d[i-1][j-1].del_num
                d[i][j].ins_num = d[i-1][j-1].ins_num
                d[i][j].sub_num = d[i-1][j-1].sub_num
                d[i][j].ins_count = d[i-1][j-1].ins_count.copy()
                d[i][j].del_count = d[i-1][j-1].del_count.copy()
                d[i][j].sub_count = d[i-1][j-1].sub_count.copy()

            else:
                suberr = d[i-1][j-1].value + 1
                inserr = d[i][j-1].value + 1
                delerr = d[i-1][j].value + 1
                
                # d[i][j].value is min(suberr, inserr, delerr)
                if suberr < inserr and suberr < delerr:
                    d[i][j].value = suberr
                    d[i][j].del_num = d[i-1][j-1].del_num
                    d[i][j].ins_num = d[i-1][j-1].ins_num
                    d[i][j].sub_num = d[i-1][j-1].sub_num + 1
                    d[i][j].ins_count = d[i-1][j-1].ins_count.copy()
                    d[i][j].del_count = d[i-1][j-1].del_count.copy()
                    d[i][j].sub_count = d[i-1][j-1].sub_count.copy()

                    temp = (text[p][i-1], trans[p][j-1])
                    if d[i][j].sub_count.has_key(temp):
                        d[i][j].sub_count[temp] += 1
                    else:
                        d[i][j].sub_count[temp] = 1

                elif delerr < inserr:
                    d[i][j].value = delerr
                    d[i][j].del_num = d[i-1][j].del_num + 1
                    d[i][j].ins_num = d[i-1][j].ins_num
                    d[i][j].sub_num = d[i-1][j].sub_num
                    d[i][j].ins_count = d[i-1][j].ins_count.copy()
                    d[i][j].del_count = d[i-1][j].del_count.copy()
                    d[i][j].sub_count = d[i-1][j].sub_count.copy()

                    s = text[p][i-1]
                    if d[i][j].del_count.has_key(s):
                        d[i][j].del_count[s] += 1
                    else:
                        d[i][j].del_count[s] = 1

                else:
                    d[i][j].value = inserr
                    d[i][j].ins_num = d[i][j-1].ins_num + 1
                    d[i][j].del_num = d[i][j-1].del_num
                    d[i][j].sub_num = d[i][j-1].sub_num
                    d[i][j].ins_count = d[i][j-1].ins_count.copy()
                    d[i][j].del_count = d[i][j-1].del_count.copy()
                    d[i][j].sub_count = d[i][j-1].sub_count.copy()

                    s = trans[p][j-1]
                    if d[i][j].ins_count.has_key(s):
                        d[i][j].ins_count[s] += 1
                    else:
                        d[i][j].ins_count[s] = 1

    temp_wer = d[len(text[p])][len(trans[p])].value
    word_errs = word_errs + temp_wer
    num_ins += d[len(text[p])][len(trans[p])].ins_num
    num_del += d[len(text[p])][len(trans[p])].del_num
    num_sub += d[len(text[p])][len(trans[p])].sub_num
    ins_dict = merge_dict(ins_dict, d[len(text[p])][len(trans[p])].ins_count)
    del_dict = merge_dict(del_dict, d[len(text[p])][len(trans[p])].del_count)
    sub_dict = merge_dict(sub_dict, d[len(text[p])][len(trans[p])].sub_count)

    if text[p] != trans[p]:
        sent_errs = sent_errs + 1


# get the result
wer = float(word_errs) / float(num_words) * 100
ser = float(sent_errs) / float(num_sent) * 100

print "ins:", ins_dict
print "del:", del_dict
print "sub:", sub_dict
print "ins number:", num_ins, ", del number:", num_del, ", sub number:", num_sub
print "error word number:", word_errs, ", error sentence number:", sent_errs 
print "wer:", wer, ", ser", ser
