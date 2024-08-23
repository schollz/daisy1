#include "chords.h"

// Function to shuffle a single array
void Chords::shuffleArray(int arr[], int size) {
  for (int i = size - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

int Chords::diff_between_notes(int note1, int note2) {
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

int Chords::note_diff_between_notes(int note1, int note2) {
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

int Chords::Regenerate(bool best_is_min) {
  shuffleArray(note_octaves, 3);

  // progression_index = rand() % CHORD_PROGRESSION_NUM;

  for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
    for (int j = 0; j < NOTES_IN_CHORD; j++) {
      progression_matrix[i][j] = (int)
          chord_dictionary[progression_matrix_dictionary[progression_index][i] -
                           1][j];
    }
  }

  int arr_best[CHORDS_IN_PROGRESSION][NOTES_IN_CHORD];
  int score_best = -1;

  for (int iterations = 0; iterations < 10; iterations++) {
    // Shuffle each array
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      shuffleArray(progression_matrix[i], NOTES_IN_CHORD);
    }

    int score_total = 0;
    for (int i = 0; i < NOTES_IN_CHORD; ++i) {
      int score = diff_between_notes(progression_matrix[1][i],
                                     progression_matrix[0][i]);
      score += diff_between_notes(progression_matrix[2][i],
                                  progression_matrix[1][i]);
      score += diff_between_notes(progression_matrix[3][i],
                                  progression_matrix[2][i]);
      score_total += score;
    }
    if ((score_total < score_best && best_is_min) ||
        (score_total > score_best && !best_is_min) || score_best == -1) {
      for (int i = 0; i < NOTES_IN_CHORD; ++i) {
        arr_best[0][i] = progression_matrix[0][i];
        arr_best[1][i] = progression_matrix[1][i];
        arr_best[2][i] = progression_matrix[2][i];
        arr_best[3][i] = progression_matrix[3][i];

        // modify array so each subsequent note is the closest to the
        arr_best[1][i] =
            note_diff_between_notes(arr_best[0][i], arr_best[1][i]) % 12;
        arr_best[2][i] =
            note_diff_between_notes(arr_best[1][i], arr_best[2][i]) % 12;
        arr_best[3][i] =
            note_diff_between_notes(arr_best[2][i], arr_best[3][i]) % 12;
      }
      score_best = score_total;
    }
  }
  // copy arr_best to progression_matrix
  int k = 0;
  for (int j = 0; j < NOTES_IN_CHORD; j++) {
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      progression_matrix[i][j] = arr_best[i][j];
      note_sequence[k] = arr_best[i][j];
      k++;
    }
  }
  return score_best;
}
