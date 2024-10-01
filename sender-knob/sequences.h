typedef struct Sequence {
  Action *actions;
  int numActions;
} Sequence;

#include "actions-cornfield-chase.h"
#include "actions-flow-state.h"
#include "actions-kids.h"
#include "actions-mars.h"
#include "actions-mountains.h"
#include "actions-stay.h"
#include "actions-unchartedworlds.h"
#include "actions-standby.h"

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
Sequence staySequence = {&stayActions[0],
                         sizeof(stayActions) / sizeof(stayActions[0])};
Sequence mountainsSequence = {&mountainsActions[0],
                              sizeof(mountainsActions) /
                                  sizeof(mountainsActions[0])};
Sequence flowStateSequence = {&flowStateActions[0],
                              sizeof(flowStateActions) /
                                  sizeof(flowStateActions[0])};
Sequence standbySequence = {&standbyActions[0],
                            sizeof(standbyActions) / sizeof(standbyActions[0])};

// clang-format off
Sequence *sequences[] = {
  &cornfieldChaseSequence,
  &flowStateSequence,
  &mountainsSequence,
  &staySequence,
  &unchartedWorldsSequence,
  &kidsSequence,
  &marsSequence,
  &standbySequence
};
// clang-format on

int numSequences = sizeof(sequences) / sizeof(sequences[0]);
