#include <iostream>
using namespace std;
// hello world program
int main() {
    cout << "Hello, World!" << endl;
    return 0;
}
// quick sort function
void quickSort(int arr[], int low, int high) {
    if (low < high) {
	int pivot = partition(arr, low, high);
	quickSort(arr, low, pivot - 1);
	quickSort(arr, pivot + 1, high);
    }
}
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
	if (arr[j] < pivot) {
	    i++;
	    swap(&arr[i], &arr[j]);
	}
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// merge sort function
void mergeSort(int arr[], int l, int r) {
    if (l < r) {
	int m = l + (r - l) / 2;
	mergeSort(arr, l, m);
	mergeSort(arr, m + 1, r);
	merge(arr, l, m, r);
    }
}
void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
    for (i = 0; i < n1; i++)
	L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
	R[j] = arr[m + 1 + j];
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
	if (L[i] <= R[j]) {
	    arr[k] = L[i];
	    i++;
	} else {
	    arr[k] = R[j];
	    j++;
	}
    }
}
// comparison sort function
void comparisonSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
	for (int j = 0; j < n - i - 1; j++) {
	    if (arr[j] > arr[j + 1]) {
		swap(&arr[j], &arr[j + 1]);
	    }
	}
    }
}
