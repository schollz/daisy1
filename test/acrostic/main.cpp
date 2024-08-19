const int NOTES_IN_CHORD = 3;

const int8_t chord_dictionary[14][NOTES_IN_CHORD] = {
    {0, 4, 7},                   // I (C major)
    {2, 5, 9 - 12},              // ii (D minor)
    {4, 7, 11 - 12},             // iii (E minor)
    {5, 9, 12 - 12},             // IV (F major)
    {7 - 12, 11 - 12, 14 - 12},  // V (G major)
    {9 - 12, 12 - 12, 16 - 12},  // vi (A minor)
    {11 - 12, 14 - 12, 17 - 12}, // vii° (B diminished)
    {0, 3, 7},                   // i (C minor)
    {2, 6, 9 - 12},              // II (D major)
    {4, 8, 11 - 12},             // III (E major)
    {5, 8 - 12, 12 - 12},        // iv (F minor)
    {7 - 12, 10 - 12, 14 - 12},  // v (G minor)
    {9 - 12, 13 - 12, 16 - 12},  // VI (A major)
    {11 - 12, 15 - 12, 17 - 12}, // VII (B major)
};

const int CHORD_PROGRESSION_NUM = 4;
const int CHORDS_IN_PROGRESSION = 4;
const uint8_t chord_progress_dictionary[CHORD_PROGRESSION_NUM]
                                       [CHORDS_IN_PROGRESSION] = {
                                           {1, 5, 6, 4}, // I V iv IV
                                           {1, 6, 4, 5}, // I vi IV V
                                           {4, 1, 5, 6}, // IV I V vi
                                           {1, 3, 6, 4}, // I iii vi IV
};

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

int chord_progression_index = 1; // rand() % CHORD_PROGRESSION_NUM;
int chord_progression[CHORDS_IN_PROGRESSION][NOTES_IN_CHORD];
int chord_note_sequence[CHORDS_IN_PROGRESSION * NOTES_IN_CHORD];

void regenerateChordProgression() {
  for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
    for (int j = 0; j < NOTES_IN_CHORD; j++) {
      chord_progression[i][j] = (int)chord_dictionary
          [chord_progress_dictionary[chord_progression_index][i] - 1][j];
    }
  }

  int arr_best[CHORDS_IN_PROGRESSION][NOTES_IN_CHORD];
  int score_best = -1;

  for (int iterations = 0; iterations < 100; iterations++) {

    // Shuffle each array
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      shuffleArray(chord_progression[i], NOTES_IN_CHORD);
    }

    int score_total = 0;
    for (int i = 0; i < NOTES_IN_CHORD; ++i) {
      int score =
          diff_between_notes(chord_progression[1][i], chord_progression[0][i]);
      score +=
          diff_between_notes(chord_progression[2][i], chord_progression[1][i]);
      score +=
          diff_between_notes(chord_progression[3][i], chord_progression[2][i]);
      score_total += score;
    }
    if (score_total < score_best || score_best == -1) {
      for (int i = 0; i < NOTES_IN_CHORD; ++i) {
        arr_best[0][i] = chord_progression[0][i];
        arr_best[1][i] = chord_progression[1][i];
        arr_best[2][i] = chord_progression[2][i];
        arr_best[3][i] = chord_progression[3][i];

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
  // copy arr_best to chord_progression
  int k = 0;
  for (int j = 0; j < NOTES_IN_CHORD; j++) {
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      chord_progression[i][j] = arr_best[i][j];
      chord_note_sequence[k] = arr_best[i][j];
      k++;
    }
  }
}
