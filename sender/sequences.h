typedef struct Sequence {
  Action *actions;
  int numActions;
} Sequence;

#include "actions-cornfield-chase.h"
#include "actions-kids.h"
#include "actions-mars.h"
#include "actions-unchartedworlds.h"

Sequence cornfieldChaseSequence = {&cornfieldChaseActions[0],
                                   sizeof(cornfieldChaseActions) /
                                       sizeof(cornfieldChaseActions[0])};
Sequence marsSequence = {&marsActions[0],
                         sizeof(marsActions) / sizeof(marsActions[0])};
Sequence kidsSequence = {&kidsActions[0],
                         sizeof(kidsActions) / sizeof(kidsActions[0])};
Sequence unchartedWorldsSequence = {&unchartedWorldsActions[0],
                                    sizeof(unchartedWorldsActions) /
                                        sizeof(unchartedWorldsActions[0])};

// clang-format off
Sequence *sequences[] = {
  &marsSequence,
  &unchartedWorldsSequence,
  &kidsSequence,
  &cornfieldChaseSequence,
};
// clang-format on

int numSequences = sizeof(sequences) / sizeof(sequences[0]);
