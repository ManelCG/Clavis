#include <algorithms.h>
#include <string.h>

_Bool quicksort_stringlist(char **arr, int low, int high){
  int partition(char **arr, int low, int high){
    char *pivot = arr[high];
    int i = low-1;
    for (int j = low; j <= high-1; j++){
      if (strcmp(arr[j], pivot) < 0){
        i++;
        {char *t = arr[i];
         arr[i] = arr[j];
         arr[j] = t;}
      }
    }
    {char *t = arr[i+1];
     arr[i+1] = arr[high];
     arr[high] = t;}
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
