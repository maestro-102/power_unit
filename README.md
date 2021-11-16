# Power unit | Блок питания
-----------------

Настольный блок питания для проверки работоспособности собираемых и проверяемых электронных устройств. \
*Собирался из того, что было под рукой.*

Содержание
-----------------
1. <a href="https://github.com/maestro-102/power_unit/blob/master/README.md#%D0%BE%D0%BF%D0%B8%D1%81%D0%B0%D0%BD%D0%B8%D0%B5">Описание</a>
2. <a href="https://github.com/maestro-102/power_unit/blob/master/README.md#%D1%84%D0%BE%D1%82%D0%BE%D0%B3%D1%80%D0%B0%D1%84%D0%B8%D0%B8">Фотографии</a>
3. <a href="https://github.com/maestro-102/power_unit/blob/master/README.md#%D1%85%D0%B0%D1%80%D0%B0%D0%BA%D1%82%D0%B5%D1%80%D0%B8%D1%81%D1%82%D0%B8%D0%BA%D0%B8">Характеристики</a>
4. <a href="https://github.com/maestro-102/power_unit/blob/master/README.md#%D0%BF%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D1%8B-%D0%B4%D0%BB%D1%8F-%D0%BF%D1%80%D0%BE%D1%81%D0%BC%D0%BE%D1%82%D1%80%D0%B0-%D1%84%D0%B0%D0%B9%D0%BB%D0%BE%D0%B2">Программы для просмотра файлов</a>
5. <a href="https://github.com/maestro-102/power_unit/blob/master/README.md#%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%82%D1%83%D1%80%D0%B0-%D0%BF%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D0%B0">Структура проекта</a>

Описание
-----------------

Структурно блок состоит из модуля преобразователя напряжения и тока с стабилизаторами питания дисплея и регуляторами оборотов вентиляторами, модуля самого блока питания м модуля индикации на OLED. Линия фиксированных 5В организована применением отдельного понижающего модуля на `LM2576`.

Фотографии
-----------------

<a href="https://github.com/maestro-102/power_unit/blob/master/images/1.jpg" target="_blank">
    <img src="https://github.com/maestro-102/power_unit/blob/master/images/1.jpg?raw=true" width=30% alt="preview">
</a>

<a href="https://github.com/maestro-102/power_unit/blob/master/images/2.jpg" target="_blank">
    <img src="https://github.com/maestro-102/power_unit/blob/master/images/2.jpg?raw=true" width=30% alt="preview">
</a>

<a href="https://github.com/maestro-102/power_unit/blob/master/images/3.jpg" target="_blank">
    <img src="https://github.com/maestro-102/power_unit/blob/master/images/3.jpg?raw=true" width=30% alt="preview">
</a>

Характеристики
-----------------

- выходное напряжение от 0 до 30В (регулируемое)
- выходной ток от 0 до 3А (регулируемое)
- стабилизация выходного тока и напряжения
- 5В (до 2А) по линии USB (фиксированное)
- автоматическая регулировка оборов охлаждаемого вентилятора
- индикация выходного тока, напряжения и потребляемой мощности на OLED дисплее

Программы для просмотра файлов
-----

1. Sprint Layout 6.0 - для проектирования печатных плат \
Расширение: \*.lay6 \
Ссылка: https://radioaktiv.ru/loads/softf/pcb/27881-sprint-layout-60-rus.html

2. Splan 7.0 - для черчения электрических схем \
Расширение: \*.spl7 \
Ссылка: http://splansoft.ru/

Структура проекта
-----------------

Описание файловой структуры проекта:

    power_unit
    ├── images                                - папка с фотографиями устройства
    ├── modules                               - модули устройства
    │   ├── converter                         - преобразователь
    │   │   ├── pcb                           - печатная плата
    │   │   └── datasheet LM2576.pdf    
    │   ├─ display                            - дисплей
    |   │   ├── firmware                      - прошивка устройства
    |   │   ├── pcb                           - печатная плата
    │   │   └── schemas                       - схема устройства
    │   ├─ fan_control                        - автоматический регулятор оборотов вентилятора
    |   │   └── shemas_fan_control_n-ch.JPG   - схема устройства
    │   ├─ power_unit                         - блок питания
    |   │   ├── pcb                           - печатная плата
    │   │   └── schemas                       - схема устройства
    └── README.md          
