#define ALL_MUSIC_REST 20

void playMusic(int notes[], int times[], int length) {
    for (int i = 0; i < length; i++) {
        tone(PIEZO_PIN, notes[i], times[i] - ALL_MUSIC_REST);
        delay(times[i]);
    }
}

void two_digits_to_chars(int num, char target[]) {
    int first_digit = num / 10;
    int second_digit = num % 10;
    target[0] = '0' + first_digit;
    target[1] = '0' + second_digit;
}

int rand_between(int rand_min, int rand_max) { // min inclusive, max exclusive
    return (rand() % (rand_max - rand_min)) + rand_min;
}

