#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} HelloWorldEvent;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "Hello World!");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    HelloWorldEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(FuriMessageQueue* event_queue) {
    // Проверяем, что контекст не нулевой
    furi_assert(event_queue);

    HelloWorldEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t hello_world_app(void* p) {
    UNUSED(p);

    // Текущее событие типа кастомного типа HelloWorldEvent
    HelloWorldEvent event;
    // Очередь событий на 8 элементов размера HelloWorldEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(HelloWorldEvent));

    // Создаем новый view port
    ViewPort* view_port = view_port_alloc();
    // Создаем callback отрисовки, без контекста
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    // Создаем callback нажатий на клавиши, в качестве контекста передаем
    // нашу очередь сообщений, чтоб запихивать в неё эти события
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Создаем GUI приложения
    Gui* gui = furi_record_open(RECORD_GUI);
    // Подключаем view port к GUI в полноэкранном режиме
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Создаем периодический таймер с коллбэком, куда в качестве
    // контекста будет передаваться наша очередь событий
    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    // Запускаем таймер
    furi_timer_start(timer, 500);

    // Бесконечный цикл обработки очереди событий
    while(1) {
        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Наше событие — это нажатие кнопки
        if(event.type == EventTypeInput) {
            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            if(event.input.key == InputKeyBack) {
                break;
            }
            // Наше событие — это сработавший таймер
        } else if(event.type == EventTypeTick) {
            // Сделаем что-то по таймеру
        }
    }

    // Очищаем таймер
    furi_timer_free(timer);

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(event_queue);

    // Чистим созданные объекты, связанные с интерфейсом
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}
