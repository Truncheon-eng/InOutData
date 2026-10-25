# InOutData

Ветка: `4-create_InOutData64`. Состояние: `9ebef7a`.

## Изменения относительно предыдущего этапа

Добавлен `InOutData64`, составные данные OTU4_CONVERTER и FRAMER TX. Общий заголовок `Theme.hpp` переименован в `Definitions.hpp`. Расширен `main.cpp`, обновлены отдельные методы существующих типов.

Ветка сохраняет контейнеры 320/128/112/8 бит и группы скалярных сигналов предыдущих этапов.

## InOutData64

`include/InOutData64.hpp` и `src/InOutData64.cpp` описывают 64 полезных бита: два 32-битных слова, восемь байт и единое поле `qword`.

Поддерживаются создание и присваивание из `uint64_t`, доступ к битам, байтам и словам, `get_qword()` / `set_qword()`, преобразования little-endian и big-endian, массивы, векторы и текстовый вывод. При включённой поддержке Verilator объявлено преобразование в `VlWide<2>`.

```cpp
#include "InOutData64.hpp"

InOutData64 data(uint64_t{0x0123456789ABCDEFULL});
auto value = data.get_qword();
auto bytes = data.to_vector_big_endian();
```

## Составные данные

| Тип | Содержимое | Байтовый результат serialize() |
| --- | --- | --- |
| `OTU4_CONVERTER_InputData` | InOutData320 + InSignalOtu4Converter | 41 байт |
| `OTU4_CONVERTER_OutputData` | InOutData128 + OutSignalOtu4Converter | 17 байт |
| `FramerTXInputData` | InOutData320 + InOutData8 + InOutSignals4 | 42 байта |
| `FramerTXOutputData` | InOutData112 + InOutData8 + InOutSignals5 | 16 байт |

Для OTU4 сначала записываются байты данных, затем байт скалярных сигналов. Вход и выход объявлены в `InOutDataOTU4_CONV_IN.hpp` и `InOutDataOTU4_CONV_OUT.hpp`.

Структуры FRAMER TX и `FramerTXTransaction` определены в `InOutFramerTXDataStruct.hpp`; `InOutFramerTXDataStrucHeader.hpp` объединяет подключения. Транзакция объединяет данные для обмена и сравнения результатов, содержит временную метку.

В `FramerTXInputData::serialize()` после 40 байт основной шины и одного байта THOXIDAT записывается `scalar_signals_i.packed << 4`. В `FramerTXOutputData::serialize()` после 14 байт данных и одного дополнительного байта записывается `scalar_signals_o.packed` без такого сдвига. Эти правила нужно учитывать.

## Примеры и проверка

В `main.cpp` добавлены примеры для конструкторов, порядка байт, векторов и операторов. Однако точка входа фактически вызывает только `test_inoutdata8()` и `test_inoutdata320()`. Остальные функции не выполняются автоматически.

Для интеграции с Verilator часть составных типов подключает внешний `VerilatorWrapper.hpp`; он не входит в эту ветку. Это отдельная зависимость интеграционного окружения.

## Сборка и запуск

Нужны компилятор g++ с поддержкой C++17, GNU Make и Unix shell. Команды выполняются в корне соответствующей ветки:

```sh
make
make run
make rebuild-release
make run-release
```

По умолчанию собирается `bin/otu4_datatype_test_debug`, release-версия – `bin/otu4_datatype_test`. `make clean` удаляет результаты сборки. При смене конфигурации используйте пересборку: debug и release используют общие объектные файлы.