typedef struct Sequence {
  Action *actions;
  int numActions;
} Sequence;

#include "actions-cornfield-chase.h"

Sequence cornfieldChaseSequence = {&cornfieldChaseActions[0],
                                   sizeof(cornfieldChaseActions) /
                                       sizeof(cornfieldChaseActions[0])};

// clang-format off
Sequence *sequences[] = {
  &cornfieldChaseSequence,
  &cornfieldChaseSequence,
};
// clang-format on

int numSequences = sizeof(sequences) / sizeof(sequences[0]);
