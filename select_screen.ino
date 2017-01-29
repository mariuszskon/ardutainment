#define SELECT_UP BUTTON0_PIN
#define SELECT_SELECT BUTTON1_PIN
#define SELECT_DOWN BUTTON2_PIN

int select_screen(const char *options[], int selection_options) {
    lcd.clear();
    bool selected = 0;
    
    int first_in_pair = 0;
    int hovering_over = 0;
    bool changed = 1;

    while (!selected) {
        if (digitalRead(SELECT_UP)) {
            hovering_over -= 1;
            if (hovering_over < 0) hovering_over = selection_options - 1;
            changed = 1;
        } else if (digitalRead(SELECT_DOWN)) {
            hovering_over = (hovering_over + 1) % selection_options;
            changed = 1;
        } else if (digitalRead(SELECT_SELECT)) {
            selected = 1;
        }

        if (changed) { // don't update screen unnecessarily
            first_in_pair = hovering_over & ~1; // round down to even
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print(options[first_in_pair]); // first in the pair
            lcd.setCursor(1, 1);
            lcd.print(options[first_in_pair + 1]); // second in the pair
            lcd.setCursor(0, hovering_over % 2); // 0 or 1: which item in the pair is hovered over
            lcd.write(">");
            delay(500);
        }

        changed = 0;
    }
    return hovering_over;
}

