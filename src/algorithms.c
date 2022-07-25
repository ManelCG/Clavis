#include <algorithms.h>
#include <string.h>

_Bool quicksort_stringlist(char **arr, int low, int high){
  void swap(char **p1, char **p2){
    char *t = *p1;
    *p1 = *p2;
    *p2 = t;
  }

  int partition(char **arr, int low, int high){
    char *pivot = arr[high];
    int i = low-1;
    for (int j = low; j <= high-1; j++){
      if (strcmp(arr[j], pivot) < 0){
        i++;
        swap(&arr[i], &arr[j]);
      }
    }
    swap(&arr[i+1], &arr[high]);
    return i+1;
  }


  _Bool sorted = false;
  int pivot_index;

  if (low < high){
    pivot_index = partition(arr, low, high);

    quicksort_stringlist(arr, low, pivot_index-1);
    quicksort_stringlist(arr, pivot_index+1, high);
    sorted = true;
  }

  return sorted;
}
