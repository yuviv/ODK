#include "NumberClassifier.h"

int NumberClassifier::predict_number(const char guess) {
  switch (guess) {
    case ZERO_SEGMENTS:
      return 0;
    case ONE_SEGMENTS_LEFT:
      return 1;
    case ONE_SEGMENTS_RIGHT:
      return 1;
    case TWO_SEGMENTS:
      return 2;
    case THREE_SEGMENTS:
      return 3;
    case FOUR_SEGMENTS:
      return 4;
    case FIVE_SEGMENTS:
      return 5;
    case SIX_SEGMENTS:
      return 6;
    case SIX_SEGMENTS_PARTIAL:
      return 6;
    case SEVEN_SEGMENTS:
      return 7;
    case EIGHT_SEGMENTS:
      return 8;
    case NINE_SEGMENTS:
      return 9;
    case NINE_SEGMENTS_PARTIAL:
      return 9;
    default:
      return -1;
  } 
}
