// Собственная реализация кнопки на прерываниях.
//
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// уровень сигнала, когда кнопка включена
#define BUTTON_ON HIGH

// минимальный интервал времени (мс), за который возможен клик. Клики за меньший промежуток расцениваются как дребезг и игнорируются
#define BUTTON_CLICK_MIN_INTERVAL 20

// максимальный интервал между кликами (мс), когда они ещё рассматриваются как одна последовательность
// это также и лаг между нажатием последовательности и реакцией лампы
#define BUTTON_MAX_SEQ_CLICKS_INTERVAL 500

// в течение этого времени после запуска контроллера можно зажать кнопку, чтобы сгенерировать EVENT_BOOT_HELD
#define BUTTON_BOOT_HELD_MAX_START_TIME 250

// если кнопка зажата при загрузке в течении этого времени, генерируется EVENT_BOOT_HELD
#define BUTTON_BOOT_HELD_DURATION 3000

// если кнопка зажата больше этого времени (мс), считается, что она удерживается
#define BUTTON_START_HOLDING_TIME 1000

// период (мс), с которым генерируется событие (тик) при зажатии кнопки
#define BUTTON_HOLDING_TICKS_PERIOD 500

namespace Button {

    enum EventType {
        EVENT_NONE = 0,
        EVENT_CLICKS, // кнопка была нажата N раз подряд
        EVENT_HOLDING, // кнопка удерживается
        EVENT_BOOT_HELD // кнопка удерживалась во время запуска микроконтроллера
    };

    struct Event {
        enum EventType type = EVENT_NONE;
        byte value = 0;
    };

    void setup(uint8_t pin);

    void loop();

    bool hasEvent();
    Event pullEvent(); // Получить последнее событие кнопки. Событие сбрасывается после вызова

} // namespace Button

#endif //GYVERLAMP_FOR_ARDUINO_BUTTON_H
