#include <iostream>

const int SIZE = 3;

// Function to shuffle a single array
void shuffleArray(int arr[], int size) {
  for (int i = size - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

// Function to print an array
void printArray(int arr[], int size) {
  for (int i = 0; i < size; ++i) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;
}

int diff_between_notes(int note1, int note2) {
  int smallest_diff = (note2 > note1) ? (note2 - note1) : (note1 - note2);
  int octaves[3] = {-12, 0, 12};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int diff = ((note2 + octaves[i]) > (note1 + octaves[j]))
                     ? (note2 + octaves[i]) - (note1 + octaves[j])
                     : (note1 + octaves[j]) - (note2 + octaves[i]);
      if (diff < smallest_diff) {
        smallest_diff = diff;
      }
    }
  }
  return smallest_diff;
}

int note_diff_between_notes(int note1, int note2) {
  int smallest_diff = (note2 > note1) ? (note2 - note1) : (note1 - note2);
  int note3 = note2;
  int octaves[3] = {-12, 0, 12};
  for (int i = 0; i < 3; i++) {
    int diff = ((note2 + octaves[i]) > (note1))
                   ? (note2 + octaves[i]) - (note1)
                   : (note1) - (note2 + octaves[i]);
    if (diff < smallest_diff) {
      smallest_diff = diff;
      note3 = note2 + octaves[i];
    }
  }
  return note3;
}

int main() {
  srand(time(0)); // Seed the random number generator

  // Define four arrays
  int arr1[SIZE] = {0, 2, 4};   // C major
  int arr2[SIZE] = {-2, 0, 2};  // A minor
  int arr3[SIZE] = {-1, 2, 4};  // E minor
  int arr4[SIZE] = {-5, -1, 2}; // G major

  int arr_best[4][SIZE];
  int score_best = -1;

  for (int iterations = 0; iterations < 10; iterations++) {

    // Shuffle each array
    shuffleArray(arr1, SIZE);
    shuffleArray(arr2, SIZE);
    shuffleArray(arr3, SIZE);
    shuffleArray(arr4, SIZE);

    int score_total = 0;
    for (int i = 0; i < SIZE; ++i) {
      int score = diff_between_notes(arr2[i], arr1[i]);
      score += diff_between_notes(arr3[i], arr2[i]);
      score += diff_between_notes(arr4[i], arr3[i]);
      score_total += score;
    }
    if (score_total > score_best || score_best == -1) {
      for (int i = 0; i < SIZE; ++i) {
        arr_best[0][i] = arr1[i];
        arr_best[1][i] = arr2[i];
        arr_best[2][i] = arr3[i];
        arr_best[3][i] = arr4[i];

        // modify array so each subsequent note is the closest to the
        arr_best[1][i] =
            note_diff_between_notes(arr_best[0][i], arr_best[1][i]);
        arr_best[2][i] =
            note_diff_between_notes(arr_best[1][i], arr_best[2][i]);
        arr_best[3][i] =
            note_diff_between_notes(arr_best[2][i], arr_best[3][i]);
      }
      score_best = score_total;
    }
  }

  std::cout << "Total Score: " << score_best << std::endl;

  for (int i = 0; i < 3; i++) {
    // print out array
    std::cout << arr_best[0][i] << " " << arr_best[1][i] << " "
              << arr_best[2][i] << " " << arr_best[3][i] << std::endl;
  }

  return 0;
}
