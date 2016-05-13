# CWP
Курсовая работа по теме : "Проверка целостности файлов"

## Перед использованием
* **mkdir obj/**
* **make**

## Как использовать
* **./integrctrl** *keys*
* **./integrctrl** *-s -f data /home/alex/test*
* **./integrctrl** *-s -r -f data /home/alex/test*
* **./integrctrl** *-c -f data /home/alex/test *

#### Keys
* *-s* - ***сохранить информацию о директории***
* *-с* - ***сверить информацию с базой данных(data)***
* *-f* - ***data full_path - путь до data, полный путь до директории***
* *-r* - ***позволяет рекурсивно проверять/сохранять вложенные дирекории***

###### Codestyle
* *`indent -kr -ci4 -cli4 -i4 -l80 -nut $file`*

