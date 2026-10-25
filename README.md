# InOutData

Ветка: `3-create_InOutSignals4_5_and_converters`. Состояние: `0838b4e`.

## Изменения относительно предыдущего этапа

К контейнерам данных на 320, 128, 112 и 8 бит добавлены составные группы скалярных сигналов. Они собирают несколько одноразрядных управляющих и статусных полей в упакованное значение.

## Состав структур

| Тип | Поля в порядке объявления | Полезная разрядность |
| --- | --- | --- |
| `InOutSignals4` | THOHIVLD, OTL4_10_DATA_VALID, RATE_CONTROL, nHARD_RESET | 4 бита |
| `InOutSignals5` | DATA_VALID_OUT, TXOHIFP_OUT, TXOHIMFP_OUT, FEC_SOF_OUT, THOHEVALID | 5 бит |
| `InSignalOtu4Converter` | DATA_VALID_IN, nHARD_RESET | 2 бита |
| `OutSignalOtu4Converter` | DATA_VALID_OUT, FRAME_BEGIN | 2 бита |

Первые два типа определены в `include/InOutFlagStruct.hpp`. Они содержат битовые поля, `packed`, `bytes[1]` и `words[1]`. Наличие 32-битного слова означает, что размер объекта не следует приравнивать к одному байту.

Два типа OTU4 определены в `include/InOutDataOTU4_CONV_SCLR.hpp`. Для них заданы `TOTAL_BYTES=1` и `BIT_MASK=0x03`; старшие шесть бит зарезервированы.

## Интерфейс

`InOutSignals4/5` предоставляют именованные геттеры и сеттеры, установку группы сигналов, вывод и интерфейс обращения по индексу для интеграции с моделями.

Типы OTU4 дополнительно поддерживают:

- создание из упакованного байта или вектора байт;
- `get_packed()`, `set_packed()`, `set_all()` и `clear()`;
- экспорт через `to_bytes()`, добавление и запись в существующий вектор;
- пакетные преобразования `batch_from_bytes()`, `batch_from_bytes_stride()`, `batch_to_bytes()`;
- сравнения, преобразования к целым типам и диагностические строки.

## Пример работы с сигналами

```cpp
#include "InOutFlagStruct.hpp"

InOutSignals4 signals;
signals.set_THOHIVLD(true);
signals.set_OTL4_10_DATA_VALID(true);
signals.set_RATE_CONTROL(false);
signals.set_nHARD_RESET(true);
```

У `InOutSignals4` конструктор по умолчанию выставляет `nHARD_RESET=1`. У `InSignalOtu4Converter` конструктор по умолчанию обнуляет `packed`; начальное состояние сброса у этих типов различается.

## Особенности использования

Порядок размещения C++ bit-field зависит от реализации компилятора. Для внешнего байтового формата нужно согласовать битовые позиции и пользоваться предусмотренными методами преобразования.

Неконстантный `operator[]` у `InOutSignals4/5` возвращает ссылку на промежуточное статическое слово: присваивание через него не обновляет `packed`. Для изменения сигналов используйте сеттеры.

## Сборка и запуск

Нужны компилятор g++ с поддержкой C++17, GNU Make. Команды выполняются в корне соответствующей ветки:

```sh
make
make run
make rebuild-release
make run-release
```

По умолчанию собирается `bin/otu4_datatype_test_debug`, release-версия – `bin/otu4_datatype_test`. `make clean` удаляет результаты сборки. При смене конфигурации используйте пересборку: debug и release используют общие объектные файлы.