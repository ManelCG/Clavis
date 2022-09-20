/*
 *  Clavis
 *  Copyright (C) 2022  Manel Castillo Giménez <manelcg@protonmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

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
