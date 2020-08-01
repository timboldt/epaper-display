
#include "global.h"

void DrawDate(DateTime &now) {
    char label[] = "DDD, MMM DD";
    now.toString(label);
    display.setFont(&FreeSerif12pt7b);
    int16_t fx, fy;
    uint16_t fw, fh;
    display.getTextBounds(label, 50, 50, &fx, &fy, &fw, &fh);
    display.setCursor(200 - fw / 2, fh);
    display.setTextColor(GxEPD_BLACK);
    display.print(label);
}

void DrawClock(int16_t x, int16_t y, const DateTime &t) {
    const int16_t clock_radius = 105;
    const int16_t clock_inner_radius = 97;
    const int16_t tick_radius = 90;
    const int16_t hour_radius = 55;
    const int16_t minute_radius = 95;
    const int16_t hour_width = 6;
    const int16_t minute_width = 4;
    const int16_t center_point_width = 2;

    // Draw the clock face.
    display.fillCircle(x, y, clock_radius, GxEPD_BLACK);
    display.fillCircle(x, y, clock_inner_radius, GxEPD_WHITE);
    for (int i = 0; i < 12; i++) {
        float dx = sin(float(i) / 12 * 2 * PI);
        float dy = cos(float(i) / 12 * 2 * PI);
        display.drawLine(x + tick_radius * dx, y - tick_radius * dy,
                         x + clock_inner_radius * dx,
                         y - clock_inner_radius * dy, GxEPD_BLACK);
    }

    // Draw the hour hand.
    float hour_angle =
        float(t.twelveHour() * 60 + t.minute()) / 60.0 / 12.0 * 2.0 * PI;
    float hx = sin(hour_angle);
    float hy = cos(hour_angle);
    float hb_x = x - hour_width * hx;
    float hb_y = y + hour_width * hy;
    display.drawTriangle(hb_x - hour_width * hy, hb_y - hour_width * hx,
                         hb_x + hour_width * hy, hb_y + hour_width * hx,
                         x + hour_radius * hx, y - hour_radius * hy,
                         GxEPD_BLACK);

    // Draw the minute hand.
    float minute_angle = float(t.minute()) / 60.0 * 2.0 * PI;
    float mx = sin(minute_angle);
    float my = cos(minute_angle);
    float mb_x = x - minute_width * mx;
    float mb_y = y + minute_width * my;
    display.fillTriangle(mb_x - minute_width * my, mb_y - minute_width * mx,
                         mb_x + minute_width * my, mb_y + minute_width * mx,
                         x + minute_radius * mx, y - minute_radius * my,
                         GxEPD_BLACK);

    // Draw the pin in the center.
    display.fillCircle(x, y, center_point_width, GxEPD_WHITE);
    display.drawCircle(x, y, center_point_width, GxEPD_BLACK);
}

void DrawGauge(int16_t x, int16_t y, const String &label, float value,
               float min_value, float max_value) {
    const int16_t gauge_radius = 20;
    const int16_t tick_radius = 17;
    const int16_t needle_radius = 18;
    const int16_t needle_width = 3;
    const int16_t pin_width = 2;

    if (value < min_value) {
        value = min_value;
    }
    if (value > max_value) {
        value = max_value;
    }
    float gauge_value = (value - min_value) / (max_value - min_value) - 0.5;

    // Draw the guage face.
    display.drawCircle(x, y, gauge_radius, GxEPD_BLACK);
    for (int i = -5; i <= 5; i++) {
        float dx = sin(i * 2 * PI / 16);
        float dy = cos(i * 2 * PI / 16);
        display.drawLine(x + tick_radius * dx, y - tick_radius * dy,
                         x + gauge_radius * dx, y - gauge_radius * dy,
                         GxEPD_BLACK);
    }
    display.fillRect(x - gauge_radius, y + 10, x + gauge_radius, y + 24,
                     GxEPD_WHITE);
    display.setFont(&Picopixel);
    int16_t fx, fy;
    uint16_t fw, fh;
    display.getTextBounds(label, x, y, &fx, &fy, &fw, &fh);
    display.setCursor(x - fw / 2, y + gauge_radius - fh / 2);
    display.setTextColor(GxEPD_BLACK);
    display.print(label);

    // Draw the needle.
    float angle = gauge_value * 10 / 16 * 2 * PI;
    float nx = sin(angle);
    float ny = cos(angle);
    display.drawLine(x - 0.1 * needle_radius * nx, y + 0.1 * needle_radius * ny,
                     x + needle_radius * nx, y - needle_radius * ny,
                     GxEPD_BLACK);

    // Draw the pin in the center.
    display.fillCircle(x, y, pin_width, GxEPD_BLACK);
    display.drawPixel(x, y, GxEPD_WHITE);
}
