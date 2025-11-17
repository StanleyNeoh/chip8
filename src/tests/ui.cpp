#include "../ui.hpp"

int main() {
    UI& ui = UI::instance();
    if (ui.init("Test Screen", 500, 500) == -1) {
        return -1;
    }

    ui.clear();
    for (int r = 0; r < 500; r++) {
        for (int c = 0; c < 500; c++) {
            bool incir = ((r - 250) * (r - 250) + (c - 250) * (c - 250)) < 250;
            if (incir) ui.draw(r, c);
            if (r == 10) ui.draw(r, c);
            if (c == 20) ui.draw(r, c);
        }
    }

    ui.display();
    while (ui.loop());
}