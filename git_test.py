import os

#print "Hello World!"
#for i in range(0, 1000):
#    print i

#******************************************
arr_test = [1, 5, 7, 9, 3, 2, 0, 4]

def sap_xep_tang_dan(arr):
    for i in range(len(arr)-1):
        for j in range(i+1, len(arr)):
            temp = None
            if arr[i] > arr[j]:
               temp = arr[j];
               arr[j] = arr[i]
               arr[i] = temp;
    print arr_test

# call function
# update for EMB_BR_02
sap_xep_tang_dan(arr_test)