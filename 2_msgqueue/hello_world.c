#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

int32_t hello_world_app(void* p) {
    UNUSED(p);

    // Текущее событие типа InputEvent
    InputEvent event;
    // Очередь событий на 8 элементов размера InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Бесконечный цикл обработки очереди событий
    while(1) {
        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
        if(event.key == InputKeyBack) {
            break;
        }
    }

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(event_queue);

    return 0;
}
