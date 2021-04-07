
#include "button.h"
#include "common.h"

namespace Button {
    static uint8_t _pin;

    static volatile bool on = false; // TODO use something more effective than bool
    static volatile bool touched = false; // кнопка нажата либо ожидается очередь нажатий

    static volatile byte clicks = 0; // счётчик кликов
    static volatile byte holdingTicks = 0; // счётчик тиков (как долго зажата кнопка)

    static volatile struct Event event;

    static volatile unsigned long changedAt = 0;

    void resetCounters() {
        clicks = 0;
        holdingTicks = 0;
    }

    void onInterrupt() {
        unsigned long now = millis();
        if (now < BUTTON_CLICK_MIN_INTERVAL) {
            // ignore any events within first milliseconds during Arduino bootstrap to allow RESET mode detection (holding the button while Arduino is starting)
            return;
        }

        on = digitalRead(_pin) == BUTTON_ON;

        if (!touched) {
            if (!on) {
                // Кнопка отпущена, но мы не знаем ничего о её нажатии - скорее всего она была зажата до запуска контроллера.
                if (now >= BUTTON_BOOT_HELD_DURATION) {
                    event.type = BUTTON_EVENT_BOOT_HELD;
                    event.clicks = 0;
                    event.holdingTicks = 0;
                }
            } else {
                // Кнопка нажата - запоминаем
                touched = true;
            }

            resetCounters();
            changedAt = now;
            return;
        }

        if (on) {
            // кнопка нажата после того, как мы запомнили её предыдущее нажатие - это скорее всего последовательность нажатий
            changedAt = now;
            return;
        }

        if (!on) {
            // кнопку отпустили

            // Проверяем, когда до этого нажимали кнопку. Если в первые моменты после запуска контроллера, то возможно EVENT_BOOT_HELD
            if (changedAt <= BUTTON_BOOT_HELD_MAX_START_TIME) {
                if (now >= BUTTON_BOOT_HELD_DURATION) {
                    event.type = BUTTON_EVENT_BOOT_HELD;
                    event.clicks = 0;
                    event.holdingTicks = 0;
                }
                touched = false;
                resetCounters();
                return;
            }

            if (holdingTicks) {
                // кнопка была зажата длительное время и её отпустили
                touched = false;
                resetCounters();
                return;
            }

            // кнопка отпущена
            if (timeDiff(now, changedAt) >= BUTTON_CLICK_MIN_INTERVAL) {
                // если кнопка удерживалась достаточное время (это не дребезг контактов)
                clicks++;
            }
        }

        changedAt = now;
    }

    void setup(uint8_t pin) {
        _pin = pin;
        attachInterrupt(digitalPinToInterrupt(pin), onInterrupt, CHANGE);
    }

    void loop() {
        if (!touched) {
            return;
        }
        if (changedAt <= BUTTON_BOOT_HELD_MAX_START_TIME) {
            return;
        }

        unsigned long timeSinceLastChange = timeDiff(millis(), changedAt);
        if (on && timeSinceLastChange > BUTTON_START_HOLDING_TIME) {
            // holding the button
            byte ticks = (timeSinceLastChange - BUTTON_START_HOLDING_TIME) / BUTTON_HOLDING_TICKS_PERIOD + 1;
            if (ticks != holdingTicks) {
                holdingTicks = ticks;
                event.type = BUTTON_EVENT_HOLDING;
                event.clicks = clicks;
                event.holdingTicks = holdingTicks;
            }
        } else if (!on && timeSinceLastChange >= BUTTON_MAX_SEQ_CLICKS_INTERVAL) {
            if (clicks && !holdingTicks) {
                event.type = BUTTON_EVENT_CLICKS;
                event.clicks = clicks;
                event.holdingTicks = 0;
            }
            touched = false;
            resetCounters();
        }
    }

    bool hasEvent() {
        return event.type != BUTTON_EVENT_NONE;
    }

    Event pullEvent() {
        struct Event res;
        memcpy(&res, (const void *) &event, sizeof(event));
//        res.type = event.type;
//        res.clicks = event.clicks;
//        res.holdingTicks = event.holdingTicks;

        event.type = BUTTON_EVENT_NONE;
        event.clicks = 0;
        event.holdingTicks = 0;
        return res;
    }

} // namespace Button
