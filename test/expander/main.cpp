#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

std::vector<int> expand_notes(std::vector<int> note_sequence,
                              int note_expansion) {
  // Create notes_all by repeating each note in note_sequence note_expansion
  // times
  std::vector<int> notes_all;
  for (int i = 0; i < 12; ++i) {
    for (int j = 0; j < note_expansion; ++j) {
      notes_all.push_back(note_sequence[i]);
    }
  }

  // Create note_ordered by eliminating duplicates
  std::vector<int> note_ordered;
  for (int i = 0; i < 12; ++i) {
    if (std::find(note_ordered.begin(), note_ordered.end(), note_sequence[i]) ==
        note_ordered.end()) {
      note_ordered.push_back(note_sequence[i]);
    }
  }

  // Append +12 and -12 to each element in note_ordered
  std::vector<int> extended_notes;
  for (int note : note_ordered) {
    extended_notes.push_back(note);
    extended_notes.push_back(note + 12);
    extended_notes.push_back(note - 12);
  }

  // Sort note_ordered
  std::sort(extended_notes.begin(), extended_notes.end());
  note_ordered = extended_notes;

  // Create brownian_movement array
  std::vector<int> brownian_movement(notes_all.size(), 0);

  // Random number generation setup
  std::srand(std::time(0));

  // Populate brownian_movement according to rules
  for (size_t i = 1; i < brownian_movement.size(); ++i) {
    int prev = brownian_movement[i - 1];
    std::vector<int> options;

    if (prev == -3)
      options = {0, 1, 2, 3, 3, 3};
    else if (prev == -2)
      options = {-1, 0, 1, 2, 2, 2};
    else if (prev == -1)
      options = {-1, 0, 1, 1, 1};
    else if (prev == 0)
      options = {-1, 0, 0, 0, 1};
    else if (prev == 1)
      options = {-1, -1, -1, 0, 1};
    else if (prev == 2)
      options = {-2, -2, -2, -1, 0, 1};
    else if (prev == 3)
      options = {-3, -3, -3, -2, -1, 0};

    brownian_movement[i] = prev + options[std::rand() % options.size()];

    // If the value goes beyond the range of -3 to 3, reset it to 0
    if (std::abs(brownian_movement[i]) > 3) {
      brownian_movement[i] = 0;
    }
  }

  // Print brownian_movement
  std::cout << "Brownian Movement: ";
  for (const int& bm : brownian_movement) {
    std::cout << bm << " ";
  }
  std::cout << std::endl;

  // Update notes_all using brownian_movement
  for (size_t i = 0; i < notes_all.size(); ++i) {
    notes_all[i] += brownian_movement[i];
  }

  // Snap each element in notes_all to the closest element in note_ordered
  for (size_t i = 0; i < notes_all.size(); ++i) {
    auto closest = std::min_element(
        note_ordered.begin(), note_ordered.end(), [&](int a, int b) {
          return std::abs(a - notes_all[i]) < std::abs(b - notes_all[i]);
        });
    notes_all[i] = *closest;
  }

  // Print notes_all
  std::cout << "Notes All: ";
  for (const int& note : notes_all) {
    std::cout << note << " ";
  }
  std::cout << std::endl;

  return notes_all;
}

int main() {
  int note_expansion = 2;  // Assume this is defined elsewhere

  std::vector<int> note_sequence = {-2, 0, 2, 4, 7, 7, 7, 7, 5, 4, 1, 2};
  std::vector<int> notes_all = expand_notes(note_sequence, note_expansion);
  return 0;
}
