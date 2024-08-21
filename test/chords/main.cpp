#include <iostream>

#include "../../lib/chords.h"

int main() {
  for (int i = 0; i < 13; i++) {
    rand();
  }

  Chords chords;
  int score = chords.Regenerate(true);

  std::cout << chords.progression_index << std::endl;
  for (size_t i = 0; i < CHORDS_IN_PROGRESSION; i++) {
    std::cout << (int)progression_matrix_dictionary[chords.progression_index][i] << " ";
  }
  std::cout << std::endl;

  for (size_t i = 0; i < CHORDS_IN_PROGRESSION; i++) {
    for (size_t j = 0; j < NOTES_IN_CHORD; j++) {
      std::cout << chords.progression_matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
  for (size_t i = 0; i < NOTES_IN_SEQUENCE; i++) {
    std::cout << chords.note_sequence[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "score: " << score << std::endl;
  return 0;
}